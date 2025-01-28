#pragma once

#include <cathedral/engine/mesh.hpp>
#include <cathedral/gfx/buffers.hpp>

#include <unordered_map>

namespace cathedral::engine
{
    class renderer;

    struct mesh_buffer
    {
        gfx::vertex_buffer vertex_buffer;
        gfx::index_buffer index_buffer;
    };

    class mesh_buffer_storage
    {
    public:
        explicit mesh_buffer_storage(renderer& rend);
        std::shared_ptr<mesh_buffer> get_mesh_buffers(const std::string& mesh_path, const engine::mesh& mesh_ref);

    private:
        renderer& _renderer;
        std::unordered_map<std::string, std::weak_ptr<mesh_buffer>> _buffers;
    };
} // namespace cathedral::engine