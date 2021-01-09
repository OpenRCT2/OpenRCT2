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

constexpr uint8_t TUNE_ID_NULL = 0xFF;

void RideAudioClearAllViewportInstances();
void RideAudioStopAllChannels();
void RideUpdateMusicChannels();
void RideUpdateMusicInstance(Ride& ride, const CoordsXYZ& rideCoords, uint16_t sampleRate);
