#pragma once

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>

#include <fmt/core.h>
#include <coel/exception.hpp>

namespace opengl {
    template <GLenum target, typename data_t, typename dim_t>
    struct texture {
        unsigned int id = (unsigned int)-1;

        struct data_configuration {
            const data_t *ptr = nullptr;
            dim_t offset = {}, dim;
            unsigned int format, type = GL_UNSIGNED_BYTE;
        };

        struct creation_configuration {
            const char *filepath = nullptr;
            data_configuration data;
            unsigned int gl_format;
            struct wrapping {
                unsigned int s = GL_REPEAT, t = GL_REPEAT, r = GL_REPEAT;
            } wrap;
            struct filtering {
                unsigned int min = GL_LINEAR, max = GL_LINEAR;
            } filter;
            glm::vec4 border_color;
            int samples = 0;
            std::uint8_t use_mipmap : 1 = false, flip_vertically : 1 = false;
        };

        texture() {
            glGenTextures(1, &id);
            bind();
        }
        texture(const creation_configuration &create_conf) : texture() {
            recreate(create_conf);
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

        void recreate(const creation_configuration &conf) {
            if (conf.filepath != nullptr) {
                if constexpr (target == GL_TEXTURE_2D || target == GL_TEXTURE_2D_MULTISAMPLE) {
                    int width, height, num_channels;
                    stbi_set_flip_vertically_on_load(conf.flip_vertically);
                    std::uint8_t *data = stbi_load(conf.filepath, &width, &height, &num_channels, 0);
                    if (data == nullptr) {
                        auto message_str = fmt::format("Failed to open texture - {}", conf.filepath);
                        throw coel::exception(message_str.c_str());
                    }
                    unsigned int format;
                    switch (num_channels) {
                    case 1: format = GL_RED; break;
                    case 3: format = GL_RGB; break;
                    case 4: format = GL_RGBA; break;
                    default: format = GL_RGB; break;
                    }
                    const auto dim_x = static_cast<decltype(dim_t::x)>(width);
                    const auto dim_y = static_cast<decltype(dim_t::y)>(height);

                    recreate(creation_configuration{
                        .filepath = nullptr,
                        .data{
                            .ptr = reinterpret_cast<decltype(data_configuration::ptr)>(data),
                            .dim = {dim_x, dim_y},
                            .format = format,
                            .type = conf.data.type,
                        },
                        .gl_format = conf.gl_format,
                        .wrap = {conf.wrap.s, conf.wrap.t, conf.wrap.r},
                        .filter = {conf.filter.min, conf.filter.max},
                        .border_color = conf.border_color,
                        .use_mipmap = conf.use_mipmap,
                    });
                    stbi_image_free(data);
                }
            } else {
                bind();
                if constexpr (target == GL_TEXTURE_2D_MULTISAMPLE) {
                    glTexImage2DMultisample(target, conf.samples, conf.gl_format, conf.data.dim.x, conf.data.dim.y, false);
                } else if constexpr (target == GL_TEXTURE_2D) {
                    glTexImage2D(target, 0, conf.gl_format, conf.data.dim.x, conf.data.dim.y, 0, conf.data.format, conf.data.type, conf.data.ptr);
                } else if constexpr (target == GL_TEXTURE_3D) {
                    glEnable(GL_TEXTURE_3D);
                    glTexImage3D(target, 0, conf.gl_format, conf.data.dim.x, conf.data.dim.y, conf.data.dim.z, 0, conf.data.format, conf.data.type, conf.data.ptr);
                    glTexParameteri(target, GL_TEXTURE_WRAP_R, conf.wrap.r);
                } else {
                    static_assert("Unsupported Texture Target");
                }
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

        void update(data_configuration conf) {
            bind();
            if constexpr (target == GL_TEXTURE_2D_MULTISAMPLE) {
                static_assert("Multisampled Textures MUST be recreated");
            } else if constexpr (target == GL_TEXTURE_2D) {
                glTexSubImage2D(target, 0, conf.offset.x, conf.offset.y, conf.dim.x, conf.dim.y, conf.format, conf.type, conf.ptr);
                glGenerateMipmap(target);
            } else if constexpr (target == GL_TEXTURE_3D) {
                glTexSubImage3D(target, 0, conf.offset.x, conf.offset.y, conf.offset.z, conf.dim.x, conf.dim.y, conf.dim.z, conf.format, conf.type, conf.ptr);
                glGenerateMipmap(target);
            } else {
                static_assert("Unsupported Texture Target");
            }
        }

        ~texture() {
            glDeleteTextures(1, &id);
        }

        inline void bind() const {
            if constexpr (target == GL_TEXTURE_3D)
                glEnable(GL_TEXTURE_3D);
            glBindTexture(target, id);
        }

        inline void bind(unsigned int index) const {
            glActiveTexture(GL_TEXTURE0 + index);
            bind();
        }

        static inline void unbind() {
            glBindTexture(target, 0);
        }
    };

    template <typename data_t = std::uint8_t>
    using texture2d = texture<GL_TEXTURE_2D, data_t, glm::uvec2>;
    template <typename data_t = std::uint8_t>
    using texture2d_multisample = texture<GL_TEXTURE_2D_MULTISAMPLE, data_t, glm::uvec2>;

    template <typename data_t = std::uint8_t>
    using texture3d = texture<GL_TEXTURE_3D, data_t, glm::uvec3>;
} // namespace opengl
