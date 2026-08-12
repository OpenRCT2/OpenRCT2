/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../Identifiers.h"
#include "../../core/FlagHolder.hpp"
#include "TileElementBase.h"

namespace OpenRCT2
{
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

    // Masks and flags for values stored in TileElement.properties.path.edges
    enum
    {
        FOOTPATH_PROPERTIES_EDGES_EDGES_MASK = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3),
        FOOTPATH_PROPERTIES_EDGES_CORNERS_MASK = (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7),
    };

    enum class FootpathElementFlag : uint8_t
    {
        isSloped,
        hasQueueBanner,
        additionIsGhost,
        blockedByVehicle,
        additionIsBroken,
        isLegacyPathEntry,
        hasJunctionRailings,
    };
    using FootpathElementFlags = FlagHolder<uint8_t, FootpathElementFlag>;

#pragma pack(push, 1)
    struct PathElement : TileElementBase
    {
        static constexpr TileElementType kElementType = TileElementType::path;

    private:
        ObjectEntryIndex surfaceIndex;  // 5
        ObjectEntryIndex railingsIndex; // 7
        uint8_t additions;              // 9 (0 means no addition)
        uint8_t edgesAndCorners;        // 10 (edges in lower 4 bits, corners in upper 4)
        FootpathElementFlags flags2;    // 11
        uint8_t slopeDirection;         // 12
        union
        {
            uint8_t additionStatus; // 13, only used for litter bins
            RideId rideIndex;       // 13
        };
        ::StationIndex stationIndex; // 15

    public:
        ObjectEntryIndex getLegacyPathEntryIndex() const;
        const FootpathObject* getLegacyPathEntry() const;
        void setLegacyPathEntryIndex(ObjectEntryIndex newIndex);
        bool hasLegacyPathEntry() const;

        ObjectEntryIndex getSurfaceEntryIndex() const;
        const FootpathSurfaceObject* getSurfaceEntry() const;
        void setSurfaceEntryIndex(ObjectEntryIndex newIndex);

        ObjectEntryIndex getRailingsEntryIndex() const;
        const FootpathRailingsObject* getRailingsEntry() const;
        void setRailingsEntryIndex(ObjectEntryIndex newIndex);

        const PathSurfaceDescriptor* getSurfaceDescriptor() const;
        const PathRailingsDescriptor* getRailingsDescriptor() const;

        uint8_t getQueueBannerDirection() const;
        void setQueueBannerDirection(uint8_t direction);

        bool isSloped() const;
        void setSloped(bool isSloped);

        bool hasJunctionRailings() const;
        void setJunctionRailings(bool hasJunctionRailings);

        Direction getSlopeDirection() const;
        void setSlopeDirection(Direction newSlope);

        RideId getRideIndex() const;
        void setRideIndex(RideId newRideIndex);

        ::StationIndex getStationIndex() const;
        void setStationIndex(::StationIndex newStationIndex);

        bool isWide() const;
        void setWide(bool isWide);

        bool isQueue() const;
        void setIsQueue(bool isQueue);
        bool hasQueueBanner() const;
        void setHasQueueBanner(bool hasQueueBanner);

        bool isBroken() const;
        void setIsBroken(bool isBroken);

        bool isBlockedByVehicle() const;
        void setIsBlockedByVehicle(bool isBlocked);

        uint8_t getEdges() const;
        void setEdges(uint8_t newEdges);
        uint8_t getCorners() const;
        void setCorners(uint8_t newCorners);
        uint8_t getEdgesAndCorners() const;
        void setEdgesAndCorners(uint8_t newEdgesAndCorners);

        bool hasAddition() const;
        uint8_t getAddition() const;
        ObjectEntryIndex getAdditionEntryIndex() const;
        const PathAdditionEntry* getAdditionEntry() const;
        void setAddition(uint8_t newAddition);
        void setAdditionEntryIndex(ObjectEntryIndex entryIndex);

        bool additionIsGhost() const;
        void setAdditionIsGhost(bool isGhost);

        uint8_t getAdditionStatus() const;
        void setAdditionStatus(uint8_t newStatus);

        bool shouldDrawPathOverSupports() const;
        void setShouldDrawPathOverSupports(bool on);

        bool isLevelCrossing(const CoordsXY& coords) const;
    };
    static_assert(sizeof(PathElement) == kTileElementSize);
#pragma pack(pop)
} // namespace OpenRCT2
