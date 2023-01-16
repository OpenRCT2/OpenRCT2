/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/Context.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/cmdline/CommandLine.hpp>
#include <openrct2/platform/Platform.h>

using namespace OpenRCT2;

/**
 * Main entry point for non-Windows systems. Windows instead uses its own DLL proxy.
 */
int main(int argc, const char** argv)
{
    int32_t rc = EXIT_SUCCESS;
    int runGame = CmdlineRun(argv, argc);
    Platform::CoreInit();
    if (runGame == EXITCODE_CONTINUE)
    {
        gOpenRCT2Headless = true;
        gOpenRCT2NoGraphics = true;

        // Run OpenRCT2 with a plain context
        auto context = CreateContext();
        rc = context->RunOpenRCT2(argc, argv);
    }
    else if (runGame == EXITCODE_FAIL)
    {
        rc = EXIT_FAILURE;
    }
    return rc;
}
