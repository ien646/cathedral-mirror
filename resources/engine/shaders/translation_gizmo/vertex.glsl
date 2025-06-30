$NODE_VARIABLE mat4 model_matrix;

layout(location = 0) out vec3 out_color;

const float GIZMO_SIZE = 1.0 / 20;

void main()
{
    vec3 gizmo_world_position = vec3(model_matrix[3]);
    vec3 gizmo_view_position = vec3(VIEW_3D * vec4(gizmo_world_position, 1.0));
    float gizmo_view_depth = gizmo_view_position.z;

    float scale = GIZMO_SIZE * gizmo_view_depth;

    vec3 woffset = (model_matrix * vec4(VERTEX_POSITION * scale, 0.0)).xyz;

    gl_Position = PROJECTION_3D * VIEW_3D * vec4(gizmo_world_position + woffset, 1.0);
    out_color = VERTEX_COLOR.rgb;
}