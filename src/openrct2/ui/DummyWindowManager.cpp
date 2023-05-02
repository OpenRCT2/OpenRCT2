/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
        WindowBase* OpenWindow(WindowClass /*wc*/) override
        {
            return nullptr;
        }
        WindowBase* OpenView(uint8_t /*view*/) override
        {
            return nullptr;
        }
        WindowBase* OpenDetails(uint8_t /*type*/, int32_t /*id*/) override
        {
            return nullptr;
        }
        WindowBase* ShowError(StringId /*title*/, StringId /*message*/, const Formatter& /*formatter*/) override
        {
            return nullptr;
        }
        WindowBase* ShowError(std::string_view /*title*/, std::string_view /*message*/) override
        {
            return nullptr;
        }
        WindowBase* OpenIntent(Intent* /*intent*/) override
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
        WindowBase* GetOwner(const Viewport* viewport) override
        {
            return nullptr;
        }
    };

    std::unique_ptr<IWindowManager> CreateDummyWindowManager()
    {
        return std::make_unique<DummyWindowManager>();
    }
} // namespace OpenRCT2::Ui
