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

    void regenerate() {
        for (int yi = 0; yi < dim.y; ++yi) {
            for (int xi = 0; xi < dim.x; ++xi) {
                auto &tile = tiles[xi + yi * dim.x];
                tile = tile_id(xi == 0 || yi == 0 || xi == dim.x - 1 || yi == dim.y - 1);
            }
        }

        recalculate_tex();
    }

    struct raycast_config {
        glm::vec2 origin, dir;
        std::uint32_t max_iter;
        float max_dist;
    };

    struct raycast_result {
        glm::ivec2 tile_index;
        std::uint32_t total_steps;
        bool hit_surface, is_vertical;
    };

    raycast_result raycast(const raycast_config &config) {
        using namespace glm;

        raycast_result result;

        result.tile_index = ivec2(config.origin);
        result.total_steps = 0;
        result.hit_surface = false;

        vec2 delta_dist{
            config.dir.y == 0 ? 0 : (config.dir.x == 0 ? 1 : std::abs(1.0f / config.dir.x)),
            config.dir.x == 0 ? 0 : (config.dir.y == 0 ? 1 : std::abs(1.0f / config.dir.y)),
        };

        vec2 to_side_dist;
        ivec2 ray_step;
        if (config.dir.x < 0) {
            ray_step.x = -1, to_side_dist.x = (config.origin.x - result.tile_index.x) * delta_dist.x;
        } else {
            ray_step.x = 1, to_side_dist.x = (result.tile_index.x + 1.0f - config.origin.x) * delta_dist.x;
        }
        if (config.dir.y < 0) {
            ray_step.y = -1, to_side_dist.y = (config.origin.y - result.tile_index.y) * delta_dist.y;
        } else {
            ray_step.y = 1, to_side_dist.y = (result.tile_index.y + 1.0f - config.origin.y) * delta_dist.y;
        }

        while (result.total_steps < config.max_iter) {
            if (to_side_dist.x < to_side_dist.y) {
                to_side_dist.x += delta_dist.x;
                result.tile_index.x += ray_step.x;
                result.is_vertical = false;
            } else {
                to_side_dist.y += delta_dist.y;
                result.tile_index.y += ray_step.y;
                result.is_vertical = true;
            }

            if (get_tile(result.tile_index) != tile_id::none) {
                result.hit_surface = true;
                break;
            }

            ++result.total_steps;
        }

        return result;
    }

    struct surface_details {
        glm::vec2 pos, nrm;
    };

    surface_details get_surface_details(const raycast_config &config, const raycast_result &result) {
        using namespace glm;

        surface_details surface;
        surface.pos = result.tile_index;

        float slope_xy = config.dir.x / config.dir.y;
        float slope_yx = config.dir.y / config.dir.x;

        if (result.is_vertical) {
            if (config.dir.y < 0) {
                surface.pos.y += 1;
                surface.nrm = vec2(0, 1);
            } else {
                surface.nrm = vec2(0, -1);
            }
            surface.pos = vec2(config.origin.x + (surface.pos.y - config.origin.y) * slope_xy, surface.pos.y);
        } else {
            if (config.dir.x < 0) {
                surface.pos.x += 1;
                surface.nrm = vec2(1, 0);
            } else {
                surface.nrm = vec2(-1, 0);
            }
            surface.pos = vec2(surface.pos.x, config.origin.y + (surface.pos.x - config.origin.x) * slope_yx);
        }

        return surface;
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

    tile_id &get_tile(glm::ivec2 coord) {
        return tiles[coord.x + coord.y * dim.x];
    }
};
