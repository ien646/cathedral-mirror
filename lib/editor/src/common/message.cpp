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
        auto* msgbox = new QMessageBox(
            QMessageBox::Icon::Critical,
            "Confirm",
            question,
            QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No,
            parent);
        msgbox->exec();
        return msgbox->result() == QMessageBox::StandardButton::Yes;
    }

    void show_info_message(const std::string& msg, QWidget* parent)
    {
        show_info_message(QString::fromStdString(msg), parent);
    }

    void show_error_message(const std::string& msg, QWidget* parent)
    {
        show_error_message(QString::fromStdString(msg), parent);
    }

    bool show_confirm_dialog(const std::string& question, QWidget* parent)
    {
        return show_confirm_dialog(QString::fromStdString(question), parent);
    }

    void show_info_message(const char* msg, QWidget* parent)
    {
        show_info_message(QString{ msg }, parent);
    }

    void show_error_message(const char* msg, QWidget* parent)
    {
        show_error_message(QString{ msg }, parent);
    }

    bool show_confirm_dialog(const char* question, QWidget* parent)
    {
        return show_confirm_dialog(QString{ question }, parent);
    }
} // namespace cathedral::editor