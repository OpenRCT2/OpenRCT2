/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

namespace OpenRCT2
{
    class Formatter;
}

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
        virtual WindowBase* openView(WindowView view) = 0;
        virtual WindowBase* openDetails(WindowDetail type, int32_t id) = 0;
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

        virtual WindowBase* Create(
            std::unique_ptr<WindowBase>&& w, WindowClass cls, ScreenCoordsXY pos, ScreenSize size, WindowFlags flags)
            = 0;

        template<typename T, typename... TArgs, typename std::enable_if<std::is_base_of<WindowBase, T>::value>::type* = nullptr>
        T* Create(
            WindowClass cls, const ScreenCoordsXY& pos = {}, ScreenSize size = {}, WindowFlags flags = {}, TArgs&&... args)
        {
            return static_cast<T*>(Create(std::make_unique<T>(std::forward<TArgs>(args)...), cls, pos, size, flags));
        }

        template<typename T, typename... TArgs, typename std::enable_if<std::is_base_of<WindowBase, T>::value>::type* = nullptr>
        T* Create(WindowClass cls, ScreenSize size, WindowFlags flags, TArgs&&... args)
        {
            return static_cast<T*>(
                Create(std::make_unique<T>(std::forward<TArgs>(args)...), cls, {}, size, flags | WindowFlag::autoPosition));
        }

        template<typename T, typename std::enable_if<std::is_base_of<WindowBase, T>::value>::type* = nullptr>
        T* FocusOrCreate(WindowClass cls, const ScreenCoordsXY& pos, ScreenSize size, WindowFlags flags = {})
        {
            auto* w = BringToFrontByClass(cls);
            if (w == nullptr)
            {
                w = Create<T>(cls, pos, size, flags);
            }
            return static_cast<T*>(w);
        }

        template<typename T, typename std::enable_if<std::is_base_of<WindowBase, T>::value>::type* = nullptr>
        T* FocusOrCreate(WindowClass cls, ScreenSize size, WindowFlags flags = {})
        {
            auto* w = BringToFrontByClass(cls);
            if (w == nullptr)
            {
                w = Create<T>(cls, size, flags);
            }
            return static_cast<T*>(w);
        }

        virtual void Close(WindowBase& window) = 0;
        virtual void CloseSurplus(int32_t cap, WindowClass avoid_classification) = 0;
        virtual void CloseByClass(WindowClass cls) = 0;
        virtual void CloseByNumber(WindowClass cls, WindowNumber number) = 0;
        virtual void CloseByNumber(WindowClass cls, EntityId number) = 0;
        virtual void CloseTop() = 0;
        virtual void CloseAll() = 0;
        virtual void CloseAllExceptClass(WindowClass cls) = 0;
        virtual void CloseAllExceptFlags(WindowFlags flags) = 0;
        virtual void CloseAllExceptNumberAndClass(WindowNumber number, WindowClass cls) = 0;
        virtual void CloseConstructionWindows() = 0;

        virtual WindowBase* FindByClass(WindowClass cls) = 0;
        virtual WindowBase* FindByNumber(WindowClass cls, WindowNumber number) = 0;
        virtual WindowBase* FindByNumber(WindowClass cls, EntityId id) = 0;
        virtual WindowBase* FindFromPoint(const ScreenCoordsXY& screenCoords) = 0;
        virtual WidgetIndex FindWidgetFromPoint(WindowBase& w, const ScreenCoordsXY& screenCoords) = 0;

        virtual void InvalidateByClass(WindowClass cls) = 0;
        virtual void InvalidateByNumber(WindowClass cls, WindowNumber number) = 0;
        virtual void InvalidateByNumber(WindowClass cls, EntityId id) = 0;
        virtual void InvalidateAll() = 0;
        virtual void InvalidateWidget(WindowBase& w, WidgetIndex widgetIndex) = 0;
        virtual void InvalidateWidgetByClass(WindowClass cls, WidgetIndex widgetIndex) = 0;
        virtual void InvalidateWidgetByNumber(WindowClass cls, WindowNumber number, WidgetIndex widgetIndex) = 0;

        virtual WindowBase* BringToFront(WindowBase& w) = 0;
        virtual WindowBase* BringToFrontByClass(WindowClass cls) = 0;
        virtual WindowBase* BringToFrontByNumber(WindowClass cls, WindowNumber number) = 0;
    };

    std::unique_ptr<IWindowManager> CreateDummyWindowManager();

    IWindowManager* GetWindowManager();
} // namespace OpenRCT2::Ui
