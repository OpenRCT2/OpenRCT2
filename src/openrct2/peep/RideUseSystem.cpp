/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideUseSystem.h"

namespace OpenRCT2::RideUse
{
    static RideHistory _history;
    static RideTypeHistory _typeHistory;

    RideHistory& GetHistory()
    {
        return _history;
    }

    RideTypeHistory& GetTypeHistory()
    {
        return _typeHistory;
    }
} // namespace OpenRCT2::RideUse
