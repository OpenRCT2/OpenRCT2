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
#include "../object/Object.h"

class FootpathObject;
class FootpathSurfaceObject;
class FootpathRailingsObject;
struct PathElement;
struct TileElement;

constexpr auto kFootpathMaxHeight = 248 * kCoordsZStep;
constexpr auto kFootpathMinHeight = 2 * kCoordsZStep;
constexpr auto kPathHeightStep = 2 * kCoordsZStep;
constexpr auto kPathClearance = 4 * kCoordsZStep;

enum class RailingEntrySupportType : uint8_t
{
    Box = 0,
    Pole = 1,
    Count
};

struct PathSurfaceDescriptor
{
    StringId Name;
    uint32_t Image;
    uint32_t PreviewImage;
    uint8_t Flags;
};

struct PathRailingsDescriptor
{
    StringId Name;
    uint32_t PreviewImage;
    uint32_t BridgeImage;
    uint32_t RailingsImage;
    RailingEntrySupportType SupportType;
    colour_t SupportColour;
    uint8_t Flags;
    uint8_t ScrollingMode;
};

using PathConstructFlags = uint8_t;
namespace OpenRCT2::PathConstructFlag
{
    constexpr PathConstructFlags IsQueue = 1 << 0;
    constexpr PathConstructFlags IsLegacyPathObject = 1 << 1;
} // namespace OpenRCT2::PathConstructFlag

struct FootpathSelection
{
    ObjectEntryIndex LegacyPath = kObjectEntryIndexNull;
    ObjectEntryIndex NormalSurface = kObjectEntryIndexNull;
    ObjectEntryIndex QueueSurface = kObjectEntryIndexNull;
    ObjectEntryIndex Railings = kObjectEntryIndexNull;
    bool IsQueueSelected{};

    ObjectEntryIndex GetSelectedSurface() const
    {
        return IsQueueSelected ? QueueSurface : NormalSurface;
    }
};

enum
{
    RAILING_ENTRY_FLAG_HAS_SUPPORT_BASE_SPRITE = (1 << 0),
    RAILING_ENTRY_FLAG_DRAW_PATH_OVER_SUPPORTS = (1 << 1), // When elevated
    RAILING_ENTRY_FLAG_NO_QUEUE_BANNER = (1 << 2),
};

enum
{
    FOOTPATH_SEARCH_SUCCESS,
    FOOTPATH_SEARCH_NOT_FOUND,
    FOOTPATH_SEARCH_INCOMPLETE,
    FOOTPATH_SEARCH_TOO_COMPLEX
};

enum
{
    SLOPE_IS_IRREGULAR_FLAG = (1 << 3), // Flag set in `DefaultPathSlope[]` and checked in `footpath_place_real`
    RAISE_FOOTPATH_FLAG = (1 << 4)
};

enum
{
    FOOTPATH_CORNER_0 = (1 << 0),
    FOOTPATH_CORNER_1 = (1 << 1),
    FOOTPATH_CORNER_2 = (1 << 2),
    FOOTPATH_CORNER_3 = (1 << 3),
};

enum
{
    FOOTPATH_CONNECTION_S = (1 << 0),
    FOOTPATH_CONNECTION_NE = (1 << 1),
    FOOTPATH_CONNECTION_W = (1 << 2),
    FOOTPATH_CONNECTION_SE = (1 << 3),
    FOOTPATH_CONNECTION_N = (1 << 4),
    FOOTPATH_CONNECTION_SW = (1 << 5),
    FOOTPATH_CONNECTION_E = (1 << 6),
    FOOTPATH_CONNECTION_NW = (1 << 7),
};

enum
{
    FOOTPATH_CONNECTED_MAP_EDGE_IGNORE_QUEUES = (1 << 0),
    FOOTPATH_CONNECTED_MAP_EDGE_UNOWN = (1 << 5),
    FOOTPATH_CONNECTED_MAP_EDGE_IGNORE_NO_ENTRY = (1 << 7)
};

extern FootpathSelection gFootpathSelection;
extern uint16_t gFootpathSelectedId;
extern CoordsXYZ gFootpathConstructFromPosition;
extern uint8_t gFootpathConstructSlope;
extern uint8_t gFootpathGroundFlags;

// Given a direction, this will return how to increase/decrease the x and y coordinates.
extern const std::array<CoordsXY, kNumOrthogonalDirections> DirectionOffsets;
extern const std::array<CoordsXY, kNumOrthogonalDirections> BinUseOffsets;
extern const std::array<CoordsXY, kNumOrthogonalDirections * 2> BenchUseOffsets;

PathElement* MapGetFootpathElement(const CoordsXYZ& coords);
void FootpathInterruptPeeps(const CoordsXYZ& footpathPos);
void FootpathRemoveLitter(const CoordsXYZ& footpathPos);
void FootpathConnectEdges(const CoordsXY& footpathPos, TileElement* tileElement, int32_t flags);
void FootpathUpdateQueueChains();
bool WallInTheWay(const CoordsXYRangedZ& fencePos, int32_t direction);
void FootpathChainRideQueue(
    RideId rideIndex, StationIndex entranceIndex, const CoordsXY& footpathPos, TileElement* tileElement, int32_t direction);
void FootpathUpdatePathWideFlags(const CoordsXY& footpathPos);
bool FootpathIsBlockedByVehicle(const TileCoordsXYZ& position);

int32_t FootpathIsConnectedToMapEdge(const CoordsXYZ& footpathPos, int32_t direction, int32_t flags);
void FootpathRemoveEdgesAt(const CoordsXY& footpathPos, TileElement* tileElement);

bool FootpathSelectDefault();
const FootpathObject* GetLegacyFootpathEntry(ObjectEntryIndex entryIndex);
const FootpathSurfaceObject* GetPathSurfaceEntry(ObjectEntryIndex entryIndex);
const FootpathRailingsObject* GetPathRailingsEntry(ObjectEntryIndex entryIndex);

void FootpathQueueChainReset();
void FootpathQueueChainPush(RideId rideIndex);
bool FootpathIsZAndDirectionValid(const PathElement& tileElement, int32_t currentZ, int32_t currentDirection);
