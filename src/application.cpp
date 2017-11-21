#include <sketch/application.hpp>

#include <stdexcept>

#include <SDL.h>

#include "fps_ctl.hpp"

namespace sk {

namespace {

void
handle_events(reactor_t& reactor)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT: reactor.on_quit(); break;
        case SDL_KEYDOWN: reactor.on_keydown(event.key.keysym.sym); break;
        case SDL_MOUSEMOTION:
            reactor.on_mouse_move(
                std::tuple{static_cast<std::size_t>(event.motion.x),
                           static_cast<std::size_t>(event.motion.y)});
            break;
        default: break;
        }
    }
}
}

application_t::application_t()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error(SDL_GetError());
    }
}

application_t::~application_t() { SDL_Quit(); }
void
application_t::add(window_t&& window)
{
    window._app = this;
    _windows.emplace_back(std::move(window));
}

int
application_t::run()
{
    // fps controller
    impl::fps_ctl_t fps_ctl;

    // application loop
    while (is_running()) {
        for (auto& window : _windows) {
            handle_events(window.reactor());
        }

        fps_ctl.update();
    }

    return EXIT_SUCCESS;
}

void
application_t::quit()
{
    _running = false;
}

bool
application_t::is_running() const
{
    return _running;
}
}
