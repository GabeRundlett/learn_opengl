#pragma once

#include <coel/opengl/core.hpp>
#include "noise.hpp"
#include <fmt/core.h>

struct chunk3d {
    static inline constexpr glm::uvec3 size = {32, 32, 32};
    static inline constexpr auto
        max_vcount = size.x * size.y * size.z * 4 * 6,
        max_icount = size.x * size.y * size.z * 6 * 6;

    glm::ivec3 index;
    glm::vec3 pos;
    std::array<std::array<std::array<std::uint8_t, size.x>, size.y>, size.z> blocks;

    struct vertex {
        glm::vec3 pos, nrm;
        glm::vec2 tex;
    };

    opengl::vertex_array vao;
    opengl::vertex_buffer_dynamic vbo = opengl::vertex_buffer_dynamic(nullptr, sizeof(vertex) * max_vcount);
    opengl::index_buffer_dynamic ibo = opengl::index_buffer_dynamic(nullptr, sizeof(unsigned int) * max_icount);
    unsigned int vcount, icount;

    vertex *vbuffer_ptr;
    unsigned int *ibuffer_ptr;

    struct block_texcoord_config {
        glm::vec2 nz, pz;
        glm::vec2 ny, py;
        glm::vec2 nx, px;
    };

    struct block_facevis_config {
        std::uint8_t nz : 1, pz : 1;
        std::uint8_t ny : 1, py : 1;
        std::uint8_t nx : 1, px : 1;
    };

    struct chunk_neighbors {
        chunk *nz = nullptr, *pz = nullptr;
        chunk *ny = nullptr, *py = nullptr;
        chunk *nx = nullptr, *px = nullptr;
    };

    static inline constexpr std::array<block_texcoord_config, 10> block_texcoords = {
        // 0 debug
        block_texcoord_config{
            .nz = {3, 2},
            .pz = {3, 2},
            .ny = {3, 2},
            .py = {3, 2},
            .nx = {3, 2},
            .px = {3, 2},
        },
        // 1 grass
        block_texcoord_config{
            .nz = {1, 0},
            .pz = {1, 0},
            .ny = {2, 0},
            .py = {0, 0},
            .nx = {1, 0},
            .px = {1, 0},
        },
        // 2 dirt
        block_texcoord_config{
            .nz = {2, 0},
            .pz = {2, 0},
            .ny = {2, 0},
            .py = {2, 0},
            .nx = {2, 0},
            .px = {2, 0},
        },
        // 3 sand
        block_texcoord_config{
            .nz = {3, 0},
            .pz = {3, 0},
            .ny = {3, 0},
            .py = {3, 0},
            .nx = {3, 0},
            .px = {3, 0},
        },
        // 4 cobbled stone
        block_texcoord_config{
            .nz = {0, 1},
            .pz = {0, 1},
            .ny = {0, 1},
            .py = {0, 1},
            .nx = {0, 1},
            .px = {0, 1},
        },
        // 5 cracked stone
        block_texcoord_config{
            .nz = {1, 1},
            .pz = {1, 1},
            .ny = {1, 1},
            .py = {1, 1},
            .nx = {1, 1},
            .px = {1, 1},
        },
        // 6 smooth stone
        block_texcoord_config{
            .nz = {2, 1},
            .pz = {2, 1},
            .ny = {2, 1},
            .py = {2, 1},
            .nx = {2, 1},
            .px = {2, 1},
        },
        // 7 gravel
        block_texcoord_config{
            .nz = {3, 1},
            .pz = {3, 1},
            .ny = {3, 1},
            .py = {3, 1},
            .nx = {3, 1},
            .px = {3, 1},
        },
        // 8 log
        block_texcoord_config{
            .nz = {0, 2},
            .pz = {0, 2},
            .ny = {1, 2},
            .py = {1, 2},
            .nx = {0, 2},
            .px = {0, 2},
        },
        // 9 leaves
        block_texcoord_config{
            .nz = {2, 2},
            .pz = {2, 2},
            .ny = {2, 2},
            .py = {2, 2},
            .nx = {2, 2},
            .px = {2, 2},
        },
    };
    static inline constexpr std::array<bool, 10> block_not_occlusive = {
        true,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
    };

    chunk(glm::ivec3 index) : index(index), pos(glm::vec3(index) * glm::vec3(size)) {
        vao.bind();
        opengl::set_layout<glm::vec3, glm::vec3, glm::vec2>();

        constexpr auto fractal_noise = [](glm::vec3 pos, float persistance, float amplitude, float lacunarity, float scale) {
            float value = 0.0f;
            for (int i = 0; i < 4; ++i) {
                value += noise(pos.x * scale, pos.y * scale, pos.z * scale) * amplitude;
                amplitude *= persistance;
                scale *= lacunarity;
            }
            return value;
        };

        for (int zi = 0; zi < blocks.size(); ++zi) {
            auto &plane = blocks[zi];
            for (int yi = 0; yi < plane.size(); ++yi) {
                auto &row = plane[yi];
                for (int xi = 0; xi < row.size(); ++xi) {
                    glm::vec3 p = glm::vec3(xi, yi, zi) + pos;
                    auto &block_id = row[xi];
                    block_id = 0;
                    float height = fractal_noise({p.x, 0, p.z}, 0.5f, 20.0f, 2.0f, 1.0f / 128);
                    if (p.y < height) {
                        int y = (int)p.y, h = (int)height + (height > 0);
                        float block_noise = fractal_noise(p + glm::vec3(100, 100, 100), 0.5f, 1.0f, 2.0f, 1.0f / 96);
                        if (block_noise < 0.7) {
                            if (h - y == 1) {
                                block_id = 1;
                            } else if (h - y < 5) {
                                block_id = 2;
                            } else
                                block_id = 6;
                        }
                    }
                }
            }
        }
    }

    void add_face_x(glm::vec3 p0, glm::vec3 p1, glm::vec3 nrm, glm::vec2 t0, glm::vec2 t1, bool flip) {
        *reinterpret_cast<std::array<vertex, 4> *>(vbuffer_ptr) = {
            vertex{.pos{p0.x, p0.y, p0.z}, .nrm = nrm, .tex{t0.x, t1.y}},
            vertex{.pos{p0.x, p1.y, p0.z}, .nrm = nrm, .tex{t0.x, t0.y}},
            vertex{.pos{p0.x, p0.y, p1.z}, .nrm = nrm, .tex{t1.x, t1.y}},
            vertex{.pos{p0.x, p1.y, p1.z}, .nrm = nrm, .tex{t1.x, t0.y}},
        };
        *reinterpret_cast<std::array<unsigned int, 6> *>(ibuffer_ptr) = {
            // clang-format off
            0 + vcount, 1 + vcount + flip, 2 + vcount - flip,
            1 + vcount, 3 + vcount - flip, 2 + vcount + flip,
            // clang-format on
        };
        vcount += 4, vbuffer_ptr += 4;
        icount += 6, ibuffer_ptr += 6;
    }
    void add_face_y(glm::vec3 p0, glm::vec3 p1, glm::vec3 nrm, glm::vec2 t0, glm::vec2 t1, bool flip) {
        *reinterpret_cast<std::array<vertex, 4> *>(vbuffer_ptr) = {
            vertex{.pos{p0.x, p0.y, p0.z}, .nrm = nrm, .tex{t0.x, t1.y}},
            vertex{.pos{p0.x, p0.y, p1.z}, .nrm = nrm, .tex{t1.x, t1.y}},
            vertex{.pos{p1.x, p0.y, p0.z}, .nrm = nrm, .tex{t0.x, t0.y}},
            vertex{.pos{p1.x, p0.y, p1.z}, .nrm = nrm, .tex{t1.x, t0.y}},
        };
        *reinterpret_cast<std::array<unsigned int, 6> *>(ibuffer_ptr) = {
            // clang-format off
            0 + vcount, 1 + vcount + flip, 2 + vcount - flip,
            1 + vcount, 3 + vcount - flip, 2 + vcount + flip,
            // clang-format on
        };
        vcount += 4, vbuffer_ptr += 4;
        icount += 6, ibuffer_ptr += 6;
    }
    void add_face_z(glm::vec3 p0, glm::vec3 p1, glm::vec3 nrm, glm::vec2 t0, glm::vec2 t1, bool flip) {
        *reinterpret_cast<std::array<vertex, 4> *>(vbuffer_ptr) = {
            vertex{.pos{p0.x, p0.y, p0.z}, .nrm = nrm, .tex{t0.x, t1.y}},
            vertex{.pos{p1.x, p0.y, p0.z}, .nrm = nrm, .tex{t1.x, t1.y}},
            vertex{.pos{p0.x, p1.y, p0.z}, .nrm = nrm, .tex{t0.x, t0.y}},
            vertex{.pos{p1.x, p1.y, p0.z}, .nrm = nrm, .tex{t1.x, t0.y}},
        };
        *reinterpret_cast<std::array<unsigned int, 6> *>(ibuffer_ptr) = {
            // clang-format off
            0 + vcount, 1 + vcount + flip, 2 + vcount - flip,
            1 + vcount, 3 + vcount - flip, 2 + vcount + flip,
            // clang-format on
        };
        vcount += 4, vbuffer_ptr += 4;
        icount += 6, ibuffer_ptr += 6;
    }

    void add_block(glm::vec3 p, int block_id, const block_facevis_config &vis) {
        if (vis.nz)
            add_face_z(p + glm::vec3{0.0f, 0.0f, 0.0f}, p + glm::vec3{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, block_texcoords[block_id].nz + glm::vec2{1.0f, 0.0f}, block_texcoords[block_id].nz + glm::vec2{0.0f, 1.0f}, true); // back
        if (vis.pz)
            add_face_z(p + glm::vec3{0.0f, 0.0f, 1.0f}, p + glm::vec3{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, block_texcoords[block_id].pz + glm::vec2{0.0f, 0.0f}, block_texcoords[block_id].pz + glm::vec2{1.0f, 1.0f}, false); // front
        if (vis.ny)
            add_face_y(p + glm::vec3{0.0f, 0.0f, 0.0f}, p + glm::vec3{1.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, block_texcoords[block_id].ny + glm::vec2{0.0f, 0.0f}, block_texcoords[block_id].ny + glm::vec2{1.0f, 1.0f}, true); // bottom
        if (vis.py)
            add_face_y(p + glm::vec3{0.0f, 1.0f, 0.0f}, p + glm::vec3{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, block_texcoords[block_id].py + glm::vec2{0.0f, 0.0f}, block_texcoords[block_id].py + glm::vec2{1.0f, 1.0f}, false); // top
        if (vis.nx)
            add_face_x(p + glm::vec3{0.0f, 0.0f, 0.0f}, p + glm::vec3{0.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, block_texcoords[block_id].nx + glm::vec2{0.0f, 0.0f}, block_texcoords[block_id].nx + glm::vec2{1.0f, 1.0f}, true); // left
        if (vis.px)
            add_face_x(p + glm::vec3{1.0f, 0.0f, 0.0f}, p + glm::vec3{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, block_texcoords[block_id].px + glm::vec2{1.0f, 0.0f}, block_texcoords[block_id].px + glm::vec2{0.0f, 1.0f}, false); // right
    }

    void begin_build_buffer() {
        vbuffer_ptr = vbo.map<vertex>();
        ibuffer_ptr = ibo.map<unsigned int>();
        vcount = 0, icount = 0;
    }
    void build_buffer(const chunk_neighbors &neighbors) {
        for (int zi = 0; zi < blocks.size(); ++zi) {
            auto &plane = blocks[zi];
            for (int yi = 0; yi < plane.size(); ++yi) {
                auto &row = plane[yi];
                for (int xi = 0; xi < row.size(); ++xi) {
                    auto &block_id = row[xi];
                    if (block_id) {
                        block_facevis_config vis{0, 0, 0, 0, 0, 0};
                        if (zi != blocks.size() - 1)
                            vis.pz = block_not_occlusive[blocks[zi + 1][yi][xi]];
                        else if (neighbors.pz)
                            vis.pz = block_not_occlusive[neighbors.pz->blocks[0][yi][xi]];
                        if (zi != 0)
                            vis.nz = block_not_occlusive[blocks[zi - 1][yi][xi]];
                        else if (neighbors.nz)
                            vis.nz = block_not_occlusive[neighbors.nz->blocks[size.z - 1][yi][xi]];

                        if (yi != blocks.size() - 1)
                            vis.py = block_not_occlusive[blocks[zi][yi + 1][xi]];
                        else if (neighbors.py)
                            vis.py = block_not_occlusive[neighbors.py->blocks[zi][0][xi]];
                        if (yi != 0)
                            vis.ny = block_not_occlusive[blocks[zi][yi - 1][xi]];
                        else if (neighbors.ny)
                            vis.ny = block_not_occlusive[neighbors.ny->blocks[zi][size.y - 1][xi]];

                        if (xi != blocks.size() - 1)
                            vis.px = block_not_occlusive[blocks[zi][yi][xi + 1]];
                        else if (neighbors.px)
                            vis.px = block_not_occlusive[neighbors.px->blocks[zi][yi][0]];
                        if (xi != 0)
                            vis.nx = block_not_occlusive[blocks[zi][yi][xi - 1]];
                        else if (neighbors.nx)
                            vis.nx = block_not_occlusive[neighbors.nx->blocks[zi][yi][size.x - 1]];
                        add_block(pos + glm::vec3(xi, yi, zi), block_id, vis);
                    }
                }
            }
        }
    }
    void end_build_buffer() {
        vbo.unmap();
        ibo.unmap();
    }
    void regenerate_buffer(const chunk_neighbors &neighbors) {
        begin_build_buffer();
        build_buffer(neighbors);
        end_build_buffer();
    }

    void draw_buffer() const {
        vao.bind();
        ibo.bind();
        glDrawElements(GL_TRIANGLES, icount, GL_UNSIGNED_INT, nullptr);
    }
};

struct chunkworld3d {
    static inline constexpr int render_dist = 1;
    std::vector<chunk *> chunk_lookup;
    std::array<std::array<std::array<chunk *, render_dist * 2>, render_dist * 2>, render_dist * 2> chunks;

    ~chunk_world() {
        for (auto &chunk_ptr : chunk_lookup)
            delete chunk_ptr;
    }

    chunk *add_chunk(glm::ivec3 index) {
        const auto c_iter = std::find_if(chunk_lookup.begin(), chunk_lookup.end(), [=](const auto &c) {
            return c->index == index;
        });
        if (c_iter == chunk_lookup.end()) {
            chunk_lookup.push_back(new chunk(index));
            return *(chunk_lookup.end() - 1);
        }
        return *c_iter;
    }

    glm::ivec3 get_chunk_index(glm::ivec3 index) {
        index.x = index.x % (render_dist * 2);
        index.y = index.y % (render_dist * 2);
        index.z = index.z % (render_dist * 2);
        if (index.x < 0)
            index.x += render_dist * 2;
        if (index.y < 0)
            index.y += render_dist * 2;
        if (index.z < 0)
            index.z += render_dist * 2;
        return index;
    }

    chunk *get_chunk(glm::ivec3 index) {
        index = get_chunk_index(index);
        chunk *result = chunks[index.z][index.y][index.x];
        return result;
    }

    chunk::chunk_neighbors get_chunk_neighbors(glm::ivec3 index) {
        chunk::chunk_neighbors neighbors;
        chunk *temp_chunk_ptr = nullptr;
        temp_chunk_ptr = get_chunk({index.x, index.y, index.z + 1});
        if (temp_chunk_ptr != nullptr && temp_chunk_ptr->index == glm::ivec3{index.x, index.y, index.z + 1})
            neighbors.pz = temp_chunk_ptr;
        temp_chunk_ptr = get_chunk({index.x, index.y, index.z - 1});
        if (temp_chunk_ptr != nullptr && temp_chunk_ptr->index == glm::ivec3{index.x, index.y, index.z - 1})
            neighbors.nz = temp_chunk_ptr;
        temp_chunk_ptr = get_chunk({index.x, index.y + 1, index.z});
        if (temp_chunk_ptr != nullptr && temp_chunk_ptr->index == glm::ivec3{index.x, index.y + 1, index.z})
            neighbors.py = temp_chunk_ptr;
        temp_chunk_ptr = get_chunk({index.x, index.y - 1, index.z});
        if (temp_chunk_ptr != nullptr && temp_chunk_ptr->index == glm::ivec3{index.x, index.y - 1, index.z})
            neighbors.ny = temp_chunk_ptr;
        temp_chunk_ptr = get_chunk({index.x + 1, index.y, index.z});
        if (temp_chunk_ptr != nullptr && temp_chunk_ptr->index == glm::ivec3{index.x + 1, index.y, index.z})
            neighbors.px = temp_chunk_ptr;
        temp_chunk_ptr = get_chunk({index.x - 1, index.y, index.z});
        if (temp_chunk_ptr != nullptr && temp_chunk_ptr->index == glm::ivec3{index.x - 1, index.y, index.z})
            neighbors.nx = temp_chunk_ptr;
        return neighbors;
    }

    void init() {
        for (int zi = 0; zi < chunks.size(); ++zi) {
            auto &plane = chunks[zi];
            for (int yi = 0; yi < plane.size(); ++yi) {
                auto &row = plane[yi];
                for (int xi = 0; xi < row.size(); ++xi) {
                    auto &chunk_ptr = row[xi];
                    // chunk_ptr = add_chunk({xi - render_dist, yi - render_dist, zi - render_dist});
                    chunk_ptr = nullptr;
                }
            }
        }
        // for (int zi = 0; zi < chunks.size(); ++zi) {
        //     auto &plane = chunks[zi];
        //     for (int yi = 0; yi < plane.size(); ++yi) {
        //         auto &row = plane[yi];
        //         for (int xi = 0; xi < row.size(); ++xi) {
        //             auto &chunk_ptr = row[xi];
        //             auto neighbors = get_chunk_neighbors(chunk_ptr->index);
        //             chunk_ptr->regenerate_buffer(neighbors);
        //         }
        //     }
        // }
    }

    void update(glm::vec3 camera_pos) {
        constexpr int updates_per_tick = 8;
        int chunk_updates = 0;
        for (int zi = 0; zi < chunks.size(); ++zi) {
            for (int yi = 0; yi < chunks[zi].size(); ++yi) {
                for (int xi = 0; xi < chunks[zi][yi].size(); ++xi) {
                    glm::ivec3 current_chunk_index = glm::ivec3(xi - render_dist, yi - render_dist, zi - render_dist);
                    // chunk *current_chunk_ptr = get_chunk(current_chunk_index);
                    glm::ivec3 test_chunk_index = current_chunk_index + glm::ivec3(camera_pos / glm::vec3(chunk::size));
                    chunk *test_chunk_ptr = get_chunk(test_chunk_index);
                    if (test_chunk_ptr == nullptr || test_chunk_ptr->index != test_chunk_index) {
                        fmt::print("adding chunk {}", (void *)test_chunk_ptr);
                        if (test_chunk_ptr)
                            fmt::print(" found: {} {} {}", test_chunk_ptr->index.x, test_chunk_ptr->index.y, test_chunk_ptr->index.z);
                        fmt::print(" check: {} {} {}\n", test_chunk_index.x, test_chunk_index.y, test_chunk_index.z);
                        auto *current_chunk_ptr = add_chunk(test_chunk_index);
                        current_chunk_index = get_chunk_index(current_chunk_index);
                        chunks[current_chunk_index.z][current_chunk_index.y][current_chunk_index.x] = current_chunk_ptr;
                        auto neighbors = get_chunk_neighbors(current_chunk_index);
                        current_chunk_ptr->regenerate_buffer(neighbors);
                        ++chunk_updates;
                    }
                    if (chunk_updates >= updates_per_tick)
                        break;
                }
                if (chunk_updates >= updates_per_tick)
                    break;
            }
            if (chunk_updates >= updates_per_tick)
                break;
        }
    }

    void draw() const {
        for (int zi = 0; zi < chunks.size(); ++zi) {
            auto &plane = chunks[zi];
            for (int yi = 0; yi < plane.size(); ++yi) {
                auto &row = plane[yi];
                for (int xi = 0; xi < row.size(); ++xi) {
                    auto &chunk_ptr = row[xi];
                    if (chunk_ptr)
                        chunk_ptr->draw_buffer();
                }
            }
        }
    }
};
