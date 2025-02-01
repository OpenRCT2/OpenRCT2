/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../localisation/StringIdType.h"

struct ResultWithMessage
{
    bool Successful{};
    StringId Message = kStringIdNone;

    bool HasMessage() const
    {
        return Message != kStringIdNone;
    }
};
