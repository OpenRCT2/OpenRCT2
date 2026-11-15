/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackCommands.h"

#include "../../OpenRCT2.h"
#include "../../core/String.hpp"
#include "../CommandLine.hpp"

#include <cstring>
#include <sstream>

namespace OpenRCT2::CommandLine::Track
{
    static ExitCode handleTrack(CommandLineArgEnumerator* argEnumerator);

    // clang-format off
    const CommandLineCommand kTrackCommands[]
    {
        DefineCommand("screenshot", "<path to track design>", kNoOptions, handleTrack),
        
        kCommandTableEnd,
    };
    // clang-format on

    static ExitCode handleTrack(CommandLineArgEnumerator* argEnumerator)
    {
        const char** argv = const_cast<const char**>(argEnumerator->GetArguments()) + argEnumerator->GetIndex() - 1;
        int32_t argc = argEnumerator->GetCount() - argEnumerator->GetIndex() + 1;

        if (argc == 0)
            return ExitCode::fail;

        if (String::iequals(argv[0], "screenshot"))
        {
            return screenshot(argEnumerator);
        }

        fprintf(stderr, "Unknown track command.\n");
        return ExitCode::fail;
    }
} // namespace OpenRCT2::CommandLine::Track
