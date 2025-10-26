$MATERIAL_TEXTURE material_texture;

$MATERIAL_VARIABLE float specular_intensity;
$MATERIAL_VARIABLE float specular_power;

layout (location = 0) in vec3 frag_pos;
layout (location = 1) in vec3 frag_normal;
layout (location = 2) in vec2 frag_uv;

layout (location = 0) out vec4 out_color;

vec3 get_camera3d_position()
{
    return vec3(VIEW_3D[3][0], VIEW_3D[3][1], VIEW_3D[3][2]);
}

vec3 point_lights_diffuse(vec3 frag_world_pos, vec3 frag_world_normal)
{
    vec3 result = vec3(0, 0, 0);
    for (int i = 0; i < ENABLED_POINT_LIGHTS; ++i)
    {
        const vec3 light_dir = normalize(POINT_LIGHTS[i].position - frag_world_pos);
        const float incidence = max(dot(frag_world_normal, light_dir), 0.0);

        const float distance = abs(distance(frag_world_pos, POINT_LIGHTS[i].position));

        float range_value = max((POINT_LIGHTS[i].range - distance) / POINT_LIGHTS[i].range, 0.0);
        range_value = pow(range_value, POINT_LIGHTS[i].falloff_coefficient);

        float applicable_intensity = range_value * POINT_LIGHTS[i].intensity * incidence;

        result += POINT_LIGHTS[i].color * applicable_intensity;
    }
    return result;
}

vec3 directional_lights_diffuse(vec3 frag_world_pos, vec3 frag_world_normal)
{
    vec3 result = vec3(0, 0, 0);
    for (int i = 0; i < ENABLED_DIRECTIONAL_LIGHTS; ++i)
    {
        const float incidence = max(dot(frag_world_normal, DIRECTIONAL_LIGHTS[i].direction), 0.0);
        result += DIRECTIONAL_LIGHTS[i].intensity * DIRECTIONAL_LIGHTS[i].color * incidence;
    }
    return result;
}

vec3 diffuse(vec3 frag_world_pos, vec3 frag_world_normal)
{
    return point_lights_diffuse(frag_world_pos, frag_world_normal) + directional_lights_diffuse(frag_world_pos, frag_world_normal);
}

vec3 point_lights_specular(vec3 view_world_pos, vec3 frag_world_pos, vec3 frag_world_normal, float specular_strength, float specularity_coefficient)
{
    vec3 result = vec3(0, 0, 0);
    for (int i = 0; i < ENABLED_POINT_LIGHTS; ++i)
    {
        const vec3 light_dir = normalize(POINT_LIGHTS[i].position - frag_world_pos);
        const vec3 view_dir = normalize(view_world_pos - frag_world_pos);
        const vec3 reflection_dir = normalize(reflect(light_dir, frag_world_normal));

        const float distance = abs(distance(frag_world_pos, POINT_LIGHTS[i].position));

        float range_value = max((POINT_LIGHTS[i].range - distance) / POINT_LIGHTS[i].range, 0.0);
        range_value = pow(range_value, POINT_LIGHTS[i].falloff_coefficient);

        float reflection_view_divergence = max(dot(view_dir, reflection_dir), 0.0);
        float specular_factor = pow(reflection_view_divergence, specularity_coefficient);

        result += POINT_LIGHTS[i].color * specular_factor * specular_strength * range_value * POINT_LIGHTS[i].intensity;
    }
    return result;
}

vec3 directional_lights_specular(vec3 view_world_pos, vec3 frag_world_pos, vec3 frag_world_normal, float specular_strength, float specularity_coefficient)
{
    vec3 result = vec3(0, 0, 0);
    for (int i = 0; i < ENABLED_DIRECTIONAL_LIGHTS; ++i)
    {
        const vec3 view_dir = normalize(view_world_pos - frag_world_pos);
        const vec3 reflection_dir = normalize(reflect(DIRECTIONAL_LIGHTS[i].direction, frag_world_normal));

        float reflection_view_divergence = max(dot(view_dir, reflection_dir), 0.0);
        float specular_factor = pow(reflection_view_divergence, specularity_coefficient);

        result += DIRECTIONAL_LIGHTS[i].color * specular_factor * specular_strength * DIRECTIONAL_LIGHTS[i].intensity;
    }
    return result;
}

vec3 specular(vec3 view_world_pos, vec3 frag_world_pos, vec3 frag_world_normal, float specular_strength, float specularity_coefficient)
{
    return point_lights_specular(view_world_pos, frag_world_pos, frag_world_normal, specular_strength, specularity_coefficient)
        + directional_lights_specular(view_world_pos, frag_world_pos, frag_world_normal, specular_strength, specularity_coefficient);
}

void main()
{
    out_color = vec4(1.0, 1.0, 1.0, 1.0)
                * vec4(diffuse(frag_pos, frag_normal)
                    + specular(get_camera3d_position(), frag_pos, frag_normal, specular_intensity, specular_power)
                    + AMBIENT_LIGHT, 1.0);
}