#include <cathedral/project/assets/texture_asset.hpp>

#include <cathedral/project/project.hpp>

#include <cathedral/compression.hpp>

#include <ien/base64.hpp>
#include <ien/io_utils.hpp>
#include <ien/serialization.hpp>

#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

namespace cathedral::project
{
    void texture_asset::save() const
    {
        std::filesystem::path fspath(_path);

        nlohmann::json json;
        json["asset"] = asset_typestr<texture_asset>();
        json["width"] = _width;
        json["height"] = _height;
        json["format"] = magic_enum::enum_name(_format);

        std::vector<std::vector<uint8_t>> compressed_mips;

        nlohmann::json mips_info = nlohmann::json::array();
        for (size_t i = 0; i < _mips.size(); ++i)
        {
            auto compressed_data = compress_data(_mips[i].data(), _mips[i].size());
            mips_info[i]["uncompressed_size"] = _mips[i].size();
            compressed_mips.push_back(std::move(compressed_data));
        }
        json["mips_info"] = mips_info;

        auto binpath = fspath;
        binpath.replace_extension(".lz4");
        ien::serializer serializer;
        serializer.serialize(compressed_mips);
        const auto mips_data = serializer.release_data();
        const bool write_mips_ok = ien::write_file_binary(binpath.string(), mips_data);

        std::filesystem::create_directories(fspath.parent_path().string());

        const bool write_json_ok = ien::write_file_text(_path, json.dump(2));
        CRITICAL_CHECK(write_json_ok);
    }

    void texture_asset::load()
    {
        const std::filesystem::path fspath(_path);

        const auto& json = get_asset_json();
        CRITICAL_CHECK(json.contains("asset") && json["asset"].get<std::string>() == asset_typestr<texture_asset>());

        _width = json["width"].get<uint32_t>();
        _height = json["height"].get<uint32_t>();
        const auto format = magic_enum::enum_cast<engine::texture_format>(json["width"].get<std::string>());
        CRITICAL_CHECK(format.has_value());
        _format = *format;

        auto binpath = fspath;
        binpath.replace_extension(".lz4");

        const std::optional<std::vector<uint8_t>> compressed_mips_data = ien::read_file_binary(binpath.string());
        CRITICAL_CHECK(compressed_mips_data);

        ien::deserializer deserializer(compressed_mips_data->data(), compressed_mips_data->size());
        const auto compressed_mips = deserializer.deserialize<std::vector<std::vector<uint8_t>>>();

        std::vector<size_t> mip_uncompressed_sizes;
        for (const auto& [key, value] : json["mips_info"].items())
        {
            mip_uncompressed_sizes.push_back(value["uncompressed_size"]);
        }

        CRITICAL_CHECK(compressed_mips.size() == mip_uncompressed_sizes.size());
        std::vector<std::vector<uint8_t>> uncompressed_mips;
        for (size_t i = 0; i < mip_uncompressed_sizes.size(); ++i)
        {
            uncompressed_mips.push_back(
                decompress_data(compressed_mips[i].data(), compressed_mips.size(), mip_uncompressed_sizes[i]));
        }

        _mips = std::move(uncompressed_mips);

        _is_loaded = true;
    }

    void texture_asset::unload()
    {
        _width = 0;
        _height = 0;
        _mips.clear();
        _is_loaded = false;
    }

    std::string texture_asset::relative_path() const
    {
        return _path.substr(_project.textures_path().size() + 1);
    }
} // namespace cathedral::project