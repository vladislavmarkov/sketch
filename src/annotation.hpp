#pragma once
#ifndef SK_IMPL_ANNOTATION_HPP
#define SK_IMPL_ANNOTATION_HPP

#include <boost/spirit/home/x3/support/utility/error_reporting.hpp>

namespace sk::impl {

namespace x3 = boost::spirit::x3;

struct error_handler_tag;

struct annotation_base {
    template <typename T, typename Iterator, typename Context>
    inline void
    on_success(
        Iterator const& first,
        Iterator const& last,
        T&              ast,
        Context const&  context)
    {
        auto& error_handler = x3::get<error_handler_tag>(context).get();
        error_handler.tag(ast, first, last);
    }
};
}

#endif // SK_IMPL_ANNOTATION_HPP
