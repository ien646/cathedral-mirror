#pragma once

#include <cathedral/core.hpp>

#include <QWidget>

FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor
{
    FORWARD_CLASS_INLINE(texture_slot_widget);

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