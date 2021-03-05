#version 440 core

layout(location = 0) in vec2 a_pos;

out vec2 v_pos;

void main() {
    v_pos = a_pos + vec2(0.5);
    gl_Position = vec4(a_pos, 0, 1);
}
