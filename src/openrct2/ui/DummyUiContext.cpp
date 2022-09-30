/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../config/Config.h"
#include "../drawing/X8DrawingEngine.h"
#include "UiContext.h"
#include "WindowManager.h"

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui
{
    /**
     * Represents the window or screen that OpenRCT2 is presented on.
     */
    class DummyUiContext final : public IUiContext
    {
    private:
        std::unique_ptr<IWindowManager> const _windowManager = CreateDummyWindowManager();

    public:
        void Initialise() override
        {
        }
        void Tick() override
        {
        }
        void Draw(rct_drawpixelinfo* /*dpi*/) override
        {
        }

        void CreateWindow() override
        {
        }
        void CloseWindow() override
        {
        }
        void RecreateWindow() override
        {
        }
        void* GetWindow() override
        {
            return nullptr;
        }
        int32_t GetWidth() override
        {
            return 0;
        }
        int32_t GetHeight() override
        {
            return 0;
        }
        ScaleQuality GetScaleQuality() override
        {
            return ScaleQuality::NearestNeighbour;
        }
        void SetFullscreenMode(FULLSCREEN_MODE /*mode*/) override
        {
        }
        const std::vector<Resolution>& GetFullscreenResolutions() override
        {
            static std::vector<Resolution> res;
            return res;
        }
        bool HasFocus() override
        {
            return false;
        }
        bool IsMinimised() override
        {
            return false;
        }
        bool IsSteamOverlayActive() override
        {
            return false;
        }
        void ProcessMessages() override
        {
        }
        void TriggerResize() override
        {
        }

        void ShowMessageBox(const std::string& /*message*/) override
        {
        }
        int32_t ShowMessageBox(const std::string&, const std::string&, const std::vector<std::string>&) override
        {
            return -1;
        }
        bool HasMenuSupport() override
        {
            return false;
        }

        int32_t ShowMenuDialog(
            const std::vector<std::string>& options, const std::string& title, const std::string& text) override
        {
            return static_cast<int32_t>(options.size());
        }
        void OpenFolder(const std::string& /*path*/) override
        {
        }
        void OpenURL(const std::string& /*url*/) override
        {
        }
        std::string ShowFileDialog(const FileDialogDesc& /*desc*/) override
        {
            return std::string();
        }
        std::string ShowDirectoryDialog(const std::string& /*title*/) override
        {
            return std::string();
        }

        // Input
        const CursorState* GetCursorState() override
        {
            return nullptr;
        }
        CursorID GetCursor() override
        {
            return CursorID::Arrow;
        }
        void SetCursor(CursorID /*cursor*/) override
        {
        }
        void SetCursorScale(uint8_t /*scale*/) override
        {
        }
        void SetCursorVisible(bool /*value*/) override
        {
        }
        ScreenCoordsXY GetCursorPosition() override
        {
            return {};
        }
        void SetCursorPosition(const ScreenCoordsXY& /*cursorPosition*/) override
        {
        }
        void SetCursorTrap(bool /*value*/) override
        {
        }
        const uint8_t* GetKeysState() override
        {
            return nullptr;
        }
        const uint8_t* GetKeysPressed() override
        {
            return nullptr;
        }
        void SetKeysPressed(uint32_t /*keysym*/, uint8_t /*scancode*/) override
        {
        }

        class X8DrawingEngineFactory final : public IDrawingEngineFactory
        {
            std::unique_ptr<IDrawingEngine> Create(
                [[maybe_unused]] DrawingEngine type, const std::shared_ptr<IUiContext>& uiContext) override
            {
                return std::make_unique<X8DrawingEngine>(uiContext);
            }
        };

        // Drawing
        std::shared_ptr<IDrawingEngineFactory> GetDrawingEngineFactory() override
        {
            return std::make_shared<X8DrawingEngineFactory>();
        }
        void DrawWeatherAnimation(IWeatherDrawer* weatherDrawer, rct_drawpixelinfo* dpi, DrawWeatherFunc drawFunc) override
        {
        }

        // Text input
        bool IsTextInputActive() override
        {
            return false;
        }
        TextInputSession* StartTextInput([[maybe_unused]] utf8* buffer, [[maybe_unused]] size_t bufferSize) override
        {
            return nullptr;
        }

        void StopTextInput() override
        {
        }

        // In-game UI
        IWindowManager* GetWindowManager() override
        {
            return _windowManager.get();
        }

        // Clipboard
        bool SetClipboardText([[maybe_unused]] const utf8* target) override
        {
            return false;
        }

        ITitleSequencePlayer* GetTitleSequencePlayer() override
        {
            return nullptr;
        }

        bool HasFilePicker() const override
        {
            return false;
        }
    };

    std::shared_ptr<IUiContext> CreateDummyUiContext()
    {
        return std::make_unique<DummyUiContext>();
    }
} // namespace OpenRCT2::Ui
