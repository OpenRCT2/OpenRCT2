/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Formatter.h"
#include "StringIdType.h"

namespace OpenRCT2
{
    struct StringWithArgs
    {
        StringId str;
        Formatter args;
    };
} // namespace OpenRCT2
