#pragma once

#include <cathedral/core.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <string>
#include <vector>

FORWARD_CLASS(happly, PLYData);

namespace cathedral::engine
{
    class mesh
    {
    public:
        mesh(const std::string& path, size_t shape_index = 0);
        mesh(
            std::vector<glm::vec3> positions,
            std::vector<glm::vec2> uvcoords,
            std::vector<glm::vec3> normals,
            std::vector<glm::vec4> colors,
            std::vector<uint32_t> indices);

        const auto& positions() const { return _pos; }

        const auto& normals() const { return _normal; }

        const auto& uvcoords() const { return _uv; }

        const auto& colors() const { return _color; }

        const auto& indices() const { return _indices; }

        size_t vertex_count() const { return _pos.size(); }

        static constexpr size_t vertex_size_bytes() { return 12 * sizeof(float); }

        std::vector<float> get_packed_data() const;

        size_t size_in_bytes() const;

    private:
        std::vector<glm::vec3> _pos;
        std::vector<glm::vec2> _uv;
        std::vector<glm::vec3> _normal;
        std::vector<glm::vec4> _color;
        std::vector<uint32_t> _indices;

        void init_for_ply(const std::string& path);

        void fill_positions(happly::PLYData& data);
        void fill_normals(happly::PLYData& data);
        void fill_uvcoords(happly::PLYData& data);
        void fill_colors(happly::PLYData& data);
        void fill_indices(happly::PLYData& data);
    };
} // namespace cathedral::engine