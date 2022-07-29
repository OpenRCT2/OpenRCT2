/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../interface/Window.h"
#include "../windows/Intent.h"

#include <memory>
#include <string>
#include <string_view>

class Formatter;

namespace OpenRCT2::Ui
{
    /**
     * Manager of in-game windows and widgets.
     */
    struct IWindowManager
    {
        virtual ~IWindowManager() = default;
        virtual void Init() abstract;
        virtual rct_window* OpenWindow(rct_windowclass wc) abstract;
        virtual rct_window* OpenView(uint8_t view) abstract;
        virtual rct_window* OpenDetails(uint8_t type, int32_t id) abstract;
        virtual rct_window* OpenIntent(Intent* intent) abstract;
        virtual void BroadcastIntent(const Intent& intent) abstract;
        virtual rct_window* ShowError(rct_string_id title, rct_string_id message, const Formatter& formatter) abstract;
        virtual rct_window* ShowError(std::string_view title, std::string_view message) abstract;
        virtual void ForceClose(rct_windowclass windowClass) abstract;
        virtual void UpdateMapTooltip() abstract;
        virtual void HandleInput() abstract;
        virtual void HandleKeyboard(bool isTitle) abstract;
        virtual std::string GetKeyboardShortcutString(std::string_view shortcutId) abstract;
        virtual void SetMainView(const ScreenCoordsXY& viewPos, ZoomLevel zoom, int32_t rotation) abstract;
        virtual void UpdateMouseWheel() abstract;
        virtual rct_window* GetOwner(const rct_viewport* viewport) abstract;
    };

    std::unique_ptr<IWindowManager> CreateDummyWindowManager();
} // namespace OpenRCT2::Ui
