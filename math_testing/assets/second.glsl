#coel_shader[type: vert, name: "my vert shader"]
#version 440 core
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_nrm;
layout(location = 2) in vec2 a_tex;
uniform mat4 u_view_mat;
uniform mat4 u_proj_mat;
out vec4 v_pos;
out vec2 v_tex;
void main() {
    v_pos = vec4(a_pos * 2, 1);
    v_tex = a_tex;
    mat4 proj_view = u_proj_mat * u_view_mat;
    gl_Position = proj_view * v_pos;
}

#coel_shader[type: frag, name: "my frag shader"]
#version 440 core
in vec4 v_pos;
in vec2 v_tex;
out vec4 frag_col;
void main() {
    frag_col = vec4(0, 0, 0, 1);
}
