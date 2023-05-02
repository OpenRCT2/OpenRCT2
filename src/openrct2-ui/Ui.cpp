/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Ui.h"

#include "SDLException.h"
#include "UiContext.h"
#include "audio/AudioContext.h"
#include "drawing/BitmapReader.h"

#include <memory>
#include <openrct2/Context.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/audio/AudioContext.h>
#include <openrct2/command_line/CommandLine.hpp>
#include <openrct2/platform/Platform.h>
#include <openrct2/ui/UiContext.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;
using namespace OpenRCT2::Ui;

template<typename T> static std::shared_ptr<T> ToShared(std::unique_ptr<T>&& src)
{
    return std::shared_ptr<T>(std::move(src));
}

/**
 * Main entry point for non-Windows systems. Windows instead uses its own DLL proxy.
 */
#if defined(_MSC_VER) && !defined(__DISABLE_DLL_PROXY__)
int NormalisedMain(int argc, const char** argv)
#else
int main(int argc, const char** argv)
#endif
{
    std::unique_ptr<IContext> context;
    int32_t rc = EXIT_SUCCESS;
    int runGame = CommandLineRun(argv, argc);
    Platform::CoreInit();
    RegisterBitmapReader();
    if (runGame == EXITCODE_CONTINUE)
    {
        if (gOpenRCT2Headless)
        {
            // Run OpenRCT2 with a plain context
            context = CreateContext();
        }
        else
        {
            // Run OpenRCT2 with a UI context
            auto env = ToShared(CreatePlatformEnvironment());
            std::shared_ptr<IAudioContext> audioContext;
            try
            {
                audioContext = ToShared(CreateAudioContext());
            }
            catch (const SDLException& e)
            {
                LOG_WARNING("Failed to create audio context. Using dummy audio context. Error message was: %s", e.what());
                audioContext = ToShared(CreateDummyAudioContext());
            }
            auto uiContext = ToShared(CreateUiContext(env));
            context = CreateContext(env, audioContext, uiContext);
        }
        rc = context->RunOpenRCT2(argc, argv);
    }
    else if (runGame == EXITCODE_FAIL)
    {
        rc = EXIT_FAILURE;
    }
    return rc;
}

#ifdef __ANDROID__
extern "C" {
int SDL_main(int argc, const char* argv[])
{
    return main(argc, argv);
}
}
#endif
