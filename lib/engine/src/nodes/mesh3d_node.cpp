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

    void mesh3d_node::set_mesh(std::shared_ptr<std::pair<gfx::vertex_buffer, gfx::index_buffer>> mesh_buffers)
    {
        _mesh_buffers = mesh_buffers;
        _mesh_path = std::nullopt;
    }

    void mesh3d_node::set_material(world_geometry_material* mat)
    {
        _material = mat;
        if (_material)
        {
            const auto layout = _material->drawable_descriptor_set_layout();
            vk::DescriptorSetAllocateInfo alloc_info;
            alloc_info.descriptorPool = _scene.get_renderer().vkctx().descriptor_pool();
            alloc_info.descriptorSetCount = 1;
            alloc_info.pSetLayouts = &layout;

            _descriptor_set =
                std::move(_scene.get_renderer().vkctx().device().allocateDescriptorSetsUnique(alloc_info)[0]);

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
        }
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
} // namespace cathedral::engine