#include <memory>
#include <cstdio>
#include <vector>

template <typename value_t>
struct vec2 {
    value_t x, y;
};
using vec2_f32 = vec2<float>;
using vec2_f64 = vec2<double>;
using vec2_i32 = vec2<std::int32_t>;
using vec2_i64 = vec2<std::int64_t>;
using vec2_u32 = vec2<std::uint32_t>;
using vec2_u64 = vec2<std::uint64_t>;

struct pixel_constraint {
};
struct percent_constraint {
};
struct ndc_constraint {
};

ndc_constraint operator""_px(unsigned long long int x) {
    return {};
}
ndc_constraint operator""_px(long double x) {
    return {};
}

ndc_constraint operator""_pc(unsigned long long int x) {
    return {};
}
ndc_constraint operator""_pc(long double x) {
    return {};
}

ndc_constraint operator+(const ndc_constraint &a, const ndc_constraint &b) {
}
ndc_constraint operator-(const ndc_constraint &a, const ndc_constraint &b) {
}

class container_2d {
  public:
    struct constraints {
        vec2<ndc_constraint> pos, dim;
    };

    struct configuration {
        container_2d *parent = nullptr;
        const char *const name = nullptr;
        constraints constrain;
    };

    container_2d *parent = nullptr;
    const char *const name = nullptr;
    constraints constraints;

  protected:
    std::vector<std::shared_ptr<container_2d>> children;
    vec2_f32 ndc_pos, ndc_dim;

  public:
    container_2d(const configuration &conf)
        : parent(conf.parent),
          name(conf.name),
          constraints(conf.constrain) {
        std::printf("container_2d() %s\n", name);
    }

    ~container_2d() {
        children.clear();
        std::printf("~container_2d() %s\n", name);
    }

    template <typename T, typename... args_t>
    auto add_child(args_t... args) -> T & {
        children.emplace_back(std::make_shared<T>(args...));
        auto &child = *reinterpret_cast<T *>(&*children.back());
        child.parent = this;
        return child;
    }

    void resize() {
        // do calculations

        for (auto &c : children)
            c->resize();
    }
};

class button : public container_2d {
  public:
    struct configuration {
        const char *const name;
        vec2<ndc_constraint> pos, size;
    };

    float value;

    button(const configuration conf)
        : container_2d({
              .name = conf.name,
              .constrain{
                  .pos = conf.pos,
                  .dim = conf.size,
              },
          }) {
        std::printf("button() %s\n", name);
    }

    ~button() {
        std::printf("~button() %s\n", name);
    }
};

int main() {
    window w(container_2d::configuration{
        .name = "my window",
        .constrain{
            .dim = {800_px, 600_px},
        },
    });

    auto &b1 = w.add_child<button>(button::configuration{
        .name = "b1",
        .pos = {20_px, 20_px},
        .size = {200_px, 100_px},
    });
    auto &b2 = w.add_child<button>(button::configuration{
        .name = "b2",
        .pos = {20_px, 200_px},
        .size = {200_px, 100_px},
    });
    auto &b3 = w.add_child<button>(button::configuration{
        .name = "b3",
        .pos = {100_pc - 220_px, 200_px},
        .size = {200_px, 100_px},
    });

    while (w.is_open()) {
        w.update();

        // do some things like
        if (b1.state.is_pressed) {
            // blah blah blah
        }

        w.present();
    }
}
