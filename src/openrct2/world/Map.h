/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

constexpr TileCoordsXY kDefaultMapSize = { 150, 150 };

namespace OpenRCT2
{
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
    enum class TrackElemType : uint16_t;
} // namespace OpenRCT2

namespace OpenRCT2::Drawing
{
    enum class Colour : uint8_t;
}

extern const std::array<CoordsXY, 8> CoordsDirectionDelta;
extern const TileCoordsXY TileDirectionDelta[];

CoordsXY GetMapSizeUnits();
CoordsXY GetMapSizeMinus2();
CoordsXY GetMapSizeMaxXY();

extern uint32_t gLandRemainingOwnershipSales;
extern uint32_t gLandRemainingConstructionSales;

extern bool gMapLandRightsUpdateSuccess;

namespace OpenRCT2
{
    struct GameState_t;
}

void ReorganiseTileElements();
const std::vector<OpenRCT2::TileElement>& GetTileElements();
void SetTileElements(OpenRCT2::GameState_t& gameState, std::vector<OpenRCT2::TileElement>&& tileElements);
void StashMap();
void UnstashMap();
std::vector<OpenRCT2::TileElement> GetReorganisedTileElementsWithoutGhosts();

void MapInit(const TileCoordsXY& size);

void MapCountRemainingLandRights();
void MapStripGhostFlagFromElements();
OpenRCT2::TileElement* MapGetFirstElementAt(const CoordsXY& tilePos);
OpenRCT2::TileElement* MapGetFirstElementAt(const TileCoordsXY& tilePos);
OpenRCT2::TileElement* MapGetNthElementAt(const CoordsXY& coords, int32_t n);
OpenRCT2::TileElement* MapGetFirstTileElementWithBaseHeightBetween(
    const TileCoordsXYRangedZ& loc, OpenRCT2::TileElementType type);
void MapSetTileElement(const TileCoordsXY& tilePos, OpenRCT2::TileElement* elements);
int32_t MapHeightFromSlope(const CoordsXY& coords, int32_t slopeDirection, bool isSloped);
OpenRCT2::BannerElement* MapGetBannerElementAt(const CoordsXYZ& bannerPos, uint8_t direction);
OpenRCT2::SurfaceElement* MapGetSurfaceElementAt(const TileCoordsXY& coords);
OpenRCT2::SurfaceElement* MapGetSurfaceElementAt(const CoordsXY& coords);
OpenRCT2::PathElement* MapGetPathElementAt(const TileCoordsXYZ& loc);
OpenRCT2::WallElement* MapGetWallElementAt(const CoordsXYZD& wallCoords);
OpenRCT2::WallElement* MapGetWallElementAt(const CoordsXYRangedZ& coords);
OpenRCT2::SmallSceneryElement* MapGetSmallSceneryElementAt(const CoordsXYZ& sceneryCoords, int32_t type, uint8_t quadrant);
OpenRCT2::EntranceElement* MapGetParkEntranceElementAt(const CoordsXYZ& entranceCoords, bool ghost);
OpenRCT2::EntranceElement* MapGetRideEntranceElementAt(const CoordsXYZ& entranceCoords, bool ghost);
OpenRCT2::EntranceElement* MapGetRideExitElementAt(const CoordsXYZ& exitCoords, bool ghost);
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
void MapGetBoundingBox(const MapRange& _range, int32_t* left, int32_t* top, int32_t* right, int32_t* bottom);
bool MapCheckCapacityAndReorganise(const CoordsXY& loc, size_t numElements = 1);
int16_t TileElementHeight(const CoordsXY& loc);
int16_t TileElementHeight(const CoordsXYZ& loc, uint8_t slope);
int16_t TileElementWaterHeight(const CoordsXY& loc);
void TileElementRemove(OpenRCT2::TileElement* tileElement);
OpenRCT2::TileElement* TileElementInsert(const CoordsXYZ& loc, int32_t occupiedQuadrants, OpenRCT2::TileElementType type);

template<typename T = OpenRCT2::TileElement>
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
    OpenRCT2::TileElement* element;
};
#ifdef PLATFORM_32BIT
static_assert(sizeof(TileElementIterator) == 12);
#endif

void TileElementIteratorBegin(TileElementIterator* it);
int32_t TileElementIteratorNext(TileElementIterator* it);
void TileElementIteratorRestartForTile(TileElementIterator* it);

void MapUpdateTiles();
int32_t MapGetHighestZ(const CoordsXY& loc);

bool TileElementWantsPathConnectionTowards(const TileCoordsXYZD& coords, const OpenRCT2::TileElement* elementToBeRemoved);

void MapRemoveOutOfRangeElements();
void MapExtendBoundarySurfaceX();
void MapExtendBoundarySurfaceY();

bool MapLargeScenerySignSetColour(
    const CoordsXYZD& signPos, int32_t sequence, OpenRCT2::Drawing::Colour mainColour, OpenRCT2::Drawing::Colour textColour);

void MapInvalidateTile(const CoordsXYRangedZ& tilePos);
void MapInvalidateTileZoom1(const CoordsXYRangedZ& tilePos);
void MapInvalidateTileZoom0(const CoordsXYRangedZ& tilePos);
void MapInvalidateTileFull(const CoordsXY& tilePos);
void MapInvalidateElement(const CoordsXY& elementPos, OpenRCT2::TileElement* tileElement);
void MapInvalidateRegion(const CoordsXY& mins, const CoordsXY& maxs);

int32_t MapGetTileSide(const CoordsXY& mapPos);
int32_t MapGetTileQuadrant(const CoordsXY& mapPos);
int32_t MapGetCornerHeight(int32_t z, int32_t slope, int32_t direction);
int32_t TileElementGetCornerHeight(const OpenRCT2::SurfaceElement* surfaceElement, int32_t direction);

void MapClearAllElements();
void ClearElementAt(const CoordsXY& loc, OpenRCT2::TileElement** elementPtr);

OpenRCT2::LargeSceneryElement* MapGetLargeScenerySegment(const CoordsXYZD& sceneryPos, int32_t sequence);
std::optional<CoordsXYZ> MapLargeSceneryGetOrigin(
    const CoordsXYZD& sceneryPos, int32_t sequence, OpenRCT2::LargeSceneryElement** outElement);

OpenRCT2::TrackElement* MapGetTrackElementAt(const CoordsXYZ& trackPos);
OpenRCT2::TileElement* MapGetTrackElementAtOfType(const CoordsXYZ& trackPos, OpenRCT2::TrackElemType trackType);
OpenRCT2::TileElement* MapGetTrackElementAtOfTypeSeq(
    const CoordsXYZ& trackPos, OpenRCT2::TrackElemType trackType, int32_t sequence);
OpenRCT2::TrackElement* MapGetTrackElementAtOfType(const CoordsXYZD& location, OpenRCT2::TrackElemType trackType);
OpenRCT2::TrackElement* MapGetTrackElementAtOfTypeSeq(
    const CoordsXYZD& location, OpenRCT2::TrackElemType trackType, int32_t sequence);
OpenRCT2::TileElement* MapGetTrackElementAtOfTypeFromRide(
    const CoordsXYZ& trackPos, OpenRCT2::TrackElemType trackType, RideId rideIndex);
OpenRCT2::TileElement* MapGetTrackElementAtFromRide(const CoordsXYZ& trackPos, RideId rideIndex);
OpenRCT2::TileElement* MapGetTrackElementAtWithDirectionFromRide(const CoordsXYZD& trackPos, RideId rideIndex);
OpenRCT2::TileElement* MapGetTrackElementAtBeforeSurfaceFromRide(const CoordsXYZ& trackPos, RideId rideIndex);

uint16_t CheckMaxAllowableLandRightsForTile(const CoordsXYZ& tileMapPos);

void FixLandOwnershipTilesWithOwnership(std::vector<TileCoordsXY> tiles, uint8_t ownership);
MapRange ClampRangeWithinMap(const MapRange& range);
void ShiftMap(const TileCoordsXY& amount);
