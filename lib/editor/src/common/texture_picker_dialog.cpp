#include <cathedral/editor/common/texture_picker_dialog.hpp>

#include <cathedral/editor/common/texture_list_widget.hpp>

#include <QPushButton>
#include <QVBoxLayout>

namespace cathedral::editor
{
    texture_picker_dialog::texture_picker_dialog(project::project& pro, QWidget* parent)
        : QDialog(parent)
    {
        resize(parent->size());

        auto* layout = new QVBoxLayout;
        setLayout(layout);

        _texture_list = new texture_list_widget(pro, this);
        layout->addWidget(_texture_list, 1);

        connect(_texture_list, &texture_list_widget::selection_changed, this, [this] { _select_button->setEnabled(true); });

        _select_button = new QPushButton("Select");
        _select_button->setEnabled(false);
        layout->addWidget(_select_button, 0, Qt::AlignmentFlag::AlignRight);
        connect(_select_button, &QPushButton::clicked, this, &texture_picker_dialog::accept);
    }

    const std::string& texture_picker_dialog::selected_name() const
    {
        return _texture_list->selected_path();
    }
} // namespace cathedral::editor