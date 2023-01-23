/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"

#include <cstdint>
#include <utility>
namespace OpenRCT2
{
    struct CoordsXYZ;
    struct Ride;

    namespace RideAudio
    {
        /**
         * Represents a particular instance of ride music that can be heard in a viewport.
         * These are created each frame via enumerating each ride / viewport.
         */
        struct ViewportRideMusicInstance
        {
            ::RideId RideId;
            uint8_t TrackIndex{};

            size_t Offset{};
            int16_t Volume{};
            int16_t Pan{};
            uint16_t Frequency{};
        };

        void ClearAllViewportInstances();
        void StopAllChannels();
        void UpdateMusicChannels();
        void UpdateMusicInstance(Ride& ride, const CoordsXYZ& rideCoords, uint16_t sampleRate);

        void DefaultStartRideMusicChannel(const ViewportRideMusicInstance& instance);
        void CircusStartRideMusicChannel(const ViewportRideMusicInstance& instance);

        std::pair<size_t, size_t> RideMusicGetTrackOffsetLength_Circus(const Ride& ride);
        std::pair<size_t, size_t> RideMusicGetTrackOffsetLength_Default(const Ride& ride);
    } // namespace RideAudio
} // namespace OpenRCT2
