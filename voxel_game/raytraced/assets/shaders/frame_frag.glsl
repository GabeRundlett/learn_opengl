#version 440 core

in vec2 v_tex;
out vec4 frag_col;

uniform sampler2D u_frame_accum_tex;

uniform float u_gamma;
uniform float u_exposure;

void main() {
    vec3 hdr_color = texture(u_frame_accum_tex, v_tex).rgb;

    vec3 col_mapped = vec3(1.0) - exp(-hdr_color * u_exposure);
    col_mapped = pow(col_mapped, vec3(1.0f / u_gamma));

    frag_col = vec4(col_mapped, 1);
}
