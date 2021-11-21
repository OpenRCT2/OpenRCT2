/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Location.hpp"

#include <cassert>
#include <cstdint>
#include <vector>

template<typename T> class TilePointerIndex
{
    std::vector<T*> TilePointers;
    uint16_t MapSize{};

public:
    TilePointerIndex() = default;

    explicit TilePointerIndex(const uint16_t mapSize, T* tileElements, size_t count)
    {
        MapSize = mapSize;
        TilePointers.reserve(MapSize * MapSize);

        size_t index = 0;
        for (size_t y = 0; y < MapSize; y++)
        {
            for (size_t x = 0; x < MapSize; x++)
            {
                assert(index < count);
                TilePointers.emplace_back(&tileElements[index]);
                do
                {
                    index++;
                } while (!tileElements[index - 1].IsLastForTile());
            }
        }
    }

    T* GetFirstElementAt(TileCoordsXY coords)
    {
        return TilePointers[coords.x + (coords.y * MapSize)];
    }

    void SetTile(TileCoordsXY coords, T* tileElement)
    {
        TilePointers[coords.x + (coords.y * MapSize)] = tileElement;
    }
};
