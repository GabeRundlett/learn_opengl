#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <coel/application.hpp>

class voxel_game : public coel::application {

  public:
    voxel_game() : coel::application({1200, 900}, "Voxel Game") {
    }
};

int main() try {
    voxel_game game;
    if (!game)
        return -1;
    game.resize();
    while (game.update()) {
    }
} catch (const coel::exception &e) {
    MessageBoxA(nullptr, e.what(), "Coel Exception", MB_OK);
}
