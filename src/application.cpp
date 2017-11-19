#include <sketch/application.hpp>

#include <stdexcept>

#include <SDL.h>

#include "fps_ctl.hpp"

namespace sk {

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
    _windows.push_back(std::move(window));
}

int
application_t::run()
{
    // fps controller
    impl::fps_ctl_t fps_ctl;

    // application loop
    while (is_running()) {
        handle_events(_reactor.get());
        for (auto& window : _windows) {
            window.on_draw_frame();
        }
        fps_ctl.update();
    }

    return EXIT_SUCCESS;
}
}
