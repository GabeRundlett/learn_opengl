#coel_shader[type: vert, name: "my vert shader"]
#version 440 core
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_nrm;
layout(location = 2) in vec2 a_tex;
uniform mat4 u_view_mat;
uniform mat4 u_proj_mat;
uniform mat4 u_cust_mat;
out vec4 v_pos;
out vec2 v_tex;
void main() {
    v_pos = u_cust_mat * vec4(a_pos * 2, 1);
    v_tex = a_tex;
    mat4 proj_view = u_proj_mat * u_view_mat;
    gl_Position = proj_view * v_pos;
}

#coel_shader[type: frag, name: "my frag shader"]
#version 440 core
in vec4 v_pos;
in vec2 v_tex;
uniform vec4 u_col1;
out vec4 frag_col;
uniform int u_style;
void main() {
    switch (u_style) {
    case 0: {
        const float thickness = 0.05f;
        if (v_tex.x > thickness && v_tex.x < 1.0f - thickness && v_tex.y > thickness && v_tex.y < 1.0f - thickness && int(gl_FragCoord.x + gl_FragCoord.y) % 5 == 0)
            discard;
        frag_col = u_col1;
    } break;
    case 1: {
        const float thickness = 0.05f;
        if (v_tex.x > thickness && v_tex.x < 1.0f - thickness && v_tex.y > thickness && v_tex.y < 1.0f - thickness && int(gl_FragCoord.x + gl_FragCoord.y) % 3 != 0)
            discard;
        frag_col = u_col1;
    } break;
    }
}
