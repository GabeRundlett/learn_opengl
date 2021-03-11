#pragma once

#include <glm/glm.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace coel {
    struct mouse_button_event {
        int button;
        int action;
        int mods;
    };
    struct key_event {
        int key;
        int scancode;
        int action;
        int mods;
    };
} // namespace coel
