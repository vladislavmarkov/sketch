#include "sdl2_display.hpp"

#include <stdexcept>

#include <SDL.h>

namespace sk::impl::sdl2::display {

std::tuple<std::size_t, std::size_t, std::size_t, std::size_t>
get_bounds(int display_index)
{
    SDL_Rect result;
    if (!SDL_GetDisplayBounds(display_index, &result)) {
        return std::forward_as_tuple(result.x, result.y, result.w, result.h);
    }

    throw std::runtime_error(SDL_GetError());
}
}
