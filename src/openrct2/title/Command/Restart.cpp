/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Restart.h"

namespace OpenRCT2::Title
{
    int16_t RestartCommand::operator()(int16_t timer)
    {
        // The restart command is used as a tag, no logic required here.
        return 0;
    }
} // namespace OpenRCT2::Title
