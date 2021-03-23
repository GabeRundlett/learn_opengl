#coel_shader[type: vert, name: "my vert shader"]
#version 330

layout(location = 0) in vec2 a_pos;

void main() {
    gl_Position = vec4(a_pos, 0, 1);
}

#coel_shader[type: frag, name: "my frag shader"]
#version 330

uniform vec2 u_frame_dim;
uniform vec2 u_mouse_pos;

uniform vec2 u_world_size;

uniform int u_point_count;
uniform vec2 u_points[100];
uniform vec2 u_intersection;

uniform sampler2D u_tilemap_tex;
uniform usampler2D u_chunk_tex;

vec2 get_coord(uint tile_id) {
    switch (tile_id) {
        case 1u: return vec2(2, 0);
        case 2u: return vec2(1, 0);
        case 3u: return vec2(0, 0);

        case 4u: return vec2(3, 0);
        case 5u: return vec2(3, 1);

        case 6u: return vec2(2, 1);
        case 7u: return vec2(1, 1);
        case 8u: return vec2(0, 1);

        case 9u: return vec2(2, 2);
        case 10u: return vec2(0, 2);
        case 11u: return vec2(1, 2);
    }
    return vec2(0);
}

out vec4 frag_col;

void main() {
    vec2 uv = vec2(gl_FragCoord.x, u_frame_dim.y - gl_FragCoord.y) / u_frame_dim.y * u_world_size;
    vec2 mp = u_mouse_pos;

    vec2 tile_pos = floor(uv);
    vec2 tile_uv = uv - tile_pos;

    uint tile_id = texture(u_chunk_tex, tile_pos / u_world_size).r;

    frag_col = texture(u_tilemap_tex, (tile_uv + get_coord(tile_id)) / 8);
    if (tile_id == 0u) {
        frag_col = vec4(vec3(0.2), 1);
        if (int((uv.x + uv.y) * 5) % 2 == 0)
            frag_col.w = 0.5;
    }

    const float tile_outline_width = 0.02;
    if (tile_uv.x < tile_outline_width || tile_uv.y < tile_outline_width ||
        tile_uv.x > 1.0 - tile_outline_width || tile_uv.y > 1.0 - tile_outline_width)
        frag_col = vec4(vec3(0.3), 1);
    
    if (uv.x > u_world_size.x) {
        frag_col.rgb *= 0.5;
    }

    float r = 0.08;


    vec2 p = u_intersection;
    if (dot(p - uv, p - uv) < r * r)
        frag_col = vec4(0.5, 0.2, 1, 1);
    
    r = 0.12;
    for (int i = 0; i < u_point_count; ++i) {
        vec2 p = u_points[i];
        if (dot(p - uv, p - uv) < r * r)
            frag_col = vec4(0.5, 0, 1, 1);
    }
}
