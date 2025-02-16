#include <cathedral/project/assets/material_asset.hpp>

#include <cathedral/cereal_serializers.hpp>
#include <cathedral/project/project.hpp>

#include <ien/io_utils.hpp>

#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>

#include <magic_enum.hpp>

#include <fstream>

namespace cathedral::project
{
    using AssetType = material_asset;

    CATHEDRAL_ASSET_SUBCLASS_IMPL(AssetType);
} // namespace cathedral::project