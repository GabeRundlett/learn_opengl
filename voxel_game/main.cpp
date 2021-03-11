#include <coel/application.hpp>
// #include "player.hpp"
// #include "chunk.hpp"
#include "player2d.hpp"
#include "chunk2d.hpp"
#include "deferred.hpp"

class voxel_game : public coel::application {
    opengl::shader_program tile_shader = opengl::shader_program(
        {.filepath = "voxel_game/assets/shaders/tile_vert.glsl"},
        {.filepath = "voxel_game/assets/shaders/tile_frag.glsl"});

    opengl::shader_uniform
        u_player_pos,
        u_mouse_pos,
        u_max_iter,
        u_scale,
        u_aspect,
        u_tilemap_tex;

    opengl::texture2d<> tilemap = opengl::texture2d<>({
        .filepath = "voxel_game/assets/textures/tilemap.png",
        .gl_format = GL_RGBA,
        .filter = {.min = GL_NEAREST, .max = GL_NEAREST},
    });

    player2d player;
    float zoom_scale = 0.1f;
    int max_iter = 100;

    static inline constexpr std::array quad_vertices = {
        // clang-format off
        -1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
         1.0f, -1.0f,
        // clang-format on
    };
    opengl::vertex_array frame_quad_vao;
    opengl::vertex_buffer frame_quad_vbo = opengl::vertex_buffer(quad_vertices.data(), quad_vertices.size() * sizeof(quad_vertices[0]));

    glm::vec2 screen_to_world(glm::vec2 p) {
        return glm::vec2((p.x / frame_dim.x - 0.5f) * frame_dim.x / frame_dim.y, p.y / frame_dim.y - 0.5f) / zoom_scale * 2.0f - player.pos;
    }

    void shader_init() {
        tile_shader.bind();
        u_player_pos = tile_shader.find_uniform("u_player_pos");
        u_mouse_pos = tile_shader.find_uniform("u_mouse_pos");
        u_max_iter = tile_shader.find_uniform("u_max_iter");
        u_scale = tile_shader.find_uniform("u_scale");
        u_aspect = tile_shader.find_uniform("u_aspect");
        u_tilemap_tex = tile_shader.find_uniform("u_tilemap_tex");
        tile_shader.send(u_scale, zoom_scale);
        tile_shader.send(u_player_pos, player.pos);
        tile_shader.send(u_aspect, 1.0f * frame_dim.x / frame_dim.y);
        tile_shader.send(u_mouse_pos, screen_to_world(mouse_pos));
        tile_shader.send(u_max_iter, max_iter);
    }

  public:
    voxel_game() : coel::application({1200, 900}, "Voxel Game") {
        show_debug_menu = true;
        shader_init();

        tile_shader.send(u_scale, zoom_scale);
        frame_quad_vao.bind();
        opengl::set_layout<glm::vec2>();
    }

    void on_update(double elapsed) {
        player.update(elapsed);
        tile_shader.bind();
        tile_shader.send(u_player_pos, player.pos);
        tile_shader.send(u_mouse_pos, screen_to_world(mouse_pos));
    }

    void on_draw() {
        opengl::framebuffer::unbind();
        glViewport(0, 0, frame_dim.x, frame_dim.y);
        glClear(GL_COLOR_BUFFER_BIT);

        tile_shader.bind();
        tile_shader.send(u_tilemap_tex, 0);
        tilemap.bind(0);

        frame_quad_vao.bind();
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
            case GLFW_KEY_UP:
                tile_shader.bind();
                tile_shader.send(u_max_iter, ++max_iter);
                break;
            case GLFW_KEY_DOWN:
                tile_shader.bind();
                tile_shader.send(u_max_iter, --max_iter);
                break;
            }
        }
    }

    void on_resize() {
        tile_shader.bind();
        tile_shader.send(u_aspect, 1.0f * frame_dim.x / frame_dim.y);
    }

    void on_mouse_scroll(const glm::dvec2 offset) {
        zoom_scale += (float)offset.y * zoom_scale * 0.1f;
        if (zoom_scale < 0.001f)
            zoom_scale = 0.001f;
        tile_shader.bind();
        tile_shader.send(u_scale, zoom_scale);
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
