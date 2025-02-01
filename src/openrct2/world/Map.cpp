/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Map.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Diagnostic.h"
#include "../Game.h"
#include "../GameState.h"
#include "../Input.h"
#include "../OpenRCT2.h"
#include "../actions/BannerRemoveAction.h"
#include "../actions/LargeSceneryRemoveAction.h"
#include "../actions/ParkEntranceRemoveAction.h"
#include "../actions/WallRemoveAction.h"
#include "../audio/audio.h"
#include "../core/EnumUtils.hpp"
#include "../core/Guard.hpp"
#include "../entity/Duck.h"
#include "../entity/EntityTweener.h"
#include "../entity/Fountain.h"
#include "../entity/PatrolArea.h"
#include "../entity/Staff.h"
#include "../interface/Cursors.h"
#include "../interface/Viewport.h"
#include "../localisation/Localisation.Date.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../object/LargeSceneryEntry.h"
#include "../object/ObjectManager.h"
#include "../object/SmallSceneryEntry.h"
#include "../object/TerrainSurfaceObject.h"
#include "../profiling/Profiling.h"
#include "../ride/RideConstruction.h"
#include "../ride/RideData.h"
#include "../ride/RideManager.hpp"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../ride/TrackDesign.h"
#include "../scenario/Scenario.h"
#include "../windows/Intent.h"
#include "../world/TilePointerIndex.hpp"
#include "Banner.h"
#include "Climate.h"
#include "Entrance.h"
#include "Footpath.h"
#include "MapAnimation.h"
#include "Park.h"
#include "Scenery.h"
#include "TileElementsView.h"
#include "TileInspector.h"
#include "tile_element/BannerElement.h"
#include "tile_element/EntranceElement.h"
#include "tile_element/LargeSceneryElement.h"
#include "tile_element/PathElement.h"
#include "tile_element/Slope.h"
#include "tile_element/SmallSceneryElement.h"
#include "tile_element/SurfaceElement.h"
#include "tile_element/TrackElement.h"

#include <iterator>
#include <memory>

using namespace OpenRCT2;

/**
 * Replaces 0x00993CCC, 0x00993CCE
 */
// clang-format off
const std::array<CoordsXY, 8> CoordsDirectionDelta = {
    CoordsXY{ -kCoordsXYStep, 0 },
    CoordsXY{               0, +kCoordsXYStep },
    CoordsXY{ +kCoordsXYStep, 0 },
    CoordsXY{               0, -kCoordsXYStep },
    CoordsXY{ -kCoordsXYStep, +kCoordsXYStep },
    CoordsXY{ +kCoordsXYStep, +kCoordsXYStep },
    CoordsXY{ +kCoordsXYStep, -kCoordsXYStep },
    CoordsXY{ -kCoordsXYStep, -kCoordsXYStep }
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

std::vector<CoordsXY> gMapSelectionTiles;

uint32_t gLandRemainingOwnershipSales;
uint32_t gLandRemainingConstructionSales;

bool gMapLandRightsUpdateSuccess;

static TilePointerIndex<TileElement> _tileIndex;
static TilePointerIndex<TileElement> _tileIndexStash;
static std::vector<TileElement> _tileElementsStash;
static size_t _tileElementsInUse;
static size_t _tileElementsInUseStash;
static TileCoordsXY _mapSizeStash;

void StashMap()
{
    auto& gameState = GetGameState();
    _tileIndexStash = std::move(_tileIndex);
    _tileElementsStash = std::move(gameState.TileElements);
    _mapSizeStash = gameState.MapSize;
    _tileElementsInUseStash = _tileElementsInUse;
}

void UnstashMap()
{
    auto& gameState = GetGameState();
    _tileIndex = std::move(_tileIndexStash);
    gameState.TileElements = std::move(_tileElementsStash);
    gameState.MapSize = _mapSizeStash;
    _tileElementsInUse = _tileElementsInUseStash;
}

CoordsXY GetMapSizeUnits()
{
    auto& gameState = GetGameState();
    return { (gameState.MapSize.x - 1) * kCoordsXYStep, (gameState.MapSize.y - 1) * kCoordsXYStep };
}
CoordsXY GetMapSizeMinus2()
{
    auto& gameState = GetGameState();
    return { (gameState.MapSize.x * kCoordsXYStep) + (8 * kCoordsXYStep - 2),
             (gameState.MapSize.y * kCoordsXYStep) + (8 * kCoordsXYStep - 2) };
}
CoordsXY GetMapSizeMaxXY()
{
    return GetMapSizeUnits() - CoordsXY{ 1, 1 };
}

const std::vector<TileElement>& GetTileElements()
{
    return GetGameState().TileElements;
}

void SetTileElements(GameState_t& gameState, std::vector<TileElement>&& tileElements)
{
    gameState.TileElements = std::move(tileElements);
    _tileIndex = TilePointerIndex<TileElement>(
        kMaximumMapSizeTechnical, gameState.TileElements.data(), gameState.TileElements.size());
    _tileElementsInUse = gameState.TileElements.size();
}

static TileElement GetDefaultSurfaceElement()
{
    TileElement el;
    el.ClearAs(TileElementType::Surface);
    el.SetLastForTile(true);
    el.BaseHeight = 14;
    el.ClearanceHeight = 14;
    el.AsSurface()->SetWaterHeight(0);
    el.AsSurface()->SetSlope(kTileSlopeFlat);
    el.AsSurface()->SetGrassLength(GRASS_LENGTH_CLEAR_0);
    el.AsSurface()->SetOwnership(OWNERSHIP_UNOWNED);
    el.AsSurface()->SetParkFences(0);
    el.AsSurface()->SetSurfaceObjectIndex(0);
    el.AsSurface()->SetEdgeObjectIndex(0);
    return el;
}

std::vector<TileElement> GetReorganisedTileElementsWithoutGhosts()
{
    std::vector<TileElement> newElements;
    newElements.reserve(std::max(MIN_TILE_ELEMENTS, GetGameState().TileElements.size()));
    for (int32_t y = 0; y < kMaximumMapSizeTechnical; y++)
    {
        for (int32_t x = 0; x < kMaximumMapSizeTechnical; x++)
        {
            auto oldSize = newElements.size();

            // Add all non-ghost elements
            const auto* element = MapGetFirstElementAt(TileCoordsXY{ x, y });
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

static void ReorganiseTileElements(GameState_t& gameState, size_t capacity)
{
    ContextSetCurrentCursor(CursorID::ZZZ);

    std::vector<TileElement> newElements;
    newElements.reserve(std::max(MIN_TILE_ELEMENTS, capacity));
    for (int32_t y = 0; y < kMaximumMapSizeTechnical; y++)
    {
        for (int32_t x = 0; x < kMaximumMapSizeTechnical; x++)
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

    SetTileElements(gameState, std::move(newElements));
}

static void ReorganiseTileElements(size_t capacity)
{
    auto& gameState = GetGameState();
    ReorganiseTileElements(gameState, capacity);
}

void ReorganiseTileElements()
{
    auto& gameState = GetGameState();
    ReorganiseTileElements(gameState, gameState.TileElements.size());
}

static bool MapCheckFreeElementsAndReorganise(size_t numElementsOnTile, size_t numNewElements)
{
    // Check hard cap on num in use tiles (this would be the size of _tileElements immediately after a reorg)
    if (_tileElementsInUse + numNewElements > kMaxTileElements)
    {
        return false;
    }

    auto& gameState = GetGameState();
    auto totalElementsRequired = numElementsOnTile + numNewElements;
    auto freeElements = gameState.TileElements.capacity() - gameState.TileElements.size();
    if (freeElements >= totalElementsRequired)
    {
        return true;
    }

    // if space issue is due to fragmentation then Reorg Tiles without increasing capacity
    if (gameState.TileElements.size() > totalElementsRequired + _tileElementsInUse)
    {
        ReorganiseTileElements();
        // This check is not expected to fail
        freeElements = gameState.TileElements.capacity() - gameState.TileElements.size();
        if (freeElements >= totalElementsRequired)
        {
            return true;
        }
    }

    // Capacity must increase to handle the space (Note capacity can go above kMaxTileElements)
    auto newCapacity = gameState.TileElements.capacity() * 2;
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

void TileElementIteratorBegin(TileElementIterator* it)
{
    it->x = 1;
    it->y = 1;
    it->element = MapGetFirstElementAt(TileCoordsXY{ 1, 1 });
}

int32_t TileElementIteratorNext(TileElementIterator* it)
{
    if (it->element == nullptr)
    {
        it->element = MapGetFirstElementAt(TileCoordsXY{ it->x, it->y });
        return it->element == nullptr ? 0 : 1;
    }

    if (!it->element->IsLastForTile())
    {
        it->element++;
        return 1;
    }

    auto& gameState = GetGameState();
    if (it->y < (gameState.MapSize.y - 2))
    {
        it->y++;
        it->element = MapGetFirstElementAt(TileCoordsXY{ it->x, it->y });
        return it->element == nullptr ? 0 : 1;
    }

    if (it->x < (gameState.MapSize.x - 2))
    {
        it->y = 1;
        it->x++;
        it->element = MapGetFirstElementAt(TileCoordsXY{ it->x, it->y });
        return it->element == nullptr ? 0 : 1;
    }

    return 0;
}

void TileElementIteratorRestartForTile(TileElementIterator* it)
{
    it->element = nullptr;
}

static bool IsTileLocationValid(const TileCoordsXY& coords)
{
    const bool is_x_valid = coords.x < kMaximumMapSizeTechnical && coords.x >= 0;
    const bool is_y_valid = coords.y < kMaximumMapSizeTechnical && coords.y >= 0;
    return is_x_valid && is_y_valid;
}

TileElement* MapGetFirstElementAt(const TileCoordsXY& tilePos)
{
    if (!IsTileLocationValid(tilePos))
    {
        LOG_VERBOSE("Trying to access element outside of range");
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
    TileElement* tileElement = MapGetFirstElementAt(loc);
    if (tileElement == nullptr)
        return nullptr;
    do
    {
        if (tileElement->GetType() != type)
            continue;
        if (tileElement->BaseHeight >= loc.baseZ && tileElement->BaseHeight <= loc.clearanceZ)
            return tileElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

void MapSetTileElement(const TileCoordsXY& tilePos, TileElement* elements)
{
    if (!MapIsLocationValid(tilePos.ToCoordsXY()))
    {
        LOG_ERROR("Trying to access element outside of range");
        return;
    }
    _tileIndex.SetTile(tilePos, elements);
}

SurfaceElement* MapGetSurfaceElementAt(const TileCoordsXY& coords)
{
    auto view = TileElementsView<SurfaceElement>(coords);

    return *view.begin();
}

SurfaceElement* MapGetSurfaceElementAt(const CoordsXY& coords)
{
    return MapGetSurfaceElementAt(TileCoordsXY{ coords });
}

PathElement* MapGetPathElementAt(const TileCoordsXYZ& loc)
{
    for (auto* element : TileElementsView<PathElement>(loc.ToCoordsXY()))
    {
        if (element->IsGhost())
            continue;
        if (element->BaseHeight != loc.z)
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
        if (element->BaseHeight != bannerTilePos.z)
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
    auto numTiles = kMaximumMapSizeTechnical * kMaximumMapSizeTechnical;

    auto& gameState = GetGameState();
    SetTileElements(gameState, std::vector<TileElement>(numTiles, GetDefaultSurfaceElement()));

    gameState.GrassSceneryTileLoopPosition = 0;
    gameState.WidePathTileLoopPosition = {};
    gameState.MapSize = size;
    MapRemoveOutOfRangeElements();
    ClearMapAnimations();

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
    auto& gameState = GetGameState();

    for (int32_t y = 0; y < gameState.MapSize.y; y++)
    {
        for (int32_t x = 0; x < gameState.MapSize.x; x++)
        {
            auto* surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
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
    auto& gameState = GetGameState();
    for (auto& element : gameState.TileElements)
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
        return kMinimumLandZ;

    // Get the surface element for the tile
    auto surfaceElement = MapGetSurfaceElementAt(loc);

    if (surfaceElement == nullptr)
    {
        return kMinimumLandZ;
    }

    auto height = surfaceElement->GetBaseZ();
    auto slope = surfaceElement->GetSlope();

    return TileElementHeight(CoordsXYZ{ loc, height }, slope);
}

int16_t TileElementHeight(const CoordsXYZ& loc, uint8_t slope)
{
    // Off the map
    if (!MapIsLocationValid(loc))
        return kMinimumLandZ;

    auto height = loc.z;

    uint8_t extra_height = (slope & kTileSlopeDiagonalFlag) >> 4; // 0x10 is the 5th bit - sets slope to double height
    // Remove the extra height bit
    slope &= kTileSlopeRaisedCornersMask;

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
    if (slope == kTileSlopeNCornerUp || slope == kTileSlopeECornerUp || slope == kTileSlopeSCornerUp
        || slope == kTileSlopeWCornerUp)
    {
        switch (slope)
        {
            case kTileSlopeNCornerUp:
                quad = xl + yl - TILE_SIZE;
                break;
            case kTileSlopeECornerUp:
                quad = xl - yl;
                break;
            case kTileSlopeSCornerUp:
                quad = TILE_SIZE - yl - xl;
                break;
            case kTileSlopeWCornerUp:
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
        case kTileSlopeNESideUp:
            height += xl / 2;
            break;
        case kTileSlopeSESideUp:
            height += (TILE_SIZE - yl) / 2;
            break;
        case kTileSlopeNWSideUp:
            height += yl / 2;
            break;
        case kTileSlopeSWSideUp:
            height += (TILE_SIZE - xl) / 2;
            break;
    }

    // One corner down
    if ((slope == kTileSlopeWCornerDown) || (slope == kTileSlopeSCornerDown) || (slope == kTileSlopeECornerDown)
        || (slope == kTileSlopeNCornerDown))
    {
        switch (slope)
        {
            case kTileSlopeWCornerDown:
                quad_extra = xl + TILE_SIZE - yl;
                quad = xl - yl;
                break;
            case kTileSlopeSCornerDown:
                quad_extra = xl + yl;
                quad = xl + yl - TILE_SIZE;
                break;
            case kTileSlopeECornerDown:
                quad_extra = TILE_SIZE - xl + yl;
                quad = yl - xl;
                break;
            case kTileSlopeNCornerDown:
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
        height += kLandHeightStep;
        // so we move *down* the slope
        if (quad < 0)
        {
            height += quad / 2;
        }
    }

    // Valleys
    if ((slope == kTileSlopeWEValley) || (slope == kTileSlopeNSValley))
    {
        switch (slope)
        {
            case kTileSlopeWEValley:
                quad = std::abs(xl - yl);
                break;
            case kTileSlopeNSValley:
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
                if (loc.z == tileElement->BaseHeight + 2)
                    return true;
            }
            else if (DirectionReverse(slopeDirection) == faceDirection && loc.z == tileElement->BaseHeight)
            {
                return true;
            }
        }
        else
        {
            if (loc.z == tileElement->BaseHeight && (tileElement->AsPath()->GetEdges() & (1 << faceDirection)))
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

    const int32_t kPracticalMapSizeBigX = GetGameState().MapSize.x * kCoordsXYStep;
    const int32_t kPracticalMapSizeBigY = GetGameState().MapSize.y * kCoordsXYStep;

    // Presumably update_path_wide_flags is too computationally expensive to call for every
    // tile every update, so gWidePathTileLoopX and gWidePathTileLoopY store the x and y
    // progress. A maximum of 128 calls is done per update.
    CoordsXY& loopPosition = GetGameState().WidePathTileLoopPosition;
    for (int32_t i = 0; i < 128; i++)
    {
        FootpathUpdatePathWideFlags(loopPosition);

        // Next x, y tile
        loopPosition.x += kCoordsXYStep;
        if (loopPosition.x >= kPracticalMapSizeBigX)
        {
            loopPosition.x = 0;
            loopPosition.y += kCoordsXYStep;
            if (loopPosition.y >= kPracticalMapSizeBigY)
            {
                loopPosition.y = 0;
            }
        }
    }
}

/**
 *
 *  rct2: 0x006A7B84
 */
int32_t MapHeightFromSlope(const CoordsXY& coords, int32_t slopeDirection, bool isSloped)
{
    if (!isSloped)
        return 0;

    switch (slopeDirection % kNumOrthogonalDirections)
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
    const bool is_x_valid = coords.x < kMaximumMapSizeBig && coords.x >= 0;
    const bool is_y_valid = coords.y < kMaximumMapSizeBig && coords.y >= 0;
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
    if (GetGameState().Cheats.sandboxMode)
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
                if (loc.z < surfaceElement->GetBaseZ() || loc.z >= surfaceElement->GetBaseZ() + kConstructionRightsClearanceBig)
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

int32_t MapGetCornerHeight(int32_t z, int32_t slope, int32_t direction)
{
    switch (direction)
    {
        case 0:
            if (slope & kTileSlopeNCornerUp)
            {
                z += 2;
                if (slope == (kTileSlopeSCornerDown | kTileSlopeDiagonalFlag))
                {
                    z += 2;
                }
            }
            break;
        case 1:
            if (slope & kTileSlopeECornerUp)
            {
                z += 2;
                if (slope == (kTileSlopeWCornerDown | kTileSlopeDiagonalFlag))
                {
                    z += 2;
                }
            }
            break;
        case 2:
            if (slope & kTileSlopeSCornerUp)
            {
                z += 2;
                if (slope == (kTileSlopeNCornerDown | kTileSlopeDiagonalFlag))
                {
                    z += 2;
                }
            }
            break;
        case 3:
            if (slope & kTileSlopeWCornerUp)
            {
                z += 2;
                if (slope == (kTileSlopeECornerDown | kTileSlopeDiagonalFlag))
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
    int32_t z = surfaceElement->BaseHeight;
    int32_t slope = surfaceElement->GetSlope();
    return MapGetCornerHeight(z, slope, direction);
}

uint8_t MapGetLowestLandHeight(const MapRange& range)
{
    auto mapSizeMax = GetMapSizeMaxXY();
    MapRange validRange = { std::max(range.GetLeft(), 32), std::max(range.GetTop(), 32),
                            std::min(range.GetRight(), mapSizeMax.x), std::min(range.GetBottom(), mapSizeMax.y) };

    uint8_t min_height = 0xFF;
    for (int32_t yi = validRange.GetTop(); yi <= validRange.GetBottom(); yi += kCoordsXYStep)
    {
        for (int32_t xi = validRange.GetLeft(); xi <= validRange.GetRight(); xi += kCoordsXYStep)
        {
            auto* surfaceElement = MapGetSurfaceElementAt(CoordsXY{ xi, yi });

            if (surfaceElement != nullptr && min_height > surfaceElement->BaseHeight)
            {
                if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !GetGameState().Cheats.sandboxMode)
                {
                    if (!MapIsLocationInPark(CoordsXY{ xi, yi }))
                    {
                        continue;
                    }
                }

                min_height = surfaceElement->BaseHeight;
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
    for (int32_t yi = validRange.GetTop(); yi <= validRange.GetBottom(); yi += kCoordsXYStep)
    {
        for (int32_t xi = validRange.GetLeft(); xi <= validRange.GetRight(); xi += kCoordsXYStep)
        {
            auto* surfaceElement = MapGetSurfaceElementAt(CoordsXY{ xi, yi });
            if (surfaceElement != nullptr)
            {
                if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !GetGameState().Cheats.sandboxMode)
                {
                    if (!MapIsLocationInPark(CoordsXY{ xi, yi }))
                    {
                        continue;
                    }
                }

                uint8_t BaseHeight = surfaceElement->BaseHeight;
                if (surfaceElement->GetSlope() & kTileSlopeRaisedCornersMask)
                    BaseHeight += 2;
                if (surfaceElement->GetSlope() & kTileSlopeDiagonalFlag)
                    BaseHeight += 2;
                if (max_height < BaseHeight)
                    max_height = BaseHeight;
            }
        }
    }
    return max_height;
}

bool MapIsLocationAtEdge(const CoordsXY& loc)
{
    return loc.x < 32 || loc.y < 32 || loc.x >= (kMaximumTileStartXY) || loc.y >= (kMaximumTileStartXY);
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
    tileElement->BaseHeight = kMaxTileElementHeight;
    _tileElementsInUse--;
    auto& gameState = GetGameState();
    if (tileElement == &gameState.TileElements.back())
    {
        gameState.TileElements.pop_back();
    }
}

/**
 *
 *  rct2: 0x00675A8E
 */
void MapRemoveAllRides()
{
    TileElementIterator it;

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
        auto screenCoord = Translate3DTo2DWithZ(rotation, CoordsXYZ{ corner, 0 });
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
        LOG_ERROR("Cannot insert new element");
        return nullptr;
    }

    auto& gameState = GetGameState();
    auto oldSize = gameState.TileElements.size();
    gameState.TileElements.resize(gameState.TileElements.size() + numElementsOnTile + numNewElements);
    _tileElementsInUse += numNewElements;
    return &gameState.TileElements[oldSize];
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
            originalTileElement->BaseHeight = kMaxTileElementHeight;
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
    newTileElement->Type = 0;
    newTileElement->SetType(type);
    newTileElement->SetBaseZ(loc.z);
    newTileElement->Flags = 0;
    newTileElement->SetLastForTile(isLastForTile);
    newTileElement->SetOccupiedQuadrants(occupiedQuadrants);
    newTileElement->SetClearanceZ(loc.z);
    newTileElement->Owner = 0;
    std::memset(&newTileElement->Pad05, 0, sizeof(newTileElement->Pad05));
    std::memset(&newTileElement->Pad08, 0, sizeof(newTileElement->Pad08));
    newTileElement++;

    // Insert rest of map elements above insert height
    if (!isLastForTile)
    {
        do
        {
            // Copy over map element
            *newTileElement = *originalTileElement;
            originalTileElement->BaseHeight = kMaxTileElementHeight;
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

    auto& gameState = GetGameState();

    // Update 43 more tiles (for each 256x256 block)
    for (int32_t j = 0; j < 43; j++)
    {
        int32_t x = 0;
        int32_t y = 0;

        uint16_t interleaved_xy = gameState.GrassSceneryTileLoopPosition;
        for (int32_t i = 0; i < 8; i++)
        {
            x = (x << 1) | (interleaved_xy & 1);
            interleaved_xy >>= 1;
            y = (y << 1) | (interleaved_xy & 1);
            interleaved_xy >>= 1;
        }

        // Repeat for each 256x256 block on the map
        for (int32_t blockY = 0; blockY < gameState.MapSize.y; blockY += 256)
        {
            for (int32_t blockX = 0; blockX < gameState.MapSize.x; blockX += 256)
            {
                auto mapPos = TileCoordsXY{ blockX + x, blockY + y }.ToCoordsXY();
                if (MapIsEdge(mapPos))
                    continue;

                auto* surfaceElement = MapGetSurfaceElementAt(mapPos);
                if (surfaceElement != nullptr)
                {
                    surfaceElement->UpdateGrassLength(mapPos);
                    SceneryUpdateTile(mapPos);
                }
            }
        }

        gameState.GrassSceneryTileLoopPosition++;
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
    auto& gameState = GetGameState();
    bool buildState = gameState.Cheats.buildInPauseMode;
    gameState.Cheats.buildInPauseMode = true;

    for (int32_t y = kMaximumMapSizeBig - kCoordsXYStep; y >= 0; y -= kCoordsXYStep)
    {
        for (int32_t x = kMaximumMapSizeBig - kCoordsXYStep; x >= 0; x -= kCoordsXYStep)
        {
            if (x == 0 || y == 0 || x >= mapSizeMax.x || y >= mapSizeMax.y)
            {
                // Note this purposely does not use LandSetRightsAction as X Y coordinates are outside of normal range.
                auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ x, y });
                if (surfaceElement != nullptr)
                {
                    surfaceElement->SetOwnership(OWNERSHIP_UNOWNED);
                    Park::UpdateFencesAroundTile({ x, y });
                }
                ClearElementsAt({ x, y });
            }
        }
    }

    // Reset cheat state
    gameState.Cheats.buildInPauseMode = buildState;
}

static void MapExtendBoundarySurfaceExtendTile(const SurfaceElement& sourceTile, SurfaceElement& destTile)
{
    destTile.SetSurfaceObjectIndex(sourceTile.GetSurfaceObjectIndex());
    destTile.SetEdgeObjectIndex(sourceTile.GetEdgeObjectIndex());
    destTile.SetGrassLength(sourceTile.GetGrassLength());
    destTile.SetOwnership(OWNERSHIP_UNOWNED);
    destTile.SetWaterHeight(sourceTile.GetWaterHeight());

    auto z = sourceTile.BaseHeight;
    auto slope = sourceTile.GetSlope() & kTileSlopeNWSideUp;
    if (slope == kTileSlopeNWSideUp)
    {
        z += 2;
        slope = kTileSlopeFlat;
        if (sourceTile.GetSlope() & kTileSlopeDiagonalFlag)
        {
            slope = kTileSlopeNCornerUp;
            if (sourceTile.GetSlope() & kTileSlopeSCornerUp)
            {
                slope = kTileSlopeWCornerUp;
                if (sourceTile.GetSlope() & kTileSlopeECornerUp)
                {
                    slope = kTileSlopeFlat;
                }
            }
        }
    }
    if (slope & kTileSlopeNCornerUp)
        slope |= kTileSlopeECornerUp;
    if (slope & kTileSlopeWCornerUp)
        slope |= kTileSlopeSCornerUp;

    destTile.SetSlope(slope);
    destTile.BaseHeight = z;
    destTile.ClearanceHeight = z;
}

/**
 * Copies the terrain and slope from the Y edge of the map to the new tiles. Used when increasing the size of the map.
 */
void MapExtendBoundarySurfaceY()
{
    auto y = GetGameState().MapSize.y - 2;
    for (auto x = 0; x < kMaximumMapSizeTechnical; x++)
    {
        auto existingTileElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y - 1 });
        auto newTileElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });

        if (existingTileElement != nullptr && newTileElement != nullptr)
        {
            MapExtendBoundarySurfaceExtendTile(*existingTileElement, *newTileElement);
        }

        Park::UpdateFences({ x << 5, y << 5 });
    }
}

/**
 * Copies the terrain and slope from the X edge of the map to the new tiles. Used when increasing the size of the map.
 */
void MapExtendBoundarySurfaceX()
{
    auto x = GetGameState().MapSize.x - 2;
    for (auto y = 0; y < kMaximumMapSizeTechnical; y++)
    {
        auto existingTileElement = MapGetSurfaceElementAt(TileCoordsXY{ x - 1, y });
        auto newTileElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
        if (existingTileElement != nullptr && newTileElement != nullptr)
        {
            MapExtendBoundarySurfaceExtendTile(*existingTileElement, *newTileElement);
        }
        Park::UpdateFences({ x << 5, y << 5 });
    }
}

/**
 * Clears the provided element properly from a certain tile, and updates
 * the pointer (when needed) passed to this function to point to the next element.
 */
void ClearElementAt(const CoordsXY& loc, TileElement** elementPtr)
{
    TileElement* element = *elementPtr;
    switch (element->GetType())
    {
        case TileElementType::Surface:
            element->BaseHeight = kMinimumLandHeight;
            element->ClearanceHeight = kMinimumLandHeight;
            element->Owner = 0;
            element->AsSurface()->SetSlope(kTileSlopeFlat);
            element->AsSurface()->SetSurfaceObjectIndex(0);
            element->AsSurface()->SetEdgeObjectIndex(0);
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
    auto& gameState = GetGameState();
    // Remove the spawn point (if there is one in the current tile)
    gameState.PeepSpawns.erase(
        std::remove_if(
            gameState.PeepSpawns.begin(), gameState.PeepSpawns.end(),
            [loc](const CoordsXY& spawn) { return spawn.ToTileStart() == loc.ToTileStart(); }),
        gameState.PeepSpawns.end());

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
    if ((surfaceElement->GetSlope() & kTileSlopeRaisedCornersMask) != kTileSlopeFlat)
        z += kLandHeightStep;
    if ((surfaceElement->GetSlope() & kTileSlopeDiagonalFlag) != 0)
        z += kLandHeightStep;

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
        if (tileElement->BaseHeight != sceneryTilePos.z)
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

            if (tileElement->BaseHeight != entranceTileCoords.z)
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

            if (tileElement->BaseHeight != entranceTileCoords.z)
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

            if (tileElement->BaseHeight != exitTileCoords.z)
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
            if (tileElement->BaseHeight != sceneryTileCoords.z)
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
    auto tileElement = MapGetLargeScenerySegment(sceneryPos, sequence);
    if (tileElement == nullptr)
        return std::nullopt;

    auto* sceneryEntry = tileElement->GetEntry();
    auto& tile = sceneryEntry->tiles[sequence];

    CoordsXY offsetPos{ tile.offset };
    auto rotatedOffsetPos = offsetPos.Rotate(sceneryPos.direction);

    auto origin = CoordsXYZ{ sceneryPos.x - rotatedOffsetPos.x, sceneryPos.y - rotatedOffsetPos.y,
                             sceneryPos.z - tile.offset.z };
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

    auto sceneryOrigin = MapLargeSceneryGetOrigin(signPos, sequence, &tileElement);
    if (!sceneryOrigin)
    {
        return false;
    }

    auto* sceneryEntry = tileElement->GetEntry();

    // Iterate through each tile of the large scenery element
    for (auto& tile : sceneryEntry->tiles)
    {
        CoordsXY offsetPos{ tile.offset };
        auto rotatedOffsetPos = offsetPos.Rotate(signPos.direction);

        auto tmpSignPos = CoordsXYZD{ sceneryOrigin->x + rotatedOffsetPos.x, sceneryOrigin->y + rotatedOffsetPos.y,
                                      sceneryOrigin->z + tile.offset.z, signPos.direction };
        tileElement = MapGetLargeScenerySegment(tmpSignPos, tile.index);
        if (tileElement != nullptr)
        {
            tileElement->SetPrimaryColour(mainColour);
            tileElement->SetSecondaryColour(textColour);

            MapInvalidateTile({ tmpSignPos, tileElement->GetBaseZ(), tileElement->GetClearanceZ() });
        }
    }

    return true;
}

static void MapInvalidateTileUnderZoom(int32_t x, int32_t y, int32_t z0, int32_t z1, ZoomLevel maxZoom)
{
    if (gOpenRCT2Headless)
        return;

    ViewportsInvalidate(x, y, z0, z1, maxZoom);
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

    int16_t base_z = surfaceElement->BaseHeight;
    int16_t clear_z = surfaceElement->BaseHeight + 2;
    if (surfaceElement->GetSlope() & kTileSlopeDiagonalFlag)
        clear_z += 2;

    auto tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    while (!(tileElement++)->IsLastForTile())
    {
        if (clear_z >= tileElement->ClearanceHeight)
            continue;

        if (base_z < tileElement->BaseHeight)
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
    for (int32_t y = 0; y < kMaximumMapSizeBig; y += kCoordsXYStep)
    {
        for (int32_t x = 0; x < kMaximumMapSizeBig; x += kCoordsXYStep)
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
TileElement* MapGetTrackElementAtOfType(const CoordsXYZ& trackPos, OpenRCT2::TrackElemType trackType)
{
    TileElement* tileElement = MapGetFirstElementAt(trackPos);
    if (tileElement == nullptr)
        return nullptr;
    auto trackTilePos = TileCoordsXYZ{ trackPos };
    do
    {
        if (tileElement->GetType() != TileElementType::Track)
            continue;
        if (tileElement->BaseHeight != trackTilePos.z)
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
TileElement* MapGetTrackElementAtOfTypeSeq(const CoordsXYZ& trackPos, OpenRCT2::TrackElemType trackType, int32_t sequence)
{
    TileElement* tileElement = MapGetFirstElementAt(trackPos);
    auto trackTilePos = TileCoordsXYZ{ trackPos };
    do
    {
        if (tileElement == nullptr)
            break;
        if (tileElement->GetType() != TileElementType::Track)
            continue;
        if (tileElement->BaseHeight != trackTilePos.z)
            continue;
        if (tileElement->AsTrack()->GetTrackType() != trackType)
            continue;
        if (tileElement->AsTrack()->GetSequenceIndex() != sequence)
            continue;

        return tileElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

TrackElement* MapGetTrackElementAtOfType(const CoordsXYZD& location, OpenRCT2::TrackElemType trackType)
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

TrackElement* MapGetTrackElementAtOfTypeSeq(const CoordsXYZD& location, OpenRCT2::TrackElemType trackType, int32_t sequence)
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
TileElement* MapGetTrackElementAtOfTypeFromRide(const CoordsXYZ& trackPos, OpenRCT2::TrackElemType trackType, RideId rideIndex)
{
    TileElement* tileElement = MapGetFirstElementAt(trackPos);
    if (tileElement == nullptr)
        return nullptr;
    auto trackTilePos = TileCoordsXYZ{ trackPos };
    do
    {
        if (tileElement->GetType() != TileElementType::Track)
            continue;
        if (tileElement->BaseHeight != trackTilePos.z)
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
        if (tileElement->BaseHeight != trackTilePos.z)
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
        if (tileElement->BaseHeight != trackTilePos.z)
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
        if (tileElement->BaseHeight != tileWallCoords.z)
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
            if (tileElement->BaseHeight - kConstructionRightsClearanceSmall > tilePos.z || tileElement->BaseHeight < tilePos.z)
            {
                destOwnership = OWNERSHIP_UNOWNED;
                break;
            }
        }
    } while (!(tileElement++)->IsLastForTile());

    return destOwnership;
}

void FixLandOwnershipTilesWithOwnership(std::vector<TileCoordsXY> tiles, uint8_t ownership)
{
    for (const auto& tile : tiles)
    {
        auto surfaceElement = MapGetSurfaceElementAt(tile);
        if (surfaceElement != nullptr)
        {
            surfaceElement->SetOwnership(ownership);
            Park::UpdateFencesAroundTile(tile.ToCoordsXY());
        }
    }
}

MapRange ClampRangeWithinMap(const MapRange& range)
{
    auto mapSizeMax = GetMapSizeMaxXY();
    auto aX = std::max<decltype(range.GetLeft())>(kCoordsXYStep, range.GetLeft());
    auto bX = std::min<decltype(range.GetRight())>(mapSizeMax.x, range.GetRight());
    auto aY = std::max<decltype(range.GetTop())>(kCoordsXYStep, range.GetTop());
    auto bY = std::min<decltype(range.GetBottom())>(mapSizeMax.y, range.GetBottom());
    MapRange validRange = MapRange{ aX, aY, bX, bY };
    return validRange;
}

static inline void shiftIfNotNull(TileCoordsXY& coords, const TileCoordsXY& amount)
{
    if (!coords.IsNull())
        coords += amount;
}

static inline void shiftIfNotNull(CoordsXY& coords, const CoordsXY& amount)
{
    if (!coords.IsNull())
        coords += amount;
}

void ShiftMap(const TileCoordsXY& amount)
{
    if (amount.x == 0 && amount.y == 0)
        return;

    auto amountToMove = amount.ToCoordsXY();
    auto& gameState = GetGameState();

    // Tile elements
    auto newElements = std::vector<TileElement>();
    for (int32_t y = 0; y < kMaximumMapSizeTechnical; y++)
    {
        for (int32_t x = 0; x < kMaximumMapSizeTechnical; x++)
        {
            auto srcX = x - amount.x;
            auto srcY = y - amount.y;
            if (x > 0 && y > 0 && x < gameState.MapSize.x - 1 && y < gameState.MapSize.y - 1 && srcX > 0 && srcY > 0
                && srcX < gameState.MapSize.x - 1 && srcY < gameState.MapSize.y - 1)
            {
                auto srcTile = _tileIndex.GetFirstElementAt(TileCoordsXY(srcX, srcY));
                do
                {
                    newElements.push_back(*srcTile);
                } while (!(srcTile++)->IsLastForTile());
            }
            else if (x == 0 || y == 0 || x == gameState.MapSize.x - 1 || y == gameState.MapSize.y - 1)
            {
                auto surface = GetDefaultSurfaceElement();
                surface.SetBaseZ(kMinimumLandZ);
                surface.SetClearanceZ(kMinimumLandZ);
                surface.AsSurface()->SetSlope(0);
                surface.AsSurface()->SetWaterHeight(0);
                newElements.push_back(surface);
            }
            else
            {
                auto copyX = std::clamp(srcX, 1, gameState.MapSize.x - 2);
                auto copyY = std::clamp(srcY, 1, gameState.MapSize.y - 2);
                auto srcTile = MapGetSurfaceElementAt(TileCoordsXY(copyX, copyY));
                if (srcTile != nullptr)
                {
                    auto tileEl = *srcTile;
                    tileEl.SetOwner(OWNERSHIP_UNOWNED);
                    tileEl.SetParkFences(0);
                    tileEl.SetLastForTile(true);
                    newElements.push_back(*reinterpret_cast<TileElement*>(&tileEl));
                }
                else
                {
                    newElements.push_back(GetDefaultSurfaceElement());
                }
            }
        }
    }

    SetTileElements(gameState, std::move(newElements));
    MapRemoveOutOfRangeElements();

    for (auto& spawn : gameState.PeepSpawns)
        shiftIfNotNull(spawn, amountToMove);

    for (auto& entrance : gameState.Park.Entrances)
        shiftIfNotNull(entrance, amountToMove);

    // Entities
    auto& entityTweener = EntityTweener::Get();
    for (auto i = 0; i < EnumValue(EntityType::Count); i++)
    {
        auto entityType = static_cast<EntityType>(i);
        auto& list = GetEntityList(entityType);
        for (const auto& entityId : list)
        {
            auto entity = GetEntity(entityId);

            // Do not tween the entity
            entityTweener.RemoveEntity(entity);

            auto location = entity->GetLocation();
            shiftIfNotNull(location, amountToMove);
            entity->MoveTo(location);

            switch (entityType)
            {
                case EntityType::Guest:
                case EntityType::Staff:
                {
                    auto peep = entity->As<Peep>();
                    if (peep != nullptr)
                    {
                        shiftIfNotNull(peep->NextLoc, amountToMove);
                        peep->DestinationX += amountToMove.x;
                        peep->DestinationY += amountToMove.y;
                        shiftIfNotNull(peep->PathfindGoal, amount);
                        for (auto& h : peep->PathfindHistory)
                            shiftIfNotNull(h, amount);
                    }
                    break;
                }
                case EntityType::Vehicle:
                {
                    auto vehicle = entity->As<Vehicle>();
                    if (vehicle != nullptr)
                    {
                        shiftIfNotNull(vehicle->TrackLocation, amountToMove);
                        shiftIfNotNull(vehicle->BoatLocation, amountToMove);
                    }
                    break;
                }
                case EntityType::Duck:
                {
                    auto duck = entity->As<Duck>();
                    if (duck != nullptr)
                    {
                        duck->target_x += amountToMove.x;
                        duck->target_y += amountToMove.y;
                    }
                    break;
                }
                case EntityType::JumpingFountain:
                {
                    auto fountain = entity->As<JumpingFountain>();
                    if (fountain != nullptr)
                    {
                        fountain->TargetX += amountToMove.x;
                        fountain->TargetY += amountToMove.y;
                    }
                }
                default:
                    break;
            }
            if (entityType == EntityType::Staff)
            {
                auto staff = entity->As<Staff>();
                if (staff != nullptr)
                {
                    auto patrol = staff->PatrolInfo;
                    if (patrol != nullptr)
                    {
                        auto positions = patrol->ToVector();
                        for (auto& p : positions)
                            shiftIfNotNull(p, amount);
                        patrol->Clear();
                        patrol->Union(positions);
                    }
                }
            }
        }
    }
    UpdateConsolidatedPatrolAreas();

    // Rides
    for (auto& ride : GetRideManager())
    {
        auto stations = ride.GetStations();
        for (auto& station : stations)
        {
            shiftIfNotNull(station.Start, amountToMove);
            shiftIfNotNull(station.Entrance, amount);
            shiftIfNotNull(station.Exit, amount);
        }

        shiftIfNotNull(ride.overall_view, amountToMove);
        shiftIfNotNull(ride.boat_hire_return_position, amount);
        shiftIfNotNull(ride.CurTestTrackLocation, amount);
        shiftIfNotNull(ride.ChairliftBullwheelLocation[0], amount);
        shiftIfNotNull(ride.ChairliftBullwheelLocation[1], amount);
        shiftIfNotNull(ride.CableLiftLoc, amountToMove);
    }

    // Banners
    auto numBanners = GetNumBanners();
    auto id = BannerIndex::FromUnderlying(0);
    size_t count = 0;
    while (count < numBanners)
    {
        auto* banner = GetBanner(id);
        if (banner != nullptr)
        {
            shiftIfNotNull(banner->position, amount);
            count++;
        }
        id = BannerIndex::FromUnderlying(id.ToUnderlying() + 1);
    }

    ShiftAllMapAnimations(amountToMove);
}
