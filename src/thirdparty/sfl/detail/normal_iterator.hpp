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

#ifndef SFL_DETAIL_NORMAL_ITERATOR_HPP_INCLUDED
#define SFL_DETAIL_NORMAL_ITERATOR_HPP_INCLUDED

#include <sfl/detail/cpp.hpp>
#include <sfl/detail/to_address.hpp>
#include <sfl/detail/type_traits.hpp>

#include <iterator>
#include <type_traits>

namespace sfl
{

namespace dtl
{

template <typename Iterator, typename Container>
class normal_iterator
{
    template <typename, typename>
    friend class normal_iterator;

    friend Container;

private:

    Iterator it_;

public:

    using difference_type   = typename std::iterator_traits<Iterator>::difference_type;
    using value_type        = typename std::iterator_traits<Iterator>::value_type;
    using pointer           = typename std::iterator_traits<Iterator>::pointer;
    using reference         = typename std::iterator_traits<Iterator>::reference;
    using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
#if SFL_CPP_VERSION >= SFL_CPP_20
    using iterator_concept  = std::contiguous_iterator_tag;
#endif

private:

    explicit normal_iterator(const Iterator& it) noexcept
        : it_(it)
    {}

public:

    // Default constructor
    normal_iterator() noexcept
        : it_()
    {}

    // Copy constructor
    normal_iterator(const normal_iterator& other) noexcept
        : it_(other.it_)
    {}

    // Converting constructor (from iterator to const_iterator)
    template <typename OtherIterator,
              sfl::dtl::enable_if_t<std::is_convertible<OtherIterator, Iterator>::value>* = nullptr>
    normal_iterator(const normal_iterator<OtherIterator, Container>& other) noexcept
        : it_(other.it_)
    {}

    // Copy assignment operator
    normal_iterator& operator=(const normal_iterator& other) noexcept
    {
        it_ = other.it_;
        return *this;
    }

    SFL_NODISCARD
    reference operator*() const noexcept
    {
        return *it_;
    }

    SFL_NODISCARD
    pointer operator->() const noexcept
    {
        return sfl::dtl::to_address(it_);
    }

    normal_iterator& operator++() noexcept
    {
        ++it_;
        return *this;
    }

    normal_iterator operator++(int) noexcept
    {
        auto temp = *this;
        ++it_;
        return temp;
    }

    normal_iterator& operator--() noexcept
    {
        --it_;
        return *this;
    }

    normal_iterator operator--(int) noexcept
    {
        auto temp = *this;
        --it_;
        return temp;
    }

    normal_iterator& operator+=(difference_type n) noexcept
    {
        it_ += n;
        return *this;
    }

    normal_iterator& operator-=(difference_type n) noexcept
    {
        it_ -= n;
        return *this;
    }

    SFL_NODISCARD
    normal_iterator operator+(difference_type n) const noexcept
    {
        return normal_iterator(it_ + n);
    }

    SFL_NODISCARD
    normal_iterator operator-(difference_type n) const noexcept
    {
        return normal_iterator(it_ - n);
    }

    SFL_NODISCARD
    reference operator[](difference_type n) const noexcept
    {
        return it_[n];
    }

    SFL_NODISCARD
    friend normal_iterator operator+(difference_type n, const normal_iterator& it) noexcept
    {
        return it + n;
    }

    SFL_NODISCARD
    friend difference_type operator-(const normal_iterator& x, const normal_iterator& y) noexcept
    {
        return x.it_ - y.it_;
    }

    SFL_NODISCARD
    friend bool operator==(const normal_iterator& x, const normal_iterator& y) noexcept
    {
        return x.it_ == y.it_;
    }

    SFL_NODISCARD
    friend bool operator!=(const normal_iterator& x, const normal_iterator& y) noexcept
    {
        return !(x == y);
    }

    SFL_NODISCARD
    friend bool operator<(const normal_iterator& x, const normal_iterator& y) noexcept
    {
        return x.it_ < y.it_;
    }

    SFL_NODISCARD
    friend bool operator>(const normal_iterator& x, const normal_iterator& y) noexcept
    {
        return y < x;
    }

    SFL_NODISCARD
    friend bool operator<=(const normal_iterator& x, const normal_iterator& y) noexcept
    {
        return !(y < x);
    }

    SFL_NODISCARD
    friend bool operator>=(const normal_iterator& x, const normal_iterator& y) noexcept
    {
        return !(x < y);
    }
};

} // namespace dtl

} // namespace sfl

#endif // SFL_DETAIL_NORMAL_ITERATOR_HPP_INCLUDED
