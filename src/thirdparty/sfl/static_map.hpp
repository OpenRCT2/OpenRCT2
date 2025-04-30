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

#ifndef SFL_STATIC_MAP_HPP_INCLUDED
#define SFL_STATIC_MAP_HPP_INCLUDED

#include <sfl/detail/allocator_traits.hpp>
#include <sfl/detail/container_compatible_range.hpp>
#include <sfl/detail/cpp.hpp>
#include <sfl/detail/exceptions.hpp>
#include <sfl/detail/functional.hpp>
#include <sfl/detail/node_static_allocator.hpp>
#include <sfl/detail/rb_tree.hpp>
#include <sfl/detail/tags.hpp>
#include <sfl/detail/type_traits.hpp>

#include <cstddef>          // size_t
#include <functional>       // equal_to, less
#include <initializer_list> // initializer_list
#include <type_traits>      // is_same, is_nothrow_xxxxx
#include <utility>          // forward, move, pair

namespace sfl
{

template < typename Key,
           typename T,
           std::size_t N,
           typename Compare = std::less<Key> >
class static_map
{
    static_assert(N > 0, "N must be greater than zero.");

public:

    using key_type       = Key;
    using mapped_type    = T;
    using value_type     = std::pair<const Key, T>;
    using key_compare    = Compare;

    class value_compare : protected key_compare
    {
        friend class static_map;

    private:

        value_compare(const key_compare& c) : key_compare(c)
        {}

    public:

        bool operator()(const value_type& x, const value_type& y) const
        {
            return key_compare::operator()(x.first, y.first);
        }
    };

private:

    using tree_type = sfl::dtl::rb_tree
    <
        key_type,
        value_type,
        sfl::dtl::first,
        key_compare,
        sfl::dtl::node_static_allocator<value_type, N>,
        static_map
    >;

    tree_type tree_;

public:

    using size_type              = typename sfl::dtl::allocator_traits<typename tree_type::allocator_type>::size_type;
    using difference_type        = typename sfl::dtl::allocator_traits<typename tree_type::allocator_type>::difference_type;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using pointer                = typename sfl::dtl::allocator_traits<typename tree_type::allocator_type>::pointer;
    using const_pointer          = typename sfl::dtl::allocator_traits<typename tree_type::allocator_type>::const_pointer;
    using iterator               = typename tree_type::iterator;
    using const_iterator         = typename tree_type::const_iterator;
    using reverse_iterator       = typename tree_type::reverse_iterator;
    using const_reverse_iterator = typename tree_type::const_reverse_iterator;

public:

    static constexpr size_type static_capacity = N;

public:

    //
    // ---- CONSTRUCTION AND DESTRUCTION --------------------------------------
    //

    static_map() noexcept
    (
        std::is_nothrow_default_constructible<Compare>::value
    )
        : tree_()
    {}

    explicit static_map(const Compare& comp) noexcept
    (
        std::is_nothrow_copy_constructible<Compare>::value
    )
        : tree_(comp)
    {}

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_input_iterator<InputIt>::value>* = nullptr>
    static_map(InputIt first, InputIt last)
        : tree_()
    {
        insert(first, last);
    }

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_input_iterator<InputIt>::value>* = nullptr>
    static_map(InputIt first, InputIt last, const Compare& comp)
        : tree_(comp)
    {
        insert(first, last);
    }

    static_map(std::initializer_list<value_type> ilist)
        : static_map(ilist.begin(), ilist.end())
    {}

    static_map(std::initializer_list<value_type> ilist, const Compare& comp)
        : static_map(ilist.begin(), ilist.end(), comp)
    {}

    static_map(const static_map& other)
        : tree_(other.tree_)
    {}

    static_map(static_map&& other)
        : tree_(std::move(other.tree_))
    {}

#if SFL_CPP_VERSION >= SFL_CPP_20

    template <sfl::dtl::container_compatible_range<value_type> Range>
    static_map(sfl::from_range_t, Range&& range)
        : tree_()
    {
        insert_range(std::forward<Range>(range));
    }

    template <sfl::dtl::container_compatible_range<value_type> Range>
    static_map(sfl::from_range_t, Range&& range, const Compare& comp)
        : tree_(comp)
    {
        insert_range(std::forward<Range>(range));
    }

#else // before C++20

    template <typename Range>
    static_map(sfl::from_range_t, Range&& range)
        : tree_()
    {
        insert_range(std::forward<Range>(range));
    }

    template <typename Range>
    static_map(sfl::from_range_t, Range&& range, const Compare& comp)
        : tree_(comp)
    {
        insert_range(std::forward<Range>(range));
    }

#endif // before C++20

    ~static_map()
    {}

    //
    // ---- ASSIGNMENT --------------------------------------------------------
    //

    static_map& operator=(const static_map& other)
    {
        tree_.operator=(other.tree_);
        return *this;
    }

    static_map& operator=(static_map&& other)
    {
        tree_.operator=(std::move(other.tree_));
        return *this;
    }

    static_map& operator=(std::initializer_list<value_type> ilist)
    {
        SFL_ASSERT(size_type(ilist.size()) <= capacity());
        tree_.assign_range_unique(ilist.begin(), ilist.end());
        return *this;
    }

    //
    // ---- KEY COMPARE -------------------------------------------------------
    //

    SFL_NODISCARD
    key_compare key_comp() const
    {
        return key_compare(tree_.ref_to_comp());
    }

    //
    // ---- VALUE COMPARE -----------------------------------------------------
    //

    SFL_NODISCARD
    value_compare value_comp() const
    {
        return value_compare(tree_.ref_to_comp());
    }

    //
    // ---- ITERATORS ---------------------------------------------------------
    //

    SFL_NODISCARD
    iterator begin() noexcept
    {
        return tree_.begin();
    }

    SFL_NODISCARD
    const_iterator begin() const noexcept
    {
        return tree_.begin();
    }

    SFL_NODISCARD
    const_iterator cbegin() const noexcept
    {
        return tree_.cbegin();
    }

    SFL_NODISCARD
    iterator end() noexcept
    {
        return tree_.end();
    }

    SFL_NODISCARD
    const_iterator end() const noexcept
    {
        return tree_.end();
    }

    SFL_NODISCARD
    const_iterator cend() const noexcept
    {
        return tree_.cend();
    }

    SFL_NODISCARD
    reverse_iterator rbegin() noexcept
    {
        return tree_.rbegin();
    }

    SFL_NODISCARD
    const_reverse_iterator rbegin() const noexcept
    {
        return tree_.rbegin();
    }

    SFL_NODISCARD
    const_reverse_iterator crbegin() const noexcept
    {
        return tree_.crbegin();
    }

    SFL_NODISCARD
    reverse_iterator rend() noexcept
    {
        return tree_.rend();
    }

    SFL_NODISCARD
    const_reverse_iterator rend() const noexcept
    {
        return tree_.rend();
    }

    SFL_NODISCARD
    const_reverse_iterator crend() const noexcept
    {
        return tree_.crend();
    }

    //
    // ---- SIZE AND CAPACITY -------------------------------------------------
    //

    SFL_NODISCARD
    bool empty() const noexcept
    {
        return tree_.empty();
    }

    SFL_NODISCARD
    bool full() const noexcept
    {
        return size() == capacity();
    }

    SFL_NODISCARD
    size_type size() const noexcept
    {
        return tree_.size();
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

    void clear()
    {
        tree_.clear();
    }

    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args)
    {
        SFL_ASSERT(!full());
        return tree_.emplace_unique(std::forward<Args>(args)...);
    }

    template <typename... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args)
    {
        SFL_ASSERT(!full());
        return tree_.emplace_hint_unique(hint, std::forward<Args>(args)...);
    }

    std::pair<iterator, bool> insert(const value_type& value)
    {
        SFL_ASSERT(!full());
        return tree_.insert_unique(value);
    }

    std::pair<iterator, bool> insert(value_type&& value)
    {
        SFL_ASSERT(!full());
        return tree_.insert_unique(std::move(value));
    }

    template <typename P,
              sfl::dtl::enable_if_t<std::is_constructible<value_type, P&&>::value>* = nullptr>
    std::pair<iterator, bool> insert(P&& value)
    {
        SFL_ASSERT(!full());
        return tree_.insert_unique(std::forward<P>(value));
    }

    iterator insert(const_iterator hint, const value_type& value)
    {
        SFL_ASSERT(!full());
        return tree_.insert_hint_unique(hint, value);
    }

    iterator insert(const_iterator hint, value_type&& value)
    {
        SFL_ASSERT(!full());
        return tree_.insert_hint_unique(hint, std::move(value));
    }

    template <typename P,
              sfl::dtl::enable_if_t<std::is_constructible<value_type, P>::value>* = nullptr>
    iterator insert(const_iterator hint, P&& value)
    {
        SFL_ASSERT(!full());
        return tree_.insert_hint_unique(hint, std::forward<P>(value));
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

    template <typename M,
              sfl::dtl::enable_if_t<std::is_assignable<mapped_type&, M&&>::value>* = nullptr>
    std::pair<iterator, bool> insert_or_assign(const Key& key, M&& obj)
    {
        SFL_ASSERT(!full());
        return insert_or_assign_aux(key, std::forward<M>(obj));
    }

    template <typename M,
              sfl::dtl::enable_if_t<std::is_assignable<mapped_type&, M&&>::value>* = nullptr>
    std::pair<iterator, bool> insert_or_assign(Key&& key, M&& obj)
    {
        SFL_ASSERT(!full());
        return insert_or_assign_aux(std::move(key), std::forward<M>(obj));
    }

    template <typename K, typename M,
              sfl::dtl::enable_if_t< sfl::dtl::has_is_transparent<Compare, K>::value &&
                                     std::is_assignable<mapped_type&, M&&>::value >* = nullptr>
    std::pair<iterator, bool> insert_or_assign(K&& key, M&& obj)
    {
        SFL_ASSERT(!full());
        return insert_or_assign_aux(std::forward<K>(key), std::forward<M>(obj));
    }

    template <typename M,
              sfl::dtl::enable_if_t<std::is_assignable<mapped_type&, M&&>::value>* = nullptr>
    iterator insert_or_assign(const_iterator hint, const Key& key, M&& obj)
    {
        SFL_ASSERT(!full());
        return insert_or_assign_aux(hint, key, std::forward<M>(obj));
    }

    template <typename M,
              sfl::dtl::enable_if_t<std::is_assignable<mapped_type&, M&&>::value>* = nullptr>
    iterator insert_or_assign(const_iterator hint, Key&& key, M&& obj)
    {
        SFL_ASSERT(!full());
        return insert_or_assign_aux(hint, std::move(key), std::forward<M>(obj));
    }

    template <typename K, typename M,
              sfl::dtl::enable_if_t< sfl::dtl::has_is_transparent<Compare, K>::value &&
                                     std::is_assignable<mapped_type&, M&&>::value >* = nullptr>
    iterator insert_or_assign(const_iterator hint, K&& key, M&& obj)
    {
        SFL_ASSERT(!full());
        return insert_or_assign_aux(hint, std::forward<K>(key), std::forward<M>(obj));
    }

    template <typename... Args>
    std::pair<iterator, bool> try_emplace(const Key& key, Args&&... args)
    {
        SFL_ASSERT(!full());
        return try_emplace_aux(key, std::forward<Args>(args)...);
    }

    template <typename... Args>
    std::pair<iterator, bool> try_emplace(Key&& key, Args&&... args)
    {
        SFL_ASSERT(!full());
        return try_emplace_aux(std::move(key), std::forward<Args>(args)...);
    }

    template <typename K, typename... Args,
              sfl::dtl::enable_if_t<
                #if defined(__GNUC__) && !defined(__clang__) && (__GNUC__ < 7)
                // This is workaround for GCC 4 bug on CentOS 7.
                !std::is_same<sfl::dtl::remove_cvref_t<Key>, sfl::dtl::remove_cvref_t<K>>::value &&
                #endif
                sfl::dtl::has_is_transparent<Compare, K>::value &&
                !std::is_convertible<K&&, const_iterator>::value &&
                !std::is_convertible<K&&, iterator>::value
              >* = nullptr>
    std::pair<iterator, bool> try_emplace(K&& key, Args&&... args)
    {
        SFL_ASSERT(!full());
        return try_emplace_aux(std::forward<K>(key), std::forward<Args>(args)...);
    }

    template <typename... Args>
    iterator try_emplace(const_iterator hint, const Key& key, Args&&... args)
    {
        SFL_ASSERT(!full());
        return try_emplace_aux(hint, key, std::forward<Args>(args)...);
    }

    template <typename... Args>
    iterator try_emplace(const_iterator hint, Key&& key, Args&&... args)
    {
        SFL_ASSERT(!full());
        return try_emplace_aux(hint, std::move(key), std::forward<Args>(args)...);
    }

    template <typename K, typename... Args,
              sfl::dtl::enable_if_t<
                #if defined(__GNUC__) && !defined(__clang__) && (__GNUC__ < 7)
                // This is workaround for GCC 4 bug on CentOS 7.
                !std::is_same<sfl::dtl::remove_cvref_t<Key>, sfl::dtl::remove_cvref_t<K>>::value &&
                #endif
                sfl::dtl::has_is_transparent<Compare, K>::value
              >* = nullptr>
    iterator try_emplace(const_iterator hint, K&& key, Args&&... args)
    {
        SFL_ASSERT(!full());
        return try_emplace_aux(hint, std::forward<K>(key), std::forward<Args>(args)...);
    }

    iterator erase(iterator pos)
    {
        return tree_.erase(pos);
    }

    iterator erase(const_iterator pos)
    {
        return tree_.erase(pos);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        return tree_.erase(first, last);
    }

    size_type erase(const Key& key)
    {
        return tree_.erase(key);
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    size_type erase(K&& x)
    {
        return tree_.erase(x);
    }

    void swap(static_map& other)
    {
        tree_.swap(other.tree_);
    }

    //
    // ---- LOOKUP ------------------------------------------------------------
    //

    SFL_NODISCARD
    iterator lower_bound(const Key& key)
    {
        return tree_.lower_bound(key);
    }

    SFL_NODISCARD
    const_iterator lower_bound(const Key& key) const
    {
        return tree_.lower_bound(key);
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    iterator lower_bound(const K& x)
    {
        return tree_.lower_bound(x);
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    const_iterator lower_bound(const K& x) const
    {
        return tree_.lower_bound(x);
    }

    SFL_NODISCARD
    iterator upper_bound(const Key& key)
    {
        return tree_.upper_bound(key);
    }

    SFL_NODISCARD
    const_iterator upper_bound(const Key& key) const
    {
        return tree_.upper_bound(key);
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    iterator upper_bound(const K& x)
    {
        return tree_.upper_bound(x);
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    const_iterator upper_bound(const K& x) const
    {
        return tree_.upper_bound(x);
    }

    SFL_NODISCARD
    std::pair<iterator, iterator> equal_range(const Key& key)
    {
        return tree_.equal_range(key);
    }

    SFL_NODISCARD
    std::pair<const_iterator, const_iterator> equal_range(const Key& key) const
    {
        return tree_.equal_range(key);
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    std::pair<iterator, iterator> equal_range(const K& x)
    {
        return tree_.equal_range(x);
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    std::pair<const_iterator, const_iterator> equal_range(const K& x) const
    {
        return tree_.equal_range(x);
    }

    SFL_NODISCARD
    iterator find(const Key& key)
    {
        return tree_.find(key);
    }

    SFL_NODISCARD
    const_iterator find(const Key& key) const
    {
        return tree_.find(key);
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    iterator find(const K& x)
    {
        return tree_.find(x);
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    const_iterator find(const K& x) const
    {
        return tree_.find(x);
    }

    SFL_NODISCARD
    size_type count(const Key& key) const
    {
        return tree_.count(key);
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    size_type count(const K& x) const
    {
        return tree_.count(x);
    }

    SFL_NODISCARD
    bool contains(const Key& key) const
    {
        return tree_.contains(key);
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    bool contains(const K& x) const
    {
        return tree_.contains(x);
    }

    //
    // ---- ELEMENT ACCESS ----------------------------------------------------
    //

    SFL_NODISCARD
    T& at(const Key& key)
    {
        auto it = find(key);

        if (it == end())
        {
            sfl::dtl::throw_out_of_range("sfl::static_map::at");
        }

        return it->second;
    }

    SFL_NODISCARD
    const T& at(const Key& key) const
    {
        auto it = find(key);

        if (it == end())
        {
            sfl::dtl::throw_out_of_range("sfl::static_map::at");
        }

        return it->second;
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    T& at(const K& x)
    {
        auto it = find(x);

        if (it == end())
        {
            sfl::dtl::throw_out_of_range("sfl::static_map::at");
        }

        return it->second;
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    const T& at(const K& x) const
    {
        auto it = find(x);

        if (it == end())
        {
            sfl::dtl::throw_out_of_range("sfl::static_map::at");
        }

        return it->second;
    }

    SFL_NODISCARD
    T& operator[](const Key& key)
    {
        return try_emplace(key).first->second;
    }

    SFL_NODISCARD
    T& operator[](Key&& key)
    {
        return try_emplace(std::move(key)).first->second;
    }

    template <typename K,
              sfl::dtl::enable_if_t<sfl::dtl::has_is_transparent<Compare, K>::value>* = nullptr>
    SFL_NODISCARD
    T& operator[](K&& key)
    {
        return try_emplace(std::forward<K>(key)).first->second;
    }

private:

    template <typename InputIt, typename Sentinel>
    void insert_range_aux(InputIt first, Sentinel last)
    {
        while (first != last)
        {
            insert(*first);
            ++first;
        }
    }

    template <typename K, typename M>
    std::pair<iterator, bool> insert_or_assign_aux(K&& key, M&& obj)
    {
        auto res = tree_.calculate_position_for_insert_unique(key);
        if (res.status)
        {
            typename tree_type::make_node_functor make_node(tree_);
            typename tree_type::node_pointer x = make_node
            (
                std::piecewise_construct,
                std::forward_as_tuple(std::forward<K>(key)),
                std::forward_as_tuple(std::forward<M>(obj))
            );
            tree_.insert(x, res.pos, res.left, tree_.data_.root(), tree_.data_.minimum());
            ++tree_.data_.size_;
            return std::make_pair(iterator(x), true);
        }
        else
        {
            iterator it(res.pos);
            it->second = std::forward<M>(obj);
            return std::make_pair(it, false);
        }
    }

    template <typename K, typename M>
    iterator insert_or_assign_aux(const_iterator hint, K&& key, M&& obj)
    {
        auto res = tree_.calculate_position_for_insert_hint_unique(hint, key);
        if (res.status)
        {
            typename tree_type::make_node_functor make_node(tree_);
            typename tree_type::node_pointer x = make_node
            (
                std::piecewise_construct,
                std::forward_as_tuple(std::forward<K>(key)),
                std::forward_as_tuple(std::forward<M>(obj))
            );
            tree_.insert(x, res.pos, res.left, tree_.data_.root(), tree_.data_.minimum());
            ++tree_.data_.size_;
            return iterator(x);
        }
        else
        {
            iterator it(res.pos);
            it->second = std::forward<M>(obj);
            return it;
        }
    }

    template <typename K, typename... Args>
    std::pair<iterator, bool> try_emplace_aux(K&& key, Args&&... args)
    {
        auto res = tree_.calculate_position_for_insert_unique(key);
        if (res.status)
        {
            typename tree_type::make_node_functor make_node(tree_);
            typename tree_type::node_pointer x = make_node
            (
                std::piecewise_construct,
                std::forward_as_tuple(std::forward<K>(key)),
                std::forward_as_tuple(std::forward<Args>(args)...)
            );
            tree_.insert(x, res.pos, res.left, tree_.data_.root(), tree_.data_.minimum());
            ++tree_.data_.size_;
            return std::make_pair(iterator(x), true);
        }
        else
        {
            return std::make_pair(iterator(res.pos), false);
        }
    }

    template <typename K, typename... Args>
    iterator try_emplace_aux(const_iterator hint, K&& key, Args&&... args)
    {
        auto res = tree_.calculate_position_for_insert_hint_unique(hint, key);
        if (res.status)
        {
            typename tree_type::make_node_functor make_node(tree_);
            typename tree_type::node_pointer x = make_node
            (
                std::piecewise_construct,
                std::forward_as_tuple(std::forward<K>(key)),
                std::forward_as_tuple(std::forward<Args>(args)...)
            );
            tree_.insert(x, res.pos, res.left, tree_.data_.root(), tree_.data_.minimum());
            ++tree_.data_.size_;
            return iterator(x);
        }
        else
        {
            return iterator(res.pos);
        }
    }
};

//
// ---- NON-MEMBER FUNCTIONS --------------------------------------------------
//

template <typename K, typename T, std::size_t N, typename C>
SFL_NODISCARD
bool operator==
(
    const static_map<K, T, N, C>& x,
    const static_map<K, T, N, C>& y
)
{
    return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
}

template <typename K, typename T, std::size_t N, typename C>
SFL_NODISCARD
bool operator!=
(
    const static_map<K, T, N, C>& x,
    const static_map<K, T, N, C>& y
)
{
    return !(x == y);
}

template <typename K, typename T, std::size_t N, typename C>
SFL_NODISCARD
bool operator<
(
    const static_map<K, T, N, C>& x,
    const static_map<K, T, N, C>& y
)
{
    return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <typename K, typename T, std::size_t N, typename C>
SFL_NODISCARD
bool operator>
(
    const static_map<K, T, N, C>& x,
    const static_map<K, T, N, C>& y
)
{
    return y < x;
}

template <typename K, typename T, std::size_t N, typename C>
SFL_NODISCARD
bool operator<=
(
    const static_map<K, T, N, C>& x,
    const static_map<K, T, N, C>& y
)
{
    return !(y < x);
}

template <typename K, typename T, std::size_t N, typename C>
SFL_NODISCARD
bool operator>=
(
    const static_map<K, T, N, C>& x,
    const static_map<K, T, N, C>& y
)
{
    return !(x < y);
}

template <typename K, typename T, std::size_t N, typename C>
void swap
(
    static_map<K, T, N, C>& x,
    static_map<K, T, N, C>& y
)
{
    x.swap(y);
}

template <typename K, typename T, std::size_t N, typename C, typename Predicate>
typename static_map<K, T, N, C>::size_type
    erase_if(static_map<K, T, N, C>& c, Predicate pred)
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

#endif // SFL_STATIC_MAP_HPP_INCLUDED
