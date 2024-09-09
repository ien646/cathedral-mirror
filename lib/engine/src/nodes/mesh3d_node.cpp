#include <cathedral/engine/nodes/mesh3d_node.hpp>

#include <cathedral/engine/scene.hpp>

namespace cathedral::engine
{
    mesh3d_node::mesh3d_node(scene& scn, const std::string& name, scene_node* parent)
        : node(scn, name, parent)
    {
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

    void mesh3d_node::set_material(material* mat)
    {
        if (_material == mat)
        {
            return;
        }

        _texture_slots.clear();
        _material = mat;
        if (_material)
        {
            const auto node_uniform_size = _material->definition().node_uniform_block_size();
            if (node_uniform_size && _uniform_data.size() != node_uniform_size)
            {
                _uniform_data.resize(node_uniform_size);
                _mesh3d_uniform_buffer.reset();

                if (node_uniform_size > 0)
                {
                    gfx::uniform_buffer_args buff_args;
                    buff_args.size = _material->definition().node_uniform_block_size();
                    buff_args.vkctx = &_scene.get_renderer().vkctx();

                    _mesh3d_uniform_buffer = std::make_unique<gfx::uniform_buffer>(buff_args);
                }
            }

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

            const auto& buffer = _mesh3d_uniform_buffer ? _mesh3d_uniform_buffer
                                                        : _scene.get_renderer().empty_uniform_buffer();

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

        if (_material->definition().node_uniform_bindings().contains(shader_uniform_binding::NODE_MODEL_MATRIX))
        {
            const auto offset =
                _material->definition().node_uniform_bindings().at(shader_uniform_binding::NODE_MODEL_MATRIX);
            const auto& model = get_world_transform().get_model_matrix();
            CRITICAL_CHECK(_uniform_data.size() >= offset + sizeof(model));
            *reinterpret_cast<glm::mat4*>(_uniform_data.data() + offset) = model;
            _uniform_needs_update = true;
        }

        if (_uniform_needs_update)
        {
            _scene.get_renderer().get_upload_queue().update_buffer(*_mesh3d_uniform_buffer, 0, _uniform_data);
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
        if (defs.definition.entries.size() > 1)
        {
            for (uint32_t i = 0; i < defs.definition.entries[1].count; ++i)
            {
                bind_node_texture_slot(_scene.get_renderer().default_texture(), i);
            }
        }
    }
} // namespace cathedral::engine