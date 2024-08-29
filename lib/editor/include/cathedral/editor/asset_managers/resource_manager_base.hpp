#pragma once

#include <cathedral/project/asset.hpp>
#include <cathedral/project/project.hpp>

#include <cathedral/editor/common/message.hpp>
#include <cathedral/editor/common/text_input_dialog.hpp>

#include <QDialog>

#include <ien/str_utils.hpp>

#include <filesystem>

namespace cathedral::editor
{
    class item_manager;

    template <project::AssetLike TAsset>
    class resource_manager_base
    {
    public:
        resource_manager_base(project::project& pro)
            : _project(pro)
        {
        }

    protected:
        project::project& _project;

        void reload()
        {
            auto* item_manager_widget = get_item_manager_widget();
            item_manager_widget->clear_items();

            for (const auto& [path, asset] : _project.get_assets<TAsset>())
            {
                const auto relative_path = ien::str_trim(ien::str_split(path, _project.get_assets_path<TAsset>())[0], '/');
                const auto name = std::filesystem::path(relative_path).replace_extension().string();

                item_manager_widget->add_item(QString::fromStdString(name));
            }

            item_manager_widget->sort_items(Qt::SortOrder::AscendingOrder);
        }

        std::unordered_map<std::string, std::shared_ptr<TAsset>> get_assets() { return _project.get_assets<TAsset>(); }

        const std::string& get_assets_path() const { return _project.get_assets_path<TAsset>(); }

        void rename_asset()
        {
            auto* item_manager_widget = get_item_manager_widget();
            if (!item_manager_widget->current_text())
            {
                return;
            }

            const auto selected_path = *item_manager_widget->current_text();
            const auto old_path =
                (std::filesystem::path(get_assets_path()) / selected_path.toStdString()).string() + ".casset";

            auto* input =
                new text_input_dialog(item_manager_widget->parentWidget(), "Rename", "New name", false, selected_path);
            input->exec();

            QString result = input->result();
            if (result.isEmpty())
            {
                return;
            }

            const auto new_path = (std::filesystem::path(get_assets_path()) / result.toStdString()).string() + ".casset";

            auto asset = _project.get_asset_by_path<TAsset>(old_path);
            CRITICAL_CHECK(asset);

            asset->move_path(new_path);

            _project.reload_assets<TAsset>();
            reload();
        }

        void delete_asset()
        {
            auto* item_manager_widget = get_item_manager_widget();
            if (!item_manager_widget->current_item())
            {
                return;
            }

            const auto selected_path = *item_manager_widget->current_text();

            const bool confirm = show_confirm_dialog("Delete '" + selected_path + "'?");
            if (confirm)
            {
                const auto full_path =
                    (std::filesystem::path(get_assets_path()) / selected_path.toStdString()).string() + ".casset";
                std::filesystem::remove(full_path);

                _project.reload_assets<TAsset>();
                reload();
            }
        }

        virtual item_manager* get_item_manager_widget() = 0;
    };
} // namespace cathedral::editor