#pragma once
#ifndef SK_WINDOW_HPP
#define SK_WINDOW_HPP

#include <iostream>
#include <string>

#include <boost/format.hpp>

namespace sk {

struct window_t {
    std::string title;
    std::string width;
    std::string height;
    bool        centered = {false};

    void
    print()
    {
        std::cout << boost::format(
                         "window\n\ttitle: \"%s\"\n\twidth: %s\n\theight: "
                         "%s\n\t%s\n") %
                         title % width % height %
                         (centered ? "centered" : "positioned");
    }
};
}

#endif // SK_WINDOW_HPP
