/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/interface/Window.h>
#include <openrct2/interface/Window_internal.h>

struct TextInputSession;

struct Window : WindowBase
{
    virtual void OnDraw(DrawPixelInfo& dpi) override;
    virtual void OnDrawWidget(WidgetIndex widgetIndex, DrawPixelInfo& dpi) override;

    void ScrollToViewport();
    void InitScrollWidgets();
    void InvalidateWidget(WidgetIndex widgetIndex);
    bool IsWidgetDisabled(WidgetIndex widgetIndex) const;
    bool IsWidgetPressed(WidgetIndex widgetIndex) const;
    void SetWidgetEnabled(WidgetIndex widgetIndex, bool value);
    void SetWidgetDisabled(WidgetIndex widgetIndex, bool value);
    void SetWidgetDisabledAndInvalidate(WidgetIndex widgetIndex, bool value);
    void SetWidgetPressed(WidgetIndex widgetIndex, bool value);
    void SetCheckboxValue(WidgetIndex widgetIndex, bool value);
    void DrawWidgets(DrawPixelInfo& dpi);
    void Close();
    void CloseOthers();
    void CloseOthersOfThisClass();
    CloseWindowModifier GetCloseModifier();
    void TextInputOpen(
        WidgetIndex callWidget, StringId title, StringId description, const Formatter& descriptionArgs, StringId existingText,
        uintptr_t existingArgs, int32_t maxLength);

    void ResizeFrame();
    void ResizeFrameWithPage();

    void ResizeSpinner(WidgetIndex widgetIndex, const ScreenCoordsXY& origin, const ScreenSize& size);
    void ResizeDropdown(WidgetIndex widgetIndex, const ScreenCoordsXY& origin, const ScreenSize& size);
};

void WindowAllWheelInput();
void ApplyScreenSaverLockSetting();
void WindowAlignTabs(WindowBase* w, WidgetIndex start_tab_id, WidgetIndex end_tab_id);
ScreenCoordsXY WindowGetViewportSoundIconPos(WindowBase& w);

namespace OpenRCT2::Ui::Windows
{
    WindowBase* WindowCreate(
        std::unique_ptr<WindowBase>&& w, WindowClass cls, ScreenCoordsXY pos, int32_t width, int32_t height, uint32_t flags);
    template<typename T, typename... TArgs, typename std::enable_if<std::is_base_of<WindowBase, T>::value>::type* = nullptr>
    T* WindowCreate(
        WindowClass cls, const ScreenCoordsXY& pos = {}, int32_t width = 0, int32_t height = 0, uint32_t flags = 0,
        TArgs&&... args)
    {
        return static_cast<T*>(WindowCreate(std::make_unique<T>(std::forward<TArgs>(args)...), cls, pos, width, height, flags));
    }
    template<typename T, typename... TArgs, typename std::enable_if<std::is_base_of<WindowBase, T>::value>::type* = nullptr>
    T* WindowCreate(WindowClass cls, int32_t width, int32_t height, uint32_t flags, TArgs&&... args)
    {
        return static_cast<T*>(
            WindowCreate(std::make_unique<T>(std::forward<TArgs>(args)...), cls, {}, width, height, flags | WF_AUTO_POSITION));
    }
    template<typename T, typename std::enable_if<std::is_base_of<WindowBase, T>::value>::type* = nullptr>
    T* WindowFocusOrCreate(WindowClass cls, const ScreenCoordsXY& pos, int32_t width, int32_t height, uint32_t flags = 0)
    {
        auto* w = WindowBringToFrontByClass(cls);
        if (w == nullptr)
        {
            w = WindowCreate<T>(cls, pos, width, height, flags);
        }
        return static_cast<T*>(w);
    }
    template<typename T, typename std::enable_if<std::is_base_of<WindowBase, T>::value>::type* = nullptr>
    T* WindowFocusOrCreate(WindowClass cls, int32_t width, int32_t height, uint32_t flags = 0)
    {
        auto* w = WindowBringToFrontByClass(cls);
        if (w == nullptr)
        {
            w = WindowCreate<T>(cls, width, height, flags);
        }
        return static_cast<T*>(w);
    }

    void RideConstructionToolupdateEntranceExit(const ScreenCoordsXY& screenCoords);
    void RideConstructionToolupdateConstruct(const ScreenCoordsXY& screenCoords);
    void RideConstructionTooldownConstruct(const ScreenCoordsXY& screenCoords);
    void UpdateGhostTrackAndArrow();
    void WindowRideConstructionKeyboardShortcutTurnLeft();
    void WindowRideConstructionKeyboardShortcutTurnRight();
    void WindowRideConstructionKeyboardShortcutUseTrackDefault();
    void WindowRideConstructionKeyboardShortcutSlopeDown();
    void WindowRideConstructionKeyboardShortcutSlopeUp();
    void WindowRideConstructionKeyboardShortcutChainLiftToggle();
    void WindowRideConstructionKeyboardShortcutBankLeft();
    void WindowRideConstructionKeyboardShortcutBankRight();
    void WindowRideConstructionKeyboardShortcutPreviousTrack();
    void WindowRideConstructionKeyboardShortcutNextTrack();
    void WindowRideConstructionKeyboardShortcutBuildCurrent();
    void WindowRideConstructionKeyboardShortcutDemolishCurrent();

    void WindowFootpathKeyboardShortcutTurnLeft();
    void WindowFootpathKeyboardShortcutTurnRight();
    void WindowFootpathKeyboardShortcutSlopeDown();
    void WindowFootpathKeyboardShortcutSlopeUp();
    void WindowFootpathKeyboardShortcutBuildCurrent();
    void WindowFootpathKeyboardShortcutDemolishCurrent();

    void WindowTileInspectorKeyboardShortcutToggleInvisibility();

    extern const StringId ColourSchemeNames[4];

    WindowBase* WindowGetListening();
    WindowClass WindowGetClassification(const WindowBase& window);

    void WindowStartTextbox(const WindowBase& callW, WidgetIndex callWidget, u8string existingText, int32_t maxLength);
    void WindowCancelTextbox();
    void WindowUpdateTextboxCaret();
    void WindowUpdateTextbox();

    const TextInputSession* GetTextboxSession();
    void SetTexboxSession(TextInputSession* session);
    bool IsUsingWidgetTextBox();
    bool TextBoxCaretIsFlashed();
    const WidgetIdentifier& GetCurrentTextBox();

    void WindowResize(WindowBase& w, int32_t dw, int32_t dh);
    void WindowInitScrollWidgets(WindowBase& w);
    void WindowUpdateScrollWidgets(WindowBase& w);

    void WindowMovePosition(WindowBase& w, const ScreenCoordsXY& screenCoords);
    void WindowSetPosition(WindowBase& w, const ScreenCoordsXY& screenCoords);
    void WindowMoveAndSnap(WindowBase& w, ScreenCoordsXY newWindowCoords, int32_t snapProximity);
    void WindowRelocateWindows(int32_t width, int32_t height);

    void WindowSetResize(WindowBase& w, int32_t minWidth, int32_t minHeight, int32_t maxWidth, int32_t maxHeight);
    bool WindowCanResize(const WindowBase& w);

    void InvalidateAllWindowsAfterInput();

    void WindowDrawWidgets(WindowBase& w, DrawPixelInfo& dpi);
    void WindowDrawViewport(DrawPixelInfo& dpi, WindowBase& w);

    void WindowZoomIn(WindowBase& w, bool atCursor);
    void WindowZoomOut(WindowBase& w, bool atCursor);
    void MainWindowZoom(bool zoomIn, bool atCursor);
} // namespace OpenRCT2::Ui::Windows
