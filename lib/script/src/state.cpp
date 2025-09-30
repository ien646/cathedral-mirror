#include <cathedral/script/state.hpp>

#include <cathedral/script/initializers.hpp>

#define STATE_NAME _cathedral_state
#define INIT(prefix) prefix##_initializer{}.initialize(STATE_NAME)

namespace cathedral::script
{
    namespace
    {
        template <typename... Args>
        struct main_state_initializer;

        template <typename... Args>
        struct main_state_initializer<std::tuple<Args...>>
        {
            static state initialize_state()
            {
                state result;
                result.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string);
                (Args{}.initialize(result), ...);
                return result;
            }
        };

        template <typename... Args>
        struct main_annotation_initializer;

        template <typename... Args>
        struct main_annotation_initializer<std::tuple<Args...>>
        {
            static std::string initializer_annotations() { return (Args{}.get_annotations() + ...); }
        };
    } // namespace

    using initializers = std::tuple<
        global_table_initializer,
        error_initializer,
        enums_initializer,
        log_initializer,
        mat_initializer,
        vec_initializer,
        engine::drawable_node_initializer,
        engine::directional_light_node_initializer,
        engine::input_initializer,
        engine::material_initializer,
        engine::mesh3d_node_initializer,
        engine::node_initializer,
        engine::point_light_node_initializer,
        engine::scene_initializer,
        engine::scene_node_initializer,
        engine::text_node_initializer,
        engine::transform_initializer>;

    state get_initial_state()
    {
        return main_state_initializer<initializers>::initialize_state();
    }

    std::string get_annotations()
    {
        constexpr auto META = "---@meta\n\n";
        return META + main_annotation_initializer<initializers>::initializer_annotations();
    }
} // namespace cathedral::script