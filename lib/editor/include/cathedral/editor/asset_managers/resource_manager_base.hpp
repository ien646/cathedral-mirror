#pragma once

#include <cathedral/core.hpp>

#include <cathedral/project/asset.hpp>
#include <cathedral/project/project.hpp>

#include <cathedral/editor/common/message.hpp>
#include <cathedral/editor/common/text_input_dialog.hpp>

#include <QDialog>

#include <ien/str_utils.hpp>

#include <filesystem>

FORWARD_CLASS(cathedral::editor, item_manager);

namespace cathedral::editor
{
    template <project::AssetLike TAsset>
    class resource_manager_base
    {
    public:
        explicit resource_manager_base(
            project::project& pro,
            std::function<std::optional<QPixmap>(const TAsset&)> icon_filter = {})
            : _project(pro)
            , _icon_filter(std::move(icon_filter))
        {
        }

    protected:
        project::project& _project;
        std::function<std::optional<QPixmap>(const TAsset&)> _icon_filter;

        void reload_item_list()
        {
            auto* item_manager_widget = get_item_manager_widget();
            item_manager_widget->clear_items();

            for (const auto& [path, asset] : _project.get_assets<TAsset>())
            {
                const auto relative_path = _project.abspath_to_name(path);
                const auto name = relative_path.ends_with(".casset")
                                      ? relative_path.substr(0, relative_path.size() - sizeof(".casset"))
                                      : relative_path;

                const auto icon_pixmap = _icon_filter ? _icon_filter(*asset) : std::nullopt;

                if (icon_pixmap.has_value())
                {
                    item_manager_widget->add_item(QString::fromStdString(name), *icon_pixmap);
                }
                else
                {
                    item_manager_widget->add_item(QString::fromStdString(name));
                }
            }

            item_manager_widget->sort_items(Qt::SortOrder::AscendingOrder);
        }

        const std::unordered_map<std::string, std::shared_ptr<TAsset>>& get_assets() const
        {
            return _project.get_assets<TAsset>();
        }

        const std::string& get_assets_path() const { return _project.get_assets_path<TAsset>(); }

        void rename_asset()
        {
            auto* item_manager_widget = get_item_manager_widget();
            if (item_manager_widget->current_text().isEmpty())
            {
                return;
            }

            const auto selected_path = item_manager_widget->current_text();
            const auto old_path = _project.name_to_abspath<TAsset>(selected_path.toStdString());

            auto* input =
                new text_input_dialog(item_manager_widget->parentWidget(), "Rename", "New name", false, selected_path);
            input->exec();

            QString result = input->result();
            if (result.isEmpty())
            {
                return;
            }

            const auto name = result.toStdString();
            const auto new_path = _project.name_to_abspath<TAsset>(name);

            auto asset = _project.get_asset_by_path<TAsset>(old_path);
            CRITICAL_CHECK(asset);

            asset->move_path(new_path);

            _project.reload_assets<TAsset>();
            reload_item_list();

            std::ignore = item_manager_widget->select_item(QString::fromStdString(name));
        }

        void delete_asset()
        {
            auto* item_manager_widget = get_item_manager_widget();
            if (!item_manager_widget->current_item())
            {
                return;
            }

            const auto selected_path = item_manager_widget->current_text();

            const bool confirm = show_confirm_dialog("Delete '" + selected_path + "'?");
            if (confirm)
            {
                const auto full_path = _project.name_to_abspath<TAsset>(selected_path.toStdString());
                std::filesystem::remove(full_path);

                _project.reload_assets<TAsset>();
                reload_item_list();
            }
        }

        bool is_asset_selected() const { return get_item_manager_widget()->current_item() != nullptr; }

        std::shared_ptr<TAsset> get_current_asset() const
        {
            CRITICAL_CHECK(is_asset_selected());
            const auto& name = get_item_manager_widget()->current_text();
            return _project.get_asset_by_relative_name<TAsset>(name.toStdString());
        };

        virtual item_manager* get_item_manager_widget() = 0;
        virtual const item_manager* get_item_manager_widget() const = 0;
    };
} // namespace cathedral::editor