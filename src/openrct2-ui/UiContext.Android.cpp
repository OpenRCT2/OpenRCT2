/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef __ANDROID__

    #include "UiContext.h"

    #include <SDL.h>
    #include <dlfcn.h>
    #include <jni.h>
    #include <openrct2/Diagnostic.h>
    #include <openrct2/core/String.hpp>
    #include <openrct2/platform/Platform.h>
    #include <openrct2/ui/UiContext.h>
    #include <sstream>
    #include <stdexcept>

namespace OpenRCT2::Ui
{
    class AndroidContext final : public IPlatformUiContext
    {
    private:
    public:
        AndroidContext()
        {
        }

        void SetWindowIcon(SDL_Window* window) override
        {
        }

        bool IsSteamOverlayAttached() override
        {
            return false;
        }

        bool HasMenuSupport() override
        {
            return false;
        }

        int32_t ShowMenuDialog(
            const std::vector<std::string>& options, const std::string& title, const std::string& text) override
        {
            return -1;
        }

        void ShowMessageBox(SDL_Window* window, const std::string& message) override
        {
            LOG_VERBOSE(message.c_str());

            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "OpenRCT2", message.c_str(), window);
        }

        std::string ShowFileDialog(SDL_Window* window, const FileDialogDesc& desc) override
        {
            LOG_WARNING("Function %s at %s:%d is a stub.", __PRETTY_FUNCTION__, __FILE__, __LINE__);

            return nullptr;
        }

        std::string ShowDirectoryDialog(SDL_Window* window, const std::string& title) override
        {
            LOG_INFO(title.c_str());
            LOG_WARNING("Function %s at %s:%d is a stub.", __PRETTY_FUNCTION__, __FILE__, __LINE__);

            return "/sdcard/rct2";
        }

        void OpenFolder(const std::string& path) override
        {
        }

        void OpenURL(const std::string& url) override
        {
            LOG_WARNING("Function %s at %s:%d is a stub.", __PRETTY_FUNCTION__, __FILE__, __LINE__);
        }

        bool HasFilePicker() const override
        {
            return false;
        }
    };

    std::unique_ptr<IPlatformUiContext> CreatePlatformUiContext()
    {
        return std::make_unique<AndroidContext>();
    }
} // namespace OpenRCT2::Ui

#endif // __ANDROID__
