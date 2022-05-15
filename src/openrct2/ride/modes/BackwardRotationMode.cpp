/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "BackwardRotationMode.h"
#include "../Vehicle.h"
#include "../../entity/Peep.h"

rct_string_id OpenRCT2::RideModes::BackwardRotation::GetOperationErrorMessage(Ride* ride) const
{
    return STR_CANT_CHANGE_NUMBER_OF_ROTATIONS;
}

void OpenRCT2::RideModes::BackwardRotation::PeepChooseSeatFromCar(Peep* peep, Ride* ride, Vehicle* vehicle) const
{
    if (vehicle == nullptr)
    {
        return;
    }
    uint8_t chosen_seat = vehicle->next_free_seat;

    chosen_seat = (((~vehicle->Pitch + 1) >> 3) & 0xF) * 2;
    if (vehicle->next_free_seat & 1)
    {
        chosen_seat++;
    }
    peep->CurrentSeat = chosen_seat;
    vehicle->next_free_seat++;

    vehicle->peep[peep->CurrentSeat] = peep->sprite_index;
    vehicle->peep_tshirt_colours[peep->CurrentSeat] = peep->TshirtColour;
}
