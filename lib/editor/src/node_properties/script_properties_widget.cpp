#include <cathedral/editor/node_properties/script_properties_widget.hpp>

#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>

namespace cathedral::editor
{
    script_properties_widget::script_properties_widget(std::shared_ptr<engine::scene_node> node)
        : _node(node)
    {
        auto* layout = new QVBoxLayout(this);
        layout->addWidget(new QLabel("Scripts"));

        _list = new QListWidget(this);

        setLayout(layout);

        update_list();
    }

    void script_properties_widget::update_list()
    {
        _list->clear();

        NOT_IMPLEMENTED();
    }
} // namespace cathedral::editor