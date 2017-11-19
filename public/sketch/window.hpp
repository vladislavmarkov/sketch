#pragma once
#ifndef SK_WINDOW_HPP
#define SK_WINDOW_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

struct SDL_Window;

namespace sk {

class window_t {
    std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>> _window;

public:
    window_t& operator=(const window_t&) = delete;
    window_t& operator=(window_t&&) = default;
    window_t(const window_t&)       = delete;
    window_t(window_t&&)            = default;

    window_t(
        const std::string& title,
        const std::tuple<std::size_t, std::size_t, std::size_t, std::size_t>&
            boundaries);
    ~window_t();

    operator SDL_Window*();
};
}

#endif // SK_WINDOW_HPP
