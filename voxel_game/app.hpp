#pragma once

#include <fmt/core.h>

#include <glm/glm.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <coel/opengl/core.hpp>

namespace coel {
    class glfw_app {
      public:
        GLFWwindow *window = nullptr;

        glfw_app(glm::ivec2 frame_dim) {
            glfwInit();
            window = glfwCreateWindow(frame_dim.x, frame_dim.y, "bruh", nullptr, nullptr);

            if (window == nullptr) {
                std::cout << "failed to create glfw window\n";
                return;
            }

            glfwMakeContextCurrent(window);
            glfwSwapInterval(1);

            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                std::cout << "failed to load opengl\n";
                glfwDestroyWindow(window);
                window = nullptr;
                return;
            }
        }
        ~glfw_app() {
            glfwDestroyWindow(window);
            glfwTerminate();
        }
    };
} // namespace coel

#include <coel/graphics/ui.hpp>

#include <voxel_game/test_scene.hpp>

#include <vector>

class game_app {
  public:
    glm::uvec2 frame_dim = {1200, 900};
    glm::vec2 mouse_pos = {};
    double now;
    bool is_active : 1 = true, is_paused : 1 = false;
    coel::glfw_app glfw;

    // clang-format off
    static inline constexpr std::array quad_vertices = {
        -1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
         1.0f, -1.0f,
    };
    // clang-format on

    test_scene scene;

    float render_resolution_scale;
    glm::uvec2 scene_frame_dim = frame_dim;
    opengl::vertex_array scene_frame_vao;
    opengl::vertex_buffer scene_frame_quad_vbo;
    opengl::framebuffer scene_frame;
    opengl::shader_program scene_frame_shader;
    opengl::shader_uniform
        u_scene_frame_gamma,
        u_scene_frame_exposure,
        u_scene_frame_tex;
    opengl::texture2d<> scene_frame_tex;
    opengl::renderbuffer scene_frame_depth_rbo;

    graphics::menu_ui menu;

    game_app()
        : glfw(frame_dim),
          scene_frame_quad_vbo(quad_vertices.data(), quad_vertices.size() * sizeof(quad_vertices[0])),
          scene_frame_shader("game/assets/shaders/scene_frame_vert.glsl", "game/assets/shaders/scene_frame_frag.glsl") {
        glfwSetWindowUserPointer(glfw.window, this);
        now = glfwGetTime();
        init();
    }
    void toggle_pause() {
        is_paused = !is_paused;
        if (is_paused) {
            glfwSetInputMode(glfw.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glfwSetInputMode(glfw.window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
            glfwSwapInterval(1);
        } else {
            glfwSetInputMode(glfw.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetInputMode(glfw.window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
            glfwSwapInterval(0);
        }
        mouse_pos = glm::vec2(frame_dim) * 0.5f;
        glfwSetCursorPos(glfw.window, mouse_pos.x, mouse_pos.y);
    }
    bool update() {
        glfwPollEvents();
        if (is_active) {
            double new_now = glfwGetTime();
            on_update(new_now - now);
            now = new_now;
            draw();
        } else
            std::this_thread::sleep_for(100ms);
        return !glfwWindowShouldClose(glfw.window);
    }
    void resize() {
        menu.resize(frame_dim);
        menu.buttons[1].pos = {20, frame_dim.y - 40 - 20};
        resize_frame();
        scene.resize(scene_frame_dim);
    }
    void resize_frame() {
        scene_frame_dim = glm::vec2(frame_dim) * render_resolution_scale;
        scene_frame_tex.regenerate({
            .dim = {scene_frame_dim.x, scene_frame_dim.y},
            .gl_format = GL_RGBA16F,
            .data_format = GL_RGBA,
            .wrap = {.s = GL_CLAMP_TO_EDGE, .t = GL_CLAMP_TO_EDGE},
            .filter = {.min = GL_LINEAR, .max = GL_LINEAR},
            .samples = 4,
            .use_mipmap = false,
        });
        scene_frame_depth_rbo.regenerate({
            .width = scene_frame_dim.x,
            .height = scene_frame_dim.y,
            .gl_format = GL_DEPTH24_STENCIL8,
        });
        scene_frame.verify();
    }
    void init() {
        set_callbacks();
        glfwSetInputMode(glfw.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetInputMode(glfw.window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        glfwSetWindowSizeLimits(glfw.window, 400, 360, GLFW_DONT_CARE, GLFW_DONT_CARE);
        scene.init();
        scene_frame_vao.bind();
        scene_frame_quad_vbo.bind();
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, reinterpret_cast<const void *>(0));
        u_scene_frame_gamma = scene_frame_shader.find_uniform("u_scene_frame_gamma");
        u_scene_frame_exposure = scene_frame_shader.find_uniform("u_scene_frame_exposure");
        u_scene_frame_tex = scene_frame_shader.find_uniform("u_scene_frame_tex");
        glUniform1i(u_scene_frame_tex.location, 0);
        scene_frame.bind();
        scene_frame.attach(scene_frame_tex, GL_COLOR_ATTACHMENT0);
        scene_frame.attach(scene_frame_depth_rbo, GL_DEPTH_STENCIL_ATTACHMENT);
        unsigned int attachment = GL_COLOR_ATTACHMENT0;
        glDrawBuffers(1, &attachment);
        scene_frame.unbind();
        add_ui();
    }
    void set_callbacks() {
        glfwSetWindowSizeCallback(glfw.window, [](GLFWwindow *glfw_window, int size_x, int size_y) -> void {
            auto &game = *static_cast<game_app *>(glfwGetWindowUserPointer(glfw_window));
            game.frame_dim = {size_x, size_y};
            game.resize();
            game.draw();
        });
        glfwSetCursorPosCallback(glfw.window, [](GLFWwindow *glfw_window, double mouse_x, double mouse_y) -> void {
            auto &game = *static_cast<game_app *>(glfwGetWindowUserPointer(glfw_window));
            game.mouse_pos = {(float)mouse_x, (float)mouse_y};

            if (game.is_paused) {
                game.menu.mouse_move(game.mouse_pos);
            } else {
                const auto screen_center = glm::vec2(game.frame_dim) * 0.5f;
                game.scene.player.move_mouse(game.mouse_pos - screen_center);

                game.mouse_pos = screen_center;
                glfwSetCursorPos(glfw_window, game.mouse_pos.x, game.mouse_pos.y);
            }
        });
        glfwSetMouseButtonCallback(glfw.window, [](GLFWwindow *glfw_window, int button, int action, int mods) -> void {
            auto &game = *static_cast<game_app *>(glfwGetWindowUserPointer(glfw_window));

            if (game.is_paused) {
                game.menu.mouse_press(button, action, mods, game.mouse_pos);
            }
        });
        glfwSetKeyCallback(glfw.window, [](GLFWwindow *glfw_window, int button, int scancode, int action, int mods) -> void {
            auto &game = *static_cast<game_app *>(glfwGetWindowUserPointer(glfw_window));
            if (action == GLFW_PRESS) {
                switch (button) {
                case GLFW_KEY_ESCAPE:
                    if (game.is_paused) {
                        if (game.menu.esc())
                            game.toggle_pause();
                    } else {
                        game.toggle_pause();
                    }
                    break;
                case GLFW_KEY_R:
                    system("CLS");
                    game.scene.shader = opengl::shader_program("game/assets/shaders/scene_vert.glsl", "game/assets/shaders/scene_frag.glsl");
                    for (auto &s : game.menu.sliders)
                        s.on_change(&game, s.range.convert(s.value));
                    break;
                default:
                    break;
                }
            }
            if (game.is_paused) {
                game.menu.process_key(button, scancode, action, mods);
            } else {
                game.scene.player.key_press(button, scancode, action, mods);
            }
        });
        glfwSetWindowIconifyCallback(glfw.window, [](GLFWwindow *glfw_window, int mode) -> void {
            auto &game = *static_cast<game_app *>(glfwGetWindowUserPointer(glfw_window));
            if (mode == GLFW_TRUE)
                game.is_active = false;
            else
                game.is_active = true;
        });
        glfwSetCharCallback(glfw.window, [](GLFWwindow *glfw_window, unsigned int codepoint) -> void {
            auto &game = *static_cast<game_app *>(glfwGetWindowUserPointer(glfw_window));
            if (game.is_paused)
                game.menu.process_char(codepoint);
        });
    }
    void add_ui() {
        menu.buttons.push_back(graphics::button{
            .pos = {20, 20},
            .size = {130, 40},
            .color_focused = {0.5, 0.55, 0.9, 1},
            .color_unfocused = {0.4, 0.47, 0.8, 1},
            .label = {
                .text = "Resume",
                .color_focused = {0.1, 0.15, 0.3, 1},
                .color_unfocused = {0.12, 0.18, 0.3, 1},
                .offset = {10, 2},
                .scale = 28,
            },
            .user_ptr = this,
            .on_release = [](void *user_ptr) -> void {
                auto *game_ptr = reinterpret_cast<game_app *>(user_ptr);
                if (game_ptr) {
                    auto &game = *game_ptr;
                    game.toggle_pause();
                }
            },
        });
        menu.buttons.push_back(graphics::button{
            .pos = {20, 0},
            .size = {100, 40},
            .color_focused = {0.9, 0.5, 0.55, 1},
            .color_unfocused = {0.8, 0.4, 0.47, 1},
            .label = {
                .text = "Exit",
                .color_focused = {0, 0, 0, 1},
                .color_unfocused = {0.2, 0.1, 0.1, 1},
                .offset = {21, 2},
                .scale = 28,
            },
            .user_ptr = this,
            .on_release = [](void *user_ptr) -> void {
                auto *game_ptr = reinterpret_cast<game_app *>(user_ptr);
                if (game_ptr) {
                    auto &game = *game_ptr;
                    glfwSetWindowShouldClose(game.glfw.window, true);
                }
            },
        });
        menu.sliders.push_back(graphics::slider{
            .pos = {20, 70},
            .size = {160, 12},
            .range = {.min = 45, .max = 135},
            .label = {
                .text = "FOV",
                .color_focused = {0.9, 0.9, 0.9, 1},
                .color_unfocused = {0.9, 0.9, 0.9, 0.7},
                .offset = {170, -4},
                .scale = 12,
            },
            .user_ptr = this,
            .on_change = [](void *user_ptr, float value) -> void {
                auto *game_ptr = reinterpret_cast<game_app *>(user_ptr);
                if (game_ptr) {
                    auto &game = *game_ptr;
                    game.scene.player.cam.fov = glm::radians(value);
                    game.scene.player.cam.update_proj();
                    game.scene.shader.bind();
                    glUniformMatrix4fv(game.scene.u_proj_mat.location, 1, false, reinterpret_cast<float *>(&game.scene.player.cam.proj_mat));
                }
            },
        });

        menu.sliders.push_back(graphics::slider{
            .pos = {20, 180},
            .size = {160, 10},
            .range = {.min = 0, .max = 5},
            .value = 0.42,
            .label = {
                .text = "Camera Gamma",
                .color_focused = {0.9, 0.9, 0.9, 1},
                .color_unfocused = {0.9, 0.9, 0.9, 0.7},
                .offset = {170, -4},
                .scale = 12,
            },

            .user_ptr = this,
            .on_change = [](void *user_ptr, float value) -> void {
                auto *game_ptr = reinterpret_cast<game_app *>(user_ptr);
                if (game_ptr) {
                    auto &game = *game_ptr;

                    game.scene_frame_shader.bind();
                    glUniform1f(game.u_scene_frame_gamma.location, value);
                }
            },
        });
        menu.sliders.push_back(graphics::slider{
            .pos = {20, 192},
            .size = {160, 10},
            .range = {.min = 0, .max = 5},
            .value = 0.02,
            .label = {
                .text = "Camera Exposure",
                .color_focused = {0.9, 0.9, 0.9, 1},
                .color_unfocused = {0.9, 0.9, 0.9, 0.7},
                .offset = {170, -4},
                .scale = 12,
            },

            .user_ptr = this,
            .on_change = [](void *user_ptr, float value) -> void {
                auto *game_ptr = reinterpret_cast<game_app *>(user_ptr);
                if (game_ptr) {
                    auto &game = *game_ptr;

                    game.scene_frame_shader.bind();
                    glUniform1f(game.u_scene_frame_exposure.location, value);
                }
            },
        });
        menu.sliders.push_back(graphics::slider{
            .pos = {20, 210},
            .size = {160, 10},
            .range = {.min = 0.1, .max = 2},
            .value = 1.0f / 1.9,
            .label = {
                .text = "Render Res. Factor",
                .color_focused = {0.9, 0.9, 0.9, 1},
                .color_unfocused = {0.9, 0.9, 0.9, 0.7},
                .offset = {170, -4},
                .scale = 12,
            },

            .user_ptr = this,
            .on_change = [](void *user_ptr, float value) -> void {
                auto *game_ptr = reinterpret_cast<game_app *>(user_ptr);
                if (game_ptr) {
                    auto &game = *game_ptr;
                    game.render_resolution_scale = value * value;
                    game.resize_frame();
                }
            },
        });
        menu.textboxes.push_back(graphics::textbox{
            .pos = {20, 222},
            .size = {166, 68},
        });
        for (auto &s : menu.sliders)
            s.on_change(this, s.range.convert(s.value));
    }
    void on_update(double elapsed) {
        scene.on_update(elapsed);
    }
    void draw() {
        scene_frame.bind();
        glViewport(0, 0, scene_frame_dim.x, scene_frame_dim.y);
        scene.draw();
        opengl::framebuffer::unbind();
        glViewport(0, 0, frame_dim.x, frame_dim.y);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glEnable(GL_MULTISAMPLE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        scene_frame_shader.bind();
        scene_frame_tex.bind(0);
        glUniform1i(u_scene_frame_tex.location, 0);
        scene_frame_vao.bind();
        glDrawArrays(GL_QUADS, 0, 4);
        if (is_paused)
            menu.draw_settings();
        glfwSwapBuffers(glfw.window);
    }
};
