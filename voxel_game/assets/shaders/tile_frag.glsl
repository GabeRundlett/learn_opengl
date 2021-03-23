#version 450

in vec3 v_pos;
in vec3 v_nrm;
in vec2 v_tex;

out vec4 frag_col;

uniform vec3 u_selected_tile_pos;
uniform vec3 u_selected_tile_nrm;
uniform sampler2D u_tilemap_tex;
uniform usampler3D u_tiles_tex;
uniform vec3 u_cube_pos;
uniform vec3 u_cube_dim;
uniform vec3 u_cam_pos;

const uint tile_id_none = 0u;
const uint tile_id_dirt = 1u;
const uint tile_id_grass = 2u;
const uint tile_id_sand = 3u;
const uint tile_id_gravel = 4u;
const uint tile_id_stone = 5u;
const uint tile_id_stone_cracked = 6u;
const uint tile_id_stone_cobbled = 7u;
const uint tile_id_leaves = 8u;
const uint tile_id_log = 9u;

const vec3 sun_col = vec3(1, 0.8, 0.7) * 20;
const vec3 sky_col = vec3(2.81f, 2.01f, 5.32f);
const vec3 sun_dir = normalize(vec3(-2, -4, -1.3));

vec2 get_tex_px(uint tile_id) {
    switch(tile_id) {
        case tile_id_dirt:          return vec2(2, 0);
        case tile_id_grass:         return vec2(1, 0);
        case tile_id_sand:          return vec2(3, 0);
        case tile_id_gravel:        return vec2(3, 1);
        case tile_id_stone:         return vec2(2, 1);
        case tile_id_stone_cracked: return vec2(1, 1);
        case tile_id_stone_cobbled: return vec2(0, 1);
        case tile_id_leaves:        return vec2(2, 2);
        case tile_id_log:           return vec2(0, 2);
    }
    return vec2(0, 0);
}
vec2 get_tex_nx(uint tile_id) {
    switch(tile_id) {
        case tile_id_dirt:          return vec2(2, 0);
        case tile_id_grass:         return vec2(1, 0);
        case tile_id_sand:          return vec2(3, 0);
        case tile_id_gravel:        return vec2(3, 1);
        case tile_id_stone:         return vec2(2, 1);
        case tile_id_stone_cracked: return vec2(1, 1);
        case tile_id_stone_cobbled: return vec2(0, 1);
        case tile_id_leaves:        return vec2(2, 2);
        case tile_id_log:           return vec2(0, 2);
    }
    return vec2(0, 0);
}
vec2 get_tex_py(uint tile_id) {
    switch(tile_id) {
        case tile_id_dirt:          return vec2(2, 0);
        case tile_id_grass:         return vec2(0, 0);
        case tile_id_sand:          return vec2(3, 0);
        case tile_id_gravel:        return vec2(3, 1);
        case tile_id_stone:         return vec2(2, 1);
        case tile_id_stone_cracked: return vec2(1, 1);
        case tile_id_stone_cobbled: return vec2(0, 1);
        case tile_id_leaves:        return vec2(2, 2);
        case tile_id_log:           return vec2(1, 2);
    }
    return vec2(0, 0);
}
vec2 get_tex_ny(uint tile_id) {
    switch(tile_id) {
        case tile_id_dirt:          return vec2(2, 0);
        case tile_id_grass:         return vec2(2, 0);
        case tile_id_sand:          return vec2(3, 0);
        case tile_id_gravel:        return vec2(3, 1);
        case tile_id_stone:         return vec2(2, 1);
        case tile_id_stone_cracked: return vec2(1, 1);
        case tile_id_stone_cobbled: return vec2(0, 1);
        case tile_id_leaves:        return vec2(2, 2);
        case tile_id_log:           return vec2(1, 2);
    }
    return vec2(0, 0);
}
vec2 get_tex_pz(uint tile_id) {
    switch(tile_id) {
        case tile_id_dirt:          return vec2(2, 0);
        case tile_id_grass:         return vec2(1, 0);
        case tile_id_sand:          return vec2(3, 0);
        case tile_id_gravel:        return vec2(3, 1);
        case tile_id_stone:         return vec2(2, 1);
        case tile_id_stone_cracked: return vec2(1, 1);
        case tile_id_stone_cobbled: return vec2(0, 1);
        case tile_id_leaves:        return vec2(2, 2);
        case tile_id_log:           return vec2(0, 2);
    }
    return vec2(0, 0);
}
vec2 get_tex_nz(uint tile_id) {
    switch(tile_id) {
        case tile_id_dirt:          return vec2(2, 0);
        case tile_id_grass:         return vec2(1, 0);
        case tile_id_sand:          return vec2(3, 0);
        case tile_id_gravel:        return vec2(3, 1);
        case tile_id_stone:         return vec2(2, 1);
        case tile_id_stone_cracked: return vec2(1, 1);
        case tile_id_stone_cobbled: return vec2(0, 1);
        case tile_id_leaves:        return vec2(2, 2);
        case tile_id_log:           return vec2(0, 2);
    }
    return vec2(0, 0);
}

struct raycast_config {
    vec3 origin, dir;
    uint max_iter;
    float max_dist;
};

struct raycast_result {
    ivec3 tile_index;
    uint total_steps;
    bool hit_surface, is_y, is_z;
};

uint get_tile(ivec3 coord) {
    return texture(u_tiles_tex, vec3(coord) / u_cube_dim).r;
}

raycast_result raycast(in raycast_config config) {
    raycast_result result;

    result.tile_index = ivec3(config.origin);
    result.total_steps = 0u;
    result.hit_surface = false;

    float delta_dist_xy = config.dir.y == 0 ? 0 : (config.dir.x == 0 ? 1 : abs(1.0f / config.dir.x));
    float delta_dist_xz = config.dir.z == 0 ? 0 : (config.dir.x == 0 ? 1 : abs(1.0f / config.dir.x));

    float delta_dist_yx = config.dir.x == 0 ? 0 : (config.dir.y == 0 ? 1 : abs(1.0f / config.dir.y));
    float delta_dist_yz = config.dir.z == 0 ? 0 : (config.dir.y == 0 ? 1 : abs(1.0f / config.dir.y));

    float delta_dist_zx = config.dir.x == 0 ? 0 : (config.dir.z == 0 ? 1 : abs(1.0f / config.dir.z));
    float delta_dist_zy = config.dir.y == 0 ? 0 : (config.dir.z == 0 ? 1 : abs(1.0f / config.dir.z));

    float to_side_dist_xy;
    float to_side_dist_xz;

    float to_side_dist_yx;
    float to_side_dist_yz;

    float to_side_dist_zx;
    float to_side_dist_zy;

    ivec3 ray_step;

    if (config.dir.x < 0) {
        ray_step.x = -1;
        to_side_dist_xy = (config.origin.x - result.tile_index.x) * delta_dist_xy;
        to_side_dist_xz = (config.origin.x - result.tile_index.x) * delta_dist_xz;
    } else {
        ray_step.x = 1;
        to_side_dist_xy = (result.tile_index.x + 1.0f - config.origin.x) * delta_dist_xy;
        to_side_dist_xz = (result.tile_index.x + 1.0f - config.origin.x) * delta_dist_xz;
    }
    if (config.dir.y < 0) {
        ray_step.y = -1;
        to_side_dist_yx = (config.origin.y - result.tile_index.y) * delta_dist_yx;
        to_side_dist_yz = (config.origin.y - result.tile_index.y) * delta_dist_yz;
    } else {
        ray_step.y = 1;
        to_side_dist_yx = (result.tile_index.y + 1.0f - config.origin.y) * delta_dist_yx;
        to_side_dist_yz = (result.tile_index.y + 1.0f - config.origin.y) * delta_dist_yz;
    }
    if (config.dir.z < 0) {
        ray_step.z = -1;
        to_side_dist_zx = (config.origin.z - result.tile_index.z) * delta_dist_zx;
        to_side_dist_zy = (config.origin.z - result.tile_index.z) * delta_dist_zy;
    } else {
        ray_step.z = 1;
        to_side_dist_zx = (result.tile_index.z + 1.0f - config.origin.z) * delta_dist_zx;
        to_side_dist_zy = (result.tile_index.z + 1.0f - config.origin.z) * delta_dist_zy;
    }

    while (result.total_steps < config.max_iter) {
        if (get_tile(result.tile_index) != tile_id_none) {
            result.hit_surface = true;
            break;
        }

        if (result.tile_index.x < 0 || result.tile_index.x > u_cube_dim.x ||
            result.tile_index.y < 0 || result.tile_index.y > u_cube_dim.y ||
            result.tile_index.z < 0 || result.tile_index.z > u_cube_dim.z) {
            break;
        }

        if (to_side_dist_xy < to_side_dist_yx) {
            if (to_side_dist_xz < to_side_dist_zx) {
                to_side_dist_xy += delta_dist_xy;
                to_side_dist_xz += delta_dist_xz;
                result.tile_index.x += ray_step.x;
                result.is_y = false;
                result.is_z = false;
            } else {
                to_side_dist_zx += delta_dist_zx;
                to_side_dist_zy += delta_dist_zy;
                result.tile_index.z += ray_step.z;
                result.is_y = false;
                result.is_z = true;
            }
        } else {
            if (to_side_dist_yz < to_side_dist_zy) {
                to_side_dist_yx += delta_dist_yx;
                to_side_dist_yz += delta_dist_yz;
                result.tile_index.y += ray_step.y;
                result.is_y = true;
                result.is_z = false;
            } else {
                to_side_dist_zx += delta_dist_zx;
                to_side_dist_zy += delta_dist_zy;
                result.tile_index.z += ray_step.z;
                result.is_y = false;
                result.is_z = true;
            }
        }

        ++result.total_steps;
    }

    return result;
}

struct surface_details {
    vec3 pos, nrm, uvw, col;
    vec2 face_uv, tile_uv;
    uint tile_id;
};

surface_details get_surface_details(in raycast_config config, in raycast_result result) {
    surface_details surface;
    surface.pos = result.tile_index;

    float slope_xy = config.dir.x / config.dir.y;
    float slope_xz = config.dir.x / config.dir.z;

    float slope_yx = config.dir.y / config.dir.x;
    float slope_yz = config.dir.y / config.dir.z;

    float slope_zx = config.dir.z / config.dir.x;
    float slope_zy = config.dir.z / config.dir.y;

    if (result.is_y) {
        if (config.dir.y < 0) {
            surface.pos.y += 1;
            surface.nrm = vec3(0, 1, 0);
        } else {
            surface.nrm = vec3(0, -1, 0);
        }
        surface.pos = vec3(
            config.origin.x + (surface.pos.y - config.origin.y) * slope_xy,
            surface.pos.y,
            config.origin.z + (surface.pos.y - config.origin.y) * slope_zy
        );
        surface.uvw = surface.pos - result.tile_index;
        surface.face_uv = vec2(surface.uvw.x, surface.uvw.z);
    } else if (result.is_z) {
        if (config.dir.z < 0) {
            surface.pos.z += 1;
            surface.nrm = vec3(0, 0, 1);
        } else {
            surface.nrm = vec3(0, 0, -1);
        }
        surface.pos = vec3(
            config.origin.x + (surface.pos.z - config.origin.z) * slope_xz,
            config.origin.y + (surface.pos.z - config.origin.z) * slope_yz,
            surface.pos.z
        );
        surface.uvw = surface.pos - result.tile_index;
        surface.face_uv = vec2(surface.uvw.x, 1 - surface.uvw.y);
    } else {
        if (config.dir.x < 0) {
            surface.pos.x += 1;
            surface.nrm = vec3(1, 0, 0);
        } else {
            surface.nrm = vec3(-1, 0, 0);
        }
        surface.pos = vec3(
            surface.pos.x,
            config.origin.y + (surface.pos.x - config.origin.x) * slope_yx,
            config.origin.z + (surface.pos.x - config.origin.x) * slope_zx
        );
        surface.uvw = surface.pos - result.tile_index;
        surface.face_uv = vec2(surface.uvw.z, 1 - surface.uvw.y);
    }
    
    surface.tile_uv = surface.face_uv;
    surface.tile_id = get_tile(result.tile_index);

    if (surface.nrm.x > 0.5) {
        surface.tile_uv += get_tex_px(surface.tile_id);
    } else if (surface.nrm.x < -0.5) {
        surface.tile_uv += get_tex_nx(surface.tile_id);
    } else if (surface.nrm.y > 0.5) {
        surface.tile_uv += get_tex_py(surface.tile_id);
    } else if (surface.nrm.y < -0.5) {
        surface.tile_uv += get_tex_ny(surface.tile_id);
    } else if (surface.nrm.z > 0.5) {
        surface.tile_uv += get_tex_pz(surface.tile_id);
    } else if (surface.nrm.z < -0.5) {
        surface.tile_uv += get_tex_nz(surface.tile_id);
    }

    if (result.hit_surface) {
        surface.col = texture(u_tilemap_tex, surface.tile_uv / 8).rgb;
    } else {
        surface.col = vec3(1);
    }

    return surface;
}

float rand(float seed) {
    return fract(sin(dot(vec2(seed) , vec2(12.9898,78.233))) * 43758.5453) * 2 - 1;
}

vec3 rand(vec3 seed) {
    return vec3(
        cos(rand(seed.x) * 3.14),
        cos(rand(seed.y) * 3.14),
        cos(rand(seed.z) * 3.14)
    );
}

vec3 random_nrm(vec3 nrm, vec3 seed, float variance) {
    return nrm + rand(seed) * variance;
}

void main() {
    vec3 cam_pos = u_cam_pos + vec3(u_cube_dim) * 0.5;
    vec3 cam_diff = (v_pos + vec3(u_cube_dim) * 0.5 - u_cube_pos) - cam_pos;
    if (cam_pos.x < 0 || cam_pos.x > u_cube_dim.x ||
        cam_pos.y < 0 || cam_pos.y > u_cube_dim.y ||
        cam_pos.z < 0 || cam_pos.z > u_cube_dim.z) {
        cam_pos += cam_diff;
    }

    raycast_config camray_conf;
    camray_conf.origin = cam_pos,
    camray_conf.dir = normalize(cam_diff),
    camray_conf.max_iter = 128u;

    raycast_result camray = raycast(camray_conf);
    surface_details surface = get_surface_details(camray_conf, camray);

    // raycast_config sunray_conf;
    // sunray_conf.origin = surface.pos,
    // sunray_conf.dir = -sun_dir,
    // sunray_conf.max_iter = 256u;
    // raycast_result sunray = raycast(sunray_conf);

    frag_col = vec4(surface.col * sky_col * sun_col * clamp(dot(-sun_dir, surface.nrm), 0.2f, 1.0f), 1);
}
