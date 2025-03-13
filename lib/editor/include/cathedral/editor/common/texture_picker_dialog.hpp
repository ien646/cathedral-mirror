#pragma once

#include <cathedral/core.hpp>

#include <QDialog>
#include <print>

class QPushButton;

FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor
{
    FORWARD_CLASS_INLINE(texture_list_widget);

    class texture_picker_dialog : public QDialog
    {
    public:
        texture_picker_dialog(project::project& pro, QWidget* parent = nullptr);
        const std::string& selected_name() const;

    private:
        texture_list_widget* _texture_list = nullptr;
        QPushButton* _select_button = nullptr;
    };
} // namespace cathedral::editor