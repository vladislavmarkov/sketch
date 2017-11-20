#pragma once
#ifndef SK_WINDOW_HPP
#define SK_WINDOW_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>

#include <sketch/reactor.hpp>

struct SDL_Window;

namespace sk {

class window_t {
    std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>> _window;

    reactor_t _reactor;

public:
    window_t& operator=(const window_t&) = delete;
    window_t& operator=(window_t&&) = default;
    window_t(const window_t&)       = delete;
    window_t(window_t&&)            = default;

    window_t(
        std::string_view title,
        const std::tuple<std::size_t, std::size_t, std::size_t, std::size_t>&
            boundaries);
    ~window_t();

    operator SDL_Window*();

    reactor_t& reactor();
    void       reactor(reactor_t&&);
};
}

#endif // SK_WINDOW_HPP
