#pragma once

#include <cathedral/engine/texture.hpp>

#include <memory>

namespace cathedral::editor2
{
    class texture_widget
    {
    public:
        explicit texture_widget(std::shared_ptr<engine::texture> texture);
        ~texture_widget();

        void tick();

        static std::pair<float, float> size();

    private:
        std::shared_ptr<engine::texture> _texture;
        void* _imgui_texture = nullptr;
    };
} // namespace cathedral::editor2