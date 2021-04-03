#pragma once

#include <coel/opengl/batch.hpp>
#include <coel/opengl/shader.hpp>
#include <coel/opengl/renderers/shaders.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace opengl { namespace renderer {
    struct ui_vertex {
        glm::vec2 pos, size, tex;
        glm::vec4 col;
    };
    class ui_batch : public batch_indexed<ui_batch, ui_vertex, 10000, 10000> {
        opengl::shader_program shader;
        opengl::shader_uniform u_view_mat;

      public:
        ui_batch() : batch_indexed<ui_batch, ui_vertex, 10000, 10000>(),
                     shader({.source_str = ui_batch_vert}, {.source_str = ui_batch_frag}) {
            u_view_mat = shader.find_uniform("u_view_mat");
            vao.bind();
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<const void *>(0));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<const void *>(1 * sizeof(glm::vec2)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<const void *>(2 * sizeof(glm::vec2)));
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<const void *>(3 * sizeof(glm::vec2)));
            vao.unbind();
        }

        void resize(glm::ivec2 size) {
            const glm::vec2 scale = {2.0f / size.x, -2.0f / size.y};
            glm::mat4 view = glm::scale(glm::translate(glm::mat4{1}, glm::vec3{-1, 1, 0}), glm::vec3{scale.x, scale.y, 1});
            shader.bind();
            glUniformMatrix4fv(u_view_mat.location, 1, false, reinterpret_cast<float *>(&view));
        }

        void submit_rect(glm::vec2 p1, glm::vec2 p2, glm::vec4 color) {
            glm::vec2 size = p2 - p1;

            submit<4, 6>(
                {
                    // top right
                    ui_vertex{
                        .pos = {p2.x, p2.y},
                        .size = size,
                        .tex = {1, 1},
                        .col = color,
                    },
                    // bottom right
                    ui_vertex{
                        .pos = {p2.x, p1.y},
                        .size = size,
                        .tex = {1, 0},
                        .col = color,
                    },
                    // bottom left
                    ui_vertex{
                        .pos = {p1.x, p1.y},
                        .size = size,
                        .tex = {0, 0},
                        .col = color,
                    },
                    // top left
                    ui_vertex{
                        .pos = {p1.x, p2.y},
                        .size = size,
                        .tex = {0, 1},
                        .col = color,
                    },
                },
                {
                    0 + current_vcount, 1 + current_vcount, 3 + current_vcount, // first triangle
                    1 + current_vcount, 2 + current_vcount, 3 + current_vcount  // second triangle
                });
        }

        void before_flush() {
            shader.bind();
        }
    };
}} // namespace opengl::renderer
