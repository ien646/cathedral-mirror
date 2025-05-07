#include <cathedral/engine/material.hpp>

#include <cathedral/engine/renderer.hpp>
#include <cathedral/engine/scene.hpp>
#include <cathedral/engine/shader_validation.hpp>
#include <cathedral/engine/vertex_input_builder.hpp>

#include <cathedral/gfx/shader_reflection.hpp>

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
        _material_descriptor_set_info = { .set_index = 1,
                                          .definition = {
                                              { gfx::descriptor_set_entry(1, 0, gfx::descriptor_type::UNIFORM, 1) } } };

        // Clear sampler entries
        {
            auto removed_range =
                std::ranges::remove_if(_material_descriptor_set_info.definition.entries, [](const auto& entry) {
                    return entry.type == gfx::descriptor_type::SAMPLER;
                });
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
            _material_descriptor_set_info.definition.entries.emplace_back(1, 1, gfx::descriptor_type::SAMPLER, mat_tex_slots);
        }

        _node_descriptor_set_info = { .set_index = 2,
                                      .definition = {
                                          { gfx::descriptor_set_entry(2, 0, gfx::descriptor_type::UNIFORM, 1) } } };

        if (const auto node_tex_slots = node_texture_slots(); node_tex_slots > 0)
        {
            _node_descriptor_set_info.definition.entries.emplace_back(2, 1, gfx::descriptor_type::SAMPLER, node_tex_slots);
        }

        gfx::pipeline_args args;
        args.vertex_shader = &_vertex_shader->gfx_shader();
        args.fragment_shader = &_fragment_shader->gfx_shader();
        args.color_attachment_formats = { _renderer->swapchain().swapchain_image_format() };
        args.color_blend_enable = true;
        args.depth_stencil_format = gfx::depthstencil_attachment::format();
        args.enable_depth = true;
        args.enable_stencil = false;
        args.cull_backfaces = false;
        args.descriptor_sets = { scene::descriptor_set_definition(),
                                 _material_descriptor_set_info,
                                 _node_descriptor_set_info };
        args.input_topology = vk::PrimitiveTopology::eTriangleList;
        args.line_width = 1.0F;
        args.polygon_mode = vk::PolygonMode::eFill;
        args.vertex_input = standard_vertex_input_description();
        args.vkctx = &_renderer->vkctx();

        _pipeline = std::make_unique<gfx::pipeline>(args);
    }

    void material::bind_material_texture_slot(const std::shared_ptr<texture>& tex, uint32_t slot)
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

    void material::set_material_binding_for_var(
        const std::string& var_name,
        const std::optional<shader_material_uniform_binding> binding)
    {
        if (binding.has_value())
        {
            if (!_mat_var_offsets.contains(var_name))
            {
                debug_log(std::format("Material variable '{}' not found on material '{}'", var_name, _args.name));
                return;
            }
            _args.material_bindings[*binding] = var_name;
        }
        else
        {
            _mat_var_offsets.erase(var_name);
        }
    }

    void material::set_node_binding_for_var(const std::string& var_name,
        const std::optional<shader_node_uniform_binding> binding)
    {
        if (binding.has_value())
        {
            if (!_node_var_offsets.contains(var_name))
            {
                debug_log(std::format("Node variable '{}' not found on material '{}'", var_name, _args.name));
                return;
            }
            _args.node_bindings[*binding] = var_name;
        }
        else
        {
            _node_var_offsets.erase(var_name);
        }
    }

    uint32_t material::get_material_binding_var_offset(const std::string& var_name)
    {
        if (_mat_var_offsets.contains(var_name))
        {
            return _mat_var_offsets[var_name];
        }
        debug_log(std::format("Material variable '{}' not found", var_name));
        return 0;
    }

    uint32_t material::get_node_binding_var_offset(const std::string& var_name)
    {
        if (_node_var_offsets.contains(var_name))
        {
            return _node_var_offsets[var_name];
        }
        debug_log(std::format("Node variable '{}' not found", var_name));
        return 0;
    }

    void material::init_descriptor_set_layouts()
    {
        _material_descriptor_set_layout =
            _material_descriptor_set_info.definition.create_descriptor_set_layout(_renderer->vkctx());

        _node_descriptor_set_layout = _node_descriptor_set_info.definition.create_descriptor_set_layout(_renderer->vkctx());
    }

    void material::init_descriptor_set()
    {
        vk::DescriptorSetAllocateInfo alloc_info;
        alloc_info.descriptorPool = _renderer->vkctx().descriptor_pool();
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &*_material_descriptor_set_layout;

        _descriptor_set = std::move(_renderer->vkctx().device().allocateDescriptorSetsUnique(alloc_info)[0]);

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

        CRITICAL_CHECK(vx_pp_data.has_value(), std::format("Unable to preprocess vertex shader source -> {}", vx_pp_data.error()));
        CRITICAL_CHECK(fg_pp_data.has_value(), std::format("Unable to preprocess fragment shader source -> {}", fg_pp_data.error()));

        _merged_pp_data = vx_pp_data->merge(*fg_pp_data);
        _merged_pp_data.clean_source = {};

        const auto vx_pp_source = preprocess_shader(gfx::shader_type::VERTEX, *vx_pp_data);
        const auto fg_pp_source = preprocess_shader(gfx::shader_type::FRAGMENT, *fg_pp_data);

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

        _vertex_shader = std::make_shared<engine::shader>(std::move(vx_gfx_shader), *vx_pp_data);
        _fragment_shader = std::make_shared<engine::shader>(std::move(fg_gfx_shader), *fg_pp_data);

        uint32_t current_offset = 0;
        for (const auto& var : _vertex_shader->preprocess_data().material_vars)
        {
            _mat_var_offsets[var.name] = current_offset;
            current_offset += gfx::shader_data_type_offset(var.type, var.count, current_offset);
        }
        _material_uniform_block_size = current_offset;

        current_offset = 0;
        for (const auto& var : _vertex_shader->preprocess_data().node_vars)
        {
            _node_var_offsets[var.name] = current_offset;
            current_offset += gfx::shader_data_type_offset(var.type, var.count, current_offset);
        }
        _node_uniform_block_size = current_offset;
    }
} // namespace cathedral::engine