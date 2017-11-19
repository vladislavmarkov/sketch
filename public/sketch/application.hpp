#pragma once
#ifndef SK_APPLICATION_HPP
#define SK_APPLICATION_HPP

#include <sketch/window.hpp>

namespace sk {

class application_t {
    std::vector<window_t> _windows;

public:
    application_t& operator=(const application_t&) = delete;
    application_t& operator=(application_t&&) = delete;
    application_t(const application_t&)       = delete;
    application_t(application_t&&)            = delete;

    application_t();
    ~application_t();

    void add(window_t&&);
    int run();
};
}

#endif // SK_APPLICATION_HPP
