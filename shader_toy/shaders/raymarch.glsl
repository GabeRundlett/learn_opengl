#coel_shader[type : vert, name : "my vert shader"]
#version 440

layout(location = 0) in vec2 a_pos;

void main() {
    gl_Position = vec4(a_pos, 0, 1);
}

#coel_shader[type : frag, name : "my frag shader"]
#version 440

out vec4 frag_col;

uniform vec2 viewport_dim;
uniform vec3 cam_pos;
uniform mat4 mvp_mat;

float sdf_sphere(in vec3 sample_pos, in vec3 sphere_center, in float sphere_radius) {
    return length(sample_pos - sphere_center) - sphere_radius;
}

#define MIN_DIST 0.01f
#define MAX_DIST 100.0f
#define MAX_ITER 1000

void main() {
    vec2 uv = (gl_FragCoord.xy - viewport_dim * 0.5) / viewport_dim.yy * 2.0f;

    vec3 ray_origin = -cam_pos;
    vec3 ray_direction = (mvp_mat * normalize(vec4(uv, 1, 0))).xyz;
    uint ray_iter = 0;
    float ray_total_dist = 0.0f;

    vec3 s1_pos = vec3(0, 0, 0);
    float s1_rad = 1.0f;

    vec3 sample_pos = ray_origin;
    bool hit = false;

    while (true) {
        float dist = sdf_sphere(sample_pos, s1_pos, s1_rad);
        if (dist < MIN_DIST) {
            hit = true;
            break;
        }
        if (ray_iter > MAX_ITER || ray_total_dist > MAX_DIST) {
            hit = false;
            break;
        }

        sample_pos += ray_direction * dist;
        ray_total_dist += dist;
        ++ray_iter;
    }

    frag_col = vec4(vec3(float(ray_iter) / 10), 1);
}
