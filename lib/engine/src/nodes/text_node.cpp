#include <cathedral/engine/nodes/text_node.hpp>

#include <cathedral/engine/primitives/quad.hpp>
#include <cathedral/engine/scene.hpp>

#include <ranges>

namespace cathedral::engine
{
    struct text_node_buffer_char
    {
        CATHEDRAL_ALIGNED_UNIFORM(glm::vec2, offset);
        CATHEDRAL_ALIGNED_UNIFORM(glm::vec2, size);
        CATHEDRAL_ALIGNED_UNIFORM(uint32_t, charcode);
    };

    void text_node::set_text(std::u32string text)
    {
        _text = std::move(text);
    }

    const std::u32string& text_node::text() const
    {
        return _text;
    }

    void text_node::set_font_name(std::optional<std::string> font_name)
    {
        _font_name = std::move(font_name);
        _font_needs_update = true;
    }

    std::optional<std::string> text_node::font_name() const
    {
        return _font_name;
    }

    void text_node::tick_setup(scene& scene)
    {
        drawable_node::tick_setup(scene);

        // Manually set the mesh to the default quad
        if (_mesh == nullptr)
        {
            _mesh = std::make_shared<mesh>(primitives::quad_mesh());
            _mesh_buffers = scene.get_mesh_buffers("__cathedral_default_quad_mesh", *_mesh);
            _needs_update_mesh = false;
        }

        if (_font_needs_update)
        {
            update_font(scene);
        }

        if (_needs_update_text_buffer)
        {
            update_text_buffer();
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
        cmdbuff.drawIndexed(ixbuff.index_count(), static_cast<uint32_t>(_text.size()), 0, 0, 0);
    }

    void text_node::update_font(scene& scene)
    {
        //_font_needs_update = false;

        if (!_font_name.has_value())
        {
            return;
        }

        _font = scene.load_font(*_font_name);

        if (_material.expired())
        {
            return;
        }

        const auto& material = _material.lock();
        const auto it = std::ranges::find_if(material->node_texture_bindings(), [](const auto& b) {
            return b.second == shader_node_texture_binding::TEXT_NODE_FONT_ATLAS;
        });

        if (it == material->node_texture_bindings().end())
        {
            log_warning(
                "text_node font was set, but the currently assigned material has no binding for text node atlas texture");
            return;
        }

        const auto& texture_name = it->first;
        const auto slot = material->get_node_texture_slot(texture_name);
        if (!slot.has_value())
        {
            log_error(
                std::format("Unable to find texture slot index for text node atlas texture with name '{}'", texture_name));
            return;
        }
        bind_node_texture_slot(scene.get_renderer(), _font->atlas_texture(), *slot);
    }

    void text_node::update_text_buffer()
    {
        if (_material.expired())
        {
            return;
        }

        const auto& material = _material.lock();
        const auto it = std::ranges::find_if(material->node_buffer_bindings(), [](const auto& b) {
            return b.second == shader_node_buffer_binding::TEXT_NODE_BUFFER;
        });

        if (it == material->node_buffer_bindings().end())
        {
            log_warning("text_node text was set, but the currently assigned material has no binding for text node buffer");
            return;
        }

        const auto& binding_name = it->first;
        const auto binding_index = material->get_node_buffer_index(binding_name);

        if (!binding_index.has_value())
        {
            log_error(std::format("Unable to find binding index for text node buffer with name '{}'", binding_name));
            return;
        }

        std::vector<std::byte> buffer_data;
        buffer_data.reserve(_text.size() * sizeof(text_node_buffer_char));

        const glm::vec2 image_size{ _font->atlas_texture()->image().width(), _font->atlas_texture()->image().height() };

        for (const char32_t ch : _text)
        {
            text_node_buffer_char bch{};
            bch.charcode = static_cast<uint32_t>(ch);
            bch.offset = glm::vec2{ _font->glyph_rects()[ch].offset } / glm::vec2{_font->glyph_bbox_size()};
            bch.size = glm::vec2{ _font->glyph_rects()[ch].size } / glm::vec2{_font->glyph_bbox_size()};

            const auto view = std::as_bytes(std::span{ &bch, 1 });
            for (const auto& b : view)
            {
                buffer_data.push_back(b);
            }
        }

        set_storage_buffer_data(*binding_index, std::move(buffer_data));
    }

    template <>
    std::shared_ptr<text_node> construct_node<text_node>(std::string name, scene_node* parent, bool enabled)
    {
        return std::make_shared<text_node>(std::move(name), parent, enabled);
    }
} // namespace cathedral::engine