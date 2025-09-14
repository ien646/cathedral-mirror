#version 450

layout (location = 0) in vec4 vertex_position;
layout (location = 1) in vec4 vertex_color;

layout (location = 0) out vec4 frag_color;

#ifndef DIRECTIONAL_LIGHT_COUNT
#define DIRECTIONAL_LIGHT_COUNT 1
#endif

#ifndef POINT_LIGHT_COUNT
#define POINT_LIGHT_COUNT 20
#endif

struct scene_point_light
{
    vec3 position;
    float intensity;
    vec3 color;
    float range;
    float falloff_coefficient;
};

struct scene_directional_light
{
    vec3 direction;
    float intensity;
    vec3 color;
};

layout (set = 0, binding = 0) uniform _scene_uniform_data_ {
    float deltatime;
    uint frame_index;
    uint enabled_point_lights;
    uint enabled_directional_lights;
    vec3 ambient_light;
    uint viewport_width;
    uint viewport_height;
    mat4 projection2d;
    mat4 projection3d;
    mat4 view2d;
    mat4 view3d;
    scene_directional_light directional_lights[DIRECTIONAL_LIGHT_COUNT];
    scene_point_light point_lights[POINT_LIGHT_COUNT];
} scene_uniform_data;

#define PROJECTION_3D scene_uniform_data.projection3d
#define VIEW_3D scene_uniform_data.view3d

void main()
{
    gl_Position = PROJECTION_3D * VIEW_3D * vec4(vertex_position.xyz, 1.0);
    frag_color = vertex_color;
}