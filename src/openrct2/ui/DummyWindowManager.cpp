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
        rct_window * OpenView(uint8 /*view*/) override { return nullptr; }
        rct_window * OpenDetails(uint8 /*type*/, sint32 /*id*/) override { return nullptr; }
        rct_window * ShowError(rct_string_id /*title*/, rct_string_id /*message*/) override { return nullptr; }
        rct_window * OpenIntent(Intent * /*intent*/) override { return nullptr; };
        void BroadcastIntent(const Intent &/*intent*/) override { }
        void ForceClose(rct_windowclass /*windowClass*/) override { }
        void UpdateMapTooltip() override { }
        void HandleInput() override { }
        void HandleKeyboard(bool /*isTitle*/) override { }
        std::string GetKeyboardShortcutString(sint32 /*shortcut*/) override { return std::string(); }
        void SetMainView(sint32 x, sint32 y, sint32 zoom, sint32 rotation) override { }
        void UpdateMouseWheel() override { }
    };

    IWindowManager * CreateDummyWindowManager()
    {
        return new DummyWindowManager();
    }
} // namespace OpenRCT2::Ui
