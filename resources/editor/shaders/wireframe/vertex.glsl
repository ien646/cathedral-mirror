$NODE_VARIABLE mat4 node_model;

void main()
{
    gl_Position = PROJECTION_3D * VIEW_3D * node_model * vec4(VERTEX_POSITION, 1.0);
}