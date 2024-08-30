#include <cathedral/editor/common/message.hpp>

#include <QMessageBox>
#include <QWidget>

namespace cathedral::editor
{
    void show_info_message(const QString& msg, QWidget* parent)
    {
        auto* msgbox =
            new QMessageBox(QMessageBox::Icon::Information, "Error", msg, QMessageBox::StandardButton::Ok, parent);
        msgbox->exec();
    }
    
    void show_error_message(const QString& msg, QWidget* parent)
    {
        auto* msgbox = new QMessageBox(QMessageBox::Icon::Critical, "Error", msg, QMessageBox::StandardButton::Ok, parent);
        msgbox->exec();
    }

    bool show_confirm_dialog(const QString& question, QWidget* parent)
    {
        auto* msgbox = new QMessageBox(QMessageBox::Icon::Critical, "Confirm", question, QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, parent);
        msgbox->exec();
        return msgbox->result() == QMessageBox::StandardButton::Yes;
    }
} // namespace cathedral::editor