#include <coel/application.hpp>
#include <coel/opengl/renderers/quad.hpp>

#include "tiles.hpp"

class tiles_game : public coel::application {
    opengl::renderer::quad quad;

    opengl::shader_program shader = opengl::shader_program("tiles_game/assets/main.glsl");
    opengl::shader_uniform u_tiles_tex, u_tilemap_tex, u_frame_dim, u_mouse_pos, u_active_item;

    tile_chunk chunk;

    tile_id item = tile_id::dirt;

    enum class action_id {
        none,
        remove_tile,
        place_tile,
    };
    action_id action = action_id::none;

    opengl::texture2d<> tilemap_tex = opengl::texture2d<>({
        .filepath = "voxel_game/assets/textures/tilemap.png",
        .gl_format = GL_RGBA,
        .filter = {.min = GL_NEAREST, .max = GL_NEAREST},
    });

  public:
    tiles_game() : coel::application({1200, 900}, "Tiles Game") {
        u_tiles_tex = shader.find_uniform("u_tiles_tex");
        u_tilemap_tex = shader.find_uniform("u_tilemap_tex");
        u_frame_dim = shader.find_uniform("u_frame_dim");
        u_mouse_pos = shader.find_uniform("u_mouse_pos");
        u_active_item = shader.find_uniform("u_active_item");
        chunk.regenerate();
    }

    void on_update(coel::duration) {
        shader.bind();
        opengl::shader_program::send(u_mouse_pos, glm::vec2(mouse_pos));

        if (action != action_id::none) {
            glm::vec2 mp = screen_to_world(mouse_pos);
            if (mp.x > 0 && mp.x < 1 && mp.y > 0 && mp.y < 1) {
                auto &tile = chunk.get_tile(mp * glm::vec2(tile_chunk::dim));
                switch (action) {
                case action_id::remove_tile: tile = tile_id::none; break;
                case action_id::place_tile: tile = item; break;
                }
                chunk.should_recalculate = true;
            }
        }

        chunk.update();
    }

    void on_draw() {
        glClear(GL_COLOR_BUFFER_BIT);

        shader.bind();

        shader.send(u_tiles_tex, 0);
        shader.send(u_tilemap_tex, 1);
        shader.send(u_active_item, (unsigned int)item);
        chunk.tex.bind(0);
        tilemap_tex.bind(1);

        quad.draw();
    }

    glm::vec2 screen_to_world(glm::vec2 p) {
        return (p + glm::vec2((float(frame_dim.y) - frame_dim.x) * 0.5, 0)) / float(frame_dim.y);
    }

    void on_mouse_button(const coel::mouse_button_event &e) {
        switch (e.button) {
        case GLFW_MOUSE_BUTTON_LEFT: action = e.action == GLFW_PRESS ? action_id::place_tile : action_id::none; break;
        case GLFW_MOUSE_BUTTON_RIGHT: action = e.action == GLFW_PRESS ? action_id::remove_tile : action_id::none; break;
        case GLFW_MOUSE_BUTTON_MIDDLE: {
            auto tile = chunk.get_tile(screen_to_world(mouse_pos) * glm::vec2(tile_chunk::dim));
            if (tile != tile_id::none)
                item = tile;
        }; break;
        default:
            break;
        }
    }

    void on_pause() { set_mouse_capture(false); }
    void on_resume() { set_mouse_capture(true); }

    void on_resize() {
        shader.bind();
        opengl::shader_program::send(u_frame_dim, glm::vec2(frame_dim));
    }
};

int main() {
    tiles_game game;
    if (!game)
        return -1;
    game.resize();
    while (game.update()) {
    }
}
