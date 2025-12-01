#pragma once

#include <cathedral/core.hpp>
#include <cathedral/editor/callback_decl.hpp>
#include <cathedral/editor/widgets/mesh_widget.hpp>

#include <functional>

FORWARD_CLASS(cathedral::engine, scene);
FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor
{
    class mesh_selector
    {
    public:
        mesh_selector(project::project& project, engine::scene& scene);
        void set_mesh_list(std::vector<std::string> names);

        void open();

        void tick();

        CATHEDRAL_DECLARE_CALLBACKS((selected, const std::string&));

    private:
        project::project& _project;
        engine::scene& _scene;
        one_time_flag _open_flag{ false };
        std::vector<std::string> _mesh_list;
        std::vector<mesh_widget> _mesh_widgets;
        std::string _filter;
        std::string _selected;
    };
} // namespace cathedral::editor