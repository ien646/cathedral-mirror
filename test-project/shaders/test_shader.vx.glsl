#version 450

layout(location = 0) in vec3 vx_pos;
layout(location = 1) in vec2 vx_uv;
layout(location = 2) in vec3 vx_normal;
layout(location = 3) in vec4 vx_color;

layout(set=0, binding=0) uniform scene_uniform_data_t
{
    float deltatime;
    uint frame_index;
    mat4 projection3d;
    mat4 view3d;
} scene_uniform_data;

layout(set=1, binding=0) uniform material_uniform_data_t
{
    vec4 tint;
} material_uniform_data;

layout(set=2, binding=0) uniform node_uniform_data_t
{
    mat4 model;
} node_uniform_data;

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec2 out_uv;

void main()
{
    gl_Position = scene_uniform_data.projection3d * scene_uniform_data.view3d * node_uniform_data.model * vec4(vx_pos, 1.0);
    out_color = vx_color * material_uniform_data.tint;
    out_uv = vx_uv;
}