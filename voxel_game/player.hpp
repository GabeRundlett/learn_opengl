#include <coel/graphics/camera.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

struct player3d {
    graphics::camera3d cam{.pos = {0, 0, 0}};

    // clang-format off
    bool move_foreward : 1 = false,
        move_backward : 1 = false,
        move_left : 1 = false,
        move_right : 1 = false,
        move_up : 1 = false,
        move_down : 1 = false;
    // clang-format on
    float move_speed = 32.0f;

    void resize_cam(glm::uvec2 frame_dim) {
        cam.aspect = static_cast<float>(frame_dim.x) / frame_dim.y;
        cam.update_proj();
    }

    void move_mouse(glm::vec2 mouse_diff) {
        cam.rot.x -= mouse_diff.y * 0.001f;
        cam.rot.y -= mouse_diff.x * 0.001f;
        cam.update_rot();
    }

    void key_press(const coel::key_event &e) {
        switch (e.action) {
        case GLFW_PRESS:
            switch (e.key) {
            case GLFW_KEY_W: move_foreward = true; break;
            case GLFW_KEY_S: move_backward = true; break;
            case GLFW_KEY_A: move_left = true; break;
            case GLFW_KEY_D: move_right = true; break;
            case GLFW_KEY_SPACE: move_up = true; break;
            case GLFW_KEY_LEFT_SHIFT: move_down = true; break;
            default: break;
            }
            break;
        case GLFW_RELEASE:
            switch (e.key) {
            case GLFW_KEY_W: move_foreward = false; break;
            case GLFW_KEY_S: move_backward = false; break;
            case GLFW_KEY_A: move_left = false; break;
            case GLFW_KEY_D: move_right = false; break;
            case GLFW_KEY_SPACE: move_up = false; break;
            case GLFW_KEY_LEFT_SHIFT: move_down = false; break;
            default: break;
            }
            break;
        }
    }

    void update(double elapsed) {
        if (move_foreward)
            cam.pos -= move_speed * cam.look * (float)elapsed;
        if (move_backward)
            cam.pos += move_speed * cam.look * (float)elapsed;
        if (move_left)
            cam.pos += move_speed * cam.look_bi * (float)elapsed;
        if (move_right)
            cam.pos -= move_speed * cam.look_bi * (float)elapsed;
        if (move_up)
            cam.pos.y += move_speed * (float)elapsed;
        if (move_down)
            cam.pos.y -= move_speed * (float)elapsed;

        cam.update_view();
    }
};
