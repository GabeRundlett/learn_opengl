#version 440

in vec2 v_pos;
out vec4 col;

uniform vec4 u_glyph_edges[200];
uniform int u_glyph_edge_types[100];
uniform int u_glyph_edge_count;
uniform float u_glyph_scale;

float line_dist(in vec2 v, in vec2 w, in vec2 p) {
    float l2 = dot(w - v, w - v);
    if (l2 == 0.0) return distance(p, v);
    float t = clamp(dot(p - v, w - v) / l2, 0, 1);
    vec2 j = v + t * (w - v);
    return distance(p, j);
}

void main() {
    float dist = 4.0f;

    for (int i = 0; i < u_glyph_edge_count * 2; i += 2) {
        vec4 edge_p1 = u_glyph_edges[i];
        vec4 edge_p2 = u_glyph_edges[i + 1];

        float scale = u_glyph_scale * 0.5;
        
        vec2 start = edge_p1.xy * scale;
        vec2 end = edge_p1.zw * scale;
        vec2 c0 = edge_p2.xy * scale;
        vec2 c1 = edge_p2.zw * scale;

        dist = min(dist, line_dist(start, end, v_pos));
    }

    col = vec4(vec3((dist - 0.02) * 100), 1);
}
