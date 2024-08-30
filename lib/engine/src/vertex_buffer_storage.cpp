#include <cathedral/engine/vertex_buffer_storage.hpp>

#include <cathedral/engine/vertex_pack.hpp>

namespace cathedral::engine
{
    vertex_buffer_storage::vertex_buffer_storage(const gfx::vulkan_context& vkctx)
        : _vkctx(vkctx)
    {
    }

    std::shared_ptr<gfx::vertex_buffer> vertex_buffer_storage::get_vertex_buffer_for_mesh(const std::string& mesh_path)
    {
        const auto generate_vxbuff = [&] -> std::shared_ptr<gfx::vertex_buffer> {
            const mesh m(mesh_path);
            const auto vertex_data = pack_vertex_data(m.positions(), m.uvcoords(), m.normals(), m.colors());

            gfx::vertex_buffer_args _args;
            _args.vertex_size = (3 * 2 * 3 * 4) * sizeof(float);
            _args.size = vertex_data.size() * sizeof(float);
            _args.vkctx = &_vkctx;

            std::shared_ptr<gfx::vertex_buffer> result = std::make_shared<gfx::vertex_buffer>(_args);
            _vertex_buffers.emplace(mesh_path, result);

            return result;
        };

        if (_vertex_buffers.count(mesh_path))
        {
            std::weak_ptr<gfx::vertex_buffer> vxbuff_wptr = _vertex_buffers[mesh_path];
            if (vxbuff_wptr.expired())
            {
                return generate_vxbuff();
            }
            return vxbuff_wptr.lock();
        }
        return generate_vxbuff();
    }
} // namespace cathedral::engine