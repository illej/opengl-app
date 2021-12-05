#include "test_clear_colour.h"
#include "test_texture.h"
#include "test_3d_cube.h"

// TODO: may be able to put this back into test.h??? do we want to??

void
test_setup (enum test_type type, struct test_data *d)
{
    d->type = type;

    switch (d->type)
    {
        case TEST_CLEAR_COLOUR: { test_clear_colour_setup (d); break; }
        case TEST_TEXTURE:      { test_texture_setup (d); break; }
        case TEST_3D_CUBE:      { test_3d_cube_setup (d); break; }
        default:
            ASSERT (!"unknown setup type");
    }
}

void
test_update (struct test_data *t, float dt)
{

}

void
test_render (struct test_data *t)
{
    switch (t->type)
    {
        case TEST_CLEAR_COLOUR: { test_clear_colour_render (t); break; }
        case TEST_TEXTURE:      { test_texture_render (t); break; }
        case TEST_3D_CUBE:      { test_3d_cube_render (t); break; }
        default:
            ASSERT (!"unknown render type");
    }
}

void
test_render_gui (struct test_data *t, struct nk_context *nk)
{
    switch (t->type)
    {
        case TEST_CLEAR_COLOUR: { test_clear_colour_render_gui (t, nk); break; }
        case TEST_TEXTURE:      { test_texture_render_gui (t, nk); break; }
        case TEST_3D_CUBE:      { test_3d_cube_render_gui (t, nk); break; }
        default:
            ASSERT (!"unknown render gui type");
    }
}

void
test_teardown (struct test_data *t)
{
    switch (t->type)
    {
        case TEST_CLEAR_COLOUR: { test_clear_colour_teardown (t); break; }
        case TEST_TEXTURE:      { test_texture_teardown (t); break; }
        case TEST_3D_CUBE:      { test_3d_cube_teardown (t); break; }
        default:
            ASSERT (!"unknown teardown type");
    }
}
