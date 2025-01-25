/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
        void Init() override {};
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
        WindowBase* ShowError(
            StringId /*title*/, StringId /*message*/, const Formatter& /*formatter*/, bool /*autoClose*/) override
        {
            return nullptr;
        }
        WindowBase* ShowError(std::string_view /*title*/, std::string_view /*message*/, bool /*autoClose*/) override
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

        WindowBase* Create(
            std::unique_ptr<WindowBase>&& w, WindowClass cls, ScreenCoordsXY pos, int32_t width, int32_t height,
            uint32_t flags) override
        {
            return nullptr;
        }

        void Close(WindowBase& window) override {};
        void CloseSurplus(int32_t cap, WindowClass avoid_classification) override {};
        void CloseByClass(WindowClass cls) override {};
        void CloseByNumber(WindowClass cls, rct_windownumber number) override {};
        void CloseByNumber(WindowClass cls, EntityId number) override {};
        void CloseTop() override {};
        void CloseAll() override {};
        void CloseAllExceptClass(WindowClass cls) override {};
        void CloseAllExceptFlags(uint16_t flags) override {};
        void CloseAllExceptNumberAndClass(rct_windownumber number, WindowClass cls) override {};
        void CloseConstructionWindows() override {};

        WindowBase* FindByClass(WindowClass cls) override
        {
            return nullptr;
        }
        WindowBase* FindByNumber(WindowClass cls, rct_windownumber number) override
        {
            return nullptr;
        }
        WindowBase* FindByNumber(WindowClass cls, EntityId id) override
        {
            return nullptr;
        }
        WindowBase* FindFromPoint(const ScreenCoordsXY& screenCoords) override
        {
            return nullptr;
        }
        WidgetIndex FindWidgetFromPoint(WindowBase& w, const ScreenCoordsXY& screenCoords) override
        {
            return kWidgetIndexNull;
        }

        void InvalidateByClass(WindowClass cls) override {};
        void InvalidateByNumber(WindowClass cls, rct_windownumber number) override {};
        void InvalidateByNumber(WindowClass cls, EntityId id) override {};
        void InvalidateAll() override {};
        void InvalidateWidget(WindowBase& w, WidgetIndex widgetIndex) override {};
        void InvalidateWidgetByClass(WindowClass cls, WidgetIndex widgetIndex) override {};
        void InvalidateWidgetByNumber(WindowClass cls, rct_windownumber number, WidgetIndex widgetIndex) override {};

        WindowBase* BringToFront(WindowBase& w) override
        {
            return nullptr;
        }
        WindowBase* BringToFrontByClass(WindowClass cls) override
        {
            return nullptr;
        }
        WindowBase* BringToFrontByClassWithFlags(WindowClass cls, uint16_t flags) override
        {
            return nullptr;
        }
        WindowBase* BringToFrontByNumber(WindowClass cls, rct_windownumber number) override
        {
            return nullptr;
        }
    };

    std::unique_ptr<IWindowManager> CreateDummyWindowManager()
    {
        return std::make_unique<DummyWindowManager>();
    }
} // namespace OpenRCT2::Ui
