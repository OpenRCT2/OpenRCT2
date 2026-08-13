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
#include "../../object/ObjectTypes.h"
#include "TileElementBase.h"

#include <cstdint>

namespace OpenRCT2
{
    class FootpathObject;
    class FootpathSurfaceObject;
    struct PathSurfaceDescriptor;

    enum class EntranceType : uint8_t
    {
        rideEntrance,
        rideExit,
        parkEntrance
    };

    enum class EntranceElementFlag : uint8_t
    {
        isLegacyPathEntry,
    };
    using EntranceElementFlags = FlagHolder<uint8_t, EntranceElementFlag>;

    enum class ParkEntranceSequence : uint8_t
    {
        centre = 0,
        left = 1,
        right = 2,
    };

#pragma pack(push, 1)

    struct EntranceElement : TileElementBase
    {
        static constexpr TileElementType kElementType = TileElementType::entrance;

    private:
        EntranceType entranceType;   // 5
        uint8_t sequenceIndex;       // 6. Only uses the lower nibble.
        StationIndex stationIndex;   // 7
        ObjectEntryIndex pathType;   // 8
        RideId rideIndex;            // A
        EntranceElementFlags flags2; // C
        ObjectEntryIndex entryIndex; // D
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
        uint8_t pad0F[1];
#pragma clang diagnostic pop

    public:
        EntranceType getEntranceType() const;
        void setEntranceType(EntranceType newType);

        RideId getRideIndex() const;
        void setRideIndex(RideId newRideIndex);

        StationIndex getStationIndex() const;
        void setStationIndex(StationIndex newStationIndex);

        ParkEntranceSequence getSequenceIndex() const;
        void setSequenceIndex(ParkEntranceSequence newSequenceIndex);

        bool hasLegacyPathEntry() const;

        ObjectEntryIndex getLegacyPathEntryIndex() const;
        const FootpathObject* getLegacyPathEntry() const;
        void setLegacyPathEntryIndex(ObjectEntryIndex newPathType);

        ObjectEntryIndex getSurfaceEntryIndex() const;
        const FootpathSurfaceObject* getSurfaceEntry() const;
        void setSurfaceEntryIndex(ObjectEntryIndex newIndex);

        const PathSurfaceDescriptor* getPathSurfaceDescriptor() const;

        int32_t getDirections() const;

        ObjectEntryIndex getEntryIndex() const;
        void setEntryIndex(ObjectEntryIndex newIndex);
    };
    static_assert(sizeof(EntranceElement) == kTileElementSize);
#pragma pack(pop)
} // namespace OpenRCT2
