/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <limits>

typedef uint16_t ride_id_t;
struct Ride;

constexpr const ride_id_t RIDE_ID_NULL = 0xFF; // std::numeric_limits<ride_id_t>::max();
constexpr const ride_id_t RideIdNewNull = std::numeric_limits<ride_id_t>::max();
