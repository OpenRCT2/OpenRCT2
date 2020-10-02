/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Map.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Game.h"
#include "../Input.h"
#include "../OpenRCT2.h"
#include "../actions/BannerRemoveAction.hpp"
#include "../actions/FootpathRemoveAction.hpp"
#include "../actions/LandLowerAction.hpp"
#include "../actions/LandRaiseAction.hpp"
#include "../actions/LandSetHeightAction.hpp"
#include "../actions/LandSetRightsAction.hpp"
#include "../actions/LargeSceneryRemoveAction.hpp"
#include "../actions/ParkEntranceRemoveAction.hpp"
#include "../actions/SmallSceneryRemoveAction.hpp"
#include "../actions/WallRemoveAction.hpp"
#include "../actions/WaterSetHeightAction.hpp"
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../core/Guard.hpp"
#include "../interface/Cursors.h"
#include "../interface/Window.h"
#include "../localisation/Date.h"
#include "../localisation/Localisation.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../object/ObjectManager.h"
#include "../object/TerrainSurfaceObject.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../ride/TrackDesign.h"
#include "../scenario/Scenario.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "Banner.h"
#include "Climate.h"
#include "Footpath.h"
#include "LargeScenery.h"
#include "MapAnimation.h"
#include "Park.h"
#include "Scenery.h"
#include "SmallScenery.h"
#include "Surface.h"
#include "TileInspector.h"
#include "Wall.h"

#include <algorithm>
#include <iterator>
#include <memory>

using namespace OpenRCT2;

/**
 * Replaces 0x00993CCC, 0x00993CCE
 */
// clang-format off
const std::array<CoordsXY, 8> CoordsDirectionDelta = {
    CoordsXY{ -COORDS_XY_STEP, 0 },
    CoordsXY{               0, +COORDS_XY_STEP },
    CoordsXY{ +COORDS_XY_STEP, 0 },
    CoordsXY{               0, -COORDS_XY_STEP },
    CoordsXY{ -COORDS_XY_STEP, +COORDS_XY_STEP },
    CoordsXY{ +COORDS_XY_STEP, +COORDS_XY_STEP },
    CoordsXY{ +COORDS_XY_STEP, -COORDS_XY_STEP },
    CoordsXY{ -COORDS_XY_STEP, -COORDS_XY_STEP }
};
// clang-format on

const TileCoordsXY TileDirectionDelta[] = { { -1, 0 },  { 0, +1 },  { +1, 0 },  { 0, -1 },
                                            { -1, +1 }, { +1, +1 }, { +1, -1 }, { -1, -1 } };

uint16_t gMapSelectFlags;
uint16_t gMapSelectType;
CoordsXY gMapSelectPositionA;
CoordsXY gMapSelectPositionB;
CoordsXYZ gMapSelectArrowPosition;
uint8_t gMapSelectArrowDirection;

uint8_t gMapGroundFlags;

uint16_t gWidePathTileLoopX;
uint16_t gWidePathTileLoopY;
uint16_t gGrassSceneryTileLoopPosition;

int16_t gMapSizeUnits;
int16_t gMapSizeMinus2;
int16_t gMapSize;
int16_t gMapSizeMaxXY;
int16_t gMapBaseZ;

TileElement gTileElements[MAX_TILE_ELEMENTS_WITH_SPARE_ROOM];
TileElement* gTileElementTilePointers[MAX_TILE_TILE_ELEMENT_POINTERS];
std::vector<CoordsXY> gMapSelectionTiles;
std::vector<PeepSpawn> gPeepSpawns;

TileElement* gNextFreeTileElement;
uint32_t gNextFreeTileElementPointerIndex;

bool gLandMountainMode;
bool gLandPaintMode;
bool gClearSmallScenery;
bool gClearLargeScenery;
bool gClearFootpath;

uint16_t gLandRemainingOwnershipSales;
uint16_t gLandRemainingConstructionSales;

bool gMapLandRightsUpdateSuccess;

static void clear_elements_at(const CoordsXY& loc);
static ScreenCoordsXY translate_3d_to_2d(int32_t rotation, const CoordsXY& pos);

void tile_element_iterator_begin(tile_element_iterator* it)
{
    it->x = 0;
    it->y = 0;
    it->element = map_get_first_element_at({ 0, 0 });
}

int32_t tile_element_iterator_next(tile_element_iterator* it)
{
    if (it->element == nullptr)
    {
        it->element = map_get_first_element_at(TileCoordsXY{ it->x, it->y }.ToCoordsXY());
        return 1;
    }

    if (!it->element->IsLastForTile())
    {
        it->element++;
        return 1;
    }

    if (it->x < (MAXIMUM_MAP_SIZE_TECHNICAL - 1))
    {
        it->x++;
        it->element = map_get_first_element_at(TileCoordsXY{ it->x, it->y }.ToCoordsXY());
        return 1;
    }

    if (it->y < (MAXIMUM_MAP_SIZE_TECHNICAL - 1))
    {
        it->x = 0;
        it->y++;
        it->element = map_get_first_element_at(TileCoordsXY{ it->x, it->y }.ToCoordsXY());
        return 1;
    }

    return 0;
}

void tile_element_iterator_restart_for_tile(tile_element_iterator* it)
{
    it->element = nullptr;
}

TileElement* map_get_first_element_at(const CoordsXY& elementPos)
{
    if (!map_is_location_valid(elementPos))
    {
        log_verbose("Trying to access element outside of range");
        return nullptr;
    }
    auto tileElementPos = TileCoordsXY{ elementPos };
    return gTileElementTilePointers[tileElementPos.x + tileElementPos.y * MAXIMUM_MAP_SIZE_TECHNICAL];
}

TileElement* map_get_nth_element_at(const CoordsXY& coords, int32_t n)
{
    TileElement* tileElement = map_get_first_element_at(coords);
    if (tileElement == nullptr)
    {
        return nullptr;
    }
    // Iterate through elements on this tile. This has to be walked, rather than
    // jumped directly to, because n may exceed element count for given tile,
    // and the order of tiles (unlike elements) is not synced over multiplayer.
    while (n >= 0)
    {
        if (n == 0)
        {
            return tileElement;
        }
        if (tileElement->IsLastForTile())
        {
            break;
        }
        tileElement++;
        n--;
    }
    // The element sought for is not within given tile.
    return nullptr;
}

void map_set_tile_element(const TileCoordsXY& tilePos, TileElement* elements)
{
    if (!map_is_location_valid(tilePos.ToCoordsXY()))
    {
        log_error("Trying to access element outside of range");
        return;
    }
    gTileElementTilePointers[tilePos.x + tilePos.y * MAXIMUM_MAP_SIZE_TECHNICAL] = elements;
}

SurfaceElement* map_get_surface_element_at(const CoordsXY& coords)
{
    TileElement* tileElement = map_get_first_element_at(coords);

    if (tileElement == nullptr)
        return nullptr;

    // Find the first surface element
    while (tileElement->GetType() != TILE_ELEMENT_TYPE_SURFACE)
    {
        if (tileElement->IsLastForTile())
            return nullptr;

        tileElement++;
    }

    return tileElement->AsSurface();
}

PathElement* map_get_path_element_at(const TileCoordsXYZ& loc)
{
    TileElement* tileElement = map_get_first_element_at(loc.ToCoordsXY());

    if (tileElement == nullptr)
        return nullptr;

    // Find the path element at known z
    do
    {
        if (tileElement->IsGhost())
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;
        if (tileElement->base_height != loc.z)
            continue;

        return tileElement->AsPath();
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

BannerElement* map_get_banner_element_at(const CoordsXYZ& bannerPos, uint8_t position)
{
    auto bannerTilePos = TileCoordsXYZ{ bannerPos };
    TileElement* tileElement = map_get_first_element_at(bannerPos);

    if (tileElement == nullptr)
        return nullptr;

    // Find the banner element at known z and position
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_BANNER)
            continue;
        if (tileElement->base_height != bannerTilePos.z)
            continue;
        if (tileElement->AsBanner()->GetPosition() != position)
            continue;

        return tileElement->AsBanner();
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

/**
 *
 *  rct2: 0x0068AB4C
 */
void map_init(int32_t size)
{
    gNextFreeTileElementPointerIndex = 0;

    for (int32_t i = 0; i < MAX_TILE_TILE_ELEMENT_POINTERS; i++)
    {
        TileElement* tile_element = &gTileElements[i];
        tile_element->ClearAs(TILE_ELEMENT_TYPE_SURFACE);
        tile_element->SetLastForTile(true);
        tile_element->base_height = 14;
        tile_element->clearance_height = 14;
        tile_element->AsSurface()->SetWaterHeight(0);
        tile_element->AsSurface()->SetSlope(TILE_ELEMENT_SLOPE_FLAT);
        tile_element->AsSurface()->SetGrassLength(GRASS_LENGTH_CLEAR_0);
        tile_element->AsSurface()->SetOwnership(OWNERSHIP_UNOWNED);
        tile_element->AsSurface()->SetParkFences(0);
        tile_element->AsSurface()->SetSurfaceStyle(TERRAIN_GRASS);
        tile_element->AsSurface()->SetEdgeStyle(TERRAIN_EDGE_ROCK);
    }

    gGrassSceneryTileLoopPosition = 0;
    gWidePathTileLoopX = 0;
    gWidePathTileLoopY = 0;
    gMapSizeUnits = size * 32 - 32;
    gMapSizeMinus2 = size * 32 - 2;
    gMapSize = size;
    gMapSizeMaxXY = size * 32 - 33;
    gMapBaseZ = 7;
    map_update_tile_pointers();
    map_remove_out_of_range_elements();
    AutoCreateMapAnimations();

    auto intent = Intent(INTENT_ACTION_MAP);
    context_broadcast_intent(&intent);
}

/**
 * Counts the number of surface tiles that offer land ownership rights for sale,
 * but haven't been bought yet. It updates gLandRemainingOwnershipSales and
 * gLandRemainingConstructionSales.
 */
void map_count_remaining_land_rights()
{
    gLandRemainingOwnershipSales = 0;
    gLandRemainingConstructionSales = 0;

    for (int32_t x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; x++)
    {
        for (int32_t y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; y++)
        {
            auto* surfaceElement = map_get_surface_element_at(TileCoordsXY{ x, y }.ToCoordsXY());
            // Surface elements are sometimes hacked out to save some space for other map elements
            if (surfaceElement == nullptr)
            {
                continue;
            }

            uint8_t flags = surfaceElement->GetOwnership();

            // Do not combine this condition with (flags & OWNERSHIP_AVAILABLE)
            // As some RCT1 parks have owned tiles with the 'construction rights available' flag also set
            if (!(flags & OWNERSHIP_OWNED))
            {
                if (flags & OWNERSHIP_AVAILABLE)
                {
                    gLandRemainingOwnershipSales++;
                }
                else if (
                    (flags & OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE) && (flags & OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED) == 0)
                {
                    gLandRemainingConstructionSales++;
                }
            }
        }
    }
}

/**
 * This is meant to strip TILE_ELEMENT_FLAG_GHOST flag from all elements when
 * importing a park.
 *
 * This can only exist in hacked parks, as we remove ghost elements while saving.
 *
 * This is less invasive than removing ghost elements themselves, as they can
 * contain valid data.
 */
void map_strip_ghost_flag_from_elements()
{
    for (auto& element : gTileElements)
    {
        element.SetGhost(false);
    }
}

/**
 *
 *  rct2: 0x0068AFFD
 */
void map_update_tile_pointers()
{
    int32_t i, x, y;

    for (i = 0; i < MAX_TILE_TILE_ELEMENT_POINTERS; i++)
    {
        gTileElementTilePointers[i] = TILE_UNDEFINED_TILE_ELEMENT;
    }

    TileElement* tileElement = gTileElements;
    TileElement** tile = gTileElementTilePointers;
    for (y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; y++)
    {
        for (x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; x++)
        {
            *tile++ = tileElement;
            while (!(tileElement++)->IsLastForTile())
                ;
        }
    }

    gNextFreeTileElement = tileElement;
}

/**
 * Return the absolute height of an element, given its (x,y) coordinates
 *
 * ax: x
 * cx: y
 * dx: return remember to & with 0xFFFF if you don't want water affecting results
 *  rct2: 0x00662783
 */
int16_t tile_element_height(const CoordsXY& loc)
{
    // Off the map
    if (!map_is_location_valid(loc))
        return MINIMUM_LAND_HEIGHT_BIG;

    // Get the surface element for the tile
    auto surfaceElement = map_get_surface_element_at(loc);

    if (surfaceElement == nullptr)
    {
        return MINIMUM_LAND_HEIGHT_BIG;
    }

    uint16_t height = surfaceElement->GetBaseZ();

    uint32_t slope = surfaceElement->GetSlope();
    uint8_t extra_height = (slope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT) >> 4; // 0x10 is the 5th bit - sets slope to double height
    // Remove the extra height bit
    slope &= TILE_ELEMENT_SLOPE_ALL_CORNERS_UP;

    int8_t quad = 0, quad_extra = 0; // which quadrant the element is in?
                                     // quad_extra is for extra height tiles

    uint8_t xl, yl; // coordinates across this tile

    uint8_t TILE_SIZE = 31;

    xl = loc.x & 0x1f;
    yl = loc.y & 0x1f;

    // Slope logic:
    // Each of the four bits in slope represents that corner being raised
    // slope == 15 (all four bits) is not used and slope == 0 is flat
    // If the extra_height bit is set, then the slope goes up two z-levels

    // We arbitrarily take the SW corner to be closest to the viewer

    // One corner up
    if (slope == TILE_ELEMENT_SLOPE_N_CORNER_UP || slope == TILE_ELEMENT_SLOPE_E_CORNER_UP
        || slope == TILE_ELEMENT_SLOPE_S_CORNER_UP || slope == TILE_ELEMENT_SLOPE_W_CORNER_UP)
    {
        switch (slope)
        {
            case TILE_ELEMENT_SLOPE_N_CORNER_UP:
                quad = xl + yl - TILE_SIZE;
                break;
            case TILE_ELEMENT_SLOPE_E_CORNER_UP:
                quad = xl - yl;
                break;
            case TILE_ELEMENT_SLOPE_S_CORNER_UP:
                quad = TILE_SIZE - yl - xl;
                break;
            case TILE_ELEMENT_SLOPE_W_CORNER_UP:
                quad = yl - xl;
                break;
        }
        // If the element is in the quadrant with the slope, raise its height
        if (quad > 0)
        {
            height += quad / 2;
        }
    }

    // One side up
    switch (slope)
    {
        case TILE_ELEMENT_SLOPE_NE_SIDE_UP:
            height += xl / 2 + 1;
            break;
        case TILE_ELEMENT_SLOPE_SE_SIDE_UP:
            height += (TILE_SIZE - yl) / 2;
            break;
        case TILE_ELEMENT_SLOPE_NW_SIDE_UP:
            height += yl / 2;
            height++;
            break;
        case TILE_ELEMENT_SLOPE_SW_SIDE_UP:
            height += (TILE_SIZE - xl) / 2;
            break;
    }

    // One corner down
    if ((slope == TILE_ELEMENT_SLOPE_W_CORNER_DN) || (slope == TILE_ELEMENT_SLOPE_S_CORNER_DN)
        || (slope == TILE_ELEMENT_SLOPE_E_CORNER_DN) || (slope == TILE_ELEMENT_SLOPE_N_CORNER_DN))
    {
        switch (slope)
        {
            case TILE_ELEMENT_SLOPE_W_CORNER_DN:
                quad_extra = xl + TILE_SIZE - yl;
                quad = xl - yl;
                break;
            case TILE_ELEMENT_SLOPE_S_CORNER_DN:
                quad_extra = xl + yl;
                quad = xl + yl - TILE_SIZE - 1;
                break;
            case TILE_ELEMENT_SLOPE_E_CORNER_DN:
                quad_extra = TILE_SIZE - xl + yl;
                quad = yl - xl;
                break;
            case TILE_ELEMENT_SLOPE_N_CORNER_DN:
                quad_extra = (TILE_SIZE - xl) + (TILE_SIZE - yl);
                quad = TILE_SIZE - yl - xl - 1;
                break;
        }

        if (extra_height)
        {
            height += quad_extra / 2;
            height++;
            return height;
        }
        // This tile is essentially at the next height level
        height += LAND_HEIGHT_STEP;
        // so we move *down* the slope
        if (quad < 0)
        {
            height += quad / 2;
        }
    }

    // Valleys
    if ((slope == TILE_ELEMENT_SLOPE_W_E_VALLEY) || (slope == TILE_ELEMENT_SLOPE_N_S_VALLEY))
    {
        switch (slope)
        {
            case TILE_ELEMENT_SLOPE_W_E_VALLEY:
                if (xl + yl <= TILE_SIZE + 1)
                {
                    return height;
                }
                quad = TILE_SIZE - xl - yl;
                break;
            case TILE_ELEMENT_SLOPE_N_S_VALLEY:
                quad = xl - yl;
                break;
        }
        if (quad > 0)
        {
            height += quad / 2;
        }
    }

    return height;
}

int16_t tile_element_water_height(const CoordsXY& loc)
{
    // Off the map
    if (!map_is_location_valid(loc))
        return 0;

    // Get the surface element for the tile
    auto surfaceElement = map_get_surface_element_at(loc);

    if (surfaceElement == nullptr)
    {
        return 0;
    }

    return surfaceElement->GetWaterHeight();
}

/**
 * Checks if the tile at coordinate at height counts as connected.
 * @return 1 if connected, 0 otherwise
 */
bool map_coord_is_connected(const TileCoordsXYZ& loc, uint8_t faceDirection)
{
    TileElement* tileElement = map_get_first_element_at(loc.ToCoordsXY());

    if (tileElement == nullptr)
        return false;

    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;

        uint8_t slopeDirection = tileElement->AsPath()->GetSlopeDirection();

        if (tileElement->AsPath()->IsSloped())
        {
            if (slopeDirection == faceDirection)
            {
                if (loc.z == tileElement->base_height + 2)
                    return true;
            }
            else if (direction_reverse(slopeDirection) == faceDirection && loc.z == tileElement->base_height)
            {
                return true;
            }
        }
        else
        {
            if (loc.z == tileElement->base_height)
                return true;
        }
    } while (!(tileElement++)->IsLastForTile());

    return false;
}

/**
 *
 *  rct2: 0x006A876D
 */
void map_update_path_wide_flags()
{
    if (gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
    {
        return;
    }

    // Presumably update_path_wide_flags is too computationally expensive to call for every
    // tile every update, so gWidePathTileLoopX and gWidePathTileLoopY store the x and y
    // progress. A maximum of 128 calls is done per update.
    uint16_t x = gWidePathTileLoopX;
    uint16_t y = gWidePathTileLoopY;
    for (int32_t i = 0; i < 128; i++)
    {
        footpath_update_path_wide_flags({ x, y });

        // Next x, y tile
        x += COORDS_XY_STEP;
        if (x >= MAXIMUM_MAP_SIZE_BIG)
        {
            x = 0;
            y += COORDS_XY_STEP;
            if (y >= MAXIMUM_MAP_SIZE_BIG)
            {
                y = 0;
            }
        }
    }
    gWidePathTileLoopX = x;
    gWidePathTileLoopY = y;
}

/**
 *
 *  rct2: 0x006A7B84
 */
int32_t map_height_from_slope(const CoordsXY& coords, int32_t slopeDirection, bool isSloped)
{
    if (!isSloped)
        return 0;

    switch (slopeDirection % NumOrthogonalDirections)
    {
        case TILE_ELEMENT_DIRECTION_WEST:
            return (31 - (coords.x & 31)) / 2;
        case TILE_ELEMENT_DIRECTION_NORTH:
            return (coords.y & 31) / 2;
        case TILE_ELEMENT_DIRECTION_EAST:
            return (coords.x & 31) / 2;
        case TILE_ELEMENT_DIRECTION_SOUTH:
            return (31 - (coords.y & 31)) / 2;
    }
    return 0;
}

bool map_is_location_valid(const CoordsXY& coords)
{
    const bool is_x_valid = coords.x < MAXIMUM_MAP_SIZE_BIG && coords.x >= 0;
    const bool is_y_valid = coords.y < MAXIMUM_MAP_SIZE_BIG && coords.y >= 0;
    return is_x_valid && is_y_valid;
}

bool map_is_edge(const CoordsXY& coords)
{
    return (coords.x < 32 || coords.y < 32 || coords.x >= gMapSizeUnits || coords.y >= gMapSizeUnits);
}

bool map_can_build_at(const CoordsXYZ& loc)
{
    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        return true;
    if (gCheatsSandboxMode)
        return true;
    if (map_is_location_owned(loc))
        return true;
    return false;
}

/**
 *
 *  rct2: 0x00664F72
 */
bool map_is_location_owned(const CoordsXYZ& loc)
{
    // This check is to avoid throwing lots of messages in logs.
    if (map_is_location_valid(loc))
    {
        auto* surfaceElement = map_get_surface_element_at(loc);
        if (surfaceElement != nullptr)
        {
            if (surfaceElement->GetOwnership() & OWNERSHIP_OWNED)
                return true;

            if (surfaceElement->GetOwnership() & OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED)
            {
                if (loc.z < surfaceElement->GetBaseZ() || loc.z - LAND_HEIGHT_STEP > surfaceElement->GetBaseZ())
                    return true;
            }
        }
    }

    gGameCommandErrorText = STR_LAND_NOT_OWNED_BY_PARK;
    return false;
}

/**
 *
 *  rct2: 0x00664F2C
 */
bool map_is_location_in_park(const CoordsXY& coords)
{
    if (map_is_location_valid(coords))
    {
        auto surfaceElement = map_get_surface_element_at(coords);
        if (surfaceElement == nullptr)
            return false;
        if (surfaceElement->GetOwnership() & OWNERSHIP_OWNED)
            return true;
    }

    gGameCommandErrorText = STR_LAND_NOT_OWNED_BY_PARK;
    return false;
}

bool map_is_location_owned_or_has_rights(const CoordsXY& loc)
{
    if (map_is_location_valid(loc))
    {
        auto surfaceElement = map_get_surface_element_at(loc);
        if (surfaceElement == nullptr)
        {
            return false;
        }
        if (surfaceElement->GetOwnership() & OWNERSHIP_OWNED)
            return true;
        if (surfaceElement->GetOwnership() & OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED)
            return true;
    }
    return false;
}

// 0x00981A1E
// Table of pre-calculated surface slopes (32) when raising the land tile for a given selection (5)
// 0x1F = new slope
// 0x20 = base height increases
const uint8_t tile_element_raise_styles[9][32] = {
    { 0x01, 0x1B, 0x03, 0x1B, 0x05, 0x21, 0x07, 0x21, 0x09, 0x1B, 0x0B, 0x1B, 0x0D, 0x21, 0x20, 0x0F,
      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x23, 0x18, 0x19, 0x1A, 0x3B, 0x1C, 0x29, 0x24, 0x1F }, // MAP_SELECT_TYPE_CORNER_0
                                                                                                        // (absolute rotation)
    { 0x02, 0x03, 0x17, 0x17, 0x06, 0x07, 0x17, 0x17, 0x0A, 0x0B, 0x22, 0x22, 0x0E, 0x20, 0x22, 0x0F,
      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x37, 0x18, 0x19, 0x1A, 0x23, 0x1C, 0x28, 0x26, 0x1F }, // MAP_SELECT_TYPE_CORNER_1
    { 0x04, 0x05, 0x06, 0x07, 0x1E, 0x24, 0x1E, 0x24, 0x0C, 0x0D, 0x0E, 0x20, 0x1E, 0x24, 0x1E, 0x0F,
      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x26, 0x18, 0x19, 0x1A, 0x21, 0x1C, 0x2C, 0x3E, 0x1F }, // MAP_SELECT_TYPE_CORNER_2
    { 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x20, 0x1D, 0x1D, 0x28, 0x28, 0x1D, 0x1D, 0x28, 0x0F,
      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x22, 0x18, 0x19, 0x1A, 0x29, 0x1C, 0x3D, 0x2C, 0x1F }, // MAP_SELECT_TYPE_CORNER_3
    { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x20, 0x20, 0x20, 0x21, 0x20, 0x28, 0x24, 0x20 }, // MAP_SELECT_TYPE_FULL
    { 0x0C, 0x0D, 0x0E, 0x20, 0x0C, 0x0D, 0x0E, 0x20, 0x0C, 0x0D, 0x0E, 0x20, 0x2C, 0x2C, 0x2C, 0x2C,
      0x0C, 0x0D, 0x0E, 0x20, 0x0C, 0x0C, 0x0E, 0x22, 0x0C, 0x0D, 0x0E, 0x21, 0x2C, 0x2C, 0x2C, 0x2C }, // MAP_SELECT_TYPE_EDGE_0
    { 0x09, 0x09, 0x0B, 0x0B, 0x0D, 0x0D, 0x20, 0x20, 0x09, 0x29, 0x0B, 0x29, 0x0D, 0x29, 0x20, 0x29,
      0x09, 0x09, 0x0B, 0x0B, 0x0D, 0x0D, 0x24, 0x22, 0x09, 0x29, 0x0B, 0x29, 0x0D, 0x29, 0x24, 0x29 }, // MAP_SELECT_TYPE_EDGE_1
    { 0x03, 0x03, 0x03, 0x23, 0x07, 0x07, 0x07, 0x23, 0x0B, 0x0B, 0x0B, 0x23, 0x20, 0x20, 0x20, 0x23,
      0x03, 0x03, 0x03, 0x23, 0x07, 0x07, 0x07, 0x23, 0x0B, 0x0B, 0x0B, 0x23, 0x20, 0x28, 0x24, 0x23 }, // MAP_SELECT_TYPE_EDGE_2
    { 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x26, 0x26, 0x0E, 0x20, 0x0E, 0x20, 0x0E, 0x20, 0x26, 0x26,
      0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x26, 0x26, 0x0E, 0x20, 0x0E, 0x21, 0x0E, 0x28, 0x26, 0x26 }, // MAP_SELECT_TYPE_EDGE_3
};

// 0x00981ABE
// Basically the inverse of the table above.
// 0x1F = new slope
// 0x20 = base height increases
const uint8_t tile_element_lower_styles[9][32] = {
    { 0x2E, 0x00, 0x2E, 0x02, 0x3E, 0x04, 0x3E, 0x06, 0x2E, 0x08, 0x2E, 0x0A, 0x3E, 0x0C, 0x3E, 0x0F,
      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x06, 0x18, 0x19, 0x1A, 0x0B, 0x1C, 0x0C, 0x3E, 0x1F }, // MAP_SELECT_TYPE_CORNER_0
    { 0x2D, 0x2D, 0x00, 0x01, 0x2D, 0x2D, 0x04, 0x05, 0x3D, 0x3D, 0x08, 0x09, 0x3D, 0x3D, 0x0C, 0x0F,
      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x07, 0x18, 0x19, 0x1A, 0x09, 0x1C, 0x3D, 0x0C, 0x1F }, // MAP_SELECT_TYPE_CORNER_1
    { 0x2B, 0x3B, 0x2B, 0x3B, 0x00, 0x01, 0x02, 0x03, 0x2B, 0x3B, 0x2B, 0x3B, 0x08, 0x09, 0x0A, 0x0F,
      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x03, 0x18, 0x19, 0x1A, 0x3B, 0x1C, 0x09, 0x0E, 0x1F }, // MAP_SELECT_TYPE_CORNER_2
    { 0x27, 0x27, 0x37, 0x37, 0x27, 0x27, 0x37, 0x37, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x0F,
      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x37, 0x18, 0x19, 0x1A, 0x03, 0x1C, 0x0D, 0x06, 0x1F }, // MAP_SELECT_TYPE_CORNER_3
    { 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x0D, 0x0E, 0x00 }, // MAP_SELECT_TYPE_FULL
    { 0x23, 0x23, 0x23, 0x23, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03,
      0x23, 0x23, 0x23, 0x23, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x00, 0x0D, 0x0E, 0x03 }, // MAP_SELECT_TYPE_EDGE_0
    { 0x26, 0x00, 0x26, 0x02, 0x26, 0x04, 0x26, 0x06, 0x00, 0x00, 0x02, 0x02, 0x04, 0x04, 0x06, 0x06,
      0x26, 0x00, 0x26, 0x02, 0x26, 0x04, 0x26, 0x06, 0x00, 0x00, 0x02, 0x0B, 0x04, 0x0D, 0x06, 0x06 }, // MAP_SELECT_TYPE_EDGE_1
    { 0x2C, 0x00, 0x00, 0x00, 0x2C, 0x04, 0x04, 0x04, 0x2C, 0x08, 0x08, 0x08, 0x2C, 0x0C, 0x0C, 0x0C,
      0x2C, 0x00, 0x00, 0x00, 0x2C, 0x04, 0x04, 0x07, 0x2C, 0x08, 0x08, 0x0B, 0x2C, 0x0C, 0x0C, 0x0C }, // MAP_SELECT_TYPE_EDGE_2
    { 0x29, 0x29, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x29, 0x29, 0x08, 0x09, 0x08, 0x09, 0x08, 0x09,
      0x29, 0x29, 0x00, 0x01, 0x00, 0x01, 0x00, 0x07, 0x29, 0x29, 0x08, 0x09, 0x08, 0x09, 0x0E, 0x09 }, // MAP_SELECT_TYPE_EDGE_3
};

int32_t map_get_corner_height(int32_t z, int32_t slope, int32_t direction)
{
    switch (direction)
    {
        case 0:
            if (slope & TILE_ELEMENT_SLOPE_N_CORNER_UP)
            {
                z += 2;
                if (slope == (TILE_ELEMENT_SLOPE_S_CORNER_DN | TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT))
                {
                    z += 2;
                }
            }
            break;
        case 1:
            if (slope & TILE_ELEMENT_SLOPE_E_CORNER_UP)
            {
                z += 2;
                if (slope == (TILE_ELEMENT_SLOPE_W_CORNER_DN | TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT))
                {
                    z += 2;
                }
            }
            break;
        case 2:
            if (slope & TILE_ELEMENT_SLOPE_S_CORNER_UP)
            {
                z += 2;
                if (slope == (TILE_ELEMENT_SLOPE_N_CORNER_DN | TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT))
                {
                    z += 2;
                }
            }
            break;
        case 3:
            if (slope & TILE_ELEMENT_SLOPE_W_CORNER_UP)
            {
                z += 2;
                if (slope == (TILE_ELEMENT_SLOPE_E_CORNER_DN | TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT))
                {
                    z += 2;
                }
            }
            break;
    }
    return z;
}

int32_t tile_element_get_corner_height(const SurfaceElement* surfaceElement, int32_t direction)
{
    int32_t z = surfaceElement->base_height;
    int32_t slope = surfaceElement->GetSlope();
    return map_get_corner_height(z, slope, direction);
}

uint8_t map_get_lowest_land_height(const MapRange& range)
{
    MapRange validRange = { std::max(range.GetLeft(), 32), std::max(range.GetTop(), 32),
                            std::min(range.GetRight(), static_cast<int32_t>(gMapSizeMaxXY)),
                            std::min(range.GetBottom(), static_cast<int32_t>(gMapSizeMaxXY)) };

    uint8_t min_height = 0xFF;
    for (int32_t yi = validRange.GetTop(); yi <= validRange.GetBottom(); yi += COORDS_XY_STEP)
    {
        for (int32_t xi = validRange.GetLeft(); xi <= validRange.GetRight(); xi += COORDS_XY_STEP)
        {
            auto* surfaceElement = map_get_surface_element_at(CoordsXY{ xi, yi });

            if (surfaceElement != nullptr && min_height > surfaceElement->base_height)
            {
                if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
                {
                    if (!map_is_location_in_park(CoordsXY{ xi, yi }))
                    {
                        continue;
                    }
                }

                min_height = surfaceElement->base_height;
            }
        }
    }
    return min_height;
}

uint8_t map_get_highest_land_height(const MapRange& range)
{
    MapRange validRange = { std::max(range.GetLeft(), 32), std::max(range.GetTop(), 32),
                            std::min(range.GetRight(), static_cast<int32_t>(gMapSizeMaxXY)),
                            std::min(range.GetBottom(), static_cast<int32_t>(gMapSizeMaxXY)) };

    uint8_t max_height = 0;
    for (int32_t yi = validRange.GetTop(); yi <= validRange.GetBottom(); yi += COORDS_XY_STEP)
    {
        for (int32_t xi = validRange.GetLeft(); xi <= validRange.GetRight(); xi += COORDS_XY_STEP)
        {
            auto* surfaceElement = map_get_surface_element_at(CoordsXY{ xi, yi });
            if (surfaceElement != nullptr)
            {
                if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
                {
                    if (!map_is_location_in_park(CoordsXY{ xi, yi }))
                    {
                        continue;
                    }
                }

                uint8_t base_height = surfaceElement->base_height;
                if (surfaceElement->GetSlope() & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
                    base_height += 2;
                if (surfaceElement->GetSlope() & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
                    base_height += 2;
                if (max_height < base_height)
                    max_height = base_height;
            }
        }
    }
    return max_height;
}

bool map_is_location_at_edge(const CoordsXY& loc)
{
    return loc.x < 32 || loc.y < 32 || loc.x >= (MAXIMUM_TILE_START_XY) || loc.y >= (MAXIMUM_TILE_START_XY);
}

/**
 *
 *  rct2: 0x0068B280
 */
void tile_element_remove(TileElement* tileElement)
{
    // Replace Nth element by (N+1)th element.
    // This loop will make tileElement point to the old last element position,
    // after copy it to it's new position
    if (!tileElement->IsLastForTile())
    {
        do
        {
            *tileElement = *(tileElement + 1);
        } while (!(++tileElement)->IsLastForTile());
    }

    // Mark the latest element with the last element flag.
    (tileElement - 1)->SetLastForTile(true);
    tileElement->base_height = MAX_ELEMENT_HEIGHT;

    if ((tileElement + 1) == gNextFreeTileElement)
    {
        gNextFreeTileElement--;
    }
}

/**
 *
 *  rct2: 0x00675A8E
 */
void map_remove_all_rides()
{
    tile_element_iterator it;

    tile_element_iterator_begin(&it);
    do
    {
        switch (it.element->GetType())
        {
            case TILE_ELEMENT_TYPE_PATH:
                if (it.element->AsPath()->IsQueue())
                {
                    it.element->AsPath()->SetHasQueueBanner(false);
                    it.element->AsPath()->SetRideIndex(RIDE_ID_NULL);
                }
                break;
            case TILE_ELEMENT_TYPE_ENTRANCE:
                if (it.element->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE)
                    break;
                [[fallthrough]];
            case TILE_ELEMENT_TYPE_TRACK:
                footpath_queue_chain_reset();
                footpath_remove_edges_at(TileCoordsXY{ it.x, it.y }.ToCoordsXY(), it.element);
                tile_element_remove(it.element);
                tile_element_iterator_restart_for_tile(&it);
                break;
        }
    } while (tile_element_iterator_next(&it));
}

/**
 *
 *  rct2: 0x0068AB1B
 */
void map_invalidate_map_selection_tiles()
{
    if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT))
        return;

    for (const auto& position : gMapSelectionTiles)
        map_invalidate_tile_full(position);
}

static void map_get_bounding_box(const MapRange& _range, int32_t* left, int32_t* top, int32_t* right, int32_t* bottom)
{
    uint32_t rotation = get_current_rotation();
    std::array<const CoordsXY, 4> corners{ CoordsXY{ _range.GetLeft(), _range.GetTop() },
                                           CoordsXY{ _range.GetRight(), _range.GetTop() },
                                           CoordsXY{ _range.GetRight(), _range.GetBottom() },
                                           CoordsXY{ _range.GetLeft(), _range.GetBottom() } };

    *left = std::numeric_limits<int32_t>::max();
    *top = std::numeric_limits<int32_t>::max();
    *right = std::numeric_limits<int32_t>::min();
    *bottom = std::numeric_limits<int32_t>::min();

    for (const auto& corner : corners)
    {
        auto screenCoord = translate_3d_to_2d(rotation, corner);
        if (screenCoord.x < *left)
            *left = screenCoord.x;
        if (screenCoord.x > *right)
            *right = screenCoord.x;
        if (screenCoord.y > *bottom)
            *bottom = screenCoord.y;
        if (screenCoord.y < *top)
            *top = screenCoord.y;
    }
}

/**
 *
 *  rct2: 0x0068AAE1
 */
void map_invalidate_selection_rect()
{
    int32_t x0, y0, x1, y1, left, right, top, bottom;

    if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
        return;

    x0 = gMapSelectPositionA.x + 16;
    y0 = gMapSelectPositionA.y + 16;
    x1 = gMapSelectPositionB.x + 16;
    y1 = gMapSelectPositionB.y + 16;
    map_get_bounding_box({ x0, y0, x1, y1 }, &left, &top, &right, &bottom);
    left -= 32;
    right += 32;
    bottom += 32;
    top -= 32 + 2080;

    for (int32_t i = 0; i < MAX_VIEWPORT_COUNT; i++)
    {
        rct_viewport* viewport = &g_viewport_list[i];
        if (viewport->width != 0)
        {
            viewport_invalidate(viewport, left, top, right, bottom);
        }
    }
}

/**
 *
 *  rct2: 0x0068B111
 */
void map_reorganise_elements()
{
    context_setcurrentcursor(CURSOR_ZZZ);

    auto newTileElements = std::make_unique<TileElement[]>(MAX_TILE_ELEMENTS_WITH_SPARE_ROOM);
    TileElement* newElementsPtr = newTileElements.get();

    if (newTileElements == nullptr)
    {
        log_fatal("Unable to allocate memory for map elements.");
        return;
    }

    for (int32_t y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; y++)
    {
        for (int32_t x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; x++)
        {
            TileElement* startElement = map_get_first_element_at(TileCoordsXY{ x, y }.ToCoordsXY());
            if (startElement == nullptr)
                continue;
            TileElement* endElement = startElement;
            while (!(endElement++)->IsLastForTile())
                ;

            const auto numElements = static_cast<uint32_t>(endElement - startElement);
            std::memcpy(newElementsPtr, startElement, numElements * sizeof(TileElement));
            newElementsPtr += numElements;
        }
    }

    const auto numElements = static_cast<uint32_t>(newElementsPtr - newTileElements.get());
    std::memcpy(gTileElements, newTileElements.get(), numElements * sizeof(TileElement));
    std::memset(gTileElements + numElements, 0, (MAX_TILE_ELEMENTS_WITH_SPARE_ROOM - numElements) * sizeof(TileElement));

    map_update_tile_pointers();
}

/**
 *
 *  rct2: 0x0068B044
 *  Returns true on space available for more elements
 *  Reorganises the map elements to check for space
 */
bool map_check_free_elements_and_reorganise(int32_t numElements)
{
    if (numElements != 0)
    {
        auto tileElementEnd = &gTileElements[MAX_TILE_ELEMENTS];

        // Check if is there is room for the required number of elements
        auto newTileElementEnd = gNextFreeTileElement + numElements;
        if (newTileElementEnd > tileElementEnd)
        {
            // Defragment the map element list
            map_reorganise_elements();

            // Check if there is any room again
            newTileElementEnd = gNextFreeTileElement + numElements;
            if (newTileElementEnd > tileElementEnd)
            {
                // Not enough spare elements left :'(
                gGameCommandErrorText = STR_ERR_LANDSCAPE_DATA_AREA_FULL;
                return false;
            }
        }
    }
    return true;
}

/**
 *
 *  rct2: 0x0068B1F6
 */
TileElement* tile_element_insert(const CoordsXYZ& loc, int32_t occupiedQuadrants)
{
    const auto& tileLoc = TileCoordsXYZ(loc);
    TileElement *originalTileElement, *newTileElement, *insertedElement;
    bool isLastForTile = false;

    if (!map_check_free_elements_and_reorganise(1))
    {
        log_error("Cannot insert new element");
        return nullptr;
    }

    newTileElement = gNextFreeTileElement;
    originalTileElement = gTileElementTilePointers[tileLoc.y * MAXIMUM_MAP_SIZE_TECHNICAL + tileLoc.x];

    // Set tile index pointer to point to new element block
    gTileElementTilePointers[tileLoc.y * MAXIMUM_MAP_SIZE_TECHNICAL + tileLoc.x] = newTileElement;

    if (originalTileElement == nullptr)
    {
        isLastForTile = true;
    }
    else
    {
        // Copy all elements that are below the insert height
        while (loc.z >= originalTileElement->GetBaseZ())
        {
            // Copy over map element
            *newTileElement = *originalTileElement;
            originalTileElement->base_height = MAX_ELEMENT_HEIGHT;
            originalTileElement++;
            newTileElement++;

            if ((newTileElement - 1)->IsLastForTile())
            {
                // No more elements above the insert element
                (newTileElement - 1)->SetLastForTile(false);
                isLastForTile = true;
                break;
            }
        }
    }

    // Insert new map element
    insertedElement = newTileElement;
    newTileElement->type = 0;
    newTileElement->SetBaseZ(loc.z);
    newTileElement->Flags = 0;
    newTileElement->SetLastForTile(isLastForTile);
    newTileElement->SetOccupiedQuadrants(occupiedQuadrants);
    newTileElement->SetClearanceZ(loc.z);
    std::memset(&newTileElement->pad_04, 0, sizeof(newTileElement->pad_04));
    std::memset(&newTileElement->pad_08, 0, sizeof(newTileElement->pad_08));
    newTileElement++;

    // Insert rest of map elements above insert height
    if (!isLastForTile)
    {
        do
        {
            // Copy over map element
            *newTileElement = *originalTileElement;
            originalTileElement->base_height = MAX_ELEMENT_HEIGHT;
            originalTileElement++;
            newTileElement++;
        } while (!((newTileElement - 1)->IsLastForTile()));
    }

    gNextFreeTileElement = newTileElement;
    return insertedElement;
}

/**
 *
 *  rct2: 0x0068BB18
 */
void map_obstruction_set_error_text(TileElement* tileElement, GameActionResult& res)
{
    Ride* ride;
    rct_scenery_entry* sceneryEntry;

    res.ErrorMessage = STR_OBJECT_IN_THE_WAY;
    switch (tileElement->GetType())
    {
        case TILE_ELEMENT_TYPE_SURFACE:
            res.ErrorMessage = STR_RAISE_OR_LOWER_LAND_FIRST;
            break;
        case TILE_ELEMENT_TYPE_PATH:
            res.ErrorMessage = STR_FOOTPATH_IN_THE_WAY;
            break;
        case TILE_ELEMENT_TYPE_TRACK:
            ride = get_ride(tileElement->AsTrack()->GetRideIndex());
            if (ride != nullptr)
            {
                res.ErrorMessage = STR_X_IN_THE_WAY;

                Formatter ft(res.ErrorMessageArgs.data());
                ride->FormatNameTo(ft);
            }
            break;
        case TILE_ELEMENT_TYPE_SMALL_SCENERY:
        {
            sceneryEntry = tileElement->AsSmallScenery()->GetEntry();
            res.ErrorMessage = STR_X_IN_THE_WAY;
            auto ft = Formatter(res.ErrorMessageArgs.data());
            ft.Add<rct_string_id>(sceneryEntry->name);
            break;
        }
        case TILE_ELEMENT_TYPE_ENTRANCE:
            switch (tileElement->AsEntrance()->GetEntranceType())
            {
                case ENTRANCE_TYPE_RIDE_ENTRANCE:
                    res.ErrorMessage = STR_RIDE_ENTRANCE_IN_THE_WAY;
                    break;
                case ENTRANCE_TYPE_RIDE_EXIT:
                    res.ErrorMessage = STR_RIDE_EXIT_IN_THE_WAY;
                    break;
                case ENTRANCE_TYPE_PARK_ENTRANCE:
                    res.ErrorMessage = STR_PARK_ENTRANCE_IN_THE_WAY;
                    break;
            }
            break;
        case TILE_ELEMENT_TYPE_WALL:
        {
            sceneryEntry = tileElement->AsWall()->GetEntry();
            res.ErrorMessage = STR_X_IN_THE_WAY;
            auto ft = Formatter(res.ErrorMessageArgs.data());
            ft.Add<rct_string_id>(sceneryEntry->name);
            break;
        }
        case TILE_ELEMENT_TYPE_LARGE_SCENERY:
        {
            sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
            res.ErrorMessage = STR_X_IN_THE_WAY;
            auto ft = Formatter(res.ErrorMessageArgs.data());
            ft.Add<rct_string_id>(sceneryEntry->name);
            break;
        }
    }
}

/**
 *
 *  rct2: 0x0068B932
 *  ax = x
 *  cx = y
 *  dl = zLow
 *  dh = zHigh
 *  ebp = clearFunc
 *  bl = bl
 */
std::unique_ptr<ConstructClearResult> MapCanConstructWithClearAt(
    const CoordsXYRangedZ& pos, CLEAR_FUNC clearFunc, QuarterTile quarterTile, uint8_t flags, uint8_t crossingMode)
{
    int32_t northZ, eastZ, baseHeight, southZ, westZ, water_height;
    northZ = eastZ = baseHeight = southZ = westZ = water_height = 0;
    auto res = std::make_unique<ConstructClearResult>();
    uint8_t slope = 0;

    res->GroundFlags = ELEMENT_IS_ABOVE_GROUND;
    bool canBuildCrossing = false;
    if (pos.x >= gMapSizeUnits || pos.y >= gMapSizeUnits || pos.x < 32 || pos.y < 32)
    {
        res->Error = GA_ERROR::INVALID_PARAMETERS;
        res->ErrorMessage = STR_OFF_EDGE_OF_MAP;
        return res;
    }

    if (gCheatsDisableClearanceChecks)
    {
        return res;
    }

    TileElement* tileElement = map_get_first_element_at(pos);
    if (tileElement == nullptr)
    {
        res->Error = GA_ERROR::UNKNOWN;
        res->ErrorMessage = STR_NONE;
        return res;
    }
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_SURFACE)
        {
            if (pos.baseZ < tileElement->GetClearanceZ() && pos.clearanceZ > tileElement->GetBaseZ()
                && !(tileElement->IsGhost()))
            {
                if (tileElement->GetOccupiedQuadrants() & (quarterTile.GetBaseQuarterOccupied()))
                {
                    goto loc_68BABC;
                }
            }
            continue;
        }
        water_height = tileElement->AsSurface()->GetWaterHeight();
        if (water_height && water_height > pos.baseZ && tileElement->GetBaseZ() < pos.clearanceZ)
        {
            res->GroundFlags |= ELEMENT_IS_UNDERWATER;
            if (water_height < pos.clearanceZ)
            {
                goto loc_68BAE6;
            }
        }
    loc_68B9B7:
        if (gParkFlags & PARK_FLAGS_FORBID_HIGH_CONSTRUCTION)
        {
            auto heightFromGround = pos.clearanceZ - tileElement->GetBaseZ();

            if (heightFromGround > (18 * COORDS_Z_STEP))
            {
                res->Error = GA_ERROR::DISALLOWED;
                res->ErrorMessage = STR_LOCAL_AUTHORITY_WONT_ALLOW_CONSTRUCTION_ABOVE_TREE_HEIGHT;
                return res;
            }
        }

        // Only allow building crossings directly on a flat surface tile.
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_SURFACE
            && (tileElement->AsSurface()->GetSlope()) == TILE_ELEMENT_SLOPE_FLAT && tileElement->GetBaseZ() == pos.baseZ)
        {
            canBuildCrossing = true;
        }

        if (quarterTile.GetZQuarterOccupied() != 0b1111)
        {
            if (tileElement->GetBaseZ() >= pos.clearanceZ)
            {
                // loc_68BA81
                res->GroundFlags |= ELEMENT_IS_UNDERGROUND;
                res->GroundFlags &= ~ELEMENT_IS_ABOVE_GROUND;
            }
            else
            {
                northZ = tileElement->GetBaseZ();
                eastZ = northZ;
                southZ = northZ;
                westZ = northZ;
                slope = tileElement->AsSurface()->GetSlope();
                if (slope & TILE_ELEMENT_SLOPE_N_CORNER_UP)
                {
                    northZ += LAND_HEIGHT_STEP;
                    if (slope == (TILE_ELEMENT_SLOPE_S_CORNER_DN | TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT))
                        northZ += LAND_HEIGHT_STEP;
                }
                if (slope & TILE_ELEMENT_SLOPE_E_CORNER_UP)
                {
                    eastZ += LAND_HEIGHT_STEP;
                    if (slope == (TILE_ELEMENT_SLOPE_W_CORNER_DN | TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT))
                        eastZ += LAND_HEIGHT_STEP;
                }
                if (slope & TILE_ELEMENT_SLOPE_S_CORNER_UP)
                {
                    southZ += LAND_HEIGHT_STEP;
                    if (slope == (TILE_ELEMENT_SLOPE_N_CORNER_DN | TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT))
                        southZ += LAND_HEIGHT_STEP;
                }
                if (slope & TILE_ELEMENT_SLOPE_W_CORNER_UP)
                {
                    westZ += LAND_HEIGHT_STEP;
                    if (slope == (TILE_ELEMENT_SLOPE_E_CORNER_DN | TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT))
                        westZ += LAND_HEIGHT_STEP;
                }
                baseHeight = pos.baseZ + (4 * COORDS_Z_STEP);
                {
                    auto baseQuarter = quarterTile.GetBaseQuarterOccupied();
                    auto zQuarter = quarterTile.GetZQuarterOccupied();
                    if ((!(baseQuarter & 0b0001) || ((zQuarter & 0b0001 || pos.baseZ >= northZ) && baseHeight >= northZ))
                        && (!(baseQuarter & 0b0010) || ((zQuarter & 0b0010 || pos.baseZ >= eastZ) && baseHeight >= eastZ))
                        && (!(baseQuarter & 0b0100) || ((zQuarter & 0b0100 || pos.baseZ >= southZ) && baseHeight >= southZ))
                        && (!(baseQuarter & 0b1000) || ((zQuarter & 0b1000 || pos.baseZ >= westZ) && baseHeight >= westZ)))
                    {
                        continue;
                    }
                }
            loc_68BABC:
                if (clearFunc != nullptr)
                {
                    if (!clearFunc(&tileElement, pos, flags, &res->Cost))
                    {
                        continue;
                    }
                }

                // Crossing mode 1: building track over path
                if (crossingMode == 1 && canBuildCrossing && tileElement->GetType() == TILE_ELEMENT_TYPE_PATH
                    && tileElement->GetBaseZ() == pos.baseZ && !tileElement->AsPath()->IsQueue()
                    && !tileElement->AsPath()->IsSloped())
                {
                    continue;
                }
                // Crossing mode 2: building path over track
                else if (
                    crossingMode == 2 && canBuildCrossing && tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK
                    && tileElement->GetBaseZ() == pos.baseZ && tileElement->AsTrack()->GetTrackType() == TrackElemType::Flat)
                {
                    auto ride = get_ride(tileElement->AsTrack()->GetRideIndex());
                    if (ride != nullptr && RideTypeDescriptors[ride->type].HasFlag(RIDE_TYPE_FLAG_SUPPORTS_LEVEL_CROSSINGS))
                    {
                        continue;
                    }
                }

                if (tileElement != nullptr)
                {
                    map_obstruction_set_error_text(tileElement, *res);
                    res->Error = GA_ERROR::NO_CLEARANCE;
                }
                return res;

            loc_68BAE6:
                if (clearFunc != nullptr)
                {
                    if (!clearFunc(&tileElement, pos, flags, &res->Cost))
                    {
                        goto loc_68B9B7;
                    }
                }
                if (tileElement != nullptr)
                {
                    res->Error = GA_ERROR::NO_CLEARANCE;
                    res->ErrorMessage = STR_CANNOT_BUILD_PARTLY_ABOVE_AND_PARTLY_BELOW_WATER;
                }
                return res;
            }
        }
    } while (!(tileElement++)->IsLastForTile());
    return res;
}

bool map_can_construct_with_clear_at(
    const CoordsXYRangedZ& pos, CLEAR_FUNC clearFunc, QuarterTile quarterTile, uint8_t flags, money32* price,
    uint8_t crossingMode)
{
    auto res = MapCanConstructWithClearAt(pos, clearFunc, quarterTile, flags, crossingMode);
    if (auto message = res->ErrorMessage.AsStringId())
        gGameCommandErrorText = *message;
    else
        gGameCommandErrorText = STR_NONE;
    std::copy(res->ErrorMessageArgs.begin(), res->ErrorMessageArgs.end(), gCommonFormatArgs);
    if (price != nullptr)
    {
        *price += res->Cost;
    }

    gMapGroundFlags = res->GroundFlags;
    return res->Error == GA_ERROR::OK;
}

/**
 *
 *  rct2: 0x0068B93A
 */
int32_t map_can_construct_at(const CoordsXYRangedZ& pos, QuarterTile bl)
{
    return map_can_construct_with_clear_at(pos, nullptr, bl, 0, nullptr, CREATE_CROSSING_MODE_NONE);
}

std::unique_ptr<ConstructClearResult> MapCanConstructAt(const CoordsXYRangedZ& pos, QuarterTile bl)
{
    return MapCanConstructWithClearAt(pos, nullptr, bl, 0, CREATE_CROSSING_MODE_NONE);
}
/**
 * Updates grass length, scenery age and jumping fountains.
 *
 *  rct2: 0x006646E1
 */
void map_update_tiles()
{
    int32_t ignoreScreenFlags = SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER;
    if (gScreenFlags & ignoreScreenFlags)
        return;

    // Update 43 more tiles
    for (int32_t j = 0; j < 43; j++)
    {
        int32_t x = 0;
        int32_t y = 0;

        uint16_t interleaved_xy = gGrassSceneryTileLoopPosition;
        for (int32_t i = 0; i < 8; i++)
        {
            x = (x << 1) | (interleaved_xy & 1);
            interleaved_xy >>= 1;
            y = (y << 1) | (interleaved_xy & 1);
            interleaved_xy >>= 1;
        }

        auto mapPos = TileCoordsXY{ x, y }.ToCoordsXY();
        auto* surfaceElement = map_get_surface_element_at(mapPos);
        if (surfaceElement != nullptr)
        {
            surfaceElement->UpdateGrassLength(mapPos);
            scenery_update_tile(mapPos);
        }

        gGrassSceneryTileLoopPosition++;
        gGrassSceneryTileLoopPosition &= 0xFFFF;
    }
}

void map_remove_provisional_elements()
{
    if (gFootpathProvisionalFlags & PROVISIONAL_PATH_FLAG_1)
    {
        footpath_provisional_remove();
        gFootpathProvisionalFlags |= PROVISIONAL_PATH_FLAG_1;
    }
    if (window_find_by_class(WC_RIDE_CONSTRUCTION) != nullptr)
    {
        ride_remove_provisional_track_piece();
        ride_entrance_exit_remove_ghost();
    }
    // This is in non performant so only make network games suffer for it
    // non networked games do not need this as its to prevent desyncs.
    if ((network_get_mode() != NETWORK_MODE_NONE) && window_find_by_class(WC_TRACK_DESIGN_PLACE) != nullptr)
    {
        auto intent = Intent(INTENT_ACTION_TRACK_DESIGN_REMOVE_PROVISIONAL);
        context_broadcast_intent(&intent);
    }
}

void map_restore_provisional_elements()
{
    if (gFootpathProvisionalFlags & PROVISIONAL_PATH_FLAG_1)
    {
        gFootpathProvisionalFlags &= ~PROVISIONAL_PATH_FLAG_1;
        footpath_provisional_set(gFootpathProvisionalType, gFootpathProvisionalPosition, gFootpathProvisionalSlope);
    }
    if (window_find_by_class(WC_RIDE_CONSTRUCTION) != nullptr)
    {
        ride_restore_provisional_track_piece();
        ride_entrance_exit_place_provisional_ghost();
    }
    // This is in non performant so only make network games suffer for it
    // non networked games do not need this as its to prevent desyncs.
    if ((network_get_mode() != NETWORK_MODE_NONE) && window_find_by_class(WC_TRACK_DESIGN_PLACE) != nullptr)
    {
        auto intent = Intent(INTENT_ACTION_TRACK_DESIGN_RESTORE_PROVISIONAL);
        context_broadcast_intent(&intent);
    }
}

/**
 * Removes elements that are out of the map size range and crops the park perimeter.
 *  rct2: 0x0068ADBC
 */
void map_remove_out_of_range_elements()
{
    int32_t mapMaxXY = gMapSizeMaxXY;

    // Ensure that we can remove elements
    //
    // NOTE: This is only a workaround for non-networked games.
    // Map resize has to become its own Game Action to properly solve this issue.
    //
    bool buildState = gCheatsBuildInPauseMode;
    gCheatsBuildInPauseMode = true;

    for (int32_t y = 0; y < MAXIMUM_MAP_SIZE_BIG; y += COORDS_XY_STEP)
    {
        for (int32_t x = 0; x < MAXIMUM_MAP_SIZE_BIG; x += COORDS_XY_STEP)
        {
            if (x == 0 || y == 0 || x >= mapMaxXY || y >= mapMaxXY)
            {
                // Note this purposely does not use LandSetRightsAction as X Y coordinates are outside of normal range.
                auto surfaceElement = map_get_surface_element_at(CoordsXY{ x, y });
                if (surfaceElement != nullptr)
                {
                    surfaceElement->SetOwnership(OWNERSHIP_UNOWNED);
                    update_park_fences_around_tile({ x, y });
                }
                clear_elements_at({ x, y });
            }
        }
    }

    // Reset cheat state
    gCheatsBuildInPauseMode = buildState;
}

static void map_extend_boundary_surface_extend_tile(const SurfaceElement& sourceTile, SurfaceElement& destTile)
{
    destTile.SetSurfaceStyle(sourceTile.GetSurfaceStyle());
    destTile.SetEdgeStyle(sourceTile.GetEdgeStyle());
    destTile.SetGrassLength(sourceTile.GetGrassLength());
    destTile.SetOwnership(OWNERSHIP_UNOWNED);
    destTile.SetWaterHeight(sourceTile.GetWaterHeight());

    auto z = sourceTile.base_height;
    auto slope = sourceTile.GetSlope() & TILE_ELEMENT_SLOPE_NW_SIDE_UP;
    if (slope == TILE_ELEMENT_SLOPE_NW_SIDE_UP)
    {
        z += 2;
        slope = TILE_ELEMENT_SLOPE_FLAT;
        if (sourceTile.GetSlope() & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
        {
            slope = TILE_ELEMENT_SLOPE_N_CORNER_UP;
            if (sourceTile.GetSlope() & TILE_ELEMENT_SLOPE_S_CORNER_UP)
            {
                slope = TILE_ELEMENT_SLOPE_W_CORNER_UP;
                if (sourceTile.GetSlope() & TILE_ELEMENT_SLOPE_E_CORNER_UP)
                {
                    slope = TILE_ELEMENT_SLOPE_FLAT;
                }
            }
        }
    }
    if (slope & TILE_ELEMENT_SLOPE_N_CORNER_UP)
        slope |= TILE_ELEMENT_SLOPE_E_CORNER_UP;
    if (slope & TILE_ELEMENT_SLOPE_W_CORNER_UP)
        slope |= TILE_ELEMENT_SLOPE_S_CORNER_UP;

    destTile.SetSlope(slope);
    destTile.base_height = z;
    destTile.clearance_height = z;
}

/**
 * Copies the terrain and slope from the edge of the map to the new tiles. Used when increasing the size of the map.
 *  rct2: 0x0068AC15
 */
void map_extend_boundary_surface()
{
    SurfaceElement *existingTileElement, *newTileElement;
    int32_t x, y;

    y = gMapSize - 2;
    for (x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; x++)
    {
        existingTileElement = map_get_surface_element_at(TileCoordsXY{ x, y - 1 }.ToCoordsXY());
        newTileElement = map_get_surface_element_at(TileCoordsXY{ x, y }.ToCoordsXY());

        if (existingTileElement && newTileElement)
        {
            map_extend_boundary_surface_extend_tile(*existingTileElement, *newTileElement);
        }

        update_park_fences({ x << 5, y << 5 });
    }

    x = gMapSize - 2;
    for (y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; y++)
    {
        existingTileElement = map_get_surface_element_at(TileCoordsXY{ x - 1, y }.ToCoordsXY());
        newTileElement = map_get_surface_element_at(TileCoordsXY{ x, y }.ToCoordsXY());

        if (existingTileElement && newTileElement)
        {
            map_extend_boundary_surface_extend_tile(*existingTileElement, *newTileElement);
        }

        update_park_fences({ x << 5, y << 5 });
    }
}

/**
 * Clears the provided element properly from a certain tile, and updates
 * the pointer (when needed) passed to this function to point to the next element.
 */
static void clear_element_at(const CoordsXY& loc, TileElement** elementPtr)
{
    TileElement* element = *elementPtr;
    switch (element->GetType())
    {
        case TILE_ELEMENT_TYPE_SURFACE:
            element->base_height = MINIMUM_LAND_HEIGHT;
            element->clearance_height = MINIMUM_LAND_HEIGHT;
            element->AsSurface()->SetSlope(TILE_ELEMENT_SLOPE_FLAT);
            element->AsSurface()->SetSurfaceStyle(TERRAIN_GRASS);
            element->AsSurface()->SetEdgeStyle(TERRAIN_EDGE_ROCK);
            element->AsSurface()->SetGrassLength(GRASS_LENGTH_CLEAR_0);
            element->AsSurface()->SetOwnership(OWNERSHIP_UNOWNED);
            element->AsSurface()->SetParkFences(0);
            element->AsSurface()->SetWaterHeight(0);
            // Because this element is not completely removed, the pointer must be updated manually
            // The rest of the elements are removed from the array, so the pointer doesn't need to be updated.
            (*elementPtr)++;
            break;
        case TILE_ELEMENT_TYPE_ENTRANCE:
        {
            int32_t rotation = element->GetDirectionWithOffset(1);
            auto seqLoc = loc;
            switch (element->AsEntrance()->GetSequenceIndex())
            {
                case 1:
                    seqLoc += CoordsDirectionDelta[rotation];
                    break;
                case 2:
                    seqLoc -= CoordsDirectionDelta[rotation];
                    break;
            }
            auto parkEntranceRemoveAction = ParkEntranceRemoveAction(CoordsXYZ{ seqLoc, element->GetBaseZ() });
            GameActions::Execute(&parkEntranceRemoveAction);
            break;
        }
        case TILE_ELEMENT_TYPE_WALL:
        {
            CoordsXYZD wallLocation = { loc.x, loc.y, element->GetBaseZ(), element->GetDirection() };
            auto wallRemoveAction = WallRemoveAction(wallLocation);
            GameActions::Execute(&wallRemoveAction);
        }
        break;
        case TILE_ELEMENT_TYPE_LARGE_SCENERY:
        {
            auto removeSceneryAction = LargeSceneryRemoveAction(
                { loc.x, loc.y, element->GetBaseZ(), element->GetDirection() }, element->AsLargeScenery()->GetSequenceIndex());
            GameActions::Execute(&removeSceneryAction);
        }
        break;
        case TILE_ELEMENT_TYPE_BANNER:
        {
            auto bannerRemoveAction = BannerRemoveAction(
                { loc.x, loc.y, element->GetBaseZ(), element->AsBanner()->GetPosition() });
            GameActions::Execute(&bannerRemoveAction);
            break;
        }
        default:
            tile_element_remove(element);
            break;
    }
}

/**
 * Clears all elements properly from a certain tile.
 *  rct2: 0x0068AE2A
 */
static void clear_elements_at(const CoordsXY& loc)
{
    // Remove the spawn point (if there is one in the current tile)
    gPeepSpawns.erase(
        std::remove_if(
            gPeepSpawns.begin(), gPeepSpawns.end(),
            [loc](const CoordsXY& spawn) { return spawn.ToTileStart() == loc.ToTileStart(); }),
        gPeepSpawns.end());

    TileElement* tileElement = map_get_first_element_at(loc);
    if (tileElement == nullptr)
        return;

    // Remove all elements except the last one
    while (!tileElement->IsLastForTile())
        clear_element_at(loc, &tileElement);

    // Remove the last element
    clear_element_at(loc, &tileElement);
}

int32_t map_get_highest_z(const CoordsXY& loc)
{
    auto surfaceElement = map_get_surface_element_at(loc);
    if (surfaceElement == nullptr)
        return -1;

    auto z = surfaceElement->GetBaseZ();

    // Raise z so that is above highest point of land and water on tile
    if ((surfaceElement->GetSlope() & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP) != TILE_ELEMENT_SLOPE_FLAT)
        z += LAND_HEIGHT_STEP;
    if ((surfaceElement->GetSlope() & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT) != 0)
        z += LAND_HEIGHT_STEP;

    z = std::max(z, surfaceElement->GetWaterHeight());
    return z;
}

LargeSceneryElement* map_get_large_scenery_segment(const CoordsXYZD& sceneryPos, int32_t sequence)
{
    TileElement* tileElement = map_get_first_element_at(sceneryPos);
    if (tileElement == nullptr)
    {
        return nullptr;
    }
    auto sceneryTilePos = TileCoordsXYZ{ sceneryPos };
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_LARGE_SCENERY)
            continue;
        if (tileElement->base_height != sceneryTilePos.z)
            continue;
        if (tileElement->AsLargeScenery()->GetSequenceIndex() != sequence)
            continue;
        if ((tileElement->GetDirection()) != sceneryPos.direction)
            continue;

        return tileElement->AsLargeScenery();
    } while (!(tileElement++)->IsLastForTile());
    return nullptr;
}

EntranceElement* map_get_park_entrance_element_at(const CoordsXYZ& entranceCoords, bool ghost)
{
    auto entranceTileCoords = TileCoordsXYZ(entranceCoords);
    TileElement* tileElement = map_get_first_element_at(entranceCoords);
    if (tileElement != nullptr)
    {
        do
        {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
                continue;

            if (tileElement->base_height != entranceTileCoords.z)
                continue;

            if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_PARK_ENTRANCE)
                continue;

            if (!ghost && tileElement->IsGhost())
                continue;

            return tileElement->AsEntrance();
        } while (!(tileElement++)->IsLastForTile());
    }
    return nullptr;
}

EntranceElement* map_get_ride_entrance_element_at(const CoordsXYZ& entranceCoords, bool ghost)
{
    auto entranceTileCoords = TileCoordsXYZ{ entranceCoords };
    TileElement* tileElement = map_get_first_element_at(entranceCoords);
    if (tileElement != nullptr)
    {
        do
        {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
                continue;

            if (tileElement->base_height != entranceTileCoords.z)
                continue;

            if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_RIDE_ENTRANCE)
                continue;

            if (!ghost && tileElement->IsGhost())
                continue;

            return tileElement->AsEntrance();
        } while (!(tileElement++)->IsLastForTile());
    }
    return nullptr;
}

EntranceElement* map_get_ride_exit_element_at(const CoordsXYZ& exitCoords, bool ghost)
{
    auto exitTileCoords = TileCoordsXYZ{ exitCoords };
    TileElement* tileElement = map_get_first_element_at(exitCoords);
    if (tileElement != nullptr)
    {
        do
        {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
                continue;

            if (tileElement->base_height != exitTileCoords.z)
                continue;

            if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_RIDE_EXIT)
                continue;

            if (!ghost && tileElement->IsGhost())
                continue;

            return tileElement->AsEntrance();
        } while (!(tileElement++)->IsLastForTile());
    }
    return nullptr;
}

SmallSceneryElement* map_get_small_scenery_element_at(const CoordsXYZ& sceneryCoords, int32_t type, uint8_t quadrant)
{
    auto sceneryTileCoords = TileCoordsXYZ{ sceneryCoords };
    TileElement* tileElement = map_get_first_element_at(sceneryCoords);
    if (tileElement != nullptr)
    {
        do
        {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
                continue;
            if (tileElement->AsSmallScenery()->GetSceneryQuadrant() != quadrant)
                continue;
            if (tileElement->base_height != sceneryTileCoords.z)
                continue;
            if (tileElement->AsSmallScenery()->GetEntryIndex() != type)
                continue;

            return tileElement->AsSmallScenery();
        } while (!(tileElement++)->IsLastForTile());
    }
    return nullptr;
}

std::optional<CoordsXYZ> map_large_scenery_get_origin(
    const CoordsXYZD& sceneryPos, int32_t sequence, LargeSceneryElement** outElement)
{
    rct_scenery_entry* sceneryEntry;
    rct_large_scenery_tile* tile;

    auto tileElement = map_get_large_scenery_segment(sceneryPos, sequence);
    if (tileElement == nullptr)
        return std::nullopt;

    sceneryEntry = tileElement->GetEntry();
    tile = &sceneryEntry->large_scenery.tiles[sequence];

    CoordsXY offsetPos{ tile->x_offset, tile->y_offset };
    auto rotatedOffsetPos = offsetPos.Rotate(sceneryPos.direction);

    auto origin = CoordsXYZ{ sceneryPos.x - rotatedOffsetPos.x, sceneryPos.y - rotatedOffsetPos.y,
                             sceneryPos.z - tile->z_offset };
    if (outElement != nullptr)
        *outElement = tileElement;
    return origin;
}

/**
 *
 *  rct2: 0x006B9B05
 */
bool map_large_scenery_sign_set_colour(const CoordsXYZD& signPos, int32_t sequence, uint8_t mainColour, uint8_t textColour)
{
    LargeSceneryElement* tileElement;
    rct_scenery_entry* sceneryEntry;
    rct_large_scenery_tile *sceneryTiles, *tile;

    auto sceneryOrigin = map_large_scenery_get_origin(signPos, sequence, &tileElement);
    if (!sceneryOrigin)
    {
        return false;
    }

    sceneryEntry = tileElement->GetEntry();
    sceneryTiles = sceneryEntry->large_scenery.tiles;

    // Iterate through each tile of the large scenery element
    sequence = 0;
    for (tile = sceneryTiles; tile->x_offset != -1; tile++, sequence++)
    {
        CoordsXY offsetPos{ tile->x_offset, tile->y_offset };
        auto rotatedOffsetPos = offsetPos.Rotate(signPos.direction);

        auto tmpSignPos = CoordsXYZD{ sceneryOrigin->x + rotatedOffsetPos.x, sceneryOrigin->y + rotatedOffsetPos.y,
                                      sceneryOrigin->z + tile->z_offset, signPos.direction };
        tileElement = map_get_large_scenery_segment(tmpSignPos, sequence);
        if (tileElement != nullptr)
        {
            tileElement->SetPrimaryColour(mainColour);
            tileElement->SetSecondaryColour(textColour);

            map_invalidate_tile({ tmpSignPos, tileElement->GetBaseZ(), tileElement->GetClearanceZ() });
        }
    }

    return true;
}

static ScreenCoordsXY translate_3d_to_2d(int32_t rotation, const CoordsXY& pos)
{
    return translate_3d_to_2d_with_z(rotation, CoordsXYZ{ pos, 0 });
}

ScreenCoordsXY translate_3d_to_2d_with_z(int32_t rotation, const CoordsXYZ& pos)
{
    auto rotated = pos.Rotate(rotation);
    // Use right shift to avoid issues like #9301
    return ScreenCoordsXY{ rotated.y - rotated.x, ((rotated.x + rotated.y) >> 1) - pos.z };
}

static void map_invalidate_tile_under_zoom(int32_t x, int32_t y, int32_t z0, int32_t z1, int32_t maxZoom)
{
    if (gOpenRCT2Headless)
        return;

    int32_t x1, y1, x2, y2;

    x += 16;
    y += 16;
    auto screenCoord = translate_3d_to_2d(get_current_rotation(), { x, y });

    x1 = screenCoord.x - 32;
    y1 = screenCoord.y - 32 - z1;
    x2 = screenCoord.x + 32;
    y2 = screenCoord.y + 32 - z0;

    for (int32_t i = 0; i < MAX_VIEWPORT_COUNT; i++)
    {
        rct_viewport* viewport = &g_viewport_list[i];
        if (viewport->width != 0 && (maxZoom == -1 || viewport->zoom <= maxZoom))
        {
            viewport_invalidate(viewport, x1, y1, x2, y2);
        }
    }
}

/**
 *
 *  rct2: 0x006EC847
 */
void map_invalidate_tile(const CoordsXYRangedZ& tilePos)
{
    map_invalidate_tile_under_zoom(tilePos.x, tilePos.y, tilePos.baseZ, tilePos.clearanceZ, -1);
}

/**
 *
 *  rct2: 0x006ECB60
 */
void map_invalidate_tile_zoom1(const CoordsXYRangedZ& tilePos)
{
    map_invalidate_tile_under_zoom(tilePos.x, tilePos.y, tilePos.baseZ, tilePos.clearanceZ, 1);
}

/**
 *
 *  rct2: 0x006EC9CE
 */
void map_invalidate_tile_zoom0(const CoordsXYRangedZ& tilePos)
{
    map_invalidate_tile_under_zoom(tilePos.x, tilePos.y, tilePos.baseZ, tilePos.clearanceZ, 0);
}

/**
 *
 *  rct2: 0x006EC6D7
 */
void map_invalidate_tile_full(const CoordsXY& tilePos)
{
    map_invalidate_tile({ tilePos, 0, 2080 });
}

void map_invalidate_element(const CoordsXY& elementPos, TileElement* tileElement)
{
    map_invalidate_tile({ elementPos, tileElement->GetBaseZ(), tileElement->GetClearanceZ() });
}

void map_invalidate_region(const CoordsXY& mins, const CoordsXY& maxs)
{
    int32_t x0, y0, x1, y1, left, right, top, bottom;

    x0 = mins.x + 16;
    y0 = mins.y + 16;

    x1 = maxs.x + 16;
    y1 = maxs.y + 16;

    map_get_bounding_box({ x0, y0, x1, y1 }, &left, &top, &right, &bottom);

    left -= 32;
    right += 32;
    bottom += 32;
    top -= 32 + 2080;

    for (int32_t i = 0; i < MAX_VIEWPORT_COUNT; i++)
    {
        rct_viewport* viewport = &g_viewport_list[i];
        if (viewport->width != 0)
        {
            viewport_invalidate(viewport, left, top, right, bottom);
        }
    }
}

int32_t map_get_tile_side(const CoordsXY& mapPos)
{
    int32_t subMapX = mapPos.x & (32 - 1);
    int32_t subMapY = mapPos.y & (32 - 1);
    return (subMapX < subMapY) ? ((subMapX + subMapY) < 32 ? 0 : 1) : ((subMapX + subMapY) < 32 ? 3 : 2);
}

int32_t map_get_tile_quadrant(const CoordsXY& mapPos)
{
    int32_t subMapX = mapPos.x & (32 - 1);
    int32_t subMapY = mapPos.y & (32 - 1);
    return (subMapX > 16) ? (subMapY < 16 ? 1 : 0) : (subMapY < 16 ? 2 : 3);
}

/**
 *
 *  rct2: 0x00693BFF
 */
bool map_surface_is_blocked(const CoordsXY& mapCoords)
{
    if (!map_is_location_valid(mapCoords))
        return true;

    auto surfaceElement = map_get_surface_element_at(mapCoords);

    if (surfaceElement == nullptr)
    {
        return true;
    }

    if (surfaceElement->GetWaterHeight() > surfaceElement->GetBaseZ())
        return true;

    int16_t base_z = surfaceElement->base_height;
    int16_t clear_z = surfaceElement->base_height + 2;
    if (surfaceElement->GetSlope() & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
        clear_z += 2;

    auto tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    while (!(tileElement++)->IsLastForTile())
    {
        if (clear_z >= tileElement->clearance_height)
            continue;

        if (base_z < tileElement->base_height)
            continue;

        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH || tileElement->GetType() == TILE_ELEMENT_TYPE_WALL)
            continue;

        if (tileElement->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
            return true;

        rct_scenery_entry* scenery = tileElement->AsSmallScenery()->GetEntry();
        if (scenery == nullptr)
        {
            return false;
        }
        if (scenery_small_entry_has_flag(scenery, SMALL_SCENERY_FLAG_FULL_TILE))
            return true;
    }
    return false;
}

/* Clears all map elements, to be used before generating a new map */
void map_clear_all_elements()
{
    for (int32_t y = 0; y < MAXIMUM_MAP_SIZE_BIG; y += COORDS_XY_STEP)
    {
        for (int32_t x = 0; x < MAXIMUM_MAP_SIZE_BIG; x += COORDS_XY_STEP)
        {
            clear_elements_at({ x, y });
        }
    }
}

/**
 * Gets the track element at x, y, z.
 * @param x x units, not tiles.
 * @param y y units, not tiles.
 * @param z Base height.
 */
TrackElement* map_get_track_element_at(const CoordsXYZ& trackPos)
{
    TileElement* tileElement = map_get_first_element_at(trackPos);
    if (tileElement == nullptr)
        return nullptr;
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (tileElement->GetBaseZ() != trackPos.z)
            continue;

        return tileElement->AsTrack();
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

/**
 * Gets the track element at x, y, z that is the given track type.
 * @param x x units, not tiles.
 * @param y y units, not tiles.
 * @param z Base height.
 */
TileElement* map_get_track_element_at_of_type(const CoordsXYZ& trackPos, int32_t trackType)
{
    TileElement* tileElement = map_get_first_element_at(trackPos);
    if (tileElement == nullptr)
        return nullptr;
    auto trackTilePos = TileCoordsXYZ{ trackPos };
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (tileElement->base_height != trackTilePos.z)
            continue;
        if (tileElement->AsTrack()->GetTrackType() != trackType)
            continue;

        return tileElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

/**
 * Gets the track element at x, y, z that is the given track type and sequence.
 * @param x x units, not tiles.
 * @param y y units, not tiles.
 * @param z Base height.
 */
TileElement* map_get_track_element_at_of_type_seq(const CoordsXYZ& trackPos, int32_t trackType, int32_t sequence)
{
    TileElement* tileElement = map_get_first_element_at(trackPos);
    auto trackTilePos = TileCoordsXYZ{ trackPos };
    do
    {
        if (tileElement == nullptr)
            break;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (tileElement->base_height != trackTilePos.z)
            continue;
        if (tileElement->AsTrack()->GetTrackType() != trackType)
            continue;
        if (tileElement->AsTrack()->GetSequenceIndex() != sequence)
            continue;

        return tileElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

TrackElement* map_get_track_element_at_of_type(const CoordsXYZD& location, int32_t trackType)
{
    auto tileElement = map_get_first_element_at(location);
    if (tileElement != nullptr)
    {
        do
        {
            auto trackElement = tileElement->AsTrack();
            if (trackElement != nullptr)
            {
                if (trackElement->GetBaseZ() != location.z)
                    continue;
                if (trackElement->GetDirection() != location.direction)
                    continue;
                if (trackElement->GetTrackType() != trackType)
                    continue;
                return trackElement;
            }
        } while (!(tileElement++)->IsLastForTile());
    }
    return nullptr;
}

TrackElement* map_get_track_element_at_of_type_seq(const CoordsXYZD& location, int32_t trackType, int32_t sequence)
{
    auto tileElement = map_get_first_element_at(location);
    if (tileElement != nullptr)
    {
        do
        {
            auto trackElement = tileElement->AsTrack();
            if (trackElement != nullptr)
            {
                if (trackElement->GetBaseZ() != location.z)
                    continue;
                if (trackElement->GetDirection() != location.direction)
                    continue;
                if (trackElement->GetTrackType() != trackType)
                    continue;
                if (trackElement->GetSequenceIndex() != sequence)
                    continue;
                return trackElement;
            }
        } while (!(tileElement++)->IsLastForTile());
    }
    return nullptr;
}

/**
 * Gets the track element at x, y, z that is the given track type and sequence.
 * @param x x units, not tiles.
 * @param y y units, not tiles.
 * @param z Base height.
 */
TileElement* map_get_track_element_at_of_type_from_ride(const CoordsXYZ& trackPos, int32_t trackType, ride_id_t rideIndex)
{
    TileElement* tileElement = map_get_first_element_at(trackPos);
    if (tileElement == nullptr)
        return nullptr;
    auto trackTilePos = TileCoordsXYZ{ trackPos };
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (tileElement->base_height != trackTilePos.z)
            continue;
        if (tileElement->AsTrack()->GetRideIndex() != rideIndex)
            continue;
        if (tileElement->AsTrack()->GetTrackType() != trackType)
            continue;

        return tileElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
};

/**
 * Gets the track element at x, y, z that is the given track type and sequence.
 * @param x x units, not tiles.
 * @param y y units, not tiles.
 * @param z Base height.
 */
TileElement* map_get_track_element_at_from_ride(const CoordsXYZ& trackPos, ride_id_t rideIndex)
{
    TileElement* tileElement = map_get_first_element_at(trackPos);
    if (tileElement == nullptr)
        return nullptr;
    auto trackTilePos = TileCoordsXYZ{ trackPos };
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (tileElement->base_height != trackTilePos.z)
            continue;
        if (tileElement->AsTrack()->GetRideIndex() != rideIndex)
            continue;

        return tileElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
};

/**
 * Gets the track element at x, y, z that is the given track type and sequence.
 * @param x x units, not tiles.
 * @param y y units, not tiles.
 * @param z Base height.
 * @param direction The direction (0 - 3).
 */
TileElement* map_get_track_element_at_with_direction_from_ride(const CoordsXYZD& trackPos, ride_id_t rideIndex)
{
    TileElement* tileElement = map_get_first_element_at(trackPos);
    if (tileElement == nullptr)
        return nullptr;
    auto trackTilePos = TileCoordsXYZ{ trackPos };
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (tileElement->base_height != trackTilePos.z)
            continue;
        if (tileElement->AsTrack()->GetRideIndex() != rideIndex)
            continue;
        if (tileElement->GetDirection() != trackPos.direction)
            continue;

        return tileElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
};

WallElement* map_get_wall_element_at(const CoordsXYRangedZ& coords)
{
    auto tileElement = map_get_first_element_at(coords);

    if (tileElement != nullptr)
    {
        do
        {
            if (tileElement->GetType() == TILE_ELEMENT_TYPE_WALL && coords.baseZ < tileElement->GetClearanceZ()
                && coords.clearanceZ > tileElement->GetBaseZ())
            {
                return tileElement->AsWall();
            }
        } while (!(tileElement++)->IsLastForTile());
    }

    return nullptr;
}

WallElement* map_get_wall_element_at(const CoordsXYZD& wallCoords)
{
    auto tileWallCoords = TileCoordsXYZ(wallCoords);
    TileElement* tileElement = map_get_first_element_at(wallCoords);
    if (tileElement == nullptr)
        return nullptr;
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_WALL)
            continue;
        if (tileElement->base_height != tileWallCoords.z)
            continue;
        if (tileElement->GetDirection() != wallCoords.direction)
            continue;

        return tileElement->AsWall();
    } while (!(tileElement++)->IsLastForTile());
    return nullptr;
}

uint16_t check_max_allowable_land_rights_for_tile(const CoordsXYZ& tileMapPos)
{
    TileElement* tileElement = map_get_first_element_at(tileMapPos);
    uint16_t destOwnership = OWNERSHIP_OWNED;

    // Sometimes done deliberately.
    if (tileElement == nullptr)
    {
        return OWNERSHIP_OWNED;
    }

    auto tilePos = TileCoordsXYZ{ tileMapPos };
    do
    {
        int32_t type = tileElement->GetType();
        if (type == TILE_ELEMENT_TYPE_PATH
            || (type == TILE_ELEMENT_TYPE_ENTRANCE
                && tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE))
        {
            destOwnership = OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED;
            // Do not own construction rights if too high/below surface
            if (tileElement->base_height - 3 > tilePos.z || tileElement->base_height < tilePos.z)
            {
                destOwnership = OWNERSHIP_UNOWNED;
                break;
            }
        }
    } while (!(tileElement++)->IsLastForTile());

    return destOwnership;
}

void FixLandOwnershipTiles(std::initializer_list<TileCoordsXY> tiles)
{
    FixLandOwnershipTilesWithOwnership(tiles, OWNERSHIP_AVAILABLE);
}

void FixLandOwnershipTilesWithOwnership(std::initializer_list<TileCoordsXY> tiles, uint8_t ownership)
{
    for (const TileCoordsXY* tile = tiles.begin(); tile != tiles.end(); ++tile)
    {
        auto surfaceElement = map_get_surface_element_at(tile->ToCoordsXY());
        if (surfaceElement != nullptr)
        {
            surfaceElement->SetOwnership(ownership);
            update_park_fences_around_tile({ (*tile).x * 32, (*tile).y * 32 });
        }
    }
}
