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

#ifndef SFL_STATIC_FLAT_MULTISET_HPP_INCLUDED
#define SFL_STATIC_FLAT_MULTISET_HPP_INCLUDED

#include <sfl/detail/container_compatible_range.hpp>
#include <sfl/detail/cpp.hpp>
#include <sfl/detail/initialized_memory_algorithms.hpp>
#include <sfl/detail/normal_iterator.hpp>
#include <sfl/detail/tags.hpp>
#include <sfl/detail/type_traits.hpp>
#include <sfl/detail/uninitialized_memory_algorithms.hpp>

#include <algorithm>        // copy, move, lower_bound, swap, swap_ranges
#include <cstddef>          // size_t
#include <functional>       // equal_to, less
#include <initializer_list> // initializer_list
#include <iterator>         // distance, next, reverse_iterator
#include <limits>           // numeric_limits
#include <memory>           // allocator, allocator_traits, pointer_traits
#include <type_traits>      // is_same, is_nothrow_xxxxx
#include <utility>          // forward, move, pair

#ifdef SFL_TEST_STATIC_FLAT_MULTISET
void test_static_flat_multiset();
#endif

namespace sfl
{

template < typename Key,
           std::size_t N,
           typename Compare = std::less<Key> >
class static_flat_multiset
{
    #ifdef SFL_TEST_STATIC_FLAT_MULTISET
    friend void ::test_static_flat_multiset();
    #endif

    static_assert(N > 0, "N must be greater than zero.");

public:

    using key_type               = Key;
    using value_type             = Key;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using key_compare            = Compare;
    using value_compare          = Compare;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using pointer                = value_type*;
    using const_pointer          = const value_type*;
    using iterator               = sfl::dtl::normal_iterator<const_pointer, static_flat_multiset>; // MUST BE const_pointer
    using const_iterator         = sfl::dtl::normal_iterator<const_pointer, static_flat_multiset>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:

    static constexpr size_type static_capacity = N;

private:

    class data_base
    {
    public:

        union
        {
            value_type first_[N];
        };

        pointer last_;

        data_base() noexcept
            : last_(first_)
        {}

        #if defined(__clang__) && (__clang_major__ == 3) // For CentOS 7
        ~data_base()
        {}
        #else
        ~data_base() noexcept
        {}
        #endif
    };

    class data : public data_base, public value_compare
    {
    public:

        data() noexcept(std::is_nothrow_default_constructible<value_compare>::value)
            : value_compare()
        {}

        data(const value_compare& comp) noexcept(std::is_nothrow_copy_constructible<value_compare>::value)
            : value_compare(comp)
        {}

        data(value_compare&& comp) noexcept(std::is_nothrow_move_constructible<value_compare>::value)
            : value_compare(std::move(comp))
        {}

        value_compare& ref_to_comp() noexcept
        {
            return *this;
        }

        const value_compare& ref_to_comp() const noexcept
        {
            return *this;
        }
    };

    data data_;

public:

    //
    // ---- CONSTRUCTION AND DESTRUCTION --------------------------------------
    //

    static_flat_multiset() noexcept(std::is_nothrow_default_constructible<Compare>::value)
        : data_()
    {}

    explicit static_flat_multiset(const Compare& comp) noexcept(std::is_nothrow_copy_constructible<Compare>::value)
        : data_(comp)
    {}

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_input_iterator<InputIt>::value>* = nullptr>
    static_flat_multiset(InputIt first, InputIt last)
        : data_()
    {
        initialize_range(first, last);
    }

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_input_iterator<InputIt>::value>* = nullptr>
    static_flat_multiset(InputIt first, InputIt last, const Compare& comp)
        : data_(comp)
    {
        initialize_range(first, last);
    }

    static_flat_multiset(std::initializer_list<value_type> ilist)
        : static_flat_multiset(ilist.begin(), ilist.end())
    {}

    static_flat_multiset(std::initializer_list<value_type> ilist, const Compare& comp)
        : static_flat_multiset(ilist.begin(), ilist.end(), comp)
    {}

    static_flat_multiset(const static_flat_multiset& other)
        : data_(other.data_.ref_to_comp())
    {
        data_.last_ = sfl::dtl::uninitialized_copy
        (
            pointer(other.data_.first_),
            pointer(other.data_.last_),
            data_.first_
        );
    }

    static_flat_multiset(static_flat_multiset&& other)
        : data_(std::move(other.data_.ref_to_comp()))
    {
        data_.last_ = sfl::dtl::uninitialized_move
        (
            std::make_move_iterator(pointer(other.data_.first_)),
            std::make_move_iterator(pointer(other.data_.last_)),
            data_.first_
        );
    }

#if SFL_CPP_VERSION >= SFL_CPP_20

    template <sfl::dtl::container_compatible_range<value_type> Range>
    static_flat_multiset(sfl::from_range_t, Range&& range)
        : data_()
    {
        initialize_range(std::forward<Range>(range));
    }

    template <sfl::dtl::container_compatible_range<value_type> Range>
    static_flat_multiset(sfl::from_range_t, Range&& range, const Compare& comp)
        : data_(comp)
    {
        initialize_range(std::forward<Range>(range));
    }

#else // before C++20

    template <typename Range>
    static_flat_multiset(sfl::from_range_t, Range&& range)
        : data_()
    {
        initialize_range(std::forward<Range>(range));
    }

    template <typename Range>
    static_flat_multiset(sfl::from_range_t, Range&& range, const Compare& comp)
        : data_(comp)
    {
        initialize_range(std::forward<Range>(range));
    }

#endif // before C++20

    ~static_flat_multiset()
    {
        sfl::dtl::destroy(data_.first_, data_.last_);
    }

    //
    // ---- ASSIGNMENT --------------------------------------------------------
    //

    static_flat_multiset& operator=(const static_flat_multiset& other)
    {
        if (this != &other)
        {
            data_.ref_to_comp() = other.data_.ref_to_comp();

            assign_range
            (
                pointer(other.data_.first_),
                pointer(other.data_.last_)
            );
        }

        return *this;
    }

    static_flat_multiset& operator=(static_flat_multiset&& other)
    {
        data_.ref_to_comp() = other.data_.ref_to_comp();

        assign_range
        (
            std::make_move_iterator(pointer(other.data_.first_)),
            std::make_move_iterator(pointer(other.data_.last_))
        );

        return *this;
    }

    static_flat_multiset& operator=(std::initializer_list<value_type> ilist)
    {
        assign_range(ilist.begin(), ilist.end());
        return *this;
    }

    //
    // ---- KEY COMPARE -------------------------------------------------------
    //

    SFL_NODISCARD
    key_compare key_comp() const
    {
        return data_.ref_to_comp();
    }

    //
    // ---- VALUE COMPARE -----------------------------------------------------
    //

    SFL_NODISCARD
    value_compare value_comp() const
    {
        return data_.ref_to_comp();
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
    // ---- MODIFIERS ---------------------------------------------------------
    //

    void clear() noexcept
    {
        sfl::dtl::destroy(data_.first_, data_.last_);
        data_.last_ = data_.first_;
    }

    template <typename... Args>
    iterator emplace(Args&&... args)
    {
        SFL_ASSERT(!full());
        return insert_aux(value_type(std::forward<Args>(args)...));
    }

    template <typename... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args)
    {
        SFL_ASSERT(!full());
        SFL_ASSERT(cbegin() <= hint && hint <= cend());
        return insert_aux(hint, value_type(std::forward<Args>(args)...));
    }

    iterator insert(const value_type& value)
    {
        SFL_ASSERT(!full());
        return insert_aux(value);
    }

    iterator insert(value_type&& value)
    {
        SFL_ASSERT(!full());
        return insert_aux(std::move(value));
    }

    iterator insert(const_iterator hint, const value_type& value)
    {
        SFL_ASSERT(!full());
        SFL_ASSERT(cbegin() <= hint && hint <= cend());
        return insert_aux(hint, value);
    }

    iterator insert(const_iterator hint, value_type&& value)
    {
        SFL_ASSERT(!full());
        SFL_ASSERT(cbegin() <= hint && hint <= cend());
        return insert_aux(hint, std::move(value));
    }

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_input_iterator<InputIt>::value>* = nullptr>
    void insert(InputIt first, InputIt last)
    {
        insert_range_aux(first, last);
    }

    void insert(std::initializer_list<value_type> ilist)
    {
        insert_range_aux(ilist.begin(), ilist.end());
    }

#if SFL_CPP_VERSION >= SFL_CPP_20

    template <sfl::dtl::container_compatible_range<value_type> Range>
    void insert_range(Range&& range)
    {
        insert_range_aux(std::ranges::begin(range), std::ranges::end(range));
    }

#else // before C++20

    template <typename Range>
    void insert_range(Range&& range)
    {
        using std::begin;
        using std::end;
        insert_range_aux(begin(range), end(range));
    }

#endif // before C++20

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

    size_type erase(const Key& key)
    {
        const auto er = equal_range(key);
        const auto n = std::distance(er.first, er.second);
        erase(er.first, er.second);
        return n;
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    size_type erase(K&& x)
    {
        const auto er = equal_range(x);
        const auto n = std::distance(er.first, er.second);
        erase(er.first, er.second);
        return n;
    }

    void swap(static_flat_multiset& other)
    {
        if (this == &other)
        {
            return;
        }

        using std::swap;

        swap(this->data_.ref_to_comp(), other.data_.ref_to_comp());

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

    //
    // ---- LOOKUP ------------------------------------------------------------
    //

    SFL_NODISCARD
    iterator lower_bound(const Key& key)
    {
        return std::lower_bound(begin(), end(), key, data_.ref_to_comp());
    }

    SFL_NODISCARD
    const_iterator lower_bound(const Key& key) const
    {
        return std::lower_bound(begin(), end(), key, data_.ref_to_comp());
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    iterator lower_bound(const K& x)
    {
        return std::lower_bound(begin(), end(), x, data_.ref_to_comp());
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    const_iterator lower_bound(const K& x) const
    {
        return std::lower_bound(begin(), end(), x, data_.ref_to_comp());
    }

    SFL_NODISCARD
    iterator upper_bound(const Key& key)
    {
        return std::upper_bound(begin(), end(), key, data_.ref_to_comp());
    }

    SFL_NODISCARD
    const_iterator upper_bound(const Key& key) const
    {
        return std::upper_bound(begin(), end(), key, data_.ref_to_comp());
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    iterator upper_bound(const K& x)
    {
        return std::upper_bound(begin(), end(), x, data_.ref_to_comp());
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    const_iterator upper_bound(const K& x) const
    {
        return std::upper_bound(begin(), end(), x, data_.ref_to_comp());
    }

    SFL_NODISCARD
    std::pair<iterator, iterator> equal_range(const Key& key)
    {
        return std::equal_range(begin(), end(), key, data_.ref_to_comp());
    }

    SFL_NODISCARD
    std::pair<const_iterator, const_iterator> equal_range(const Key& key) const
    {
        return std::equal_range(begin(), end(), key, data_.ref_to_comp());
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    std::pair<iterator, iterator> equal_range(const K& x)
    {
        return std::equal_range(begin(), end(), x, data_.ref_to_comp());
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    std::pair<const_iterator, const_iterator> equal_range(const K& x) const
    {
        return std::equal_range(begin(), end(), x, data_.ref_to_comp());
    }

    SFL_NODISCARD
    iterator find(const Key& key)
    {
        auto it = lower_bound(key);

        if (it != end() && data_.ref_to_comp()(key, *it))
        {
            it = end();
        }

        return it;
    }

    SFL_NODISCARD
    const_iterator find(const Key& key) const
    {
        auto it = lower_bound(key);

        if (it != end() && data_.ref_to_comp()(key, *it))
        {
            it = end();
        }

        return it;
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    iterator find(const K& x)
    {
        auto it = lower_bound(x);

        if (it != end() && data_.ref_to_comp()(x, *it))
        {
            it = end();
        }

        return it;
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    const_iterator find(const K& x) const
    {
        auto it = lower_bound(x);

        if (it != end() && data_.ref_to_comp()(x, *it))
        {
            it = end();
        }

        return it;
    }

    SFL_NODISCARD
    size_type count(const Key& key) const
    {
        const auto er = equal_range(key);
        return std::distance(er.first, er.second);
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    size_type count(const K& x) const
    {
        const auto er = equal_range(x);
        return std::distance(er.first, er.second);
    }

    SFL_NODISCARD
    bool contains(const Key& key) const
    {
        return find(key) != end();
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    bool contains(const K& x) const
    {
        return find(x) != end();
    }

    //
    // ---- ELEMENT ACCESS ----------------------------------------------------
    //

    SFL_NODISCARD
    value_type* data() noexcept
    {
        return data_.first_;
    }

    SFL_NODISCARD
    const value_type* data() const noexcept
    {
        return data_.first_;
    }

private:

    template <typename InputIt, typename Sentinel>
    void initialize_range(InputIt first, Sentinel last)
    {
        SFL_TRY
        {
            while (first != last)
            {
                insert(*first);
                ++first;
            }
        }
        SFL_CATCH (...)
        {
            sfl::dtl::destroy(data_.first_, data_.last_);
            SFL_RETHROW;
        }
    }

#if SFL_CPP_VERSION >= SFL_CPP_20

    template <sfl::dtl::container_compatible_range<value_type> Range>
    void initialize_range(Range&& range)
    {
        initialize_range(std::ranges::begin(range), std::ranges::end(range));
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

    template <typename ForwardIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_forward_iterator<ForwardIt>::value>* = nullptr>
    void assign_range(ForwardIt first, ForwardIt last)
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

    template <typename Value>
    iterator insert_aux(Value&& value)
    {
        return insert_exactly_at(lower_bound(value), std::forward<Value>(value));
    }

    template <typename Value>
    iterator insert_aux(const_iterator hint, Value&& value)
    {
        if (is_insert_hint_good(hint, value))
        {
            return insert_exactly_at(hint, std::forward<Value>(value));
        }

        // Hint is not good. Use non-hinted function.
        return insert_aux(std::forward<Value>(value));
    }

    template <typename InputIt, typename Sentinel>
    void insert_range_aux(InputIt first, Sentinel last)
    {
        while (first != last)
        {
            insert(*first);
            ++first;
        }
    }

    template <typename Value>
    iterator insert_exactly_at(const_iterator pos, Value&& value)
    {
        SFL_ASSERT(!full());
        SFL_ASSERT(cbegin() <= pos && pos <= cend());

        const pointer p1 = data_.first_ + std::distance(cbegin(), pos);

        if (p1 == data_.last_)
        {
            sfl::dtl::construct_at
            (
                p1,
                std::forward<Value>(value)
            );

            ++data_.last_;
        }
        else
        {
            // This container can contain duplicates.
            // Create temporary value before making place for new element.
            value_type temp(std::forward<Value>(value));

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

            *p1 = std::move(temp);
        }

        return iterator(p1);
    }

    template <typename Value>
    bool is_insert_hint_good(const_iterator hint, const Value& value)
    {
        return
            // If `hint` == `value`
            (
                hint != end() &&
                !data_.ref_to_comp()(*hint, value) &&
                !data_.ref_to_comp()(value, *hint)
            )
            ||
            // If `hint - 1` == `value`
            (
                hint != begin() &&
                !data_.ref_to_comp()(*(hint - 1), value) &&
                !data_.ref_to_comp()(value, *(hint - 1))
            )
            ||
            // If `hint - 1` < `value` and `value` < `hint`
            (
                (hint == begin() || data_.ref_to_comp()(*(hint - 1), value)) &&
                (hint == end()   || data_.ref_to_comp()(value, *hint))
            );
    }
};

//
// ---- NON-MEMBER FUNCTIONS --------------------------------------------------
//

template <typename K, std::size_t N, typename C>
SFL_NODISCARD
bool operator==
(
    const static_flat_multiset<K, N, C>& x,
    const static_flat_multiset<K, N, C>& y
)
{
    return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
}

template <typename K, std::size_t N, typename C>
SFL_NODISCARD
bool operator!=
(
    const static_flat_multiset<K, N, C>& x,
    const static_flat_multiset<K, N, C>& y
)
{
    return !(x == y);
}

template <typename K, std::size_t N, typename C>
SFL_NODISCARD
bool operator<
(
    const static_flat_multiset<K, N, C>& x,
    const static_flat_multiset<K, N, C>& y
)
{
    return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <typename K, std::size_t N, typename C>
SFL_NODISCARD
bool operator>
(
    const static_flat_multiset<K, N, C>& x,
    const static_flat_multiset<K, N, C>& y
)
{
    return y < x;
}

template <typename K, std::size_t N, typename C>
SFL_NODISCARD
bool operator<=
(
    const static_flat_multiset<K, N, C>& x,
    const static_flat_multiset<K, N, C>& y
)
{
    return !(y < x);
}

template <typename K, std::size_t N, typename C>
SFL_NODISCARD
bool operator>=
(
    const static_flat_multiset<K, N, C>& x,
    const static_flat_multiset<K, N, C>& y
)
{
    return !(x < y);
}

template <typename K, std::size_t N, typename C>
void swap
(
    static_flat_multiset<K, N, C>& x,
    static_flat_multiset<K, N, C>& y
)
{
    x.swap(y);
}

template <typename K, std::size_t N, typename C, typename Predicate>
typename static_flat_multiset<K, N, C>::size_type
erase_if(static_flat_multiset<K, N, C>& c, Predicate pred)
{
    auto old_size = c.size();

    for (auto it = c.begin(); it != c.end(); )
    {
        if (pred(*it))
        {
            it = c.erase(it);
        }
        else
        {
            ++it;
        }
    }

    return old_size - c.size();
}

} // namespace sfl

#endif // SFL_STATIC_FLAT_MULTISET_HPP_INCLUDED
