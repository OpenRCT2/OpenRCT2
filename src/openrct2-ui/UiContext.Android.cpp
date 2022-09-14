/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef __ANDROID__

#    include "UiContext.h"

#    include <SDL.h>
#    include <dlfcn.h>
#    include <jni.h>
#    include <openrct2/common.h>
#    include <openrct2/core/String.hpp>
#    include <openrct2/platform/Platform.h>
#    include <openrct2/ui/UiContext.h>
#    include <sstream>
#    include <stdexcept>

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

        int32_t ShowMenuDialog(const std::vector<std::string>& options, std::string_view title, std::string_view text) override
        {
            return -1;
        }

        void ShowMessageBox(SDL_Window* window, std::string_view message) override
        {
            log_verbose(std::string{ message }.c_str());

            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "OpenRCT2", std::string{ message }.c_str(), window);
        }

        std::string ShowFileDialog(SDL_Window* window, const FileDialogDesc& desc) override
        {
            STUB();

            return nullptr;
        }

        std::string ShowDirectoryDialog(SDL_Window* window, std::string_view title) override
        {
            log_info(std::string{ title }.c_str());
            STUB();

            return "/sdcard/rct2";
        }

        void OpenFolder(u8string_view path) override
        {
        }

        void OpenURL(std::string_view url) override
        {
            STUB();
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
