#include <sketch/window.hpp>

#include <SDL.h>

namespace sk {

window_t::window_t(
    std::string_view title,
    const std::tuple<std::size_t, std::size_t, std::size_t, std::size_t>&
        boundaries)
    : _window(
          SDL_CreateWindow(
              title.data(),
              static_cast<int>(std::get<0>(boundaries)), // x
              static_cast<int>(std::get<1>(boundaries)), // y
              static_cast<int>(std::get<2>(boundaries)), // w
              static_cast<int>(std::get<3>(boundaries)), // h
              SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP |
                  SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_INPUT_GRABBED |
                  SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_SHOWN),
          [](SDL_Window* ptr) { SDL_DestroyWindow(ptr); })
{
    if (!_window) {
        throw std::runtime_error(SDL_GetError());
    }
}

window_t::~window_t() = default;

window_t::operator SDL_Window*() { return _window.get(); }
void
window_t::reactor(reactor_t&& reactor)
{
    _reactor = std::move(reactor);
}

reactor_t&
window_t::reactor()
{
    return _reactor;
}
}
