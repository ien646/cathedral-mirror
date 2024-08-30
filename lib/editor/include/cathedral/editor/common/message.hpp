#pragma once

class QString;
class QWidget;

namespace cathedral::editor
{
    void show_info_message(const QString& msg, QWidget* parent = nullptr);
    void show_error_message(const QString& msg, QWidget* parent = nullptr);
    bool show_confirm_dialog(const QString& question, QWidget* parent = nullptr);
}