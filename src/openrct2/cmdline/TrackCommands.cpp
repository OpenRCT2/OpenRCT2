/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../CmdlineTrack.h"
#include "CommandLine.hpp"

#ifdef _WIN32
#    include "../core/String.hpp"
#endif


static constexpr const CommandLineOptionDefinition TrackOptions[]{ OptionTableEnd };

static exitcode_t HandleTrack(CommandLineArgEnumerator* argEnumerator);

const CommandLineCommand CommandLine::TrackCommands[]{ DefineCommand("info", "<trackfile>", TrackOptions, HandleTrack),
    CommandTableEnd
};


static exitcode_t HandleTrack(CommandLineArgEnumerator* argEnumerator)
{
    const char** argv = const_cast<const char**>(argEnumerator->GetArguments()) + argEnumerator->GetIndex() - 1;
    int32_t argc = argEnumerator->GetCount() - argEnumerator->GetIndex() + 1;
    int32_t result = cmdline_for_track(argv, argc);
    if (result < 0)
    {
        return EXITCODE_FAIL;
    }
    return EXITCODE_OK;
}
