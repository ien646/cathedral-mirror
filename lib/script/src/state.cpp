#include <cathedral/script/state.hpp>

#include <cathedral/script/initializers.hpp>

#define STATE_NAME _cathedral_state
#define INIT(prefix) prefix ## _initializer{}.initialize(STATE_NAME)

namespace cathedral::script
{
    state get_initial_state()
    {
        state STATE_NAME;
        INIT(enums);
        INIT(mat);
        INIT(vec);

        INIT(engine::mesh3d_node);
        INIT(engine::node);
        INIT(engine::scene);
        INIT(engine::scene_node);
        INIT(engine::transform);

        return STATE_NAME;
    }
}