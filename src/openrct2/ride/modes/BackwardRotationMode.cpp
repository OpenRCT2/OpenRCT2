#include "BackwardRotationMode.h"
/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

rct_string_id OpenRCT2::RideModes::BackwardRotation::GetOperationErrorMessage(Ride* ride) const
{
    return STR_CANT_CHANGE_NUMBER_OF_ROTATIONS;
}
