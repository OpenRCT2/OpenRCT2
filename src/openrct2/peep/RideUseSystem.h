/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once
#include "../core/GroupVector.hpp"
#include "../ride/RideTypes.h"

namespace OpenRCT2::RideUse
{
    using RideHistory = GroupVector<uint16_t, ride_id_t>;
    using RideTypeHistory = GroupVector<uint16_t, RideType>;

    RideHistory& GetHistory();
    RideTypeHistory& GetTypeHistory();

} // namespace OpenRCT2::RideUse
