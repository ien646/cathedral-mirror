#include "cathedral/engine/scene.hpp"

#include <cathedral/engine/nodes/text_node.hpp>

namespace cathedral::engine
{
    struct text_node_buffer_char
    {
        CATHEDRAL_ALIGNED_UNIFORM(glm::vec2, offset);
        CATHEDRAL_ALIGNED_UNIFORM(glm::vec2, size);
        CATHEDRAL_ALIGNED_UNIFORM(uint32_t, charcode);
    };

    void text_node::set_text(std::string text)
    {
        _text = std::move(text);
    }

    const std::string& text_node::text() const
    {
        return _text;
    }

    void text_node::set_font_name(std::optional<std::string> font_name)
    {
        _font_name = std::move(font_name);
    }

    std::optional<std::string> text_node::font_name() const
    {
        return _font_name;
    }

    void text_node::tick_setup(scene& scene)
    {
        drawable_node::tick_setup(scene);

        if (_font_needs_update)
        {
            update_font(scene);
        }
    }

    std::shared_ptr<scene_node> text_node::copy(const std::string& name, const bool copy_children) const
    {
        auto result = copy_drawable<text_node>(name, copy_children);

        result->_text = _text;
        result->_font_name = _font_name;

        return result;
    }

    void text_node::render(scene& scene)
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

        if (!_font_name.has_value() || _font == nullptr)
        {
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

        NOT_IMPLEMENTED();
    }

    void text_node::update_font(scene& scene)
    {
        _font_needs_update = false;

        if (!_font_name.has_value())
        {
            return;
        }

        _font = scene.load_font(*_font_name);
    }

    void text_node::update_text_buffer(scene& scene)
    {
        if (_material.expired())
        {
            return;
        }
    }
} // namespace cathedral::engine