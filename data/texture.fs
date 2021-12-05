#version 330 core

in vec2 v_texture_coords;
out vec4 colour;

uniform vec4 u_colour;
uniform sampler2D u_texture;

void main()
{
    vec4 texture_colour = texture(u_texture, v_texture_coords);
    colour = texture_colour;
}
