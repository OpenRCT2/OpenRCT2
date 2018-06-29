/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WindowManager.h"

namespace OpenRCT2::Ui
{
    class DummyWindowManager final : public IWindowManager
    {
        void Init() override {};
        rct_window * OpenWindow(rct_windowclass /*wc*/) override { return nullptr; }
        rct_window * OpenView(uint8_t /*view*/) override { return nullptr; }
        rct_window * OpenDetails(uint8_t /*type*/, int32_t /*id*/) override { return nullptr; }
        rct_window * ShowError(rct_string_id /*title*/, rct_string_id /*message*/) override { return nullptr; }
        rct_window * OpenIntent(Intent * /*intent*/) override { return nullptr; };
        void BroadcastIntent(const Intent &/*intent*/) override { }
        void ForceClose(rct_windowclass /*windowClass*/) override { }
        void UpdateMapTooltip() override { }
        void HandleInput() override { }
        void HandleKeyboard(bool /*isTitle*/) override { }
        std::string GetKeyboardShortcutString(int32_t /*shortcut*/) override { return std::string(); }
        void SetMainView(int32_t x, int32_t y, int32_t zoom, int32_t rotation) override { }
        void UpdateMouseWheel() override { }
        rct_window* GetOwner(const rct_viewport* viewport) override { return nullptr; }
    };

    IWindowManager * CreateDummyWindowManager()
    {
        return new DummyWindowManager();
    }
} // namespace OpenRCT2::Ui
