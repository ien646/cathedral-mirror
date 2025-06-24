mat4 get_normal_matrix(mat4 model)
{
    return inverse(transpose(model));
}