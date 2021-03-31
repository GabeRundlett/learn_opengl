#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <coel/application.hpp>
#include <coel/opengl/renderers/quad.hpp>

#include "screen.hpp"

class my_app : public coel::application {
    opengl::renderer::quad quad;

    std::filesystem::path shader_path = std::filesystem::current_path() / "software_rendering/main.glsl";
    opengl::shader_program shader;
    opengl::shader_uniform u_viewport_dim, u_screen_tex;
    coel::clock::time_point prev_update_time;

    screen screen;
    opengl::texture2d<> screen_tex;

  public:
    my_app() : coel::application({800, 600}, "Software Rendering") {
        use_raw_mouse(true);
        reload_shader();
    }

    void reload_shader() {
        shader = opengl::shader_program(shader_path);
        u_viewport_dim = shader.find_uniform("viewport_dim");
        u_screen_tex = shader.find_uniform("screen_tex");
        prev_update_time = coel::clock::now();
    }

    void on_draw() {
        screen.draw();

        screen_tex.recreate({
            .data{
                .ptr = (uint8_t *)screen.frame.pixels.data(),
                .offset = {0, 0},
                .dim = screen.frame.size,
                .format = GL_RGBA,
            },
            .gl_format = GL_RGBA8,
            .wrap = {.s = GL_CLAMP_TO_EDGE, .t = GL_CLAMP_TO_EDGE},
            .filter = {.min = GL_NEAREST, .max = GL_NEAREST},
            .use_mipmap = false,
        });

        screen_tex.bind(0);
        shader.bind();
        opengl::shader_program::send(u_viewport_dim, glm::vec2(frame_dim));
        opengl::shader_program::send(u_screen_tex, 0);
        quad.draw();
    }

    void on_resize() {
        screen.resize(frame_dim / 16u);
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
