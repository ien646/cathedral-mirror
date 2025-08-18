$NODE_VARIABLE vec3 text_color;

struct text_buffer_char
{
    vec2 offset;
    vec2 size;
    uint charcode;
};

layout (location = 0) in vec3 frag_pos;
layout (location = 1) in vec3 frag_normal;
layout (location = 2) in vec2 frag_uv;

layout (location = 0) out vec4 out_color;

$NODE_TEXTURE font_atlas;

void main()
{
    float v = texture(font_atlas, frag_uv).x;
    out_color = vec4(v, v, v, v) * vec4(text_color, 1.0);
}