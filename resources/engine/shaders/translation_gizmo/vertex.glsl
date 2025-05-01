$NODE_VARIABLE mat4 model_matrix;

const float GIZMO_SCALE = 1 / 50;

layout(location = 0) out vec3 out_color;

void main()
{
    vec4 pos = PROJECTION_3D * VIEW_3D * model_matrix * vec4(VERTEX_POSITION, 1.0);    

    mat4 mod_model = model_matrix;
    mod_model[0][0] = sign(model_matrix[0][0]) * pos.z * GIZMO_SCALE;
    mod_model[1][1] = sign(model_matrix[1][1]) * pos.z * GIZMO_SCALE;
    mod_model[2][2] = sign(model_matrix[2][2]) * pos.z * GIZMO_SCALE;

    gl_Position = PROJECTION_3D * VIEW_3D * mod_model * vec4(VERTEX_POSITION, 1.0);
    out_color = VERTEX_COLOR.rgb;
}