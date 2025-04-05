#include <catch2/catch_test_macros.hpp>

#include <cathedral/engine/shader_preprocess.hpp>

using namespace cathedral;

constexpr const char* SOURCE_A_PRE = R"glsl(
    $MATERIAL_VARIABLE vec2 mvar_1;
    $MATERIAL_VARIABLE float mvar_2[16];
    $MATERIAL_VARIABLE mat4 mvar_3;

    $NODE_VARIABLE vec3 nvar_1[55];
    $NODE_VARIABLE mat3x2 nvar_2;
    $NODE_VARIABLE dvec4 nvar_3;

    layout(location=0) out vec3 color;
    layout(location=1) out vec3 normal;

    void main()
    {
        // whatever
    }
)glsl";

TEST_CASE("test")
{
    auto proc = engine::preprocess_shader(gfx::shader_type::VERTEX, SOURCE_A_PRE);
    REQUIRE(false == false);
}