/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
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

// clang-format on

class TooltipWindow final : public Window
{
private:
    utf8 _tooltipText[sizeof(gCommonStringFormatBuffer)]{};
    int16_t _tooltipNumLines = 1;

public:
    TooltipWindow(const OpenRCT2String& message, ScreenCoordsXY screenCoords)
    {
        int32_t textWidth = FormatTextForTooltip(message);
        width = textWidth + 3;
        height = ((_tooltipNumLines + 1) * font_get_line_height(FontStyle::Small)) + 4;

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

        windowPos = screenCoords;
    }

    void OnOpen() override
    {
        widgets = window_tooltip_widgets;
        reset_tooltip_not_shown();
    }

    void OnUpdate() override
    {
        reset_tooltip_not_shown();
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        int32_t left = windowPos.x;
        int32_t top = windowPos.y;
        int32_t right = windowPos.x + width - 1;
        int32_t bottom = windowPos.y + height - 1;

        // Background
        gfx_filter_rect(&dpi, { { left + 1, top + 1 }, { right - 1, bottom - 1 } }, FilterPaletteID::Palette45);
        gfx_filter_rect(&dpi, { { left + 1, top + 1 }, { right - 1, bottom - 1 } }, FilterPaletteID::PaletteGlassLightOrange);

        // Sides
        gfx_filter_rect(&dpi, { { left + 0, top + 2 }, { left + 0, bottom - 2 } }, FilterPaletteID::PaletteDarken3);
        gfx_filter_rect(&dpi, { { right + 0, top + 2 }, { right + 0, bottom - 2 } }, FilterPaletteID::PaletteDarken3);
        gfx_filter_rect(&dpi, { { left + 2, bottom + 0 }, { right - 2, bottom + 0 } }, FilterPaletteID::PaletteDarken3);
        gfx_filter_rect(&dpi, { { left + 2, top + 0 }, { right - 2, top + 0 } }, FilterPaletteID::PaletteDarken3);

        // Corners
        gfx_filter_pixel(&dpi, { left + 1, top + 1 }, FilterPaletteID::PaletteDarken3);
        gfx_filter_pixel(&dpi, { right - 1, top + 1 }, FilterPaletteID::PaletteDarken3);
        gfx_filter_pixel(&dpi, { left + 1, bottom - 1 }, FilterPaletteID::PaletteDarken3);
        gfx_filter_pixel(&dpi, { right - 1, bottom - 1 }, FilterPaletteID::PaletteDarken3);

        // Text
        left = windowPos.x + ((width + 1) / 2) - 1;
        top = windowPos.y + 1;
        draw_string_centred_raw(&dpi, { left, top }, _tooltipNumLines, _tooltipText, FontStyle::Small);
    }

private:
    // Returns the width of the new tooltip text
    int32_t FormatTextForTooltip(const OpenRCT2String& message)
    {
        utf8 tempBuffer[sizeof(gCommonStringFormatBuffer)];
        format_string(tempBuffer, sizeof(tempBuffer), message.str, message.args.Data());

        OpenRCT2String formattedMessage{ STR_STRING_TOOLTIP, Formatter() };
        formattedMessage.args.Add<const char*>(tempBuffer);
        format_string(_tooltipText, sizeof(_tooltipText), formattedMessage.str, formattedMessage.args.Data());

        auto textWidth = gfx_get_string_width_new_lined(_tooltipText, FontStyle::Small);
        textWidth = std::min(textWidth, 196);

        int32_t numLines;
        textWidth = gfx_wrap_string(_tooltipText, textWidth + 1, FontStyle::Small, &numLines);
        _tooltipNumLines = numLines;
        return textWidth;
    }
};

void WindowTooltipReset(const ScreenCoordsXY& screenCoords)
{
    gTooltipCursor = screenCoords;
    gTooltipTimeout = 0;
    gTooltipWidget.window_classification = WindowClass::Null;
    input_set_state(InputState::Normal);
    input_set_flag(INPUT_FLAG_4, false);
}

void WindowTooltipShow(const OpenRCT2String& message, ScreenCoordsXY screenCoords)
{
    auto* w = window_find_by_class(WindowClass::Error);
    if (w != nullptr)
        return;

    auto tooltipWindow = std::make_unique<TooltipWindow>(message, screenCoords);
    auto windowPos = tooltipWindow->windowPos;
    auto width = tooltipWindow->width;
    auto height = tooltipWindow->height;
    WindowCreate(std::move(tooltipWindow), WindowClass::Tooltip, windowPos, width, height, WF_TRANSPARENT | WF_STICK_TO_FRONT);
}

void WindowTooltipOpen(rct_window* widgetWindow, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (widgetWindow == nullptr || widgetIndex == -1)
        return;

    auto widget = &widgetWindow->widgets[widgetIndex];
    window_event_invalidate_call(widgetWindow);

    OpenRCT2String result;
    if (widget->flags & WIDGET_FLAGS::TOOLTIP_IS_STRING)
    {
        auto tooltipString = widget->sztooltip;
        if (*tooltipString == 0)
            return;

        result.str = STR_STRING_TOOLTIP;
        result.args = Formatter();
        result.args.Add<const char*>(tooltipString);

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

    WindowTooltipShow(result, screenCoords);
}

void WindowTooltipClose()
{
    window_close_by_class(WindowClass::Tooltip);
    gTooltipTimeout = 0;
    gTooltipWidget.window_classification = WindowClass::Null;
}
