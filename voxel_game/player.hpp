#include <coel/application.hpp>
#include <coel/graphics/camera.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <numbers>

struct player3d {
    graphics::camera3d cam{.pos = {0.5, 0.5, 0.5}, .rot = {-0.6, -0.7, 0}};

    // clang-format off
    bool move_foreward : 1 = false,
        move_backward : 1 = false,
        move_left : 1 = false,
        move_right : 1 = false,
        move_up : 1 = false,
        move_down : 1 = false,
        move_sprint : 1 = false;
    // clang-format on
    float move_speed = 8.0f;
    float move_sprint_mult = 4.0f;
    float mouse_sens = 0.001f;

    void resize_cam(glm::uvec2 frame_dim) {
        cam.aspect = static_cast<float>(frame_dim.x) / frame_dim.y;
        cam.update_proj();
    }

    void move_mouse(glm::vec2 mouse_diff) {
        cam.rot.x -= mouse_diff.y * mouse_sens;
        if (cam.rot.x < static_cast<float>(-std::numbers::pi / 2 + 0.1))
            cam.rot.x = static_cast<float>(-std::numbers::pi / 2 + 0.1);
        if (cam.rot.x > static_cast<float>(std::numbers::pi / 2 - 0.1))
            cam.rot.x = static_cast<float>(std::numbers::pi / 2 - 0.1);
        cam.rot.y -= mouse_diff.x * mouse_sens;
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
            case GLFW_KEY_LEFT_CONTROL: move_sprint = true; break;
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
            case GLFW_KEY_LEFT_CONTROL: move_sprint = false; break;
            default: break;
            }
            break;
        }
    }

    void update(coel::duration elapsed) {
        float speed = move_speed;
        if (move_sprint)
            speed *= move_sprint_mult;
        if (move_foreward)
            cam.pos -= speed * cam.look * (float)elapsed.count();
        if (move_backward)
            cam.pos += speed * cam.look * (float)elapsed.count();
        if (move_left)
            cam.pos += speed * normalize(glm::vec3(cam.look_bi.x, 0, cam.look_bi.z)) * (float)elapsed.count();
        if (move_right)
            cam.pos -= speed * normalize(glm::vec3(cam.look_bi.x, 0, cam.look_bi.z)) * (float)elapsed.count();
        if (move_up)
            cam.pos.y += speed * (float)elapsed.count();
        if (move_down)
            cam.pos.y -= speed * (float)elapsed.count();

        cam.update_view();
    }
};
