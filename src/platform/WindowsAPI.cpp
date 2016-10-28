#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include <SDL_platform.h>

#ifdef __WINDOWS__

#include <windows.foundation.h>
#include <windows.gaming.ui.h>
#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>

#pragma comment(lib, "runtimeobject.lib")

using namespace ABI::Windows::Gaming::UI;
using namespace ABI::Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Details;
using namespace Microsoft::WRL::Wrappers;

static bool _wrlInitialised;
static ComPtr<IGameBarStatics> _gameBar;

extern "C"
{
    void wrl_initialise()
    {
        if (SUCCEEDED(Initialize(RO_INIT_MULTITHREADED)))
        {
            _wrlInitialised = true;

            // Get a GameBar instance
            GetActivationFactory(HStringReference(RuntimeClass_Windows_Gaming_UI_GameBar).Get(), &_gameBar);
        }
    }

    void wrl_dispose()
    {
        if (_wrlInitialised)
        {
            _gameBar = nullptr;
            Uninitialize();
        }
    }

    bool wrl_is_gamebar_visible()
    {
        if (_gameBar != nullptr)
        {
            boolean value;
            if (SUCCEEDED(_gameBar->get_Visible(&value)))
            {
                return value != 0;
            }
        }
        return false;
    }
}

#endif
