#include <cathedral/script/state.hpp>

#include <cathedral/script/initializers.hpp>

#define STATE_NAME _cathedral_state
#define INIT(prefix) prefix##_initializer{}.initialize(STATE_NAME)

namespace cathedral::script
{
    namespace
    {
        template <typename... Args>
        state initialize_state()
        {
            state result;
            result.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string);
            (Args{}.initialize(result), ...);
            return result;
        }

        template <typename... Args>
        std::string initializer_annotations()
        {
            return (Args{}.get_annotations() + ...);
        }
    } // namespace

    state get_initial_state()
    {
        return initialize_state<
            global_table_initializer,
            error_initializer,
            enums_initializer,
            log_initializer,
            mat_initializer,
            vec_initializer,
            engine::directional_light_node_initializer,
            engine::material_initializer,
            engine::mesh3d_node_initializer,
            engine::node_initializer,
            engine::point_light_node_initializer,
            engine::scene_initializer,
            engine::scene_node_initializer,
            engine::transform_initializer>();
    }

    std::string get_annotations()
    {
        return initializer_annotations<
            global_table_initializer,
            error_initializer,
            enums_initializer,
            log_initializer,
            mat_initializer,
            vec_initializer,
            engine::directional_light_node_initializer,
            engine::material_initializer,
            engine::mesh3d_node_initializer,
            engine::node_initializer,
            engine::point_light_node_initializer,
            engine::scene_initializer,
            engine::scene_node_initializer,
            engine::transform_initializer>();
    }
} // namespace cathedral::script