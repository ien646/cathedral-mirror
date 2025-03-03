#include <cathedral/editor/scene_tree.hpp>

#include <cathedral/editor/common/message.hpp>
#include <cathedral/editor/common/text_input_dialog.hpp>

#include <cathedral/engine/nodes/node.hpp>

#include <QMenu>
#include <QPaintEvent>
#include <QPainter>

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

        connect(this, &QTreeWidget::customContextMenuRequested, this, [this](const QPoint& pos) {
            QMenu menu(this);

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

            auto* add_node_action = menu.addAction(selected_route.empty() ? "Add root node" : "Add child node");
            connect(add_node_action, &QAction::triggered, this, [this, &selected_route] {
                auto* name_dialog = new text_input_dialog(this, "New node", "Name: ", false);
                if (name_dialog->exec() != QDialog::Accepted)
                {
                    return;
                }

                const auto name = name_dialog->result();
                if (selected_route.empty())
                {
                    if (_scene->root_nodes().contains(name.toStdString()))
                    {
                        show_error_message(
                            QString::fromStdString(std::format("Node with name '{}' already exists", name.toStdString())),
                            this);
                        return;
                    }

                    _scene->add_root_node<engine::node>(name.toStdString());
                }
                else
                {
                    CRITICAL_CHECK(_scene->root_nodes().contains(selected_route[0]));

                    auto current_node = _scene->root_nodes().at(selected_route[0]);
                    for (const auto& route_segment : selected_route | std::views::drop(1))
                    {
                        auto find_it = std::ranges::find_if(
                            current_node->children(),
                            [&route_segment](const std::shared_ptr<engine::scene_node>& child) {
                                return child->name() == route_segment;
                            });

                        CRITICAL_CHECK(find_it != current_node->children().end());
                        current_node = *find_it;
                    }

                    auto dupe_it = std::ranges::find_if(
                        current_node->children(),
                        [&name](const std::shared_ptr<engine::scene_node>& child) { return child->name() == name; });
                    
                    if(dupe_it != current_node->children().end())
                    {
                        show_error_message("A node with that name already exists");
                        return;
                    }

                    current_node->add_child_node<engine::node>(name.toStdString());
                }
                update_tree();
            });

            menu.exec(mapToGlobal(pos));
        });

        connect(_refresh_timer, &QTimer::timeout, this, [this] { update_tree(); });

        update_tree();
    }

    void scene_tree::set_scene(engine::scene* scene)
    {
        _scene = scene;
        update_tree();
    }

    void scene_tree::update_tree()
    {
        if (_scene != nullptr)
        {
            for (const auto& [name, node] : _scene->root_nodes())
            {
                process_node(nullptr, *node, name);
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
        else
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
        CRITICAL_CHECK(result != nullptr);
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
} // namespace cathedral::editor