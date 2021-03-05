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

vec3 coord_floor(in vec3 p) {
    return floor(p * textureSize(u_voxel_tex, 0)) / textureSize(u_voxel_tex, 0);
}

uint sample_density(in vec3 p) {
    // p.x = 1.0f / 16.0f * p.x;
    return texture(u_voxel_tex, coord_floor(p)).r;
}

#if 0
void march(inout bool hit, inout vec3 ray_pos, in vec3 ray_dir) {
    vec3 origin = ray_pos;
    // "pitch" angle 
    float theta = atan(ray_dir.y, ray_dir.x);
    float theta2 = atan(ray_dir.z, sqrt(ray_dir.y * ray_dir.y + ray_dir.x * ray_dir.x));

    vec3 d = ray_pos - floor(ray_pos);
    vec3 del = vec3(-tan(theta), 1.0/tan(theta), -tan(theta2));

    vec3 tilestep = vec3(1);
    if (ray_dir.x < 0) tilestep.x = -1;
    if (ray_dir.y < 0) tilestep.y = -1;
    if (ray_dir.z < 0) tilestep.z = -1;

    vec3 intercept = vec3(
        ray_pos.x + d.x + -d.y * del.y,
        ray_pos.y + d.y + -d.x * del.z,
        ray_pos.z + d.z + -d.z * del.z
        );

    int iter = 0;
    const int MAX_ITER = 1000;
    while (iter < MAX_ITER && !hit && dot(ray_pos - origin, ray_pos - origin) < 10) {
        ++iter;
        while (iter < MAX_ITER && !hit && intercept.x * tilestep.x < ray_pos.x * tilestep.x) {
            ++iter;
            if (sample_density(vec3(ray_pos.x, ray_pos.y, ray_pos.z)) > 0.1) {
                hit = true;
                break;
            }
            ray_pos.x += tilestep.x;
            intercept.y += del.y * tilestep.x;
            intercept.z += del.z * tilestep.x;
        }
        while (iter < MAX_ITER && !hit && intercept.y * tilestep.y < ray_pos.y * tilestep.y) {
            ++iter;
            if (sample_density(vec3(ray_pos.x, ray_pos.y, ray_pos.z)) > 0.1) {
                hit = true;
                break;
            }
            intercept.x += del.x * tilestep.y;
            ray_pos.y += tilestep.y;
            intercept.z += del.z * tilestep.z;
        }
        while (iter < MAX_ITER && !hit && intercept.z * tilestep.z < ray_pos.z * tilestep.z) {
            ++iter;
            if (sample_density(vec3(ray_pos.x, ray_pos.y, ray_pos.z)) > 0.1) {
                hit = true;
                break;
            }
            intercept.x += del.x * tilestep.z;
            intercept.y += del.y * tilestep.z;
            ray_pos.z += tilestep.z;
        }
    }
}
#endif

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
        // if (density < 0.5) discard;
        // vec3 p = floor(sample_pos * u_cube_dim) / u_cube_dim;
        // col = vec4(abs(sin(p * 293) * cos(p.xzy * 228.7) + cos(p.zxy * 231.5) * sin(p.yzx * 243.7) ) * 5, 1);
        // col = vec4(sample_nrm, 1);
        col = vec4(1, 0, 1, 1);
    } break;
    case 1:
        col = vec4(50, 50, 50, 1);
        break;
    }
}
