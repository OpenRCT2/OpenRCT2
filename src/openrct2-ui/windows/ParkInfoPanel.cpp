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
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/ui/WindowManager.h>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
    constexpr auto kPanelWidth = 142;

    enum ParkInfoPanelWidgetIdx : WidgetIndex
    {
        WIDX_LEFT_OUTSET,
        WIDX_LEFT_INSET,
        WIDX_MONEY,
        WIDX_GUESTS,
        WIDX_PARK_RATING,
    };

    // clang-format off
    static constexpr Widget kParkInfoPanelWidgets[] =
    {
        makeWidget({0,  0}, {kPanelWidth - 0, 34}, WidgetType::imgBtn,       WindowColour::primary                                                    ), // Left outset panel
        makeWidget({2,  2}, {kPanelWidth - 4, 30}, WidgetType::empty,        WindowColour::primary                                                    ), // Left inset panel
        makeWidget({2,  1}, {kPanelWidth - 4, 12}, WidgetType::hiddenButton, WindowColour::primary, 0xFFFFFFFF, STR_PROFIT_PER_WEEK_AND_PARK_VALUE_TIP), // Money
        makeWidget({2, 11}, {kPanelWidth - 4, 12}, WidgetType::hiddenButton, WindowColour::primary                                                    ), // Guests
        makeWidget({2, 21}, {kPanelWidth - 4, 11}, WidgetType::hiddenButton, WindowColour::primary, 0xFFFFFFFF, STR_PARK_RATING_TIP                   )  // Park rating
    };
    // clang-format on

    class ParkInfoPanel final : public Window
    {
    private:
        Drawing::Colour GetHoverWidgetColour(WidgetIndex index)
        {
            return (
                gHoverWidget.windowClassification == WindowClass::bottomToolbar && gHoverWidget.widgetIndex == index
                    ? static_cast<Drawing::Colour>(Drawing::Colour::white)
                    : colours[0].colour);
        }

        void DrawLeftPanel(RenderTarget& rt)
        {
            const auto& leftPanelWidget = widgets[WIDX_LEFT_OUTSET];

            const auto topLeft = windowPos + ScreenCoordsXY{ leftPanelWidget.left + 1, leftPanelWidget.top + 1 };
            const auto bottomRight = windowPos + ScreenCoordsXY{ leftPanelWidget.right - 1, leftPanelWidget.bottom - 1 };
            // Draw green inset rectangle on panel
            Rectangle::fillInset(
                rt, { topLeft, bottomRight }, colours[0], Rectangle::BorderStyle::inset, Rectangle::FillBrightness::light,
                Rectangle::FillMode::none);

            // Figure out how much line height we have to work with.
            uint32_t line_height = FontGetLineHeight(FontStyle::medium);

            auto& gameState = getGameState();

            // Draw money
            if (!gameState.park.flags.has(ParkFlag::noMoney))
            {
                const auto& widget = widgets[WIDX_MONEY];
                auto screenCoords = ScreenCoordsXY{ windowPos.x + widget.midX(),
                                                    windowPos.y + widget.midY() - (line_height == 10 ? 5 : 6) };

                auto colour = GetHoverWidgetColour(WIDX_MONEY);
                StringId stringId = gameState.park.cash < 0 ? STR_BOTTOM_TOOLBAR_CASH_NEGATIVE : STR_BOTTOM_TOOLBAR_CASH;
                auto ft = Formatter();
                ft.Add<money64>(gameState.park.cash);
                drawText(rt, screenCoords, stringId, ft, { colour, TextAlignment::centre });
            }

            static constexpr StringId kGuestCountFormats[] = {
                STR_BOTTOM_TOOLBAR_NUM_GUESTS_STABLE,
                STR_BOTTOM_TOOLBAR_NUM_GUESTS_DECREASE,
                STR_BOTTOM_TOOLBAR_NUM_GUESTS_INCREASE,
            };

            static constexpr StringId kGuestCountFormatsSingular[] = {
                STR_BOTTOM_TOOLBAR_NUM_GUESTS_STABLE_SINGULAR,
                STR_BOTTOM_TOOLBAR_NUM_GUESTS_DECREASE_SINGULAR,
                STR_BOTTOM_TOOLBAR_NUM_GUESTS_INCREASE_SINGULAR,
            };

            // Draw guests
            {
                const auto& widget = widgets[WIDX_GUESTS];
                auto screenCoords = ScreenCoordsXY{ windowPos.x + widget.midX(), windowPos.y + widget.midY() - 6 };

                StringId stringId = gameState.park.numGuestsInPark == 1
                    ? kGuestCountFormatsSingular[gameState.park.guestChangeModifier]
                    : kGuestCountFormats[gameState.park.guestChangeModifier];
                auto colour = GetHoverWidgetColour(WIDX_GUESTS);
                auto ft = Formatter();
                ft.Add<uint32_t>(gameState.park.numGuestsInPark);
                drawText(rt, screenCoords, stringId, ft, { colour, TextAlignment::centre });
            }

            // Draw park rating
            {
                const auto& widget = widgets[WIDX_PARK_RATING];
                auto screenCoords = windowPos + ScreenCoordsXY{ widget.left + 11, widget.midY() - 5 };

                DrawParkRating(
                    rt, colours[1].colour, false, screenCoords, std::max(10, ((gameState.park.rating / 4) * 263) / 256));
            }
        }

        void DrawParkRating(RenderTarget& rt, Colour colour, bool blink, const ScreenCoordsXY& coords, uint8_t factor)
        {
            int16_t bar_width = (factor * 114) / 255;
            Rectangle::fillInset(
                rt, { coords + ScreenCoordsXY{ 1, 1 }, coords + ScreenCoordsXY{ 114, 9 } }, colours[0],
                Rectangle::BorderStyle::inset, Rectangle::FillBrightness::light, Rectangle::FillMode::none);
            if (!blink || GameIsPaused() || (gCurrentRealTimeTicks & 8))
            {
                if (bar_width > 2)
                {
                    Rectangle::fillInset(
                        rt, { coords + ScreenCoordsXY{ 2, 2 }, coords + ScreenCoordsXY{ bar_width - 1, 8 } },
                        ColourWithFlags{ colour });
                }
            }

            // Draw thumbs on the sides
            GfxDrawSprite(rt, ImageId(SPR_RATING_LOW), coords - ScreenCoordsXY{ 14, 0 });
            GfxDrawSprite(rt, ImageId(SPR_RATING_HIGH), coords + ScreenCoordsXY{ 114, 0 });
        }

    public:
        ParkInfoPanel()
        {
            setWidgets(kParkInfoPanelWidgets);
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_LEFT_OUTSET:
                case WIDX_MONEY:
                    if (!getGameState().park.flags.has(ParkFlag::noMoney))
                        ContextOpenWindow(WindowClass::finances);
                    break;
                case WIDX_GUESTS:
                    ContextOpenWindowView(WindowView::parkGuests);
                    break;
                case WIDX_PARK_RATING:
                    ContextOpenWindowView(WindowView::parkRating);
                    break;
            }
        }

        StringWithArgs onTooltip(WidgetIndex widgetIndex, StringId fallback) override
        {
            const auto& gameState = getGameState();
            auto ft = Formatter();

            switch (widgetIndex)
            {
                case WIDX_MONEY:
                    ft.Add<money64>(gameState.park.currentProfit);
                    ft.Add<money64>(gameState.park.value);
                    break;
                case WIDX_PARK_RATING:
                    ft.Add<int16_t>(gameState.park.rating);
                    break;
            }
            return { fallback, ft };
        }

        void onPrepareDraw() override
        {
            // Figure out how much line height we have to work with.
            uint32_t line_height = FontGetLineHeight(FontStyle::medium);

            // Reset dimensions as appropriate -- in case we're switching languages.
            height = line_height * 2 + 12;
            windowPos.y = ContextGetHeight() - height;

            // Change height of widgets in accordance with line height.
            // TODO: replace offset to full width with window size/position
            widgets[WIDX_LEFT_OUTSET].bottom = line_height * 3 + 3;
            widgets[WIDX_LEFT_INSET].bottom = line_height * 3 + 1;

            // Reposition left widgets in accordance with line height... depending on whether there is money in play.
            if (getGameState().park.flags.has(ParkFlag::noMoney))
            {
                widgets[WIDX_MONEY].setHidden();
                widgets[WIDX_GUESTS].top = 1;
                widgets[WIDX_GUESTS].bottom = line_height + 7;
                widgets[WIDX_PARK_RATING].top = line_height + 8;
                widgets[WIDX_PARK_RATING].bottom = height - 1;
            }
            else
            {
                widgets[WIDX_MONEY].setVisible();
                widgets[WIDX_MONEY].bottom = widgets[WIDX_MONEY].top + line_height;
                widgets[WIDX_GUESTS].top = widgets[WIDX_MONEY].bottom + 1;
                widgets[WIDX_GUESTS].bottom = widgets[WIDX_GUESTS].top + line_height;
                widgets[WIDX_PARK_RATING].top = widgets[WIDX_GUESTS].bottom - 1;
                widgets[WIDX_PARK_RATING].bottom = height - 1;
            }
        }

        void onDraw(RenderTarget& rt) override
        {
            const auto& leftWidget = widgets[WIDX_LEFT_OUTSET];

            // Draw panel grey backgrounds
            auto leftTop = windowPos + ScreenCoordsXY{ leftWidget.left, leftWidget.top };
            auto rightBottom = windowPos + ScreenCoordsXY{ leftWidget.right, leftWidget.bottom };
            Rectangle::filter(rt, { leftTop, rightBottom }, FilterPaletteID::palette51);

            drawWidgets(rt);

            DrawLeftPanel(rt);
        }

        CursorID onCursor(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords, CursorID cursorId) override
        {
            switch (widgetIndex)
            {
                case WIDX_MONEY:
                case WIDX_GUESTS:
                case WIDX_PARK_RATING:
                    gTooltipCloseTimeout = gCurrentRealTimeTicks + 2000;
                    break;
            }
            return cursorId;
        }
    };

    WindowBase* parkInfoPanelOpen()
    {
        // Figure out how much line height we have to work with.
        uint32_t lineHeight = FontGetLineHeight(FontStyle::medium);
        int32_t panelHeight = lineHeight * 2 + 12;

        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->Create<ParkInfoPanel>(
            WindowClass::parkInfoPanel, ScreenCoordsXY(0, ContextGetHeight() - panelHeight), { kPanelWidth, panelHeight },
            { WindowFlag::stickToFront, WindowFlag::transparent, WindowFlag::noBackground, WindowFlag::noTitleBar });

        return window;
    }
} // namespace OpenRCT2::Ui::Windows
