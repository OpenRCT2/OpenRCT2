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

#ifndef SFL_SMALL_UNORDERED_FLAT_MULTIMAP_HPP_INCLUDED
#define SFL_SMALL_UNORDERED_FLAT_MULTIMAP_HPP_INCLUDED

#include <sfl/detail/container_compatible_range.hpp>
#include <sfl/detail/cpp.hpp>
#include <sfl/detail/exceptions.hpp>
#include <sfl/detail/ignore_unused.hpp>
#include <sfl/detail/initialized_memory_algorithms.hpp>
#include <sfl/detail/normal_iterator.hpp>
#include <sfl/detail/tags.hpp>
#include <sfl/detail/to_address.hpp>
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

#ifdef SFL_TEST_SMALL_UNORDERED_FLAT_MULTIMAP
template <int>
void test_small_unordered_flat_multimap();
#endif

namespace sfl
{

template < typename Key,
           typename T,
           std::size_t N,
           typename KeyEqual = std::equal_to<Key>,
           typename Allocator = std::allocator<std::pair<Key, T>> >
class small_unordered_flat_multimap
{
    #ifdef SFL_TEST_SMALL_UNORDERED_FLAT_MULTIMAP
    template <int>
    friend void ::test_small_unordered_flat_multimap();
    #endif

public:

    using allocator_type   = Allocator;
    using allocator_traits = std::allocator_traits<allocator_type>;
    using key_type         = Key;
    using mapped_type      = T;
    using value_type       = std::pair<Key, T>;
    using size_type        = typename allocator_traits::size_type;
    using difference_type  = typename allocator_traits::difference_type;
    using key_equal        = KeyEqual;
    using reference        = value_type&;
    using const_reference  = const value_type&;
    using pointer          = typename allocator_traits::pointer;
    using const_pointer    = typename allocator_traits::const_pointer;
    using iterator         = sfl::dtl::normal_iterator<pointer, small_unordered_flat_multimap>;
    using const_iterator   = sfl::dtl::normal_iterator<const_pointer, small_unordered_flat_multimap>;

    class value_equal : protected key_equal
    {
        friend class small_unordered_flat_multimap;

    private:

        value_equal(const key_equal& e) : key_equal(e)
        {}

    public:

        bool operator()(const value_type& x, const value_type& y) const
        {
            return key_equal::operator()(x.first, y.first);
        }
    };

    static_assert
    (
        std::is_same<typename Allocator::value_type, value_type>::value,
        "Allocator::value_type must be same as sfl::small_unordered_flat_multimap::value_type."
    );

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

    template <bool WithInternalStorage = true, typename = void>
    class data_base
    {
    private:

        union
        {
            value_type internal_storage_[N];
        };

    public:

        pointer first_;
        pointer last_;
        pointer eos_;

        data_base() noexcept
            : first_(std::pointer_traits<pointer>::pointer_to(*internal_storage_))
            , last_(first_)
            , eos_(first_ + N)
        {}

        #if defined(__clang__) && (__clang_major__ == 3) // For CentOS 7
        ~data_base()
        {}
        #else
        ~data_base() noexcept
        {}
        #endif

        pointer internal_storage() noexcept
        {
            return std::pointer_traits<pointer>::pointer_to(*internal_storage_);
        }
    };

    template <typename Dummy>
    class data_base<false, Dummy>
    {
    public:

        pointer first_;
        pointer last_;
        pointer eos_;

        data_base() noexcept
            : first_(nullptr)
            , last_(nullptr)
            , eos_(nullptr)
        {}

        pointer internal_storage() noexcept
        {
            return nullptr;
        }
    };

    class data : public data_base<(N > 0)>, public allocator_type, public ultra_equal
    {
    public:

        data() noexcept
        (
            std::is_nothrow_default_constructible<allocator_type>::value &&
            std::is_nothrow_default_constructible<ultra_equal>::value
        )
            : allocator_type()
            , ultra_equal()
        {}

        data(const ultra_equal& equal) noexcept
        (
            std::is_nothrow_default_constructible<allocator_type>::value &&
            std::is_nothrow_copy_constructible<ultra_equal>::value
        )
            : allocator_type()
            , ultra_equal(equal)
        {}

        data(const allocator_type& alloc) noexcept
        (
            std::is_nothrow_copy_constructible<allocator_type>::value &&
            std::is_nothrow_default_constructible<ultra_equal>::value
        )
            : allocator_type(alloc)
            , ultra_equal()
        {}

        data(const ultra_equal& equal, const allocator_type& alloc) noexcept
        (
            std::is_nothrow_copy_constructible<allocator_type>::value &&
            std::is_nothrow_copy_constructible<ultra_equal>::value
        )
            : allocator_type(alloc)
            , ultra_equal(equal)
        {}

        data(ultra_equal&& equal, allocator_type&& alloc) noexcept
        (
            std::is_nothrow_move_constructible<allocator_type>::value &&
            std::is_nothrow_move_constructible<ultra_equal>::value
        )
            : allocator_type(std::move(alloc))
            , ultra_equal(std::move(equal))
        {}

        data(ultra_equal&& equal, const allocator_type& alloc) noexcept
        (
            std::is_nothrow_copy_constructible<allocator_type>::value &&
            std::is_nothrow_move_constructible<ultra_equal>::value
        )
            : allocator_type(alloc)
            , ultra_equal(std::move(equal))
        {}

        allocator_type& ref_to_alloc() noexcept
        {
            return *this;
        }

        const allocator_type& ref_to_alloc() const noexcept
        {
            return *this;
        }

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

    small_unordered_flat_multimap() noexcept
    (
        std::is_nothrow_default_constructible<Allocator>::value &&
        std::is_nothrow_default_constructible<KeyEqual>::value
    )
        : data_()
    {}

    explicit small_unordered_flat_multimap(const KeyEqual& equal) noexcept
    (
        std::is_nothrow_default_constructible<Allocator>::value &&
        std::is_nothrow_copy_constructible<KeyEqual>::value
    )
        : data_(equal)
    {}

    explicit small_unordered_flat_multimap(const Allocator& alloc) noexcept
    (
        std::is_nothrow_copy_constructible<Allocator>::value &&
        std::is_nothrow_default_constructible<KeyEqual>::value
    )
        : data_(alloc)
    {}

    explicit small_unordered_flat_multimap(const KeyEqual& equal,
                                           const Allocator& alloc) noexcept
    (
        std::is_nothrow_copy_constructible<Allocator>::value &&
        std::is_nothrow_copy_constructible<KeyEqual>::value
    )
        : data_(equal, alloc)
    {}

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_input_iterator<InputIt>::value>* = nullptr>
    small_unordered_flat_multimap(InputIt first, InputIt last)
        : data_()
    {
        initialize_range(first, last);
    }

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_input_iterator<InputIt>::value>* = nullptr>
    small_unordered_flat_multimap(InputIt first, InputIt last,
                                  const KeyEqual& equal)
        : data_(equal)
    {
        initialize_range(first, last);
    }

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_input_iterator<InputIt>::value>* = nullptr>
    small_unordered_flat_multimap(InputIt first, InputIt last,
                                  const Allocator& alloc)
        : data_(alloc)
    {
        initialize_range(first, last);
    }

    template <typename InputIt,
              sfl::dtl::enable_if_t<sfl::dtl::is_input_iterator<InputIt>::value>* = nullptr>
    small_unordered_flat_multimap(InputIt first, InputIt last,
                                  const KeyEqual& equal,
                                  const Allocator& alloc)
        : data_(equal, alloc)
    {
        initialize_range(first, last);
    }

    small_unordered_flat_multimap(std::initializer_list<value_type> ilist)
        : small_unordered_flat_multimap(ilist.begin(), ilist.end())
    {}

    small_unordered_flat_multimap(std::initializer_list<value_type> ilist,
                                  const KeyEqual& equal)
        : small_unordered_flat_multimap(ilist.begin(), ilist.end(), equal)
    {}

    small_unordered_flat_multimap(std::initializer_list<value_type> ilist,
                                  const Allocator& alloc)
        : small_unordered_flat_multimap(ilist.begin(), ilist.end(), alloc)
    {}

    small_unordered_flat_multimap(std::initializer_list<value_type> ilist,
                                  const KeyEqual& equal, const Allocator& alloc)
        : small_unordered_flat_multimap(ilist.begin(), ilist.end(), equal, alloc)
    {}

    small_unordered_flat_multimap(const small_unordered_flat_multimap& other)
        : data_
        (
            other.data_.ref_to_equal(),
            allocator_traits::select_on_container_copy_construction
            (
                other.data_.ref_to_alloc()
            )
        )
    {
        initialize_copy(other);
    }

    small_unordered_flat_multimap(const small_unordered_flat_multimap& other,
                                  const Allocator& alloc)
        : data_
        (
            other.data_.ref_to_equal(),
            alloc
        )
    {
        initialize_copy(other);
    }

    small_unordered_flat_multimap(small_unordered_flat_multimap&& other)
        : data_
        (
            std::move(other.data_.ref_to_equal()),
            std::move(other.data_.ref_to_alloc())
        )
    {
        initialize_move(other);
    }

    small_unordered_flat_multimap(small_unordered_flat_multimap&& other,
                                  const Allocator& alloc)
        : data_
        (
            std::move(other.data_.ref_to_equal()),
            alloc
        )
    {
        initialize_move(other);
    }

#if SFL_CPP_VERSION >= SFL_CPP_20

    template <sfl::dtl::container_compatible_range<value_type> Range>
    small_unordered_flat_multimap(sfl::from_range_t, Range&& range)
        : data_()
    {
        initialize_range(std::forward<Range>(range));
    }

    template <sfl::dtl::container_compatible_range<value_type> Range>
    small_unordered_flat_multimap(sfl::from_range_t, Range&& range, const KeyEqual& equal)
        : data_(equal)
    {
        initialize_range(std::forward<Range>(range));
    }

    template <sfl::dtl::container_compatible_range<value_type> Range>
    small_unordered_flat_multimap(sfl::from_range_t, Range&& range, const Allocator& alloc)
        : data_(alloc)
    {
        initialize_range(std::forward<Range>(range));
    }

    template <sfl::dtl::container_compatible_range<value_type> Range>
    small_unordered_flat_multimap(sfl::from_range_t, Range&& range, const KeyEqual& equal, const Allocator& alloc)
        : data_(equal, alloc)
    {
        initialize_range(std::forward<Range>(range));
    }

#else // before C++20

    template <typename Range>
    small_unordered_flat_multimap(sfl::from_range_t, Range&& range)
        : data_()
    {
        initialize_range(std::forward<Range>(range));
    }

    template <typename Range>
    small_unordered_flat_multimap(sfl::from_range_t, Range&& range, const KeyEqual& equal)
        : data_(equal)
    {
        initialize_range(std::forward<Range>(range));
    }

    template <typename Range>
    small_unordered_flat_multimap(sfl::from_range_t, Range&& range, const Allocator& alloc)
        : data_(alloc)
    {
        initialize_range(std::forward<Range>(range));
    }

    template <typename Range>
    small_unordered_flat_multimap(sfl::from_range_t, Range&& range, const KeyEqual& equal, const Allocator& alloc)
        : data_(equal, alloc)
    {
        initialize_range(std::forward<Range>(range));
    }

#endif // before C++20

    ~small_unordered_flat_multimap()
    {
        sfl::dtl::destroy_a
        (
            data_.ref_to_alloc(),
            data_.first_,
            data_.last_
        );

        if (data_.first_ != data_.internal_storage())
        {
            sfl::dtl::deallocate
            (
                data_.ref_to_alloc(),
                data_.first_,
                std::distance(data_.first_, data_.eos_)
            );
        }
    }

    //
    // ---- ASSIGNMENT --------------------------------------------------------
    //

    small_unordered_flat_multimap& operator=(const small_unordered_flat_multimap& other)
    {
        assign_copy(other);
        return *this;
    }

    small_unordered_flat_multimap& operator=(small_unordered_flat_multimap&& other)
    {
        assign_move(other);
        return *this;
    }

    small_unordered_flat_multimap& operator=(std::initializer_list<value_type> ilist)
    {
        clear();
        insert(ilist.begin(), ilist.end());
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

    void reserve(size_type new_cap)
    {
        check_size(new_cap, "sfl::small_unordered_flat_multimap::reserve");

        if (new_cap > capacity())
        {
            if (new_cap <= N)
            {
                if (data_.first_ == data_.internal_storage())
                {
                    // Do nothing. We are already using internal storage.
                }
                else
                {
                    // We are not using internal storage but new capacity
                    // can fit in internal storage.

                    pointer new_first = data_.internal_storage();
                    pointer new_last  = new_first;
                    pointer new_eos   = new_first + N;

                    new_last = sfl::dtl::uninitialized_move_if_noexcept_a
                    (
                        data_.ref_to_alloc(),
                        data_.first_,
                        data_.last_,
                        new_first
                    );

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
                        std::distance(data_.first_, data_.eos_)
                    );

                    data_.first_ = new_first;
                    data_.last_  = new_last;
                    data_.eos_   = new_eos;
                }
            }
            else
            {
                pointer new_first = sfl::dtl::allocate(data_.ref_to_alloc(), new_cap);
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
                        new_first,
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

                if (data_.first_ != data_.internal_storage())
                {
                    sfl::dtl::deallocate
                    (
                        data_.ref_to_alloc(),
                        data_.first_,
                        std::distance(data_.first_, data_.eos_)
                    );
                }

                data_.first_ = new_first;
                data_.last_  = new_last;
                data_.eos_   = new_eos;
            }
        }
    }

    void shrink_to_fit()
    {
        const size_type new_cap = size();

        if (new_cap < capacity())
        {
            if (new_cap <= N)
            {
                if (data_.first_ == data_.internal_storage())
                {
                    // Do nothing. We are already using internal storage.
                }
                else
                {
                    // We are not using internal storage but new capacity
                    // can fit in internal storage.

                    pointer new_first = data_.internal_storage();
                    pointer new_last  = new_first;
                    pointer new_eos   = new_first + N;

                    new_last = sfl::dtl::uninitialized_move_if_noexcept_a
                    (
                        data_.ref_to_alloc(),
                        data_.first_,
                        data_.last_,
                        new_first
                    );

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
                        std::distance(data_.first_, data_.eos_)
                    );

                    data_.first_ = new_first;
                    data_.last_  = new_last;
                    data_.eos_   = new_eos;
                }
            }
            else
            {
                pointer new_first = sfl::dtl::allocate(data_.ref_to_alloc(), new_cap);
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
                        new_first,
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

                if (data_.first_ != data_.internal_storage())
                {
                    sfl::dtl::deallocate
                    (
                        data_.ref_to_alloc(),
                        data_.first_,
                        std::distance(data_.first_, data_.eos_)
                    );
                }

                data_.first_ = new_first;
                data_.last_  = new_last;
                data_.eos_   = new_eos;
            }
        }
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
    iterator emplace(Args&&... args)
    {
        return emplace_back(std::forward<Args>(args)...);
    }

    template <typename... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args)
    {
        SFL_ASSERT(cbegin() <= hint && hint <= cend());
        sfl::dtl::ignore_unused(hint);
        return emplace_back(std::forward<Args>(args)...);
    }

    iterator insert(const value_type& value)
    {
        return emplace_back(value);
    }

    iterator insert(value_type&& value)
    {
        return emplace_back(std::move(value));
    }

    template <typename P,
              sfl::dtl::enable_if_t<std::is_constructible<value_type, P&&>::value>* = nullptr>
    iterator insert(P&& value)
    {
        return emplace_back(std::forward<P>(value));
    }

    iterator insert(const_iterator hint, const value_type& value)
    {
        SFL_ASSERT(cbegin() <= hint && hint <= cend());
        sfl::dtl::ignore_unused(hint);
        return emplace_back(value);
    }

    iterator insert(const_iterator hint, value_type&& value)
    {
        SFL_ASSERT(cbegin() <= hint && hint <= cend());
        sfl::dtl::ignore_unused(hint);
        return emplace_back(std::move(value));
    }

    template <typename P,
              sfl::dtl::enable_if_t<std::is_constructible<value_type, P&&>::value>* = nullptr>
    iterator insert(const_iterator hint, P&& value)
    {
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

        sfl::dtl::destroy_at_a(data_.ref_to_alloc(), data_.last_);

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

            sfl::dtl::destroy_a(data_.ref_to_alloc(), new_last, data_.last_);

            data_.last_ = new_last;
        }
        else
        {
            const pointer p2 = p1 + count2;

            sfl::dtl::move(p2, data_.last_, p1);

            const pointer new_last = p2;

            sfl::dtl::destroy_a(data_.ref_to_alloc(), new_last, data_.last_);

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

    void swap(small_unordered_flat_multimap& other)
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

        swap(this->data_.ref_to_equal(), other.data_.ref_to_equal());

        if
        (
            this->data_.first_ == this->data_.internal_storage() &&
            other.data_.first_ == other.data_.internal_storage()
        )
        {
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

                sfl::dtl::uninitialized_move_a
                (
                    this->data_.ref_to_alloc(),
                    other.data_.first_ + this_size,
                    other.data_.first_ + other_size,
                    this->data_.first_ + this_size
                );

                sfl::dtl::destroy_a
                (
                    other.data_.ref_to_alloc(),
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

                sfl::dtl::uninitialized_move_a
                (
                    other.data_.ref_to_alloc(),
                    this->data_.first_ + other_size,
                    this->data_.first_ + this_size,
                    other.data_.first_ + other_size
                );

                sfl::dtl::destroy_a
                (
                    this->data_.ref_to_alloc(),
                    this->data_.first_ + other_size,
                    this->data_.first_ + this_size
                );
            }

            this->data_.last_ = this->data_.first_ + other_size;
            other.data_.last_ = other.data_.first_ + this_size;
        }
        else if
        (
            this->data_.first_ == this->data_.internal_storage() &&
            other.data_.first_ != other.data_.internal_storage()
        )
        {
            pointer new_other_first = other.data_.internal_storage();
            pointer new_other_last  = new_other_first;
            pointer new_other_eos   = new_other_first + N;

            new_other_last = sfl::dtl::uninitialized_move_a
            (
                other.data_.ref_to_alloc(),
                this->data_.first_,
                this->data_.last_,
                new_other_first
            );

            sfl::dtl::destroy_a
            (
                this->data_.ref_to_alloc(),
                this->data_.first_,
                this->data_.last_
            );

            this->data_.first_ = other.data_.first_;
            this->data_.last_  = other.data_.last_;
            this->data_.eos_   = other.data_.eos_;

            other.data_.first_ = new_other_first;
            other.data_.last_  = new_other_last;
            other.data_.eos_   = new_other_eos;
        }
        else if
        (
            this->data_.first_ != this->data_.internal_storage() &&
            other.data_.first_ == other.data_.internal_storage()
        )
        {
            pointer new_this_first = this->data_.internal_storage();
            pointer new_this_last  = new_this_first;
            pointer new_this_eos   = new_this_first + N;

            new_this_last = sfl::dtl::uninitialized_move_a
            (
                this->data_.ref_to_alloc(),
                other.data_.first_,
                other.data_.last_,
                new_this_first
            );

            sfl::dtl::destroy_a
            (
                other.data_.ref_to_alloc(),
                other.data_.first_,
                other.data_.last_
            );

            other.data_.first_ = this->data_.first_;
            other.data_.last_  = this->data_.last_;
            other.data_.eos_   = this->data_.eos_;

            this->data_.first_ = new_this_first;
            this->data_.last_  = new_this_last;
            this->data_.eos_   = new_this_eos;
        }
        else
        {
            swap(this->data_.first_, other.data_.first_);
            swap(this->data_.last_,  other.data_.last_);
            swap(this->data_.eos_,   other.data_.eos_);
        }
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
        return sfl::dtl::to_address(data_.first_);
    }

    SFL_NODISCARD
    const value_type* data() const noexcept
    {
        return sfl::dtl::to_address(data_.first_);
    }

private:

    void check_size(size_type n, const char* msg)
    {
        if (n > max_size())
        {
            sfl::dtl::throw_length_error(msg);
        }
    }

    size_type calculate_new_capacity(size_type num_additional_elements, const char* msg)
    {
        const size_type size = this->size();
        const size_type capacity = this->capacity();
        const size_type max_size = this->max_size();

        if (max_size - size < num_additional_elements)
        {
            sfl::dtl::throw_length_error(msg);
        }
        else if (max_size - capacity < capacity / 2)
        {
            return max_size;
        }
        else if (size + num_additional_elements < capacity + capacity / 2)
        {
            return std::max(N, capacity + capacity / 2);
        }
        else
        {
            return std::max(N, size + num_additional_elements);
        }
    }

    void reset(size_type new_cap = N)
    {
        sfl::dtl::destroy_a
        (
            data_.ref_to_alloc(),
            data_.first_,
            data_.last_
        );

        if (data_.first_ != data_.internal_storage())
        {
            sfl::dtl::deallocate
            (
                data_.ref_to_alloc(),
                data_.first_,
                std::distance(data_.first_, data_.eos_)
            );
        }

        data_.first_ = data_.internal_storage();
        data_.last_  = data_.first_;
        data_.eos_   = data_.first_ + N;

        if (new_cap > N)
        {
            data_.first_ = sfl::dtl::allocate(data_.ref_to_alloc(), new_cap);
            data_.last_  = data_.first_;
            data_.eos_   = data_.first_ + new_cap;

            // If allocation throws, first_, last_ and eos_ will be valid
            // (they will be pointing to internal_storage).
        }
    }

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
            sfl::dtl::destroy_a
            (
                data_.ref_to_alloc(),
                data_.first_,
                data_.last_
            );

            if (data_.first_ != data_.internal_storage())
            {
                sfl::dtl::deallocate
                (
                    data_.ref_to_alloc(),
                    data_.first_,
                    std::distance(data_.first_, data_.eos_)
                );
            }

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

    void initialize_copy(const small_unordered_flat_multimap& other)
    {
        const size_type n = other.size();

        check_size(n, "sfl::small_unordered_flat_multimap::initialize_copy");

        if (n > N)
        {
            data_.first_ = sfl::dtl::allocate(data_.ref_to_alloc(), n);
            data_.last_  = data_.first_;
            data_.eos_   = data_.first_ + n;
        }

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
            if (n > N)
            {
                sfl::dtl::deallocate(data_.ref_to_alloc(), data_.first_, n);
            }

            SFL_RETHROW;
        }
    }

    void initialize_move(small_unordered_flat_multimap& other)
    {
        if (other.data_.first_ == other.data_.internal_storage())
        {
            data_.last_ = sfl::dtl::uninitialized_move_a
            (
                data_.ref_to_alloc(),
                other.data_.first_,
                other.data_.last_,
                data_.first_
            );
        }
        else if (data_.ref_to_alloc() == other.data_.ref_to_alloc())
        {
            data_.first_ = other.data_.first_;
            data_.last_  = other.data_.last_;
            data_.eos_   = other.data_.eos_;

            other.data_.first_ = nullptr;
            other.data_.last_  = nullptr;
            other.data_.eos_   = nullptr;
        }
        else
        {
            const size_type n = other.size();

            check_size(n, "sfl::small_unordered_flat_multimap::initialize_move");

            if (n > N)
            {
                data_.first_ = sfl::dtl::allocate(data_.ref_to_alloc(), n);
                data_.last_  = data_.first_;
                data_.eos_   = data_.first_ + n;
            }

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
                if (n > N)
                {
                    sfl::dtl::deallocate(data_.ref_to_alloc(), data_.first_, n);
                }

                SFL_RETHROW;
            }
        }
    }

    template <typename ForwardIt>
    void assign_range(ForwardIt first, ForwardIt last)
    {
        const size_type n = std::distance(first, last);

        check_size(n, "sfl::small_unordered_flat_multimap::assign_range");

        if (n <= capacity())
        {
            const size_type s = size();

            if (n <= s)
            {
                pointer new_last = sfl::dtl::copy
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
                ForwardIt mid = std::next(first, s);

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
        else
        {
            reset(n);

            data_.last_ = sfl::dtl::uninitialized_copy_a
            (
                data_.ref_to_alloc(),
                first,
                last,
                data_.first_
            );
        }
    }

    void assign_copy(const small_unordered_flat_multimap& other)
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

            data_.ref_to_equal() = other.data_.ref_to_equal();

            assign_range(other.data_.first_, other.data_.last_);
        }
    }

    void assign_move(small_unordered_flat_multimap& other)
    {
        if (allocator_traits::propagate_on_container_move_assignment::value)
        {
            if (data_.ref_to_alloc() != other.data_.ref_to_alloc())
            {
                reset();
            }

            data_.ref_to_alloc() = std::move(other.data_.ref_to_alloc());
        }

        data_.ref_to_equal() = other.data_.ref_to_equal();

        if (other.data_.first_ == other.data_.internal_storage())
        {
            assign_range
            (
                std::make_move_iterator(other.data_.first_),
                std::make_move_iterator(other.data_.last_)
            );
        }
        else if (data_.ref_to_alloc() == other.data_.ref_to_alloc())
        {
            reset();

            data_.first_ = other.data_.first_;
            data_.last_  = other.data_.last_;
            data_.eos_   = other.data_.eos_;

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
            const size_type new_cap =
                calculate_new_capacity(1, "sfl::small_unordered_flat_multimap::emplace_back");

            pointer new_first;
            pointer new_last;
            pointer new_eos;

            if (new_cap <= N && data_.first_ != data_.internal_storage())
            {
                new_first = data_.internal_storage();
                new_last  = new_first;
                new_eos   = new_first + N;
            }
            else
            {
                new_first = sfl::dtl::allocate(data_.ref_to_alloc(), new_cap);
                new_last  = new_first;
                new_eos   = new_first + new_cap;
            }

            const pointer p = new_first + size();

            SFL_TRY
            {
                sfl::dtl::construct_at_a
                (
                    data_.ref_to_alloc(),
                    p,
                    std::forward<Args>(args)...
                );

                new_last = nullptr;

                new_last = sfl::dtl::uninitialized_move_if_noexcept_a
                (
                    data_.ref_to_alloc(),
                    data_.first_,
                    data_.last_,
                    new_first
                );

                ++new_last;
            }
            SFL_CATCH (...)
            {
                if (new_last == nullptr)
                {
                    sfl::dtl::destroy_at_a
                    (
                        data_.ref_to_alloc(),
                        p
                    );
                }
                else
                {
                    // Nothing to do
                }

                if (new_first != data_.internal_storage())
                {
                    sfl::dtl::deallocate
                    (
                        data_.ref_to_alloc(),
                        new_first,
                        new_cap
                    );
                }

                SFL_RETHROW;
            }

            sfl::dtl::destroy_a
            (
                data_.ref_to_alloc(),
                data_.first_,
                data_.last_
            );

            if (data_.first_ != data_.internal_storage())
            {
                sfl::dtl::deallocate
                (
                    data_.ref_to_alloc(),
                    data_.first_,
                    std::distance(data_.first_, data_.eos_)
                );
            }

            data_.first_ = new_first;
            data_.last_  = new_last;
            data_.eos_   = new_eos;

            return iterator(p);
        }
    }
};

//
// ---- NON-MEMBER FUNCTIONS --------------------------------------------------
//

template <typename K, typename T, std::size_t N, typename E, typename A>
SFL_NODISCARD
bool operator==
(
    const small_unordered_flat_multimap<K, T, N, E, A>& x,
    const small_unordered_flat_multimap<K, T, N, E, A>& y
)
{
    return x.size() == y.size() && std::is_permutation(x.begin(), x.end(), y.begin());
}

template <typename K, typename T, std::size_t N, typename E, typename A>
SFL_NODISCARD
bool operator!=
(
    const small_unordered_flat_multimap<K, T, N, E, A>& x,
    const small_unordered_flat_multimap<K, T, N, E, A>& y
)
{
    return !(x == y);
}

template <typename K, typename T, std::size_t N, typename E, typename A>
void swap
(
    small_unordered_flat_multimap<K, T, N, E, A>& x,
    small_unordered_flat_multimap<K, T, N, E, A>& y
)
{
    x.swap(y);
}

template <typename K, typename T, std::size_t N, typename E, typename A,
          typename Predicate>
typename small_unordered_flat_multimap<K, T, N, E, A>::size_type
    erase_if(small_unordered_flat_multimap<K, T, N, E, A>& c, Predicate pred)
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

#endif // SFL_SMALL_UNORDERED_FLAT_MULTIMAP_HPP_INCLUDED
