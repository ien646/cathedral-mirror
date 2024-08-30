#include <cathedral/engine/nodes/mesh3d_node.hpp>

#include <cathedral/engine/scene.hpp>

namespace cathedral::engine
{
    mesh3d_node::mesh3d_node(scene& scn, const std::string& name, scene_node* parent)
        : node(scn, name, parent)
    {
        gfx::uniform_buffer_args uniform_buffer_args;
        uniform_buffer_args.size = sizeof(mesh3d_node_uniform_data);
        uniform_buffer_args.vkctx = &_scene.get_renderer().vkctx();

        _mesh3d_uniform_buffer = std::make_unique<gfx::uniform_buffer>(uniform_buffer_args);
    }

    void mesh3d_node::set_mesh(const std::string& path)
    {
        _mesh_buffers = _scene.get_mesh_buffers(path);
        _mesh_path = path;
    }

    void mesh3d_node::set_mesh(std::shared_ptr<mesh_buffer> mesh_buffer)
    {
        _mesh_buffers = mesh_buffer;
        _mesh_path = std::nullopt;
    }

    void mesh3d_node::set_material(world_geometry_material* mat)
    {
        _texture_slots.clear();

        _material = mat;
        if (_material)
        {
            if (!_descriptor_set)
            {
                const auto layout = _material->node_descriptor_set_layout();
                vk::DescriptorSetAllocateInfo alloc_info;
                alloc_info.descriptorPool = _scene.get_renderer().vkctx().descriptor_pool();
                alloc_info.descriptorSetCount = 1;
                alloc_info.pSetLayouts = &layout;

                _descriptor_set =
                    std::move(_scene.get_renderer().vkctx().device().allocateDescriptorSetsUnique(alloc_info)[0]);
            }

            vk::DescriptorBufferInfo buffer_info;
            buffer_info.buffer = _mesh3d_uniform_buffer->buffer();
            buffer_info.offset = 0;
            buffer_info.range = _mesh3d_uniform_buffer->size();

            vk::WriteDescriptorSet write;
            write.descriptorCount = 1;
            write.descriptorType = vk::DescriptorType::eUniformBuffer;
            write.pBufferInfo = &buffer_info;
            write.dstArrayElement = 0;
            write.dstBinding = 0;
            write.dstSet = *_descriptor_set;
            _scene.get_renderer().vkctx().device().updateDescriptorSets(write, {});

            init_default_textures();
        }
    }

    void mesh3d_node::bind_node_texture_slot(std::shared_ptr<texture> tex, uint32_t slot)
    {
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

        _scene.get_renderer().vkctx().device().updateDescriptorSets(write, {});
    }

    void mesh3d_node::tick(double deltatime)
    {
        node::tick(deltatime);
        if (!_material)
        {
            return;
        }

        const auto& model = get_world_transform().get_model_matrix();
        if (_uniform_data.model_matrix != model)
        {
            _uniform_data.model_matrix = model;
            _uniform_needs_update = true;
        }

        if (_uniform_needs_update)
        {
            _scene.get_renderer()
                .get_upload_queue()
                .update_buffer(*_mesh3d_uniform_buffer, 0, &_uniform_data, sizeof(_uniform_data));
            _uniform_needs_update = false;
        }

        auto& [vxbuff, ixbuff] = *_mesh_buffers;

        vk::CommandBuffer cmdbuff = _scene.get_renderer().render_cmdbuff();
        cmdbuff.bindPipeline(vk::PipelineBindPoint::eGraphics, _material->pipeline().get());
        cmdbuff.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            _material->pipeline().pipeline_layout(),
            0,
            { _scene.descriptor_set(), _material->descriptor_set(), *_descriptor_set },
            {});
        cmdbuff.bindVertexBuffers(0, vxbuff.buffer(), { 0 });
        cmdbuff.bindIndexBuffer(ixbuff.buffer(), 0, vk::IndexType::eUint32);
        cmdbuff.drawIndexed(ixbuff.index_count(), 1, 0, 0, 0);
    }

    void mesh3d_node::init_default_textures()
    {
        const auto defs = _material->node_descriptor_set_definition();
        if (defs.definition.entries.size() >= 1)
        {
            for (size_t i = 0; i < defs.definition.entries[1].count; ++i)
            {
                bind_node_texture_slot(_scene.get_renderer().default_texture(), i);
            }
        }
    }
} // namespace cathedral::engine