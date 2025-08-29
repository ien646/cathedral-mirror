#include "cathedral/editor/common/message.hpp"

#include <cathedral/editor/dialogs/settings_dialog.hpp>

#include <cathedral/editor/utils.hpp>
#include <cathedral/engine/engine_settings.hpp>
#include <cathedral/project/project.hpp>

#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>

#include <magic_enum.hpp>

namespace cathedral::editor
{
    namespace
    {
        QWidget* get_edit_widget(
            QWidget* parent,
            project::project& pro,
            const std::string& name,
            const setting_value& value,
            bool& changed)
        {
            switch (value.type())
            {
            case setting_type::INT64: {
                auto* result = new QSpinBox(parent);
                result->setMinimum(std::numeric_limits<int>::min());
                result->setMaximum(std::numeric_limits<int>::max());
                result->setValue(value.as_int());
                parent->connect(result, &QSpinBox::valueChanged, parent, [name, &pro, &changed](const int v) {
                    const auto settings = pro.get_settings();
                    settings->set<int64_t>(name, v);
                    pro.save_settings();
                    changed = true;
                });
                return result;
            }
            case setting_type::DOUBLE: {
                auto* result = new QDoubleSpinBox(parent);
                result->setMinimum(std::numeric_limits<double>::lowest());
                result->setMaximum(std::numeric_limits<double>::max());
                result->setValue(value.as_double());
                parent->connect(result, &QDoubleSpinBox::valueChanged, parent, [name, &pro, &changed](const double v) {
                    const auto settings = pro.get_settings();
                    settings->set(name, v);
                    pro.save_settings();
                    changed = true;
                });
                return result;
            }
            case setting_type::STRING: {
                auto* result = new QLineEdit(parent);
                result->setText(QSTR(value.as_string()));
                parent->connect(result, &QLineEdit::textChanged, parent, [name, &pro, &changed](const QString& v) {
                    const auto settings = pro.get_settings();
                    settings->set(name, v.toStdString());
                    pro.save_settings();
                    changed = true;
                });
                return result;
            }
            case setting_type::EMPTY:
            default:
                return nullptr;
            }
        }
    } // namespace

    settings_dialog::settings_dialog(project::project& pro, QWidget* parent)
        : QDialog(parent)
        , _project(pro)
    {
        auto* main_layout = new QVBoxLayout(this);
        setLayout(main_layout);

        auto* tab_widget = new QTabWidget(this);
        main_layout->addWidget(tab_widget);

        tab_widget->addTab(init_engine_tab(), "Engine");
    }

    QWidget* settings_dialog::init_engine_tab()
    {
        auto* widget = new QWidget(this);
        auto* layout = new QFormLayout();
        widget->setLayout(layout);

        const auto engine_settings = engine::engine_settings_interface(_project.get_settings());

        for (const auto& [value, name] : magic_enum::enum_entries<engine::engine_setting>())
        {
            layout->addRow(
                QSTR(name),
                get_edit_widget(
                    this,
                    _project,
                    engine_settings.get_setting_key(value),
                    engine_settings.get(value),
                    _settings_changed));
        }

        return widget;
    }

    void settings_dialog::closeEvent(QCloseEvent* event)
    {
        if (_settings_changed)
        {
            show_info_message("Some settings will only be applied after editor restart");
        }
        QDialog::closeEvent(event);
    }
} // namespace cathedral::editor