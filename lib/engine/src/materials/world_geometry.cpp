#include <cathedral/engine/materials/world_geometry.hpp>

#include "cathedral/engine/materials/definitions.hpp"
#include <cathedral/engine/scene.hpp>
#include <cathedral/engine/vertex_input_builder.hpp>

namespace cathedral::engine
{
    world_geometry_material::world_geometry_material(renderer& rend, world_geometry_material_args args)
        : material(rend)
        , _args(args)
    {
        init_pipeline();

        gfx::uniform_buffer_args uniform_buffer_args;
        uniform_buffer_args.size = sizeof(world_geometry_material_uniform_data);
        uniform_buffer_args.vkctx = &_renderer.vkctx();

        _material_uniform = std::make_unique<gfx::uniform_buffer>(uniform_buffer_args);

        init_descriptor_set_layouts();
        init_descriptor_set();
        init_default_textures();
    }

    void world_geometry_material::update_uniform(std::function<void(world_geometry_material_uniform_data&)> func)
    {
        const auto previous_data = _uniform_data;
        func(_uniform_data);
        if (previous_data != _uniform_data)
        {
            _renderer.get_upload_queue().update_buffer(*_material_uniform, 0, &_uniform_data, sizeof(_uniform_data));
        }
    }

    void world_geometry_material::update()
    {
        if (_uniform_needs_update)
        {
            _renderer.get_upload_queue().update_buffer(*_material_uniform, 0, &_uniform_data, sizeof(_uniform_data));
            _uniform_needs_update = false;
        }
    }

    void world_geometry_material::init_pipeline()
    {
        gfx::pipeline_args args;
        args.vertex_shader = _args.vertex_shader;
        args.fragment_shader = _args.fragment_shader;
        args.color_attachment_formats = { _renderer.swapchain().swapchain_image_format() };
        args.color_blend_enable = true;
        args.depth_stencil_format = _renderer.depthstencil_attachment().format();
        args.enable_depth = true;
        args.enable_stencil = false;
        args.cull_backfaces = false;
        args.descriptor_sets = {
            scene::descriptor_set_definition(),
            material_descriptor_set_definition(),
            node_descriptor_set_definition()
        };
        args.input_topology = vk::PrimitiveTopology::eTriangleList;
        args.line_width = 1.0f;
        args.polygon_mode = vk::PolygonMode::eFill;
        args.vertex_input = standard_vertex_input_description();
        args.vkctx = &_renderer.vkctx();

        _pipeline = std::make_unique<gfx::pipeline>(args);
    }

    void world_geometry_material::init_descriptor_set_layouts()
    {
        _material_descriptor_set_layout =
            material_descriptor_set_definition().definition.create_descriptor_set_layout(_renderer.vkctx());

        _node_descriptor_set_layout =
            node_descriptor_set_definition().definition.create_descriptor_set_layout(_renderer.vkctx());
    }

    void world_geometry_material::init_descriptor_set()
    {
        vk::DescriptorSetAllocateInfo alloc_info;
        alloc_info.descriptorPool = _renderer.vkctx().descriptor_pool();
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &*_material_descriptor_set_layout;

        _descriptor_set = std::move(_renderer.vkctx().device().allocateDescriptorSetsUnique(alloc_info)[0]);

        vk::DescriptorBufferInfo buffer_info;
        buffer_info.buffer = _material_uniform->buffer();
        buffer_info.offset = 0;
        buffer_info.range = _material_uniform->size();

        vk::WriteDescriptorSet write;
        write.descriptorCount = 1;
        write.descriptorType = vk::DescriptorType::eUniformBuffer;
        write.pBufferInfo = &buffer_info;
        write.dstArrayElement = 0;
        write.dstBinding = 0;
        write.dstSet = *_descriptor_set;
        _renderer.vkctx().device().updateDescriptorSets(write, {});
    }

    void world_geometry_material::bind_material_texture_slot(std::shared_ptr<texture> tex, uint32_t slot)
    {
        if(slot >= _texture_slots.size())
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

    void world_geometry_material::init_default_textures()
    {
        const auto defs = material_descriptor_set_definition();
        if(defs.definition.entries.size() >= 1)
        {
            for(size_t i = 0; i < defs.definition.entries[1].count; ++i)
            {
                bind_material_texture_slot(_renderer.default_texture(), i);
            }
        }
    }

    gfx::pipeline_descriptor_set world_geometry_material::material_descriptor_set_definition()
    {
        gfx::pipeline_descriptor_set result;
        result.set_index = 1;
        result.definition.entries = {
            gfx::descriptor_set_entry(result.set_index, 0, gfx::descriptor_type::UNIFORM, 1), // material params
        };

        // texture slots
        if (_args.material_texture_slots > 0)
        {
            result.definition.entries
                .emplace_back(result.set_index, 1, gfx::descriptor_type::SAMPLER, _args.material_texture_slots);
        }

        return result;
    }

    gfx::pipeline_descriptor_set world_geometry_material::node_descriptor_set_definition()
    {
        gfx::pipeline_descriptor_set result;
        result.set_index = 2;
        result.definition.entries = {
            gfx::descriptor_set_entry(result.set_index, 0, gfx::descriptor_type::UNIFORM, 1) // node params
        };

        // texture slots
        if (_args.node_texture_slots > 0)
        {
            result.definition.entries
                .emplace_back(result.set_index, 1, gfx::descriptor_type::SAMPLER, _args.node_texture_slots);
        }

        return result;
    }
} // namespace cathedral::engine