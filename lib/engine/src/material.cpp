#include <cathedral/engine/material.hpp>

#include <cathedral/engine/renderer.hpp>
#include <cathedral/engine/scene.hpp>
#include <cathedral/engine/shader_validation.hpp>
#include <cathedral/engine/vertex_input_builder.hpp>
#include <cathedral/gfx/check.hpp>
#include <cathedral/gfx/shader_reflection.hpp>

#include <ien/io_utils.hpp>

#include <numeric>
#include <ranges>

namespace cathedral::engine
{
    namespace
    {
        uint32_t global_material_uid_counter = 0;
    }

    gfx::vertex_input_description standard_vertex_input_description()
    {
        gfx::vertex_input_description result;
        result.vertex_size = static_cast<uint32_t>(mesh::vertex_size_bytes());
        result.attributes = vertex_input_builder()
                                .push(gfx::vertex_data_type::VEC3F) // POS
                                .push(gfx::vertex_data_type::VEC2F) // UV
                                .push(gfx::vertex_data_type::VEC3F) // NORM
                                .push(gfx::vertex_data_type::VEC4F) // RGBA
                                .build();

        return result;
    }

    material::material(renderer* rend, material_args args)
        : _uid(global_material_uid_counter++)
        , _renderer(rend)
        , _args(std::move(args))
    {
        CRITICAL_CHECK_NOTNULL(_renderer);
        CRITICAL_CHECK(!_args.vertex_shader_source.empty(), "Empty vertex shader source");
        CRITICAL_CHECK(!_args.fragment_shader_source.empty(), "Empty fragment shader source");

        initialize();
    }

    void material::initialize()
    {
        init_shaders_and_data();

        if (_material_uniform_block_size > 0)
        {
            gfx::uniform_buffer_args buff_args;
            buff_args.size = _material_uniform_block_size;
            buff_args.vkctx = &_renderer->vkctx();

            _material_uniform = std::make_unique<gfx::uniform_buffer>(buff_args);
        }

        init_pipeline();
        init_descriptor_set_layouts();
        init_descriptor_set();
        init_default_textures();

        _uniform_data.resize(_material_uniform_block_size);
    }

    void material::init_pipeline()
    {
        _material_descriptor_set_info = { .set_index = MATERIAL_DESCRIPTOR_SET_INDEX,
                                          .definition = { { gfx::descriptor_set_entry(
                                              MATERIAL_DESCRIPTOR_SET_INDEX,
                                              UNIFORM_BUFFER_BINDING_INDEX,
                                              gfx::descriptor_type::UNIFORM,
                                              1) } } };

        // Clear sampler entries
        {
            auto removed_range = std::ranges::remove_if(
                _material_descriptor_set_info.definition.entries,
                [](const auto& entry) { return entry.type == gfx::descriptor_type::SAMPLER; });
            _material_descriptor_set_info.definition.entries.erase(removed_range.begin(), removed_range.end());
        }

        {
            auto removed_range = std::ranges::remove_if(_node_descriptor_set_info.definition.entries, [](const auto& entry) {
                return entry.type == gfx::descriptor_type::SAMPLER;
            });
            _node_descriptor_set_info.definition.entries.erase(removed_range.begin(), removed_range.end());
        }

        if (const auto mat_tex_slots = material_texture_slots(); mat_tex_slots > 0)
        {
            _material_descriptor_set_info.definition.entries.emplace_back(
                MATERIAL_DESCRIPTOR_SET_INDEX,
                SAMPLER_BINDING_INDEX,
                gfx::descriptor_type::SAMPLER,
                mat_tex_slots);
        }

        _node_descriptor_set_info = { .set_index = NODE_DESCRIPTOR_SET_INDEX,
                                      .definition = { { gfx::descriptor_set_entry(
                                          NODE_DESCRIPTOR_SET_INDEX,
                                          UNIFORM_BUFFER_BINDING_INDEX,
                                          gfx::descriptor_type::UNIFORM,
                                          1) } } };

        if (const auto node_tex_slots = node_texture_slots(); node_tex_slots > 0)
        {
            _node_descriptor_set_info.definition.entries.emplace_back(
                NODE_DESCRIPTOR_SET_INDEX,
                SAMPLER_BINDING_INDEX,
                gfx::descriptor_type::SAMPLER,
                node_tex_slots);
        }

        for (uint32_t i = 0; i < _merged_pp_data.material_buffers.size(); i++)
        {
            const uint32_t binding_index = STORAGE_BUFFER_FIRST_BINDING_INDEX + i;
            const gfx::descriptor_set_entry entry = { MATERIAL_DESCRIPTOR_SET_INDEX,
                                                      binding_index,
                                                      gfx::descriptor_type::STORAGE,
                                                      1 };
            _material_descriptor_set_info.definition.entries.push_back(entry);
        }

        for (uint32_t i = 0; i < _merged_pp_data.node_buffers.size(); i++)
        {
            const uint32_t binding_index = STORAGE_BUFFER_FIRST_BINDING_INDEX + i;
            const gfx::descriptor_set_entry entry = { NODE_DESCRIPTOR_SET_INDEX,
                                                      binding_index,
                                                      gfx::descriptor_type::STORAGE,
                                                      1 };
            _node_descriptor_set_info.definition.entries.push_back(entry);
        }

        gfx::pipeline_args args;
        args.vertex_shader = &_vertex_shader->gfx_shader();
        args.fragment_shader = &_fragment_shader->gfx_shader();
        args.color_attachment_formats = { _renderer->swapchain().swapchain_image_format() };
        args.color_blend_enable = true;
        args.depth_stencil_format = gfx::depthstencil_attachment::format();
        args.enable_depth = true;
        args.enable_stencil = false;
        args.cull_backfaces = _args.cull_backfaces;
        args.descriptor_sets = { scene::descriptor_set_definition(),
                                 _material_descriptor_set_info,
                                 _node_descriptor_set_info };
        args.input_topology = vk::PrimitiveTopology::eTriangleList;
        args.line_width = 1.0F;
        args.polygon_mode = _args.wireframe ? vk::PolygonMode::eLine : vk::PolygonMode::eFill;
        args.vertex_input = standard_vertex_input_description();
        args.vkctx = &_renderer->vkctx();
        args.msaa_samples = _args._internal.msaa_samples;
        args.msaa_sample_shading = _args._internal.msaa_sample_shading;

        _pipeline = std::make_unique<gfx::pipeline>(args);
    }

    void material::bind_material_texture_slot(const std::string& name, const uint32_t slot)
    {
        if (_renderer->textures().contains(name))
        {
            bind_material_texture_slot(_renderer->textures().at(name), slot);
        }
        else
        {
            bind_material_texture_slot(_renderer->default_texture(), slot);
        }
    }

    void material::bind_material_texture_slot(const std::shared_ptr<texture>& tex, const uint32_t slot)
    {
        CRITICAL_CHECK(slot < material_texture_slots(), "Attempt to bind texture to non-available slot index");
        if (slot >= _texture_slots.size())
        {
            _texture_slots.resize(slot + 1);
        }
        _texture_slots[slot] = tex;

        vk::DescriptorImageInfo info;
        info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        info.imageView = tex->imageview();
        info.sampler = tex->sampler().get_sampler();

        vk::WriteDescriptorSet write;
        write.pImageInfo = &info;
        write.descriptorCount = 1;
        write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        write.dstArrayElement = slot;
        write.dstBinding = 1;
        write.dstSet = *_descriptor_set;
        write.pTexelBufferView = nullptr;

        _renderer->vkctx().device().updateDescriptorSets(write, {});
    }

    void material::update_uniform(const std::function<void(std::span<std::byte>&)>& func)
    {
        if (_uniform_data.empty())
        {
            return;
        }

        const auto previous_data = _uniform_data;
        auto span = std::span{ _uniform_data.data(), _uniform_data.size() };
        func(span);
        if (previous_data != _uniform_data)
        {
            _uniform_needs_update = true;
        }
    }

    void material::update()
    {
        if (_needs_pipeline_update)
        {
            init_pipeline();
            force_rebind_textures();
            _needs_pipeline_update = false;
        }

        if (_material_uniform && _uniform_needs_update)
        {
            _renderer->get_upload_queue().update_buffer(*_material_uniform, 0, _uniform_data);
            _uniform_needs_update = false;
        }

        for (uint32_t i = 0; i < _storage_buffers_needs_update.size(); ++i)
        {
            update_storage_buffer(STORAGE_BUFFER_FIRST_BINDING_INDEX + i);
        }
    }

    const std::vector<std::shared_ptr<texture>>& material::bound_textures() const
    {
        return _texture_slots;
    }

    const gfx::pipeline& material::pipeline() const
    {
        return *_pipeline;
    }

    vk::DescriptorSetLayout material::material_descriptor_set_layout() const
    {
        return *_material_descriptor_set_layout;
    }

    vk::DescriptorSetLayout material::node_descriptor_set_layout() const
    {
        return *_node_descriptor_set_layout;
    }

    vk::DescriptorSet material::descriptor_set() const
    {
        return *_descriptor_set;
    }

    const gfx::pipeline_descriptor_set& material::material_descriptor_set_definition() const
    {
        return _material_descriptor_set_info;
    }

    const gfx::pipeline_descriptor_set& material::node_descriptor_set_definition() const
    {
        return _node_descriptor_set_info;
    }

    std::shared_ptr<shader> material::vertex_shader() const
    {
        return _vertex_shader;
    }

    std::shared_ptr<shader> material::fragment_shader() const
    {
        return _fragment_shader;
    }

    material_domain material::domain() const
    {
        return _args.domain;
    }

    void material::set_domain(const material_domain domain)
    {
        _args.domain = domain;
    }

    uint32_t material::material_uniform_block_size() const
    {
        return _material_uniform_block_size;
    }

    uint32_t material::material_texture_slots() const
    {
        return static_cast<uint32_t>(_merged_pp_data.material_textures.size());
    }

    const std::vector<std::string>& material::material_texture_names() const
    {
        return _merged_pp_data.material_textures;
    }

    uint32_t material::node_uniform_block_size() const
    {
        return _node_uniform_block_size;
    }

    uint32_t material::node_texture_slots() const
    {
        return static_cast<uint32_t>(_merged_pp_data.node_textures.size());
    }

    const std::vector<std::string>& material::node_texture_names() const
    {
        return _merged_pp_data.node_textures;
    }

    const material_uniform_bindings_t& material::material_uniform_bindings() const
    {
        return _args.material_uniform_bindings;
    }

    const material_texture_bindings_t& material::material_texture_bindings() const
    {
        return _args.material_texture_bindings;
    }

    const material_buffer_bindings_t& material::material_buffer_bindings() const
    {
        return _args.material_buffer_bindings;
    }

    const node_uniform_bindings_t& material::node_uniform_bindings() const
    {
        return _args.node_uniform_bindings;
    }

    const node_texture_bindings_t& material::node_texture_bindings() const
    {
        return _args.node_texture_bindings;
    }

    const node_buffer_bindings_t& material::node_buffer_bindings() const
    {
        return _args.node_buffer_bindings;
    }

    const std::vector<shader_variable>& material::material_uniform_variables() const
    {
        return _merged_pp_data.material_uniform_vars;
    }

    const std::vector<shader_variable>& material::node_uniform_variables() const
    {
        return _merged_pp_data.node_uniform_vars;
    }

    void material::force_pipeline_update()
    {
        _needs_pipeline_update = true;
    }

    void material::force_rebind_textures()
    {
        const auto slots_copy = _texture_slots;
        _texture_slots.clear();

        for (size_t i = 0; i < slots_copy.size(); ++i)
        {
            bind_material_texture_slot(slots_copy[i], static_cast<uint32_t>(i));
        }
    }

    void material::set_material_uniform_binding_for_var(
        const std::string& var_name,
        const std::optional<shader_material_uniform_binding> binding)
    {
        if (binding.has_value())
        {
            if (!_mat_var_offsets.contains(var_name))
            {
                log_error(std::format("Material variable '{}' not found on material '{}'", var_name, _args.name));
                return;
            }
            _args.material_uniform_bindings[var_name] = binding.value();
        }
        else
        {
            _mat_var_offsets.erase(var_name);
        }
    }

    void material::set_node_uniform_binding_for_var(
        const std::string& var_name,
        const std::optional<shader_node_uniform_binding> binding)
    {
        if (binding.has_value())
        {
            if (!_node_var_offsets.contains(var_name))
            {
                log_error(std::format("Node variable '{}' not found on material '{}'", var_name, _args.name));
                return;
            }
            _args.node_uniform_bindings[var_name] = binding.value();
        }
        else
        {
            _node_var_offsets.erase(var_name);
        }
    }

    void material::set_material_texture_binding_for_var(
        const std::string& var_name,
        const std::optional<shader_material_texture_binding> binding)
    {
        if (binding.has_value())
        {
            _args.material_texture_bindings[var_name] = binding.value();
        }
        else
        {
            _args.material_texture_bindings.erase(var_name);
        }
    }

    void material::set_node_texture_binding_for_var(
        const std::string& var_name,
        const std::optional<shader_node_texture_binding> binding)
    {
        if (binding.has_value())
        {
            _args.node_texture_bindings[var_name] = binding.value();
        }
        else
        {
            _args.node_texture_bindings.erase(var_name);
        }
    }

    void material::set_material_buffer_binding_for_var(
        const std::string& var_name,
        const std::optional<shader_material_buffer_binding> binding)
    {
        if (binding.has_value())
        {
            _args.material_buffer_bindings[var_name] = binding.value();
        }
        else
        {
            _args.material_buffer_bindings.erase(var_name);
        }
    }

    void material::set_node_buffer_binding_for_var(
        const std::string& var_name,
        std::optional<shader_node_buffer_binding> binding)
    {
        if (binding.has_value())
        {
            _args.node_buffer_bindings[var_name] = binding.value();
        }
        else
        {
            _args.node_buffer_bindings.erase(var_name);
        }
    }

    std::optional<uint32_t> material::get_material_uniform_var_offset(const std::string& var_name)
    {
        if (_mat_var_offsets.contains(var_name))
        {
            return _mat_var_offsets[var_name];
        }
        log_error(std::format("Material variable '{}' not found", var_name));
        return {};
    }

    std::optional<uint32_t> material::get_node_uniform_var_offset(const std::string& var_name)
    {
        if (_node_var_offsets.contains(var_name))
        {
            return _node_var_offsets[var_name];
        }
        log_error(std::format("Node variable '{}' not found", var_name));
        return {};
    }

    std::optional<uint32_t> material::get_material_buffer_index(const std::string& name) const
    {
        const auto& buffer_vars = _merged_pp_data.material_buffers;
        for (uint32_t i = 0; i < buffer_vars.size(); ++i)
        {
            if (buffer_vars[i].name == name)
            {
                return i;
            }
        }
        log_error(std::format("Material buffer '{}' not found", name));
        return {};
    }

    std::optional<uint32_t> material::get_node_buffer_index(const std::string& name) const
    {
        const auto& buffer_vars = _merged_pp_data.node_buffers;
        for (uint32_t i = 0; i < buffer_vars.size(); ++i)
        {
            if (buffer_vars[i].name == name)
            {
                return i;
            }
        }
        log_error(std::format("Node buffer '{}' not found", name));
        return {};
    }

    std::optional<uint32_t> material::get_material_texture_slot(const std::string& name) const
    {
        const auto& texture_names = _merged_pp_data.material_textures;
        for (uint32_t i = 0; i < texture_names.size(); ++i)
        {
            if (texture_names[i] == name)
            {
                return i;
            }
        }
        log_error(std::format("Material texture '{}' not found", name));
        return {};
    }

    std::optional<uint32_t> material::get_node_texture_slot(const std::string& name) const
    {
        const auto& texture_names = _merged_pp_data.node_textures;
        for (uint32_t i = 0; i < texture_names.size(); ++i)
        {
            if (texture_names[i] == name)
            {
                return i;
            }
        }
        log_error(std::format("Material texture '{}' not found", name));
        return {};
    }

    std::vector<std::string> material::material_buffer_names() const
    {
        return _merged_pp_data.material_buffers
               | std::views::transform([](const auto& var) { return var.name; })
               | std::ranges::to<std::vector<std::string>>();
    }

    std::vector<std::string> material::node_buffer_names() const
    {
        return _merged_pp_data.node_buffers
               | std::views::transform([](const auto& var) { return var.name; })
               | std::ranges::to<std::vector<std::string>>();
    }

    void material::set_storage_buffer_data(const uint32_t binding_index, std::vector<std::byte> data)
    {
        CRITICAL_CHECK(_storage_buffers_data.size() > binding_index, "Material storage buffer binding index out of range");

        _storage_buffers_data[binding_index] = std::move(data);
        _storage_buffers_needs_update[binding_index] = true;
    }

    void material::set_msaa_samples(const vk::SampleCountFlagBits samples)
    {
        _args._internal.msaa_samples = samples;
        initialize();
    }

    void material::set_msaa_sample_shading(const bool enabled)
    {
        _args._internal.msaa_sample_shading = enabled;
        initialize();
    }

    material material::create_dummy_material(material_args args)
    {
        material result(std::move(args));
        result.init_shaders_and_data();
        return result;
    }

    void material::init_descriptor_set_layouts()
    {
        _material_descriptor_set_layout = _material_descriptor_set_info.definition.create_descriptor_set_layout(
            _renderer->vkctx());

        _node_descriptor_set_layout = _node_descriptor_set_info.definition.create_descriptor_set_layout(_renderer->vkctx());
    }

    void material::init_descriptor_set()
    {
        vk::DescriptorSetAllocateInfo alloc_info;
        alloc_info.descriptorPool = _renderer->vkctx().descriptor_pool();
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &*_material_descriptor_set_layout;

        _descriptor_set = std::move(
            CATHEDRAL_VK_RESULT_VALUE_CHECKED(_renderer->vkctx().device().allocateDescriptorSetsUnique(alloc_info))[0]);

        const auto& buffer = _material_uniform ? _material_uniform : _renderer->empty_uniform_buffer();

        vk::DescriptorBufferInfo buffer_info;
        buffer_info.buffer = buffer->buffer();
        buffer_info.offset = 0;
        buffer_info.range = buffer->size();

        vk::WriteDescriptorSet write;
        write.descriptorCount = 1;
        write.descriptorType = vk::DescriptorType::eUniformBuffer;
        write.pBufferInfo = &buffer_info;
        write.dstArrayElement = 0;
        write.dstBinding = 0;
        write.dstSet = *_descriptor_set;
        _renderer->vkctx().device().updateDescriptorSets(write, {});
    }

    void material::init_default_textures()
    {
        const auto it = std::ranges::find_if(
            _material_descriptor_set_info.definition.entries,
            [](const gfx::descriptor_set_entry& entry) { return entry.set == 1 && entry.binding == 1; });

        if (it != _material_descriptor_set_info.definition.entries.end())
        {
            const auto& textures_binding = *it;
            for (uint32_t i = 0; i < textures_binding.count; ++i)
            {
                bind_material_texture_slot(_renderer->default_texture(), i);
            }
        }
    }

    void material::init_shaders_and_data()
    {
        auto vx_pp_data = get_shader_preprocess_data(_args.vertex_shader_source);
        auto fg_pp_data = get_shader_preprocess_data(_args.fragment_shader_source);

        CRITICAL_CHECK(
            vx_pp_data.has_value(),
            std::format("Unable to preprocess vertex shader source -> {}", vx_pp_data.error()));
        CRITICAL_CHECK(
            fg_pp_data.has_value(),
            std::format("Unable to preprocess fragment shader source -> {}", fg_pp_data.error()));

        _merged_pp_data = vx_pp_data->merge(*fg_pp_data);
        _merged_pp_data.clean_source = {};

        const auto vx_pp_source = preprocess_shader(
            gfx::shader_type::VERTEX,
            _merged_pp_data,
            vx_pp_data->clean_source,
            vx_pp_data->main_function_block);
        const auto fg_pp_source = preprocess_shader(
            gfx::shader_type::FRAGMENT,
            _merged_pp_data,
            fg_pp_data->clean_source,
            fg_pp_data->main_function_block);

        CRITICAL_CHECK(vx_pp_source.has_value(), "Vertex shader code generation failed");
        CRITICAL_CHECK(fg_pp_source.has_value(), "Fragment shader code generation failed");

        gfx::shader_args vx_gfx_shader_args;
        vx_gfx_shader_args.source = *vx_pp_source;
        vx_gfx_shader_args.type = gfx::shader_type::VERTEX;

        gfx::shader_args fg_gfx_shader_args;
        fg_gfx_shader_args.source = *fg_pp_source;
        fg_gfx_shader_args.type = gfx::shader_type::FRAGMENT;

        auto vx_gfx_shader = std::make_shared<gfx::shader>(std::move(vx_gfx_shader_args));
        auto fg_gfx_shader = std::make_shared<gfx::shader>(std::move(fg_gfx_shader_args));

        vx_gfx_shader->compile();
        fg_gfx_shader->compile();

        _vertex_shader = std::make_shared<shader>(std::move(vx_gfx_shader), *vx_pp_data);
        _fragment_shader = std::make_shared<shader>(std::move(fg_gfx_shader), *fg_pp_data);

        uint32_t current_offset = 0;
        for (const auto& var : _merged_pp_data.material_uniform_vars)
        {
            const auto alignment = gfx::shader_data_type_alignment(var.type);
            if (current_offset % alignment != 0)
            {
                current_offset += alignment - (current_offset % alignment);
            }

            _mat_var_offsets[var.name] = current_offset;
            current_offset += gfx::shader_data_type_offset(var.type, var.count, current_offset);
        }
        _material_uniform_block_size = current_offset;

        current_offset = 0;
        for (const auto& var : _merged_pp_data.node_uniform_vars)
        {
            const auto alignment = gfx::shader_data_type_alignment(var.type);
            if (current_offset % alignment != 0)
            {
                current_offset += alignment - (current_offset % alignment);
            }

            _node_var_offsets[var.name] = current_offset;
            current_offset += gfx::shader_data_type_offset(var.type, var.count, current_offset);
        }
        _node_uniform_block_size = current_offset;

        _storage_buffers.clear();
        _storage_buffers_data.clear();
        _storage_buffers_needs_update.clear();

        for ([[maybe_unused]] const auto& mat_buffer : _merged_pp_data.material_buffers)
        {
            _storage_buffers.push_back(_renderer->default_storage_buffer());

            std::vector<std::byte> data;
            data.resize(4, static_cast<std::byte>(0));

            _storage_buffers_data.push_back(std::move(data));
            _storage_buffers_needs_update.push_back(true);
        }
    }

    void material::update_storage_buffer(const uint32_t binding_index)
    {
        CRITICAL_CHECK(binding_index >= STORAGE_BUFFER_FIRST_BINDING_INDEX, "Invalid storage buffer binding index");

        const uint32_t buffer_index = binding_index - STORAGE_BUFFER_FIRST_BINDING_INDEX;
        const auto& data = _storage_buffers_data[buffer_index];

        if (data.empty())
        {
            _storage_buffers[buffer_index] = _renderer->default_storage_buffer();
        }
        else if (_storage_buffers[buffer_index]->size() != data.size())
        {
            gfx::storage_buffer_args args;
            args.size = data.size();
            args.vkctx = &_renderer->vkctx();

            _storage_buffers[buffer_index] = std::make_shared<gfx::storage_buffer>(std::move(args));

            auto& queue = _renderer->get_upload_queue();
            queue.update_buffer(*_storage_buffers[buffer_index], 0, data);
        }

        vk::DescriptorBufferInfo buffer_info;
        buffer_info.buffer = _storage_buffers[buffer_index]->buffer();
        buffer_info.offset = 0;
        buffer_info.range = _storage_buffers[buffer_index]->size();

        vk::WriteDescriptorSet write;
        write.descriptorCount = 1;
        write.dstBinding = binding_index;
        write.dstSet = *_descriptor_set;
        write.pBufferInfo = &buffer_info;
        write.descriptorType = vk::DescriptorType::eStorageBuffer;
        write.dstArrayElement = 0;
        write.pImageInfo = nullptr;
        write.pTexelBufferView = nullptr;

        _renderer->vkctx().device().updateDescriptorSets(write, {});
    }
} // namespace cathedral::engine