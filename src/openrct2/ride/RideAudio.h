/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

struct CoordsXYZ;
struct Ride;

namespace OpenRCT2::RideAudio
{
    void ClearAllViewportInstances();
    void StopAllChannels();
    void UpdateMusicChannels();
    void UpdateMusicInstance(Ride& ride, const CoordsXYZ& rideCoords, uint16_t sampleRate);
} // namespace OpenRCT2::RideAudio
