/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "Location.hpp"

#include <array>
#include <initializer_list>
#include <optional>
#include <vector>

constexpr uint8_t kMinimumLandHeight = 2;
constexpr uint8_t kMaximumLandHeight = 254;
constexpr uint8_t kMinimumWaterHeight = 2;
constexpr uint8_t kMaximumWaterHeight = 254;
/**
 * The land height that counts as 0 metres/feet for the land height labels and altitude graphs.
 */
constexpr uint8_t kMapBaseZ = 7;

constexpr uint8_t kMinimumMapSizeTechnical = 5;
constexpr uint16_t kMaximumMapSizeTechnical = 1001;
constexpr int16_t kMinimumMapSizePractical = (kMinimumMapSizeTechnical - 2);
constexpr int16_t kMaximumMapSizePractical = (kMaximumMapSizeTechnical - 2);
constexpr const int32_t kMaximumMapSizeBig = kCoordsXYStep * kMaximumMapSizeTechnical;
constexpr int32_t kMaximumTileStartXY = kMaximumMapSizeBig - kCoordsXYStep;
constexpr const int32_t kLandHeightStep = 2 * kCoordsZStep;
constexpr const int32_t kWaterHeightStep = 2 * kCoordsZStep;
constexpr const int32_t kMinimumLandZ = kMinimumLandHeight * kCoordsZStep;
constexpr TileCoordsXY kDefaultMapSize = { 150, 150 };
// How high construction has to be off the ground when the player owns construction rights, in tile coords.
constexpr uint8_t kConstructionRightsClearanceSmall = 3;
// Same as previous, but in big coords.
constexpr const uint8_t kConstructionRightsClearanceBig = 3 * kCoordsZStep;

constexpr int16_t kMapMinimumXY = (-kMaximumMapSizeTechnical);

constexpr uint32_t kMaxTileElementsWithSpaceRoom = 0x1000000;
constexpr uint32_t kMaxTileElements = kMaxTileElementsWithSpaceRoom - 512;

using PeepSpawn = CoordsXYZD;
struct BannerElement;
struct EntranceElement;
struct LargeSceneryElement;
struct PathElement;
struct SmallSceneryElement;
struct SurfaceElement;
struct TileElement;
struct TrackElement;
struct WallElement;
enum class TileElementType : uint8_t;

namespace OpenRCT2
{
    enum class TrackElemType : uint16_t;
}

struct CoordsXYE : public CoordsXY
{
    CoordsXYE() = default;
    constexpr CoordsXYE(int32_t _x, int32_t _y, TileElement* _e)
        : CoordsXY(_x, _y)
        , element(_e)
    {
    }

    constexpr CoordsXYE(const CoordsXY& c, TileElement* _e)
        : CoordsXY(c)
        , element(_e)
    {
    }
    TileElement* element = nullptr;
};

enum
{
    MAP_SELECT_FLAG_ENABLE = 1 << 0,
    MAP_SELECT_FLAG_ENABLE_CONSTRUCT = 1 << 1,
    MAP_SELECT_FLAG_ENABLE_ARROW = 1 << 2,
    MAP_SELECT_FLAG_GREEN = 1 << 3,
};

enum
{
    MAP_SELECT_TYPE_CORNER_0,
    MAP_SELECT_TYPE_CORNER_1,
    MAP_SELECT_TYPE_CORNER_2,
    MAP_SELECT_TYPE_CORNER_3,
    MAP_SELECT_TYPE_FULL,
    MAP_SELECT_TYPE_FULL_WATER,
    MAP_SELECT_TYPE_FULL_LAND_RIGHTS,
    MAP_SELECT_TYPE_QUARTER_0,
    MAP_SELECT_TYPE_QUARTER_1,
    MAP_SELECT_TYPE_QUARTER_2,
    MAP_SELECT_TYPE_QUARTER_3,
    MAP_SELECT_TYPE_EDGE_0,
    MAP_SELECT_TYPE_EDGE_1,
    MAP_SELECT_TYPE_EDGE_2,
    MAP_SELECT_TYPE_EDGE_3,
};

extern const std::array<CoordsXY, 8> CoordsDirectionDelta;
extern const TileCoordsXY TileDirectionDelta[];

CoordsXY GetMapSizeUnits();
CoordsXY GetMapSizeMinus2();
CoordsXY GetMapSizeMaxXY();

extern uint16_t gMapSelectFlags;
extern uint16_t gMapSelectType;
extern CoordsXY gMapSelectPositionA;
extern CoordsXY gMapSelectPositionB;
extern CoordsXYZ gMapSelectArrowPosition;
extern uint8_t gMapSelectArrowDirection;

extern std::vector<CoordsXY> gMapSelectionTiles;

extern uint32_t gLandRemainingOwnershipSales;
extern uint32_t gLandRemainingConstructionSales;

extern bool gMapLandRightsUpdateSuccess;

namespace OpenRCT2
{
    struct GameState_t;
}

void ReorganiseTileElements();
const std::vector<TileElement>& GetTileElements();
void SetTileElements(OpenRCT2::GameState_t& gameState, std::vector<TileElement>&& tileElements);
void StashMap();
void UnstashMap();
std::vector<TileElement> GetReorganisedTileElementsWithoutGhosts();

void MapInit(const TileCoordsXY& size);

void MapCountRemainingLandRights();
void MapStripGhostFlagFromElements();
TileElement* MapGetFirstElementAt(const CoordsXY& tilePos);
TileElement* MapGetFirstElementAt(const TileCoordsXY& tilePos);
TileElement* MapGetNthElementAt(const CoordsXY& coords, int32_t n);
TileElement* MapGetFirstTileElementWithBaseHeightBetween(const TileCoordsXYRangedZ& loc, TileElementType type);
void MapSetTileElement(const TileCoordsXY& tilePos, TileElement* elements);
int32_t MapHeightFromSlope(const CoordsXY& coords, int32_t slopeDirection, bool isSloped);
BannerElement* MapGetBannerElementAt(const CoordsXYZ& bannerPos, uint8_t direction);
SurfaceElement* MapGetSurfaceElementAt(const TileCoordsXY& coords);
SurfaceElement* MapGetSurfaceElementAt(const CoordsXY& coords);
PathElement* MapGetPathElementAt(const TileCoordsXYZ& loc);
WallElement* MapGetWallElementAt(const CoordsXYZD& wallCoords);
WallElement* MapGetWallElementAt(const CoordsXYRangedZ& coords);
SmallSceneryElement* MapGetSmallSceneryElementAt(const CoordsXYZ& sceneryCoords, int32_t type, uint8_t quadrant);
EntranceElement* MapGetParkEntranceElementAt(const CoordsXYZ& entranceCoords, bool ghost);
EntranceElement* MapGetRideEntranceElementAt(const CoordsXYZ& entranceCoords, bool ghost);
EntranceElement* MapGetRideExitElementAt(const CoordsXYZ& exitCoords, bool ghost);
uint8_t MapGetHighestLandHeight(const MapRange& range);
uint8_t MapGetLowestLandHeight(const MapRange& range);
bool MapCoordIsConnected(const TileCoordsXYZ& loc, uint8_t faceDirection);
void MapUpdatePathWideFlags();
bool MapIsLocationValid(const CoordsXY& coords);
bool MapIsEdge(const CoordsXY& coords);
bool MapCanBuildAt(const CoordsXYZ& loc);
bool MapIsLocationOwned(const CoordsXYZ& loc);
bool MapIsLocationInPark(const CoordsXY& coords);
bool MapIsLocationOwnedOrHasRights(const CoordsXY& loc);
bool MapSurfaceIsBlocked(const CoordsXY& mapCoords);
void MapRemoveAllRides();
void MapInvalidateMapSelectionTiles();
void MapInvalidateSelectionRect();
bool MapCheckCapacityAndReorganise(const CoordsXY& loc, size_t numElements = 1);
int16_t TileElementHeight(const CoordsXY& loc);
int16_t TileElementHeight(const CoordsXYZ& loc, uint8_t slope);
int16_t TileElementWaterHeight(const CoordsXY& loc);
void TileElementRemove(TileElement* tileElement);
TileElement* TileElementInsert(const CoordsXYZ& loc, int32_t occupiedQuadrants, TileElementType type);

template<typename T = TileElement>
T* MapGetFirstTileElementWithBaseHeightBetween(const TileCoordsXYRangedZ& loc)
{
    auto* element = MapGetFirstTileElementWithBaseHeightBetween(loc, T::kElementType);
    return element != nullptr ? element->template as<T>() : nullptr;
}

template<typename T>
T* TileElementInsert(const CoordsXYZ& loc, int32_t occupiedQuadrants)
{
    auto* element = TileElementInsert(loc, occupiedQuadrants, T::kElementType);
    return (element != nullptr) ? element->template as<T>() : nullptr;
}

struct TileElementIterator
{
    int32_t x;
    int32_t y;
    TileElement* element;
};
#ifdef PLATFORM_32BIT
static_assert(sizeof(TileElementIterator) == 12);
#endif

void TileElementIteratorBegin(TileElementIterator* it);
int32_t TileElementIteratorNext(TileElementIterator* it);
void TileElementIteratorRestartForTile(TileElementIterator* it);

void MapUpdateTiles();
int32_t MapGetHighestZ(const CoordsXY& loc);

bool TileElementWantsPathConnectionTowards(const TileCoordsXYZD& coords, const TileElement* const elementToBeRemoved);

void MapRemoveOutOfRangeElements();
void MapExtendBoundarySurfaceX();
void MapExtendBoundarySurfaceY();

bool MapLargeScenerySignSetColour(const CoordsXYZD& signPos, int32_t sequence, uint8_t mainColour, uint8_t textColour);

void MapInvalidateTile(const CoordsXYRangedZ& tilePos);
void MapInvalidateTileZoom1(const CoordsXYRangedZ& tilePos);
void MapInvalidateTileZoom0(const CoordsXYRangedZ& tilePos);
void MapInvalidateTileFull(const CoordsXY& tilePos);
void MapInvalidateElement(const CoordsXY& elementPos, TileElement* tileElement);
void MapInvalidateRegion(const CoordsXY& mins, const CoordsXY& maxs);

int32_t MapGetTileSide(const CoordsXY& mapPos);
int32_t MapGetTileQuadrant(const CoordsXY& mapPos);
int32_t MapGetCornerHeight(int32_t z, int32_t slope, int32_t direction);
int32_t TileElementGetCornerHeight(const SurfaceElement* surfaceElement, int32_t direction);

void MapClearAllElements();
void ClearElementAt(const CoordsXY& loc, TileElement** elementPtr);

LargeSceneryElement* MapGetLargeScenerySegment(const CoordsXYZD& sceneryPos, int32_t sequence);
std::optional<CoordsXYZ> MapLargeSceneryGetOrigin(
    const CoordsXYZD& sceneryPos, int32_t sequence, LargeSceneryElement** outElement);

TrackElement* MapGetTrackElementAt(const CoordsXYZ& trackPos);
TileElement* MapGetTrackElementAtOfType(const CoordsXYZ& trackPos, OpenRCT2::TrackElemType trackType);
TileElement* MapGetTrackElementAtOfTypeSeq(const CoordsXYZ& trackPos, OpenRCT2::TrackElemType trackType, int32_t sequence);
TrackElement* MapGetTrackElementAtOfType(const CoordsXYZD& location, OpenRCT2::TrackElemType trackType);
TrackElement* MapGetTrackElementAtOfTypeSeq(const CoordsXYZD& location, OpenRCT2::TrackElemType trackType, int32_t sequence);
TileElement* MapGetTrackElementAtOfTypeFromRide(const CoordsXYZ& trackPos, OpenRCT2::TrackElemType trackType, RideId rideIndex);
TileElement* MapGetTrackElementAtFromRide(const CoordsXYZ& trackPos, RideId rideIndex);
TileElement* MapGetTrackElementAtWithDirectionFromRide(const CoordsXYZD& trackPos, RideId rideIndex);

bool MapIsLocationAtEdge(const CoordsXY& loc);

uint16_t CheckMaxAllowableLandRightsForTile(const CoordsXYZ& tileMapPos);

void FixLandOwnershipTilesWithOwnership(std::vector<TileCoordsXY> tiles, uint8_t ownership);
MapRange ClampRangeWithinMap(const MapRange& range);
void ShiftMap(const TileCoordsXY& amount);
