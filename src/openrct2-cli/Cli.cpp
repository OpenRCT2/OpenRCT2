/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/Context.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/command_line/CommandLine.hpp>

using namespace OpenRCT2;
using namespace OpenRCT2::CommandLine;

/**
 * Main entry point for non-Windows systems. Windows instead uses its own DLL proxy.
 */
int main(int argc, const char** argv)
{
    int32_t rc = EXIT_SUCCESS;
    auto runGame = CommandLineRun(argv, argc);
    if (runGame == Exitcode::launch)
    {
        gOpenRCT2Headless = true;
        gOpenRCT2NoGraphics = true;

        // Run OpenRCT2 with a plain context
        auto context = CreateContext();
        rc = context->RunOpenRCT2(argc, argv);
    }
    else if (runGame == Exitcode::fail)
    {
        rc = EXIT_FAILURE;
    }
    return rc;
}
