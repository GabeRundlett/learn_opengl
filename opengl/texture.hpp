#pragma once

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>

namespace opengl {
    template <GLenum target>
    struct texture {
        unsigned int id = -1;

        struct configuration {
            const char *filepath = nullptr;
            const std::uint8_t *data = nullptr;
            unsigned int width, height;
            unsigned int data_format, data_type = GL_UNSIGNED_BYTE, gl_format;
            struct wrapping {
                unsigned int s = GL_REPEAT, t = GL_REPEAT;
            } wrap;
            struct filtering {
                unsigned int min = GL_LINEAR, max = GL_LINEAR;
            } filter;
            glm::vec4 border_color;
            int samples = 0;
            std::uint8_t use_mipmap : 1 = false;
        };

        texture() {
            glGenTextures(1, &id);
            bind();
        }
        texture(const configuration &conf) : texture() {
            regenerate(conf);
        }
        texture(const texture &) = delete;
        texture(texture &&other) {
            this->~texture();
            id = other.id;
            other.id = -1;
        }
        texture &operator=(const texture &) = delete;
        texture &operator=(texture &&other) {
            this->~texture();
            id = other.id;
            other.id = -1;
            return *this;
        }

        void regenerate(const configuration &conf) {
            if (conf.filepath != nullptr) {
                int width, height, num_channels;
                std::uint8_t *data = stbi_load(conf.filepath, &width, &height, &num_channels, 0);

                unsigned int format;
                switch (num_channels) {
                case 1: format = GL_RED; break;
                case 3: format = GL_RGB; break;
                case 4: format = GL_RGBA; break;
                }

                regenerate({
                    .filepath = nullptr,
                    .data = data,
                    .width = (unsigned int)width,
                    .height = (unsigned int)height,
                    .data_format = format,
                    .data_type = conf.data_type,
                    .gl_format = conf.gl_format,
                    .wrap = {conf.wrap.s, conf.wrap.t},
                    .filter = {conf.filter.min, conf.filter.max},
                    .border_color = conf.border_color,
                    .use_mipmap = conf.use_mipmap,
                });

                stbi_image_free(data);
            } else {
                bind();
                if constexpr (target == GL_TEXTURE_2D_MULTISAMPLE) {
                    glTexImage2DMultisample(target, conf.samples, conf.gl_format, conf.width, conf.height, false);
                } else {
                    glTexImage2D(target, 0, conf.gl_format, conf.width, conf.height, 0, conf.data_format, conf.data_type, conf.data);
                    glTexParameteri(target, GL_TEXTURE_WRAP_S, conf.wrap.s);
                    glTexParameteri(target, GL_TEXTURE_WRAP_T, conf.wrap.t);
                    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, conf.filter.min);
                    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, conf.filter.max);
                    if (conf.use_mipmap)
                        glGenerateMipmap(target);
                    if (conf.wrap.s == GL_CLAMP_TO_BORDER || conf.wrap.t == GL_CLAMP_TO_BORDER)
                        glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, reinterpret_cast<const float *>(&conf.border_color));
                }
            }
        }

        ~texture() {
            glDeleteTextures(1, &id);
        }

        inline void bind() const {
            glBindTexture(target, id);
        }

        inline void bind(unsigned int index) const {
            glActiveTexture(GL_TEXTURE0 + index);
            glBindTexture(target, id);
        }

        static inline void unbind() {
            glBindTexture(target, 0);
        }
    };

    using texture2d = texture<GL_TEXTURE_2D>;
    using texture2d_multisample = texture<GL_TEXTURE_2D_MULTISAMPLE>;
} // namespace opengl
