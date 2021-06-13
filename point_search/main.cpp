#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "../ui_testing/ui2.hpp"
#include <coel/opengl/renderers/quad.hpp>

#include "point_search.h"
#include <vector>

class my_app : public coel::application {
  public:
    ui_window window;

    std::vector<Point> points;
    std::size_t points_count = 100000, out_count = 20, query_count = 50;
    std::vector<Point> out_points;
    std::size_t out_points_found = 0;
    Rect bound_rect = {400, 100, 600, 200};

    struct dll_algo {
        std::string name;
        HINSTANCE dll_module_instance;
        T_create create_func;
        T_search search_func;
        T_destroy destroy_func;

        dll_algo(const std::filesystem::path &dll_path) {
            name = dll_path.filename().string();
            dll_module_instance = LoadLibrary(dll_path.string().c_str());
            create_func = (T_create)GetProcAddress(dll_module_instance, "create");
            search_func = (T_search)GetProcAddress(dll_module_instance, "search");
            destroy_func = (T_destroy)GetProcAddress(dll_module_instance, "destroy");
            if (!create_func || !search_func || !destroy_func) {
                std::cout << "could not load all functions from " << dll_path << std::endl;
            }
        }
    };

    std::vector<dll_algo> algos = {
        dll_algo("C:/Users/gabe/Downloads/challenge/build/Release/reference.dll"),
        dll_algo("C:/Users/gabe/Downloads/challenge/build/Release/mysolution0.dll"),
    };

    void points_init() {
        points.clear();

        const auto range_rand = [](float min, float max) { return static_cast<float>(rand() % (1 << 10)) / (1 << 10) * (max - min) + min; };
        for (int i = 0; i < points_count; ++i) {
            glm::vec2 p = {range_rand(10, 1000 - 10), range_rand(10, 800 - 10)};
            points.push_back({.rank = i, .x = p.x, .y = p.y});
        }
    }

    void run_algo() {
        points_init();
        for (const auto &algo : algos) {
            auto points_cpy = points;
            SearchContext *sc = algo.create_func(&points_cpy.front(), &points_cpy.back());
            points_cpy.clear();
            out_points.clear();
            out_points.resize(out_count);
            auto queryT0 = coel::clock::now();
            for (int i = 0; i < query_count; ++i) {
                out_points_found = algo.search_func(sc, bound_rect, out_count, &out_points[0]);
            }
            auto queryT1 = coel::clock::now();
            fmt::print("{} took {}\n", algo.name, coel::duration(queryT1 - queryT0).count() / query_count);
            out_points_found = algo.search_func(sc, bound_rect, out_count, &out_points[0]);
            algo.destroy_func(sc);
        }
    }

    my_app() : coel::application({800, 800}, "Point Search") {
        use_raw_mouse(true);
        resize();

        window.components = {{
            std::make_shared<ui_button>(button{
                .text = []() { return "Re-run"s; },
                .rect = [&]() { return coel::rectangular_container{window.rect.top_left + glm::vec2{10, 10}, window.rect.top_left + glm::vec2{25, 25}}; },
                .call = [&]() { run_algo(); },
            }),
            std::make_shared<ui_slider>(slider{
                .text = [&]() { return fmt::format("In Point Count: {}", points_count); },
                .rect = [&]() { return coel::rectangular_container{window.rect.top_left + glm::vec2{10, 60}, window.rect.top_left + glm::vec2{150, 70}}; },
                .call = [&](float value) { points_count = value; },
                .value = float(points_count),
                .range = {.min = 10, .max = 10'000'000},
            }),
            std::make_shared<ui_slider>(slider{
                .text = [&]() { return fmt::format("Out Point Count: {}", out_count); },
                .rect = [&]() { return coel::rectangular_container{window.rect.top_left + glm::vec2{10, 100}, window.rect.top_left + glm::vec2{150, 110}}; },
                .call = [&](float value) { out_count = value; },
                .value = float(out_count),
                .range = {.min = 10, .max = 100},
            }),
            std::make_shared<ui_slider>(slider{
                .text = [&]() { return fmt::format("Query Count: {}", query_count); },
                .rect = [&]() { return coel::rectangular_container{window.rect.top_left + glm::vec2{10, 140}, window.rect.top_left + glm::vec2{150, 150}}; },
                .call = [&](float value) { query_count = value; },
                .value = float(query_count),
                .range = {.min = 1, .max = 1000},
            }),
            std::make_shared<ui_slider>(slider{
                .text = [&]() { return fmt::format("Rect lx", bound_rect.lx); },
                .rect = [&]() { return coel::rectangular_container{window.rect.top_left + glm::vec2{10, 250}, window.rect.top_left + glm::vec2{150, 260}}; },
                .call = [&](float value) { bound_rect.lx = value; },
                .value = float(bound_rect.lx),
                .range = {.min = 0, .max = 800},
            }),
            std::make_shared<ui_slider>(slider{
                .text = [&]() { return fmt::format("Rect ly", bound_rect.ly); },
                .rect = [&]() { return coel::rectangular_container{window.rect.top_left + glm::vec2{10, 280}, window.rect.top_left + glm::vec2{150, 290}}; },
                .call = [&](float value) { bound_rect.ly = value; },
                .value = float(bound_rect.ly),
                .range = {.min = 0, .max = 800},
            }),
            std::make_shared<ui_slider>(slider{
                .text = [&]() { return fmt::format("Rect hx", bound_rect.hx); },
                .rect = [&]() { return coel::rectangular_container{window.rect.top_left + glm::vec2{10, 310}, window.rect.top_left + glm::vec2{150, 320}}; },
                .call = [&](float value) { bound_rect.hx = value; },
                .value = float(bound_rect.hx),
                .range = {.min = 0, .max = 800},
            }),
            std::make_shared<ui_slider>(slider{
                .text = [&]() { return fmt::format("Rect hy", bound_rect.hy); },
                .rect = [&]() { return coel::rectangular_container{window.rect.top_left + glm::vec2{10, 340}, window.rect.top_left + glm::vec2{150, 350}}; },
                .call = [&](float value) { bound_rect.hy = value; },
                .value = float(bound_rect.hy),
                .range = {.min = 0, .max = 800},
            }),
        }};

        run_algo();
    }

    void on_draw() {
        is_paused ? glClearColor(0.2f, 0.2f, 0.2f, 1.0f) : glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        window.draw(ui_batch, text_batch);

        ui_batch.submit_rect({bound_rect.lx, bound_rect.ly}, {bound_rect.hx, bound_rect.hy}, {1.0f, 1.0f, 1.0f, 0.5f});
        for (auto i = 0; i < points.size(); i += 100) {
            const auto &p = points[i];
            ui_batch.submit_rect({p.x - 1, p.y - 1}, {p.x + 1, p.y + 1}, {static_cast<float>(p.rank) / points.size(), 0.0f, 0.0f, 0.5f});
        }
        for (auto i = 0; i < out_points_found; ++i) {
            const auto &p = out_points[i];
            ui_batch.submit_rect({p.x - 2, p.y - 2}, {p.x + 2, p.y + 2}, {static_cast<float>(p.rank) / points.size(), 1.0f, 1.0f, 0.5f});
        }
    }

    void on_event() override {
        window.on_event(input);
    }

    void on_resize() override {
        window.resize(frame_dim);
    }

    void on_pause() { set_mouse_capture(false); }
    void on_resume() { set_mouse_capture(true); }
};

int main() try {
    my_app game;
    if (!game)
        return -1;
    while (game.update()) {
    }
} catch (const coel::exception &e) {
    MessageBoxA(nullptr, e.what(), "Coel Exception", MB_OK);
}
