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

#ifndef SFL_STATIC_VECTOR_HPP_INCLUDED
#define SFL_STATIC_VECTOR_HPP_INCLUDED

#include "private.hpp"

#include <algorithm>        // copy, move, swap, swap_ranges
#include <cstddef>          // size_t
#include <initializer_list> // initializer_list
#include <iterator>         // distance, next, reverse_iterator
#include <limits>           // numeric_limits
#include <memory>           // allocator, allocator_traits, pointer_traits
#include <type_traits>      // is_same, is_nothrow_xxxxx
#include <utility>          // forward, move, pair

namespace sfl
{

template <typename T, std::size_t N>
class static_vector
{
    static_assert(N > 0, "N must be greater than zero.");

public:

    using value_type             = T;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using pointer                = value_type*;
    using const_pointer          = const value_type*;
    using iterator               = sfl::dtl::normal_iterator<pointer, static_vector>;
    using const_iterator         = sfl::dtl::normal_iterator<const_pointer, static_vector>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:

    static constexpr size_type static_capacity = N;

private:

    struct data
    {
        union
        {
            value_type first_[N];
        };

        pointer last_;

        data() noexcept
            : last_(first_)
        {}

        ~data() noexcept
        {}
    };

    data data_;

public:

    //
    // ---- CONSTRUCTION AND DESTRUCTION --------------------------------------
    //

    static_vector() noexcept
    {}

    static_vector(size_type n)
    {
        SFL_ASSERT(n <= capacity());

        data_.last_ = sfl::dtl::uninitialized_value_construct_n
        (
            data_.first_,
            n
        );
    }

    static_vector(size_type n, sfl::default_init_t)
    {
        SFL_ASSERT(n <= capacity());

        data_.last_ = sfl::dtl::uninitialized_default_construct_n
        (
            data_.first_,
            n
        );
    }

    static_vector(size_type n, const T& value)
    {
        SFL_ASSERT(n <= capacity());

        data_.last_ = sfl::dtl::uninitialized_fill_n
        (
            data_.first_,
            n,
            value
        );
    }

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_exactly_input_iterator<InputIt>::value>* = nullptr>
    static_vector(InputIt first, InputIt last)
    {
        SFL_TRY
        {
            while (first != last)
            {
                emplace_back(*first);
                ++first;
            }
        }
        SFL_CATCH (...)
        {
            sfl::dtl::destroy(data_.first_, data_.last_);
            SFL_RETHROW;
        }
    }

    template <typename ForwardIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_forward_iterator<ForwardIt>::value>* = nullptr>
    static_vector(ForwardIt first, ForwardIt last)
    {
        SFL_ASSERT(size_type(std::distance(first, last)) <= capacity());

        data_.last_ = sfl::dtl::uninitialized_copy
        (
            first,
            last,
            data_.first_
        );
    }

    static_vector(std::initializer_list<T> ilist)
        : static_vector(ilist.begin(), ilist.end())
    {}

    static_vector(const static_vector& other)
    {
        data_.last_ = sfl::dtl::uninitialized_copy
        (
            pointer(other.data_.first_),
            pointer(other.data_.last_),
            data_.first_
        );
    }

    static_vector(static_vector&& other)
    {
        data_.last_ = sfl::dtl::uninitialized_move
        (
            std::make_move_iterator(pointer(other.data_.first_)),
            std::make_move_iterator(pointer(other.data_.last_)),
            data_.first_
        );
    }

    ~static_vector() noexcept
    {
        sfl::dtl::destroy(data_.first_, data_.last_);
    }

    //
    // ---- ASSIGNMENT --------------------------------------------------------
    //

    void assign(size_type n, const T& value)
    {
        SFL_ASSERT(n <= capacity());

        const size_type size = this->size();

        const pointer new_last = data_.first_ + n;

        if (n <= size)
        {
            sfl::dtl::fill
            (
                data_.first_,
                new_last,
                value
            );

            sfl::dtl::destroy
            (
                new_last,
                data_.last_
            );
        }
        else
        {
            sfl::dtl::fill
            (
                data_.first_,
                data_.last_,
                value
            );

            sfl::dtl::uninitialized_fill
            (
                data_.last_,
                new_last,
                value
            );
        }

        data_.last_ = new_last;
    }

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_exactly_input_iterator<InputIt>::value>* = nullptr>
    void assign(InputIt first, InputIt last)
    {
        pointer curr = data_.first_;

        while (first != last && curr != data_.last_)
        {
            *curr = *first;
            ++curr;
            ++first;
        }

        if (first != last)
        {
            do
            {
                emplace_back(*first);
                ++first;
            }
            while (first != last);
        }
        else if (curr < data_.last_)
        {
            sfl::dtl::destroy(curr, data_.last_);
            data_.last_ = curr;
        }
    }

    template <typename ForwardIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_forward_iterator<ForwardIt>::value>* = nullptr>
    void assign(ForwardIt first, ForwardIt last)
    {
        SFL_ASSERT(size_type(std::distance(first, last)) <= capacity());

        const size_type n = std::distance(first, last);

        const size_type size = this->size();

        if (n <= size)
        {
            const pointer new_last = sfl::dtl::copy
            (
                first,
                last,
                data_.first_
            );

            sfl::dtl::destroy
            (
                new_last,
                data_.last_
            );

            data_.last_ = new_last;
        }
        else
        {
            const ForwardIt mid = std::next(first, size);

            sfl::dtl::copy
            (
                first,
                mid,
                data_.first_
            );

            data_.last_ = sfl::dtl::uninitialized_copy
            (
                mid,
                last,
                data_.last_
            );
        }
    }

    void assign(std::initializer_list<T> ilist)
    {
        assign(ilist.begin(), ilist.end());
    }

    static_vector& operator=(const static_vector& other)
    {
        if (this != &other)
        {
            assign
            (
                pointer(other.data_.first_),
                pointer(other.data_.last_)
            );
        }
        return *this;
    }

    static_vector& operator=(static_vector&& other)
    {
        assign
        (
            std::make_move_iterator(pointer(other.data_.first_)),
            std::make_move_iterator(pointer(other.data_.last_))
        );
        return *this;
    }

    static_vector& operator=(std::initializer_list<T> ilist)
    {
        assign(ilist.begin(), ilist.end());
        return *this;
    }

    //
    // ---- ITERATORS ---------------------------------------------------------
    //

    SFL_NODISCARD
    iterator begin() noexcept
    {
        return iterator(data_.first_);
    }

    SFL_NODISCARD
    const_iterator begin() const noexcept
    {
        return const_iterator(data_.first_);
    }

    SFL_NODISCARD
    const_iterator cbegin() const noexcept
    {
        return const_iterator(data_.first_);
    }

    SFL_NODISCARD
    iterator end() noexcept
    {
        return iterator(data_.last_);
    }

    SFL_NODISCARD
    const_iterator end() const noexcept
    {
        return const_iterator(data_.last_);
    }

    SFL_NODISCARD
    const_iterator cend() const noexcept
    {
        return const_iterator(data_.last_);
    }

    SFL_NODISCARD
    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    SFL_NODISCARD
    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    SFL_NODISCARD
    const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    SFL_NODISCARD
    reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    SFL_NODISCARD
    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    SFL_NODISCARD
    const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    SFL_NODISCARD
    iterator nth(size_type pos) noexcept
    {
        SFL_ASSERT(pos <= size());
        return iterator(data_.first_ + pos);
    }

    SFL_NODISCARD
    const_iterator nth(size_type pos) const noexcept
    {
        SFL_ASSERT(pos <= size());
        return const_iterator(data_.first_ + pos);
    }

    SFL_NODISCARD
    size_type index_of(const_iterator pos) const noexcept
    {
        SFL_ASSERT(cbegin() <= pos && pos <= cend());
        return std::distance(cbegin(), pos);
    }

    //
    // ---- SIZE AND CAPACITY -------------------------------------------------
    //

    SFL_NODISCARD
    bool empty() const noexcept
    {
        return data_.first_ == data_.last_;
    }

    SFL_NODISCARD
    bool full() const noexcept
    {
        return std::distance(begin(), end()) == N;
    }

    SFL_NODISCARD
    size_type size() const noexcept
    {
        return std::distance(begin(), end());
    }

    SFL_NODISCARD
    static constexpr size_type max_size() noexcept
    {
        return N;
    }

    SFL_NODISCARD
    static constexpr size_type capacity() noexcept
    {
        return N;
    }

    SFL_NODISCARD
    size_type available() const noexcept
    {
        return capacity() - size();
    }

    //
    // ---- ELEMENT ACCESS ----------------------------------------------------
    //

    SFL_NODISCARD
    reference at(size_type pos)
    {
        if (pos >= size())
        {
            sfl::dtl::throw_out_of_range("sfl::static_vector::at");
        }

        return *(data_.first_ + pos);
    }

    SFL_NODISCARD
    const_reference at(size_type pos) const
    {
        if (pos >= size())
        {
            sfl::dtl::throw_out_of_range("sfl::static_vector::at");
        }

        return *(data_.first_ + pos);
    }

    SFL_NODISCARD
    reference operator[](size_type pos) noexcept
    {
        SFL_ASSERT(pos < size());
        return *(data_.first_ + pos);
    }

    SFL_NODISCARD
    const_reference operator[](size_type pos) const noexcept
    {
        SFL_ASSERT(pos < size());
        return *(data_.first_ + pos);
    }

    SFL_NODISCARD
    reference front() noexcept
    {
        SFL_ASSERT(!empty());
        return *data_.first_;
    }

    SFL_NODISCARD
    const_reference front() const noexcept
    {
        SFL_ASSERT(!empty());
        return *data_.first_;
    }

    SFL_NODISCARD
    reference back() noexcept
    {
        SFL_ASSERT(!empty());
        return *(data_.last_ - 1);
    }

    SFL_NODISCARD
    const_reference back() const noexcept
    {
        SFL_ASSERT(!empty());
        return *(data_.last_ - 1);
    }

    SFL_NODISCARD
    T* data() noexcept
    {
        return data_.first_;
    }

    SFL_NODISCARD
    const T* data() const noexcept
    {
        return data_.first_;
    }

    //
    // ---- MODIFIERS ---------------------------------------------------------
    //

    void clear() noexcept
    {
        sfl::dtl::destroy(data_.first_, data_.last_);
        data_.last_ = data_.first_;
    }

    template <typename... Args>
    iterator emplace(const_iterator pos, Args&&... args)
    {
        SFL_ASSERT(!full());
        SFL_ASSERT(cbegin() <= pos && pos <= cend());

        const pointer p1 = data_.first_ + std::distance(cbegin(), pos);

        if (p1 == data_.last_)
        {
            sfl::dtl::construct_at
            (
                p1,
                std::forward<Args>(args)...
            );

            ++data_.last_;
        }
        else
        {
            // This container can contain duplicates so we must
            // create new element now as a temporary value.
            value_type tmp(std::forward<Args>(args)...);

            const pointer p2 = data_.last_ - 1;

            const pointer old_last = data_.last_;

            sfl::dtl::construct_at
            (
                data_.last_,
                std::move(*p2)
            );

            ++data_.last_;

            sfl::dtl::move_backward
            (
                p1,
                p2,
                old_last
            );

            *p1 = std::move(tmp);
        }

        return iterator(p1);
    }

    iterator insert(const_iterator pos, const T& value)
    {
        return emplace(pos, value);
    }

    iterator insert(const_iterator pos, T&& value)
    {
        return emplace(pos, std::move(value));
    }

    iterator insert(const_iterator pos, size_type n, const T& value)
    {
        SFL_ASSERT(n <= available());
        SFL_ASSERT(cbegin() <= pos && pos <= cend());

        if (n == 0)
        {
            return begin() + std::distance(cbegin(), pos);
        }

        const value_type tmp(value);

        const pointer p1 = data_.first_ + std::distance(cbegin(), pos);
        const pointer p2 = p1 + n;

        if (p2 <= data_.last_)
        {
            const pointer p3 = data_.last_ - n;

            const pointer old_last = data_.last_;

            data_.last_ = sfl::dtl::uninitialized_move
            (
                p3,
                data_.last_,
                data_.last_
            );

            sfl::dtl::move_backward
            (
                p1,
                p3,
                old_last
            );

            sfl::dtl::fill
            (
                p1,
                p2,
                tmp
            );
        }
        else
        {
            const pointer old_last = data_.last_;

            sfl::dtl::uninitialized_fill
            (
                data_.last_,
                p2,
                tmp
            );

            data_.last_ = p2;

            data_.last_ = sfl::dtl::uninitialized_move
            (
                p1,
                old_last,
                data_.last_
            );

            sfl::dtl::fill
            (
                p1,
                old_last,
                tmp
            );
        }

        return iterator(p1);
    }

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_exactly_input_iterator<InputIt>::value>* = nullptr>
    iterator insert(const_iterator pos, InputIt first, InputIt last)
    {
        const difference_type offset = std::distance(cbegin(), pos);

        while (first != last)
        {
            pos = emplace(pos, *first);
            ++pos;
            ++first;
        }

        return begin() + offset;
    }

    template <typename ForwardIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_forward_iterator<ForwardIt>::value>* = nullptr>
    iterator insert(const_iterator pos, ForwardIt first, ForwardIt last)
    {
        SFL_ASSERT(size_type(std::distance(first, last)) <= available());
        SFL_ASSERT(cbegin() <= pos && pos <= cend());

        if (first == last)
        {
            return begin() + std::distance(cbegin(), pos);
        }

        const size_type n = std::distance(first, last);

        const pointer p1 = data_.first_ + std::distance(cbegin(), pos);
        const pointer p2 = p1 + n;

        if (p2 <= data_.last_)
        {
            const pointer p3 = data_.last_ - n;

            const pointer old_last = data_.last_;

            data_.last_ = sfl::dtl::uninitialized_move
            (
                p3,
                data_.last_,
                data_.last_
            );

            sfl::dtl::move_backward
            (
                p1,
                p3,
                old_last
            );

            sfl::dtl::copy
            (
                first,
                last,
                p1
            );
        }
        else
        {
            const pointer old_last = data_.last_;

            const ForwardIt mid = std::next(first, std::distance(pos, cend()));

            data_.last_ = sfl::dtl::uninitialized_copy
            (
                mid,
                last,
                data_.last_
            );

            data_.last_ = sfl::dtl::uninitialized_move
            (
                p1,
                old_last,
                data_.last_
            );

            sfl::dtl::copy
            (
                first,
                mid,
                p1
            );
        }

        return iterator(p1);
    }

    iterator insert(const_iterator pos, std::initializer_list<T> ilist)
    {
        return insert(pos, ilist.begin(), ilist.end());
    }

    template <typename... Args>
    reference emplace_back(Args&&... args)
    {
        SFL_ASSERT(!full());

        const pointer old_last = data_.last_;

        sfl::dtl::construct_at(data_.last_, std::forward<Args>(args)...);

        ++data_.last_;

        return *old_last;
    }

    void push_back(const T& value)
    {
        emplace_back(value);
    }

    void push_back(T&& value)
    {
        emplace_back(std::move(value));
    }

    void pop_back()
    {
        SFL_ASSERT(!empty());

        --data_.last_;

        sfl::dtl::destroy_at(data_.last_);
    }

    iterator erase(const_iterator pos)
    {
        SFL_ASSERT(cbegin() <= pos && pos < cend());

        const pointer p = data_.first_ + std::distance(cbegin(), pos);

        data_.last_ = sfl::dtl::move(p + 1, data_.last_, p);

        sfl::dtl::destroy_at(data_.last_);

        return iterator(p);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        SFL_ASSERT(cbegin() <= first && first <= last && last <= cend());

        if (first == last)
        {
            return begin() + std::distance(cbegin(), first);
        }

        const pointer p1 = data_.first_ + std::distance(cbegin(), first);
        const pointer p2 = data_.first_ + std::distance(cbegin(), last);

        const pointer new_last = sfl::dtl::move(p2, data_.last_, p1);

        sfl::dtl::destroy(new_last, data_.last_);

        data_.last_ = new_last;

        return iterator(p1);
    }

    void resize(size_type n)
    {
        SFL_ASSERT(n <= capacity());

        const size_type size = this->size();

        if (n < size)
        {
            const pointer new_last = data_.first_ + n;

            sfl::dtl::destroy
            (
                new_last,
                data_.last_
            );

            data_.last_ = new_last;
        }
        else if (n > size)
        {
            const size_type delta = n - size;

            data_.last_ = sfl::dtl::uninitialized_value_construct_n
            (
                data_.last_,
                delta
            );
        }
    }

    void resize(size_type n, sfl::default_init_t)
    {
        SFL_ASSERT(n <= capacity());

        const size_type size = this->size();

        if (n < size)
        {
            const pointer new_last = data_.first_ + n;

            sfl::dtl::destroy
            (
                new_last,
                data_.last_
            );

            data_.last_ = new_last;
        }
        else if (n > size)
        {
            const size_type delta = n - size;

            data_.last_ = sfl::dtl::uninitialized_default_construct_n
            (
                data_.last_,
                delta
            );
        }
    }

    void resize(size_type n, const T& value)
    {
        SFL_ASSERT(n <= capacity());

        const size_type size = this->size();

        if (n < size)
        {
            const pointer new_last = data_.first_ + n;

            sfl::dtl::destroy
            (
                new_last,
                data_.last_
            );

            data_.last_ = new_last;
        }
        else if (n > size)
        {
            const size_type delta = n - size;

            data_.last_ = sfl::dtl::uninitialized_fill_n
            (
                data_.last_,
                delta,
                value
            );
        }
    }

    void swap(static_vector& other)
    {
        if (this == &other)
        {
            return;
        }

        const size_type this_size  = this->size();
        const size_type other_size = other.size();

        if (this_size <= other_size)
        {
            std::swap_ranges
            (
                this->data_.first_,
                this->data_.first_ + this_size,
                other.data_.first_
            );

            sfl::dtl::uninitialized_move
            (
                other.data_.first_ + this_size,
                other.data_.first_ + other_size,
                this->data_.first_ + this_size
            );

            sfl::dtl::destroy
            (
                other.data_.first_ + this_size,
                other.data_.first_ + other_size
            );
        }
        else
        {
            std::swap_ranges
            (
                other.data_.first_,
                other.data_.first_ + other_size,
                this->data_.first_
            );

            sfl::dtl::uninitialized_move
            (
                this->data_.first_ + other_size,
                this->data_.first_ + this_size,
                other.data_.first_ + other_size
            );

            sfl::dtl::destroy
            (
                this->data_.first_ + other_size,
                this->data_.first_ + this_size
            );
        }

        this->data_.last_ = this->data_.first_ + other_size;
        other.data_.last_ = other.data_.first_ + this_size;
    }
};

//
// ---- NON-MEMBER FUNCTIONS --------------------------------------------------
//

template <typename T, std::size_t N>
SFL_NODISCARD
bool operator==
(
    const static_vector<T, N>& x,
    const static_vector<T, N>& y
)
{
    return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
}

template <typename T, std::size_t N>
SFL_NODISCARD
bool operator!=
(
    const static_vector<T, N>& x,
    const static_vector<T, N>& y
)
{
    return !(x == y);
}

template <typename T, std::size_t N>
SFL_NODISCARD
bool operator<
(
    const static_vector<T, N>& x,
    const static_vector<T, N>& y
)
{
    return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <typename T, std::size_t N>
SFL_NODISCARD
bool operator>
(
    const static_vector<T, N>& x,
    const static_vector<T, N>& y
)
{
    return y < x;
}

template <typename T, std::size_t N>
SFL_NODISCARD
bool operator<=
(
    const static_vector<T, N>& x,
    const static_vector<T, N>& y
)
{
    return !(y < x);
}

template <typename T, std::size_t N>
SFL_NODISCARD
bool operator>=
(
    const static_vector<T, N>& x,
    const static_vector<T, N>& y
)
{
    return !(x < y);
}

template <typename T, std::size_t N>
void swap
(
    static_vector<T, N>& x,
    static_vector<T, N>& y
)
{
    x.swap(y);
}

template <typename T, std::size_t N, typename U>
typename static_vector<T, N>::size_type
    erase(static_vector<T, N>& c, const U& value)
{
    auto it = std::remove(c.begin(), c.end(), value);
    auto r = std::distance(it, c.end());
    c.erase(it, c.end());
    return r;
}

template <typename T, std::size_t N, typename Predicate>
typename static_vector<T, N>::size_type
    erase_if(static_vector<T, N>& c, Predicate pred)
{
    auto it = std::remove_if(c.begin(), c.end(), pred);
    auto r = std::distance(it, c.end());
    c.erase(it, c.end());
    return r;
}

} // namespace sfl

#endif // SFL_STATIC_VECTOR_HPP_INCLUDED
