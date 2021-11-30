#ifndef INDEX_BUFFER_H
#define INDEX_BUFFER_H

#include "renderer.h"

struct index_buffer
{
    unsigned int renderer_id;
    unsigned int count;
};

void
ib_create (struct index_buffer *ib, unsigned int *data, unsigned int count)
{
    ASSERT (sizeof (unsigned int) == sizeof (GLuint));

    ib->count = count;
    GLCALL (glGenBuffers (1, &ib->renderer_id));
    GLCALL (glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ib->renderer_id));
    GLCALL (glBufferData (GL_ELEMENT_ARRAY_BUFFER, ib->count * sizeof (unsigned int), data, GL_STATIC_DRAW));
}

void
ib_delete (struct index_buffer *ib)
{
    GLCALL (glDeleteBuffers (1, &ib->renderer_id));
}



void
ib_bind (struct index_buffer *ib)
{
    GLCALL (glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ib->renderer_id));
}

void
ib_unbind (void)
{
    GLCALL (glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0));
}

inline unsigned int
ib_count_get (struct index_buffer *ib)
{
    return ib->count;
}

#endif /* INDEX_BUFFER_H */
