#pragma once
#ifndef SKETCH_MAIN_HEADER_HPP
#define SKETCH_MAIN_HEADER_HPP

#include <string_view>

#include <sketch/application.hpp>
#include <sketch/window.hpp>

namespace sk {

window_t load_sketch(std::string_view filename);
}

#endif // SKETCH_MAIN_HEADER_HPP
