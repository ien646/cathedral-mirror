#pragma once

#include <cathedral/engine/mesh_buffer_storage.hpp>
#include <cathedral/gfx/buffers.hpp>

#include <variant>

namespace cathedral::engine
{
    class renderer_deleter
    {
    public:
        using resource = std::variant<gfx::generic_buffer*, std::shared_ptr<mesh_buffer>, vk::UniqueDescriptorSet>;

        void add_generic_buffer(gfx::generic_buffer* buffer_owning_ptr);
        void add_mesh_buffer(std::shared_ptr<mesh_buffer> mesh_buffer);
        void add_unique_descriptor_set(vk::UniqueDescriptorSet dset);

        void delete_all_resources();

    private:
        std::vector<resource> _resources;
    };
} // namespace cathedral::engine