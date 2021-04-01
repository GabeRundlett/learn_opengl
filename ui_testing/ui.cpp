#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <coel/application.hpp>
#include <concepts>
#include <algorithm>

template<typename t>
concept container = requires(const t obj) {
    { obj.contains(glm::vec2{}) } -> std::convertible_to<bool>;
};

template<typename t>
concept renderable = requires(t obj) {
    obj.draw();
};

template<typename t>
concept pressable = requires(t obj) {
    container<t>;
    { obj.is_pressed() } -> std::convertible_to<bool>;
    { obj.press_offset() } -> std::convertible_to<glm::vec2>;
    obj.press(glm::vec2{});
    obj.release();
};

template<typename t>
concept draggable = requires(t obj) {
    pressable<t>;
    obj.drag(glm::vec2{});
};

template<typename t>
bool is_hovered(const coel::input_state &input, const t &obj) requires container<t> {
    return obj.contains(input.mouse.cursor_pos);
}

template<typename t>
void press(const coel::input_state &input, t &obj) requires pressable<t> {
    if (input.mouse.buttons[GLFW_MOUSE_BUTTON_LEFT]) {
        if (!obj.is_pressed()) {
            if (is_hovered(input, obj) && input.mouse.active.action == GLFW_PRESS)
                obj.press(input.mouse.cursor_pos);
        }
    } else {
        obj.release();
    }
}

template<typename t>
void grab(const coel::input_state &input, t &obj) requires draggable<t> {
    if (input.mouse.buttons[GLFW_MOUSE_BUTTON_LEFT]) {
        if (!obj.is_pressed()) {
            if (is_hovered(input, obj) && input.mouse.active.action == GLFW_PRESS) {
                obj.press(input.mouse.cursor_pos);
                obj.drag(input.mouse.cursor_pos);
            }
        } else {
            obj.drag(input.mouse.cursor_pos);
        }
    } else {
        obj.drag(input.mouse.cursor_pos);
        obj.release();
    }
}

opengl::renderer::ui_batch *ui;

struct rectangle_container_state {
    glm::vec2 pos = {0, 0}, size = {100, 100};
};

struct draggable_state {
    bool grabbed = false;
    glm::vec2 grab_offset;
};

struct resizeable_state {
    enum edge_flag {
        right_edge = 1 << 0,
        bottom_edge = 1 << 1,
        left_edge = 1 << 2,
        top_edge = 1 << 3,
    };
    bool grabbed = false;
    glm::vec2 grab_offset;
    int edge = 0;
};

struct block {
    rectangle_container_state rect;
    draggable_state drag_state;
    resizeable_state resize_state;

    static inline constexpr glm::vec2 resize_margin{10, 10};

    void draw() {
        ui->submit_rect(rect.pos, rect.size, {0.1, 0.1, 0.1, 1});
        ui->submit_rect(rect.pos + resize_margin, rect.size - resize_margin * 2.0f, {0.08, 0.08, 0.08, 1});
    }
    bool contains(glm::vec2 p) const {
        return p.x >= rect.pos.x && p.x < rect.pos.x + rect.size.x
            && p.y >= rect.pos.y && p.y < rect.pos.y + rect.size.y;
    }
    bool is_pressed() const {
        return drag_state.grabbed || resize_state.grabbed;
    }
    void press(glm::vec2 p) {
        if (p.x >= rect.pos.x + rect.size.x - resize_margin.x) {
            resize_state.edge |= resizeable_state::right_edge;
            resize_state.grab_offset.x = rect.size.x - p.x;
            resize_state.grabbed = true;
        } 
        if (p.y >= rect.pos.y + rect.size.y - resize_margin.y) {
            resize_state.edge |= resizeable_state::bottom_edge;
            resize_state.grab_offset.y = rect.size.y - p.y;
            resize_state.grabbed = true;
        }
        if (p.x <= rect.pos.x + resize_margin.x) {
            resize_state.edge |= resizeable_state::left_edge;
            resize_state.grab_offset.x = p.x - rect.pos.x;
            resize_state.grabbed = true;
        } 
        if (p.y <= rect.pos.y + resize_margin.y) {
            resize_state.edge |= resizeable_state::top_edge;
            resize_state.grab_offset.y = p.y - rect.pos.y;
            resize_state.grabbed = true;
        }
        
        if (!resize_state.grabbed) {
            drag_state.grab_offset = rect.pos - p;
            drag_state.grabbed = true;
        }
    }
    void drag(glm::vec2 p) {
        if (drag_state.grabbed) {
            rect.pos = drag_state.grab_offset + p;
        } else {
            if (resize_state.edge & resizeable_state::right_edge)
                rect.size.x = resize_state.grab_offset.x + p.x;
            if (resize_state.edge & resizeable_state::bottom_edge)
                rect.size.y = resize_state.grab_offset.y + p.y;

            if (resize_state.edge & resizeable_state::left_edge)
                rect.size.x += rect.pos.x - p.x, rect.pos.x = p.x + resize_state.grab_offset.x;
            if (resize_state.edge & resizeable_state::top_edge)
                rect.size.y += rect.pos.y - p.y, rect.pos.y = p.y + resize_state.grab_offset.y;
            
            rect.size.x = std::max(rect.size.x, resize_margin.x * 2.0f + 1.0f);
            rect.size.y = std::max(rect.size.y, resize_margin.y * 2.0f + 1.0f);
        }
    }
    void release() {
        drag_state.grabbed = false;
        resize_state.grabbed = false;
        resize_state.edge = 0;
    }
};

class my_app : public coel::application {
  public:
    block a;

    my_app() : coel::application({800, 600}, "simple game") {
        use_raw_mouse(true);
        ui = &ui_batch;
    }

    void on_draw() {
        is_paused ? glClearColor(0.2f, 0.2f, 0.2f, 1.0f) : glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        a.draw();
    }

    void on_event() {
        grab(input, a);
    }

    void on_pause() { set_mouse_capture(false); }
    void on_resume() { set_mouse_capture(true); }
};

int main() try {
    my_app game;
    if (!game)
        return -1;
    game.resize();
    while (game.update()) {
    }
} catch (const coel::exception &e) {
    MessageBoxA(nullptr, e.what(), "Coel Exception", MB_OK);
}