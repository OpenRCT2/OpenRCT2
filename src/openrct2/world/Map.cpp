/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
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
#include "../actions/FootpathRemoveAction.hpp"
#include "../actions/LandSetHeightAction.hpp"
#include "../actions/LargeSceneryRemoveAction.hpp"
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

using namespace OpenRCT2;

/**
 * Replaces 0x00993CCC, 0x00993CCE
 */
const CoordsXY CoordsDirectionDelta[] = { { -32, 0 },   { 0, +32 },   { +32, 0 },   { 0, -32 },
                                          { -32, +32 }, { +32, +32 }, { +32, -32 }, { -32, -32 } };

const TileCoordsXY TileDirectionDelta[] = { { -1, 0 },  { 0, +1 },  { +1, 0 },  { 0, -1 },
                                            { -1, +1 }, { +1, +1 }, { +1, -1 }, { -1, -1 } };

uint16_t gMapSelectFlags;
uint16_t gMapSelectType;
LocationXY16 gMapSelectPositionA;
LocationXY16 gMapSelectPositionB;
LocationXYZ16 gMapSelectArrowPosition;
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

TileElement gTileElements[MAX_TILE_TILE_ELEMENT_POINTERS * 3];
TileElement* gTileElementTilePointers[MAX_TILE_TILE_ELEMENT_POINTERS];
LocationXY16 gMapSelectionTiles[300];
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

LocationXYZ16 gCommandPosition;

bool gMapLandRightsUpdateSuccess;

static void clear_elements_at(int32_t x, int32_t y);
static void translate_3d_to_2d(int32_t rotation, int32_t* x, int32_t* y);

void rotate_map_coordinates(int16_t* x, int16_t* y, int32_t rotation)
{
    int32_t temp;

    switch (rotation)
    {
        case TILE_ELEMENT_DIRECTION_WEST:
            break;
        case TILE_ELEMENT_DIRECTION_NORTH:
            temp = *x;
            *x = *y;
            *y = -temp;
            break;
        case TILE_ELEMENT_DIRECTION_EAST:
            *x = -*x;
            *y = -*y;
            break;
        case TILE_ELEMENT_DIRECTION_SOUTH:
            temp = *y;
            *y = *x;
            *x = -temp;
            break;
    }
}

LocationXY16 coordinate_3d_to_2d(const LocationXYZ16* coordinate_3d, int32_t rotation)
{
    LocationXY16 coordinate_2d;

    switch (rotation)
    {
        default:
        case 0:
            coordinate_2d.x = coordinate_3d->y - coordinate_3d->x;
            coordinate_2d.y = ((coordinate_3d->y + coordinate_3d->x) >> 1) - coordinate_3d->z;
            break;
        case 1:
            coordinate_2d.x = -coordinate_3d->y - coordinate_3d->x;
            coordinate_2d.y = ((coordinate_3d->y - coordinate_3d->x) >> 1) - coordinate_3d->z;
            break;
        case 2:
            coordinate_2d.x = -coordinate_3d->y + coordinate_3d->x;
            coordinate_2d.y = ((-coordinate_3d->y - coordinate_3d->x) >> 1) - coordinate_3d->z;
            break;
        case 3:
            coordinate_2d.x = coordinate_3d->y + coordinate_3d->x;
            coordinate_2d.y = ((-coordinate_3d->y + coordinate_3d->x) >> 1) - coordinate_3d->z;
            break;
    }
    return coordinate_2d;
}

void tile_element_iterator_begin(tile_element_iterator* it)
{
    it->x = 0;
    it->y = 0;
    it->element = map_get_first_element_at(0, 0);
}

int32_t tile_element_iterator_next(tile_element_iterator* it)
{
    if (it->element == nullptr)
    {
        it->element = map_get_first_element_at(it->x, it->y);
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
        it->element = map_get_first_element_at(it->x, it->y);
        return 1;
    }

    if (it->y < (MAXIMUM_MAP_SIZE_TECHNICAL - 1))
    {
        it->x = 0;
        it->y++;
        it->element = map_get_first_element_at(it->x, it->y);
        return 1;
    }

    return 0;
}

void tile_element_iterator_restart_for_tile(tile_element_iterator* it)
{
    it->element = nullptr;
}

TileElement* map_get_first_element_at(int32_t x, int32_t y)
{
    if (x < 0 || y < 0 || x > (MAXIMUM_MAP_SIZE_TECHNICAL - 1) || y > (MAXIMUM_MAP_SIZE_TECHNICAL - 1))
    {
        log_error("Trying to access element outside of range");
        return nullptr;
    }
    return gTileElementTilePointers[x + y * MAXIMUM_MAP_SIZE_TECHNICAL];
}

TileElement* map_get_nth_element_at(int32_t x, int32_t y, int32_t n)
{
    TileElement* tileElement = map_get_first_element_at(x, y);
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

void map_set_tile_elements(int32_t x, int32_t y, TileElement* elements)
{
    if (x < 0 || y < 0 || x > (MAXIMUM_MAP_SIZE_TECHNICAL - 1) || y > (MAXIMUM_MAP_SIZE_TECHNICAL - 1))
    {
        log_error("Trying to access element outside of range");
        return;
    }
    gTileElementTilePointers[x + y * MAXIMUM_MAP_SIZE_TECHNICAL] = elements;
}

TileElement* map_get_surface_element_at(int32_t x, int32_t y)
{
    TileElement* tileElement = map_get_first_element_at(x, y);

    if (tileElement == nullptr)
        return nullptr;

    // Find the first surface element
    while (tileElement->GetType() != TILE_ELEMENT_TYPE_SURFACE)
    {
        if (tileElement->IsLastForTile())
            return nullptr;

        tileElement++;
    }

    return tileElement;
}

TileElement* map_get_surface_element_at(const CoordsXY coords)
{
    return map_get_surface_element_at(coords.x / 32, coords.y / 32);
}

TileElement* map_get_path_element_at(int32_t x, int32_t y, int32_t z)
{
    TileElement* tileElement = map_get_first_element_at(x, y);

    if (tileElement == nullptr)
        return nullptr;

    // Find the path element at known z
    do
    {
        if (tileElement->IsGhost())
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;
        if (tileElement->base_height != z)
            continue;

        return tileElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

BannerElement* map_get_banner_element_at(int32_t x, int32_t y, int32_t z, uint8_t position)
{
    TileElement* tileElement = map_get_first_element_at(x, y);

    if (tileElement == nullptr)
        return nullptr;

    // Find the banner element at known z and position
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_BANNER)
            continue;
        if (tileElement->base_height != z)
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
    gNumMapAnimations = 0;
    gNextFreeTileElementPointerIndex = 0;

    for (int32_t i = 0; i < MAX_TILE_TILE_ELEMENT_POINTERS; i++)
    {
        TileElement* tile_element = &gTileElements[i];
        tile_element->ClearAs(TILE_ELEMENT_TYPE_SURFACE);
        tile_element->flags = TILE_ELEMENT_FLAG_LAST_TILE;
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
            TileElement* element = map_get_surface_element_at(x, y);
            // Surface elements are sometimes hacked out to save some space for other map elements
            if (element == nullptr)
            {
                continue;
            }

            uint8_t flags = element->AsSurface()->GetOwnership();

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
int32_t tile_element_height(int32_t x, int32_t y)
{
    TileElement* tileElement;

    // Off the map
    if ((unsigned)x >= 8192 || (unsigned)y >= 8192)
        return 16;

    // Truncate subtile coordinates
    int32_t x_tile = x & 0xFFFFFFE0;
    int32_t y_tile = y & 0xFFFFFFE0;

    // Get the surface element for the tile
    tileElement = map_get_surface_element_at({ x_tile, y_tile });

    if (tileElement == nullptr)
    {
        return 16;
    }

    uint32_t height = (tileElement->AsSurface()->GetWaterHeight() << 20) | (tileElement->base_height << 3);

    uint32_t slope = tileElement->AsSurface()->GetSlope();
    uint8_t extra_height = (slope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT) >> 4; // 0x10 is the 5th bit - sets slope to double height
    // Remove the extra height bit
    slope &= TILE_ELEMENT_SLOPE_ALL_CORNERS_UP;

    int8_t quad = 0, quad_extra = 0; // which quadrant the element is in?
                                     // quad_extra is for extra height tiles

    uint8_t xl, yl; // coordinates across this tile

    uint8_t TILE_SIZE = 31;

    xl = x & 0x1f;
    yl = y & 0x1f;

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
        height += 0x10;
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

/**
 * Checks if the tile at coordinate at height counts as connected.
 * @return 1 if connected, 0 otherwise
 */
bool map_coord_is_connected(int32_t x, int32_t y, int32_t z, uint8_t faceDirection)
{
    TileElement* tileElement = map_get_first_element_at(x, y);

    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;

        uint8_t slopeDirection = tileElement->AsPath()->GetSlopeDirection();

        if (tileElement->AsPath()->IsSloped())
        {
            if (slopeDirection == faceDirection)
            {
                if (z == tileElement->base_height + 2)
                    return true;
            }
            else if (direction_reverse(slopeDirection) == faceDirection && z == tileElement->base_height)
            {
                return true;
            }
        }
        else
        {
            if (z == tileElement->base_height)
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
        footpath_update_path_wide_flags(x, y);

        // Next x, y tile
        x += 32;
        if (x >= 8192)
        {
            x = 0;
            y += 32;
            if (y >= 8192)
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
int32_t map_height_from_slope(const CoordsXY coords, int32_t slope, bool isSloped)
{
    if (!isSloped)
        return 0;

    switch (slope & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK)
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

bool map_is_location_valid(const CoordsXY coords)
{
    if (coords.x < (MAXIMUM_MAP_SIZE_TECHNICAL * 32) && coords.x >= 0 && coords.y < (MAXIMUM_MAP_SIZE_TECHNICAL * 32)
        && coords.y >= 0)
    {
        return true;
    }
    return false;
}

bool map_is_edge(const CoordsXY coords)
{
    return (coords.x < 32 || coords.y < 32 || coords.x >= gMapSizeUnits || coords.y >= gMapSizeUnits);
}

bool map_can_build_at(int32_t x, int32_t y, int32_t z)
{
    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        return true;
    if (gCheatsSandboxMode)
        return true;
    if (map_is_location_owned(x, y, z))
        return true;
    return false;
}

/**
 *
 *  rct2: 0x00664F72
 */
bool map_is_location_owned(int32_t x, int32_t y, int32_t z)
{
    // This check is to avoid throwing lots of messages in logs.
    if (map_is_location_valid({ x, y }))
    {
        TileElement* tileElement = map_get_surface_element_at({ x, y });
        if (tileElement != nullptr)
        {
            if (tileElement->AsSurface()->GetOwnership() & OWNERSHIP_OWNED)
                return true;

            if (tileElement->AsSurface()->GetOwnership() & OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED)
            {
                z /= 8;
                if (z < tileElement->base_height || z - 2 > tileElement->base_height)
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
bool map_is_location_in_park(const CoordsXY coords)
{
    if (map_is_location_valid(coords))
    {
        TileElement* tileElement = map_get_surface_element_at(coords);
        if (tileElement == nullptr)
            return false;
        if (tileElement->AsSurface()->GetOwnership() & OWNERSHIP_OWNED)
            return true;
    }

    gGameCommandErrorText = STR_LAND_NOT_OWNED_BY_PARK;
    return false;
}

bool map_is_location_owned_or_has_rights(int32_t x, int32_t y)
{
    if (map_is_location_valid({ x, y }))
    {
        TileElement* tileElement = map_get_surface_element_at({ x, y });
        if (tileElement == nullptr)
        {
            return false;
        }
        if (tileElement->AsSurface()->GetOwnership() & OWNERSHIP_OWNED)
            return true;
        if (tileElement->AsSurface()->GetOwnership() & OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED)
            return true;
    }
    return false;
}

/**
 *
 *  rct2: 0x006B909A
 */
void game_command_set_large_scenery_colour(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi, [[maybe_unused]] int32_t* edi,
    int32_t* ebp)
{
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
    int32_t x = *eax;
    int32_t y = *ecx;
    uint8_t tile_element_direction = *ebx >> 8;
    uint8_t flags = *ebx & 0xFF;
    uint8_t base_height = *edx;
    uint8_t tileIndex = *edx >> 8;
    uint8_t colour1 = *ebp;
    uint8_t colour2 = *ebp >> 8;
    int32_t z = tile_element_height(x, y);
    gCommandPosition.x = x + 16;
    gCommandPosition.y = y + 16;
    gCommandPosition.z = z;

    TileElement* tile_element = map_get_large_scenery_segment(x, y, base_height, tile_element_direction, tileIndex);

    if (tile_element == nullptr)
    {
        *ebx = 0;
        return;
    }

    if ((flags & GAME_COMMAND_FLAG_GHOST) && !(tile_element->IsGhost()))
    {
        *ebx = 0;
        return;
    }

    rct_scenery_entry* scenery_entry = tile_element->AsLargeScenery()->GetEntry();

    // Work out the base tile coordinates (Tile with index 0)
    LocationXYZ16 baseTile = {
        scenery_entry->large_scenery.tiles[tileIndex].x_offset, scenery_entry->large_scenery.tiles[tileIndex].y_offset,
        static_cast<int16_t>((base_height * 8) - scenery_entry->large_scenery.tiles[tileIndex].z_offset)
    };
    rotate_map_coordinates(&baseTile.x, &baseTile.y, tile_element_direction);
    baseTile.x = x - baseTile.x;
    baseTile.y = y - baseTile.y;

    for (int32_t i = 0; scenery_entry->large_scenery.tiles[i].x_offset != -1; ++i)
    {
        assert(i < MAXIMUM_MAP_SIZE_TECHNICAL);

        // Work out the current tile coordinates
        LocationXYZ16 currentTile = { scenery_entry->large_scenery.tiles[i].x_offset,
                                      scenery_entry->large_scenery.tiles[i].y_offset,
                                      scenery_entry->large_scenery.tiles[i].z_offset };
        rotate_map_coordinates(&currentTile.x, &currentTile.y, tile_element_direction);
        currentTile.x += baseTile.x;
        currentTile.y += baseTile.y;
        currentTile.z += baseTile.z;

        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
        {
            if (!map_is_location_owned(currentTile.x, currentTile.y, currentTile.z))
            {
                *ebx = MONEY32_UNDEFINED;
                return;
            }
        }

        if (flags & GAME_COMMAND_FLAG_APPLY)
        {
            TileElement* tileElement = map_get_large_scenery_segment(
                currentTile.x, currentTile.y, base_height, tile_element_direction, i);

            tileElement->AsLargeScenery()->SetPrimaryColour(colour1);
            tileElement->AsLargeScenery()->SetSecondaryColour(colour2);

            map_invalidate_tile_full(currentTile.x, currentTile.y);
        }
    }
    *ebx = 0;
}

/**
 *
 *  rct2: 0x00663CCD
 */
static money32 map_change_surface_style(
    int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint8_t surfaceStyle, uint8_t edgeStyle, uint8_t flags)
{
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;

    x0 = std::max(x0, 32);
    y0 = std::max(y0, 32);
    x1 = std::min(x1, (int32_t)gMapSizeMaxXY);
    y1 = std::min(y1, (int32_t)gMapSizeMaxXY);

    int32_t xMid, yMid;

    xMid = (x0 + x1) / 2 + 16;
    yMid = (y0 + y1) / 2 + 16;

    int32_t heightMid = tile_element_height(xMid, yMid);

    gCommandPosition.x = xMid;
    gCommandPosition.y = yMid;
    gCommandPosition.z = heightMid;

    // Do nothing during pause
    if (game_is_paused() && !gCheatsBuildInPauseMode)
    {
        return 0;
    }

    // Do nothing if not in editor, sandbox mode or landscaping is forbidden
    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode
        && (gParkFlags & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES))
    {
        return 0;
    }

    money32 surfaceCost = 0;
    money32 edgeCost = 0;
    for (int32_t x = x0; x <= x1; x += 32)
    {
        for (int32_t y = y0; y <= y1; y += 32)
        {
            if (x > 0x1FFF)
                continue;
            if (y > 0x1FFF)
                continue;

            if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
            {
                if (!map_is_location_in_park({ x, y }))
                    continue;
            }

            auto surfaceElement = map_get_surface_element_at({ x, y })->AsSurface();
            if (surfaceElement == nullptr)
            {
                continue;
            }

            if (surfaceStyle != 0xFF)
            {
                uint8_t cur_terrain = surfaceElement->GetSurfaceStyle();

                if (surfaceStyle != cur_terrain)
                {
                    // Prevent network-originated value of surfaceStyle from causing
                    // invalid access.
                    uint8_t style = surfaceStyle & 0x1F;
                    auto& objManager = GetContext()->GetObjectManager();
                    const auto surfaceObj = static_cast<TerrainSurfaceObject*>(
                        objManager.GetLoadedObject(OBJECT_TYPE_TERRAIN_SURFACE, style));
                    if (surfaceObj != nullptr)
                    {
                        surfaceCost += surfaceObj->Price;
                    }

                    if (flags & GAME_COMMAND_FLAG_APPLY)
                    {
                        surfaceElement->SetSurfaceStyle(surfaceStyle);

                        map_invalidate_tile_full(x, y);
                        footpath_remove_litter(x, y, tile_element_height(x, y));
                    }
                }
            }

            if (edgeStyle != 0xFF)
            {
                uint8_t currentEdge = surfaceElement->GetEdgeStyle();

                if (edgeStyle != currentEdge)
                {
                    edgeCost += 100;

                    if (flags & GAME_COMMAND_FLAG_APPLY)
                    {
                        surfaceElement->SetEdgeStyle(edgeStyle);
                        map_invalidate_tile_full(x, y);
                    }
                }
            }

            if (flags & GAME_COMMAND_FLAG_APPLY)
            {
                if (surfaceElement->CanGrassGrow() && (surfaceElement->GetGrassLength() & 7) != GRASS_LENGTH_CLEAR_0)
                {
                    surfaceElement->SetGrassLength(GRASS_LENGTH_CLEAR_0);
                    map_invalidate_tile_full(x, y);
                }
            }
        }
    }

    if (flags & GAME_COMMAND_FLAG_APPLY && gGameCommandNestLevel == 1)
    {
        LocationXYZ16 coord;
        coord.x = ((x0 + x1) / 2) + 16;
        coord.y = ((y0 + y1) / 2) + 16;
        coord.z = tile_element_height(coord.x, coord.y);
        network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
    }

    return (gParkFlags & PARK_FLAGS_NO_MONEY) ? 0 : surfaceCost + edgeCost;
}

/**
 *
 *  rct2: 0x00663CCD
 */
void game_command_change_surface_style(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi, int32_t* edi, int32_t* ebp)
{
    *ebx = map_change_surface_style(
        (int16_t)(*eax & 0xFFFF), (int16_t)(*ecx & 0xFFFF), (int16_t)(*edi & 0xFFFF), (int16_t)(*ebp & 0xFFFF), *edx & 0xFF,
        (*edx & 0xFF00) >> 8, *ebx & 0xFF);
}

// 0x00981A1E
#define SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT 0x20
// Table of pre-calculated surface slopes (32) when raising the land tile for a given selection (5)
// 0x1F = new slope
// 0x20 = base height increases
static constexpr const uint8_t tile_element_raise_styles[9][32] = {
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
static constexpr const uint8_t tile_element_lower_styles[9][32] = {
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

int32_t tile_element_get_corner_height(const TileElement* tileElement, int32_t direction)
{
    int32_t z = tileElement->base_height;
    int32_t slope = tileElement->AsSurface()->GetSlope();
    return map_get_corner_height(z, slope, direction);
}

static money32 map_set_land_ownership(uint8_t flags, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t newOwnership)
{
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LAND_PURCHASE;

    if (!(flags & GAME_COMMAND_FLAG_APPLY))
        return 0;

    // Clamp to maximum addressable element to prevent long loop spamming the log
    x1 = std::clamp<int16_t>(x1, 32, gMapSizeUnits - 32);
    y1 = std::clamp<int16_t>(y1, 32, gMapSizeUnits - 32);
    x2 = std::clamp<int16_t>(x2, 32, gMapSizeUnits - 32);
    y2 = std::clamp<int16_t>(y2, 32, gMapSizeUnits - 32);
    gMapLandRightsUpdateSuccess = false;
    map_buy_land_rights(x1, y1, x2, y2, BUY_LAND_RIGHTS_FLAG_SET_OWNERSHIP_WITH_CHECKS, flags | (newOwnership << 8));

    if (!gMapLandRightsUpdateSuccess)
        return 0;

    int16_t x = std::clamp<int16_t>(x1, 32, gMapSizeUnits - 32);
    int16_t y = std::clamp<int16_t>(y1, 32, gMapSizeUnits - 32);

    x += 16;
    y += 16;

    int16_t z = tile_element_height(x, y) & 0xFFFF;
    audio_play_sound_at_location(SOUND_PLACE_ITEM, x, y, z);
    return 0;
}

/**
 *
 *  rct2: 0x006648E3
 */
void game_command_set_land_ownership(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi, int32_t* edi, int32_t* ebp)
{
    int32_t flags = *ebx & 0xFF;

    *ebx = map_set_land_ownership(flags, *eax & 0xFFFF, *ecx & 0xFFFF, *edi & 0xFFFF, *ebp & 0xFFFF, *edx & 0xFF);

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        map_count_remaining_land_rights();
    }
}

static uint8_t map_get_lowest_land_height(int32_t xMin, int32_t xMax, int32_t yMin, int32_t yMax)
{
    xMin = std::max(xMin, 32);
    yMin = std::max(yMin, 32);
    xMax = std::min(xMax, (int32_t)gMapSizeMaxXY);
    yMax = std::min(yMax, (int32_t)gMapSizeMaxXY);

    uint8_t min_height = 0xFF;
    for (int32_t yi = yMin; yi <= yMax; yi += 32)
    {
        for (int32_t xi = xMin; xi <= xMax; xi += 32)
        {
            TileElement* tile_element = map_get_surface_element_at({ xi, yi });
            if (tile_element != nullptr && min_height > tile_element->base_height)
            {
                min_height = tile_element->base_height;
            }
        }
    }
    return min_height;
}

static uint8_t map_get_highest_land_height(int32_t xMin, int32_t xMax, int32_t yMin, int32_t yMax)
{
    xMin = std::max(xMin, 32);
    yMin = std::max(yMin, 32);
    xMax = std::min(xMax, (int32_t)gMapSizeMaxXY);
    yMax = std::min(yMax, (int32_t)gMapSizeMaxXY);

    uint8_t max_height = 0;
    for (int32_t yi = yMin; yi <= yMax; yi += 32)
    {
        for (int32_t xi = xMin; xi <= xMax; xi += 32)
        {
            TileElement* tile_element = map_get_surface_element_at({ xi, yi });
            if (tile_element != nullptr)
            {
                uint8_t base_height = tile_element->base_height;
                if (tile_element->AsSurface()->GetSlope() & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
                    base_height += 2;
                if (tile_element->AsSurface()->GetSlope() & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
                    base_height += 2;
                if (max_height < base_height)
                    max_height = base_height;
            }
        }
    }
    return max_height;
}

static money32 raise_land(
    int32_t flags, int32_t x, int32_t y, int32_t z, int32_t point_a_x, int32_t point_a_y, int32_t point_b_x, int32_t point_b_y,
    int32_t selectionType)
{
    money32 cost = 0;
    size_t tableRow = selectionType;

    // The selections between MAP_SELECT_TYPE_FULL and MAP_SELECT_TYPE_EDGE_0 are not included in the tables
    if (selectionType >= MAP_SELECT_TYPE_EDGE_0 && selectionType <= MAP_SELECT_TYPE_EDGE_3)
        tableRow -= MAP_SELECT_TYPE_EDGE_0 - MAP_SELECT_TYPE_FULL - 1;

    if ((flags & GAME_COMMAND_FLAG_APPLY) && gGameCommandNestLevel == 1)
    {
        audio_play_sound_at_location(SOUND_PLACE_ITEM, x, y, z);
    }

    // Keep big coordinates within map boundaries
    point_a_x = std::max<decltype(point_a_x)>(32, point_a_x);
    point_b_x = std::min<decltype(point_b_x)>(gMapSizeMaxXY, point_b_x);
    point_a_y = std::max<decltype(point_a_y)>(32, point_a_y);
    point_b_y = std::min<decltype(point_b_y)>(gMapSizeMaxXY, point_b_y);

    uint8_t min_height = map_get_lowest_land_height(point_a_x, point_b_x, point_a_y, point_b_y);

    for (int32_t y_coord = point_a_y; y_coord <= point_b_y; y_coord += 32)
    {
        for (int32_t x_coord = point_a_x; x_coord <= point_b_x; x_coord += 32)
        {
            TileElement* tile_element = map_get_surface_element_at(x_coord / 32, y_coord / 32);
            if (tile_element != nullptr)
            {
                uint8_t height = tile_element->base_height;
                if (height <= min_height)
                {
                    uint8_t raisedCorners = tile_element->AsSurface()->GetSlope();
                    uint8_t slope = tile_element_raise_styles[tableRow][raisedCorners];

                    if (slope & SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT)
                        height += 2;

                    slope &= TILE_ELEMENT_SURFACE_SLOPE_MASK;
                    auto landSetHeightAction = LandSetHeightAction({ x_coord, y_coord }, height, slope);
                    auto res = (flags & GAME_COMMAND_FLAG_APPLY) ? landSetHeightAction.Execute() : landSetHeightAction.Query();

                    if (res->Error != GA_ERROR::OK)
                    {
                        return MONEY32_UNDEFINED;
                    }
                    cost += res->Cost;
                }
            }
        }
    }

    // Force ride construction to recheck area
    _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_RECHECK;

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
    gCommandPosition.x = x;
    gCommandPosition.y = y;
    gCommandPosition.z = z;
    return cost;
}

static money32 lower_land(
    int32_t flags, int32_t x, int32_t y, int32_t z, int32_t point_a_x, int32_t point_a_y, int32_t point_b_x, int32_t point_b_y,
    int32_t selectionType)
{
    money32 cost = 0;
    size_t tableRow = selectionType;

    // The selections between MAP_SELECT_TYPE_FULL and MAP_SELECT_TYPE_EDGE_0 are not included in the tables
    if (selectionType >= MAP_SELECT_TYPE_EDGE_0 && selectionType <= MAP_SELECT_TYPE_EDGE_3)
        tableRow -= MAP_SELECT_TYPE_EDGE_0 - MAP_SELECT_TYPE_FULL - 1;

    // Keep big coordinates within map boundaries
    point_a_x = std::max<decltype(point_a_x)>(32, point_a_x);
    point_b_x = std::min<decltype(point_b_x)>(gMapSizeMaxXY, point_b_x);
    point_a_y = std::max<decltype(point_a_y)>(32, point_a_y);
    point_b_y = std::min<decltype(point_b_y)>(gMapSizeMaxXY, point_b_y);

    if ((flags & GAME_COMMAND_FLAG_APPLY) && gGameCommandNestLevel == 1)
    {
        audio_play_sound_at_location(SOUND_PLACE_ITEM, x, y, z);
    }

    uint8_t max_height = map_get_highest_land_height(point_a_x, point_b_x, point_a_y, point_b_y);

    for (int32_t y_coord = point_a_y; y_coord <= point_b_y; y_coord += 32)
    {
        for (int32_t x_coord = point_a_x; x_coord <= point_b_x; x_coord += 32)
        {
            TileElement* tile_element = map_get_surface_element_at(x_coord / 32, y_coord / 32);
            if (tile_element != nullptr)
            {
                uint8_t height = tile_element->base_height;
                if (tile_element->AsSurface()->GetSlope() & TILE_ELEMENT_SURFACE_RAISED_CORNERS_MASK)
                    height += 2;
                if (tile_element->AsSurface()->GetSlope() & TILE_ELEMENT_SURFACE_DIAGONAL_FLAG)
                    height += 2;

                if (height >= max_height)
                {
                    height = tile_element->base_height;
                    uint8_t currentSlope = tile_element->AsSurface()->GetSlope();
                    uint8_t newSlope = tile_element_lower_styles[tableRow][currentSlope];
                    if (newSlope & SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT)
                        height -= 2;

                    newSlope &= TILE_ELEMENT_SURFACE_SLOPE_MASK;

                    auto landSetHeightAction = LandSetHeightAction({ x_coord, y_coord }, height, newSlope);
                    landSetHeightAction.SetFlags(flags);

                    auto res = (flags & GAME_COMMAND_FLAG_APPLY) ? GameActions::ExecuteNested(&landSetHeightAction)
                                                                 : GameActions::QueryNested(&landSetHeightAction);
                    if (res->Error != GA_ERROR::OK)
                    {
                        return MONEY32_UNDEFINED;
                    }
                    cost += res->Cost;
                }
            }
        }
    }

    // Force ride construction to recheck area
    _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_RECHECK;

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
    gCommandPosition.x = x;
    gCommandPosition.y = y;
    gCommandPosition.z = z;
    return cost;
}

money32 raise_water(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t flags)
{
    money32 cost = 0;
    bool waterHeightChanged = false;

    uint8_t max_height = 0xFF;

    x0 = std::max(x0, (int16_t)32);
    y0 = std::max(y0, (int16_t)32);
    x1 = std::min(x1, gMapSizeMaxXY);
    y1 = std::min(y1, gMapSizeMaxXY);

    for (int32_t yi = y0; yi <= y1; yi += 32)
    {
        for (int32_t xi = x0; xi <= x1; xi += 32)
        {
            TileElement* tile_element = map_get_surface_element_at({ xi, yi });
            if (tile_element != nullptr)
            {
                uint8_t height = tile_element->base_height;
                if (tile_element->AsSurface()->GetWaterHeight() > 0)
                    height = tile_element->AsSurface()->GetWaterHeight() * 2;
                if (max_height > height)
                    max_height = height;
            }
        }
    }

    for (int32_t yi = y0; yi <= y1; yi += 32)
    {
        for (int32_t xi = x0; xi <= x1; xi += 32)
        {
            TileElement* tile_element = map_get_surface_element_at({ xi, yi });
            if (tile_element != nullptr)
            {
                if (tile_element->base_height <= max_height)
                {
                    uint8_t height = tile_element->AsSurface()->GetWaterHeight();
                    if (height != 0)
                    {
                        height *= 2;
                        if (height > max_height)
                            continue;
                        height += 2;
                    }
                    else
                    {
                        height = tile_element->base_height + 2;
                    }

                    auto waterSetHeightAction = WaterSetHeightAction({ xi, yi }, height);
                    waterSetHeightAction.SetFlags(flags);
                    auto res = flags & GAME_COMMAND_FLAG_APPLY ? GameActions::ExecuteNested(&waterSetHeightAction)
                                                               : GameActions::QueryNested(&waterSetHeightAction);
                    if (res->Error != GA_ERROR::OK)
                    {
                        gGameCommandErrorText = res->ErrorMessage;
                        // set gCommonFormatArguments to res->ErrorArgs
                        return MONEY32_UNDEFINED;
                    }

                    cost += res->Cost;
                    waterHeightChanged = true;
                }
            }
        }
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        int32_t x = ((x0 + x1) / 2) + 16;
        int32_t y = ((y0 + y1) / 2) + 16;
        int32_t z = tile_element_height(x, y);
        int16_t water_height_z = z >> 16;
        int16_t base_height_z = z;
        z = water_height_z;
        if (z != 0)
            z = base_height_z;

        LocationXYZ16 coord;
        coord.x = x;
        coord.y = y;
        coord.z = z;
        network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);

        gCommandPosition.x = x;
        gCommandPosition.y = y;
        gCommandPosition.z = z;
        if (waterHeightChanged)
        {
            audio_play_sound_at_location(SOUND_LAYING_OUT_WATER, x, y, z);
        }
    }

    // Force ride construction to recheck area
    _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_RECHECK;

    return cost;
}

money32 lower_water(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t flags)
{
    money32 cost = 0;
    bool waterHeightChanged = false;

    uint8_t min_height = 0;

    x0 = std::max(x0, (int16_t)32);
    y0 = std::max(y0, (int16_t)32);
    x1 = std::min(x1, gMapSizeMaxXY);
    y1 = std::min(y1, gMapSizeMaxXY);

    for (int32_t yi = y0; yi <= y1; yi += 32)
    {
        for (int32_t xi = x0; xi <= x1; xi += 32)
        {
            TileElement* tile_element = map_get_surface_element_at({ xi, yi });
            if (tile_element != nullptr)
            {
                uint8_t height = tile_element->AsSurface()->GetWaterHeight();
                if (height != 0)
                {
                    height *= 2;
                    if (height > min_height)
                        min_height = height;
                }
            }
        }
    }

    for (int32_t yi = y0; yi <= y1; yi += 32)
    {
        for (int32_t xi = x0; xi <= x1; xi += 32)
        {
            TileElement* tile_element = map_get_surface_element_at({ xi, yi });
            if (tile_element != nullptr)
            {
                uint8_t height = tile_element->AsSurface()->GetWaterHeight();
                if (height != 0)
                {
                    height *= 2;
                    if (height < min_height)
                        continue;
                    height -= 2;
                    auto waterSetHeightAction = WaterSetHeightAction({ xi, yi }, height);
                    waterSetHeightAction.SetFlags(flags);
                    auto res = flags & GAME_COMMAND_FLAG_APPLY ? GameActions::ExecuteNested(&waterSetHeightAction)
                                                               : GameActions::QueryNested(&waterSetHeightAction);
                    if (res->Error != GA_ERROR::OK)
                    {
                        gGameCommandErrorText = res->ErrorMessage;
                        // set gCommonFormatArguments to res->ErrorArgs
                        return MONEY32_UNDEFINED;
                    }

                    cost += res->Cost;
                    waterHeightChanged = true;
                }
            }
        }
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        int32_t x = ((x0 + x1) / 2) + 16;
        int32_t y = ((y0 + y1) / 2) + 16;
        int32_t z = tile_element_height(x, y);
        int16_t water_height_z = z >> 16;
        int16_t base_height_z = z;
        z = water_height_z;
        if (z == 0)
            z = base_height_z;

        LocationXYZ16 coord;
        coord.x = x;
        coord.y = y;
        coord.z = z;
        network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);

        gCommandPosition.x = x;
        gCommandPosition.y = y;
        gCommandPosition.z = z;
        if (waterHeightChanged)
        {
            audio_play_sound_at_location(SOUND_LAYING_OUT_WATER, x, y, z);
        }
    }

    // Force ride construction to recheck area
    _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_RECHECK;

    return cost;
}

/**
 *
 *  rct2: 0x0068C542
 */
void game_command_raise_land(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi, int32_t* edi, int32_t* ebp)
{
    *ebx = raise_land(
        *ebx, *eax, *ecx, tile_element_height(*eax, *ecx), (int16_t)(*edx & 0xFFFF), (int16_t)(*ebp & 0xFFFF), *edx >> 16,
        *ebp >> 16, *edi & 0xFFFF);
}

/**
 *
 *  rct2: 0x0068C6D1
 */
void game_command_lower_land(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi, int32_t* edi, int32_t* ebp)
{
    *ebx = lower_land(
        *ebx, *eax, *ecx, tile_element_height(*eax, *ecx), (int16_t)(*edx & 0xFFFF), (int16_t)(*ebp & 0xFFFF), *edx >> 16,
        *ebp >> 16, *edi & 0xFFFF);
}

static money32 smooth_land_tile(
    int32_t direction, uint8_t flags, int32_t x, int32_t y, TileElement* tileElement, bool raiseLand)
{
    int32_t targetBaseZ = tileElement->base_height;
    int32_t slope = tileElement->AsSurface()->GetSlope();
    if (raiseLand)
    {
        slope = tile_element_raise_styles[direction][slope];
        if (slope & SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT)
        {
            targetBaseZ += 2;
            slope &= ~SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT;
        }
    }
    else
    {
        slope = tile_element_lower_styles[direction][slope];
        if (slope & SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT)
        {
            targetBaseZ -= 2;
            slope &= ~SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT;
        }
    }

    auto landSetHeightAction = LandSetHeightAction({ x, y }, targetBaseZ, slope);
    landSetHeightAction.SetFlags(flags);
    auto res = (flags & GAME_COMMAND_FLAG_APPLY) ? GameActions::ExecuteNested(&landSetHeightAction)
                                                 : GameActions::QueryNested(&landSetHeightAction);

    if (res->Error == GA_ERROR::OK)
    {
        return res->Cost;
    }
    else
    {
        return MONEY32_UNDEFINED;
    }
}

static money32 smooth_land_row_by_edge(
    int32_t flags, int32_t x, int32_t y, int32_t expectedLandHeight1, int32_t expectedLandHeight2, int32_t stepX, int32_t stepY,
    int32_t direction1, int32_t direction2, int32_t checkDirection1, int32_t checkDirection2, bool raiseLand)
{
    uint8_t shouldContinue = 0xF;
    int32_t landChangePerTile = raiseLand ? -2 : 2;
    TileElement *tileElement, *nextTileElement;
    money32 totalCost = 0;

    // check if we need to start at all
    if (!map_is_location_valid({ x, y }) || !map_is_location_valid({ x + stepX, y + stepY }))
    {
        return 0;
    }
    tileElement = map_get_surface_element_at({ x, y });
    nextTileElement = map_get_surface_element_at({ x + stepX, y + stepY });
    if (tileElement == nullptr || nextTileElement == nullptr)
    {
        return 0;
    }
    if (tile_element_get_corner_height(tileElement, checkDirection1) != expectedLandHeight1 + (raiseLand ? -2 : 2))
    {
        shouldContinue &= ~0x1;
    }
    if (tile_element_get_corner_height(tileElement, checkDirection2) != expectedLandHeight2 + (raiseLand ? -2 : 2))
    {
        shouldContinue &= ~0x2;
    }
    if (tile_element_get_corner_height(tileElement, checkDirection1)
        != tile_element_get_corner_height(nextTileElement, direction1))
    {
        shouldContinue &= ~0x1;
    }
    if (tile_element_get_corner_height(tileElement, checkDirection2)
        != tile_element_get_corner_height(nextTileElement, direction2))
    {
        shouldContinue &= ~0x2;
    }
    while ((shouldContinue & 0x3) != 0)
    {
        shouldContinue = ((shouldContinue << 2) | 0x3) & shouldContinue;
        x += stepX;
        y += stepY;
        // check if we need to continue after raising the current tile
        // this needs to be checked before the tile is changed
        if (!map_is_location_valid({ x + stepX, y + stepY }))
        {
            shouldContinue &= ~0x3;
        }
        else
        {
            tileElement = nextTileElement;
            nextTileElement = map_get_surface_element_at({ x + stepX, y + stepY });
            if (nextTileElement == nullptr)
            {
                shouldContinue &= ~0x3;
            }
            if (tile_element_get_corner_height(tileElement, direction1) + landChangePerTile
                != tile_element_get_corner_height(tileElement, checkDirection1))
            {
                shouldContinue &= ~0x1;
            }
            if (tile_element_get_corner_height(tileElement, direction2) + landChangePerTile
                != tile_element_get_corner_height(tileElement, checkDirection2))
            {
                shouldContinue &= ~0x2;
            }
            if ((shouldContinue & 0x1)
                && tile_element_get_corner_height(tileElement, checkDirection1)
                    != tile_element_get_corner_height(nextTileElement, direction1))
            {
                shouldContinue &= ~0x1;
            }
            if ((shouldContinue & 0x2)
                && tile_element_get_corner_height(tileElement, checkDirection2)
                    != tile_element_get_corner_height(nextTileElement, direction2))
            {
                shouldContinue &= ~0x2;
            }
        }
        expectedLandHeight1 += landChangePerTile;

        // change land of current tile
        int32_t targetBaseZ = tileElement->base_height;
        int32_t slope = tileElement->AsSurface()->GetSlope();
        int32_t oldSlope = slope;
        if (raiseLand)
        {
            if (shouldContinue & 0x4)
            {
                slope = tile_element_raise_styles[direction1][slope];
                if (slope & SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT)
                {
                    targetBaseZ += 2;
                    slope &= ~SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT;
                }
            }
            if ((shouldContinue & 0x8)
                && map_get_corner_height(tileElement->base_height, oldSlope, direction2)
                    == map_get_corner_height(targetBaseZ, slope, direction2))
            {
                slope = tile_element_raise_styles[direction2][slope];
                if (slope & SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT)
                {
                    targetBaseZ += 2;
                    slope &= ~SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT;
                }
            }
        }
        else
        {
            if (shouldContinue & 0x4)
            {
                slope = tile_element_lower_styles[direction1][slope];
                if (slope & SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT)
                {
                    targetBaseZ -= 2;
                    slope &= ~SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT;
                }
            }
            if ((shouldContinue & 0x8)
                && map_get_corner_height(tileElement->base_height, oldSlope, direction2)
                    == map_get_corner_height(targetBaseZ, slope, direction2))
            {
                slope = tile_element_lower_styles[direction2][slope];
                if (slope & SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT)
                {
                    targetBaseZ -= 2;
                    slope &= ~SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT;
                }
            }
        }
        auto landSetHeightAction = LandSetHeightAction({ x, y }, targetBaseZ, slope);
        landSetHeightAction.SetFlags(flags);
        auto res = (flags & GAME_COMMAND_FLAG_APPLY) ? GameActions::ExecuteNested(&landSetHeightAction)
                                                     : GameActions::QueryNested(&landSetHeightAction);
        if (res->Error == GA_ERROR::OK)
        {
            totalCost += res->Cost;
        }
    }
    return totalCost;
}

static money32 smooth_land_row_by_corner(
    int32_t flags, int32_t x, int32_t y, int32_t expectedLandHeight, int32_t stepX, int32_t stepY, int32_t direction,
    int32_t checkDirection, bool raiseLand)
{
    bool shouldContinue = true;
    TileElement *tileElement, *nextTileElement;
    money32 totalCost = 0;
    money32 result;
    int32_t landChangePerTile;
    if (stepX == 0 || stepY == 0)
    {
        landChangePerTile = raiseLand ? -2 : 2;
    }
    else
    {
        landChangePerTile = raiseLand ? -4 : 4;
    }

    // check if we need to start at all
    if (!map_is_location_valid({ x, y }) || !map_is_location_valid({ x + stepX, y + stepY }))
    {
        return 0;
    }
    tileElement = map_get_surface_element_at({ x, y });
    nextTileElement = map_get_surface_element_at((x + stepX) >> 5, (y + stepY) >> 5);
    if (tileElement == nullptr || nextTileElement == nullptr)
    {
        return 0;
    }
    if (tile_element_get_corner_height(tileElement, checkDirection) != expectedLandHeight + (raiseLand ? -2 : 2))
    {
        return 0;
    }
    if (tile_element_get_corner_height(tileElement, checkDirection)
        != tile_element_get_corner_height(nextTileElement, direction))
    {
        return 0;
    }
    while (shouldContinue)
    {
        x += stepX;
        y += stepY;
        // check if we need to continue after raising the current tile
        // this needs to be checked before the tile is changed
        if (!map_is_location_valid({ x + stepX, y + stepY }))
        {
            shouldContinue = false;
        }
        else
        {
            tileElement = nextTileElement;
            nextTileElement = map_get_surface_element_at((x + stepX) >> 5, (y + stepY) >> 5);
            if (nextTileElement == nullptr)
            {
                shouldContinue = false;
            }
            if (tile_element_get_corner_height(tileElement, direction) + landChangePerTile
                != tile_element_get_corner_height(tileElement, checkDirection))
            {
                shouldContinue = false;
            }
            if (shouldContinue
                && tile_element_get_corner_height(tileElement, checkDirection)
                    != tile_element_get_corner_height(nextTileElement, direction))
            {
                shouldContinue = false;
            }
        }
        if (stepX * stepY != 0)
        {
            totalCost += smooth_land_row_by_corner(
                flags, x, y, expectedLandHeight + (landChangePerTile / 2), 0, stepY, direction, checkDirection ^ 3, raiseLand);
            totalCost += smooth_land_row_by_corner(
                flags, x, y, expectedLandHeight + (landChangePerTile / 2), stepX, 0, direction, checkDirection ^ 1, raiseLand);
        }
        expectedLandHeight += landChangePerTile;
        // change land of current tile
        result = smooth_land_tile(direction, flags, x, y, tileElement, raiseLand);
        if (result != MONEY32_UNDEFINED)
        {
            totalCost += result;
        }
    }
    return totalCost;
}

static money32 smooth_land(
    int32_t flags, int32_t centreX, int32_t centreY, int32_t mapLeft, int32_t mapTop, int32_t mapRight, int32_t mapBottom,
    int32_t command)
{
    // break up information in command
    const bool raiseLand = command < 0x7FFF;
    const int32_t selectionType = command & 0x7FFF;
    const int32_t heightOffset = raiseLand ? 2 : -2;

    // Cap bounds to map
    mapLeft = std::max(mapLeft, 32);
    mapTop = std::max(mapTop, 32);
    mapRight = std::clamp(mapRight, 0, (MAXIMUM_MAP_SIZE_TECHNICAL - 1) * 32);
    mapBottom = std::clamp(mapBottom, 0, (MAXIMUM_MAP_SIZE_TECHNICAL - 1) * 32);

    // Play sound (only once)
    int32_t centreZ = tile_element_height(centreX, centreY);
    if ((flags & GAME_COMMAND_FLAG_APPLY) && gGameCommandNestLevel == 1)
    {
        audio_play_sound_at_location(SOUND_PLACE_ITEM, centreX, centreY, centreZ);
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        LocationXYZ16 coord;
        coord.x = centreX + 16;
        coord.y = centreY + 16;
        coord.z = tile_element_height(coord.x, coord.y);
        network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
    }

    // Do the smoothing
    money32 totalCost = 0;
    switch (selectionType)
    {
        case MAP_SELECT_TYPE_FULL:
        {
            uint8_t minHeight = heightOffset + map_get_lowest_land_height(mapLeft, mapRight, mapTop, mapBottom);
            uint8_t maxHeight = heightOffset + map_get_highest_land_height(mapLeft, mapRight, mapTop, mapBottom);

            // Smooth the 4 corners
            { // top-left
                TileElement* tileElement = map_get_surface_element_at({ mapLeft, mapTop });
                int32_t z = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 2), minHeight, maxHeight);
                totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, -32, -32, 0, 2, raiseLand);
            }
            { // bottom-left
                TileElement* tileElement = map_get_surface_element_at(mapLeft >> 5, mapBottom >> 5);
                int32_t z = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 3), minHeight, maxHeight);
                totalCost += smooth_land_row_by_corner(flags, mapLeft, mapBottom, z, -32, 32, 1, 3, raiseLand);
            }
            { // bottom-right
                TileElement* tileElement = map_get_surface_element_at(mapRight >> 5, mapBottom >> 5);
                int32_t z = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 0), minHeight, maxHeight);
                totalCost += smooth_land_row_by_corner(flags, mapRight, mapBottom, z, 32, 32, 2, 0, raiseLand);
            }
            { // top-right
                TileElement* tileElement = map_get_surface_element_at(mapRight >> 5, mapTop >> 5);
                int32_t z = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 1), minHeight, maxHeight);
                totalCost += smooth_land_row_by_corner(flags, mapRight, mapTop, z, 32, -32, 3, 1, raiseLand);
            }

            // Smooth the edges
            TileElement* tileElement = nullptr;
            int32_t z1, z2;
            for (int32_t y = mapTop; y <= mapBottom; y += 32)
            {
                tileElement = map_get_surface_element_at({ mapLeft, y });
                z1 = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 3), minHeight, maxHeight);
                z2 = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 2), minHeight, maxHeight);
                totalCost += smooth_land_row_by_edge(flags, mapLeft, y, z1, z2, -32, 0, 0, 1, 3, 2, raiseLand);

                tileElement = map_get_surface_element_at({ mapRight, y });
                z1 = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 1), minHeight, maxHeight);
                z2 = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 0), minHeight, maxHeight);
                totalCost += smooth_land_row_by_edge(flags, mapRight, y, z1, z2, 32, 0, 2, 3, 1, 0, raiseLand);
            }

            for (int32_t x = mapLeft; x <= mapRight; x += 32)
            {
                tileElement = map_get_surface_element_at({ x, mapTop });
                z1 = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 1), minHeight, maxHeight);
                z2 = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 2), minHeight, maxHeight);
                totalCost += smooth_land_row_by_edge(flags, x, mapTop, z1, z2, 0, -32, 0, 3, 1, 2, raiseLand);

                tileElement = map_get_surface_element_at({ x, mapBottom });
                z1 = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 0), minHeight, maxHeight);
                z2 = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 3), minHeight, maxHeight);
                totalCost += smooth_land_row_by_edge(flags, x, mapBottom, z1, z2, 0, 32, 1, 2, 0, 3, raiseLand);
            }
            break;
        }
        case MAP_SELECT_TYPE_CORNER_0:
        case MAP_SELECT_TYPE_CORNER_1:
        case MAP_SELECT_TYPE_CORNER_2:
        case MAP_SELECT_TYPE_CORNER_3:
        {
            TileElement* tileElement = map_get_surface_element_at({ mapLeft, mapTop });
            uint8_t newBaseZ = tileElement->base_height;
            uint8_t newSlope = tileElement->AsSurface()->GetSlope();

            if (raiseLand)
            {
                newSlope = tile_element_raise_styles[selectionType][newSlope];
            }
            else
            {
                newSlope = tile_element_lower_styles[selectionType][newSlope];
            }

            if (newSlope & SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT)
            {
                newBaseZ += heightOffset;
                newSlope &= ~SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT;
            }

            // Smooth the corners
            int32_t z = map_get_corner_height(newBaseZ, newSlope, 2);
            totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, -32, -32, 0, 2, raiseLand);
            z = map_get_corner_height(newBaseZ, newSlope, 0);
            totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, 32, 32, 2, 0, raiseLand);
            z = map_get_corner_height(newBaseZ, newSlope, 3);
            totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, -32, 32, 1, 3, raiseLand);
            z = map_get_corner_height(newBaseZ, newSlope, 1);
            totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, 32, -32, 3, 1, raiseLand);

            // Smooth the edges
            switch (selectionType)
            {
                case MAP_SELECT_TYPE_CORNER_0:
                    z = map_get_corner_height(newBaseZ, newSlope, 0);
                    totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, 32, 0, 3, 0, raiseLand);
                    totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, 0, 32, 1, 0, raiseLand);
                    z = map_get_corner_height(newBaseZ, newSlope, 3);
                    totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, -32, 0, 0, 3, raiseLand);
                    z = map_get_corner_height(newBaseZ, newSlope, 1);
                    totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, 0, -32, 0, 1, raiseLand);
                    break;
                case MAP_SELECT_TYPE_CORNER_1:
                    z = map_get_corner_height(newBaseZ, newSlope, 1);
                    totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, 32, 0, 2, 1, raiseLand);
                    totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, 0, -32, 0, 1, raiseLand);
                    z = map_get_corner_height(newBaseZ, newSlope, 2);
                    totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, -32, 0, 1, 2, raiseLand);
                    z = map_get_corner_height(newBaseZ, newSlope, 0);
                    totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, 0, 32, 1, 0, raiseLand);
                    break;
                case MAP_SELECT_TYPE_CORNER_2:
                    z = map_get_corner_height(newBaseZ, newSlope, 2);
                    totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, -32, 0, 1, 2, raiseLand);
                    totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, 0, -32, 3, 2, raiseLand);
                    z = map_get_corner_height(newBaseZ, newSlope, 1);
                    totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, 32, 0, 2, 1, raiseLand);
                    z = map_get_corner_height(newBaseZ, newSlope, 3);
                    totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, 0, 32, 2, 3, raiseLand);
                    break;
                case MAP_SELECT_TYPE_CORNER_3:
                    z = map_get_corner_height(newBaseZ, newSlope, 3);
                    totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, -32, 0, 0, 3, raiseLand);
                    totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, 0, 32, 2, 3, raiseLand);
                    z = map_get_corner_height(newBaseZ, newSlope, 0);
                    totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, 32, 0, 3, 0, raiseLand);
                    z = map_get_corner_height(newBaseZ, newSlope, 2);
                    totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, 0, -32, 3, 2, raiseLand);
                    break;
            }
            break;
        }
        case MAP_SELECT_TYPE_EDGE_0:
        case MAP_SELECT_TYPE_EDGE_1:
        case MAP_SELECT_TYPE_EDGE_2:
        case MAP_SELECT_TYPE_EDGE_3:
        {
            // TODO: Handle smoothing by edge
            // Get the two corners to raise
            TileElement* surfaceElement = map_get_surface_element_at({ mapLeft, mapTop });
            uint8_t newBaseZ = surfaceElement->base_height;
            uint8_t oldSlope = surfaceElement->AsSurface()->GetSlope();
            uint8_t newSlope = oldSlope;
            int32_t rowIndex = selectionType - (MAP_SELECT_TYPE_EDGE_0 - MAP_SELECT_TYPE_FULL - 1);

            if (raiseLand)
            {
                newSlope = tile_element_raise_styles[rowIndex][oldSlope];
            }
            else
            {
                newSlope = tile_element_lower_styles[rowIndex][oldSlope];
            }

            const bool changeBaseHeight = newSlope & SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT;
            if (changeBaseHeight)
            {
                newBaseZ += heightOffset;
                newSlope &= ~SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT;
            }

            const uint8_t edge = selectionType - MAP_SELECT_TYPE_EDGE_0;

            // Table with corners for each edge selection. The first two are the selected corners, the latter two are the
            // opposites
            static constexpr uint8_t cornerIndices[][4] = {
                { 2, 3, 1, 0 }, // MAP_SELECT_TYPE_EDGE_0
                { 3, 0, 2, 1 }, // MAP_SELECT_TYPE_EDGE_1
                { 0, 1, 3, 2 }, // MAP_SELECT_TYPE_EDGE_2
                { 1, 2, 0, 3 }, // MAP_SELECT_TYPE_EDGE_3
            };
            // Big coordinate offsets for the neigbouring tile for the given edge selection
            static constexpr sLocationXY8 stepOffsets[] = {
                { -32, 0 },
                { 0, 32 },
                { 32, 0 },
                { 0, -32 },
            };

            // Smooth higher and lower edges
            uint8_t c1 = cornerIndices[edge][0];
            uint8_t c2 = cornerIndices[edge][1];
            uint8_t c3 = cornerIndices[edge][2];
            uint8_t c4 = cornerIndices[edge][3];
            uint8_t z1 = map_get_corner_height(newBaseZ, newSlope, c1);
            uint8_t z2 = map_get_corner_height(newBaseZ, newSlope, c2);
            uint8_t z3 = map_get_corner_height(newBaseZ, newSlope, c3);
            uint8_t z4 = map_get_corner_height(newBaseZ, newSlope, c4);
            // Smooth the edge at the top of the new slope
            totalCost += smooth_land_row_by_edge(
                flags, mapLeft, mapTop, z1, z2, stepOffsets[edge].x, stepOffsets[edge].y, c3, c4, c1, c2, raiseLand);
            // Smooth the edge at the bottom of the new slope
            totalCost += smooth_land_row_by_edge(
                flags, mapLeft, mapTop, z3, z4, -stepOffsets[edge].x, -stepOffsets[edge].y, c1, c2, c3, c4, raiseLand);

            // Smooth corners
            totalCost += smooth_land_row_by_corner(
                flags, mapLeft, mapTop, z1, -stepOffsets[edge].y, stepOffsets[edge].x, c2, c1, raiseLand);
            totalCost += smooth_land_row_by_corner(
                flags, mapLeft, mapTop, z2, stepOffsets[edge].y, -stepOffsets[edge].x, c1, c2, raiseLand);
            int32_t z = map_get_corner_height(newBaseZ, newSlope, 2);
            totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, -32, -32, 0, 2, raiseLand);
            z = map_get_corner_height(newBaseZ, newSlope, 0);
            totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, 32, 32, 2, 0, raiseLand);
            z = map_get_corner_height(newBaseZ, newSlope, 3);
            totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, -32, 32, 1, 3, raiseLand);
            z = map_get_corner_height(newBaseZ, newSlope, 1);
            totalCost += smooth_land_row_by_corner(flags, mapLeft, mapTop, z, 32, -32, 3, 1, raiseLand);
            break;
        }
    } // switch selectionType

    // Raise / lower the land tool selection area
    int32_t commandType = raiseLand ? GAME_COMMAND_RAISE_LAND : GAME_COMMAND_LOWER_LAND;
    int32_t mapLeftRight = mapLeft | (mapRight << 16);
    int32_t mapTopBottom = mapTop | (mapBottom << 16);
    money32 cost = game_do_command(centreX, flags, centreY, mapLeftRight, commandType, command & 0x7FFF, mapTopBottom);
    if (cost == MONEY32_UNDEFINED)
    {
        return MONEY32_UNDEFINED;
    }

    totalCost += cost;

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
    gCommandPosition.x = centreX;
    gCommandPosition.y = centreY;
    gCommandPosition.z = centreZ;
    return totalCost;
}

/**
 *
 *  rct2: 0x0068BC01
 */
void game_command_smooth_land(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi, int32_t* edi, int32_t* ebp)
{
    int32_t flags = *ebx & 0xFF;
    int32_t centreX = *eax & 0xFFFF;
    int32_t centreY = *ecx & 0xFFFF;
    int32_t mapLeft = (int16_t)(*edx & 0xFFFF);
    int32_t mapTop = (int16_t)(*ebp & 0xFFFF);
    int32_t mapRight = (int16_t)(*edx >> 16);
    int32_t mapBottom = (int16_t)(*ebp >> 16);
    int32_t command = *edi;
    *ebx = smooth_land(flags, centreX, centreY, mapLeft, mapTop, mapRight, mapBottom, command);
}

/**
 *
 *  rct2: 0x006E66A0
 */
void game_command_raise_water(
    int32_t* eax, int32_t* ebx, int32_t* ecx, [[maybe_unused]] int32_t* edx, [[maybe_unused]] int32_t* esi, int32_t* edi,
    int32_t* ebp)
{
    *ebx = raise_water(
        (int16_t)(*eax & 0xFFFF), (int16_t)(*ecx & 0xFFFF), (int16_t)(*edi & 0xFFFF), (int16_t)(*ebp & 0xFFFF), (uint8_t)*ebx);
}

/**
 *
 *  rct2: 0x006E6878
 */
void game_command_lower_water(
    int32_t* eax, int32_t* ebx, int32_t* ecx, [[maybe_unused]] int32_t* edx, [[maybe_unused]] int32_t* esi, int32_t* edi,
    int32_t* ebp)
{
    *ebx = lower_water(
        (int16_t)(*eax & 0xFFFF), (int16_t)(*ecx & 0xFFFF), (int16_t)(*edi & 0xFFFF), (int16_t)(*ebp & 0xFFFF), (uint8_t)*ebx);
}

bool map_is_location_at_edge(int32_t x, int32_t y)
{
    return x < 32 || y < 32 || x >= ((MAXIMUM_MAP_SIZE_TECHNICAL - 1) * 32) || y >= ((MAXIMUM_MAP_SIZE_TECHNICAL - 1) * 32);
}

/**
 *
 *  rct2: 0x006B893C
 */
void game_command_place_large_scenery(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi, int32_t* edi, int32_t* ebp)
{
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
    int32_t x = (int16_t)*eax;
    int32_t y = (int16_t)*ecx;
    int32_t z = (int16_t)*ebp;
    colour_t colour1 = *edx & TILE_ELEMENT_COLOUR_MASK;
    colour_t colour2 = (*edx >> 8) & TILE_ELEMENT_COLOUR_MASK;
    uint8_t flags = *ebx;
    uint8_t rotation = *ebx >> 8;
    uint8_t entry_index = *edi;
    int32_t base_height = tile_element_height(x, y);
    gCommandPosition.x = x + 16;
    gCommandPosition.y = y + 16;
    gCommandPosition.z = base_height;
    gSceneryGroundFlags = 0;
    BannerIndex banner_id = BANNER_INDEX_NULL;
    money32 supportsCost = 0;

    if (game_is_paused() && !gCheatsBuildInPauseMode)
    {
        gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
        *ebx = MONEY32_UNDEFINED;
        return;
    }

    if (entry_index >= 128)
    {
        log_warning("Invalid game command for scenery placement, entry_index = %u", entry_index);
        *ebx = MONEY32_UNDEFINED;
        return;
    }

    rct_scenery_entry* scenery_entry = get_large_scenery_entry(entry_index);
    if (scenery_entry == nullptr)
    {
        log_warning("Invalid game command for scenery placement, entry_index = %u", entry_index);
        *ebx = MONEY32_UNDEFINED;
        return;
    }

    if (scenery_entry->large_scenery.scrolling_mode != SCROLLING_MODE_NONE)
    {
        banner_id = create_new_banner(flags);

        if (banner_id == BANNER_INDEX_NULL)
        {
            *ebx = MONEY32_UNDEFINED;
            return;
        }

        if (flags & GAME_COMMAND_FLAG_APPLY)
        {
            rct_banner* banner = &gBanners[banner_id];
            banner->flags |= BANNER_FLAG_IS_LARGE_SCENERY;
            banner->type = 0;
            banner->x = x / 32;
            banner->y = y / 32;

            ride_id_t rideIndex = banner_get_closest_ride_index(x, y, z);
            if (rideIndex != RIDE_ID_NULL)
            {
                banner->ride_index = rideIndex;
                banner->flags |= BANNER_FLAG_LINKED_TO_RIDE;
            }
        }
    }

    uint32_t num_elements = 0;
    int16_t maxHeight = -1;
    for (rct_large_scenery_tile* tile = scenery_entry->large_scenery.tiles; tile->x_offset != -1; tile++)
    {
        num_elements++;

        LocationXY16 curTile = { tile->x_offset, tile->y_offset };

        rotate_map_coordinates(&curTile.x, &curTile.y, rotation);

        curTile.x += x;
        curTile.y += y;

        if (curTile.x >= 0x1FFF || curTile.y >= 0x1FFF || curTile.x < 0 || curTile.y < 0)
        {
            continue;
        }

        TileElement* tile_element = map_get_surface_element_at({ curTile.x, curTile.y });
        if (tile_element != nullptr)
        {
            int32_t height = tile_element->base_height * 8;
            int32_t slope = tile_element->AsSurface()->GetSlope();

            if (slope & 0xF)
            {
                height += 16;
                if (slope & 0x10)
                {
                    height += 16;
                }
            }

            if (height > maxHeight)
            {
                maxHeight = height;
            }
        }
    }

    if (z != 0)
    {
        maxHeight = z;
    }

    if (!map_check_free_elements_and_reorganise(num_elements))
    {
        *ebx = MONEY32_UNDEFINED;
        return;
    }

    gCommandPosition.z = maxHeight;
    uint8_t tile_num = 0;
    for (rct_large_scenery_tile* tile = scenery_entry->large_scenery.tiles; tile->x_offset != -1; tile++, tile_num++)
    {
        LocationXY16 curTile = { tile->x_offset, tile->y_offset };

        rotate_map_coordinates(&curTile.x, &curTile.y, rotation);

        curTile.x += x;
        curTile.y += y;

        int32_t zLow = (tile->z_offset + maxHeight) / 8;
        int32_t zHigh = (tile->z_clearance / 8) + zLow;

        QuarterTile quarterTile = QuarterTile{ static_cast<uint8_t>(tile->flags >> 12), 0 }.Rotate(rotation);
        if (!map_can_construct_with_clear_at(
                curTile.x, curTile.y, zLow, zHigh, &map_place_scenery_clear_func, quarterTile, flags, &supportsCost,
                CREATE_CROSSING_MODE_NONE))
        {
            *ebx = MONEY32_UNDEFINED;
            return;
        }

        if ((gMapGroundFlags & ELEMENT_IS_UNDERWATER) || (gMapGroundFlags & ELEMENT_IS_UNDERGROUND))
        {
            *ebx = MONEY32_UNDEFINED;
            return;
        }

        int32_t b = gMapGroundFlags & (ELEMENT_IS_ABOVE_GROUND | ELEMENT_IS_UNDERGROUND);
        if (!gCheatsDisableClearanceChecks)
        {
            if (gSceneryGroundFlags && !(gSceneryGroundFlags & b))
            {
                gGameCommandErrorText = STR_CANT_BUILD_PARTLY_ABOVE_AND_PARTLY_BELOW_GROUND;
                *ebx = MONEY32_UNDEFINED;
                return;
            }
        }
        gSceneryGroundFlags = b;

        if (curTile.x >= gMapSizeUnits || curTile.y >= gMapSizeUnits)
        {
            gGameCommandErrorText = STR_OFF_EDGE_OF_MAP;
            *ebx = MONEY32_UNDEFINED;
            return;
        }

        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !map_is_location_owned(curTile.x, curTile.y, zLow * 8)
            && !gCheatsSandboxMode)
        {
            *ebx = MONEY32_UNDEFINED;
            return;
        }

        if (flags & GAME_COMMAND_FLAG_APPLY)
        {
            if (!(flags & GAME_COMMAND_FLAG_GHOST))
            {
                footpath_remove_litter(curTile.x, curTile.y, zLow * 8);
                if (!gCheatsDisableClearanceChecks)
                {
                    wall_remove_at(curTile.x, curTile.y, zLow * 8, zHigh * 8);
                }
            }
            if (gGameCommandNestLevel == 1 && !(*ebx & GAME_COMMAND_FLAG_GHOST))
            {
                LocationXYZ16 coord;
                coord.x = x + 16;
                coord.y = y + 16;
                coord.z = tile_element_height(coord.x, coord.y);
                network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
            }

            TileElement* new_tile_element = tile_element_insert(
                curTile.x / 32, curTile.y / 32, zLow, quarterTile.GetBaseQuarterOccupied());
            assert(new_tile_element != nullptr);
            map_animation_create(MAP_ANIMATION_TYPE_LARGE_SCENERY, curTile.x, curTile.y, zLow);

            new_tile_element->clearance_height = zHigh;
            new_tile_element->SetType(TILE_ELEMENT_TYPE_LARGE_SCENERY);
            new_tile_element->SetDirection(rotation);

            auto newSceneryElement = new_tile_element->AsLargeScenery();
            newSceneryElement->SetEntryIndex(entry_index);
            newSceneryElement->SetSequenceIndex(tile_num);

            newSceneryElement->SetPrimaryColour(colour1);
            newSceneryElement->SetSecondaryColour(colour2);

            if (banner_id != BANNER_INDEX_NULL)
            {
                newSceneryElement->SetBannerIndex(banner_id);
            }

            if (flags & GAME_COMMAND_FLAG_GHOST)
            {
                new_tile_element->SetGhost(true);
            }

            if (tile_num == 0)
            {
                gSceneryTileElement = new_tile_element;
            }
            map_invalidate_tile_full(curTile.x, curTile.y);
        }
    }

    // Force ride construction to recheck area
    _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_RECHECK;

    *ebx = (scenery_entry->large_scenery.price * 10) + supportsCost;
    if (gParkFlags & PARK_FLAGS_NO_MONEY)
    {
        *ebx = 0;
    }
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
    (tileElement - 1)->flags |= TILE_ELEMENT_FLAG_LAST_TILE;
    tileElement->base_height = 0xFF;

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
                footpath_remove_edges_at(it.x * 32, it.y * 32, it.element);
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
    LocationXY16* position;

    if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT))
        return;

    for (position = gMapSelectionTiles; position->x != -1; position++)
        map_invalidate_tile_full(position->x, position->y);
}

void map_get_bounding_box(
    int32_t ax, int32_t ay, int32_t bx, int32_t by, int32_t* left, int32_t* top, int32_t* right, int32_t* bottom)
{
    int32_t x, y;
    x = ax;
    y = ay;
    uint32_t rotation = get_current_rotation();
    translate_3d_to_2d(rotation, &x, &y);
    *left = x;
    *right = x;
    *top = y;
    *bottom = y;
    x = bx;
    y = ay;
    translate_3d_to_2d(rotation, &x, &y);
    if (x < *left)
        *left = x;
    if (x > *right)
        *right = x;
    if (y > *bottom)
        *bottom = y;
    if (y < *top)
        *top = y;
    x = bx;
    y = by;
    translate_3d_to_2d(rotation, &x, &y);
    if (x < *left)
        *left = x;
    if (x > *right)
        *right = x;
    if (y > *bottom)
        *bottom = y;
    if (y < *top)
        *top = y;
    x = ax;
    y = by;
    translate_3d_to_2d(rotation, &x, &y);
    if (x < *left)
        *left = x;
    if (x > *right)
        *right = x;
    if (y > *bottom)
        *bottom = y;
    if (y < *top)
        *top = y;
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
    map_get_bounding_box(x0, y0, x1, y1, &left, &top, &right, &bottom);
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

    TileElement* new_tile_elements = (TileElement*)malloc(
        3 * (MAXIMUM_MAP_SIZE_TECHNICAL * MAXIMUM_MAP_SIZE_TECHNICAL) * sizeof(TileElement));
    TileElement* new_elements_pointer = new_tile_elements;

    if (new_tile_elements == nullptr)
    {
        log_fatal("Unable to allocate memory for map elements.");
        return;
    }

    uint32_t num_elements;

    for (int32_t y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; y++)
    {
        for (int32_t x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; x++)
        {
            TileElement* startElement = map_get_first_element_at(x, y);
            TileElement* endElement = startElement;
            while (!(endElement++)->IsLastForTile())
                ;

            num_elements = (uint32_t)(endElement - startElement);
            std::memcpy(new_elements_pointer, startElement, num_elements * sizeof(TileElement));
            new_elements_pointer += num_elements;
        }
    }

    num_elements = (uint32_t)(new_elements_pointer - new_tile_elements);
    std::memcpy(gTileElements, new_tile_elements, num_elements * sizeof(TileElement));
    std::memset(
        gTileElements + num_elements, 0,
        (3 * (MAXIMUM_MAP_SIZE_TECHNICAL * MAXIMUM_MAP_SIZE_TECHNICAL) - num_elements) * sizeof(TileElement));

    free(new_tile_elements);

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
TileElement* tile_element_insert(int32_t x, int32_t y, int32_t z, int32_t flags)
{
    TileElement *originalTileElement, *newTileElement, *insertedElement;

    if (!map_check_free_elements_and_reorganise(1))
    {
        log_error("Cannot insert new element");
        return nullptr;
    }

    newTileElement = gNextFreeTileElement;
    originalTileElement = gTileElementTilePointers[y * MAXIMUM_MAP_SIZE_TECHNICAL + x];

    // Set tile index pointer to point to new element block
    gTileElementTilePointers[y * MAXIMUM_MAP_SIZE_TECHNICAL + x] = newTileElement;

    // Copy all elements that are below the insert height
    while (z >= originalTileElement->base_height)
    {
        // Copy over map element
        *newTileElement = *originalTileElement;
        originalTileElement->base_height = 255;
        originalTileElement++;
        newTileElement++;

        if ((newTileElement - 1)->flags & TILE_ELEMENT_FLAG_LAST_TILE)
        {
            // No more elements above the insert element
            (newTileElement - 1)->flags &= ~TILE_ELEMENT_FLAG_LAST_TILE;
            flags |= TILE_ELEMENT_FLAG_LAST_TILE;
            break;
        }
    }

    // Insert new map element
    insertedElement = newTileElement;
    newTileElement->type = 0;
    newTileElement->base_height = z;
    newTileElement->flags = flags;
    newTileElement->clearance_height = z;
    std::memset(&newTileElement->pad_04, 0, sizeof(newTileElement->pad_04));
    newTileElement++;

    // Insert rest of map elements above insert height
    if (!(flags & TILE_ELEMENT_FLAG_LAST_TILE))
    {
        do
        {
            // Copy over map element
            *newTileElement = *originalTileElement;
            originalTileElement->base_height = 255;
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
void map_obstruction_set_error_text(TileElement* tileElement)
{
    rct_string_id errorStringId;
    Ride* ride;
    rct_scenery_entry* sceneryEntry;

    errorStringId = STR_OBJECT_IN_THE_WAY;
    switch (tileElement->GetType())
    {
        case TILE_ELEMENT_TYPE_SURFACE:
            errorStringId = STR_RAISE_OR_LOWER_LAND_FIRST;
            break;
        case TILE_ELEMENT_TYPE_PATH:
            errorStringId = STR_FOOTPATH_IN_THE_WAY;
            break;
        case TILE_ELEMENT_TYPE_TRACK:
            ride = get_ride(tileElement->AsTrack()->GetRideIndex());
            errorStringId = STR_X_IN_THE_WAY;
            set_format_arg(0, rct_string_id, ride->name);
            set_format_arg(2, uint32_t, ride->name_arguments);
            break;
        case TILE_ELEMENT_TYPE_SMALL_SCENERY:
            sceneryEntry = tileElement->AsSmallScenery()->GetEntry();
            errorStringId = STR_X_IN_THE_WAY;
            set_format_arg(0, rct_string_id, sceneryEntry->name);
            break;
        case TILE_ELEMENT_TYPE_ENTRANCE:
            switch (tileElement->AsEntrance()->GetEntranceType())
            {
                case ENTRANCE_TYPE_RIDE_ENTRANCE:
                    errorStringId = STR_RIDE_ENTRANCE_IN_THE_WAY;
                    break;
                case ENTRANCE_TYPE_RIDE_EXIT:
                    errorStringId = STR_RIDE_EXIT_IN_THE_WAY;
                    break;
                case ENTRANCE_TYPE_PARK_ENTRANCE:
                    errorStringId = STR_PARK_ENTRANCE_IN_THE_WAY;
                    break;
            }
            break;
        case TILE_ELEMENT_TYPE_WALL:
            sceneryEntry = tileElement->AsWall()->GetEntry();
            errorStringId = STR_X_IN_THE_WAY;
            set_format_arg(0, rct_string_id, sceneryEntry->name);
            break;
        case TILE_ELEMENT_TYPE_LARGE_SCENERY:
            sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
            errorStringId = STR_X_IN_THE_WAY;
            set_format_arg(0, rct_string_id, sceneryEntry->name);
            break;
    }

    gGameCommandErrorText = errorStringId;
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
bool map_can_construct_with_clear_at(
    int32_t x, int32_t y, int32_t zLow, int32_t zHigh, CLEAR_FUNC clearFunc, QuarterTile bl, uint8_t flags, money32* price,
    uint8_t crossingMode)
{
    int32_t al, ah, bh, cl, ch, water_height;
    al = ah = bh = cl = ch = water_height = 0;
    uint8_t slope = 0;

    gMapGroundFlags = ELEMENT_IS_ABOVE_GROUND;
    bool canBuildCrossing = false;
    if (x >= gMapSizeUnits || y >= gMapSizeUnits || x < 32 || y < 32)
    {
        gGameCommandErrorText = STR_OFF_EDGE_OF_MAP;
        return false;
    }

    if (gCheatsDisableClearanceChecks)
    {
        return true;
    }

    TileElement* tileElement = map_get_first_element_at(x / 32, y / 32);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_SURFACE)
        {
            if (zLow < tileElement->clearance_height && zHigh > tileElement->base_height && !(tileElement->IsGhost()))
            {
                if (tileElement->flags & (bl.GetBaseQuarterOccupied()))
                {
                    goto loc_68BABC;
                }
            }
            continue;
        }
        water_height = tileElement->AsSurface()->GetWaterHeight() * 2;
        if (water_height && water_height > zLow && tileElement->base_height < zHigh)
        {
            gMapGroundFlags |= ELEMENT_IS_UNDERWATER;
            if (water_height < zHigh)
            {
                goto loc_68BAE6;
            }
        }
    loc_68B9B7:
        if (gParkFlags & PARK_FLAGS_FORBID_HIGH_CONSTRUCTION)
        {
            al = zHigh - tileElement->base_height;
            if (al >= 0)
            {
                if (al > 18)
                {
                    gGameCommandErrorText = STR_LOCAL_AUTHORITY_WONT_ALLOW_CONSTRUCTION_ABOVE_TREE_HEIGHT;
                    return false;
                }
            }
        }

        // Only allow building crossings directly on a flat surface tile.
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_SURFACE
            && (tileElement->AsSurface()->GetSlope()) == TILE_ELEMENT_SLOPE_FLAT && tileElement->base_height == zLow)
        {
            canBuildCrossing = true;
        }

        if (bl.GetZQuarterOccupied() != 0b1111)
        {
            if (tileElement->base_height >= zHigh)
            {
                // loc_68BA81
                gMapGroundFlags |= ELEMENT_IS_UNDERGROUND;
                gMapGroundFlags &= ~ELEMENT_IS_ABOVE_GROUND;
            }
            else
            {
                al = tileElement->base_height;
                ah = al;
                cl = al;
                ch = al;
                slope = tileElement->AsSurface()->GetSlope();
                if (slope & TILE_ELEMENT_SLOPE_N_CORNER_UP)
                {
                    al += 2;
                    if (slope == (TILE_ELEMENT_SLOPE_S_CORNER_DN | TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT))
                        al += 2;
                }
                if (slope & TILE_ELEMENT_SLOPE_E_CORNER_UP)
                {
                    ah += 2;
                    if (slope == (TILE_ELEMENT_SLOPE_W_CORNER_DN | TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT))
                        ah += 2;
                }
                if (slope & TILE_ELEMENT_SLOPE_S_CORNER_UP)
                {
                    cl += 2;
                    if (slope == (TILE_ELEMENT_SLOPE_N_CORNER_DN | TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT))
                        cl += 2;
                }
                if (slope & TILE_ELEMENT_SLOPE_W_CORNER_UP)
                {
                    ch += 2;
                    if (slope == (TILE_ELEMENT_SLOPE_E_CORNER_DN | TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT))
                        ch += 2;
                }
                bh = zLow + 4;
                {
                    auto baseQuarter = bl.GetBaseQuarterOccupied();
                    auto zQuarter = bl.GetZQuarterOccupied();
                    if ((!(baseQuarter & 0b0001) || ((zQuarter & 0b0001 || zLow >= al) && bh >= al))
                        && (!(baseQuarter & 0b0010) || ((zQuarter & 0b0010 || zLow >= ah) && bh >= ah))
                        && (!(baseQuarter & 0b0100) || ((zQuarter & 0b0100 || zLow >= cl) && bh >= cl))
                        && (!(baseQuarter & 0b1000) || ((zQuarter & 0b1000 || zLow >= ch) && bh >= ch)))
                    {
                        continue;
                    }
                }
            loc_68BABC:
                if (clearFunc != nullptr)
                {
                    if (!clearFunc(&tileElement, x, y, flags, price))
                    {
                        continue;
                    }
                }

                // Crossing mode 1: building track over path
                if (crossingMode == 1 && canBuildCrossing && tileElement->GetType() == TILE_ELEMENT_TYPE_PATH
                    && tileElement->base_height == zLow && !tileElement->AsPath()->IsQueue()
                    && !tileElement->AsPath()->IsSloped())
                {
                    continue;
                }
                // Crossing mode 2: building path over track
                else if (
                    crossingMode == 2 && canBuildCrossing && tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK
                    && tileElement->base_height == zLow && tileElement->AsTrack()->GetTrackType() == TRACK_ELEM_FLAT)
                {
                    Ride* ride = get_ride(tileElement->AsTrack()->GetRideIndex());
                    if (ride->type == RIDE_TYPE_MINIATURE_RAILWAY)
                    {
                        continue;
                    }
                }

                if (tileElement != nullptr)
                {
                    map_obstruction_set_error_text(tileElement);
                }
                return false;

            loc_68BAE6:
                if (clearFunc != nullptr)
                {
                    if (!clearFunc(&tileElement, x, y, flags, price))
                    {
                        goto loc_68B9B7;
                    }
                }
                if (tileElement != nullptr)
                {
                    gGameCommandErrorText = STR_CANNOT_BUILD_PARTLY_ABOVE_AND_PARTLY_BELOW_WATER;
                }
                return false;
            }
        }
    } while (!(tileElement++)->IsLastForTile());
    return true;
}

/**
 *
 *  rct2: 0x0068B93A
 */
int32_t map_can_construct_at(int32_t x, int32_t y, int32_t zLow, int32_t zHigh, QuarterTile bl)
{
    return map_can_construct_with_clear_at(x, y, zLow, zHigh, nullptr, bl, 0, nullptr, CREATE_CROSSING_MODE_NONE);
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

        TileElement* tileElement = map_get_surface_element_at(x, y);
        if (tileElement != nullptr)
        {
            tileElement->AsSurface()->UpdateGrassLength({ x * 32, y * 32 });
            scenery_update_tile(x * 32, y * 32);
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
}

void map_restore_provisional_elements()
{
    if (gFootpathProvisionalFlags & PROVISIONAL_PATH_FLAG_1)
    {
        gFootpathProvisionalFlags &= ~PROVISIONAL_PATH_FLAG_1;
        footpath_provisional_set(
            gFootpathProvisionalType, gFootpathProvisionalPosition.x, gFootpathProvisionalPosition.y,
            gFootpathProvisionalPosition.z, gFootpathProvisionalSlope);
    }
    if (window_find_by_class(WC_RIDE_CONSTRUCTION) != nullptr)
    {
        ride_restore_provisional_track_piece();
        ride_entrance_exit_place_provisional_ghost();
    }
}

/**
 * Removes elements that are out of the map size range and crops the park perimeter.
 *  rct2: 0x0068ADBC
 */
void map_remove_out_of_range_elements()
{
    int32_t mapMaxXY = gMapSizeMaxXY;

    for (int32_t y = 0; y < (MAXIMUM_MAP_SIZE_TECHNICAL * 32); y += 32)
    {
        for (int32_t x = 0; x < (MAXIMUM_MAP_SIZE_TECHNICAL * 32); x += 32)
        {
            if (x == 0 || y == 0 || x >= mapMaxXY || y >= mapMaxXY)
            {
                map_buy_land_rights(x, y, x, y, BUY_LAND_RIGHTS_FLAG_UNOWN_TILE, GAME_COMMAND_FLAG_APPLY);
                clear_elements_at(x, y);
            }
        }
    }
}

/**
 * Copies the terrain and slope from the edge of the map to the new tiles. Used when increasing the size of the map.
 *  rct2: 0x0068AC15
 */
void map_extend_boundary_surface()
{
    SurfaceElement *existingTileElement, *newTileElement;
    int32_t x, y, z, slope;

    y = gMapSize - 2;
    for (x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; x++)
    {
        existingTileElement = map_get_surface_element_at(x, y - 1)->AsSurface();
        newTileElement = map_get_surface_element_at(x, y)->AsSurface();
        newTileElement->SetSurfaceStyle(existingTileElement->GetSurfaceStyle());
        newTileElement->SetEdgeStyle(existingTileElement->GetEdgeStyle());
        newTileElement->SetGrassLength(existingTileElement->GetGrassLength());
        newTileElement->SetOwnership(OWNERSHIP_UNOWNED);
        newTileElement->SetWaterHeight(existingTileElement->GetWaterHeight());

        z = existingTileElement->base_height;
        slope = existingTileElement->GetSlope() & TILE_ELEMENT_SLOPE_NW_SIDE_UP;
        if (slope == TILE_ELEMENT_SLOPE_NW_SIDE_UP)
        {
            z += 2;
            slope = TILE_ELEMENT_SLOPE_FLAT;
            if (existingTileElement->GetSlope() & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
            {
                slope = TILE_ELEMENT_SLOPE_N_CORNER_UP;
                if (existingTileElement->GetSlope() & TILE_ELEMENT_SLOPE_S_CORNER_UP)
                {
                    slope = TILE_ELEMENT_SLOPE_W_CORNER_UP;
                    if (existingTileElement->GetSlope() & TILE_ELEMENT_SLOPE_E_CORNER_UP)
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

        newTileElement->SetSlope(slope);
        newTileElement->base_height = z;
        newTileElement->clearance_height = z;

        update_park_fences({ x << 5, y << 5 });
    }

    x = gMapSize - 2;
    for (y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; y++)
    {
        existingTileElement = map_get_surface_element_at(x - 1, y)->AsSurface();
        newTileElement = map_get_surface_element_at(x, y)->AsSurface();

        newTileElement->SetSurfaceStyle(existingTileElement->GetSurfaceStyle());
        newTileElement->SetEdgeStyle(existingTileElement->GetEdgeStyle());
        newTileElement->SetGrassLength(existingTileElement->GetGrassLength());
        newTileElement->SetOwnership(OWNERSHIP_UNOWNED);
        newTileElement->SetWaterHeight(existingTileElement->GetWaterHeight());

        z = existingTileElement->base_height;
        slope = existingTileElement->GetSlope() & TILE_ELEMENT_SLOPE_NE_SIDE_UP;
        if (slope == TILE_ELEMENT_SLOPE_NE_SIDE_UP)
        {
            z += 2;
            slope = TILE_ELEMENT_SLOPE_FLAT;
            if (existingTileElement->GetSlope() & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
            {
                slope = TILE_ELEMENT_SLOPE_N_CORNER_UP;
                if (existingTileElement->GetSlope() & TILE_ELEMENT_SLOPE_S_CORNER_UP)
                {
                    slope = TILE_ELEMENT_SLOPE_E_CORNER_UP;
                    if (existingTileElement->GetSlope() & TILE_ELEMENT_SLOPE_W_CORNER_UP)
                    {
                        slope = TILE_ELEMENT_SLOPE_FLAT;
                    }
                }
            }
        }
        if (slope & TILE_ELEMENT_SLOPE_N_CORNER_UP)
            slope |= TILE_ELEMENT_SLOPE_W_CORNER_UP;
        if (slope & TILE_ELEMENT_SLOPE_E_CORNER_UP)
            slope |= TILE_ELEMENT_SLOPE_S_CORNER_UP;

        newTileElement->SetSlope(slope);
        newTileElement->base_height = z;
        newTileElement->clearance_height = z;

        update_park_fences({ x << 5, y << 5 });
    }
}

/**
 * Clears the provided element properly from a certain tile, and updates
 * the pointer (when needed) passed to this function to point to the next element.
 */
static void clear_element_at(int32_t x, int32_t y, TileElement** elementPtr)
{
    TileElement* element = *elementPtr;
    switch (element->GetType())
    {
        case TILE_ELEMENT_TYPE_SURFACE:
            element->base_height = 2;
            element->clearance_height = 2;
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
            switch (element->AsEntrance()->GetSequenceIndex())
            {
                case 1:
                    x += CoordsDirectionDelta[rotation].x;
                    y += CoordsDirectionDelta[rotation].y;
                    break;
                case 2:
                    x -= CoordsDirectionDelta[rotation].x;
                    y -= CoordsDirectionDelta[rotation].y;
                    break;
            }
            gGameCommandErrorTitle = STR_CANT_REMOVE_THIS;
            game_do_command(x, GAME_COMMAND_FLAG_APPLY, y, element->base_height / 2, GAME_COMMAND_REMOVE_PARK_ENTRANCE, 0, 0);
            break;
        }
        case TILE_ELEMENT_TYPE_WALL:
        {
            TileCoordsXYZD wallLocation = { x >> 5, y >> 5, element->base_height, element->GetDirection() };
            auto wallRemoveAction = WallRemoveAction(wallLocation);
            GameActions::Execute(&wallRemoveAction);
        }
        break;
        case TILE_ELEMENT_TYPE_LARGE_SCENERY:
        {
            auto removeSceneryAction = LargeSceneryRemoveAction(
                x, y, element->base_height, element->GetDirection(), element->AsLargeScenery()->GetSequenceIndex());
            GameActions::Execute(&removeSceneryAction);
        }
        break;
        case TILE_ELEMENT_TYPE_BANNER:
            gGameCommandErrorTitle = STR_CANT_REMOVE_THIS;
            game_do_command(
                x, GAME_COMMAND_FLAG_APPLY, y, (element->base_height) | ((element->AsBanner()->GetPosition() & 3) << 8),
                GAME_COMMAND_REMOVE_BANNER, 0, 0);
            break;
        default:
            tile_element_remove(element);
            break;
    }
}

/**
 * Clears all elements properly from a certain tile.
 *  rct2: 0x0068AE2A
 */
static void clear_elements_at(int32_t x, int32_t y)
{
    // Remove the spawn point (if there is one in the current tile)
    gPeepSpawns.erase(
        std::remove_if(
            gPeepSpawns.begin(), gPeepSpawns.end(),
            [x, y](const auto& spawn) { return floor2(spawn.x, 32) == x && floor2(spawn.y, 32) == y; }),
        gPeepSpawns.end());

    TileElement* tileElement = map_get_first_element_at(x >> 5, y >> 5);

    // Remove all elements except the last one
    while (!tileElement->IsLastForTile())
        clear_element_at(x, y, &tileElement);

    // Remove the last element
    clear_element_at(x, y, &tileElement);
}

int32_t map_get_highest_z(int32_t tileX, int32_t tileY)
{
    TileElement* tileElement;
    uint32_t z;

    tileElement = map_get_surface_element_at(tileX, tileY);
    if (tileElement == nullptr)
        return -1;

    z = tileElement->base_height * 8;

    // Raise z so that is above highest point of land and water on tile
    if ((tileElement->AsSurface()->GetSlope() & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP) != TILE_ELEMENT_SLOPE_FLAT)
        z += 16;
    if ((tileElement->AsSurface()->GetSlope() & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT) != 0)
        z += 16;

    z = std::max(z, tileElement->AsSurface()->GetWaterHeight() * 16);
    return z;
}

TileElement* map_get_large_scenery_segment(int32_t x, int32_t y, int32_t z, int32_t direction, int32_t sequence)
{
    TileElement* tileElement = map_get_first_element_at(x >> 5, y >> 5);
    if (tileElement == nullptr)
    {
        return nullptr;
    }
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_LARGE_SCENERY)
            continue;
        if (tileElement->base_height != z)
            continue;
        if (tileElement->AsLargeScenery()->GetSequenceIndex() != sequence)
            continue;
        if ((tileElement->GetDirection()) != direction)
            continue;

        return tileElement;
    } while (!(tileElement++)->IsLastForTile());
    return nullptr;
}

EntranceElement* map_get_park_entrance_element_at(int32_t x, int32_t y, int32_t z, bool ghost)
{
    TileElement* tileElement = map_get_first_element_at(x >> 5, y >> 5);
    if (tileElement != nullptr)
    {
        do
        {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
                continue;

            if (tileElement->base_height != z)
                continue;

            if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_PARK_ENTRANCE)
                continue;

            if ((ghost == false) && (tileElement->IsGhost()))
                continue;

            return tileElement->AsEntrance();
        } while (!(tileElement++)->IsLastForTile());
    }
    return nullptr;
}

EntranceElement* map_get_ride_entrance_element_at(int32_t x, int32_t y, int32_t z, bool ghost)
{
    TileElement* tileElement = map_get_first_element_at(x >> 5, y >> 5);
    if (tileElement != nullptr)
    {
        do
        {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
                continue;

            if (tileElement->base_height != z)
                continue;

            if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_RIDE_ENTRANCE)
                continue;

            if ((ghost == false) && (tileElement->IsGhost()))
                continue;

            return tileElement->AsEntrance();
        } while (!(tileElement++)->IsLastForTile());
    }
    return nullptr;
}

EntranceElement* map_get_ride_exit_element_at(int32_t x, int32_t y, int32_t z, bool ghost)
{
    TileElement* tileElement = map_get_first_element_at(x >> 5, y >> 5);
    if (tileElement != nullptr)
    {
        do
        {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
                continue;

            if (tileElement->base_height != z)
                continue;

            if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_RIDE_EXIT)
                continue;

            if ((ghost == false) && (tileElement->IsGhost()))
                continue;

            return tileElement->AsEntrance();
        } while (!(tileElement++)->IsLastForTile());
    }
    return nullptr;
}

TileElement* map_get_small_scenery_element_at(int32_t x, int32_t y, int32_t z, int32_t type, uint8_t quadrant)
{
    TileElement* tileElement = map_get_first_element_at(x >> 5, y >> 5);
    if (tileElement != nullptr)
    {
        do
        {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
                continue;
            if (tileElement->AsSmallScenery()->GetSceneryQuadrant() != quadrant)
                continue;
            if (tileElement->base_height != z)
                continue;
            if (tileElement->AsSmallScenery()->GetEntryIndex() != type)
                continue;

            return tileElement;
        } while (!(tileElement++)->IsLastForTile());
    }
    return nullptr;
}

bool map_large_scenery_get_origin(
    int32_t x, int32_t y, int32_t z, int32_t direction, int32_t sequence, int32_t* outX, int32_t* outY, int32_t* outZ,
    TileElement** outElement)
{
    TileElement* tileElement;
    rct_scenery_entry* sceneryEntry;
    rct_large_scenery_tile* tile;
    int16_t offsetX, offsetY;

    tileElement = map_get_large_scenery_segment(x, y, z, direction, sequence);
    if (tileElement == nullptr)
        return false;

    sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
    tile = &sceneryEntry->large_scenery.tiles[sequence];

    offsetX = tile->x_offset;
    offsetY = tile->y_offset;
    rotate_map_coordinates(&offsetX, &offsetY, direction);

    *outX = x - offsetX;
    *outY = y - offsetY;
    *outZ = (z * 8) - tile->z_offset;
    if (outElement != nullptr)
        *outElement = tileElement;
    return true;
}

/**
 *
 *  rct2: 0x006B9B05
 */
bool sign_set_colour(
    int32_t x, int32_t y, int32_t z, int32_t direction, int32_t sequence, uint8_t mainColour, uint8_t textColour)
{
    TileElement* tileElement;
    rct_scenery_entry* sceneryEntry;
    rct_large_scenery_tile *sceneryTiles, *tile;
    int16_t offsetX, offsetY;
    int32_t x0, y0, z0;

    if (!map_large_scenery_get_origin(x, y, z, direction, sequence, &x0, &y0, &z0, &tileElement))
    {
        return false;
    }

    sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
    sceneryTiles = sceneryEntry->large_scenery.tiles;

    // Iterate through each tile of the large scenery element
    sequence = 0;
    for (tile = sceneryTiles; tile->x_offset != -1; tile++, sequence++)
    {
        offsetX = tile->x_offset;
        offsetY = tile->y_offset;
        rotate_map_coordinates(&offsetX, &offsetY, direction);

        x = x0 + offsetX;
        y = y0 + offsetY;
        z = (z0 + tile->z_offset) / 8;
        tileElement = map_get_large_scenery_segment(x, y, z, direction, sequence);
        if (tileElement != nullptr)
        {
            tileElement->AsLargeScenery()->SetPrimaryColour(mainColour);
            tileElement->AsLargeScenery()->SetSecondaryColour(textColour);

            map_invalidate_tile(x, y, tileElement->base_height * 8, tileElement->clearance_height * 8);
        }
    }

    return true;
}

static void translate_3d_to_2d(int32_t rotation, int32_t* x, int32_t* y)
{
    int32_t rx, ry;

    switch (rotation & 3)
    {
        default:
        case 0:
            rx = (*y) - (*x);
            ry = (*x) + (*y);
            break;
        case 1:
            rx = -(*x) - (*y);
            ry = (*y) - (*x);
            break;
        case 2:
            rx = (*x) - (*y);
            ry = -(*x) - (*y);
            break;
        case 3:
            rx = (*x) + (*y);
            ry = (*x) - (*y);
            break;
    }
    ry /= 2;

    *x = rx;
    *y = ry;
}

CoordsXY translate_3d_to_2d_with_z(int32_t rotation, CoordsXYZ pos)
{
    CoordsXY result = {};
    switch (rotation & 3)
    {
        default:
        case 0:
            result.x = pos.y - pos.x;
            result.y = (pos.x + pos.y) / 2 - pos.z;
            break;
        case 1:
            result.x = -pos.x - pos.y;
            result.y = (pos.y - pos.x) / 2 - pos.z;
            break;
        case 2:
            result.x = pos.x - pos.y;
            result.y = (-pos.x - pos.y) / 2 - pos.z;
            break;
        case 3:
            result.x = pos.x + pos.y;
            result.y = (pos.x - pos.y) / 2 - pos.z;
            break;
    }
    return result;
}

static void map_invalidate_tile_under_zoom(int32_t x, int32_t y, int32_t z0, int32_t z1, int32_t maxZoom)
{
    if (gOpenRCT2Headless)
        return;

    int32_t x1, y1, x2, y2;

    x += 16;
    y += 16;
    translate_3d_to_2d(get_current_rotation(), &x, &y);

    x1 = x - 32;
    y1 = y - 32 - z1;
    x2 = x + 32;
    y2 = y + 32 - z0;

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
void map_invalidate_tile(int32_t x, int32_t y, int32_t z0, int32_t z1)
{
    map_invalidate_tile_under_zoom(x, y, z0, z1, -1);
}

/**
 *
 *  rct2: 0x006ECB60
 */
void map_invalidate_tile_zoom1(int32_t x, int32_t y, int32_t z0, int32_t z1)
{
    map_invalidate_tile_under_zoom(x, y, z0, z1, 1);
}

/**
 *
 *  rct2: 0x006EC9CE
 */
void map_invalidate_tile_zoom0(int32_t x, int32_t y, int32_t z0, int32_t z1)
{
    map_invalidate_tile_under_zoom(x, y, z0, z1, 0);
}

/**
 *
 *  rct2: 0x006EC6D7
 */
void map_invalidate_tile_full(int32_t x, int32_t y)
{
    map_invalidate_tile(x, y, 0, 2080);
}

void map_invalidate_element(int32_t x, int32_t y, TileElement* tileElement)
{
    map_invalidate_tile(x, y, tileElement->base_height * 8, tileElement->clearance_height * 8);
}

void map_invalidate_region(const LocationXY16& mins, const LocationXY16& maxs)
{
    int32_t x0, y0, x1, y1, left, right, top, bottom;

    x0 = mins.x + 16;
    y0 = mins.y + 16;

    x1 = maxs.x + 16;
    y1 = maxs.y + 16;

    map_get_bounding_box(x0, y0, x1, y1, &left, &top, &right, &bottom);

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

int32_t map_get_tile_side(int32_t mapX, int32_t mapY)
{
    int32_t subMapX = mapX & (32 - 1);
    int32_t subMapY = mapY & (32 - 1);
    return (subMapX < subMapY) ? ((subMapX + subMapY) < 32 ? 0 : 1) : ((subMapX + subMapY) < 32 ? 3 : 2);
}

int32_t map_get_tile_quadrant(int32_t mapX, int32_t mapY)
{
    int32_t subMapX = mapX & (32 - 1);
    int32_t subMapY = mapY & (32 - 1);
    return (subMapX > 16) ? (subMapY < 16 ? 1 : 0) : (subMapY < 16 ? 2 : 3);
}

/**
 *
 *  rct2: 0x00693BFF
 */
bool map_surface_is_blocked(int16_t x, int16_t y)
{
    TileElement* tileElement;
    if (x >= 8192 || y >= 8192)
        return true;

    tileElement = map_get_surface_element_at({ x, y });

    if (tileElement == nullptr)
    {
        return true;
    }

    int16_t water_height = tileElement->AsSurface()->GetWaterHeight();
    water_height *= 2;
    if (water_height > tileElement->base_height)
        return true;

    int16_t base_z = tileElement->base_height;
    int16_t clear_z = tileElement->base_height + 2;
    if (tileElement->AsSurface()->GetSlope() & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
        clear_z += 2;

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
    for (int32_t y = 0; y < (MAXIMUM_MAP_SIZE_TECHNICAL * 32); y += 32)
    {
        for (int32_t x = 0; x < (MAXIMUM_MAP_SIZE_TECHNICAL * 32); x += 32)
        {
            clear_elements_at(x, y);
        }
    }
}

void game_command_modify_tile(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi, int32_t* edi, int32_t* ebp)
{
    const int32_t flags = *ebx;
    const int32_t x = *ecx & 0xFF;
    const int32_t y = (*ecx >> 8) & 0xFF;
    const TILE_INSPECTOR_INSTRUCTION_TYPE instruction = static_cast<TILE_INSPECTOR_INSTRUCTION_TYPE>(*eax);

    switch (instruction)
    {
        case TILE_INSPECTOR_ANY_REMOVE:
        {
            const int16_t elementIndex = *edx;
            *ebx = tile_inspector_remove_element_at(x, y, elementIndex, flags);
            break;
        }
        case TILE_INSPECTOR_ANY_SWAP:
        {
            const int32_t firstIndex = *edx;
            const int32_t secondIndex = *edi;
            *ebx = tile_inspector_swap_elements_at(x, y, firstIndex, secondIndex, flags);
            break;
        }
        case TILE_INSPECTOR_ANY_INSERT_CORRUPT:
        {
            const int16_t elementIndex = *edx;
            *ebx = tile_inspector_insert_corrupt_at(x, y, elementIndex, flags);
            break;
        }
        case TILE_INSPECTOR_ANY_ROTATE:
        {
            const int16_t elementIndex = *edx;
            *ebx = tile_inspector_rotate_element_at(x, y, elementIndex, flags);
            break;
        }
        case TILE_INSPECTOR_ANY_PASTE:
        {
            TileElement elementToPaste;
            const int32_t data[] = { *edx, *edi };
            assert_struct_size(data, sizeof(elementToPaste));
            std::memcpy(&elementToPaste, data, 8);
            *ebx = tile_inspector_paste_element_at(x, y, elementToPaste, flags);
            break;
        }
        case TILE_INSPECTOR_ANY_SORT:
        {
            *ebx = tile_inspector_sort_elements_at(x, y, flags);
            break;
        }
        case TILE_INSPECTOR_ANY_BASE_HEIGHT_OFFSET:
        {
            const int16_t elementIndex = *edx;
            const int8_t heightOffset = *edi;
            *ebx = tile_inspector_any_base_height_offset(x, y, elementIndex, heightOffset, flags);
            break;
        }
        case TILE_INSPECTOR_SURFACE_SHOW_PARK_FENCES:
        {
            const bool showFences = *edx;
            *ebx = tile_inspector_surface_show_park_fences(x, y, showFences, flags);
            break;
        }
        case TILE_INSPECTOR_SURFACE_TOGGLE_CORNER:
        {
            const int32_t cornerIndex = *edx;
            *ebx = tile_inspector_surface_toggle_corner(x, y, cornerIndex, flags);
            break;
        }
        case TILE_INSPECTOR_SURFACE_TOGGLE_DIAGONAL:
        {
            *ebx = tile_inspector_surface_toggle_diagonal(x, y, flags);
            break;
        }
        case TILE_INSPECTOR_PATH_SET_SLOPE:
        {
            const int32_t elementIndex = *edx;
            const bool sloped = *edi;
            *ebx = tile_inspector_path_set_sloped(x, y, elementIndex, sloped, flags);
            break;
        }
        case TILE_INSPECTOR_PATH_TOGGLE_EDGE:
        {
            const int32_t elementIndex = *edx;
            const int32_t edgeIndex = *edi;
            *ebx = tile_inspector_path_toggle_edge(x, y, elementIndex, edgeIndex, flags);
            break;
        }
        case TILE_INSPECTOR_ENTRANCE_MAKE_USABLE:
        {
            const int32_t elementIndex = *edx;
            *ebx = tile_inspector_entrance_make_usable(x, y, elementIndex, flags);
            break;
        }
        case TILE_INSPECTOR_WALL_SET_SLOPE:
        {
            const int32_t elementIndex = *edx;
            const int32_t slopeValue = *edi;
            *ebx = tile_inspector_wall_set_slope(x, y, elementIndex, slopeValue, flags);
            break;
        }
        case TILE_INSPECTOR_TRACK_BASE_HEIGHT_OFFSET:
        {
            const int32_t elementIndex = *edx;
            const int8_t heightOffset = *edi;
            *ebx = tile_inspector_track_base_height_offset(x, y, elementIndex, heightOffset, flags);
            break;
        }
        case TILE_INSPECTOR_TRACK_SET_CHAIN:
        {
            const int32_t elementIndex = *edx;
            const bool entireTrackBlock = *edi;
            const bool setChain = *ebp;
            *ebx = tile_inspector_track_set_chain(x, y, elementIndex, entireTrackBlock, setChain, flags);
            break;
        }
        case TILE_INSPECTOR_SCENERY_SET_QUARTER_LOCATION:
        {
            const int32_t elementIndex = *edx;
            const int32_t quarterIndex = *edi;
            *ebx = tile_inspector_scenery_set_quarter_location(x, y, elementIndex, quarterIndex, flags);
            break;
        }
        case TILE_INSPECTOR_SCENERY_SET_QUARTER_COLLISION:
        {
            const int32_t elementIndex = *edx;
            const int32_t quarterIndex = *edi;
            *ebx = tile_inspector_scenery_set_quarter_collision(x, y, elementIndex, quarterIndex, flags);
            break;
        }
        case TILE_INSPECTOR_BANNER_TOGGLE_BLOCKING_EDGE:
        {
            const int32_t elementIndex = *edx;
            const int32_t edgeIndex = *edi;
            *ebx = tile_inspector_banner_toggle_blocking_edge(x, y, elementIndex, edgeIndex, flags);
            break;
        }
        case TILE_INSPECTOR_CORRUPT_CLAMP:
        {
            const int32_t elementIndex = *edx;
            *ebx = tile_inspector_corrupt_clamp(x, y, elementIndex, flags);
            break;
        }
        default:
            log_error("invalid instruction");
            *ebx = MONEY32_UNDEFINED;
            break;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY && gGameCommandNestLevel == 1 && !(flags & GAME_COMMAND_FLAG_GHOST)
        && *ebx != MONEY32_UNDEFINED)
    {
        LocationXYZ16 coord;
        coord.x = (x << 5) + 16;
        coord.y = (y << 5) + 16;
        coord.z = tile_element_height(coord.x, coord.y);
        network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
    }
}

/**
 * Gets the track element at x, y, z.
 * @param x x units, not tiles.
 * @param y y units, not tiles.
 * @param z Base height.
 */
TileElement* map_get_track_element_at(int32_t x, int32_t y, int32_t z)
{
    TileElement* tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (tileElement->base_height != z)
            continue;

        return tileElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

/**
 * Gets the track element at x, y, z that is the given track type.
 * @param x x units, not tiles.
 * @param y y units, not tiles.
 * @param z Base height.
 */
TileElement* map_get_track_element_at_of_type(int32_t x, int32_t y, int32_t z, int32_t trackType)
{
    TileElement* tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (tileElement->base_height != z)
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
TileElement* map_get_track_element_at_of_type_seq(int32_t x, int32_t y, int32_t z, int32_t trackType, int32_t sequence)
{
    TileElement* tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (tileElement == nullptr)
            break;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (tileElement->base_height != z)
            continue;
        if (tileElement->AsTrack()->GetTrackType() != trackType)
            continue;
        if (tileElement->AsTrack()->GetSequenceIndex() != sequence)
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
TileElement* map_get_track_element_at_of_type_from_ride(int32_t x, int32_t y, int32_t z, int32_t trackType, ride_id_t rideIndex)
{
    TileElement* tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (tileElement->base_height != z)
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
TileElement* map_get_track_element_at_from_ride(int32_t x, int32_t y, int32_t z, ride_id_t rideIndex)
{
    TileElement* tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (tileElement->base_height != z)
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
TileElement* map_get_track_element_at_with_direction_from_ride(
    int32_t x, int32_t y, int32_t z, int32_t direction, ride_id_t rideIndex)
{
    TileElement* tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (tileElement->base_height != z)
            continue;
        if (tileElement->AsTrack()->GetRideIndex() != rideIndex)
            continue;
        if (tileElement->GetDirection() != direction)
            continue;

        return tileElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
};

void map_offset_with_rotation(int16_t* x, int16_t* y, int16_t offsetX, int16_t offsetY, uint8_t rotation)
{
    switch (rotation & 3)
    {
        case TILE_ELEMENT_DIRECTION_WEST:
            *x += offsetX;
            *y += offsetY;
            break;
        case TILE_ELEMENT_DIRECTION_NORTH:
            *x += offsetY;
            *y -= offsetX;
            break;
        case TILE_ELEMENT_DIRECTION_EAST:
            *x -= offsetX;
            *y -= offsetY;
            break;
        case TILE_ELEMENT_DIRECTION_SOUTH:
            *x -= offsetY;
            *y += offsetX;
            break;
    }
}

TileElement* map_get_wall_element_at(int32_t x, int32_t y, int32_t z, int32_t direction)
{
    TileElement* tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_WALL)
            continue;
        if (tileElement->base_height != z)
            continue;
        if (tileElement->GetDirection() != direction)
            continue;

        return tileElement;
    } while (!(tileElement++)->IsLastForTile());
    return nullptr;
}

uint16_t check_max_allowable_land_rights_for_tile(uint8_t x, uint8_t y, uint8_t base_z)
{
    TileElement* tileElement = map_get_first_element_at(x, y);
    uint16_t destOwnership = OWNERSHIP_OWNED;

    // Sometimes done deliberately.
    if (tileElement == nullptr)
    {
        return OWNERSHIP_OWNED;
    }

    do
    {
        int32_t type = tileElement->GetType();
        if (type == TILE_ELEMENT_TYPE_PATH
            || (type == TILE_ELEMENT_TYPE_ENTRANCE
                && tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE))
        {
            destOwnership = OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED;
            // Do not own construction rights if too high/below surface
            if (tileElement->base_height - 3 > base_z || tileElement->base_height < base_z)
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
    TileElement* currentElement;
    for (const TileCoordsXY* tile = tiles.begin(); tile != tiles.end(); ++tile)
    {
        currentElement = map_get_surface_element_at((*tile).x, (*tile).y);
        currentElement->AsSurface()->SetOwnership(ownership);
        update_park_fences_around_tile({ (*tile).x * 32, (*tile).y * 32 });
    }
}
