/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "common.h"
#include "core/Identifier.hpp"

#include <limits>

using BannerIndex = TIdentifier<uint16_t, std::numeric_limits<uint16_t>::max(), struct BannerIndexTag>;
using ParkEntranceIndex = TIdentifier<uint8_t, std::numeric_limits<uint8_t>::max(), struct ParkEntranceIndexTag>;
using RideId = TIdentifier<uint16_t, std::numeric_limits<uint16_t>::max(), struct RideIdTag>;
using EntityId = TIdentifier<uint16_t, std::numeric_limits<uint16_t>::max(), struct EntityIdTag>;
using StationIndex = TIdentifier<uint8_t, std::numeric_limits<uint8_t>::max(), struct StationIndexTag>;
