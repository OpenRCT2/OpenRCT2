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

#ifndef SFL_DEVECTOR_HPP_INCLUDED
#define SFL_DEVECTOR_HPP_INCLUDED

#include <sfl/detail/container_compatible_range.hpp>
#include <sfl/detail/cpp.hpp>
#include <sfl/detail/exceptions.hpp>
#include <sfl/detail/index_sequence.hpp>
#include <sfl/detail/initialized_memory_algorithms.hpp>
#include <sfl/detail/normal_iterator.hpp>
#include <sfl/detail/scope_guard.hpp>
#include <sfl/detail/tags.hpp>
#include <sfl/detail/to_address.hpp>
#include <sfl/detail/type_traits.hpp>
#include <sfl/detail/uninitialized_memory_algorithms.hpp>

#include <algorithm>        // copy, move, swap, swap_ranges
#include <cstddef>          // size_t
#include <initializer_list> // initializer_list
#include <iterator>         // distance, next, reverse_iterator
#include <limits>           // numeric_limits
#include <memory>           // allocator, allocator_traits, pointer_traits
#include <type_traits>      // is_same, is_nothrow_xxxxx
#include <tuple>            // tuple
#include <utility>          // forward, move, pair

#ifdef SFL_TEST_DEVECTOR
template <int>
void test_devector();
#endif

namespace sfl
{

template <typename T, typename Allocator = std::allocator<T>>
class devector
{
    #ifdef SFL_TEST_DEVECTOR
    template <int>
    friend void ::test_devector();
    #endif

public:

    using allocator_type         = Allocator;
    using allocator_traits       = std::allocator_traits<Allocator>;
    using value_type             = T;
    using size_type              = typename allocator_traits::size_type;
    using difference_type        = typename allocator_traits::difference_type;
    using reference              = T&;
    using const_reference        = const T&;
    using pointer                = typename allocator_traits::pointer;
    using const_pointer          = typename allocator_traits::const_pointer;
    using iterator               = sfl::dtl::normal_iterator<pointer, devector>;
    using const_iterator         = sfl::dtl::normal_iterator<const_pointer, devector>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static_assert
    (
        std::is_same<typename Allocator::value_type, value_type>::value,
        "Allocator::value_type must be same as sfl::devector::value_type."
    );

private:

    class data_base
    {
    public:

        pointer bos_;   // Begin of storage
        pointer first_; // First element in vector
        pointer last_;  // One-past-last element in vector
        pointer eos_;   // End of storage

        data_base() noexcept
            : bos_(nullptr)
            , first_(nullptr)
            , last_(nullptr)
            , eos_(nullptr)
        {}
    };

    class data : public data_base, public allocator_type
    {
    public:

        data() noexcept(std::is_nothrow_default_constructible<allocator_type>::value)
            : allocator_type()
        {}

        data(const allocator_type& alloc) noexcept(std::is_nothrow_copy_constructible<allocator_type>::value)
            : allocator_type(alloc)
        {}

        data(allocator_type&& other) noexcept(std::is_nothrow_move_constructible<allocator_type>::value)
            : allocator_type(std::move(other))
        {}

        allocator_type& ref_to_alloc() noexcept
        {
            return *this;
        }

        const allocator_type& ref_to_alloc() const noexcept
        {
            return *this;
        }
    };

    data data_;

public:

    //
    // ---- CONSTRUCTION AND DESTRUCTION --------------------------------------
    //

    devector() noexcept(std::is_nothrow_default_constructible<Allocator>::value)
        : data_()
    {}

    explicit devector(const Allocator& alloc) noexcept(std::is_nothrow_copy_constructible<Allocator>::value)
        : data_(alloc)
    {}

    devector(size_type n)
        : data_()
    {
        initialize_default_n(n);
    }

    explicit devector(size_type n, const Allocator& alloc)
        : data_(alloc)
    {
        initialize_default_n(n);
    }

    devector(size_type n, const T& value)
        : data_()
    {
        initialize_fill_n(n, value);
    }

    devector(size_type n, const T& value, const Allocator& alloc)
        : data_(alloc)
    {
        initialize_fill_n(n, value);
    }

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_input_iterator<InputIt>::value>* = nullptr>
    devector(InputIt first, InputIt last)
        : data_()
    {
        initialize_range(first, last);
    }

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_input_iterator<InputIt>::value>* = nullptr>
    devector(InputIt first, InputIt last, const Allocator& alloc)
        : data_(alloc)
    {
        initialize_range(first, last);
    }

    devector(std::initializer_list<T> ilist)
        : devector(ilist.begin(), ilist.end())
    {}

    devector(std::initializer_list<T> ilist, const Allocator& alloc)
        : devector(ilist.begin(), ilist.end(), alloc)
    {}

    devector(const devector& other)
        : data_
        (
            allocator_traits::select_on_container_copy_construction
            (
                other.data_.ref_to_alloc()
            )
        )
    {
        initialize_copy(other);
    }

    devector(const devector& other, const Allocator& alloc)
        : data_(alloc)
    {
        initialize_copy(other);
    }

    devector(devector&& other)
        : data_(std::move(other.data_.ref_to_alloc()))
    {
        initialize_move(other);
    }

    devector(devector&& other, const Allocator& alloc)
        : data_(alloc)
    {
        initialize_move(other);
    }

#if SFL_CPP_VERSION >= SFL_CPP_20

    template <sfl::dtl::container_compatible_range<value_type> Range>
    devector(sfl::from_range_t, Range&& range)
        : data_()
    {
        initialize_range(std::forward<Range>(range));
    }

    template <sfl::dtl::container_compatible_range<value_type> Range>
    devector(sfl::from_range_t, Range&& range, const Allocator& alloc)
        : data_(alloc)
    {
        initialize_range(std::forward<Range>(range));
    }

#else // before C++20

    template <typename Range>
    devector(sfl::from_range_t, Range&& range)
        : data_()
    {
        initialize_range(std::forward<Range>(range));
    }

    template <typename Range>
    devector(sfl::from_range_t, Range&& range, const Allocator& alloc)
        : data_(alloc)
    {
        initialize_range(std::forward<Range>(range));
    }

#endif // before C++20

    ~devector()
    {
        sfl::dtl::destroy_a
        (
            data_.ref_to_alloc(),
            data_.first_,
            data_.last_
        );

        sfl::dtl::deallocate
        (
            data_.ref_to_alloc(),
            data_.bos_,
            std::distance(data_.bos_, data_.eos_)
        );
    }

    //
    // ---- ASSIGNMENT --------------------------------------------------------
    //

    void assign(size_type n, const T& value)
    {
        assign_fill_n(n, value);
    }

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_input_iterator<InputIt>::value>* = nullptr>
    void assign(InputIt first, InputIt last)
    {
        assign_range(first, last);
    }

    void assign(std::initializer_list<T> ilist)
    {
        assign_range(ilist.begin(), ilist.end());
    }

#if SFL_CPP_VERSION >= SFL_CPP_20

    template <sfl::dtl::container_compatible_range<value_type> Range>
    void assign_range(Range&& range)
    {
        if constexpr (std::ranges::forward_range<Range>)
        {
            assign_range(std::ranges::begin(range), std::ranges::end(range), std::forward_iterator_tag());
        }
        else
        {
            assign_range(std::ranges::begin(range), std::ranges::end(range), std::input_iterator_tag());
        }
    }

#else // before C++20

    template <typename Range>
    void assign_range(Range&& range)
    {
        using std::begin;
        using std::end;
        assign_range(begin(range), end(range));
    }

#endif // before C++20

    devector& operator=(const devector& other)
    {
        assign_copy(other);
        return *this;
    }

    devector& operator=(devector&& other)
    {
        assign_move(other);
        return *this;
    }

    devector& operator=(std::initializer_list<T> ilist)
    {
        assign_range(ilist.begin(), ilist.end());
        return *this;
    }

    //
    // ---- ALLOCATOR ---------------------------------------------------------
    //

    SFL_NODISCARD
    allocator_type get_allocator() const noexcept
    {
        return data_.ref_to_alloc();
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
    size_type size() const noexcept
    {
        return std::distance(data_.first_, data_.last_);
    }

    SFL_NODISCARD
    size_type max_size() const noexcept
    {
        return std::min<size_type>
        (
            allocator_traits::max_size(data_.ref_to_alloc()),
            std::numeric_limits<difference_type>::max() / sizeof(value_type)
        );
    }

    SFL_NODISCARD
    size_type capacity() const noexcept
    {
        return std::distance(data_.bos_, data_.eos_);
    }

    SFL_NODISCARD
    size_type available_front() const noexcept
    {
        return std::distance(data_.bos_, data_.first_);
    }

    SFL_NODISCARD
    size_type available_back() const noexcept
    {
        return std::distance(data_.last_, data_.eos_);
    }

    void reserve_front(size_type new_capacity)
    {
        const size_type max_size = this->max_size();
        const size_type available_back = this->available_back();

        if (new_capacity > max_size - available_back)
        {
            sfl::dtl::throw_length_error("sfl::devector::reserve_front");
        }

        const size_type size = this->size();
        const size_type available_front = this->available_front();

        if (new_capacity > size + available_front)
        {
            const size_type n = available_back + new_capacity;

            pointer new_bos   = sfl::dtl::allocate(data_.ref_to_alloc(), n);
            pointer new_eos   = new_bos + n;
            pointer new_first = new_eos - std::distance(data_.first_, data_.eos_);
            pointer new_last  = new_first;

            SFL_TRY
            {
                new_last = sfl::dtl::uninitialized_move_if_noexcept_a
                (
                    data_.ref_to_alloc(),
                    data_.first_,
                    data_.last_,
                    new_first
                );
            }
            SFL_CATCH (...)
            {
                sfl::dtl::deallocate
                (
                    data_.ref_to_alloc(),
                    new_bos,
                    n
                );

                SFL_RETHROW;
            }

            sfl::dtl::destroy_a
            (
                data_.ref_to_alloc(),
                data_.first_,
                data_.last_
            );

            sfl::dtl::deallocate
            (
                data_.ref_to_alloc(),
                data_.bos_,
                std::distance(data_.bos_, data_.eos_)
            );

            data_.bos_   = new_bos;
            data_.first_ = new_first;
            data_.last_  = new_last;
            data_.eos_   = new_eos;
        }
    }

    void reserve_back(size_type new_capacity)
    {
        const size_type max_size = this->max_size();
        const size_type available_front = this->available_front();

        if (new_capacity > max_size - available_front)
        {
            sfl::dtl::throw_length_error("sfl::devector::reserve_back");
        }

        const size_type size = this->size();
        const size_type available_back = this->available_back();

        if (new_capacity > size + available_back)
        {
            const size_type n = available_front + new_capacity;

            pointer new_bos   = sfl::dtl::allocate(data_.ref_to_alloc(), n);
            pointer new_eos   = new_bos + n;
            pointer new_first = new_bos + std::distance(data_.bos_, data_.first_);
            pointer new_last  = new_first;

            SFL_TRY
            {
                new_last = sfl::dtl::uninitialized_move_if_noexcept_a
                (
                    data_.ref_to_alloc(),
                    data_.first_,
                    data_.last_,
                    new_first
                );
            }
            SFL_CATCH (...)
            {
                sfl::dtl::deallocate
                (
                    data_.ref_to_alloc(),
                    new_bos,
                    n
                );

                SFL_RETHROW;
            }

            sfl::dtl::destroy_a
            (
                data_.ref_to_alloc(),
                data_.first_,
                data_.last_
            );

            sfl::dtl::deallocate
            (
                data_.ref_to_alloc(),
                data_.bos_,
                std::distance(data_.bos_, data_.eos_)
            );

            data_.bos_   = new_bos;
            data_.first_ = new_first;
            data_.last_  = new_last;
            data_.eos_   = new_eos;
        }
    }

    void shrink_to_fit()
    {
        const size_type new_cap = size();

        if (new_cap < capacity())
        {
            pointer new_bos   = sfl::dtl::allocate(data_.ref_to_alloc(), new_cap);
            pointer new_first = new_bos;
            pointer new_last  = new_first;
            pointer new_eos   = new_first + new_cap;

            SFL_TRY
            {
                new_last = sfl::dtl::uninitialized_move_if_noexcept_a
                (
                    data_.ref_to_alloc(),
                    data_.first_,
                    data_.last_,
                    new_first
                );
            }
            SFL_CATCH (...)
            {
                sfl::dtl::deallocate
                (
                    data_.ref_to_alloc(),
                    new_bos,
                    new_cap
                );

                SFL_RETHROW;
            }

            sfl::dtl::destroy_a
            (
                data_.ref_to_alloc(),
                data_.first_,
                data_.last_
            );

            sfl::dtl::deallocate
            (
                data_.ref_to_alloc(),
                data_.bos_,
                std::distance(data_.bos_, data_.eos_)
            );

            data_.bos_   = new_bos;
            data_.first_ = new_first;
            data_.last_  = new_last;
            data_.eos_   = new_eos;
        }
    }

    //
    // ---- ELEMENT ACCESS ----------------------------------------------------
    //

    SFL_NODISCARD
    reference at(size_type pos)
    {
        if (pos >= size())
        {
            sfl::dtl::throw_out_of_range("sfl::devector::at");
        }

        return *(data_.first_ + pos);
    }

    SFL_NODISCARD
    const_reference at(size_type pos) const
    {
        if (pos >= size())
        {
            sfl::dtl::throw_out_of_range("sfl::devector::at");
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
        return sfl::dtl::to_address(data_.first_);
    }

    SFL_NODISCARD
    const T* data() const noexcept
    {
        return sfl::dtl::to_address(data_.first_);
    }

    //
    // ---- MODIFIERS ---------------------------------------------------------
    //

    void clear() noexcept
    {
        sfl::dtl::destroy_a
        (
            data_.ref_to_alloc(),
            data_.first_,
            data_.last_
        );

        data_.last_ = data_.first_;
    }

    template <typename... Args>
    iterator emplace(const_iterator pos, Args&&... args)
    {
        SFL_ASSERT(cbegin() <= pos && pos <= cend());

        if (pos == cbegin())
        {
            return emplace_front_aux(std::forward<Args>(args)...);
        }
        else if (pos == cend())
        {
            return emplace_back_aux(std::forward<Args>(args)...);
        }
        else
        {
            value_type tmp(std::forward<Args>(args)...);

            emplace_proxy<value_type&&> proxy(std::move(tmp));

            return insert_aux(pos, 1, proxy);
        }
    }

    iterator insert(const_iterator pos, const T& value)
    {
        SFL_ASSERT(cbegin() <= pos && pos <= cend());
        return emplace(pos, value);
    }

    iterator insert(const_iterator pos, T&& value)
    {
        SFL_ASSERT(cbegin() <= pos && pos <= cend());
        return emplace(pos, std::move(value));
    }

    iterator insert(const_iterator pos, size_type n, const T& value)
    {
        SFL_ASSERT(cbegin() <= pos && pos <= cend());
        return insert_fill_n(pos, n, value);
    }

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_input_iterator<InputIt>::value>* = nullptr>
    iterator insert(const_iterator pos, InputIt first, InputIt last)
    {
        SFL_ASSERT(cbegin() <= pos && pos <= cend());
        return insert_range(pos, first, last);
    }

    iterator insert(const_iterator pos, std::initializer_list<T> ilist)
    {
        SFL_ASSERT(cbegin() <= pos && pos <= cend());
        return insert_range(pos, ilist.begin(), ilist.end());
    }

#if SFL_CPP_VERSION >= SFL_CPP_20

    template <sfl::dtl::container_compatible_range<value_type> Range>
    iterator insert_range(const_iterator pos, Range&& range)
    {
        SFL_ASSERT(cbegin() <= pos && pos <= cend());
        if constexpr (std::ranges::forward_range<Range>)
        {
            return insert_range(pos, std::ranges::begin(range), std::ranges::end(range), std::forward_iterator_tag());
        }
        else
        {
            return insert_range(pos, std::ranges::begin(range), std::ranges::end(range), std::input_iterator_tag());
        }
    }

#else // before C++20

    template <typename Range>
    iterator insert_range(const_iterator pos, Range&& range)
    {
        SFL_ASSERT(cbegin() <= pos && pos <= cend());
        using std::begin;
        using std::end;
        return insert_range(pos, begin(range), end(range));
    }

#endif // before C++20

    template <typename... Args>
    reference emplace_front(Args&&... args)
    {
        return *emplace_front_aux(std::forward<Args>(args)...);
    }

    template <typename... Args>
    reference emplace_back(Args&&... args)
    {
        return *emplace_back_aux(std::forward<Args>(args)...);
    }

    void push_front(const T& value)
    {
        emplace_front(value);
    }

    void push_front(T&& value)
    {
        emplace_front(std::move(value));
    }

    void push_back(const T& value)
    {
        emplace_back(value);
    }

    void push_back(T&& value)
    {
        emplace_back(std::move(value));
    }

#if SFL_CPP_VERSION >= SFL_CPP_20

    template <sfl::dtl::container_compatible_range<value_type> Range>
    void prepend_range(Range&& range)
    {
        insert_range(begin(), std::forward<Range>(range));
    }

    template <sfl::dtl::container_compatible_range<value_type> Range>
    void append_range(Range&& range)
    {
        insert_range(end(), std::forward<Range>(range));
    }

#else // before C++20

    template <typename Range>
    void prepend_range(Range&& range)
    {
        insert_range(begin(), std::forward<Range>(range));
    }

    template <typename Range>
    void append_range(Range&& range)
    {
        insert_range(end(), std::forward<Range>(range));
    }

#endif // before C++20

    void pop_front()
    {
        SFL_ASSERT(!empty());

        sfl::dtl::destroy_at_a(data_.ref_to_alloc(), std::addressof(*data_.first_));

        ++data_.first_;
    }

    void pop_back()
    {
        SFL_ASSERT(!empty());

        --data_.last_;

        sfl::dtl::destroy_at_a(data_.ref_to_alloc(), std::addressof(*data_.last_));
    }

    iterator erase(const_iterator pos)
    {
        SFL_ASSERT(cbegin() <= pos && pos < cend());

        const size_type dist_to_begin = std::distance(cbegin(), pos);
        const size_type dist_to_end   = std::distance(pos, cend());

        const pointer p1 = data_.first_ + std::distance(cbegin(), pos);
        const pointer p2 = p1 + 1;

        if (dist_to_begin < dist_to_end)
        {
            const pointer old_first = data_.first_;

            data_.first_ = sfl::dtl::move_backward(data_.first_, p1, p2);

            sfl::dtl::destroy_at_a(data_.ref_to_alloc(), old_first);

            return iterator(p2);
        }
        else
        {
            data_.last_ = sfl::dtl::move(p2, data_.last_, p1);

            sfl::dtl::destroy_at_a(data_.ref_to_alloc(), data_.last_);

            return iterator(p1);
        }
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        SFL_ASSERT(cbegin() <= first && first <= last && last <= cend());

        if (first == last)
        {
            return begin() + std::distance(cbegin(), first);
        }

        const size_type dist_to_begin = std::distance(cbegin(), first);
        const size_type dist_to_end   = std::distance(last, cend());

        if (dist_to_begin < dist_to_end)
        {
            const pointer p1 = data_.first_ + std::distance(cbegin(), first);
            const pointer p2 = data_.first_ + std::distance(cbegin(), last);

            const pointer new_first = sfl::dtl::move_backward(data_.first_, p1, p2);

            sfl::dtl::destroy_a(data_.ref_to_alloc(), data_.first_, new_first);

            data_.first_ = new_first;

            return iterator(p2);
        }
        else
        {
            const pointer p1 = data_.first_ + std::distance(cbegin(), first);
            const pointer p2 = data_.first_ + std::distance(cbegin(), last);

            const pointer new_last = sfl::dtl::move(p2, data_.last_, p1);

            sfl::dtl::destroy_a(data_.ref_to_alloc(), new_last, data_.last_);

            data_.last_ = new_last;

            return iterator(p1);
        }
    }

    void resize(size_type n)
    {
        const size_type size = this->size();

        if (n <= size)
        {
            const pointer new_last = data_.first_ + n;

            sfl::dtl::destroy_a
            (
                data_.ref_to_alloc(),
                new_last,
                data_.last_
            );

            data_.last_ = new_last;
        }
        else
        {
            const size_type available_back = this->available_back();

            if (n <= size + available_back)
            {
                data_.last_ = sfl::dtl::uninitialized_default_construct_n_a
                (
                    data_.ref_to_alloc(),
                    data_.last_,
                    n - size
                );
            }
            else
            {
                const size_type available_front = this->available_front();

                if (n <= available_front + size + available_back)
                {
                    sfl::dtl::uninitialized_default_construct_a
                    (
                        data_.ref_to_alloc(),
                        data_.last_,
                        data_.eos_
                    );

                    data_.last_ = data_.eos_;

                    const pointer new_first = data_.first_ - (n - (size + available_back));

                    sfl::dtl::uninitialized_default_construct_a
                    (
                        data_.ref_to_alloc(),
                        new_first,
                        data_.first_
                    );

                    data_.first_ = new_first;
                }
                else
                {
                    grow_storage_back(n - (available_front + size + available_back));

                    data_.last_ = sfl::dtl::uninitialized_default_construct_n_a
                    (
                        data_.ref_to_alloc(),
                        data_.last_,
                        n - (size + available_front)
                    );

                    sfl::dtl::uninitialized_default_construct_a
                    (
                        data_.ref_to_alloc(),
                        data_.bos_,
                        data_.first_
                    );

                    data_.first_ = data_.bos_;
                }
            }
        }
    }

    void resize(size_type n, const T& value)
    {
        const size_type size = this->size();

        if (n <= size)
        {
            const pointer new_last = data_.first_ + n;

            sfl::dtl::destroy_a
            (
                data_.ref_to_alloc(),
                new_last,
                data_.last_
            );

            data_.last_ = new_last;
        }
        else
        {
            const size_type available_back = this->available_back();

            if (n <= size + available_back)
            {
                data_.last_ = sfl::dtl::uninitialized_fill_n_a
                (
                    data_.ref_to_alloc(),
                    data_.last_,
                    n - size,
                    value
                );
            }
            else
            {
                const size_type available_front = this->available_front();

                if (n <= available_front + size + available_back)
                {
                    sfl::dtl::uninitialized_fill_a
                    (
                        data_.ref_to_alloc(),
                        data_.last_,
                        data_.eos_,
                        value
                    );

                    data_.last_ = data_.eos_;

                    const pointer new_first = data_.first_ - (n - (size + available_back));

                    sfl::dtl::uninitialized_fill_a
                    (
                        data_.ref_to_alloc(),
                        new_first,
                        data_.first_,
                        value
                    );

                    data_.first_ = new_first;
                }
                else
                {
                    grow_storage_back(n - (available_front + size + available_back));

                    data_.last_ = sfl::dtl::uninitialized_fill_n_a
                    (
                        data_.ref_to_alloc(),
                        data_.last_,
                        n - (size + available_front),
                        value
                    );

                    sfl::dtl::uninitialized_fill_a
                    (
                        data_.ref_to_alloc(),
                        data_.bos_,
                        data_.first_,
                        value
                    );

                    data_.first_ = data_.bos_;
                }
            }
        }
    }

    void resize_front(size_type n)
    {
        const size_type size = this->size();

        if (n <= size)
        {
            const pointer new_first = data_.first_ + (size - n);

            sfl::dtl::destroy_a
            (
                data_.ref_to_alloc(),
                data_.first_,
                new_first
            );

            data_.first_ = new_first;
        }
        else
        {
            const size_type available_front = this->available_front();

            if (n > size + available_front)
            {
                grow_storage_front(n - (size + available_front));
            }

            const pointer new_first = data_.first_ - (n - size);

            sfl::dtl::uninitialized_default_construct_a
            (
                data_.ref_to_alloc(),
                new_first,
                data_.first_
            );

            data_.first_ = new_first;
        }
    }

    void resize_front(size_type n, const T& value)
    {
        const size_type size = this->size();

        if (n <= size)
        {
            const pointer new_first = data_.first_ + (size - n);

            sfl::dtl::destroy_a
            (
                data_.ref_to_alloc(),
                data_.first_,
                new_first
            );

            data_.first_ = new_first;
        }
        else
        {
            const size_type available_front = this->available_front();

            if (n > size + available_front)
            {
                grow_storage_front(n - (size + available_front));
            }

            const pointer new_first = data_.first_ - (n - size);

            sfl::dtl::uninitialized_fill_a
            (
                data_.ref_to_alloc(),
                new_first,
                data_.first_,
                value
            );

            data_.first_ = new_first;
        }
    }

    void resize_back(size_type n)
    {
        const size_type size = this->size();

        if (n <= size)
        {
            const pointer new_last = data_.first_ + n;

            sfl::dtl::destroy_a
            (
                data_.ref_to_alloc(),
                new_last,
                data_.last_
            );

            data_.last_ = new_last;
        }
        else
        {
            const size_type available_back = this->available_back();

            if (n > size + available_back)
            {
                grow_storage_back(n - (size + available_back));
            }

            data_.last_ = sfl::dtl::uninitialized_default_construct_n_a
            (
                data_.ref_to_alloc(),
                data_.last_,
                n - size
            );
        }
    }

    void resize_back(size_type n, const T& value)
    {
        const size_type size = this->size();

        if (n <= size)
        {
            const pointer new_last = data_.first_ + n;

            sfl::dtl::destroy_a
            (
                data_.ref_to_alloc(),
                new_last,
                data_.last_
            );

            data_.last_ = new_last;
        }
        else
        {
            const size_type available_back = this->available_back();

            if (n > size + available_back)
            {
                grow_storage_back(n - (size + available_back));
            }

            data_.last_ = sfl::dtl::uninitialized_fill_n_a
            (
                data_.ref_to_alloc(),
                data_.last_,
                n - size,
                value
            );
        }
    }

    void swap(devector& other)
    {
        if (this == &other)
        {
            return;
        }

        using std::swap;

        SFL_ASSERT
        (
            allocator_traits::propagate_on_container_swap::value ||
            this->data_.ref_to_alloc() == other.data_.ref_to_alloc()
        );

        // If this and other allocator compares equal then one allocator
        // can deallocate memory allocated by another allocator.
        // One allocator can safely destroy_a elements constructed by other
        // allocator regardless the two allocators compare equal or not.

        if (allocator_traits::propagate_on_container_swap::value)
        {
            swap(this->data_.ref_to_alloc(), other.data_.ref_to_alloc());
        }

        swap(this->data_.bos_,   other.data_.bos_);
        swap(this->data_.first_, other.data_.first_);
        swap(this->data_.last_,  other.data_.last_);
        swap(this->data_.eos_,   other.data_.eos_);
    }

private:

    template <typename... Args>
    class emplace_proxy
    {
    private:

        std::tuple<Args&...> args_;

    private:

        template <std::size_t... Ints>
        pointer aux_uninitialized_insert_n(allocator_type& alloc, pointer dest, std::size_t n, const sfl::dtl::index_sequence<Ints...>&)
        {
            if (n == 0)
            {
                return dest;
            }
            else
            {
                SFL_ASSERT(n == 1);
                sfl::dtl::construct_at_a(alloc, dest, std::forward<Args>(std::get<Ints>(args_))...);
                return dest + 1;
            }
        }

        template <std::size_t... Ints>
        pointer aux_insert_n(pointer dest, std::size_t n, const sfl::dtl::index_sequence<Ints...>&)
        {
            if (n == 0)
            {
                return dest;
            }
            else
            {
                SFL_ASSERT(n == 1);
                *dest = value_type(std::forward<Args>(std::get<Ints>(args_))...);
                return dest + 1;
            }
        }

    public:

        explicit emplace_proxy(Args&&... args)
            : args_(args...)
        {}

        pointer uninitialized_insert_n(allocator_type& alloc, pointer dest, std::size_t n)
        {
            return aux_uninitialized_insert_n(alloc, dest, n, sfl::dtl::index_sequence_for<Args...>());
        }

        pointer insert_n(pointer dest, std::size_t n)
        {
            return aux_insert_n(dest, n, sfl::dtl::index_sequence_for<Args...>());
        }
    };

    template <typename ForwardIt>
    class insert_range_proxy
    {
    private:

        ForwardIt first_;

    public:

        explicit insert_range_proxy(ForwardIt first)
            : first_(first)
        {}

        pointer uninitialized_insert_n(allocator_type& alloc, pointer dest, std::size_t n)
        {
            const ForwardIt last = std::next(first_, n);
            const pointer result = sfl::dtl::uninitialized_copy_a(alloc, first_, last, dest);
            first_ = last;
            return result;
        }

        pointer insert_n(pointer dest, std::size_t n)
        {
            const ForwardIt last = std::next(first_, n);
            const pointer result = sfl::dtl::copy(first_, last, dest);
            first_ = last;
            return result;
        }
    };

    class insert_fill_n_proxy
    {
    private:

        const value_type& value_;

    public:

        explicit insert_fill_n_proxy(const value_type& value)
            : value_(value)
        {}

        pointer uninitialized_insert_n(allocator_type& alloc, pointer dest, std::size_t n)
        {
            const pointer last = dest + n;
            sfl::dtl::uninitialized_fill_a(alloc, dest, dest + n, value_);
            return last;
        }

        pointer insert_n(pointer dest, std::size_t n)
        {
            const pointer last = dest + n;
            sfl::dtl::fill(dest, dest + n, value_);
            return last;
        }
    };

    ///////////////////////////////////////////////////////////////////////////

    void check_size(size_type n, const char* msg)
    {
        if (n > max_size())
        {
            sfl::dtl::throw_length_error(msg);
        }
    }

    size_type calculate_additional_capacity_for_grow_storage_front(size_type num_additional_elements)
    {
        const size_type capacity_front  = std::distance(data_.bos_, data_.last_);
        const size_type available_front = std::distance(data_.bos_, data_.first_);

        if (num_additional_elements >= available_front)
        {
            return std::max
            (
                num_additional_elements - available_front,
                capacity_front / 2
            );
        }
        else
        {
            return capacity_front / 2;
        }
    }

    size_type calculate_additional_capacity_for_grow_storage_back(size_type num_additional_elements)
    {
        const size_type capacity_back  = std::distance(data_.first_, data_.eos_);
        const size_type available_back = std::distance(data_.last_,  data_.eos_);

        if (num_additional_elements >= available_back)
        {
            return std::max
            (
                num_additional_elements - available_back,
                capacity_back / 2
            );
        }
        else
        {
            return capacity_back / 2;
        }
    }

    void reset(size_type new_cap = 0)
    {
        sfl::dtl::destroy_a
        (
            data_.ref_to_alloc(),
            data_.first_,
            data_.last_
        );

        sfl::dtl::deallocate
        (
            data_.ref_to_alloc(),
            data_.first_,
            std::distance(data_.bos_, data_.eos_)
        );

        // First set pointers to null ...
        data_.bos_   = nullptr;
        data_.first_ = nullptr;
        data_.last_  = nullptr;
        data_.eos_   = nullptr;

        // ... and then allocate new storage. If allocation fails,
        // first_, last_ and eos_ won't be dangling pointers.
        if (new_cap > 0)
        {
            data_.bos_   = sfl::dtl::allocate(data_.ref_to_alloc(), new_cap);
            data_.first_ = data_.bos_;
            data_.last_  = data_.bos_;
            data_.eos_   = data_.bos_ + new_cap;
        }
    }

    void grow_storage_front(size_type additional_capacity)
    {
        const size_type capacity = this->capacity();
        const size_type max_size = this->max_size();

        if (additional_capacity > max_size - capacity)
        {
            sfl::dtl::throw_length_error("sfl::devector::grow_storage_front");
        }

        const size_type n = capacity + additional_capacity;

        pointer new_bos   = sfl::dtl::allocate(data_.ref_to_alloc(), n);
        pointer new_eos   = new_bos + n;
        pointer new_first = new_eos - std::distance(data_.first_, data_.eos_);
        pointer new_last  = new_first;

        SFL_TRY
        {
            new_last = sfl::dtl::uninitialized_move_if_noexcept_a
            (
                data_.ref_to_alloc(),
                data_.first_,
                data_.last_,
                new_first
            );
        }
        SFL_CATCH (...)
        {
            sfl::dtl::deallocate
            (
                data_.ref_to_alloc(),
                new_bos,
                n
            );

            SFL_RETHROW;
        }

        sfl::dtl::destroy_a
        (
            data_.ref_to_alloc(),
            data_.first_,
            data_.last_
        );

        sfl::dtl::deallocate
        (
            data_.ref_to_alloc(),
            data_.bos_,
            std::distance(data_.bos_, data_.eos_)
        );

        data_.bos_   = new_bos;
        data_.first_ = new_first;
        data_.last_  = new_last;
        data_.eos_   = new_eos;
    }

    void grow_storage_back(size_type additional_capacity)
    {
        const size_type capacity = this->capacity();
        const size_type max_size = this->max_size();

        if (additional_capacity > max_size - capacity)
        {
            sfl::dtl::throw_length_error("sfl::devector::grow_storage_back");
        }

        const size_type n = capacity + additional_capacity;

        pointer new_bos   = sfl::dtl::allocate(data_.ref_to_alloc(), n);
        pointer new_eos   = new_bos + n;
        pointer new_first = new_bos + std::distance(data_.bos_, data_.first_);
        pointer new_last  = new_first;

        SFL_TRY
        {
            new_last = sfl::dtl::uninitialized_move_if_noexcept_a
            (
                data_.ref_to_alloc(),
                data_.first_,
                data_.last_,
                new_first
            );
        }
        SFL_CATCH (...)
        {
            sfl::dtl::deallocate
            (
                data_.ref_to_alloc(),
                new_bos,
                n
            );

            SFL_RETHROW;
        }

        sfl::dtl::destroy_a
        (
            data_.ref_to_alloc(),
            data_.first_,
            data_.last_
        );

        sfl::dtl::deallocate
        (
            data_.ref_to_alloc(),
            data_.bos_,
            std::distance(data_.bos_, data_.eos_)
        );

        data_.bos_   = new_bos;
        data_.first_ = new_first;
        data_.last_  = new_last;
        data_.eos_   = new_eos;
    }

    void initialize_default_n(size_type n)
    {
        check_size(n, "sfl::devector::initialize_default_n");

        data_.bos_   = sfl::dtl::allocate(data_.ref_to_alloc(), n);
        data_.first_ = data_.bos_;
        data_.last_  = data_.first_;
        data_.eos_   = data_.bos_ + n;

        SFL_TRY
        {
            data_.last_ = sfl::dtl::uninitialized_default_construct_n_a
            (
                data_.ref_to_alloc(),
                data_.first_,
                n
            );
        }
        SFL_CATCH (...)
        {
            sfl::dtl::deallocate(data_.ref_to_alloc(), data_.bos_, n);
            SFL_RETHROW;
        }
    }

    void initialize_fill_n(size_type n, const T& value)
    {
        check_size(n, "sfl::devector::initialize_fill_n");

        data_.bos_   = sfl::dtl::allocate(data_.ref_to_alloc(), n);
        data_.first_ = data_.bos_;
        data_.last_  = data_.first_;
        data_.eos_   = data_.bos_ + n;

        SFL_TRY
        {
            data_.last_ = sfl::dtl::uninitialized_fill_n_a
            (
                data_.ref_to_alloc(),
                data_.first_,
                n,
                value
            );
        }
        SFL_CATCH (...)
        {
            sfl::dtl::deallocate(data_.ref_to_alloc(), data_.bos_, n);
            SFL_RETHROW;
        }
    }

    template <typename InputIt>
    void initialize_range(InputIt first, InputIt last)
    {
        initialize_range(first, last, typename std::iterator_traits<InputIt>::iterator_category());
    }

    template <typename InputIt, typename Sentinel>
    void initialize_range(InputIt first, Sentinel last, std::input_iterator_tag)
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
            sfl::dtl::destroy_a
            (
                data_.ref_to_alloc(),
                data_.first_,
                data_.last_
            );

            sfl::dtl::deallocate
            (
                data_.ref_to_alloc(),
                data_.bos_,
                std::distance(data_.bos_, data_.eos_)
            );

            SFL_RETHROW;
        }
    }

    template <typename ForwardIt, typename Sentinel>
    void initialize_range(ForwardIt first, Sentinel last, std::forward_iterator_tag)
    {
        const size_type n = std::distance(first, last);

        check_size(n, "sfl::devector::initialize_range");

        data_.bos_   = sfl::dtl::allocate(data_.ref_to_alloc(), n);
        data_.first_ = data_.bos_;
        data_.last_  = data_.first_;
        data_.eos_   = data_.bos_ + n;

        SFL_TRY
        {
            data_.last_ = sfl::dtl::uninitialized_copy_a
            (
                data_.ref_to_alloc(),
                first,
                last,
                data_.first_
            );
        }
        SFL_CATCH (...)
        {
            sfl::dtl::deallocate(data_.ref_to_alloc(), data_.bos_, n);
            SFL_RETHROW;
        }
    }

#if SFL_CPP_VERSION >= SFL_CPP_20

    template <sfl::dtl::container_compatible_range<value_type> Range>
    void initialize_range(Range&& range)
    {
        if constexpr (std::ranges::forward_range<Range>)
        {
            initialize_range(std::ranges::begin(range), std::ranges::end(range), std::forward_iterator_tag());
        }
        else
        {
            initialize_range(std::ranges::begin(range), std::ranges::end(range), std::input_iterator_tag());
        }
    }

#else // before C++20

    template <typename Range>
    void initialize_range(Range&& range)
    {
        using std::begin;
        using std::end;
        initialize_range(begin(range), end(range));
    }

#endif // before C++20

    void initialize_copy(const devector& other)
    {
        const size_type n = other.size();

        check_size(n, "sfl::devector::initialize_copy");

        data_.bos_   = sfl::dtl::allocate(data_.ref_to_alloc(), n);
        data_.first_ = data_.bos_;
        data_.last_  = data_.first_;
        data_.eos_   = data_.bos_ + n;

        SFL_TRY
        {
            data_.last_ = sfl::dtl::uninitialized_copy_a
            (
                data_.ref_to_alloc(),
                other.data_.first_,
                other.data_.last_,
                data_.first_
            );
        }
        SFL_CATCH (...)
        {
            sfl::dtl::deallocate(data_.ref_to_alloc(), data_.bos_, n);
            SFL_RETHROW;
        }
    }

    void initialize_move(devector& other)
    {
        if (data_.ref_to_alloc() == other.data_.ref_to_alloc())
        {
            data_.bos_   = other.data_.bos_;
            data_.first_ = other.data_.first_;
            data_.last_  = other.data_.last_;
            data_.eos_   = other.data_.eos_;

            other.data_.bos_   = nullptr;
            other.data_.first_ = nullptr;
            other.data_.last_  = nullptr;
            other.data_.eos_   = nullptr;
        }
        else
        {
            const size_type n = other.size();

            check_size(n, "sfl::devector::initialize_move");

            data_.bos_   = sfl::dtl::allocate(data_.ref_to_alloc(), n);
            data_.first_ = data_.bos_;
            data_.last_  = data_.first_;
            data_.eos_   = data_.bos_ + n;

            SFL_TRY
            {
                data_.last_ = sfl::dtl::uninitialized_move_a
                (
                    data_.ref_to_alloc(),
                    other.data_.first_,
                    other.data_.last_,
                    data_.first_
                );
            }
            SFL_CATCH (...)
            {
                sfl::dtl::deallocate(data_.ref_to_alloc(), data_.bos_, n);
                SFL_RETHROW;
            }
        }
    }

    void assign_fill_n(size_type n, const T& value)
    {
        const size_type size = this->size();

        if (n <= size)
        {
            const pointer new_last = data_.first_ + n;

            sfl::dtl::fill
            (
                data_.first_,
                new_last,
                value
            );

            sfl::dtl::destroy_a
            (
                data_.ref_to_alloc(),
                new_last,
                data_.last_
            );

            data_.last_ = new_last;
        }
        else
        {
            const size_type available_back = this->available_back();

            if (n <= size + available_back)
            {
                sfl::dtl::fill
                (
                    data_.first_,
                    data_.last_,
                    value
                );

                data_.last_ = sfl::dtl::uninitialized_fill_n_a
                (
                    data_.ref_to_alloc(),
                    data_.last_,
                    n - size,
                    value
                );
            }
            else
            {
                const size_type available_front = this->available_front();

                if (n <= available_front + size + available_back)
                {
                    sfl::dtl::fill
                    (
                        data_.first_,
                        data_.last_,
                        value
                    );

                    sfl::dtl::uninitialized_fill_a
                    (
                        data_.ref_to_alloc(),
                        data_.last_,
                        data_.eos_,
                        value
                    );

                    data_.last_ = data_.eos_;

                    const pointer new_first = data_.first_ - (n - (size + available_back));

                    sfl::dtl::uninitialized_fill_a
                    (
                        data_.ref_to_alloc(),
                        new_first,
                        data_.first_,
                        value
                    );

                    data_.first_ = new_first;
                }
                else
                {
                    grow_storage_back(n - (available_front + size + available_back));

                    sfl::dtl::fill
                    (
                        data_.first_,
                        data_.last_,
                        value
                    );

                    data_.last_ = sfl::dtl::uninitialized_fill_n_a
                    (
                        data_.ref_to_alloc(),
                        data_.last_,
                        n - (size + available_front),
                        value
                    );

                    sfl::dtl::uninitialized_fill_a
                    (
                        data_.ref_to_alloc(),
                        data_.bos_,
                        data_.first_,
                        value
                    );

                    data_.first_ = data_.bos_;
                }
            }
        }
    }

    template <typename InputIt>
    void assign_range(InputIt first, InputIt last)
    {
        assign_range(first, last, typename std::iterator_traits<InputIt>::iterator_category());
    }

    template <typename InputIt, typename Sentinel>
    void assign_range(InputIt first, Sentinel last, std::input_iterator_tag)
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
            sfl::dtl::destroy_a(data_.ref_to_alloc(), curr, data_.last_);
            data_.last_ = curr;
        }
    }

    template <typename ForwardIt, typename Sentinel>
    void assign_range(ForwardIt first, Sentinel last, std::forward_iterator_tag)
    {
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

            sfl::dtl::destroy_a
            (
                data_.ref_to_alloc(),
                new_last,
                data_.last_
            );

            data_.last_ = new_last;
        }
        else
        {
            const size_type available_back = this->available_back();

            if (n <= size + available_back)
            {
                const ForwardIt mid = std::next(first, size);

                sfl::dtl::copy
                (
                    first,
                    mid,
                    data_.first_
                );

                data_.last_ = sfl::dtl::uninitialized_copy_a
                (
                    data_.ref_to_alloc(),
                    mid,
                    last,
                    data_.last_
                );
            }
            else
            {
                const size_type available_front = this->available_front();

                if (n <= available_front + size + available_back)
                {
                    const ForwardIt mid1 = std::next(first, n - (size + available_back));
                    const ForwardIt mid2 = std::next(mid1, size);

                    sfl::dtl::copy
                    (
                        mid1,
                        mid2,
                        data_.first_
                    );

                    data_.last_ = sfl::dtl::uninitialized_copy_a
                    (
                        data_.ref_to_alloc(),
                        mid2,
                        last,
                        data_.last_
                    );

                    const pointer new_first = data_.first_ - (n - (size + available_back));

                    sfl::dtl::uninitialized_copy_a
                    (
                        data_.ref_to_alloc(),
                        first,
                        mid1,
                        new_first
                    );

                    data_.first_ = new_first;
                }
                else
                {
                    grow_storage_back(n - (available_front + size + available_back));

                    const ForwardIt mid1 = std::next(first, available_front);
                    const ForwardIt mid2 = std::next(mid1, size);

                    sfl::dtl::copy
                    (
                        mid1,
                        mid2,
                        data_.first_
                    );

                    data_.last_ = sfl::dtl::uninitialized_copy_a
                    (
                        data_.ref_to_alloc(),
                        mid2,
                        last,
                        data_.last_
                    );

                    sfl::dtl::uninitialized_copy_a
                    (
                        data_.ref_to_alloc(),
                        first,
                        mid1,
                        data_.bos_
                    );

                    data_.first_ = data_.bos_;
                }
            }
        }
    }

    void assign_copy(const devector& other)
    {
        if (this != &other)
        {
            if (allocator_traits::propagate_on_container_copy_assignment::value)
            {
                if (data_.ref_to_alloc() != other.data_.ref_to_alloc())
                {
                    reset();
                }

                data_.ref_to_alloc() = other.data_.ref_to_alloc();
            }

            assign_range
            (
                other.data_.first_,
                other.data_.last_
            );
        }
    }

    void assign_move(devector& other)
    {
        if (allocator_traits::propagate_on_container_move_assignment::value)
        {
            if (data_.ref_to_alloc() != other.data_.ref_to_alloc())
            {
                reset();
            }

            data_.ref_to_alloc() = std::move(other.data_.ref_to_alloc());
        }

        if (data_.ref_to_alloc() == other.data_.ref_to_alloc())
        {
            reset();

            data_.bos_   = other.data_.bos_;
            data_.first_ = other.data_.first_;
            data_.last_  = other.data_.last_;
            data_.eos_   = other.data_.eos_;

            other.data_.bos_   = nullptr;
            other.data_.first_ = nullptr;
            other.data_.last_  = nullptr;
            other.data_.eos_   = nullptr;
        }
        else
        {
            assign_range
            (
                std::make_move_iterator(other.data_.first_),
                std::make_move_iterator(other.data_.last_)
            );
        }
    }

    ///////////////////////////////////////////////////////////////////////////

    template <typename... Args>
    iterator emplace_front_aux(Args&&... args)
    {
        if (data_.first_ != data_.bos_)
        {
            const pointer new_first = data_.first_ - 1;

            sfl::dtl::construct_at_a
            (
                data_.ref_to_alloc(),
                new_first,
                std::forward<Args>(args)...
            );

            data_.first_ = new_first;

            return iterator(new_first);
        }
        else
        {
            value_type tmp(std::forward<Args>(args)...);

            emplace_proxy<value_type&&> proxy(std::move(tmp));

            const size_type additional_capacity = calculate_additional_capacity_for_grow_storage_front(1);

            if (available_back() > additional_capacity)
            {
                return insert_aux_shift_to_back(additional_capacity, cbegin(), 1, proxy);
            }
            else
            {
                return insert_aux_grow_storage_front(additional_capacity, cbegin(), 1, proxy);
            }
        }
    }

    template <typename... Args>
    iterator emplace_back_aux(Args&&... args)
    {
        if (data_.last_ != data_.eos_)
        {
            const pointer old_last = data_.last_;

            sfl::dtl::construct_at_a
            (
                data_.ref_to_alloc(),
                data_.last_,
                std::forward<Args>(args)...
            );

            ++data_.last_;

            return iterator(old_last);
        }
        else
        {
            value_type tmp(std::forward<Args>(args)...);

            emplace_proxy<value_type&&> proxy(std::move(tmp));

            const size_type additional_capacity = calculate_additional_capacity_for_grow_storage_back(1);

            if (available_front() > additional_capacity)
            {
                return insert_aux_shift_to_front(additional_capacity, cend(), 1, proxy);
            }
            else
            {
                return insert_aux_grow_storage_back(additional_capacity, cend(), 1, proxy);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////

    template <typename InsertionProxy>
    iterator insert_aux
    (
        const_iterator insert_pos,
        size_type insert_size,
        InsertionProxy& proxy
    )
    {
        SFL_ASSERT(cbegin() <= insert_pos && insert_pos <= cend());

        if (insert_size == 0)
        {
            return begin() + std::distance(cbegin(), insert_pos);
        }

        const size_type dist_to_begin = std::distance(cbegin(), insert_pos);
        const size_type dist_to_end   = std::distance(insert_pos, cend());

        if (dist_to_begin < dist_to_end)
        {
            if (available_front() >= insert_size)
            {
                if (dist_to_begin > insert_size)
                {
                    //                      p11
                    //                 p10  |   p20          p30
                    // ................[aaaabbbb|zzzzzzzzzzzz].................
                    // ...........[aaaabbbb|....|zzzzzzzzzzzz].................
                    //            q10  |   q20  q30          q40
                    //                 q11

                    const pointer& q10 = data_.first_ - insert_size;
                    const pointer& q11 = data_.first_;
                    const pointer& q20 = q10 + dist_to_begin;
                    const pointer& q30 = data_.first_ + dist_to_begin;

                    const pointer& p10 = data_.first_;
                    const pointer& p11 = p10 + std::distance(q10, q11);
                    const pointer& p20 = q30;

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p10,
                        p11,
                        q10
                    );

                    const auto sg1 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q10, q11);
                    });

                    if (p11 != q11)
                    {
                        sfl::dtl::move
                        (
                            p11,
                            p20,
                            q11
                        );
                    }

                    proxy.insert_n
                    (
                        q20,
                        insert_size
                    );

                    sg1.dismiss();

                    data_.first_ = q10;

                    return iterator(q20);
                }
                else
                {
                    //                     p10      p20          p30
                    // ....................[aaaaaaaa|zzzzzzzzzzzz].............
                    // .......[aaaaaaaa|............|zzzzzzzzzzzz].............
                    //        q10      q20 |        q30          q40
                    //                     q21

                    const pointer& q10 = data_.first_ - insert_size;
                    const pointer& q20 = q10 + dist_to_begin;
                    const pointer& q21 = data_.first_;
                    const pointer& q30 = data_.first_ + dist_to_begin;

                    const pointer& p10 = data_.first_;
                    const pointer& p20 = q30;

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p10,
                        p20,
                        q10
                    );

                    const auto sg1 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q10, q20);
                    });

                    proxy.uninitialized_insert_n
                    (
                        data_.ref_to_alloc(),
                        q20,
                        std::distance(q20, q21)
                    );

                    const auto sg2 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q20, q21);
                    });

                    proxy.insert_n
                    (
                        q21,
                        std::distance(q21, q30)
                    );

                    sg1.dismiss();
                    sg2.dismiss();

                    data_.first_ = q10;

                    return iterator(q20);
                }
            }
            else
            {
                const size_type additional_capacity = calculate_additional_capacity_for_grow_storage_front(insert_size);

                if (available_back() > additional_capacity)
                {
                    return insert_aux_shift_to_back(additional_capacity, insert_pos, insert_size, proxy);
                }
                else
                {
                    return insert_aux_grow_storage_front(additional_capacity, insert_pos, insert_size, proxy);
                }
            }
        }
        else
        {
            if (available_back() >= insert_size)
            {
                if (dist_to_end > insert_size)
                {
                    //                              p21
                    //        p10          p20      |     p30
                    // .......[aaaaaaaaaaaa|yyyyyyyyzzzzzz]....................
                    // .......[aaaaaaaaaaaa|.....|yyyyyyyyzzzzzz]..............
                    //        q10          q20   q30      |   q40
                    //                                    q31

                    const pointer& q20 = data_.first_ + dist_to_begin;
                    const pointer& q30 = q20 + insert_size;
                    const pointer& q31 = data_.last_;
                    const pointer& q40 = q30 + dist_to_end;

                    const pointer& p20 = q20;
                    const pointer& p21 = p20 + std::distance(q30, q31);
                    const pointer& p30 = q31;

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p21,
                        p30,
                        q31
                    );

                    const auto sg1 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q31, q40);
                    });

                    if (p21 != q31)
                    {
                        sfl::dtl::move_backward
                        (
                            p20,
                            p21,
                            q31
                        );
                    }

                    proxy.insert_n
                    (
                        q20,
                        insert_size
                    );

                    sg1.dismiss();

                    data_.last_ = q40;

                    return iterator(q20);
                }
                else
                {
                    //        p10          p20      p30
                    // .......[aaaaaaaaaaaa|zzzzzzzz]..........................
                    // .......[aaaaaaaaaaaa|............|zzzzzzzz].............
                    //        q10          q20      |   q30      q40
                    //                              q21

                    const pointer& q20 = data_.first_ + dist_to_begin;
                    const pointer& q21 = data_.last_;
                    const pointer& q30 = q20 + insert_size;
                    const pointer& q40 = q30 + dist_to_end;

                    const pointer& p20 = q20;
                    const pointer& p30 = q21;

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p20,
                        p30,
                        q30
                    );

                    const auto sg1 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q30, q40);
                    });

                    proxy.insert_n
                    (
                        q20,
                        dist_to_end
                    );

                    proxy.uninitialized_insert_n
                    (
                        data_.ref_to_alloc(),
                        q21,
                        std::distance(q21, q30)
                    );

                    sg1.dismiss();

                    data_.last_ = q40;

                    return iterator(q20);
                }
            }
            else
            {
                const size_type additional_capacity = calculate_additional_capacity_for_grow_storage_back(insert_size);

                if (available_front() > additional_capacity)
                {
                    return insert_aux_shift_to_front(additional_capacity, insert_pos, insert_size, proxy);
                }
                else
                {
                    return insert_aux_grow_storage_back(additional_capacity, insert_pos, insert_size, proxy);
                }
            }
        }
    }

    template <typename InsertionProxy>
    iterator insert_aux_shift_to_back
    (
        size_type shift_size,
        const_iterator insert_pos,
        size_type insert_size,
        InsertionProxy& proxy
    )
    {
        SFL_ASSERT(shift_size > 0);
        SFL_ASSERT(cbegin() <= insert_pos && insert_pos <= cend());
        SFL_ASSERT(insert_size > 0);

        const pointer& p10 = data_.first_;
        const pointer& p20 = p10 + std::distance(cbegin(), insert_pos);
        const pointer& p30 = data_.last_;

        const pointer& q10 = data_.first_ + shift_size - insert_size;
        const pointer& q20 = q10 + std::distance(cbegin(), insert_pos);
        const pointer& q30 = q20 + insert_size;
        const pointer& q40 = q30 + std::distance(insert_pos, cend());

        if (q30 >= p30)
        {
            if (q20 >= p30)
            {
                if (q10 >= p30)
                {
                    //     p10     p20     p30
                    // ....[aaaaaaa|zzzzzzz]...................................
                    // .........................[aaaaaaa|.......|zzzzzzz]......
                    //                          q10     q20     q30     q40

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p10,
                        p20,
                        q10
                    );

                    const auto sg1 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q10, q20);
                    });

                    proxy.uninitialized_insert_n
                    (
                        data_.ref_to_alloc(),
                        q20,
                        insert_size
                    );

                    const auto sg2 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q20, q30);
                    });

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p20,
                        p30,
                        q30
                    );

                    sfl::dtl::destroy_a
                    (
                        data_.ref_to_alloc(),
                        p10,
                        p30
                    );

                    sg1.dismiss();
                    sg2.dismiss();
                }
                else if (q10 >= p20)
                {
                    //         p11     p21
                    //     p10 |   p20 |   p30
                    // ....[aaabbbb|zzzzzzz]...................................
                    // ................[aaabbbb|.......|zzzzzzz]...............
                    //                 q10 |   q20     q30     q40
                    //                     q11

                    const pointer& q11 = p30;
                    const pointer& p11 = p10 + std::distance(q10, q11);
                    const pointer& p21 = q10;

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p20,
                        p30,
                        q30
                    );

                    const auto sg1 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q30, q40);
                    });

                    proxy.uninitialized_insert_n
                    (
                        data_.ref_to_alloc(),
                        q20,
                        insert_size
                    );

                    const auto sg2 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q20, q30);
                    });

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p11,
                        p20,
                        q11
                    );

                    const auto sg3 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q11, q20);
                    });

                    sfl::dtl::move
                    (
                        p10,
                        p11,
                        q10
                    );

                    sfl::dtl::destroy_a
                    (
                        data_.ref_to_alloc(),
                        p10,
                        p21
                    );

                    sg1.dismiss();
                    sg2.dismiss();
                    sg3.dismiss();
                }
                else if (q10 >= p10)
                {
                    //                p11 p12
                    //     p10        |   |    p20   p30
                    // ....[aaaaaaaaaaaaaabbbbb|zzzzz]...........................
                    // ...............[aaaaaaaaaaaaaabbbbb|.......|zzzzz]......
                    //                q10            |    q20     q30   q40
                    //                               q11

                    const pointer& q11 = p30;
                    const pointer& p11 = q10;
                    const pointer& p12 = p10 + std::distance(q10, q11);

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p20,
                        p30,
                        q30
                    );

                    const auto sg1 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q30, q40);
                    });

                    proxy.uninitialized_insert_n
                    (
                        data_.ref_to_alloc(),
                        q20,
                        insert_size
                    );

                    const auto sg2 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q20, q30);
                    });

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p12,
                        p20,
                        q11
                    );

                    const auto sg3 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q11, q20);
                    });

                    if (p12 != q11)
                    {
                        sfl::dtl::move_backward
                        (
                            p10,
                            p12,
                            q11
                        );
                    }

                    sfl::dtl::destroy_a
                    (
                        data_.ref_to_alloc(),
                        p10,
                        p11
                    );

                    sg1.dismiss();
                    sg2.dismiss();
                    sg3.dismiss();
                }
                else //  q10 < p10
                {
                    // Impossible case
                    SFL_ASSERT(false);
                }
            }
            else if (q20 >= p20)
            {
                if (q10 >= p20)
                {
                    //                  p21
                    //     p10     p20  |           p30
                    // ....[aaaaaaa|zzzzzzzzzzzzzzzz]..........................
                    // .................[aaaaaaa|.......|zzzzzzzzzzzzzzzz].....
                    //                  q10     q20 |   q30              q40
                    //                              q21

                    const pointer& q21 = p30;
                    const pointer& p21 = q10;

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p20,
                        p30,
                        q30
                    );

                    const auto sg1 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q30, q40);
                    });

                    sfl::dtl::move
                    (
                        p10,
                        p20,
                        q10
                    );

                    proxy.insert_n
                    (
                        q20,
                        std::distance(q20, q21)
                    );

                    proxy.uninitialized_insert_n
                    (
                        data_.ref_to_alloc(),
                        q21,
                        std::distance(q21, q30)
                    );

                    sfl::dtl::destroy_a
                    (
                        data_.ref_to_alloc(),
                        p10,
                        p21
                    );

                    sg1.dismiss();
                }
                else if (q10 >= p10)
                {
                    //         p11
                    //     p10 |   p20             p30
                    // ....[aaaaaaa|zzzzzzzzzzzzzzz]...........................
                    // ........[aaaaaaa|................|zzzzzzzzzzzzzzz]......
                    //         q10     q20         |    q30             q40
                    //                             q21

                    const pointer& q21 = p30;
                    const pointer& p11 = q10;

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p20,
                        p30,
                        q30
                    );

                    const auto sg1 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q30, q40);
                    });

                    if (p20 != q20)
                    {
                        sfl::dtl::move_backward
                        (
                            p10,
                            p20,
                            q20
                        );
                    }

                    proxy.insert_n
                    (
                        q20,
                        std::distance(q20, q21)
                    );

                    proxy.uninitialized_insert_n
                    (
                        data_.ref_to_alloc(),
                        q21,
                        std::distance(q21, q30)
                    );

                    sfl::dtl::destroy_a
                    (
                        data_.ref_to_alloc(),
                        p10,
                        p11
                    );

                    sg1.dismiss();
                }
                else // q10 < p10
                {
                    // Impossible case
                    SFL_ASSERT(false);
                }
            }
            else if (q20 >= p10)
            {
                //              p11
                //         p10  |       p20      p30
                // ........[aaaabbbbbbbb|zzzzzzzz].............................
                // ...[aaaabbbbbbbb|....................|zzzzzzzz].............
                //    q10  |       q20           |      q30      q40
                //         q11                   q21

                const pointer& q11 = p10;
                const pointer& q21 = p30;
                const pointer& p11 = p10 + std::distance(q10, q11);

                sfl::dtl::uninitialized_move_a
                (
                    data_.ref_to_alloc(),
                    p20,
                    p30,
                    q30
                );

                const auto sg1 = sfl::dtl::make_scope_guard([&](){
                    sfl::dtl::destroy_a(data_.ref_to_alloc(), q30, q40);
                });

                sfl::dtl::uninitialized_move_a
                (
                    data_.ref_to_alloc(),
                    p10,
                    p11,
                    q10
                );

                const auto sg2 = sfl::dtl::make_scope_guard([&](){
                    sfl::dtl::destroy_a(data_.ref_to_alloc(), q10, q10);
                });

                if (p11 != q11)
                {
                    sfl::dtl::move
                    (
                        p11,
                        p20,
                        q11
                    );
                }

                proxy.insert_n
                (
                    q20,
                    std::distance(q20, q21)
                );

                proxy.uninitialized_insert_n
                (
                    data_.ref_to_alloc(),
                    q21,
                    std::distance(q21, q30)
                );

                sg1.dismiss();
                sg2.dismiss();
            }
            else // q20 < p10
            {
                //                 p10     p20     p30
                // ................[aaaaaaa|zzzzzzz]...........................
                // ...[aaaaaaa|........................|zzzzzzz]...............
                //    q10     q20  |               |   q30     q40
                //                 q21             q22

                const pointer& q21 = p10;
                const pointer& q22 = p30;

                sfl::dtl::uninitialized_move_a
                (
                    data_.ref_to_alloc(),
                    p10,
                    p20,
                    q10
                );

                const auto sg1 = sfl::dtl::make_scope_guard([&](){
                    sfl::dtl::destroy_a(data_.ref_to_alloc(), q10, q20);
                });

                sfl::dtl::uninitialized_move_a
                (
                    data_.ref_to_alloc(),
                    p20,
                    p30,
                    q30
                );

                const auto sg2 = sfl::dtl::make_scope_guard([&](){
                    sfl::dtl::destroy_a(data_.ref_to_alloc(), q30, q40);
                });

                proxy.uninitialized_insert_n
                (
                    data_.ref_to_alloc(),
                    q20,
                    std::distance(q20, q21)
                );

                const auto sg3 = sfl::dtl::make_scope_guard([&](){
                    sfl::dtl::destroy_a(data_.ref_to_alloc(), q20, q21);
                });

                proxy.insert_n
                (
                    q21,
                    std::distance(q21, q22)
                );

                proxy.uninitialized_insert_n
                (
                    data_.ref_to_alloc(),
                    q22,
                    std::distance(q22, q30)
                );

                sg1.dismiss();
                sg2.dismiss();
                sg3.dismiss();
            }
        }
        else // q30 < p30
        {
            if (q20 >= p20)
            {
                if (q10 >= p20)
                {
                    //               p21 p22
                    //    p10    p20 |   |               p30
                    // ...[aaaaaa|yyyyyyyzzzzzzzzzzzzzzzz].....................
                    // ..............[aaaaaa|....|yyyyyyyzzzzzzzzzzzzzzzz].....
                    //               q10    q20  q30     |               q40
                    //                                   q31

                    const pointer& q31 = p30;
                    const pointer& p21 = q10;
                    const pointer& p22 = p20 + std::distance(q30, q31);

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p22,
                        p30,
                        q31
                    );

                    const auto sg1 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q31, q40);
                    });

                    if (p22 != q31)
                    {
                        sfl::dtl::move_backward
                        (
                            p20,
                            p22,
                            q31
                        );
                    }

                    sfl::dtl::move
                    (
                        p10,
                        p20,
                        q10
                    );

                    proxy.insert_n
                    (
                        q20,
                        insert_size
                    );

                    sfl::dtl::destroy_a
                    (
                        data_.ref_to_alloc(),
                        p10,
                        p21
                    );

                    sg1.dismiss();
                }
                else if (q10 >= p10)
                {
                    //        p11        p21
                    //    p10 |  p20     |               p30
                    // ...[aaaaaa|yyyyyyyzzzzzzzzzzzzzzzz].....................
                    // .......[aaaaaa|...........|yyyyyyyzzzzzzzzzzzzzzzz].....
                    //        q10    q20         q30     |               q40
                    //                                   q31

                    const pointer& q31 = p30;
                    const pointer& p11 = q10;
                    const pointer& p21 = p20 + std::distance(q30, q31);

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p21,
                        p30,
                        q31
                    );

                    const auto sg1 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q31, q40);
                    });

                    if (p21 != q31)
                    {
                        sfl::dtl::move_backward
                        (
                            p20,
                            p21,
                            q31
                        );
                    }

                    if (p20 != q20)
                    {
                        sfl::dtl::move_backward
                        (
                            p10,
                            p20,
                            q20
                        );
                    }

                    proxy.insert_n
                    (
                        q20,
                        insert_size
                    );

                    sfl::dtl::destroy_a
                    (
                        data_.ref_to_alloc(),
                        p10,
                        p11
                    );

                    sg1.dismiss();
                }
                else // q10 < p10
                {
                    // Impossible case
                    SFL_ASSERT(false);
                }
            }
            else if (q20 >= p10)
            {
                //                            p11    p21
                //                   p10      |  p20 |       p30
                // ..................[aaaaaaaabbb|yyyzzzzzzzz].................
                // .........[aaaaaaaabbb|................|yyyzzzzzzzz].........
                //          q10      |  q20              q30 |       q40
                //                   q11                     q31

                const pointer& q11 = p10;
                const pointer& q31 = p30;
                const pointer& p11 = p10 + std::distance(q10, q11);
                const pointer& p21 = p20 + std::distance(q30, q31);

                sfl::dtl::uninitialized_move_a
                (
                    data_.ref_to_alloc(),
                    p10,
                    p11,
                    q10
                );

                const auto sg1 = sfl::dtl::make_scope_guard([&](){
                    sfl::dtl::destroy_a(data_.ref_to_alloc(), q10, q11);
                });

                if (p11 != q11)
                {
                    sfl::dtl::move
                    (
                        p11,
                        p20,
                        q11
                    );
                }

                sfl::dtl::uninitialized_move_a
                (
                    data_.ref_to_alloc(),
                    p21,
                    p30,
                    q31
                );

                const auto sg2 = sfl::dtl::make_scope_guard([&](){
                    sfl::dtl::destroy_a(data_.ref_to_alloc(), q31, q40);
                });

                if (p21 != q31)
                {
                    sfl::dtl::move_backward
                    (
                        p20,
                        p21,
                        q31
                    );
                }

                proxy.insert_n
                (
                    q20,
                    insert_size
                );

                sg1.dismiss();
                sg2.dismiss();
            }
            else // q20 < p10
            {
                //                                    p21
                //                  p10      p20      |    p30
                // .................[aaaaaaaa|zzzzzzzzyyyyy]...................
                // ...[aaaaaaaa|..................|zzzzzzzzyyyyy]..............
                //    q10      q20  |             q30      |    q40
                //                  q21                    q31

                const pointer& q21 = p10;
                const pointer& q31 = p30;
                const pointer& p21 = p20 + std::distance(q30, q31);

                sfl::dtl::uninitialized_move_a
                (
                    data_.ref_to_alloc(),
                    p10,
                    p20,
                    q10
                );

                const auto sg1 = sfl::dtl::make_scope_guard([&](){
                    sfl::dtl::destroy_a(data_.ref_to_alloc(), q10, q20);
                });

                sfl::dtl::uninitialized_move_a
                (
                    data_.ref_to_alloc(),
                    p21,
                    p30,
                    q31
                );

                const auto sg2 = sfl::dtl::make_scope_guard([&](){
                    sfl::dtl::destroy_a(data_.ref_to_alloc(), q31, q40);
                });

                if (p21 != q31)
                {
                    sfl::dtl::move_backward
                    (
                        p20,
                        p21,
                        q31
                    );
                }

                proxy.uninitialized_insert_n
                (
                    data_.ref_to_alloc(),
                    q20,
                    std::distance(q20, q21)
                );

                const auto sg3 = sfl::dtl::make_scope_guard([&](){
                    sfl::dtl::destroy_a(data_.ref_to_alloc(), q20, q21);
                });

                proxy.insert_n
                (
                    q21,
                    std::distance(q21, q30)
                );

                sg1.dismiss();
                sg2.dismiss();
                sg3.dismiss();
            }
        }

        data_.first_ = q10;
        data_.last_  = q40;

        return iterator(q20);
    }

    template <typename InsertionProxy>
    iterator insert_aux_shift_to_front
    (
        size_type shift_size,
        const_iterator insert_pos,
        size_type insert_size,
        InsertionProxy& proxy
    )
    {
        SFL_ASSERT(shift_size > 0);
        SFL_ASSERT(cbegin() <= insert_pos && insert_pos <= cend());
        SFL_ASSERT(insert_size > 0);

        const pointer& p10 = data_.first_;
        const pointer& p20 = p10 + std::distance(cbegin(), insert_pos);
        const pointer& p30 = data_.last_;

        const pointer& q10 = data_.first_ - shift_size;
        const pointer& q20 = q10 + std::distance(cbegin(), insert_pos);
        const pointer& q30 = q20 + insert_size;
        const pointer& q40 = q30 + std::distance(insert_pos, cend());

        if (q20 <= p10)
        {
            if (q30 <= p10)
            {
                if (q40 <= p10)
                {
                    //                                  p10     p20     p30
                    // .................................[aaaaaaa|zzzzzzz]......
                    // ....[aaaaaaa|.......|zzzzzzz]...........................
                    //     q10     q20     q30     q40

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p10,
                        p20,
                        q10
                    );

                    const auto sg1 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q10, q20);
                    });

                    proxy.uninitialized_insert_n
                    (
                        data_.ref_to_alloc(),
                        q20,
                        insert_size
                    );

                    const auto sg2 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_n_a(data_.ref_to_alloc(), q20, insert_size);
                    });

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p20,
                        p30,
                        q30
                    );

                    sfl::dtl::destroy_a
                    (
                        data_.ref_to_alloc(),
                        p10,
                        p30
                    );

                    sg1.dismiss();
                    sg2.dismiss();
                }
                else if (q40 <= p20)
                {
                    //                                               p21
                    //                                  p10     p20  |  p30
                    // .................................[aaaaaaa|yyyyzzzz].....
                    // ....[aaaaaaa|...............|yyyyzzzz]..................
                    //     q10     q20             q30  |   q40
                    //                                  q31

                    const pointer& q31 = p10;
                    const pointer& p21 = p20 + std::distance(q30, q31);

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p10,
                        p20,
                        q10
                    );

                    const auto sg1 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q10, q20);
                    });

                    proxy.uninitialized_insert_n
                    (
                        data_.ref_to_alloc(),
                        q20,
                        insert_size
                    );

                    const auto sg2 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_n_a(data_.ref_to_alloc(), q20, insert_size);
                    });

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p20,
                        p21,
                        q30
                    );

                    const auto sg3 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q30, q31);
                    });

                    if (p21 != q31) // To avoid moving elements into themselves.
                    {
                        sfl::dtl::move
                        (
                            p21,
                            p30,
                            q31
                        );
                    }

                    sfl::dtl::destroy_a
                    (
                        data_.ref_to_alloc(),
                        q40,
                        p30
                    );

                    sg1.dismiss();
                    sg2.dismiss();
                    sg3.dismiss();
                }
                else if (q40 <= p30)
                {
                    //                                     p21
                    //                            p10  p20 |            p30
                    // ...........................[aaaa|yyyyzzzzzzzzzzzz]......
                    // ....[aaaa|............|yyyyzzzzzzzzzzzz]................
                    //     q10  q20          q30  |           q40
                    //                            q31

                    const pointer& q31 = p10;
                    const pointer& p21 = p20 + std::distance(q30, q31);

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p10,
                        p20,
                        q10
                    );

                    const auto sg1 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q10, q20);
                    });

                    proxy.uninitialized_insert_n
                    (
                        data_.ref_to_alloc(),
                        q20,
                        insert_size
                    );

                    const auto sg2 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_n_a(data_.ref_to_alloc(), q20, insert_size);
                    });

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p20,
                        p21,
                        q30
                    );

                    const auto sg3 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q30, q31);
                    });

                    if (p21 != q31) // To avoid moving elements into themselves.
                    {
                        sfl::dtl::move
                        (
                            p21,
                            p30,
                            q31
                        );
                    }

                    sfl::dtl::destroy_a
                    (
                        data_.ref_to_alloc(),
                        q40,
                        p30
                    );

                    sg1.dismiss();
                    sg2.dismiss();
                    sg3.dismiss();
                }
                else // q40 > p30
                {
                    // Impossible case
                    SFL_ASSERT(false);
                }
            }
            else if (q30 <= p20)
            {
                if (q40 <= p20)
                {
                    //                                  p11
                    //                        p10       |    p20  p30
                    // .......................[aaaaaaaaaaaaaa|zzzz]............
                    // ...[aaaaaaaaaaaaaa|.........|zzzz]......................
                    //    q10            q20  |    q30  q40
                    //                        q21

                    const pointer& q21 = p10;
                    const pointer& p11 = q40;

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p10,
                        p20,
                        q10
                    );

                    const auto sg1 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q10, q20);
                    });

                    proxy.uninitialized_insert_n
                    (
                        data_.ref_to_alloc(),
                        q20,
                        std::distance(q20, q21)
                    );

                    const auto sg2 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q20, q21);
                    });

                    proxy.insert_n
                    (
                        q21,
                        std::distance(q21, q30)
                    );

                    if (p20 != q30) // To avoid moving elements into themselves.
                    {
                        sfl::dtl::move
                        (
                            p20,
                            p30,
                            q30
                        );
                    }

                    sfl::dtl::destroy_a
                    (
                        data_.ref_to_alloc(),
                        p11,
                        p30
                    );

                    sg1.dismiss();
                    sg2.dismiss();
                }
                else if (q40 <= p30)
                {
                    //
                    //                        p10            p20           p30
                    // .......................[aaaaaaaaaaaaaa|zzzzzzzzzzzzzz]...
                    // ...[aaaaaaaaaaaaaa|.........|zzzzzzzzzzzzzz].............
                    //    q10            q20  |    q30           q40
                    //                        q21

                    const pointer& q21 = p10;

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p10,
                        p20,
                        q10
                    );

                    const auto sg1 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q10, q20);
                    });

                    proxy.uninitialized_insert_n
                    (
                        data_.ref_to_alloc(),
                        q20,
                        std::distance(q20, q21)
                    );

                    const auto sg2 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q20, q21);
                    });

                    proxy.insert_n
                    (
                        q21,
                        std::distance(q21, q30)
                    );

                    if (p20 != q30) // To avoid moving elements into themselves.
                    {
                        std::move
                        (
                            p20,
                            p30,
                            q30
                        );
                    }

                    sfl::dtl::destroy_a
                    (
                        data_.ref_to_alloc(),
                        q40,
                        p30
                    );

                    sg1.dismiss();
                    sg2.dismiss();
                }
                else // q40 > p30
                {
                    // Impossible case
                    SFL_ASSERT(false);
                }
            }
            else if (q30 <= p30)
            {
                //                                              p21
                //                              p10     p20     |   p30
                // .............................[aaaaaaa|yyyyyyyzzzz]..........
                // ....[aaaaaaa|............................|yyyyyyyzzzz]......
                //     q10     q20              |           q30     |   q40
                //                              q21                 q31

                const pointer& q21 = p10;
                const pointer& q31 = p30;
                const pointer& p21 = p20 + std::distance(q30, q31);

                sfl::dtl::uninitialized_move_a
                (
                    data_.ref_to_alloc(),
                    p10,
                    p20,
                    q10
                );

                const auto sg1 = sfl::dtl::make_scope_guard([&](){
                    sfl::dtl::destroy_a(data_.ref_to_alloc(), q10, q20);
                });

                proxy.uninitialized_insert_n
                (
                    data_.ref_to_alloc(),
                    q20,
                    std::distance(q20, q21)
                );

                const auto sg2 = sfl::dtl::make_scope_guard([&](){
                    sfl::dtl::destroy_a(data_.ref_to_alloc(), q20, q21);
                });

                sfl::dtl::uninitialized_move_a
                (
                    data_.ref_to_alloc(),
                    p21,
                    p30,
                    q31
                );

                const auto sg3 = sfl::dtl::make_scope_guard([&](){
                    sfl::dtl::destroy_a(data_.ref_to_alloc(), q31, q40);
                });

                if (p21 != q31) // To avoid moving elements into themselves.
                {
                    sfl::dtl::move_backward
                    (
                        p20,
                        p21,
                        q31
                    );
                }

                proxy.insert_n
                (
                    q21,
                    std::distance(q21, q30)
                );

                sg1.dismiss();
                sg2.dismiss();
                sg3.dismiss();
            }
            else // q30 > p30
            {
                //
                //                        p10     p20    p30
                // .......................[aaaaaaa|zzzzzz].....................
                // ....[aaaaaaa|.............................|zzzzzz]..........
                //     q10     q20        |              |   q30    q40
                //                        q21            q22

                const pointer& q21 = p10;
                const pointer& q22 = p30;

                sfl::dtl::uninitialized_move_a
                (
                    data_.ref_to_alloc(),
                    p10,
                    p20,
                    q10
                );

                const auto sg1 = sfl::dtl::make_scope_guard([&](){
                    sfl::dtl::destroy_a(data_.ref_to_alloc(), q10, q20);
                });

                proxy.uninitialized_insert_n
                (
                    data_.ref_to_alloc(),
                    q20,
                    std::distance(q20, q21)
                );

                const auto sg2 = sfl::dtl::make_scope_guard([&](){
                    sfl::dtl::destroy_a(data_.ref_to_alloc(), q20, q21);
                });

                sfl::dtl::uninitialized_move_a
                (
                    data_.ref_to_alloc(),
                    p20,
                    p30,
                    q30
                );

                const auto sg3 = sfl::dtl::make_scope_guard([&](){
                    sfl::dtl::destroy_a(data_.ref_to_alloc(), q30, q40);
                });

                proxy.insert_n
                (
                    q21,
                    std::distance(q21, q22)
                );

                proxy.uninitialized_insert_n
                (
                    data_.ref_to_alloc(),
                    q22,
                    std::distance(q22, q30)
                );

                const auto sg4 = sfl::dtl::make_scope_guard([&](){
                    sfl::dtl::destroy_a(data_.ref_to_alloc(), q22, q30);
                });

                sg1.dismiss();
                sg2.dismiss();
                sg3.dismiss();
                sg4.dismiss();
            }
        }
        else // q20 > p10
        {
            if (q30 <= p20)
            {
                if (q40 <= p20)
                {
                    //                                p11
                    //                  p10           |   p20   p30
                    // .................[aaaaaaaaaaaaabbbb|zzzzz]..............
                    // ...[aaaaaaaaaaaaabbbb|.....|zzzzz]......................
                    //    q10           |   q20   q30   q40
                    //                  q11

                    const pointer& q11 = p10;
                    const pointer& p11 = p10 + std::distance(q10, q11);

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p10,
                        p11,
                        q10
                    );

                    const auto sg1 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q10, q11);
                    });

                    if (p11 != q11) // To avoid moving elements into themselves.
                    {
                        sfl::dtl::move
                        (
                            p11,
                            p20,
                            q11
                        );
                    }

                    proxy.insert_n
                    (
                        q20,
                        insert_size
                    );

                    if (p20 != q30) // To avoid moving elements into themselves.
                    {
                        sfl::dtl::move
                        (
                            p20,
                            p30,
                            q30
                        );
                    }

                    sfl::dtl::destroy_a
                    (
                        data_.ref_to_alloc(),
                        q40,
                        p30
                    );

                    sg1.dismiss();
                }
                else if (q40 <= p30)
                {
                    //                                p11
                    //                  p10           |   p20        p30
                    // .................[aaaaaaaaaaaaabbbb|zzzzzzzzzz].........
                    // ...[aaaaaaaaaaaaabbbb|.....|zzzzzzzzzz].................
                    //    q10           |   q20   q30        q40
                    //                  q11

                    const pointer& q11 = p10;
                    const pointer& p11 = p10 + std::distance(q10, q11);

                    sfl::dtl::uninitialized_move_a
                    (
                        data_.ref_to_alloc(),
                        p10,
                        p11,
                        q10
                    );

                    const auto sg1 = sfl::dtl::make_scope_guard([&](){
                        sfl::dtl::destroy_a(data_.ref_to_alloc(), q10, q20);
                    });

                    if (p11 != q11) // To avoid moving elements into themselves.
                    {
                        sfl::dtl::move
                        (
                            p11,
                            p20,
                            q11
                        );
                    }

                    proxy.insert_n
                    (
                        q20,
                        insert_size
                    );

                    if (p20 != q30) // To avoid moving elements into themselves.
                    {
                        sfl::dtl::move
                        (
                            p20,
                            p30,
                            q30
                        );
                    }

                    sfl::dtl::destroy_a
                    (
                        data_.ref_to_alloc(),
                        q40,
                        p30
                    );

                    sg1.dismiss();
                }
                else // q40 > p30
                {
                    // Impossible case
                    SFL_ASSERT(false);
                }
            }
            else if (q30 <= p30)
            {
                //                                p11        p21
                //                  p10           |   p20    |   p30
                // .................[aaaaaaaaaaaaabbbb|yyyyyyzzzz].............
                // ...[aaaaaaaaaaaaabbbb|.................|yyyyyyzzzz]........
                //    q10           |   q20               q30    |   q40
                //                  q11                          q31

                const pointer& q11 = p10;
                const pointer& q31 = p30;
                const pointer& p11 = p10 + std::distance(q10, q11);
                const pointer& p21 = p20 + std::distance(q30, q31);

                sfl::dtl::uninitialized_move_a
                (
                    data_.ref_to_alloc(),
                    p10,
                    p11,
                    q10
                );

                const auto sg1 = sfl::dtl::make_scope_guard([&](){
                    sfl::dtl::destroy_a(data_.ref_to_alloc(), q10, q11);
                });

                if (p11 != q11) // To avoid moving elements into themselves.
                {
                    sfl::dtl::move
                    (
                        p11,
                        p20,
                        q11
                    );
                }

                sfl::dtl::uninitialized_move_a
                (
                    data_.ref_to_alloc(),
                    p21,
                    p30,
                    q31
                );

                const auto sg2 = sfl::dtl::make_scope_guard([&](){
                    sfl::dtl::destroy_a(data_.ref_to_alloc(), q31, q40);
                });

                if (p21 != q31) // To avoid moving elements into themselves.
                {
                    sfl::dtl::move_backward
                    (
                        p20,
                        p21,
                        q31
                    );
                }

                proxy.insert_n
                (
                    q20,
                    insert_size
                );

                sg1.dismiss();
                sg2.dismiss();
            }
            else // q30 > p30
            {
                //                                p11
                //                  p10           |   p20   p30
                // .................[aaaaaaaaaaaaabbbb|zzzzz]..................
                // ...[aaaaaaaaaaaaabbbb|........................|zzzzz].......
                //    q10           |   q20                 |    q30   q40
                //                  q11                     q21

                const pointer& q11 = p10;
                const pointer& q21 = p30;
                const pointer& p11 = p10 + std::distance(q10, q11);

                sfl::dtl::uninitialized_move_a
                (
                    data_.ref_to_alloc(),
                    p10,
                    p11,
                    q10
                );

                const auto sg1 = sfl::dtl::make_scope_guard([&](){
                    sfl::dtl::destroy_a(data_.ref_to_alloc(), q10, q11);
                });

                if (p11 != q11) // To avoid moving elements into themselves.
                {
                    sfl::dtl::move
                    (
                        p11,
                        p20,
                        q11
                    );
                }

                sfl::dtl::uninitialized_move_a
                (
                    data_.ref_to_alloc(),
                    p20,
                    p30,
                    q30
                );

                const auto sg2 = sfl::dtl::make_scope_guard([&](){
                    sfl::dtl::destroy_a(data_.ref_to_alloc(), q30, q40);
                });

                proxy.insert_n
                (
                    q20,
                    std::distance(q20, q21)
                );

                proxy.uninitialized_insert_n
                (
                    data_.ref_to_alloc(),
                    q21,
                    std::distance(q21, q30)
                );

                sg1.dismiss();
                sg2.dismiss();
            }
        }

        data_.first_ = q10;
        data_.last_  = q40;

        return iterator(q20);
    }

    template <typename InsertionProxy>
    iterator insert_aux_grow_storage_front
    (
        size_type additional_capacity,
        const_iterator insert_pos,
        size_type insert_size,
        InsertionProxy& proxy
    )
    {
        SFL_ASSERT(additional_capacity > 0);
        SFL_ASSERT(cbegin() <= insert_pos && insert_pos <= cend());
        SFL_ASSERT(insert_size > 0);

        if (additional_capacity > max_size() - capacity())
        {
            sfl::dtl::throw_length_error("sfl::devector::insert_aux_grow_storage_front");
        }

        const size_type offset = std::distance(cbegin(), insert_pos);

        const size_type new_cap = capacity() + additional_capacity;

        pointer new_bos   = sfl::dtl::allocate(data_.ref_to_alloc(), new_cap);
        pointer new_eos   = new_bos + new_cap;
        pointer new_first = new_eos - std::distance(data_.last_, data_.eos_) - size() - insert_size;
        pointer new_last  = new_first;

        const pointer p = new_first + offset;

        SFL_TRY
        {
            new_last = sfl::dtl::uninitialized_move_a
            (
                data_.ref_to_alloc(),
                data_.first_,
                data_.first_ + offset,
                new_first
            );

            new_last = proxy.uninitialized_insert_n
            (
                data_.ref_to_alloc(),
                new_last,
                insert_size
            );

            new_last = sfl::dtl::uninitialized_move_a
            (
                data_.ref_to_alloc(),
                data_.first_ + offset,
                data_.last_,
                new_last
            );
        }
        SFL_CATCH (...)
        {
            sfl::dtl::destroy_a
            (
                data_.ref_to_alloc(),
                new_first,
                new_last
            );

            sfl::dtl::deallocate
            (
                data_.ref_to_alloc(),
                new_bos,
                new_cap
            );

            SFL_RETHROW;
        }

        sfl::dtl::destroy_a
        (
            data_.ref_to_alloc(),
            data_.first_,
            data_.last_
        );

        sfl::dtl::deallocate
        (
            data_.ref_to_alloc(),
            data_.bos_,
            std::distance(data_.bos_, data_.eos_)
        );

        data_.bos_   = new_bos;
        data_.first_ = new_first;
        data_.last_  = new_last;
        data_.eos_   = new_eos;

        return iterator(p);
    }

    template <typename InsertionProxy>
    iterator insert_aux_grow_storage_back
    (
        size_type additional_capacity,
        const_iterator insert_pos,
        size_type insert_size,
        InsertionProxy& proxy
    )
    {
        SFL_ASSERT(additional_capacity > 0);
        SFL_ASSERT(cbegin() <= insert_pos && insert_pos <= cend());
        SFL_ASSERT(insert_size > 0);

        if (additional_capacity > max_size() - capacity())
        {
            sfl::dtl::throw_length_error("sfl::devector::insert_aux_grow_storage_back");
        }

        const size_type offset = std::distance(cbegin(), insert_pos);

        const size_type new_cap = capacity() + additional_capacity;

        pointer new_bos   = sfl::dtl::allocate(data_.ref_to_alloc(), new_cap);
        pointer new_eos   = new_bos + new_cap;
        pointer new_first = new_bos + std::distance(data_.bos_, data_.first_);
        pointer new_last  = new_first;

        const pointer p = new_first + offset;

        SFL_TRY
        {
            new_last = sfl::dtl::uninitialized_move_a
            (
                data_.ref_to_alloc(),
                data_.first_,
                data_.first_ + offset,
                new_first
            );

            new_last = proxy.uninitialized_insert_n
            (
                data_.ref_to_alloc(),
                new_last,
                insert_size
            );

            new_last = sfl::dtl::uninitialized_move_a
            (
                data_.ref_to_alloc(),
                data_.first_ + offset,
                data_.last_,
                new_last
            );
        }
        SFL_CATCH (...)
        {
            sfl::dtl::destroy_a
            (
                data_.ref_to_alloc(),
                new_first,
                new_last
            );

            sfl::dtl::deallocate
            (
                data_.ref_to_alloc(),
                new_bos,
                new_cap
            );

            SFL_RETHROW;
        }

        sfl::dtl::destroy_a
        (
            data_.ref_to_alloc(),
            data_.first_,
            data_.last_
        );

        sfl::dtl::deallocate
        (
            data_.ref_to_alloc(),
            data_.bos_,
            std::distance(data_.bos_, data_.eos_)
        );

        data_.bos_   = new_bos;
        data_.first_ = new_first;
        data_.last_  = new_last;
        data_.eos_   = new_eos;

        return iterator(p);
    }

    ///////////////////////////////////////////////////////////////////////////

    iterator insert_fill_n(const_iterator pos, size_type n, const T& value)
    {
        insert_fill_n_proxy proxy(value);
        return insert_aux(pos, n, proxy);
    }

    template <typename InputIt>
    iterator insert_range(const_iterator pos, InputIt first, InputIt last)
    {
        return insert_range(pos, first, last, typename std::iterator_traits<InputIt>::iterator_category());
    }

    template <typename InputIt, typename Sentinel>
    iterator insert_range(const_iterator pos, InputIt first, Sentinel last, std::input_iterator_tag)
    {
        const difference_type offset = std::distance(cbegin(), pos);

        while (first != last)
        {
            pos = insert(pos, *first);
            ++pos;
            ++first;
        }

        return begin() + offset;
    }

    template <typename ForwardIt, typename Sentinel>
    iterator insert_range(const_iterator pos, ForwardIt first, Sentinel last, std::forward_iterator_tag)
    {
        const size_type n = std::distance(first, last);
        insert_range_proxy<ForwardIt> proxy(first);
        return insert_aux(pos, n, proxy);
    }
};

//
// ---- NON-MEMBER FUNCTIONS --------------------------------------------------
//

template <typename T, typename A>
SFL_NODISCARD
bool operator==
(
    const devector<T, A>& x,
    const devector<T, A>& y
)
{
    return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
}

template <typename T, typename A>
SFL_NODISCARD
bool operator!=
(
    const devector<T, A>& x,
    const devector<T, A>& y
)
{
    return !(x == y);
}

template <typename T, typename A>
SFL_NODISCARD
bool operator<
(
    const devector<T, A>& x,
    const devector<T, A>& y
)
{
    return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <typename T, typename A>
SFL_NODISCARD
bool operator>
(
    const devector<T, A>& x,
    const devector<T, A>& y
)
{
    return y < x;
}

template <typename T, typename A>
SFL_NODISCARD
bool operator<=
(
    const devector<T, A>& x,
    const devector<T, A>& y
)
{
    return !(y < x);
}

template <typename T, typename A>
SFL_NODISCARD
bool operator>=
(
    const devector<T, A>& x,
    const devector<T, A>& y
)
{
    return !(x < y);
}

template <typename T, typename A>
void swap
(
    devector<T, A>& x,
    devector<T, A>& y
)
{
    x.swap(y);
}

template <typename T, typename A, typename U>
typename devector<T, A>::size_type
    erase(devector<T, A>& c, const U& value)
{
    auto it = std::remove(c.begin(), c.end(), value);
    auto r = std::distance(it, c.end());
    c.erase(it, c.end());
    return r;
}

template <typename T, typename A, typename Predicate>
typename devector<T, A>::size_type
    erase_if(devector<T, A>& c, Predicate pred)
{
    auto it = std::remove_if(c.begin(), c.end(), pred);
    auto r = std::distance(it, c.end());
    c.erase(it, c.end());
    return r;
}

//
// ---- DEDUCTION GUIDES ------------------------------------------------------
//

#if SFL_CPP_VERSION >= SFL_CPP_17

template < typename InputIt,
           typename Allocator = std::allocator<typename std::iterator_traits<InputIt>::value_type>,
           sfl::dtl::enable_if_t<sfl::dtl::is_input_iterator<InputIt>::value>* = nullptr >
devector(InputIt, InputIt, Allocator = Allocator())
    -> devector<typename std::iterator_traits<InputIt>::value_type, Allocator>;

#endif

} // namespace sfl

#endif // SFL_DEVECTOR_HPP_INCLUDED
