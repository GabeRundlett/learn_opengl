#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <coel/application.hpp>
#include <coel/game/player.hpp>
#include <coel/opengl/renderers/quad.hpp>
#include "chunk.hpp"
#include "../../ui_testing/ui2.hpp"

using namespace glm;
using namespace std::chrono_literals;

class voxel_game : public coel::application {
    opengl::shader_program tile_shader;

    opengl::shader_uniform
        u_view_mat,
        u_proj_mat,
        u_cube_pos,
        u_cube_dim,
        u_cam_pos,
        u_selected_tile_pos,
        u_selected_tile_nrm,
        u_tilemap_tex,
        u_tiles_tex,
        u_tile_shader_accum_tex,
        u_camera_valid,
        u_valid_frame_count,
        u_frame_index,
        u_time,
        u_hotbar_id,
        u_frame_dim;

    coel::player3d player;
    ui_window window;

    std::vector<chunk3d *> chunks = {};

    chunk3d::raycast_information tile_pick_ray;
    bool should_remove = false, should_place = false;
    coel::clock::time_point last_remove = now, last_place = now;

    float exposure = 0.1f, gamma = 1.2f;
    chunk3d::tile_id hotbar_id = chunk3d::dirt;
    int frame_index = 0;
    int valid_frame_count = 0;

    opengl::shader_program frame_shader;
    opengl::shader_uniform
        u_exposure,
        u_gamma,
        u_frame_shader_accum_tex;

    opengl::framebuffer frame_accum_buffer;
    opengl::texture2d<> frame_accum_col;
    opengl::renderbuffer frame_accum_depth_rbo;

    opengl::renderer::quad quad;

    void shader_init() {
        try {
            tile_shader = opengl::shader_program(
                {.filepath = "voxel_game/raytraced/assets/shaders/tile_vert.glsl"},
                {.filepath = "voxel_game/raytraced/assets/shaders/tile_frag.glsl"});
            
            frame_shader = opengl::shader_program(
                {.filepath = "voxel_game/raytraced/assets/shaders/frame_vert.glsl"},
                {.filepath = "voxel_game/raytraced/assets/shaders/frame_frag.glsl"});
            
            chunks.clear();
            int chunk_radius = 0;
            for (int zi = -chunk_radius; zi <= chunk_radius; ++zi) {
                for (int yi = -chunk_radius; yi <= chunk_radius; ++yi) {
                    for (int xi = -chunk_radius; xi <= chunk_radius; ++xi) {
                        chunks.emplace_back(new chunk3d(glm::vec3(xi, yi, zi)));
                    }
                }
            }
        } catch (const coel::exception &e) {
            MessageBoxA(nullptr, e.what(), "Coel Exception", MB_OK);
        }

        u_view_mat = tile_shader.find_uniform("u_view_mat");
        u_proj_mat = tile_shader.find_uniform("u_proj_mat");
        u_cube_dim = tile_shader.find_uniform("u_cube_dim");
        u_cube_pos = tile_shader.find_uniform("u_cube_pos");
        u_cam_pos = tile_shader.find_uniform("u_cam_pos");

        u_selected_tile_pos = tile_shader.find_uniform("u_selected_tile_pos");
        u_selected_tile_nrm = tile_shader.find_uniform("u_selected_tile_nrm");
        u_tilemap_tex = tile_shader.find_uniform("u_tilemap_tex");
        u_tiles_tex = tile_shader.find_uniform("u_tiles_tex");
        u_frame_dim = tile_shader.find_uniform("u_frame_dim");

        u_frame_index = tile_shader.find_uniform("u_frame_index");
        u_tile_shader_accum_tex = tile_shader.find_uniform("u_accum_tex");
        
        u_time = tile_shader.find_uniform("u_time");
        u_hotbar_id = tile_shader.find_uniform("u_hotbar_id");

        u_valid_frame_count = tile_shader.find_uniform("u_valid_frame_count");

        tile_shader.bind();
        opengl::shader_program::send(u_cam_pos, player.cam.pos);
        opengl::shader_program::send(u_proj_mat, player.cam.proj_mat);
        opengl::shader_program::send(u_view_mat, player.cam.view_mat);
        opengl::shader_program::send(u_hotbar_id, hotbar_id);

        u_frame_shader_accum_tex = frame_shader.find_uniform("u_frame_accum_tex");
        u_exposure = frame_shader.find_uniform("u_exposure");
        u_gamma = frame_shader.find_uniform("u_gamma");
        frame_shader.bind();
        opengl::shader_program::send(u_exposure, exposure);
        opengl::shader_program::send(u_gamma, gamma);

        on_resize();
    }

    void camera_invalidate() {
        valid_frame_count = 0;
    }

  public:
    voxel_game() : coel::application({1200, 900}, "Voxel Game") {
        // show_debug_menu = true;
        use_vsync(false);
        use_raw_mouse(true);

        frame_accum_buffer.bind();
        unsigned int attachments[] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(sizeof(attachments) / sizeof(attachments[0]), attachments);
        opengl::framebuffer::attach(frame_accum_col, GL_COLOR_ATTACHMENT0);
        opengl::framebuffer::attach(frame_accum_depth_rbo, GL_DEPTH_STENCIL_ATTACHMENT);
        opengl::framebuffer::unbind();

        shader_init();

        resize();
        window.components = {{
            std::make_shared<ui_button>(button{
                .text = []() { return "Resume"s; },
                .rect = [&]() { return coel::rectangular_container{window.rect.top_left + glm::vec2{10, 10}, window.rect.top_left + glm::vec2{25, 25}}; },
                .call = [&]() { toggle_pause(); },
            }),
            std::make_shared<ui_slider>(slider{
                .text = [&]() { return fmt::format("FOV: {}", glm::degrees(player.cam.fov)); },
                .rect = [&]() { return coel::rectangular_container{window.rect.top_left + glm::vec2{10, 70}, window.rect.top_left + glm::vec2{150, 80}}; },
                .call = [&](float value) { player.cam.set_fov(value); on_resize(); },

                .value = 90.0f,
                .range = {.min = 10.0f, .max = 175.0f},
            }),
            std::make_shared<ui_slider>(slider{
                .text = [&]() { return fmt::format("Move Speed: {}", player.move_speed); },
                .rect = [&]() { return coel::rectangular_container{window.rect.top_left + glm::vec2{10, 120}, window.rect.top_left + glm::vec2{150, 130}}; },
                .call = [&](float value) { player.move_speed = value; },

                .value = 8.0f,
                .range = {.min = 0.01f, .max = 20.0f},
            }),
            std::make_shared<ui_slider>(slider{
                .text = [&]() { return fmt::format("Move Sprint Multiplier: {}", player.move_sprint_mult); },
                .rect = [&]() { return coel::rectangular_container{window.rect.top_left + glm::vec2{10, 154}, window.rect.top_left + glm::vec2{150, 164}}; },
                .call = [&](float value) { player.move_sprint_mult = value; },

                .value = 4.0f,
                .range = {.min = 0.01f, .max = 16.0f},
            }),
            std::make_shared<ui_slider>(slider{
                .text = [&]() { return fmt::format("Exposure: {}", exposure); },
                .rect = [&]() { return coel::rectangular_container{window.rect.top_left + glm::vec2{10, 210}, window.rect.top_left + glm::vec2{150, 220}}; },
                .call = [&](float value) { frame_shader.bind(); exposure = value; opengl::shader_program::send(u_exposure, value); },

                .value = 0.1f,
                .range = {.min = 0.0f, .max = 4.0f},
            }),
            std::make_shared<ui_slider>(slider{
                .text = [&]() { return fmt::format("Gamma: {}", gamma); },
                .rect = [&]() { return coel::rectangular_container{window.rect.top_left + glm::vec2{10, 244}, window.rect.top_left + glm::vec2{150, 254}}; },
                .call = [&](float value) { frame_shader.bind(); gamma = value; opengl::shader_program::send(u_gamma, value); },

                .value = 1.2f,
                .range = {.min = 0.0f, .max = 4.0f},
            }),
        }};
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
                                chunk->get_tile(tile_pick_ray.hit_info.pos + tile_pick_ray.hit_info.nrm + vec3(xi, yi, zi)) = hotbar_id;
                        }
                    }
                }
                chunk->invalidate();
                camera_invalidate();
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
                camera_invalidate();
            }
        }
    }

    void on_update(coel::duration elapsed) {
        auto player_updated = player.update(elapsed);
        if (player_updated) camera_invalidate();

        if (!is_paused) {
            if (now - last_place > 0.01s)
                place();
            if (now - last_remove > 0.01s)
                remove();
        }

        tile_shader.bind();
        opengl::shader_program::send(u_view_mat, player.cam.view_mat);
        opengl::shader_program::send(u_valid_frame_count, valid_frame_count);
        opengl::shader_program::send(u_cam_pos, player.cam.pos);
        opengl::shader_program::send(u_time, float(coel::duration(now - start_time).count()));
        opengl::shader_program::send(u_hotbar_id, hotbar_id);

        for (auto &chunk : chunks)
            chunk->update();
    }

    void on_draw() {
        frame_accum_buffer.bind();
        glViewport(0, 0, frame_dim.x, frame_dim.y);

        glClearColor(1.81f, 2.01f, 5.32f, 1.0f);
        if (valid_frame_count == 0)
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
            opengl::shader_program::send(u_tile_shader_accum_tex, 2);
            frame_accum_col.bind(2);
            opengl::shader_program::send(u_cube_pos, vec3(chunk->pos));
            opengl::shader_program::send(u_cube_dim, vec3(chunk->dim));
            opengl::shader_program::send(u_frame_index, frame_index);
            chunk->tilemap_tex.bind(0);
            chunk->tiles_tex.bind(1);
            chunk->vao.bind();
            glDrawArrays(GL_TRIANGLES, 0, 36);
            ++frame_index;
        }

        opengl::framebuffer::unbind();
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glEnable(GL_MULTISAMPLE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.1f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        frame_shader.bind();
        opengl::shader_program::send(u_frame_shader_accum_tex, 0);
        frame_accum_col.bind(0);
        quad.draw();

        ++valid_frame_count;

        glDisable(GL_DEPTH_TEST);
        if (is_paused)
            window.draw(ui_batch, text_batch);
    }

    void on_key(const coel::key_event &e) {
        if (!is_paused) {
            player.key_press(e);
        }
        if (e.action == GLFW_PRESS) {
            switch (e.key) {
            case GLFW_KEY_R: shader_init(); break;
            case GLFW_KEY_LEFT: hotbar_id = chunk3d::tile_id((hotbar_id - 2) % 10 + 1); break;
            case GLFW_KEY_RIGHT: hotbar_id = chunk3d::tile_id((hotbar_id) % 10 + 1); break;
            }
        }
    }

    void on_mouse_move() {
        if (!is_paused) {
            const auto screen_center = glm::vec2(frame_dim) * 0.5f;
            player.move_mouse(input.mouse.cursor_pos - screen_center);
            set_mouse_pos(screen_center);
            camera_invalidate();
        }
    }

    void on_event() {
        window.on_event(input);
    }

    void on_resize() {
        window.resize(frame_dim);
        player.resize_cam(frame_dim);
        tile_shader.bind();
        opengl::shader_program::send(u_proj_mat, player.cam.proj_mat);
        opengl::shader_program::send(u_frame_dim, glm::vec2(frame_dim));
        
        frame_accum_col.recreate({
            .data = {
                .dim = frame_dim,
                .format = GL_RGBA,
            },
            .gl_format = GL_RGBA16F,
            .wrap = {.s = GL_CLAMP_TO_EDGE, .t = GL_CLAMP_TO_EDGE},
            .filter = {.min = GL_LINEAR, .max = GL_LINEAR},
            .samples = 4,
            .use_mipmap = false,
        });
        
        frame_accum_depth_rbo.recreate({
            .dim = frame_dim,
            .gl_format = GL_DEPTH24_STENCIL8,
        });

        camera_invalidate();
    }

    void on_mouse_button(const coel::mouse_button_event &e) {
        if (!is_paused) {
            if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
                should_remove = e.action == GLFW_PRESS;
                if (should_remove) {
                    remove();
                    last_remove = now + 200ms;
                }
            }
            if (e.button == GLFW_MOUSE_BUTTON_RIGHT) {
                should_place = e.action == GLFW_PRESS;
                if (should_place) {
                    place();
                    last_place = now + 200ms;
                }
            }
            if (e.button == GLFW_MOUSE_BUTTON_MIDDLE) {
                for (auto &chunk : chunks) {
                    chunk->raycast(player.cam.pos, -player.cam.look, tile_pick_ray);
                    if (tile_pick_ray.hit) {
                        hotbar_id = (chunk3d::tile_id)chunk->get_tile(tile_pick_ray.hit_info.pos);
                    }
                }
            }
        }
    }

    void on_mouse_scroll(const glm::dvec2 offset) {
        hotbar_id = chunk3d::tile_id((hotbar_id - 1 - static_cast<int>(offset.y)) % 10 + 1);
    }

    void on_pause() { set_mouse_capture(false); }
    void on_resume() { set_mouse_capture(true); }
};

int main() try {
    voxel_game game;
    if (!game)
        return -1;
    while (game.update()) {
    }
} catch (const coel::exception &e) {
    MessageBoxA(nullptr, e.what(), "Coel Exception", MB_OK);
}
