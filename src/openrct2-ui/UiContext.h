/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include <string>
#include <vector>

struct SDL_Window;

namespace OpenRCT2
{
    struct IContext;
    struct IPlatformEnvironment;
} // namespace OpenRCT2

namespace OpenRCT2::Ui
{
    struct FileDialogDesc;
    struct IUiContext;

    class InGameConsole;
    class InputManager;
    class ShortcutManager;

    struct IPlatformUiContext
    {
        virtual ~IPlatformUiContext() = default;
        virtual void SetWindowIcon(SDL_Window* window) = 0;
        virtual bool IsSteamOverlayAttached() = 0;

        virtual void ShowMessageBox(SDL_Window* window, const std::string& message) = 0;
        virtual bool HasMenuSupport() = 0;
        virtual int32_t ShowMenuDialog(
            const std::vector<std::string>& options, const std::string& title, const std::string& text)
            = 0;
        virtual void OpenFolder(const std::string& path) = 0;

        virtual void OpenURL(const std::string& url) = 0;
        virtual std::string ShowFileDialog(SDL_Window* window, const FileDialogDesc& desc) = 0;
        virtual std::string ShowDirectoryDialog(SDL_Window* window, const std::string& title) = 0;

        virtual bool HasFilePicker() const = 0;
    };

    [[nodiscard]] std::unique_ptr<IUiContext> CreateUiContext(const std::shared_ptr<IPlatformEnvironment>& env);
    [[nodiscard]] std::unique_ptr<IPlatformUiContext> CreatePlatformUiContext();

    [[nodiscard]] InGameConsole& GetInGameConsole();
    [[nodiscard]] InputManager& GetInputManager();
    [[nodiscard]] ShortcutManager& GetShortcutManager();
} // namespace OpenRCT2::Ui
