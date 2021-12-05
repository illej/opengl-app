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
va_add_vb_layout (struct vertex_array *vao, struct vb_layout *layout)
{
    unsigned int element_count = layout->element_count;
    unsigned int offset = 0;

    va_bind (vao);

    for (unsigned int i = 0; i < element_count; i++)
    {
        struct vb_element *e = &layout->elements[i];

        vb_bind (e->vbo);

        printf ("enabling vertex attrib array index=%u vbo=%u\n", i, e->vbo->renderer_id);
        printf ("  count=%u type=%u norm=%u stride=%u offset=%u\n",
                e->count, e->type, e->normalised, vb_layout_stride_get (layout), offset);

        GLCALL (glEnableVertexAttribArray (i));
        GLCALL (glVertexAttribPointer (i, e->count, e->type, e->normalised, 0, 0));
        //GLCALL (glVertexAttribPointer (i, e->count, e->type, e->normalised, vb_layout_stride_get (layout),
        //                               (GLvoid *) ((char *) 0 + offset)));

        offset += e->count * vb_element_type_size (e->type);
    }
}

#endif /* VERTEX_ARRAY_H */
