#pragma once

#include <glad/glad.h>
#include <iostream>

#include <coel/opengl/texture.hpp>

namespace opengl {
    struct renderbuffer {
        unsigned int id;

        struct configuration {
            glm::uvec2 dim;
            unsigned int gl_format;
        };

        renderbuffer() {
            glGenRenderbuffers(1, &id);
            bind();
        }
        renderbuffer(const configuration &conf) : renderbuffer() {
            regenerate(conf);
        }
        renderbuffer(const renderbuffer &) = delete;
        renderbuffer(renderbuffer &&other) {
            this->~renderbuffer();
            id = other.id;
            other.id = (unsigned int)-1;
        }
        renderbuffer &operator=(const renderbuffer &) = delete;
        renderbuffer &operator=(renderbuffer &&other) {
            this->~renderbuffer();
            id = other.id;
            other.id = (unsigned int)-1;
            return *this;
        }
        ~renderbuffer() {
            glDeleteRenderbuffers(1, &id);
        }

        void regenerate(const configuration &conf) {
            glRenderbufferStorage(GL_RENDERBUFFER, conf.gl_format, conf.dim.x, conf.dim.y);
        }

        inline void bind() {
            glBindRenderbuffer(GL_RENDERBUFFER, id);
        }

        static inline void unbind() {
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }
    };

    struct framebuffer {
        unsigned int id;

        framebuffer() {
            glGenFramebuffers(1, &id);
            glBindFramebuffer(GL_FRAMEBUFFER, id);
        }
        framebuffer(const framebuffer &) = delete;
        framebuffer(framebuffer &&other) {
            this->~framebuffer();
            id = other.id;
            other.id = (unsigned int)-1;
        }
        framebuffer &operator=(const framebuffer &) = delete;
        framebuffer &operator=(framebuffer &&other) {
            this->~framebuffer();
            id = other.id;
            other.id = (unsigned int)-1;
            return *this;
        }
        ~framebuffer() {
            glDeleteFramebuffers(1, &id);
        }

        template <GLenum target>
        static inline void attach(const texture<target, std::uint8_t, glm::uvec2> &tex, unsigned int gl_attachment_id) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, gl_attachment_id, target, tex.id, 0);
        }

        static inline void attach(const renderbuffer &rbo, unsigned int gl_attachment_id) {
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, gl_attachment_id, GL_RENDERBUFFER, rbo.id);
        }

        static void verify() {
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "failed to complete creation of framebuffer\n";
        }

        inline void bind() const {
            glBindFramebuffer(GL_FRAMEBUFFER, id);
        }

        static inline void unbind() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    };
} // namespace opengl
