#version 440 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec3 color;

out vec3 v_color;
void main(void)
{
    v_color = color;
    gl_Position = vec4(pos, 0, 1.0);
}
