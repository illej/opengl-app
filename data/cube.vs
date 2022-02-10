#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 in_uv;

out vec2 uv;

uniform mat4 u_mvp;

void main()
{
    gl_Position = u_mvp * position;
    uv = in_uv;
}

