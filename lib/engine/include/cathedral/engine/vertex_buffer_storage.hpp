#pragma once

#include <cathedral/engine/mesh.hpp>
#include <cathedral/gfx/buffers.hpp>

#include <unordered_map>

namespace cathedral::engine
{
    class renderer;

    class vertex_buffer_storage
    {
    public:
        vertex_buffer_storage(renderer& rend);
        std::shared_ptr<std::pair<gfx::vertex_buffer, gfx::index_buffer>> get_mesh_buffers(
            const std::string& mesh_path);

    private:
        renderer& _renderer;
        std::unordered_map<std::string, std::weak_ptr<std::pair<gfx::vertex_buffer, gfx::index_buffer>>> _buffers;
    };
} // namespace cathedral::engine