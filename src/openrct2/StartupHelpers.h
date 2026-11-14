/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "core/StringTypes.h"
#include "localisation/StringIdType.h"

namespace OpenRCT2
{
    struct IPlatformEnvironment;
    struct IContext;
    enum class StartupOpenStatus
    {
        // Opening the file has failed
        fileOpenFailure,
        // The opened file is a park and the game should switch the active scene to it
        loadedParkFile,
        // The opened file is content, which has been installed, and the game should show the title screen
        installedContent,
    };
    struct StartupOpenResult
    {
        StartupOpenStatus status;
        StringId message = kStringIdNone;
    };

    StartupOpenResult loadStartupFile(IContext& context, IPlatformEnvironment& env, const u8string& originalPath);
} // namespace OpenRCT2
