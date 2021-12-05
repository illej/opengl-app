#ifndef TEST_H
#define TEST_H

//#include "renderer.h"
//
enum test_type
{
    TEST_INALID = 0,

    TEST_CLEAR_COLOUR,
    TEST_TEXTURE,
    TEST_3D_CUBE,
};

struct test_data
{
    enum test_type type;

    /* CLEAR COLOUR */
    float colour[4];

    /* TEXTURE */
    float vert_data[16];
    unsigned int vert_size;
    unsigned int indices[6];
    unsigned int n_indices;

    struct vertex_array vao;
    struct vertex_buffer vbo;
    struct vb_layout layout;

    struct index_buffer ibo;

    struct shader shader;
    mat4_t mvp;

    struct texture texture;

    struct renderer renderer;

    /* 3D CUBE */
    struct vertex_buffer vert_vbo;
    struct vertex_buffer colour_vbo;
    struct vb_layout layout_vert;
    struct vb_layout layout_colour;
};

void test_setup (enum test_type type, struct test_data *d);
void test_update (struct test_data *t, float dt);
void test_render (struct test_data *t);
void test_render_gui (struct test_data *t, struct nk_context *nk);
void test_teardown (struct test_data *t);

#endif /* TEST_H */
