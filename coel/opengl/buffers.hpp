#pragma once

#include <glad/glad.h>
#include <cstddef>

namespace opengl {
    struct vertex_array {
        unsigned int id;

        vertex_array() {
            glGenVertexArrays(1, &id);
            bind();
        }
        vertex_array(const vertex_array &) = delete;
        vertex_array(vertex_array &&other) {
            this->~vertex_array();
            id = other.id;
            other.id = (unsigned int)-1;
        }
        vertex_array &operator=(const vertex_array &) = delete;
        vertex_array &operator=(vertex_array &&other) {
            this->~vertex_array();
            id = other.id;
            other.id = (unsigned int)-1;
            return *this;
        }

        ~vertex_array() {
            glDeleteBuffers(1, &id);
        }

        void bind() const {
            glBindVertexArray(id);
        }

        static inline void unbind() {
            glBindVertexArray(0);
        }
    };

    template <GLenum target, GLenum usage>
    struct buffer {
        unsigned int id;

        buffer() {
            glGenBuffers(1, &id);
            bind();
        }
        template <typename value_t>
        buffer(const value_t *const data, const std::size_t size) : buffer() {
            glBufferData(target, size, data, usage);
        }
        buffer(const buffer &) = delete;
        buffer(buffer &&other) {
            this->~buffer();
            id = other.id;
            other.id = -1;
        }
        buffer &operator=(const buffer &) = delete;
        buffer &operator=(buffer &&other) {
            this->~buffer();
            id = other.id;
            other.id = -1;
            return *this;
        }
        ~buffer() {
            glDeleteBuffers(1, &id);
        }

        void bind() const {
            glBindBuffer(target, id);
        }

        static inline void unbind() {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    };

    using vertex_buffer = buffer<GL_ARRAY_BUFFER, GL_STATIC_DRAW>;
    using index_buffer = buffer<GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW>;
} // namespace opengl
