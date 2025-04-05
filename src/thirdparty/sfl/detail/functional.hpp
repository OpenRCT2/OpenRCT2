//
// Copyright (c) 2022 Slaven Falandys
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#ifndef SFL_DETAIL_FUNCTIONAL_HPP_INCLUDED
#define SFL_DETAIL_FUNCTIONAL_HPP_INCLUDED

#include <sfl/detail/cpp.hpp>

namespace sfl
{

namespace dtl
{

// Function object type whose operator() returns its argument unchanged.
struct identity
{
    template <typename T>
    SFL_NODISCARD
    T& operator()(T& t) const noexcept
    {
        return t;
    }

    template <typename T>
    SFL_NODISCARD
    const T& operator()(const T& t) const noexcept
    {
        return t;
    }
};

// Function object type whose operator() returns first element of pair unchanged.
struct first
{
    template <typename Pair>
    SFL_NODISCARD
    typename Pair::first_type& operator()(Pair& p) const noexcept
    {
        return p.first;
    }

    template <typename Pair>
    SFL_NODISCARD
    const typename Pair::first_type& operator()(const Pair& p) const noexcept
    {
        return p.first;
    }
};

} // namespace dtl

} // namespace sfl

#endif // SFL_DETAIL_FUNCTIONAL_HPP_INCLUDED
