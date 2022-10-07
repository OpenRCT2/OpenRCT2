/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../localisation/Formatter.h"
#include "WindowManager.h"

namespace OpenRCT2::Ui
{
    class DummyWindowManager final : public IWindowManager
    {
        void Init() override{};
        rct_window* OpenWindow(WindowClass /*wc*/) override
        {
            return nullptr;
        }
        rct_window* OpenView(uint8_t /*view*/) override
        {
            return nullptr;
        }
        rct_window* OpenDetails(uint8_t /*type*/, int32_t /*id*/) override
        {
            return nullptr;
        }
        rct_window* ShowError(StringId /*title*/, StringId /*message*/, const Formatter& /*formatter*/) override
        {
            return nullptr;
        }
        rct_window* ShowError(std::string_view /*title*/, std::string_view /*message*/) override
        {
            return nullptr;
        }
        rct_window* OpenIntent(Intent* /*intent*/) override
        {
            return nullptr;
        };
        void BroadcastIntent(const Intent& /*intent*/) override
        {
        }
        void ForceClose(WindowClass /*windowClass*/) override
        {
        }
        void UpdateMapTooltip() override
        {
        }
        void HandleInput() override
        {
        }
        void HandleKeyboard(bool /*isTitle*/) override
        {
        }
        std::string GetKeyboardShortcutString(std::string_view /*shortcutId*/) override
        {
            return std::string();
        }
        void SetMainView(const ScreenCoordsXY& viewPos, ZoomLevel zoom, int32_t rotation) override
        {
        }
        void UpdateMouseWheel() override
        {
        }
        rct_window* GetOwner(const rct_viewport* viewport) override
        {
            return nullptr;
        }
    };

    std::unique_ptr<IWindowManager> CreateDummyWindowManager()
    {
        return std::make_unique<DummyWindowManager>();
    }
} // namespace OpenRCT2::Ui
