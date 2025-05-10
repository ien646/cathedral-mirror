vec3 specular(vec3 view_world_pos, vec3 frag_world_pos, vec3 frag_world_normal, float specular_strength, float specularity_coefficient)
{
    vec3 result = vec3(0, 0, 0);
    for(int i = 0; i < ENABLED_POINT_LIGHTS; ++i)
    {
        const vec3 light_dir = normalize(POINT_LIGHTS[i].position - frag_world_pos);
        const vec3 view_dir = normalize(view_world_pos - frag_world_pos);
        const vec3 reflection_dir = reflect(light_dir, frag_world_normal);

        const float distance = abs(distance(frag_world_pos, POINT_LIGHTS[i].position));

        float range_value = max((POINT_LIGHTS[i].range - distance) / POINT_LIGHTS[i].range, 0.0);
        range_value = pow(range_value, POINT_LIGHTS[i].falloff_coefficient);

        float reflection_view_divergence = max(dot(view_dir, reflection_dir), 0.0);
        float specular_factor = pow(reflection_view_divergence * specular_strength, specularity_coefficient);

        result += POINT_LIGHTS[i].color * range_value * specular_factor;
    }
    return result;
}