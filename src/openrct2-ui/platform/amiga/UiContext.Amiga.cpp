/*****************************************************************************
 * IPlatformUiContext for AmigaOS: no external dialogs, no file picker (the
 * game falls back to its own in-game file browser), message boxes via
 * EasyRequest.
 *****************************************************************************/
#ifdef __amigaos__

    #include "../../UiContext.h"
    #include "amiga_ui.h"

    #include <memory>
    #include <openrct2/ui/UiContext.h>
    #include <string>
    #include <vector>

namespace OpenRCT2::Ui
{
    class AmigaContext final : public IPlatformUiContext
    {
    public:
        void SetWindowIcon(SDL_Window*) override
        {
        }

        bool IsSteamOverlayAttached() override
        {
            return false;
        }

        void ShowMessageBox(SDL_Window*, const std::string& message) override
        {
            amiga_ui_request("OpenRCT2", message.c_str(), "OK");
        }

        bool HasMenuSupport() override
        {
            return false;
        }

        int32_t ShowMenuDialog(const std::vector<std::string>&, const std::string&, const std::string&) override
        {
            return -1;
        }

        void OpenFolder(const std::string&) override
        {
        }

        void OpenURL(const std::string&) override
        {
        }

        std::string ShowFileDialog(SDL_Window*, const FileDialogDesc&) override
        {
            return {};
        }

        std::string ShowDirectoryDialog(SDL_Window*, const std::string&) override
        {
            return {};
        }

        bool HasFilePicker() const override
        {
            return false;
        }
    };

    std::unique_ptr<IPlatformUiContext> CreatePlatformUiContext()
    {
        return std::make_unique<AmigaContext>();
    }
} // namespace OpenRCT2::Ui

#endif // __amigaos__
