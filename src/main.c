#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <GL/glew.h> // Always include before gl.h and glfw3.h
#include <SDL2/SDL.h>

#define MATH_3D_IMPLEMENTATION
#include <math_3d.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_GL3_IMPLEMENTATION
#include <nuklear.h>
#include <nuklear_sdl_gl3.h>

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

#include "renderer.h"
#include <test.h>
#include <test.c>

/*
 * TODO: replace 'unsigned int' with 'u32'
 */

static bool g__running = true;
static float g__triangle_vertices[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f, 1.0f, 0.0f
};

struct cube
{
    struct shader shader;
    mat4_t mvp;
    GLuint triangle_count;
    GLuint vao;
    GLuint vert_vbo;
    GLuint colour_vbo;
};

static GLfloat cube_verts[] = {
    -1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
};
static GLfloat cube_colours[] = {
    0.583f,  0.771f,  0.014f,
    0.609f,  0.115f,  0.436f,
    0.327f,  0.483f,  0.844f,
    0.822f,  0.569f,  0.201f,
    0.435f,  0.602f,  0.223f,
    0.310f,  0.747f,  0.185f,
    0.597f,  0.770f,  0.761f,
    0.559f,  0.436f,  0.730f,
    0.359f,  0.583f,  0.152f,
    0.483f,  0.596f,  0.789f,
    0.559f,  0.861f,  0.639f,
    0.195f,  0.548f,  0.859f,
    0.014f,  0.184f,  0.576f,
    0.771f,  0.328f,  0.970f,
    0.406f,  0.615f,  0.116f,
    0.676f,  0.977f,  0.133f,
    0.971f,  0.572f,  0.833f,
    0.140f,  0.616f,  0.489f,
    0.997f,  0.513f,  0.064f,
    0.945f,  0.719f,  0.592f,
    0.543f,  0.021f,  0.978f,
    0.279f,  0.317f,  0.505f,
    0.167f,  0.620f,  0.077f,
    0.347f,  0.857f,  0.137f,
    0.055f,  0.953f,  0.042f,
    0.714f,  0.505f,  0.345f,
    0.783f,  0.290f,  0.734f,
    0.722f,  0.645f,  0.174f,
    0.302f,  0.455f,  0.848f,
    0.225f,  0.587f,  0.040f,
    0.517f,  0.713f,  0.338f,
    0.053f,  0.959f,  0.120f,
    0.393f,  0.621f,  0.362f,
    0.673f,  0.211f,  0.457f,
    0.820f,  0.883f,  0.371f,
    0.982f,  0.099f,  0.879f
};

static void
cube_3d_setup (struct cube *cube, int width, int height)
{
    printf ("creating vao\n");
    GLuint vao;
    GLCALL (glGenVertexArrays (1, &vao));
    GLCALL (glBindVertexArray (vao));
    cube->vao = vao;

    printf ("creating vert vbo\n");
    GLuint vert_vbo;
    GLCALL (glGenBuffers (1, &vert_vbo));
    GLCALL (glBindBuffer (GL_ARRAY_BUFFER, vert_vbo));
    GLCALL (glBufferData (GL_ARRAY_BUFFER, sizeof (cube_verts), cube_verts, GL_STATIC_DRAW));
    cube->vert_vbo = vert_vbo;

    printf ("creating colour vbo\n");
    GLuint colour_vbo;
    GLCALL (glGenBuffers (1, &colour_vbo));
    GLCALL (glBindBuffer (GL_ARRAY_BUFFER, colour_vbo));
    GLCALL (glBufferData (GL_ARRAY_BUFFER, sizeof (cube_colours), cube_colours, GL_STATIC_DRAW));
    cube->colour_vbo = colour_vbo;

    printf ("enabling vao 0\n");
    GLCALL (glEnableVertexAttribArray (0));
    GLCALL (glBindBuffer (GL_ARRAY_BUFFER, vert_vbo));
    GLCALL (glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, 0));

    printf ("enabling vao 1\n");
    GLCALL (glEnableVertexAttribArray (1));
    GLCALL (glBindBuffer (GL_ARRAY_BUFFER, colour_vbo));
    GLCALL (glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, 0));

    printf ("creating shader\n");
    shader_create (&cube->shader, "data/cube.vs", "data/cube.fs");
    shader_bind (&cube->shader);

    printf ("creating 3d projection\n");
    mat4_t projection = m4_perspective (45.0f, (float) width / (float) height, 0.1f, 100.0f);
    mat4_t view = m4_look_at (vec3 (4, 3, -3),
                              vec3 (0, 0, 0),
                              vec3 (0, 1, 0));
    mat4_t model = m4_identity ();
    mat4_t mvp = m4_mul(m4_mul (projection, view), model);

    printf ("setting uniform\n");
    shader_set_uniform_m4f (&cube->shader, "u_mvp", &mvp.m[0][0]);
    cube->mvp = mvp;

    cube->triangle_count = 12 * 3;
}

static void
cube_3d_render (struct cube *cube)
{
    GLCALL (glEnable (GL_DEPTH_TEST));
    GLCALL (glDepthFunc (GL_LESS));
    GLCALL (glClearColor (0.1f, 0.1f, 0.1f, 1.0f));
    GLCALL (glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    GLCALL (glBindVertexArray (cube->vao));
    shader_bind (&cube->shader);
    shader_set_uniform_m4f (&cube->shader, "u_mvp", &cube->mvp.m[0][0]);

    GLCALL (glDrawArrays (GL_TRIANGLES, 0, cube->triangle_count));

    GLCALL (glDisable (GL_DEPTH_TEST));
}

static void
handle_os_events (struct nk_context *nk)
{
    SDL_Event event;

   nk_input_begin (nk);
    while (SDL_PollEvent(&event) != 0)
    {
        switch (event.type)
        {
            case SDL_QUIT:
            {
                // SDL_Log ("quit received");
                g__running = false;
            } break;
            case SDL_KEYDOWN:
            {
                // SDL_Log ("keydown");

                SDL_Keycode code = event.key.keysym.sym;
                if (code == SDLK_ESCAPE)
                {
                    g__running = false;
                }
            } break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEMOTION:
            {
                // SDL_Log ("mouse input");
            } break;
            case SDL_TEXTINPUT:
            {
                // SDL_Log("TEXTINPUT event received");
            } break;
            case SDL_TEXTEDITING:
            {
                // SDL_Log("TEXTEDITING event received");
            } break;
        }

       nk_sdl_handle_event (&event);
    }
   nk_input_end (nk);
}

int
main (int argc, char **argv)
{
    int width = 800;
    int height = 600;

    /* Init SDL */
    SDL_Init (SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute (SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute (SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute (SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window *window = SDL_CreateWindow ("opengl-app",
                                           SDL_WINDOWPOS_CENTERED,
                                           SDL_WINDOWPOS_CENTERED,
                                           width, height,
                                           SDL_WINDOW_OPENGL);
    SDL_GLContext *glctx = SDL_GL_CreateContext (window);
    SDL_GL_SetSwapInterval (1); // vsync

    /* Init OpenGL */
    ASSERT (glewInit () == GLEW_OK);

    /* Init GUI */
    struct nk_context *nkctx = nk_sdl_init (window);
    struct nk_colorf bg;
    {
        struct nk_font_atlas *atlas;
        nk_sdl_font_stash_begin (&atlas);
        nk_sdl_font_stash_end ();
        bg.r = 0.10f;
        bg.g = 0.18f;
        bg.b = 0.24f;
        bg.a = 1.0f;
    }

    printf ("hello opengl (%s)\n", glGetString (GL_VERSION));

    struct cube cube = {0};
    cube_3d_setup (&cube, width, height);

    while (g__running)
    {
        handle_os_events (nkctx);

        cube_3d_render (&cube);

        SDL_GL_SwapWindow (window);
    }

    nk_sdl_shutdown ();
    SDL_GL_DeleteContext (glctx);
    SDL_DestroyWindow (window);
    SDL_Quit ();

    return 0;
}
