#version 440

in vec3 v_pos;
in vec3 v_nrm;
in vec2 v_tex;
out vec4 col;

uniform vec3 u_cam_pos;
vec3 light_pos = vec3(1.2, 2, 3);

float u_diffuse_intensity = 1.0;
float u_diffuse_ambience = 0.0;
float u_specular_intensity = 1.0;
float u_specular_sharpness = 1.0;
float u_specular_roughness = 1.0;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

void main() {
    vec3 ld = light_pos - v_pos;
    vec3 cd = normalize(v_pos - u_cam_pos);
    vec3 rd = normalize(reflect(ld, v_nrm));

    float dist2 = ld.x * ld.x + ld.y * ld.y + ld.z * ld.z;
    float diff = max(dot(normalize(ld), v_nrm) / dist2 * u_diffuse_intensity, 0.0f) + u_diffuse_ambience;

    // float spec_roughness = u_specular_sharpness * (1 + texture(u_roughness_tex, v_tex * u_texture_scale).r * u_specular_roughness);
    // float spec = pow(max(dot(cd, rd), 0.0f), pow(2, spec_roughness)) * u_specular_intensity;
    //texture(u_albedo_tex, v_tex * u_texture_scale).rgb
    col = vec4(vec3(1, 1, 1) * diff, 1);
}
