#pragma once

#include <cstddef>
#include <coel/opengl/buffers.hpp>
#include <vector>

namespace opengl {
    template <typename self_t, typename vertex_t, std::size_t max_vcount_i>
    class batch {
      protected:
        using vertex = vertex_t;

        static constexpr std::size_t
            max_vcount = max_vcount_i,
            vbuffer_size = sizeof(vertex) * max_vcount;

        opengl::vertex_array vao;
        opengl::vertex_buffer_dynamic vbo = opengl::vertex_buffer_dynamic(nullptr, sizeof(vertex) * max_vcount);

        unsigned int current_vcount = 0;
        vertex *vbuffer_ptr = nullptr;
        bool complete = true;

      public:
        ~batch() {
            if (!complete)
                end();
        }

        void begin() {
            if (!complete)
                throw coel::exception("Attempting to begin batch while incomplete");
            complete = false;
            vao.bind();
            vbuffer_ptr = vbo.map<vertex>();
            current_vcount = 0;
        }

        void submit(const std::array<vertex, 3> &vertices) {
            if (current_vcount + 3 > max_vcount) {
                end();
                flush();
                begin();
            }

            *reinterpret_cast<std::array<vertex_t, 3> *>(vbuffer_ptr) = vertices;

            vbuffer_ptr += 3;
            current_vcount += 3;
        }

        void end() {
            vbo.unmap();
            vbo.unbind();
            complete = true;
        }

        void flush() {
            static_cast<self_t *>(this)->before_flush();
            vao.bind();
            glDrawArrays(GL_TRIANGLES, 0, current_vcount);
        }
    };

    template <typename self_t, typename vertex_t, std::size_t max_vcount_i, std::size_t max_icount_i>
    class batch_indexed {
      protected:
        using vertex = vertex_t;

        static constexpr std::size_t
            max_vcount = max_vcount_i,
            max_icount = max_icount_i,
            vbuffer_size = sizeof(vertex) * max_vcount,
            ibuffer_size = sizeof(unsigned int) * max_icount;

        opengl::vertex_array vao;
        opengl::vertex_buffer_dynamic vbo = opengl::vertex_buffer_dynamic(nullptr, sizeof(vertex) * max_vcount);
        opengl::index_buffer_dynamic ibo = opengl::index_buffer_dynamic(nullptr, sizeof(unsigned int) * max_icount);

        unsigned int current_vcount = 0, current_icount = 0;
        vertex *vbuffer_ptr = nullptr;
        unsigned int *ibuffer_ptr = nullptr;
        bool complete = true;

      public:
        ~batch_indexed() {
            if (!complete)
                end();
        }

        void begin() {
            if (!complete)
                throw coel::exception("Attempting to begin batch while incomplete");
            complete = false;
            vao.bind();
            vbuffer_ptr = vbo.map<vertex>();
            ibuffer_ptr = ibo.map<unsigned int>();
            current_vcount = 0;
            current_icount = 0;
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

        void submit(const std::vector<vertex> &vertices,
                    const std::vector<unsigned int> &indices) {
            auto vcount = vertices.size(), icount = indices.size();

            if (current_vcount + vcount > max_vcount || current_icount + icount > max_icount) {
                end();
                flush();
                begin();
            }
            for (const auto &v : vertices) {
                *vbuffer_ptr = v;
                ++vbuffer_ptr;
            }
            for (const auto &i : indices) {
                *ibuffer_ptr = i + current_vcount;
                ++ibuffer_ptr;
            }

            // vbuffer_ptr += vcount, ibuffer_ptr += icount;
            current_vcount += vcount, current_icount += icount;
        }

        void end() {
            vbo.unmap();
            vbo.unbind();
            ibo.unmap();
            ibo.unbind();
            complete = true;
        }

        void flush() {
            static_cast<self_t *>(this)->before_flush();
            vao.bind();
            ibo.bind();
            glDrawElements(GL_TRIANGLES, current_icount, GL_UNSIGNED_INT, nullptr);
        }
    };
} // namespace opengl
