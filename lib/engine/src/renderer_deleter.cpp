#include <cathedral/engine/renderer_deleter.hpp>

namespace cathedral::engine
{
    void renderer_deleter::add_generic_buffer(gfx::generic_buffer* buffer_owning_ptr)
    {
        _resources.emplace_back(buffer_owning_ptr);
    }

    void renderer_deleter::add_mesh_buffer(std::shared_ptr<mesh_buffer> mesh_buffer)
    {
        _resources.emplace_back(std::move(mesh_buffer));
    }

    void renderer_deleter::add_unique_descriptor_set(vk::UniqueDescriptorSet dset)
    {
        _resources.emplace_back(std::move(dset));
    }

    void renderer_deleter::delete_all_resources()
    {
        // Manually delete any owning pointers
        for (auto& resource : _resources)
        {
            std::visit(
                []<typename T>(const T& vval) {
                    if constexpr (std::is_pointer_v<T>)
                    {
                        delete vval;
                    }
                },
                resource);
        }

        // Delete any remaining RAII objects automatically
        _resources.clear();
    }
} // namespace cathedral::engine