#version 430 core

layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec2 a_size;
layout (location = 2) in vec2 a_tex;
layout (location = 3) in vec4 a_col;

uniform mat4 u_view_mat;

out vec2 v_tex;
out vec2 v_size;
out vec4 v_col;

void main() {
    v_tex = a_tex, v_size = a_size, v_col = a_col;
    gl_Position = u_view_mat * vec4(a_pos, 0, 1.0);
}
