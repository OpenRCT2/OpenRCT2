/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "EntranceElement.h"

#include "../../Context.h"
#include "../../object/FootpathObject.h"
#include "../../object/FootpathSurfaceObject.h"
#include "../../object/ObjectManager.h"

namespace OpenRCT2
{
    // rct2: 0x0097B974
    static constexpr std::array<std::array<uint16_t, 8>, 3> kEntranceDirections = { {
        { (4), 0, 0, 0, 0, 0, 0, 0 },     // EntranceType::rideEntrance,
        { (4), 0, 0, 0, 0, 0, 0, 0 },     // EntranceType::rideExit,
        { (4 | 1), 0, 0, 0, 0, 0, 0, 0 }, // EntranceType::parkEntrance
    } };

    EntranceType EntranceElement::getEntranceType() const
    {
        return entranceType;
    }

    void EntranceElement::setEntranceType(EntranceType newType)
    {
        entranceType = newType;
    }

    RideId EntranceElement::getRideIndex() const
    {
        return rideIndex;
    }

    void EntranceElement::setRideIndex(RideId newRideIndex)
    {
        rideIndex = newRideIndex;
    }

    StationIndex EntranceElement::getStationIndex() const
    {
        return stationIndex;
    }

    void EntranceElement::setStationIndex(StationIndex newStationIndex)
    {
        stationIndex = newStationIndex;
    }

    uint8_t EntranceElement::getSequenceIndex() const
    {
        return sequenceIndex & 0xF;
    }

    void EntranceElement::setSequenceIndex(uint8_t newSequenceIndex)
    {
        sequenceIndex &= ~0xF;
        sequenceIndex |= (newSequenceIndex & 0xF);
    }

    bool EntranceElement::hasLegacyPathEntry() const
    {
        return (flags2 & ENTRANCE_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY) != 0;
    }

    ObjectEntryIndex EntranceElement::getLegacyPathEntryIndex() const
    {
        if (hasLegacyPathEntry())
            return pathType;

        return kObjectEntryIndexNull;
    }

    const FootpathObject* EntranceElement::getLegacyPathEntry() const
    {
        auto& objMgr = GetContext()->GetObjectManager();
        return objMgr.GetLoadedObject<FootpathObject>(getLegacyPathEntryIndex());
    }

    void EntranceElement::setLegacyPathEntryIndex(ObjectEntryIndex newPathType)
    {
        pathType = newPathType;
        flags2 |= ENTRANCE_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY;
    }

    ObjectEntryIndex EntranceElement::getSurfaceEntryIndex() const
    {
        if (hasLegacyPathEntry())
            return kObjectEntryIndexNull;

        return pathType;
    }

    const FootpathSurfaceObject* EntranceElement::getSurfaceEntry() const
    {
        auto& objMgr = GetContext()->GetObjectManager();
        return objMgr.GetLoadedObject<FootpathSurfaceObject>(getSurfaceEntryIndex());
    }

    void EntranceElement::setSurfaceEntryIndex(ObjectEntryIndex newIndex)
    {
        pathType = newIndex;
        flags2 &= ~ENTRANCE_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY;
    }

    const PathSurfaceDescriptor* EntranceElement::getPathSurfaceDescriptor() const
    {
        if (hasLegacyPathEntry())
        {
            const auto* legacyPathEntry = getLegacyPathEntry();
            if (legacyPathEntry == nullptr)
                return nullptr;

            return &legacyPathEntry->GetPathSurfaceDescriptor();
        }

        const auto* surfaceEntry = getSurfaceEntry();
        if (surfaceEntry == nullptr)
            return nullptr;

        return &surfaceEntry->GetDescriptor();
    }

    int32_t EntranceElement::getDirections() const
    {
        return kEntranceDirections[EnumValue(getEntranceType())][getSequenceIndex()];
    }

    ObjectEntryIndex EntranceElement::getEntryIndex() const
    {
        return entryIndex;
    }

    void EntranceElement::setEntryIndex(ObjectEntryIndex newIndex)
    {
        entryIndex = newIndex;
    }
} // namespace OpenRCT2
