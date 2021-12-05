#version 330 core

in vec3 frag_colour; 

out vec3 colour;

void main()
{
    colour = frag_colour;
}
