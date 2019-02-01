/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Numerics.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <type_traits>

namespace Random
{
    using namespace Numerics;

    /**
     * FixedSeedSequence adheres to the _Named Requirement_ `SeedSequence`.
     */
    template<size_t _N = 0> class FixedSeedSequence
    {
    public:
        typedef uint32_t result_type;

        static constexpr size_t N = _N;
        static constexpr result_type default_seed = 0x1234567F;

        explicit FixedSeedSequence()
        {
            std::fill(v.begin(), v.end(), default_seed);
        }

        template<
            typename... T, typename std::enable_if<sizeof...(T) == N, int>::type = 0,
            typename std::enable_if<(std::is_convertible<T, result_type>::value && ...), int>::type = 0>
        explicit FixedSeedSequence(T... s)
            : v{ static_cast<result_type>(s)... }
        {
        }

        template<typename _It, typename = decltype(*std::declval<_It&>(), ++std::declval<_It&>(), void())>
        explicit FixedSeedSequence(_It begin, _It end)
        {
            std::copy(begin, end, v.begin());
        }

        template<typename T>
        explicit FixedSeedSequence(std::initializer_list<T> il)
            : FixedSeedSequence(il.begin(), il.end())
        {
        }

        template<typename _It> void generate(_It begin, _It end)
        {
            std::copy_n(v.begin(), std::min((size_t)(end - begin), N), begin);
        }

        constexpr size_t size() const
        {
            return N;
        }

        template<typename _It> constexpr void param(_It ob) const
        {
            std::copy(v.begin(), v.end(), ob);
        }

    protected:
        std::array<result_type, N> v;
    };

    typedef FixedSeedSequence<2> Rct2Seed;

    /**
     * RotateEngine adheres to the _Named Requirement_ `RandomNumberEngine`
     * https://en.cppreference.com/w/cpp/named_req/RandomNumberEngine
     */
    template<typename UIntType, UIntType __x, size_t __r1, size_t __r2> class RotateEngine
    {
        static_assert(std::is_unsigned<UIntType>::value, "Type must be unsigned integral.");

    public:
        typedef UIntType result_type;
        static constexpr result_type x = __x;
        static constexpr size_t r1 = __r1;
        static constexpr size_t r2 = __r2;
        static constexpr result_type default_seed = 1;

        static constexpr result_type min()
        {
            return std::numeric_limits<result_type>::min();
        }

        static constexpr result_type max()
        {
            return std::numeric_limits<result_type>::max();
        }

        explicit RotateEngine(result_type seed_value = default_seed)
        {
            seed(seed_value);
        }

        RotateEngine(RotateEngine& r)
        {
            s0 = r.s0;
            s1 = r.s1;
        }

        template<typename Sseq, typename = typename std::enable_if<!std::is_same<Sseq, RotateEngine>::value>::type>
        explicit RotateEngine(Sseq& seed_seq)
        {
            seed(seed_seq);
        }

        void seed(result_type s = default_seed)
        {
            s0 = s;
        }

        template<typename Sseq> typename std::enable_if<std::is_class<Sseq>::value, void>::type seed(Sseq& seed_seq)
        {
            std::array<result_type, 2> s;
            seed_seq.generate(s.begin(), s.end());
            s0 = s[0];
            s1 = s[1];
        }

        void discard(size_t n)
        {
            for (; n > 0; n--)
                (*this)();
        }
        result_type operator()()
        {
            auto s0z = s0;
            s0 += ror(s1 ^ x, r1);
            s1 = ror(s0z, r2);
            return s1;
        }

        friend bool operator==(const RotateEngine& lhs, const RotateEngine& rhs)
        {
            return lhs.s0 == rhs.s0 && lhs.s1 == rhs.s1;
        }

        friend std::ostream& operator<<(std::ostream& os, const RotateEngine& e)
        {
            os << e.s0 << ' ' << e.s1;
            return os;
        }

        friend std::istream& operator>>(std::istream& is, RotateEngine& e)
        {
            is >> e.s0;
            is >> e.s1;
            return is;
        }

    protected:
        result_type s0;
        result_type s1;
    };

    typedef RotateEngine<uint32_t, 0x1234567F, 7, 3> Rct2Engine;

} // namespace Random
