#pragma once

#include <cathedral/core.hpp>

#include <QWidget>

FORWARD_CLASS_INLINE(QHBoxLayout);
FORWARD_CLASS_INLINE(QLabel);
FORWARD_CLASS_INLINE(QLineEdit);
FORWARD_CLASS_INLINE(QPushButton);

namespace cathedral::editor
{
    enum class path_selector_mode : uint8_t
    {
        FILE,
        DIRECTORY
    };

    class path_selector : public QWidget
    {
        Q_OBJECT

    public:
        path_selector(path_selector_mode mode, const QString& label = {}, QWidget* parent = nullptr);

        void set_text(const QString& str);
        QString text() const;

    private:
        path_selector_mode _mode;
        QHBoxLayout* _main_layout = nullptr;
        QLabel* _label = nullptr;
        QLineEdit* _line_edit = nullptr;
        QPushButton* _browse_button = nullptr;

        void handle_browse_click();

    signals:
        void paths_selected(QStringList paths);
    };
} // namespace cathedral::editor