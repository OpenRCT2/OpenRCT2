/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#include "ContinuousCircuitMode.h"
#include "../Ride.h"

bool OpenRCT2::RideModes::ContinuousCircuit::CanHaveMultipleCircuits(const Ride* ride) const
{
    if (!(ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_ALLOW_MULTIPLE_CIRCUITS)))
        return false;

    // Must have no more than one vehicle and one station
    if (ride->num_vehicles > 1 || ride->num_stations > 1)
        return false;

    return true;
}
