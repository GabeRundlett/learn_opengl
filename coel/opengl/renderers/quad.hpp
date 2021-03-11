#pragma once

#include <coel/opengl/shader.hpp>
#include <coel/opengl/buffers.hpp>

namespace opengl { namespace renderer {
    class quad {
        // clang-format off
        static inline constexpr std::array quad_vertices = {
            -1.0f, -1.0f,
            -1.0f,  1.0f,
            1.0f,  1.0f,
            1.0f, -1.0f,
        };
        // clang-format on

        opengl::vertex_array vao;
        opengl::vertex_buffer vbo = opengl::vertex_buffer(quad_vertices.data(), quad_vertices.size() * sizeof(quad_vertices[0]));

      public:
        quad() {
            vao.bind();
            opengl::vertex_array::set_layout<glm::vec2>();
        }

        void draw() {
            vao.bind();
            glDrawArrays(GL_QUADS, 0, 4);
        }
    };
}} // namespace opengl::renderer
