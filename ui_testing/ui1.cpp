#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <coel/application.hpp>
#include "ui1.hpp"

struct ui_slider {
    coel::rectangular_container rect;
    bool grabbed = false, hovered = false;
    float val = 0.5;

    bool contains(glm::vec2 p) const { return rect.contains(p); }

    bool is_hovered() const { return hovered; }
    void mouse_enter(glm::vec2) { hovered = true; }
    void mouse_over(glm::vec2) {}
    void mouse_exit() { hovered = false; }

    bool is_pressed() const { return grabbed; }
    void press(glm::vec2) {
        // auto pix_val = val * (rect.bottom_right.x - rect.top_left.x) + rect.top_left.x;
        // grabbed = pix_val - 2 < p.x && p.x < pix_val + 2;
        grabbed = true;
    }
    void drag(glm::vec2 p) {
        val = (p.x - rect.top_left.x) / (rect.bottom_right.x - rect.top_left.x);
        val = std::max(std::min(val, 1.0f), 0.0f);
    }
    void release() { grabbed = false; }

    void draw(opengl::renderer::ui_batch &ui) const {
        glm::vec4 color = {0.25, 0.25, 0.25, 1};
        if (hovered | grabbed)
            color.r = 0.5f;
        ui.submit_rect(rect.top_left, rect.bottom_right, color);
        auto pix_val = val * (rect.bottom_right.x - rect.top_left.x) + rect.top_left.x;
        ui.submit_rect({pix_val - 2.0f, rect.top_left.y}, {pix_val + 2.0f, rect.bottom_right.y}, {color.r * 2, color.r * 2, color.r * 2, 1});
    }
};

struct ui_toggle_box {
    coel::rectangular_container rect;
    bool pressed = false, hovered = false;
    bool toggled = false;

    bool contains(glm::vec2 p) const { return rect.contains(p); }

    bool is_hovered() const { return hovered; }
    void mouse_enter(glm::vec2) { hovered = true; }
    void mouse_over(glm::vec2) {}
    void mouse_exit() { hovered = false; }

    bool is_pressed() const { return pressed; }
    void press(glm::vec2) {
        pressed = true;
    }
    void release() {
        pressed = false;
        toggled = !toggled;
    }

    void draw(opengl::renderer::ui_batch &ui) const {
        glm::vec4 color = {0.25, 0.25, 0.25, 1};
        if (hovered | pressed)
            color.r = 0.5f;
        ui.submit_rect(rect.top_left, rect.bottom_right, color);
        glm::vec2 toggle_border{4, 4};
        if (toggled)
            ui.submit_rect(rect.top_left + toggle_border, rect.bottom_right - toggle_border, {color.r * 2, color.r * 2, color.r * 2, 1});
    }
};

struct ui_window {
    coel::rectangular_container rect;

    coel::rectangle_mover mover;
    coel::rectangle_resizer resizer;

    glm::vec2 resize_margin{10, 10};
    bool hovered = false;

    ui_toggle_box box1{.rect{{2, 2}, {32, 32}}};

    bool contains(glm::vec2 p) const { return rect.contains(p); }

    bool is_hovered() const { return hovered || box1.is_hovered(); }
    void mouse_enter(glm::vec2 p) {
        auto box1_rect = box1.rect;
        box1.rect.top_left += rect.top_left;
        box1.rect.bottom_right += rect.top_left;
        if (box1.contains(p))
            box1.mouse_enter(p);
        else
            hovered = false;
        box1.rect = box1_rect;
    }
    void mouse_over(glm::vec2 p) {
        auto box1_rect = box1.rect;
        box1.rect.top_left += rect.top_left;
        box1.rect.bottom_right += rect.top_left;
        if (box1.contains(p))
            box1.mouse_over(p);
        box1.rect = box1_rect;
    }
    void mouse_exit() {
        hovered = false;
        box1.mouse_exit();
    }

    bool is_pressed() const { return mover.grabbed || resizer.grabbed || box1.is_pressed(); }
    void press(glm::vec2 p) {
        {
            auto box1_rect = box1.rect;
            box1.rect.top_left += rect.top_left;
            box1.rect.bottom_right += rect.top_left;
            if (box1.contains(p))
                box1.press(p);
            box1.rect = box1_rect;
        }
        if (!box1.contains(p)) {
            resizer.press(p, rect, resize_margin);
            if (!resizer.is_pressed())
                mover.press(p, rect);
        }
    }
    void drag(glm::vec2 p) {
        if (box1.is_pressed()) {
        } else if (mover.is_pressed())
            mover.drag(p, rect);
        else if (resizer.is_pressed())
            resizer.drag(p, rect, resize_margin);
    }
    void release() {
        mover.grabbed = false, resizer.grabbed = false, resizer.edge = 0;
        box1.release();
    }

    void draw(opengl::renderer::ui_batch &ui) {
        glm::vec4 color = {0.25, 0.25, 0.25, 1};
        if (hovered)
            color.r = 0.5f;
        if (mover.grabbed)
            color.g = 0.5f;
        if (resizer.grabbed)
            color.b = 0.5f;
        ui.submit_rect(rect.top_left, rect.bottom_right, color);
        ui.submit_rect(rect.top_left + resize_margin, rect.bottom_right - resize_margin, {0.08, 0.08, 0.08, 0.5});
        auto box1_rect = box1.rect;
        box1.rect.top_left += rect.top_left;
        box1.rect.bottom_right += rect.top_left;
        box1.draw(ui);
        box1.rect = box1_rect;
    }
};

class my_app : public coel::application {
  public:
    std::vector<ui_window> ui_windows{{
        {.rect{{240, 100}, {440, 300}}},
        {.rect{{240, 350}, {540, 500}}},
        {.rect{{640, 150}, {700, 200}}},
        {.rect{{590, 250}, {790, 400}}},
    }};
    std::vector<ui_toggle_box> ui_toggle_boxes{{
        {.rect{{10, 10}, {40, 40}}},
        {.rect{{10, 45}, {40, 75}}},
        {.rect{{10, 80}, {40, 110}}},
        {.rect{{10, 115}, {40, 145}}},
    }};
    std::vector<ui_slider> ui_sliders{{
        {.rect{{10, 160}, {200, 176}}},
        {.rect{{10, 190}, {200, 206}}},
        {.rect{{10, 220}, {200, 236}}},
        {.rect{{10, 250}, {200, 266}}},
    }};

    my_app() : coel::application({800, 800}, "user interface") {
        use_raw_mouse(true);
    }

    void on_draw() {
        is_paused ? glClearColor(0.2f, 0.2f, 0.2f, 1.0f) : glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (auto &i : ui_toggle_boxes)
            i.draw(ui_batch);
        for (auto &i : ui_sliders)
            i.draw(ui_batch);
        for (auto &i : ui_windows)
            i.draw(ui_batch);
    }

    void on_event() {
        for (auto &i : ui_toggle_boxes)
            if (press(input, i))
                break;
        for (auto &i : ui_sliders)
            if (grab(input, i))
                break;

        std::ranges::reverse_view i_rev{ui_windows};
        if (auto grabbed = std::ranges::find_if(i_rev, [&](auto &b) { return grab(input, b); }); grabbed != ui_windows.rend()) {
            auto new_end = *grabbed;
            ui_windows.erase(std::next(grabbed).base());
            ui_windows.push_back(new_end);
        }
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
