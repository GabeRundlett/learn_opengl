#pragma once

#include <exception>

namespace coel {
    class exception : public std::exception {
      public:
        exception(const char *const message) : std::exception(message) {}
    };
} // namespace coel
