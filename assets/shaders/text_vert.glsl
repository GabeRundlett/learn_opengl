#version 430 core

layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec2 a_tex;
layout (location = 2) in vec4 a_col;
layout (location = 3) in float a_font_size;

uniform mat4 u_view_mat;

out vec2 v_tex;
out vec4 v_col;
out float v_font_size;

void main() {
    v_tex = a_tex, v_col = a_col, v_font_size = a_font_size;
    gl_Position = u_view_mat * vec4(a_pos, 0, 1.0);
}
