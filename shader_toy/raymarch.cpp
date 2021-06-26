#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <coel/application.hpp>
#include <coel/opengl/renderers/quad.hpp>
#include <coel/game/player.hpp>

#include <iostream>

class my_app : public coel::application {
    opengl::renderer::quad quad;

    std::filesystem::path shader_path = std::filesystem::current_path() / "shader_toy/shaders/raymarch.glsl";
    opengl::shader_program shader;
    opengl::shader_uniform u_viewport_dim, u_cam_pos, u_mvp_mat;
    coel::clock::time_point shader_prev_update_time;
    std::filesystem::file_time_type shader_last_write_time = std::filesystem::file_time_type::min();

    coel::player3d player;

  public:
    my_app() : coel::application({800, 600}, "Shader Toy") {
        use_vsync(true);

        player.cam.pos = {0.0f, 0.0f, 2.0f};
        player.cam.update_view();
    }

    void reload_shader() {
        shader = opengl::shader_program(shader_path);
        u_viewport_dim = shader.find_uniform("viewport_dim");
        u_cam_pos = shader.find_uniform("cam_pos");
        u_mvp_mat = shader.find_uniform("mvp_mat");
    }

    void update_shaders() {
        using namespace std::chrono_literals;
        if (now - shader_prev_update_time > 0.1s && is_active &&
            shader_last_write_time != std::filesystem::last_write_time(shader_path)) {
            try {
                reload_shader();
                shader_prev_update_time = coel::clock::now();
                shader_last_write_time = std::filesystem::last_write_time(shader_path);
            } catch (const coel::exception &e) {
                MessageBoxA(nullptr, e.what(), "Shader error", MB_OK);
            }
        }
    }

    void on_draw() {
        shader.bind();
        opengl::shader_program::send(u_cam_pos, player.cam.pos);
        opengl::shader_program::send(u_viewport_dim, glm::vec2(frame_dim));
        opengl::shader_program::send(u_mvp_mat, player.cam.view_mat);

        quad.draw();
    }

    void on_update(coel::duration elapsed) {
        update_shaders();
        auto player_updated = player.update(elapsed);
    }

    void on_key(const coel::key_event &e) {
        if (!is_paused) {
            player.key_press(e);
        }
    }

    void on_mouse_move() {
        if (!is_paused) {
            const auto screen_center = glm::vec2(frame_dim) * 0.5f;
            player.move_mouse(input.mouse.cursor_pos - screen_center);
            set_mouse_pos(screen_center);
        }
    }

    void on_resize() {
        player.resize_cam(frame_dim);
        shader.bind();
    }

    void on_pause() { set_mouse_capture(false); }
    void on_resume() { set_mouse_capture(true); }
};

int main() try {
    my_app game;
    if (!game)
        return -1;
    game.resize();
    while (game.update()) {
    }
} catch (const coel::exception &e) {
    MessageBoxA(nullptr, e.what(), "Coel Exception", MB_OK);
}
