#include <catch2/catch_test_macros.hpp>
#include <cathedral/engine/camera.hpp>
#include <cathedral/engine/frustum.hpp>

using namespace cathedral;
using namespace cathedral::engine;

TEST_CASE("Frustum tests")
{
    // Camera is looking forward towards (0,0,0) from negative Z
    const perspective_camera camera(60, 1.0F, 0.01F, 100.0F, glm::vec3{ 0.0F, 0.0F, -5.0F });

    const auto frustum = camera.get_frustum_planes();

    SECTION("Point tests")
    {
        REQUIRE(is_point_inside_frustum({ 0.0F, 0.0F, 0.0F }, frustum, false));
        REQUIRE(is_point_inside_frustum({ 1.0F, 0.0F, 0.0F }, frustum, false));
        REQUIRE(is_point_inside_frustum({ -1.0F, 0.0F, 0.0F }, frustum, false));
        REQUIRE(is_point_inside_frustum({ 0.0F, 1.0F, 0.0F }, frustum, false));
        REQUIRE(is_point_inside_frustum({ 0.0F, -1.0F, 0.0F }, frustum, false));
        REQUIRE_FALSE(is_point_inside_frustum({ 50.0F, 0.0F, 0.0F }, frustum, false));
        REQUIRE_FALSE(is_point_inside_frustum({ -10.0F, 0.0F, 0.0F }, frustum, false));
        REQUIRE(is_point_inside_frustum({ 0.0F, 0.0F, 5.0F }, frustum, false));
        REQUIRE_FALSE(is_point_inside_frustum({ 0.0F, 0.0F, -6.0F }, frustum, false));
        REQUIRE_FALSE(is_point_inside_frustum({ 0.0F, 0.0F, -5.01F }, frustum, false));
        REQUIRE(is_point_inside_frustum({ 0.0F, 0.0F, -4.95F }, frustum, false));
        REQUIRE(is_point_inside_frustum({ 0.0F, 0.0F, 94.95F }, frustum, false));
        REQUIRE_FALSE(is_point_inside_frustum({ 0.0F, 0.0F, 95.95F }, frustum, false));
    }

    SECTION("Sphere tests")
    {
        REQUIRE(is_sphere_inside_frustum({ { 0.0F, 0.0F, 0.0F }, 1.0F }, frustum));
        REQUIRE(is_sphere_inside_frustum({ { 1.0F, 0.0F, 0.0F }, 1.0F }, frustum));
        REQUIRE(is_sphere_inside_frustum({ { 1.0F, 1.0F, 0.0F }, 1.0F }, frustum));
        REQUIRE(is_sphere_inside_frustum({ { 0.0F, 0.0F, 5.0F }, 1.0F }, frustum));
        REQUIRE(is_sphere_inside_frustum({ { 0.0F, 0.0F, -5.0F }, 1.0F }, frustum));
        REQUIRE_FALSE(is_sphere_inside_frustum({ { 0.0F, 0.0F, -6.01F }, 1.0F }, frustum));
        REQUIRE_FALSE(is_sphere_inside_frustum({ { 100.0F, 0.0F, 0.0F }, 1.0F }, frustum));
        REQUIRE(is_sphere_inside_frustum({ { 0.0F, 0.0F, 95.0F }, 1.0F }, frustum));
        REQUIRE_FALSE(is_sphere_inside_frustum({ { 0.0F, 0.0F, 96.01F }, 1.0F }, frustum));
        REQUIRE_FALSE(is_sphere_inside_frustum({ { 0.0F, 100.0F, 0.00F }, 1.0F }, frustum));
    }

    SECTION("AABB tests")
    {
        aabb test_aabb{ .min = { -0.5F, -0.5F, -0.5F }, .max = { 0.5F, 0.5F, 0.5F } };
        REQUIRE(is_aabb_inside_frustum(test_aabb, frustum));

        test_aabb.min = {-500.0F, -500.0F, -500.0F };
        REQUIRE(is_aabb_inside_frustum(test_aabb, frustum));

        test_aabb.max = {-400.0F, -400.0F, -400.0F };
        REQUIRE_FALSE(is_aabb_inside_frustum(test_aabb, frustum));

        test_aabb.min = {500.0F, 500.0F, 500.0F };
        test_aabb.max = {400.0F, 400.0F, 400.0F };
        REQUIRE_FALSE(is_aabb_inside_frustum(test_aabb, frustum));
    }
}