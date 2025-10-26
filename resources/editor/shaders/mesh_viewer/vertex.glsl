$NODE_VARIABLE mat4 node_model_matrix;

layout (location = 0) out vec3 frag_pos;
layout (location = 1) out vec3 frag_normal;
layout (location = 2) out vec2 frag_uv;

void main()
{
    gl_Position = PROJECTION_3D * VIEW_3D * node_model_matrix * vec4(VERTEX_POSITION, 1.0);
    frag_pos = vec3(node_model_matrix * vec4(VERTEX_POSITION, 1.0));
    frag_normal = vec3(inverse(transpose(node_model_matrix)) * vec4(VERTEX_NORMAL, 1.0));
    frag_uv = VERTEX_UVCOORD;
}