#pragma once

#include "ui1.hpp"
#include <coel/application.hpp>

#include <functional>
#include <memory>

#include <string>
using namespace std::string_literals;

#include <iostream>

class ui_component {
  public:
    ui_component() {}
    virtual ~ui_component() {}

    virtual bool on_event(coel::input_state &input) = 0;
    virtual void draw(opengl::renderer::ui_batch &ui, opengl::renderer::text_batch &text) = 0;
};

struct button {
    std::function<std::string()> text;
    std::function<coel::rectangular_container()> rect;
    std::function<void()> call;
};
class ui_button : public ui_component {
    coel::rectangular_container rect;
    bool pressed = false, hovered = false;
    bool toggled = false;

    button config;

  public:
    ui_button(button config) : config(config) { rect = config.rect(); }
    ~ui_button() {}

    bool on_event(coel::input_state &input) override { return coel::press(input, *this); }
    bool contains(glm::vec2 p) const { return rect.contains(p); }
    bool is_hovered() const { return hovered; }
    void mouse_enter(glm::vec2) { hovered = true; }
    void mouse_over(glm::vec2) {}
    void mouse_exit() { hovered = false; }
    bool is_pressed() const { return pressed; }
    void press(glm::vec2) { pressed = true; }
    void release() { pressed = false, toggled = !toggled; }

    void draw(opengl::renderer::ui_batch &ui, opengl::renderer::text_batch &) override {
        glm::vec4 color = {0.25, 0.25, 0.25, 1};
        if (hovered | pressed)
            color.r = 0.5f;
        ui.submit_rect(rect.top_left, rect.bottom_right, color);
        glm::vec2 toggle_border{4, 4};
        if (toggled)
            ui.submit_rect(rect.top_left + toggle_border, rect.bottom_right - toggle_border, {color.r * 2, color.r * 2, color.r * 2, 1});
    }
};

struct slider {
    std::function<std::string()> text;
    std::function<coel::rectangular_container()> rect;
    std::function<void(float)> call;

    float value;
    struct range_conf {
        float min, max;
    } range;
};
class ui_slider : public ui_component {
    coel::rectangular_container rect;
    bool grabbed = false, hovered = false;
    float val;

    slider config;

  public:
    ui_slider(slider config) : val((config.value - config.range.min) / (config.range.max - config.range.min)), config(config) { rect = config.rect(); }
    ~ui_slider() {}

    bool on_event(coel::input_state &input) override { return coel::grab(input, *this); }
    bool contains(glm::vec2 p) const { return rect.contains(p); }
    bool is_hovered() const { return hovered; }
    void mouse_enter(glm::vec2) { hovered = true; }
    void mouse_over(glm::vec2) {}
    void mouse_exit() { hovered = false; }
    bool is_pressed() const { return grabbed; }
    void press(glm::vec2) { grabbed = true; }
    void drag(glm::vec2 p) {
        val = (p.x - rect.top_left.x) / (rect.bottom_right.x - rect.top_left.x);
        val = std::max(std::min(val, 1.0f), 0.0f);
        config.call(val * (config.range.max - config.range.min) + config.range.min);
    }
    void release() { grabbed = false; }

    void draw(opengl::renderer::ui_batch &ui, opengl::renderer::text_batch &text) override {
        glm::vec4 color = {0.25, 0.25, 0.25, 1};
        if (hovered | grabbed)
            color.b = 0.5f;
        ui.submit_rect(rect.top_left, rect.bottom_right, color);
        auto pix_val = val * (rect.bottom_right.x - rect.top_left.x) + rect.top_left.x;
        ui.submit_rect({pix_val - 2.0f, rect.top_left.y}, {pix_val + 2.0f, rect.bottom_right.y}, {color.b * 2, color.b * 2, color.b * 2, 1});

        constexpr glm::vec4 text_col = {0.9f, 0.9f, 0.9f, 1.0f};
        opengl::renderer::component_bounds bounds;
        text.submit(glm::vec2(rect.top_left.x, rect.top_left.y - 20), config.text(), 14.0f, text_col, &bounds);
    }
};

struct ui_window {
    coel::rectangular_container rect;
    std::vector<std::shared_ptr<ui_component>> components;

    void on_event(coel::input_state &input) {
        for (auto &c : components)
            if (c->on_event(input))
                break;
    }

    void resize(glm::ivec2 frame_dim) {
        rect = {
            {10, 10},
            {210, float(frame_dim.y) - 10},
        };
    }

    void draw(opengl::renderer::ui_batch &ui, opengl::renderer::text_batch &text) {
        const glm::vec4 color = {0.1, 0.1, 0.1, 1};
        ui.submit_rect(rect.top_left, rect.bottom_right, color);
        for (auto &c : components)
            c->draw(ui, text);
    }
};
