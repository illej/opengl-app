#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <GL/glew.h> // Always include before gl.h and glfw3.h
#include <SDL2/SDL.h>

#define MATH_3D_IMPLEMENTATION
#include <math_3d.h>


static bool g__running = true;
static const float g__triangle_vertices[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f, 1.0f, 0.0f
};
static const float g__cube_vertices[] = {
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
static const float g__cube_colour_data[] = {
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
        buf = malloc (len);
        if (buf)
        {
            fread (buf, 1, len, fp);
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
compile_shader (unsigned int type, char *source)
{
    unsigned int id = glCreateShader (type);
    glShaderSource (id, 1/* count */, &source, NULL/* length */);
    glCompileShader (id);

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

static unsigned int
create_shader (char *vertex_file, char *fragment_file)
{
    unsigned int vert_id = 0;
    unsigned int frag_id = 0;
    unsigned int program_id = 0;
    char *vertex_source = read_file_to_str (vertex_file);
    char *fragment_source = read_file_to_str (fragment_file);

    if (vertex_source && fragment_source)
    {
        vert_id = compile_shader (GL_VERTEX_SHADER, vertex_source);
        frag_id = compile_shader (GL_FRAGMENT_SHADER, fragment_source);
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

    return program_id;
}

static void
handle_os_events (void)
{
    SDL_Event event;

    while (SDL_PollEvent(&event) != 0)
    {
        switch (event.type)
        {
            case SDL_QUIT:
            {
                SDL_Log ("quit received");
                g__running = false;
            } break;
            case SDL_KEYDOWN:
            {
                SDL_Log ("keydown");

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
                SDL_Log("TEXTINPUT event received");
            } break;
            case SDL_TEXTEDITING:
            {
                SDL_Log("TEXTEDITING event received");
            } break;
        }
    }
}

int
main (int argc, char **argv)
{
    int width = 800;
    int height = 600;

    SDL_Init (SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute (SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute (SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute (SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window *window = SDL_CreateWindow ("opengl",
                                           SDL_WINDOWPOS_CENTERED,
                                           SDL_WINDOWPOS_CENTERED,
                                           width, height,
                                           SDL_WINDOW_OPENGL);
    SDL_GLContext *glctx = SDL_GL_CreateContext (window);

    glewInit ();

    printf ("hello opengl (%s)\n", glGetString (GL_VERSION));

    // z sorting
    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LESS);

    // cube vertex buffer
    unsigned int vertex_buffer;
    glGenBuffers (1, &vertex_buffer);
    glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer);
    // glBufferData (GL_ARRAY_BUFFER, sizeof (g__triangle_vertices), g__triangle_vertices, GL_STATIC_DRAW);
    glBufferData (GL_ARRAY_BUFFER, sizeof (g__cube_vertices), g__cube_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray (0);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // colour buffer
    unsigned int colour_buffer;
    glGenBuffers (1, &colour_buffer);
    glBindBuffer (GL_ARRAY_BUFFER, colour_buffer);
    glBufferData (GL_ARRAY_BUFFER, sizeof (g__cube_colour_data), g__cube_colour_data, GL_STATIC_DRAW);

    glEnableVertexAttribArray (1);
    glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // shaders
    unsigned int shader_program_id = create_shader ("data/vertex.shader", "data/fragment.shader");
    unsigned int matrix_id = glGetUniformLocation (shader_program_id, "MVP");

    mat4_t projection = m4_perspective (45.0f, (float) width / (float) height, 0.1f, 100.0f);
    mat4_t view = m4_look_at (vec3 (4, 3, 3),
                              vec3 (0, 0, 0),
                              vec3 (0, 1, 0));
    mat4_t model = m4_identity ();
    mat4_t mvp = m4_mul(m4_mul (projection, view), model);

    glUseProgram (shader_program_id);

    while (g__running)
    {
        handle_os_events ();

        // compute_matrix_from_input ();

        glClearColor (0.0f, 0.15f, 0.3f, 1.0f);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv (matrix_id, 1, GL_FALSE, &mvp.m[0][0]);
        // glDrawArrays (GL_TRIANGLES, 0, sizeof (g__triangle_vertices));
        glDrawArrays (GL_TRIANGLES, 0, sizeof (g__cube_vertices));

        SDL_GL_SwapWindow (window);
    }

    glDeleteProgram (shader_program_id);

    SDL_GL_DeleteContext (glctx);
    SDL_DestroyWindow (window);
    SDL_Quit ();

    return 0;
}