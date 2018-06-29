/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

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
        IWindowManager * const _windowManager = CreateDummyWindowManager();

    public:
        void Update() override { }
        void Draw(rct_drawpixelinfo * /*dpi*/) override { }

        void CreateWindow() override { }
        void CloseWindow() override { }
        void RecreateWindow() override { }
        void * GetWindow() override { return nullptr; }
        int32_t GetWidth() override { return 0; }
        int32_t GetHeight() override { return 0; }
        int32_t GetScaleQuality() override { return 0; }
        void SetFullscreenMode(FULLSCREEN_MODE /*mode*/) override { }
        std::vector<Resolution> GetFullscreenResolutions() override { return std::vector<Resolution>(); }
        bool HasFocus() override { return false; }
        bool IsMinimised() override { return false; }
        bool IsSteamOverlayActive() override { return false; }
        void ProcessMessages() override { }
        void TriggerResize() override { }

        void ShowMessageBox(const std::string &/*message*/) override { }
        std::string ShowFileDialog(const FileDialogDesc &/*desc*/) override { return std::string(); }
        std::string ShowDirectoryDialog(const std::string &/*title*/) override { return std::string(); }

        // Input
        const CursorState * GetCursorState() override { return nullptr; }
        CURSOR_ID GetCursor() override { return CURSOR_ARROW; }
        void SetCursor(CURSOR_ID /*cursor*/) override { }
        void SetCursorScale(uint8_t /*scale*/) override  { }
        void SetCursorVisible(bool /*value*/) override { }
        void GetCursorPosition(int32_t * /*x*/, int32_t * /*y*/) override { }
        void SetCursorPosition(int32_t /*x*/, int32_t /*y*/) override { }
        void SetCursorTrap(bool /*value*/) override { }
        const uint8_t * GetKeysState() override { return nullptr; }
        const uint8_t * GetKeysPressed() override { return nullptr; }
        void SetKeysPressed(uint32_t /*keysym*/, uint8_t /*scancode*/) override { }

        class X8DrawingEngineFactory final : public IDrawingEngineFactory
        {
            std::unique_ptr<IDrawingEngine>
                Create([[maybe_unused]] DRAWING_ENGINE_TYPE type, const std::shared_ptr<IUiContext>& uiContext) override
            {
                return std::make_unique<X8DrawingEngine>(uiContext);
            }
        };

        // Drawing
        std::shared_ptr<IDrawingEngineFactory> GetDrawingEngineFactory() override
        {
            return std::make_shared<X8DrawingEngineFactory>();
        }
        void DrawRainAnimation(IRainDrawer* rainDrawer, rct_drawpixelinfo* dpi, DrawRainFunc drawFunc) override { }

        // Text input
        bool IsTextInputActive() override { return false; }
        TextInputSession* StartTextInput([[maybe_unused]] utf8* buffer, [[maybe_unused]] size_t bufferSize) override
        {
            return nullptr;
        }

        void StopTextInput() override
        {
        }

        // In-game UI
        IWindowManager * GetWindowManager() override
        {
            return _windowManager;
        }

        // Clipboard
        bool SetClipboardText([[maybe_unused]] const utf8* target) override
        {
            return false;
        }

        ITitleSequencePlayer * GetTitleSequencePlayer() override { return nullptr; }

        ~DummyUiContext() { delete _windowManager; }
    };

    std::shared_ptr<IUiContext> CreateDummyUiContext()
    {
        return std::make_unique<DummyUiContext>();
    }
} // namespace OpenRCT2::Ui
