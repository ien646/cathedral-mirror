#include "cathedral/editor/node_properties/point_light_properties_widget.hpp"

#include <cathedral/editor/properties_dock_widget.hpp>

#include <cathedral/editor/common/dock_title.hpp>

#include <cathedral/editor/node_properties/camera3d_properties_widget.hpp>
#include <cathedral/editor/node_properties/mesh3d_properties_widget.hpp>
#include <cathedral/editor/node_properties/node_properties_widget.hpp>
#include <cathedral/editor/node_properties/camera2d_properties_widget.hpp>

#include <QLayout>
#include <QScrollArea>

#include <utility>

namespace cathedral::editor
{
    properties_dock_widget::properties_dock_widget(
        project::project* pro,
        std::shared_ptr<engine::scene> scene,
        QWidget* parent)
        : QDockWidget(parent)
        , _project(pro)
        , _scene(std::move(scene))
    {
        layout()->setSpacing(0);
        layout()->setContentsMargins(8, 0, 0, 0);

        setTitleBarWidget(new dock_title("Properties", this));

        _scroll_area = new QScrollArea(this);
        setWidget(_scroll_area);

        _scroll_area->setWidget(new QWidget(this));
        _scroll_area->setWidgetResizable(true);
    }

    void properties_dock_widget::clear_node()
    {
        _scroll_area->setWidget(new QWidget(this));
        _scroll_area->setWidgetResizable(true);
    }

    void properties_dock_widget::set_node(engine::camera2d_node* node)
    {
        set_node_generic(new camera2d_properties_widget(_scroll_area, node));
    }

    void properties_dock_widget::set_node(engine::camera3d_node* node)
    {
        set_node_generic(new camera3d_properties_widget(_scroll_area, node));
    }

    void properties_dock_widget::set_node(engine::mesh3d_node* node)
    {
        set_node_generic(new mesh3d_properties_widget(_project, _scene, _scroll_area, node));
    }

    void properties_dock_widget::set_node(engine::node* node)
    {
        set_node_generic(new node_properties_widget(_project, _scroll_area, node, true));
    }

    void properties_dock_widget::set_node(engine::point_light_node* node)
    {
        set_node_generic(new point_light_properties_widget(_project, _scene, _scroll_area, node));
    }

    template <typename TWidget>
    void properties_dock_widget::set_node_generic(TWidget* widget)
    {
        _properties_widget = widget;

        const auto previous_size = _scroll_area->size();

        _scroll_area->setWidget(widget);
        _scroll_area->setWidgetResizable(true);

        _scroll_area->resize(previous_size);
    }
} // namespace cathedral::editor