#pragma once

#include "../../RideData.h"
#include "../../Track.h"

// clang-format off
constexpr const RideTypeDescriptor SpiralRollerCoasterRTD = 
{ 
    SET_FIELD(ExtraTrackPieces, (1ULL << TRACK_BOOSTER)) 
};
// clang-format on
