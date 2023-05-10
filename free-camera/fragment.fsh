#version 440 core
in vec2 v_texCoord;

layout (location = 0) uniform sampler2D texWall;
layout (location = 1) uniform sampler2D texFace;

out vec4 fragColor;
void main(void)
{
    fragColor = texture(texFace, v_texCoord);
    fragColor = mix(texture(texWall, v_texCoord), fragColor , fragColor.a);
}
