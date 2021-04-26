#pragma once

#include <coel/exception.hpp>

#include <glad/glad.h>
#include <cstddef>
#include <fmt/core.h>

namespace opengl {
    struct vertex_array {
      private:
        template <typename attr_t>
        static inline constexpr unsigned int get_attrib_type() = delete;
        template <>
        inline constexpr unsigned int get_attrib_type<float>() { return GL_FLOAT; }
        template <>
        inline constexpr unsigned int get_attrib_type<glm::vec2>() { return GL_FLOAT; }
        template <>
        inline constexpr unsigned int get_attrib_type<glm::vec3>() { return GL_FLOAT; }
        template <>
        inline constexpr unsigned int get_attrib_type<glm::vec4>() { return GL_FLOAT; }

        template <typename attr_t>
        static inline constexpr unsigned int get_attrib_dim() = delete;
        template <>
        inline constexpr unsigned int get_attrib_dim<float>() { return 1; }
        template <>
        inline constexpr unsigned int get_attrib_dim<glm::vec2>() { return 2; }
        template <>
        inline constexpr unsigned int get_attrib_dim<glm::vec3>() { return 3; }
        template <>
        inline constexpr unsigned int get_attrib_dim<glm::vec4>() { return 4; }

      public:
        unsigned int id;

        template <typename... param_t>
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

        template <typename attr_t, typename... param_t>
        static inline void set_layout(unsigned int i = 0, std::size_t offset = 0, int stride = 0) {
            if constexpr (sizeof...(param_t) > 0)
                if (stride == 0)
                    stride = sizeof(attr_t) + (sizeof(param_t) + ...);
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(
                i, get_attrib_dim<attr_t>(), get_attrib_type<attr_t>(),
                GL_FALSE, stride, reinterpret_cast<const void *>(offset));
            if constexpr (sizeof...(param_t) > 0)
                set_layout<param_t...>(i + 1, offset + sizeof(attr_t), stride);
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
        buffer(const void *const data, const std::size_t size) : buffer() {
            glBufferData(target, size, data, usage);
        }
        buffer(const buffer &) = delete;
        buffer(buffer &&other) {
            this->~buffer();
            id = other.id;
            other.id = (unsigned int)-1;
        }
        buffer &operator=(const buffer &) = delete;
        buffer &operator=(buffer &&other) {
            this->~buffer();
            id = other.id;
            other.id = (unsigned int)-1;
            return *this;
        }
        ~buffer() {
            glDeleteBuffers(1, &id);
        }

        void bind() const {
            glBindBuffer(target, id);
        }

        template <typename elem_t>
        elem_t *map(unsigned int access = GL_READ_WRITE) {
            static_assert(usage == GL_DYNAMIC_DRAW);
            bind();
            return reinterpret_cast<elem_t *>(glMapBuffer(target, access));
        }

        void unmap() const {
            bind();
            glUnmapBuffer(target);
        }

        static inline void unbind() {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    };

    using vertex_buffer = buffer<GL_ARRAY_BUFFER, GL_STATIC_DRAW>;
    using vertex_buffer_dynamic = buffer<GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW>;
    using index_buffer = buffer<GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW>;
    using index_buffer_dynamic = buffer<GL_ELEMENT_ARRAY_BUFFER, GL_DYNAMIC_DRAW>;
} // namespace opengl
