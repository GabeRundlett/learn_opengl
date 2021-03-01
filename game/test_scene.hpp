#pragma once

#include <game/player.hpp>
#include <coel/opengl/model.hpp>

struct test_scene {
    player3d player;

    struct vertex {
        glm::vec3 pos, nrm;
        glm::vec2 tex;
    };

    // clang-format off
    static inline const std::array cube_vertices = {
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
    };
    // clang-format on

    opengl::vertex_array cube_vao;
    opengl::vertex_buffer cube_vbo;
    glm::mat4 cube_modl_mat, light_modl_mat;

    opengl::shader_program shader;
    opengl::shader_uniform
        u_cam_pos,
        u_view_mat,
        u_proj_mat,
        u_modl_mat,
        u_texture_index,
        u_texture_scale,
        u_material_index,
        u_diffuse_intensity,
        u_diffuse_ambience,
        u_specular_intensity,
        u_specular_sharpness,
        u_specular_roughness,
        u_albedo_tex,
        u_normal_tex,
        u_roughness_tex;

    // opengl::model3d terrain, nanosuit;
    // opengl::shader_program model_shader;

    opengl::texture2d
        cube_albedo_tex,
        cube_normal_tex,
        cube_roughness_tex;

    std::vector<float> voxels;
    opengl::texture3d cube_voxel_tex;

    test_scene()
        : cube_vao(),
          cube_vbo(cube_vertices.data(), cube_vertices.size() * sizeof(cube_vertices[0])),
          shader("game/assets/shaders/scene_vert.glsl", "game/assets/shaders/scene_frag.glsl"),
          //   terrain("game/assets/models/test_cube/test_cube.obj"),
          //   nanosuit("game/assets/models/nanosuit/nanosuit.obj"),
          cube_albedo_tex({
              .filepath = "game/assets/textures/planks_013/Planks013_1K_Color.jpg",
              .gl_format = GL_SRGB_ALPHA,
              .filter = {.min = GL_LINEAR_MIPMAP_LINEAR},
              .use_mipmap = true,
          }),
          cube_normal_tex({
              .filepath = "game/assets/textures/planks_013/Planks013_1K_Normal.jpg",
              .gl_format = GL_SRGB_ALPHA,
              .filter = {.min = GL_LINEAR_MIPMAP_LINEAR},
              .use_mipmap = true,
          }),
          cube_roughness_tex({
              .filepath = "game/assets/textures/planks_013/Planks013_1K_Roughness.jpg",
              .gl_format = GL_RGBA,
              .filter = {.min = GL_LINEAR_MIPMAP_LINEAR},
              .use_mipmap = true,
          }),
          voxels{
              // clang-format off
              0, 0, 0, 0,
              0, 0, 0, 0,
              0, 0, 0, 0,
              
              0, 0, 0, 0,
              0, 0, 0, 0,
              0, 0, 0, 0,
              
              0, 0, 0, 0,
              0, 0, 0, 0,
              0, 0, 0, 0,
              // clang-format on
          },
          cube_voxel_tex({
              .data_format = GL_RGBA,
              .data_type = GL_RGBA,
              .gl_format = GL_RED,
              .filter = {.min = GL_NEAREST, .max = GL_NEAREST},
          }) {
    }

    void init() {
        cube_vao.bind();
        cube_vbo.bind();
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, reinterpret_cast<const void *>(0));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, reinterpret_cast<const void *>(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, reinterpret_cast<const void *>(6 * sizeof(float)));
        cube_vao.unbind();
        glEnable(GL_TEXTURE_3D);

        player.init();
        u_view_mat = shader.find_uniform("u_view_mat");
        u_proj_mat = shader.find_uniform("u_proj_mat");
        u_albedo_tex = shader.find_uniform("u_albedo_tex");
        u_normal_tex = shader.find_uniform("u_normal_tex");
        u_roughness_tex = shader.find_uniform("u_roughness_tex");
        u_texture_index = shader.find_uniform("u_tex_index");
        u_texture_scale = shader.find_uniform("u_texture_scale");
        u_cam_pos = shader.find_uniform("u_cam_pos");
        u_material_index = shader.find_uniform("u_material_index");
        u_modl_mat = shader.find_uniform("u_modl_mat");
        u_diffuse_intensity = shader.find_uniform("u_diffuse_intensity");
        u_diffuse_ambience = shader.find_uniform("u_diffuse_ambience");
        u_specular_intensity = shader.find_uniform("u_specular_intensity");
        u_specular_sharpness = shader.find_uniform("u_specular_sharpness");
        u_specular_roughness = shader.find_uniform("u_specular_roughness");
        cube_modl_mat = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, 0, 0));
        light_modl_mat = glm::scale(glm::translate(glm::identity<glm::mat4>(), glm::vec3(1.2, 2, 3)), glm::vec3(0.2, 0.2, 0.2));
        glUniform1i(u_albedo_tex.location, 0);
        glUniform1i(u_normal_tex.location, 1);
        glUniform1i(u_roughness_tex.location, 2);
    }

    void resize(glm::ivec2 size) {
        player.resize_cam(size);
        // mesh_batch.shader.bind();
        // glUniformMatrix4fv(mesh_batch.u_proj_mat.location, 1, false, reinterpret_cast<float *>(&player.cam.proj_mat));
        shader.bind();
        glUniformMatrix4fv(u_proj_mat.location, 1, false, reinterpret_cast<float *>(&player.cam.proj_mat));
    }

    void on_update(double elapsed) {
        player.update(elapsed);
        // mesh_batch.shader.bind();
        // glUniformMatrix4fv(mesh_batch.u_view_mat.location, 1, false, reinterpret_cast<float *>(&player.cam.view_mat));
        shader.bind();
        glUniformMatrix4fv(u_view_mat.location, 1, false, reinterpret_cast<float *>(&player.cam.view_mat));
        glUniform3fv(u_cam_pos.location, 1, reinterpret_cast<float *>(&player.cam.pos));
    }

    void draw_cube(glm::vec3 pos, glm::vec3 size, float texture_scale) {
        glm::mat4 modl_mat = glm::scale(glm::translate(glm::mat4(1), pos), size);
        glUniformMatrix4fv(u_modl_mat.location, 1, false, reinterpret_cast<float *>(&modl_mat));
        glUniform1i(u_material_index.location, 0);
        glUniform1f(u_texture_scale.location, texture_scale);
        cube_albedo_tex.bind(0);
        cube_normal_tex.bind(1);
        cube_roughness_tex.bind(2);
        cube_vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    void draw() {
        glClearColor(1.81, 2.01, 5.32, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        // glEnable(GL_CULL_FACE);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // terrain.draw();

        shader.bind();
        draw_cube({0, 0.5, 0}, {6, 4, 6}, 1);

        glUniformMatrix4fv(u_modl_mat.location, 1, false, reinterpret_cast<float *>(&light_modl_mat));
        glUniform1i(u_material_index.location, 1);
        cube_vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
};