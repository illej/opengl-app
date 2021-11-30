#ifndef TEXTURE_H
#define TEXTURE_H

struct texture
{
    unsigned int id;

    unsigned char *data;
    int width;
    int height;
    int bytes_per_pixel;
};

static void
texture_create (struct texture *tex, char *file)
{
    stbi_set_flip_vertically_on_load (1);

    tex->data = stbi_load (file, &tex->width, &tex->height, &tex->bytes_per_pixel, 4);
    if (tex->data)
    {
        GLCALL (glGenTextures (1, &tex->id));
        GLCALL (glBindTexture (GL_TEXTURE_2D, tex->id));

        GLCALL (glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL (glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCALL (glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCALL (glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

        GLCALL (glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->data));
        GLCALL (glBindTexture (GL_TEXTURE_2D, 0));

        stbi_image_free (tex->data);
    }
    else
    {
        printf ("failed to load image \"%s\"\n", file);
        ASSERT (!"failed to texture");
    }
}

static void
texture_delete (struct texture *tex)
{
    GLCALL (glDeleteTextures (1, &tex->id));
}

void
texture_bind (struct texture *tex, unsigned int slot)
{
    GLCALL (glActiveTexture (GL_TEXTURE0 + slot));
    GLCALL (glBindTexture (GL_TEXTURE_2D, tex->id));
}

texture_unbind (void)
{
    GLCALL (glBindTexture (GL_TEXTURE_2D, 0));
}

#endif

