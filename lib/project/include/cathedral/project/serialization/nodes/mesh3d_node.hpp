#pragma once

#include <cathedral/engine/nodes/mesh3d_node.hpp>

#include <cathedral/project/serialization/transform.hpp>

#include <cathedral/glm_serializers.hpp>

#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include <ranges>

namespace cereal
{
    template <typename Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const cathedral::engine::mesh3d_node& node)
    {
        ar(make_nvp("node", cereal::base_class<cathedral::engine::node>(&node)),
           make_nvp("mesh_name", node.mesh_name()),
           make_nvp("material_name", node.material_name()),
           make_nvp("node_textures", node.texture_names()),
           make_nvp("uniform_data", node.raw_uniform_data()));
    }

    template <typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::engine::mesh3d_node& node)
    {
        std::optional<std::string> mesh_name;
        std::optional<std::string> material_name;
        std::vector<std::string> texture_names;
        std::vector<std::byte> raw_uniform_data;

        ar(cereal::base_class<cathedral::engine::node>(&node), mesh_name, material_name, texture_names, raw_uniform_data);

        if (mesh_name)
        {
            node.set_mesh(mesh_name);
        }
        if (material_name)
        {
            node.set_material(material_name);
        }

        for (uint32_t i = 0; i < texture_names.size(); ++i)
        {
            const auto& tex_name = texture_names[i];
            if (tex_name == cathedral::engine::DEFAULT_TEXTURE_NAME)
            {
                continue;
            }
            node.bind_node_texture_slot(tex_name, i);
        }

        node.set_raw_uniform_data(std::move(raw_uniform_data));
    }
} // namespace cereal

CEREAL_REGISTER_TYPE(cathedral::engine::mesh3d_node);
CEREAL_REGISTER_POLYMORPHIC_RELATION(cathedral::engine::scene_node, cathedral::engine::mesh3d_node);