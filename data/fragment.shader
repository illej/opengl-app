#version 330 core

in vec3 fragment_colour;
out vec3 colour;

void main()
{
    colour = fragment_colour;
}