#version 330 core

in vec2 v_pos;
out vec4 col;
uniform sampler2D u_tilemap_tex;
uniform usampler2D u_tiles_tex;
uniform vec2 u_player_pos;
uniform vec2 u_mouse_pos;

uniform vec2 u_ray_pos;
uniform vec2 u_ray_nrm;
uniform int u_ray_hit;

uniform float u_scale;

const int MAX_ITER = 100;
const int THRESHOLD = 200;
const float EPSILON = 0.001f;

// int floor(float fp) {
//     int i = int(fp);
//     return (fp < i) ? (i - 1) : (i);
// }
// ivec2 floor(vec2 fp) {
//     return ivec2(floor(fp.x), floor(fp.y));
// }

int sample_tiles(vec2 p) {
    return int(texture(u_tiles_tex, p / textureSize(u_tiles_tex, 0)).r) & 0xFF;
}

bool intersect(vec2 p) {
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

bool raycast(vec2 ray_origin, inout vec2 ray_pos, inout vec2 ray_nrm, vec2 ray_dir) {
    ray_pos = ray_origin;
    vec2 ray_p = floor(ray_origin);
    vec2 ray_d = ray_p - ray_origin;
    ivec2 ray_step = ivec2(-1);
    if (ray_dir.x > 0)
        ray_d.x += 1, ray_step.x = 1;
    if (ray_dir.y > 0)
        ray_d.y += 1, ray_step.y = 1;
    int iter = 0;
    if (ray_dir.y == 0) {
        while (iter < MAX_ITER) {
            ray_pos += vec2(ray_d.x, 0);
            ray_d.x = float(ray_step.x);
            if (intersect(ray_pos + vec2(ray_step.x * EPSILON, 0))) {
                ray_pos.x += ray_step.x * EPSILON;
                ray_nrm = vec2(-ray_step.x, 0);
                return true;
            }
            ++iter;
        }
    } else {
        vec2 ray_step_slope = vec2(ray_dir.y / ray_dir.x, ray_dir.x / ray_dir.y);
        vec2 to_travel_x = vec2(ray_d.x, ray_step_slope.x * ray_d.x);
        vec2 to_travel_y = vec2(ray_step_slope.y * ray_d.y, ray_d.y);
        while (iter < MAX_ITER) {
            while (iter < MAX_ITER && to_travel_x.x * ray_step.x < to_travel_y.x * ray_step.x) {
                ray_pos += to_travel_x;
                if (intersect(ray_pos + vec2(ray_step.x * EPSILON, 0))) {
                    ray_pos.x += ray_step.x * EPSILON;
                    ray_nrm = vec2(-ray_step.x, 0);
                    return true;
                }
                to_travel_y -= to_travel_x;
                to_travel_x = vec2(
                    ray_step.x,
                    ray_step_slope.x * ray_step.x);
                ++iter;
            }
            while (iter < MAX_ITER && to_travel_x.x * ray_step.x >= to_travel_y.x * ray_step.x) {
                ray_pos += to_travel_y;
                if (intersect(ray_pos + vec2(0, ray_step.y * EPSILON))) {
                    ray_pos.y += ray_step.y * EPSILON;
                    ray_nrm = vec2(0, -ray_step.y);
                    return true;
                }
                to_travel_x -= to_travel_y;
                to_travel_y = vec2(
                    ray_step_slope.y * ray_step.y,
                    ray_step.y);
                ++iter;
            }
            ++iter;
        }
    }
    return false;
}

void main() {
    ivec2 tile_pos = ivec2(floor(v_pos));
    vec2 ray_origin = -u_player_pos;
    vec2 ray_pos, ray_nrm;

    bool hit = raycast(ray_origin, ray_pos, ray_nrm, u_mouse_pos - ray_origin);

    if (length(v_pos - u_mouse_pos) < 0.01 / u_scale) {
        col = vec4(0.1, 0.5, 0.8, 1);
    } else if (length(v_pos + u_player_pos) < 0.01 / u_scale) {
        col = vec4(1, 0, 0, 1);
    } else if (segment_dist(v_pos, ray_origin, ray_pos) < 0.002 / u_scale) {
        col = vec4(0.9, 0.5, 0.9, 1);
    } else if (segment_dist(v_pos, ray_pos, ray_pos + ray_nrm * 2) < 0.002 / u_scale) {
        col = vec4(abs(ray_nrm), 0, 1);
    } else {
        int value = sample_tiles(v_pos);
        if (value > THRESHOLD) {
            if (value > 230) {
                col = texture(u_tilemap_tex, (mod(v_pos, 1) + vec2(0, 2)) * 16.f / 128.f);
            } else {
                col = texture(u_tilemap_tex, (mod(v_pos, 1) + vec2(2, 2)) * 16.f / 128.f);
            }
        } else {
            col = vec4(0.12, 0.11, 0.1, 1);
        }
    }
}
