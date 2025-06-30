#include <cathedral/editor/logs_dock_widget.hpp>

#include <cathedral/editor/common/dock_title.hpp>
#include <cathedral/editor/utils.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QTimer>

namespace cathedral::editor
{
    class log_line_widget final : public QWidget
    {
    public:
        explicit log_line_widget(QWidget* parent, QColor background_color, const QString& text)
            : QWidget(parent)
            , _text(text)
            , _text_label(new QLabel(this))
        {
            setObjectName("log_line_widget");

            setStyleSheet(
                "QLabel{ padding: 0px; margin: 0px; color: #000000; background-color:" + background_color.name() + " }");

            setContentsMargins(0, 0, 0, 0);

            auto* const icon = new QLabel(this);
            icon->setText(">> ");
            _text_label->setText(text);
            _text_label->setWordWrap(true);

            icon->setContentsMargins(0, 0, 0, 0);
            _text_label->setContentsMargins(0, 0, 0, 0);
            icon->setContentsMargins(10, 0, 10, 0);
            _text_label->setContentsMargins(10, 0, 10, 0);

            auto* const layout = new QHBoxLayout;
            layout->setSpacing(0);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->addWidget(icon, 0);
            layout->addWidget(_text_label, 1);

            setLayout(layout);
        }

        const auto& text() const { return _text; }

        void increase_count(const int amount = 1)
        {
            _count += amount;
            _text_label->setText(QString{ "[times: %1] %2" }.arg(QString::number(_count)).arg(_text));
        }

    private:
        int _count = 1;
        QString _text;
        QLabel* _text_label = nullptr;
    };

    const std::unordered_map<log_level, QColor> log_level_colors = { { log_level::INFO, QColor(150, 150, 200) },
                                                                     { log_level::WARNING, QColor(200, 200, 100) },
                                                                     { log_level::ERROR, QColor(255, 145, 145) } };

    logs_dock_widget::logs_dock_widget(QWidget* parent)
        : QDockWidget("Logs", parent)
    {
        setObjectName("logs_dock_widget");

        _log = new QTextEdit(this);
        _log->setReadOnly(true);
        setWidget(_log);

        _timer = new QTimer(this);
        _timer->setInterval(200);
        _timer->start();

        setTitleBarWidget(new dock_title("Logs", this));

        connect(_timer, &QTimer::timeout, this, [this] {
            for (auto& [level, message] : get_global_log_database().take_log_lines())
            {
                _log->append(
                    QString{ "<span style='background-color:%1'>%2</span>" }
                        .arg(log_level_colors.at(level).name())
                        .arg(QString::fromStdString(std::move(message))));
            }
        });
    }

    void logs_dock_widget::resizeEvent(QResizeEvent* event)
    {
        QDockWidget::resizeEvent(event);
    }
} // namespace cathedral::editor