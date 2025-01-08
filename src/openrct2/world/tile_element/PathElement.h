/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../Identifiers.h"
#include "TileElementBase.h"

class FootpathObject;
class FootpathRailingsObject;
class FootpathSurfaceObject;
struct PathAdditionEntry;
struct PathRailingsDescriptor;
struct PathSurfaceDescriptor;

using ObjectEntryIndex = uint16_t;

// Masks for values stored in TileElement.type
enum
{
    FOOTPATH_ELEMENT_TYPE_FLAG_IS_QUEUE = (1 << 0),
    FOOTPATH_ELEMENT_TYPE_FLAG_IS_WIDE = (1 << 1),
    FOOTPATH_ELEMENT_TYPE_DIRECTION_MASK = (1 << 6) | (1 << 7),
};

// Masks and flags for values stored in TileElement.properties.path.type
enum
{
    FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK = (1 << 0) | (1 << 1),
    FOOTPATH_PROPERTIES_FLAG_IS_SLOPED = (1 << 2),
    FOOTPATH_PROPERTIES_FLAG_HAS_QUEUE_BANNER = (1 << 3),
    FOOTPATH_PROPERTIES_TYPE_MASK = (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7),
};

// Masks and flags for values stored in TileElement.properties.path.edges
enum
{
    FOOTPATH_PROPERTIES_EDGES_EDGES_MASK = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3),
    FOOTPATH_PROPERTIES_EDGES_CORNERS_MASK = (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7),
};

enum
{
    FOOTPATH_ELEMENT_FLAGS2_IS_SLOPED = 1 << 0,
    FOOTPATH_ELEMENT_FLAGS2_HAS_QUEUE_BANNER = (1 << 1),
    FOOTPATH_ELEMENT_FLAGS2_ADDITION_IS_GHOST = (1 << 2),
    FOOTPATH_ELEMENT_FLAGS2_BLOCKED_BY_VEHICLE = (1 << 3),
    FOOTPATH_ELEMENT_FLAGS2_ADDITION_IS_BROKEN = (1 << 4),
    FOOTPATH_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY = (1 << 5),
    FOOTPATH_ELEMENT_FLAGS2_HAS_JUNCTION_RAILINGS = (1 << 6),
};

#pragma pack(push, 1)
struct PathElement : TileElementBase
{
    static constexpr TileElementType kElementType = TileElementType::Path;

private:
    ObjectEntryIndex SurfaceIndex;  // 5
    ObjectEntryIndex RailingsIndex; // 7
    uint8_t Additions;              // 9 (0 means no addition)
    uint8_t EdgesAndCorners;        // 10 (edges in lower 4 bits, corners in upper 4)
    uint8_t Flags2;                 // 11
    uint8_t SlopeDirection;         // 12
    union
    {
        uint8_t AdditionStatus; // 13, only used for litter bins
        RideId rideIndex;       // 13
    };
    ::StationIndex StationIndex; // 15

public:
    ObjectEntryIndex GetLegacyPathEntryIndex() const;
    const FootpathObject* GetLegacyPathEntry() const;
    void SetLegacyPathEntryIndex(ObjectEntryIndex newIndex);
    bool HasLegacyPathEntry() const;

    ObjectEntryIndex GetSurfaceEntryIndex() const;
    const FootpathSurfaceObject* GetSurfaceEntry() const;
    void SetSurfaceEntryIndex(ObjectEntryIndex newIndex);

    ObjectEntryIndex GetRailingsEntryIndex() const;
    const FootpathRailingsObject* GetRailingsEntry() const;
    void SetRailingsEntryIndex(ObjectEntryIndex newIndex);

    const PathSurfaceDescriptor* GetSurfaceDescriptor() const;
    const PathRailingsDescriptor* GetRailingsDescriptor() const;

    uint8_t GetQueueBannerDirection() const;
    void SetQueueBannerDirection(uint8_t direction);

    bool IsSloped() const;
    void SetSloped(bool isSloped);

    bool HasJunctionRailings() const;
    void SetJunctionRailings(bool hasJunctionRailings);

    Direction GetSlopeDirection() const;
    void SetSlopeDirection(Direction newSlope);

    RideId GetRideIndex() const;
    void SetRideIndex(RideId newRideIndex);

    ::StationIndex GetStationIndex() const;
    void SetStationIndex(::StationIndex newStationIndex);

    bool IsWide() const;
    void SetWide(bool isWide);

    bool IsQueue() const;
    void SetIsQueue(bool isQueue);
    bool HasQueueBanner() const;
    void SetHasQueueBanner(bool hasQueueBanner);

    bool IsBroken() const;
    void SetIsBroken(bool isBroken);

    bool IsBlockedByVehicle() const;
    void SetIsBlockedByVehicle(bool isBlocked);

    uint8_t GetEdges() const;
    void SetEdges(uint8_t newEdges);
    uint8_t GetCorners() const;
    void SetCorners(uint8_t newCorners);
    uint8_t GetEdgesAndCorners() const;
    void SetEdgesAndCorners(uint8_t newEdgesAndCorners);

    bool HasAddition() const;
    uint8_t GetAddition() const;
    ObjectEntryIndex GetAdditionEntryIndex() const;
    const PathAdditionEntry* GetAdditionEntry() const;
    void SetAddition(uint8_t newAddition);
    void SetAdditionEntryIndex(ObjectEntryIndex entryIndex);

    bool AdditionIsGhost() const;
    void SetAdditionIsGhost(bool isGhost);

    uint8_t GetAdditionStatus() const;
    void SetAdditionStatus(uint8_t newStatus);

    bool ShouldDrawPathOverSupports() const;
    void SetShouldDrawPathOverSupports(bool on);

    bool IsLevelCrossing(const CoordsXY& coords) const;
};
static_assert(sizeof(PathElement) == kTileElementSize);
#pragma pack(pop)
