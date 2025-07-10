#include "cathedral/engine/camera.hpp"

#include <cathedral/engine/frustum.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("frustum point tests")
{
    // Camera is looking forward towards (0,0,0) from negative Z
    cathedral::engine::perspective_camera camera(60, 1.0F, 0.01F, 100.0F, glm::vec3{ 0.0F, 0.0F, -5.0F });

    const auto frustum = camera.get_frustum_planes();

    SECTION("Frustum contains (0,0,0)")
    {
        REQUIRE(cathedral::engine::is_point_inside_frustum({ 0.0F, 0.0F, 0.0F }, frustum, false));
    }

    SECTION("Frustum contains (1,0,0)")
    {
        REQUIRE(cathedral::engine::is_point_inside_frustum({ 1.0F, 0.0F, 0.0F }, frustum, false));
    }

    SECTION("Frustum contains (-1,0,0)")
    {
        REQUIRE(cathedral::engine::is_point_inside_frustum({ -1.0F, 0.0F, 0.0F }, frustum, false));
    }

    SECTION("Frustum contains (0,1,0)")
    {
        REQUIRE(cathedral::engine::is_point_inside_frustum({ 0.0F, 1.0F, 0.0F }, frustum, false));
    }

    SECTION("Frustum contains (0,-1,0)")
    {
        REQUIRE(cathedral::engine::is_point_inside_frustum({ 0.0F, -1.0F, 0.0F }, frustum, false));
    }

    SECTION("Frustum does not contain (50,0,0)")
    {
        REQUIRE_FALSE(cathedral::engine::is_point_inside_frustum({ 50.0F, 0.0F, 0.0F }, frustum, false));
    }

    SECTION("Frustum does not contain (-10,0,0)")
    {
        REQUIRE_FALSE(cathedral::engine::is_point_inside_frustum({ -10.0F, 0.0F, 0.0F }, frustum, false));
    }

    SECTION("Frustum contains (0,0,5)")
    {
        REQUIRE(cathedral::engine::is_point_inside_frustum({ 0.0F, 0.0F, 5.0F }, frustum, false));
    }

    SECTION("Frustum does not contain (0,0,-6)")
    {
        REQUIRE_FALSE(cathedral::engine::is_point_inside_frustum({ 0.0F, 0.0F, -6.0F }, frustum, false));
    }

    SECTION("Frustum does not contain (0,0,-5.01)")
    {
        REQUIRE_FALSE(cathedral::engine::is_point_inside_frustum({ 0.0F, 0.0F, -5.01F }, frustum, false));
    }

    SECTION("Frustum contains (0,0,-4.95)")
    {
        REQUIRE(cathedral::engine::is_point_inside_frustum({ 0.0F, 0.0F, -4.95F }, frustum, false));
    }

    SECTION("Frustum contains (0,0,94.5F)")
    {
        REQUIRE(cathedral::engine::is_point_inside_frustum({ 0.0F, 0.0F, 94.95F }, frustum, false));
    }

    SECTION("Frustum does not contain (0,0,95.5F)")
    {
        REQUIRE_FALSE(cathedral::engine::is_point_inside_frustum({ 0.0F, 0.0F, 95.95F }, frustum, false));
    }
}