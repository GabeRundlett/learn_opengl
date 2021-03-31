#coel_shader[type: vert, name: ""]
#version 440 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec4 a_col;

out vec2 v_tex;
out vec4 v_col;

uniform mat4 u_view_mat = mat4(1);
uniform mat4 u_proj_mat;

void main() {
    v_col = a_col;
    mat4 proj_view = u_proj_mat * u_view_mat;
    gl_Position = proj_view * vec4(a_pos, 1);
}

#coel_shader[type: frag, name: ""]
#version 440 core

in vec4 v_col;
out vec4 frag_col;

void main() {
    frag_col = v_col;
}
