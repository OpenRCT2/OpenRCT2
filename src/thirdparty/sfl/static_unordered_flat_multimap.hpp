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

#ifndef SFL_STATIC_UNORDERED_FLAT_MULTIMAP_HPP_INCLUDED
#define SFL_STATIC_UNORDERED_FLAT_MULTIMAP_HPP_INCLUDED

#include <sfl/detail/container_compatible_range.hpp>
#include <sfl/detail/cpp.hpp>
#include <sfl/detail/ignore_unused.hpp>
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

#ifdef SFL_TEST_STATIC_UNORDERED_FLAT_MULTIMAP
void test_static_unordered_flat_multimap();
#endif

namespace sfl
{

template < typename Key,
           typename T,
           std::size_t N,
           typename KeyEqual = std::equal_to<Key> >
class static_unordered_flat_multimap
{
    #ifdef SFL_TEST_STATIC_UNORDERED_FLAT_MULTIMAP
    friend void ::test_static_unordered_flat_multimap();
    #endif

    static_assert(N > 0, "N must be greater than zero.");

public:

    using key_type         = Key;
    using mapped_type      = T;
    using value_type       = std::pair<Key, T>;
    using size_type        = std::size_t;
    using difference_type  = std::ptrdiff_t;
    using key_equal        = KeyEqual;
    using reference        = value_type&;
    using const_reference  = const value_type&;
    using pointer          = value_type*;
    using const_pointer    = const value_type*;
    using iterator         = sfl::dtl::normal_iterator<pointer, static_unordered_flat_multimap>;
    using const_iterator   = sfl::dtl::normal_iterator<const_pointer, static_unordered_flat_multimap>;

    class value_equal : protected key_equal
    {
        friend class static_unordered_flat_multimap;

    private:

        value_equal(const key_equal& e)
            : key_equal(e)
        {}

    public:

        bool operator()(const value_type& x, const value_type& y) const
        {
            return key_equal::operator()(x.first, y.first);
        }
    };

public:

    static constexpr size_type static_capacity = N;

private:

    // Like `value_equal` but with additional operators.
    // For internal use only.
    class ultra_equal : public key_equal
    {
    public:

        ultra_equal() noexcept(std::is_nothrow_default_constructible<key_equal>::value)
        {}

        ultra_equal(const key_equal& e) noexcept(std::is_nothrow_copy_constructible<key_equal>::value)
            : key_equal(e)
        {}

        ultra_equal(key_equal&& e) noexcept(std::is_nothrow_move_constructible<key_equal>::value)
            : key_equal(std::move(e))
        {}

        bool operator()(const value_type& x, const value_type& y) const
        {
            return key_equal::operator()(x.first, y.first);
        }

        template <typename K>
        bool operator()(const value_type& x, const K& y) const
        {
            return key_equal::operator()(x.first, y);
        }

        template <typename K>
        bool operator()(const K& x, const value_type& y) const
        {
            return key_equal::operator()(x, y.first);
        }
    };

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

    class data : public data_base, public ultra_equal
    {
    public:

        data() noexcept(std::is_nothrow_default_constructible<ultra_equal>::value)
            : ultra_equal()
        {}

        data(const ultra_equal& equal) noexcept(std::is_nothrow_copy_constructible<ultra_equal>::value)
            : ultra_equal(equal)
        {}

        data(ultra_equal&& equal) noexcept(std::is_nothrow_move_constructible<ultra_equal>::value)
            : ultra_equal(std::move(equal))
        {}

        ultra_equal& ref_to_equal() noexcept
        {
            return *this;
        }

        const ultra_equal& ref_to_equal() const noexcept
        {
            return *this;
        }
    };

    data data_;

public:

    //
    // ---- CONSTRUCTION AND DESTRUCTION --------------------------------------
    //

    static_unordered_flat_multimap() noexcept(std::is_nothrow_default_constructible<KeyEqual>::value)
        : data_()
    {}

    explicit static_unordered_flat_multimap(const KeyEqual& equal) noexcept(std::is_nothrow_copy_constructible<KeyEqual>::value)
        : data_(equal)
    {}

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_input_iterator<InputIt>::value>* = nullptr>
    static_unordered_flat_multimap(InputIt first, InputIt last)
        : data_()
    {
        initialize_range(first, last);
    }

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_input_iterator<InputIt>::value>* = nullptr>
    static_unordered_flat_multimap(InputIt first, InputIt last, const KeyEqual& equal)
        : data_(equal)
    {
        initialize_range(first, last);
    }

    static_unordered_flat_multimap(std::initializer_list<value_type> ilist)
        : static_unordered_flat_multimap(ilist.begin(), ilist.end())
    {}

    static_unordered_flat_multimap(std::initializer_list<value_type> ilist, const KeyEqual& equal)
        : static_unordered_flat_multimap(ilist.begin(), ilist.end(), equal)
    {}

    static_unordered_flat_multimap(const static_unordered_flat_multimap& other)
        : data_(other.data_.ref_to_equal())
    {
        data_.last_ = sfl::dtl::uninitialized_copy
        (
            pointer(other.data_.first_),
            pointer(other.data_.last_),
            data_.first_
        );
    }

    static_unordered_flat_multimap(static_unordered_flat_multimap&& other)
        : data_(std::move(other.data_.ref_to_equal()))
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
    static_unordered_flat_multimap(sfl::from_range_t, Range&& range)
        : data_()
    {
        initialize_range(std::forward<Range>(range));
    }

    template <sfl::dtl::container_compatible_range<value_type> Range>
    static_unordered_flat_multimap(sfl::from_range_t, Range&& range, const KeyEqual& equal)
        : data_(equal)
    {
        initialize_range(std::forward<Range>(range));
    }

#else // before C++20

    template <typename Range>
    static_unordered_flat_multimap(sfl::from_range_t, Range&& range)
        : data_()
    {
        initialize_range(std::forward<Range>(range));
    }

    template <typename Range>
    static_unordered_flat_multimap(sfl::from_range_t, Range&& range, const KeyEqual& equal)
        : data_(equal)
    {
        initialize_range(std::forward<Range>(range));
    }

#endif // before C++20

    ~static_unordered_flat_multimap()
    {
        sfl::dtl::destroy(data_.first_, data_.last_);
    }

    //
    // ---- ASSIGNMENT --------------------------------------------------------
    //

    static_unordered_flat_multimap& operator=(const static_unordered_flat_multimap& other)
    {
        if (this != &other)
        {
            data_.ref_to_equal() = other.data_.ref_to_equal();

            assign_range
            (
                pointer(other.data_.first_),
                pointer(other.data_.last_)
            );
        }

        return *this;
    }

    static_unordered_flat_multimap& operator=(static_unordered_flat_multimap&& other)
    {
        data_.ref_to_equal() = other.data_.ref_to_equal();

        assign_range
        (
            std::make_move_iterator(pointer(other.data_.first_)),
            std::make_move_iterator(pointer(other.data_.last_))
        );

        return *this;
    }

    static_unordered_flat_multimap& operator=(std::initializer_list<value_type> ilist)
    {
        assign_range(ilist.begin(), ilist.end());
        return *this;
    }

    //
    // ---- KEY EQUAL ---------------------------------------------------------
    //

    SFL_NODISCARD
    key_equal key_eq() const
    {
        return data_.ref_to_equal();
    }

    //
    // ---- VALUE EQUAL -------------------------------------------------------
    //

    SFL_NODISCARD
    value_equal value_eq() const
    {
        return value_equal(data_.ref_to_equal());
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
        return emplace_back(std::forward<Args>(args)...);
    }

    template <typename... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args)
    {
        SFL_ASSERT(!full());
        SFL_ASSERT(cbegin() <= hint && hint <= cend());
        sfl::dtl::ignore_unused(hint);
        return emplace_back(std::forward<Args>(args)...);
    }

    iterator insert(const value_type& value)
    {
        SFL_ASSERT(!full());
        return emplace_back(value);
    }

    iterator insert(value_type&& value)
    {
        SFL_ASSERT(!full());
        return emplace_back(std::move(value));
    }

    template <typename P,
              sfl::dtl::enable_if_t<std::is_constructible<value_type, P&&>::value>* = nullptr>
    iterator insert(P&& value)
    {
        SFL_ASSERT(!full());
        return emplace_back(std::forward<P>(value));
    }

    iterator insert(const_iterator hint, const value_type& value)
    {
        SFL_ASSERT(!full());
        SFL_ASSERT(cbegin() <= hint && hint <= cend());
        sfl::dtl::ignore_unused(hint);
        return emplace_back(value);
    }

    iterator insert(const_iterator hint, value_type&& value)
    {
        SFL_ASSERT(!full());
        SFL_ASSERT(cbegin() <= hint && hint <= cend());
        sfl::dtl::ignore_unused(hint);
        return emplace_back(std::move(value));
    }

    template <typename P,
              sfl::dtl::enable_if_t<std::is_constructible<value_type, P&&>::value>* = nullptr>
    iterator insert(const_iterator hint, P&& value)
    {
        SFL_ASSERT(!full());
        SFL_ASSERT(cbegin() <= hint && hint <= cend());
        sfl::dtl::ignore_unused(hint);
        return emplace_back(std::forward<P>(value));
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

    iterator erase(iterator pos)
    {
        return erase(const_iterator(pos));
    }

    iterator erase(const_iterator pos)
    {
        SFL_ASSERT(cbegin() <= pos && pos < cend());

        const difference_type offset = std::distance(cbegin(), pos);

        const pointer p = data_.first_ + offset;

        if (p < data_.last_ - 1)
        {
            *p = std::move(*(data_.last_ - 1));
        }

        --data_.last_;

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

        const difference_type count1 = std::distance(first, last);
        const difference_type count2 = std::distance(last, cend());

        const difference_type offset = std::distance(cbegin(), first);

        const pointer p1 = data_.first_ + offset;

        if (count1 >= count2)
        {
            const pointer p2 = p1 + count1;

            const pointer new_last = sfl::dtl::move(p2, data_.last_, p1);

            sfl::dtl::destroy(new_last, data_.last_);

            data_.last_ = new_last;
        }
        else
        {
            const pointer p2 = p1 + count2;

            sfl::dtl::move(p2, data_.last_, p1);

            const pointer new_last = p2;

            sfl::dtl::destroy(new_last, data_.last_);

            data_.last_ = new_last;
        }

        return iterator(p1);
    }

    size_type erase(const Key& key)
    {
        size_type n = 0;

        for (auto it = begin(); it != end();)
        {
            if (data_.ref_to_equal()(*it, key))
            {
                it = erase(it);
                ++n;
            }
            else
            {
                ++it;
            }
        }

        return n;
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<KeyEqual, K>::value>* = nullptr>
    size_type erase(K&& x)
    {
        size_type n = 0;

        for (auto it = begin(); it != end();)
        {
            if (data_.ref_to_equal()(*it, x))
            {
                it = erase(it);
                ++n;
            }
            else
            {
                ++it;
            }
        }

        return n;
    }

    void swap(static_unordered_flat_multimap& other)
    {
        if (this == &other)
        {
            return;
        }

        using std::swap;

        swap(this->data_.ref_to_equal(), other.data_.ref_to_equal());

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
    iterator find(const Key& key)
    {
        for (auto it = begin(); it != end(); ++it)
        {
            if (data_.ref_to_equal()(*it, key))
            {
                return it;
            }
        }

        return end();
    }

    SFL_NODISCARD
    const_iterator find(const Key& key) const
    {
        for (auto it = begin(); it != end(); ++it)
        {
            if (data_.ref_to_equal()(*it, key))
            {
                return it;
            }
        }

        return end();
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<KeyEqual, K>::value>* = nullptr>
    SFL_NODISCARD
    iterator find(const K& x)
    {
        for (auto it = begin(); it != end(); ++it)
        {
            if (data_.ref_to_equal()(*it, x))
            {
                return it;
            }
        }

        return end();
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<KeyEqual, K>::value>* = nullptr>
    SFL_NODISCARD
    const_iterator find(const K& x) const
    {
        for (auto it = begin(); it != end(); ++it)
        {
            if (data_.ref_to_equal()(*it, x))
            {
                return it;
            }
        }

        return end();
    }

    SFL_NODISCARD
    size_type count(const Key& key) const
    {
        size_type n = 0;

        for (auto it = begin(); it != end(); ++it)
        {
            if (data_.ref_to_equal()(*it, key))
            {
                ++n;
            }
        }

        return n;
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<KeyEqual, K>::value>* = nullptr>
    SFL_NODISCARD
    size_type count(const K& x) const
    {
        size_type n = 0;

        for (auto it = begin(); it != end(); ++it)
        {
            if (data_.ref_to_equal()(*it, x))
            {
                ++n;
            }
        }

        return n;
    }

    SFL_NODISCARD
    bool contains(const Key& key) const
    {
        return find(key) != end();
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<KeyEqual, K>::value>* = nullptr>
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

    template <typename InputIt, typename Sentinel>
    void insert_range_aux(InputIt first, Sentinel last)
    {
        while (first != last)
        {
            insert(*first);
            ++first;
        }
    }

    template <typename... Args>
    iterator emplace_back(Args&&... args)
    {
        SFL_ASSERT(!full());

        const pointer old_last = data_.last_;

        sfl::dtl::construct_at
        (
            data_.last_,
            std::forward<Args>(args)...
        );

        ++data_.last_;

        return iterator(old_last);
    }
};

//
// ---- NON-MEMBER FUNCTIONS --------------------------------------------------
//

template <typename K, typename T, std::size_t N, typename E>
SFL_NODISCARD
bool operator==
(
    const static_unordered_flat_multimap<K, T, N, E>& x,
    const static_unordered_flat_multimap<K, T, N, E>& y
)
{
    return x.size() == y.size() && std::is_permutation(x.begin(), x.end(), y.begin());
}

template <typename K, typename T, std::size_t N, typename E>
SFL_NODISCARD
bool operator!=
(
    const static_unordered_flat_multimap<K, T, N, E>& x,
    const static_unordered_flat_multimap<K, T, N, E>& y
)
{
    return !(x == y);
}

template <typename K, typename T, std::size_t N, typename E>
void swap
(
    static_unordered_flat_multimap<K, T, N, E>& x,
    static_unordered_flat_multimap<K, T, N, E>& y
)
{
    x.swap(y);
}

template <typename K, typename T, std::size_t N, typename E, typename Predicate>
typename static_unordered_flat_multimap<K, T, N, E>::size_type
    erase_if(static_unordered_flat_multimap<K, T, N, E>& c, Predicate pred)
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

#endif // SFL_STATIC_UNORDERED_FLAT_MULTIMAP_HPP_INCLUDED
