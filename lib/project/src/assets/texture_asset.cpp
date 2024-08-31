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
    void texture_asset::save() const
    {
        CRITICAL_CHECK(_width > 0);
        CRITICAL_CHECK(_height > 0);
        CRITICAL_CHECK(!_mip_sizes.empty());

        std::filesystem::path fspath(_path);

        nlohmann::json json;
        json["asset"] = asset_typestr<SELF>();
        json["width"] = _width;
        json["height"] = _height;
        json["format"] = magic_enum::enum_name(_format);

        nlohmann::json mips_info = nlohmann::json::array();
        for (size_t i = 0; i < _mip_sizes.size(); ++i)
        {
            const auto [mip_w, mip_h] = _mip_sizes[i];

            mips_info[i]["uncompressed_size"] = calc_texture_size(mip_w, mip_h, _format);
            mips_info[i]["width"] = mip_w;
            mips_info[i]["height"] = mip_h;
        }
        json["mips_info"] = mips_info;

        write_asset_json(json);
    }

    void texture_asset::load()
    {
        unload();

        const std::filesystem::path fspath(_path);

        const auto& json = get_asset_json();
        CRITICAL_CHECK(json.contains("asset") && json["asset"].get<std::string>() == asset_typestr<SELF>());

        CRITICAL_CHECK(json.contains("width") && json.contains("height"));
        _width = json["width"].get<uint32_t>();
        _height = json["height"].get<uint32_t>();

        CRITICAL_CHECK(json.contains("format"));
        const auto format_opt = magic_enum::enum_cast<engine::texture_format>(json["format"].get<std::string>());

        CRITICAL_CHECK(format_opt.has_value());
        _format = *format_opt;

        for (const auto& mip_info : json["mips_info"])
        {
            const uint32_t width = mip_info["width"].get<uint32_t>();
            const uint32_t height = mip_info["height"].get<uint32_t>();
            _mip_sizes.emplace_back(width, height);
        }

        _is_loaded = true;
    }

    void texture_asset::unload()
    {
        _width = 0;
        _height = 0;
        _mip_sizes.clear();
        _is_loaded = false;
    }

    std::string texture_asset::relative_path() const
    {
        return _path.substr(_project.textures_path().size() + 1);
    }

    std::vector<std::vector<uint8_t>> texture_asset::load_mips() const
    {
        const std::filesystem::path fspath(_path);

        const auto& json = get_asset_json();
        CRITICAL_CHECK(json.contains("asset") && json["asset"].get<std::string>() == asset_typestr<SELF>());

        CRITICAL_CHECK(std::filesystem::exists(get_binpath()));
        const std::optional<std::vector<uint8_t>> compressed_mips_data = ien::read_file_binary(get_binpath());
        CRITICAL_CHECK(compressed_mips_data.has_value());

        ien::deserializer deserializer(compressed_mips_data->data(), compressed_mips_data->size());
        const auto compressed_mips = deserializer.deserialize<std::vector<std::vector<uint8_t>>>();

        std::vector<size_t> mip_uncompressed_sizes;
        for (const auto& [key, value] : json["mips_info"].items())
        {
            mip_uncompressed_sizes.push_back(value["uncompressed_size"].get<uint32_t>());
        }

        std::vector<std::vector<uint8_t>> uncompressed_mips;
        for (size_t i = 0; i < mip_uncompressed_sizes.size(); ++i)
        {
            uncompressed_mips.push_back(
                decompress_data(compressed_mips[i].data(), compressed_mips[i].size(), mip_uncompressed_sizes[i]));
        }

        return uncompressed_mips;
    }

    std::vector<uint8_t> texture_asset::load_single_mip(uint32_t mip_index) const
    {
        const std::filesystem::path fspath(_path);

        const auto& json = get_asset_json();
        CRITICAL_CHECK(json.contains("asset") && json["asset"].get<std::string>() == asset_typestr<SELF>());

        CRITICAL_CHECK(std::filesystem::exists(get_binpath()));
        const std::optional<std::vector<uint8_t>> compressed_mips_data = ien::read_file_binary(get_binpath());
        CRITICAL_CHECK(compressed_mips_data.has_value());

        ien::deserializer deserializer(compressed_mips_data->data(), compressed_mips_data->size());
        const auto mip_count = deserializer.deserialize<IEN_SERIALIZE_CONTAINER_SIZE_T>();
        CRITICAL_CHECK(_mip_sizes.size() == mip_count);

        for (size_t i = 0; i < mip_index; ++i)
        {
            const auto mip_length = deserializer.deserialize<IEN_SERIALIZE_CONTAINER_SIZE_T>();
            deserializer.advance(mip_length);
        }

        const auto compressed_mip = deserializer.deserialize<std::vector<uint8_t>>();
        const auto [mip_w, mip_h] = _mip_sizes[mip_index];
        const auto uncompressed_size = engine::calc_texture_size(mip_w, mip_h, _format);
        return decompress_data(compressed_mip.data(), compressed_mip.size(), uncompressed_size);
    }

    void texture_asset::save_mips(const std::vector<std::vector<uint8_t>>& mips) const
    {
        const std::filesystem::path fspath(_path);

        std::vector<std::vector<uint8_t>> compressed_mips;
        for (size_t i = 0; i < _mip_sizes.size(); ++i)
        {
            compressed_mips.push_back(compress_data(mips[i].data(), mips[i].size()));
        }

        ien::serializer serializer;
        serializer.serialize(compressed_mips);
        write_asset_binary(serializer.data());
    }

    size_t texture_asset::texture_size_bytes() const
    {
        return ien::get_file_size(get_binpath());
    }

    uint32_t texture_asset::get_closest_sized_mip_index(
        uint32_t width,
        uint32_t height,
        const std::vector<std::pair<uint32_t, uint32_t>>& mip_sizes)
    {
        if (mip_sizes.size() == 1)
        {
            return 0;
        }

        size_t i = 0;
        for (auto it = mip_sizes.rbegin(); it < mip_sizes.rend(); ++it)
        {
            const auto [mipw, miph] = *it;
            if (mipw >= width && miph >= height)
            {
                return mip_sizes.size() - 1 - i;
            }
            ++i;
        }
        return 0;
    }
} // namespace cathedral::project