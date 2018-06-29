/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include <string>
#include <openrct2/common.h>

struct SDL_Window;

namespace OpenRCT2
{
    interface IContext;
    interface IPlatformEnvironment;

    namespace Ui
    {
        struct      FileDialogDesc;
        class       InGameConsole;
        interface   IUiContext;

        interface IPlatformUiContext
        {
            virtual ~IPlatformUiContext() = default;
            virtual void SetWindowIcon(SDL_Window * window) abstract;
            virtual bool IsSteamOverlayAttached() abstract;

            virtual void        ShowMessageBox(SDL_Window * window, const std::string &message) abstract;
            virtual std::string ShowFileDialog(SDL_Window * window, const FileDialogDesc &desc) abstract;
            virtual std::string ShowDirectoryDialog(SDL_Window * window, const std::string &title) abstract;
        };

        std::unique_ptr<IUiContext> CreateUiContext(const std::shared_ptr<IPlatformEnvironment>& env);
        IPlatformUiContext * CreatePlatformUiContext();

        InGameConsole& GetInGameConsole();
    } // namespace Ui
} // namespace OpenRCT2
