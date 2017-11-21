#include <sketch/reactor.hpp>

#include <iostream>

#include <sketch/window.hpp>

namespace sk {

namespace {

void
default_on_draw(gsl::not_null<window_t*>)
{
    // do nothing
    std::cout << __FUNCTION__ << '\n';
}

void
default_on_quit(gsl::not_null<window_t*> window)
{
    // do nothing
    std::cout << __FUNCTION__ << '\n';
    window->quit();
}

void
default_on_keydown(
    gsl::not_null<window_t*>, [[maybe_unused]] std::size_t keycode)
{
    // do nothing
    std::cout << __FUNCTION__ << '\n';
}

void
default_on_mouse_move(
    gsl::not_null<window_t*>,
    [[maybe_unused]] const std::tuple<std::size_t, std::size_t>& point)
{
    // do nothing
    std::cout << __FUNCTION__ << '\n';
}
}

reactor_t::reactor_t()
    : _on_draw(default_on_draw),
      _on_quit(default_on_quit),
      _on_keydown(default_on_keydown),
      _on_mouse_move(default_on_mouse_move)
{
}

void
reactor_t::on_draw()
{
    _on_draw(_window);
}

void
reactor_t::on_quit()
{
    _on_quit(_window);
}

void
reactor_t::on_keydown(std::size_t keycode)
{
    _on_keydown(_window, keycode);
}

void
reactor_t::on_mouse_move(const std::tuple<std::size_t, std::size_t>& point)
{
    _on_mouse_move(_window, point);
}
}
