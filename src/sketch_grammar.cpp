#include "sketch_grammar.hpp"

#include <fstream>
#include <locale>

#include <boost/spirit/home/x3.hpp>

#include "window.hpp"

namespace sk {

namespace x3 = boost::spirit::x3;

namespace {

std::wstring
read_file(const std::string& filename)
{
    std::wifstream file(filename);
    file.imbue(std::locale("en_US.UTF-8"));
    if (!file) throw std::runtime_error("failed to open file");
    return {std::istreambuf_iterator<wchar_t>(file),
            (std::istreambuf_iterator<wchar_t>())};
}

const auto title       = x3::rule<struct title, std::string>{"title"};
const auto title_def   = x3::lexeme['"' >> +(x3::char_ - '"') >> '"'];
const auto percent     = x3::rule<struct percent, std::string>{"percent"};
const auto percent_def = x3::raw[+x3::digit > '%'];
const auto width       = x3::rule<struct width, std::string>{"width"};
const auto width_def =
    x3::lit("width") > '=' >
    percent[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })];
const auto height = x3::rule<struct height, std::string>{"height"};
const auto height_def =
    x3::lit("height") > '=' >
    percent[([](auto& ctx) { x3::_val(ctx) = x3::_attr(ctx); })];
const auto centered = x3::rule<struct centered, bool>{"centered"};
const auto centered_def =
    x3::lit("centered")[([](auto& ctx) { x3::_val(ctx) = true; })];
const auto attribute = x3::rule<struct attribute,
                                std::tuple<std::optional<std::string>,
                                           std::optional<std::string>,
                                           std::optional<bool>>>{"attribute"};
const auto attribute_def =
    width[([](auto& ctx) { std::get<0>(x3::_val(ctx)) = x3::_attr(ctx); })] |
    height[([](auto& ctx) { std::get<1>(x3::_val(ctx)) = x3::_attr(ctx); })] |
    centered[([](auto& ctx) { std::get<2>(x3::_val(ctx)) = x3::_attr(ctx); })];
const auto window = x3::rule<struct window, sk::window_t>{"window"};
const auto window_def =
    x3::lit("window") > '=' >
    title[([](auto& ctx) { x3::_val(ctx).title = x3::_attr(ctx); })] > ':' >
    +attribute[([](auto& ctx) {
        if (std::get<0>(x3::_attr(ctx))) {
            x3::_val(ctx).width = *std::get<0>(x3::_attr(ctx));
        }

        if (std::get<1>(x3::_attr(ctx))) {
            x3::_val(ctx).height = *std::get<1>(x3::_attr(ctx));
        }

        if (std::get<2>(x3::_attr(ctx))) {
            x3::_val(ctx).centered = *std::get<2>(x3::_attr(ctx));
        }
    })];

// window parsing rules
BOOST_SPIRIT_DEFINE(attribute, centered, height, percent, title, width, window)

const auto skipper     = x3::rule<struct skipper>{"skipper"};
const auto skipper_def = x3::space | "/*" >> *(x3::char_ - "*/") >> "*/" |
                         "//" >> *(x3::char_ - x3::eol - x3::eoi);

BOOST_SPIRIT_DEFINE(skipper) // comments
}

void
create_sketch(const std::string& filename)
{
    const auto input = read_file(filename);
    auto       first = std::cbegin(input), last = std::cend(input);
    window_t   win;
    if (x3::phrase_parse(first, last, window, skipper, win) && first == last) {
        std::cout << "parsed successful" << std::endl;
        win.print();
    }
}
}
