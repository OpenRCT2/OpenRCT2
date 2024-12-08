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

#ifndef SFL_SEGMENTED_VECTOR_HPP_INCLUDED
#define SFL_SEGMENTED_VECTOR_HPP_INCLUDED

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

template < typename T,
           std::size_t N,
           typename Allocator = std::allocator<T> >
class segmented_vector
{
    static_assert(N > 0, "N must be greater than zero.");

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

private:

    using segment_allocator      = typename std::allocator_traits<allocator_type>::template rebind_alloc<pointer>;
    using segment_pointer        = typename std::allocator_traits<segment_allocator>::pointer;

public:

    using iterator               = sfl::dtl::segmented_iterator<segment_pointer, pointer,       N, segmented_vector>;
    using const_iterator         = sfl::dtl::segmented_iterator<segment_pointer, const_pointer, N, segmented_vector>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static_assert
    (
        std::is_same<typename Allocator::value_type, value_type>::value,
        "Allocator::value_type must be same as sfl::segmented_vector::value_type."
    );

public:

    static constexpr size_type segment_capacity = N;

private:

    class data_base
    {
    public:

        // ---- TABLE (OF SEGMENTS) ----

        segment_pointer table_first_; // First element in table
        segment_pointer table_last_;  // One-past-last element in table
        segment_pointer table_eos_;   // End of storage

        // ---- ELEMENTS IN VECTOR ----

        iterator first_; // First element in vector
        iterator last_;  // One-past-last element in vector
        iterator eos_;   // End of storage
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

    segmented_vector()
        : data_()
    {
        initialize_empty();
    }

    explicit segmented_vector(const Allocator& alloc)
        : data_(alloc)
    {
        initialize_empty();
    }

    segmented_vector(size_type n)
        : data_()
    {
        initialize_default_n(n);
    }

    explicit segmented_vector(size_type n, const Allocator& alloc)
        : data_(alloc)
    {
        initialize_default_n(n);
    }

    segmented_vector(size_type n, const T& value)
        : data_()
    {
        initialize_fill_n(n, value);
    }

    segmented_vector(size_type n, const T& value, const Allocator& alloc)
        : data_(alloc)
    {
        initialize_fill_n(n, value);
    }

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_input_iterator<InputIt>::value>* = nullptr>
    segmented_vector(InputIt first, InputIt last)
        : data_()
    {
        initialize_range(first, last);
    }

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_input_iterator<InputIt>::value>* = nullptr>
    segmented_vector(InputIt first, InputIt last, const Allocator& alloc)
        : data_(alloc)
    {
        initialize_range(first, last);
    }

    segmented_vector(std::initializer_list<T> ilist)
        : segmented_vector(ilist.begin(), ilist.end())
    {}

    segmented_vector(std::initializer_list<T> ilist, const Allocator& alloc)
        : segmented_vector(ilist.begin(), ilist.end(), alloc)
    {}

    segmented_vector(const segmented_vector& other)
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

    segmented_vector(const segmented_vector& other, const Allocator& alloc)
        : data_(alloc)
    {
        initialize_copy(other);
    }

    segmented_vector(segmented_vector&& other)
        : data_(std::move(other.data_.ref_to_alloc()))
    {
        initialize_move(other);
    }

    segmented_vector(segmented_vector&& other, const Allocator& alloc)
        : data_(alloc)
    {
        initialize_move(other);
    }

    ~segmented_vector()
    {
        sfl::dtl::destroy_a
        (
            data_.ref_to_alloc(),
            data_.first_,
            data_.last_
        );

        deallocate_storage();
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

    segmented_vector& operator=(const segmented_vector& other)
    {
        assign_copy(other);
        return *this;
    }

    segmented_vector& operator=(segmented_vector&& other)
    {
        assign_move(other);
        return *this;
    }

    segmented_vector& operator=(std::initializer_list<T> ilist)
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
        return data_.first_;
    }

    SFL_NODISCARD
    const_iterator begin() const noexcept
    {
        return data_.first_;
    }

    SFL_NODISCARD
    const_iterator cbegin() const noexcept
    {
        return data_.first_;
    }

    SFL_NODISCARD
    iterator end() noexcept
    {
        return data_.last_;
    }

    SFL_NODISCARD
    const_iterator end() const noexcept
    {
        return data_.last_;
    }

    SFL_NODISCARD
    const_iterator cend() const noexcept
    {
        return data_.last_;
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

        const auto i = pos / N;
        const auto j = pos - i * N;

        const auto seg = data_.table_first_ + i;
        const auto elem = *seg + j;

        return iterator(seg, elem);
    }

    SFL_NODISCARD
    const_iterator nth(size_type pos) const noexcept
    {
        SFL_ASSERT(pos <= size());

        const auto i = pos / N;
        const auto j = pos - i * N;

        const auto seg = data_.table_first_ + i;
        const auto elem = *seg + j;

        return iterator(seg, elem);
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
        return data_.last_ == data_.first_;
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
        return std::distance(data_.first_, data_.eos_);
    }

    SFL_NODISCARD
    size_type available() const noexcept
    {
        return std::distance(data_.last_, data_.eos_);
    }

    void reserve(size_type new_capacity)
    {
        const size_type capacity = this->capacity();

        if (new_capacity > capacity)
        {
            grow_storage(new_capacity - capacity);
        }
    }

    void shrink_to_fit()
    {
        shrink_storage();
    }

    //
    // ---- ELEMENT ACCESS ----------------------------------------------------
    //

    SFL_NODISCARD
    reference at(size_type pos)
    {
        if (pos >= size())
        {
            sfl::dtl::throw_out_of_range("sfl::segmented_vector::at");
        }

        const auto i = pos / N;
        const auto j = pos - i * N;

        return *(*(data_.table_first_ + i) + j);
    }

    SFL_NODISCARD
    const_reference at(size_type pos) const
    {
        if (pos >= size())
        {
            sfl::dtl::throw_out_of_range("sfl::segmented_vector::at");
        }

        const auto i = pos / N;
        const auto j = pos - i * N;

        return *(*(data_.table_first_ + i) + j);
    }

    SFL_NODISCARD
    reference operator[](size_type pos) noexcept
    {
        SFL_ASSERT(pos < size());

        const auto i = pos / N;
        const auto j = pos - i * N;

        return *(*(data_.table_first_ + i) + j);
    }

    SFL_NODISCARD
    const_reference operator[](size_type pos) const noexcept
    {
        SFL_ASSERT(pos < size());

        const auto i = pos / N;
        const auto j = pos - i * N;

        return *(*(data_.table_first_ + i) + j);
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
        return *(--iterator(data_.last_));
    }

    SFL_NODISCARD
    const_reference back() const noexcept
    {
        SFL_ASSERT(!empty());
        return *(--iterator(data_.last_));
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

        iterator p1 = begin() + std::distance(cbegin(), pos);

        if (data_.last_ == data_.eos_)
        {
            const size_type offset = std::distance(cbegin(), pos);
            grow_storage(1);
            p1 = nth(offset);
        }

        if (p1 == data_.last_)
        {
            sfl::dtl::construct_at_a
            (
                data_.ref_to_alloc(),
                std::addressof(*data_.last_),
                std::forward<Args>(args)...
            );

            ++data_.last_;
        }
        else
        {
            const iterator p2 = --iterator(data_.last_);

            const iterator old_last = data_.last_;

            // The order of operations is critical. First we will construct
            // temporary value because arguments `args...` can contain
            // reference to element in this container and after that
            // we will move elements and insert new element.

            value_type tmp(std::forward<Args>(args)...);

            sfl::dtl::construct_at_a
            (
                data_.ref_to_alloc(),
                std::addressof(*old_last),
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

        return p1;
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

    template <typename... Args>
    reference emplace_back(Args&&... args)
    {
        if (data_.last_ == data_.eos_)
        {
            grow_storage(1);
        }

        const iterator old_last = data_.last_;

        sfl::dtl::construct_at_a
        (
            data_.ref_to_alloc(),
            std::addressof(*data_.last_),
            std::forward<Args>(args)...
        );

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

        sfl::dtl::destroy_at_a(data_.ref_to_alloc(), std::addressof(*data_.last_));
    }

    iterator erase(const_iterator pos)
    {
        SFL_ASSERT(cbegin() <= pos && pos < cend());

        const iterator p = begin() + std::distance(cbegin(), pos);

        data_.last_ = sfl::dtl::move(p + 1, data_.last_, p);

        sfl::dtl::destroy_at_a(data_.ref_to_alloc(), std::addressof(*data_.last_));

        return p;
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        SFL_ASSERT(cbegin() <= first && first <= last && last <= cend());

        if (first == last)
        {
            return begin() + std::distance(cbegin(), first);
        }

        const iterator p1 = begin() + std::distance(cbegin(), first);
        const iterator p2 = begin() + std::distance(cbegin(), last);

        const iterator new_last = sfl::dtl::move(p2, data_.last_, p1);

        sfl::dtl::destroy_a(data_.ref_to_alloc(), new_last, data_.last_);

        data_.last_ = new_last;

        return p1;
    }

    void resize(size_type n)
    {
        const size_type size = this->size();

        if (n <= size)
        {
            const iterator new_last = nth(n);

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
            const size_type delta = n - size;

            const size_type capacity = this->capacity();

            if (n > capacity)
            {
                grow_storage(n - capacity);
            }

            data_.last_ = sfl::dtl::uninitialized_default_construct_n_a
            (
                data_.ref_to_alloc(),
                data_.last_,
                delta
            );
        }
    }

    void resize(size_type n, const T& value)
    {
        const size_type size = this->size();

        if (n <= size)
        {
            const iterator new_last = nth(n);

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
            const size_type delta = n - size;

            const size_type capacity = this->capacity();

            if (n > capacity)
            {
                grow_storage(n - capacity);
            }

            data_.last_ = sfl::dtl::uninitialized_fill_n_a
            (
                data_.ref_to_alloc(),
                data_.last_,
                delta,
                value
            );
        }
    }

    void swap(segmented_vector& other) noexcept
    {
        SFL_ASSERT
        (
            allocator_traits::propagate_on_container_swap::value ||
            this->data_.ref_to_alloc() == other.data_.ref_to_alloc()
        );

        if (this == &other)
        {
            return;
        }

        using std::swap;

        if (allocator_traits::propagate_on_container_swap::value)
        {
            swap(this->data_.ref_to_alloc(), other.data_.ref_to_alloc());
        }

        swap(this->data_, other.data_);
    }

private:

    // Allocates table for given number of elements (segments).
    // It does not construct any element (segment).
    // It only allocates memory for table.
    //
    segment_pointer allocate_table(size_type n)
    {
        segment_allocator seg_alloc(data_.ref_to_alloc());
        return sfl::dtl::allocate(seg_alloc, n);
    }

    // Deallocates table.
    // It does not destroy_a any element (segment).
    // It only deallocates memory used by table.
    //
    void deallocate_table(segment_pointer p, size_type n) noexcept
    {
        segment_allocator seg_alloc(data_.ref_to_alloc());
        sfl::dtl::deallocate(seg_alloc, p, n);
    }

    // Allocates memory for single segment.
    // It does not construct any element.
    // It only allocates memory for segment.
    //
    pointer allocate_segment()
    {
        return sfl::dtl::allocate(data_.ref_to_alloc(), N);
    }

    // Deallocates memory used by single segment.
    // It does not destroy_a any element.
    // It only deallocates memory used by segment.
    //
    void deallocate_segment(pointer p) noexcept
    {
        sfl::dtl::deallocate(data_.ref_to_alloc(), p, N);
    }

    // Allocates memory for multiple segments.
    // It does not construct any element.
    // It only allocates memory for segments.
    //
    void allocate_segments(segment_pointer first, segment_pointer last)
    {
        segment_pointer curr = first;

        SFL_TRY
        {
            while (curr != last)
            {
                *curr = allocate_segment();
                ++curr;
            }
        }
        SFL_CATCH (...)
        {
            deallocate_segments(first, curr);
            SFL_RETHROW;
        }
    }

    // Deallocates memory used by multiple segments.
    // It does not destroy_a any element.
    // It only deallocates memory used by segments.
    //
    void deallocate_segments(segment_pointer first, segment_pointer last) noexcept
    {
        while (first != last)
        {
            deallocate_segment(*first);
            ++first;
        }
    }

    static constexpr size_type min_table_capacity() noexcept
    {
        return 8;
    }

    // Allocates storage big enough to keep given number of elements.
    // It does not construct any element.
    // It only allocates memory.
    //
    void allocate_storage(size_type num_elements)
    {
        if (num_elements > max_size())
        {
            sfl::dtl::throw_length_error("sfl::segmented_vector::allocate_storage");
        }

        // Required capacity of table
        const size_type table_required =
            num_elements / N + 1;

        const size_type table_capacity =
            std::max(table_required, min_table_capacity());

        data_.table_first_ = allocate_table(table_capacity);
        data_.table_last_  = data_.table_first_ + table_required;
        data_.table_eos_   = data_.table_first_ + table_capacity;

        SFL_TRY
        {
            allocate_segments(data_.table_first_, data_.table_last_);

            data_.first_.segment_ =  data_.table_first_;
            data_.first_.local_   = *data_.table_first_;

            data_.last_ = data_.first_;

            data_.eos_.segment_ =  (data_.table_last_ - 1);
            data_.eos_.local_   = *(data_.table_last_ - 1) + (N - 1);
        }
        SFL_CATCH (...)
        {
            deallocate_table(data_.table_first_, table_capacity);
            SFL_RETHROW;
        }
    }

    // Deallocates storage.
    // It does not destroy_a any element.
    // It only deallocates memory.
    //
    void deallocate_storage() noexcept
    {
        deallocate_segments
        (
            data_.table_first_,
            data_.table_last_
        );

        deallocate_table
        (
            data_.table_first_,
            std::distance(data_.table_first_, data_.table_eos_)
        );
    }

    // Increases capacity for given number of elements.
    // It does not construct any element.
    // It only allocates memory.
    //
    void grow_storage(size_type num_additional_elements)
    {
        if (max_size() - capacity() < num_additional_elements)
        {
            sfl::dtl::throw_length_error("sfl::segmented_vector::grow_storage");
        }

        // Required capacity of table
        const size_type table_required =
            num_additional_elements / N + 1;

        // Available capacity of table
        const size_type table_available =
            std::distance(data_.table_last_, data_.table_eos_);

        // Increase table capacity if neccessary
        if (table_required > table_available)
        {
            const size_type table_capacity =
                std::distance(data_.table_first_, data_.table_eos_);

            const size_type new_table_capacity = std::max
            (
                table_capacity + table_capacity / 2,
                table_capacity - table_available + table_required
            );

            // Distance (in segments) from FIRST element to LAST element.
            const size_type dist1 =
                std::distance(data_.first_.segment_, data_.last_.segment_);

            // Distance (in segments) from FIRST element to END OF STORAGE.
            const size_type dist2 =
                std::distance(data_.first_.segment_, data_.eos_.segment_);

            // Allocate new table. No effects if allocation fails.
            const segment_pointer new_table_first =
                allocate_table(new_table_capacity);

            const segment_pointer new_table_eos =
                new_table_first + new_table_capacity;

            // Initialize LAST element in new table (noexecept).
            const segment_pointer new_table_last = sfl::dtl::copy
            (
                data_.table_first_,
                data_.table_last_,
                new_table_first
            );

            // Deallocate old table (noexcept).
            deallocate_table(data_.table_first_, table_capacity);

            // Update pointers (noexcept).
            data_.table_first_ = new_table_first;
            data_.table_last_  = new_table_last;
            data_.table_eos_   = new_table_eos;

            // Update iterators (noexcept).
            data_.first_.segment_ = new_table_first;
            data_.last_.segment_  = new_table_first + dist1;
            data_.eos_.segment_   = new_table_first + dist2;
        }

        const segment_pointer new_table_last =
            data_.table_last_ + table_required;

        // Allocate additional segments. No effects if allocation fails.
        allocate_segments(data_.table_last_, new_table_last);

        // Update table (noexcept).
        data_.table_last_ = new_table_last;

        // Update iterators (noexcept).
        data_.eos_.segment_ =   data_.table_last_ - 1;
        data_.eos_.local_   = *(data_.table_last_ - 1) + (N - 1);
    }

    // Removes unused capacity.
    // It does not destroy_a any element.
    // It only deallocates memory.
    //
    void shrink_storage()
    {
        // Destroy empty segments.
        {
            const segment_pointer new_table_last = data_.last_.segment_ + 1;

            deallocate_segments(new_table_last, data_.table_last_);

            data_.table_last_ = new_table_last;

            data_.eos_.segment_ =  (data_.table_last_ - 1);
            data_.eos_.local_   = *(data_.table_last_ - 1) + (N - 1);
        }

        // Shrink table.
        {
            const size_type table_capacity =
                std::distance(data_.table_first_, data_.table_eos_);

            const size_type table_size =
                std::distance(data_.table_first_, data_.table_last_);

            const size_type new_table_capacity =
                std::max(table_size, min_table_capacity());

            // Distance (in segments) from FIRST element to LAST element.
            const size_type dist =
                std::distance(data_.first_.segment_, data_.last_.segment_);

            // Allocate new table. No effects if allocation fails.
            const segment_pointer new_table_first =
                allocate_table(new_table_capacity);

            const segment_pointer new_table_eos =
                new_table_first + new_table_capacity;

            // Initialize LAST element in new table (noexecept).
            const segment_pointer new_table_last = sfl::dtl::copy
            (
                data_.table_first_,
                data_.table_last_,
                new_table_first
            );

            // Deallocate old table (noexcept).
            deallocate_table(data_.table_first_, table_capacity);

            // Update pointers (noexcept).
            data_.table_first_ = new_table_first;
            data_.table_last_  = new_table_last;
            data_.table_eos_   = new_table_eos;

            // Update iterators (noexcept).
            data_.first_.segment_ = data_.table_first_;
            data_.last_.segment_  = data_.table_first_ + dist;
            data_.eos_.segment_   = data_.table_last_ - 1;
        }
    }

    void initialize_empty()
    {
        allocate_storage(0);
    }

    void initialize_default_n(size_type n)
    {
        allocate_storage(n);

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
            deallocate_storage();
            SFL_RETHROW;
        }
    }

    void initialize_fill_n(size_type n, const T& value)
    {
        allocate_storage(n);

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
            deallocate_storage();
            SFL_RETHROW;
        }
    }

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_exactly_input_iterator<InputIt>::value>* = nullptr>
    void initialize_range(InputIt first, InputIt last)
    {
        allocate_storage(0);

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
            clear();
            deallocate_storage();
            SFL_RETHROW;
        }
    }

    template <typename ForwardIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_forward_iterator<ForwardIt>::value>* = nullptr>
    void initialize_range(ForwardIt first, ForwardIt last)
    {
        allocate_storage(std::distance(first, last));

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
            deallocate_storage();
            SFL_RETHROW;
        }
    }

    void initialize_copy(const segmented_vector& other)
    {
        allocate_storage(other.size());

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
            deallocate_storage();
            SFL_RETHROW;
        }
    }

    void initialize_move(segmented_vector& other)
    {
        if (data_.ref_to_alloc() == other.data_.ref_to_alloc())
        {
            allocate_storage(0);
            using std::swap;
            swap(data_, other.data_);
        }
        else
        {
            allocate_storage(other.size());

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
                deallocate_storage();
                SFL_RETHROW;
            }
        }
    }

    void assign_fill_n(size_type n, const T& value)
    {
        const size_type size = this->size();

        if (n <= size)
        {
            const iterator new_last = data_.first_ + n;

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
            const size_type capacity = this->capacity();

            if (n > capacity)
            {
                grow_storage(n - capacity);
            }

            sfl::dtl::fill
            (
                data_.first_,
                data_.first_ + size,
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
    }

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_exactly_input_iterator<InputIt>::value>* = nullptr>
    void assign_range(InputIt first, InputIt last)
    {
        iterator curr = data_.first_;

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

    template <typename ForwardIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_forward_iterator<ForwardIt>::value>* = nullptr>
    void assign_range(ForwardIt first, ForwardIt last)
    {
        const size_type n = std::distance(first, last);

        const size_type size = this->size();

        if (n <= size)
        {
            const iterator new_last = sfl::dtl::copy
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
            const size_type capacity = this->capacity();

            if (n > capacity)
            {
                grow_storage(n - capacity);
            }

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
    }

    void assign_copy(const segmented_vector& other)
    {
        if (this != &other)
        {
            if (allocator_traits::propagate_on_container_copy_assignment::value)
            {
                if (data_.ref_to_alloc() != other.data_.ref_to_alloc())
                {
                    // Create temporary vector using other allocator.
                    // There are no effects if allocation fails.
                    segmented_vector temp(other.data_.ref_to_alloc());

                    // Clear and destroy_a current storage (noexcept).
                    clear();
                    deallocate_storage();

                    // Set pointers to null (noexcept).
                    data_.table_first_ = nullptr;
                    data_.table_last_  = nullptr;
                    data_.table_eos_   = nullptr;

                    // Swap storage of this and temporary vector (noexcept).
                    using std::swap;
                    swap(data_, temp.data_);

                    // Temporary vector has no allocated storage (pointers
                    // are set to null) so destructor of temporary vector
                    // has nothing to do.
                }

                // Propagate allocator (noexcept).
                data_.ref_to_alloc() = other.data_.ref_to_alloc();
            }

            assign_range
            (
                other.data_.first_,
                other.data_.last_
            );
        }
    }

    void assign_move(segmented_vector& other)
    {
        using std::swap;

        if (data_.ref_to_alloc() == other.data_.ref_to_alloc())
        {
            // Create temporary container using allocator of "this".
            // There are no effects if allocation fails.
            // NOTE: We could also use allocator of "other". There are no
            // difference since both allocators compare equal.
            segmented_vector temp(data_.ref_to_alloc());

            // Clear storage of "this" (noexcept).
            clear();

            // Destroy storage of "this" (noexcept).
            // NOTE: This does not set pointers to null.
            deallocate_storage();

            // Set pointers of "this" to null pointers (noexcept).
            data_.table_first_ = nullptr;
            data_.table_last_  = nullptr;
            data_.table_eos_   = nullptr;

            // Current state:
            //  - "this" has no allocated storage (pointers are null).

            // Swap storage of "this" and "other" (noexcept)
            swap(data_, other.data_);

            // After swap:
            //  - "this" owns storage previously owned by "other".
            //  - "other" has no allocated storage (pointers are null).

            // Swap storage of "other" and "temp" (noexcept)
            swap(other.data_, temp.data_);

            // After swap:
            //  - "other" owns storage previously owned by "temp".
            //  - "temp" has no allocated storage (pointers are null).
            //    This is OK in this case. Destructor of "temp" has
            //    nothing to do.

            if (allocator_traits::propagate_on_container_move_assignment::value)
            {
                // Propagate allocator (noexcept).
                data_.ref_to_alloc() = std::move(other.data_.ref_to_alloc());
            }
        }
        else if (allocator_traits::propagate_on_container_move_assignment::value)
        {
            // Create temporary container using allocator of "other".
            // There are no effects if allocation fails.
            segmented_vector temp(other.data_.ref_to_alloc());

            // Clear storage of "this" (noexcept).
            clear();

            // Destroy storage of "this" (noexcept).
            // NOTE: This does not set pointers to null.
            deallocate_storage();

            // Set pointers of "this" to null pointers (noexcept).
            data_.table_first_ = nullptr;
            data_.table_last_  = nullptr;
            data_.table_eos_   = nullptr;

            // Current state:
            //  - "this" has no allocated storage (pointers are null).

            // Swap storage of "this" and "temp" (noexcept)
            swap(data_, temp.data_);

            // After swap:
            //  - "this" owns storage previously owned by "temp".
            //    That storage was allocated by allocator of "temp"
            //    (i.e. copy of allocator of "other").
            //    "this" cannot deallocate that storage because allocators
            //    of "this" and "other" does not compare equal.
            //  - "temp" has no allocated storage (pointers are null).
            //    This is OK in this case. Destructor of "temp" has
            //    nothing to do.

            // Propagate allocator (noexcept).
            data_.ref_to_alloc() = std::move(other.data_.ref_to_alloc());

            // After propagation:
            //  - "this" owns storage previously owned by "temp", but now
            //    "this" can deallocate that storage.

            // Move elements one-by-one from "other" to "this" (can throw)
            assign_range
            (
                std::make_move_iterator(other.data_.first_),
                std::make_move_iterator(other.data_.last_)
            );
        }
        else
        {
            // Move elements one-by-one from "other" to "this" (can throw)
            assign_range
            (
                std::make_move_iterator(other.data_.first_),
                std::make_move_iterator(other.data_.last_)
            );
        }
    }

    iterator insert_fill_n(const_iterator pos, size_type n, const T& value)
    {
        if (n == 0)
        {
            return begin() + std::distance(cbegin(), pos);
        }

        const value_type tmp(value);

        const size_type dist_to_begin = std::distance(cbegin(), pos);

        const size_type available = this->available();

        if (available < n)
        {
            grow_storage(n - available);
        }

        const iterator p1 = data_.first_ + dist_to_begin;
        const iterator p2 = p1 + n;

        if (p2 <= data_.last_)
        {
            const iterator p3 = data_.last_ - n;

            const iterator old_last = data_.last_;

            data_.last_ = sfl::dtl::uninitialized_move_a
            (
                data_.ref_to_alloc(),
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
            const iterator old_last = data_.last_;

            sfl::dtl::uninitialized_fill_a
            (
                data_.ref_to_alloc(),
                data_.last_,
                p2,
                tmp
            );

            data_.last_ = p2;

            data_.last_ = sfl::dtl::uninitialized_move_a
            (
                data_.ref_to_alloc(),
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

        return p1;
    }

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_exactly_input_iterator<InputIt>::value>* = nullptr>
    iterator insert_range(const_iterator pos, InputIt first, InputIt last)
    {
        const size_type offset = std::distance(cbegin(), pos);

        while (first != last)
        {
            pos = insert(pos, *first);
            ++pos;
            ++first;
        }

        return nth(offset);
    }

    template <typename ForwardIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_forward_iterator<ForwardIt>::value>* = nullptr>
    iterator insert_range(const_iterator pos, ForwardIt first, ForwardIt last)
    {
        if (first == last)
        {
            return begin() + std::distance(cbegin(), pos);
        }

        const size_type n = std::distance(first, last);

        const size_type dist_to_begin = std::distance(cbegin(), pos);
        const size_type dist_to_end   = std::distance(pos, cend());

        const size_type available = this->available();

        if (available < n)
        {
            grow_storage(n - available);
        }

        const iterator p1 = data_.first_ + dist_to_begin;
        const iterator p2 = p1 + n;

        if (p2 <= data_.last_)
        {
            const iterator p3 = data_.last_ - n;

            const iterator old_last = data_.last_;

            data_.last_ = sfl::dtl::uninitialized_move_a
            (
                data_.ref_to_alloc(),
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
            const iterator old_last = data_.last_;

            const ForwardIt mid = std::next(first, dist_to_end);

            data_.last_ = sfl::dtl::uninitialized_copy_a
            (
                data_.ref_to_alloc(),
                mid,
                last,
                data_.last_
            );

            data_.last_ = sfl::dtl::uninitialized_move_a
            (
                data_.ref_to_alloc(),
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

        return p1;
    }
};

//
// ---- NON-MEMBER FUNCTIONS --------------------------------------------------
//

template <typename T, std::size_t N, typename A>
SFL_NODISCARD
bool operator==
(
    const segmented_vector<T, N, A>& x,
    const segmented_vector<T, N, A>& y
)
{
    return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
}

template <typename T, std::size_t N, typename A>
SFL_NODISCARD
bool operator!=
(
    const segmented_vector<T, N, A>& x,
    const segmented_vector<T, N, A>& y
)
{
    return !(x == y);
}

template <typename T, std::size_t N, typename A>
SFL_NODISCARD
bool operator<
(
    const segmented_vector<T, N, A>& x,
    const segmented_vector<T, N, A>& y
)
{
    return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <typename T, std::size_t N, typename A>
SFL_NODISCARD
bool operator>
(
    const segmented_vector<T, N, A>& x,
    const segmented_vector<T, N, A>& y
)
{
    return y < x;
}

template <typename T, std::size_t N, typename A>
SFL_NODISCARD
bool operator<=
(
    const segmented_vector<T, N, A>& x,
    const segmented_vector<T, N, A>& y
)
{
    return !(y < x);
}

template <typename T, std::size_t N, typename A>
SFL_NODISCARD
bool operator>=
(
    const segmented_vector<T, N, A>& x,
    const segmented_vector<T, N, A>& y
)
{
    return !(x < y);
}

template <typename T, std::size_t N, typename A>
void swap
(
    segmented_vector<T, N, A>& x,
    segmented_vector<T, N, A>& y
)
{
    x.swap(y);
}

template <typename T, std::size_t N, typename A, typename U>
typename segmented_vector<T, N, A>::size_type
    erase(segmented_vector<T, N, A>& c, const U& value)
{
    auto it = std::remove(c.begin(), c.end(), value);
    auto r = std::distance(it, c.end());
    c.erase(it, c.end());
    return r;
}

template <typename T, std::size_t N, typename A, typename Predicate>
typename segmented_vector<T, N, A>::size_type
    erase_if(segmented_vector<T, N, A>& c, Predicate pred)
{
    auto it = std::remove_if(c.begin(), c.end(), pred);
    auto r = std::distance(it, c.end());
    c.erase(it, c.end());
    return r;
}

} // namespace sfl

#endif // SFL_SEGMENTED_VECTOR_HPP_INCLUDED
