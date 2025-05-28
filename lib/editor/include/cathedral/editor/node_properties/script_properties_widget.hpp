#pragma once

#include "cathedral/engine/scene_node.hpp"

#include <cathedral/core.hpp>

#include <QWidget>

FORWARD_CLASS_INLINE(QListWidget);

namespace cathedral::editor
{
    class script_properties_widget final : public QWidget
    {
    public:
        explicit script_properties_widget(std::shared_ptr<engine::scene_node> node);

        void update_list();

    private:
        std::shared_ptr<engine::scene_node> _node;
        QListWidget* _list = nullptr;
    };
} // namespace cathedral::editor