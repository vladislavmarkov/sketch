#pragma once
#ifndef SK_REACTOR_HPP
#define SK_REACTOR_HPP

#include <cstdint>
#include <functional>
#include <tuple>
#include <type_traits>

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

    template <typename FuncType>
    void
    set_on_draw(FuncType&& draw_func)
    {
        static_assert(std::is_invocable_v<FuncType>);
        _on_draw = std::forward<FuncType>(draw_func);
    }

    template <typename FuncType>
    void
    set_on_quit(FuncType&& quit_func)
    {
        static_assert(std::is_invocable_v<FuncType>);
        _on_quit = std::forward<FuncType>(quit_func);
    }

    template <typename FuncType>
    void
    set_on_keydown(FuncType&& keydown_func)
    {
        static_assert(std::is_invocable_v<FuncType, std::size_t>);
        _on_keydown = std::forward<FuncType>(keydown_func);
    }

    template <typename FuncType>
    void
    set_on_mouse_move(FuncType&& mouse_move_func)
    {
        static_assert(
            std::is_invocable_v<FuncType,
                                const std::tuple<std::size_t, std::size_t>&>);
        _on_mouse_move = std::forward<FuncType>(mouse_move_func);
    }
};
}

#endif // SK_REACTOR_HPP
