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

#ifndef SFL_DETAIL_NODE_SMALL_ALLOCATOR_HPP_INCLUDED
#define SFL_DETAIL_NODE_SMALL_ALLOCATOR_HPP_INCLUDED

#include <sfl/detail/allocator_traits.hpp>
#include <sfl/detail/cpp.hpp>
#include <sfl/detail/ignore_unused.hpp>
#include <sfl/detail/static_pool.hpp>
#include <sfl/detail/to_address.hpp>

#include <cstddef>      // size_t, ptrdiff_t
#include <memory>       // allocator, allocator_traits, pointer_traits
#include <utility>      // move
#include <type_traits>  // is_xxxxx, true_type, false_type

namespace sfl
{

namespace dtl
{

template <typename T, std::size_t N, typename BaseAllocator = std::allocator<T>>
class node_small_allocator : public BaseAllocator
{
    static_assert
    (
        std::is_same<typename BaseAllocator::value_type, T>::value,
        "BaseAllocator::value_type must be same as T."
    );

    template <typename, std::size_t, typename>
    friend class node_small_allocator;

private:

    using base_allocator_type = BaseAllocator;

    using static_pool_type = sfl::dtl::static_pool<T, N>;

public:

    using value_type         = T;
    using pointer            = typename sfl::dtl::allocator_traits<base_allocator_type>::pointer;
    using const_pointer      = typename sfl::dtl::allocator_traits<base_allocator_type>::const_pointer;
    using void_pointer       = typename sfl::dtl::allocator_traits<base_allocator_type>::void_pointer;
    using const_void_pointer = typename sfl::dtl::allocator_traits<base_allocator_type>::const_void_pointer;
    using reference          = T&;
    using const_reference    = const T&;
    using size_type          = std::size_t;
    using difference_type    = std::ptrdiff_t;

    using propagate_on_container_copy_assignment = typename sfl::dtl::allocator_traits<base_allocator_type>::propagate_on_container_copy_assignment;
    using propagate_on_container_move_assignment = typename sfl::dtl::allocator_traits<base_allocator_type>::propagate_on_container_move_assignment;
    using propagate_on_container_swap            = typename sfl::dtl::allocator_traits<base_allocator_type>::propagate_on_container_swap;
    using is_always_equal                        = typename sfl::dtl::allocator_traits<base_allocator_type>::is_always_equal;
    using is_partially_propagable                = std::true_type;

    template <typename U>
    struct rebind
    {
        using other = node_small_allocator
        <
            U,
            N,
            typename sfl::dtl::allocator_traits<base_allocator_type>::template rebind_alloc<U>
        >;
    };

private:

    SFL_NODISCARD
    base_allocator_type& base() noexcept
    {
        return *static_cast<base_allocator_type*>(this);
    }

    SFL_NODISCARD
    const base_allocator_type& base() const noexcept
    {
        return *static_cast<const base_allocator_type*>(this);
    }

private:

    static_pool_type pool_;

public:

    node_small_allocator() noexcept
    (
        std::is_nothrow_default_constructible<base_allocator_type>::value &&
        std::is_nothrow_default_constructible<static_pool_type>::value
    )
    {}

    node_small_allocator(const node_small_allocator& other) noexcept
        : base_allocator_type(other.base())
    {}

    node_small_allocator(node_small_allocator&& other) noexcept
        : base_allocator_type(std::move(other.base()))
    {}

    template <typename T2, std::size_t N2, typename Allocator2>
    node_small_allocator(const node_small_allocator<T2, N2, Allocator2>& other) noexcept
        : base_allocator_type(other.base())
    {}

    template <typename T2, std::size_t N2, typename Allocator2>
    node_small_allocator(node_small_allocator<T2, N2, Allocator2>&& other) noexcept
        : base_allocator_type(std::move(other.base()))
    {}

    explicit node_small_allocator(const base_allocator_type& other) noexcept
        : base_allocator_type(other)
    {}

    explicit node_small_allocator(base_allocator_type&& other) noexcept
        : base_allocator_type(std::move(other))
    {}

    node_small_allocator& operator=(const node_small_allocator& other) noexcept
    {
        base_allocator_type::operator=(other.base());
        return *this;
    }

    node_small_allocator& operator=(node_small_allocator&& other) noexcept
    {
        base_allocator_type::operator=(std::move(other.base()));
        return *this;
    }

    template <typename T2, std::size_t N2, typename Allocator2>
    node_small_allocator& operator=(const node_small_allocator<T2, N2, Allocator2>& other) noexcept
    {
        base_allocator_type::operator=(other.base());
        return *this;
    }

    template <typename T2, std::size_t N2, typename Allocator2>
    node_small_allocator& operator=(node_small_allocator<T2, N2, Allocator2>&& other) noexcept
    {
        base_allocator_type::operator=(std::move(other.base()));
        return *this;
    }

    node_small_allocator& operator=(const base_allocator_type& other) noexcept
    {
        base_allocator_type::operator=(other);
        return *this;
    }

    node_small_allocator& operator=(base_allocator_type&& other) noexcept
    {
        base_allocator_type::operator=(std::move(other));
        return *this;
    }

    SFL_NODISCARD
    pointer allocate(size_type n)
    {
        SFL_ASSERT(n == 1);

        sfl::dtl::ignore_unused(n);

        if (!pool_.full())
        {
            return std::pointer_traits<pointer>::pointer_to(*pool_.allocate());
        }
        else
        {
            return sfl::dtl::allocator_traits<base_allocator_type>::allocate(base(), 1);
        }
    }

    SFL_NODISCARD
    pointer allocate(size_type n, const void* hint)
    {
        SFL_ASSERT(n == 1);

        sfl::dtl::ignore_unused(n);

        if (!pool_.full())
        {
            return std::pointer_traits<pointer>::pointer_to(*pool_.allocate());
        }
        else
        {
            return sfl::dtl::allocator_traits<base_allocator_type>::allocate(base(), 1, hint);
        }
    }

    void deallocate(pointer p, std::size_t n) noexcept
    {
        SFL_ASSERT(n == 1);

        sfl::dtl::ignore_unused(n);

        if (pool_.contains(sfl::dtl::to_address(p)))
        {
            pool_.deallocate(sfl::dtl::to_address(p));
        }
        else
        {
            sfl::dtl::allocator_traits<base_allocator_type>::deallocate(base(), p, 1);
        }
    }

    SFL_NODISCARD
    bool is_storage_unpropagable(pointer p) const noexcept
    {
        return pool_.contains(sfl::dtl::to_address(p));
    }
};

template <typename T, typename BaseAllocator>
class node_small_allocator<T, 0, BaseAllocator> : public BaseAllocator
{
    static_assert
    (
        std::is_same<typename BaseAllocator::value_type, T>::value,
        "BaseAllocator::value_type must be same as T."
    );

    template <typename, std::size_t, typename>
    friend class node_small_allocator;

private:

    using base_allocator_type = BaseAllocator;

public:

    using value_type         = T;
    using pointer            = typename sfl::dtl::allocator_traits<base_allocator_type>::pointer;
    using const_pointer      = typename sfl::dtl::allocator_traits<base_allocator_type>::const_pointer;
    using void_pointer       = typename sfl::dtl::allocator_traits<base_allocator_type>::void_pointer;
    using const_void_pointer = typename sfl::dtl::allocator_traits<base_allocator_type>::const_void_pointer;
    using reference          = T&;
    using const_reference    = const T&;
    using size_type          = std::size_t;
    using difference_type    = std::ptrdiff_t;

    using propagate_on_container_copy_assignment = typename sfl::dtl::allocator_traits<base_allocator_type>::propagate_on_container_copy_assignment;
    using propagate_on_container_move_assignment = typename sfl::dtl::allocator_traits<base_allocator_type>::propagate_on_container_move_assignment;
    using propagate_on_container_swap            = typename sfl::dtl::allocator_traits<base_allocator_type>::propagate_on_container_swap;
    using is_always_equal                        = typename sfl::dtl::allocator_traits<base_allocator_type>::is_always_equal;
    using is_partially_propagable                = std::false_type;

    template <typename U>
    struct rebind
    {
        using other = node_small_allocator
        <
            U,
            0,
            typename sfl::dtl::allocator_traits<base_allocator_type>::template rebind_alloc<U>
        >;
    };

private:

    SFL_NODISCARD
    base_allocator_type& base() noexcept
    {
        return *static_cast<base_allocator_type*>(this);
    }

    SFL_NODISCARD
    const base_allocator_type& base() const noexcept
    {
        return *static_cast<const base_allocator_type*>(this);
    }

public:

    node_small_allocator() noexcept(std::is_nothrow_default_constructible<base_allocator_type>::value)
    {}

    node_small_allocator(const node_small_allocator& other) noexcept
        : base_allocator_type(other.base())
    {}

    node_small_allocator(node_small_allocator&& other) noexcept
        : base_allocator_type(std::move(other.base()))
    {}

    template <typename T2, std::size_t N2, typename Allocator2>
    node_small_allocator(const node_small_allocator<T2, N2, Allocator2>& other) noexcept
        : base_allocator_type(other.base())
    {}

    template <typename T2, std::size_t N2, typename Allocator2>
    node_small_allocator(node_small_allocator<T2, N2, Allocator2>&& other) noexcept
        : base_allocator_type(std::move(other.base()))
    {}

    explicit node_small_allocator(const base_allocator_type& other) noexcept
        : base_allocator_type(other)
    {}

    explicit node_small_allocator(base_allocator_type&& other) noexcept
        : base_allocator_type(std::move(other))
    {}

    node_small_allocator& operator=(const node_small_allocator& other) noexcept
    {
        base_allocator_type::operator=(other.base());
        return *this;
    }

    node_small_allocator& operator=(node_small_allocator&& other) noexcept
    {
        base_allocator_type::operator=(std::move(other.base()));
        return *this;
    }

    template <typename T2, std::size_t N2, typename Allocator2>
    node_small_allocator& operator=(const node_small_allocator<T2, N2, Allocator2>& other) noexcept
    {
        base_allocator_type::operator=(other.base());
        return *this;
    }

    template <typename T2, std::size_t N2, typename Allocator2>
    node_small_allocator& operator=(node_small_allocator<T2, N2, Allocator2>&& other) noexcept
    {
        base_allocator_type::operator=(std::move(other.base()));
        return *this;
    }

    node_small_allocator& operator=(const base_allocator_type& other) noexcept
    {
        base_allocator_type::operator=(other);
        return *this;
    }

    node_small_allocator& operator=(base_allocator_type&& other) noexcept
    {
        base_allocator_type::operator=(std::move(other));
        return *this;
    }

    SFL_NODISCARD
    pointer allocate(size_type n)
    {
        SFL_ASSERT(n == 1);

        sfl::dtl::ignore_unused(n);

        return sfl::dtl::allocator_traits<base_allocator_type>::allocate(base(), 1);
    }

    SFL_NODISCARD
    pointer allocate(size_type n, const void* hint)
    {
        SFL_ASSERT(n == 1);

        sfl::dtl::ignore_unused(n);

        return sfl::dtl::allocator_traits<base_allocator_type>::allocate(base(), 1, hint);
    }

    void deallocate(pointer p, std::size_t n) noexcept
    {
        SFL_ASSERT(n == 1);

        sfl::dtl::ignore_unused(n);

        sfl::dtl::allocator_traits<base_allocator_type>::deallocate(base(), p, 1);
    }
};

} // namespace dtl

} // namespace sfl

#endif // SFL_DETAIL_NODE_SMALL_ALLOCATOR_HPP_INCLUDED
