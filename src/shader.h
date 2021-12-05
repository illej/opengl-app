#ifndef SHADER_H
#define SHADER_H

#include "renderer.h"

struct uniform_location_cache_entry
{
    char name[32];
    int location;
};

struct shader
{
    unsigned int id;
    unsigned int cache_len;
    struct uniform_location_cache_entry uniform_location_cache[256];
};

static char *
read_file_to_str (char *file)
{
    size_t len = 0;
    char *buf = NULL;
    FILE *fp = fopen (file, "r");

    if (fp)
    {
        fseek (fp, 0, SEEK_END);
        len = ftell (fp);
        fseek (fp, 0, SEEK_SET);

        buf = malloc (len + 1);
        memset (buf, 0, len + 1);

        if (buf)
        {
            fread (buf, 1, len, fp);
            buf[len] = '\0';

#if 0
            for (int i = 0; i < len; i++)
            {
                char c = buf[i];
                if (c == '\0') c = '!';

                printf ("[%c]", c);
            }
            printf ("\n");
#endif
        }

        fclose (fp);
    }

    if (!buf)
    {
        printf ("failed to read file \"%s\"\n", file);
    }

    return buf;
}

static unsigned int
shader_compile (unsigned int type, char *source)
{
    GLCALL (unsigned int id = glCreateShader (type));
    GLCALL (glShaderSource (id, 1/* count */, &source, NULL/* length */));
    GLCALL (glCompileShader (id));

    int result;
    glGetShaderiv (id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int len;
        glGetShaderiv (id, GL_INFO_LOG_LENGTH, &len);
        char *message = alloca (len * sizeof (char));
        glGetShaderInfoLog (id, len, NULL, message);

        printf ("failed to compile %s shader: \"%s\"\n",
                (type == GL_VERTEX_SHADER ? "vertex" : "fragment"),
                message);

        glDeleteShader (id);
        id = 0;
    }

    return id;
}

void
shader_create (struct shader *shader, char *vertex_file, char *fragment_file)
{
    unsigned int vert_id = 0;
    unsigned int frag_id = 0;
    unsigned int program_id = 0;
    char *vertex_source = read_file_to_str (vertex_file);
    char *fragment_source = read_file_to_str (fragment_file);

    if (vertex_source && fragment_source)
    {
        vert_id = shader_compile (GL_VERTEX_SHADER, vertex_source);
        frag_id = shader_compile (GL_FRAGMENT_SHADER, fragment_source);
        program_id = glCreateProgram ();

        glAttachShader (program_id, vert_id);
        glAttachShader (program_id, frag_id);
        glLinkProgram (program_id);
        glValidateProgram (program_id);

        int result;
        glGetProgramiv (program_id, GL_LINK_STATUS, &result);
        if (result == GL_FALSE)
        {
            int len;
            glGetProgramiv (program_id, GL_INFO_LOG_LENGTH, &len);
            char *message = alloca (len * sizeof (char));
            glGetProgramInfoLog (program_id, len, NULL, message);

            printf ("failed to link shader program: \"%s\"\n", message);

            program_id = 0;
        }

        glDetachShader (program_id, vert_id);
        glDetachShader (program_id, frag_id);

        glDeleteShader (vert_id);
        glDeleteShader (frag_id);
    }

    if (vertex_source) free (vertex_source);
    if (fragment_source) free (fragment_source);

    shader->id = program_id;

    ASSERT (program_id != 0);
}

void
shader_delete (struct shader *shader)
{
    GLCALL (glDeleteProgram (shader->id));
}

void
shader_bind (struct shader *shader)
{
    GLCALL (glUseProgram (shader->id));
}

void
shader_unbind (void)
{
    GLCALL (glUseProgram (0));
}

static int
shader_get_uniform_location (struct shader *shader, char *name)
{
    int location = -1;
    bool found = false;

    for (unsigned int i = 0; i < shader->cache_len; i++)
    {
        if (strcmp (name, shader->uniform_location_cache[i].name) == 0)
        {
            location = shader->uniform_location_cache[i].location;
            found = true;

            // printf ("cache entry found for %s %d\n", name, location);
            break;
        }
    }

    if (!found)
    {
        GLCALL (int location = glGetUniformLocation (shader->id, name));
        if (location  == -1)
        {
            printf ("Warning: uniform %s does not exist\n", name);
        }

        if (shader->cache_len + 1 < ARRAY_LEN (shader->uniform_location_cache))
        {
//            printf ("cache entry not found for %s\n", name);

            struct uniform_location_cache_entry *e = &shader->uniform_location_cache[shader->cache_len++];
            snprintf (e->name, 32, "%s", name);
            e->location = location;

//            printf ("entry added (%u)\n", shader->cache_len);
        }
        else
        {
            ASSERT (!"uniform location cache full");
        }
    }

    return location;
}

void
shader_set_uniform_4f (struct shader *shader, char *name, float v0, float v1, float v2, float v3)
{
    int location = shader_get_uniform_location (shader, name);
    GLCALL (glUniform4f (location, v0, v1, v2, v3));
}

void
shader_set_uniform_m4f (struct shader *shader, char *name, float *fptr)
{
    int location = shader_get_uniform_location (shader, name);
    GLCALL (glUniformMatrix4fv (location, 1, GL_FALSE, fptr));
}

void
shader_set_uniform_1f (struct shader *shader, char *name, float value)
{
    GLCALL (glUniform1f (shader_get_uniform_location (shader, name), value));
}

void
shader_set_uniform_1i (struct shader *shader, char *name, int value)
{
    GLCALL (glUniform1i (shader_get_uniform_location (shader, name), value));
}

#endif /* SHADER_H */
