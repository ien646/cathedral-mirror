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

        void render(scene& scene) override;

        void update_font(scene& scene);
        void update_text_buffer();
    };
} // namespace cathedral::engine