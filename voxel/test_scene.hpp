#pragma once

#include <voxel_game/player.hpp>

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
    opengl::vertex_buffer cube_vbo = opengl::vertex_buffer(cube_vertices.data(), cube_vertices.size() * sizeof(cube_vertices[0]));
    glm::mat4 cube_modl_mat, light_modl_mat;

    opengl::shader_program shader = opengl::shader_program(
        {.filepath = "voxel/assets/shaders/scene_vert.glsl"},
        {.filepath = "voxel/assets/shaders/scene_frag.glsl"});
    opengl::shader_uniform
        u_cam_pos,
        u_cube_pos,
        u_cube_dim,
        u_view_mat,
        u_proj_mat,
        u_modl_mat,
        u_voxel_tex,
        u_material_index;

    glm::uvec3 lattice_dim = {1, 1, 1};
    std::vector<glm::u8vec4> voxels = std::vector<glm::u8vec4>(lattice_dim.x * lattice_dim.y * lattice_dim.z);
    opengl::texture3d<glm::u8vec4> voxel_tex = opengl::texture3d<glm::u8vec4>({
        .data = nullptr,
        .dim = {lattice_dim.x, lattice_dim.y, lattice_dim.z},
        .gl_format = GL_R32UI,
        .data_format = GL_RED_INTEGER,
        .data_type = GL_UNSIGNED_INT,
        .wrap = {.s = GL_CLAMP_TO_EDGE, .t = GL_CLAMP_TO_EDGE, .r = GL_CLAMP_TO_EDGE},
        .filter = {.min = GL_NEAREST, .max = GL_NEAREST},
        .border_color = {0, 0, 0, 0},
    });

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

        player.init();
        u_cam_pos = shader.find_uniform("u_cam_pos");
        u_cube_pos = shader.find_uniform("u_cube_pos");
        u_cube_dim = shader.find_uniform("u_cube_dim");
        u_view_mat = shader.find_uniform("u_view_mat");
        u_proj_mat = shader.find_uniform("u_proj_mat");
        u_modl_mat = shader.find_uniform("u_modl_mat");
        u_voxel_tex = shader.find_uniform("u_voxel_tex");
        u_material_index = shader.find_uniform("u_material_index");
        cube_modl_mat = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, 0, 0));
        light_modl_mat = glm::scale(glm::translate(glm::identity<glm::mat4>(), glm::vec3(1.2, 2, 3)), glm::vec3(0.2, 0.2, 0.2));
        glUniform1i(u_voxel_tex.location, 0);

        for (uint32_t z = 0; z < lattice_dim.z; ++z) {
            for (uint32_t y = 0; y < lattice_dim.y; ++y) {
                for (uint32_t x = 0; x < lattice_dim.x; ++x) {
                    auto &vox = voxels[x + y * lattice_dim.x + z * lattice_dim.x * lattice_dim.y];
                    std::uint8_t val;
                    val = ((sin(0.04 * x + 0.038 * z) + 1) * 5 > y) * 200;
                    // val = 200 * ((x + y + z) % 2);
                    vox = {val, val, val, 255};
                }
            }
        }

        voxel_tex.bind();
        glTexSubImage3D(
            GL_TEXTURE_3D, 0,
            0, 0, 0,
            lattice_dim.x, lattice_dim.y, lattice_dim.z,
            GL_RED_INTEGER, GL_UNSIGNED_INT,
            voxels.data());
    }

    void resize(glm::ivec2 size) {
        player.resize_cam(size);
        shader.bind();
        glUniformMatrix4fv(u_proj_mat.location, 1, false, reinterpret_cast<float *>(&player.cam.proj_mat));
    }

    void on_update(double elapsed) {
        player.update(elapsed);
        shader.bind();
        glUniformMatrix4fv(u_view_mat.location, 1, false, reinterpret_cast<float *>(&player.cam.view_mat));
        glUniform3fv(u_cam_pos.location, 1, reinterpret_cast<float *>(&player.cam.pos));
    }

    void draw_cube(glm::vec3 pos, glm::vec3 size, float) {
        glm::mat4 modl_mat = glm::scale(glm::translate(glm::mat4(1), pos), size);
        glUniformMatrix4fv(u_modl_mat.location, 1, false, reinterpret_cast<float *>(&modl_mat));
        glUniform3fv(u_cube_pos.location, 1, reinterpret_cast<float *>(&pos));
        glUniform3fv(u_cube_dim.location, 1, reinterpret_cast<float *>(&size));
        glUniform1i(u_material_index.location, 0);
        glUniform1i(u_voxel_tex.location, 0);
        voxel_tex.bind(0);
        cube_vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // for (auto &f : voxels)
        //     f = {0, 0, 0, 0};
        // glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_FLOAT, voxels.data());
        // int i = 0;
        // for (auto &f : voxels)
        //     std::cout << f.r << " " << f.g << " " << f.b << " " << f.a << (i++ % 16 != 15 ? ", " : "\n");
        // std::cin.get();
    }

    void draw() {
        glClearColor(1.81f, 2.01f, 5.32f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        // glEnable(GL_CULL_FACE);
        glDisable(GL_CULL_FACE);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        shader.bind();
        draw_cube({0, 0, 0}, glm::vec3(lattice_dim) * 0.25f, 1);

        // glUniformMatrix4fv(u_modl_mat.location, 1, false, reinterpret_cast<float *>(&light_modl_mat));
        // glUniform1i(u_material_index.location, 1);
        // cube_vao.bind();
        // glDrawArrays(GL_TRIANGLES, 0, 36);
    }
};
