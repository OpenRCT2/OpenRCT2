/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

namespace OpenRCT2
{
    enum class TrackElemType : uint16_t;
}

namespace OpenRCT2::TrackMetadata
{
    enum class TrackRoll : uint8_t
    {
        none = 0,
        left = 2,
        right = 4,
        upsideDown = 15,
    };

    enum class TrackPitch : uint8_t
    {
        none = 0,
        up25 = 2,
        up60 = 4,
        down25 = 6,
        down60 = 8,
        up90 = 10,
        down90 = 18,

        tower = 10,
        reverseFreefall = 10
    };

    struct PitchAndRoll
    {
        TrackPitch pitch;
        TrackRoll roll;
    };
    constexpr bool operator==(const PitchAndRoll& vb1, const PitchAndRoll& vb2)
    {
        return vb1.pitch == vb2.pitch && vb1.roll == vb2.roll;
    }
    constexpr bool operator!=(const PitchAndRoll& vb1, const PitchAndRoll& vb2)
    {
        return !(vb1 == vb2);
    }

    PitchAndRoll TrackPitchAndRollStart(TrackElemType trackType);
    PitchAndRoll TrackPitchAndRollEnd(TrackElemType trackType);
} // namespace OpenRCT2::TrackMetadata
