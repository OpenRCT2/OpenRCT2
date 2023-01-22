/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if defined(DEBUG) && defined(_WIN32)
#    include <windows.h>
#endif

#include "Diagnostics.hpp"

namespace OpenRCT2::Debug
{
    void Break()
    {
#if defined(DEBUG)
#    if defined(_WIN32)
        if (IsDebuggerPresent())
        {
            DebugBreak();
        }
#    endif
#endif
    }
} // namespace OpenRCT2::Debug
