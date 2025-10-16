#pragma once

#include <cathedral/engine/texture.hpp>

#include <memory>

namespace cathedral::editor2
{
    class texture_select_widget
    {
    public:
        explicit texture_select_widget(std::shared_ptr<engine::texture> texture);

    private:
        std::shared_ptr<engine::texture> _texture;
        ImTex
    };
} // namespace cathedral::editor2