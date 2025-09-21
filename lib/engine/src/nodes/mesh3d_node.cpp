#include <cathedral/engine/nodes/mesh3d_node.hpp>

#include <cathedral/engine/scene.hpp>

namespace cathedral::engine
{
    void mesh3d_node::render(scene& scene)
    {
        if (_disabled || (_disabled_in_editor && scene.in_editor_mode()))
        {
            return;
        }

        if (_material.expired())
        {
            if (_material_name.has_value())
            {
                _material = scene.load_material(*_material_name);
                _material_uid = _material.expired() ? std::numeric_limits<uint32_t>::max() : _material.lock()->uid();
                _needs_update_material = true;
            }
            else
            {
                _material = {};
                _material_uid = std::numeric_limits<uint32_t>::max();
                _needs_update_material = true;
            }
            // Avoid rendering the current frame, since modified material resources have to be
            // recreated (i.e. descriptors), and doing it mid frame is a no-no
            return;
        }

        if (_needs_update_mesh)
        {
            _needs_update_mesh = false;
            if (_mesh_name.has_value())
            {
                _mesh = scene.load_mesh(*_mesh_name);
                _mesh_buffers = renderer_resource(scene.get_mesh_buffers(*_mesh_name, *_mesh), &scene.get_renderer());
            }
            else
            {
                return;
            }
        }

        if (*_mesh_buffers == nullptr)
        {
            return;
        }

        const auto material = _material.lock();

        update_bindings();

        if (_uniform_needs_update && !_uniform_data.empty())
        {
            scene.get_renderer().get_upload_queue().update_buffer(*_node_uniform_buffer, 0, _uniform_data);
            _uniform_needs_update = false;
        }

        auto& [vxbuff, ixbuff] = **_mesh_buffers;

        const auto cmdbuff_type = [&] {
            switch (material->domain())
            {
            case material_domain::OPAQUE:
                return render_domain::OPAQUE;
            case material_domain::TRANSPARENT:
                return render_domain::TRANSPARENT;
            case material_domain::OVERLAY:
                return render_domain::OVERLAY;
            default:
                CRITICAL_ERROR("Unhandled material definition domain");
            }
        }();

        const vk::CommandBuffer cmdbuff = scene.get_renderer().render_cmdbuff(cmdbuff_type);
        cmdbuff.bindPipeline(vk::PipelineBindPoint::eGraphics, material->pipeline().get());
        cmdbuff.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            material->pipeline().pipeline_layout(),
            0,
            { scene.descriptor_set(), material->descriptor_set(), **_descriptor_set },
            {});
        cmdbuff.bindVertexBuffers(0, vxbuff.buffer(), { 0 });
        cmdbuff.bindIndexBuffer(ixbuff.buffer(), 0, vk::IndexType::eUint32);
        cmdbuff.drawIndexed(ixbuff.index_count(), _instance_count, 0, 0, 0);
    }

    std::unique_ptr<scene_node> mesh3d_node::copy(const std::string& name, const bool copy_children) const
    {
        return copy_drawable<mesh3d_node>(name, copy_children);
    }

    template <>
    std::unique_ptr<mesh3d_node> construct_node<mesh3d_node>(std::string name, scene_node* parent, bool enabled)
    {
        return std::make_unique<mesh3d_node>(std::move(name), parent, enabled);
    }
} // namespace cathedral::engine