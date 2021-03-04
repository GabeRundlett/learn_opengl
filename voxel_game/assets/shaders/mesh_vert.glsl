#version 440

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_nrm;
layout(location = 2) in vec2 a_tex;

out vec3 v_pos;
out vec3 v_nrm;
out vec2 v_tex;

uniform mat4 u_view_mat = mat4(1);
uniform mat4 u_proj_mat = mat4(1);
uniform mat4 u_modl_mat = mat4(1);

void main() {
    v_pos = (u_modl_mat * glm::vec4(a_pos, 1)).xyz, v_nrm = a_nrm, v_tex = a_tex;
    gl_Position = u_proj_mat * u_view_mat * vec4(v_pos, 1);
}
