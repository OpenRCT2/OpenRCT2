/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Ui::Windows
{
    enum
    {
        WIDX_BACKGROUND
    };

    static constexpr Widget _tooltipWidgets[] = {
        MakeWidget({ 0, 0 }, { 200, 32 }, WindowWidgetType::ImgBtn, WindowColour::Primary),
    };

    class TooltipWindow final : public Window
    {
    private:
        u8string _tooltipText;
        int16_t _tooltipNumLines = 1;

    public:
        TooltipWindow(const OpenRCT2String& message, ScreenCoordsXY screenCoords)
        {
            int32_t textWidth = FormatTextForTooltip(message);
            int32_t textHeight = ((_tooltipNumLines + 1) * FontGetLineHeight(FontStyle::Small));

            width = textWidth + 5;
            height = textHeight + 4;

            SetWidgets(_tooltipWidgets);
            widgets[WIDX_BACKGROUND].right = width;
            widgets[WIDX_BACKGROUND].bottom = height;

            UpdatePosition(screenCoords);
        }

        void UpdatePosition(ScreenCoordsXY screenCoords)
        {
            int32_t screenWidth = ContextGetWidth();
            int32_t screenHeight = ContextGetHeight();
            screenCoords.x = std::clamp(screenCoords.x - (width / 2), 0, screenWidth - width);

            // TODO The cursor size will be relative to the window DPI.
            //      The amount to offset the y should be adjusted.

            const int32_t cursorHeight = 20;
            const int32_t cursorMargin = 4;
            const int32_t cursorOffset = cursorHeight + cursorMargin;

            const int32_t maxY = screenHeight - height;

            if (screenCoords.y + cursorOffset > maxY)
            {
                // Display the tooltip above the cursor if there is not enough space below.
                screenCoords.y -= (height + cursorMargin);
            }
            else
            {
                // Display the tooltip under the cursor if there is enough space below.
                screenCoords.y += cursorOffset;
            }

            screenCoords.y = std::clamp(screenCoords.y, cursorOffset, maxY);

            if (windowPos != screenCoords)
            {
                WindowSetPosition(*this, screenCoords);
            }
        }

        void OnOpen() override
        {
            ResetTooltipNotShown();
        }

        void OnUpdate() override
        {
            UpdatePosition(gTooltipCursor);
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            int32_t left = windowPos.x;
            int32_t top = windowPos.y;
            int32_t right = windowPos.x + width - 1;
            int32_t bottom = windowPos.y + height - 1;

            // Background
            GfxFilterRect(dpi, { { left + 1, top + 1 }, { right - 1, bottom - 1 } }, FilterPaletteID::Palette45);
            GfxFilterRect(dpi, { { left + 1, top + 1 }, { right - 1, bottom - 1 } }, FilterPaletteID::PaletteGlassLightOrange);

            // Sides
            GfxFilterRect(dpi, { { left + 0, top + 2 }, { left + 0, bottom - 2 } }, FilterPaletteID::PaletteDarken3);
            GfxFilterRect(dpi, { { right + 0, top + 2 }, { right + 0, bottom - 2 } }, FilterPaletteID::PaletteDarken3);
            GfxFilterRect(dpi, { { left + 2, bottom + 0 }, { right - 2, bottom + 0 } }, FilterPaletteID::PaletteDarken3);
            GfxFilterRect(dpi, { { left + 2, top + 0 }, { right - 2, top + 0 } }, FilterPaletteID::PaletteDarken3);

            // Corners
            GfxFilterPixel(dpi, { left + 1, top + 1 }, FilterPaletteID::PaletteDarken3);
            GfxFilterPixel(dpi, { right - 1, top + 1 }, FilterPaletteID::PaletteDarken3);
            GfxFilterPixel(dpi, { left + 1, bottom - 1 }, FilterPaletteID::PaletteDarken3);
            GfxFilterPixel(dpi, { right - 1, bottom - 1 }, FilterPaletteID::PaletteDarken3);

            // Text
            left = windowPos.x + ((width + 1) / 2) - 1;
            top = windowPos.y + 1;
            DrawStringCentredRaw(dpi, { left, top }, _tooltipNumLines, _tooltipText.data(), FontStyle::Small);
        }

    private:
        // Returns the width of the new tooltip text
        int32_t FormatTextForTooltip(const OpenRCT2String& message)
        {
            const u8string tempString = FormatStringIDLegacy(message.str, message.args.Data());

            OpenRCT2String formattedMessage{ STR_STRING_TOOLTIP, Formatter() };
            formattedMessage.args.Add<const char*>(tempString.c_str());
            const u8string tooltipTextUnwrapped = FormatStringIDLegacy(formattedMessage.str, formattedMessage.args.Data());

            auto textWidth = GfxGetStringWidthNewLined(tooltipTextUnwrapped, FontStyle::Small);
            textWidth = std::min(textWidth, 196);

            int32_t numLines;
            textWidth = GfxWrapString(tooltipTextUnwrapped, textWidth + 1, FontStyle::Small, &_tooltipText, &numLines);
            _tooltipNumLines = numLines;
            return textWidth;
        }
    };

    void WindowTooltipReset(const ScreenCoordsXY& screenCoords)
    {
        gTooltipCursor = screenCoords;
        gTooltipCloseTimeout = 0;
        gTooltipWidget.window_classification = WindowClass::Null;
        InputSetState(InputState::Normal);
        InputSetFlag(INPUT_FLAG_4, false);
    }

    void WindowTooltipShow(const OpenRCT2String& message, ScreenCoordsXY screenCoords)
    {
        auto tooltipWindow = std::make_unique<TooltipWindow>(message, screenCoords);
        auto windowPos = tooltipWindow->windowPos;
        auto width = tooltipWindow->width;
        auto height = tooltipWindow->height;

        auto* windowMgr = GetWindowManager();
        windowMgr->Create(
            std::move(tooltipWindow), WindowClass::Tooltip, windowPos, width, height, WF_TRANSPARENT | WF_STICK_TO_FRONT);
    }

    void WindowTooltipOpen(WindowBase* widgetWindow, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
    {
        if (widgetWindow == nullptr || widgetIndex == kWidgetIndexNull)
            return;

        auto widget = &widgetWindow->widgets[widgetIndex];
        widgetWindow->OnPrepareDraw();

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
            gTooltipWidget.window_classification = widgetWindow->classification;
            gTooltipWidget.window_number = widgetWindow->number;
            gTooltipWidget.widget_index = widgetIndex;
            result = widgetWindow->OnTooltip(widgetIndex, stringId);
            if (result.str == kStringIdNone)
                return;
        }

        WindowTooltipShow(result, screenCoords);
    }

    void WindowTooltipClose()
    {
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->CloseByClass(WindowClass::Tooltip);

        gTooltipCloseTimeout = 0;
        gTooltipWidget.window_classification = WindowClass::Null;
    }
} // namespace OpenRCT2::Ui::Windows
