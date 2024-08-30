#include <cathedral/editor/common/texture_list_widget.hpp>

#include <cathedral/editor/common/message.hpp>
#include <cathedral/editor/common/texture_slot_widget.hpp>
#include <cathedral/editor/texture_utils.hpp>

#include <cathedral/engine/texture_decompression.hpp>

#include <cathedral/project/project.hpp>

#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QtConcurrent>

#include <magic_enum.hpp>

namespace cathedral::editor
{
    texture_list_widget::texture_list_widget(project::project& pro, QWidget* parent)
        : QWidget(parent)
        , _project(pro)
    {
        const auto& texture_assets = _project.get_assets<project::texture_asset>();
        if (texture_assets.empty())
        {
            show_error_message("No textures defined!");
        }

        auto* dialog_layout = new QVBoxLayout;
        setLayout(dialog_layout);

        auto* textures_list = new QListWidget;
        textures_list->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
        textures_list->setSpacing(0);
        textures_list->setContentsMargins(0, 0, 0, 0);
        dialog_layout->addWidget(textures_list);

        for (const auto& [path, asset] : texture_assets)
        {
            auto* widget = new texture_slot_widget(this);
            auto* list_item = new QListWidgetItem;
            list_item->setFlags(Qt::ItemFlag::NoItemFlags);
            list_item->setSizeHint(QSize{ 100, 100 });

            textures_list->addItem(list_item);
            textures_list->setItemWidget(list_item, widget);
            _slot_widgets.push_back(widget);

            connect(widget, &texture_slot_widget::clicked, this, [this, widget, path = path] {
                for (auto* other : _slot_widgets)
                {
                    other->unmarkSelected();
                }
                widget->markSelected();
                _selected_path = path;
                emit selection_changed();
            });

            asset->load();

            widget->set_slot_index(0);
            widget->set_dimensions(asset->width(), asset->height());
            widget->set_format(QString::fromStdString(std::string{ magic_enum::enum_name(asset->format()) }));

            const auto name = _project.relpath_to_name(asset->relative_path());
            widget->set_name(QString::fromStdString(name));

            const auto closest_mip_index =
                project::texture_asset::get_closest_sized_mip_index(widget->width(), widget->height(), asset->mip_sizes());

            QtConcurrent::run([asset = asset, closest_mip_index] -> QImage {
                const auto [mip_w, mip_h] = asset->mip_sizes()[closest_mip_index];
                const auto mip = asset->load_single_mip(closest_mip_index);
                return mip_to_qimage(mip, mip_w, mip_h, asset->format());
            }).then([widget](QImage img) { widget->set_image(img); });
        }
    }
} // namespace cathedral::editor