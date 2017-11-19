#include "fps_ctl.hpp"

#include <thread>

using namespace std::chrono_literals;

namespace sk::impl {

namespace {

constexpr std::size_t etalon_fps = {60};
}

void
fps_ctl_t::update()
{
    ++_frames;
    auto now = std::chrono::steady_clock::now();
    if (now - _last_update_time >= 1s) {
        _current_fps      = _frames;
        _frames           = 0;
        _last_update_time = now;
        _sleep_duration += (_current_fps > etalon_fps) ? 1ms : -1ms;
    }
    std::this_thread::sleep_for(_sleep_duration);
}

std::size_t
fps_ctl_t::get_fps() const
{
    return _current_fps;
}
}
