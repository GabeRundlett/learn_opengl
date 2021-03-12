#version 440 core

in vec4 v_pos;
in vec3 v_nrm;
in vec2 v_tex;
out vec4 col;

uniform vec3 u_cam_pos;
uniform vec3 u_cube_pos;
uniform vec3 u_cube_dim;

uniform usampler3D u_voxel_tex;
uniform int u_material_index;

const int MAX_ITER = 100;
const float THRESHOLD = 0;

vec3 coord_floor(in vec3 p) {
    return floor(p * textureSize(u_voxel_tex, 0)) / textureSize(u_voxel_tex, 0);
}

int fast_floor(float fp) {
    int i = int(fp);
    return (fp < i) ? (i - 1) : (i);
}
ivec2 fast_floor(vec2 fp) {
    return ivec2(fast_floor(fp.x), fast_floor(fp.y));
}

uint sample_density(in vec3 p) {
    // p.x = 1.0f / 16.0f * p.x;
    return texture(u_voxel_tex, coord_floor(p)).r;
}

bool intersect(vec2 p) {
    return sample_density(vec3(p, 0)) > THRESHOLD;
}

bool raycast(in vec2 ray_origin, inout vec2 ray_pos, in vec2 ray_dir) {
    ray_pos = ray_origin;
    
    vec2 ray_p = fast_floor(ray_origin);
    vec2 ray_d = ray_p - ray_origin;

    ivec2 ray_step = ivec2(-1);

    if (ray_dir.x > 0) {
        ray_d.x += 1;
        ray_step.x = 1;
    }
    if (ray_dir.y > 0) {
        ray_d.y += 1;
        ray_step.y = 1;
    }

    vec2 ray_step_slope = vec2(ray_dir.y / ray_dir.x, ray_dir.x / ray_dir.y);
    
    vec2 to_travel_x = vec2(
        ray_d.x,
        ray_step_slope.x * ray_d.x
    );
    vec2 to_travel_y = vec2(
        ray_step_slope.y * ray_d.y,
        ray_d.y
    );

    int iter = 0;
    while (iter < MAX_ITER) {
        while (iter < MAX_ITER && to_travel_x.x * ray_step.x < to_travel_y.x * ray_step.x) {
            ray_pos += to_travel_x;

            if (intersect(ray_pos + vec2(ray_step.x * 0.001, 0))) {
                ray_pos.x += ray_step.x * 0.001;
                return true;
            }
            
            to_travel_y -= to_travel_x;
            to_travel_x = vec2(
                ray_step.x,
                ray_step_slope.x * ray_step.x
            );
            ++iter;
        }
        while (iter < MAX_ITER && to_travel_x.x * ray_step.x >= to_travel_y.x * ray_step.x) {
            ray_pos += to_travel_y;

            if (intersect(ray_pos + vec2(0, ray_step.y * 0.001))) {
                ray_pos.y += ray_step.y * 0.001;
                return true;
            }

            to_travel_x -= to_travel_y;
            to_travel_y = vec2(
                ray_step_slope.y * ray_step.y,
                ray_step.y
            );
            ++iter;
        }
    }

    return false;
}

void main() {
    switch (u_material_index) {
    case 0: {
        vec3 space_scale = vec3(1) / u_cube_dim;
        vec3 space_offset = vec3(0.5, 0.5, 0.5);
        vec3 frag_pos = v_pos.xyz * space_scale + space_offset;
        vec3 cam_pos = u_cam_pos * space_scale + space_offset;
        vec3 cam_diff = frag_pos - cam_pos;
        float cam_ray_len = length(cam_diff);
        vec3 cam_ray_dir = cam_diff / cam_ray_len;
        vec3 sample_pos;
        float depth;
        if (cam_pos.x < 0 || cam_pos.x > 1 || 
            cam_pos.y < 0 || cam_pos.y > 1 ||
            cam_pos.z < 0 || cam_pos.z > 1) {
            sample_pos = frag_pos;
            depth = cam_ray_len;
        } else {
            sample_pos = cam_pos;
            depth = 0.0f;
        }
        float density = 0.0f;
        vec3 prev_sample_pos = sample_pos;
        vec3 sample_nrm = vec3(0);
        for (int i = 0; i < 5000; i++) {
            if (sample_pos.x < 0 || sample_pos.x > 1 ||
                sample_pos.y < 0 || sample_pos.y > 1 ||
                sample_pos.z < 0 || sample_pos.z > 1) {
                density = 0;
                break;
            }
            if ((sample_density(sample_pos) & 0x000000ff) > 128) {
                vec3 p0 = coord_floor(sample_pos);
                vec3 p1 = coord_floor(prev_sample_pos);
                if (p1.x != p0.x) {
                    sample_nrm.x = sign(p1.x - p0.x);
                } else if (p1.y != p0.y) {
                    sample_nrm.y = sign(p1.y - p0.y);
                } else if (p1.z != p0.z) {
                    sample_nrm.z = sign(p1.z - p0.z);
                }
                density = 1;
                break;
            }
            float delta_depth = 0.0001 * depth + 0.0001;
            depth += sqrt(delta_depth);
            prev_sample_pos = sample_pos;
            sample_pos += cam_ray_dir * delta_depth;
        }

        if (density < 0.5) discard;
        vec3 p = floor(sample_pos * u_cube_dim) / u_cube_dim;
        col = vec4(abs(sin(p * 293) * cos(p.xzy * 228.7) + cos(p.zxy * 231.5) * sin(p.yzx * 243.7) ) * 5, 1);
        // col = vec4(sample_nrm, 1);
        // col = vec4(1, 0, 1, 1);
    } break;
    case 1:
        col = vec4(50, 50, 50, 1);
        break;
    }
}
