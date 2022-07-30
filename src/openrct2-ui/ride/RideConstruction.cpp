/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideConstruction.h"

#include <openrct2/actions/RideCreateAction.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideTypes.h>

/**
 *
 *  rct2: 0x006B4800
 */
void ride_construct_new(RideSelection listItem)
{
    int32_t rideEntryIndex = ride_get_entry_index(listItem.Type, listItem.EntryIndex);
    int32_t colour1 = ride_get_random_colour_preset_index(listItem.Type);
    int32_t colour2 = ride_get_unused_preset_vehicle_colour(rideEntryIndex);

    auto gameAction = RideCreateAction(listItem.Type, listItem.EntryIndex, colour1, colour2, gLastEntranceStyle);

    gameAction.SetCallback([](const GameAction* ga, const GameActions::Result* result) {
        if (result->Error != GameActions::Status::Ok)
            return;
        const auto rideIndex = result->GetData<RideId>();
        auto ride = get_ride(rideIndex);
        ride_construct(ride);
    });

    GameActions::Execute(&gameAction);
}
