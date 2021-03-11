#pragma once

#include <coel/opengl/core.hpp>
#include "noise.hpp"

struct chunk2d_vertex {
    glm::vec2 pos, tex;
};

static inline constexpr glm::ivec2 chunk_size = {128, 128};

class chunk2d : public opengl::batch<chunk2d, chunk2d_vertex, 4 * chunk_size.x * chunk_size.y, 6 * chunk_size.x * chunk_size.y> {
    std::vector<std::uint8_t> tiles;
    glm::vec2 pos;

  public:
    chunk2d(glm::vec2 pos) : pos(pos) {
        tiles.reserve(chunk_size.x * chunk_size.y);
        for (int yi = 0; yi < chunk_size.y; ++yi) {
            for (int xi = 0; xi < chunk_size.x; ++xi) {
                float val = fractal_noise(
                    glm::vec2(xi, yi) + pos,
                    {
                        .amplitude = 1.0f,
                        .persistance = 0.8f,
                        .scale = 0.01f,
                        .lacunarity = 2.0f,
                        .octaves = 4,
                    });
                if (val > 0)
                    tiles.push_back(0);
                else
                    tiles.push_back(1);
            }
        }

        vao.bind();
        opengl::vertex_array::set_layout<glm::vec2, glm::vec2>();

        begin();
        for (int yi = 0; yi < chunk_size.y; ++yi) {
            for (int xi = 0; xi < chunk_size.x; ++xi) {
                glm::vec2 tex = tiles[xi + yi * chunk_size.x] ? glm::vec2{0.0f, 1.0f} : glm::vec2{2.0f, 0.0f};
                submit_tile(pos + glm::vec2{xi, yi}, pos + glm::vec2{xi + 1, yi + 1}, tex);
            }
        }
        end();
    }

    void submit_tile(glm::vec2 p0, glm::vec2 p1, glm::vec2 t) {
        submit(
            std::array<chunk2d_vertex, 4>{
                chunk2d_vertex{{p0.x, p0.y}, {t.x + 0, t.y + 1}},
                chunk2d_vertex{{p1.x, p0.y}, {t.x + 1, t.y + 1}},
                chunk2d_vertex{{p1.x, p1.y}, {t.x + 1, t.y + 0}},
                chunk2d_vertex{{p0.x, p1.y}, {t.x + 0, t.y + 0}},
            },
            std::array<unsigned int, 6>{
                0 + current_vcount, 1 + current_vcount, 3 + current_vcount,
                1 + current_vcount, 2 + current_vcount, 3 + current_vcount});
    }

    void before_flush() {}
};

class chunkworld2d {
  public:
    std::vector<chunk2d> chunks;

    chunkworld2d() {
        int render_dist = 1;
        for (int yi = -render_dist; yi < render_dist; ++yi) {
            for (int xi = -render_dist; xi < render_dist; ++xi) {
                chunks.emplace_back(glm::vec2{xi * chunk_size.x, yi * chunk_size.y});
            }
        }
    }

    void draw() {
        for (auto &chunk : chunks) {
            chunk.flush();
        }
    }
};
