#pragma once

#include <cstdint>
#include <array>

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
                tile = tile_id(rand() % 12);
            }
        }

        recalculate_tex();
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

    tile_id &get_tile(glm::vec2 p) {
        return tiles[int(p.x) % dim.x + int(p.y) % dim.x * dim.x];
    }
};
