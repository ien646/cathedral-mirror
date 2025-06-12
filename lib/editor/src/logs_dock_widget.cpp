#include <cathedral/editor/logs_dock_widget.hpp>

#include <cathedral/editor/common/dock_title.hpp>
#include <cathedral/editor/utils.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QTimer>
#include <qlayout.h>

namespace cathedral::editor
{
    class log_line_widget final : public QWidget
    {
    public:
        explicit log_line_widget(QWidget* parent, QColor background_color, const QString& text)
            : QWidget(parent)
        {
            setStyleSheet("QLabel{ padding: 0px; margin: 0px; color: #000000; background-color:" + background_color.name() + " }");

            setContentsMargins(0, 0, 0, 0);

            auto* const icon = new QLabel(this);
            icon->setText(">> ");
            auto* const label = new QLabel(this);
            label->setText(text);
            label->setWordWrap(true);

            icon->setContentsMargins(0, 0, 0, 0);
            label->setContentsMargins(0, 0, 0, 0);
            icon->setContentsMargins(10, 0, 10, 0);
            label->setContentsMargins(10, 0, 10, 0);

            auto* const layout = new QHBoxLayout(this);
            layout->setSpacing(0);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->addWidget(icon, 0);
            layout->addWidget(label, 1);

            setLayout(layout);
        }
    };

    const std::unordered_map<log_level, QColor> log_level_colors = { { log_level::INFO, QColor(150, 150, 200) },
                                                                     { log_level::WARNING, QColor(200, 200, 100) },
                                                                     { log_level::ERROR, QColor(255, 145, 145) } };

    logs_dock_widget::logs_dock_widget(QWidget* parent)
        : QDockWidget("Logs", parent)
    {
        _list = new QListWidget(this);
        _list->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        _list->setResizeMode(QListView::ResizeMode::Adjust);
        setWidget(_list);

        _timer = new QTimer(this);
        _timer->setInterval(200);
        _timer->start();

        setTitleBarWidget(new dock_title("Logs", this));

        connect(_timer, &QTimer::timeout, this, [this] {
            for (auto& [level, message] : get_global_log_database().take_log_lines())
            {
                auto* line_widget = new log_line_widget(this, log_level_colors.at(level), QSTR(message));
                auto* item = new QListWidgetItem(_list);
                _list->addItem(item);
                _list->setItemWidget(item, line_widget);
                _line_widgets.push_back(line_widget);
            }
        });
    }

    void logs_dock_widget::resizeEvent(QResizeEvent* event)
    {
        QDockWidget::resizeEvent(event);

        for (size_t i = 0; i < _line_widgets.size(); ++i)
        {
            _list->item(static_cast<int>(i))->setSizeHint({_list->width()-2, _line_widgets.at(i)->heightForWidth(_list->width())});
            update();
            _line_widgets.at(i)->update();
        }
    }
} // namespace cathedral::editor