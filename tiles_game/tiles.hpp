#pragma once

#include <cstdint>
#include <array>
#include <set>

#include <coel/graphics/noise.hpp>

enum class tile_id : std::uint32_t {
    none,

    dirt,
    dirt_grass,
    grass,

    sand,
    gravel,

    stone,
    stone_cracked,
    stone_cobbled,

    leaves,
    log_side,
    log_top,
};

struct tile_chunk {
    static inline constexpr glm::ivec2 dim = {16, 16};
    std::vector<tile_id> tiles = std::vector<tile_id>(dim.x * dim.y);

    opengl::texture2d<tile_id> tex = opengl::texture2d<tile_id>({
        .data{
            .dim = dim,
            .format = GL_RED_INTEGER,
            .type = GL_UNSIGNED_INT,
        },
        .gl_format = GL_R32UI,
        .wrap = {.s = GL_CLAMP_TO_BORDER, .t = GL_CLAMP_TO_BORDER},
        .filter = {.min = GL_NEAREST, .max = GL_NEAREST},
        .border_color = {0, 0, 0, 0},
    });

    bool should_recalculate = false;

    void update() {
        if (should_recalculate) {
            should_recalculate = false;
            recalculate_tex();
        }
    }

    struct surface_info {
        glm::vec2 pos, nrm;
        tile_id *tile;
    };

    struct raycast_info {
        glm::vec2 origin, dir;
        std::uint32_t max_iter;
        float max_dist;
        std::uint32_t steps;
        bool hit_surface;
    };

    void regenerate() {
        for (int yi = 0; yi < dim.y; ++yi) {
            for (int xi = 0; xi < dim.x; ++xi) {
                auto &tile = tiles[xi + yi * dim.x];
                tile = tile_id(0);
            }
        }

        recalculate_tex();
    }

    std::vector<glm::vec2> raycast(raycast_info &ray) {
        std::vector<glm::vec2> intersections;

        ray.hit_surface = false;
        ray.steps = 0;

        glm::vec2 current_pos = ray.origin;
        glm::ivec2 ray_step{int(ray.dir.x > 0), int(ray.dir.y > 0)};
        glm::ivec2 ray_origin_i{int(std::floor(ray.origin.x)), int(std::floor(ray.origin.y))};
        glm::vec2 ray_d = glm::vec2(ray_origin_i + ray_step) - ray.origin;
        ray_step = ray_step * 2 - 1;

        // intersections.push_back(glm::vec2(ray_origin_i) + glm::vec2(0.5, 0.5));
        // intersections.push_back(current_pos);
        // if (get_tile(current_pos) != tile_id::none) {
        //     ray.hit_surface = true;
        //     return intersections;
        // }

        float slope_xy = ray.dir.y / ray.dir.x;
        float slope_yx = ray.dir.x / ray.dir.y;

        glm::vec2 to_travel_x{ray_d.x, slope_xy * ray_d.x};
        glm::vec2 to_travel_y{slope_yx * ray_d.y, ray_d.y};

        std::uint32_t iter = 0;

        while (ray.steps < ray.max_iter && iter < ray.max_iter) {
            while (ray.steps < ray.max_iter && to_travel_x.x * ray_step.x <= to_travel_y.x * ray_step.x) {
                ++ray.steps;
                current_pos += to_travel_x;
                const auto del = current_pos - ray.origin;
                if (del.x * del.x + del.y * del.y > ray.max_dist * ray.max_dist)
                    return intersections;

                auto test_pos = get_tile_coord(current_pos + glm::vec2(ray_step.x * 0.0001, 0));
                auto tile = &get_tile(test_pos);

                intersections.push_back(glm::vec2(test_pos) + glm::vec2(0.5, 0.5));
                intersections.push_back(current_pos);

                if (*tile != tile_id::none) {
                    ray.hit_surface = true;
                    return intersections;
                }

                to_travel_y -= to_travel_x;
                to_travel_x = {float(ray_step.x), slope_xy * ray_step.x};
            }
            while (ray.steps < ray.max_iter && to_travel_y.y * ray_step.y <= to_travel_x.y * ray_step.y) {
                ++ray.steps;
                current_pos += to_travel_y;
                const auto del = current_pos - ray.origin;
                if (del.x * del.x + del.y * del.y > ray.max_dist * ray.max_dist)
                    return intersections;

                auto test_pos = get_tile_coord(current_pos + glm::vec2(0, ray_step.y * 0.001));
                auto tile = &get_tile(test_pos);

                intersections.push_back(glm::vec2(test_pos) + glm::vec2(0.5, 0.5));
                intersections.push_back(current_pos);

                if (*tile != tile_id::none) {
                    ray.hit_surface = true;
                    return intersections;
                }

                to_travel_x -= to_travel_y;
                to_travel_y = {slope_yx * ray_step.y, float(ray_step.y)};
            }
            ++iter;
        }

        return intersections;
    }

    void recalculate_tex() {
        tex.update({
            .ptr = tiles.data(),
            .offset = {0, 0},
            .dim = dim,
            .format = GL_RED_INTEGER,
            .type = GL_UNSIGNED_INT,
        });
    }

    glm::ivec2 get_tile_coord(glm::vec2 p) {
        return {int(p.x) % dim.x, int(p.y) % dim.x};
    }

    tile_id &get_tile(glm::vec2 p) {
        auto coord = get_tile_coord(p);
        return tiles[coord.x + coord.y * dim.x];
    }
};
