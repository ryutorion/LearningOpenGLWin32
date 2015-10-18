#version 450

in vec4 position;

layout(std140) uniform MVP {
    mat4 Model;
    mat4 View;
    mat4 Projection;
};

void main(void)
{
    gl_Position = Projection * View * Model * position;
}
