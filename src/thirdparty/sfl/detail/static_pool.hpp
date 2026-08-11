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

#ifndef SFL_DETAIL_STATIC_POOL_HPP_INCLUDED
#define SFL_DETAIL_STATIC_POOL_HPP_INCLUDED

#include <sfl/detail/cpp.hpp>

#include <cstddef> // size_t, ptrdiff_t
#include <memory>  // addressof

namespace sfl
{

namespace dtl
{

template <typename T, std::size_t N>
class static_pool
{
    static_assert(N > 0, "N must be greater than zero.");

public:

    using value_type      = T;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;

private:

    union element
    {
    public:

        element* next_;

        T value_;

    public:

        element() noexcept
        {}

        element(const element& other) = delete;

        element(element&& other) = delete;

        element& operator=(const element& other) = delete;

        element& operator=(element&& other) = delete;

        ~element() noexcept
        {}
    };

    element storage_[N];

    size_type n_allocated = 0;

    size_type n_initialized = 0;

    element* next_ = nullptr;

public:

    static_pool() noexcept
    {}

    static_pool(const static_pool& other) = delete;

    static_pool(static_pool&& other) = delete;

    static_pool& operator=(const static_pool& other) = delete;

    static_pool& operator=(static_pool&& other) = delete;

    ~static_pool() noexcept
    {
        SFL_ASSERT(empty());
    }

    SFL_NODISCARD
    bool empty() const noexcept
    {
        return n_allocated == 0;
    }

    SFL_NODISCARD
    bool full() const noexcept
    {
        return n_allocated == N;
    }

    SFL_NODISCARD
    static constexpr size_type max_size() noexcept
    {
        return N;
    }

    SFL_NODISCARD
    bool contains(const T* p) const noexcept
    {
        const element* q = reinterpret_cast<const element*>(p);
        return storage_ <= q && q < storage_ + N;
    }

    SFL_NODISCARD
    T* allocate() noexcept
    {
        SFL_ASSERT(n_allocated < N);

        if (n_allocated < n_initialized)
        {
            // Nothing to do.
        }
        else
        {
            SFL_ASSERT(n_initialized < N);

            element* p = std::addressof(storage_[n_initialized++]);

            p->next_ = nullptr;

            next_ = p;
        }

        element* p = next_;

        next_ = p->next_;

        ++n_allocated;

        return std::addressof(p->value_);
    }

    void deallocate(T* p) noexcept
    {
        SFL_ASSERT(n_allocated > 0);
        SFL_ASSERT(contains(p));

        element* q = reinterpret_cast<element*>(p);

        q->next_ = next_;

        next_ = q;

        --n_allocated;
    }
};

} // namespace dtl

} // namespace sfl

#endif // SFL_DETAIL_STATIC_POOL_HPP_INCLUDED
