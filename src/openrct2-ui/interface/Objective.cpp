/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Objective.h"

#include <cstdint>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.Date.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/scenario/Scenario.h>

namespace OpenRCT2::Ui
{
    const StringId kObjectiveNames[] = {
        STR_OBJECTIVE_NONE,
        STR_OBJECTIVE_GUESTS_BY,
        STR_OBJECTIVE_PARK_VALUE_BY,
        STR_OBJECTIVE_HAVE_FUN,
        STR_OBJECTIVE_BUILD_THE_BEST,
        STR_OBJECTIVE_10_ROLLERCOASTERS,
        STR_OBJECTIVE_GUESTS_AND_RATING,
        STR_OBJECTIVE_MONTHLY_RIDE_INCOME,
        STR_OBJECTIVE_10_ROLLERCOASTERS_LENGTH,
        STR_OBJECTIVE_FINISH_5_ROLLERCOASTERS,
        STR_OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE,
        STR_OBJECTIVE_MONTHLY_FOOD_INCOME,
    };

    void formatObjective(Formatter& ft, Objective objective)
    {
        if (objective.Type == OBJECTIVE_BUILD_THE_BEST)
        {
            StringId rideTypeString = kStringIdNone;
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
                ft.Add<ride_rating>(objective.MinimumExcitement);
            else
                ft.Add<money64>(objective.Currency);
        }
    }
} // namespace OpenRCT2::Ui
