#pragma once

#include <cathedral/editor2/widget.hpp>
#include <functional>

namespace cathedral::editor2
{
    struct menubar_callbacks
    {
        std::function<void()> new_project = nullptr;
        std::function<void()> open_project = nullptr;
        std::function<void()> close = nullptr;

        std::function<void()> new_scene = nullptr;
        std::function<void()> open_scene = nullptr;
        std::function<void()> save_scene = nullptr;
        std::function<void()> save_scene_as = nullptr;

        std::function<void()> material_manager = nullptr;
        std::function<void()> mesh_manager = nullptr;
        std::function<void()> script_manager = nullptr;
        std::function<void()> shader_manager = nullptr;
        std::function<void()> texture_manager = nullptr;

        std::function<void()> capture_screenshot = nullptr;
        std::function<void()> about = nullptr;
    };

    class menubar final : public widget<menubar_callbacks>
    {
    public:
        void tick() override;
    };
} // namespace cathedral::editor2