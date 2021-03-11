#version 440 core

in vec2 v_tex;
out vec4 col;

uniform sampler2D u_frame_pos_tex;
uniform sampler2D u_frame_nrm_tex;
uniform sampler2D u_frame_col_tex;

uniform float u_gamma;
uniform float u_exposure;
uniform vec3 u_cam_pos;

void main() {
    vec3 hdr_color = texture(u_frame_col_tex, v_tex).rgb;
    vec3 position = texture(u_frame_pos_tex, v_tex).xyz;
    vec3 normal = texture(u_frame_nrm_tex, v_tex).rgb;
    
    vec3 light_dir = normalize(vec3(-1, -3, -1.5));
    vec3 light_intensity = vec3(1.0f);
    vec3 light_ambience = vec3(0.1f);
    vec3 diff = ((dot(-light_dir, normal) + 1.0f) * light_intensity + light_ambience) * hdr_color;

    vec3 col_mapped = vec3(1.0) - exp(-diff * u_exposure);
    col_mapped = pow(col_mapped, vec3(1.0f / u_gamma));

    col = vec4(col_mapped, 1);
}
