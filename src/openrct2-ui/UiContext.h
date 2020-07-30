/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include <openrct2/common.h>
#include <string>

struct SDL_Window;

namespace OpenRCT2
{
    struct IContext;
    struct IPlatformEnvironment;

    namespace Ui
    {
        struct FileDialogDesc;
        class InGameConsole;
        struct IUiContext;

        struct IPlatformUiContext
        {
            virtual ~IPlatformUiContext() = default;
            virtual void SetWindowIcon(SDL_Window* window) abstract;
            virtual bool IsSteamOverlayAttached() abstract;

            virtual void ShowMessageBox(SDL_Window* window, const std::string& message) abstract;
            virtual void OpenFolder(const std::string& path) abstract;

            virtual void OpenURL(const std::string& url) abstract;
            virtual std::string ShowFileDialog(SDL_Window* window, const FileDialogDesc& desc) abstract;
            virtual std::string ShowDirectoryDialog(SDL_Window* window, const std::string& title) abstract;
        };

        std::unique_ptr<IUiContext> CreateUiContext(const std::shared_ptr<IPlatformEnvironment>& env);
        IPlatformUiContext* CreatePlatformUiContext();

        InGameConsole& GetInGameConsole();
    } // namespace Ui
} // namespace OpenRCT2
