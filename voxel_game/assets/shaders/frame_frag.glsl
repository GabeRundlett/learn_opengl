#version 440 core

in vec2 v_tex;
out vec4 col;

uniform sampler2D u_frame_col_tex;
uniform vec2 u_frame_dim;

float u_gamma = 1.8f;
float u_exposure = 0.05f;

void main() {
    vec3 hdr_color = texture(u_frame_col_tex, v_tex).rgb;
    vec3 col_mapped = vec3(1.0) - exp(-hdr_color * u_exposure);
    col_mapped = pow(col_mapped, vec3(1.0f / u_gamma));

    col = vec4(col_mapped, 1);

    vec2 uv = (gl_FragCoord.xy - vec2((u_frame_dim.x - u_frame_dim.y) * 0.5, 0)) / u_frame_dim.yy;
    const float cross_length = 0.004;
    const float cross_thickness = 0.001;
    if (uv.x > 0.5-cross_length && uv.x < 0.5+cross_length && uv.y > 0.5-cross_thickness && uv.y < 0.5+cross_thickness || 
        uv.x > 0.5-cross_thickness && uv.x < 0.5+cross_thickness && uv.y > 0.5-cross_length && uv.y < 0.5+cross_length)
        col = vec4(1-col.rgb, 1);
}
