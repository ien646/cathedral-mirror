#include <cathedral/project/assets/atlas_asset.hpp>

namespace cathedral::project
{
    void atlas_asset::set_atlas(engine::atlas atlas)
    {
        _atlas = std::move(atlas);
    }

    const engine::atlas& atlas_asset::atlas() const
    {
        return _atlas;
    }
} // namespace cathedral::project