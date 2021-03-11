#pragma once

#include <coel/opengl/core.hpp>

class deferred_renderer {
  public:
    glm::uvec2 frame_dim;

  private:
    static inline constexpr std::array quad_vertices = {
        // clang-format off
        -1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
         1.0f, -1.0f,
        // clang-format on
    };

    opengl::framebuffer frame;
    opengl::vertex_array frame_quad_vao;
    opengl::vertex_buffer frame_quad_vbo = opengl::vertex_buffer(quad_vertices.data(), quad_vertices.size() * sizeof(quad_vertices[0]));
    opengl::shader_program frame_shader = opengl::shader_program({.filepath = "voxel_game/assets/shaders/frame_vert.glsl"}, {.filepath = "voxel_game/assets/shaders/frame_frag.glsl"});
    opengl::shader_uniform
        u_frame_gamma,
        u_frame_exposure,
        u_frame_pos_tex,
        u_frame_nrm_tex,
        u_frame_col_tex;
    opengl::texture2d<> frame_pos_tex, frame_nrm_tex, frame_col_tex;
    opengl::renderbuffer frame_depth_rbo;

  public:
    deferred_renderer(glm::uvec2 frame_dim) : frame_dim(frame_dim) {
        frame_quad_vao.bind();
        opengl::set_layout<glm::vec2>();

        frame_shader.bind();
        u_frame_gamma = frame_shader.find_uniform("u_gamma");
        u_frame_exposure = frame_shader.find_uniform("u_exposure");
        u_frame_pos_tex = frame_shader.find_uniform("u_frame_pos_tex");
        u_frame_nrm_tex = frame_shader.find_uniform("u_frame_nrm_tex");
        u_frame_col_tex = frame_shader.find_uniform("u_frame_col_tex");
        frame_shader.send(u_frame_gamma, 1.2f);
        frame_shader.send(u_frame_exposure, 1.0f);
        frame.bind();
        unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
        glDrawBuffers(3, attachments);
        frame.attach(frame_pos_tex, GL_COLOR_ATTACHMENT0);
        frame.attach(frame_nrm_tex, GL_COLOR_ATTACHMENT1);
        frame.attach(frame_col_tex, GL_COLOR_ATTACHMENT2);
        frame.attach(frame_depth_rbo, GL_DEPTH_STENCIL_ATTACHMENT);
    }

    void use() {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, frame_dim.x, frame_dim.y);
        frame.bind();
    }

    void draw() {
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glEnable(GL_MULTISAMPLE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        frame_shader.bind();
        frame_shader.send(u_frame_pos_tex, 0);
        frame_shader.send(u_frame_nrm_tex, 1);
        frame_shader.send(u_frame_col_tex, 2);
        frame_pos_tex.bind(0);
        frame_nrm_tex.bind(1);
        frame_col_tex.bind(2);
        frame_quad_vao.bind();
        glDrawArrays(GL_QUADS, 0, 4);
    }

    void resize(glm::uvec2 new_dim) {
        frame_dim = new_dim;
        frame_pos_tex.regenerate({
            .dim = frame_dim,
            .gl_format = GL_RGBA16F,
            .data_format = GL_RGBA,
            .wrap = {.s = GL_CLAMP_TO_EDGE, .t = GL_CLAMP_TO_EDGE},
            .filter = {.min = GL_LINEAR, .max = GL_LINEAR},
            .samples = 4,
            .use_mipmap = false,
        });
        frame_nrm_tex.regenerate({
            .dim = frame_dim,
            .gl_format = GL_RGBA16F,
            .data_format = GL_RGBA,
            .wrap = {.s = GL_CLAMP_TO_EDGE, .t = GL_CLAMP_TO_EDGE},
            .filter = {.min = GL_LINEAR, .max = GL_LINEAR},
            .samples = 4,
            .use_mipmap = false,
        });
        frame_col_tex.regenerate({
            .dim = frame_dim,
            .gl_format = GL_RGBA16F,
            .data_format = GL_RGBA,
            .wrap = {.s = GL_CLAMP_TO_EDGE, .t = GL_CLAMP_TO_EDGE},
            .filter = {.min = GL_LINEAR, .max = GL_LINEAR},
            .samples = 4,
            .use_mipmap = false,
        });
        frame_depth_rbo.regenerate({
            .dim = frame_dim,
            .gl_format = GL_DEPTH24_STENCIL8,
        });
        frame.verify();
    }
};
