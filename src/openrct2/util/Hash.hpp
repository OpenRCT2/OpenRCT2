/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <functional>

namespace OpenRCT2::Util::Hash
{
    // based on boost container_hash

    template<size_t S>
    struct HashMixer;

    template<>
    struct HashMixer<8>
    {
        static uint64_t mix(uint64_t hash)
        {
            constexpr uint64_t kM = 0xe9846af9b1a615d;

            hash ^= hash >> 32;
            hash *= kM;
            hash ^= hash >> 32;
            hash *= kM;
            hash ^= hash >> 28;

            return hash;
        }
    };

    template<>
    struct HashMixer<4>
    {
        static uint32_t mix(uint32_t hash)
        {
            constexpr uint32_t kM1 = 0x21f0aaad;
            constexpr uint32_t kM2 = 0x735a2d97;

            hash ^= hash >> 16;
            hash *= kM1;
            hash ^= hash >> 15;
            hash *= kM2;
            hash ^= hash >> 15;

            return hash;
        }
    };

    /**
     * Usage:
     *
     *     struct TileCoordsXYHash
     *     {
     *         size_t operator()(const TileCoordsXY& pos) const noexcept
     *         {
     *             using namespace Util::Hash;
     *             size_t hash = 0;
     *             update(hash, pos.x);
     *             update(hash, pos.y);
     *             return hash;
     *         }
     *     };
     */
    template<class T>
    void update(size_t& hash, T const& value)
    {
        hash = hash + 0x9e3779b9 + std::hash<T>()(value);
        hash = HashMixer<sizeof(size_t)>::mix(hash);
    }

} // namespace OpenRCT2::Util::Hash
