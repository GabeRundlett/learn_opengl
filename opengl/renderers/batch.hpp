#pragma once

#include <cstddef>
#include "../buffers.hpp"

namespace opengl { namespace renderer {
    template <typename self_t, typename vertex_t, std::size_t max_vcount_i, std::size_t max_icount_i>
    class batch {
      protected:
        using vertex = vertex_t;

        static constexpr std::size_t
            max_vcount = max_vcount_i,
            max_icount = max_icount_i,
            vbuffer_size = sizeof(vertex) * max_vcount,
            ibuffer_size = sizeof(unsigned int) * max_icount;

        opengl::vertex_array vao;
        unsigned int vbo_id, ibo_id;

        unsigned int current_vcount = 0, current_icount = 0;
        vertex *vbuffer_ptr = nullptr;
        unsigned int *ibuffer_ptr = nullptr;

      public:
        batch() : vao() {
            glGenBuffers(1, &vbo_id);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
            glBufferData(GL_ARRAY_BUFFER, vbuffer_size, nullptr, GL_DYNAMIC_DRAW);

            glGenBuffers(1, &ibo_id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibuffer_size, nullptr, GL_DYNAMIC_DRAW);
        }

        void begin() {
            vao.bind();
            glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);

            vbuffer_ptr = reinterpret_cast<decltype(vbuffer_ptr)>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
            ibuffer_ptr = reinterpret_cast<decltype(ibuffer_ptr)>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE));
        }

        template <std::size_t vcount, std::size_t icount>
        void submit(const std::array<vertex, vcount> &vertices,
                    const std::array<unsigned int, icount> &indices) {
            if (current_vcount + vcount > max_vcount ||
                current_icount + icount > max_icount) {
                end();
                flush();
                begin();
            }

            *reinterpret_cast<std::array<vertex_t, vcount> *>(vbuffer_ptr) = vertices;
            *reinterpret_cast<std::array<unsigned int, icount> *>(ibuffer_ptr) = indices;

            vbuffer_ptr += vcount, ibuffer_ptr += icount;
            current_vcount += vcount, current_icount += icount;
        }

        void end() {
            vao.bind();
            glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);

            glUnmapBuffer(GL_ARRAY_BUFFER);
            glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        }

        void flush() {
            static_cast<self_t *>(this)->before_flush();
            vao.bind();
            glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);
            glDrawElements(GL_TRIANGLES, current_icount, GL_UNSIGNED_INT, nullptr);
            current_vcount = 0;
            current_icount = 0;
        }
    };
}} // namespace opengl::renderer
