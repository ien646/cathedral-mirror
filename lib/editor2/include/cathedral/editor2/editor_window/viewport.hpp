#pragma once

#include <cathedral/core.hpp>

#include <glm/vec2.hpp>

#include <imgui.h>

FORWARD_CLASS(cathedral::engine, scene);

namespace cathedral::editor2
{
    class editor_viewport
    {
    public:
        void tick(ImGuiID dockspace_id);

        glm::ivec2 position();
        glm::ivec2 size();

    private:
        glm::ivec2 _position{0, 0};
        glm::ivec2 _size{1, 1};
    };
} // namespace cathedral::editor2