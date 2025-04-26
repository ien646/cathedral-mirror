$NODE_VARIABLE mat4 model_matrix;

layout(location = 0) out vec3 out_color;

void main()
{
    gl_Position = PROJECTION_3D * VIEW_3D * model_matrix * vec4(VERTEX_POSITION, 1.0);
    out_color = VERTEX_COLOR.rgb;
}