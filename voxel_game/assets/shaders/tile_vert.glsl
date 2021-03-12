#version 440 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_nrm;
layout(location = 2) in vec2 a_tex;

out vec3 v_pos;
out vec3 v_nrm;
out vec2 v_tex;

uniform mat4 u_view_mat = mat4(1);
uniform mat4 u_proj_mat;

void main() {
    v_pos = a_pos, v_nrm = a_nrm, v_tex = a_tex;
    mat4 proj_view = u_proj_mat * u_view_mat;
    gl_Position = proj_view * vec4(v_pos, 1);
}
