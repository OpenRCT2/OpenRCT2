/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <string>
#include "../windows/Intent.h"
#include "../common.h"

#include "../interface/Window.h"

namespace OpenRCT2::Ui
{
    /**
     * Manager of in-game windows and widgets.
     */
    interface IWindowManager
    {
        virtual ~IWindowManager() = default;
        virtual void Init() abstract;
        virtual rct_window * OpenWindow(rct_windowclass wc) abstract;
        virtual rct_window * OpenView(uint8_t view) abstract;
        virtual rct_window * OpenDetails(uint8_t type, int32_t id) abstract;
        virtual rct_window * OpenIntent(Intent * intent) abstract;
        virtual void BroadcastIntent(const Intent &intent) abstract;
        virtual rct_window * ShowError(rct_string_id title, rct_string_id message) abstract;
        virtual void ForceClose(rct_windowclass windowClass) abstract;
        virtual void UpdateMapTooltip() abstract;
        virtual void HandleInput() abstract;
        virtual void HandleKeyboard(bool isTitle) abstract;
        virtual std::string GetKeyboardShortcutString(int32_t shortcut) abstract;
        virtual void SetMainView(int32_t x, int32_t y, int32_t zoom, int32_t rotation) abstract;
        virtual void UpdateMouseWheel() abstract;
        virtual rct_window* GetOwner(const rct_viewport* viewport) abstract;
    };

    IWindowManager * CreateDummyWindowManager();
} // namespace OpenRCT2::Ui
