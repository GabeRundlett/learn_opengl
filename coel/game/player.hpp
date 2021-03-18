#include <coel/application.hpp>
#include <coel/graphics/camera.hpp>

#include <numbers>

namespace coel {
    static inline constexpr auto sign(auto x) -> decltype(x) {
        return x < 0 ? -1 : 1;
    }

    struct player3d {
        graphics::camera3d cam;
        glm::vec3 vel = {0, 0, 0};
        glm::vec3 vel_dir = glm::normalize(vel);

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
        float current_move_speed = move_speed;

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

        void key_press(const key_event &e) {
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

        void update(duration elapsed) {
            bool moving = move_foreward | move_backward | move_left | move_right | move_up | move_down;
            if (!moving)
                current_move_speed = move_speed;
            else if (move_sprint)
                current_move_speed = move_speed * move_sprint_mult;

            float accel = 500.0f;
            float dt = (float)elapsed.count();

            if (move_foreward)
                vel -= accel * cam.look * dt;
            if (move_backward)
                vel += accel * cam.look * dt;
            if (move_left)
                vel += accel * normalize(glm::vec3(cam.look_bi.x, 0, cam.look_bi.z)) * dt;
            if (move_right)
                vel -= accel * normalize(glm::vec3(cam.look_bi.x, 0, cam.look_bi.z)) * dt;
            if (move_up)
                vel.y += accel * dt;
            if (move_down)
                vel.y -= accel * dt;

            float speed = glm::length(vel);
            if (speed != 0) {
                vel_dir = vel / speed;
                speed = std::min(speed, current_move_speed);
                vel = vel_dir * speed;

                cam.pos += vel * dt;
                glm::vec3 decel = vel_dir * 15.0f * dt;

                vel -= glm::vec3{
                    sign(vel.x) * std::min(std::abs(vel.x), std::abs(decel.x)),
                    sign(vel.y) * std::min(std::abs(vel.y), std::abs(decel.y)),
                    sign(vel.z) * std::min(std::abs(vel.z), std::abs(decel.z)),
                };
            }
            cam.update_view();
        }
    };
} // namespace coel
