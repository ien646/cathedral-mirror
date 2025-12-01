#pragma once

#include <cathedral/core.hpp>
#include <cathedral/engine/texture.hpp>

#include <imgui.h>

#include <memory>

FORWARD_CLASS(cathedral::engine, renderer);
FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor
{
    class mesh_widget
    {
    public:
        mesh_widget(std::string mesh_name, project::project& project, engine::renderer& renderer);

        void tick();

        static std::pair<float, float> size();

    private:
        project::project& _project;
        engine::renderer& _renderer;
        std::string _mesh_name;

        std::shared_ptr<engine::texture> _texture;
        void* _imgui_texture;

        void init_texture();
    };
} // namespace cathedral::editor