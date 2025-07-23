#include <cathedral/editor/common/atlas_viewer.hpp>

#include <cathedral/editor/texture_utils.hpp>
#include <cathedral/engine/atlas.hpp>
#include <cathedral/project/assets/texture_asset.hpp>
#include <cathedral/project/project.hpp>

#include <QGraphicsPixmapItem>

namespace cathedral::editor
{
    atlas_viewer::atlas_viewer(project::project& pro, const std::string& atlas_name, QWidget* parent)
        : QGraphicsView(parent)
        , _project(pro)
        , _atlas_name(atlas_name)
    {
        init_scene();
    }

    void atlas_viewer::init_scene()
    {
        _scene = new QGraphicsScene(this);

        const auto atlas_asset = _project.atlas_assets().at(_atlas_name);
        if (atlas_asset->texture_ref().has_value())
        {
            const auto texture_asset = _project.texture_assets().at(*atlas_asset->texture_ref());
            const auto mip = texture_asset->load_single_mip(0);
            const QPixmap pixmap = QPixmap::fromImage(
                mip_to_qimage(mip, texture_asset->width(), texture_asset->height(), texture_asset->format()));

            auto* pixmap_item = _scene->addPixmap(pixmap);
            pixmap_item->setTransformationMode(Qt::TransformationMode::SmoothTransformation);
        }
    }
} // namespace cathedral::editor