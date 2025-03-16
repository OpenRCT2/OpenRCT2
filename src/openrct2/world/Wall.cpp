/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Wall.h"

#include "Map.h"
#include "tile_element/TileElement.h"
#include "tile_element/WallElement.h"

/**
 *
 *  rct2: 0x006E588E
 */
void WallRemoveAt(const CoordsXYRangedZ& wallPos)
{
    for (auto wallElement = MapGetWallElementAt(wallPos); wallElement != nullptr; wallElement = MapGetWallElementAt(wallPos))
    {
        reinterpret_cast<TileElement*>(wallElement)->RemoveBannerEntry();
        MapInvalidateTileZoom1({ wallPos, wallElement->GetBaseZ(), wallElement->GetBaseZ() + 72 });
        TileElementRemove(reinterpret_cast<TileElement*>(wallElement));
    }
}

/**
 *
 *  rct2: 0x006E57E6
 */
void WallRemoveAtZ(const CoordsXYZ& wallPos)
{
    WallRemoveAt({ wallPos, wallPos.z, wallPos.z + 48 });
}

/**
 *
 *  rct2: 0x006E5935
 */
void WallRemoveIntersectingWalls(const CoordsXYRangedZ& wallPos, Direction direction)
{
    TileElement* tileElement;

    tileElement = MapGetFirstElementAt(wallPos);
    if (tileElement == nullptr)
        return;
    do
    {
        if (tileElement->GetType() != TileElementType::Wall)
            continue;

        if (tileElement->GetClearanceZ() <= wallPos.baseZ || tileElement->GetBaseZ() >= wallPos.clearanceZ)
            continue;

        if (direction != tileElement->GetDirection())
            continue;

        tileElement->RemoveBannerEntry();
        MapInvalidateTileZoom1({ wallPos, tileElement->GetBaseZ(), tileElement->GetBaseZ() + 72 });
        TileElementRemove(tileElement);
        tileElement--;
    } while (!(tileElement++)->IsLastForTile());
}

#pragma region Edge Slopes Table

// clang-format off
// rct2: 0x009A3FEC
constexpr static uint8_t kLandSlopeToWallSlope[][kNumOrthogonalDirections] = {
    //  Top right                        Bottom right                   Bottom left                       Top left
    { 0,                             0,                             0,                             0                             },
    { 0,                             EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_DOWNWARDS,          0                             },
    { 0,                             0,                             EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_DOWNWARDS          },
    { 0,                             EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_DOWNWARDS          },
    { EDGE_SLOPE_DOWNWARDS,          0,                             0,                             EDGE_SLOPE_UPWARDS            },
    { EDGE_SLOPE_DOWNWARDS,          EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_DOWNWARDS,          EDGE_SLOPE_UPWARDS            },
    { EDGE_SLOPE_DOWNWARDS,          0,                             EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_ELEVATED           },
    { EDGE_SLOPE_DOWNWARDS,          EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_ELEVATED           },
    { EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_DOWNWARDS,          0,                             0                             },
    { EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_DOWNWARDS,          0                             },
    { EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_DOWNWARDS,          EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_DOWNWARDS          },
    { EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_DOWNWARDS          },
    { EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_DOWNWARDS,          0,                             EDGE_SLOPE_UPWARDS            },
    { EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_DOWNWARDS,          EDGE_SLOPE_UPWARDS            },
    { EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_DOWNWARDS,          EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_ELEVATED           },
    { EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_ELEVATED,           EDGE_SLOPE_ELEVATED           },
    { 0,                             0,                             0,                             0                             },
    { 0,                             0,                             0,                             0                             },
    { 0,                             0,                             0,                             0                             },
    { 0,                             0,                             0,                             0                             },
    { 0,                             0,                             0,                             0                             },
    { 0,                             0,                             0,                             0                             },
    { 0,                             0,                             0,                             0                             },
    { EDGE_SLOPE_DOWNWARDS,          EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_UPWARDS_ELEVATED,   EDGE_SLOPE_DOWNWARDS_ELEVATED },
    { 0,                             0,                             0,                             0                             },
    { 0,                             0,                             0,                             0                             },
    { 0,                             0,                             0,                             0                             },
    { EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_UPWARDS_ELEVATED,   EDGE_SLOPE_DOWNWARDS_ELEVATED, EDGE_SLOPE_DOWNWARDS          },
    { 0,                             0,                             0,                             0                             },
    { EDGE_SLOPE_UPWARDS_ELEVATED,   EDGE_SLOPE_DOWNWARDS_ELEVATED, EDGE_SLOPE_DOWNWARDS,          EDGE_SLOPE_UPWARDS            },
    { EDGE_SLOPE_DOWNWARDS_ELEVATED, EDGE_SLOPE_DOWNWARDS,          EDGE_SLOPE_UPWARDS,            EDGE_SLOPE_UPWARDS_ELEVATED   },
    { 0,                             0,                             0,                             0                             },
};
// clang-format on

#pragma endregion

uint8_t GetWallSlopeFromEdgeSlope(uint8_t Slope, uint8_t Edge)
{
    return kLandSlopeToWallSlope[Slope][Edge];
}
