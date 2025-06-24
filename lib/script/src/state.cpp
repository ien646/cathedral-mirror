#include <cathedral/script/state.hpp>

#include <cathedral/script/initializers.hpp>

#define STATE_NAME _cathedral_state
#define INIT(prefix) prefix ## _initializer{}.initialize(STATE_NAME)

namespace cathedral::script
{
    state get_initial_state()
    {
        state STATE_NAME;
        STATE_NAME.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string);
        INIT(global_table);
        INIT(error);
        INIT(enums);
        INIT(log);
        INIT(mat);
        INIT(vec);

        INIT(engine::directional_light_node);
        INIT(engine::material);
        INIT(engine::mesh3d_node);
        INIT(engine::node);
        INIT(engine::point_light_node);
        INIT(engine::scene);
        INIT(engine::scene_node);
        INIT(engine::transform);

        return STATE_NAME;
    }
}