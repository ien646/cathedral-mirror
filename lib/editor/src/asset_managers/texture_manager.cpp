#include <cathedral/editor/asset_managers/texture_manager.hpp>

#include <cathedral/editor/asset_managers/dialogs/new_texture_dialog.hpp>
#include <cathedral/editor/common/message.hpp>

#include <cathedral/engine/texture_mip.hpp>

#include <cathedral/project/assets/texture_asset.hpp>
#include <cathedral/project/project.hpp>

#include <ien/initializers.hpp>

#include <magic_enum.hpp>

#include "ui_texture_manager.h"

namespace cathedral::editor
{
    texture_manager::texture_manager(project::project& pro, QWidget* parent)
        : QMainWindow(parent)
        , _ui(new Ui::texture_manager)
        , _project(pro)
    {
        _ui->setupUi(this);

        connect(_ui->actionClose, &QAction::triggered, this, &texture_manager::close);

        connect(_ui->itemManagerWidget, &item_manager::add_clicked, this, [this] {
            auto* diag = new new_texture_dialog(this);
            if (diag->exec() != QDialog::DialogCode::Accepted)
            {
                return;
            }

            const auto format = magic_enum::enum_cast<engine::texture_format>(diag->format().toStdString());
            if (!format)
            {
                show_error_message("Invalid format", this);
                return;
            }

            const auto mip_levels = diag->mips();
            const auto mipgen_filter = magic_enum::enum_cast<ien::resize_filter>(diag->mipgen_filter().toStdString());
            if(!mipgen_filter)
            {
                show_error_message("Invalid mipmap generation filter", this);
                return;
            }

            const ien::image_format request_image_format = [format = *format] {
                using namespace cathedral::engine;
                using namespace ien;
                switch (format)
                {
                case texture_format::R8_LINEAR:
                case texture_format::R8_SRGB:
                    return image_format::R;
                case texture_format::R8G8_LINEAR:
                case texture_format::R8G8_SRGB:
                    return image_format::RG;
                case texture_format::R8G8B8_LINEAR:
                case texture_format::R8G8B8_SRGB:
                    return image_format::RGB;
                case texture_format::DXT1_BC1_SRGB:
                case texture_format::DXT5_BC3_SRGB:
                case texture_format::DXT1_BC1_LINEAR:
                case texture_format::DXT5_BC3_LINEAR:
                case texture_format::R8G8B8A8_SRGB:
                case texture_format::R8G8B8A8_LINEAR:
                    return image_format::RGBA;
                }
                CRITICAL_ERROR("Unhandled texture format");
            }();

            ien::image source_image(diag->image_path().toStdString(), request_image_format);

            std::vector<std::vector<uint8_t>> mips;

            const auto mip0_data = ien::conditional_init<std::vector<uint8_t>>(
                is_compressed_format(*format),
                [&image = source_image, format = *format] {
                    return create_compressed_texture_data(image, engine::get_format_compression_type(format));
                },
                [&image = source_image] {
                    std::vector<uint8_t> result(image.size());
                    std::memcpy(result.data(), image.data(), image.size());
                    return result;
                });
            mips.push_back(std::move(mip0_data));

            if (mip_levels > 1)
            {
                for (const auto& mip : engine::create_image_mips(source_image, *mipgen_filter, mip_levels - 1))
                {
                    if (is_compressed_format(*format))
                    {
                        mips.push_back(create_compressed_texture_data(mip, engine::get_format_compression_type(*format)));
                    }
                    else
                    {
                        auto& vec = mips.emplace_back();
                        vec.resize(mip.size());
                        std::memcpy(vec.data(), mip.data(), vec.size());
                    }
                }
            }

            const auto full_path = _project.textures_path() + "/" + diag->name().toStdString();

            project::texture_asset new_asset(project::texture_asset(_project, full_path));
            new_asset.set_width(source_image.width());
            new_asset.set_height(source_image.height());
            new_asset.set_format(*format);
            new_asset.set_mips(std::move(mips));
            new_asset.mark_as_manually_loaded();
            new_asset.save();
        });
    }
} // namespace cathedral::editor