#version 450

in vec4 position;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main(void)
{
    gl_Position = Projection * View * Model * position;
}
