#include <cathedral/project/assets/mesh_asset.hpp>

#include <cathedral/binary_serializers.hpp>
#include <cathedral/project/project.hpp>

#include <ien/io_utils.hpp>
#include <ien/serialization.hpp>

#include <nlohmann/json.hpp>

namespace cathedral::project
{
    void mesh_asset::save() const
    {
        nlohmann::json json;
        json["uncompressed_size"] = _uncompressed_data_size;

        write_asset_json(json);
    }

    void mesh_asset::load()
    {
        const auto& json = get_asset_json();
        _uncompressed_data_size = json["uncompressed_size"].get<uint32_t>();

        _is_loaded = true;
    }

    void mesh_asset::unload()
    {
        _uncompressed_data_size = 0;
        _is_loaded = false;
    }

    std::string mesh_asset::relative_path() const
    {
        return _path.substr(_project.meshes_path().size() + 1);
    }

    void mesh_asset::save_mesh(const engine::mesh& mesh)
    {
        ien::serializer serializer;
        serializer.serialize(mesh.positions());
        serializer.serialize(mesh.uvcoords());
        serializer.serialize(mesh.normals());
        serializer.serialize(mesh.colors());
        serializer.serialize(mesh.indices());

        write_asset_binary(serializer.data());

        _uncompressed_data_size = static_cast<uint32_t>(serializer.data().size());
    }

    [[nodiscard]] engine::mesh mesh_asset::load_mesh() const
    {
        const auto opt_data = ien::read_file_binary(get_binpath());
        CRITICAL_CHECK(opt_data.has_value());

        ien::deserializer deserializer(std::span{*opt_data});
        auto positions = deserializer.deserialize<std::vector<glm::vec3>>();
        auto uvcoords = deserializer.deserialize<std::vector<glm::vec2>>();
        auto normals = deserializer.deserialize<std::vector<glm::vec3>>();
        auto colors = deserializer.deserialize<std::vector<glm::vec4>>();
        auto indices = deserializer.deserialize<std::vector<uint32_t>>();

        return { std::move(positions), std::move(uvcoords), std::move(normals), std::move(colors), std::move(indices) };
    }
} // namespace cathedral::project