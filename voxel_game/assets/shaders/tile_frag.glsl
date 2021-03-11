#version 440 core

in vec2 v_pos;
out vec4 col;
uniform sampler2D u_tilemap_tex;
uniform vec2 u_player_pos;
uniform vec2 u_mouse_pos;
uniform float u_scale;
uniform int u_max_iter;

const int perm[256] = {
    151, 160, 137, 91, 90, 15,
    131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
    190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
    88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
    77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
    102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
    135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
    5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
    223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
    129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
    251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
    49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
    138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
};

int hash(int i) {
    return perm[i % 255];
}

float grad(int hash, float x) {
    const int h = hash & 0x0F;
    float grad = 1.0f + (h & 0x07);
    if ((h & 8) != 0)
        grad = -grad;
    return grad * x;
}
float grad(int hash, vec2 p) {
    const int h = hash & 0x3f;
    const float u = h < 4 ? p.x : p.y;
    const float v = h < 4 ? p.y : p.x;
    return ((h & 1) != 0 ? -u : u) + ((h & 2) != 0 ? -2.0f * v : 2.0f * v);
}
float grad(int hash, vec3 p) {
    int h = hash & 0x0f;
    float u = h < 8 ? p.x : p.y;
    float v = h < 4 ? p.y : h == 12 || h == 14 ? p.x : p.z;
    return ((h & 1) != 0 ? -u : u) + ((h & 2) != 0 ? -v : v);
}

int fast_floor(float fp) {
    int i = int(fp);
    return (fp < i) ? (i - 1) : (i);
}
ivec2 fast_floor(vec2 fp) {
    return ivec2(fast_floor(fp.x), fast_floor(fp.y));
}

float noise(vec2 sample_pos) {
    float n0, n1, n2; // Noise contributions from the three corners

    // Skewing/Unskewing factors for 2D
    const float F2 = 0.366025403f; // F2 = (sqrt(3) - 1) / 2
    const float G2 = 0.211324865f; // G2 = (3 - sqrt(3)) / 6   = F2 / (1 + 2 * K)

    // Skew the input space to determine which simplex cell we're in
    const float s = (sample_pos.x + sample_pos.y) * F2; // Hairy factor for 2D
    const vec2 scaled_sample_pos = sample_pos + s;
    const ivec2 ij = fast_floor(scaled_sample_pos);

    // Unskew the cell origin back to (x,y) space
    const float t = float(ij.x + ij.y) * G2;
    const vec2 P0 = ij - t, p0 = sample_pos - P0;

    ivec2 ij1; // Offsets for second (middle) corner of simplex in (i,j) coords
    if (p0.x > p0.y) ij1 = ivec2(1, 0);
    else ij1 = ivec2(0, 1);

    // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
    // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
    // c = (3-sqrt(3))/6

    const vec2 p1 = p0 - ij1 + G2;
    const vec2 p2 = p0 - 1.0f + 2.0f * G2;

    // Work out the hashed gradient indices of the three simplex corners
    const int gi0 = hash(ij.x + hash(ij.y));
    const int gi1 = hash(ij.x + ij1.x + hash(ij.y + ij1.y));
    const int gi2 = hash(ij.x + 1 + hash(ij.y + 1));

    // Calculate the contribution from the first corner
    float t0 = 0.5f - p0.x * p0.x - p0.y * p0.y;
    if (t0 < 0.0f) {
        n0 = 0.0f;
    } else {
        t0 *= t0;
        n0 = t0 * t0 * grad(gi0, p0);
    }

    // Calculate the contribution from the second corner
    float t1 = 0.5f - p1.x * p1.x - p1.y * p1.y;
    if (t1 < 0.0f) {
        n1 = 0.0f;
    } else {
        t1 *= t1;
        n1 = t1 * t1 * grad(gi1, p1);
    }

    // Calculate the contribution from the third corner
    float t2 = 0.5f - p2.x * p2.x - p2.y * p2.y;
    if (t2 < 0.0f) {
        n2 = 0.0f;
    } else {
        t2 *= t2;
        n2 = t2 * t2 * grad(gi2, p2);
    }

    // Add contributions from each corner to get the final noise value.
    // The result is scaled to return values in the interval [-1,1].
    return 45.23065f * (n0 + n1 + n2);
}

struct fractal_noise_conifg {
    float amplitude;
    float persistance;
    float scale;
    float lacunarity;
    int octaves;
};

float fractal_noise(vec2 sample_pos, fractal_noise_conifg config) {
    float value = 0.0f;
    for (int i = 0; i < config.octaves; ++i) {
        value += noise(sample_pos * config.scale) * config.amplitude;
        config.amplitude *= config.persistance;
        config.scale *= config.lacunarity;
    }
    return value;
}


fractal_noise_conifg noise_conf;
float threshold = 0;

bool intersect(vec2 p) {
    return fractal_noise(fast_floor(p), noise_conf) > threshold;
}

vec3 get_color(float value, vec2 p) {
    float delta = 1;
    vec2 value_at_offset = vec2(
        fractal_noise(fast_floor(p) - vec2(delta, 0), noise_conf),
        fractal_noise(fast_floor(p) - vec2(0, delta), noise_conf)
    );
    vec2 slope = (vec2(value) - value_at_offset) / delta;
    // return vec3((slope) * 10, 0);
    if (dot(slope, vec2(0, 2)) - value > 0) {
        return vec3(0.3, 0.5, 0.12);
    } else if (dot(slope, vec2(0, 6)) - value > 0) {
        return vec3(0.4, 0.25, 0.12);
    } else {
        return vec3(0.25, 0.25, 0.25);
    }
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

#if 0
// NAIVE METHOD
bool raycast(in vec2 ray_origin, inout vec2 ray_pos, in vec2 ray_dir) {
    ray_pos = ray_origin;

    const int max_steps = 100;
    const float step_delta = 0.5;

    for (int i = 0; i < max_steps; ++i) {
        if (intersect(ray_pos)) return true;
        ray_pos += ray_dir * step_delta;
    }

    return false;
}
#else
// ACCURATE METHOD
bool raycast(in vec2 ray_origin, inout vec2 ray_pos, in vec2 ray_dir) {
    ray_pos = ray_origin;
    
    vec2 ray_p = fast_floor(ray_origin);
    vec2 ray_offset = ray_origin - ray_p;
    vec2 ray_d = -ray_offset;

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
    while (iter < u_max_iter) {
        while (iter < u_max_iter && to_travel_x.x * ray_step.x < to_travel_y.x * ray_step.x) {
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
        while (iter < u_max_iter && to_travel_x.x * ray_step.x >= to_travel_y.x * ray_step.x) {
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
#endif

void main() {
    noise_conf.amplitude = 0.5f;
    noise_conf.persistance = 0.4f;
    noise_conf.scale = 0.025f;
    noise_conf.lacunarity = 2.0f;
    noise_conf.octaves = 4;

    ivec2 tile_pos = fast_floor(v_pos);
    
    vec2 ray_origin = -u_player_pos;
    vec2 ray_pos;
    bool hit = raycast(ray_origin, ray_pos, normalize(u_mouse_pos - ray_origin));

    if (length(v_pos - u_mouse_pos) < 0.02 / u_scale) {
        col = vec4(0.1, 0.5, 0.8, 1);
    } else if (length(v_pos + u_player_pos) < 0.03 / u_scale) {
        col = vec4(1, 0, 0, 1);
    } else if (segment_dist(v_pos, ray_origin, ray_pos) < 0.01 / u_scale) {
        col = vec4(0.9, 0.5, 0.9, 1);
    } else {
        float value = fractal_noise(fast_floor(v_pos), noise_conf);
        bool intersects = value > threshold;
        col = vec4(intersects ? get_color(value, v_pos) : vec3(0.12, 0.11, 0.1), 1);
    }
    
    if (length(v_pos - ray_pos) < 0.02 / u_scale) {
        if (length(v_pos - ray_pos) < 0.015 / u_scale) {
            float value = fractal_noise(fast_floor(ray_pos), noise_conf);
            bool intersects = value > threshold;
            col = vec4(intersects ? get_color(value, ray_pos) : vec3(0.12, 0.11, 0.1), 1);
        } else {
            col = vec4(hit ? vec3(1, 1, 1) : vec3(1, 0, 0), 1);
        }
    }
}
