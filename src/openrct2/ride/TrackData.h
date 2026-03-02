/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../paint/support/MetalSupports.h"
#include "ted/TrackElemType.h"

namespace OpenRCT2::TrackMetadata
{
    enum class TrackCurve : uint8_t;
    enum class TrackPitch : uint8_t;
    enum class TrackRoll : uint8_t;
    struct TrackElementDescriptor;

    struct TrackDescriptor
    {
        bool startsDiagonally;
        TrackPitch slopeStart;
        TrackRoll rollStart;
        TrackCurve trackCurve;
        TrackPitch slopeEnd;
        TrackRoll rollEnd;
        TrackElemType trackElement;
    };

    extern const TrackDescriptor gTrackDescriptors[186];

    struct DodgemsTrackSize
    {
        uint8_t left;
        uint8_t top;
        uint8_t right;
        uint8_t bottom;
    };

    constexpr DodgemsTrackSize GetDodgemsTrackSize(TrackElemType type)
    {
        if (type == TrackElemType::flatTrack2x2)
            return { 4, 4, 59, 59 };
        if (type == TrackElemType::flatTrack4x4)
            return { 4, 4, 123, 123 };
        if (type == TrackElemType::flatTrack2x4)
            return { 4, 4, 59, 123 };
        return { 0, 0, 0, 0 };
    }

    const TrackElementDescriptor& GetTrackElementDescriptor(TrackElemType type);
} // namespace OpenRCT2::TrackMetadata
