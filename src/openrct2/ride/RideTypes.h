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

typedef uint8_t ride_id_t;
typedef uint16_t ride_idnew_t; // Temporary, old one can be removed after switching save format.
struct Ride;

constexpr const ride_idnew_t RideIdNewNull = 0xFFFF;
