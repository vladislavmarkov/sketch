#pragma once
#ifndef SK_IMPL_SDL2_DISPLAY_HPP
#define SK_IMPL_SDL2_DISPLAY_HPP

#include <cstdint>
#include <tuple>

namespace sk::impl::sdl2::display {

std::tuple<std::size_t, std::size_t, std::size_t, std::size_t>
get_bounds(int display_index = 0);
}

#endif // SK_IMPL_SDL2_DISPLAY_HPP
