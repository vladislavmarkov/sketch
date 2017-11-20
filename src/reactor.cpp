#include <iostream>

#include <sketch/reactor.hpp>

namespace sk {

namespace {

void
default_on_draw()
{
    // do nothing
    std::cout << __FUNCTION__ << '\n';
}

void
default_on_quit()
{
    // do nothing
    std::cout << __FUNCTION__ << '\n';
}

void default_on_keydown([[maybe_unused]] std::size_t keycode)
{
    // do nothing
    std::cout << __FUNCTION__ << '\n';
}

void default_on_mouse_move(
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
    _on_draw();
}

void
reactor_t::on_quit()
{
    _on_quit();
}

void
reactor_t::on_keydown(std::size_t keycode)
{
    _on_keydown(keycode);
}

void
reactor_t::on_mouse_move(const std::tuple<std::size_t, std::size_t>& point)
{
    _on_mouse_move(point);
}
}
