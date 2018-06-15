/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef __ANDROID__

#include <dlfcn.h>
#include <sstream>
#include <stdexcept>
#include <openrct2/common.h>
#include <openrct2/core/String.hpp>
#include <openrct2/ui/UiContext.h>
#include "UiContext.h"

#include <SDL.h>

namespace OpenRCT2::Ui
{

    class AndroidContext final : public IPlatformUiContext
    {
    private:

    public:
        AndroidContext()
        {
        }

        void SetWindowIcon(SDL_Window * window) override
        {
        }

        bool IsSteamOverlayAttached() override
        {
            return false;
        }

        void ShowMessageBox(SDL_Window * window, const std::string &message) override
        {
            log_verbose(message.c_str());

            STUB();
        }

        std::string ShowFileDialog(SDL_Window * window, const FileDialogDesc &desc) override
        {
            STUB();

            return nullptr;
        }

        std::string ShowDirectoryDialog(SDL_Window * window, const std::string &title) override
        {
            log_info(title.c_str());
            STUB();

            return "/sdcard/rct2";
        }
    };

    IPlatformUiContext * CreatePlatformUiContext()
    {
        return new AndroidContext();
    }
}

#endif // __ANDROID__
