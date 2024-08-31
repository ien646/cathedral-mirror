#pragma once

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

namespace cathedral::editor
{
    class path_selector : public QWidget
    {
    public:
        path_selector(QWidget* parent);

        void set_text(const QString& str);

    private:
        QHBoxLayout* _main_layout = nullptr;
        QLineEdit* _line_edit = nullptr;
        QPushButton* _browse_button = nullptr;
    };
}