/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PathElement.h"

#include "../../Context.h"
#include "../../Diagnostic.h"
#include "../../object/FootpathObject.h"
#include "../../object/FootpathRailingsObject.h"
#include "../../object/FootpathSurfaceObject.h"
#include "../../object/ObjectEntryManager.h"
#include "../../object/ObjectManager.h"
#include "../../object/PathAdditionEntry.h"
#include "../Footpath.h"

bool PathElement::IsSloped() const
{
    return (Flags2 & FOOTPATH_ELEMENT_FLAGS2_IS_SLOPED) != 0;
}

void PathElement::SetSloped(bool isSloped)
{
    Flags2 &= ~FOOTPATH_ELEMENT_FLAGS2_IS_SLOPED;
    if (isSloped)
        Flags2 |= FOOTPATH_ELEMENT_FLAGS2_IS_SLOPED;
}

bool PathElement::HasJunctionRailings() const
{
    return Flags2 & FOOTPATH_ELEMENT_FLAGS2_HAS_JUNCTION_RAILINGS;
}

void PathElement::SetJunctionRailings(bool hasJunctionRailings)
{
    Flags2 &= ~FOOTPATH_ELEMENT_FLAGS2_HAS_JUNCTION_RAILINGS;
    if (hasJunctionRailings)
        Flags2 |= FOOTPATH_ELEMENT_FLAGS2_HAS_JUNCTION_RAILINGS;
}

Direction PathElement::GetSlopeDirection() const
{
    return SlopeDirection;
}

void PathElement::SetSlopeDirection(Direction newSlope)
{
    SlopeDirection = newSlope;
}

bool PathElement::IsQueue() const
{
    return (Type & FOOTPATH_ELEMENT_TYPE_FLAG_IS_QUEUE) != 0;
}

void PathElement::SetIsQueue(bool isQueue)
{
    Type &= ~FOOTPATH_ELEMENT_TYPE_FLAG_IS_QUEUE;
    if (isQueue)
        Type |= FOOTPATH_ELEMENT_TYPE_FLAG_IS_QUEUE;
}

bool PathElement::HasQueueBanner() const
{
    return (Flags2 & FOOTPATH_ELEMENT_FLAGS2_HAS_QUEUE_BANNER) != 0;
}

void PathElement::SetHasQueueBanner(bool hasQueueBanner)
{
    Flags2 &= ~FOOTPATH_ELEMENT_FLAGS2_HAS_QUEUE_BANNER;
    if (hasQueueBanner)
        Flags2 |= FOOTPATH_ELEMENT_FLAGS2_HAS_QUEUE_BANNER;
}

bool PathElement::IsBroken() const
{
    return (Flags2 & FOOTPATH_ELEMENT_FLAGS2_ADDITION_IS_BROKEN) != 0;
}

void PathElement::SetIsBroken(bool isBroken)
{
    if (isBroken)
    {
        Flags2 |= FOOTPATH_ELEMENT_FLAGS2_ADDITION_IS_BROKEN;
    }
    else
    {
        Flags2 &= ~FOOTPATH_ELEMENT_FLAGS2_ADDITION_IS_BROKEN;
    }
}

bool PathElement::IsBlockedByVehicle() const
{
    return (Flags2 & FOOTPATH_ELEMENT_FLAGS2_BLOCKED_BY_VEHICLE) != 0;
}

void PathElement::SetIsBlockedByVehicle(bool isBlocked)
{
    if (isBlocked)
    {
        Flags2 |= FOOTPATH_ELEMENT_FLAGS2_BLOCKED_BY_VEHICLE;
    }
    else
    {
        Flags2 &= ~FOOTPATH_ELEMENT_FLAGS2_BLOCKED_BY_VEHICLE;
    }
}

::StationIndex PathElement::GetStationIndex() const
{
    return StationIndex;
}

void PathElement::SetStationIndex(::StationIndex newStationIndex)
{
    StationIndex = newStationIndex;
}

bool PathElement::IsWide() const
{
    return (Type & FOOTPATH_ELEMENT_TYPE_FLAG_IS_WIDE) != 0;
}

void PathElement::SetWide(bool isWide)
{
    Type &= ~FOOTPATH_ELEMENT_TYPE_FLAG_IS_WIDE;
    if (isWide)
        Type |= FOOTPATH_ELEMENT_TYPE_FLAG_IS_WIDE;
}

bool PathElement::HasAddition() const
{
    return Additions != 0;
}

uint8_t PathElement::GetAddition() const
{
    return Additions;
}

ObjectEntryIndex PathElement::GetAdditionEntryIndex() const
{
    // `Additions` is set to 0 when there is no addition, so the value 1 corresponds with path addition slot 0, etc.
    return GetAddition() - 1;
}

const PathAdditionEntry* PathElement::GetAdditionEntry() const
{
    if (!HasAddition())
        return nullptr;
    return OpenRCT2::ObjectManager::GetObjectEntry<PathAdditionEntry>(GetAdditionEntryIndex());
}

void PathElement::SetAddition(uint8_t newAddition)
{
    Additions = newAddition;
}

void PathElement::SetAdditionEntryIndex(ObjectEntryIndex entryIndex)
{
    // `Additions` is set to 0 when there is no addition, so the value 1 corresponds with path addition slot 0, etc.
    Additions = entryIndex + 1;
}

bool PathElement::AdditionIsGhost() const
{
    return (Flags2 & FOOTPATH_ELEMENT_FLAGS2_ADDITION_IS_GHOST) != 0;
}

void PathElement::SetAdditionIsGhost(bool isGhost)
{
    Flags2 &= ~FOOTPATH_ELEMENT_FLAGS2_ADDITION_IS_GHOST;
    if (isGhost)
        Flags2 |= FOOTPATH_ELEMENT_FLAGS2_ADDITION_IS_GHOST;
}

ObjectEntryIndex PathElement::GetLegacyPathEntryIndex() const
{
    if (Flags2 & FOOTPATH_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY)
        return SurfaceIndex;

    return kObjectEntryIndexNull;
}

const FootpathObject* PathElement::GetLegacyPathEntry() const
{
    return GetLegacyFootpathEntry(GetLegacyPathEntryIndex());
}

void PathElement::SetLegacyPathEntryIndex(ObjectEntryIndex newIndex)
{
    SurfaceIndex = newIndex;
    RailingsIndex = kObjectEntryIndexNull;
    Flags2 |= FOOTPATH_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY;
}

bool PathElement::HasLegacyPathEntry() const
{
    return (Flags2 & FOOTPATH_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY) != 0;
}

const PathSurfaceDescriptor* PathElement::GetSurfaceDescriptor() const
{
    if (HasLegacyPathEntry())
    {
        const auto* legacyPathEntry = GetLegacyPathEntry();
        if (legacyPathEntry == nullptr)
            return nullptr;

        if (IsQueue())
            return &legacyPathEntry->GetQueueSurfaceDescriptor();

        return &legacyPathEntry->GetPathSurfaceDescriptor();
    }

    const auto* surfaceEntry = GetSurfaceEntry();
    if (surfaceEntry == nullptr)
        return nullptr;

    return &surfaceEntry->GetDescriptor();
}

const PathRailingsDescriptor* PathElement::GetRailingsDescriptor() const
{
    if (HasLegacyPathEntry())
    {
        const auto* legacyPathEntry = GetLegacyPathEntry();
        if (legacyPathEntry == nullptr)
            return nullptr;

        return &legacyPathEntry->GetPathRailingsDescriptor();
    }

    const auto* railingsEntry = GetRailingsEntry();
    if (railingsEntry == nullptr)
        return nullptr;

    return &railingsEntry->GetDescriptor();
}

ObjectEntryIndex PathElement::GetSurfaceEntryIndex() const
{
    if (Flags2 & FOOTPATH_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY)
        return kObjectEntryIndexNull;

    return SurfaceIndex;
}

const FootpathSurfaceObject* PathElement::GetSurfaceEntry() const
{
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    return objMgr.GetLoadedObject<FootpathSurfaceObject>(GetSurfaceEntryIndex());
}

void PathElement::SetSurfaceEntryIndex(ObjectEntryIndex newIndex)
{
    SurfaceIndex = newIndex;
    Flags2 &= ~FOOTPATH_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY;
}

ObjectEntryIndex PathElement::GetRailingsEntryIndex() const
{
    if (Flags2 & FOOTPATH_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY)
        return kObjectEntryIndexNull;

    return RailingsIndex;
}

const FootpathRailingsObject* PathElement::GetRailingsEntry() const
{
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    return objMgr.GetLoadedObject<FootpathRailingsObject>(GetRailingsEntryIndex());
}

void PathElement::SetRailingsEntryIndex(ObjectEntryIndex newEntryIndex)
{
    RailingsIndex = newEntryIndex;
    Flags2 &= ~FOOTPATH_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY;
}

uint8_t PathElement::GetQueueBannerDirection() const
{
    return ((Type & FOOTPATH_ELEMENT_TYPE_DIRECTION_MASK) >> 6);
}

void PathElement::SetQueueBannerDirection(uint8_t direction)
{
    Type &= ~FOOTPATH_ELEMENT_TYPE_DIRECTION_MASK;
    Type |= (direction << 6);
}

bool PathElement::ShouldDrawPathOverSupports() const
{
    // TODO: make this an actual decision of the tile element.
    return (GetRailingsDescriptor()->Flags & RAILING_ENTRY_FLAG_DRAW_PATH_OVER_SUPPORTS);
}

void PathElement::SetShouldDrawPathOverSupports(bool on)
{
    LOG_VERBOSE("Setting 'draw path over supports' to %d", static_cast<size_t>(on));
}
