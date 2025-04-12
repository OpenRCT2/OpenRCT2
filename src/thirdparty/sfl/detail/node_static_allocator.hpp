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

#ifndef SFL_DETAIL_NODE_STATIC_ALLOCATOR_HPP_INCLUDED
#define SFL_DETAIL_NODE_STATIC_ALLOCATOR_HPP_INCLUDED

#include <sfl/detail/cpp.hpp>
#include <sfl/detail/ignore_unused.hpp>
#include <sfl/detail/static_pool.hpp>

#include <cstddef>      // size_t, ptrdiff_t
#include <utility>      // move
#include <type_traits>  // is_xxxxx, true_type, false_type

namespace sfl
{

namespace dtl
{

template <typename T, std::size_t N>
class node_static_allocator
{
private:

    using static_pool_type = sfl::dtl::static_pool<T, N>;

public:

    using value_type      = T;
    using pointer         = T*;
    using const_pointer   = const T*;
    using reference       = T&;
    using const_reference = const T&;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;

    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::false_type;
    using propagate_on_container_swap            = std::false_type;
    using is_always_equal                        = std::false_type;
    using is_partially_propagable                = std::true_type;

    template <typename U>
    struct rebind
    {
        using other = node_static_allocator<U, N>;
    };

private:

    sfl::dtl::static_pool<T, N> pool_;

public:

    node_static_allocator() noexcept(std::is_nothrow_default_constructible<static_pool_type>::value)
    {}

    node_static_allocator(const node_static_allocator& /*other*/) noexcept
    {}

    node_static_allocator(node_static_allocator&& /*other*/) noexcept
    {}

    template <typename Node2, std::size_t N2>
    node_static_allocator(const node_static_allocator<Node2, N2>& /*other*/) noexcept
    {}

    template <typename Node2, std::size_t N2>
    node_static_allocator(node_static_allocator<Node2, N2>&& /*other*/) noexcept
    {}

    node_static_allocator& operator=(const node_static_allocator& /*other*/) noexcept
    {
        return *this;
    }

    node_static_allocator& operator=(node_static_allocator&& /*other*/) noexcept
    {
        return *this;
    }

    template <typename Node2, std::size_t N2>
    node_static_allocator& operator=(const node_static_allocator<Node2, N2>& /*other*/) noexcept
    {
        return *this;
    }

    template <typename Node2, std::size_t N2>
    node_static_allocator& operator=(node_static_allocator<Node2, N2>&& /*other*/) noexcept
    {
        return *this;
    }

    SFL_NODISCARD
    pointer allocate(size_type n)
    {
        SFL_ASSERT(n == 1);

        sfl::dtl::ignore_unused(n);

        return pool_.allocate();
    }

    void deallocate(pointer p, std::size_t n) noexcept
    {
        SFL_ASSERT(n == 1);

        sfl::dtl::ignore_unused(n);

        pool_.deallocate(p);
    }

    SFL_NODISCARD
    static constexpr size_type max_size() noexcept
    {
        return static_pool_type::max_size();
    }

    SFL_NODISCARD
    bool is_storage_unpropagable(pointer p) const noexcept
    {
        sfl::dtl::ignore_unused(p);
        return true;
    }
};

template <typename T1, typename T2, std::size_t N>
SFL_NODISCARD
bool operator==
(
    const node_static_allocator<T1, N>& x,
    const node_static_allocator<T2, N>& y
) noexcept
{
    sfl::dtl::ignore_unused(x, y);
    return false;
}

template <typename T1, typename T2, std::size_t N>
SFL_NODISCARD
bool operator!=
(
    const node_static_allocator<T1, N>& x,
    const node_static_allocator<T2, N>& y
) noexcept
{
    return !(x == y);
}

} // namespace dtl

} // namespace sfl

#endif // SFL_DETAIL_NODE_STATIC_ALLOCATOR_HPP_INCLUDED
