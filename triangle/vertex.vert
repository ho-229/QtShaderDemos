#version 440
const vec2 vertexs[4] = {
    vec2(0, 0.5),
    vec2(-0.5, -0.5),
    vec2(0, -0.5),
    vec2(0.5, -0.5),
};

void main() {
    gl_Position = vec4(vertexs[gl_VertexIndex], 0, 1);
}
