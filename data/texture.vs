#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texture_coords;

out vec2 v_texture_coords;

uniform mat4 u_mvp;

void main()
{
    gl_Position = u_mvp * position;
    v_texture_coords = texture_coords;
}

