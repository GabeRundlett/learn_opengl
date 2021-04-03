#pragma once

#include <concepts>
#include <algorithm>
#include <ranges>

#include <coel/input.hpp>

namespace coel {
    template <typename t>
    concept container = requires(const t obj) {
        { obj.contains(glm::vec2{}) } -> std::convertible_to<bool>;
    };

    template <typename t>
    concept hoverable = requires(t obj) {
        requires container<t>;
        { obj.is_hovered() } -> std::convertible_to<bool>;
        obj.mouse_enter(glm::vec2{});
        obj.mouse_over(glm::vec2{});
        obj.mouse_exit();
    };

    template <typename t>
    concept pressable = requires(t obj) {
        requires hoverable<t>;
        { obj.is_pressed() } -> std::convertible_to<bool>;
        obj.press(glm::vec2{});
        obj.release();
    };

    template <typename t>
    concept grabbable = requires(t obj) {
        requires pressable<t>;
        obj.drag(glm::vec2{});
    };

    template <hoverable t>
    bool hover(const coel::input_state &input, t &obj) {
        if (obj.is_hovered()) {
            if (obj.contains(input.mouse.cursor_pos)) {
                obj.mouse_over(input.mouse.cursor_pos);
                return true;
            } else {
                obj.mouse_exit();
            }
        } else if (obj.contains(input.mouse.cursor_pos)) {
            obj.mouse_enter(input.mouse.cursor_pos);
            return true;
        }
        return false;
    }

    template <pressable t>
    bool press(const coel::input_state &input, t &obj) {
        if (obj.is_pressed() && !input.mouse.buttons[GLFW_MOUSE_BUTTON_LEFT] && input.mouse.active.action == GLFW_RELEASE)
            obj.release();
        else if (hover(input, obj) && input.mouse.buttons[GLFW_MOUSE_BUTTON_LEFT] && input.mouse.active.action == GLFW_PRESS)
            obj.press(input.mouse.cursor_pos);
        return obj.is_pressed();
    }

    template <grabbable t>
    bool grab(const coel::input_state &input, t &obj) {
        if (press(input, obj)) {
            obj.drag(input.mouse.cursor_pos);
            return true;
        }
        return false;
    }

    struct rectangular_container {
        glm::vec2 top_left, bottom_right;

        bool contains(const glm::vec2 p) const {
            return p.x >= top_left.x && p.x < bottom_right.x && p.y >= top_left.y && p.y < bottom_right.y;
        }
    };

    struct rectangle_mover {
        bool grabbed = false;
        glm::vec2 grab_offset;

        constexpr bool is_pressed() const { return grabbed; }

        void press(const glm::vec2 p, rectangular_container &rect) {
            grab_offset = rect.top_left - p;
            grabbed = true;
        }

        void drag(const glm::vec2 p, rectangular_container &rect) const {
            auto current_width = rect.bottom_right - rect.top_left;
            rect.top_left = grab_offset + p;
            rect.bottom_right = grab_offset + p + current_width;
        }

        void release() {
            grabbed = false;
        }
    };

    struct rectangle_resizer {
        enum edge_flag {
            right_edge = 1 << 0,
            bottom_edge = 1 << 1,
            left_edge = 1 << 2,
            top_edge = 1 << 3,
        };
        bool grabbed = false;
        glm::vec2 grab_offset;
        int edge = 0;

        constexpr bool is_pressed() const { return grabbed; }

        void press(const glm::vec2 p, const rectangular_container &rect, const glm::vec2 resize_margin = {0, 0}) {
            if (p.x >= rect.bottom_right.x - resize_margin.x) {
                edge |= right_edge;
                grab_offset.x = rect.bottom_right.x - p.x;
                grabbed = true;
            }
            if (p.y >= rect.bottom_right.y - resize_margin.y) {
                edge |= bottom_edge;
                grab_offset.y = rect.bottom_right.y - p.y;
                grabbed = true;
            }
            if (p.x <= rect.top_left.x + resize_margin.x) {
                edge |= left_edge;
                grab_offset.x = rect.top_left.x - p.x;
                grabbed = true;
            }
            if (p.y <= rect.top_left.y + resize_margin.y) {
                edge |= top_edge;
                grab_offset.y = rect.top_left.y - p.y;
                grabbed = true;
            }
        }

        void drag(const glm::vec2 p, rectangular_container &rect, const glm::vec2 resize_margin = {0, 0}) {
            if (edge & right_edge)
                rect.bottom_right.x = std::max(grab_offset.x + p.x, rect.top_left.x + resize_margin.x * 2 + 1.0f);
            if (edge & bottom_edge)
                rect.bottom_right.y = std::max(grab_offset.y + p.y, rect.top_left.y + resize_margin.y * 2 + 1.0f);
            if (edge & left_edge)
                rect.top_left.x = std::min(grab_offset.x + p.x, rect.bottom_right.x - resize_margin.x * 2 - 1.0f);
            if (edge & top_edge)
                rect.top_left.y = std::min(grab_offset.y + p.y, rect.bottom_right.y - resize_margin.y * 2 - 1.0f);
        }

        void release() {
            grabbed = false;
            edge = 0;
        }
    };
} // namespace coel
