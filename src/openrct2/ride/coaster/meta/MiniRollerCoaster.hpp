#pragma once

#include "../../RideData.h"
#include "../../Track.h"

// clang-format off
constexpr const RideTypeDescriptor MiniRollerCoasterRTD = 
{
    SET_FIELD(ExtraTrackPieces, (1ULL << TRACK_BOOSTER)),
};
// clang-format on
