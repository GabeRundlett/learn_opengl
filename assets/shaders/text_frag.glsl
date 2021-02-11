#version 430 core

in vec2 v_tex;
in vec4 v_col;
in float v_font_size;

out vec4 frag_col;

uniform sampler2D u_text_atlas;

float u_weight = 0.32;
float u_thickness = 0.20;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main() {
    vec3 tex_val = texture(u_text_atlas, v_tex).rgb;
    ivec2 sz = textureSize(u_text_atlas, 0).xy;
    float dx = dFdx(v_tex.x) * sz.x; 
    float dy = dFdy(v_tex.y) * sz.y;
    float toPixels = 8.0 * inversesqrt(dx * dx + dy * dy);
    float sigDist = median(tex_val.r, tex_val.g, tex_val.b);
    float w = fwidth(sigDist) * u_weight + u_thickness / v_font_size / 14.0f;
    float opacity = clamp(smoothstep(0.5 - w, 0.5 + w, sigDist), 0.0f, 1.f);
	// frag_col = mix(vec4(1, 0, 1, 1), v_col, opacity);
    frag_col = v_col;
    frag_col.w *= opacity;
}
