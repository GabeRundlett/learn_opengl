#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <coel/application.hpp>
#include <coel/game/player.hpp>

#include "ui2.hpp"

class my_app : public coel::application {
  public:
    coel::player3d player;
    ui_window window;

    my_app() : coel::application({800, 600}, "user interface") {
        use_raw_mouse(true);

        resize();
        window.components = {{
            std::make_shared<ui_button>(button{
                .text = []() { return "Resume"s; },
                .rect = [&]() { return coel::rectangular_container{window.rect.top_left + glm::vec2{10, 10}, window.rect.top_left + glm::vec2{25, 25}}; },
                .call = [&]() { toggle_pause(); },
            }),
            std::make_shared<ui_slider>(slider{
                .text = [&]() { return fmt::format("FOV: {}", player.cam.fov); },
                .rect = [&]() { return coel::rectangular_container{window.rect.top_left + glm::vec2{10, 60}, window.rect.top_left + glm::vec2{150, 70}}; },
                .call = [&](float value) { player.cam.set_fov(value); },
                .range = {.min = 10.0f, .max = 175.0f},
            }),
            std::make_shared<ui_slider>(slider{
                .text = [&]() { return fmt::format("Move Speed: {}", player.move_speed); },
                .rect = [&]() { return coel::rectangular_container{window.rect.top_left + glm::vec2{10, 100}, window.rect.top_left + glm::vec2{150, 110}}; },
                .call = [&](float value) { player.move_speed = value; },
                .range = {.min = 0.01f, .max = 10.0f},
            }),
        }};
    }

    void on_draw() {
        is_paused ? glClearColor(0.2f, 0.2f, 0.2f, 1.0f) : glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (is_paused)
            window.draw(ui_batch, text_batch);
    }

    void on_event() {
        window.on_event(input);
    }

    void on_resize() override {
        window.resize(frame_dim);
    }

    void on_pause() { set_mouse_capture(false); }
    void on_resume() { set_mouse_capture(true); }
};

int main() try {
    my_app game;
    if (!game)
        return -1;
    while (game.update()) {
    }
} catch (const coel::exception &e) {
    MessageBoxA(nullptr, e.what(), "Coel Exception", MB_OK);
}
