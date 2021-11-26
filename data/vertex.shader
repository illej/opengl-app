#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 colour;
uniform mat4 MVP;

out vec3 fragment_colour;

void main()
{
    gl_Position = MVP * position;
    fragment_colour = colour;
}