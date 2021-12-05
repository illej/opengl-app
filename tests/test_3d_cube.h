#ifndef TEST_3D_CUBE_H
#define TEST_3D_CUBE_H

static float g_cube_vert_data[] = {
    -1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
};

static float g_cube_colour_data[] = {
    0.583f,  0.771f,  0.014f,
    0.609f,  0.115f,  0.436f,
    0.327f,  0.483f,  0.844f,
    0.822f,  0.569f,  0.201f,
    0.435f,  0.602f,  0.223f,
    0.310f,  0.747f,  0.185f,
    0.597f,  0.770f,  0.761f,
    0.559f,  0.436f,  0.730f,
    0.359f,  0.583f,  0.152f,
    0.483f,  0.596f,  0.789f,
    0.559f,  0.861f,  0.639f,
    0.195f,  0.548f,  0.859f,
    0.014f,  0.184f,  0.576f,
    0.771f,  0.328f,  0.970f,
    0.406f,  0.615f,  0.116f,
    0.676f,  0.977f,  0.133f,
    0.971f,  0.572f,  0.833f,
    0.140f,  0.616f,  0.489f,
    0.997f,  0.513f,  0.064f,
    0.945f,  0.719f,  0.592f,
    0.543f,  0.021f,  0.978f,
    0.279f,  0.317f,  0.505f,
    0.167f,  0.620f,  0.077f,
    0.347f,  0.857f,  0.137f,
    0.055f,  0.953f,  0.042f,
    0.714f,  0.505f,  0.345f,
    0.783f,  0.290f,  0.734f,
    0.722f,  0.645f,  0.174f,
    0.302f,  0.455f,  0.848f,
    0.225f,  0.587f,  0.040f,
    0.517f,  0.713f,  0.338f,
    0.053f,  0.959f,  0.120f,
    0.393f,  0.621f,  0.362f,
    0.673f,  0.211f,  0.457f,
    0.820f,  0.883f,  0.371f,
    0.982f,  0.099f,  0.879f
};

void test_3d_cube_setup (struct test_data *t)
{
    va_create (&t->vao);
    vb_layout_create (&t->layout);

    vb_create (&t->vert_vbo, g_cube_vert_data, sizeof (g_cube_vert_data));
    printf ("vert_vbo id=%u\n", t->vert_vbo.renderer_id);
    vb_layout_push_f (&t->layout, &t->vert_vbo, 3); // xyz

    vb_create (&t->colour_vbo, g_cube_colour_data, sizeof (g_cube_colour_data));
    printf ("colour_vbo id=%u\n", t->colour_vbo.renderer_id);
    vb_layout_push_f (&t->layout, &t->colour_vbo, 3); // rgb

    va_add_vb_layout (&t->vao, &t->layout);

    shader_create (&t->shader, "data/cube.vs", "data/cube.fs");
    shader_bind (&t->shader);

    mat4_t projection = m4_perspective (45.0f, 800.0f / 600.0f, 0.01f, 100.0f);
    mat4_t view = m4_look_at (vec3 (4, 3, 3),
                              vec3 (0, 0, 0),
                              vec3 (0, 1, 0));
    mat4_t model = m4_identity ();
    t->mvp = m4_mul (m4_mul (projection, view), model);
    shader_set_uniform_m4f (&t->shader, "u_mvp", &t->mvp.m[0][0]);
}

void test_3d_cube_update (struct test_data *t, float dt) {}

void test_3d_cube_render (struct test_data *t)
{
    renderer_clear ();

    shader_bind (&t->shader);
    shader_set_uniform_m4f (&t->shader, "u_mvp", &t->mvp.m[0][0]);

    va_bind (&t->vao);
    GLCALL (glDrawArrays (GL_TRIANGLES, 0, 12 * 3));
}

void test_3d_cube_render_gui (struct test_data *t, struct nk_context *nk) {}

void
test_3d_cube_teardown (struct test_data *t)
{
    shader_delete (&t->shader);
}

#endif /* TEST_3D_CUBE_H */
