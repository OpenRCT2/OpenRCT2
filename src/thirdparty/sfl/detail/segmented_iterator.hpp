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

#ifndef SFL_DETAIL_SEGMENTED_ITERATOR_HPP_INCLUDED
#define SFL_DETAIL_SEGMENTED_ITERATOR_HPP_INCLUDED

#include <sfl/detail/cpp.hpp>
#include <sfl/detail/to_address.hpp>
#include <sfl/detail/type_traits.hpp>

#include <cstddef>
#include <iterator>
#include <type_traits>

namespace sfl
{

namespace dtl
{

template <typename SegmentIterator, typename LocalIterator, std::size_t SegmentSize, typename Container>
class segmented_iterator
{
    template <typename, typename, std::size_t, typename>
    friend class segmented_iterator;

    friend Container;

    template <typename>
    friend struct sfl::dtl::segmented_iterator_traits;

private:

    SegmentIterator segment_;
    LocalIterator   local_;

public:

    using difference_type   = typename std::iterator_traits<LocalIterator>::difference_type;
    using value_type        = typename std::iterator_traits<LocalIterator>::value_type;
    using pointer           = typename std::iterator_traits<LocalIterator>::pointer;
    using reference         = typename std::iterator_traits<LocalIterator>::reference;
    using iterator_category = typename std::iterator_traits<LocalIterator>::iterator_category;

private:

    explicit segmented_iterator(const SegmentIterator& segment, const LocalIterator& local) noexcept
        : segment_(segment)
        , local_(local)
    {}

public:

    // Default constructor
    segmented_iterator() noexcept
        : segment_()
        , local_()
    {}

    // Copy constructor
    segmented_iterator(const segmented_iterator& other) noexcept
        : segment_(other.segment_)
        , local_(other.local_)
    {}

    // Converting constructor (from iterator to const_iterator)
    template <typename OtherSegmentIterator, typename OtherLocalIterator,
              sfl::dtl::enable_if_t< std::is_convertible<OtherSegmentIterator, SegmentIterator>::value &&
                                     std::is_convertible<OtherLocalIterator, LocalIterator>::value >* = nullptr >
    segmented_iterator(const segmented_iterator<OtherSegmentIterator, OtherLocalIterator, SegmentSize, Container>& other) noexcept
        : segment_(other.segment_)
        , local_(other.local_)
    {}

    // Copy assignment operator
    segmented_iterator& operator=(const segmented_iterator& other) noexcept
    {
        segment_ = other.segment_;
        local_ = other.local_;
        return *this;
    }

    SFL_NODISCARD
    reference operator*() const noexcept
    {
        return *local_;
    }

    SFL_NODISCARD
    pointer operator->() const noexcept
    {
        return sfl::dtl::to_address(local_);
    }

    segmented_iterator& operator++() noexcept
    {
        ++local_;

        if (local_ == *segment_ + SegmentSize)
        {
            ++segment_;
            local_ = *segment_;
        }

        return *this;
    }

    segmented_iterator operator++(int) noexcept
    {
        auto temp = *this;
        this->operator++();
        return temp;
    }

    segmented_iterator& operator--() noexcept
    {
        if (local_ == *segment_)
        {
            --segment_;
            local_ = *segment_ + SegmentSize;
        }

        --local_;

        return *this;
    }

    segmented_iterator operator--(int) noexcept
    {
        auto temp = *this;
        this->operator--();
        return temp;
    }

    segmented_iterator& operator+=(difference_type n) noexcept
    {
        const difference_type offset = std::distance(LocalIterator(*segment_), local_) + n;

        if (offset >= 0 && offset < difference_type(SegmentSize))
        {
            local_ += n;
        }
        else
        {
            const difference_type segment_offset =
                offset > 0
                    ? offset / difference_type(SegmentSize)
                    : -difference_type((-offset - 1) / SegmentSize) - 1;

            segment_ += segment_offset;

            local_ = *segment_ + (offset - segment_offset * difference_type(SegmentSize));
        }

        return *this;
    }

    segmented_iterator& operator-=(difference_type n) noexcept
    {
        return this->operator+=(-n);
    }

    SFL_NODISCARD
    segmented_iterator operator+(difference_type n) const noexcept
    {
        auto temp = *this;
        temp += n;
        return temp;
    }

    SFL_NODISCARD
    segmented_iterator operator-(difference_type n) const noexcept
    {
        auto temp = *this;
        temp -= n;
        return temp;
    }

    SFL_NODISCARD
    reference operator[](difference_type n) const noexcept
    {
        auto temp = *this;
        temp += n;
        return *temp;
    }

    SFL_NODISCARD
    friend segmented_iterator operator+(difference_type n, const segmented_iterator& it) noexcept
    {
        return it + n;
    }

    SFL_NODISCARD
    friend difference_type operator-(const segmented_iterator& x, const segmented_iterator& y) noexcept
    {
        return (x.segment_ - y.segment_) * difference_type(SegmentSize)
             + (x.local_ - *x.segment_) - (y.local_ - *y.segment_);
    }

    SFL_NODISCARD
    friend bool operator==(const segmented_iterator& x, const segmented_iterator& y) noexcept
    {
        return x.local_ == y.local_;
    }

    SFL_NODISCARD
    friend bool operator!=(const segmented_iterator& x, const segmented_iterator& y) noexcept
    {
        return !(x == y);
    }

    SFL_NODISCARD
    friend bool operator<(const segmented_iterator& x, const segmented_iterator& y) noexcept
    {
        return (x.segment_ == y.segment_) ? (x.local_ < y.local_) : (x.segment_ < y.segment_);
    }

    SFL_NODISCARD
    friend bool operator>(const segmented_iterator& x, const segmented_iterator& y) noexcept
    {
        return y < x;
    }

    SFL_NODISCARD
    friend bool operator<=(const segmented_iterator& x, const segmented_iterator& y) noexcept
    {
        return !(y < x);
    }

    SFL_NODISCARD
    friend bool operator>=(const segmented_iterator& x, const segmented_iterator& y) noexcept
    {
        return !(x < y);
    }
};

template <typename SegmentIterator, typename LocalIterator, std::size_t SegmentSize, typename Container>
struct segmented_iterator_traits<sfl::dtl::segmented_iterator<SegmentIterator, LocalIterator, SegmentSize, Container>>
{
    using is_segmented_iterator = std::true_type;

    using iterator = sfl::dtl::segmented_iterator<SegmentIterator, LocalIterator, SegmentSize, Container>;

    using segment_iterator = SegmentIterator;

    using local_iterator = LocalIterator;

    static segment_iterator segment(iterator it) noexcept
    {
        return it.segment_;
    }

    static local_iterator local(iterator it) noexcept
    {
        return it.local_;
    }

    static local_iterator begin(segment_iterator it) noexcept
    {
        return *it;
    }

    static local_iterator end(segment_iterator it) noexcept
    {
        return *it + SegmentSize;
    }

    static iterator compose(segment_iterator segment, local_iterator local) noexcept
    {
        SFL_ASSERT(begin(segment) <= local && local <= end(segment));

        if (local == end(segment))
        {
            ++segment;
            local = begin(segment);
        }

        return iterator(segment, local);
    }
};

} // namespace dtl

} // namespace sfl

#endif // SFL_DETAIL_SEGMENTED_ITERATOR_HPP_INCLUDED
