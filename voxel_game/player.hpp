#include <coel/glfw_window.hpp>
#include <coel/graphics/camera.hpp>

struct player3d {
    graphics::camera3d cam;

    // clang-format off
    bool move_foreward : 1 = false,
        move_backward : 1 = false,
        move_left : 1 = false,
        move_right : 1 = false,
        move_up : 1 = false,
        move_down : 1 = false;
    // clang-format on

    void init() {
        cam.pos.z = 5;
        cam.pos.y = 2;
        cam.move_speed = 10;
    }

    void resize_cam(glm::ivec2 frame_dim) {
        cam.aspect = static_cast<float>(frame_dim.x) / frame_dim.y;
        cam.update_proj();
    }

    void move_mouse(glm::vec2 mouse_diff) {
        cam.rot.x -= mouse_diff.y * 0.001;
        cam.rot.y -= mouse_diff.x * 0.001;
        cam.update_rot();
    }

    void key_press(int button, int scancode, int action, int mods) {
        switch (action) {
        case GLFW_PRESS:
            switch (button) {
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
            switch (button) {
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
            cam.pos -= cam.move_speed * cam.look * (float)elapsed;
        if (move_backward)
            cam.pos += cam.move_speed * cam.look * (float)elapsed;
        if (move_left)
            cam.pos += cam.move_speed * cam.look_bi * (float)elapsed;
        if (move_right)
            cam.pos -= cam.move_speed * cam.look_bi * (float)elapsed;
        if (move_up)
            cam.pos.y += cam.move_speed * (float)elapsed;
        if (move_down)
            cam.pos.y -= cam.move_speed * (float)elapsed;

        cam.update_view();
    }
};
