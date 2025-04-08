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
    auto pp_data = *engine::get_shader_preprocess_data(SOURCE_A_PRE);

    SECTION("Material vars")
    {
        REQUIRE(pp_data.material_vars.size() == 3);

        REQUIRE(pp_data.material_vars[0].name == "mvar_1");
        REQUIRE(pp_data.material_vars[0].binding == std::nullopt);
        REQUIRE(pp_data.material_vars[0].type == gfx::shader_data_type::VEC2);
        REQUIRE(pp_data.material_vars[0].count == 1);

        REQUIRE(pp_data.material_vars[1].name == "mvar_2");
        REQUIRE(pp_data.material_vars[1].binding == std::nullopt);
        REQUIRE(pp_data.material_vars[1].type == gfx::shader_data_type::FLOAT);
        REQUIRE(pp_data.material_vars[1].count == 16);

        REQUIRE(pp_data.material_vars[2].name == "mvar_3");
        REQUIRE(pp_data.material_vars[2].binding == std::nullopt);
        REQUIRE(pp_data.material_vars[2].type == gfx::shader_data_type::MAT4X4);
        REQUIRE(pp_data.material_vars[2].count == 1);
    }

    SECTION("Node vars")
    {
        REQUIRE(pp_data.node_vars.size() == 3);

        REQUIRE(pp_data.node_vars[0].name == "nvar_1");
        REQUIRE(pp_data.node_vars[0].binding == std::nullopt);
        REQUIRE(pp_data.node_vars[0].type == gfx::shader_data_type::VEC3);
        REQUIRE(pp_data.node_vars[0].count == 55);

        REQUIRE(pp_data.node_vars[1].name == "nvar_2");
        REQUIRE(pp_data.node_vars[1].binding == std::nullopt);
        REQUIRE(pp_data.node_vars[1].type == gfx::shader_data_type::MAT3X2);
        REQUIRE(pp_data.node_vars[1].count == 1);

        REQUIRE(pp_data.node_vars[2].name == "nvar_3");
        REQUIRE(pp_data.node_vars[2].binding == std::nullopt);
        REQUIRE(pp_data.node_vars[2].type == gfx::shader_data_type::DVEC4);
        REQUIRE(pp_data.node_vars[2].count == 1);
    }

    SECTION("Material textures")
    {
        REQUIRE(pp_data.material_textures.size() == 3);
        REQUIRE(pp_data.material_textures[0] == "mtex_1");
        REQUIRE(pp_data.material_textures[1] == "mtex_2");
        REQUIRE(pp_data.material_textures[2] == "mtex_3");
    }

    SECTION("Node textures")
    {
        REQUIRE(pp_data.node_textures.size() == 3);
        REQUIRE(pp_data.node_textures[0] == "ntex_1");
        REQUIRE(pp_data.node_textures[1] == "ntex_2");
        REQUIRE(pp_data.node_textures[2] == "ntex_3");
    }
}