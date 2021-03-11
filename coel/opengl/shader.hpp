#pragma once

#include <glad/glad.h>

#include <fstream>
#include <iostream>

#include <string>
#include <iomanip>
#include <regex>
#include <sstream>
#include <string_view>
#include <filesystem>

namespace opengl {
    namespace detail {
        enum class shader_type {
            none,
            vert,
            frag,
        };

        constexpr std::string_view
            SHADER_TYPE_VERT_IDENT = "vert",
            SHADER_TYPE_FRAG_IDENT = "frag";

        struct shader_info {
            std::string name{}, source_filepath{};
            std::stringstream source_stream{};
            shader_type type = shader_type::none;
            std::size_t line_num = 0;
        };

        std::vector<shader_info> load_shader(const std::filesystem::path &filepath) {
            std::ifstream shader_file(filepath);
            std::vector<shader_info> result_vec{};
            if (shader_file.is_open()) {
                const std::regex shader_regex(R"reg(#\s*coel_shader\s*\[\s*type\s*:\s*(\w+)\s*,\s*name\s*:\s*"(.*)"\s*\]\s*)reg");
                std::smatch matches;
                std::string line;
                for (std::size_t line_num = 0; getline(shader_file, line); ++line_num) {
                    if (std::regex_match(line, matches, shader_regex)) {
                        result_vec.push_back({});
                        result_vec.back().name = matches[2];
                        result_vec.back().line_num = line_num + 2;
                        if (matches[1].str() == SHADER_TYPE_VERT_IDENT)
                            result_vec.back().type = shader_type::vert;
                        else if (matches[1].str() == SHADER_TYPE_FRAG_IDENT)
                            result_vec.back().type = shader_type::frag;
                    } else {
                        if (result_vec.size() > 0)
                            result_vec.back().source_stream << line << '\n';
                    }
                }
            } else {
                std::cout << "failed to open file\n";
            }
            return result_vec;
        }
    } // namespace detail

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
        shader_program(const std::filesystem::path &custom_shader_filepath) : shader_program() {
            auto shader_infos = detail::load_shader(custom_shader_filepath);

            shader<GL_VERTEX_SHADER> vert_shader({
                .source_str =
                    std::find_if(
                        shader_infos.begin(), shader_infos.end(),
                        [](const detail::shader_info &s) { return s.type == detail::shader_type::vert; })
                        ->source_stream.str()
                        .c_str(),
            });
            shader<GL_FRAGMENT_SHADER> frag_shader({
                .source_str =
                    std::find_if(
                        shader_infos.begin(), shader_infos.end(),
                        [](const detail::shader_info &s) { return s.type == detail::shader_type::frag; })
                        ->source_stream.str()
                        .c_str(),
            });

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
            return {.location = glGetUniformLocation(id, name)};
        }

        static void send(const shader_uniform &uniform, int value) { glUniform1i(uniform.location, value); }
        static void send(const shader_uniform &uniform, float value) { glUniform1f(uniform.location, value); }
        static void send(const shader_uniform &uniform, const glm::vec2 &v) {
            glUniform2fv(uniform.location, 1, reinterpret_cast<const float *>(&v));
        }
        static void send(const shader_uniform &uniform, const glm::vec3 &v) {
            glUniform3fv(uniform.location, 1, reinterpret_cast<const float *>(&v));
        }
        static void send(const shader_uniform &uniform, const glm::vec4 &v) {
            glUniform4fv(uniform.location, 1, reinterpret_cast<const float *>(&v));
        }
        static void send(const shader_uniform &uniform, const glm::mat4 &m) {
            glUniformMatrix4fv(uniform.location, 1, false, reinterpret_cast<const float *>(&m));
        }
        static void send_array(const shader_uniform &uniform, const glm::vec2 *const v, const unsigned int count) {
            glUniform2fv(uniform.location, count, reinterpret_cast<const float *>(v));
        }
        static void send_array(const shader_uniform &uniform, const glm::vec3 *const v, const unsigned int count) {
            glUniform3fv(uniform.location, count, reinterpret_cast<const float *>(v));
        }
        static void send_array(const shader_uniform &uniform, const glm::vec4 *const v, const unsigned int count) {
            glUniform4fv(uniform.location, count, reinterpret_cast<const float *>(v));
        }
        static void send_array(const shader_uniform &uniform, const glm::mat4 *const m, const unsigned int count) {
            glUniformMatrix4fv(uniform.location, count, false, reinterpret_cast<const float *>(m));
        }
    };
} // namespace opengl
