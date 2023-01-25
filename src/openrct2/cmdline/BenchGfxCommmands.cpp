/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Screenshot.h"
#include "CommandLine.hpp"

static exitcode_t HandleBenchGfx(CommandLineArgEnumerator* argEnumerator);

const CommandLineCommand CommandLine::BenchGfxCommands[]{
    // Main commands
    DefineCommand("", "<file> [iterations count]", nullptr, HandleBenchGfx), CommandTableEnd
};

static exitcode_t HandleBenchGfx(CommandLineArgEnumerator* argEnumerator)
{
    const char** argv = const_cast<const char**>(argEnumerator->GetArguments()) + argEnumerator->GetIndex();
    int32_t argc = argEnumerator->GetCount() - argEnumerator->GetIndex();
    int32_t result = CmdlineForGfxbench(argv, argc);
    if (result < 0)
    {
        return EXITCODE_FAIL;
    }
    return EXITCODE_OK;
}
