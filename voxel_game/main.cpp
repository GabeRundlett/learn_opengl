#include <coel/application.hpp>
// #include "player.hpp"
// #include "chunk.hpp"
#include "player2d.hpp"
#include "chunk2d.hpp"

using namespace glm;

class voxel_game : public coel::application {
    opengl::shader_program tile_shader = opengl::shader_program(
        {.filepath = "voxel_game/assets/shaders/tile_vert.glsl"},
        {.filepath = "voxel_game/assets/shaders/tile_frag.glsl"});

    opengl::shader_uniform
        u_player_pos,
        u_mouse_pos,
        u_ray_pos,
        u_ray_nrm,
        u_ray_hit,
        u_scale,
        u_aspect,
        u_tilemap_tex,
        u_tiles_tex;

    opengl::texture2d<> tilemap_tex = opengl::texture2d<>({
        .filepath = "voxel_game/assets/textures/tilemap.png",
        .gl_format = GL_RGBA,
        .filter = {.min = GL_NEAREST, .max = GL_NEAREST},
    });

    glm::uvec2 grid_dim = {128, 128};
    std::vector<glm::u8vec4> tiles = std::vector<glm::u8vec4>(grid_dim.x * grid_dim.y);
    opengl::texture2d<glm::u8vec4> tiles_tex;

    player2d player;
    float zoom_scale = 0.1f;

    static inline constexpr std::array quad_vertices = {
        // clang-format off
        -1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
         1.0f, -1.0f,
        // clang-format on
    };
    opengl::vertex_array quad_vao;
    opengl::vertex_buffer quad_vbo = opengl::vertex_buffer(quad_vertices.data(), quad_vertices.size() * sizeof(quad_vertices[0]));

    glm::vec2 screen_to_world(glm::vec2 p) {
        return glm::vec2((p.x / frame_dim.x - 0.5f) * frame_dim.x / frame_dim.y, p.y / frame_dim.y - 0.5f) / zoom_scale * 2.0f - player.pos;
    }

    void shader_init() {
        u_player_pos = tile_shader.find_uniform("u_player_pos");
        u_mouse_pos = tile_shader.find_uniform("u_mouse_pos");

        u_ray_pos = tile_shader.find_uniform("u_ray_pos");
        u_ray_nrm = tile_shader.find_uniform("u_ray_nrm");
        u_ray_hit = tile_shader.find_uniform("u_ray_hit");

        u_scale = tile_shader.find_uniform("u_scale");
        u_aspect = tile_shader.find_uniform("u_aspect");
        u_tilemap_tex = tile_shader.find_uniform("u_tilemap_tex");
        u_tiles_tex = tile_shader.find_uniform("u_tiles_tex");

        tile_shader.bind();
        opengl::shader_program::send(u_scale, zoom_scale);
        opengl::shader_program::send(u_player_pos, player.pos);
        opengl::shader_program::send(u_aspect, 1.0f * frame_dim.x / frame_dim.y);
        opengl::shader_program::send(u_mouse_pos, screen_to_world(mouse_pos));
    }

    static inline constexpr auto MAX_ITER = 1000;
    static inline constexpr auto THRESHOLD = 200;
    static inline constexpr auto EPSILON = 0.001f;

    int sample_tiles(vec2 p) {
        int x = (int(p.x) - int(p.x < 0)) % grid_dim.x;
        int y = (int(p.y) - int(p.y < 0)) % grid_dim.y;
        auto &tile = tiles[x + y * grid_dim.x];
        return tile.r;
    }

    bool intersect(vec2 p) {
        return sample_tiles(p) > THRESHOLD;
    }

    vec2 floor(vec2 p) {
        return vec2(int(p.x) - int(p.x < 0), int(p.y) - int(p.y < 0));
    }

    bool raycast(vec2 ray_origin, vec2 &ray_pos, vec2 &ray_nrm, vec2 ray_dir) {
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

  public:
    voxel_game() : coel::application({1200, 900}, "Voxel Game") {
        show_debug_menu = true;
        use_vsync(false);
        shader_init();

        quad_vao.bind();
        opengl::vertex_array::set_layout<glm::vec2>();

        for (uint32_t y = 0; y < grid_dim.y; ++y) {
            for (uint32_t x = 0; x < grid_dim.x; ++x) {
                auto &tile = tiles[x + y * grid_dim.x];
                float fx = 3.14f / grid_dim.x * x;
                float fy = 3.14f / grid_dim.y * y;
                std::uint8_t v1 = std::uint8_t(sin(fx) * 300 + sin(fy + cos(fx * 4) * 0.2) * 300);
                tile = {v1, 0, 0, 255};
            }
        }

        tiles_tex.bind();
        tiles_tex.regenerate({
            .data = tiles.data(),
            .dim = {grid_dim.x, grid_dim.y},
            .gl_format = GL_R32UI,
            .data_format = GL_RED_INTEGER,
            .data_type = GL_UNSIGNED_INT,
            .wrap = {.s = GL_REPEAT, .t = GL_REPEAT},
            .filter = {.min = GL_NEAREST, .max = GL_NEAREST},
        });
    }

    void on_update(coel::duration elapsed) {
        player.update(elapsed);

        tile_shader.bind();
        vec2 sm = screen_to_world(mouse_pos);
        opengl::shader_program::send(u_player_pos, player.pos);
        opengl::shader_program::send(u_mouse_pos, sm);

        vec2 ray_origin = -player.pos, ray_pos = {0, 0}, ray_nrm = {0, 0};
        int ray_hit = (int)raycast(ray_origin, ray_pos, ray_nrm, sm - ray_origin);

        opengl::shader_program::send(u_ray_pos, ray_pos);
        opengl::shader_program::send(u_ray_nrm, ray_nrm);
        opengl::shader_program::send(u_ray_hit, ray_hit);
    }

    void on_draw() {
        glClear(GL_COLOR_BUFFER_BIT);

        tile_shader.bind();
        opengl::shader_program::send(u_tilemap_tex, 0);
        opengl::shader_program::send(u_tiles_tex, 1);
        tilemap_tex.bind(0);
        tiles_tex.bind(1);

        quad_vao.bind();
        glDrawArrays(GL_QUADS, 0, 4);
    }

    void on_key(const coel::key_event &e) {
        player.key_press(e);
        if (e.action == GLFW_PRESS) {
            switch (e.key) {
            case GLFW_KEY_R:
                tile_shader = opengl::shader_program(
                    {.filepath = "voxel_game/assets/shaders/tile_vert.glsl"},
                    {.filepath = "voxel_game/assets/shaders/tile_frag.glsl"});
                shader_init();
                break;
            }
        }
    }

    void on_resize() {
        tile_shader.bind();
        opengl::shader_program::send(u_aspect, 1.0f * frame_dim.x / frame_dim.y);
    }

    void on_mouse_scroll(const glm::dvec2 offset) {
        zoom_scale += (float)offset.y * zoom_scale * 0.1f;
        if (zoom_scale < 0.001f)
            zoom_scale = 0.001f;
        tile_shader.bind();
        opengl::shader_program::send(u_scale, zoom_scale);
    }
};

int main() {
    voxel_game game;
    if (!game)
        return -1;
    game.resize();
    while (game.update()) {
    }
}
