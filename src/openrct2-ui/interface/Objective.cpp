/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Objective.h"

#include <cstdint>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/scenario/Scenario.h>

void formatObjective(Formatter& ft, Objective objective)
{
    if (objective.Type == OBJECTIVE_BUILD_THE_BEST)
    {
        StringId rideTypeString = STR_NONE;
        auto rideTypeId = objective.RideId;
        if (rideTypeId != kRideTypeNull && rideTypeId < RIDE_TYPE_COUNT)
        {
            rideTypeString = GetRideTypeDescriptor(rideTypeId).Naming.Name;
        }
        ft.Add<StringId>(rideTypeString);
    }
    else if (objective.Type == OBJECTIVE_GUESTS_BY)
    {
        ft.Add<int32_t>(objective.NumGuests);
        ft.Add<int16_t>(DateGetTotalMonths(MONTH_OCTOBER, objective.Year));
    }
    else if (objective.Type == OBJECTIVE_GUESTS_AND_RATING)
    {
        ft.Add<int32_t>(objective.NumGuests);
    }
    else if (objective.Type == OBJECTIVE_10_ROLLERCOASTERS_LENGTH)
    {
        ft.Add<int16_t>(objective.MinimumLength);
    }
    else
    {
        ft.Add<int16_t>(objective.NumGuests);
        ft.Add<int16_t>(DateGetTotalMonths(MONTH_OCTOBER, objective.Year));
        if (objective.Type == OBJECTIVE_FINISH_5_ROLLERCOASTERS)
            ft.Add<uint16_t>(objective.MinimumExcitement);
        else
            ft.Add<money64>(objective.Currency);
    }
}
