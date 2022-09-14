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
#include <openrct2/core/String.hpp>
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

            virtual void ShowMessageBox(SDL_Window* window, std::string_view message) abstract;
            virtual bool HasMenuSupport() abstract;
            virtual int32_t ShowMenuDialog(
                const std::vector<std::string>& options, std::string_view title, std::string_view text) abstract;
            virtual void OpenFolder(u8string_view path) abstract;

            virtual void OpenURL(std::string_view url) abstract;
            virtual std::string ShowFileDialog(SDL_Window* window, const FileDialogDesc& desc) abstract;
            virtual std::string ShowDirectoryDialog(SDL_Window* window, std::string_view title) abstract;

            virtual bool HasFilePicker() const abstract;
        };

        [[nodiscard]] std::unique_ptr<IUiContext> CreateUiContext(const std::shared_ptr<IPlatformEnvironment>& env);
        [[nodiscard]] std::unique_ptr<IPlatformUiContext> CreatePlatformUiContext();

        [[nodiscard]] InGameConsole& GetInGameConsole();
    } // namespace Ui
} // namespace OpenRCT2
