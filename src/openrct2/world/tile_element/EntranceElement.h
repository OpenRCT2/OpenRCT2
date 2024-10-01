/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../Identifiers.h"
#include "../TileElement.h"

#include <cstdint>

enum
{
    ENTRANCE_TYPE_RIDE_ENTRANCE,
    ENTRANCE_TYPE_RIDE_EXIT,
    ENTRANCE_TYPE_PARK_ENTRANCE
};

#pragma pack(push, 1)
struct EntranceElement;

struct EntranceElement : TileElementBase
{
    static constexpr TileElementType ElementType = TileElementType::Entrance;

private:
    uint8_t entranceType;        // 5
    uint8_t SequenceIndex;       // 6. Only uses the lower nibble.
    StationIndex stationIndex;   // 7
    ObjectEntryIndex PathType;   // 8
    RideId rideIndex;            // A
    uint8_t flags2;              // C
    ObjectEntryIndex entryIndex; // D
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t Pad0F[1];
#pragma clang diagnostic pop

public:
    uint8_t GetEntranceType() const;
    void SetEntranceType(uint8_t newType);

    RideId GetRideIndex() const;
    void SetRideIndex(RideId newRideIndex);

    StationIndex GetStationIndex() const;
    void SetStationIndex(StationIndex newStationIndex);

    uint8_t GetSequenceIndex() const;
    void SetSequenceIndex(uint8_t newSequenceIndex);

    bool HasLegacyPathEntry() const;

    ObjectEntryIndex GetLegacyPathEntryIndex() const;
    const FootpathObject* GetLegacyPathEntry() const;
    void SetLegacyPathEntryIndex(ObjectEntryIndex newPathType);

    ObjectEntryIndex GetSurfaceEntryIndex() const;
    const FootpathSurfaceObject* GetSurfaceEntry() const;
    void SetSurfaceEntryIndex(ObjectEntryIndex newIndex);

    const PathSurfaceDescriptor* GetPathSurfaceDescriptor() const;

    int32_t GetDirections() const;

    ObjectEntryIndex getEntryIndex() const;
    void setEntryIndex(ObjectEntryIndex newIndex);
};
static_assert(sizeof(EntranceElement) == kTileElementSize);
#pragma pack(pop)
