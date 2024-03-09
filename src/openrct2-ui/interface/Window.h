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

struct Window : WindowBase
{
    virtual bool IsLegacy() override;
    virtual void OnDraw(DrawPixelInfo& dpi) override;
    virtual void OnDrawWidget(WidgetIndex widgetIndex, DrawPixelInfo& dpi) override;

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
};

void WindowAllWheelInput();
void ApplyScreenSaverLockSetting();
void WindowAlignTabs(WindowBase* w, WidgetIndex start_tab_id, WidgetIndex end_tab_id);
ScreenCoordsXY WindowGetViewportSoundIconPos(WindowBase& w);

namespace OpenRCT2::Ui::Windows
{
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
} // namespace OpenRCT2::Ui::Windows
