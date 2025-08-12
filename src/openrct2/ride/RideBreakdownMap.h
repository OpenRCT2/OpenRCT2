#pragma once
/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../core/EnumMap.hpp"
#include "Ride.h"

#include <cassert>
#include <string>

#ifdef ENABLE_SCRIPTING
static const EnumMap<uint8_t> BreakdownMap // The types of breakdowns.
    ({ { "safety_cut_out", BREAKDOWN_SAFETY_CUT_OUT },
       { "restraints_stuck_closed", BREAKDOWN_RESTRAINTS_STUCK_CLOSED },
       { "restraints_stuck_open", BREAKDOWN_RESTRAINTS_STUCK_OPEN },
       { "doors_stuck_closed", BREAKDOWN_DOORS_STUCK_CLOSED },
       { "doors_stuck_open", BREAKDOWN_DOORS_STUCK_OPEN },
       { "vehicle_malfunction", BREAKDOWN_VEHICLE_MALFUNCTION },
       { "brakes_failure", BREAKDOWN_BRAKES_FAILURE },
       { "control_failure", BREAKDOWN_CONTROL_FAILURE } });
#endif
