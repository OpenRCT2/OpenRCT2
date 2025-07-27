/*****************************************************************************
 * Copyright (c) 2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

namespace OpenRCT2::World::MapGenerator
{
    class Noise
    {
    public:
        virtual ~Noise() = default;
        virtual float Generate(float x, float y) = 0;
    };
} // namespace OpenRCT2::World::MapGenerator
