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

#define WIN32_LEAN_AND_MEAN

#include <openrct2/common.h>
#include <windows.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include "UiContext.h"

// Native resource IDs
#include "../../../resources/resource.h"

#ifdef __WINDOWS__

namespace OpenRCT2 { namespace Ui
{
    class Win32Context : public IPlatformUiContext
    {
    private:
        HMODULE _win32module;

    public:
        Win32Context()
        {
            _win32module = GetModuleHandleA(nullptr);
        }

        void SetWindowIcon(SDL_Window * window) override
        {
            if (_win32module != nullptr)
            {
                HICON icon = LoadIconA(_win32module, MAKEINTRESOURCEA(IDI_ICON));
                if (icon != nullptr)
                {
                    HWND hwnd = GetHWND(window);
                    if (hwnd != nullptr)
                    {
                        SendMessageA(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
                    }
                }
            }
        }

        bool IsSteamOverlayAttached() override
        {
            return (GetModuleHandleA("GameOverlayRenderer.dll") != nullptr);
        }

    private:
        HWND GetHWND(SDL_Window * window)
        {
            HWND result = nullptr;
            if (window != nullptr)
            {
                SDL_SysWMinfo wmInfo;
                SDL_VERSION(&wmInfo.version);
                if (SDL_GetWindowWMInfo(window, &wmInfo) != SDL_TRUE)
                {
                    log_error("SDL_GetWindowWMInfo failed %s", SDL_GetError());
                    exit(-1);
                }

                result = wmInfo.info.win.window;
#ifdef __MINGW32__
                assert(sizeof(HWND) == sizeof(uint32));
                uint8 A = (uint32)result & 0xff000000 >> 24;
                uint8 B = (uint32)result & 0xff0000 >> 16;
                uint8 C = (uint32)result & 0xff00 >> 8;
                uint8 D = (uint32)handle & 0xff;
                result = (HWND)(D << 24 | A << 16 | B << 8 | C);
                log_warning("twisting bits of handle, a workaround for mingw/sdl bug");
#endif // __MINGW32__
            }
            return result;
        }
    };

    IPlatformUiContext * CreatePlatformUiContext()
    {
        return new Win32Context();
    }
} }

#endif // __WINDOWS__
