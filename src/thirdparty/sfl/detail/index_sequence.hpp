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

#ifndef SFL_DETAIL_INDEX_SEQUENCE_HPP_INCLUDED
#define SFL_DETAIL_INDEX_SEQUENCE_HPP_INCLUDED

#include <cstddef> // size_t

namespace sfl
{

namespace dtl
{

template <std::size_t... Ints>
struct index_sequence
{
    using type = index_sequence;
};

template <typename IndexSequence1, typename IndexSequence2>
struct index_sequence_concat;

template <std::size_t... Ints1, std::size_t... Ints2>
struct index_sequence_concat< sfl::dtl::index_sequence<Ints1...>,
                              sfl::dtl::index_sequence<Ints2...> >
    : sfl::dtl::index_sequence<Ints1..., (sizeof...(Ints1) + Ints2)...>
{};

template <std::size_t N>
struct make_index_sequence
    : sfl::dtl::index_sequence_concat< typename sfl::dtl::make_index_sequence<N/2>::type,
                                       typename sfl::dtl::make_index_sequence<N - N/2>::type >::type
{};

template <>
struct make_index_sequence<0> : sfl::dtl::index_sequence<>
{};

template <>
struct make_index_sequence<1> : sfl::dtl::index_sequence<0>
{};

template <typename... T>
using index_sequence_for = sfl::dtl::make_index_sequence<sizeof...(T)>;

} // namespace dtl

} // namespace sfl

#endif // SFL_DETAIL_INDEX_SEQUENCE_HPP_INCLUDED
