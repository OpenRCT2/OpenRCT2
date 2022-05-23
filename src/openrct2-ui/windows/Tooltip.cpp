/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Tooltip.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>

// clang-format off
enum {
    WIDX_BACKGROUND
};

static rct_widget window_tooltip_widgets[] = {
    MakeWidget({0, 0}, {200, 32}, WindowWidgetType::ImgBtn, WindowColour::Primary),
    WIDGETS_END,
};

static rct_window_event_list window_tooltip_events([](auto& events)
{
    events.paint = &WindowTooltipPaint;
});
// clang-format on
static utf8 _tooltipText[sizeof(gCommonStringFormatBuffer)];
static int16_t _tooltipNumLines;
static int16_t deltaTime = 25; // 1tick = 1/40 = 0.025
static bool bTooltipVisibleFlag = false;
static int16_t TooltipDisplayDefaultWaitTime = 2000;
static int16_t TooltipDisplayShortWaitTime = 100;
static int16_t TooltipDisplayWaitTimeLimit = 8000;
static int16_t TooltipDisplayWaitTime = 1000;
static int16_t TooltipOnTimeCounter;
static int16_t TooltipOffCounter;

void Reset()
{
    bTooltipVisibleFlag = false;
    TooltipOnTimeCounter = 0;
    TooltipOffCounter = 0;

    TooltipDisplayWaitTime = TooltipDisplayDefaultWaitTime;
}

void WindowTooltipResetOpenSpeed()
{
    TooltipDisplayWaitTime = TooltipDisplayDefaultWaitTime;
}

void WindowTooltipFastOpen()
{
    TooltipDisplayWaitTime = TooltipDisplayShortWaitTime;
}

void WindowTooltipUpdate(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (w != nullptr && WidgetIsVisible(w, widgetIndex))
    {
        auto& widget = w->widgets[widgetIndex];
        auto stringId = widget.tooltip;

        if ((stringId != STR_NONE) || (widget.flags & WIDGET_FLAGS::TOOLTIP_IS_STRING))
        {
            if (gTooltipWidget.window_number != w->number || gTooltipWidget.window_classification != w->classification
                || gTooltipWidget.widget_index != widgetIndex)
            {
                if (!bTooltipVisibleFlag)
                {
                    ScreenCoordsXY cursorMoved = lastTooltipCursor - screenCoords;
                    if (cursorMoved.x <= 1 || cursorMoved.y <= 1)
                    {
                        TooltipOnTimeCounter += deltaTime;
                        if (TooltipOnTimeCounter >= TooltipDisplayWaitTime)
                        {
                            TooltipOffCounter = 0;
                            TooltipOnTimeCounter = TooltipDisplayWaitTime + 1000;
                            bTooltipVisibleFlag = true;
                            WindowTooltipOpen(w, widgetIndex, screenCoords);
                        }
                    }
                    else
                        TooltipOnTimeCounter = 0;
                }
                else
                {
                    TooltipOnTimeCounter = TooltipDisplayWaitTime + 1000;
                    TooltipOffCounter = 0;
                    WindowTooltipClose();
                    WindowTooltipOpen(w, widgetIndex, screenCoords);
                }
            }
        }
        else
        {
            WindowTooltipClose();
        }
    }
    else
    {
        WindowTooltipClose();
    }

    if (bTooltipVisibleFlag)
    {
        if (window_find_by_class(WC_TOOLTIP) == nullptr)
        {
            TooltipOnTimeCounter -= deltaTime;
            if (TooltipOnTimeCounter <= TooltipDisplayWaitTime)
            {
                TooltipOnTimeCounter = 0;
                bTooltipVisibleFlag = false;
            }
        }
        else if (window_find_by_class(WC_TOOLTIP) != nullptr)
        {
            TooltipOffCounter += deltaTime;
            if (TooltipOffCounter >= TooltipDisplayWaitTimeLimit)
            {
                TooltipOffCounter = 0;
                TooltipOnTimeCounter = 0;
                bTooltipVisibleFlag = false;
                window_close_by_class(WC_TOOLTIP);
            }
        }
    }

    lastTooltipCursor = screenCoords;
}

void WindowTooltipReset(const ScreenCoordsXY& screenCoords)
{
    bTooltipVisibleFlag = false;
    TooltipOnTimeCounter = 0;
    TooltipOffCounter = 0;
    TooltipDisplayWaitTime = TooltipDisplayDefaultWaitTime;
    gTooltipWidget.window_classification = 255;
    input_set_state(InputState::Normal);
    input_set_flag(INPUT_FLAG_4, false);
}

// Returns the width of the new tooltip text
static int32_t FormatTextForTooltip(const OpenRCT2String& message)
{
    utf8 tempBuffer[sizeof(gCommonStringFormatBuffer)];
    format_string(tempBuffer, sizeof(tempBuffer), message.str, message.args.Data());

    OpenRCT2String formattedMessage{ STR_STRING_TOOLTIP, Formatter() };
    formattedMessage.args.Add<const char*>(tempBuffer);
    format_string(_tooltipText, sizeof(_tooltipText), formattedMessage.str, formattedMessage.args.Data());

    auto textWidth = gfx_get_string_width_new_lined(_tooltipText, FontSpriteBase::SMALL);
    textWidth = std::min(textWidth, 196);

    int32_t numLines;
    textWidth = gfx_wrap_string(_tooltipText, textWidth + 1, FontSpriteBase::SMALL, &numLines);
    _tooltipNumLines = numLines;
    return textWidth;
}

void WindowTooltipShow(const OpenRCT2String& message, ScreenCoordsXY screenCoords)
{
    auto* w = window_find_by_class(WC_ERROR);
    if (w != nullptr)
        return;

    int32_t textWidth = FormatTextForTooltip(message);
    int32_t width = textWidth + 3;
    int32_t height = ((_tooltipNumLines + 1) * font_get_line_height(FontSpriteBase::SMALL)) + 4;
    window_tooltip_widgets[WIDX_BACKGROUND].right = width;
    window_tooltip_widgets[WIDX_BACKGROUND].bottom = height;

    int32_t screenWidth = context_get_width();
    int32_t screenHeight = context_get_height();
    screenCoords.x = std::clamp(screenCoords.x - (width / 2), 0, screenWidth - width);

    // TODO The cursor size will be relative to the window DPI.
    //      The amount to offset the y should be adjusted.

    int32_t max_y = screenHeight - height;
    screenCoords.y += 26; // Normally, we'd display the tooltip 26 lower
    if (screenCoords.y > max_y)
        // If y is too large, the tooltip could be forced below the cursor if we'd just clamped y,
        // so we'll subtract a bit more
        screenCoords.y -= height + 40;
    screenCoords.y = std::clamp(screenCoords.y, 22, max_y);

    w = WindowCreate(screenCoords, width, height, &window_tooltip_events, WC_TOOLTIP, WF_TRANSPARENT | WF_STICK_TO_FRONT);
    w->widgets = window_tooltip_widgets;
}

/**
 *
 *  rct2: 0x006EA10D
 */
void WindowTooltipOpen(rct_window* widgetWindow, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCords)
{
    if (widgetWindow == nullptr || widgetIndex == -1)
        return;

    auto widget = &widgetWindow->widgets[widgetIndex];
    window_event_invalidate_call(widgetWindow);

    OpenRCT2String result;
    if (widget->flags & WIDGET_FLAGS::TOOLTIP_IS_STRING)
    {
        result.str = STR_STRING_TOOLTIP;
        result.args = Formatter();
        result.args.Add<const char*>(widget->sztooltip);

        gTooltipWidget.window_classification = widgetWindow->classification;
        gTooltipWidget.window_number = widgetWindow->number;
        gTooltipWidget.widget_index = widgetIndex;
    }
    else
    {
        auto stringId = widget->tooltip;
        if (stringId == STR_NONE)
            return;

        gTooltipWidget.window_classification = widgetWindow->classification;
        gTooltipWidget.window_number = widgetWindow->number;
        gTooltipWidget.widget_index = widgetIndex;
        result = window_event_tooltip_call(widgetWindow, widgetIndex, stringId);
        if (result.str == STR_NONE)
            return;
    }

    WindowTooltipShow(result, screenCords);
}

/**
 *
 *  rct2: 0x006E98C6
 */
void WindowTooltipClose()
{
    window_close_by_class(WC_TOOLTIP);
    WindowTooltipResetOpenSpeed();
    gTooltipWidget.window_classification = 255;
}

/**
 *
 *  rct2: 0x006EA41D
 */

void WindowTooltipPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    int32_t left = w->windowPos.x;
    int32_t top = w->windowPos.y;
    int32_t right = w->windowPos.x + w->width - 1;
    int32_t bottom = w->windowPos.y + w->height - 1;

    // Background
    gfx_filter_rect(dpi, { { left + 1, top + 1 }, { right - 1, bottom - 1 } }, FilterPaletteID::Palette45);
    gfx_filter_rect(dpi, { { left + 1, top + 1 }, { right - 1, bottom - 1 } }, FilterPaletteID::PaletteGlassLightOrange);

    // Sides
    gfx_filter_rect(dpi, { { left + 0, top + 2 }, { left + 0, bottom - 2 } }, FilterPaletteID::PaletteDarken3);
    gfx_filter_rect(dpi, { { right + 0, top + 2 }, { right + 0, bottom - 2 } }, FilterPaletteID::PaletteDarken3);
    gfx_filter_rect(dpi, { { left + 2, bottom + 0 }, { right - 2, bottom + 0 } }, FilterPaletteID::PaletteDarken3);
    gfx_filter_rect(dpi, { { left + 2, top + 0 }, { right - 2, top + 0 } }, FilterPaletteID::PaletteDarken3);

    // Corners
    gfx_filter_pixel(dpi, { left + 1, top + 1 }, FilterPaletteID::PaletteDarken3);
    gfx_filter_pixel(dpi, { right - 1, top + 1 }, FilterPaletteID::PaletteDarken3);
    gfx_filter_pixel(dpi, { left + 1, bottom - 1 }, FilterPaletteID::PaletteDarken3);
    gfx_filter_pixel(dpi, { right - 1, bottom - 1 }, FilterPaletteID::PaletteDarken3);

    // Text
    left = w->windowPos.x + ((w->width + 1) / 2) - 1;
    top = w->windowPos.y + 1;
    draw_string_centred_raw(dpi, { left, top }, _tooltipNumLines, _tooltipText, FontSpriteBase::SMALL);
}
