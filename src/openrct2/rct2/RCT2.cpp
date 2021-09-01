/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../object/Object.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"

#include <cstdint>

RideType RCT2RideTypeToOpenRCT2RideType(RideType rct2RideType, const rct_ride_entry* rideEntry)
{
    switch (rct2RideType)
    {
        case RideType::CORKSCREW_ROLLER_COASTER:
            if (rideEntry != nullptr && !(ride_entry_get_supported_track_pieces(rideEntry) & (1ULL << TRACK_VERTICAL_LOOP)))
                return RideType::HYPERCOASTER;
            else
                return RideType::CORKSCREW_ROLLER_COASTER;
        case RideType::JUNIOR_ROLLER_COASTER:
            if (rideEntry != nullptr && ride_entry_get_supported_track_pieces(rideEntry) & (1ULL << TRACK_SLOPE_STEEP))
                return RideType::CLASSIC_MINI_ROLLER_COASTER;
            else
                return RideType::JUNIOR_ROLLER_COASTER;
        case RideType::CAR_RIDE:
            if (rideEntry != nullptr && ride_entry_get_supported_track_pieces(rideEntry) & (1ULL << TRACK_SLOPE_STEEP))
                return RideType::MONSTER_TRUCKS;
            else
                return RideType::CAR_RIDE;
        case RideType::TWISTER_ROLLER_COASTER:
            if (rideEntry != nullptr && rideEntry->flags & RIDE_ENTRY_FLAG_NO_INVERSIONS)
                return RideType::HYPER_TWISTER;
            else
                return RideType::TWISTER_ROLLER_COASTER;
        case RideType::STEEL_WILD_MOUSE:
            if (rideEntry != nullptr && !(ride_entry_get_supported_track_pieces(rideEntry) & (1ULL << TRACK_SLOPE_STEEP)))
                return RideType::SPINNING_WILD_MOUSE;
            else
                return RideType::STEEL_WILD_MOUSE;

        default:
            return rct2RideType;
    }
}

bool RCT2RideTypeNeedsConversion(RideType rct2RideType)
{
    switch (rct2RideType)
    {
        case RideType::CORKSCREW_ROLLER_COASTER:
        case RideType::JUNIOR_ROLLER_COASTER:
        case RideType::CAR_RIDE:
        case RideType::TWISTER_ROLLER_COASTER:
        case RideType::STEEL_WILD_MOUSE:
            return true;

        default:
            return false;
    }
}

RideType OpenRCT2RideTypeToRCT2RideType(RideType openrct2Type)
{
    switch (openrct2Type)
    {
        case RideType::HYPERCOASTER:
            return RideType::CORKSCREW_ROLLER_COASTER;
        case RideType::CLASSIC_MINI_ROLLER_COASTER:
            return RideType::JUNIOR_ROLLER_COASTER;
        case RideType::MONSTER_TRUCKS:
            return RideType::CAR_RIDE;
        case RideType::HYPER_TWISTER:
            return RideType::TWISTER_ROLLER_COASTER;
        case RideType::SPINNING_WILD_MOUSE:
            return RideType::STEEL_WILD_MOUSE;

        default:
            return openrct2Type;
    }
}

size_t GetRCT2StringBufferLen(const char* buffer, size_t maxBufferLen)
{
    constexpr char MULTIBYTE = static_cast<char>(255);
    size_t len = 0;
    for (size_t i = 0; i < maxBufferLen; i++)
    {
        auto ch = buffer[i];
        if (ch == MULTIBYTE)
        {
            i += 2;

            // Check if reading two more bytes exceeds max buffer len
            if (i < maxBufferLen)
            {
                len += 3;
            }
        }
        else if (ch == '\0')
        {
            break;
        }
        else
        {
            len++;
        }
    }
    return len;
}

uint8_t rct2_ride::GetMinCarsPerTrain() const
{
    return min_max_cars_per_train >> 4;
}

uint8_t rct2_ride::GetMaxCarsPerTrain() const
{
    return min_max_cars_per_train & 0xF;
}

void rct2_ride::SetMinCarsPerTrain(uint8_t newValue)
{
    min_max_cars_per_train &= ~0xF0;
    min_max_cars_per_train |= (newValue << 4);
}

void rct2_ride::SetMaxCarsPerTrain(uint8_t newValue)
{
    min_max_cars_per_train &= ~0x0F;
    min_max_cars_per_train |= newValue & 0x0F;
}

bool RCT2TrackTypeIsBooster(RideType rideType, uint16_t trackType)
{
    // Boosters share their ID with the Spinning Control track.
    return rideType != RideType::SPINNING_WILD_MOUSE && rideType != RideType::STEEL_WILD_MOUSE
        && trackType == TrackElemType::Booster;
}

track_type_t RCT2TrackTypeToOpenRCT2(RCT12TrackType origTrackType, RideType rideType)
{
    if (GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE))
        return RCT12FlatTrackTypeToOpenRCT2(origTrackType);
    if (origTrackType == TrackElemType::RotationControlToggleAlias && !RCT2TrackTypeIsBooster(rideType, origTrackType))
        return TrackElemType::RotationControlToggle;

    return origTrackType;
}

RCT12TrackType OpenRCT2TrackTypeToRCT2(track_type_t origTrackType)
{
    if (origTrackType == TrackElemType::RotationControlToggle)
        return TrackElemType::RotationControlToggleAlias;

    // This function is safe to run this way round.
    return OpenRCT2FlatTrackTypeToRCT12(origTrackType);
}
