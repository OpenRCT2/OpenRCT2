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

#ifndef SFL_COMPACT_VECTOR_HPP_INCLUDED
#define SFL_COMPACT_VECTOR_HPP_INCLUDED

#include <sfl/detail/container_compatible_range.hpp>
#include <sfl/detail/cpp.hpp>
#include <sfl/detail/exceptions.hpp>
#include <sfl/detail/initialized_memory_algorithms.hpp>
#include <sfl/detail/normal_iterator.hpp>
#include <sfl/detail/tags.hpp>
#include <sfl/detail/to_address.hpp>
#include <sfl/detail/type_traits.hpp>
#include <sfl/detail/uninitialized_memory_algorithms.hpp>

#include <algorithm>        // copy, move, swap, swap_ranges
#include <initializer_list> // initializer_list
#include <iterator>         // distance, next, reverse_iterator
#include <limits>           // numeric_limits
#include <memory>           // allocator, allocator_traits, pointer_traits
#include <type_traits>      // is_same, is_nothrow_xxxxx
#include <utility>          // forward, move, pair

namespace sfl
{

template <typename T, typename Allocator = std::allocator<T>>
class compact_vector
{
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
    using iterator               = sfl::dtl::normal_iterator<pointer, compact_vector>;
    using const_iterator         = sfl::dtl::normal_iterator<const_pointer, compact_vector>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static_assert
    (
        std::is_same<typename Allocator::value_type, value_type>::value,
        "Allocator::value_type must be same as sfl::compact_vector::value_type."
    );

private:

    class data_base
    {
    public:

        pointer first_;
        pointer last_;

        data_base() noexcept
            : first_(nullptr)
            , last_(nullptr)
        {}
    };

    class data : public data_base , public allocator_type
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

    compact_vector() noexcept
    (
        std::is_nothrow_default_constructible<Allocator>::value
    )
        : data_()
    {}

    explicit compact_vector(const Allocator& alloc) noexcept
    (
        std::is_nothrow_copy_constructible<Allocator>::value
    )
        : data_(alloc)
    {}

    explicit compact_vector(size_type n)
        : data_()
    {
        initialize_default_n(n);
    }

    explicit compact_vector(size_type n, const Allocator& alloc)
        : data_(alloc)
    {
        initialize_default_n(n);
    }

    compact_vector(size_type n, const T& value)
        : data_()
    {
        initialize_fill_n(n, value);
    }

    compact_vector(size_type n, const T& value, const Allocator& alloc)
        : data_(alloc)
    {
        initialize_fill_n(n, value);
    }

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_input_iterator<InputIt>::value>* = nullptr>
    compact_vector(InputIt first, InputIt last)
        : data_()
    {
        initialize_range(first, last);
    }

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_input_iterator<InputIt>::value>* = nullptr>
    compact_vector(InputIt first, InputIt last, const Allocator& alloc)
        : data_(alloc)
    {
        initialize_range(first, last);
    }

    compact_vector(std::initializer_list<T> ilist)
        : compact_vector(ilist.begin(), ilist.end())
    {}

    compact_vector(std::initializer_list<T> ilist, const Allocator& alloc)
        : compact_vector(ilist.begin(), ilist.end(), alloc)
    {}

    compact_vector(const compact_vector& other)
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

    compact_vector(const compact_vector& other, const Allocator& alloc)
        : data_(alloc)
    {
        initialize_copy(other);
    }

    compact_vector(compact_vector&& other)
        : data_(std::move(other.data_.ref_to_alloc()))
    {
        initialize_move(other);
    }

    compact_vector(compact_vector&& other, const Allocator& alloc)
        : data_(alloc)
    {
        initialize_move(other);
    }

#if SFL_CPP_VERSION >= SFL_CPP_20

    template <sfl::dtl::container_compatible_range<value_type> Range>
    compact_vector(sfl::from_range_t, Range&& range)
        : data_()
    {
        initialize_range(std::forward<Range>(range));
    }

    template <sfl::dtl::container_compatible_range<value_type> Range>
    compact_vector(sfl::from_range_t, Range&& range, const Allocator& alloc)
        : data_(alloc)
    {
        initialize_range(std::forward<Range>(range));
    }

#else // before C++20

    template <typename Range>
    compact_vector(sfl::from_range_t, Range&& range)
        : data_()
    {
        initialize_range(std::forward<Range>(range));
    }

    template <typename Range>
    compact_vector(sfl::from_range_t, Range&& range, const Allocator& alloc)
        : data_(alloc)
    {
        initialize_range(std::forward<Range>(range));
    }

#endif // before C++20

    ~compact_vector()
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
            data_.last_ - data_.first_
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

    compact_vector& operator=(const compact_vector& other)
    {
        assign_copy(other);
        return *this;
    }

    compact_vector& operator=(compact_vector&& other)
    {
        assign_move(other);
        return *this;
    }

    compact_vector& operator=(std::initializer_list<T> ilist)
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
        return size();
    }

    SFL_NODISCARD
    size_type available() const noexcept
    {
        return 0;
    }

    //
    // ---- ELEMENT ACCESS ----------------------------------------------------
    //

    SFL_NODISCARD
    reference at(size_type pos)
    {
        if (pos >= size())
        {
            sfl::dtl::throw_out_of_range("sfl::compact_vector::at");
        }

        return *(data_.first_ + pos);
    }

    SFL_NODISCARD
    const_reference at(size_type pos) const
    {
        if (pos >= size())
        {
            sfl::dtl::throw_out_of_range("sfl::compact_vector::at");
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

        sfl::dtl::deallocate
        (
            data_.ref_to_alloc(),
            data_.first_,
            data_.last_ - data_.first_
        );

        data_.first_ = nullptr;
        data_.last_  = nullptr;
    }

    template <typename... Args>
    iterator emplace(const_iterator pos, Args&&... args)
    {
        SFL_ASSERT(cbegin() <= pos && pos <= cend());

        const difference_type offset = std::distance(cbegin(), pos);

        const size_type new_size = size() + 1;

        if (new_size < size() || new_size > max_size())
        {
            sfl::dtl::throw_length_error("sfl::compact_vector::emplace");
        }

        pointer new_first = sfl::dtl::allocate(data_.ref_to_alloc(), new_size);
        pointer new_last  = new_first;

        SFL_TRY
        {
            // The order of operations is critical. First we will construct
            // new element in new storage because arguments `args...` can
            // contain reference to element in this container and after
            // that we will move elements from old to new storage.

            sfl::dtl::construct_at_a
            (
                data_.ref_to_alloc(),
                new_first + offset,
                std::forward<Args>(args)...
            );

            new_last = nullptr;

            new_last = sfl::dtl::uninitialized_move_if_noexcept_a
            (
                data_.ref_to_alloc(),
                data_.first_,
                data_.first_ + offset,
                new_first
            );

            ++new_last;

            new_last = sfl::dtl::uninitialized_move_if_noexcept_a
            (
                data_.ref_to_alloc(),
                data_.first_ + offset,
                data_.last_,
                new_last
            );
        }
        SFL_CATCH (...)
        {
            if (new_last == nullptr)
            {
                sfl::dtl::destroy_at_a
                (
                    data_.ref_to_alloc(),
                    new_first + offset
                );
            }
            else
            {
                sfl::dtl::destroy_a
                (
                    data_.ref_to_alloc(),
                    new_first,
                    new_last
                );
            }

            sfl::dtl::deallocate
            (
                data_.ref_to_alloc(),
                new_first,
                new_size
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
            data_.first_,
            data_.last_ - data_.first_
        );

        data_.first_ = new_first;
        data_.last_  = new_last;

        return begin() + offset;
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
    reference emplace_back(Args&&... args)
    {
        return *emplace(cend(), std::forward<Args>(args)...);
    }

    void push_back(const T& value)
    {
        emplace(cend(), value);
    }

    void push_back(T&& value)
    {
        emplace(cend(), std::move(value));
    }

#if SFL_CPP_VERSION >= SFL_CPP_20

    template <sfl::dtl::container_compatible_range<value_type> Range>
    void append_range(Range&& range)
    {
        insert_range(end(), std::forward<Range>(range));
    }

#else // before C++20

    template <typename Range>
    void append_range(Range&& range)
    {
        insert_range(end(), std::forward<Range>(range));
    }

#endif // before C++20

    void pop_back()
    {
        SFL_ASSERT(!empty());
        erase(const_iterator(data_.last_ - 1));
    }

    iterator erase(const_iterator pos)
    {
        SFL_ASSERT(cbegin() <= pos && pos < cend());
        return erase(pos, pos + 1);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        SFL_ASSERT(cbegin() <= first && first <= last && last <= cend());

        if (first == last)
        {
            return begin() + std::distance(cbegin(), first);
        }

        const difference_type offset1 = std::distance(cbegin(), first);
        const difference_type offset2 = std::distance(cbegin(), last);

        const size_type new_size = size() - std::distance(first, last);

        pointer new_first = sfl::dtl::allocate(data_.ref_to_alloc(), new_size);
        pointer new_last  = new_first;

        SFL_TRY
        {
            new_last = sfl::dtl::uninitialized_move_if_noexcept_a
            (
                data_.ref_to_alloc(),
                data_.first_,
                data_.first_ + offset1,
                new_first
            );

            new_last = sfl::dtl::uninitialized_move_if_noexcept_a
            (
                data_.ref_to_alloc(),
                data_.first_ + offset2,
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
                new_first,
                new_size
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
            data_.first_,
            data_.last_ - data_.first_
        );

        data_.first_ = new_first;
        data_.last_  = new_last;

        return begin() + offset1;
    }

    void resize(size_type n)
    {
        if (n == size())
        {
            return;
        }

        if (n > max_size())
        {
            sfl::dtl::throw_length_error("sfl::compact_vector::resize");
        }

        pointer new_first = sfl::dtl::allocate(data_.ref_to_alloc(), n);
        pointer new_last  = new_first;

        SFL_TRY
        {
            if (n > size())
            {
                new_last = sfl::dtl::uninitialized_move_if_noexcept_a
                (
                    data_.ref_to_alloc(),
                    data_.first_,
                    data_.last_,
                    new_first
                );

                new_last = sfl::dtl::uninitialized_default_construct_n_a
                (
                    data_.ref_to_alloc(),
                    new_last,
                    n - size()
                );
            }
            else if (n < size())
            {
                new_last = sfl::dtl::uninitialized_move_if_noexcept_a
                (
                    data_.ref_to_alloc(),
                    data_.first_,
                    data_.first_ + n,
                    new_first
                );
            }
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
                new_first,
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
            data_.first_,
            data_.last_ - data_.first_
        );

        data_.first_ = new_first;
        data_.last_  = new_last;
    }

    void resize(size_type n, const T& value)
    {
        if (n == size())
        {
            return;
        }

        if (n > max_size())
        {
            sfl::dtl::throw_length_error("sfl::compact_vector::resize");
        }

        pointer new_first = sfl::dtl::allocate(data_.ref_to_alloc(), n);
        pointer new_last  = new_first;

        SFL_TRY
        {
            if (n > size())
            {
                new_last = sfl::dtl::uninitialized_move_if_noexcept_a
                (
                    data_.ref_to_alloc(),
                    data_.first_,
                    data_.last_,
                    new_first
                );

                new_last = sfl::dtl::uninitialized_fill_n_a
                (
                    data_.ref_to_alloc(),
                    new_last,
                    n - size(),
                    value
                );
            }
            else if (n < size())
            {
                new_last = sfl::dtl::uninitialized_move_if_noexcept_a
                (
                    data_.ref_to_alloc(),
                    data_.first_,
                    data_.first_ + n,
                    new_first
                );
            }
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
                new_first,
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
            data_.first_,
            data_.last_ - data_.first_
        );

        data_.first_ = new_first;
        data_.last_  = new_last;
    }

    void swap(compact_vector& other)
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

        swap(this->data_.first_, other.data_.first_);
        swap(this->data_.last_,  other.data_.last_);
    }

private:

    void initialize_default_n(size_type n)
    {
        if (n > max_size())
        {
            sfl::dtl::throw_length_error
            (
                "sfl::compact_vector::initialize_default_n"
            );
        }

        data_.first_ = sfl::dtl::allocate(data_.ref_to_alloc(), n);
        data_.last_  = data_.first_;

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
            sfl::dtl::deallocate
            (
                data_.ref_to_alloc(),
                data_.first_,
                n
            );

            SFL_RETHROW;
        }
    }

    void initialize_fill_n(size_type n, const T& value)
    {
        if (n > max_size())
        {
            sfl::dtl::throw_length_error
            (
                "sfl::compact_vector::initialize_fill_n"
            );
        }

        data_.first_ = sfl::dtl::allocate(data_.ref_to_alloc(), n);
        data_.last_  = data_.first_;

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
            sfl::dtl::deallocate
            (
                data_.ref_to_alloc(),
                data_.first_,
                n
            );

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
                data_.first_,
                data_.last_ - data_.first_
            );

            SFL_RETHROW;
        }
    }

    template <typename ForwardIt, typename Sentinel>
    void initialize_range(ForwardIt first, Sentinel last, std::forward_iterator_tag)
    {
        const size_type n = std::distance(first, last);

        if (n > max_size())
        {
            sfl::dtl::throw_length_error
            (
                "sfl::compact_vector::initialize_range"
            );
        }

        data_.first_ = sfl::dtl::allocate(data_.ref_to_alloc(), n);
        data_.last_  = data_.first_;

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
            sfl::dtl::deallocate
            (
                data_.ref_to_alloc(),
                data_.first_,
                n
            );

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

    void initialize_copy(const compact_vector& other)
    {
        const size_type n = other.size();

        if (n > max_size())
        {
            sfl::dtl::throw_length_error
            (
                "sfl::compact_vector::initialize_copy"
            );
        }

        data_.first_ = sfl::dtl::allocate(data_.ref_to_alloc(), n);
        data_.last_  = data_.first_;

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
            sfl::dtl::deallocate
            (
                data_.ref_to_alloc(),
                data_.first_,
                n
            );

            SFL_RETHROW;
        }
    }

    void initialize_move(compact_vector& other)
    {
        if (data_.ref_to_alloc() == other.data_.ref_to_alloc())
        {
            data_.first_ = other.data_.first_;
            data_.last_  = other.data_.last_;

            other.data_.first_ = nullptr;
            other.data_.last_  = nullptr;
        }
        else
        {
            const size_type n = other.size();

            if (n > max_size())
            {
                sfl::dtl::throw_length_error
                (
                    "sfl::compact_vector::initialize_move"
                );
            }

            data_.first_ = sfl::dtl::allocate(data_.ref_to_alloc(), n);
            data_.last_  = data_.first_;

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
                sfl::dtl::deallocate
                (
                    data_.ref_to_alloc(),
                    data_.first_,
                    n
                );

                SFL_RETHROW;
            }
        }
    }

    void assign_fill_n(size_type n, const T& value)
    {
        if (n > max_size())
        {
            sfl::dtl::throw_length_error
            (
                "sfl::compact_vector::assign_fill_n"
            );
        }

        if (n == size())
        {
            sfl::dtl::fill
            (
                data_.first_,
                data_.last_,
                value
            );
        }
        else
        {
            clear();

            data_.first_ = sfl::dtl::allocate(data_.ref_to_alloc(), n);
            data_.last_  = data_.first_;

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
                clear();
                SFL_RETHROW;
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
        clear();

        while (first != last)
        {
            emplace_back(*first);
            ++first;
        }
    }

    template <typename ForwardIt, typename Sentinel>
    void assign_range(ForwardIt first, Sentinel last, std::forward_iterator_tag)
    {
        const size_type n = std::distance(first, last);

        if (n > max_size())
        {
            sfl::dtl::throw_length_error
            (
                "sfl::compact_vector::assign_fill_n"
            );
        }

        if (n == size())
        {
            sfl::dtl::copy
            (
                first,
                last,
                data_.first_
            );
        }
        else
        {
            clear();

            data_.first_ = sfl::dtl::allocate(data_.ref_to_alloc(), n);
            data_.last_  = data_.first_;

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
                clear();
                SFL_RETHROW;
            }
        }
    }

    void assign_copy(const compact_vector& other)
    {
        if (this != &other)
        {
            if (allocator_traits::propagate_on_container_copy_assignment::value)
            {
                if (data_.ref_to_alloc() != other.data_.ref_to_alloc())
                {
                    clear();
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

    void assign_move(compact_vector& other)
    {
        if (allocator_traits::propagate_on_container_move_assignment::value)
        {
            if (data_.ref_to_alloc() != other.data_.ref_to_alloc())
            {
                clear();
            }

            data_.ref_to_alloc() = std::move(other.data_.ref_to_alloc());
        }

        if (data_.ref_to_alloc() == other.data_.ref_to_alloc())
        {
            clear();

            data_.first_ = other.data_.first_;
            data_.last_  = other.data_.last_;

            other.data_.first_ = nullptr;
            other.data_.last_  = nullptr;
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

    iterator insert_fill_n(const_iterator pos, size_type n, const T& value)
    {
        const difference_type offset = std::distance(cbegin(), pos);

        if (n != 0)
        {
            const size_type new_size = size() + n;

            if (new_size < size() || new_size > max_size())
            {
                sfl::dtl::throw_length_error
                (
                    "sfl::compact_vector::insert_fill_n"
                );
            }

            pointer new_first = sfl::dtl::allocate(data_.ref_to_alloc(), new_size);
            pointer new_last  = new_first;

            SFL_TRY
            {
                // `value` can be a reference to an element in this
                // container. First we will create `n` copies of `value`
                // and after that we can move elements.

                sfl::dtl::uninitialized_fill_n_a
                (
                    data_.ref_to_alloc(),
                    new_first + offset,
                    n,
                    value
                );

                new_last = nullptr;

                new_last = sfl::dtl::uninitialized_move_if_noexcept_a
                (
                    data_.ref_to_alloc(),
                    data_.first_,
                    data_.first_ + offset,
                    new_first
                );

                new_last += n;

                new_last = sfl::dtl::uninitialized_move_if_noexcept_a
                (
                    data_.ref_to_alloc(),
                    data_.first_ + offset,
                    data_.last_,
                    new_last
                );
            }
            SFL_CATCH (...)
            {
                if (new_last == nullptr)
                {
                    sfl::dtl::destroy_n_a
                    (
                        data_.ref_to_alloc(),
                        new_first + offset,
                        n
                    );
                }
                else
                {
                    sfl::dtl::destroy_a
                    (
                        data_.ref_to_alloc(),
                        new_first,
                        new_last
                    );
                }

                sfl::dtl::deallocate
                (
                    data_.ref_to_alloc(),
                    new_first,
                    new_size
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
                data_.first_,
                data_.last_ - data_.first_
            );

            data_.first_ = new_first;
            data_.last_  = new_last;
        }

        return begin() + offset;
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
        const difference_type offset = std::distance(cbegin(), pos);

        if (first != last)
        {
            const size_type new_size = size() + std::distance(first, last);

            if (new_size < size() || new_size > max_size())
            {
                sfl::dtl::throw_length_error
                (
                    "sfl::compact_vector::insert_range"
                );
            }

            pointer new_first = sfl::dtl::allocate(data_.ref_to_alloc(), new_size);
            pointer new_last  = new_first;

            SFL_TRY
            {
                new_last = sfl::dtl::uninitialized_move_if_noexcept_a
                (
                    data_.ref_to_alloc(),
                    data_.first_,
                    data_.first_ + offset,
                    new_first
                );

                new_last = sfl::dtl::uninitialized_copy_a
                (
                    data_.ref_to_alloc(),
                    first,
                    last,
                    new_last
                );

                new_last = sfl::dtl::uninitialized_move_if_noexcept_a
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
                    new_first,
                    new_size
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
                data_.first_,
                data_.last_ - data_.first_
            );

            data_.first_ = new_first;
            data_.last_  = new_last;
        }

        return begin() + offset;
    }
};

//
// ---- NON-MEMBER FUNCTIONS --------------------------------------------------
//

template <typename T, typename A>
SFL_NODISCARD
bool operator==
(
    const compact_vector<T, A>& x,
    const compact_vector<T, A>& y
)
{
    return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
}

template <typename T, typename A>
SFL_NODISCARD
bool operator!=
(
    const compact_vector<T, A>& x,
    const compact_vector<T, A>& y
)
{
    return !(x == y);
}

template <typename T, typename A>
SFL_NODISCARD
bool operator<
(
    const compact_vector<T, A>& x,
    const compact_vector<T, A>& y
)
{
    return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <typename T, typename A>
SFL_NODISCARD
bool operator>
(
    const compact_vector<T, A>& x,
    const compact_vector<T, A>& y
)
{
    return y < x;
}

template <typename T, typename A>
SFL_NODISCARD
bool operator<=
(
    const compact_vector<T, A>& x,
    const compact_vector<T, A>& y
)
{
    return !(y < x);
}

template <typename T, typename A>
SFL_NODISCARD
bool operator>=
(
    const compact_vector<T, A>& x,
    const compact_vector<T, A>& y
)
{
    return !(x < y);
}

template <typename T, typename A>
void swap
(
    compact_vector<T, A>& x,
    compact_vector<T, A>& y
)
{
    x.swap(y);
}

template <typename T, typename A, typename U>
typename compact_vector<T, A>::size_type
    erase(compact_vector<T, A>& c, const U& value)
{
    auto it = std::remove(c.begin(), c.end(), value);
    auto r = std::distance(it, c.end());
    c.erase(it, c.end());
    return r;
}

template <typename T, typename A, typename Predicate>
typename compact_vector<T, A>::size_type
    erase_if(compact_vector<T, A>& c, Predicate pred)
{
    auto it = std::remove_if(c.begin(), c.end(), pred);
    auto r = std::distance(it, c.end());
    c.erase(it, c.end());
    return r;
}

} // namespace sfl

#endif // SFL_COMPACT_VECTOR_HPP_INCLUDED
