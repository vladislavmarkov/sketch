#include <sketch.hpp>

#include <cstdint>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <locale>
#include <stdexcept>
#include <variant>

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/spirit/home/x3/support/utility/error_reporting.hpp>

#include <sketch/window.hpp>

#include "annotation.hpp"
#include "error_handler.hpp"

namespace sk {

using namespace std::literals::string_literals;

namespace fs = std::experimental::filesystem;
namespace x3 = boost::spirit::x3;

namespace {
using percent_t    = double;              // in 1/100ths
using pixels_t     = std::size_t;         // self-explanatory
using fullscreen_t = std::optional<bool>; // if true, then window is
                                          // fullscreened

/* window width type can be unspecified or specified to be screen-wide, or
 * specified in percent or specified in pixels
 */
using width_t = std::optional<std::variant<bool, percent_t, pixels_t>>;

/* window height type can be unspecified or specified to be screen-high, or
 * specified in percent or specified in pixels
 */
using height_t = std::optional<std::variant<bool, percent_t, pixels_t>>;

/* horizontal window coordinates can be represented by pixels or by percents of
 * screen, or window can be simply centered horizontally
 */
using horizontal_t = std::variant<bool, percent_t, pixels_t>;

/* vertical window coordinates can be represented by pixels or by percents of
 * screen, or window can be simply centered vertically
 */
using vertical_t = std::variant<bool, percent_t, pixels_t>;

// point for window is represented by a pair of window coordinates
using point_t = std::pair<horizontal_t, vertical_t>;

/* window position is represented by a point or window can be simply
 * centered both horizontally and vertically */
using position_t = std::optional<std::variant<bool, point_t>>;

class window_ast {
    std::string  _title;
    width_t      _width;
    height_t     _height;
    position_t   _position;
    fullscreen_t _fullscreen;

public:
    bool
    set_title(const std::string& t)
    {
        if (!_title.empty()) {
            std::cerr << "title is already set\n";
            return false;
        }

        _title = t;
        return true;
    }

    template <typename Width>
    bool
    set_width(const Width& p)
    {
        if (_width) {
            std::cerr << "width is already set\n";
            return false;
        }

        if (_fullscreen) {
            std::cerr << "you can't set window width since it's fullscreen\n";
            return false;
        }

        _width = p;
        return true;
    }

    template <typename Height>
    bool
    set_height(const Height& p)
    {
        if (_height) {
            std::cerr << "height is already set\n";
            return false;
        }

        if (_fullscreen) {
            std::cerr << "you can't set window height since it's fullscreen\n";
            return false;
        }

        _height = p;
        return true;
    }

    template <typename Position>
    bool
    set_position(const Position& p)
    {
        if (_position) {
            std::cerr << "position is already set\n";
            return false;
        }

        if (_fullscreen) {
            std::cerr << "you can't set window position since it's "
                         "fullscreen\n";
            return false;
        }

        _position = p;
        return true;
    }

    bool
    set_fullscreen()
    {
        if (_fullscreen) {
            std::cerr << "window is already set to fullscreen\n";
            return false;
        }

        if (_width || _height || _position) {
            std::cerr << "you can't set window to fullscreen as you already "
                         "set width, height or position attribute\n";
            return false;
        }

        _fullscreen = true;
        return true;
    }

    void
    print() try {
        std::cout << R"(window ")" << _title << "\"\n";

        if (_fullscreen) {
            std::cout << "\tfullscreen\n";
            return;
        }

        std::string width_str = "unspecified";
        if (_width) {
            if (std::holds_alternative<bool>(*_width)) {
                width_str = "screen-wide";
            } else if (std::holds_alternative<percent_t>(*_width)) {
                const auto width_percent = std::get<percent_t>(*_width);
                width_str                = std::to_string(
                                static_cast<std::size_t>(width_percent * 100)) +
                            "%";
            } else {
                width_str = std::to_string(std::get<pixels_t>(*_width)) + "px";
            }
        }
        std::cout << "\twidth = " << width_str << "\n";

        std::string height_str = "unspecified";
        if (_height) {
            if (std::holds_alternative<bool>(*_height)) {
                height_str = "screen-high";
            } else if (std::holds_alternative<percent_t>(*_height)) {
                const auto height_percent = std::get<percent_t>(*_height);
                height_str =
                    std::to_string(
                        static_cast<std::size_t>(height_percent * 100)) +
                    "%";
            } else {
                height_str = std::to_string(std::get<pixels_t>(*_height)) +
                             "p"
                             "x";
            }
        }
        std::cout << "\theight = " << height_str << "\n";

        std::string position_str = "unspecified";
        if (_position) {
            if (std::holds_alternative<bool>(*_position)) {
                position_str = "centered";
            } else {
                const auto pos = std::get<point_t>(*_position);
                if (std::holds_alternative<bool>(pos.first) &&
                    std::holds_alternative<bool>(pos.second)) {
                    position_str = "centered";
                } else {
                    position_str = "position = { ";
                    if (std::holds_alternative<bool>(pos.first)) {
                        position_str += "h-centered";
                    } else if (std::holds_alternative<percent_t>(pos.first)) {
                        const auto h_percent = std::get<percent_t>(pos.first);
                        position_str +=
                            std::to_string(
                                static_cast<std::size_t>(h_percent * 100)) +
                            "%";
                    } else {
                        position_str +=
                            std::to_string(std::get<pixels_t>(pos.first)) +
                            "p"
                            "x";
                    }

                    position_str += ", ";

                    if (std::holds_alternative<bool>(pos.second)) {
                        position_str += "v-centered }";
                    } else if (std::holds_alternative<percent_t>(pos.second)) {
                        const auto v_percent = std::get<percent_t>(pos.second);
                        position_str +=
                            std::to_string(
                                static_cast<std::size_t>(v_percent * 100)) +
                            "%";
                    } else {
                        position_str +=
                            std::to_string(std::get<pixels_t>(pos.second)) +
                            "px";
                    }

                    position_str += " }";
                }
            }
        }
        std::cout << '\t' << position_str << '\n';
    } catch (std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
        throw;
    }
};

auto skipper = x3::rule<struct skipper>("skipper") =
    x3::space | "/*" >> *(x3::char_ - "*/") >> "*/" |
    "//" >> *(x3::char_ - x3::eol - x3::eoi);

struct line_ending_tag : impl::error_handler_base, impl::annotation_base {
};
auto line_ending = x3::rule<line_ending_tag>("line_ending") =
    x3::no_skip[x3::skip(skipper - x3::eol)[x3::eol]];

struct single_quoted_string_tag : impl::error_handler_base,
                                  impl::annotation_base {
};
auto single_quoted_string =
    x3::rule<single_quoted_string_tag, std::string>("single_quoted_string") =
        '\'' >> x3::lexeme[*(~x3::char_('\''))] >> '\'';

struct double_quoted_string_tag : impl::error_handler_base,
                                  impl::annotation_base {
};
auto double_quoted_string =
    x3::rule<double_quoted_string_tag, std::string>("double_quoted_string") =
        '"' >> x3::lexeme[*(~x3::char_('"'))] >> '"';

struct quoted_string_tag : impl::error_handler_base, impl::annotation_base {
};
auto quoted_string = x3::rule<quoted_string_tag, std::string>("quoted_string") =
    single_quoted_string | double_quoted_string;

struct title_tag : impl::error_handler_base, impl::annotation_base {
};
auto title = x3::rule<title_tag, std::string>("title") = quoted_string;

struct centered_tag : impl::error_handler_base, impl::annotation_base {
};
auto centered = x3::rule<centered_tag, bool>("centered") =
    x3::lit("centered")[([](auto& ctx) { x3::_val(ctx) = true; })];

struct percent_tag : impl::error_handler_base, impl::annotation_base {
};
auto percent = x3::rule<percent_tag, percent_t>("percent") =
    (x3::uint_[([](auto& ctx) {
         x3::_val(ctx) = static_cast<percent_t>(x3::_attr(ctx)) / 100.0;
     })] >>
     '%');

struct pixels_tag : impl::error_handler_base, impl::annotation_base {
};
auto pixels = x3::rule<pixels_tag, pixels_t>{"pixels"} =
    x3::uint_[([](auto& ctx) {
        x3::_val(ctx) = static_cast<pixels_t>(x3::_attr(ctx));
    })] >>
    "px";

struct full_tag : impl::error_handler_base, impl::annotation_base {
};
auto full = x3::rule<full_tag, bool>{"full"} = x3::matches[x3::lit("full")];

struct width_tag : impl::error_handler_base, impl::annotation_base {
};
auto width = x3::rule<width_tag, width_t>{"width"} =
    x3::lit("width") > '=' >
    (percent[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })] |
     pixels[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })] |
     full[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })]);

struct height_tag : impl::error_handler_base, impl::annotation_base {
};
auto height = x3::rule<height_tag, height_t>{"height"} =
    x3::lit("height") > '=' >
    (percent[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })] |
     pixels[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })] |
     full[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })]);

struct horizontal_tag : impl::error_handler_base, impl::annotation_base {
};
auto horizontal = x3::rule<horizontal_tag, horizontal_t>{"horizontal"} =
    centered[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })] |
    percent[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })] |
    pixels[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })];

struct vertical_tag : impl::error_handler_base, impl::annotation_base {
};
auto vertical = x3::rule<vertical_tag, vertical_t>{"vertical"} =
    centered[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })] |
    percent[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })] |
    pixels[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })];

struct point_tag : impl::error_handler_base, impl::annotation_base {
};
auto point = x3::rule<point_tag, point_t>{"point"} =
    x3::lit("position") > '=' >
    horizontal[([](auto& ctx) { x3::_val(ctx).first = x3::_attr(ctx); })] >
    ',' > vertical[([](auto& ctx) { x3::_val(ctx).second = x3::_attr(ctx); })];

struct position_tag : impl::error_handler_base, impl::annotation_base {
};
auto position = x3::rule<position_tag, position_t>{"position"} =
    centered | point;

struct fullscreen_tag : impl::error_handler_base, impl::annotation_base {
};
auto fullscreen = x3::rule<fullscreen_tag, fullscreen_t>{"fullscreen"} =
    x3::lit("fullscreen")[([](auto& ctx) { x3::_val(ctx) = true; })];

struct attribute_tag : impl::error_handler_base, impl::annotation_base {
};
auto attribute = x3::rule<
    attribute_tag,
    std::tuple<width_t, height_t, position_t, fullscreen_t>>{"attribute"} =
    width[([](auto& ctx) { std::get<0>(x3::_val(ctx)) = x3::_attr(ctx); })] |
    height[([](auto& ctx) { std::get<1>(x3::_val(ctx)) = x3::_attr(ctx); })] |
    position[([](auto& ctx) { std::get<2>(x3::_val(ctx)) = x3::_attr(ctx); })] |
    fullscreen[(
        [](auto& ctx) { std::get<3>(x3::_val(ctx)) = x3::_attr(ctx); })];

struct window_tag : impl::error_handler_base, impl::annotation_base {
};
auto window = x3::rule<window_tag, window_ast>("window") =
    x3::lit("window") > '=' >
    title[([](auto& ctx) { x3::_val(ctx).set_title(x3::_attr(ctx)); })] > ':' >
    +(line_ending >> attribute[([](auto& ctx) {
          if (std::get<0>(x3::_attr(ctx))) {
              if (!x3::_val(ctx).set_width(std::get<0>(x3::_attr(ctx)))) {
                  x3::_pass(ctx) = false;
              };
          }
          if (std::get<1>(x3::_attr(ctx))) {
              if (!x3::_val(ctx).set_height(std::get<1>(x3::_attr(ctx)))) {
                  x3::_pass(ctx) = false;
              }
          }
          if (std::get<2>(x3::_attr(ctx))) {
              if (!x3::_val(ctx).set_position(std::get<2>(x3::_attr(ctx)))) {
                  x3::_pass(ctx) = false;
              }
          }
          if (std::get<3>(x3::_attr(ctx))) {
              if (!x3::_val(ctx).set_fullscreen()) {
                  x3::_pass(ctx) = false;
              }
          }
      })]);

std::wstring
read_file(std::string_view filename)
{
    std::wifstream file(filename.data());
    file.imbue(std::locale("en_US.UTF-8"));
    if (!file) {
        throw std::runtime_error("failed to open file");
    }
    return {std::istreambuf_iterator<wchar_t>(file),
            (std::istreambuf_iterator<wchar_t>())};
}
}

void
load_sketch(std::string_view filename)
{
    if (filename.empty()) {
        throw std::invalid_argument("filename is an empty string");
    }

    if (!fs::exists(filename)) {
        throw std::runtime_error("no such file");
    }

    auto                              input = read_file(filename);
    impl::iterator_t<decltype(input)> first(std::cbegin(input)),
        last(std::cend(input));
    window_ast                             win_ast = {};
    impl::error_handler_t<decltype(input)> error_handler(
        first, last, std::cerr);
    const auto parser =
        x3::with<impl::error_handler_tag>(std::ref(error_handler))[window];
    if (x3::phrase_parse(first, last, parser, skipper, win_ast) &&
        first == last) {
        win_ast.print();
    } else {
        std::wcerr << R"(failed to parse: ")" << std::wstring(first, last)
                   << "\"\n";
    }
}
}
