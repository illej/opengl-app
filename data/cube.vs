#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 colour;

out vec3 frag_colour;

uniform mat4 u_mvp;

void main()
{
    gl_Position = u_mvp * position;
    frag_colour = colour;
}

