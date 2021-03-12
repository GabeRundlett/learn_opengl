#version 330 core

in vec3 v_pos;
in vec3 v_nrm;
in vec2 v_tex;

out vec4 col;

uniform sampler2D u_tilemap_tex;
uniform usampler3D u_tiles_tex;
uniform vec3 u_cam_pos;

uniform float u_scale;

const int MAX_ITER = 100;
const int THRESHOLD = 0;
const float EPSILON = 0.001f;

int sample_tiles(vec3 p) {
    return int(texture(u_tiles_tex, p / textureSize(u_tiles_tex, 0)).r) & 0xFF;
}

bool intersect(vec3 p) {
    return sample_tiles(p) > THRESHOLD;
}

float segment_dist(vec2 p, vec2 c1, vec2 c2) {
    float x = p.x;
    float y = p.y;
    float x1 = c1.x;
    float x2 = c2.x;
    float y1 = c1.y;
    float y2 = c2.y;

    float A = x - x1;
    float B = y - y1;
    float C = x2 - x1;
    float D = y2 - y1;

    float dot_ = A * C + B * D;
    float len_sq = C * C + D * D;
    float param = -1;
    if (len_sq != 0) //in case of 0 length line
        param = dot_ / len_sq;

    float xx, yy;

    if (param < 0) {
        xx = x1;
        yy = y1;
    }
    else if (param > 1) {
        xx = x2;
        yy = y2;
    }
    else {
        xx = x1 + param * C;
        yy = y1 + param * D;
    }

    float dx = x - xx;
    float dy = y - yy;
    return sqrt(dx * dx + dy * dy);
}

struct intersection_info {
    vec3 pos, nrm;
    int steps;
    bool hit;
};

void raycast(vec3 ray_origin, vec3 ray_dir, inout intersection_info i) {
    i.hit = false;
    i.pos = ray_origin;
    vec3 ray_p = floor(ray_origin);
    vec3 ray_d = ray_p - ray_origin;
    ivec3 ray_step = ivec3(-1);
    if (ray_dir.x > 0)
        ray_d.x += 1, ray_step.x = 1;
    if (ray_dir.y > 0)
        ray_d.y += 1, ray_step.y = 1;
    if (ray_dir.z > 0)
        ray_d.z += 1, ray_step.z = 1;
    i.steps = 0;
    float slope_xy = ray_dir.y / ray_dir.x;
    float slope_xz = ray_dir.z / ray_dir.x;
    float slope_yx = ray_dir.x / ray_dir.y;
    float slope_yz = ray_dir.z / ray_dir.y;
    float slope_zx = ray_dir.x / ray_dir.z;
    float slope_zy = ray_dir.y / ray_dir.z;
    vec3 to_travel_x = vec3(ray_d.x, slope_xy * ray_d.x, slope_xz * ray_d.x);
    vec3 to_travel_y = vec3(slope_yx * ray_d.y, ray_d.y, slope_yz * ray_d.y);
    vec3 to_travel_z = vec3(slope_zx * ray_d.z, slope_zy * ray_d.y, ray_d.z);
    if (ray_dir.x == 0 || ray_dir.y == 0 || ray_dir.z == 0)
        return;
    while (i.steps < MAX_ITER) {
        while (i.steps < MAX_ITER &&
            to_travel_x.x * ray_step.x < to_travel_y.x * ray_step.x && 
            to_travel_x.x * ray_step.x < to_travel_z.x * ray_step.x) {
            i.pos += to_travel_x;
            if (intersect(i.pos + vec3(ray_step.x * EPSILON, 0, 0))) {
                i.pos.x += ray_step.x * EPSILON;
                i.nrm = vec3(-ray_step.x, 0, 0);
                i.hit = true;
                return;
            }
            to_travel_x = vec3(ray_step.x, slope_xy * ray_step.x, slope_xz * ray_step.x);
            to_travel_y -= to_travel_x;
            to_travel_z -= to_travel_x;
            ++i.steps;
        }
        while (i.steps < MAX_ITER &&
            to_travel_y.y * ray_step.y < to_travel_x.y * ray_step.y && 
            to_travel_y.y * ray_step.y < to_travel_z.y * ray_step.y) {
            i.pos += to_travel_z;
            if (intersect(i.pos + vec3(0, ray_step.y * EPSILON, 0))) {
                i.pos.y += ray_step.y * EPSILON;
                i.nrm = vec3(0, -ray_step.y, 0);
                i.hit = true;
                return;
            }
            to_travel_x -= to_travel_y;
            to_travel_y = vec3(slope_yx * ray_step.y, ray_step.y, slope_yz * ray_step.y);
            to_travel_z -= to_travel_y;
            ++i.steps;
        }
        while (i.steps < MAX_ITER &&
            to_travel_z.z * ray_step.z < to_travel_y.z * ray_step.z && 
            to_travel_z.z * ray_step.z < to_travel_x.z * ray_step.z) {
            i.pos += to_travel_z;
            if (intersect(i.pos + vec3(0, 0, ray_step.z * EPSILON))) {
                i.pos.z += ray_step.z * EPSILON;
                i.nrm = vec3(0, 0, -ray_step.z);
                i.hit = true;
                return;
            }
            to_travel_x -= to_travel_z;
            to_travel_y -= to_travel_z;
            to_travel_z = vec3(slope_zx * ray_step.z, slope_zy * ray_step.y, ray_d.z);
            ++i.steps;
        }
        ++i.steps;
    }
}

void main() {
    vec3 ray_origin = u_cam_pos;
    vec3 ray_dir = v_pos - u_cam_pos;
    intersection_info i;

    raycast(ray_origin, ray_dir, i);

    if (i.hit) {
        col = vec4(vec3(1, 0, 0), 1);
    } else {
        col = vec4(vec3(0, 1, 0), 1);
    }
}
