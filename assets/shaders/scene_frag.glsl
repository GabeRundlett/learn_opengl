#version 440

in vec3 v_pos;
in vec3 v_nrm;
in vec2 v_tex;
out vec4 col;

uniform sampler2D u_albedo_tex;
uniform sampler2D u_normal_tex;
uniform sampler2D u_roughness_tex;

uniform int u_tex_index = 7;
uniform float u_texture_scale;
uniform int u_material_index;
uniform vec3 u_cam_pos;
vec3 light_pos = vec3(1.2, 2, 3);

uniform float u_diffuse_intensity;
uniform float u_diffuse_ambience;
uniform float u_specular_intensity;
uniform float u_specular_sharpness;
uniform float u_specular_roughness;

void main() {
    switch (u_material_index) {
    case 0: {
        vec3 ld = light_pos - v_pos;
        vec3 cd = normalize(v_pos - u_cam_pos);
        vec3 rd = normalize(reflect(ld, v_nrm));

        float dist2 = ld.x * ld.x + ld.y * ld.y + ld.z * ld.z;
        float diff = max(dot(normalize(ld), v_nrm) / dist2 * u_diffuse_intensity, 0.0f) + u_diffuse_ambience;

        float spec_roughness = u_specular_sharpness * (1 + texture(u_roughness_tex, v_tex * u_texture_scale).r * u_specular_roughness);
        float spec = pow(max(dot(cd, rd), 0.0f), pow(2, spec_roughness)) * u_specular_intensity;

        switch(u_tex_index) {
        default:
        case 1: col = vec4(0, 0, 0, 1); break;
        case 2: col = vec4(v_pos, 1); break;
        case 3: col = texture(u_albedo_tex, v_tex * u_texture_scale); break;
        case 4: col = texture(u_normal_tex, v_tex * u_texture_scale); break;
        case 5: col = vec4(vec3(diff), 1); break;
        case 6: col = vec4(vec3(spec), 1); break;
        case 7: col = vec4(texture(u_albedo_tex, v_tex * u_texture_scale).rgb * diff + spec, 1); break;
    } break;
    case 1:
        col = vec4(50, 50, 50, 1);
        break; 
    }
    }
}