/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include "../actions/BannerRemoveAction.h"
#include "../actions/LargeSceneryRemoveAction.h"
#include "../actions/ParkEntranceRemoveAction.h"
#include "../actions/WallRemoveAction.h"
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
#include "../profiling/Profiling.h"
#include "../ride/RideConstruction.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../ride/TrackDesign.h"
#include "../scenario/Scenario.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/TilePointerIndex.hpp"
#include "Banner.h"
#include "Climate.h"
#include "Footpath.h"
#include "LargeScenery.h"
#include "MapAnimation.h"
#include "Park.h"
#include "Scenery.h"
#include "SmallScenery.h"
#include "Surface.h"
#include "TileElementsView.h"
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

const TileCoordsXY TileDirectionDelta[] = {
    { -1, 0 }, { 0, +1 }, { +1, 0 }, { 0, -1 }, { -1, +1 }, { +1, +1 }, { +1, -1 }, { -1, -1 },
};

constexpr size_t MIN_TILE_ELEMENTS = 1024;

uint16_t gMapSelectFlags;
uint16_t gMapSelectType;
CoordsXY gMapSelectPositionA;
CoordsXY gMapSelectPositionB;
CoordsXYZ gMapSelectArrowPosition;
uint8_t gMapSelectArrowDirection;

TileCoordsXY gWidePathTileLoopPosition;
uint16_t gGrassSceneryTileLoopPosition;

TileCoordsXY gMapSize;
int32_t gMapBaseZ;

std::vector<CoordsXY> gMapSelectionTiles;
std::vector<PeepSpawn> gPeepSpawns;

bool gLandMountainMode;
bool gLandPaintMode;
bool gClearSmallScenery;
bool gClearLargeScenery;
bool gClearFootpath;

uint32_t gLandRemainingOwnershipSales;
uint32_t gLandRemainingConstructionSales;

bool gMapLandRightsUpdateSuccess;

static TilePointerIndex<TileElement> _tileIndex;
static std::vector<TileElement> _tileElements;
static TilePointerIndex<TileElement> _tileIndexStash;
static std::vector<TileElement> _tileElementsStash;
static size_t _tileElementsInUse;
static size_t _tileElementsInUseStash;
static TileCoordsXY _mapSizeStash;
static int32_t _currentRotationStash;

void StashMap()
{
    _tileIndexStash = std::move(_tileIndex);
    _tileElementsStash = std::move(_tileElements);
    _mapSizeStash = gMapSize;
    _currentRotationStash = gCurrentRotation;
    _tileElementsInUseStash = _tileElementsInUse;
}

void UnstashMap()
{
    _tileIndex = std::move(_tileIndexStash);
    _tileElements = std::move(_tileElementsStash);
    gMapSize = _mapSizeStash;
    gCurrentRotation = _currentRotationStash;
    _tileElementsInUse = _tileElementsInUseStash;
}

const std::vector<TileElement>& GetTileElements()
{
    return _tileElements;
}

void SetTileElements(std::vector<TileElement>&& tileElements)
{
    _tileElements = std::move(tileElements);
    _tileIndex = TilePointerIndex<TileElement>(MAXIMUM_MAP_SIZE_TECHNICAL, _tileElements.data(), _tileElements.size());
    _tileElementsInUse = _tileElements.size();
}

static TileElement GetDefaultSurfaceElement()
{
    TileElement el;
    el.ClearAs(TileElementType::Surface);
    el.SetLastForTile(true);
    el.base_height = 14;
    el.clearance_height = 14;
    el.AsSurface()->SetWaterHeight(0);
    el.AsSurface()->SetSlope(TILE_ELEMENT_SLOPE_FLAT);
    el.AsSurface()->SetGrassLength(GRASS_LENGTH_CLEAR_0);
    el.AsSurface()->SetOwnership(OWNERSHIP_UNOWNED);
    el.AsSurface()->SetParkFences(0);
    el.AsSurface()->SetSurfaceStyle(0);
    el.AsSurface()->SetEdgeStyle(0);
    return el;
}

std::vector<TileElement> GetReorganisedTileElementsWithoutGhosts()
{
    std::vector<TileElement> newElements;
    newElements.reserve(std::max(MIN_TILE_ELEMENTS, _tileElements.size()));
    for (int32_t y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; y++)
    {
        for (int32_t x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; x++)
        {
            auto oldSize = newElements.size();

            // Add all non-ghost elements
            const auto* element = MapGetFirstElementAt(TileCoordsXY{ x, y }.ToCoordsXY());
            if (element != nullptr)
            {
                do
                {
                    if (!element->IsGhost())
                    {
                        newElements.push_back(*element);
                    }
                } while (!(element++)->IsLastForTile());
            }

            // Insert default surface element if no elements were added
            auto newSize = newElements.size();
            if (oldSize == newSize)
            {
                newElements.push_back(GetDefaultSurfaceElement());
            }

            // Ensure last element of tile has last flag set
            auto& lastEl = newElements.back();
            lastEl.SetLastForTile(true);
        }
    }
    return newElements;
}

static void ReorganiseTileElements(size_t capacity)
{
    ContextSetCurrentCursor(CursorID::ZZZ);

    std::vector<TileElement> newElements;
    newElements.reserve(std::max(MIN_TILE_ELEMENTS, capacity));
    for (int32_t y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; y++)
    {
        for (int32_t x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; x++)
        {
            const auto* element = MapGetFirstElementAt(TileCoordsXY{ x, y });
            if (element == nullptr)
            {
                newElements.push_back(GetDefaultSurfaceElement());
            }
            else
            {
                do
                {
                    newElements.push_back(*element);
                } while (!(element++)->IsLastForTile());
            }
        }
    }

    SetTileElements(std::move(newElements));
}

void ReorganiseTileElements()
{
    ReorganiseTileElements(_tileElements.size());
}

static bool MapCheckFreeElementsAndReorganise(size_t numElementsOnTile, size_t numNewElements)
{
    // Check hard cap on num in use tiles (this would be the size of _tileElements immediately after a reorg)
    if (_tileElementsInUse + numNewElements > MAX_TILE_ELEMENTS)
    {
        return false;
    }

    auto totalElementsRequired = numElementsOnTile + numNewElements;
    auto freeElements = _tileElements.capacity() - _tileElements.size();
    if (freeElements >= totalElementsRequired)
    {
        return true;
    }

    // if space issue is due to fragmentation then Reorg Tiles without increasing capacity
    if (_tileElements.size() > totalElementsRequired + _tileElementsInUse)
    {
        ReorganiseTileElements();
        // This check is not expected to fail
        freeElements = _tileElements.capacity() - _tileElements.size();
        if (freeElements >= totalElementsRequired)
        {
            return true;
        }
    }

    // Capacity must increase to handle the space (Note capacity can go above MAX_TILE_ELEMENTS)
    auto newCapacity = _tileElements.capacity() * 2;
    ReorganiseTileElements(newCapacity);
    return true;
}

static size_t CountElementsOnTile(const CoordsXY& loc);

bool MapCheckCapacityAndReorganise(const CoordsXY& loc, size_t numElements)
{
    auto numElementsOnTile = CountElementsOnTile(loc);
    return MapCheckFreeElementsAndReorganise(numElementsOnTile, numElements);
}

static void ClearElementsAt(const CoordsXY& loc);
static ScreenCoordsXY Translate3DTo2D(int32_t rotation, const CoordsXY& pos);

void TileElementIteratorBegin(tile_element_iterator* it)
{
    it->x = 0;
    it->y = 0;
    it->element = MapGetFirstElementAt(TileCoordsXY{ 0, 0 });
}

int32_t TileElementIteratorNext(tile_element_iterator* it)
{
    if (it->element == nullptr)
    {
        it->element = MapGetFirstElementAt(TileCoordsXY{ it->x, it->y });
        return 1;
    }

    if (!it->element->IsLastForTile())
    {
        it->element++;
        return 1;
    }

    if (it->y < (MAXIMUM_MAP_SIZE_TECHNICAL - 1))
    {
        it->y++;
        it->element = MapGetFirstElementAt(TileCoordsXY{ it->x, it->y });
        return 1;
    }

    if (it->x < (MAXIMUM_MAP_SIZE_TECHNICAL - 1))
    {
        it->y = 0;
        it->x++;
        it->element = MapGetFirstElementAt(TileCoordsXY{ it->x, it->y });
        return 1;
    }

    return 0;
}

void TileElementIteratorRestartForTile(tile_element_iterator* it)
{
    it->element = nullptr;
}

static bool IsTileLocationValid(const TileCoordsXY& coords)
{
    const bool is_x_valid = coords.x < MAXIMUM_MAP_SIZE_TECHNICAL && coords.x >= 0;
    const bool is_y_valid = coords.y < MAXIMUM_MAP_SIZE_TECHNICAL && coords.y >= 0;
    return is_x_valid && is_y_valid;
}

TileElement* MapGetFirstElementAt(const TileCoordsXY& tilePos)
{
    if (!IsTileLocationValid(tilePos))
    {
        log_verbose("Trying to access element outside of range");
        return nullptr;
    }
    return _tileIndex.GetFirstElementAt(tilePos);
}

TileElement* MapGetFirstElementAt(const CoordsXY& elementPos)
{
    return MapGetFirstElementAt(TileCoordsXY{ elementPos });
}

TileElement* MapGetNthElementAt(const CoordsXY& coords, int32_t n)
{
    TileElement* tileElement = MapGetFirstElementAt(coords);
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

TileElement* MapGetFirstTileElementWithBaseHeightBetween(const TileCoordsXYRangedZ& loc, TileElementType type)
{
    TileElement* tileElement = MapGetFirstElementAt(loc.ToCoordsXY());
    if (tileElement == nullptr)
        return nullptr;
    do
    {
        if (tileElement->GetType() != type)
            continue;
        if (tileElement->base_height >= loc.baseZ && tileElement->base_height <= loc.clearanceZ)
            return tileElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

void MapSetTileElement(const TileCoordsXY& tilePos, TileElement* elements)
{
    if (!MapIsLocationValid(tilePos.ToCoordsXY()))
    {
        log_error("Trying to access element outside of range");
        return;
    }
    _tileIndex.SetTile(tilePos, elements);
}

SurfaceElement* MapGetSurfaceElementAt(const CoordsXY& coords)
{
    auto view = TileElementsView<SurfaceElement>(coords);

    return *view.begin();
}

PathElement* MapGetPathElementAt(const TileCoordsXYZ& loc)
{
    for (auto* element : TileElementsView<PathElement>(loc.ToCoordsXY()))
    {
        if (element->IsGhost())
            continue;
        if (element->base_height != loc.z)
            continue;
        return element;
    }
    return nullptr;
}

BannerElement* MapGetBannerElementAt(const CoordsXYZ& bannerPos, uint8_t position)
{
    const auto bannerTilePos = TileCoordsXYZ{ bannerPos };
    for (auto* element : TileElementsView<BannerElement>(bannerPos))
    {
        if (element->base_height != bannerTilePos.z)
            continue;
        if (element->GetPosition() != position)
            continue;
        return element;
    }
    return nullptr;
}

/**
 *
 *  rct2: 0x0068AB4C
 */
void MapInit(const TileCoordsXY& size)
{
    auto numTiles = MAXIMUM_MAP_SIZE_TECHNICAL * MAXIMUM_MAP_SIZE_TECHNICAL;
    SetTileElements(std::vector<TileElement>(numTiles, GetDefaultSurfaceElement()));

    gGrassSceneryTileLoopPosition = 0;
    gWidePathTileLoopPosition = {};
    gMapSize = size;
    gMapBaseZ = 7;
    MapRemoveOutOfRangeElements();
    MapAnimationAutoCreate();

    auto intent = Intent(INTENT_ACTION_MAP);
    ContextBroadcastIntent(&intent);
}

/**
 * Counts the number of surface tiles that offer land ownership rights for sale,
 * but haven't been bought yet. It updates gLandRemainingOwnershipSales and
 * gLandRemainingConstructionSales.
 */
void MapCountRemainingLandRights()
{
    gLandRemainingOwnershipSales = 0;
    gLandRemainingConstructionSales = 0;

    for (int32_t y = 0; y < gMapSize.y; y++)
    {
        for (int32_t x = 0; x < gMapSize.x; x++)
        {
            auto* surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y }.ToCoordsXY());
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
void MapStripGhostFlagFromElements()
{
    for (auto& element : _tileElements)
    {
        element.SetGhost(false);
    }
}

/**
 * Return the absolute height of an element, given its (x,y) coordinates
 *
 * ax: x
 * cx: y
 * dx: return remember to & with 0xFFFF if you don't want water affecting results
 *  rct2: 0x00662783
 */
int16_t TileElementHeight(const CoordsXY& loc)
{
    // Off the map
    if (!MapIsLocationValid(loc))
        return MINIMUM_LAND_HEIGHT_BIG;

    // Get the surface element for the tile
    auto surfaceElement = MapGetSurfaceElementAt(loc);

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

    uint8_t TILE_SIZE = 32;

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
            height += xl / 2;
            break;
        case TILE_ELEMENT_SLOPE_SE_SIDE_UP:
            height += (TILE_SIZE - yl) / 2;
            break;
        case TILE_ELEMENT_SLOPE_NW_SIDE_UP:
            height += yl / 2;
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
                quad = xl + yl - TILE_SIZE;
                break;
            case TILE_ELEMENT_SLOPE_E_CORNER_DN:
                quad_extra = TILE_SIZE - xl + yl;
                quad = yl - xl;
                break;
            case TILE_ELEMENT_SLOPE_N_CORNER_DN:
                quad_extra = (TILE_SIZE - xl) + (TILE_SIZE - yl);
                quad = TILE_SIZE - yl - xl;
                break;
        }

        if (extra_height)
        {
            height += quad_extra / 2;
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
                quad = std::abs(xl - yl);
                break;
            case TILE_ELEMENT_SLOPE_N_S_VALLEY:
                quad = std::abs(xl + yl - TILE_SIZE);
                break;
        }
        height += quad / 2;
    }

    return height;
}

int16_t TileElementWaterHeight(const CoordsXY& loc)
{
    // Off the map
    if (!MapIsLocationValid(loc))
        return 0;

    // Get the surface element for the tile
    auto surfaceElement = MapGetSurfaceElementAt(loc);

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
bool MapCoordIsConnected(const TileCoordsXYZ& loc, uint8_t faceDirection)
{
    TileElement* tileElement = MapGetFirstElementAt(loc);

    if (tileElement == nullptr)
        return false;

    do
    {
        if (tileElement->GetType() != TileElementType::Path)
            continue;

        uint8_t slopeDirection = tileElement->AsPath()->GetSlopeDirection();

        if (tileElement->AsPath()->IsSloped())
        {
            if (slopeDirection == faceDirection)
            {
                if (loc.z == tileElement->base_height + 2)
                    return true;
            }
            else if (DirectionReverse(slopeDirection) == faceDirection && loc.z == tileElement->base_height)
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
void MapUpdatePathWideFlags()
{
    PROFILED_FUNCTION();

    if (gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
    {
        return;
    }

    // Presumably update_path_wide_flags is too computationally expensive to call for every
    // tile every update, so gWidePathTileLoopX and gWidePathTileLoopY store the x and y
    // progress. A maximum of 128 calls is done per update.
    auto x = gWidePathTileLoopPosition.x;
    auto y = gWidePathTileLoopPosition.y;
    for (int32_t i = 0; i < 128; i++)
    {
        FootpathUpdatePathWideFlags({ x, y });

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
    gWidePathTileLoopPosition.x = x;
    gWidePathTileLoopPosition.y = y;
}

/**
 *
 *  rct2: 0x006A7B84
 */
int32_t MapHeightFromSlope(const CoordsXY& coords, int32_t slopeDirection, bool isSloped)
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

bool MapIsLocationValid(const CoordsXY& coords)
{
    const bool is_x_valid = coords.x < MAXIMUM_MAP_SIZE_BIG && coords.x >= 0;
    const bool is_y_valid = coords.y < MAXIMUM_MAP_SIZE_BIG && coords.y >= 0;
    return is_x_valid && is_y_valid;
}

bool MapIsEdge(const CoordsXY& coords)
{
    auto mapSizeUnits = GetMapSizeUnits();
    return (coords.x < 32 || coords.y < 32 || coords.x >= mapSizeUnits.x || coords.y >= mapSizeUnits.y);
}

bool MapCanBuildAt(const CoordsXYZ& loc)
{
    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        return true;
    if (gCheatsSandboxMode)
        return true;
    if (MapIsLocationOwned(loc))
        return true;
    return false;
}

/**
 *
 *  rct2: 0x00664F72
 */
bool MapIsLocationOwned(const CoordsXYZ& loc)
{
    // This check is to avoid throwing lots of messages in logs.
    if (MapIsLocationValid(loc))
    {
        auto* surfaceElement = MapGetSurfaceElementAt(loc);
        if (surfaceElement != nullptr)
        {
            if (surfaceElement->GetOwnership() & OWNERSHIP_OWNED)
                return true;

            if (surfaceElement->GetOwnership() & OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED)
            {
                if (loc.z < surfaceElement->GetBaseZ() || loc.z >= surfaceElement->GetBaseZ() + ConstructionRightsClearanceBig)
                    return true;
            }
        }
    }
    return false;
}

/**
 *
 *  rct2: 0x00664F2C
 */
bool MapIsLocationInPark(const CoordsXY& coords)
{
    if (MapIsLocationValid(coords))
    {
        auto surfaceElement = MapGetSurfaceElementAt(coords);
        if (surfaceElement == nullptr)
            return false;
        if (surfaceElement->GetOwnership() & OWNERSHIP_OWNED)
            return true;
    }
    return false;
}

bool MapIsLocationOwnedOrHasRights(const CoordsXY& loc)
{
    if (MapIsLocationValid(loc))
    {
        auto surfaceElement = MapGetSurfaceElementAt(loc);
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
    {
        0x01, 0x1B, 0x03, 0x1B, 0x05, 0x21, 0x07, 0x21, 0x09, 0x1B, 0x0B, 0x1B, 0x0D, 0x21, 0x20, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x23, 0x18, 0x19, 0x1A, 0x3B, 0x1C, 0x29, 0x24, 0x1F,
    }, // MAP_SELECT_TYPE_CORNER_0
       // (absolute rotation)
    {
        0x02, 0x03, 0x17, 0x17, 0x06, 0x07, 0x17, 0x17, 0x0A, 0x0B, 0x22, 0x22, 0x0E, 0x20, 0x22, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x37, 0x18, 0x19, 0x1A, 0x23, 0x1C, 0x28, 0x26, 0x1F,
    }, // MAP_SELECT_TYPE_CORNER_1
    {
        0x04, 0x05, 0x06, 0x07, 0x1E, 0x24, 0x1E, 0x24, 0x0C, 0x0D, 0x0E, 0x20, 0x1E, 0x24, 0x1E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x26, 0x18, 0x19, 0x1A, 0x21, 0x1C, 0x2C, 0x3E, 0x1F,
    }, // MAP_SELECT_TYPE_CORNER_2
    {
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x20, 0x1D, 0x1D, 0x28, 0x28, 0x1D, 0x1D, 0x28, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x22, 0x18, 0x19, 0x1A, 0x29, 0x1C, 0x3D, 0x2C, 0x1F,
    }, // MAP_SELECT_TYPE_CORNER_3
    {
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x20, 0x20, 0x20, 0x21, 0x20, 0x28, 0x24, 0x20,
    }, // MAP_SELECT_TYPE_FULL
    {
        0x0C, 0x0D, 0x0E, 0x20, 0x0C, 0x0D, 0x0E, 0x20, 0x0C, 0x0D, 0x0E, 0x20, 0x2C, 0x2C, 0x2C, 0x2C,
        0x0C, 0x0D, 0x0E, 0x20, 0x0C, 0x0C, 0x0E, 0x22, 0x0C, 0x0D, 0x0E, 0x21, 0x2C, 0x2C, 0x2C, 0x2C,
    }, // MAP_SELECT_TYPE_EDGE_0
    {
        0x09, 0x09, 0x0B, 0x0B, 0x0D, 0x0D, 0x20, 0x20, 0x09, 0x29, 0x0B, 0x29, 0x0D, 0x29, 0x20, 0x29,
        0x09, 0x09, 0x0B, 0x0B, 0x0D, 0x0D, 0x24, 0x22, 0x09, 0x29, 0x0B, 0x29, 0x0D, 0x29, 0x24, 0x29,
    }, // MAP_SELECT_TYPE_EDGE_1
    {
        0x03, 0x03, 0x03, 0x23, 0x07, 0x07, 0x07, 0x23, 0x0B, 0x0B, 0x0B, 0x23, 0x20, 0x20, 0x20, 0x23,
        0x03, 0x03, 0x03, 0x23, 0x07, 0x07, 0x07, 0x23, 0x0B, 0x0B, 0x0B, 0x23, 0x20, 0x28, 0x24, 0x23,
    }, // MAP_SELECT_TYPE_EDGE_2
    {
        0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x26, 0x26, 0x0E, 0x20, 0x0E, 0x20, 0x0E, 0x20, 0x26, 0x26,
        0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x26, 0x26, 0x0E, 0x20, 0x0E, 0x21, 0x0E, 0x28, 0x26, 0x26,
    }, // MAP_SELECT_TYPE_EDGE_3
};

// 0x00981ABE
// Basically the inverse of the table above.
// 0x1F = new slope
// 0x20 = base height increases
const uint8_t tile_element_lower_styles[9][32] = {
    {
        0x2E, 0x00, 0x2E, 0x02, 0x3E, 0x04, 0x3E, 0x06, 0x2E, 0x08, 0x2E, 0x0A, 0x3E, 0x0C, 0x3E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x06, 0x18, 0x19, 0x1A, 0x0B, 0x1C, 0x0C, 0x3E, 0x1F,
    }, // MAP_SELECT_TYPE_CORNER_0
    {
        0x2D, 0x2D, 0x00, 0x01, 0x2D, 0x2D, 0x04, 0x05, 0x3D, 0x3D, 0x08, 0x09, 0x3D, 0x3D, 0x0C, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x07, 0x18, 0x19, 0x1A, 0x09, 0x1C, 0x3D, 0x0C, 0x1F,
    }, // MAP_SELECT_TYPE_CORNER_1
    {
        0x2B, 0x3B, 0x2B, 0x3B, 0x00, 0x01, 0x02, 0x03, 0x2B, 0x3B, 0x2B, 0x3B, 0x08, 0x09, 0x0A, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x03, 0x18, 0x19, 0x1A, 0x3B, 0x1C, 0x09, 0x0E, 0x1F,
    }, // MAP_SELECT_TYPE_CORNER_2
    {
        0x27, 0x27, 0x37, 0x37, 0x27, 0x27, 0x37, 0x37, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x37, 0x18, 0x19, 0x1A, 0x03, 0x1C, 0x0D, 0x06, 0x1F,
    }, // MAP_SELECT_TYPE_CORNER_3
    {
        0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x0D, 0x0E, 0x00,
    }, // MAP_SELECT_TYPE_FULL
    {
        0x23, 0x23, 0x23, 0x23, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03,
        0x23, 0x23, 0x23, 0x23, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x00, 0x0D, 0x0E, 0x03,
    }, // MAP_SELECT_TYPE_EDGE_0
    {
        0x26, 0x00, 0x26, 0x02, 0x26, 0x04, 0x26, 0x06, 0x00, 0x00, 0x02, 0x02, 0x04, 0x04, 0x06, 0x06,
        0x26, 0x00, 0x26, 0x02, 0x26, 0x04, 0x26, 0x06, 0x00, 0x00, 0x02, 0x0B, 0x04, 0x0D, 0x06, 0x06,
    }, // MAP_SELECT_TYPE_EDGE_1
    {
        0x2C, 0x00, 0x00, 0x00, 0x2C, 0x04, 0x04, 0x04, 0x2C, 0x08, 0x08, 0x08, 0x2C, 0x0C, 0x0C, 0x0C,
        0x2C, 0x00, 0x00, 0x00, 0x2C, 0x04, 0x04, 0x07, 0x2C, 0x08, 0x08, 0x0B, 0x2C, 0x0C, 0x0C, 0x0C,
    }, // MAP_SELECT_TYPE_EDGE_2
    {
        0x29, 0x29, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x29, 0x29, 0x08, 0x09, 0x08, 0x09, 0x08, 0x09,
        0x29, 0x29, 0x00, 0x01, 0x00, 0x01, 0x00, 0x07, 0x29, 0x29, 0x08, 0x09, 0x08, 0x09, 0x0E, 0x09,
    }, // MAP_SELECT_TYPE_EDGE_3
};

int32_t MapGetCornerHeight(int32_t z, int32_t slope, int32_t direction)
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

int32_t TileElementGetCornerHeight(const SurfaceElement* surfaceElement, int32_t direction)
{
    int32_t z = surfaceElement->base_height;
    int32_t slope = surfaceElement->GetSlope();
    return MapGetCornerHeight(z, slope, direction);
}

uint8_t MapGetLowestLandHeight(const MapRange& range)
{
    auto mapSizeMax = GetMapSizeMaxXY();
    MapRange validRange = { std::max(range.GetLeft(), 32), std::max(range.GetTop(), 32),
                            std::min(range.GetRight(), mapSizeMax.x), std::min(range.GetBottom(), mapSizeMax.y) };

    uint8_t min_height = 0xFF;
    for (int32_t yi = validRange.GetTop(); yi <= validRange.GetBottom(); yi += COORDS_XY_STEP)
    {
        for (int32_t xi = validRange.GetLeft(); xi <= validRange.GetRight(); xi += COORDS_XY_STEP)
        {
            auto* surfaceElement = MapGetSurfaceElementAt(CoordsXY{ xi, yi });

            if (surfaceElement != nullptr && min_height > surfaceElement->base_height)
            {
                if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
                {
                    if (!MapIsLocationInPark(CoordsXY{ xi, yi }))
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

uint8_t MapGetHighestLandHeight(const MapRange& range)
{
    auto mapSizeMax = GetMapSizeMaxXY();
    MapRange validRange = { std::max(range.GetLeft(), 32), std::max(range.GetTop(), 32),
                            std::min(range.GetRight(), mapSizeMax.x), std::min(range.GetBottom(), mapSizeMax.y) };

    uint8_t max_height = 0;
    for (int32_t yi = validRange.GetTop(); yi <= validRange.GetBottom(); yi += COORDS_XY_STEP)
    {
        for (int32_t xi = validRange.GetLeft(); xi <= validRange.GetRight(); xi += COORDS_XY_STEP)
        {
            auto* surfaceElement = MapGetSurfaceElementAt(CoordsXY{ xi, yi });
            if (surfaceElement != nullptr)
            {
                if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
                {
                    if (!MapIsLocationInPark(CoordsXY{ xi, yi }))
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

bool MapIsLocationAtEdge(const CoordsXY& loc)
{
    return loc.x < 32 || loc.y < 32 || loc.x >= (MAXIMUM_TILE_START_XY) || loc.y >= (MAXIMUM_TILE_START_XY);
}

/**
 *
 *  rct2: 0x0068B280
 */
void TileElementRemove(TileElement* tileElement)
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
    _tileElementsInUse--;
    if (tileElement == &_tileElements.back())
    {
        _tileElements.pop_back();
    }
}

/**
 *
 *  rct2: 0x00675A8E
 */
void MapRemoveAllRides()
{
    tile_element_iterator it;

    TileElementIteratorBegin(&it);
    do
    {
        switch (it.element->GetType())
        {
            case TileElementType::Path:
                if (it.element->AsPath()->IsQueue())
                {
                    it.element->AsPath()->SetHasQueueBanner(false);
                    it.element->AsPath()->SetRideIndex(RideId::GetNull());
                }
                break;
            case TileElementType::Entrance:
                if (it.element->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE)
                    break;
                [[fallthrough]];
            case TileElementType::Track:
                FootpathQueueChainReset();
                FootpathRemoveEdgesAt(TileCoordsXY{ it.x, it.y }.ToCoordsXY(), it.element);
                TileElementRemove(it.element);
                TileElementIteratorRestartForTile(&it);
                break;
            default:
                break;
        }
    } while (TileElementIteratorNext(&it));
}

/**
 *
 *  rct2: 0x0068AB1B
 */
void MapInvalidateMapSelectionTiles()
{
    if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT))
        return;

    for (const auto& position : gMapSelectionTiles)
        MapInvalidateTileFull(position);
}

static void MapGetBoundingBox(const MapRange& _range, int32_t* left, int32_t* top, int32_t* right, int32_t* bottom)
{
    uint32_t rotation = GetCurrentRotation();
    const std::array corners{
        CoordsXY{ _range.GetLeft(), _range.GetTop() },
        CoordsXY{ _range.GetRight(), _range.GetTop() },
        CoordsXY{ _range.GetRight(), _range.GetBottom() },
        CoordsXY{ _range.GetLeft(), _range.GetBottom() },
    };

    *left = std::numeric_limits<int32_t>::max();
    *top = std::numeric_limits<int32_t>::max();
    *right = std::numeric_limits<int32_t>::min();
    *bottom = std::numeric_limits<int32_t>::min();

    for (const auto& corner : corners)
    {
        auto screenCoord = Translate3DTo2D(rotation, corner);
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
void MapInvalidateSelectionRect()
{
    int32_t x0, y0, x1, y1, left, right, top, bottom;

    if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
        return;

    x0 = gMapSelectPositionA.x + 16;
    y0 = gMapSelectPositionA.y + 16;
    x1 = gMapSelectPositionB.x + 16;
    y1 = gMapSelectPositionB.y + 16;
    MapGetBoundingBox({ x0, y0, x1, y1 }, &left, &top, &right, &bottom);
    left -= 32;
    right += 32;
    bottom += 32;
    top -= 32 + 2080;

    ViewportsInvalidate({ { left, top }, { right, bottom } });
}

static size_t CountElementsOnTile(const CoordsXY& loc)
{
    size_t count = 0;
    auto* element = _tileIndex.GetFirstElementAt(TileCoordsXY(loc));
    do
    {
        count++;
    } while (!(element++)->IsLastForTile());
    return count;
}

static TileElement* AllocateTileElements(size_t numElementsOnTile, size_t numNewElements)
{
    if (!MapCheckFreeElementsAndReorganise(numElementsOnTile, numNewElements))
    {
        log_error("Cannot insert new element");
        return nullptr;
    }

    auto oldSize = _tileElements.size();
    _tileElements.resize(_tileElements.size() + numElementsOnTile + numNewElements);
    _tileElementsInUse += numNewElements;
    return &_tileElements[oldSize];
}

/**
 *
 *  rct2: 0x0068B1F6
 */
TileElement* TileElementInsert(const CoordsXYZ& loc, int32_t occupiedQuadrants, TileElementType type)
{
    const auto& tileLoc = TileCoordsXYZ(loc);

    auto numElementsOnTileOld = CountElementsOnTile(loc);
    auto* newTileElement = AllocateTileElements(numElementsOnTileOld, 1);
    auto* originalTileElement = _tileIndex.GetFirstElementAt(tileLoc);
    if (newTileElement == nullptr)
    {
        return nullptr;
    }

    // Set tile index pointer to point to new element block
    _tileIndex.SetTile(tileLoc, newTileElement);

    bool isLastForTile = false;
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
    auto* insertedElement = newTileElement;
    newTileElement->type = 0;
    newTileElement->SetType(type);
    newTileElement->SetBaseZ(loc.z);
    newTileElement->Flags = 0;
    newTileElement->SetLastForTile(isLastForTile);
    newTileElement->SetOccupiedQuadrants(occupiedQuadrants);
    newTileElement->SetClearanceZ(loc.z);
    newTileElement->owner = 0;
    std::memset(&newTileElement->pad_05, 0, sizeof(newTileElement->pad_05));
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

    return insertedElement;
}

/**
 * Updates grass length, scenery age and jumping fountains.
 *
 *  rct2: 0x006646E1
 */
void MapUpdateTiles()
{
    PROFILED_FUNCTION();

    int32_t ignoreScreenFlags = SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER;
    if (gScreenFlags & ignoreScreenFlags)
        return;

    // Update 43 more tiles (for each 256x256 block)
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

        // Repeat for each 256x256 block on the map
        for (int32_t blockY = 0; blockY < gMapSize.y; blockY += 256)
        {
            for (int32_t blockX = 0; blockX < gMapSize.x; blockX += 256)
            {
                auto mapPos = TileCoordsXY{ blockX + x, blockY + y }.ToCoordsXY();
                auto* surfaceElement = MapGetSurfaceElementAt(mapPos);
                if (surfaceElement != nullptr)
                {
                    surfaceElement->UpdateGrassLength(mapPos);
                    SceneryUpdateTile(mapPos);
                }
            }
        }

        gGrassSceneryTileLoopPosition++;
        gGrassSceneryTileLoopPosition &= 0xFFFF;
    }
}

void MapRemoveProvisionalElements()
{
    PROFILED_FUNCTION();

    if (gProvisionalFootpath.Flags & PROVISIONAL_PATH_FLAG_1)
    {
        FootpathProvisionalRemove();
        gProvisionalFootpath.Flags |= PROVISIONAL_PATH_FLAG_1;
    }
    if (WindowFindByClass(WindowClass::RideConstruction) != nullptr)
    {
        ride_remove_provisional_track_piece();
        RideEntranceExitRemoveGhost();
    }
    // This is in non performant so only make network games suffer for it
    // non networked games do not need this as its to prevent desyncs.
    if ((network_get_mode() != NETWORK_MODE_NONE) && WindowFindByClass(WindowClass::TrackDesignPlace) != nullptr)
    {
        auto intent = Intent(INTENT_ACTION_TRACK_DESIGN_REMOVE_PROVISIONAL);
        ContextBroadcastIntent(&intent);
    }
}

void MapRestoreProvisionalElements()
{
    PROFILED_FUNCTION();

    if (gProvisionalFootpath.Flags & PROVISIONAL_PATH_FLAG_1)
    {
        gProvisionalFootpath.Flags &= ~PROVISIONAL_PATH_FLAG_1;
        FootpathProvisionalSet(
            gProvisionalFootpath.SurfaceIndex, gProvisionalFootpath.RailingsIndex, gProvisionalFootpath.Position,
            gProvisionalFootpath.Slope, gProvisionalFootpath.ConstructFlags);
    }
    if (WindowFindByClass(WindowClass::RideConstruction) != nullptr)
    {
        ride_restore_provisional_track_piece();
        RideEntranceExitPlaceProvisionalGhost();
    }
    // This is in non performant so only make network games suffer for it
    // non networked games do not need this as its to prevent desyncs.
    if ((network_get_mode() != NETWORK_MODE_NONE) && WindowFindByClass(WindowClass::TrackDesignPlace) != nullptr)
    {
        auto intent = Intent(INTENT_ACTION_TRACK_DESIGN_RESTORE_PROVISIONAL);
        ContextBroadcastIntent(&intent);
    }
}

/**
 * Removes elements that are out of the map size range and crops the park perimeter.
 *  rct2: 0x0068ADBC
 */
void MapRemoveOutOfRangeElements()
{
    auto mapSizeMax = GetMapSizeMaxXY();

    // Ensure that we can remove elements
    //
    // NOTE: This is only a workaround for non-networked games.
    // Map resize has to become its own Game Action to properly solve this issue.
    //
    bool buildState = gCheatsBuildInPauseMode;
    gCheatsBuildInPauseMode = true;

    for (int32_t y = MAXIMUM_MAP_SIZE_BIG - COORDS_XY_STEP; y >= 0; y -= COORDS_XY_STEP)
    {
        for (int32_t x = MAXIMUM_MAP_SIZE_BIG - COORDS_XY_STEP; x >= 0; x -= COORDS_XY_STEP)
        {
            if (x == 0 || y == 0 || x >= mapSizeMax.x || y >= mapSizeMax.y)
            {
                // Note this purposely does not use LandSetRightsAction as X Y coordinates are outside of normal range.
                auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ x, y });
                if (surfaceElement != nullptr)
                {
                    surfaceElement->SetOwnership(OWNERSHIP_UNOWNED);
                    ParkUpdateFencesAroundTile({ x, y });
                }
                ClearElementsAt({ x, y });
            }
        }
    }

    // Reset cheat state
    gCheatsBuildInPauseMode = buildState;
}

static void MapExtendBoundarySurfaceExtendTile(const SurfaceElement& sourceTile, SurfaceElement& destTile)
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
 * Copies the terrain and slope from the Y edge of the map to the new tiles. Used when increasing the size of the map.
 */
void MapExtendBoundarySurfaceY()
{
    auto y = gMapSize.y - 2;
    for (auto x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; x++)
    {
        auto existingTileElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y - 1 }.ToCoordsXY());
        auto newTileElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y }.ToCoordsXY());

        if (existingTileElement != nullptr && newTileElement != nullptr)
        {
            MapExtendBoundarySurfaceExtendTile(*existingTileElement, *newTileElement);
        }

        ParkUpdateFences({ x << 5, y << 5 });
    }
}

/**
 * Copies the terrain and slope from the X edge of the map to the new tiles. Used when increasing the size of the map.
 */
void MapExtendBoundarySurfaceX()
{
    auto x = gMapSize.x - 2;
    for (auto y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; y++)
    {
        auto existingTileElement = MapGetSurfaceElementAt(TileCoordsXY{ x - 1, y }.ToCoordsXY());
        auto newTileElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y }.ToCoordsXY());
        if (existingTileElement != nullptr && newTileElement != nullptr)
        {
            MapExtendBoundarySurfaceExtendTile(*existingTileElement, *newTileElement);
        }
        ParkUpdateFences({ x << 5, y << 5 });
    }
}

/**
 * Clears the provided element properly from a certain tile, and updates
 * the pointer (when needed) passed to this function to point to the next element.
 */
static void ClearElementAt(const CoordsXY& loc, TileElement** elementPtr)
{
    TileElement* element = *elementPtr;
    switch (element->GetType())
    {
        case TileElementType::Surface:
            element->base_height = MINIMUM_LAND_HEIGHT;
            element->clearance_height = MINIMUM_LAND_HEIGHT;
            element->owner = 0;
            element->AsSurface()->SetSlope(TILE_ELEMENT_SLOPE_FLAT);
            element->AsSurface()->SetSurfaceStyle(0);
            element->AsSurface()->SetEdgeStyle(0);
            element->AsSurface()->SetGrassLength(GRASS_LENGTH_CLEAR_0);
            element->AsSurface()->SetOwnership(OWNERSHIP_UNOWNED);
            element->AsSurface()->SetParkFences(0);
            element->AsSurface()->SetWaterHeight(0);
            // Because this element is not completely removed, the pointer must be updated manually
            // The rest of the elements are removed from the array, so the pointer doesn't need to be updated.
            (*elementPtr)++;
            break;
        case TileElementType::Entrance:
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
            auto result = GameActions::ExecuteNested(&parkEntranceRemoveAction);
            // If asking nicely did not work, forcibly remove this to avoid an infinite loop.
            if (result.Error != GameActions::Status::Ok)
            {
                TileElementRemove(element);
            }
            break;
        }
        case TileElementType::Wall:
        {
            CoordsXYZD wallLocation = { loc.x, loc.y, element->GetBaseZ(), element->GetDirection() };
            auto wallRemoveAction = WallRemoveAction(wallLocation);
            auto result = GameActions::ExecuteNested(&wallRemoveAction);
            // If asking nicely did not work, forcibly remove this to avoid an infinite loop.
            if (result.Error != GameActions::Status::Ok)
            {
                TileElementRemove(element);
            }
        }
        break;
        case TileElementType::LargeScenery:
        {
            auto removeSceneryAction = LargeSceneryRemoveAction(
                { loc.x, loc.y, element->GetBaseZ(), element->GetDirection() }, element->AsLargeScenery()->GetSequenceIndex());
            auto result = GameActions::ExecuteNested(&removeSceneryAction);
            // If asking nicely did not work, forcibly remove this to avoid an infinite loop.
            if (result.Error != GameActions::Status::Ok)
            {
                TileElementRemove(element);
            }
        }
        break;
        case TileElementType::Banner:
        {
            auto bannerRemoveAction = BannerRemoveAction(
                { loc.x, loc.y, element->GetBaseZ(), element->AsBanner()->GetPosition() });
            auto result = GameActions::ExecuteNested(&bannerRemoveAction);
            // If asking nicely did not work, forcibly remove this to avoid an infinite loop.
            if (result.Error != GameActions::Status::Ok)
            {
                TileElementRemove(element);
            }
            break;
        }
        default:
            TileElementRemove(element);
            break;
    }
}

/**
 * Clears all elements properly from a certain tile.
 *  rct2: 0x0068AE2A
 */
static void ClearElementsAt(const CoordsXY& loc)
{
    // Remove the spawn point (if there is one in the current tile)
    gPeepSpawns.erase(
        std::remove_if(
            gPeepSpawns.begin(), gPeepSpawns.end(),
            [loc](const CoordsXY& spawn) { return spawn.ToTileStart() == loc.ToTileStart(); }),
        gPeepSpawns.end());

    TileElement* tileElement = MapGetFirstElementAt(loc);
    if (tileElement == nullptr)
        return;

    // Remove all elements except the last one
    while (!tileElement->IsLastForTile())
        ClearElementAt(loc, &tileElement);

    // Remove the last element
    ClearElementAt(loc, &tileElement);
}

int32_t MapGetHighestZ(const CoordsXY& loc)
{
    auto surfaceElement = MapGetSurfaceElementAt(loc);
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

LargeSceneryElement* MapGetLargeScenerySegment(const CoordsXYZD& sceneryPos, int32_t sequence)
{
    TileElement* tileElement = MapGetFirstElementAt(sceneryPos);
    if (tileElement == nullptr)
    {
        return nullptr;
    }
    auto sceneryTilePos = TileCoordsXYZ{ sceneryPos };
    do
    {
        if (tileElement->GetType() != TileElementType::LargeScenery)
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

EntranceElement* MapGetParkEntranceElementAt(const CoordsXYZ& entranceCoords, bool ghost)
{
    auto entranceTileCoords = TileCoordsXYZ(entranceCoords);
    TileElement* tileElement = MapGetFirstElementAt(entranceCoords);
    if (tileElement != nullptr)
    {
        do
        {
            if (tileElement->GetType() != TileElementType::Entrance)
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

EntranceElement* MapGetRideEntranceElementAt(const CoordsXYZ& entranceCoords, bool ghost)
{
    auto entranceTileCoords = TileCoordsXYZ{ entranceCoords };
    TileElement* tileElement = MapGetFirstElementAt(entranceCoords);
    if (tileElement != nullptr)
    {
        do
        {
            if (tileElement->GetType() != TileElementType::Entrance)
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

EntranceElement* MapGetRideExitElementAt(const CoordsXYZ& exitCoords, bool ghost)
{
    auto exitTileCoords = TileCoordsXYZ{ exitCoords };
    TileElement* tileElement = MapGetFirstElementAt(exitCoords);
    if (tileElement != nullptr)
    {
        do
        {
            if (tileElement->GetType() != TileElementType::Entrance)
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

SmallSceneryElement* MapGetSmallSceneryElementAt(const CoordsXYZ& sceneryCoords, int32_t type, uint8_t quadrant)
{
    auto sceneryTileCoords = TileCoordsXYZ{ sceneryCoords };
    TileElement* tileElement = MapGetFirstElementAt(sceneryCoords);
    if (tileElement != nullptr)
    {
        do
        {
            if (tileElement->GetType() != TileElementType::SmallScenery)
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

std::optional<CoordsXYZ> MapLargeSceneryGetOrigin(
    const CoordsXYZD& sceneryPos, int32_t sequence, LargeSceneryElement** outElement)
{
    rct_large_scenery_tile* tile;

    auto tileElement = MapGetLargeScenerySegment(sceneryPos, sequence);
    if (tileElement == nullptr)
        return std::nullopt;

    auto* sceneryEntry = tileElement->GetEntry();
    tile = &sceneryEntry->tiles[sequence];

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
bool MapLargeScenerySignSetColour(const CoordsXYZD& signPos, int32_t sequence, uint8_t mainColour, uint8_t textColour)
{
    LargeSceneryElement* tileElement;
    rct_large_scenery_tile *sceneryTiles, *tile;

    auto sceneryOrigin = MapLargeSceneryGetOrigin(signPos, sequence, &tileElement);
    if (!sceneryOrigin)
    {
        return false;
    }

    auto* sceneryEntry = tileElement->GetEntry();
    sceneryTiles = sceneryEntry->tiles;

    // Iterate through each tile of the large scenery element
    sequence = 0;
    for (tile = sceneryTiles; tile->x_offset != -1; tile++, sequence++)
    {
        CoordsXY offsetPos{ tile->x_offset, tile->y_offset };
        auto rotatedOffsetPos = offsetPos.Rotate(signPos.direction);

        auto tmpSignPos = CoordsXYZD{ sceneryOrigin->x + rotatedOffsetPos.x, sceneryOrigin->y + rotatedOffsetPos.y,
                                      sceneryOrigin->z + tile->z_offset, signPos.direction };
        tileElement = MapGetLargeScenerySegment(tmpSignPos, sequence);
        if (tileElement != nullptr)
        {
            tileElement->SetPrimaryColour(mainColour);
            tileElement->SetSecondaryColour(textColour);

            MapInvalidateTile({ tmpSignPos, tileElement->GetBaseZ(), tileElement->GetClearanceZ() });
        }
    }

    return true;
}

static ScreenCoordsXY Translate3DTo2D(int32_t rotation, const CoordsXY& pos)
{
    return Translate3DTo2DWithZ(rotation, CoordsXYZ{ pos, 0 });
}

ScreenCoordsXY Translate3DTo2DWithZ(int32_t rotation, const CoordsXYZ& pos)
{
    auto rotated = pos.Rotate(rotation);
    // Use right shift to avoid issues like #9301
    return ScreenCoordsXY{ rotated.y - rotated.x, ((rotated.x + rotated.y) >> 1) - pos.z };
}

static void MapInvalidateTileUnderZoom(int32_t x, int32_t y, int32_t z0, int32_t z1, ZoomLevel maxZoom)
{
    if (gOpenRCT2Headless)
        return;

    int32_t x1, y1, x2, y2;

    x += 16;
    y += 16;
    auto screenCoord = Translate3DTo2D(GetCurrentRotation(), { x, y });

    x1 = screenCoord.x - 32;
    y1 = screenCoord.y - 32 - z1;
    x2 = screenCoord.x + 32;
    y2 = screenCoord.y + 32 - z0;

    ViewportsInvalidate({ { x1, y1 }, { x2, y2 } }, maxZoom);
}

/**
 *
 *  rct2: 0x006EC847
 */
void MapInvalidateTile(const CoordsXYRangedZ& tilePos)
{
    MapInvalidateTileUnderZoom(tilePos.x, tilePos.y, tilePos.baseZ, tilePos.clearanceZ, ZoomLevel{ -1 });
}

/**
 *
 *  rct2: 0x006ECB60
 */
void MapInvalidateTileZoom1(const CoordsXYRangedZ& tilePos)
{
    MapInvalidateTileUnderZoom(tilePos.x, tilePos.y, tilePos.baseZ, tilePos.clearanceZ, ZoomLevel{ 1 });
}

/**
 *
 *  rct2: 0x006EC9CE
 */
void MapInvalidateTileZoom0(const CoordsXYRangedZ& tilePos)
{
    MapInvalidateTileUnderZoom(tilePos.x, tilePos.y, tilePos.baseZ, tilePos.clearanceZ, ZoomLevel{ 0 });
}

/**
 *
 *  rct2: 0x006EC6D7
 */
void MapInvalidateTileFull(const CoordsXY& tilePos)
{
    MapInvalidateTile({ tilePos, 0, 2080 });
}

void MapInvalidateElement(const CoordsXY& elementPos, TileElement* tileElement)
{
    MapInvalidateTile({ elementPos, tileElement->GetBaseZ(), tileElement->GetClearanceZ() });
}

void MapInvalidateRegion(const CoordsXY& mins, const CoordsXY& maxs)
{
    int32_t x0, y0, x1, y1, left, right, top, bottom;

    x0 = mins.x + 16;
    y0 = mins.y + 16;

    x1 = maxs.x + 16;
    y1 = maxs.y + 16;

    MapGetBoundingBox({ x0, y0, x1, y1 }, &left, &top, &right, &bottom);

    left -= 32;
    right += 32;
    bottom += 32;
    top -= 32 + 2080;

    ViewportsInvalidate({ { left, top }, { right, bottom } });
}

int32_t MapGetTileSide(const CoordsXY& mapPos)
{
    int32_t subMapX = mapPos.x & (32 - 1);
    int32_t subMapY = mapPos.y & (32 - 1);
    return (subMapX < subMapY) ? ((subMapX + subMapY) < 32 ? 0 : 1) : ((subMapX + subMapY) < 32 ? 3 : 2);
}

int32_t MapGetTileQuadrant(const CoordsXY& mapPos)
{
    int32_t subMapX = mapPos.x & (32 - 1);
    int32_t subMapY = mapPos.y & (32 - 1);
    return (subMapX > 16) ? (subMapY < 16 ? 1 : 0) : (subMapY < 16 ? 2 : 3);
}

/**
 *
 *  rct2: 0x00693BFF
 */
bool MapSurfaceIsBlocked(const CoordsXY& mapCoords)
{
    if (!MapIsLocationValid(mapCoords))
        return true;

    auto surfaceElement = MapGetSurfaceElementAt(mapCoords);

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

        if (tileElement->GetType() == TileElementType::Path || tileElement->GetType() == TileElementType::Wall)
            continue;

        if (tileElement->GetType() != TileElementType::SmallScenery)
            return true;

        auto* sceneryEntry = tileElement->AsSmallScenery()->GetEntry();
        if (sceneryEntry == nullptr)
        {
            return false;
        }
        if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE))
            return true;
    }
    return false;
}

/* Clears all map elements, to be used before generating a new map */
void MapClearAllElements()
{
    for (int32_t y = 0; y < MAXIMUM_MAP_SIZE_BIG; y += COORDS_XY_STEP)
    {
        for (int32_t x = 0; x < MAXIMUM_MAP_SIZE_BIG; x += COORDS_XY_STEP)
        {
            ClearElementsAt({ x, y });
        }
    }
}

/**
 * Gets the track element at x, y, z.
 * @param x x units, not tiles.
 * @param y y units, not tiles.
 * @param z Base height.
 */
TrackElement* MapGetTrackElementAt(const CoordsXYZ& trackPos)
{
    TileElement* tileElement = MapGetFirstElementAt(trackPos);
    if (tileElement == nullptr)
        return nullptr;
    do
    {
        if (tileElement->GetType() != TileElementType::Track)
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
TileElement* MapGetTrackElementAtOfType(const CoordsXYZ& trackPos, track_type_t trackType)
{
    TileElement* tileElement = MapGetFirstElementAt(trackPos);
    if (tileElement == nullptr)
        return nullptr;
    auto trackTilePos = TileCoordsXYZ{ trackPos };
    do
    {
        if (tileElement->GetType() != TileElementType::Track)
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
TileElement* MapGetTrackElementAtOfTypeSeq(const CoordsXYZ& trackPos, track_type_t trackType, int32_t sequence)
{
    TileElement* tileElement = MapGetFirstElementAt(trackPos);
    auto trackTilePos = TileCoordsXYZ{ trackPos };
    do
    {
        if (tileElement == nullptr)
            break;
        if (tileElement->GetType() != TileElementType::Track)
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

TrackElement* MapGetTrackElementAtOfType(const CoordsXYZD& location, track_type_t trackType)
{
    auto tileElement = MapGetFirstElementAt(location);
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

TrackElement* MapGetTrackElementAtOfTypeSeq(const CoordsXYZD& location, track_type_t trackType, int32_t sequence)
{
    auto tileElement = MapGetFirstElementAt(location);
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
TileElement* MapGetTrackElementAtOfTypeFromRide(const CoordsXYZ& trackPos, track_type_t trackType, RideId rideIndex)
{
    TileElement* tileElement = MapGetFirstElementAt(trackPos);
    if (tileElement == nullptr)
        return nullptr;
    auto trackTilePos = TileCoordsXYZ{ trackPos };
    do
    {
        if (tileElement->GetType() != TileElementType::Track)
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
TileElement* MapGetTrackElementAtFromRide(const CoordsXYZ& trackPos, RideId rideIndex)
{
    TileElement* tileElement = MapGetFirstElementAt(trackPos);
    if (tileElement == nullptr)
        return nullptr;
    auto trackTilePos = TileCoordsXYZ{ trackPos };
    do
    {
        if (tileElement->GetType() != TileElementType::Track)
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
TileElement* MapGetTrackElementAtWithDirectionFromRide(const CoordsXYZD& trackPos, RideId rideIndex)
{
    TileElement* tileElement = MapGetFirstElementAt(trackPos);
    if (tileElement == nullptr)
        return nullptr;
    auto trackTilePos = TileCoordsXYZ{ trackPos };
    do
    {
        if (tileElement->GetType() != TileElementType::Track)
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

WallElement* MapGetWallElementAt(const CoordsXYRangedZ& coords)
{
    auto tileElement = MapGetFirstElementAt(coords);

    if (tileElement != nullptr)
    {
        do
        {
            if (tileElement->GetType() == TileElementType::Wall && coords.baseZ < tileElement->GetClearanceZ()
                && coords.clearanceZ > tileElement->GetBaseZ())
            {
                return tileElement->AsWall();
            }
        } while (!(tileElement++)->IsLastForTile());
    }

    return nullptr;
}

WallElement* MapGetWallElementAt(const CoordsXYZD& wallCoords)
{
    auto tileWallCoords = TileCoordsXYZ(wallCoords);
    TileElement* tileElement = MapGetFirstElementAt(wallCoords);
    if (tileElement == nullptr)
        return nullptr;
    do
    {
        if (tileElement->GetType() != TileElementType::Wall)
            continue;
        if (tileElement->base_height != tileWallCoords.z)
            continue;
        if (tileElement->GetDirection() != wallCoords.direction)
            continue;

        return tileElement->AsWall();
    } while (!(tileElement++)->IsLastForTile());
    return nullptr;
}

uint16_t CheckMaxAllowableLandRightsForTile(const CoordsXYZ& tileMapPos)
{
    TileElement* tileElement = MapGetFirstElementAt(tileMapPos);
    uint16_t destOwnership = OWNERSHIP_OWNED;

    // Sometimes done deliberately.
    if (tileElement == nullptr)
    {
        return OWNERSHIP_OWNED;
    }

    auto tilePos = TileCoordsXYZ{ tileMapPos };
    do
    {
        auto type = tileElement->GetType();
        if (type == TileElementType::Path
            || (type == TileElementType::Entrance
                && tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE))
        {
            destOwnership = OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED;
            // Do not own construction rights if too high/below surface
            if (tileElement->base_height - ConstructionRightsClearanceSmall > tilePos.z || tileElement->base_height < tilePos.z)
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

void FixLandOwnershipTilesWithOwnership(std::initializer_list<TileCoordsXY> tiles, uint8_t ownership, bool doNotDowngrade)
{
    for (const TileCoordsXY* tile = tiles.begin(); tile != tiles.end(); ++tile)
    {
        auto surfaceElement = MapGetSurfaceElementAt(tile->ToCoordsXY());
        if (surfaceElement != nullptr)
        {
            if (doNotDowngrade && surfaceElement->GetOwnership() == OWNERSHIP_OWNED)
                continue;

            surfaceElement->SetOwnership(ownership);
            ParkUpdateFencesAroundTile({ (*tile).x * 32, (*tile).y * 32 });
        }
    }
}

MapRange ClampRangeWithinMap(const MapRange& range)
{
    auto mapSizeMax = GetMapSizeMaxXY();
    auto aX = std::max<decltype(range.GetLeft())>(COORDS_XY_STEP, range.GetLeft());
    auto bX = std::min<decltype(range.GetRight())>(mapSizeMax.x, range.GetRight());
    auto aY = std::max<decltype(range.GetTop())>(COORDS_XY_STEP, range.GetTop());
    auto bY = std::min<decltype(range.GetBottom())>(mapSizeMax.y, range.GetBottom());
    MapRange validRange = MapRange{ aX, aY, bX, bY };
    return validRange;
}
