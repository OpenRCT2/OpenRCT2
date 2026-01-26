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
#include "../actions/CommandFlag.h"
#include "../drawing/Colour.h"
#include "../drawing/ImageIndexType.h"
#include "../drawing/ScrollingText.h"
#include "../localisation/StringIdType.h"
#include "../object/ObjectTypes.h"
#include "Location.hpp"

#include <array>

namespace OpenRCT2
{
    class FootpathObject;
    class FootpathSurfaceObject;
    class FootpathRailingsObject;

    struct PathElement;
    struct SurfaceElement;
    struct TileElement;
} // namespace OpenRCT2

constexpr auto kFootpathMaxHeight = 248 * kCoordsZStep;
constexpr auto kFootpathMinHeight = 2 * kCoordsZStep;
constexpr auto kPathHeightStep = 2 * kCoordsZStep;
constexpr auto kPathClearance = 4 * kCoordsZStep;

enum class RailingEntrySupportType : uint8_t
{
    box = 0,
    pole = 1,
    count
};

struct PathSurfaceDescriptor
{
    StringId name = kStringIdNone;
    ImageIndex image{};
    ImageIndex previewImage{};
    uint8_t flags{};
};
constexpr PathSurfaceDescriptor kPathSurfaceDescriptorDummy{};

struct PathRailingsDescriptor
{
    StringId name = kStringIdNone;
    ImageIndex previewImage{};
    ImageIndex bridgeImage{};
    ImageIndex railingsImage{};
    RailingEntrySupportType supportType{};
    OpenRCT2::Drawing::Colour supportColour = OpenRCT2::Drawing::kColourNull;
    uint8_t flags{};
    uint8_t scrollingMode = kScrollingModeNone;
};
constexpr PathRailingsDescriptor kPathRailingsDescriptorDummy{};

using PathConstructFlags = uint8_t;
namespace OpenRCT2::PathConstructFlag
{
    constexpr PathConstructFlags IsQueue = 1 << 0;
    constexpr PathConstructFlags IsLegacyPathObject = 1 << 1;
} // namespace OpenRCT2::PathConstructFlag

struct FootpathSelection
{
    OpenRCT2::ObjectEntryIndex legacyPath = OpenRCT2::kObjectEntryIndexNull;
    OpenRCT2::ObjectEntryIndex normalSurface = OpenRCT2::kObjectEntryIndexNull;
    OpenRCT2::ObjectEntryIndex queueSurface = OpenRCT2::kObjectEntryIndexNull;
    OpenRCT2::ObjectEntryIndex railings = OpenRCT2::kObjectEntryIndexNull;
    bool isQueueSelected{};

    OpenRCT2::ObjectEntryIndex getSelectedSurface() const
    {
        return isQueueSelected ? queueSurface : normalSurface;
    }
};

enum class FootpathSlopeType : uint8_t
{
    flat,
    sloped,
    /**
     * Land has one corner down, raise the Z coordinate and place a flat piece.
     */
    raise,
    /**
     * Terrain has a shape that allows for two different path slopes, and as such it cannot autoplace a piece
     * without further context of the surrounding paths.
     */
    irregular,
};

struct FootpathSlope
{
    FootpathSlopeType type{};
    Direction direction{};

    constexpr bool operator==(const FootpathSlope& rhs) const
    {
        return type == rhs.type && direction == rhs.direction;
    }
};

struct FootpathPlacementResult
{
    int32_t baseZ{};
    FootpathSlope slope{};

    bool isValid()
    {
        return baseZ > 0;
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
extern uint8_t gFootpathGroundFlags;

// Given a direction, this will return how to increase/decrease the x and y coordinates.
extern const std::array<CoordsXY, kNumOrthogonalDirections> DirectionOffsets;
extern const std::array<CoordsXY, kNumOrthogonalDirections> BinUseOffsets;
extern const std::array<CoordsXY, kNumOrthogonalDirections * 2> BenchUseOffsets;

OpenRCT2::PathElement* MapGetFootpathElement(const CoordsXYZ& coords);
void FootpathInterruptPeeps(const CoordsXYZ& footpathPos);
void FootpathRemoveLitter(const CoordsXYZ& footpathPos);
void FootpathConnectEdges(
    const CoordsXY& footpathPos, OpenRCT2::TileElement* tileElement, OpenRCT2::GameActions::CommandFlags flags);
void FootpathUpdateQueueChains();
void FootpathChainRideQueue(
    RideId rideIndex, StationIndex entranceIndex, const CoordsXY& footpathPos, OpenRCT2::TileElement* tileElement,
    int32_t direction);
void FootpathUpdatePathWideFlags(const CoordsXY& footpathPos);
bool FootpathIsBlockedByVehicle(const TileCoordsXYZ& position);

int32_t FootpathIsConnectedToMapEdge(const CoordsXYZ& footpathPos, int32_t direction, int32_t flags);
void FootpathRemoveEdgesAt(const CoordsXY& footpathPos, OpenRCT2::TileElement* tileElement);

const OpenRCT2::FootpathObject* GetLegacyFootpathEntry(OpenRCT2::ObjectEntryIndex entryIndex);
const OpenRCT2::FootpathSurfaceObject* GetPathSurfaceEntry(OpenRCT2::ObjectEntryIndex entryIndex);
const OpenRCT2::FootpathRailingsObject* GetPathRailingsEntry(OpenRCT2::ObjectEntryIndex entryIndex);

void FootpathQueueChainReset();
void FootpathQueueChainPush(RideId rideIndex);
int32_t FootpathQueueCountConnections(const CoordsXY& position, const OpenRCT2::PathElement& pathElement);
bool FootpathIsZAndDirectionValid(const OpenRCT2::PathElement& tileElement, int32_t currentZ, int32_t currentDirection);

FootpathPlacementResult FootpathGetOnTerrainPlacement(const TileCoordsXY& location);
FootpathPlacementResult FootpathGetOnTerrainPlacement(const OpenRCT2::SurfaceElement& surfaceElement);
