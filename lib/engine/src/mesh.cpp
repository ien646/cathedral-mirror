#include <cathedral/engine/mesh.hpp>

#include <cmath>
#include <happly.h>

namespace cathedral::engine
{
    mesh::mesh(const std::string& path, [[maybe_unused]] size_t shape_index)
    {
        init_for_ply(path);
    }

    void mesh::init_for_ply(const std::string& path)
    {
        happly::PLYData ply(path);

        auto& vertex = ply.getElement("vertex");

        // position
        for (const auto& pos : ply.getVertexPositions())
        {
            _pos.emplace_back(pos[0], pos[1], pos[2]);
        }

        // color
        if (vertex.hasProperty("red"))
        {
            for (const auto& col : ply.getVertexColors())
            {
                _color.emplace_back(col[0], col[1], col[2], 1.0f);
            }
        }
        else
        {
            for (size_t i = 0; i < _pos.size(); ++i)
            {
                _color.emplace_back(
                    std::fmod(static_cast<float>(i) / 2, 1.0f),
                    std::fmod(static_cast<float>(i) / 3, 1.0f),
                    std::fmod(static_cast<float>(i) / 5, 1.0f),
                    1.0f);
            }
        }

        // uv
        if (vertex.hasProperty("s"))
        {
            auto u = vertex.getProperty<float>("s");
            auto v = vertex.getProperty<float>("t");
            CRITICAL_CHECK(u.size() == v.size());
            for (size_t i = 0; i < u.size(); ++i)
            {
                _uv.emplace_back(u[i], v[i]);
            }
        }
        else
        {
            for (size_t i = 0; i < _pos.size(); ++i)
            {
                _uv.emplace_back(0.0f, 0.0f);
            }
        }

        // normal
        if (vertex.hasProperty("nx"))
        {
            auto nx = vertex.getProperty<float>("nx");
            auto ny = vertex.getProperty<float>("ny");
            auto nz = vertex.getProperty<float>("nz");
            CRITICAL_CHECK(nx.size() == ny.size() && nx.size() == nz.size());
            for (size_t i = 0; i < nx.size(); ++i)
            {
                _normal.emplace_back(nx[i], ny[i], nz[i]);
            }
        }
        else
        {
            for (size_t i = 0; i < _pos.size(); ++i)
            {
                _normal.emplace_back(0.0f, 0.0f, 0.0f);
            }
        }

        // indices
        for (const auto& indices : ply.getFaceIndices())
        {
            if (indices.size() == 3)
            {
                _indices.push_back(indices[0]);
                _indices.push_back(indices[1]);
                _indices.push_back(indices[2]);
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