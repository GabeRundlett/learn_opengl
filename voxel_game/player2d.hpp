#pragma once

#include <glm/glm.hpp>

class player2d {
    // clang-format off
    bool move_left : 1 = false,
        move_right : 1 = false,
        move_up : 1 = false,
        move_down : 1 = false;
    // clang-format on
  public:
    glm::vec2 pos = {0, 0};
    float move_speed = 32.0f;

    void key_press(const coel::key_event &e) {
        switch (e.action) {
        case GLFW_PRESS:
            switch (e.key) {
            case GLFW_KEY_W: move_up = true; break;
            case GLFW_KEY_S: move_down = true; break;
            case GLFW_KEY_A: move_left = true; break;
            case GLFW_KEY_D: move_right = true; break;
            default: break;
            }
            break;
        case GLFW_RELEASE:
            switch (e.key) {
            case GLFW_KEY_W: move_up = false; break;
            case GLFW_KEY_S: move_down = false; break;
            case GLFW_KEY_A: move_left = false; break;
            case GLFW_KEY_D: move_right = false; break;
            default: break;
            }
            break;
        }
    }

    void update(double elapsed) {
        if (move_left)
            pos.x += move_speed * (float)elapsed;
        if (move_right)
            pos.x -= move_speed * (float)elapsed;
        if (move_up)
            pos.y += move_speed * (float)elapsed;
        if (move_down)
            pos.y -= move_speed * (float)elapsed;
    }
};
