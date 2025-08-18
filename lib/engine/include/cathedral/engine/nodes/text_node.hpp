#pragma once

#include <cathedral/engine/font.hpp>
#include <cathedral/engine/nodes/bits/drawable_node.hpp>
#include <cathedral/engine/nodes/node.hpp>

namespace cathedral::engine
{
    class material;

    class text_node final : public drawable_node
    {
    public:
        using drawable_node::drawable_node;

        void set_text(std::u32string text);
        const std::u32string& text() const;

        void set_font_name(std::optional<std::string> font_name);
        std::optional<std::string> font_name() const;

        void tick_setup(scene& scene) override;

        std::shared_ptr<scene_node> copy(const std::string& name, bool copy_children) const override;

        constexpr const char* typestr() const override { return typestr_from_type(type()); }

        constexpr node_type type() const override { return node_type::TEXT_NODE; }

    private:
        std::u32string _text;
        bool _needs_update_text_buffer = true;

        std::optional<std::string> _font_name;
        std::shared_ptr<font> _font;
        bool _font_needs_update = true;
        bool _color_needs_update = true;
        bool _stride_needs_update = true;

        glm::vec3 _text_color = glm::vec3(1.0f, 1.0f, 1.0f);
        float _horizontal_stride = 0.05F;

        void render(scene& scene) override;

        void update_font(scene& scene);
        void update_text_buffer();
        void init_material(const scene& scene);
        void update_color();
        void update_horizontal_stride();
    };

    template <>
    std::shared_ptr<text_node> construct_node<text_node>(std::string name, scene_node* parent, bool enabled);
} // namespace cathedral::engine