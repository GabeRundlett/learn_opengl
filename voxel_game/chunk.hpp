#pragma once

#include "noise.hpp"
#include <fmt/core.h>

struct chunk3d {
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

    static inline constexpr glm::uvec3 dim = {128, 128, 128};
    std::vector<std::uint32_t> tiles = std::vector<std::uint32_t>(dim.x * dim.y * dim.z);
    opengl::texture3d<std::uint32_t> tiles_tex = opengl::texture3d<std::uint32_t>({
        .data{
            .dim = {dim.x, dim.y, dim.z},
            .format = GL_RED_INTEGER,
            .type = GL_UNSIGNED_INT,
        },
        .gl_format = GL_R32UI,
        .wrap = {.s = GL_CLAMP_TO_BORDER, .t = GL_CLAMP_TO_BORDER, .r = GL_CLAMP_TO_BORDER},
        .filter = {.min = GL_NEAREST, .max = GL_NEAREST},
        .border_color = {0, 0, 0, 0},
    });

    opengl::texture2d<> tilemap_tex = opengl::texture2d<>({
        .filepath = "voxel_game/assets/textures/tilemap.png",
        .gl_format = GL_RGBA,
        .filter = {.min = GL_NEAREST, .max = GL_NEAREST},
    });

    chunk3d() {
        vao.bind();
        opengl::vertex_array::set_layout<glm::vec3, glm::vec3, glm::vec2>();

        fractal_noise_config noise_conf{
            .amplitude = 1.0f,
            .persistance = 0.5f,
            .scale = 0.01f,
            .lacunarity = 2.0f,
            .octaves = 4,
        };
        for (std::uint32_t z = 0; z < dim.z; ++z) {
            for (std::uint32_t y = 0; y < dim.y; ++y) {
                for (std::uint32_t x = 0; x < dim.x; ++x) {
                    auto &tile = tiles[x + y * dim.x + z * dim.x * dim.y];
                    float y_scale = 1.0f / (y + 1);
                    float density = fractal_noise(glm::vec3(x, y, z), noise_conf) * 1000 * y_scale * y_scale;
                    std::uint8_t val = 0;
                    if (density > 0.2)
                        val = 1;
                    else if (density > 0.15)
                        val = 2;
                    else if (density > 0.125)
                        val = 3;
                    else if (density > 0.05)
                        val = 4;
                    if (y == 0)
                        val = 1;
                    tile = val;
                }
            }
        }

        update();
    }

    struct hit_information {
        glm::vec3 pos, nrm;
        int tile_id;
    };
    struct raycast_information {
        bool hit;
        int steps;
        hit_information hit_info;
    };

    auto &get_tile(glm::vec3 fp) {
        glm::ivec3 p = fp + glm::vec3(dim) * 0.5f;
        if (p.x < 0)
            p.x = 0;
        else if (p.x > int(dim.x - 1))
            p.x = dim.x - 1;
        if (p.y < 0)
            p.y = 0;
        else if (p.y > int(dim.y - 1))
            p.y = dim.y - 1;
        if (p.z < 0)
            p.z = 0;
        else if (p.z > int(dim.z - 1))
            p.z = dim.z - 1;
        return tiles[p.x + p.y * dim.x + p.z * dim.x * dim.y];
    }

    void raycast(glm::vec3 ray_origin, glm::vec3 ray_dir, raycast_information &i) {
        using namespace glm;

        const int MAX_ITER = 1000;
        const float EPSILON = 0.001f;
        const vec3 space_scale = vec3(1);

        using namespace glm;
        i.hit = false;
        i.hit_info.pos = ray_origin;
        i.hit_info.tile_id = get_tile(i.hit_info.pos);
        if (i.hit_info.tile_id != 0) {
            i.hit_info.nrm = vec3(0, 0, 0);
            i.hit = true;
            return;
        }
        vec3 ray_p = floor(ray_origin / space_scale) * space_scale;
        vec3 ray_d = ray_p - ray_origin;
        vec3 ray_step = -space_scale;
        if (ray_dir.x > 0)
            ray_d.x += space_scale.x, ray_step.x *= -1;
        if (ray_dir.y > 0)
            ray_d.y += space_scale.y, ray_step.y *= -1;
        if (ray_dir.z > 0)
            ray_d.z += space_scale.z, ray_step.z *= -1;
        i.steps = 0;
        float slope_xy = ray_dir.y / ray_dir.x, slope_xz = ray_dir.z / ray_dir.x;
        float slope_yx = ray_dir.x / ray_dir.y, slope_yz = ray_dir.z / ray_dir.y;
        float slope_zx = ray_dir.x / ray_dir.z, slope_zy = ray_dir.y / ray_dir.z;
        if (ray_dir.x == 0 || ray_dir.y == 0 || ray_dir.z == 0)
            return;
        vec3 to_travel_x = vec3(ray_d.x, slope_xy * ray_d.x, slope_xz * ray_d.x);
        vec3 to_travel_y = vec3(slope_yx * ray_d.y, ray_d.y, slope_yz * ray_d.y);
        vec3 to_travel_z = vec3(slope_zx * ray_d.z, slope_zy * ray_d.z, ray_d.z);
        while (i.steps < MAX_ITER) {
            while (i.steps < MAX_ITER &&
                   to_travel_x.x * ray_step.x < to_travel_y.x * ray_step.x &&
                   to_travel_x.x * ray_step.x < to_travel_z.x * ray_step.x) {
                i.hit_info.pos += to_travel_x;
                // if (i.hit_info.pos.x < 0 || i.hit_info.pos.y < 0 || i.hit_info.pos.z < 0 ||
                //     i.hit_info.pos.x > 1 || i.hit_info.pos.y > 1 || i.hit_info.pos.z > 1)
                //     return;
                i.hit_info.tile_id = get_tile(i.hit_info.pos + vec3(ray_step.x * EPSILON, 0, 0));
                if (i.hit_info.tile_id != 0) {
                    i.hit_info.nrm = vec3(sign(-ray_step.x), 0, 0);
                    i.hit = true;
                    return;
                }
                to_travel_y -= to_travel_x;
                to_travel_z -= to_travel_x;
                to_travel_x = vec3(
                    ray_step.x,
                    slope_xy * ray_step.x,
                    slope_xz * ray_step.x);
                ++i.steps;
            }
            while (i.steps < MAX_ITER &&
                   to_travel_y.y * ray_step.y < to_travel_x.y * ray_step.y &&
                   to_travel_y.y * ray_step.y < to_travel_z.y * ray_step.y) {
                i.hit_info.pos += to_travel_y;
                // if (i.hit_info.pos.x < 0 || i.hit_info.pos.y < 0 || i.hit_info.pos.z < 0 ||
                //     i.hit_info.pos.x > 1 || i.hit_info.pos.y > 1 || i.hit_info.pos.z > 1)
                //     return;
                i.hit_info.tile_id = get_tile(i.hit_info.pos + vec3(0, ray_step.y * EPSILON, 0));
                if (i.hit_info.tile_id != 0) {
                    i.hit_info.nrm = vec3(0, sign(-ray_step.y), 0);
                    i.hit = true;
                    return;
                }
                to_travel_x -= to_travel_y;
                to_travel_z -= to_travel_y;
                to_travel_y = vec3(
                    slope_yx * ray_step.y,
                    ray_step.y,
                    slope_yz * ray_step.y);
                ++i.steps;
            }
            while (i.steps < MAX_ITER &&
                   to_travel_z.z * ray_step.z < to_travel_y.z * ray_step.z &&
                   to_travel_z.z * ray_step.z < to_travel_x.z * ray_step.z) {
                i.hit_info.pos += to_travel_z;
                // if (i.hit_info.pos.x < 0 || i.hit_info.pos.y < 0 || i.hit_info.pos.z < 0 ||
                //     i.hit_info.pos.x > 1 || i.hit_info.pos.y > 1 || i.hit_info.pos.z > 1)
                //     return;
                i.hit_info.tile_id = get_tile(i.hit_info.pos + vec3(0, 0, ray_step.z * EPSILON));
                if (i.hit_info.tile_id != 0) {
                    i.hit_info.nrm = vec3(0, 0, sign(-ray_step.z));
                    i.hit = true;
                    return;
                }
                to_travel_x -= to_travel_z;
                to_travel_y -= to_travel_z;
                to_travel_z = vec3(
                    slope_zx * ray_step.z,
                    slope_zy * ray_step.z,
                    ray_step.z);
                ++i.steps;
            }
            ++i.steps;
        }
    }

    void update() {
        tiles_tex.bind();
        tiles_tex.update({
            .ptr = tiles.data(),
            .dim = {dim.x, dim.y, dim.z},
            .format = GL_RED_INTEGER,
            .type = GL_UNSIGNED_INT,
        });
    }
};
