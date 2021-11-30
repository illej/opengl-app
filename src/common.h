#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <GL/glew.h>

#define ARRAY_LEN(arr) (sizeof ((arr)) / sizeof ((arr)[0]))
#define ASSERT(expr) if (!(expr)) { printf ("Assertion failed at %s(%d): %s\n", __func__, __LINE__, #expr); *(int *) 0 = 0; }

#if DEBUG
  #define GLCALL(expr) expr; ASSERT (gl_check_error (#expr, __FILE__, __LINE__))
#else
  #define GLCALL(expr) expr;
#endif

bool
gl_check_error (char *func, char *file, int line)
{
    bool ok = true;

    GLenum error;
    while ((error = glGetError ()) != GL_NO_ERROR)
    {
        printf ("[OpenGL Error] (%d) %s [%s:%d]\n", error, func, file, line);

        ok = false;
    }

    return ok;
}

#endif /* COMMON_H */
