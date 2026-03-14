/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/StringTypes.h"
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

struct ResultWithMessageString
{
    bool successful{};
    u8string message{};

    bool hasMessage() const
    {
        return !message.empty();
    }
};
