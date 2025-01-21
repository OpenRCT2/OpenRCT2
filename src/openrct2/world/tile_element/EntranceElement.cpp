/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "EntranceElement.h"

#include "../../Context.h"
#include "../../object/EntranceObject.h"
#include "../../object/FootpathObject.h"
#include "../../object/FootpathSurfaceObject.h"
#include "../../object/ObjectManager.h"
#include "../Entrance.h"

// rct2: 0x0097B974
static constexpr uint16_t kEntranceDirections[] = {
    (4),     0, 0, 0, 0, 0, 0, 0, // ENTRANCE_TYPE_RIDE_ENTRANCE,
    (4),     0, 0, 0, 0, 0, 0, 0, // ENTRANCE_TYPE_RIDE_EXIT,
    (4 | 1), 0, 0, 0, 0, 0, 0, 0, // ENTRANCE_TYPE_PARK_ENTRANCE
};

uint8_t EntranceElement::GetEntranceType() const
{
    return entranceType;
}

void EntranceElement::SetEntranceType(uint8_t newType)
{
    entranceType = newType;
}

RideId EntranceElement::GetRideIndex() const
{
    return rideIndex;
}

void EntranceElement::SetRideIndex(RideId newRideIndex)
{
    rideIndex = newRideIndex;
}

StationIndex EntranceElement::GetStationIndex() const
{
    return stationIndex;
}

void EntranceElement::SetStationIndex(StationIndex newStationIndex)
{
    stationIndex = newStationIndex;
}

uint8_t EntranceElement::GetSequenceIndex() const
{
    return SequenceIndex & 0xF;
}

void EntranceElement::SetSequenceIndex(uint8_t newSequenceIndex)
{
    SequenceIndex &= ~0xF;
    SequenceIndex |= (newSequenceIndex & 0xF);
}

bool EntranceElement::HasLegacyPathEntry() const
{
    return (flags2 & ENTRANCE_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY) != 0;
}

ObjectEntryIndex EntranceElement::GetLegacyPathEntryIndex() const
{
    if (HasLegacyPathEntry())
        return PathType;

    return kObjectEntryIndexNull;
}

const FootpathObject* EntranceElement::GetLegacyPathEntry() const
{
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    return objMgr.GetLoadedObject<FootpathObject>(GetLegacyPathEntryIndex());
}

void EntranceElement::SetLegacyPathEntryIndex(ObjectEntryIndex newPathType)
{
    PathType = newPathType;
    flags2 |= ENTRANCE_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY;
}

ObjectEntryIndex EntranceElement::GetSurfaceEntryIndex() const
{
    if (HasLegacyPathEntry())
        return kObjectEntryIndexNull;

    return PathType;
}

const FootpathSurfaceObject* EntranceElement::GetSurfaceEntry() const
{
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    return objMgr.GetLoadedObject<FootpathSurfaceObject>(GetSurfaceEntryIndex());
}

void EntranceElement::SetSurfaceEntryIndex(ObjectEntryIndex newIndex)
{
    PathType = newIndex;
    flags2 &= ~ENTRANCE_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY;
}

const PathSurfaceDescriptor* EntranceElement::GetPathSurfaceDescriptor() const
{
    if (HasLegacyPathEntry())
    {
        const auto* legacyPathEntry = GetLegacyPathEntry();
        if (legacyPathEntry == nullptr)
            return nullptr;

        return &legacyPathEntry->GetPathSurfaceDescriptor();
    }

    const auto* surfaceEntry = GetSurfaceEntry();
    if (surfaceEntry == nullptr)
        return nullptr;

    return &surfaceEntry->GetDescriptor();
}

int32_t EntranceElement::GetDirections() const
{
    return kEntranceDirections[(GetEntranceType() * 8) + GetSequenceIndex()];
}

ObjectEntryIndex EntranceElement::getEntryIndex() const
{
    return entryIndex;
}

void EntranceElement::setEntryIndex(ObjectEntryIndex newIndex)
{
    entryIndex = newIndex;
}
