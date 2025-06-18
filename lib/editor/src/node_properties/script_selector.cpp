#include "cathedral/editor/asset_managers/script_manager.hpp"
#include "cathedral/editor/common/list_selector.hpp"
#include "cathedral/editor/utils.hpp"
#include "cathedral/engine/native_script_registry.hpp"
#include "cathedral/engine/scene_node.hpp"
#include "cathedral/project/utils.hpp"

#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <cathedral/editor/node_properties/script_selector.hpp>

namespace cathedral::editor
{
    script_selector::script_selector(QWidget* parent, project::project& project, engine::scene_node& scene_node)
        : QWidget(parent)
        , _project(project)
        , _scene_node(scene_node)
    {
        auto* main_layout = new QVBoxLayout(this);
        setLayout(main_layout);

        _script_list = new QListWidget(this);
        _script_list->setMinimumHeight(48);
        main_layout->addWidget(_script_list, 0);

        auto* buttons_layout = new QHBoxLayout(this);
        main_layout->addLayout(buttons_layout, 0);

        auto* add_button = new QPushButton("+", this);
        auto* remove_button = new QPushButton("-", this);
        auto* move_up_button = new QPushButton("v", this);
        auto* move_down_button = new QPushButton("^", this);
        add_button->setMinimumWidth(16);
        remove_button->setMinimumWidth(16);
        move_up_button->setMinimumWidth(16);
        move_down_button->setMinimumWidth(16);

        remove_button->setEnabled(false);
        remove_button->setEnabled(false);
        remove_button->setEnabled(false);

        buttons_layout->addWidget(add_button);
        buttons_layout->addWidget(remove_button);
        buttons_layout->addWidget(move_up_button);
        buttons_layout->addWidget(move_down_button);

        reload_script_list();

        connect(
            _script_list,
            &QListWidget::itemSelectionChanged,
            this,
            [this, remove_button, move_up_button, move_down_button] {
                if (_script_list->selectedItems().size() > 0)
                {
                    remove_button->setEnabled(true);
                    const auto& selected_item = _script_list->selectedItems()[0];
                    const auto row = _script_list->indexFromItem(selected_item).row();

                    move_up_button->setEnabled(row != 0);
                    move_down_button->setEnabled(row != _script_list->count() - 1);
                }
                else
                {
                    remove_button->setEnabled(false);
                    move_up_button->setEnabled(false);
                    move_down_button->setEnabled(false);
                }
            });

        connect(add_button, &QPushButton::clicked, this, [this] {
            const QPixmap dynamic_icon(":/icons/dynamic_icon.png");
            const QPixmap native_icon(":/icons/native_icon.png");

            std::unordered_set<std::string> native_scripts;
            for (const auto& name : engine::get_native_script_registry() | std::views::keys)
            {
                native_scripts.emplace(name);
            }

            std::vector<list_selector_item> items;
            for (const auto& name : _project.script_assets() | std::views::keys)
            {
                if (native_scripts.contains(name))
                {
                    native_scripts.erase(name);
                }
                items.emplace_back(QSTR(name), dynamic_icon);
            }

            for (const auto& native_script : native_scripts)
            {
                items.emplace_back(QSTR(native_script), native_icon);
            }

            std::ranges::sort(items, [](const list_selector_item& lhs, const list_selector_item& rhs) {
                return lhs.text < rhs.text;
            });

            auto* selector = new list_selector(this, std::move(items));
            selector->exec();

            if (selector->result() == QDialog::Accepted)
            {
                const auto& name = selector->selected_value();
                _scene_node.add_script(name.toStdString());
                reload_script_list();
            }
        });

        connect(remove_button, &QPushButton::clicked, this, [this] {
            if (_script_list->selectedItems().size() > 0)
            {
                const auto& name = _script_list->selectedItems().at(0)->text();
                _scene_node.remove_script(name.toStdString());
            }
            reload_script_list();
        });

        connect(move_up_button, &QPushButton::clicked, this, [this] { log_error("Not implemented!"); });

        connect(move_down_button, &QPushButton::clicked, this, [this] { log_error("Not implemented!"); });
    }

    void script_selector::reload_script_list() const
    {
        _script_list->clear();
        for (const auto& name : _scene_node.script_names())
        {
            const QPixmap dynamic_icon(":/icons/dynamic_icon.png");
            const QPixmap native_icon(":/icons/native_icon.png");
            const QPixmap overriden_icon(":/icons/overriden_icon.png");

            const project::script_type type = project::get_script_type(name, _project);
            const QPixmap& icon = [&] {
                switch (type)
                {
                case project::script_type::NATIVE:
                    return native_icon;
                case project::script_type::DYNAMIC:
                    return dynamic_icon;
                case project::script_type::DYNAMIC_OVERRIDING:
                    return overriden_icon;
                case project::script_type::NOT_FOUND:
                default:
                    CRITICAL_ERROR("Invalid script type");
                }
            }();

            auto* list_item = new QListWidgetItem(QIcon(icon), QSTR(name));
            _script_list->addItem(list_item);
        }
    }
} // namespace cathedral::editor