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

#define WIDTH 800
#define HEIGHT 600
#define DT 1000.0f / 60.0f
#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024
#define kilobytes(x) ((x) * 1024)
#define megabytes(x) (kilobytes(x) * 1024)
#define gigabytes(x) (megabytes(x) * 1024)
#define terabytes(x) (gigabytes(x) * 1024)

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
    GLuint triangle_count;
    GLuint vao;
    GLuint vert_vbo;
    GLuint colour_vbo;
};

typedef struct
{
    float x, y;
} vec2_t;
static vec2_t vec2 (float x, float y) { return (vec2_t) { x, y }; }

typedef struct
{
    unsigned int x, y, z;
} vec3u_t;
static vec3u_t vec3u (unsigned int x, unsigned int y, unsigned int z) { return (vec3u_t) { x, y, z }; }

struct input
{
    vec3_t position;

    float horizontal_angle;
    float vertical_angle;

    float initial_fov;

    float speed;
    float mouse_speed;

    vec3_t direction;
    vec3_t right;
    vec3_t up;

    mat4_t projection;
    mat4_t model;
    mat4_t view;
};


#define OBJ_MAX megabytes (6)
unsigned int vertex_indices[OBJ_MAX];
unsigned int uv_indices[OBJ_MAX];
unsigned int normal_indices[OBJ_MAX];

unsigned int vert_count = 0;
unsigned int uv_count = 0;
unsigned int normal_count = 0;

vec3_t temp_vertices[OBJ_MAX];
vec2_t temp_uvs[OBJ_MAX];
vec3_t temp_normals[OBJ_MAX];

unsigned int temp_vertex_count = 0;
unsigned int temp_uv_count = 0;
unsigned int temp_normal_count = 0;

vec3_t final_vertices[OBJ_MAX];
vec2_t final_uvs[OBJ_MAX];
vec3_t final_normals[OBJ_MAX];

unsigned int final_vert_count = 0;
unsigned int final_uv_count = 0;
unsigned int final_normal_count = 0;

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
handle_input (SDL_Window *window, struct input *input, SDL_Keycode key, float dt)
{

    int x = 0;
    int y = 0;

    SDL_GetMouseState (&x, &y);
    SDL_WarpMouseInWindow (window, WIDTH / 2, HEIGHT / 2);

    input->horizontal_angle += input->mouse_speed * dt * (float) (WIDTH / 2 - x);
    input->vertical_angle += input->mouse_speed * dt * (float) (HEIGHT / 2 - y);

    input->direction = vec3 (cos (input->vertical_angle) * sin (input->horizontal_angle),
                             sin (input->vertical_angle),
                             cos (input->vertical_angle) * cos (input->horizontal_angle));
    input->right = vec3 (sin (input->horizontal_angle - 3.14f / 2.0f),
                         0,
                         cos (input->horizontal_angle - 3.14f / 2.0f));
    input->up = v3_cross (input->right, input->direction);

    float step = dt * input->speed;
    if (key == SDLK_w)
    {
        input->position = v3_add(input->position, v3_muls (input->direction, step));
    }
    if (key == SDLK_s)
    {
        input->position = v3_sub(input->position, v3_muls (input->direction, step));
    }
    if (key == SDLK_a)
    {
        input->position = v3_sub(input->position, v3_muls (input->right, step));
    }
    if (key == SDLK_d)
    {
        input->position = v3_add(input->position, v3_muls (input->right, step));
    }
}

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
    GLCALL (glBufferData (GL_ARRAY_BUFFER, final_vert_count * sizeof (vec3_t), final_vertices, GL_STATIC_DRAW));
    cube->vert_vbo = vert_vbo;

    printf ("creating colour vbo\n");
    GLuint colour_vbo;
    GLCALL (glGenBuffers (1, &colour_vbo));
    GLCALL (glBindBuffer (GL_ARRAY_BUFFER, colour_vbo));
    GLCALL (glBufferData (GL_ARRAY_BUFFER, final_uv_count * sizeof (vec2_t), final_uvs, GL_STATIC_DRAW));
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

    cube->triangle_count = 12 * 3;
}

static void
cube_3d_render (struct cube *cube, struct input *input)
{
    GLCALL (glEnable (GL_DEPTH_TEST));
    GLCALL (glDepthFunc (GL_LESS));
    GLCALL (glEnable (GL_CULL_FACE));
    GLCALL (glClearColor (0.1f, 0.1f, 0.1f, 1.0f));
    GLCALL (glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    GLCALL (glBindVertexArray (cube->vao));
    shader_bind (&cube->shader);

    mat4_t projection = m4_perspective (45.0f, (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);
    mat4_t view = m4_look_at (input->position,
                              v3_add (input->position, input->direction),
                              input->up);
    mat4_t model = m4_identity ();
    mat4_t mvp = m4_mul(m4_mul (projection, view), model);

    shader_set_uniform_m4f (&cube->shader, "u_mvp", &mvp.m[0][0]);

    GLCALL (glDrawArrays (GL_TRIANGLES, 0, cube->triangle_count));

    GLCALL (glDisable (GL_DEPTH_TEST));
}

static void
handle_os_events (struct nk_context *nk, SDL_Keycode *key)
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
                SDL_Keycode code = event.key.keysym.sym;

                // SDL_Log ("keydown: %d", code);

                if (code == SDLK_ESCAPE)
                {
                    g__running = false;
                }
                *key = code;
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

static void
load_obj (char *file)
{
    FILE *fp = fopen (file, "r");
    if (fp)
    {
        while (true)
        {
            char word[256];

            int res = fscanf (fp, "%s", word);
            if (res == EOF) break;

            if (strcmp (word, "v") == 0)
            {
                vec3_t v;

                fscanf (fp, "%f %f %f\n", &v.x, &v.y, &v.z);

                ASSERT (temp_vertex_count + 1 < ARRAY_LEN (temp_vertices));
                temp_vertices[temp_vertex_count++] = v;
            }
            else if (strcmp (word, "vt") == 0)
            {
                vec2_t v;

                fscanf (fp, "%f %f\n", &v.x, &v.y);

                ASSERT (temp_uv_count + 1 < ARRAY_LEN (temp_uvs));
                temp_uvs[temp_uv_count++] = v;
            }
            else if (strcmp (word, "vn") == 0)
            {
                vec3_t v;

                fscanf (fp, "%f %f %f\n", &v.x, &v.y, &v.z);

                ASSERT (temp_normal_count + 1 < ARRAY_LEN (temp_normals));
                temp_normals[temp_normal_count++] = v;
            }
            else if (strcmp (word, "f") == 0)
            {
                unsigned int vert_index[3];
                unsigned int uv_index[3];
                unsigned int normal_index[3];

                int matches = fscanf (fp, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                                      &vert_index[0], &uv_index[0], &normal_index[0],
                                      &vert_index[1], &uv_index[1], &normal_index[1],
                                      &vert_index[2], &uv_index[2], &normal_index[2]);
                ASSERT (matches == 9);

                if (vert_count + 1 >= ARRAY_LEN (vertex_indices))
                {
                    printf ("reached max vert_count: %u\n", vert_count);
                    break;
                }

                if (uv_count + 1 >= ARRAY_LEN (uv_indices))
                {
                    printf ("reached max uv_count: %u\n", uv_count);
                    break;
                }
                if (normal_count + 1 >= ARRAY_LEN (normal_indices))
                {
                    printf ("reach max normal_count: %u\n", normal_count);
                    break;
                }

                vertex_indices[vert_count++] = vert_index[0];
                vertex_indices[vert_count++] = vert_index[1];
                vertex_indices[vert_count++] = vert_index[2];

                uv_indices[uv_count++] = uv_index[0];
                uv_indices[uv_count++] = uv_index[1];
                uv_indices[uv_count++] = uv_index[2];

                normal_indices[normal_count++] = normal_index[0];
                normal_indices[normal_count++] = normal_index[1];
                normal_indices[normal_count++] = normal_index[2];
            }
        }

        printf ("finished reading\n", file);
        printf ("  vert_count   : %u\n", vert_count);
        printf ("  uv_count     : %u\n", uv_count);
        printf ("  normal_count : %u\n", normal_count);

        printf ("parsing data\n");

        for (unsigned int i = 0; i < vert_count; i++)
        {
            unsigned int vert_index = vertex_indices[i];
            vec3_t vertex = temp_vertices[vert_index - 1];

            // printf ("  adding [%u] %f %f %f\n", i, vertex.x, vertex.y, vertex.z);
            if (final_vert_count + 1 >= ARRAY_LEN (final_vertices))
            {
                printf ("max final verts reached: %u\n", final_vert_count);
                ASSERT (!"blah verts");
            }

            final_vertices[final_vert_count++] = vertex;
        }

        for (unsigned int i = 0; i < uv_count; i++)
        {
            unsigned int uv_index = uv_indices[i];
            vec2_t uv = temp_uvs[uv_index - 1];

            final_uvs[final_uv_count++] = uv;
        }

        for (unsigned int i = 0; i < normal_count; i++)
        {
            unsigned int normal_index = normal_indices[i];
            vec3_t normal = temp_normals[normal_index - 1];

            final_normals[final_normal_count++] = normal;
        }

        printf ("finished parsing\n");
        printf ("  final_vert_count   : %u\n", final_vert_count);
        printf ("  final_uv_count     : %u\n", final_uv_count);
        printf ("  final_normal_count : %u\n", final_normal_count);

        fclose (fp);
    }
    else
    {
        ASSERT (!"failed to read file");
    }
}

int
main (int argc, char **argv)
{
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
                                           WIDTH, HEIGHT,
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

    struct input input = {0};
    input.position = vec3 (0, 0, 5);
    input.horizontal_angle = 3.14f;
    input.initial_fov = 45.0f;
    input.speed = 0.005f;
    input.mouse_speed = 0.0005f;

    load_obj ("data/cube.obj");
    struct cube cube = {0};
    cube_3d_setup (&cube, WIDTH, HEIGHT);

    while (g__running)
    {
        SDL_Keycode key = 0;

        handle_os_events (nkctx, &key);
        handle_input (window, &input, key, DT);
        cube_3d_render (&cube, &input);

        SDL_GL_SwapWindow (window);
    }

    nk_sdl_shutdown ();
    SDL_GL_DeleteContext (glctx);
    SDL_DestroyWindow (window);
    SDL_Quit ();

    return 0;
}
