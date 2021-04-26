#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <coel/application.hpp>
#include <coel/opengl/renderers/quad.hpp>

#include <algorithm>

#include "tiles.hpp"

class tiles_game : public coel::application {
    opengl::renderer::quad quad;

    opengl::shader_program shader = opengl::shader_program("tiles_game/assets/main.glsl");
    opengl::shader_uniform
        u_frame_dim,
        u_mouse_pos,
        u_world_size,
        u_tilemap_tex,
        u_chunk_tex,
        u_point_count,
        u_points,
        u_intersection_count,
        u_intersection;

    opengl::texture2d<> tilemap_tex = opengl::texture2d<>({
        .filepath = "voxel_game/raytraced/assets/textures/tilemap.png",
        .gl_format = GL_RGBA,
        .filter = {.min = GL_NEAREST, .max = GL_NEAREST},
    });

    struct drag_point {
        bool grabbed = false;
        glm::vec2 pos;
    };
    std::vector<drag_point> points{
        {.pos = {1, 3}},
        {.pos = {5, 4}},
    };

    glm::vec2 world_mouse_pos;
    tile_chunk chunk;

  public:
    tiles_game() : coel::application({1200, 900}, "Tiles Game") {
        u_frame_dim = shader.find_uniform("u_frame_dim");
        u_mouse_pos = shader.find_uniform("u_mouse_pos");
        u_world_size = shader.find_uniform("u_world_size");
        u_tilemap_tex = shader.find_uniform("u_tilemap_tex");
        u_chunk_tex = shader.find_uniform("u_chunk_tex");
        u_point_count = shader.find_uniform("u_point_count");
        u_points = shader.find_uniform("u_points");
        u_intersection = shader.find_uniform("u_intersection");

        chunk.regenerate();
    }

    glm::vec2 screen_to_world(glm::vec2 p) {
        return p / float(frame_dim.y) * glm::vec2(chunk.dim);
    }

    void on_draw() {
        glClear(GL_COLOR_BUFFER_BIT);

        shader.bind();

        opengl::shader_program::send(u_mouse_pos, world_mouse_pos);
        opengl::shader_program::send(u_world_size, glm::vec2(chunk.dim));

        tile_chunk::raycast_config raycast_conf{
            .origin = points[0].pos,
            .dir = points[1].pos - points[0].pos,
            .max_iter = 100,
        };

        auto raycast = chunk.raycast(raycast_conf);
        auto surface = chunk.get_surface_details(raycast_conf, raycast);

        opengl::shader_program::send(u_intersection, surface.pos);

        auto point_count = std::min((int)(points.size() + raycast.points.size()), 100);
        std::vector<glm::vec2> point_positions(point_count);
        point_positions.front() = points[0].pos;
        point_positions.back() = points[1].pos;
        for (int i = 1; i < point_count - 1; ++i) 
            point_positions[i] = raycast.points[i - 1];
        opengl::shader_program::send(u_point_count, point_count);
        opengl::shader_program::send(u_points, point_positions.data(), point_count);

        chunk.update();
        opengl::shader_program::send(u_tilemap_tex, 0);
        tilemap_tex.bind(0);
        opengl::shader_program::send(u_chunk_tex, 1);
        chunk.tex.bind(1);

        quad.draw();
    }

    void on_mouse_move() {
        world_mouse_pos = screen_to_world(input.mouse.cursor_pos);
        for (auto &p : points) {
            if (p.grabbed) {
                if (glfwGetKey(glfw.window_ptr, GLFW_KEY_LEFT_CONTROL))
                    p.pos = coel::floor(world_mouse_pos + glm::vec2(0.5, 0.5));
                else
                    p.pos = world_mouse_pos;
            }
        }
    }

    void on_mouse_button(const coel::mouse_button_event &e) {
        if (e.action == GLFW_PRESS) {
            switch (e.button) {
            case GLFW_MOUSE_BUTTON_LEFT:
                if (!grab_point()) {
                    chunk.get_tile(world_mouse_pos) = tile_id::dirt;
                    chunk.recalculate_tex();
                }
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                chunk.get_tile(world_mouse_pos) = tile_id::none;
                chunk.recalculate_tex();
                break;
            }
        } else if (e.action == GLFW_RELEASE) {
            for (auto &p : points)
                p.grabbed = false;
        }
    }

    bool grab_point() {
        const float r = 0.12f;
        for (auto &p : points) {
            if (glm::dot(world_mouse_pos - p.pos, world_mouse_pos - p.pos) < r * r) {
                p.grabbed = true;
                return true;
            }
        }
        return false;
    }

    void on_resize() {
        shader.bind();
        opengl::shader_program::send(u_frame_dim, glm::vec2(frame_dim));
    }
};

int main() try {
    tiles_game game;
    if (!game)
        return -1;
    game.resize();
    while (game.update()) {
    }
} catch (const coel::exception &e) {
    MessageBoxA(nullptr, e.what(), "Coel Exception", MB_OK);
}
