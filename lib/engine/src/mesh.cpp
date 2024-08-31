#include <cathedral/engine/mesh.hpp>

#include <happly.h>

namespace cathedral::engine
{
    mesh::mesh(const std::string& path, size_t shape_index)
    {
        init_for_ply(path);
    }

    void mesh::init_for_ply(const std::string& path)
    {
        happly::PLYData ply(path);

        auto pos = ply.getVertexPositions();
        auto col = ply.getVertexColors();
        auto uv_s = ply.getElement("vertex").getProperty<float>("s");
        auto uv_t = ply.getElement("vertex").getProperty<float>("t");
        auto nx = ply.getElement("vertex").getProperty<float>("nx");
        auto ny = ply.getElement("vertex").getProperty<float>("ny");
        auto nz = ply.getElement("vertex").getProperty<float>("nz");
        auto faceIndices = ply.getFaceIndices();

        for(const auto& indices : faceIndices)
        {
            auto pos0 = pos[indices[0]];
            auto pos1 = pos[indices[1]];
            auto pos2 = pos[indices[2]];

            auto col0 = col[indices[0]];
            auto col1 = col[indices[1]];
            auto col2 = col[indices[2]];

            glm::vec3 norm0(nx[indices[0]], ny[indices[0]], nz[indices[0]]);
            glm::vec3 norm1(nx[indices[1]], ny[indices[1]], nz[indices[1]]);
            glm::vec3 norm2(nx[indices[2]], ny[indices[2]], nz[indices[2]]);

            glm::vec2 uv0(uv_s[indices[0]], uv_t[indices[0]]);
            glm::vec2 uv1(uv_s[indices[1]], uv_t[indices[1]]);
            glm::vec2 uv2(uv_s[indices[2]], uv_t[indices[2]]);

            _pos.emplace_back(pos0[0], pos0[1], pos0[2]);
            _pos.emplace_back(pos1[0], pos1[1], pos1[2]);
            _pos.emplace_back(pos2[0], pos2[1], pos2[2]);

            _color.emplace_back(col0[0], col0[1], col0[2]);
            _color.emplace_back(col1[0], col1[1], col1[2]);
            _color.emplace_back(col2[0], col2[1], col2[2]);

            _normal.push_back(norm0);
            _normal.push_back(norm1);
            _normal.push_back(norm2);

            _uv.push_back(uv0);
            _uv.push_back(uv1);
            _uv.push_back(uv2);
        }
    }
} // namespace cathedral