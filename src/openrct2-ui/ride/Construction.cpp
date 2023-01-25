/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Construction.h"

#include <openrct2/actions/RideCreateAction.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ride/RideTypes.h>

/**
 *
 *  rct2: 0x006B4800
 */
void RideConstructNew(RideSelection listItem)
{
    int32_t rideEntryIndex = RideGetEntryIndex(listItem.Type, listItem.EntryIndex);
    int32_t colour1 = RideGetRandomColourPresetIndex(listItem.Type);
    int32_t colour2 = RideGetUnusedPresetVehicleColour(rideEntryIndex);

    auto gameAction = RideCreateAction(listItem.Type, listItem.EntryIndex, colour1, colour2, gLastEntranceStyle);

    gameAction.SetCallback([](const GameAction* ga, const GameActions::Result* result) {
        if (result->Error != GameActions::Status::Ok)
            return;
        const auto rideIndex = result->GetData<RideId>();
        auto ride = GetRide(rideIndex);
        RideConstructionStart(*ride);
    });

    GameActions::Execute(&gameAction);
}
