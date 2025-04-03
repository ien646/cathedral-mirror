#include <cathedral/engine/material.hpp>

#include <cathedral/engine/renderer.hpp>
#include <cathedral/engine/scene.hpp>
#include <cathedral/engine/shader_validation.hpp>
#include <cathedral/engine/vertex_input_builder.hpp>

#include <cathedral/gfx/shader_reflection.hpp>

namespace cathedral::engine
{
    material_definition create_matdef_from_shader_reflection(
        const gfx::shader& vertex_shader,
        const gfx::shader& fragment_shader)
    {
        constexpr uint32_t MATERIAL_SET_INDEX = 1;
        constexpr uint32_t NODE_SET_INDEX = 2;
        constexpr uint32_t UNIFORM_BINDING_INDEX = 0;
        constexpr uint32_t TEXTURE_BINDING_INDEX = 1;

        std::optional<std::string> vxshader_validation_result = validate_shader(vertex_shader);
        std::optional<std::string> fgshader_validation_result = validate_shader(fragment_shader);

        const auto vertex_shader_refl = gfx::get_shader_reflection_info(vertex_shader);
        const auto fragment_shader_refl = gfx::get_shader_reflection_info(fragment_shader);

        const auto get_shader_texture_count = [](const gfx::shader_reflection_info& refl, uint32_t set) {
            auto it = std::ranges::find_if(refl.descriptor_sets, [set](const gfx::shader_reflection_descriptor_set& dset) {
                return dset.set == set && dset.binding == TEXTURE_BINDING_INDEX;
            });
            if (it != refl.descriptor_sets.end())
            {
                return it->count;
            }
            return 0U;
        };

        const uint32_t max_material_textures = std::max(
            get_shader_texture_count(vertex_shader_refl, MATERIAL_SET_INDEX),
            get_shader_texture_count(fragment_shader_refl, MATERIAL_SET_INDEX));

        const uint32_t max_node_textures = std::max(
            get_shader_texture_count(vertex_shader_refl, NODE_SET_INDEX),
            get_shader_texture_count(fragment_shader_refl, NODE_SET_INDEX));

        const auto get_material_uniform_size = [&](const auto& refl) {
            auto it = std::ranges::find_if(refl.descriptor_sets, [&](const gfx::shader_reflection_descriptor_set& dset) {
                return dset.set == MATERIAL_SET_INDEX && dset.binding == UNIFORM_BINDING_INDEX;
            });
            return it->size;
        };

        const auto get_node_uniform_size = [&](const auto& refl) {
            auto it = std::ranges::find_if(refl.descriptor_sets, [&](const gfx::shader_reflection_descriptor_set& dset) {
                return dset.set == NODE_SET_INDEX && dset.binding == UNIFORM_BINDING_INDEX;
            });
            return it->size;
        };

        const uint32_t material_uniform_size_vertex = get_material_uniform_size(vertex_shader_refl);
        const uint32_t material_uniform_size_fragment = get_material_uniform_size(fragment_shader_refl);

        material_definition matdef;
        matdef.set_material_texture_slot_count(max_material_textures);
        matdef.set_node_texture_slot_count(max_node_textures);

        return matdef;
    }

    gfx::vertex_input_description standard_vertex_input_description()
    {
        gfx::vertex_input_description result;
        result.vertex_size = mesh::vertex_size_bytes();
        result.attributes = vertex_input_builder()
                                .push(gfx::vertex_data_type::VEC3F) // POS
                                .push(gfx::vertex_data_type::VEC2F) // UV
                                .push(gfx::vertex_data_type::VEC3F) // NORM
                                .push(gfx::vertex_data_type::VEC4F) // RGBA
                                .build();

        return result;
    }

    material::material(renderer* rend, material_args args)
        : _renderer(rend)
        , _args(std::move(args))
    {
        CRITICAL_CHECK_NOTNULL(_renderer);

        _matdef = create_matdef_from_shader_reflection(*_args.vertex_shader, *_args.fragment_shader);

        if (_matdef.material_uniform_block_size() > 0)
        {
            gfx::uniform_buffer_args buff_args;
            buff_args.size = _matdef.material_uniform_block_size();
            buff_args.vkctx = &_renderer->vkctx();

            _material_uniform = std::make_unique<gfx::uniform_buffer>(buff_args);
        }

        init_pipeline();
        init_descriptor_set_layouts();
        init_descriptor_set();
        init_default_textures();

        _uniform_data.resize(_matdef.material_uniform_block_size());
    }

    void material::init_pipeline()
    {
        _material_descriptor_set_info = { .set_index = 1,
                                          .definition = {
                                              { gfx::descriptor_set_entry(1, 0, gfx::descriptor_type::UNIFORM, 1) } } };

        // Clear sampler entries
        std::ranges::remove_if(_material_descriptor_set_info.definition.entries, [](const gfx::descriptor_set_entry& entry) {
            return entry.type == gfx::descriptor_type::SAMPLER;
        });
        std::ranges::remove_if(_node_descriptor_set_info.definition.entries, [](const gfx::descriptor_set_entry& entry) {
            return entry.type == gfx::descriptor_type::SAMPLER;
        });

        if (const auto mat_tex_slots = _matdef.material_texture_slot_count(); mat_tex_slots > 0)
        {
            _material_descriptor_set_info.definition.entries.emplace_back(1, 1, gfx::descriptor_type::SAMPLER, mat_tex_slots);
        }

        _node_descriptor_set_info = { .set_index = 2,
                                      .definition = {
                                          { gfx::descriptor_set_entry(2, 0, gfx::descriptor_type::UNIFORM, 1) } } };

        if (const auto node_tex_slots = _matdef.node_texture_slot_count(); node_tex_slots > 0)
        {
            _node_descriptor_set_info.definition.entries.emplace_back(2, 1, gfx::descriptor_type::SAMPLER, node_tex_slots);
        }

        gfx::pipeline_args args;
        args.vertex_shader = _args.vertex_shader.get();
        args.fragment_shader = _args.fragment_shader.get();
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
        CRITICAL_CHECK(slot < _matdef.material_texture_slot_count(), "Attempt to bind texture to non-available slot index");
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

    void material::update_uniform(const std::function<void(std::span<std::byte>)>& func)
    {
        if (_uniform_data.empty())
        {
            return;
        }

        const auto previous_data = _uniform_data;
        func(_uniform_data);
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

    void material::set_vertex_shader(std::shared_ptr<gfx::shader> shader)
    {
        _args.vertex_shader = std::move(shader);
        _needs_pipeline_update = true;
    }

    void material::set_fragment_shader(std::shared_ptr<gfx::shader> shader)
    {
        _args.fragment_shader = std::move(shader);
        _needs_pipeline_update = true;
    }

    void material::force_pipeline_update()
    {
        _needs_pipeline_update = true;
    }

    void material::force_rebind_textures()
    {
        auto slots = _texture_slots;
        _texture_slots.clear();

        for (size_t i = 0; i < slots.size(); ++i)
        {
            bind_material_texture_slot(slots[i], static_cast<uint32_t>(i));
        }
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
        if (!_material_descriptor_set_info.definition.entries.empty())
        {
            const auto& textures_binding = _material_descriptor_set_info.definition.entries[1];
            for (uint32_t i = 0; i < textures_binding.count; ++i)
            {
                bind_material_texture_slot(_renderer->default_texture(), i);
            }
        }
    }
} // namespace cathedral::engine