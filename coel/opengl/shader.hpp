#pragma once

#include <glad/glad.h>

#include <fstream>
#include <iostream>

namespace opengl {
    struct shader_config {
        const char *const filepath = nullptr;
        const char *const source_str = nullptr;
    };

    template <GLenum type>
    struct shader {
      private:
        void create_from_source(const char *const source_str) {
            int success;
            char info_log[512];
            glShaderSource(id, 1, &source_str, nullptr);
            glCompileShader(id);
            glGetShaderiv(id, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(id, sizeof info_log, nullptr, info_log);
                std::cout << "Error: shader compilation failed\n"
                          << info_log << "\n";
            }
        }

      public:
        unsigned int id;
        shader() {
            id = glCreateShader(type);
        }
        shader(const shader_config &conf) : shader() {
            if (conf.filepath != nullptr) {
                std::ifstream source_file(conf.filepath);
                if (source_file.is_open()) {
                    std::string source_str;
                    source_file.seekg(0, std::ios::end);
                    source_str.reserve(source_file.tellg());
                    source_file.seekg(0, std::ios::beg);
                    source_str.assign(
                        std::istreambuf_iterator<char>(source_file),
                        std::istreambuf_iterator<char>());
                    create_from_source(source_str.c_str());
                    source_file.close();
                } else {
                    std::cout << "Error: Failed to open shader file\n"
                              << "  - " << conf.filepath << "\n";
                }
            } else if (conf.source_str != nullptr) {
                create_from_source(conf.source_str);
            }
        }
        shader(const shader &) = delete;
        shader(shader &&other) {
            this->~shader();
            id = other.id;
            other.id = -1;
        }
        shader &operator=(const shader &) = delete;
        shader &operator=(shader &&other) {
            this->~shader();
            id = other.id;
            other.id = -1;
            return *this;
        }
        ~shader() {
            glDeleteShader(id);
        }
    };

    struct shader_uniform {
        int location;
    };

    struct shader_program {
        unsigned int id;

        shader_program() {
            id = glCreateProgram();
        }
        shader_program(const shader_config &vert_shader_conf, const shader_config &frag_shader_conf) : shader_program() {
            shader<GL_VERTEX_SHADER> vert_shader(vert_shader_conf);
            shader<GL_FRAGMENT_SHADER> frag_shader(frag_shader_conf);

            int success;
            char info_log[512];
            glAttachShader(id, vert_shader.id);
            glAttachShader(id, frag_shader.id);
            glLinkProgram(id);
            glGetProgramiv(id, GL_LINK_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(id, sizeof info_log, nullptr, info_log);
                std::cout << "Error: shader linkage failed\n"
                          << info_log << "\n";
            }

            bind();
        }
        shader_program(const shader_program &) = delete;
        shader_program(shader_program &&other) {
            this->~shader_program();
            id = other.id;
            other.id = (unsigned int)-1;
        }
        shader_program &operator=(const shader_program &) = delete;
        shader_program &operator=(shader_program &&other) {
            this->~shader_program();
            id = other.id;
            other.id = (unsigned int)-1;
            return *this;
        }
        ~shader_program() {
            if (id != (unsigned int)-1)
                glDeleteProgram(id);
        }

        inline void bind() const {
            glUseProgram(id);
        }

        static inline void unbind() {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        inline shader_uniform find_uniform(const char *const name) const {
            bind();
            return {.location = glGetUniformLocation(id, name)};
        }
    };
} // namespace opengl
