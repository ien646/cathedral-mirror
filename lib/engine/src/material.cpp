#include <cathedral/engine/material.hpp>

#include <cathedral/engine/renderer.hpp>
#include <cathedral/engine/scene.hpp>
#include <cathedral/engine/vertex_input_builder.hpp>

namespace cathedral::engine
{
    gfx::vertex_input_description standard_vertex_input_description()
    {
        gfx::vertex_input_description result;
        result.vertex_size = (3 + 2 + 3 + 4) * sizeof(float);
        result.attributes = vertex_input_builder()
                                .push(gfx::vertex_data_type::VEC3F) // POS
                                .push(gfx::vertex_data_type::VEC2F) // UV
                                .push(gfx::vertex_data_type::VEC3F) // NORM
                                .push(gfx::vertex_data_type::VEC4F) // RGBA
                                .build();

        return result;
    }

    material::material(renderer& rend, material_args args)
        : _renderer(rend)
        , _args(std::move(args))
    {
        if (_args.def.material_uniform_block_size() > 0)
        {
            gfx::uniform_buffer_args buff_args;
            buff_args.size = _args.def.material_uniform_block_size();
            buff_args.vkctx = &rend.vkctx();

            _material_uniform = std::make_unique<gfx::uniform_buffer>(buff_args);
        }

        init_pipeline();
        init_descriptor_set_layouts();
        init_descriptor_set();
        init_default_textures();

        _uniform_data.resize(_args.def.material_uniform_block_size());
    }

    void material::init_pipeline()
    {
        _material_descriptor_set_info = { .set_index = 1,
                                          .definition = {
                                              { gfx::descriptor_set_entry(1, 0, gfx::descriptor_type::UNIFORM, 1) } } };

        if (const auto mat_tex_slots = _args.def.material_texture_slot_count(); mat_tex_slots > 0)
        {
            _material_descriptor_set_info.definition.entries.emplace_back(1, 1, gfx::descriptor_type::SAMPLER, mat_tex_slots);
        }

        _node_descriptor_set_info = { .set_index = 2,
                                      .definition = {
                                          { gfx::descriptor_set_entry(2, 0, gfx::descriptor_type::UNIFORM, 1) } } };

        if (const auto node_tex_slots = _args.def.node_texture_slot_count(); node_tex_slots > 0)
        {
            _node_descriptor_set_info.definition.entries.emplace_back(2, 1, gfx::descriptor_type::SAMPLER, node_tex_slots);
        }

        gfx::pipeline_args args;
        args.vertex_shader = _args.vertex_shader.get();
        args.fragment_shader = _args.fragment_shader.get();
        args.color_attachment_formats = { _renderer.swapchain().swapchain_image_format() };
        args.color_blend_enable = true;
        args.depth_stencil_format = gfx::depthstencil_attachment::format();
        args.enable_depth = true;
        args.enable_stencil = false;
        args.cull_backfaces = false;
        args.descriptor_sets = { scene::descriptor_set_definition(),
                                 _material_descriptor_set_info,
                                 _node_descriptor_set_info };
        args.input_topology = vk::PrimitiveTopology::eTriangleList;
        args.line_width = 1.0f;
        args.polygon_mode = vk::PolygonMode::eFill;
        args.vertex_input = standard_vertex_input_description();
        args.vkctx = &_renderer.vkctx();

        _pipeline = std::make_unique<gfx::pipeline>(args);
    }

    void material::bind_material_texture_slot(std::shared_ptr<texture> tex, uint32_t slot)
    {
        CRITICAL_CHECK(slot < _args.def.material_texture_slot_count());
        if (slot >= _texture_slots.size())
        {
            _texture_slots.resize(slot + 1);
        }
        _texture_slots.insert(_texture_slots.begin() + slot, tex);

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

        _renderer.vkctx().device().updateDescriptorSets(write, {});
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
        if (_material_uniform && _uniform_needs_update)
        {
            _renderer.get_upload_queue().update_buffer(*_material_uniform, 0, _uniform_data);
            _uniform_needs_update = false;
        }
    }

    void material::init_descriptor_set_layouts()
    {
        _material_descriptor_set_layout =
            _material_descriptor_set_info.definition.create_descriptor_set_layout(_renderer.vkctx());

        _node_descriptor_set_layout = _node_descriptor_set_info.definition.create_descriptor_set_layout(_renderer.vkctx());
    }

    void material::init_descriptor_set()
    {
        vk::DescriptorSetAllocateInfo alloc_info;
        alloc_info.descriptorPool = _renderer.vkctx().descriptor_pool();
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &*_material_descriptor_set_layout;

        _descriptor_set = std::move(_renderer.vkctx().device().allocateDescriptorSetsUnique(alloc_info)[0]);

        const auto& buffer = _material_uniform ? _material_uniform : _renderer.empty_uniform_buffer();

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
        _renderer.vkctx().device().updateDescriptorSets(write, {});
    }

    void material::init_default_textures()
    {
        if (_material_descriptor_set_info.definition.entries.size() >= 1)
        {
            const auto& textures_binding = _material_descriptor_set_info.definition.entries[1];
            for (uint32_t i = 0; i < textures_binding.count; ++i)
            {
                bind_material_texture_slot(_renderer.default_texture(), i);
            }
        }
    }
} // namespace cathedral::engine