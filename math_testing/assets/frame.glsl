#coel_shader[type: vert, name: "my vert shader"]
#version 440 core
layout(location = 0) in vec2 a_pos;
out vec2 v_pos;
void main() {
    v_pos = a_pos / 2 + 0.5;
    gl_Position = vec4(a_pos, 0, 1);
}

#coel_shader[type: frag, name: "my frag shader"]
#version 440 core
in vec2 v_pos;
uniform sampler2D u_player_frame;
uniform sampler2D u_second_frame;
out vec4 frag_col;
void main() {
    vec2 uv = v_pos * vec2(2, 1);
    if (uv.x < 1) {
        frag_col = texture(u_player_frame, uv);
    } else {
        uv.x -= 1;
        frag_col = texture(u_second_frame, uv);
    }
}
