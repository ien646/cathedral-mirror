#pragma once

#include <QDialog>
#include <print>

class QPushButton;

namespace cathedral::project
{
    class project;
}

namespace cathedral::editor
{
    class texture_list_widget;

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