#pragma once

#include <cathedral/core.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <string>
#include <vector>

namespace cathedral::engine
{
    class mesh
    {
    public:
        mesh(const std::string& path, size_t shape_index = 0);

        inline const auto& positions() const { return _pos; }

        inline const auto& normals() const { return _normal; }

        inline const auto& uvcoords() const { return _uv; }

        inline const auto& colors() const { return _color; }

        inline const auto& indices() const { return _indices; }

        inline size_t vertex_count() const { return _pos.size(); }

        static constexpr size_t vertex_size_bytes() { return 11 * sizeof(float); }

    private:
        std::vector<glm::vec3> _pos;
        std::vector<glm::vec3> _normal;
        std::vector<glm::vec2> _uv;
        std::vector<glm::vec4> _color;
        std::vector<uint32_t> _indices;

        void init_for_ply(const std::string& path);
    };
} // namespace cathedral::engine