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

            connect(widget, &texture_slot_widget::clicked, this, [this, widget, path] {
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

            const auto name =
                asset->relative_path().ends_with(".casset")
                    ? asset->relative_path().substr(0, asset->relative_path().size() - sizeof(".casset") + 1)
                    : asset->relative_path();
            widget->set_name(QString::fromStdString(name));

            QtConcurrent::run([asset] -> std::vector<std::vector<uint8_t>> {
                return asset->load_mips();
            }).then([asset, widget](std::vector<std::vector<uint8_t>> mips) {
                const auto closest_mip_index = project::texture_asset::
                    get_closest_sized_mip_index(widget->width(), widget->height(), asset->mip_sizes());
                const auto [mip_w, mip_h] = asset->mip_sizes()[closest_mip_index];
                const std::vector<uint8_t> closest_mip = [&] -> std::vector<uint8_t> {
                    if (engine::is_compressed_format(asset->format()))
                    {
                        return engine::decompress_texture_data(
                            mips[closest_mip_index].data(),
                            mips[closest_mip_index].size(),
                            mip_w,
                            mip_h,
                            engine::get_format_compression_type(asset->format()));
                    }
                    else
                    {
                        return mips[closest_mip_index];
                    }
                }();

                const auto qrgba_data = image_data_to_qrgba(closest_mip, asset->format());

                QImage img(qrgba_data.data(), mip_w, mip_h, QImage::Format::Format_RGBA8888);
                widget->set_image(img);
            });
        }
    }
} // namespace cathedral::editor