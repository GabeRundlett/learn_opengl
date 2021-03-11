#version 440 core

in vec4 v_pos;
in vec3 v_nrm;
in vec2 v_tex;

out vec4 pos;
out vec4 nrm;
out vec4 col;

uniform sampler2D u_tilemap_tex;

void main() {
    pos = v_pos;
    nrm = vec4(v_nrm, 1);
    col = texture(u_tilemap_tex, v_tex);
}
