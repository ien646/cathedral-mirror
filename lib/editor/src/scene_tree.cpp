
#include <cathedral/editor/scene_tree.hpp>

#include <cathedral/editor/add_node_dialog.hpp>
#include <cathedral/editor/utils.hpp>

#include <cathedral/editor/common/message.hpp>
#include <cathedral/editor/common/text_input_dialog.hpp>

#include <cathedral/editor/editor_nodes/gizmos/translation_gizmo.hpp>

#include <cathedral/engine/nodes/mesh3d_node.hpp>
#include <cathedral/engine/nodes/node.hpp>

#include <QMenu>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>

#include <ranges>
#include <stack>

namespace cathedral::editor
{
    scene_tree::scene_tree(QWidget* parent)
        : QTreeWidget(parent)
    {
        setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
        setFocusPolicy(Qt::FocusPolicy::NoFocus);
        setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);

        setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

        _refresh_timer = new QTimer(this);
        _refresh_timer->setTimerType(Qt::TimerType::CoarseTimer);
        _refresh_timer->setSingleShot(false);
        _refresh_timer->setInterval(500);
        _refresh_timer->start();

        connect(this, &QTreeWidget::itemClicked, this, [this](QTreeWidgetItem* item, [[maybe_unused]] int col) {
            auto node = get_node_for_tree_item(item);
            _selected_node = node;

            emit node_selected(node.get());
            update_tree();
        });

        connect(this, &QTreeWidget::itemExpanded, this, [this](QTreeWidgetItem* item) {
            _expanded_nodes.insert(get_node_for_tree_item(item)->get_full_name("/"));
            update();
        });

        connect(this, &QTreeWidget::itemCollapsed, this, [this](QTreeWidgetItem* item) {
            _expanded_nodes.erase(get_node_for_tree_item(item)->get_full_name("/"));
            update();
        });

        connect(this, &QTreeWidget::customContextMenuRequested, this, &SELF::handle_custom_context_menu_request);

        connect(_refresh_timer, &QTimer::timeout, this, [this] { update_tree(); });

        update_tree();
    }

    void scene_tree::set_scene(engine::scene* scene)
    {
        _scene = scene;

        _selected_node = {};
        _translation_gizmo = {};
        _expanded_nodes.clear();
        _node_to_item.clear();
        _item_to_node.clear();

        clear();
        update_tree();
    }

    void scene_tree::reload()
    {
        _selected_node = {};
        emit node_selected(nullptr);
        _translation_gizmo->disable();
        update_tree();
    }

    void scene_tree::update_tree()
    {
        if (_scene != nullptr)
        {
            for (const auto& node : _scene->root_nodes())
            {
                process_node(nullptr, *node, node->name());
            }

            if (_translation_gizmo == nullptr)
            {
                _translation_gizmo = std::dynamic_pointer_cast<engine::node>(get_translation_gizmo_node(*_scene));
            }
            
            if (_selected_node.expired())
            {
                _translation_gizmo->disable();
            }
            else if (const auto& selected_node = std::dynamic_pointer_cast<engine::node>(_selected_node.lock()))
            {
                _translation_gizmo->enable();
                _translation_gizmo->set_local_position(selected_node->local_position());
            }
        }

        viewport()->update();
    }

    void scene_tree::process_node(QTreeWidgetItem* parent_widget, engine::scene_node& scene_node, const std::string& name)
    {
        QTreeWidgetItem* current_widget = nullptr;
        if (_node_to_item.contains(&scene_node))
        {
            current_widget = _node_to_item.at(&scene_node);
        }
        else if (!scene_node.hidden_in_editor())
        {
            if (parent_widget != nullptr)
            {
                current_widget = new QTreeWidgetItem(parent_widget);
            }
            else
            {
                current_widget = new QTreeWidgetItem(this);
            }
            _node_to_item.emplace(&scene_node, current_widget);
            _item_to_node.emplace(current_widget, &scene_node);
            current_widget->setText(0, QString::fromStdString(name));
            if (_expanded_nodes.contains(scene_node.get_full_name("/")))
            {
                current_widget->setExpanded(true);
            }
        }

        for (const auto& child_node : scene_node.children())
        {
            const std::string child_name = std::string(child_node->name());
            process_node(current_widget, *child_node, child_name);
        }
    }

    void scene_tree::paintEvent(QPaintEvent* ev)
    {
        QTreeWidget::paintEvent(ev);

        if (_scene == nullptr)
        {
            return;
        }

        if (!_selected_node.expired())
        {
            const auto* selected_item = get_tree_item_for_node(_selected_node.lock().get());
            if (selected_item != nullptr)
            {
                const QRect child_rect = visualItemRect(selected_item);
                QPainter painter(viewport());
                painter.setPen(QColor(0x888888));
                painter.setBrush(QBrush(QColor(128, 128, 196, 64)));
                painter.drawRect(child_rect);
            }
        }
    }

    void scene_tree::mousePressEvent(QMouseEvent* ev)
    {
        _refresh_timer->stop();
        QTreeWidget::mousePressEvent(ev);
    }

    void scene_tree::mouseReleaseEvent(QMouseEvent* ev)
    {
        _refresh_timer->start();
        QTreeWidget::mouseReleaseEvent(ev);
    }

    QTreeWidgetItem* scene_tree::get_tree_item_for_node(engine::scene_node* node)
    {
        const std::vector<engine::scene_node*> branch = node->get_node_branch();
        if (branch.empty())
        {
            return nullptr;
        }

        QTreeWidgetItem* result = findItems(QString::fromStdString(branch[0]->name()), Qt::MatchFlag::MatchExactly)[0];
        CRITICAL_CHECK_NOTNULL(result);
        for (size_t i = 1; i < branch.size(); ++i)
        {
            const engine::scene_node* current_node = branch[i];
            for (int child_index = 0; child_index < result->childCount(); ++child_index)
            {
                QTreeWidgetItem* child = result->child(child_index);
                if ((child != nullptr) && child->text(0).toStdString() == current_node->name())
                {
                    result = child;
                    break;
                }
            }
        }

        return result;
    }

    std::shared_ptr<engine::scene_node> scene_tree::get_node_for_tree_item(QTreeWidgetItem* item)
    {
        std::stack<std::string> selection_stack;
        QTreeWidgetItem* current_item = item;
        while (current_item != nullptr)
        {
            selection_stack.push(current_item->text(0).toStdString());
            current_item = current_item->parent();
        }

        auto node = _scene->get_node(selection_stack.top());
        selection_stack.pop();

        while (!selection_stack.empty())
        {
            node = node->get_child(selection_stack.top());
            selection_stack.pop();
        }

        return node;
    }

    void scene_tree::handle_custom_context_menu_request(const QPoint& pos)
    {
        const auto route = get_node_route_at_position(pos);

        QMenu menu(this);
        auto* add_node_action = menu.addAction(route.empty() ? "Add root node" : "Add child node");
        connect(add_node_action, &QAction::triggered, this, [this, &route] { handle_add_node(route); });

        if (!route.empty())
        {
            CRITICAL_CHECK(_scene->contains_node(route[0]), "Invalid node route");
            std::shared_ptr<engine::scene_node> node = _scene->get_node(route[0]);
            for (const auto& route_segment : route | std::views::drop(1))
            {
                node = node->get_child(route_segment);
            }

            _selected_node = node;
            emit node_selected(node.get());
            update_tree();

            auto* rename_node_action = menu.addAction("Rename");
            connect(rename_node_action, &QAction::triggered, this, [this, &route] { handle_rename_node(route); });

            auto* remove_node_action = menu.addAction("Remove");
            connect(remove_node_action, &QAction::triggered, this, [this, &route] { handle_remove_node(route); });

            auto* duplicate_node_action = menu.addAction("Duplicate");
            connect(duplicate_node_action, &QAction::triggered, this, [this, &route] { handle_duplicate_node(route); });
        }

        menu.exec(mapToGlobal(pos));
    }

    std::vector<std::string> scene_tree::get_node_route_at_position(const QPoint& pos) const
    {
        std::vector<std::string> selected_route = {};
        auto* selected_item = this->itemAt(pos);
        if (selected_item != nullptr)
        {
            selected_route.push_back(selected_item->text(0).toStdString());

            while (auto* parent_item = selected_item->parent())
            {
                selected_route.push_back(parent_item->text(0).toStdString());
                selected_item = parent_item;
            }
        }
        if (selected_route.size() > 1)
        {
            std::ranges::reverse(selected_route);
        }
        return selected_route;
    }

    void scene_tree::handle_add_node(const std::vector<std::string>& route)
    {
        auto* dialog = new add_node_dialog(this);
        if (dialog->exec() != QDialog::Accepted)
        {
            return;
        }

        const auto& [name, type] = dialog->result_value();
        if (route.empty())
        {
            if (_scene->contains_node(name))
            {
                show_error_message(QSTR("Node with name '{}' already exists", name), this);
                return;
            }

            _scene->add_root_node(name, type);
        }
        else
        {
            CRITICAL_CHECK(_scene->contains_node(route[0]), "Node not found in scene");

            auto current_node = _scene->get_node(route[0]);
            for (const auto& route_segment : route | std::views::drop(1))
            {
                current_node = current_node->get_child(route_segment);
            }

            if (current_node->contains_child(name))
            {
                show_error_message("A node with that name already exists");
                return;
            }

            current_node->add_child_node(name, type);
        }
        update_tree();
    }

    void scene_tree::handle_rename_node(const std::vector<std::string>& route)
    {
        CRITICAL_CHECK(!route.empty(), "Empty route");
        CRITICAL_CHECK(_scene->contains_node(route[0]), "Child node route does not contain parent");

        auto current_node = _scene->get_node(route[0]);
        for (const auto& route_segment : route | std::views::drop(1))
        {
            CRITICAL_CHECK(current_node->contains_child(route_segment), "Unable to find child in route");
            current_node = current_node->get_child(route_segment);
        }

        auto* input_dialog = new text_input_dialog(this, "Rename node", "Name: ", false, QSTR(current_node->name()));
        if (input_dialog->exec() != QDialog::Accepted)
        {
            return;
        }

        const auto& new_name = input_dialog->result();
        current_node->set_name(new_name.toStdString());

        auto* tree_item = _node_to_item.at(current_node.get());
        tree_item->setText(0, QSTR(current_node->name()));

        update_tree();
    }

    void scene_tree::handle_remove_node(const std::vector<std::string>& route)
    {
        CRITICAL_CHECK(!route.empty(), "Empty route");
        CRITICAL_CHECK(_scene->contains_node(route[0]), "Child node route does not contain parent");

        if (route.size() == 1) // root node
        {
            auto* node_ptr = _scene->get_node(route[0]).get();

            auto* item = _node_to_item.at(node_ptr);

            _node_to_item.erase(node_ptr);
            _item_to_node.erase(item);

            delete item;

            _scene->remove_node(route[0]);
        }
        else
        {
            auto current_node = _scene->get_node(route[0]);
            for (const auto& route_segment : route | std::views::drop(1))
            {
                CRITICAL_CHECK(current_node->contains_child(route_segment), "Unable to find child in route");
                current_node = current_node->get_child(route_segment);
            }

            auto* node_ptr = current_node.get();
            current_node->parent()->remove_child(route.back());

            auto* item = _node_to_item.at(node_ptr);

            _node_to_item.erase(node_ptr);
            _item_to_node.erase(item);

            delete item;
        }

        update_tree();
    }

    void scene_tree::handle_duplicate_node(const std::vector<std::string>& route)
    {
        CRITICAL_CHECK(!route.empty(), "Empty route");
        CRITICAL_CHECK(_scene->contains_node(route[0]), "Child node route does not contain parent");

        auto current_node = _scene->get_node(route[0]);
        for (const auto& route_segment : route | std::views::drop(1))
        {
            CRITICAL_CHECK(current_node->contains_child(route_segment), "Unable to find child in route");
            current_node = current_node->get_child(route_segment);
        }

        auto* input_dialog = new text_input_dialog(this, "New node name:", "Name: ", false, QSTR(current_node->name()));
        if (input_dialog->exec() != QDialog::Accepted)
        {
            return;
        }

        const auto copy_name = input_dialog->result().toStdString();

        auto copy = current_node->copy(copy_name, true);
        if (!current_node->has_parent())
        {
            if (_scene->contains_node(copy_name))
            {
                show_error_message(std::format("Root node with name '{}' already exists", copy_name));
                return;
            }
            _scene->add_root_node(copy);
        }
        else
        {
            if (current_node->contains_child(copy_name))
            {
                show_error_message(std::format("Child node with name '{}' already exists", copy_name));
                return;
            }

            current_node->parent()->add_child_node(copy);
        }

        _selected_node = copy;
        emit node_selected(copy.get());

        update_tree();
    }
} // namespace cathedral::editor