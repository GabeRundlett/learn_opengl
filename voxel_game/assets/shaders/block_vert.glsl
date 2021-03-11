#version 440 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_nrm;
layout(location = 2) in vec2 a_tex;

out vec4 v_pos;
out vec3 v_nrm;
out vec2 v_tex;

uniform mat4 u_view_mat = mat4(1);
uniform mat4 u_proj_mat = mat4(1);

void main() {
    v_pos = vec4(a_pos, 1);
    v_nrm = a_nrm;
    v_tex = vec2(a_tex.x, a_tex.y) * 16.0f / 128;
    
    mat4 proj_view = u_proj_mat * u_view_mat;
    gl_Position = proj_view * v_pos;
}
