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

#ifndef SFL_DETAIL_ALLOCATOR_TRAITS_HPP_INCLUDED
#define SFL_DETAIL_ALLOCATOR_TRAITS_HPP_INCLUDED

#include <sfl/detail/cpp.hpp>
#include <sfl/detail/ignore_unused.hpp>
#include <sfl/detail/type_traits.hpp>

#include <limits>       // numeric_limits
#include <memory>       // pointer_traits
#include <type_traits>  // true_type, false_type
#include <utility>      // declval

#ifdef _MSC_VER // Visual C++
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

namespace sfl
{

namespace dtl
{

template <typename Allocator>
class allocator_traits
{
private:

    template <typename Alloc, typename T>
    struct has_rebind
    {
    private:
        template <typename Alloc2, typename T2>
        static std::true_type test(typename Alloc2::template rebind<T2>*);

        template <typename Alloc2, typename T2>
        static std::false_type test(...);

    public:
        using type = decltype(test<Alloc, T>(nullptr));
    };

    template <typename Alloc, typename T, typename = typename has_rebind<Alloc, T>::type>
    struct rebind_alloc_aux;

    template <typename Alloc, typename T>
    struct rebind_alloc_aux<Alloc, T, std::true_type>
    {
        using type = typename Alloc::template rebind<T>::other;
    };

    template <template <typename, typename...> class Alloc, typename T, typename U, typename... Args>
    struct rebind_alloc_aux<Alloc<T, Args...>, U, std::false_type>
    {
        using type = Alloc<U, Args...>;
    };

    ///////////////////////////////////////////////////////////////////////////

    template <typename Alloc, typename SizeType, typename ConstVoidPointer>
    struct has_allocate_hint
    {
    private:
        template <typename Alloc2, typename SizeType2, typename ConstVoidPointer2,
                  sfl::dtl::void_t<decltype(std::declval<Alloc2>().allocate(std::declval<SizeType2>(),
                                                                            std::declval<ConstVoidPointer2>()))>* = nullptr>
        static std::true_type test(int);

        template <typename Alloc2, typename SizeType2, typename ConstVoidPointer2>
        static std::false_type test(...);

    public:
        using type = decltype(test<Alloc, SizeType, ConstVoidPointer>(0));
    };

    ///////////////////////////////////////////////////////////////////////////

    template <typename Alloc>
    struct has_max_size
    {
    private:
        template <typename Alloc2>
        static std::true_type test(decltype(&Alloc2::max_size));

        template <typename Alloc2>
        static std::false_type test(...);

    public:
        using type = decltype(test<Alloc>(nullptr));
    };

    ///////////////////////////////////////////////////////////////////////////

    template <typename Alloc>
    struct has_select_on_container_copy_construction
    {
    private:
        template <typename Alloc2>
        static std::true_type test(decltype(&Alloc2::select_on_container_copy_construction));

        template <typename Alloc2>
        static std::false_type test(...);

    public:
        using type = decltype(test<Alloc>(nullptr));
    };

public:

    using allocator_type = Allocator;

    using value_type = typename Allocator::value_type;

    ///////////////////////////////////////////////////////////////////////////
    #define SFL_NESTED_TYPE_OR_ALTERNATIVE(TNAME, ALTERNATIVE)              \
    private:                                                                \
        template <typename T> static typename T::TNAME test_##TNAME(T*);    \
        template <typename T> static ALTERNATIVE       test_##TNAME(...);   \
        using priv_##TNAME = decltype(test_##TNAME<Allocator>(nullptr));    \
    public:                                                                 \
    ///////////////////////////////////////////////////////////////////////////

    SFL_NESTED_TYPE_OR_ALTERNATIVE(pointer, value_type*)
    using pointer = priv_pointer;

    SFL_NESTED_TYPE_OR_ALTERNATIVE(const_pointer, typename std::pointer_traits<pointer>::template rebind<const value_type>)
    using const_pointer = priv_const_pointer;

    SFL_NESTED_TYPE_OR_ALTERNATIVE(void_pointer, typename std::pointer_traits<pointer>::template rebind<void>)
    using void_pointer = priv_void_pointer;

    SFL_NESTED_TYPE_OR_ALTERNATIVE(const_void_pointer, typename std::pointer_traits<pointer>::template rebind<const void>)
    using const_void_pointer = priv_const_void_pointer;

    SFL_NESTED_TYPE_OR_ALTERNATIVE(difference_type, typename std::pointer_traits<pointer>::difference_type)
    using difference_type = priv_difference_type;

    SFL_NESTED_TYPE_OR_ALTERNATIVE(size_type, typename std::make_unsigned<difference_type>::type)
    using size_type = priv_size_type;

    SFL_NESTED_TYPE_OR_ALTERNATIVE(propagate_on_container_copy_assignment, std::false_type)
    using propagate_on_container_copy_assignment = priv_propagate_on_container_copy_assignment;

    SFL_NESTED_TYPE_OR_ALTERNATIVE(propagate_on_container_move_assignment, std::false_type)
    using propagate_on_container_move_assignment = priv_propagate_on_container_move_assignment;

    SFL_NESTED_TYPE_OR_ALTERNATIVE(propagate_on_container_swap, std::false_type)
    using propagate_on_container_swap = priv_propagate_on_container_swap;

    SFL_NESTED_TYPE_OR_ALTERNATIVE(is_always_equal, typename std::is_empty<Allocator>::type)
    using is_always_equal = priv_is_always_equal;

    SFL_NESTED_TYPE_OR_ALTERNATIVE(is_partially_propagable, std::false_type)
    using is_partially_propagable = priv_is_partially_propagable;

    ///////////////////////////////////////////////////////////////////////////
    #undef SFL_NESTED_TYPE_OR_ALTERNATIVE
    ///////////////////////////////////////////////////////////////////////////

    template <typename T>
    using rebind_alloc = typename rebind_alloc_aux<Allocator, T>::type;

    template <typename T>
    using rebind_traits = allocator_traits<rebind_alloc<T>>;

public:

    SFL_NODISCARD
    static pointer allocate(Allocator& a, size_type n)
    {
        return a.allocate(n);
    }

    SFL_NODISCARD
    static pointer allocate(Allocator& a, size_type n, const_void_pointer hint)
    {
        return priv_allocate(a, n, hint, typename has_allocate_hint<Allocator, size_type, const_void_pointer>::type());
    }

    static void deallocate(Allocator& a, pointer p, size_type n)
    {
        a.deallocate(p, n);
    }

    SFL_NODISCARD
    static size_type max_size(const Allocator& a) noexcept
    {
        return priv_max_size(a, typename has_max_size<Allocator>::type());
    }

    SFL_NODISCARD
    static Allocator select_on_container_copy_construction(const Allocator& a)
    {
        return priv_select_on_container_copy_construction(a, typename has_select_on_container_copy_construction<Allocator>::type());
    }

    SFL_NODISCARD
    static bool is_storage_unpropagable(const Allocator& a, pointer p) noexcept
    {
        return priv_is_storage_unpropagable(a, p, is_partially_propagable());
    }

private:

    static pointer priv_allocate(Allocator& a, size_type n, const_void_pointer hint, std::true_type)
    {
        return a.allocate(n, hint);
    }

    static pointer priv_allocate(Allocator& a, size_type n, const_void_pointer hint, std::false_type)
    {
        sfl::dtl::ignore_unused(hint);
        return a.allocate(n);
    }

    static size_type priv_max_size(const Allocator& a, std::true_type)
    {
        return a.max_size();
    }

    static size_type priv_max_size(const Allocator& a, std::false_type)
    {
        sfl::dtl::ignore_unused(a);
        return std::numeric_limits<size_type>::max() / sizeof(value_type);
    }

    static Allocator priv_select_on_container_copy_construction(const Allocator& a, std::true_type)
    {
        return a.select_on_container_copy_construction(a);
    }

    static Allocator priv_select_on_container_copy_construction(const Allocator& a, std::false_type)
    {
        return a;
    }

    static bool priv_is_storage_unpropagable(const Allocator& a, pointer p, std::true_type) noexcept
    {
        return a.is_storage_unpropagable(p);
    }

    static bool priv_is_storage_unpropagable(const Allocator& a, pointer p, std::false_type) noexcept
    {
        sfl::dtl::ignore_unused(a, p);
        return false;
    }
};

} // namespace dtl

} // namespace sfl

#ifdef _MSC_VER // Visual C++
#pragma warning(pop)
#endif

#endif // SFL_DETAIL_ALLOCATOR_TRAITS_HPP_INCLUDED
