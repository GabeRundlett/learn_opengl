#pragma once

#include <glm/glm.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <bitset>

namespace coel {

    enum mouse_button {
        left,
        right,
        middle,
    };

    enum key {
        a, b, c,
    };

    struct input_state {
        struct {
            std::bitset<32> buttons;
            struct {
                int button = -1, mods = -1, action = -1;
            } active;

            glm::vec2 cursor_pos;
            glm::vec2 cursor_offset;
            glm::vec2 scroll_offset;
        } mouse;

        struct {
            struct {
                int key = -1, mods = -1, action = -1;
            } active;
            std::bitset<512> keys;
        } keyboard;
    };

    struct mouse_button_event {
        int button;
        int action;
        int mods;
    };
    struct key_event {
        int key;
        int scancode;
        int action;
        int mods;
    };
} // namespace coel
