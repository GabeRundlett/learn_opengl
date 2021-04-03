#include <coel/application.hpp>
#include <coel/game/player.hpp>
#include <coel/opengl/renderers/quad.hpp>

class my_app : public coel::application {
    opengl::texture2d<> player_frame_tex, second_frame_tex;
    opengl::framebuffer player_frame, second_frame;
    opengl::renderbuffer player_rbo, second_rbo;
    opengl::renderer::quad quad;

    coel::player3d player;
    coel::player3d second;

    struct vertex {
        glm::vec3 pos, nrm;
        glm::vec2 tex;
    };
    static inline const std::array cube_vertices = {
        // clang-format off
        vertex{.pos{-0.5f, -0.5f, -0.5f}, .nrm{ 0.0f,  0.0f, -1.0f}, .tex{0.0f, 0.0f}},
        vertex{.pos{ 0.5f,  0.5f, -0.5f}, .nrm{ 0.0f,  0.0f, -1.0f}, .tex{1.0f, 1.0f}},
        vertex{.pos{ 0.5f, -0.5f, -0.5f}, .nrm{ 0.0f,  0.0f, -1.0f}, .tex{1.0f, 0.0f}},
        vertex{.pos{ 0.5f,  0.5f, -0.5f}, .nrm{ 0.0f,  0.0f, -1.0f}, .tex{1.0f, 1.0f}},
        vertex{.pos{-0.5f, -0.5f, -0.5f}, .nrm{ 0.0f,  0.0f, -1.0f}, .tex{0.0f, 0.0f}},
        vertex{.pos{-0.5f,  0.5f, -0.5f}, .nrm{ 0.0f,  0.0f, -1.0f}, .tex{0.0f, 1.0f}},

        vertex{.pos{-0.5f, -0.5f,  0.5f}, .nrm{ 0.0f,  0.0f,  1.0f}, .tex{0.0f, 0.0f}},
        vertex{.pos{ 0.5f, -0.5f,  0.5f}, .nrm{ 0.0f,  0.0f,  1.0f}, .tex{1.0f, 0.0f}},
        vertex{.pos{ 0.5f,  0.5f,  0.5f}, .nrm{ 0.0f,  0.0f,  1.0f}, .tex{1.0f, 1.0f}},
        vertex{.pos{ 0.5f,  0.5f,  0.5f}, .nrm{ 0.0f,  0.0f,  1.0f}, .tex{1.0f, 1.0f}},
        vertex{.pos{-0.5f,  0.5f,  0.5f}, .nrm{ 0.0f,  0.0f,  1.0f}, .tex{0.0f, 1.0f}},
        vertex{.pos{-0.5f, -0.5f,  0.5f}, .nrm{ 0.0f,  0.0f,  1.0f}, .tex{0.0f, 0.0f}},
        
        vertex{.pos{-0.5f,  0.5f,  0.5f}, .nrm{-1.0f,  0.0f,  0.0f}, .tex{1.0f, 0.0f}},
        vertex{.pos{-0.5f,  0.5f, -0.5f}, .nrm{-1.0f,  0.0f,  0.0f}, .tex{1.0f, 1.0f}},
        vertex{.pos{-0.5f, -0.5f, -0.5f}, .nrm{-1.0f,  0.0f,  0.0f}, .tex{0.0f, 1.0f}},
        vertex{.pos{-0.5f, -0.5f, -0.5f}, .nrm{-1.0f,  0.0f,  0.0f}, .tex{0.0f, 1.0f}},
        vertex{.pos{-0.5f, -0.5f,  0.5f}, .nrm{-1.0f,  0.0f,  0.0f}, .tex{0.0f, 0.0f}},
        vertex{.pos{-0.5f,  0.5f,  0.5f}, .nrm{-1.0f,  0.0f,  0.0f}, .tex{1.0f, 0.0f}},

        vertex{.pos{ 0.5f,  0.5f, -0.5f}, .nrm{ 1.0f,  0.0f,  0.0f}, .tex{1.0f, 1.0f}},
        vertex{.pos{ 0.5f,  0.5f,  0.5f}, .nrm{ 1.0f,  0.0f,  0.0f}, .tex{1.0f, 0.0f}},
        vertex{.pos{ 0.5f, -0.5f, -0.5f}, .nrm{ 1.0f,  0.0f,  0.0f}, .tex{0.0f, 1.0f}},
        vertex{.pos{ 0.5f, -0.5f,  0.5f}, .nrm{ 1.0f,  0.0f,  0.0f}, .tex{0.0f, 0.0f}},
        vertex{.pos{ 0.5f, -0.5f, -0.5f}, .nrm{ 1.0f,  0.0f,  0.0f}, .tex{0.0f, 1.0f}},
        vertex{.pos{ 0.5f,  0.5f,  0.5f}, .nrm{ 1.0f,  0.0f,  0.0f}, .tex{1.0f, 0.0f}},

        vertex{.pos{-0.5f, -0.5f, -0.5f}, .nrm{ 0.0f, -1.0f,  0.0f}, .tex{0.0f, 1.0f}},
        vertex{.pos{ 0.5f, -0.5f, -0.5f}, .nrm{ 0.0f, -1.0f,  0.0f}, .tex{1.0f, 1.0f}},
        vertex{.pos{ 0.5f, -0.5f,  0.5f}, .nrm{ 0.0f, -1.0f,  0.0f}, .tex{1.0f, 0.0f}},
        vertex{.pos{ 0.5f, -0.5f,  0.5f}, .nrm{ 0.0f, -1.0f,  0.0f}, .tex{1.0f, 0.0f}},
        vertex{.pos{-0.5f, -0.5f,  0.5f}, .nrm{ 0.0f, -1.0f,  0.0f}, .tex{0.0f, 0.0f}},
        vertex{.pos{-0.5f, -0.5f, -0.5f}, .nrm{ 0.0f, -1.0f,  0.0f}, .tex{0.0f, 1.0f}},

        vertex{.pos{-0.5f,  0.5f, -0.5f}, .nrm{ 0.0f,  1.0f,  0.0f}, .tex{0.0f, 1.0f}},
        vertex{.pos{ 0.5f,  0.5f,  0.5f}, .nrm{ 0.0f,  1.0f,  0.0f}, .tex{1.0f, 0.0f}},
        vertex{.pos{ 0.5f,  0.5f, -0.5f}, .nrm{ 0.0f,  1.0f,  0.0f}, .tex{1.0f, 1.0f}},
        vertex{.pos{ 0.5f,  0.5f,  0.5f}, .nrm{ 0.0f,  1.0f,  0.0f}, .tex{1.0f, 0.0f}},
        vertex{.pos{-0.5f,  0.5f, -0.5f}, .nrm{ 0.0f,  1.0f,  0.0f}, .tex{0.0f, 1.0f}},
        vertex{.pos{-0.5f,  0.5f,  0.5f}, .nrm{ 0.0f,  1.0f,  0.0f}, .tex{0.0f, 0.0f}},
        // clang-format on
    };

    opengl::vertex_array vao;
    opengl::vertex_buffer vbo = opengl::vertex_buffer(cube_vertices.data(), cube_vertices.size() * sizeof(cube_vertices[0]));

    opengl::shader_program player_shader = opengl::shader_program("math_testing/assets/player.glsl");
    opengl::shader_uniform
        u_player_view_mat,
        u_player_proj_mat,
        u_player_cust_mat,
        u_player_style,
        u_player_col1;
    opengl::shader_program second_shader = opengl::shader_program("math_testing/assets/second.glsl");
    opengl::shader_uniform
        u_second_view_mat,
        u_second_proj_mat;
    opengl::shader_program frame_shader = opengl::shader_program("math_testing/assets/frame.glsl");
    opengl::shader_uniform
        u_player_frame,
        u_second_frame;

    int cam_num = 0;
    bool increase = false, decrease = false;

    glm::mat4 custom_mat = glm::scale(glm::identity<glm::mat4>(), {1, 1, 1});
    glm::mat4 second_proj_view;

    void shaders_init() {
        u_player_view_mat = player_shader.find_uniform("u_view_mat");
        u_player_proj_mat = player_shader.find_uniform("u_proj_mat");
        u_player_cust_mat = player_shader.find_uniform("u_cust_mat");
        u_player_style = player_shader.find_uniform("u_style");
        u_player_col1 = player_shader.find_uniform("u_col1");
        player_shader.bind();
        opengl::shader_program::send(u_player_view_mat, player.cam.view_mat);
        opengl::shader_program::send(u_player_proj_mat, player.cam.proj_mat);
        opengl::shader_program::send(u_player_cust_mat, custom_mat);
        opengl::shader_program::send(u_player_col1, glm::vec4(0, 0, 0, 1));

        u_second_view_mat = second_shader.find_uniform("u_view_mat");
        u_second_proj_mat = second_shader.find_uniform("u_proj_mat");
        second_shader.bind();
        opengl::shader_program::send(u_second_view_mat, second.cam.view_mat);
        opengl::shader_program::send(u_second_proj_mat, second.cam.proj_mat);

        u_player_frame = frame_shader.find_uniform("u_player_frame");
        u_second_frame = frame_shader.find_uniform("u_second_frame");
    }

  public:
    my_app() : coel::application({1200, 600}, "simple game") {
        use_raw_mouse(true);

        second.cam.pos = {0, 0, 3};
        second.cam.znear = 0.5;
        second.cam.zfar = 10;
        second.cam.fov = glm::radians(70.0f);
        second.cam.update_proj();

        vao.bind();
        opengl::vertex_array::set_layout<glm::vec3, glm::vec3, glm::vec2>();

        shaders_init();

        player_frame.bind();
        unsigned int player_frame_attachments[] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(sizeof(player_frame_attachments) / sizeof(unsigned int), player_frame_attachments);
        opengl::framebuffer::attach(player_frame_tex, GL_COLOR_ATTACHMENT0);
        opengl::framebuffer::attach(player_rbo, GL_DEPTH_STENCIL_ATTACHMENT);

        second_frame.bind();
        unsigned int second_frame_attachments[] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(sizeof(second_frame_attachments) / sizeof(unsigned int), second_frame_attachments);
        opengl::framebuffer::attach(second_frame_tex, GL_COLOR_ATTACHMENT0);
        opengl::framebuffer::attach(second_rbo, GL_DEPTH_STENCIL_ATTACHMENT);
    }

    void on_update(coel::duration elapsed) {
        player.update(elapsed);
        player_shader.bind();
        opengl::shader_program::send(u_player_view_mat, player.cam.view_mat);

        second.update(elapsed);
        second_shader.bind();
        opengl::shader_program::send(u_second_view_mat, player.cam.view_mat);

        glm::uvec2 elem{0, 3};

        // 0, 0 is x shift x (scale)
        // 1, 0 is y shift x
        // 2, 0 is z shift x
        // 3, 0 is w shift x (shear)

        // 0, 1 is x shift y
        // 1, 1 is y shift y (scale)
        // 2, 1 is z shift y
        // 3, 1 is w shift y (shear)

        // 0, 2 is x shift z
        // 1, 2 is y shift z
        // 2, 2 is z shift z (scale from projection {0, 0})
        // 3, 2 is w shift z (scale from cam.pos)

        // 0, 3 is y rotation
        // 1, 3 is x rotation
        // 2, 3 is z-range scale
        // 3, 3 is z scale

        // float speed = 1;
        if (increase) {
            auto proj_inv = glm::inverse(second.cam.proj_mat);
            // proj_inv[elem.x][elem.y] += float(elapsed.count()) * speed;
            // std::cout << proj_inv[elem.x][elem.y] << "\n";
            for (int yi = 0; yi < 4; ++yi) {
                for (int xi = 0; xi < 4; ++xi)
                    fmt::print("{:>10} ", proj_inv[yi][xi]);
                fmt::print("\n");
            }
            second.cam.proj_mat = glm::inverse(proj_inv);
            opengl::shader_program::send(u_second_proj_mat, second.cam.proj_mat);
        }
        if (decrease) {
            auto proj_inv = glm::inverse(second.cam.proj_mat);
            // proj_inv[elem.x][elem.y] -= float(elapsed.count()) * speed;
            // std::cout << proj_inv[elem.x][elem.y] << "\n";
            second.cam.proj_mat = glm::inverse(proj_inv);
            opengl::shader_program::send(u_second_proj_mat, second.cam.proj_mat);
        }
    }

    void on_draw() {
        vao.bind();

        player_frame.bind();
        glViewport(0, 0, frame_dim.x / 2, frame_dim.y);
        (!is_paused && cam_num == 0) ? glClearColor(0.15f, 0.15f, 0.18f, 1.0f) : glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        player_shader.bind();

        opengl::shader_program::send(u_player_view_mat, player.cam.view_mat);
        opengl::shader_program::send(u_player_cust_mat, custom_mat);
        opengl::shader_program::send(u_player_style, 0);
        opengl::shader_program::send(u_player_col1, glm::vec4(0, 0, 0, 1));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // opengl::shader_program::send(u_player_cust_mat, glm::inverse(second.cam.view_mat));
        // opengl::shader_program::send(u_player_outline_col, glm::vec4(1, 0, 0, 1));
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        // opengl::shader_program::send(u_player_cust_mat, glm::inverse(second.cam.proj_mat));
        // opengl::shader_program::send(u_player_outline_col, glm::vec4(0, 0, 1, 1));
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        opengl::shader_program::send(u_player_cust_mat, glm::inverse(second.cam.proj_mat * second.cam.view_mat));
        opengl::shader_program::send(u_player_style, 1);
        opengl::shader_program::send(u_player_col1, glm::vec4(1, 0, 1, 1));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        second_frame.bind();
        glViewport(0, 0, frame_dim.x / 2, frame_dim.y);
        (!is_paused && cam_num == 1) ? glClearColor(0.18f, 0.15f, 0.15f, 1.0f) : glClearColor(0.3f, 0.2f, 0.2f, 1.0f);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        second_shader.bind();
        opengl::shader_program::send(u_second_view_mat, second.cam.view_mat);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        opengl::framebuffer::unbind();
        glViewport(0, 0, frame_dim.x, frame_dim.y);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glClear(GL_COLOR_BUFFER_BIT);
        frame_shader.bind();
        opengl::shader_program::send(u_player_frame, 0);
        player_frame_tex.bind(0);
        opengl::shader_program::send(u_second_frame, 1);
        second_frame_tex.bind(1);
        quad.draw();
    }

    void on_key(const coel::key_event &e) {
        if (!is_paused) {
            switch (e.key) {
            case GLFW_KEY_1: cam_num = 0; break;
            case GLFW_KEY_2: cam_num = 1; break;
            case GLFW_KEY_UP: increase = e.action; break;
            case GLFW_KEY_DOWN: decrease = e.action; break;
            }
            switch (cam_num) {
            case 0: player.key_press(e); break;
            case 1: second.key_press(e); break;
            }
        }
    }

    void on_mouse_move() {
        if (!is_paused) {
            const auto screen_center = glm::vec2(frame_dim) * 0.5f;
            switch (cam_num) {
            case 0: player.move_mouse(input.mouse.cursor_pos - screen_center); break;
            case 1: second.move_mouse(input.mouse.cursor_pos - screen_center); break;
            }
            set_mouse_pos(screen_center);
        }
    }

    void on_mouse_button(const coel::mouse_button_event &e) {
        if (is_paused) {
            if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
                cam_num = input.mouse.cursor_pos.x > frame_dim.x / 2;
                toggle_pause();
            }
        }
    }

    void on_resize() {
        glm::uvec2 player_frame_dim = {frame_dim.x / 2, frame_dim.y};
        player_frame.bind();
        player_frame_tex.recreate({
            .data{
                .dim = player_frame_dim,
                .format = GL_RGBA,
            },
            .gl_format = GL_RGBA16F,
            .wrap = {.s = GL_CLAMP_TO_EDGE, .t = GL_CLAMP_TO_EDGE},
            .filter = {.min = GL_LINEAR, .max = GL_LINEAR},
            .samples = 4,
            .use_mipmap = false,
        });
        player_rbo.recreate({
            .dim = player_frame_dim,
            .gl_format = GL_DEPTH24_STENCIL8,
        });
        player_frame.verify();
        player.resize_cam(player_frame_dim);
        player_shader.bind();
        opengl::shader_program::send(u_player_proj_mat, player.cam.proj_mat);

        glm::uvec2 second_frame_dim = {frame_dim.x / 2, frame_dim.y};
        second_frame.bind();
        second_frame_tex.recreate({
            .data{
                .dim = second_frame_dim,
                .format = GL_RGBA,
            },
            .gl_format = GL_RGBA16F,
            .wrap = {.s = GL_CLAMP_TO_EDGE, .t = GL_CLAMP_TO_EDGE},
            .filter = {.min = GL_LINEAR, .max = GL_LINEAR},
            .samples = 4,
            .use_mipmap = false,
        });
        second_rbo.recreate({
            .dim = second_frame_dim,
            .gl_format = GL_DEPTH24_STENCIL8,
        });
        second_frame.verify();
        second.resize_cam(second_frame_dim);
        second_shader.bind();
        opengl::shader_program::send(u_second_proj_mat, second.cam.proj_mat);
    }

    void on_pause() { set_mouse_capture(false); }
    void on_resume() { set_mouse_capture(true); }
};

int main() {
    my_app game;
    if (!game)
        return -1;
    game.resize();
    while (game.update()) {
    }
}
