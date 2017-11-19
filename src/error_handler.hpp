#pragma once
#ifndef SK_IMPL_ERROR_HANDLER_HPP
#define SK_IMPL_ERROR_HANDLER_HPP

#include <string>

#include <boost/core/demangle.hpp>
#include <boost/spirit/home/support/iterators/line_pos_iterator.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/spirit/home/x3/support/utility/error_reporting.hpp>

namespace sk::impl {

namespace x3 = boost::spirit::x3;

// tag used to get our error handler from the context
struct error_handler_tag;

template <typename Iterator>
using error_handler = typename x3::error_handler<Iterator>;

struct error_handler_base {
    template <typename Iterator, typename Exception, typename Context>
    x3::error_handler_result
    on_error(
        Iterator&, Iterator const&, Exception const& x, Context const& context)
    {
        std::string message = "error! expecting: " +
                              boost::core::demangle(x.which().c_str()) +
                              " here: ";
        auto& error_handler = x3::get<error_handler_tag>(context).get();
        error_handler(x.where(), message);
        return x3::error_handler_result::fail;
    }
};

template <typename StringType>
using iterator_t = typename boost::spirit::line_pos_iterator<
    typename StringType::const_iterator>;

template <typename SpaceType>
using phrase_context_t = typename x3::phrase_parse_context<SpaceType>::type;

template <typename StringType>
using error_handler_t = x3::error_handler<iterator_t<StringType>>;

template <typename StringType, typename SpaceType>
using context_t = typename x3::with_context<
    error_handler_tag,
    std::reference_wrapper<error_handler_t<StringType>> const,
    phrase_context_t<SpaceType>>::type;
}

#endif // SK_IMPL_ERROR_HANDLER_HPP
