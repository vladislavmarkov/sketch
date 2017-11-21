#pragma once
#ifndef SK_APPLICATION_HPP
#define SK_APPLICATION_HPP

#include <sketch/window.hpp>

namespace sk {

class application_t final {
    std::vector<window_t> _windows;
    bool                  _running = {true};

public:
    application_t& operator=(const application_t&) = delete;
    application_t& operator=(application_t&&) = delete;
    application_t(const application_t&)       = delete;
    application_t(application_t&&)            = delete;

    application_t();
    ~application_t();

    void add(window_t&&);
    int  run();
    void quit();
    bool is_running() const;
};
}

#endif // SK_APPLICATION_HPP
