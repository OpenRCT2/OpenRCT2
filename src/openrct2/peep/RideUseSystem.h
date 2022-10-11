/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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
    using RideHistory = GroupVector<EntityId, RideId>;
    using RideTypeHistory = GroupVector<EntityId, uint16_t>;

    RideHistory& GetHistory();
    RideTypeHistory& GetTypeHistory();

} // namespace OpenRCT2::RideUse
