/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/EnumMap.hpp"
#include "Ride.h"

#ifdef ENABLE_SCRIPTING
static const EnumMap<Breakdown> kBreakdownMap(
    { { "safety_cut_out", Breakdown::safetyCutOut },
      { "restraints_stuck_closed", Breakdown::restraintsStuckClosed },
      { "restraints_stuck_open", Breakdown::restraintsStuckOpen },
      { "doors_stuck_closed", Breakdown::doorsStuckClosed },
      { "doors_stuck_open", Breakdown::doorsStuckOpen },
      { "vehicle_malfunction", Breakdown::vehicleMalfunction },
      { "brakes_failure", Breakdown::brakesFailure },
      { "control_failure", Breakdown::controlFailure } });
#endif
