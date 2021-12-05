#ifndef TEST_TEXTURE_H
#define TEST_TEXTURE_H

void test_texture_setup (struct test_data *t)
{
    unsigned int i = 0;

    t->vert_data[i++] = -0.5f; // bottom-left of view (x, y)
    t->vert_data[i++] = -0.5f;
    t->vert_data[i++] = 0.0f; // bottom-left of texture (u, v)
    t->vert_data[i++] = 0.0f;
    t->vert_data[i++] = 0.5f; // bottom-right (x, y)
    t->vert_data[i++] = -0.5f;
    t->vert_data[i++] = 1.0f; // bottom-right (u, v)
    t->vert_data[i++] = 0.0f;
    t->vert_data[i++] = 0.5f; // top-right (x, y)
    t->vert_data[i++] = 0.5f;
    t->vert_data[i++] = 1.0f; // top-right (u, v)
    t->vert_data[i++] = 1.0f;
    t->vert_data[i++] = -0.5f; // top-left (x, y)
    t->vert_data[i++] = 0.5f;
    t->vert_data[i++] = 0.0f; // top-left (u, v)
    t->vert_data[i++] = 1.0f;

    t->vert_size = i * sizeof (float);

    va_create (&t->vao);
    vb_create (&t->vbo, t->vert_data, t->vert_size);
    vb_layout_create (&t->layout);
    vb_layout_push_f (&t->layout, &t->vbo,2); // xy
    vb_layout_push_f (&t->layout, &t->vbo, 2); // uv
    va_add_vb_layout (&t->vao, &t->layout);

    t->n_indices = 0;
    t->indices[t->n_indices++] = 0;
    t->indices[t->n_indices++] = 1;
    t->indices[t->n_indices++] = 2;
    t->indices[t->n_indices++] = 2;
    t->indices[t->n_indices++] = 3;
    t->indices[t->n_indices++] = 0;
    ib_create (&t->ibo, t->indices, t->n_indices);

    shader_create (&t->shader, "data/texture.vs", "data/texture.fs");
    shader_bind (&t->shader);

    t->mvp = m4_ortho (-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
    shader_set_uniform_m4f (&t->shader, "u_mvp", &t->mvp.m[0][0]);

    texture_create (&t->texture, "data/tifa.png");
    texture_bind (&t->texture, 0);
    shader_set_uniform_1i (&t->shader, "u_texture", 0);
}

void test_texture_update (struct test_data *t, float dt) {}

void test_texture_render (struct test_data *t)
{
    renderer_clear ();

    shader_bind (&t->shader);
    shader_set_uniform_m4f (&t->shader, "u_mvp", &t->mvp.m[0][0]);

    renderer_draw (&t->renderer, &t->vao, &t->ibo, &t->shader);
}

void test_texture_render_gui (struct test_data *t, struct nk_context *nk) {}

void
test_texture_teardown (struct test_data *t)
{
    shader_delete (&t->shader);
}

#endif /* TEST_TEXTURE_H */
