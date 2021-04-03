#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <coel/application.hpp>
#include <coel/opengl/renderers/quad.hpp>

#include <coel/graphics/ui.hpp>

class my_app : public coel::application {
    graphics::menu_ui<my_app> menu;
    bool move_button = false;
    bool size_button = false;

  public:
    my_app() : coel::application({1200, 600}, "matrix2d math") {
        menu.buttons.push_back(graphics::button<my_app>{
            .user_ptr = this,
            .get_pos = [](const graphics::button<my_app> &, my_app &app) -> glm::vec2 {
                static glm::vec2 p = {20, 20};
                if (app.move_button)
                    p = app.input.mouse.cursor_pos;
                return p;
            },
            .get_size = [](const graphics::button<my_app> &self, my_app &app) -> glm::vec2 {
                static glm::vec2 s = {200, 40};
                if (app.size_button)
                    s = app.input.mouse.cursor_pos - self.get_pos(self, app); 
                return s;
            },
            .color_focused = {0.5, 0.55, 0.9, 1},
            .color_unfocused = {0.4, 0.47, 0.8, 1},
            .label = {
                .text = "Resume",
                .color_focused = {0.1, 0.15, 0.3, 1},
                .color_unfocused = {0.12, 0.18, 0.3, 1},
                .offset = {10, 2},
                .scale = 28,
            },
            .on_release = [](const graphics::button<my_app> &, my_app &app) -> void { app.toggle_pause(); },
        });
    }

    void on_draw() override {
        glClear(GL_COLOR_BUFFER_BIT);
        if (is_paused)
            menu.draw_settings();
    }

    void on_mouse_move() override {
        menu.mouse_move(input.mouse.cursor_pos);
    }

    void on_mouse_button(const coel::mouse_button_event &e) override {
        menu.mouse_press(e, input.mouse.cursor_pos);
    }

    void on_key(const coel::key_event &e) override {
        menu.process_key(e);
        move_button = e.key == GLFW_KEY_A && e.action != GLFW_RELEASE;
        size_button = e.key == GLFW_KEY_S && e.action != GLFW_RELEASE;
    }

    void on_resize() override {
        menu.resize(frame_dim);
    }
};

int main() try {
    my_app app;
    if (!app)
        return -1;
    app.resize();
    while (app.update()) {
    }
} catch (const coel::exception &e) {
    MessageBoxA(nullptr, e.what(), "Coel Exception", MB_OK);
}
