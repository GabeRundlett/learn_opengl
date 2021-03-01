#version 440

layout(location = 0) in vec2 a_pos;

out vec2 v_tex;

void main() {
    v_tex = a_pos * 0.5 + 0.5;
    gl_Position = vec4(a_pos, 0, 1);
}