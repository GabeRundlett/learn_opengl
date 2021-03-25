#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <coel/application.hpp>
#include <coel/game/player.hpp>
#include <coel/opengl/renderers/quad.hpp>
#include "chunk.hpp"

using namespace glm;
using namespace std::chrono_literals;

class voxel_game : public coel::application {
    opengl::shader_program frame_shader = opengl::shader_program(
        {.filepath = "voxel_game/raytraced/assets/shaders/frame_vert.glsl"},
        {.filepath = "voxel_game/raytraced/assets/shaders/frame_frag.glsl"});
    opengl::shader_uniform
        u_scene_frame_tex,
        u_scene_frame_dim;
    opengl::texture2d<> scene_frame_tex;
    opengl::framebuffer scene_frame;
    opengl::renderbuffer scene_rbo;
    opengl::renderer::quad quad;

    opengl::shader_program tile_shader = opengl::shader_program(
        {.filepath = "voxel_game/raytraced/assets/shaders/tile_vert.glsl"},
        {.filepath = "voxel_game/raytraced/assets/shaders/tile_frag.glsl"});

    opengl::shader_uniform
        u_view_mat,
        u_proj_mat,
        u_cube_pos,
        u_cube_dim,
        u_cam_pos,
        u_selected_tile_pos,
        u_selected_tile_nrm,
        u_tilemap_tex,
        u_tiles_tex;

    coel::player3d player;

    std::vector<chunk3d *> chunks = {};

    chunk3d::raycast_information tile_pick_ray;
    bool should_remove = false, should_place = false;
    coel::clock::time_point last_remove = now, last_place = now;

    void shader_init() {
        u_scene_frame_tex = frame_shader.find_uniform("u_frame_col_tex");
        u_scene_frame_dim = frame_shader.find_uniform("u_frame_dim");

        u_view_mat = tile_shader.find_uniform("u_view_mat");
        u_proj_mat = tile_shader.find_uniform("u_proj_mat");
        u_cube_dim = tile_shader.find_uniform("u_cube_dim");
        u_cube_pos = tile_shader.find_uniform("u_cube_pos");
        u_cam_pos = tile_shader.find_uniform("u_cam_pos");

        u_selected_tile_pos = tile_shader.find_uniform("u_selected_tile_pos");
        u_selected_tile_nrm = tile_shader.find_uniform("u_selected_tile_nrm");
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

        scene_frame.bind();
        unsigned int attachments[] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(sizeof(attachments) / sizeof(unsigned int), attachments);
        opengl::framebuffer::attach(scene_frame_tex, GL_COLOR_ATTACHMENT0);
        opengl::framebuffer::attach(scene_rbo, GL_DEPTH_STENCIL_ATTACHMENT);

        shader_init();

        player.cam.fov = glm::radians(70.0f);
        player.cam.update_proj();
        player.move_sprint_mult = 16;

        int chunk_radius = 8;
        for (int zi = -chunk_radius; zi < chunk_radius; ++zi) {
            for (int yi = -chunk_radius / 4; yi < chunk_radius / 4; ++yi) {
                for (int xi = -chunk_radius; xi < chunk_radius; ++xi) {
                    chunks.emplace_back(new chunk3d(glm::vec3(xi, yi, zi)));
                }
            }
        }
    }

    ~voxel_game() {
        for (auto &chunk : chunks)
            delete chunk;
    }

    void place() {
        for (auto &chunk : chunks) {
            chunk->raycast(player.cam.pos, -player.cam.look, tile_pick_ray);
            if (tile_pick_ray.hit) {
                tile_pick_ray.hit_info.pos -= tile_pick_ray.hit_info.nrm * 0.5f;
                opengl::shader_program::send(u_selected_tile_pos, tile_pick_ray.hit_info.pos);
                opengl::shader_program::send(u_selected_tile_nrm, tile_pick_ray.hit_info.nrm);
            }
            if (should_place && tile_pick_ray.hit) {
                float radius = 1;
                last_place = now;
                for (float zi = -radius; zi < radius; ++zi) {
                    for (float yi = -radius; yi < radius; ++yi) {
                        for (float xi = -radius; xi < radius; ++xi) {
                            if (xi * xi + yi * yi + zi * zi < radius * radius)
                                chunk->get_tile(tile_pick_ray.hit_info.pos + tile_pick_ray.hit_info.nrm + vec3(xi, yi, zi)) = chunk3d::dirt;
                        }
                    }
                }
                chunk->invalidate();
            }
        }
    }

    void remove() {
        for (auto &chunk : chunks) {
            chunk->raycast(player.cam.pos, -player.cam.look, tile_pick_ray);
            if (tile_pick_ray.hit) {
                tile_pick_ray.hit_info.pos -= tile_pick_ray.hit_info.nrm * 0.5f;
                opengl::shader_program::send(u_selected_tile_pos, tile_pick_ray.hit_info.pos);
                opengl::shader_program::send(u_selected_tile_nrm, tile_pick_ray.hit_info.nrm);
            }
            if (should_remove && tile_pick_ray.hit) {
                float radius = 1;
                last_remove = now;
                for (float zi = -radius; zi < radius; ++zi) {
                    for (float yi = -radius; yi < radius; ++yi) {
                        for (float xi = -radius; xi < radius; ++xi) {
                            if (xi * xi + yi * yi + zi * zi < radius * radius)
                                chunk->get_tile(tile_pick_ray.hit_info.pos + vec3(xi, yi, zi)) = chunk3d::none;
                        }
                    }
                }
                chunk->invalidate();
            }
        }
    }

    void on_update(coel::duration elapsed) {
        player.update(elapsed);

        tile_shader.bind();
        opengl::shader_program::send(u_view_mat, player.cam.view_mat);
        opengl::shader_program::send(u_cam_pos, player.cam.pos);

        if (now - last_place > 0.02s)
            place();

        if (now - last_remove > 0.02s)
            remove();

        for (auto &chunk : chunks)
            chunk->update();
    }

    void on_draw() {
        scene_frame.bind();
        glViewport(0, 0, frame_dim.x, frame_dim.y);

        glClearColor(1.81f, 2.01f, 5.32f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (auto &chunk : chunks) {
            tile_shader.bind();
            opengl::shader_program::send(u_tilemap_tex, 0);
            opengl::shader_program::send(u_tiles_tex, 1);
            opengl::shader_program::send(u_cube_pos, vec3(chunk->pos));
            opengl::shader_program::send(u_cube_dim, vec3(chunk->dim));
            chunk->tilemap_tex.bind(0);
            chunk->tiles_tex.bind(1);
            chunk->vao.bind();
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        opengl::framebuffer::unbind();
        glViewport(0, 0, frame_dim.x, frame_dim.y);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glClear(GL_COLOR_BUFFER_BIT);
        frame_shader.bind();
        opengl::shader_program::send(u_scene_frame_tex, 0);
        scene_frame_tex.bind(0);
        quad.draw();
    }

    void on_key(const coel::key_event &e) {
        if (!is_paused) {
            player.key_press(e);
        }
        if (e.action == GLFW_PRESS) {
            switch (e.key) {
            case GLFW_KEY_R:
                tile_shader = opengl::shader_program(
                    {.filepath = "voxel_game/raytraced/assets/shaders/tile_vert.glsl"},
                    {.filepath = "voxel_game/raytraced/assets/shaders/tile_frag.glsl"});
                shader_init();
                break;
            }
        }
    }

    void on_mouse_move() {
        if (!is_paused) {
            const auto screen_center = glm::vec2(frame_dim) * 0.5f;
            player.move_mouse(mouse_pos - screen_center);
            set_mouse_pos(screen_center);
        }
    }

    void on_resize() {
        scene_frame_tex.recreate({
            .data{
                .dim = frame_dim,
                .format = GL_RGBA,
            },
            .gl_format = GL_RGBA16F,
            .wrap = {.s = GL_CLAMP_TO_EDGE, .t = GL_CLAMP_TO_EDGE},
            .filter = {.min = GL_LINEAR, .max = GL_LINEAR},
            .use_mipmap = false,
        });
        scene_rbo.recreate({
            .dim = frame_dim,
            .gl_format = GL_DEPTH24_STENCIL8,
        });
        scene_frame.verify();
        frame_shader.bind();
        opengl::shader_program::send(u_scene_frame_dim, glm::vec2(frame_dim));

        player.resize_cam(frame_dim);
        tile_shader.bind();
        opengl::shader_program::send(u_proj_mat, player.cam.proj_mat);
    }

    void on_mouse_button(const coel::mouse_button_event &e) {
        if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
            should_remove = e.action == GLFW_PRESS;
            remove();
            last_remove = now + 100ms;
        }
        if (e.button == GLFW_MOUSE_BUTTON_RIGHT) {
            should_place = e.action == GLFW_PRESS;
            place();
            last_place = now + 100ms;
        }
    }

    void on_pause() { set_mouse_capture(false); }
    void on_resume() { set_mouse_capture(true); }
};

int main() try {
    voxel_game game;
    if (!game)
        return -1;
    game.resize();
    while (game.update()) {
    }
} catch (const coel::exception &e) {
    MessageBoxA(nullptr, e.what(), "Coel Exception", MB_OK);
}
