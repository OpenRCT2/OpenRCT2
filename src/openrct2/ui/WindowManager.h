/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

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
        virtual void Init() = 0;
        virtual WindowBase* OpenWindow(WindowClass wc) = 0;
        virtual WindowBase* OpenView(uint8_t view) = 0;
        virtual WindowBase* OpenDetails(uint8_t type, int32_t id) = 0;
        virtual WindowBase* OpenIntent(Intent* intent) = 0;
        virtual void BroadcastIntent(const Intent& intent) = 0;
        virtual WindowBase* ShowError(StringId title, StringId message, const Formatter& formatter, bool autoClose = false) = 0;
        virtual WindowBase* ShowError(std::string_view title, std::string_view message, bool autoClose = false) = 0;
        virtual void ForceClose(WindowClass windowClass) = 0;
        virtual void UpdateMapTooltip() = 0;
        virtual void HandleInput() = 0;
        virtual void HandleKeyboard(bool isTitle) = 0;
        virtual std::string GetKeyboardShortcutString(std::string_view shortcutId) = 0;
        virtual void SetMainView(const ScreenCoordsXY& viewPos, ZoomLevel zoom, int32_t rotation) = 0;
        virtual void UpdateMouseWheel() = 0;
        virtual WindowBase* GetOwner(const Viewport* viewport) = 0;
    };

    std::unique_ptr<IWindowManager> CreateDummyWindowManager();
} // namespace OpenRCT2::Ui
