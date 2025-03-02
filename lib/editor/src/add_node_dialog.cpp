#include <cathedral/editor/add_node_dialog.hpp>

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

namespace cathedral::editor
{
    add_node_dialog::add_node_dialog(QWidget* parent)
        : QDialog(parent)
    {
        setWindowModality(Qt::WindowModality::ApplicationModal);

        auto* mainLayout = new QVBoxLayout();
    }
}