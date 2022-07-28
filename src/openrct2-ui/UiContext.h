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
#include <vector>

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
            virtual bool HasMenuSupport() abstract;
            virtual int32_t ShowMenuDialog(
                const std::vector<std::string>& options, const std::string& title, const std::string& text) abstract;
            virtual void OpenFolder(const std::string& path) abstract;

            virtual void OpenURL(const std::string& url) abstract;
            virtual std::string ShowFileDialog(SDL_Window* window, const FileDialogDesc& desc) abstract;
            virtual std::string ShowDirectoryDialog(SDL_Window* window, const std::string& title) abstract;

            virtual bool HasFilePicker() const abstract;
        };

        [[nodiscard]] std::unique_ptr<IUiContext> CreateUiContext(const std::shared_ptr<IPlatformEnvironment>& env);
        [[nodiscard]] std::unique_ptr<IPlatformUiContext> CreatePlatformUiContext();

        [[nodiscard]] InGameConsole& GetInGameConsole();
    } // namespace Ui
} // namespace OpenRCT2
