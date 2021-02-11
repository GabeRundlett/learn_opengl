#include "app.hpp"

int main() {
    game_app game;

    if (!game.glfw.window)
        return -1;

    game.resize();
    while (game.update()) {
    }
}
