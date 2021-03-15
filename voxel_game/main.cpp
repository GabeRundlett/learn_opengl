#include <coel/application.hpp>
#include "chunk.hpp"
#include "player.hpp"
#include "noise.hpp"

using namespace glm;
using namespace std::chrono_literals;

class voxel_game : public coel::application {
    opengl::shader_program tile_shader = opengl::shader_program(
        {.filepath = "voxel_game/assets/shaders/tile_vert.glsl"},
        {.filepath = "voxel_game/assets/shaders/tile_frag.glsl"});

    opengl::shader_uniform
        u_view_mat,
        u_proj_mat,
        u_cube_dim,
        u_cam_pos,
        u_selected_tile_pos,
        u_tilemap_tex,
        u_tiles_tex;

    player3d player;
    chunk3d chunk;

    chunk3d::raycast_information tile_pick_ray;
    bool should_remove = false, should_place = false;
    coel::clock::time_point last_remove = now, last_place = now;

    void shader_init() {
        u_view_mat = tile_shader.find_uniform("u_view_mat");
        u_proj_mat = tile_shader.find_uniform("u_proj_mat");
        u_cube_dim = tile_shader.find_uniform("u_cube_dim");
        u_cam_pos = tile_shader.find_uniform("u_cam_pos");

        u_selected_tile_pos = tile_shader.find_uniform("u_selected_tile_pos");
        u_tilemap_tex = tile_shader.find_uniform("u_tilemap_tex");
        u_tiles_tex = tile_shader.find_uniform("u_tiles_tex");

        tile_shader.bind();
        opengl::shader_program::send(u_cam_pos, player.cam.pos);
        opengl::shader_program::send(u_proj_mat, player.cam.proj_mat);
        opengl::shader_program::send(u_view_mat, player.cam.view_mat);
    }

  public:
    voxel_game() : coel::application({1200, 900}, "Voxel Game") {
        show_debug_menu = true;
        use_vsync(false);
        use_raw_mouse(true);
        shader_init();
    }

    void on_update(coel::duration elapsed) {
        player.update(elapsed);
        chunk.raycast(player.cam.pos, -player.cam.look, tile_pick_ray);

        tile_shader.bind();
        opengl::shader_program::send(u_view_mat, player.cam.view_mat);
        opengl::shader_program::send(u_cam_pos, player.cam.pos);

        if (tile_pick_ray.hit) {
            tile_pick_ray.hit_info.pos -= tile_pick_ray.hit_info.nrm * 0.5f;
            opengl::shader_program::send(u_selected_tile_pos, tile_pick_ray.hit_info.pos);
        }

        if (should_place && tile_pick_ray.hit && now - last_place > 0.05s) {
            last_place = now;
            float radius = 3;
            for (float zi = -radius; zi < radius; ++zi) {
                for (float yi = -radius; yi < radius; ++yi) {
                    for (float xi = -radius; xi < radius; ++xi) {
                        if (xi * xi + yi * yi + zi * zi < radius * radius)
                            chunk.get_tile(tile_pick_ray.hit_info.pos + tile_pick_ray.hit_info.nrm + vec3(xi, yi, zi)) = 1;
                    }
                }
            }
        } else if (should_remove && tile_pick_ray.hit && now - last_remove > 0.05s) {
            last_remove = now;
            float radius = 8;
            for (float zi = -radius; zi < radius; ++zi) {
                for (float yi = -radius; yi < radius; ++yi) {
                    for (float xi = -radius; xi < radius; ++xi) {
                        if (xi * xi + yi * yi + zi * zi < radius * radius)
                            chunk.get_tile(tile_pick_ray.hit_info.pos + vec3(xi, yi, zi)) = 0;
                    }
                }
            }
        }

        if (should_remove || should_place)
            chunk.update();
    }

    void on_draw() {
        glClearColor(1.81f / 5, 2.01f / 5, 5.32f / 5, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        tile_shader.bind();
        opengl::shader_program::send(u_tilemap_tex, 0);
        opengl::shader_program::send(u_tiles_tex, 1);
        opengl::shader_program::send(u_cube_dim, vec3(chunk.dim));
        chunk.tilemap_tex.bind(0);
        chunk.tiles_tex.bind(1);
        chunk.vao.bind();
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

    void on_mouse_button(const coel::mouse_button_event &e) {
        if (e.button == GLFW_MOUSE_BUTTON_LEFT)
            should_remove = e.action == GLFW_PRESS;
        if (e.button == GLFW_MOUSE_BUTTON_RIGHT)
            should_place = e.action == GLFW_PRESS;
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
