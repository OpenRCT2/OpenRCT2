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

#ifndef SFL_DETAIL_TYPE_TRAITS_HPP_INCLUDED
#define SFL_DETAIL_TYPE_TRAITS_HPP_INCLUDED

#include <iterator>
#include <type_traits>

namespace sfl
{

namespace dtl
{

template <typename...>
using void_t = void;

template <bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template <typename T>
using remove_cvref_t = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

//
// This struct provides information about segmented iterators.
//
// The architecture about segmented iterator traits is based on this article:
// "Segmented Iterators and Hierarchical Algorithms", Matthew H. Austern.
//
template <typename T>
struct segmented_iterator_traits
{
    using is_segmented_iterator = std::false_type;

    //
    // Specialized struct must define the following types and functions:
    //
    // using iterator         = xxxxx; (it is usually `T`)
    // using segment_iterator = xxxxx;
    // using local_iterator   = xxxxx;
    //
    // static segment_iterator segment(iterator);
    // static local_iterator   local(iterator);
    //
    // static local_iterator begin(segment_iterator);
    // static local_iterator end(segment_iterator);
    //
    // static iterator compose(segment_iterator, local_iterator);
    //
};

//
// Checks if `T` is segmented iterator.
//
template <typename T>
struct is_segmented_iterator :
    sfl::dtl::segmented_iterator_traits<T>::is_segmented_iterator {};

//
// Checks if `T` is input iterator.
//
template <typename Iterator, typename = void>
struct is_input_iterator : std::false_type {};

template <typename Iterator>
struct is_input_iterator<
    Iterator,
    sfl::dtl::enable_if_t<
        std::is_convertible<
            typename std::iterator_traits<Iterator>::iterator_category,
            std::input_iterator_tag
        >::value
    >
> : std::true_type {};

//
// Checks if `T` is exactly input iterator.
//
template <typename T, typename = void>
struct is_exactly_input_iterator : std::false_type {};

template <typename T>
struct is_exactly_input_iterator<
    T,
    sfl::dtl::enable_if_t<
        std::is_convertible<
            typename std::iterator_traits<T>::iterator_category,
            std::input_iterator_tag
        >::value
        &&
       !std::is_convertible<
            typename std::iterator_traits<T>::iterator_category,
            std::forward_iterator_tag
        >::value
    >
> : std::true_type {};

//
// Checks if `T` is forward iterator.
//
template <typename T, typename = void>
struct is_forward_iterator : std::false_type {};

template <typename T>
struct is_forward_iterator<
    T,
    sfl::dtl::enable_if_t<
        std::is_convertible<
            typename std::iterator_traits<T>::iterator_category,
            std::forward_iterator_tag
        >::value
    >
> : std::true_type {};

//
// Checks if `T` is random access iterator.
//
template <typename T, typename = void>
struct is_random_access_iterator : std::false_type {};

template <typename T>
struct is_random_access_iterator<
    T,
    sfl::dtl::enable_if_t<
        std::is_convertible<
            typename std::iterator_traits<T>::iterator_category,
            std::random_access_iterator_tag
        >::value
    >
> : std::true_type {};

//
// Checks if `Type` has member `is_transparent`.
//
template <typename Type, typename SfinaeType, typename = void>
struct has_is_transparent : std::false_type {};

template <typename Type, typename SfinaeType>
struct has_is_transparent<
    Type, SfinaeType, sfl::dtl::void_t<typename Type::is_transparent>
> : std::true_type {};

} // namespace dtl

} // namespace sfl

#endif // SFL_DETAIL_TYPE_TRAITS_HPP_INCLUDED
