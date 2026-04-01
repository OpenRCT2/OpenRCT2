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
    struct TrackElementDescriptor;

    const TrackElementDescriptor& GetTrackElementDescriptor(TrackElemType type);
} // namespace OpenRCT2::TrackMetadata
