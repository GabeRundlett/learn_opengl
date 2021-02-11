#version 440

in vec2 v_tex;
out vec4 col;

uniform sampler2D u_scene_frame_tex;
uniform float u_scene_frame_gamma;
uniform float u_scene_frame_exposure;

void main() {
    float gamma = u_scene_frame_gamma;
    float exposure = u_scene_frame_exposure;
    vec3 hdr_color = texture(u_scene_frame_tex, v_tex).rgb;
  
    // reinhard tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdr_color * exposure);
    // gamma correction 
    mapped = pow(mapped, vec3(1.0f / gamma));
    col = vec4(mapped, 1.0f);
    // col = vec4(v_tex, 0.5, 1);
}