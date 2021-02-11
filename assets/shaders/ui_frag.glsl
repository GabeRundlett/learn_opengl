#version 430 core

in vec2 v_tex;
in vec2 v_size;
in vec4 v_col;

out vec4 frag_col;

void main() {
    vec2 pos = v_tex * v_size;
    float r = 0;
    frag_col = v_col;

    if (pos.x < r) {
        if (pos.y < r) {
            vec2 p = pos - vec2(r, r);
            if (p.x * p.x + p.y * p.y > r * r)
                discard;
        } else if (pos.y > v_size.y - r) {
            vec2 p = pos - vec2(r, v_size.y - r);
            if (p.x * p.x + p.y * p.y > r * r)
                discard;
        }
    } else if (pos.x > v_size.x - r) {
        if (pos.y < r) {
            vec2 p = pos - vec2(v_size.x - r, r);
            if (p.x * p.x + p.y * p.y > r * r)
                discard;
        } else if (pos.y > v_size.y - r) {
            vec2 p = pos - vec2(v_size.x - r, v_size.y - r);
            if (p.x * p.x + p.y * p.y > r * r)
                discard;
        }
    }
}