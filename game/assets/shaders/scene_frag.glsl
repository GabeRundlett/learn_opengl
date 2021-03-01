#version 440

in vec4 v_pos;
in vec3 v_nrm;
in vec2 v_tex;
out vec4 col;

uniform sampler2D u_albedo_tex;
uniform sampler2D u_normal_tex;
uniform sampler2D u_roughness_tex;

uniform int u_tex_index = 0;
uniform float u_texture_scale;
uniform int u_material_index;
uniform vec3 u_cam_pos;
vec3 light_pos = vec3(1.2, 2, 3);

uniform float u_diffuse_intensity;
uniform float u_diffuse_ambience;
uniform float u_specular_intensity;
uniform float u_specular_sharpness;
uniform float u_specular_roughness;

vec3 ld = light_pos - v_pos.xyz;
vec3 cd = normalize(v_pos.xyz - u_cam_pos);
vec3 rd = normalize(reflect(ld, v_nrm));

float dist2 = ld.x * ld.x + ld.y * ld.y + ld.z * ld.z;
float diff = max(dot(normalize(ld), v_nrm) / dist2 * u_diffuse_intensity, 0.0f) + u_diffuse_ambience;
float spec_roughness = u_specular_sharpness * (1 + texture(u_roughness_tex, v_tex * u_texture_scale).r * u_specular_roughness);
float spec = pow(max(dot(cd, rd), 0.0f), pow(2, spec_roughness)) * u_specular_intensity;

#define MIN_DEPTH 0.0
#define MAX_DEPTH 10.0
#define EPSILON 0.001
#define MAX_MARCHING_STEPS 100

float sphere_sdf(vec3 p, vec3 center, float radius) {
    return length(p - center) - radius;
}

float smooth_min(float a, float b) {
    float k = 0.1;
    float h = clamp(0.5 + 0.5*(a-b)/k, 0.0, 1.0);
	return mix(a, b, h) - k*h*(1.0-h);
}

float scene_sdf(vec3 p) {
    float dist = MAX_DEPTH;
    dist = smooth_min(sphere_sdf(p, vec3(-1, 0.5, 0), 1), dist);
    dist = smooth_min(sphere_sdf(p, vec3(2, 0.5, 0), 0.5), dist);
    return dist;
}

vec3 estimate_nrm(vec3 p) {
    return normalize(vec3(
        scene_sdf(vec3(p.x + EPSILON, p.y, p.z)) - scene_sdf(vec3(p.x - EPSILON, p.y, p.z)),
        scene_sdf(vec3(p.x, p.y + EPSILON, p.z)) - scene_sdf(vec3(p.x, p.y - EPSILON, p.z)),
        scene_sdf(vec3(p.x, p.y, p.z + EPSILON)) - scene_sdf(vec3(p.x, p.y, p.z - EPSILON))
    ));
}

void raymarch() {
    vec3 ray_dir = cd;
    vec3 eye = u_cam_pos;

    float depth = MIN_DEPTH;
    bool hit = false;

    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        float dist = scene_sdf(eye + depth * ray_dir);
        if (dist < EPSILON) 
            hit = true;
        depth += dist;
        if (depth > MAX_DEPTH)
            break;
    }

    if (hit) {
        gl_FragDepth = 1;
        vec3 hit_pos = depth * ray_dir + eye;
        vec3 hit_nrm = estimate_nrm(hit_pos);
        ld = light_pos - hit_pos;
        dist2 = ld.x * ld.x + ld.y * ld.y + ld.z * ld.z;
        float df = max(dot(normalize(ld), hit_nrm) / dist2 * u_diffuse_intensity, 0.0f) + u_diffuse_ambience;
        col = vec4(vec3(1) * df, 1);
    } else {
        discard;
    }
}

void main() {
    switch (u_material_index) {
    case 0:
        switch(u_tex_index) {
        case 0: raymarch(); break;
        case 1: col = vec4(0, 0, 0, 1); break;
        case 2: col = vec4(v_pos.xyz, 1); break;
        case 3: col = texture(u_albedo_tex, v_tex * u_texture_scale); break;
        case 4: col = texture(u_normal_tex, v_tex * u_texture_scale); break;
        case 5: col = vec4(vec3(diff), 1); break;
        case 6: col = vec4(vec3(spec), 1); break;
        case 7: col = vec4(texture(u_albedo_tex, v_tex * u_texture_scale).rgb * diff + spec, 1); break;
        default: col = vec4(1, 0, 1, 1); break;
        } break;
    case 1:
        col = vec4(50, 50, 50, 1);
        break;
    }
}