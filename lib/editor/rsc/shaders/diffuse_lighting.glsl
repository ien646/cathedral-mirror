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