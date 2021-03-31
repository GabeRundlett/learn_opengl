#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <algorithm>

template <typename pixel_t>
struct frame {
    glm::ivec2 size;
    std::vector<pixel_t> pixels;

    void resize(glm::uvec2 dim) {
        size = dim;
        pixels.resize(size.x * size.y);
    }

    auto &operator()(int x, int y) {
        return pixels[x + y * size.x];
    }
    const auto &operator()(int x, int y) const {
        return pixels[x + y * size.x];
    }
};

struct screen {
    using pixel = glm::u8vec4;
    frame<pixel> frame;

    std::array<glm::vec2, 3> points{{
        {-0.5f, -0.2f},
        {0.8f, -0.8f},
        {0.3f, 0.8f},
    }};

    void clear() {
        for (auto &p : frame.pixels) {
            p.r = 31;
            p.g = 31;
            p.b = 31;
            p.a = 255;
        }
    }

    constexpr auto from_ndc(glm::vec2 ndc) const {
        return glm::ivec2{
            (ndc.x + 1.0f) * 0.5f * frame.size.x,
            (ndc.y + 1.0f) * 0.5f * frame.size.y,
        };
    }

    void draw() {
        std::array<glm::ivec2, 3> screen_points;

        glm::ivec2 min_index = {0, 0}, max_index = {0, 0};
        for (int i = 0; i < points.size(); ++i) {
            screen_points[i] = from_ndc(points[i]);
            if (screen_points[i].x < screen_points[min_index.x].x)
                min_index.x = i;
            else if (screen_points[i].x > screen_points[max_index.x].x)
                max_index.x = i;
            if (screen_points[i].y < screen_points[min_index.y].y)
                min_index.y = i;
            else if (screen_points[i].y > screen_points[max_index.y].y)
                max_index.y = i;
        }
        // int bend_index = 3 - (min_index.y + max_index.y);
        const auto
            &p0 = screen_points[min_index.y],
            &p1 = screen_points[max_index.y]; //&p2 = screen_points[bend_index];

        const glm::ivec2 bound_margin = {0, 0};

        const glm::ivec2
            bound_min = {std::max(screen_points[min_index.x].x - bound_margin.x, 0),            std::max(screen_points[min_index.y].y - bound_margin.y, 0)},
            bound_max = {std::min(screen_points[max_index.x].x + bound_margin.x, frame.size.x), std::min(screen_points[max_index.y].y + bound_margin.y, frame.size.y)};

        float slope01 = float(p1.y - p0.y) / (p1.x - p0.x);

        for (int yi = bound_min.y; yi < bound_max.y; ++yi) {
            for (int xi = bound_min.x; xi < bound_max.x; ++xi) {
                glm::ivec2 screen_p = {xi, yi};
                auto &p = frame(xi, yi);

                if ((xi - p0.x) * slope01 > yi - p0.y)
                    p = {255, 255, 255, 255};
                // else if (glm::dot(glm::vec2(p0 - screen_p), glm::vec2(p0 - screen_p)) < 100)
                //     p = {255, 0, 0, 255};
                // else if (glm::dot(glm::vec2(p1 - screen_p), glm::vec2(p1 - screen_p)) < 100)
                //     p = {0, 255, 0, 255};
                // else if (glm::dot(glm::vec2(p2 - screen_p), glm::vec2(p2 - screen_p)) < 100)
                //     p = {0, 0, 255, 255};
                // else if ((xi + yi) % 16 < 8)
                //     p = {50, 50, 50, 255};
            }
        }
    }

    void resize(glm::uvec2 dim) {
        frame.resize(dim);
        clear();
    }
};
