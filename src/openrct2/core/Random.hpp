/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Meta.hpp"
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
    template<size_t _num = 0> class FixedSeedSequence
    {
    public:
        using result_type = uint32_t;

        static constexpr size_t N = _num;
        static constexpr result_type default_seed = 0x1234567F;

        explicit FixedSeedSequence()
        {
            std::fill(v.begin(), v.end(), default_seed);
        }

        template<
            typename... T, typename std::enable_if<sizeof...(T) == N, int>::type = 0,
            typename std::enable_if<Meta::all_convertible<result_type, T...>::value, int>::type = 0>
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

        template<typename _It> void generate(_It begin, _It end) const
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

    template<typename UIntType> struct RotateEngineState
    {
        using value_type = UIntType;

        value_type s0;
        value_type s1;
    };

    /**
     * RotateEngine adheres to the _Named Requirement_ `RandomNumberEngine`
     * https://en.cppreference.com/w/cpp/named_req/RandomNumberEngine
     */
    template<typename UIntType, UIntType _x, size_t _r1, size_t _r2> class RotateEngine : protected RotateEngineState<UIntType>
    {
        static_assert(std::is_unsigned<UIntType>::value, "Type must be unsigned integral.");

        using RotateEngineState<UIntType>::s0;
        using RotateEngineState<UIntType>::s1;

    public:
        using result_type = UIntType;
        using state_type = RotateEngineState<UIntType>;

        static constexpr result_type x = _x;
        static constexpr size_t r1 = _r1;
        static constexpr size_t r2 = _r2;
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
            s1 = s;
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

        const state_type& state() const
        {
            return *this;
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
    };

    namespace Rct2
    {
        using Engine = RotateEngine<uint32_t, 0x1234567F, 7, 3>;
        using Seed = FixedSeedSequence<2>;
        using State = Engine::state_type;
    } // namespace Rct2
} // namespace Random
