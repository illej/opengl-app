#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include "renderer.h"

struct vertex_array
{
    unsigned int renderer_id;
};

static void
va_create (struct vertex_array *vao)
{
    GLCALL (glGenVertexArrays (1, &vao->renderer_id));
}

static void
va_delete (struct vertex_array *vao)
{
    GLCALL (glDeleteVertexArrays (1, &vao->renderer_id));
}

static void
va_bind (struct vertex_array *vao)
{
    GLCALL (glBindVertexArray (vao->renderer_id));
}

static void
va_unbind (void)
{
    GLCALL (glBindVertexArray (0));
}

static void
va_add_vb_layout (struct vertex_array *vao, struct vertex_buffer *vbo, struct vb_layout *layout)
{
    unsigned int element_count = layout->element_count;
    unsigned int offset = 0;

    va_bind (vao);
    vb_bind (vbo);

    for (unsigned int i = 0; i < element_count; i++)
    {
        struct vb_element *e = &layout->elements[i];

        GLCALL (glEnableVertexAttribArray (i));
        GLCALL (glVertexAttribPointer (i, e->count, e->type, e->normalised, vb_layout_stride_get (layout),
                                       (GLvoid *) ((char *) 0 + offset)));

        offset += e->count * vb_element_type_size (e->type);
    }
}

#endif /* VERTEX_ARRAY_H */
