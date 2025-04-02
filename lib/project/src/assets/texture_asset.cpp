#include <cathedral/project/assets/texture_asset.hpp>

#include <cathedral/project/project.hpp>

#include <cathedral/compression.hpp>

#include <ien/base64.hpp>
#include <ien/fs_utils.hpp>
#include <ien/io_utils.hpp>
#include <ien/serialization.hpp>

#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

namespace cathedral::project
{
    CATHEDRAL_ASSET_SUBCLASS_IMPL(texture_asset);

    std::vector<std::vector<std::byte>> texture_asset::load_mips() const
    {
        CRITICAL_CHECK(std::filesystem::exists(binpath()), "Texture asset binpath not found");
        const std::optional<std::vector<std::byte>> compressed_mips_data = ien::read_file_binary(binpath());
        CRITICAL_CHECK(compressed_mips_data.has_value(), "Unable to read texture mips data from file");

        ien::deserializer deserializer(std::span{ *compressed_mips_data });
        const auto compressed_mips = deserializer.deserialize<std::vector<std::vector<std::byte>>>();

        std::vector<size_t> mip_uncompressed_sizes;
        mip_uncompressed_sizes.reserve(_mip_dimensions.size());
        for (const auto& mip_dim : _mip_dimensions)
        {
            mip_uncompressed_sizes.push_back(engine::calc_texture_size(mip_dim.x, mip_dim.y, _format));
        }

        std::vector<std::vector<std::byte>> uncompressed_mips;
        uncompressed_mips.reserve(mip_uncompressed_sizes.size());
        for (size_t i = 0; i < mip_uncompressed_sizes.size(); ++i)
        {
            uncompressed_mips.push_back(
                decompress_data({ compressed_mips[i].data(), compressed_mips[i].size() }, mip_uncompressed_sizes[i]));
        }

        return uncompressed_mips;
    }

    std::vector<std::byte> texture_asset::load_single_mip(uint32_t mip_index) const
    {
        CRITICAL_CHECK(std::filesystem::exists(binpath()), "Texture asset binpath not found");
        const std::optional<std::vector<std::byte>> compressed_mips_data = ien::read_file_binary(binpath());
        CRITICAL_CHECK(compressed_mips_data.has_value(), "Unable to read texture mips data from file");

        ien::deserializer deserializer(std::span{ *compressed_mips_data });
        const auto mip_count = deserializer.deserialize<IEN_SERIALIZE_CONTAINER_SIZE_T>();
        CRITICAL_CHECK(_mip_dimensions.size() == mip_count, "Deserialization failure: mip count mismatch");

        for (size_t i = 0; i < mip_index; ++i)
        {
            const auto mip_length = deserializer.deserialize<IEN_SERIALIZE_CONTAINER_SIZE_T>();
            deserializer.advance(mip_length);
        }

        const auto compressed_mip = deserializer.deserialize<std::vector<std::byte>>();
        const auto& mip_dim = _mip_dimensions[mip_index];
        const auto uncompressed_size = engine::calc_texture_size(mip_dim.x, mip_dim.y, _format);
        return decompress_data(compressed_mip, uncompressed_size);
    }

    void texture_asset::save_mips(const std::vector<std::vector<std::byte>>& mips) const
    {
        std::vector<std::vector<std::byte>> compressed_mips;
        compressed_mips.reserve(_mip_dimensions.size());
        for (size_t i = 0; i < _mip_dimensions.size(); ++i)
        {
            compressed_mips.push_back(compress_data(mips[i]));
        }

        ien::serializer serializer;
        serializer.serialize(compressed_mips);
        write_asset_binary(serializer.data());
    }

    size_t texture_asset::texture_size_bytes() const
    {
        return ien::get_file_size(binpath());
    }

    uint32_t texture_asset::get_closest_sized_mip_index(
        uint32_t width,
        uint32_t height,
        const std::vector<glm::uvec2>& mip_sizes)
    {
        if (mip_sizes.size() == 1)
        {
            return 0;
        }

        size_t i = 0;
        for (auto it = mip_sizes.rbegin(); it < mip_sizes.rend(); ++it)
        {
            if (it->x >= width && it->y >= height)
            {
                return static_cast<uint32_t>(mip_sizes.size() - 1 - i);
            }
            ++i;
        }
        return 0;
    }
} // namespace cathedral::project