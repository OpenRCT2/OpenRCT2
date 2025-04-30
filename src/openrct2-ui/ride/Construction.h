/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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

namespace OpenRCT2
{
    enum class RideConstructionState : uint8_t;

    struct SpecialElement
    {
        OpenRCT2::TrackElemType TrackType;
        bool Disabled;
    };

    struct SpecialElementsDropdownState
    {
        bool HasActiveElements = false;
        std::vector<SpecialElement> Elements{};
        size_t PreferredNumRows{};
    };

    SpecialElementsDropdownState BuildSpecialElementsList(
        const Ride& currentRide, uint8_t buildDirection, TrackPitch buildSlope, TrackRoll buildBank,
        RideConstructionState state);

    void RideConstructNew(RideSelection listItem);
    CoordsXYZD RideGetEntranceOrExitPositionFromScreenPosition(const ScreenCoordsXY& screenCoords);
} // namespace OpenRCT2
