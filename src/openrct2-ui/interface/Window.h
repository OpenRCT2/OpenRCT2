/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
