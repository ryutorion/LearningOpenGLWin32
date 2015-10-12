#version 450

in vec4 position;

uniform mat4 Model;

void main(void)
{
    gl_Position = Model * position;
}
