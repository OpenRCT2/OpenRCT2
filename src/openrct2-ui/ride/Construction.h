/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/core/EnumUtils.hpp>
#include <openrct2/ride/RideTypes.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/ted/TrackElemType.h>
#include <vector>

namespace OpenRCT2
{
    namespace TrackMetadata
    {
        enum class TrackPitch : uint8_t;
        enum class TrackRoll : uint8_t;
    } // namespace TrackMetadata

    enum class RideConstructionState : uint8_t;

    struct SpecialElement
    {
        TrackElemType TrackType;
        bool Disabled;
    };

    struct SpecialElementsDropdownState
    {
        bool HasActiveElements = false;
        std::vector<SpecialElement> Elements{};
        size_t PreferredNumRows{};
    };

    SpecialElementsDropdownState BuildSpecialElementsList(
        const Ride& currentRide, uint8_t buildDirection, TrackMetadata::TrackPitch buildSlope,
        TrackMetadata::TrackRoll buildBank, RideConstructionState state);

    void RideConstructNew(RideSelection listItem);
    CoordsXYZD RideGetEntranceOrExitPositionFromScreenPosition(const ScreenCoordsXY& screenCoords);
} // namespace OpenRCT2
