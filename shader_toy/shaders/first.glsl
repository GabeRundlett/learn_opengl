#coel_shader[type: vert, name: "my vert shader"]

layout(location = 0) in vec2 a_pos;

void main() {
    gl_Position = vec4(a_pos, 0, 1);
}

#coel_shader[type: frag, name: "my frag shader"]

out vec4 frag_col;

uniform vec2 viewport_dim;

void main() {
    vec2 uv = (gl_FragCoord.xy - viewport_dim * 0.5) / viewport_dim.yy * 2.0f;
    frag_col = vec4(uv, 0, 1);
}
