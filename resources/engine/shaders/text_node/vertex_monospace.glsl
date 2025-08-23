$NODE_VARIABLE mat4 node_model_matrix;
$NODE_VARIABLE float horizontal_stride;
$NODE_VARIABLE uvec2 atlas_size;
$NODE_VARIABLE uvec2 glyph_size;

struct text_buffer_char
{
    vec2 offset;
    vec2 size;
    float horizontal_advance;
    float left_bearing;
    float kerning;
    uint charcode;
};
$NODE_BUFFER text_buffer { text_buffer_char chars[]; };

layout (location = 0) out vec3 frag_pos;
layout (location = 1) out vec3 frag_normal;
layout (location = 2) out vec2 frag_uv;

float remap(float value, float min1, float max1, float min2, float max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

vec3 get_char_local_offset(text_buffer_char ch)
{
    vec3 result = vec3(ch.offset * vec2(2.0, 2.0), 0.0);
    result.x -= 1.0 - ch.size.x;
    return result;
}

mat4 get_normal_matrix(mat4 model)
{
    return inverse(transpose(model));
}

vec3 get_camera3d_position()
{
    return vec3(VIEW_3D[3][0], VIEW_3D[3][1], VIEW_3D[3][2]);
}

void main()
{
    text_buffer_char ch = text_buffer.chars[gl_InstanceIndex];

    vec4 world_pos = node_model_matrix * vec4(VERTEX_POSITION, 1.0);

    float camera_distance = distance(get_camera3d_position(), vec3(world_pos));

    vec3 pos = VERTEX_POSITION + (vec3(1.0, 0.0, -0.002 * sqrt(camera_distance)) * gl_InstanceIndex);
    pos -= get_char_local_offset(ch);
    pos -= vec3(0.0, 1.0, 0.0);

    float stride = horizontal_stride * gl_InstanceIndex;
    pos.x += stride;

    gl_Position = PROJECTION_3D * VIEW_3D * node_model_matrix * vec4(pos, 1.0);
    frag_pos = vec3(node_model_matrix * vec4(pos, 1.0));
    frag_normal = vec3(get_normal_matrix(node_model_matrix) * vec4(VERTEX_NORMAL, 1.0));

    uint row_count = atlas_size.x / glyph_size.x;
    uint col_count = atlas_size.y / glyph_size.y;

    uint row = ch.charcode / row_count;
    uint col = ch.charcode % col_count;

    float ox = float(col) * (1.0 / row_count);
    float oy = float(row) * (1.0 / col_count);

    float uvx = remap(VERTEX_UVCOORD.x, 0.0, 1.0, ox, ox + (1.0 / row_count) - 0.001);
    float uvy = remap(VERTEX_UVCOORD.y, 0.0, 1.0, oy, oy + (1.0 / col_count) - 0.001);

    frag_uv = vec2(uvx, uvy);
}