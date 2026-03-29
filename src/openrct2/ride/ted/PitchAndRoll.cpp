/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PitchAndRoll.h"

#include "../TrackData.h"
#include "TrackElementDescriptor.h"

namespace OpenRCT2::TrackMetadata
{
    PitchAndRoll TrackPitchAndRollStart(TrackElemType trackType)
    {
        const auto& ted = GetTrackElementDescriptor(trackType);
        return { ted.definition.pitchStart, ted.definition.rollStart };
    }

    PitchAndRoll TrackPitchAndRollEnd(TrackElemType trackType)
    {
        const auto& ted = GetTrackElementDescriptor(trackType);
        return { ted.definition.pitchEnd, ted.definition.rollEnd };
    }
} // namespace OpenRCT2::TrackMetadata
