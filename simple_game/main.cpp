#include <coel/application.hpp>

class my_app : public coel::application {
  public:
    my_app() : coel::application({800, 600}, "simple game") {
        use_raw_mouse(true);
    }

    void on_draw() {
        is_paused ? glClearColor(0.2f, 0.2f, 0.2f, 1.0f) : glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void on_pause() { set_mouse_capture(false); }
    void on_resume() { set_mouse_capture(true); }
};

int main() {
    my_app game;
    if (!game)
        return -1;
    game.resize();
    while (game.update()) {
    }
}
