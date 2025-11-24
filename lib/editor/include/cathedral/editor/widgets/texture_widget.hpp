#pragma once

#include <cathedral/engine/texture.hpp>

#include <memory>

namespace cathedral::editor
{
    class texture_widget
    {
    public:
        explicit texture_widget(std::shared_ptr<engine::texture> texture);

        void tick();
        void set_texture(std::shared_ptr<engine::texture> texture);

        static std::pair<float, float> size();

    private:
        std::shared_ptr<engine::texture> _texture;
        void* _imgui_texture = nullptr;

        bool _texture_changed = false;
    };
} // namespace cathedral::editor