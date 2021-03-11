#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace graphics {
    struct camera3d {
        glm::vec3 pos = {0, 0, 0}, rot = {0, 0, 0};
        float fov = glm::radians(90.0f), aspect = 1.0f, znear = 0.01f, zfar = 1000.0f;

        glm::vec3 look = {0, 0, 1}, look_bi = {-1, 0, 0}, look_up = {0, 1, 0};
        glm::mat4 proj_mat = glm::identity<glm::mat4>(), view_mat = glm::identity<glm::mat4>();

        void update_proj() {
            proj_mat = glm::perspective(fov, aspect, znear, zfar);
        }

        void update_rot() {
            look = glm::rotateY(glm::vec3{0, 0, 1}, rot.y);
            look_bi = {-look.z, look.y, look.x};
        }

        void update_view() {
            view_mat = glm::translate(
                glm::rotate(
                    glm::rotate(glm::identity<glm::mat4>(), -rot.x, glm::vec3(1, 0, 0)),
                    -rot.y, glm::vec3(0, 1, 0)),
                -pos);
        }
    };
} // namespace graphics
