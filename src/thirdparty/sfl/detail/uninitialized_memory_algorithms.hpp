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

#ifndef SFL_DETAIL_UNINITIALIZED_MEMORY_ALGORITHMS_HPP_INCLUDED
#define SFL_DETAIL_UNINITIALIZED_MEMORY_ALGORITHMS_HPP_INCLUDED

#include <sfl/detail/cpp.hpp>
#include <sfl/detail/to_address.hpp>
#include <sfl/detail/type_traits.hpp>

#include <algorithm>
#include <iterator>
#include <memory>
#include <utility>

namespace sfl
{

namespace dtl
{

template <typename Allocator, typename Size>
auto allocate(Allocator& a, Size n) -> typename std::allocator_traits<Allocator>::pointer
{
    if (n != 0)
    {
        return std::allocator_traits<Allocator>::allocate(a, n);
    }
    return nullptr;
}

template <typename Allocator, typename Pointer, typename Size>
void deallocate(Allocator& a, Pointer p, Size n) noexcept
{
    if (p != nullptr)
    {
        std::allocator_traits<Allocator>::deallocate(a, p, n);
    }
}

template <typename Allocator, typename Pointer, typename... Args>
void construct_at_a(Allocator& a, Pointer p, Args&&... args)
{
    std::allocator_traits<Allocator>::construct
    (
        a,
        sfl::dtl::to_address(p),
        std::forward<Args>(args)...
    );
}

template <typename Allocator, typename Pointer>
void destroy_at_a(Allocator& a, Pointer p) noexcept
{
    std::allocator_traits<Allocator>::destroy
    (
        a,
        sfl::dtl::to_address(p)
    );
}

template <typename Allocator, typename ForwardIt,
          sfl::dtl::enable_if_t< !sfl::dtl::is_segmented_iterator<ForwardIt>::value >* = nullptr>
void destroy_a(Allocator& a, ForwardIt first, ForwardIt last) noexcept
{
    while (first != last)
    {
        sfl::dtl::destroy_at_a(a, std::addressof(*first));
        ++first;
    }
}

template <typename Allocator, typename ForwardIt,
          sfl::dtl::enable_if_t< sfl::dtl::is_segmented_iterator<ForwardIt>::value >* = nullptr>
void destroy_a(Allocator& a, ForwardIt first, ForwardIt last) noexcept
{
    using traits = sfl::dtl::segmented_iterator_traits<ForwardIt>;

    auto first_seg = traits::segment(first);
    auto last_seg  = traits::segment(last);

    if (first_seg == last_seg)
    {
        sfl::dtl::destroy_a
        (
            a,
            traits::local(first),
            traits::local(last)
        );
    }
    else
    {
        sfl::dtl::destroy_a
        (
            a,
            traits::local(first),
            traits::end(first_seg)
        );

        ++first_seg;

        while (first_seg != last_seg)
        {
            sfl::dtl::destroy_a
            (
                a,
                traits::begin(first_seg),
                traits::end(first_seg)
            );

            ++first_seg;
        }

        sfl::dtl::destroy_a
        (
            a,
            traits::begin(last_seg),
            traits::local(last)
        );
    }
}

template <typename Allocator, typename ForwardIt, typename Size,
          sfl::dtl::enable_if_t< !sfl::dtl::is_segmented_iterator<ForwardIt>::value >* = nullptr>
ForwardIt destroy_n_a(Allocator& a, ForwardIt first, Size n) noexcept
{
    while (n > 0)
    {
        sfl::dtl::destroy_at_a(a, std::addressof(*first));
        ++first;
        --n;
    }
    return first;
}

template <typename Allocator, typename ForwardIt, typename Size,
          sfl::dtl::enable_if_t< sfl::dtl::is_segmented_iterator<ForwardIt>::value >* = nullptr>
ForwardIt destroy_n_a(Allocator& a, ForwardIt first, Size n) noexcept
{
    using traits = sfl::dtl::segmented_iterator_traits<ForwardIt>;

    auto curr_local = traits::local(first);
    auto curr_seg   = traits::segment(first);

    auto remainining = n;

    while (true)
    {
        using difference_type =
            typename std::iterator_traits<typename traits::local_iterator>::difference_type;

        const auto count = std::min<difference_type>
        (
            remainining,
            std::distance(curr_local, traits::end(curr_seg))
        );

        curr_local = sfl::dtl::destroy_n_a
        (
            a,
            curr_local,
            count
        );

        remainining -= count;

        SFL_ASSERT(remainining <= n && "Bug in algorithm. Please report it.");

        if (remainining == 0)
        {
            return traits::compose(curr_seg, curr_local);
        }

        ++curr_seg;

        curr_local = traits::begin(curr_seg);
    }
}

template <typename Allocator, typename ForwardIt,
          sfl::dtl::enable_if_t< !sfl::dtl::is_segmented_iterator<ForwardIt>::value >* = nullptr>
void uninitialized_default_construct_a(Allocator& a, ForwardIt first, ForwardIt last)
{
    ForwardIt curr = first;
    SFL_TRY
    {
        while (curr != last)
        {
            sfl::dtl::construct_at_a(a, std::addressof(*curr));
            ++curr;
        }
    }
    SFL_CATCH (...)
    {
        sfl::dtl::destroy_a(a, first, curr);
        SFL_RETHROW;
    }
}

template <typename Allocator, typename ForwardIt,
          sfl::dtl::enable_if_t< sfl::dtl::is_segmented_iterator<ForwardIt>::value >* = nullptr>
void uninitialized_default_construct_a(Allocator& a, ForwardIt first, ForwardIt last)
{
    using traits = sfl::dtl::segmented_iterator_traits<ForwardIt>;

    auto first_seg = traits::segment(first);
    auto last_seg  = traits::segment(last);

    if (first_seg == last_seg)
    {
        sfl::dtl::uninitialized_default_construct_a
        (
            a,
            traits::local(first),
            traits::local(last)
        );
    }
    else
    {
        sfl::dtl::uninitialized_default_construct_a
        (
            a,
            traits::local(first),
            traits::end(first_seg)
        );

        ++first_seg;

        SFL_TRY
        {
            while (first_seg != last_seg)
            {
                sfl::dtl::uninitialized_default_construct_a
                (
                    a,
                    traits::begin(first_seg),
                    traits::end(first_seg)
                );

                ++first_seg;
            }

            sfl::dtl::uninitialized_default_construct_a
            (
                a,
                traits::begin(last_seg),
                traits::local(last)
            );
        }
        SFL_CATCH (...)
        {
            sfl::dtl::destroy_a
            (
                a,
                first,
                traits::compose(first_seg, traits::begin(first_seg))
            );

            SFL_RETHROW;
        }
    }
}

template <typename Allocator, typename ForwardIt, typename Size,
          sfl::dtl::enable_if_t< !sfl::dtl::is_segmented_iterator<ForwardIt>::value >* = nullptr>
ForwardIt uninitialized_default_construct_n_a(Allocator& a, ForwardIt first, Size n)
{
    ForwardIt curr = first;
    SFL_TRY
    {
        while (n > 0)
        {
            sfl::dtl::construct_at_a(a, std::addressof(*curr));
            ++curr;
            --n;
        }
        return curr;
    }
    SFL_CATCH (...)
    {
        sfl::dtl::destroy_a(a, first, curr);
        SFL_RETHROW;
    }
}

template <typename Allocator, typename ForwardIt, typename Size,
          sfl::dtl::enable_if_t< sfl::dtl::is_segmented_iterator<ForwardIt>::value >* = nullptr>
ForwardIt uninitialized_default_construct_n_a(Allocator& a, ForwardIt first, Size n)
{
    using traits = sfl::dtl::segmented_iterator_traits<ForwardIt>;

    auto curr_local = traits::local(first);
    auto curr_seg   = traits::segment(first);

    auto remainining = n;

    SFL_TRY
    {
        while (true)
        {
            using difference_type =
                typename std::iterator_traits<typename traits::local_iterator>::difference_type;

            const auto count = std::min<difference_type>
            (
                remainining,
                std::distance(curr_local, traits::end(curr_seg))
            );

            curr_local = sfl::dtl::uninitialized_default_construct_n_a
            (
                a,
                curr_local,
                count
            );

            remainining -= count;

            SFL_ASSERT(remainining <= n && "Bug in algorithm. Please report it.");

            if (remainining == 0)
            {
                return traits::compose(curr_seg, curr_local);
            }

            ++curr_seg;

            curr_local = traits::begin(curr_seg);
        }
    }
    SFL_CATCH (...)
    {
        sfl::dtl::destroy_n_a(a, first, n - remainining);
        SFL_RETHROW;
    }
}

template <typename Allocator, typename ForwardIt, typename T,
          sfl::dtl::enable_if_t< !sfl::dtl::is_segmented_iterator<ForwardIt>::value >* = nullptr>
void uninitialized_fill_a(Allocator& a, ForwardIt first, ForwardIt last, const T& value)
{
    ForwardIt curr = first;
    SFL_TRY
    {
        while (curr != last)
        {
            sfl::dtl::construct_at_a(a, std::addressof(*curr), value);
            ++curr;
        }
    }
    SFL_CATCH (...)
    {
        sfl::dtl::destroy_a(a, first, curr);
        SFL_RETHROW;
    }
}

template <typename Allocator, typename ForwardIt, typename T,
          sfl::dtl::enable_if_t< sfl::dtl::is_segmented_iterator<ForwardIt>::value >* = nullptr>
void uninitialized_fill_a(Allocator& a, ForwardIt first, ForwardIt last, const T& value)
{
    using traits = sfl::dtl::segmented_iterator_traits<ForwardIt>;

    auto first_seg = traits::segment(first);
    auto last_seg  = traits::segment(last);

    if (first_seg == last_seg)
    {
        sfl::dtl::uninitialized_fill_a
        (
            a,
            traits::local(first),
            traits::local(last),
            value
        );
    }
    else
    {
        sfl::dtl::uninitialized_fill_a
        (
            a,
            traits::local(first),
            traits::end(first_seg),
            value
        );

        ++first_seg;

        SFL_TRY
        {
            while (first_seg != last_seg)
            {
                sfl::dtl::uninitialized_fill_a
                (
                    a,
                    traits::begin(first_seg),
                    traits::end(first_seg),
                    value
                );

                ++first_seg;
            }

            sfl::dtl::uninitialized_fill_a
            (
                a,
                traits::begin(last_seg),
                traits::local(last),
                value
            );
        }
        SFL_CATCH (...)
        {
            sfl::dtl::destroy_a
            (
                a,
                first,
                traits::compose(first_seg, traits::begin(first_seg))
            );

            SFL_RETHROW;
        }
    }
}

template <typename Allocator, typename ForwardIt, typename Size, typename T,
          sfl::dtl::enable_if_t< !sfl::dtl::is_segmented_iterator<ForwardIt>::value >* = nullptr>
ForwardIt uninitialized_fill_n_a(Allocator& a, ForwardIt first, Size n, const T& value)
{
    ForwardIt curr = first;
    SFL_TRY
    {
        while (n > 0)
        {
            sfl::dtl::construct_at_a(a, std::addressof(*curr), value);
            ++curr;
            --n;
        }
        return curr;
    }
    SFL_CATCH (...)
    {
        sfl::dtl::destroy_a(a, first, curr);
        SFL_RETHROW;
    }
}

template <typename Allocator, typename ForwardIt, typename Size, typename T,
          sfl::dtl::enable_if_t< sfl::dtl::is_segmented_iterator<ForwardIt>::value >* = nullptr>
ForwardIt uninitialized_fill_n_a(Allocator& a, ForwardIt first, Size n, const T& value)
{
    using traits = sfl::dtl::segmented_iterator_traits<ForwardIt>;

    auto curr_local = traits::local(first);
    auto curr_seg   = traits::segment(first);

    auto remainining = n;

    SFL_TRY
    {
        while (true)
        {
            using difference_type =
                typename std::iterator_traits<typename traits::local_iterator>::difference_type;

            const auto count = std::min<difference_type>
            (
                remainining,
                std::distance(curr_local, traits::end(curr_seg))
            );

            curr_local = sfl::dtl::uninitialized_fill_n_a
            (
                a,
                curr_local,
                count,
                value
            );

            remainining -= count;

            SFL_ASSERT(remainining <= n && "Bug in algorithm. Please report it.");

            if (remainining == 0)
            {
                return traits::compose(curr_seg, curr_local);
            }

            ++curr_seg;

            curr_local = traits::begin(curr_seg);
        }
    }
    SFL_CATCH (...)
    {
        sfl::dtl::destroy_n_a(a, first, n - remainining);
        SFL_RETHROW;
    }
}

template <typename Allocator, typename InputIt, typename ForwardIt,
          sfl::dtl::enable_if_t< (!sfl::dtl::is_segmented_iterator<InputIt>::value &&
                                  !sfl::dtl::is_segmented_iterator<ForwardIt>::value) ||
                                 (!sfl::dtl::is_segmented_iterator<InputIt>::value &&
                                   sfl::dtl::is_segmented_iterator<ForwardIt>::value &&
                                  !sfl::dtl::is_random_access_iterator<InputIt>::value) >* = nullptr>
ForwardIt uninitialized_copy_a(Allocator& a, InputIt first, InputIt last, ForwardIt d_first)
{
    ForwardIt d_curr = d_first;
    SFL_TRY
    {
        while (first != last)
        {
            sfl::dtl::construct_at_a(a, std::addressof(*d_curr), *first);
            ++d_curr;
            ++first;
        }
        return d_curr;
    }
    SFL_CATCH (...)
    {
        sfl::dtl::destroy_a(a, d_first, d_curr);
        SFL_RETHROW;
    }
}

template <typename Allocator, typename InputIt, typename ForwardIt,
          sfl::dtl::enable_if_t< !sfl::dtl::is_segmented_iterator<InputIt>::value &&
                                  sfl::dtl::is_segmented_iterator<ForwardIt>::value &&
                                  sfl::dtl::is_random_access_iterator<InputIt>::value >* = nullptr>
ForwardIt uninitialized_copy_a(Allocator& a, InputIt first, InputIt last, ForwardIt d_first)
{
    using traits = sfl::dtl::segmented_iterator_traits<ForwardIt>;

    if (first == last)
    {
        return d_first;
    }
    else
    {
        auto curr = first;

        auto d_local = traits::local(d_first);
        auto d_seg   = traits::segment(d_first);

        SFL_TRY
        {
            while (true)
            {
                using difference_type =
                    typename std::iterator_traits<InputIt>::difference_type;

                const auto count = std::min<difference_type>
                (
                    std::distance(curr, last),
                    std::distance(d_local, traits::end(d_seg))
                );

                const auto next = curr + count;

                d_local = sfl::dtl::uninitialized_copy_a
                (
                    a,
                    curr,
                    next,
                    d_local
                );

                curr = next;

                if (curr == last)
                {
                    return traits::compose(d_seg, d_local);
                }

                ++d_seg;

                d_local = traits::begin(d_seg);
            }
        }
        SFL_CATCH (...)
        {
            sfl::dtl::destroy_a
            (
                a,
                d_first,
                traits::compose(d_seg, d_local)
            );
            SFL_RETHROW;
        }
    }
}

template <typename Allocator, typename InputIt, typename ForwardIt,
          sfl::dtl::enable_if_t< sfl::dtl::is_segmented_iterator<InputIt>::value >* = nullptr>
ForwardIt uninitialized_copy_a(Allocator& a, InputIt first, InputIt last, ForwardIt d_first)
{
    using traits = sfl::dtl::segmented_iterator_traits<InputIt>;

    auto first_seg = traits::segment(first);
    auto last_seg  = traits::segment(last);

    if (first_seg == last_seg)
    {
        return sfl::dtl::uninitialized_copy_a
        (
            a,
            traits::local(first),
            traits::local(last),
            d_first
        );
    }
    else
    {
        auto d_curr = d_first;

        d_curr = sfl::dtl::uninitialized_copy_a
        (
            a,
            traits::local(first),
            traits::end(first_seg),
            d_curr
        );

        ++first_seg;

        SFL_TRY
        {
            while (first_seg != last_seg)
            {
                d_curr = sfl::dtl::uninitialized_copy_a
                (
                    a,
                    traits::begin(first_seg),
                    traits::end(first_seg),
                    d_curr
                );

                ++first_seg;
            }

            d_curr = sfl::dtl::uninitialized_copy_a
            (
                a,
                traits::begin(last_seg),
                traits::local(last),
                d_curr
            );

            return d_curr;
        }
        SFL_CATCH (...)
        {
            sfl::dtl::destroy_a
            (
                a,
                d_first,
                d_curr
            );
            SFL_RETHROW;
        }
    }
}

template <typename Allocator, typename InputIt, typename ForwardIt,
          sfl::dtl::enable_if_t< (!sfl::dtl::is_segmented_iterator<InputIt>::value &&
                                  !sfl::dtl::is_segmented_iterator<ForwardIt>::value) ||
                                 (!sfl::dtl::is_segmented_iterator<InputIt>::value &&
                                   sfl::dtl::is_segmented_iterator<ForwardIt>::value &&
                                  !sfl::dtl::is_random_access_iterator<InputIt>::value) >* = nullptr>
ForwardIt uninitialized_move_a(Allocator& a, InputIt first, InputIt last, ForwardIt d_first)
{
    ForwardIt d_curr = d_first;
    SFL_TRY
    {
        while (first != last)
        {
            sfl::dtl::construct_at_a(a, std::addressof(*d_curr), std::move(*first));
            ++d_curr;
            ++first;
        }
        return d_curr;
    }
    SFL_CATCH (...)
    {
        sfl::dtl::destroy_a(a, d_first, d_curr);
        SFL_RETHROW;
    }
}

template <typename Allocator, typename InputIt, typename ForwardIt,
          sfl::dtl::enable_if_t< !sfl::dtl::is_segmented_iterator<InputIt>::value &&
                                  sfl::dtl::is_segmented_iterator<ForwardIt>::value &&
                                  sfl::dtl::is_random_access_iterator<InputIt>::value >* = nullptr>
ForwardIt uninitialized_move_a(Allocator& a, InputIt first, InputIt last, ForwardIt d_first)
{
    using traits = sfl::dtl::segmented_iterator_traits<ForwardIt>;

    if (first == last)
    {
        return d_first;
    }
    else
    {
        auto curr = first;

        auto d_local = traits::local(d_first);
        auto d_seg   = traits::segment(d_first);

        SFL_TRY
        {
            while (true)
            {
                using difference_type =
                    typename std::iterator_traits<InputIt>::difference_type;

                const auto count = std::min<difference_type>
                (
                    std::distance(curr, last),
                    std::distance(d_local, traits::end(d_seg))
                );

                const auto next = curr + count;

                d_local = sfl::dtl::uninitialized_move_a
                (
                    a,
                    curr,
                    next,
                    d_local
                );

                curr = next;

                if (curr == last)
                {
                    return traits::compose(d_seg, d_local);
                }

                ++d_seg;

                d_local = traits::begin(d_seg);
            }
        }
        SFL_CATCH (...)
        {
            sfl::dtl::destroy_a
            (
                a,
                d_first,
                traits::compose(d_seg, d_local)
            );
            SFL_RETHROW;
        }
    }
}

template <typename Allocator, typename InputIt, typename ForwardIt,
          sfl::dtl::enable_if_t< sfl::dtl::is_segmented_iterator<InputIt>::value >* = nullptr>
ForwardIt uninitialized_move_a(Allocator& a, InputIt first, InputIt last, ForwardIt d_first)
{
    using traits = sfl::dtl::segmented_iterator_traits<InputIt>;

    auto first_seg = traits::segment(first);
    auto last_seg  = traits::segment(last);

    if (first_seg == last_seg)
    {
        return sfl::dtl::uninitialized_move_a
        (
            a,
            traits::local(first),
            traits::local(last),
            d_first
        );
    }
    else
    {
        auto d_curr = d_first;

        d_curr = sfl::dtl::uninitialized_move_a
        (
            a,
            traits::local(first),
            traits::end(first_seg),
            d_curr
        );

        ++first_seg;

        SFL_TRY
        {
            while (first_seg != last_seg)
            {
                d_curr = sfl::dtl::uninitialized_move_a
                (
                    a,
                    traits::begin(first_seg),
                    traits::end(first_seg),
                    d_curr
                );

                ++first_seg;
            }

            d_curr = sfl::dtl::uninitialized_move_a
            (
                a,
                traits::begin(last_seg),
                traits::local(last),
                d_curr
            );

            return d_curr;
        }
        SFL_CATCH (...)
        {
            sfl::dtl::destroy_a
            (
                a,
                d_first,
                d_curr
            );
            SFL_RETHROW;
        }
    }
}

template <typename Allocator, typename InputIt, typename ForwardIt,
          sfl::dtl::enable_if_t< (!sfl::dtl::is_segmented_iterator<InputIt>::value &&
                                  !sfl::dtl::is_segmented_iterator<ForwardIt>::value) ||
                                 (!sfl::dtl::is_segmented_iterator<InputIt>::value &&
                                   sfl::dtl::is_segmented_iterator<ForwardIt>::value &&
                                  !sfl::dtl::is_random_access_iterator<InputIt>::value) >* = nullptr>
ForwardIt uninitialized_move_if_noexcept_a(Allocator& a, InputIt first, InputIt last, ForwardIt d_first)
{
    ForwardIt d_curr = d_first;
    SFL_TRY
    {
        while (first != last)
        {
            sfl::dtl::construct_at_a(a, std::addressof(*d_curr), std::move_if_noexcept(*first));
            ++d_curr;
            ++first;
        }
        return d_curr;
    }
    SFL_CATCH (...)
    {
        sfl::dtl::destroy_a(a, d_first, d_curr);
        SFL_RETHROW;
    }
}

template <typename Allocator, typename InputIt, typename ForwardIt,
          sfl::dtl::enable_if_t< !sfl::dtl::is_segmented_iterator<InputIt>::value &&
                                  sfl::dtl::is_segmented_iterator<ForwardIt>::value &&
                                  sfl::dtl::is_random_access_iterator<InputIt>::value >* = nullptr>
ForwardIt uninitialized_move_if_noexcept_a(Allocator& a, InputIt first, InputIt last, ForwardIt d_first)
{
    using traits = sfl::dtl::segmented_iterator_traits<ForwardIt>;

    if (first == last)
    {
        return d_first;
    }
    else
    {
        auto curr = first;

        auto d_local = traits::local(d_first);
        auto d_seg   = traits::segment(d_first);

        SFL_TRY
        {
            while (true)
            {
                using difference_type =
                    typename std::iterator_traits<InputIt>::difference_type;

                const auto count = std::min<difference_type>
                (
                    std::distance(curr, last),
                    std::distance(d_local, traits::end(d_seg))
                );

                const auto next = curr + count;

                d_local = sfl::dtl::uninitialized_move_if_noexcept_a
                (
                    a,
                    curr,
                    next,
                    d_local
                );

                curr = next;

                if (curr == last)
                {
                    return traits::compose(d_seg, d_local);
                }

                ++d_seg;

                d_local = traits::begin(d_seg);
            }
        }
        SFL_CATCH (...)
        {
            sfl::dtl::destroy_a
            (
                a,
                d_first,
                traits::compose(d_seg, d_local)
            );
            SFL_RETHROW;
        }
    }
}

template <typename Allocator, typename InputIt, typename ForwardIt,
          sfl::dtl::enable_if_t< sfl::dtl::is_segmented_iterator<InputIt>::value >* = nullptr>
ForwardIt uninitialized_move_if_noexcept_a(Allocator& a, InputIt first, InputIt last, ForwardIt d_first)
{
    using traits = sfl::dtl::segmented_iterator_traits<InputIt>;

    auto first_seg = traits::segment(first);
    auto last_seg  = traits::segment(last);

    if (first_seg == last_seg)
    {
        return sfl::dtl::uninitialized_move_if_noexcept_a
        (
            a,
            traits::local(first),
            traits::local(last),
            d_first
        );
    }
    else
    {
        auto d_curr = d_first;

        d_curr = sfl::dtl::uninitialized_move_if_noexcept_a
        (
            a,
            traits::local(first),
            traits::end(first_seg),
            d_curr
        );

        ++first_seg;

        SFL_TRY
        {
            while (first_seg != last_seg)
            {
                d_curr = sfl::dtl::uninitialized_move_if_noexcept_a
                (
                    a,
                    traits::begin(first_seg),
                    traits::end(first_seg),
                    d_curr
                );

                ++first_seg;
            }

            d_curr = sfl::dtl::uninitialized_move_if_noexcept_a
            (
                a,
                traits::begin(last_seg),
                traits::local(last),
                d_curr
            );

            return d_curr;
        }
        SFL_CATCH (...)
        {
            sfl::dtl::destroy_a
            (
                a,
                d_first,
                d_curr
            );
            SFL_RETHROW;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename T>
void default_construct_at(T* p)
{
    ::new (static_cast<void*>(p)) T;
}

template <typename T>
void value_construct_at(T* p)
{
    ::new (static_cast<void*>(p)) T();
}

template <typename T, typename... Args>
void construct_at(T* p, Args&&... args)
{
    ::new (static_cast<void*>(p)) T(std::forward<Args>(args)...);
}

template <typename T>
void destroy_at(T* p) noexcept
{
    p->~T();
}

template <typename ForwardIt>
void destroy(ForwardIt first, ForwardIt last) noexcept
{
    while (first != last)
    {
        sfl::dtl::destroy_at(std::addressof(*first));
        ++first;
    }
}

template <typename ForwardIt, typename Size>
ForwardIt uninitialized_default_construct_n(ForwardIt first, Size n)
{
    ForwardIt curr = first;
    SFL_TRY
    {
        while (n > 0)
        {
            sfl::dtl::default_construct_at(std::addressof(*curr));
            ++curr;
            --n;
        }
        return curr;
    }
    SFL_CATCH (...)
    {
        sfl::dtl::destroy(first, curr);
        SFL_RETHROW;
    }
}

template <typename ForwardIt, typename Size>
ForwardIt uninitialized_value_construct_n(ForwardIt first, Size n)
{
    ForwardIt curr = first;
    SFL_TRY
    {
        while (n > 0)
        {
            sfl::dtl::value_construct_at(std::addressof(*curr));
            ++curr;
            --n;
        }
        return curr;
    }
    SFL_CATCH (...)
    {
        sfl::dtl::destroy(first, curr);
        SFL_RETHROW;
    }
}

template <typename ForwardIt, typename T>
void uninitialized_fill(ForwardIt first, ForwardIt last, const T& value)
{
    ForwardIt curr = first;
    SFL_TRY
    {
        while (curr != last)
        {
            sfl::dtl::construct_at(std::addressof(*curr), value);
            ++curr;
        }
    }
    SFL_CATCH (...)
    {
        sfl::dtl::destroy(first, curr);
        SFL_RETHROW;
    }
}

template <typename ForwardIt, typename Size, typename T>
ForwardIt uninitialized_fill_n(ForwardIt first, Size n, const T& value)
{
    ForwardIt curr = first;
    SFL_TRY
    {
        while (n > 0)
        {
            sfl::dtl::construct_at(std::addressof(*curr), value);
            ++curr;
            --n;
        }
        return curr;
    }
    SFL_CATCH (...)
    {
        sfl::dtl::destroy(first, curr);
        SFL_RETHROW;
    }
}

template <typename InputIt, typename ForwardIt>
ForwardIt uninitialized_copy(InputIt first, InputIt last, ForwardIt d_first)
{
    ForwardIt d_curr = d_first;
    SFL_TRY
    {
        while (first != last)
        {
            sfl::dtl::construct_at(std::addressof(*d_curr), *first);
            ++d_curr;
            ++first;
        }
        return d_curr;
    }
    SFL_CATCH (...)
    {
        sfl::dtl::destroy(d_first, d_curr);
        SFL_RETHROW;
    }
}

template <typename InputIt, typename ForwardIt>
ForwardIt uninitialized_move(InputIt first, InputIt last, ForwardIt d_first)
{
    ForwardIt d_curr = d_first;
    SFL_TRY
    {
        while (first != last)
        {
            sfl::dtl::construct_at(std::addressof(*d_curr), std::move(*first));
            ++d_curr;
            ++first;
        }
        return d_curr;
    }
    SFL_CATCH (...)
    {
        sfl::dtl::destroy(d_first, d_curr);
        SFL_RETHROW;
    }
}

} // namespace dtl

} // namespace sfl

#endif // SFL_DETAIL_UNINITIALIZED_MEMORY_ALGORITHMS_HPP_INCLUDED
