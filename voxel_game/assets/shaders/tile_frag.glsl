#version 330 core

in vec3 v_pos;
in vec3 v_nrm;
in vec2 v_tex;

out vec4 col;

uniform vec3 u_selected_tile_pos;
uniform sampler2D u_tilemap_tex;
uniform usampler3D u_tiles_tex;
uniform vec3 u_cam_pos;
uniform vec3 u_cube_dim;

const int MAX_ITER = 128 * 3;
const float EPSILON = 0.0001f;

struct hit_information {
    vec3 pos, nrm;
    int tile_id;
};

struct raycast_information {
    bool hit;
    int steps;
    hit_information hit_info;
};

struct tile_texcoords {
    vec2 px, nx, py, ny, pz, nz;
};

vec3 coord_floor(in vec3 p) {
    return floor(p * textureSize(u_tiles_tex, 0)) / textureSize(u_tiles_tex, 0);
}

int sample_tiles(vec3 p) {
    return int(texture(u_tiles_tex, coord_floor(p)).r);
}

/*
tile_texcoords get_texcoords(int tile_id) {
    tile_texcoords t;
    switch(tile_id) {
        case 1:
            t.px = vec2(0, 0), t.nx = vec2(0, 0);
            t.py = vec2(0, 0), t.ny = vec2(0, 0);
            t.pz = vec2(0, 0), t.nz = vec2(0, 0);
            break;
        case 2:
            t.px = vec2(0, 0), t.nx = vec2(0, 0);
            t.py = vec2(0, 0), t.ny = vec2(0, 0);
            t.pz = vec2(0, 0), t.nz = vec2(0, 0);
            break;
    }
    return t;
}
*/

vec2 get_tex_px(int tile_id) {
    switch(tile_id) {
        case 1: return vec2(2, 1);
        case 2: return vec2(1, 0);
        case 3: return vec2(2, 0);
        case 4: return vec2(0, 2);
    }
    return vec2(0, 0);
}
vec2 get_tex_nx(int tile_id) {
    switch(tile_id) {
        case 1: return vec2(2, 1);
        case 2: return vec2(1, 0);
        case 3: return vec2(2, 0);
        case 4: return vec2(0, 2);
    }
    return vec2(0, 0);
}
vec2 get_tex_py(int tile_id) {
    switch(tile_id) {
        case 1: return vec2(2, 1);
        case 2: return vec2(0, 0);
        case 3: return vec2(2, 0);
        case 4: return vec2(1, 2);
    }
    return vec2(0, 0);
}
vec2 get_tex_ny(int tile_id) {
    switch(tile_id) {
        case 1: return vec2(2, 1);
        case 2: return vec2(2, 0);
        case 3: return vec2(2, 0);
        case 4: return vec2(1, 2);
    }
    return vec2(0, 0);
}
vec2 get_tex_pz(int tile_id) {
    switch(tile_id) {
        case 1: return vec2(2, 1);
        case 2: return vec2(1, 0);
        case 3: return vec2(2, 0);
        case 4: return vec2(0, 2);
    }
    return vec2(0, 0);
}
vec2 get_tex_nz(int tile_id) {
    switch(tile_id) {
        case 1: return vec2(2, 1);
        case 2: return vec2(1, 0);
        case 3: return vec2(2, 0);
        case 4: return vec2(0, 2);
    }
    return vec2(0, 0);
}

vec3 space_scale = vec3(1) / u_cube_dim;
vec3 space_offset = vec3(0.5, 0.5, 0.5);

void raycast(vec3 ray_origin, vec3 ray_dir, inout raycast_information i) {
    i.hit = false;
    i.hit_info.pos = ray_origin;
    i.hit_info.tile_id = sample_tiles(i.hit_info.pos);
    if (i.hit_info.tile_id != 0) {
        i.hit_info.nrm = vec3(0, 0, 0);
        i.hit = true;
        return;
    }
    vec3 ray_p = floor(ray_origin / space_scale) * space_scale;
    vec3 ray_d = ray_p - ray_origin;
    vec3 ray_step = -space_scale;
    if (ray_dir.x > 0)
        ray_d.x += space_scale.x, ray_step.x *= -1;
    if (ray_dir.y > 0)
        ray_d.y += space_scale.y, ray_step.y *= -1;
    if (ray_dir.z > 0)
        ray_d.z += space_scale.z, ray_step.z *= -1;
    i.steps = 0;
    float slope_xy = ray_dir.y / ray_dir.x, slope_xz = ray_dir.z / ray_dir.x;
    float slope_yx = ray_dir.x / ray_dir.y, slope_yz = ray_dir.z / ray_dir.y;
    float slope_zx = ray_dir.x / ray_dir.z, slope_zy = ray_dir.y / ray_dir.z;
    if (ray_dir.x == 0 || ray_dir.y == 0 || ray_dir.z == 0)
        return;
    vec3 to_travel_x = vec3(ray_d.x, slope_xy * ray_d.x, slope_xz * ray_d.x);
    vec3 to_travel_y = vec3(slope_yx * ray_d.y, ray_d.y, slope_yz * ray_d.y);
    vec3 to_travel_z = vec3(slope_zx * ray_d.z, slope_zy * ray_d.z, ray_d.z);
    while (i.steps < MAX_ITER) {
        while (i.steps < MAX_ITER &&
            to_travel_x.x * ray_step.x < to_travel_y.x * ray_step.x && 
            to_travel_x.x * ray_step.x < to_travel_z.x * ray_step.x) {
            i.hit_info.pos += to_travel_x;
            // if (i.hit_info.pos.x < 0 || i.hit_info.pos.y < 0 || i.hit_info.pos.z < 0 || 
            //     i.hit_info.pos.x > 1 || i.hit_info.pos.y > 1 || i.hit_info.pos.z > 1)
            //     return;
            i.hit_info.tile_id = sample_tiles(i.hit_info.pos + vec3(ray_step.x * EPSILON, 0, 0));
            if (i.hit_info.tile_id != 0) {
                i.hit_info.nrm = vec3(sign(-ray_step.x), 0, 0);
                i.hit = true;
                return;
            }
            to_travel_y -= to_travel_x;
            to_travel_z -= to_travel_x;
            to_travel_x = vec3(
                ray_step.x,
                slope_xy * ray_step.x,
                slope_xz * ray_step.x);
            ++i.steps;
        }
        while (i.steps < MAX_ITER &&
            to_travel_y.y * ray_step.y < to_travel_x.y * ray_step.y && 
            to_travel_y.y * ray_step.y < to_travel_z.y * ray_step.y) {
            i.hit_info.pos += to_travel_y;
            // if (i.hit_info.pos.x < 0 || i.hit_info.pos.y < 0 || i.hit_info.pos.z < 0 || 
            //     i.hit_info.pos.x > 1 || i.hit_info.pos.y > 1 || i.hit_info.pos.z > 1)
            //     return;
            i.hit_info.tile_id = sample_tiles(i.hit_info.pos + vec3(0, ray_step.y * EPSILON, 0));
            if (i.hit_info.tile_id != 0) {
                i.hit_info.nrm = vec3(0, sign(-ray_step.y), 0);
                i.hit = true;
                return;
            }
            to_travel_x -= to_travel_y;
            to_travel_z -= to_travel_y;
            to_travel_y = vec3(
                slope_yx * ray_step.y,
                ray_step.y,
                slope_yz * ray_step.y);
            ++i.steps;
        }
        while (i.steps < MAX_ITER &&
            to_travel_z.z * ray_step.z < to_travel_y.z * ray_step.z && 
            to_travel_z.z * ray_step.z < to_travel_x.z * ray_step.z) {
            i.hit_info.pos += to_travel_z;
            // if (i.hit_info.pos.x < 0 || i.hit_info.pos.y < 0 || i.hit_info.pos.z < 0 || 
            //     i.hit_info.pos.x > 1 || i.hit_info.pos.y > 1 || i.hit_info.pos.z > 1)
            //     return;
            i.hit_info.tile_id = sample_tiles(i.hit_info.pos + vec3(0, 0, ray_step.z * EPSILON));
            if (i.hit_info.tile_id != 0) {
                i.hit_info.nrm = vec3(0, 0, sign(-ray_step.z));
                i.hit = true;
                return;
            }
            to_travel_x -= to_travel_z;
            to_travel_y -= to_travel_z;
            to_travel_z = vec3(
                slope_zx * ray_step.z,
                slope_zy * ray_step.z,
                ray_step.z);
            ++i.steps;
        }
        ++i.steps;
    }
}

float sq(float x) {return x * x;}

void main() {
    vec3 frag_pos = v_pos.xyz * space_scale + space_offset;
    vec3 cam_pos = u_cam_pos * space_scale + space_offset;
    vec3 cam_diff = frag_pos - cam_pos;

    vec3 ray_origin = cam_pos;
    // if (cam_pos.x < 0 || cam_pos.y < 0 || cam_pos.z < 0 || 
    //     cam_pos.x > 1 || cam_pos.y > 1 || cam_pos.z > 1)
    //     ray_origin = frag_pos;
    vec3 ray_dir = cam_diff;

    raycast_information cam_raycast;
    raycast(ray_origin, ray_dir, cam_raycast);
    if (!cam_raycast.hit) discard;

    vec3 light_dir = normalize(vec3(1, -3, -2));

    raycast_information light_raycast;
    raycast(cam_raycast.hit_info.pos + cam_raycast.hit_info.nrm * space_scale * EPSILON, -light_dir, light_raycast);
    float light = max(dot(cam_raycast.hit_info.nrm, -light_dir), 0.1) * 2 * max(float(!light_raycast.hit), 0.3);

    vec3 p = floor(cam_raycast.hit_info.pos * u_cube_dim) / u_cube_dim;
    vec3 c = (cam_raycast.hit_info.pos - p) * u_cube_dim;

    vec2 face_coord = vec2(0), face_tex = vec2(0);
    // tile_texcoords texcoords = get_texcoords(cam_raycast.hit_info.tile_id);
    vec3 hit_tan, hit_bitan;
    float ao = 0;
    if (cam_raycast.hit_info.nrm.x != 0) {
        face_coord = c.zy;
        face_coord.y = 1 - face_coord.y;
        hit_tan = vec3(0, 1, 0);
        hit_bitan = vec3(0, 0, 1);
        ao += float(sample_tiles(cam_raycast.hit_info.pos + (cam_raycast.hit_info.nrm * 0.5 - hit_tan) * space_scale) != 0) * face_coord.y * 0.25;
        ao += float(sample_tiles(cam_raycast.hit_info.pos + (cam_raycast.hit_info.nrm * 0.5 + hit_tan) * space_scale) != 0) * (1 - face_coord.y) * 0.25;
        ao += float(sample_tiles(cam_raycast.hit_info.pos + (cam_raycast.hit_info.nrm * 0.5 - hit_bitan) * space_scale) != 0) * (1 - face_coord.x) * 0.25;
        ao += float(sample_tiles(cam_raycast.hit_info.pos + (cam_raycast.hit_info.nrm * 0.5 + hit_bitan) * space_scale) != 0) * face_coord.x * 0.25;
        if (cam_raycast.hit_info.nrm.x > 0) face_tex = face_coord + get_tex_px(cam_raycast.hit_info.tile_id);
        else face_tex = face_coord + get_tex_nx(cam_raycast.hit_info.tile_id);
    } else if (cam_raycast.hit_info.nrm.y != 0) {
        face_coord = c.xz;
        hit_tan = vec3(1, 0, 0);
        hit_bitan = vec3(0, 0, 1);
        ao += float(sample_tiles(cam_raycast.hit_info.pos + (cam_raycast.hit_info.nrm * 0.5 - hit_tan) * space_scale) != 0) * (1 - face_coord.x) * 0.25;
        ao += float(sample_tiles(cam_raycast.hit_info.pos + (cam_raycast.hit_info.nrm * 0.5 + hit_tan) * space_scale) != 0) * face_coord.x * 0.25;
        ao += float(sample_tiles(cam_raycast.hit_info.pos + (cam_raycast.hit_info.nrm * 0.5 - hit_bitan) * space_scale) != 0) * (1 - face_coord.y) * 0.25;
        ao += float(sample_tiles(cam_raycast.hit_info.pos + (cam_raycast.hit_info.nrm * 0.5 + hit_bitan) * space_scale) != 0) * face_coord.y * 0.25;
        if (cam_raycast.hit_info.nrm.y > 0) face_tex = face_coord + get_tex_py(cam_raycast.hit_info.tile_id);
        else face_tex = face_coord + get_tex_ny(cam_raycast.hit_info.tile_id);
    } else if (cam_raycast.hit_info.nrm.z != 0) {
        face_coord = c.xy;
        face_coord.y = 1 - face_coord.y;
        hit_tan = vec3(1, 0, 0);
        hit_bitan = vec3(0, 1, 0);
        ao += float(sample_tiles(cam_raycast.hit_info.pos + (cam_raycast.hit_info.nrm * 0.5 - hit_tan) * space_scale) != 0) * (1 - face_coord.x) * 0.25;
        ao += float(sample_tiles(cam_raycast.hit_info.pos + (cam_raycast.hit_info.nrm * 0.5 + hit_tan) * space_scale) != 0) * face_coord.x * 0.25;
        ao += float(sample_tiles(cam_raycast.hit_info.pos + (cam_raycast.hit_info.nrm * 0.5 - hit_bitan) * space_scale) != 0) * face_coord.y * 0.25;
        ao += float(sample_tiles(cam_raycast.hit_info.pos + (cam_raycast.hit_info.nrm * 0.5 + hit_bitan) * space_scale) != 0) * (1 - face_coord.y) * 0.25;
        if (cam_raycast.hit_info.nrm.z > 0) face_tex = face_coord + get_tex_pz(cam_raycast.hit_info.tile_id);
        else face_tex = face_coord + get_tex_nz(cam_raycast.hit_info.tile_id);
    }
    ao = 1 - sq(ao) * 4;
    col = vec4(texture(u_tilemap_tex, face_tex / 128 * 16).rgb * light * 1.5 * ao, 1);
    // col = vec4(coord_floor(cam_raycast.hit_info.pos), 1);
    vec3 floor_selected_tile = coord_floor(u_selected_tile_pos * space_scale + space_offset);
    vec3 floor_cam_hit_tile = coord_floor(cam_raycast.hit_info.pos - cam_raycast.hit_info.nrm * 0.5 * space_scale);

    if (floor_cam_hit_tile == floor_selected_tile) {
        const float selection_outline_width = 0.02;
        if (face_coord.x < selection_outline_width ||
            face_coord.x > 1.0f - selection_outline_width ||
            face_coord.y < selection_outline_width ||
            face_coord.y > 1.0f - selection_outline_width)
        col = vec4(vec3(1)-col.rgb, 1);
    }
}
