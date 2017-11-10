#include "sketch_grammar.hpp"

#include <cstdint>
#include <fstream>
#include <locale>
#include <variant>

#include <boost/spirit/home/x3.hpp>

#include "window.hpp"

namespace sk {

namespace ast {
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
}

namespace x3 = boost::spirit::x3;

namespace {

class window_ast {
    std::string       _title;
    ast::width_t      _width;
    ast::height_t     _height;
    ast::position_t   _position;
    ast::fullscreen_t _fullscreen;

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
            } else if (std::holds_alternative<ast::percent_t>(*_width)) {
                const auto width_percent = std::get<ast::percent_t>(*_width);
                width_str                = std::to_string(
                                static_cast<std::size_t>(width_percent * 100)) +
                            "%";
            } else {
                width_str =
                    std::to_string(std::get<ast::pixels_t>(*_width)) + "px";
            }
        }
        std::cout << "\twidth = " << width_str << "\n";

        std::string height_str = "unspecified";
        if (_height) {
            if (std::holds_alternative<bool>(*_height)) {
                height_str = "screen-high";
            } else if (std::holds_alternative<ast::percent_t>(*_height)) {
                const auto height_percent = std::get<ast::percent_t>(*_height);
                height_str =
                    std::to_string(
                        static_cast<std::size_t>(height_percent * 100)) +
                    "%";
            } else {
                height_str =
                    std::to_string(std::get<ast::pixels_t>(*_height)) + "px";
            }
        }
        std::cout << "\theight = " << height_str << "\n";

        std::string position_str = "unspecified";
        if (_position) {
            if (std::holds_alternative<bool>(*_position)) {
                position_str = "centered";
            } else {
                const auto pos = std::get<ast::point_t>(*_position);
                if (std::holds_alternative<bool>(pos.first) &&
                    std::holds_alternative<bool>(pos.second)) {
                    position_str = "centered";
                } else {
                    position_str = "position = { ";
                    if (std::holds_alternative<bool>(pos.first)) {
                        position_str += "h-centered";
                    } else if (
                        std::holds_alternative<ast::percent_t>(pos.first)) {
                        const auto h_percent =
                            std::get<ast::percent_t>(pos.first);
                        position_str +=
                            std::to_string(
                                static_cast<std::size_t>(h_percent * 100)) +
                            "%";
                    } else {
                        position_str +=
                            std::to_string(std::get<ast::pixels_t>(pos.first)) +
                            "px";
                    }

                    position_str += ", ";

                    if (std::holds_alternative<bool>(pos.second)) {
                        position_str += "v-centered }";
                    } else if (
                        std::holds_alternative<ast::percent_t>(pos.second)) {
                        const auto v_percent =
                            std::get<ast::percent_t>(pos.second);
                        position_str +=
                            std::to_string(
                                static_cast<std::size_t>(v_percent * 100)) +
                            "%";
                    } else {
                        position_str +=
                            std::to_string(
                                std::get<ast::pixels_t>(pos.second)) +
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

std::wstring
read_file(const std::string& filename)
{
    std::wifstream file(filename);
    file.imbue(std::locale("en_US.UTF-8"));
    if (!file) {
        throw std::runtime_error("failed to open file");
    }
    return {std::istreambuf_iterator<wchar_t>(file),
            (std::istreambuf_iterator<wchar_t>())};
}

struct error_handler_tag;

struct error_handler_base {
    template <typename Iterator, typename Exception, typename Context>
    x3::error_handler_result
    on_error(
        Iterator& /*first*/,
        Iterator const& /*last*/,
        Exception const& x,
        Context const&   context)
    {
        std::string message_ = "error! expecting: " + x.which() + " here: ";
        auto&       error_handler = x3::get<error_handler_tag>(context).get();
        error_handler(x.where(), message_);
        return x3::error_handler_result::fail;
    }
};

struct window_class;

const auto skipper     = x3::rule<struct skipper>{"skipper"};
const auto skipper_def = x3::space | "/*" >> *(x3::char_ - "*/") >> "*/" |
                         "//" >> *(x3::char_ - x3::eol - x3::eoi);

const auto line_ending     = x3::rule<struct line_ending>{"line_ending"};
const auto line_ending_def = x3::no_skip[x3::skip(skipper - x3::eol)[x3::eol]];

const auto single_quoted_string =
    x3::rule<struct single_quoted_string, std::string>{"single_quoted_string"};
const auto single_quoted_string_def =
    x3::lexeme['\'' >> +(x3::char_ - '\'') >> '\''];

const auto double_quoted_string =
    x3::rule<struct double_quoted_string, std::string>{"double_quoted_string"};
const auto double_quoted_string_def =
    x3::lexeme['"' >> +(x3::char_ - '"') >> '"'];

const auto quoted_string =
    x3::rule<struct quoted_string, std::string>{"quoted_string"};
const auto quoted_string_def = single_quoted_string | double_quoted_string;

const auto title     = x3::rule<struct title, std::string>{"title"};
const auto title_def = quoted_string;

const auto centered = x3::rule<struct centered, bool>{"centered"};
const auto centered_def =
    x3::lit("centered")[([](auto& ctx) { x3::_val(ctx) = true; })];

const auto percent = x3::rule<struct percent, ast::percent_t>{"percent"};
const auto percent_def =
    (x3::uint_[([](auto& ctx) {
         x3::_val(ctx) = static_cast<ast::percent_t>(x3::_attr(ctx)) / 100.0;
     })] >>
     '%');

const auto pixels     = x3::rule<struct pixels, ast::pixels_t>{"pixels"};
const auto pixels_def = x3::uint_[([](auto& ctx) {
                            x3::_val(ctx) =
                                static_cast<ast::pixels_t>(x3::_attr(ctx));
                        })] >>
                        "px";

const auto full = x3::rule<struct full, bool>{"full"};
const auto full_def =
    x3::lit("full")[([](auto& ctx) { x3::_val(ctx) = true; })];

const auto width = x3::rule<struct width, ast::width_t>{"width"};
const auto
    width_def = x3::lit("width") > '=' >
                (percent[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })] |
                 pixels[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })] |
                 full[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })]);

const auto height = x3::rule<struct height, ast::height_t>{"height"};
const auto
    height_def = x3::lit("height") > '=' >
                 (percent[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })] |
                  pixels[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })] |
                  full[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })]);

const auto horizontal =
    x3::rule<struct horizontal, ast::horizontal_t>{"horizontal"};
const auto horizontal_def =
    centered[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })] |
    percent[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })] |
    pixels[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })];

const auto vertical = x3::rule<struct vertical, ast::vertical_t>{"vertical"};
const auto vertical_def =
    centered[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })] |
    percent[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })] |
    pixels[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })];

const auto point = x3::rule<struct point, ast::point_t>{"point"};
const auto point_def =
    x3::lit("position") > '=' >
    horizontal[([](auto& ctx) { x3::_val(ctx).first = x3::_attr(ctx); })] >
    ',' > vertical[([](auto& ctx) { x3::_val(ctx).second = x3::_attr(ctx); })];

const auto position = x3::rule<struct position, ast::position_t>{"position"};
const auto position_def = centered | point;

const auto fullscreen =
    x3::rule<struct fullscreen, ast::fullscreen_t>{"fullscreen"};
const auto fullscreen_def =
    x3::lit("fullscreen")[([](auto& ctx) { x3::_val(ctx) = true; })];

const auto attribute = x3::rule<
    struct attribute,
    std::
        tuple<ast::width_t, ast::height_t, ast::position_t, ast::fullscreen_t>>{
    "attribute"};

const auto attribute_def =
    width[([](auto& ctx) { std::get<0>(x3::_val(ctx)) = x3::_attr(ctx); })] |
    height[([](auto& ctx) { std::get<1>(x3::_val(ctx)) = x3::_attr(ctx); })] |
    position[([](auto& ctx) { std::get<2>(x3::_val(ctx)) = x3::_attr(ctx); })] |
    fullscreen[(
        [](auto& ctx) { std::get<3>(x3::_val(ctx)) = x3::_attr(ctx); })];

const auto window = x3::rule<struct window_class, window_ast>{"window"};

const auto window_def =
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

// window parsing rules
BOOST_SPIRIT_DEFINE(
    attribute,
    centered,
    double_quoted_string,
    full,
    fullscreen,
    height,
    horizontal,
    line_ending,
    percent,
    pixels,
    point,
    position,
    quoted_string,
    single_quoted_string,
    skipper,
    title,
    vertical,
    width,
    window)

struct window_class {
    // error handler
    template <typename Iterator, typename Exception, typename Context>
    x3::error_handler_result
    on_error(
        Iterator& /* first */,
        Iterator const&  last,
        Exception const& x,
        Context const& /* ctx */)
    {
        std::cerr << "error! expecting: " << x.which() << R"( here: ")"
                  << std::string(x.where(), last) << "\"\n";
        return x3::error_handler_result::fail;
    }
};
}

void
create_sketch(const std::string& filename)
{
    const auto input = read_file(filename);
    auto       first = std::cbegin(input), last = std::cend(input);
    window_ast win_ast;
    if (x3::phrase_parse(first, last, window, skipper, win_ast) &&
        first == last) {
        std::cout << "parsed successful" << std::endl;
        win_ast.print();
    } else {
        std::wcerr << R"(failed to parse: ")" << std::wstring(first, last)
                   << "\"\n";
    }
}
}
