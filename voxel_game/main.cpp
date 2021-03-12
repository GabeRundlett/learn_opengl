#include <coel/application.hpp>
// #include "player.hpp"
// #include "chunk.hpp"
#include "player.hpp"

using namespace glm;

class voxel_game : public coel::application {
    opengl::shader_program tile_shader = opengl::shader_program(
        {.filepath = "voxel_game/assets/shaders/tile_vert.glsl"},
        {.filepath = "voxel_game/assets/shaders/tile_frag.glsl"});

    opengl::shader_uniform
        u_view_mat,
        u_proj_mat,
        u_cam_pos,
        u_tilemap_tex,
        u_tiles_tex;

    opengl::texture2d<> tilemap_tex = opengl::texture2d<>({
        .filepath = "voxel_game/assets/textures/tilemap.png",
        .gl_format = GL_RGBA,
        .filter = {.min = GL_NEAREST, .max = GL_NEAREST},
    });

    glm::uvec3 grid_dim = {128, 128, 128};
    std::vector<glm::u8vec4> tiles = std::vector<glm::u8vec4>(grid_dim.x * grid_dim.y * grid_dim.z);
    opengl::texture3d<glm::u8vec4> tiles_tex;

    player3d player;

    struct vertex {
        glm::vec3 pos, nrm;
        glm::vec2 tex;
    };

    // clang-format off
    static inline const std::array cube_vertices = {
        vertex{.pos{-0.5f, -0.5f, -0.5f}, .nrm{ 0.0f,  0.0f, -1.0f}, .tex{0.0f, 0.0f}},
        vertex{.pos{ 0.5f,  0.5f, -0.5f}, .nrm{ 0.0f,  0.0f, -1.0f}, .tex{1.0f, 1.0f}},
        vertex{.pos{ 0.5f, -0.5f, -0.5f}, .nrm{ 0.0f,  0.0f, -1.0f}, .tex{1.0f, 0.0f}},
        vertex{.pos{ 0.5f,  0.5f, -0.5f}, .nrm{ 0.0f,  0.0f, -1.0f}, .tex{1.0f, 1.0f}},
        vertex{.pos{-0.5f, -0.5f, -0.5f}, .nrm{ 0.0f,  0.0f, -1.0f}, .tex{0.0f, 0.0f}},
        vertex{.pos{-0.5f,  0.5f, -0.5f}, .nrm{ 0.0f,  0.0f, -1.0f}, .tex{0.0f, 1.0f}},

        vertex{.pos{-0.5f, -0.5f,  0.5f}, .nrm{ 0.0f,  0.0f,  1.0f}, .tex{0.0f, 0.0f}},
        vertex{.pos{ 0.5f, -0.5f,  0.5f}, .nrm{ 0.0f,  0.0f,  1.0f}, .tex{1.0f, 0.0f}},
        vertex{.pos{ 0.5f,  0.5f,  0.5f}, .nrm{ 0.0f,  0.0f,  1.0f}, .tex{1.0f, 1.0f}},
        vertex{.pos{ 0.5f,  0.5f,  0.5f}, .nrm{ 0.0f,  0.0f,  1.0f}, .tex{1.0f, 1.0f}},
        vertex{.pos{-0.5f,  0.5f,  0.5f}, .nrm{ 0.0f,  0.0f,  1.0f}, .tex{0.0f, 1.0f}},
        vertex{.pos{-0.5f, -0.5f,  0.5f}, .nrm{ 0.0f,  0.0f,  1.0f}, .tex{0.0f, 0.0f}},
        
        vertex{.pos{-0.5f,  0.5f,  0.5f}, .nrm{-1.0f,  0.0f,  0.0f}, .tex{1.0f, 0.0f}},
        vertex{.pos{-0.5f,  0.5f, -0.5f}, .nrm{-1.0f,  0.0f,  0.0f}, .tex{1.0f, 1.0f}},
        vertex{.pos{-0.5f, -0.5f, -0.5f}, .nrm{-1.0f,  0.0f,  0.0f}, .tex{0.0f, 1.0f}},
        vertex{.pos{-0.5f, -0.5f, -0.5f}, .nrm{-1.0f,  0.0f,  0.0f}, .tex{0.0f, 1.0f}},
        vertex{.pos{-0.5f, -0.5f,  0.5f}, .nrm{-1.0f,  0.0f,  0.0f}, .tex{0.0f, 0.0f}},
        vertex{.pos{-0.5f,  0.5f,  0.5f}, .nrm{-1.0f,  0.0f,  0.0f}, .tex{1.0f, 0.0f}},

        vertex{.pos{ 0.5f,  0.5f, -0.5f}, .nrm{ 1.0f,  0.0f,  0.0f}, .tex{1.0f, 1.0f}},
        vertex{.pos{ 0.5f,  0.5f,  0.5f}, .nrm{ 1.0f,  0.0f,  0.0f}, .tex{1.0f, 0.0f}},
        vertex{.pos{ 0.5f, -0.5f, -0.5f}, .nrm{ 1.0f,  0.0f,  0.0f}, .tex{0.0f, 1.0f}},
        vertex{.pos{ 0.5f, -0.5f,  0.5f}, .nrm{ 1.0f,  0.0f,  0.0f}, .tex{0.0f, 0.0f}},
        vertex{.pos{ 0.5f, -0.5f, -0.5f}, .nrm{ 1.0f,  0.0f,  0.0f}, .tex{0.0f, 1.0f}},
        vertex{.pos{ 0.5f,  0.5f,  0.5f}, .nrm{ 1.0f,  0.0f,  0.0f}, .tex{1.0f, 0.0f}},

        vertex{.pos{-0.5f, -0.5f, -0.5f}, .nrm{ 0.0f, -1.0f,  0.0f}, .tex{0.0f, 1.0f}},
        vertex{.pos{ 0.5f, -0.5f, -0.5f}, .nrm{ 0.0f, -1.0f,  0.0f}, .tex{1.0f, 1.0f}},
        vertex{.pos{ 0.5f, -0.5f,  0.5f}, .nrm{ 0.0f, -1.0f,  0.0f}, .tex{1.0f, 0.0f}},
        vertex{.pos{ 0.5f, -0.5f,  0.5f}, .nrm{ 0.0f, -1.0f,  0.0f}, .tex{1.0f, 0.0f}},
        vertex{.pos{-0.5f, -0.5f,  0.5f}, .nrm{ 0.0f, -1.0f,  0.0f}, .tex{0.0f, 0.0f}},
        vertex{.pos{-0.5f, -0.5f, -0.5f}, .nrm{ 0.0f, -1.0f,  0.0f}, .tex{0.0f, 1.0f}},

        vertex{.pos{-0.5f,  0.5f, -0.5f}, .nrm{ 0.0f,  1.0f,  0.0f}, .tex{0.0f, 1.0f}},
        vertex{.pos{ 0.5f,  0.5f,  0.5f}, .nrm{ 0.0f,  1.0f,  0.0f}, .tex{1.0f, 0.0f}},
        vertex{.pos{ 0.5f,  0.5f, -0.5f}, .nrm{ 0.0f,  1.0f,  0.0f}, .tex{1.0f, 1.0f}},
        vertex{.pos{ 0.5f,  0.5f,  0.5f}, .nrm{ 0.0f,  1.0f,  0.0f}, .tex{1.0f, 0.0f}},
        vertex{.pos{-0.5f,  0.5f, -0.5f}, .nrm{ 0.0f,  1.0f,  0.0f}, .tex{0.0f, 1.0f}},
        vertex{.pos{-0.5f,  0.5f,  0.5f}, .nrm{ 0.0f,  1.0f,  0.0f}, .tex{0.0f, 0.0f}},
    };
    // clang-format on
    opengl::vertex_array cube_vao;
    opengl::vertex_buffer cube_vbo = opengl::vertex_buffer(cube_vertices.data(), cube_vertices.size() * sizeof(cube_vertices[0]));

    void shader_init() {
        u_view_mat = tile_shader.find_uniform("u_view_mat");
        u_proj_mat = tile_shader.find_uniform("u_proj_mat");
        u_cam_pos = tile_shader.find_uniform("u_cam_pos");

        u_tilemap_tex = tile_shader.find_uniform("u_tilemap_tex");
        u_tiles_tex = tile_shader.find_uniform("u_tiles_tex");

        tile_shader.bind();
        opengl::shader_program::send(u_cam_pos, player.cam.pos);
        opengl::shader_program::send(u_proj_mat, player.cam.proj_mat);
        opengl::shader_program::send(u_view_mat, player.cam.view_mat);
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
        // use_vsync(false);

        shader_init();

        cube_vao.bind();
        opengl::vertex_array::set_layout<glm::vec3, glm::vec3, glm::vec2>();

        for (uint32_t z = 0; z < grid_dim.z; ++z) {
            for (uint32_t y = 0; y < grid_dim.y; ++y) {
                for (uint32_t x = 0; x < grid_dim.x; ++x) {
                    auto &tile = tiles[x + y * grid_dim.x];
                    std::uint8_t val = ((sin(0.04 * x + 0.038 * z) + 1) * 5 > y) * 200;
                    tile = {val, val, val, 255};
                }
            }
        }

        tiles_tex.bind();
        tiles_tex.regenerate({
            .data = tiles.data(),
            .dim = {grid_dim.x, grid_dim.y, grid_dim.z},
            .gl_format = GL_R32UI,
            .data_format = GL_RED_INTEGER,
            .data_type = GL_UNSIGNED_INT,
            .wrap = {.s = GL_REPEAT, .t = GL_REPEAT, .r = GL_REPEAT},
            .filter = {.min = GL_NEAREST, .max = GL_NEAREST},
            .border_color = {0, 0, 0, 0},
        });
    }

    void on_update(coel::duration elapsed) {
        player.update(elapsed);

        tile_shader.bind();
        opengl::shader_program::send(u_view_mat, player.cam.view_mat);
        opengl::shader_program::send(u_cam_pos, player.cam.pos);
    }

    void on_draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        tile_shader.bind();
        opengl::shader_program::send(u_tilemap_tex, 0);
        opengl::shader_program::send(u_tiles_tex, 1);
        tilemap_tex.bind(0);
        tiles_tex.bind(1);

        cube_vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    void on_key(const coel::key_event &e) {
        if (!is_paused) {
            player.key_press(e);
        }
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

    void on_mouse_move() {
        if (!is_paused) {
            const auto screen_center = glm::vec2(frame_dim) * 0.5f;
            player.move_mouse(mouse_pos - screen_center);

            // set_mouse_pos(screen_center);
            mouse_pos = screen_center;
            glfwSetCursorPos(glfw.window_ptr, mouse_pos.x, mouse_pos.y);
        }
    }

    void on_resize() {
        player.resize_cam(frame_dim);
        tile_shader.bind();
        opengl::shader_program::send(u_proj_mat, player.cam.proj_mat);
    }

    void on_pause() { set_mouse_capture(false); }
    void on_resume() { set_mouse_capture(true); }
};

int main() {
    voxel_game game;
    if (!game)
        return -1;
    game.resize();
    while (game.update()) {
    }
}
