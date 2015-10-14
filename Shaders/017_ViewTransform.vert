#version 450

in vec4 position;

uniform mat4 Model;
uniform mat4 View;

void main(void)
{
    gl_Position = View * Model * position;
}
