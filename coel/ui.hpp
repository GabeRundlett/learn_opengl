#pragma once

#include <vector>
#include <memory>

namespace coel {

    namespace ui_units {
        struct unit_constraint {
            float value;
        };

        unit_constraint operator""_px(std::size_t x) {
            //
        }

        unit_constraint operator""_px(long double x) {
            //
        }

        unit_constraint operator""_pc(std::size_t x) {
            //
        }

        unit_constraint operator""_pc(long double x) {
            //
        }
    } // namespace ui_units

    struct vec2 {
        float x, y;
    };
    struct ivec2 {
        int x, y;
    };
    struct unit_constraint_vec2 {
        ui_units::unit_constraint x, y;
    };

    struct container_config {
        unit_constraint_vec2 pos, size;
    };
    class container {
      public:
        container_config config;
        std::vector<std::shared_ptr<container>> children;

        container(const container_config &conf) : config(conf) {
        }

        auto add_container(const container_config &conf) -> container & {
            children.push_back(std::make_shared<container>(conf));
            return *children.back();
        }

        template <typename T>
        auto add(typename T::config_t conf) -> T & {
            children.push_back(std::make_shared<T>(conf));
            return *dynamic_cast<T *>(&*children.back());
        }
    };

    struct slider_config {
        vec2 pos, size;
    };
    class slider : public container {
      public:
        using config_t = slider_config;

        float value() const;
    };

    struct ui_window_config {
        ivec2 size;
    };

    class ui_window : public container {
      public:
        ui_window_config config;

        // ui_window(const ui_window_config &conf) : container({.pos = {0, 0}, .size = conf.size}), config(conf) {
        // }

        bool is_open() const;

        void update();
        void present();
    };
} // namespace coel
