#pragma once

#include <QWidget>

namespace cathedral::project
{
    class project;
}

namespace cathedral::editor
{
    class texture_slot_widget;

    class texture_list_widget : public QWidget
    {
        Q_OBJECT

    public:
        texture_list_widget(project::project& pro, QWidget* parent = nullptr);

        const auto& selected_path() const { return _selected_path; };

    signals:
        void selection_changed();

    private:
        project::project& _project;
        std::vector<texture_slot_widget*> _slot_widgets;
        std::string _selected_path;
    };
} // namespace cathedral::editor