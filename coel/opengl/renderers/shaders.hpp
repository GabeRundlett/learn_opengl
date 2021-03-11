#pragma once

namespace opengl { namespace renderer {
    static constexpr const char *const ui_batch_vert = R""(
        #version 440 core

        layout (location = 0) in vec2 a_pos;
        layout (location = 1) in vec2 a_size;
        layout (location = 2) in vec2 a_tex;
        layout (location = 3) in vec4 a_col;

        uniform mat4 u_view_mat;

        out vec2 v_tex;
        out vec2 v_size;
        out vec4 v_col;

        void main() {
            v_tex = a_tex, v_size = a_size, v_col = a_col;
            gl_Position = u_view_mat * vec4(a_pos, 0, 1.0);
        }
    )"";

    static constexpr const char *const ui_batch_frag = R""(
        #version 440 core

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
    )"";

    static constexpr const char *const text_batch_vert = R""(
        #version 440 core

        layout (location = 0) in vec2 a_pos;
        layout (location = 1) in vec2 a_tex;
        layout (location = 2) in vec4 a_col;
        layout (location = 3) in float a_font_size;

        uniform mat4 u_view_mat;

        out vec2 v_tex;
        out vec4 v_col;
        out float v_font_size;

        void main() {
            v_tex = a_tex, v_col = a_col, v_font_size = a_font_size;
            gl_Position = u_view_mat * vec4(a_pos, 0, 1.0);
        }
    )"";

    static constexpr const char *const text_batch_frag = R""(
        #version 440 core

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
            if (opacity < 0.01) discard;
            frag_col = v_col;
            frag_col.w *= opacity;
        }
    )"";

    static constexpr const char *const deferred3d_vert = R""(
        #version 440 core

        layout(location = 0) in vec2 a_pos;

        out vec2 v_tex;

        void main() {
            v_tex = a_pos * 0.5 + 0.5;
            gl_Position = vec4(a_pos, 0, 1);
        }
    )"";

    static constexpr const char *const deferred3d_frag = R""(
        #version 440 core

        in vec2 v_tex;
        out vec4 col;

        uniform sampler2D u_frame_pos_tex;
        uniform sampler2D u_frame_nrm_tex;
        uniform sampler2D u_frame_col_tex;

        uniform float u_gamma;
        uniform float u_exposure;
        uniform vec3 u_cam_pos;

        void main() {
            vec3 hdr_color = texture(u_frame_col_tex, v_tex).rgb;
            vec3 position = texture(u_frame_pos_tex, v_tex).xyz;
            vec3 normal = texture(u_frame_nrm_tex, v_tex).rgb;

            vec3 col_mapped = vec3(1.0) - exp(-diff * u_exposure);
            col_mapped = pow(col_mapped, vec3(1.0f / u_gamma));

            col = vec4(col_mapped, 1);
        }
    )"";
}} // namespace opengl::renderer
