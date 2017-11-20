#pragma once
#ifndef SK_REACTOR_HPP
#define SK_REACTOR_HPP

#include <cstdint>
#include <functional>

namespace sk {

class reactor_t {
    std::function<void()>            _on_draw;
    std::function<void()>            _on_quit;
    std::function<void(std::size_t)> _on_keydown;
    std::function<void(const std::tuple<std::size_t, std::size_t>&)>
        _on_mouse_move;

public:
    reactor_t& operator=(const reactor_t&) = delete;
    reactor_t& operator=(reactor_t&&) = default;
    reactor_t();
    reactor_t(const reactor_t&) = delete;
    reactor_t(reactor_t&&)      = default;
    ~reactor_t()                = default;

    void on_draw();
    void on_quit();
    void on_keydown(std::size_t);
    void on_mouse_move(const std::tuple<std::size_t, std::size_t>&);
};
}

#endif // SK_REACTOR_HPP
