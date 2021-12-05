#ifndef TEST_CLEAR_COLOUR_H
#define TEST_CLEAR_COLOUR_H

#include "test.h"

void
test_clear_colour_setup (struct test_data *t)
{
    unsigned int i = 0;

    t->colour[i++] = 0.2f; 
    t->colour[i++] = 0.3f; 
    t->colour[i++] = 0.8f; 
    t->colour[i++] = 1.0f; 
}

void
test_clear_colour_update (struct test_data *t, float dt)
{

}

void
test_clear_colour_render (struct test_data *t)
{
    GLCALL (glClearColor (t->colour[0], t->colour[1], t->colour[2], t->colour[3]));
    GLCALL (glClear (GL_COLOR_BUFFER_BIT));
}

void
test_clear_colour_render_gui (struct test_data *t, struct nk_context *ctx)
{
    if (nk_begin (ctx, "Clear Colour", nk_rect (50, 50, 220, 220),
                  NK_WINDOW_MOVABLE | NK_WINDOW_BORDER | NK_WINDOW_CLOSABLE))
    {
        nk_layout_row_begin (ctx, NK_STATIC, 30, 2);
        {
            nk_layout_row_push (ctx, 50);
            nk_label (ctx, "R:", NK_TEXT_LEFT);
            nk_layout_row_push (ctx, 110);
            nk_slider_float (ctx, 0, &t->colour[0], 1.0f, 0.1f);
        }
        nk_layout_row_end (ctx);

        nk_layout_row_begin (ctx, NK_STATIC, 30, 2);
        {
            nk_layout_row_push (ctx, 50);
            nk_label (ctx, "G:", NK_TEXT_LEFT);
            nk_layout_row_push (ctx, 110);
            nk_slider_float (ctx, 0, &t->colour[1], 1.0f, 0.1f);
        }
        nk_layout_row_end (ctx);

        nk_layout_row_begin (ctx, NK_STATIC, 30, 2);
        {
            nk_layout_row_push (ctx, 50);
            nk_label (ctx, "B:", NK_TEXT_LEFT);
            nk_layout_row_push (ctx, 110);
            nk_slider_float (ctx, 0, &t->colour[2], 1.0f, 0.1f);
        }
        nk_layout_row_end (ctx);

        nk_layout_row_begin (ctx, NK_STATIC, 30, 2);
        {
            nk_layout_row_push (ctx, 50);
            nk_label (ctx, "A:", NK_TEXT_LEFT);
            nk_layout_row_push (ctx, 110);
            nk_slider_float (ctx, 0, &t->colour[3], 1.0f, 0.1f);
        }
        nk_layout_row_end (ctx);
    }
    nk_end (ctx);

    nk_sdl_render (NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);
}

void test_clear_colour_teardown (struct test_data *t) {}

#endif /* TEST_CLEAR_COLOUR_H */
