#pragma once

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>

namespace opengl {
    template <GLenum target, typename data_t, typename dim_t>
    struct texture {
        unsigned int id = (unsigned int)-1;

        struct configuration {
            const char *filepath = nullptr;
            const data_t *data = nullptr;
            dim_t dim;
            // unsigned int width, height;
            unsigned int gl_format, data_format, data_type = GL_UNSIGNED_BYTE;
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
                if constexpr (target == GL_TEXTURE_2D || target == GL_TEXTURE_2D_MULTISAMPLE) {
                    int width, height, num_channels;
                    stbi_set_flip_vertically_on_load(conf.flip_vertically);
                    std::uint8_t *data = stbi_load(conf.filepath, &width, &height, &num_channels, 0);
                    if (data == nullptr) {
                        std::cout << "Failed to open texture\n"
                                  << " - " << conf.filepath << "\n";
                        return;
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

                    regenerate(configuration{
                        .filepath = nullptr,
                        .data = reinterpret_cast<decltype(configuration::data)>(data),
                        .dim = {dim_x, dim_y},
                        .gl_format = conf.gl_format,
                        .data_format = format,
                        .data_type = conf.data_type,
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
                    glTexImage2DMultisample(target, conf.samples, conf.gl_format, conf.dim.x, conf.dim.y, false);
                    glTexParameteri(target, GL_TEXTURE_WRAP_S, conf.wrap.s);
                    glTexParameteri(target, GL_TEXTURE_WRAP_T, conf.wrap.t);
                    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, conf.filter.min);
                    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, conf.filter.max);
                    if (conf.use_mipmap)
                        glGenerateMipmap(target);
                    if (conf.wrap.s == GL_CLAMP_TO_BORDER || conf.wrap.t == GL_CLAMP_TO_BORDER)
                        glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, reinterpret_cast<const float *>(&conf.border_color));
                } else if constexpr (target == GL_TEXTURE_2D) {
                    glTexImage2D(target, 0, conf.gl_format, conf.dim.x, conf.dim.y, 0, conf.data_format, conf.data_type, conf.data);
                    glTexParameteri(target, GL_TEXTURE_WRAP_S, conf.wrap.s);
                    glTexParameteri(target, GL_TEXTURE_WRAP_T, conf.wrap.t);
                    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, conf.filter.min);
                    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, conf.filter.max);
                    if (conf.use_mipmap)
                        glGenerateMipmap(target);
                    if (conf.wrap.s == GL_CLAMP_TO_BORDER || conf.wrap.t == GL_CLAMP_TO_BORDER)
                        glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, reinterpret_cast<const float *>(&conf.border_color));
                } else if constexpr (target == GL_TEXTURE_3D) {
                    glEnable(GL_TEXTURE_3D);
                    glTexImage3D(target, 0, conf.gl_format, conf.dim.x, conf.dim.y, conf.dim.z, 0, conf.data_format, conf.data_type, conf.data);
                    glTexParameteri(target, GL_TEXTURE_WRAP_S, conf.wrap.s);
                    glTexParameteri(target, GL_TEXTURE_WRAP_T, conf.wrap.t);
                    glTexParameteri(target, GL_TEXTURE_WRAP_R, conf.wrap.r);
                    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, conf.filter.min);
                    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, conf.filter.max);
                    if (conf.use_mipmap)
                        glGenerateMipmap(target);
                    if (conf.wrap.s == GL_CLAMP_TO_BORDER || conf.wrap.t == GL_CLAMP_TO_BORDER || conf.wrap.r == GL_CLAMP_TO_BORDER)
                        glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, reinterpret_cast<const float *>(&conf.border_color));
                }
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
