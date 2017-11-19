#pragma once
#ifndef SK_WINDOW_HPP
#define SK_WINDOW_HPP

#include <string>

namespace sk {

struct window_t {
    std::string title;
    std::string width;
    std::string height;
    bool        centered = {false};
};
}

#endif // SK_WINDOW_HPP
