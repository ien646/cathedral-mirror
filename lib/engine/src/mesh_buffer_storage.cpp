#include <cathedral/engine/mesh_buffer_storage.hpp>

#include <cathedral/engine/scene.hpp>
#include <cathedral/engine/vertex_pack.hpp>

namespace cathedral::engine
{
    mesh_buffer_storage::mesh_buffer_storage(renderer& rend)
        : _renderer(rend)
    {
    }

    std::shared_ptr<mesh_buffer> mesh_buffer_storage::get_mesh_buffers(const std::string& mesh_path, const engine::mesh& mesh_ref)
    {
        const auto generate_vxbuff = [&]() {
            const auto vertex_data = mesh_ref.get_packed_data();

            gfx::vertex_buffer_args vxbuff_args;
            vxbuff_args.vertex_size = mesh::vertex_size_bytes();
            vxbuff_args.size = vertex_data.size() * sizeof(float);
            vxbuff_args.vkctx = &_renderer.vkctx();

            gfx::vertex_buffer vxbuff(vxbuff_args);

            gfx::index_buffer_args ixbuff_args;
            ixbuff_args.size = mesh_ref.indices().size() * sizeof(unsigned int);
            ixbuff_args.vkctx = &_renderer.vkctx();

            gfx::index_buffer ixbuff(ixbuff_args);

            auto& upload_queue = _renderer.get_upload_queue();
            upload_queue.update_buffer(vxbuff, 0, std::span{ vertex_data });
            upload_queue.update_buffer(ixbuff, 0, std::span{ mesh_ref.indices() });

            auto shptr = std::make_shared<mesh_buffer>(
                mesh_buffer{ .vertex_buffer = std::move(vxbuff), .index_buffer = std::move(ixbuff) });

            _buffers.try_emplace(mesh_path, shptr);
            return shptr;
        };

        if (_buffers.contains(mesh_path))
        {
            std::weak_ptr<mesh_buffer> buff_wptr = _buffers[mesh_path];
            if (buff_wptr.expired())
            {
                return generate_vxbuff();
            }
            return buff_wptr.lock();
        }
        return generate_vxbuff();
    }
} // namespace cathedral::engine