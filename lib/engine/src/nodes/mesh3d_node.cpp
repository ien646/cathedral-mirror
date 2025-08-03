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
                _mesh_buffers = scene.get_mesh_buffers(*_mesh_name, *_mesh);
            }
            else
            {
                return;
            }
        }

        if (_mesh_buffers == nullptr)
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

        auto& [vxbuff, ixbuff] = *_mesh_buffers;

        const auto cmdbuff_type = [&] {
            switch (material->domain())
            {
            case material_domain::OPAQUE:
                return render_cmdbuff_type::OPAQUE;
            case material_domain::TRANSPARENT:
                return render_cmdbuff_type::TRANSPARENT;
            case material_domain::OVERLAY:
                return render_cmdbuff_type::OVERLAY;
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
            { scene.descriptor_set(), material->descriptor_set(), *_descriptor_set },
            {});
        cmdbuff.bindVertexBuffers(0, vxbuff.buffer(), { 0 });
        cmdbuff.bindIndexBuffer(ixbuff.buffer(), 0, vk::IndexType::eUint32);
        cmdbuff.drawIndexed(ixbuff.index_count(), 1, 0, 0, 0);
    }

    std::shared_ptr<scene_node> mesh3d_node::copy(const std::string& name, const bool copy_children) const
    {
        auto result = std::make_shared<mesh3d_node>(name, _parent, !_disabled);

        node::copy_into(*result, copy_children);

        if (_mesh_name.has_value())
        {
            result->set_mesh(_mesh_name);
        }
        result->set_material(_material_name);

        for (uint32_t i = 0; i < static_cast<uint32_t>(_texture_slots.size()); ++i)
        {
            result->bind_node_texture_slot(_texture_names[i], i);
        }

        result->_uniform_data = _uniform_data;

        return result;
    }
} // namespace cathedral::engine