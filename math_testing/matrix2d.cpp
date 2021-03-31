#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <coel/application.hpp>
#include <coel/opengl/renderers/quad.hpp>

#include <coel/graphics/ui.hpp>

class my_app : public coel::application {
    graphics::menu_ui menu;

public:
    my_app() : coel::application({1200, 600}, "matrix2d math") {
        menu.buttons.push_back(graphics::button{
            .pos = {20, 20},
            .size = {130, 40},
            .color_focused = {0.5, 0.55, 0.9, 1},
            .color_unfocused = {0.4, 0.47, 0.8, 1},
            .label = {
                .text = "Resume",
                .color_focused = {0.1, 0.15, 0.3, 1},
                .color_unfocused = {0.12, 0.18, 0.3, 1},
                .offset = {10, 2},
                .scale = 28,
            },
            .user_ptr = this,
            .on_release = [](void *user_ptr) -> void {
                auto *app_ptr = reinterpret_cast<my_app *>(user_ptr);
                if (app_ptr) {
                    auto &app = *app_ptr;
                    app.toggle_pause();
                }
            },
        });
    }

    void on_draw() override {
        glClear(GL_COLOR_BUFFER_BIT);
        if (is_paused)
            menu.draw_settings();
    }

    void on_mouse_move() override {
        menu.mouse_move(mouse_pos);
    }

    void on_mouse_button(const coel::mouse_button_event &e) override {
        menu.mouse_press(e, mouse_pos);
    }

    void on_key(const coel::key_event &e) override {
        menu.process_key(e);
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
