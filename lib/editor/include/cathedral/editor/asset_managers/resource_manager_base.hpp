#pragma once

#include <cathedral/core.hpp>

#include <cathedral/project/asset.hpp>
#include <cathedral/project/project.hpp>

#include <cathedral/editor/common/item_manager.hpp>
#include <cathedral/editor/common/message.hpp>
#include <cathedral/editor/common/text_input_dialog.hpp>

#include <QDialog>

#include <ien/str_utils.hpp>

#include <filesystem>

namespace cathedral::editor
{
    struct rename_result
    {
        std::string before;
        std::string after;
    };

    template <project::concepts::Asset TAsset>
    class resource_manager_base
    {
    public:
        explicit resource_manager_base(
            project::project* pro,
            std::function<std::optional<QPixmap>(const TAsset&)> icon_filter = {})
            : _project(pro)
            , _icon_filter(std::move(icon_filter))
        {
        }

        virtual ~resource_manager_base() = default;

    protected:
        project::project* _project;
        std::function<std::optional<QPixmap>(const TAsset&)> _icon_filter;

        void reload_item_list()
        {
            auto* item_manager_widget = get_item_manager_widget();
            item_manager_widget->clear_items();

            for (const auto& [name, asset] : _project->get_assets<TAsset>())
            {
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
            return _project->get_assets<TAsset>();
        }

        const std::string& get_assets_path() const { return _project->get_assets_path<TAsset>(); }

        [[nodiscard]] std::optional<rename_result> rename_asset()
        {
            auto* item_manager_widget = get_item_manager_widget();
            if (item_manager_widget->current_text().isEmpty())
            {
                return {};
            }

            const auto selected_path = item_manager_widget->current_text();
            const auto old_path = _project->name_to_abspath<TAsset>(selected_path.toStdString());

            text_input_dialog input (item_manager_widget->parentWidget(), "Rename", "New name", false, selected_path);
            input.exec();

            const QString& new_name = input.result();
            if (new_name.isEmpty())
            {
                return std::nullopt;
            }

            const auto name = new_name.toStdString();
            const auto new_path = _project->name_to_abspath<TAsset>(name);

            auto asset = _project->get_asset_by_path<TAsset>(old_path);
            const auto before_name = asset->name();

            CRITICAL_CHECK(asset, "Asset not found");

            asset->move_path(new_path);

            _project->reload_assets<TAsset>();
            reload_item_list();

            const bool select_ok = item_manager_widget->select_item(new_name);
            CRITICAL_CHECK(select_ok, "Unable to select renamed asset");

            return rename_result{ .before = before_name, .after = new_name.toStdString() };
        }

        void delete_asset()
        {
            auto* item_manager_widget = get_item_manager_widget();
            if (!item_manager_widget->current_item())
            {
                return;
            }

            const auto& selected_path = item_manager_widget->current_text();
            if (show_confirm_dialog("Delete '" + selected_path + "'?"))
            {
                const auto asset = _project->get_assets<TAsset>().at(selected_path.toStdString());
                std::filesystem::remove(asset->absolute_path());

                const auto& binpath = asset->binpath();
                if (std::filesystem::exists(binpath))
                {
                    std::filesystem::remove(binpath);
                }

                _project->reload_assets<TAsset>();
                reload_item_list();
            }
        }

        bool is_asset_selected() const { return get_item_manager_widget()->current_item() != nullptr; }

        std::shared_ptr<TAsset> get_current_asset() const
        {
            if (!is_asset_selected())
            {
                return nullptr;
            }
            const auto& name = get_item_manager_widget()->current_text();
            return _project->get_asset_by_name<TAsset>(name.toStdString());
        };

        virtual item_manager* get_item_manager_widget() = 0;
        virtual const item_manager* get_item_manager_widget() const = 0;
    };
} // namespace cathedral::editor