#version 440 core
layout(std140, binding = 0) uniform buf {
    float qt_Opacity;
    vec4 color;
};
layout(location = 0) out vec4 fragColor;
void main() {
    fragColor = color * qt_Opacity;
}
