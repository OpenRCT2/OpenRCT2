/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/interface/Window.h>
#include <openrct2/interface/WindowBase.h>

struct TextInputSession;

namespace OpenRCT2::Ui
{
    constexpr ScreenSize kMaxWindowSize = { 5000, 5000 };

    struct Window : WindowBase
    {
        void OnDraw(RenderTarget& rt) override;
        void OnDrawWidget(WidgetIndex widgetIndex, RenderTarget& rt) override;

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
        void DrawWidgets(RenderTarget& rt);
        void Close();
        void CloseOthers();
        void CloseOthersOfThisClass();
        CloseWindowModifier GetCloseModifier();
        void TextInputOpen(
            WidgetIndex callWidget, StringId title, StringId description, const Formatter& descriptionArgs,
            StringId existingText, uintptr_t existingArgs, int32_t maxLength);

        void ResizeSpinner(WidgetIndex widgetIndex, const ScreenCoordsXY& origin, const ScreenSize& size);
        void ResizeDropdown(WidgetIndex widgetIndex, const ScreenCoordsXY& origin, const ScreenSize& size);
    };

    void WindowAllWheelInput();
    void ApplyScreenSaverLockSetting();
    void WindowAlignTabs(WindowBase* w, WidgetIndex start_tab_id, WidgetIndex end_tab_id);
    ScreenCoordsXY WindowGetViewportSoundIconPos(WindowBase& w);
} // namespace OpenRCT2::Ui

namespace OpenRCT2::Ui::Windows
{
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

    void WindowResizeByDelta(WindowBase& w, int16_t dw, int16_t dh);
    void WindowInitScrollWidgets(WindowBase& w);
    void WindowUpdateScrollWidgets(WindowBase& w);

    void WindowMovePosition(WindowBase& w, const ScreenCoordsXY& screenCoords);
    void WindowSetPosition(WindowBase& w, const ScreenCoordsXY& screenCoords);
    void WindowMoveAndSnap(WindowBase& w, ScreenCoordsXY newWindowCoords, int32_t snapProximity);
    void WindowRelocateWindows(int32_t width, int32_t height);

    bool WindowSetResize(WindowBase& w, ScreenSize minSize, ScreenSize maxSize);
    bool WindowCanResize(const WindowBase& w);

    void InvalidateAllWindowsAfterInput();

    void WindowDrawWidgets(WindowBase& w, RenderTarget& rt);
    void WindowDrawViewport(RenderTarget& rt, WindowBase& w);

    void WindowZoomIn(WindowBase& w, bool atCursor);
    void WindowZoomOut(WindowBase& w, bool atCursor);
    void MainWindowZoom(bool zoomIn, bool atCursor);
} // namespace OpenRCT2::Ui::Windows
