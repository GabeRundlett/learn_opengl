#pragma once

#include <glad/glad.h>

#include <fstream>
#include <iostream>

namespace opengl {
    template <GLenum type>
    struct shader {
        unsigned int id;
        shader() {
            id = glCreateShader(type);
        }
        shader(const char *const filepath) : shader() {
            std::ifstream source_file(filepath);
            if (source_file.is_open()) {
                std::string source_str;
                source_file.seekg(0, std::ios::end);
                source_str.reserve(source_file.tellg());
                source_file.seekg(0, std::ios::beg);
                source_str.assign(
                    std::istreambuf_iterator<char>(source_file),
                    std::istreambuf_iterator<char>());
                const char *source_cstr = source_str.c_str();
                int success;
                char info_log[512];
                glShaderSource(id, 1, &source_cstr, nullptr);
                glCompileShader(id);
                glGetShaderiv(id, GL_COMPILE_STATUS, &success);
                if (!success) {
                    glGetShaderInfoLog(id, sizeof info_log, nullptr, info_log);
                    std::cout << "Error: shader compilation failed\n"
                              << info_log << "\n";
                }
                source_file.close();
            } else {
                std::cout << "Error: Failed to open shader file\n"
                          << "  - " << filepath << "\n";
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
        shader_program(const char *const vert_shader_source, const char *const frag_shader_source) : shader_program() {
            shader<GL_VERTEX_SHADER> vert_shader(vert_shader_source);
            shader<GL_FRAGMENT_SHADER> frag_shader(frag_shader_source);

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
            other.id = -1;
        }
        shader_program &operator=(const shader_program &) = delete;
        shader_program &operator=(shader_program &&other) {
            this->~shader_program();
            id = other.id;
            other.id = -1;
            return *this;
        }
        ~shader_program() {
            if (id != -1)
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
