#coel_shader[type: vert, name: "my vert shader"]
#version 440

layout(location = 0) in vec2 a_pos;

void main() {
    gl_Position = vec4(a_pos, 0, 1);
}

#coel_shader[type: frag, name: "my frag shader"]
#version 440

out vec4 frag_col;

uniform vec2 viewport_dim;
uniform sampler2D screen_tex;

void main() {
    vec2 uv = gl_FragCoord.xy / viewport_dim.xy;
    frag_col = texture(screen_tex, uv);
}
