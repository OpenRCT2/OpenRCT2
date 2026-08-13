/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/localisation/Localisation.Date.h>
#include <openrct2/ui/WindowManager.h>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
    constexpr auto kPanelWidth = 142;

    enum DateInfoPanelWidgetIdx : WidgetIndex
    {
        WIDX_RIGHT_OUTSET,
        WIDX_RIGHT_INSET,
        WIDX_DATE
    };

    // clang-format off
    static constexpr Widget kDateInfoPanelWidgets[] =
    {
        makeWidget({0, 0}, {kPanelWidth - 0, 34}, WidgetType::imgBtn,       WindowColour::primary), // Right outset panel
        makeWidget({2, 2}, {kPanelWidth - 4, 30}, WidgetType::empty,        WindowColour::primary), // Right inset panel
        makeWidget({2, 2}, {kPanelWidth - 4, 12}, WidgetType::hiddenButton, WindowColour::primary)  // Date
    };
    // clang-format on

    class DateInfoPanel final : public Window
    {
    private:
        Drawing::Colour GetHoverWidgetColour(WidgetIndex index)
        {
            return (
                gHoverWidget.windowClassification == WindowClass::dateInfoPanel && gHoverWidget.widgetIndex == index
                    ? static_cast<Drawing::Colour>(Drawing::Colour::white)
                    : colours[0].colour);
        }

        void DrawRightPanel(RenderTarget& rt)
        {
            const auto& rightPanelWidget = widgets[WIDX_RIGHT_OUTSET];

            const auto topLeft = windowPos + ScreenCoordsXY{ rightPanelWidget.left + 1, rightPanelWidget.top + 1 };
            const auto bottomRight = windowPos + ScreenCoordsXY{ rightPanelWidget.right - 1, rightPanelWidget.bottom - 1 };
            // Draw green inset rectangle on panel
            Rectangle::fillInset(
                rt, { topLeft, bottomRight }, colours[0], Rectangle::BorderStyle::inset, Rectangle::FillBrightness::light,
                Rectangle::FillMode::none);

            auto screenCoords = ScreenCoordsXY{ (rightPanelWidget.left + rightPanelWidget.right) / 2 + windowPos.x,
                                                rightPanelWidget.top + windowPos.y + 2 };

            // Date
            auto& date = GetDate();
            int32_t year = date.GetYear() + 1;
            int32_t month = date.GetMonth();
            int32_t day = date.GetDay();

            auto colour = GetHoverWidgetColour(WIDX_DATE);
            StringId stringId = DateFormatStringFormatIds[Config::Get().general.dateFormat];
            auto ft = Formatter();
            ft.Add<StringId>(DateDayNames[day]);
            ft.Add<int16_t>(month);
            ft.Add<int16_t>(year);
            drawText(rt, screenCoords, stringId, ft, { colour, TextAlignment::centre });

            // Figure out how much line height we have to work with.
            uint32_t line_height = FontGetLineHeight(FontStyle::medium);

            // Temperature
            screenCoords = { windowPos.x + rightPanelWidget.left + 15, static_cast<int32_t>(screenCoords.y + line_height + 1) };

            int32_t temperature = getGameState().weatherCurrent.temperature;
            StringId format = STR_CELSIUS_VALUE;
            if (Config::Get().general.temperatureFormat == TemperatureUnit::fahrenheit)
            {
                temperature = Weather::celsiusToFahrenheit(temperature);
                format = STR_FAHRENHEIT_VALUE;
            }
            ft = Formatter();
            ft.Add<int16_t>(temperature);
            drawText(rt, screenCoords + ScreenCoordsXY{ 0, 6 }, format, ft);
            screenCoords.x += 30;

            // Current weather
            auto currentWeatherSpriteId = Weather::getWeatherSpriteId(getGameState().weatherCurrent.weatherType);
            GfxDrawSprite(rt, ImageId(currentWeatherSpriteId), screenCoords);

            // Next weather
            auto nextWeatherSpriteId = Weather::getWeatherSpriteId(getGameState().weatherNext.weatherType);
            if (currentWeatherSpriteId != nextWeatherSpriteId)
            {
                if (getGameState().weatherUpdateTimer < 960)
                {
                    GfxDrawSprite(rt, ImageId(SPR_NEXT_WEATHER), screenCoords + ScreenCoordsXY{ 27, 5 });
                    GfxDrawSprite(rt, ImageId(nextWeatherSpriteId), screenCoords + ScreenCoordsXY{ 40, 0 });
                }
            }
        }

    public:
        DateInfoPanel()
        {
            setWidgets(kDateInfoPanelWidgets);
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_RIGHT_OUTSET:
                case WIDX_DATE:
                    ContextOpenWindow(WindowClass::recentNews);
                    break;
            }
        }

        void onPrepareDraw() override
        {
            // Figure out how much line height we have to work with.
            uint32_t line_height = FontGetLineHeight(FontStyle::medium);

            // Reset dimensions as appropriate -- in case we're switching languages.
            height = line_height * 2 + 12;
            windowPos.y = ContextGetHeight() - height;

            // Change height of widgets in accordance with line height.
            widgets[WIDX_RIGHT_OUTSET].bottom = line_height * 3 + 3;
            widgets[WIDX_RIGHT_INSET].bottom = line_height * 3 + 1;
            widgets[WIDX_DATE].bottom = line_height + 1;

            // Anchor the middle and right panel to the right
            // TODO: replace offset to full width with window size/position
            auto x = width - 1;
            widgets[WIDX_RIGHT_OUTSET].right = x;
            x -= 2;
            widgets[WIDX_RIGHT_INSET].right = x;
            x -= 137;
            widgets[WIDX_RIGHT_INSET].left = x;
            x -= 2;
            widgets[WIDX_RIGHT_OUTSET].left = x;
            x--;
            widgets[WIDX_DATE].left = widgets[WIDX_RIGHT_OUTSET].left + 2;
            widgets[WIDX_DATE].right = widgets[WIDX_RIGHT_OUTSET].right - 2;
        }

        void onDraw(RenderTarget& rt) override
        {
            const auto& rightWidget = widgets[WIDX_RIGHT_OUTSET];

            // Draw panel grey backgrounds
            auto leftTop = windowPos + ScreenCoordsXY{ rightWidget.left, rightWidget.top };
            auto rightBottom = windowPos + ScreenCoordsXY{ rightWidget.right, rightWidget.bottom };
            Rectangle::filter(rt, { leftTop, rightBottom }, FilterPaletteID::palette51);

            drawWidgets(rt);

            DrawRightPanel(rt);
        }

        CursorID onCursor(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords, CursorID cursorId) override
        {
            switch (widgetIndex)
            {
                case WIDX_DATE:
                    gTooltipCloseTimeout = gCurrentRealTimeTicks + 2000;
                    break;
            }
            return cursorId;
        }
    };

    WindowBase* dateInfoPanelOpen()
    {
        // Figure out how much line height we have to work with.
        uint32_t lineHeight = FontGetLineHeight(FontStyle::medium);
        int32_t panelHeight = lineHeight * 2 + 12;

        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->Create<DateInfoPanel>(
            WindowClass::dateInfoPanel, ScreenCoordsXY(ContextGetWidth() - kPanelWidth - 1, ContextGetHeight() - panelHeight),
            { kPanelWidth, panelHeight },
            { WindowFlag::stickToFront, WindowFlag::transparent, WindowFlag::noBackground, WindowFlag::noTitleBar });

        return window;
    }
} // namespace OpenRCT2::Ui::Windows
