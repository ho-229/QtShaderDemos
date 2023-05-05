#version 440 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

out vec2 v_texCoord;
out vec3 v_color;
void main(void)
{
    v_color = color;
    v_texCoord = texCoord;
    gl_Position = vec4(pos, 0, 1.0);
}
