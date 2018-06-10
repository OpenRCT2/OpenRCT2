#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#include <openrct2/audio/AudioContext.h>
#include <openrct2/Context.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/ui/UiContext.h>
#include "audio/AudioContext.h"
#include "drawing/BitmapReader.h"
#include "Ui.h"
#include "UiContext.h"

#include <openrct2/platform/platform.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;
using namespace OpenRCT2::Ui;

template<typename T>
static std::shared_ptr<T> to_shared(std::unique_ptr<T>&& src)
{
    return std::shared_ptr<T>(std::move(src));
}

/**
 * Main entry point for non-Windows systems. Windows instead uses its own DLL proxy.
 */
#if defined(_MSC_VER) && !defined(__DISABLE_DLL_PROXY__)
int NormalisedMain(int argc, const char * * argv)
#else
int main(int argc, const char * * argv)
#endif
{
    int runGame = cmdline_run(argv, argc);
    core_init();
    RegisterBitmapReader();
    if (runGame == 1)
    {
        if (gOpenRCT2Headless)
        {
            // Run OpenRCT2 with a plain context
            auto context = CreateContext();
            context->RunOpenRCT2(argc, argv);
        }
        else
        {
            // Run OpenRCT2 with a UI context
            auto env = to_shared(CreatePlatformEnvironment());
            auto audioContext = to_shared(CreateAudioContext());
            auto uiContext = to_shared(CreateUiContext(env));
            auto context = CreateContext(env, audioContext, uiContext);

            context->RunOpenRCT2(argc, argv);
        }
    }
    return gExitCode;
}


#ifdef __ANDROID__
extern "C" {
int SDL_main(int argc, const char *argv[])
{
    return main(argc, argv);
}
}
#endif
