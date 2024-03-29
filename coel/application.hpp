#pragma once

#include <coel/input.hpp>
#include <coel/opengl/core.hpp>
#include <coel/opengl/renderers/ui_batch.hpp>
#include <coel/opengl/renderers/text_batch.hpp>
#include <coel/exception.hpp>

#include <fmt/core.h>
#include <thread>

namespace coel {
    using clock = std::chrono::steady_clock;
    using duration = std::chrono::duration<long double>;

    class application {
        class glfw_app {
          public:
            GLFWwindow *window_ptr = nullptr;
            glfw_app(const glm::uvec2 frame_dim, const std::string &title) {
                glfwInit();
                window_ptr = glfwCreateWindow(frame_dim.x, frame_dim.y, title.c_str(), nullptr, nullptr);

                if (window_ptr == nullptr)
                    throw coel::exception("Failed to create GLFW window");

                glfwMakeContextCurrent(window_ptr);

                if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
                    throw coel::exception("Failed to create GLFW window");
            }
            ~glfw_app() {
                glfwDestroyWindow(window_ptr);
                glfwTerminate();
            }
        };

        inline void set_callbacks() {
            glfwSetWindowSizeCallback(glfw.window_ptr, [](GLFWwindow *glfw_window_ptr, int size_x, int size_y) -> void {
                auto app_ptr = static_cast<application *>(glfwGetWindowUserPointer(glfw_window_ptr));
                if (app_ptr) {
                    app_ptr->frame_dim = {size_x, size_y};
                    app_ptr->resize();
                    app_ptr->on_event();
                }
            });

            glfwSetCursorPosCallback(glfw.window_ptr, [](GLFWwindow *glfw_window_ptr, double mouse_x, double mouse_y) -> void {
                auto app_ptr = static_cast<application *>(glfwGetWindowUserPointer(glfw_window_ptr));
                if (app_ptr) {
                    app_ptr->input.mouse.cursor_offset = glm::vec2(mouse_x, mouse_y) - app_ptr->input.mouse.cursor_pos;
                    app_ptr->input.mouse.cursor_pos = glm::vec2(mouse_x, mouse_y);
                    app_ptr->on_mouse_move();
                    app_ptr->on_event();
                    app_ptr->on_event();
                }
            });

            glfwSetScrollCallback(glfw.window_ptr, [](GLFWwindow *glfw_window_ptr, double offset_x, double offset_y) -> void {
                auto app_ptr = static_cast<application *>(glfwGetWindowUserPointer(glfw_window_ptr));
                if (app_ptr) {
                    app_ptr->on_mouse_scroll(glm::dvec2{offset_x, offset_y});
                    app_ptr->on_event();
                }
            });

            glfwSetMouseButtonCallback(glfw.window_ptr, [](GLFWwindow *glfw_window_ptr, int button, int action, int mods) -> void {
                auto app_ptr = static_cast<application *>(glfwGetWindowUserPointer(glfw_window_ptr));
                if (app_ptr) {
                    app_ptr->on_mouse_button({button, action, mods});
                    app_ptr->input.mouse.buttons[button] = action != GLFW_RELEASE;
                    app_ptr->input.mouse.active = {
                        .button = button,
                        .mods = mods,
                        .action = action,
                    };
                    app_ptr->on_event();
                    app_ptr->input.mouse.active = {};
                }
            });

            glfwSetKeyCallback(glfw.window_ptr, [](GLFWwindow *glfw_window_ptr, int key, int scancode, int action, int mods) -> void {
                auto app_ptr = static_cast<application *>(glfwGetWindowUserPointer(glfw_window_ptr));
                if (app_ptr) {
                    switch (action) {
                    case GLFW_PRESS:
                        switch (key) {
                        case GLFW_KEY_ESCAPE:
                            app_ptr->toggle_pause();
                            break;
                        case GLFW_KEY_F3:
                            app_ptr->show_debug_menu = !app_ptr->show_debug_menu;
                            break;
                        }
                    }
                    app_ptr->on_key({key, scancode, action, mods});
                    app_ptr->input.keyboard.keys[key] = action != GLFW_RELEASE;
                    app_ptr->input.keyboard.active = {
                        .key = key,
                        .mods = mods,
                        .action = action,
                    };
                    app_ptr->on_event();
                    app_ptr->input.keyboard.active = {};
                }
                    
            });

            glfwSetCharCallback(glfw.window_ptr, [](GLFWwindow *glfw_window_ptr, unsigned int codepoint) -> void {
                auto app_ptr = static_cast<application *>(glfwGetWindowUserPointer(glfw_window_ptr));
                if (app_ptr) {
                    app_ptr->on_char(codepoint);
                    app_ptr->on_event();
                }
            });

            glfwSetWindowIconifyCallback(glfw.window_ptr, [](GLFWwindow *glfw_window_ptr, int mode) -> void {
                auto app_ptr = static_cast<application *>(glfwGetWindowUserPointer(glfw_window_ptr));
                if (app_ptr) {
                    app_ptr->is_active = mode;
                    app_ptr->on_event();
                }
            });
        }

      protected:
        virtual void on_mouse_move() {}
        virtual void on_mouse_scroll(const glm::dvec2) {}
        virtual void on_mouse_button(const mouse_button_event &) {}
        virtual void on_key(const key_event &) {}
        virtual void on_char(unsigned int) {}

        virtual void on_draw() {}
        virtual void on_resize() {}
        virtual void on_update(duration) {}
        virtual void on_pause() {}
        virtual void on_resume() {}

        virtual void on_event() {}

      public:
        glm::uvec2 frame_dim = {800, 600};
        std::string title = "coel application";

        glfw_app glfw = glfw_app(frame_dim, title);

        clock::time_point now, start_time;
        bool is_active : 1 = true, is_paused : 1 = true, show_debug_menu : 1 = false;
        input_state input;
        // glm::vec2 mouse_pos{0, 0}, mouse_pos_diff{0, 0};

        opengl::renderer::ui_batch ui_batch;
        opengl::renderer::text_batch text_batch = opengl::renderer::text_batch("coel/assets/RobotoFontAtlas.png");

        // debug info
        clock::time_point debug_frame_begin;
        unsigned int debug_frames_passed = 0, debug_prev_frames_passed = 1;
        glm::vec2 debug_text_pos = {16, 16};

        void toggle_pause() {
            is_paused = !is_paused;
            is_paused ? on_pause() : on_resume();
        }

      public:
        application(glm::uvec2 frame_dim, const char *const title_str) : frame_dim(frame_dim), title(title_str) {
            glfwSetWindowUserPointer(glfw.window_ptr, this);
            use_vsync(true);
            set_callbacks();
            now = clock::now();
            debug_frame_begin = now;
            start_time = now;
        }

        virtual ~application() {}

        operator bool() {
            return glfw.window_ptr != nullptr;
        }

        bool update() {
            glfwPollEvents();
            using namespace std::chrono_literals;
            if (is_active) {
                auto new_now = clock::now();
                on_update(duration(new_now - now));
                now = new_now;
                // debug info
                ++debug_frames_passed;
                if (now - debug_frame_begin > 1s) {
                    debug_prev_frames_passed = debug_frames_passed;
                    debug_frames_passed = 0;
                    debug_frame_begin = now;
                }
                redraw();
            } else
                std::this_thread::sleep_for(100ms);
            return !glfwWindowShouldClose(glfw.window_ptr);
        }
        void redraw() {
            glViewport(0, 0, frame_dim.x, frame_dim.y);
            ui_batch.begin();
            text_batch.begin();

            on_draw();
            if (show_debug_menu) {
                constexpr glm::vec4 text_col = {0.9f, 0.9f, 0.9f, 1.0f};

                opengl::renderer::component_bounds bounds;
                glm::vec2 border = {8, 8};

                text_batch.submit(
                    debug_text_pos,
                    fmt::format(
                        "Frames/s: {}\nWindow Size: ({}, {})",
                        debug_prev_frames_passed,
                        frame_dim.x, frame_dim.y),
                    14.0f, text_col, &bounds);

                ui_batch.submit_rect(bounds.min - border, bounds.max + border, {0.0f, 0.0f, 0.0f, 0.5f});
            }

            ui_batch.end();
            text_batch.end();
            ui_batch.flush();
            text_batch.flush();

            glfwSwapBuffers(glfw.window_ptr);
        }
        void resize() {
            ui_batch.resize(frame_dim);
            text_batch.resize(frame_dim);

            on_resize();
            redraw();
        }
        inline void use_vsync(bool should_use_vsync) const {
            glfwSwapInterval(should_use_vsync);
        }
        inline void use_raw_mouse(bool should_use_raw) const {
            glfwSetInputMode(glfw.window_ptr, GLFW_RAW_MOUSE_MOTION, should_use_raw ? GLFW_TRUE : GLFW_FALSE);
        }
        inline void set_mouse_capture(bool should_capture) {
            glfwSetCursorPos(glfw.window_ptr, frame_dim.x / 2, frame_dim.y / 2);
            input.mouse.cursor_pos = glm::vec2(frame_dim.x / 2, frame_dim.y / 2);
            glfwSetInputMode(glfw.window_ptr, GLFW_CURSOR, should_capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        }
        inline void set_mouse_pos(const glm::vec2 p) {
            input.mouse.cursor_pos = p;
            glfwSetCursorPos(glfw.window_ptr, p.x, p.y);
        }
    };
} // namespace coel
