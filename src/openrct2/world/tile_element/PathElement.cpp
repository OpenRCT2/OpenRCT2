/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

namespace OpenRCT2
{
    bool PathElement::isSloped() const
    {
        return (flags2 & FOOTPATH_ELEMENT_FLAGS2_IS_SLOPED) != 0;
    }

    void PathElement::setSloped(bool isSloped)
    {
        flags2 &= ~FOOTPATH_ELEMENT_FLAGS2_IS_SLOPED;
        if (isSloped)
            flags2 |= FOOTPATH_ELEMENT_FLAGS2_IS_SLOPED;
    }

    bool PathElement::hasJunctionRailings() const
    {
        return flags2 & FOOTPATH_ELEMENT_FLAGS2_HAS_JUNCTION_RAILINGS;
    }

    void PathElement::setJunctionRailings(bool hasJunctionRailings)
    {
        flags2 &= ~FOOTPATH_ELEMENT_FLAGS2_HAS_JUNCTION_RAILINGS;
        if (hasJunctionRailings)
            flags2 |= FOOTPATH_ELEMENT_FLAGS2_HAS_JUNCTION_RAILINGS;
    }

    Direction PathElement::getSlopeDirection() const
    {
        return slopeDirection;
    }

    void PathElement::setSlopeDirection(Direction newSlope)
    {
        slopeDirection = newSlope;
    }

    bool PathElement::isQueue() const
    {
        return (type & FOOTPATH_ELEMENT_TYPE_FLAG_IS_QUEUE) != 0;
    }

    void PathElement::setIsQueue(bool isQueue)
    {
        type &= ~FOOTPATH_ELEMENT_TYPE_FLAG_IS_QUEUE;
        if (isQueue)
            type |= FOOTPATH_ELEMENT_TYPE_FLAG_IS_QUEUE;
    }

    bool PathElement::hasQueueBanner() const
    {
        return (flags2 & FOOTPATH_ELEMENT_FLAGS2_HAS_QUEUE_BANNER) != 0;
    }

    void PathElement::setHasQueueBanner(bool hasQueueBanner)
    {
        flags2 &= ~FOOTPATH_ELEMENT_FLAGS2_HAS_QUEUE_BANNER;
        if (hasQueueBanner)
            flags2 |= FOOTPATH_ELEMENT_FLAGS2_HAS_QUEUE_BANNER;
    }

    bool PathElement::isBroken() const
    {
        return (flags2 & FOOTPATH_ELEMENT_FLAGS2_ADDITION_IS_BROKEN) != 0;
    }

    void PathElement::setIsBroken(bool isBroken)
    {
        if (isBroken)
        {
            flags2 |= FOOTPATH_ELEMENT_FLAGS2_ADDITION_IS_BROKEN;
        }
        else
        {
            flags2 &= ~FOOTPATH_ELEMENT_FLAGS2_ADDITION_IS_BROKEN;
        }
    }

    bool PathElement::isBlockedByVehicle() const
    {
        return (flags2 & FOOTPATH_ELEMENT_FLAGS2_BLOCKED_BY_VEHICLE) != 0;
    }

    void PathElement::setIsBlockedByVehicle(bool isBlocked)
    {
        if (isBlocked)
        {
            flags2 |= FOOTPATH_ELEMENT_FLAGS2_BLOCKED_BY_VEHICLE;
        }
        else
        {
            flags2 &= ~FOOTPATH_ELEMENT_FLAGS2_BLOCKED_BY_VEHICLE;
        }
    }

    ::StationIndex PathElement::getStationIndex() const
    {
        return stationIndex;
    }

    void PathElement::setStationIndex(::StationIndex newStationIndex)
    {
        stationIndex = newStationIndex;
    }

    bool PathElement::isWide() const
    {
        return (type & FOOTPATH_ELEMENT_TYPE_FLAG_IS_WIDE) != 0;
    }

    void PathElement::setWide(bool isWide)
    {
        type &= ~FOOTPATH_ELEMENT_TYPE_FLAG_IS_WIDE;
        if (isWide)
            type |= FOOTPATH_ELEMENT_TYPE_FLAG_IS_WIDE;
    }

    bool PathElement::hasAddition() const
    {
        return additions != 0;
    }

    uint8_t PathElement::getAddition() const
    {
        return additions;
    }

    ObjectEntryIndex PathElement::getAdditionEntryIndex() const
    {
        // `Additions` is set to 0 when there is no addition, so the value 1 corresponds with path addition slot 0, etc.
        return getAddition() - 1;
    }

    const PathAdditionEntry* PathElement::getAdditionEntry() const
    {
        if (!hasAddition())
            return nullptr;
        return ObjectEntryManager::GetObjectEntry<PathAdditionEntry>(getAdditionEntryIndex());
    }

    void PathElement::setAddition(uint8_t newAddition)
    {
        additions = newAddition;
    }

    void PathElement::setAdditionEntryIndex(ObjectEntryIndex entryIndex)
    {
        // `Additions` is set to 0 when there is no addition, so the value 1 corresponds with path addition slot 0, etc.
        additions = entryIndex + 1;
    }

    bool PathElement::additionIsGhost() const
    {
        return (flags2 & FOOTPATH_ELEMENT_FLAGS2_ADDITION_IS_GHOST) != 0;
    }

    void PathElement::setAdditionIsGhost(bool isGhost)
    {
        flags2 &= ~FOOTPATH_ELEMENT_FLAGS2_ADDITION_IS_GHOST;
        if (isGhost)
            flags2 |= FOOTPATH_ELEMENT_FLAGS2_ADDITION_IS_GHOST;
    }

    ObjectEntryIndex PathElement::getLegacyPathEntryIndex() const
    {
        if (flags2 & FOOTPATH_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY)
            return surfaceIndex;

        return kObjectEntryIndexNull;
    }

    const FootpathObject* PathElement::getLegacyPathEntry() const
    {
        return GetLegacyFootpathEntry(getLegacyPathEntryIndex());
    }

    void PathElement::setLegacyPathEntryIndex(ObjectEntryIndex newIndex)
    {
        surfaceIndex = newIndex;
        railingsIndex = kObjectEntryIndexNull;
        flags2 |= FOOTPATH_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY;
    }

    bool PathElement::hasLegacyPathEntry() const
    {
        return (flags2 & FOOTPATH_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY) != 0;
    }

    const PathSurfaceDescriptor* PathElement::getSurfaceDescriptor() const
    {
        if (hasLegacyPathEntry())
        {
            const auto* legacyPathEntry = getLegacyPathEntry();
            if (legacyPathEntry == nullptr)
                return nullptr;

            if (isQueue())
                return &legacyPathEntry->GetQueueSurfaceDescriptor();

            return &legacyPathEntry->GetPathSurfaceDescriptor();
        }

        const auto* surfaceEntry = getSurfaceEntry();
        if (surfaceEntry == nullptr)
            return nullptr;

        return &surfaceEntry->GetDescriptor();
    }

    const PathRailingsDescriptor* PathElement::getRailingsDescriptor() const
    {
        if (hasLegacyPathEntry())
        {
            const auto* legacyPathEntry = getLegacyPathEntry();
            if (legacyPathEntry == nullptr)
                return nullptr;

            return &legacyPathEntry->GetPathRailingsDescriptor();
        }

        const auto* railingsEntry = getRailingsEntry();
        if (railingsEntry == nullptr)
            return nullptr;

        return &railingsEntry->GetDescriptor();
    }

    ObjectEntryIndex PathElement::getSurfaceEntryIndex() const
    {
        if (flags2 & FOOTPATH_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY)
            return kObjectEntryIndexNull;

        return surfaceIndex;
    }

    const FootpathSurfaceObject* PathElement::getSurfaceEntry() const
    {
        auto& objMgr = GetContext()->GetObjectManager();
        return objMgr.GetLoadedObject<FootpathSurfaceObject>(getSurfaceEntryIndex());
    }

    void PathElement::setSurfaceEntryIndex(ObjectEntryIndex newIndex)
    {
        surfaceIndex = newIndex;
        flags2 &= ~FOOTPATH_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY;
    }

    ObjectEntryIndex PathElement::getRailingsEntryIndex() const
    {
        if (flags2 & FOOTPATH_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY)
            return kObjectEntryIndexNull;

        return railingsIndex;
    }

    const FootpathRailingsObject* PathElement::getRailingsEntry() const
    {
        auto& objMgr = GetContext()->GetObjectManager();
        return objMgr.GetLoadedObject<FootpathRailingsObject>(getRailingsEntryIndex());
    }

    void PathElement::setRailingsEntryIndex(ObjectEntryIndex newIndex)
    {
        railingsIndex = newIndex;
        flags2 &= ~FOOTPATH_ELEMENT_FLAGS2_LEGACY_PATH_ENTRY;
    }

    uint8_t PathElement::getQueueBannerDirection() const
    {
        return ((type & FOOTPATH_ELEMENT_TYPE_DIRECTION_MASK) >> 6);
    }

    void PathElement::setQueueBannerDirection(uint8_t direction)
    {
        type &= ~FOOTPATH_ELEMENT_TYPE_DIRECTION_MASK;
        type |= (direction << 6);
    }

    bool PathElement::shouldDrawPathOverSupports() const
    {
        // TODO: make this an actual decision of the tile element.
        const auto* const railings = getRailingsDescriptor();
        return railings != nullptr ? railings->flags & RAILING_ENTRY_FLAG_DRAW_PATH_OVER_SUPPORTS : false;
    }

    void PathElement::setShouldDrawPathOverSupports(bool on)
    {
        LOG_VERBOSE("Setting 'draw path over supports' to %d", static_cast<size_t>(on));
    }
} // namespace OpenRCT2
