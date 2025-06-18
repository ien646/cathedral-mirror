#pragma once

#include <cathedral/core.hpp>

#include <QWidget>

FORWARD_CLASS(cathedral::engine, scene);
FORWARD_CLASS(cathedral::engine, scene_node);

namespace cathedral::editor
{
    class script_selector final : public QWidget
    {
    public:
        script_selector(QWidget* parent, project::project& project, engine::scene& scene, engine::scene_node& scene_node);

    private:
        project::project& _project;
        engine::scene& _scene;
        engine::scene_node& _scene_node;

        QListWidget* _script_list = nullptr;

        void reload_script_list() const;
    };
} // namespace cathedral::editor