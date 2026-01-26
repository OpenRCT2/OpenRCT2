/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Theme.h"

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/entity/Guest.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/interface/ColourWithFlags.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.Date.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/management/Finance.h>
#include <openrct2/management/NewsItem.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/PeepAnimationsObject.h>
#include <openrct2/peep/PeepSpriteIds.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Park.h>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
    enum WindowGameBottomToolbarWidgetIdx
    {
        WIDX_LEFT_OUTSET,
        WIDX_LEFT_INSET,
        WIDX_MONEY,
        WIDX_GUESTS,
        WIDX_PARK_RATING,

        WIDX_MIDDLE_OUTSET,
        WIDX_MIDDLE_INSET,
        WIDX_NEWS_SUBJECT,
        WIDX_NEWS_LOCATE,

        WIDX_RIGHT_OUTSET,
        WIDX_RIGHT_INSET,
        WIDX_DATE
    };

    // clang-format off
    static constexpr Widget window_game_bottom_toolbar_widgets[] =
    {
        makeWidget({  0,  0}, {142, 34}, WidgetType::imgBtn,      WindowColour::primary                                                     ), // Left outset panel
        makeWidget({  2,  2}, {138, 30}, WidgetType::imgBtn,      WindowColour::primary                                                     ), // Left inset panel
        makeWidget({  2,  1}, {138, 12}, WidgetType::hiddenButton,WindowColour::primary , 0xFFFFFFFF, STR_PROFIT_PER_WEEK_AND_PARK_VALUE_TIP), // Money window
        makeWidget({  2, 11}, {138, 12}, WidgetType::hiddenButton,WindowColour::primary                                                     ), // Guests window
        makeWidget({  2, 21}, {138, 11}, WidgetType::hiddenButton,WindowColour::primary , 0xFFFFFFFF, STR_PARK_RATING_TIP                   ), // Park rating window

        makeWidget({142,  0}, {356, 34}, WidgetType::imgBtn,      WindowColour::tertiary                                                    ), // Middle outset panel
        makeWidget({144,  2}, {352, 30}, WidgetType::hiddenButton,WindowColour::tertiary                                                    ), // Middle inset panel
        makeWidget({147,  5}, { 24, 24}, WidgetType::flatBtn,     WindowColour::secondary, 0xFFFFFFFF, STR_SHOW_SUBJECT_TIP                 ), // Associated news item window
        makeWidget({469,  5}, { 24, 24}, WidgetType::flatBtn,     WindowColour::secondary, ImageId(SPR_LOCATE), STR_LOCATE_SUBJECT_TIP      ), // Scroll to news item target

        makeWidget({498,  0}, {142, 34}, WidgetType::imgBtn,      WindowColour::primary                                                     ), // Right outset panel
        makeWidget({500,  2}, {138, 30}, WidgetType::imgBtn,      WindowColour::primary                                                     ), // Right inset panel
        makeWidget({500,  2}, {138, 12}, WidgetType::hiddenButton,WindowColour::primary                                                     ), // Date
    };
    // clang-format on

    uint8_t gToolbarDirtyFlags;

    class GameBottomToolbar final : public Window
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
            if (!(gameState.park.flags & PARK_FLAGS_NO_MONEY))
            {
                const auto& widget = widgets[WIDX_MONEY];
                auto screenCoords = ScreenCoordsXY{ windowPos.x + widget.midX(),
                                                    windowPos.y + widget.midY() - (line_height == 10 ? 5 : 6) };

                auto colour = GetHoverWidgetColour(WIDX_MONEY);
                StringId stringId = gameState.park.cash < 0 ? STR_BOTTOM_TOOLBAR_CASH_NEGATIVE : STR_BOTTOM_TOOLBAR_CASH;
                auto ft = Formatter();
                ft.Add<money64>(gameState.park.cash);
                DrawTextBasic(rt, screenCoords, stringId, ft, { colour, TextAlignment::centre });
            }

            static constexpr StringId _guestCountFormats[] = {
                STR_BOTTOM_TOOLBAR_NUM_GUESTS_STABLE,
                STR_BOTTOM_TOOLBAR_NUM_GUESTS_DECREASE,
                STR_BOTTOM_TOOLBAR_NUM_GUESTS_INCREASE,
            };

            static constexpr StringId _guestCountFormatsSingular[] = {
                STR_BOTTOM_TOOLBAR_NUM_GUESTS_STABLE_SINGULAR,
                STR_BOTTOM_TOOLBAR_NUM_GUESTS_DECREASE_SINGULAR,
                STR_BOTTOM_TOOLBAR_NUM_GUESTS_INCREASE_SINGULAR,
            };

            // Draw guests
            {
                const auto& widget = widgets[WIDX_GUESTS];
                auto screenCoords = ScreenCoordsXY{ windowPos.x + widget.midX(), windowPos.y + widget.midY() - 6 };

                StringId stringId = gameState.park.numGuestsInPark == 1
                    ? _guestCountFormatsSingular[gameState.park.guestChangeModifier]
                    : _guestCountFormats[gameState.park.guestChangeModifier];
                auto colour = GetHoverWidgetColour(WIDX_GUESTS);
                auto ft = Formatter();
                ft.Add<uint32_t>(gameState.park.numGuestsInPark);
                DrawTextBasic(rt, screenCoords, stringId, ft, { colour, TextAlignment::centre });
            }

            // Draw park rating
            {
                const auto& widget = widgets[WIDX_PARK_RATING];
                auto screenCoords = windowPos + ScreenCoordsXY{ widget.left + 11, widget.midY() - 5 };

                DrawParkRating(
                    rt, colours[3].colour, false, screenCoords, std::max(10, ((gameState.park.rating / 4) * 263) / 256));
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
            DrawTextBasic(rt, screenCoords, stringId, ft, { colour, TextAlignment::centre });

            // Figure out how much line height we have to work with.
            uint32_t line_height = FontGetLineHeight(FontStyle::medium);

            // Temperature
            screenCoords = { windowPos.x + rightPanelWidget.left + 15, static_cast<int32_t>(screenCoords.y + line_height + 1) };

            int32_t temperature = getGameState().weatherCurrent.temperature;
            StringId format = STR_CELSIUS_VALUE;
            if (Config::Get().general.temperatureFormat == TemperatureUnit::Fahrenheit)
            {
                temperature = ClimateCelsiusToFahrenheit(temperature);
                format = STR_FAHRENHEIT_VALUE;
            }
            ft = Formatter();
            ft.Add<int16_t>(temperature);
            DrawTextBasic(rt, screenCoords + ScreenCoordsXY{ 0, 6 }, format, ft);
            screenCoords.x += 30;

            // Current weather
            auto currentWeatherSpriteId = ClimateGetWeatherSpriteId(getGameState().weatherCurrent.weatherType);
            GfxDrawSprite(rt, ImageId(currentWeatherSpriteId), screenCoords);

            // Next weather
            auto nextWeatherSpriteId = ClimateGetWeatherSpriteId(getGameState().weatherNext.weatherType);
            if (currentWeatherSpriteId != nextWeatherSpriteId)
            {
                if (getGameState().weatherUpdateTimer < 960)
                {
                    GfxDrawSprite(rt, ImageId(SPR_NEXT_WEATHER), screenCoords + ScreenCoordsXY{ 27, 5 });
                    GfxDrawSprite(rt, ImageId(nextWeatherSpriteId), screenCoords + ScreenCoordsXY{ 40, 0 });
                }
            }
        }

        void DrawNewsItem(RenderTarget& rt)
        {
            const auto& middleOutsetWidget = widgets[WIDX_MIDDLE_OUTSET];
            auto* newsItem = News::GetItem(0);

            // Current news item
            Rectangle::fillInset(
                rt,

                { windowPos + ScreenCoordsXY{ middleOutsetWidget.left + 1, middleOutsetWidget.top + 1 },
                  windowPos + ScreenCoordsXY{ middleOutsetWidget.right - 1, middleOutsetWidget.bottom - 1 } },
                colours[2], Rectangle::BorderStyle::inset, Rectangle::FillBrightness::light, Rectangle::FillMode::none);

            // Text
            auto screenCoords = windowPos + ScreenCoordsXY{ middleOutsetWidget.midX(), middleOutsetWidget.top + 11 };
            int32_t itemWidth = middleOutsetWidget.width() - 63;
            DrawNewsTicker(
                rt, screenCoords, itemWidth, Drawing::Colour::brightGreen, STR_BOTTOM_TOOLBAR_NEWS_TEXT, newsItem->text,
                newsItem->ticks);

            const auto& newsSubjectWidget = widgets[WIDX_NEWS_SUBJECT];
            screenCoords = windowPos + ScreenCoordsXY{ newsSubjectWidget.left, newsSubjectWidget.top };
            switch (newsItem->type)
            {
                case News::ItemType::ride:
                    GfxDrawSprite(rt, ImageId(SPR_RIDE), screenCoords);
                    break;
                case News::ItemType::peepOnRide:
                case News::ItemType::peep:
                {
                    if (newsItem->hasButton())
                        break;

                    RenderTarget clippedRT;
                    if (!ClipRenderTarget(clippedRT, rt, screenCoords + ScreenCoordsXY{ 1, 1 }, 22, 22))
                    {
                        break;
                    }

                    auto peep = getGameState().entities.TryGetEntity<Peep>(EntityId::FromUnderlying(newsItem->assoc));
                    if (peep == nullptr)
                        return;

                    auto clipCoords = ScreenCoordsXY{ 10, 19 };
                    auto* staff = peep->As<Staff>();
                    if (staff != nullptr && staff->isEntertainer())
                    {
                        clipCoords.y += 3;
                    }

                    auto& objManager = GetContext()->GetObjectManager();
                    auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep->AnimationObjectIndex);

                    uint32_t image_id_base = animObj->GetPeepAnimation(peep->AnimationGroup).baseImage;
                    image_id_base += currentFrame & 0xFFFFFFFC;
                    image_id_base++;

                    auto image_id = ImageId(image_id_base, peep->TshirtColour, peep->TrousersColour);
                    GfxDrawSprite(clippedRT, image_id, clipCoords);

                    auto* guest = peep->As<Guest>();
                    if (guest == nullptr)
                        return;

                    // There are only 6 walking frames available for each item,
                    // as well as 1 sprite for sitting and 1 for standing still.
                    auto itemFrame = (currentFrame / 4) % 6;

                    if (guest->AnimationGroup == PeepAnimationGroup::hat)
                    {
                        auto itemOffset = kPeepSpriteHatItemStart + 1;
                        auto imageId = ImageId(itemOffset + itemFrame * 4, guest->HatColour);
                        GfxDrawSprite(clippedRT, imageId, clipCoords);
                        return;
                    }

                    if (guest->AnimationGroup == PeepAnimationGroup::balloon)
                    {
                        auto itemOffset = kPeepSpriteBalloonItemStart + 1;
                        auto imageId = ImageId(itemOffset + itemFrame * 4, guest->BalloonColour);
                        GfxDrawSprite(clippedRT, imageId, clipCoords);
                        return;
                    }

                    if (guest->AnimationGroup == PeepAnimationGroup::umbrella)
                    {
                        auto itemOffset = kPeepSpriteUmbrellaItemStart + 1;
                        auto imageId = ImageId(itemOffset + itemFrame * 4, guest->UmbrellaColour);
                        GfxDrawSprite(clippedRT, imageId, clipCoords);
                        return;
                    }
                    break;
                }
                case News::ItemType::money:
                case News::ItemType::campaign:
                    GfxDrawSprite(rt, ImageId(SPR_FINANCE), screenCoords);
                    break;
                case News::ItemType::research:
                    GfxDrawSprite(rt, ImageId(newsItem->assoc < 0x10000 ? SPR_NEW_SCENERY : SPR_NEW_RIDE), screenCoords);
                    break;
                case News::ItemType::peeps:
                    GfxDrawSprite(rt, ImageId(SPR_GUESTS), screenCoords);
                    break;
                case News::ItemType::award:
                    GfxDrawSprite(rt, ImageId(SPR_AWARD), screenCoords);
                    break;
                case News::ItemType::graph:
                    GfxDrawSprite(rt, ImageId(SPR_GRAPH), screenCoords);
                    break;
                case News::ItemType::null:
                case News::ItemType::blank:
                case News::ItemType::count:
                    break;
            }
        }

        void DrawMiddlePanel(RenderTarget& rt)
        {
            Widget* middleOutsetWidget = &widgets[WIDX_MIDDLE_OUTSET];

            Rectangle::fillInset(
                rt,
                { windowPos + ScreenCoordsXY{ middleOutsetWidget->left + 1, middleOutsetWidget->top + 1 },
                  windowPos + ScreenCoordsXY{ middleOutsetWidget->right - 1, middleOutsetWidget->bottom - 1 } },
                colours[0], Rectangle::BorderStyle::inset, Rectangle::FillBrightness::light, Rectangle::FillMode::none);

            // Figure out how much line height we have to work with.
            uint32_t line_height = FontGetLineHeight(FontStyle::medium);

            ScreenCoordsXY middleWidgetCoords(
                windowPos.x + middleOutsetWidget->midX(), windowPos.y + middleOutsetWidget->top + line_height + 1);
            int32_t panelWidth = middleOutsetWidget->width() - 63;

            // Check if there is a map tooltip to draw
            StringId stringId;
            auto ft = GetMapTooltip();
            std::memcpy(&stringId, ft.Data(), sizeof(StringId));
            if (stringId == kStringIdNone)
            {
                // TODO: this string probably shouldn't be reused for this
                DrawTextWrapped(
                    rt, middleWidgetCoords, panelWidth, STR_TITLE_SEQUENCE_OPENRCT2, ft, { colours[0], TextAlignment::centre });
            }
            else
            {
                // Show tooltip in bottom toolbar
                DrawTextWrapped(rt, middleWidgetCoords, panelWidth, STR_STRINGID, ft, { colours[0], TextAlignment::centre });
            }
        }

        void InvalidateDirtyWidgets()
        {
            if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_MONEY)
            {
                gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_MONEY;
                invalidateWidget(WIDX_LEFT_INSET);
            }

            if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_DATE)
            {
                gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_DATE;
                invalidateWidget(WIDX_RIGHT_INSET);
            }

            if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_PEEP_COUNT)
            {
                gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_PEEP_COUNT;
                invalidateWidget(WIDX_LEFT_INSET);
            }

            if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_CLIMATE)
            {
                gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_CLIMATE;
                invalidateWidget(WIDX_RIGHT_INSET);
            }

            if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_PARK_RATING)
            {
                gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_PARK_RATING;
                invalidateWidget(WIDX_LEFT_INSET);
            }
        }

    public:
        GameBottomToolbar()
        {
            setWidgets(window_game_bottom_toolbar_widgets);

            currentFrame = 0;
            initScrollWidgets();

            // Reset the middle widget to not show by default.
            // If it is required to be shown news_update will reshow it.
            widgets[WIDX_MIDDLE_OUTSET].type = WidgetType::empty;
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            News::Item* newsItem;

            switch (widgetIndex)
            {
                case WIDX_LEFT_OUTSET:
                case WIDX_MONEY:
                    if (!(getGameState().park.flags & PARK_FLAGS_NO_MONEY))
                        ContextOpenWindow(WindowClass::finances);
                    break;
                case WIDX_GUESTS:
                    ContextOpenWindowView(WindowView::parkGuests);
                    break;
                case WIDX_PARK_RATING:
                    ContextOpenWindowView(WindowView::parkRating);
                    break;
                case WIDX_MIDDLE_INSET:
                    if (News::IsQueueEmpty())
                    {
                        ContextOpenWindow(WindowClass::recentNews);
                    }
                    else
                    {
                        News::CloseCurrentItem();
                    }
                    break;
                case WIDX_NEWS_SUBJECT:
                    newsItem = News::GetItem(0);
                    News::OpenSubject(newsItem->type, newsItem->assoc);
                    break;
                case WIDX_NEWS_LOCATE:
                    if (News::IsQueueEmpty())
                        break;

                    {
                        newsItem = News::GetItem(0);

                        auto subjectLoc = News::GetSubjectLocation(newsItem->type, newsItem->assoc);

                        if (!subjectLoc.has_value())
                            break;

                        WindowBase* mainWindow = WindowGetMain();
                        if (mainWindow != nullptr)
                            WindowScrollToLocation(*mainWindow, subjectLoc.value());
                    }
                    break;
                case WIDX_RIGHT_OUTSET:
                case WIDX_DATE:
                    ContextOpenWindow(WindowClass::recentNews);
                    break;
            }
        }

        OpenRCT2String onTooltip(WidgetIndex widgetIndex, StringId fallback) override
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
            widgets[WIDX_LEFT_OUTSET].bottom = widgets[WIDX_MIDDLE_OUTSET].bottom = widgets[WIDX_RIGHT_OUTSET].bottom
                = line_height * 3 + 3;
            widgets[WIDX_LEFT_INSET].bottom = widgets[WIDX_MIDDLE_INSET].bottom = widgets[WIDX_RIGHT_INSET].bottom = line_height
                    * 3
                + 1;

            // Reposition left widgets in accordance with line height... depending on whether there is money in play.
            if (getGameState().park.flags & PARK_FLAGS_NO_MONEY)
            {
                widgets[WIDX_MONEY].type = WidgetType::empty;
                widgets[WIDX_GUESTS].top = 1;
                widgets[WIDX_GUESTS].bottom = line_height + 7;
                widgets[WIDX_PARK_RATING].top = line_height + 8;
                widgets[WIDX_PARK_RATING].bottom = height - 1;
            }
            else
            {
                widgets[WIDX_MONEY].type = WidgetType::hiddenButton;
                widgets[WIDX_MONEY].bottom = widgets[WIDX_MONEY].top + line_height;
                widgets[WIDX_GUESTS].top = widgets[WIDX_MONEY].bottom + 1;
                widgets[WIDX_GUESTS].bottom = widgets[WIDX_GUESTS].top + line_height;
                widgets[WIDX_PARK_RATING].top = widgets[WIDX_GUESTS].bottom - 1;
                widgets[WIDX_PARK_RATING].bottom = height - 1;
            }

            // Reposition right widgets in accordance with line height, too.
            widgets[WIDX_DATE].bottom = line_height + 1;

            // Anchor the middle and right panel to the right
            int32_t x = ContextGetWidth();
            width = x;
            x--;
            widgets[WIDX_RIGHT_OUTSET].right = x;
            x -= 2;
            widgets[WIDX_RIGHT_INSET].right = x;
            x -= 137;
            widgets[WIDX_RIGHT_INSET].left = x;
            x -= 2;
            widgets[WIDX_RIGHT_OUTSET].left = x;
            x--;
            widgets[WIDX_MIDDLE_OUTSET].right = x;
            x -= 2;
            widgets[WIDX_MIDDLE_INSET].right = x;
            x -= 3;
            widgets[WIDX_NEWS_LOCATE].right = x;
            x -= 23;
            widgets[WIDX_NEWS_LOCATE].left = x;
            widgets[WIDX_DATE].left = widgets[WIDX_RIGHT_OUTSET].left + 2;
            widgets[WIDX_DATE].right = widgets[WIDX_RIGHT_OUTSET].right - 2;

            widgets[WIDX_LEFT_INSET].type = WidgetType::empty;
            widgets[WIDX_RIGHT_INSET].type = WidgetType::empty;

            if (News::IsQueueEmpty())
            {
                if (!(ThemeGetFlags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR))
                {
                    widgets[WIDX_MIDDLE_OUTSET].type = WidgetType::empty;
                    widgets[WIDX_MIDDLE_INSET].type = WidgetType::empty;
                    widgets[WIDX_NEWS_SUBJECT].type = WidgetType::empty;
                    widgets[WIDX_NEWS_LOCATE].type = WidgetType::empty;
                }
                else
                {
                    widgets[WIDX_MIDDLE_OUTSET].type = WidgetType::imgBtn;
                    widgets[WIDX_MIDDLE_INSET].type = WidgetType::hiddenButton;
                    widgets[WIDX_NEWS_SUBJECT].type = WidgetType::empty;
                    widgets[WIDX_NEWS_LOCATE].type = WidgetType::empty;
                    widgets[WIDX_MIDDLE_OUTSET].colour = 0;
                    widgets[WIDX_MIDDLE_INSET].colour = 0;
                }
            }
            else
            {
                News::Item* newsItem = News::GetItem(0);
                widgets[WIDX_MIDDLE_OUTSET].type = WidgetType::imgBtn;
                widgets[WIDX_MIDDLE_INSET].type = WidgetType::hiddenButton;
                widgets[WIDX_NEWS_SUBJECT].type = WidgetType::flatBtn;
                widgets[WIDX_NEWS_LOCATE].type = WidgetType::flatBtn;
                widgets[WIDX_MIDDLE_OUTSET].colour = 2;
                widgets[WIDX_MIDDLE_INSET].colour = 2;
                disabledWidgets &= ~(1uLL << WIDX_NEWS_SUBJECT);
                disabledWidgets &= ~(1uLL << WIDX_NEWS_LOCATE);

                // Find out if the news item is no longer valid
                auto subjectLoc = News::GetSubjectLocation(newsItem->type, newsItem->assoc);

                if (!subjectLoc.has_value())
                    disabledWidgets |= (1uLL << WIDX_NEWS_LOCATE);

                if (!(newsItem->typeHasSubject()))
                {
                    disabledWidgets |= (1uLL << WIDX_NEWS_SUBJECT);
                    widgets[WIDX_NEWS_SUBJECT].type = WidgetType::empty;
                }

                if (newsItem->hasButton())
                {
                    disabledWidgets |= (1uLL << WIDX_NEWS_SUBJECT);
                    disabledWidgets |= (1uLL << WIDX_NEWS_LOCATE);
                }
            }
        }

        void onDraw(RenderTarget& rt) override
        {
            const auto& leftWidget = widgets[WIDX_LEFT_OUTSET];
            const auto& rightWidget = widgets[WIDX_RIGHT_OUTSET];
            const auto& middleWidget = widgets[WIDX_MIDDLE_OUTSET];

            // Draw panel grey backgrounds
            auto leftTop = windowPos + ScreenCoordsXY{ leftWidget.left, leftWidget.top };
            auto rightBottom = windowPos + ScreenCoordsXY{ leftWidget.right, leftWidget.bottom };
            Rectangle::filter(rt, { leftTop, rightBottom }, FilterPaletteID::palette51);

            leftTop = windowPos + ScreenCoordsXY{ rightWidget.left, rightWidget.top };
            rightBottom = windowPos + ScreenCoordsXY{ rightWidget.right, rightWidget.bottom };
            Rectangle::filter(rt, { leftTop, rightBottom }, FilterPaletteID::palette51);

            if (ThemeGetFlags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR)
            {
                // Draw grey background
                leftTop = windowPos + ScreenCoordsXY{ middleWidget.left, middleWidget.top };
                rightBottom = windowPos + ScreenCoordsXY{ middleWidget.right, middleWidget.bottom };
                Rectangle::filter(rt, { leftTop, rightBottom }, FilterPaletteID::palette51);
            }

            drawWidgets(rt);

            DrawLeftPanel(rt);
            DrawRightPanel(rt);

            if (!News::IsQueueEmpty())
            {
                DrawNewsItem(rt);
            }
            else if (ThemeGetFlags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR)
            {
                DrawMiddlePanel(rt);
            }
        }

        void onUpdate() override
        {
            currentFrame++;
            if (currentFrame >= 24)
                currentFrame = 0;

            InvalidateDirtyWidgets();
        }

        CursorID onCursor(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords, CursorID cursorId) override
        {
            switch (widgetIndex)
            {
                case WIDX_MONEY:
                case WIDX_GUESTS:
                case WIDX_PARK_RATING:
                case WIDX_DATE:
                    gTooltipCloseTimeout = gCurrentRealTimeTicks + 2000;
                    break;
            }
            return cursorId;
        }

        void onPeriodicUpdate() override
        {
            InvalidateDirtyWidgets();
        }
    };

    /**
     * Creates the main game bottom toolbar window.
     */
    WindowBase* GameBottomToolbarOpen()
    {
        int32_t screenWidth = ContextGetWidth();
        int32_t screenHeight = ContextGetHeight();

        // Figure out how much line height we have to work with.
        uint32_t lineHeight = FontGetLineHeight(FontStyle::medium);
        int32_t toolbarHeight = lineHeight * 2 + 12;

        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->Create<GameBottomToolbar>(
            WindowClass::bottomToolbar, ScreenCoordsXY(0, screenHeight - toolbarHeight), { screenWidth, toolbarHeight },
            { WindowFlag::stickToFront, WindowFlag::transparent, WindowFlag::noBackground, WindowFlag::noTitleBar });

        return window;
    }

    void WindowGameBottomToolbarInvalidateNewsItem()
    {
        if (gLegacyScene == LegacyScene::playing)
        {
            auto* windowMgr = GetWindowManager();
            windowMgr->InvalidateWidgetByClass(WindowClass::bottomToolbar, WIDX_MIDDLE_OUTSET);
        }
    }
} // namespace OpenRCT2::Ui::Windows
