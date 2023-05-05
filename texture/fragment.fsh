#version 440 core
in vec3 v_color;
in vec2 v_texCoord;

layout (location = 0) uniform sampler2D texWall;
layout (location = 1) uniform sampler2D texFace;

out vec4 fragColor;
void main(void)
{
    fragColor = texture(texWall, v_texCoord) * vec4(v_color, 1);
    fragColor = mix(texture(texFace, v_texCoord), fragColor, 0.6);
}
