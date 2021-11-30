#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"
#include "vertex_buffer.h"
#include "vertex_array.h"
#include "index_buffer.h"
#include "shader.h"
#include "texture.h"

struct renderer
{
    unsigned int id;
};

static void
renderer_clear (void)
{
    GLCALL (glClearColor (0.0f, 0.15f, 0.3f, 1.0f));
    GLCALL (glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
};

static void
renderer_draw (struct renderer *r, struct vertex_array *vao, struct index_buffer *ibo, struct shader *shader)
{
    shader_bind (shader);
    va_bind (vao);
    ib_bind (ibo);

    GLCALL (glDrawElements (GL_TRIANGLES, ib_count_get (ibo), GL_UNSIGNED_INT, NULL));
}

#endif /* RENDERER_H */
