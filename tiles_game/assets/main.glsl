#coel_shader[type: vert, name: "my vert shader"]
#version 440 core

layout(location = 0) in vec2 a_pos;

void main() {
    gl_Position = vec4(a_pos, 0, 1);
}

#coel_shader[type: frag, name: "my frag shader"]
#version 440 core

uniform usampler2D u_tiles_tex;
uniform sampler2D u_tilemap_tex;
uniform vec2 u_frame_dim;
uniform vec2 u_mouse_pos;
uniform uint u_active_item;

out vec4 frag_col;

vec2 screen_to_world(vec2 p) {
    return (p + vec2((u_frame_dim.y - u_frame_dim.x) * 0.5, 0)) / u_frame_dim.yy;
}

vec2 get_coord(uint tile_id) {
    switch (tile_id) {
        case 1: return vec2(2, 0);
        case 2: return vec2(1, 0);
        case 3: return vec2(0, 0);

        case 4: return vec2(3, 0);
        case 5: return vec2(3, 1);

        case 6: return vec2(2, 1);
        case 7: return vec2(1, 1);
        case 8: return vec2(0, 1);

        case 9: return vec2(2, 2);
        case 10: return vec2(0, 2);
        case 11: return vec2(1, 2);
    }
    return vec2(0);
}

void main() {
    vec2 uv = screen_to_world(gl_FragCoord.xy * vec2(1, -1)) + vec2(0, 1);
    vec2 mouse = screen_to_world(u_mouse_pos);
    vec2 tile_count = textureSize(u_tiles_tex, 0);
    vec2 tile_scale = 1.0f / tile_count;
    vec2 tile_coord = floor(uv * tile_count);
    vec2 mouse_tile_coord = floor(mouse * tile_count) * 2 - tile_coord;
    vec2 tile_uv = uv * tile_count - tile_coord;
    float aspect = float(u_frame_dim.x) / u_frame_dim.y;

    uint tile_id = texture(u_tiles_tex, uv).r;

    if (uv.y > 0.1 && uv.y < 0.9 && uv.x > -0.12 && uv.x < -0.02) {
        frag_col = vec4(1, 0, 1, 1);
    } else {
        frag_col = texture(u_tilemap_tex, (tile_uv + get_coord(tile_id)) / 8);
        const float outline = 0.1;
        if (tile_coord == mouse_tile_coord && (mouse.x < 1 && mouse.x > 0 && mouse.y > 0 && mouse.y < 1) && (
            tile_uv.x < outline || tile_uv.x > 1.0 - outline || 
            tile_uv.y < outline || tile_uv.y > 1.0 - outline
        )) frag_col = texture(u_tilemap_tex, (tile_uv + get_coord(u_active_item)) / 8);
        else if (tile_id == 0) discard;
    }
}
