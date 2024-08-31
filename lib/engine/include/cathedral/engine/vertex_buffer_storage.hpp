#pragma once

#include <cathedral/engine/mesh.hpp>
#include <cathedral/gfx/buffers.hpp>

#include <unordered_map>

namespace cathedral::engine
{
    class vertex_buffer_storage
    {
    public:
        vertex_buffer_storage(const gfx::vulkan_context& vkctx);
        std::shared_ptr<gfx::vertex_buffer> get_vertex_buffer_for_mesh(const std::string& mesh_path);

    private:
        const gfx::vulkan_context& _vkctx;
        std::unordered_map<std::string, std::weak_ptr<gfx::vertex_buffer>> _vertex_buffers;
    };
} // namespace cathedral::engine