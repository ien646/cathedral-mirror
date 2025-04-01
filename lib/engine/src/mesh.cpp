#include <cathedral/engine/mesh.hpp>

#include <cathedral/engine/vertex_pack.hpp>

#include <cmath>
#include <happly.h>
#include <utility>

namespace cathedral::engine
{
    mesh::mesh(const std::string& path, [[maybe_unused]] size_t shape_index)
    {
        init_for_ply(path);
    }

    mesh::mesh(
        std::vector<glm::vec3> positions,
        std::vector<glm::vec2> uvcoords,
        std::vector<glm::vec3> normals,
        std::vector<glm::vec4> colors,
        std::vector<uint32_t> indices)
        : _pos(std::move(positions))
        , _uv(std::move(uvcoords))
        , _normal(std::move(normals))
        , _color(std::move(colors))
        , _indices(std::move(indices))
    {
    }

    std::vector<float> mesh::get_packed_data() const
    {
        return pack_vertex_data(_pos, _uv, _normal, _color);
    }

    size_t mesh::size_in_bytes() const
    {
        return (_pos.size() * sizeof(decltype(_pos)::value_type)) + (_uv.size() * sizeof(decltype(_uv)::value_type)) +
               (_normal.size() * sizeof(decltype(_normal)::value_type)) +
               (_color.size() * sizeof(decltype(_color)::value_type)) +
               (_indices.size() * sizeof(decltype(_indices)::value_type));
    }

    void mesh::init_for_ply(const std::string& path)
    {
        happly::PLYData ply(path);

        fill_positions(ply);
        fill_colors(ply);
        fill_uvcoords(ply);
        fill_normals(ply);
        fill_indices(ply);
    }

    void mesh::fill_positions(happly::PLYData& data)
    {
        // position
        for (const auto& pos : data.getVertexPositions())
        {
            _pos.emplace_back(pos[0], pos[1], pos[2]);
        }
    }

    void mesh::fill_normals(happly::PLYData& data)
    {
        auto& vertex = data.getElement("vertex");

        // normal
        if (vertex.hasProperty("nx"))
        {
            auto nx = vertex.getProperty<float>("nx");
            auto ny = vertex.getProperty<float>("ny");
            auto nz = vertex.getProperty<float>("nz");

            CRITICAL_CHECK(nx.size() == ny.size(), "Invalid vertex normal propsize");
            CRITICAL_CHECK(nx.size() == nz.size(), "Invalid vertex normal propsize");

            for (size_t i = 0; i < nx.size(); ++i)
            {
                _normal.emplace_back(nx[i], ny[i], nz[i]);
            }
        }
        else
        {
            for (size_t i = 0; i < _pos.size(); ++i)
            {
                _normal.emplace_back(0.0F, 0.0F, 0.0F);
            }
        }
    }

    void mesh::fill_uvcoords(happly::PLYData& data)
    {
        auto& vertex = data.getElement("vertex");

        // uv
        if (vertex.hasProperty("s"))
        {
            auto u = vertex.getProperty<float>("s");
            auto v = vertex.getProperty<float>("t");
            CRITICAL_CHECK(u.size() == v.size(), "Invalid vxcoord propsize");
            for (size_t i = 0; i < u.size(); ++i)
            {
                _uv.emplace_back(u[i], v[i]);
            }
        }
        else
        {
            for (size_t i = 0; i < _pos.size(); ++i)
            {
                _uv.emplace_back(0.0F, 0.0F);
            }
        }
    }

    void mesh::fill_colors(happly::PLYData& data)
    {
        auto& vertex = data.getElement("vertex");

        // color
        if (vertex.hasProperty("red"))
        {
            for (const auto& col : data.getVertexColors())
            {
                _color.emplace_back(col[0], col[1], col[2], 1.0F);
            }
        }
        else
        {
            for (size_t i = 0; i < _pos.size(); ++i)
            {
                _color.emplace_back(
                    std::fmod(static_cast<float>(i) / 2, 1.0F),
                    std::fmod(static_cast<float>(i) / 3, 1.0F),
                    std::fmod(static_cast<float>(i) / 5, 1.0F),
                    1.0F);
            }
        }
    }

    void mesh::fill_indices(happly::PLYData& data)
    {
        // indices
        for (const auto& indices : data.getFaceIndices())
        {
            if (indices.size() == 3)
            {
                _indices.push_back(static_cast<uint32_t>(indices[0]));
                _indices.push_back(static_cast<uint32_t>(indices[1]));
                _indices.push_back(static_cast<uint32_t>(indices[2]));
            }
            else if (indices.size() == 4)
            {
                CRITICAL_ERROR("Unhandled quad mesh face. Please triangulate.");
            }
            else
            {
                CRITICAL_ERROR("Unhandled non triangle mesh face. Please triangulate.");
            }
        }
    }
} // namespace cathedral::engine