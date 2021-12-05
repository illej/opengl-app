#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include "renderer.h"

struct vertex_buffer
{
    unsigned int renderer_id;
};

struct vb_element
{
    struct vertex_buffer *vbo; // TODO: this is bad lol
    unsigned int count;
    unsigned int type;
    unsigned char normalised;
};

struct vb_layout
{
    unsigned int element_count;
    struct vb_element elements[256];
    unsigned int stride;
};

unsigned int
vb_element_type_size (unsigned int type)
{
    switch (type)
    {
        case GL_FLOAT: return 4;
        case GL_UNSIGNED_INT: return 4;
        case GL_UNSIGNED_BYTE: return 1;
        default:
            ASSERT (!"unknown type");
            return 0;
    }
}

void
vb_layout_create (struct vb_layout *layout)
{
    layout->element_count = 0;
    layout->stride = 0;
}

static bool
__vb_layout_push (struct vb_layout *layout, struct vertex_buffer *vbo, unsigned int count, unsigned int type, unsigned char normalised)
{
    bool ok = false;

    if (layout->element_count + 1 < ARRAY_LEN (layout->elements))
    {
        struct vb_element *e = &layout->elements[layout->element_count++];
        e->vbo = vbo;
        e->count = count;
        e->type = type;
        e->normalised = normalised;

        ok = true;
    }
    else
    {
        ASSERT (!"max vb elements reached");
    }

    return ok;
}

void
vb_layout_push_f (struct vb_layout *layout, struct vertex_buffer *vbo, unsigned int count)
{
    if (__vb_layout_push (layout, vbo, count, GL_FLOAT, GL_FALSE))
    {
        layout->stride += vb_element_type_size (GL_FLOAT) * count;
    }
}

void
vb_layout_push_uint (struct vb_layout *layout, unsigned int count)
{
    if (__vb_layout_push (layout, 0, count, GL_UNSIGNED_INT, GL_FALSE))
    {
        layout->stride += vb_element_type_size (GL_UNSIGNED_INT) * count;
    }
}

void
vb_layout_push_uchar (struct vb_layout *layout, unsigned int count)
{
    if (__vb_layout_push (layout, 0, count, GL_UNSIGNED_BYTE, GL_TRUE))
    {
        layout->stride += vb_element_type_size (GL_UNSIGNED_BYTE) * count;
    }
}

unsigned int
vb_layout_stride_get (struct vb_layout *layout)
{
    return layout->stride;
}

void
vb_create (struct vertex_buffer *vb, void *data, unsigned int len)
{
    GLCALL (glGenBuffers (1, &vb->renderer_id));
    GLCALL (glBindBuffer (GL_ARRAY_BUFFER, vb->renderer_id));
    GLCALL (glBufferData (GL_ARRAY_BUFFER, len, data, GL_STATIC_DRAW));
}

void
vb_delete (struct vertex_buffer *vb)
{
    GLCALL (glDeleteBuffers (1, &vb->renderer_id));
}

void
vb_bind (struct vertex_buffer *vb)
{
    GLCALL (glBindBuffer (GL_ARRAY_BUFFER, vb->renderer_id));
}

void
vb_unbind (void)
{
    GLCALL (glBindBuffer (GL_ARRAY_BUFFER, 0));
}

#endif /* VERTEX_BUFFER_H */
