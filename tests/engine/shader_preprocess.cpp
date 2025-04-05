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

    $MATERIAL_TEXTURE mtex_1;
    $MATERIAL_TEXTURE mtex_2;

    $NODE_TEXTURE ntex_1;
    $NODE_TEXTURE ntex_2;

    $MATERIAL_TEXTURE mtex_3;

    $NODE_TEXTURE ntex_3;

    layout(location=0) out vec3 color;
    layout(location=1) out vec3 normal;

    void main()
    {
        // whatever
    }
)glsl";

TEST_CASE("shader variables")
{
    auto proc = engine::preprocess_shader(gfx::shader_type::VERTEX, SOURCE_A_PRE);

    SECTION("Material vars")
    {
        REQUIRE(proc->material_vars.size() == 3);

        REQUIRE(proc->material_vars[0].name == "mvar_1");
        REQUIRE(proc->material_vars[0].binding == std::nullopt);
        REQUIRE(proc->material_vars[0].type == gfx::shader_data_type::VEC2);
        REQUIRE(proc->material_vars[0].count == 1);

        REQUIRE(proc->material_vars[1].name == "mvar_2");
        REQUIRE(proc->material_vars[1].binding == std::nullopt);
        REQUIRE(proc->material_vars[1].type == gfx::shader_data_type::FLOAT);
        REQUIRE(proc->material_vars[1].count == 16);

        REQUIRE(proc->material_vars[2].name == "mvar_3");
        REQUIRE(proc->material_vars[2].binding == std::nullopt);
        REQUIRE(proc->material_vars[2].type == gfx::shader_data_type::MAT4X4);
        REQUIRE(proc->material_vars[2].count == 1);
    }

    SECTION("Node vars")
    {
        REQUIRE(proc->node_vars.size() == 3);

        REQUIRE(proc->node_vars[0].name == "nvar_1");
        REQUIRE(proc->node_vars[0].binding == std::nullopt);
        REQUIRE(proc->node_vars[0].type == gfx::shader_data_type::VEC3);
        REQUIRE(proc->node_vars[0].count == 55);

        REQUIRE(proc->node_vars[1].name == "nvar_2");
        REQUIRE(proc->node_vars[1].binding == std::nullopt);
        REQUIRE(proc->node_vars[1].type == gfx::shader_data_type::MAT3X2);
        REQUIRE(proc->node_vars[1].count == 1);

        REQUIRE(proc->node_vars[2].name == "nvar_3");
        REQUIRE(proc->node_vars[2].binding == std::nullopt);
        REQUIRE(proc->node_vars[2].type == gfx::shader_data_type::DVEC4);
        REQUIRE(proc->node_vars[2].count == 1);
    }

    SECTION("Material textures")
    {
        REQUIRE(proc->material_textures.size() == 3);
        REQUIRE(proc->material_textures[0] == "mtex_1");
        REQUIRE(proc->material_textures[1] == "mtex_2");
        REQUIRE(proc->material_textures[2] == "mtex_3");
    }

    SECTION("Node textures")
    {
        REQUIRE(proc->node_textures.size() == 3);
        REQUIRE(proc->node_textures[0] == "ntex_1");
        REQUIRE(proc->node_textures[1] == "ntex_2");
        REQUIRE(proc->node_textures[2] == "ntex_3");
    }
}