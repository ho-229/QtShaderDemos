#version 440 core
in vec3 v_color;

out vec4 fragColor;
void main(void)
{
    fragColor = vec4(v_color, 1);
}
