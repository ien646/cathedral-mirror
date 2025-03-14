#include <cathedral/project/assets/mesh_asset.hpp>

#include <cathedral/binary_serializers.hpp>
#include <cathedral/project/project.hpp>

#include <ien/io_utils.hpp>
#include <ien/serialization.hpp>

namespace cathedral::project
{
    CATHEDRAL_ASSET_SUBCLASS_IMPL(mesh_asset);

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
        const auto opt_data = ien::read_file_binary(binpath());
        CRITICAL_CHECK(opt_data.has_value());

        ien::deserializer deserializer(std::span{ *opt_data });
        auto positions = deserializer.deserialize<std::vector<glm::vec3>>();
        auto uvcoords = deserializer.deserialize<std::vector<glm::vec2>>();
        auto normals = deserializer.deserialize<std::vector<glm::vec3>>();
        auto colors = deserializer.deserialize<std::vector<glm::vec4>>();
        auto indices = deserializer.deserialize<std::vector<uint32_t>>();

        return { std::move(positions), std::move(uvcoords), std::move(normals), std::move(colors), std::move(indices) };
    }
} // namespace cathedral::project