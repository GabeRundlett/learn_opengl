#include <simple_game/app.hpp>

int main() {
    game_app game;
    if (!game)
        return -1;
    game.resize();
    while (game.update()) {
    }
}
