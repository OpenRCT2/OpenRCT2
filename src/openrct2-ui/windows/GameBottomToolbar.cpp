/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Theme.h"

#include <algorithm>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/config/Config.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/entity/Guest.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/management/Finance.h>
#include <openrct2/management/NewsItem.h>
#include <openrct2/sprites.h>
#include <openrct2/world/Climate.h>
#include <openrct2/world/Park.h>

// clang-format off
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

static Widget window_game_bottom_toolbar_widgets[] =
{
    MakeWidget({  0,  0}, {142, 34}, WindowWidgetType::ImgBtn,      WindowColour::Primary                                                     ), // Left outset panel
    MakeWidget({  2,  2}, {138, 30}, WindowWidgetType::ImgBtn,      WindowColour::Primary                                                     ), // Left inset panel
    MakeWidget({  2,  1}, {138, 12}, WindowWidgetType::FlatBtn,     WindowColour::Primary , 0xFFFFFFFF, STR_PROFIT_PER_WEEK_AND_PARK_VALUE_TIP), // Money window
    MakeWidget({  2, 11}, {138, 12}, WindowWidgetType::FlatBtn,     WindowColour::Primary                                                     ), // Guests window
    MakeWidget({  2, 21}, {138, 11}, WindowWidgetType::FlatBtn,     WindowColour::Primary , 0xFFFFFFFF, STR_PARK_RATING_TIP                   ), // Park rating window

    MakeWidget({142,  0}, {356, 34}, WindowWidgetType::ImgBtn,      WindowColour::Tertiary                                                    ), // Middle outset panel
    MakeWidget({144,  2}, {352, 30}, WindowWidgetType::FlatBtn,     WindowColour::Tertiary                                                    ), // Middle inset panel
    MakeWidget({147,  5}, { 24, 24}, WindowWidgetType::FlatBtn,     WindowColour::Tertiary, 0xFFFFFFFF, STR_SHOW_SUBJECT_TIP                  ), // Associated news item window
    MakeWidget({469,  5}, { 24, 24}, WindowWidgetType::FlatBtn,     WindowColour::Tertiary, ImageId(SPR_LOCATE), STR_LOCATE_SUBJECT_TIP                ), // Scroll to news item target

    MakeWidget({498,  0}, {142, 34}, WindowWidgetType::ImgBtn,      WindowColour::Primary                                                     ), // Right outset panel
    MakeWidget({500,  2}, {138, 30}, WindowWidgetType::ImgBtn,      WindowColour::Primary                                                     ), // Right inset panel
    MakeWidget({500,  2}, {138, 12}, WindowWidgetType::FlatBtn,     WindowColour::Primary                                                     ), // Date
    WIDGETS_END,
};

uint8_t gToolbarDirtyFlags;

class GameBottomToolbar final : public Window
{
private:

    void DrawLeftPanel(DrawPixelInfo &dpi)
    {
        const auto topLeft = windowPos
            + ScreenCoordsXY{ window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].left + 1,
                              window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].top + 1 };
        const auto bottomRight = windowPos
            + ScreenCoordsXY{ window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].right - 1,
                              window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].bottom - 1 };
        // Draw green inset rectangle on panel
        GfxFillRectInset(&dpi, { topLeft, bottomRight }, colours[1], INSET_RECT_F_30);

        // Figure out how much line height we have to work with.
        uint32_t line_height = FontGetLineHeight(FontStyle::Medium);

        // Draw money
        if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
        {
            Widget widget = window_game_bottom_toolbar_widgets[WIDX_MONEY];
            auto screenCoords = ScreenCoordsXY{ windowPos.x + widget.midX(),
                                                windowPos.y + widget.midY() - (line_height == 10 ? 5 : 6) };

            colour_t colour
                = (gHoverWidget.window_classification == WindowClass::BottomToolbar && gHoverWidget.widget_index == WIDX_MONEY
                       ? COLOUR_WHITE
                       : NOT_TRANSLUCENT(colours[0]));
            StringId stringId = gCash < 0 ? STR_BOTTOM_TOOLBAR_CASH_NEGATIVE : STR_BOTTOM_TOOLBAR_CASH;
            auto ft = Formatter();
            ft.Add<money64>(gCash);
            DrawTextBasic(dpi, screenCoords, stringId, ft, { colour, TextAlignment::CENTRE });
        }

        static constexpr const StringId _guestCountFormats[] = {
            STR_BOTTOM_TOOLBAR_NUM_GUESTS_STABLE,
            STR_BOTTOM_TOOLBAR_NUM_GUESTS_DECREASE,
            STR_BOTTOM_TOOLBAR_NUM_GUESTS_INCREASE,
        };

        static constexpr const StringId _guestCountFormatsSingular[] = {
            STR_BOTTOM_TOOLBAR_NUM_GUESTS_STABLE_SINGULAR,
            STR_BOTTOM_TOOLBAR_NUM_GUESTS_DECREASE_SINGULAR,
            STR_BOTTOM_TOOLBAR_NUM_GUESTS_INCREASE_SINGULAR,
        };

        // Draw guests
        {
            Widget widget = window_game_bottom_toolbar_widgets[WIDX_GUESTS];
            auto screenCoords = ScreenCoordsXY{ windowPos.x + widget.midX(), windowPos.y + widget.midY() - 6 };

            StringId stringId = gNumGuestsInPark == 1 ? _guestCountFormatsSingular[gGuestChangeModifier]
                                                      : _guestCountFormats[gGuestChangeModifier];
            colour_t colour
                = (gHoverWidget.window_classification == WindowClass::BottomToolbar && gHoverWidget.widget_index == WIDX_GUESTS
                       ? COLOUR_WHITE
                       : NOT_TRANSLUCENT(colours[0]));
            auto ft = Formatter();
            ft.Add<uint32_t>(gNumGuestsInPark);
            DrawTextBasic(dpi, screenCoords, stringId, ft, { colour, TextAlignment::CENTRE });
        }

        // Draw park rating
        {
            Widget widget = window_game_bottom_toolbar_widgets[WIDX_PARK_RATING];
            auto screenCoords = windowPos + ScreenCoordsXY{ widget.left + 11, widget.midY() - 5 };

            DrawParkRating(
                dpi, colours[3], screenCoords, std::max(10, ((gParkRating / 4) * 263) / 256));
        }
    }

    /**
     *
     *  rct2: 0x0066C76C
     */
    void DrawParkRating(DrawPixelInfo &dpi, int32_t colour, const ScreenCoordsXY& coords, uint8_t factor)
    {
        int16_t bar_width;

        bar_width = (factor * 114) / 255;
        GfxFillRectInset(
            &dpi, { coords + ScreenCoordsXY{ 1, 1 }, coords + ScreenCoordsXY{ 114, 9 } }, colours[1], INSET_RECT_F_30);
        if (!(colour & BAR_BLINK) || GameIsPaused() || (gCurrentRealTimeTicks & 8))
        {
            if (bar_width > 2)
            {
                GfxFillRectInset(&dpi, { coords + ScreenCoordsXY{ 2, 2 }, coords + ScreenCoordsXY{ bar_width - 1, 8 } }, colour, 0);
            }
        }

        // Draw thumbs on the sides
        GfxDrawSprite(&dpi, ImageId(SPR_RATING_LOW), coords - ScreenCoordsXY{ 14, 0 });
        GfxDrawSprite(&dpi, ImageId(SPR_RATING_HIGH), coords + ScreenCoordsXY{ 114, 0 });
    }

    void DrawRightPanel(DrawPixelInfo &dpi)
    {
        const auto topLeft = windowPos
            + ScreenCoordsXY{ window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].left + 1,
                              window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].top + 1 };
        const auto bottomRight = windowPos
            + ScreenCoordsXY{ window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].right - 1,
                              window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].bottom - 1 };
        // Draw green inset rectangle on panel
        GfxFillRectInset(&dpi, { topLeft, bottomRight }, colours[1], INSET_RECT_F_30);

        auto screenCoords = ScreenCoordsXY{ (window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].left
                                             + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].right)
                                                    / 2
                                                + windowPos.x,
                                            window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].top + windowPos.y + 2 };

        // Date
        auto& date = GetDate();
        int32_t year = date.GetYear() + 1;
        int32_t month = date.GetMonth();
        int32_t day = date.GetDay();

        colour_t colour
            = (gHoverWidget.window_classification == WindowClass::BottomToolbar && gHoverWidget.widget_index == WIDX_DATE
                   ? COLOUR_WHITE
                   : NOT_TRANSLUCENT(colours[0]));
        StringId stringId = DateFormatStringFormatIds[gConfigGeneral.DateFormat];
        auto ft = Formatter();
        ft.Add<StringId>(DateDayNames[day]);
        ft.Add<int16_t>(month);
        ft.Add<int16_t>(year);
        DrawTextBasic(dpi, screenCoords, stringId, ft, { colour, TextAlignment::CENTRE });

        // Figure out how much line height we have to work with.
        uint32_t line_height = FontGetLineHeight(FontStyle::Medium);

        // Temperature
        screenCoords = { windowPos.x + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].left + 15,
                         static_cast<int32_t>(screenCoords.y + line_height + 1) };

        int32_t temperature = gClimateCurrent.Temperature;
        StringId format = STR_CELSIUS_VALUE;
        if (gConfigGeneral.TemperatureFormat == TemperatureUnit::Fahrenheit)
        {
            temperature = ClimateCelsiusToFahrenheit(temperature);
            format = STR_FAHRENHEIT_VALUE;
        }
        ft = Formatter();
        ft.Add<int16_t>(temperature);
        DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ 0, 6 }, format, ft);
        screenCoords.x += 30;

        // Current weather
        auto currentWeatherSpriteId = ClimateGetWeatherSpriteId(gClimateCurrent);
        GfxDrawSprite(&dpi, ImageId(currentWeatherSpriteId), screenCoords);

        // Next weather
        auto nextWeatherSpriteId = ClimateGetWeatherSpriteId(gClimateNext);
        if (currentWeatherSpriteId != nextWeatherSpriteId)
        {
            if (gClimateUpdateTimer < 960)
            {
                GfxDrawSprite(&dpi, ImageId(SPR_NEXT_WEATHER), screenCoords + ScreenCoordsXY{ 27, 5 });
                GfxDrawSprite(&dpi, ImageId(nextWeatherSpriteId), screenCoords + ScreenCoordsXY{ 40, 0 });
            }
        }
    }

    /**
     *
     *  rct2: 0x0066BFA5
     */
    void DrawNewsItem(DrawPixelInfo &dpi)
    {
        int32_t itemWidth;
        News::Item* newsItem;
        Widget* middleOutsetWidget;

        middleOutsetWidget = &window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET];
        newsItem = News::GetItem(0);

        // Current news item
        GfxFillRectInset(
        &dpi,

        { windowPos + ScreenCoordsXY{ middleOutsetWidget->left + 1, middleOutsetWidget->top + 1 },
        windowPos + ScreenCoordsXY{ middleOutsetWidget->right - 1, middleOutsetWidget->bottom - 1 } },
        colours[2], INSET_RECT_F_30);

        // Text
        auto screenCoords = windowPos + ScreenCoordsXY{ middleOutsetWidget->midX(), middleOutsetWidget->top + 11 };
        itemWidth = middleOutsetWidget->width() - 62;
        DrawNewsTicker(
        dpi, screenCoords, itemWidth, COLOUR_BRIGHT_GREEN, STR_BOTTOM_TOOLBAR_NEWS_TEXT, newsItem->Text, newsItem->Ticks);

        screenCoords = windowPos
        + ScreenCoordsXY{ window_game_bottom_toolbar_widgets[WIDX_NEWS_SUBJECT].left,
                      window_game_bottom_toolbar_widgets[WIDX_NEWS_SUBJECT].top };
        switch (newsItem->Type)
        {
            case News::ItemType::Ride:
                GfxDrawSprite(&dpi, ImageId(SPR_RIDE), screenCoords);
                break;
            case News::ItemType::PeepOnRide:
            case News::ItemType::Peep:
            {
                if (newsItem->HasButton())
                    break;

                DrawPixelInfo cliped_dpi;
                if (!ClipDrawPixelInfo(&cliped_dpi, &dpi, screenCoords + ScreenCoordsXY{ 1, 1 }, 22, 22))
                {
                    break;
                }

                auto peep = TryGetEntity<Peep>(EntityId::FromUnderlying(newsItem->Assoc));
                if (peep == nullptr)
                    return;

                auto clipCoords = ScreenCoordsXY{ 10, 19 };
                auto* staff = peep->As<Staff>();
                if (staff != nullptr && staff->AssignedStaffType == StaffType::Entertainer)
                {
                    clipCoords.y += 3;
                }

                uint32_t image_id_base = GetPeepAnimation(peep->SpriteType).base_image;
                image_id_base += frame_no & 0xFFFFFFFC;
                image_id_base++;

                auto image_id = ImageId(image_id_base, peep->TshirtColour, peep->TrousersColour);
                GfxDrawSprite(&cliped_dpi, image_id, clipCoords);

                auto* guest = peep->As<Guest>();
                if (guest != nullptr)
                {
                    if (image_id_base >= 0x2A1D && image_id_base < 0x2A3D)
                    {
                        GfxDrawSprite(&cliped_dpi, ImageId(image_id_base + 32, guest->BalloonColour), clipCoords);
                    }
                    else if (image_id_base >= 0x2BBD && image_id_base < 0x2BDD)
                    {
                        GfxDrawSprite(&cliped_dpi, ImageId(image_id_base + 32, guest->UmbrellaColour), clipCoords);
                    }
                    else if (image_id_base >= 0x29DD && image_id_base < 0x29FD)
                    {
                        GfxDrawSprite(&cliped_dpi, ImageId(image_id_base + 32, guest->HatColour), clipCoords);
                    }
                }
                break;
            }
            case News::ItemType::Money:
            case News::ItemType::Campaign:
                GfxDrawSprite(&dpi, ImageId(SPR_FINANCE), screenCoords);
                break;
            case News::ItemType::Research:
                GfxDrawSprite(&dpi, ImageId(newsItem->Assoc < 0x10000 ? SPR_NEW_SCENERY : SPR_NEW_RIDE), screenCoords);
                break;
            case News::ItemType::Peeps:
                GfxDrawSprite(&dpi, ImageId(SPR_GUESTS), screenCoords);
                break;
            case News::ItemType::Award:
                GfxDrawSprite(&dpi, ImageId(SPR_AWARD), screenCoords);
                break;
            case News::ItemType::Graph:
                GfxDrawSprite(&dpi, ImageId(SPR_GRAPH), screenCoords);
                break;
            case News::ItemType::Null:
            case News::ItemType::Blank:
            case News::ItemType::Count:
                break;
        }
    }

    void DrawMiddlePanel(DrawPixelInfo &dpi)
    {
        Widget* middleOutsetWidget = &window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET];

        GfxFillRectInset(
            &dpi,
            { windowPos + ScreenCoordsXY{ middleOutsetWidget->left + 1, middleOutsetWidget->top + 1 },
              windowPos + ScreenCoordsXY{ middleOutsetWidget->right - 1, middleOutsetWidget->bottom - 1 } },
            colours[1], INSET_RECT_F_30);

        // Figure out how much line height we have to work with.
        uint32_t line_height = FontGetLineHeight(FontStyle::Medium);

        ScreenCoordsXY middleWidgetCoords(
            windowPos.x + middleOutsetWidget->midX(), windowPos.y + middleOutsetWidget->top + line_height + 1);
        int32_t panelWidth = middleOutsetWidget->width() - 62;

        // Check if there is a map tooltip to draw
        StringId stringId;
        auto ft = GetMapTooltip();
        std::memcpy(&stringId, ft.Data(), sizeof(StringId));
        if (stringId == STR_NONE)
        {
            DrawTextWrapped(
                dpi, middleWidgetCoords, panelWidth, STR_TITLE_SEQUENCE_OPENRCT2, ft, { colours[0], TextAlignment::CENTRE });
        }
        else
        {
            // Show tooltip in bottom toolbar
            DrawTextWrapped(dpi, middleWidgetCoords, panelWidth, STR_STRINGID, ft, { colours[0], TextAlignment::CENTRE });
        }
    }

    /**
     *
     *  rct2: 0x0066C6F2
     */
    void InvalidateDirtyWidgets()
    {
        if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_MONEY)
        {
            gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_MONEY;
            WidgetInvalidate(*this, WIDX_LEFT_INSET);
        }

        if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_DATE)
        {
            gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_DATE;
            WidgetInvalidate(*this, WIDX_RIGHT_INSET);
        }

        if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_PEEP_COUNT)
        {
            gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_PEEP_COUNT;
            WidgetInvalidate(*this, WIDX_LEFT_INSET);
        }

        if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_CLIMATE)
        {
            gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_CLIMATE;
            WidgetInvalidate(*this, WIDX_RIGHT_INSET);
        }

        if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_PARK_RATING)
        {
            gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_PARK_RATING;
            WidgetInvalidate(*this, WIDX_LEFT_INSET);
        }
    }


public:
    /**
     *
     *  rct2: 0x0066C588
     */
    void OnMouseUp(WidgetIndex widgetIndex) override
    {

        News::Item* newsItem;

        switch (widgetIndex)
        {
            case WIDX_LEFT_OUTSET:
            case WIDX_MONEY:
                if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
                    ContextOpenWindow(WindowClass::Finances);
                break;
            case WIDX_GUESTS:
                ContextOpenWindowView(WV_PARK_GUESTS);
                break;
            case WIDX_PARK_RATING:
                ContextOpenWindowView(WV_PARK_RATING);
                break;
            case WIDX_MIDDLE_INSET:
                if (News::IsQueueEmpty())
                {
                    ContextOpenWindow(WindowClass::RecentNews);
                }
                else
                {
                    News::CloseCurrentItem();
                }
                break;
            case WIDX_NEWS_SUBJECT:
                newsItem = News::GetItem(0);
                News::OpenSubject(newsItem->Type, newsItem->Assoc);
                break;
            case WIDX_NEWS_LOCATE:
                if (News::IsQueueEmpty())
                    break;

                {
                    newsItem = News::GetItem(0);

                    auto subjectLoc = News::GetSubjectLocation(newsItem->Type, newsItem->Assoc);

                    if (!subjectLoc.has_value())
                        break;

                    WindowBase* mainWindow = WindowGetMain();
                    if (mainWindow != nullptr)
                        WindowScrollToLocation(*mainWindow, subjectLoc.value());
                }
                break;
            case WIDX_RIGHT_OUTSET:
            case WIDX_DATE:
                ContextOpenWindow(WindowClass::RecentNews);
                break;
            default:
            break;
        }
    }

    OpenRCT2String OnTooltip(WidgetIndex widgetIndex, StringId fallback) override
    {
        auto ft = Formatter();

            switch (widgetIndex)
            {
                case WIDX_MONEY:
                    ft.Add<money64>(gCurrentProfit);
                    ft.Add<money64>(gParkValue);
                    break;
                case WIDX_PARK_RATING:
                    ft.Add<int16_t>(gParkRating);
                    break;
                default:
                break;
            }
        return { fallback, ft };
    }

    /**
     *
     *  rct2: 0x0066BBA0
     */
    void Invalidate()
    {
        // Figure out how much line height we have to work with.
        uint32_t line_height = FontGetLineHeight(FontStyle::Medium);

        // Reset dimensions as appropriate -- in case we're switching languages.
        height = line_height * 2 + 12;
        windowPos.y = ContextGetHeight() - height;

        // Change height of widgets in accordance with line height.
        widgets[WIDX_LEFT_OUTSET].bottom = widgets[WIDX_MIDDLE_OUTSET].bottom = widgets[WIDX_RIGHT_OUTSET].bottom
            = line_height * 3 + 3;
        widgets[WIDX_LEFT_INSET].bottom = widgets[WIDX_MIDDLE_INSET].bottom = widgets[WIDX_RIGHT_INSET].bottom
            = line_height * 3 + 1;

        // Reposition left widgets in accordance with line height... depending on whether there is money in play.
        if (gParkFlags & PARK_FLAGS_NO_MONEY)
        {
            widgets[WIDX_MONEY].type = WindowWidgetType::Empty;
            widgets[WIDX_GUESTS].top = 1;
            widgets[WIDX_GUESTS].bottom = line_height + 7;
            widgets[WIDX_PARK_RATING].top = line_height + 8;
            widgets[WIDX_PARK_RATING].bottom = height - 1;
        }
        else
        {
            widgets[WIDX_MONEY].type = WindowWidgetType::FlatBtn;
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
        window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].right = x;
        x -= 2;
        window_game_bottom_toolbar_widgets[WIDX_RIGHT_INSET].right = x;
        x -= 137;
        window_game_bottom_toolbar_widgets[WIDX_RIGHT_INSET].left = x;
        x -= 2;
        window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].left = x;
        x--;
        window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].right = x;
        x -= 2;
        window_game_bottom_toolbar_widgets[WIDX_MIDDLE_INSET].right = x;
        x -= 3;
        window_game_bottom_toolbar_widgets[WIDX_NEWS_LOCATE].right = x;
        x -= 23;
        window_game_bottom_toolbar_widgets[WIDX_NEWS_LOCATE].left = x;
        window_game_bottom_toolbar_widgets[WIDX_DATE].left = window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].left + 2;
        window_game_bottom_toolbar_widgets[WIDX_DATE].right = window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].right - 2;

        window_game_bottom_toolbar_widgets[WIDX_LEFT_INSET].type = WindowWidgetType::Empty;
        window_game_bottom_toolbar_widgets[WIDX_RIGHT_INSET].type = WindowWidgetType::Empty;

        if (News::IsQueueEmpty())
        {
            if (!(ThemeGetFlags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR))
            {
                window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].type = WindowWidgetType::Empty;
                window_game_bottom_toolbar_widgets[WIDX_MIDDLE_INSET].type = WindowWidgetType::Empty;
                window_game_bottom_toolbar_widgets[WIDX_NEWS_SUBJECT].type = WindowWidgetType::Empty;
                window_game_bottom_toolbar_widgets[WIDX_NEWS_LOCATE].type = WindowWidgetType::Empty;
            }
            else
            {
                window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].type = WindowWidgetType::ImgBtn;
                window_game_bottom_toolbar_widgets[WIDX_MIDDLE_INSET].type = WindowWidgetType::FlatBtn;
                window_game_bottom_toolbar_widgets[WIDX_NEWS_SUBJECT].type = WindowWidgetType::Empty;
                window_game_bottom_toolbar_widgets[WIDX_NEWS_LOCATE].type = WindowWidgetType::Empty;
                window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].colour = 0;
                window_game_bottom_toolbar_widgets[WIDX_MIDDLE_INSET].colour = 0;
            }
        }
        else
        {
            News::Item* newsItem = News::GetItem(0);
            window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].type = WindowWidgetType::ImgBtn;
            window_game_bottom_toolbar_widgets[WIDX_MIDDLE_INSET].type = WindowWidgetType::FlatBtn;
            window_game_bottom_toolbar_widgets[WIDX_NEWS_SUBJECT].type = WindowWidgetType::FlatBtn;
            window_game_bottom_toolbar_widgets[WIDX_NEWS_LOCATE].type = WindowWidgetType::FlatBtn;
            window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].colour = 2;
            window_game_bottom_toolbar_widgets[WIDX_MIDDLE_INSET].colour = 2;
            disabled_widgets &= ~(1uLL << WIDX_NEWS_SUBJECT);
            disabled_widgets &= ~(1uLL << WIDX_NEWS_LOCATE);

            // Find out if the news item is no longer valid
            auto subjectLoc = News::GetSubjectLocation(newsItem->Type, newsItem->Assoc);

            if (!subjectLoc.has_value())
                disabled_widgets |= (1uLL << WIDX_NEWS_LOCATE);

            if (!(newsItem->TypeHasSubject()))
            {
                disabled_widgets |= (1uLL << WIDX_NEWS_SUBJECT);
                window_game_bottom_toolbar_widgets[WIDX_NEWS_SUBJECT].type = WindowWidgetType::Empty;
            }

            if (newsItem->HasButton())
            {
                disabled_widgets |= (1uLL << WIDX_NEWS_SUBJECT);
                disabled_widgets |= (1uLL << WIDX_NEWS_LOCATE);
            }
        }
    }

    /**
     *
     *  rct2: 0x0066BC87
     */
    void OnDraw(DrawPixelInfo &dpi) override
    {
        auto leftWidget = window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET];
        auto rightWidget = window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET];
        auto middleWidget = window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET];

        // Draw panel grey backgrounds
        auto leftTop = windowPos + ScreenCoordsXY{ leftWidget.left, leftWidget.top };
        auto rightBottom = windowPos + ScreenCoordsXY{ leftWidget.right, leftWidget.bottom };
        GfxFilterRect(&dpi, { leftTop, rightBottom }, FilterPaletteID::Palette51);

        leftTop = windowPos + ScreenCoordsXY{ rightWidget.left, rightWidget.top };
        rightBottom = windowPos + ScreenCoordsXY{ rightWidget.right, rightWidget.bottom };
        GfxFilterRect(&dpi, { leftTop, rightBottom }, FilterPaletteID::Palette51);

        if (ThemeGetFlags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR)
        {
            // Draw grey background
            leftTop = windowPos + ScreenCoordsXY{ middleWidget.left, middleWidget.top };
            rightBottom = windowPos + ScreenCoordsXY{ middleWidget.right, middleWidget.bottom };
            GfxFilterRect(&dpi, { leftTop, rightBottom }, FilterPaletteID::Palette51);
        }

        WindowDrawWidgets(*this, &dpi);

        DrawLeftPanel(dpi);
        DrawRightPanel(dpi);

        if (!News::IsQueueEmpty())
        {
            DrawNewsItem(dpi);
        }
        else if (ThemeGetFlags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR)
        {
            DrawMiddlePanel(dpi);
        }
    }

    /**
     *
     *  rct2: 0x0066C6D8
     */
    void OnUpdate() override
    {
        frame_no++;
        if (frame_no >= 24)
            frame_no = 0;

        InvalidateDirtyWidgets();
    }

    /**
     *
     *  rct2: 0x0066C644
     */
    CursorID OnCursor(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords, CursorID cursorId) override
    {
        switch (widgetIndex)
        {
            case WIDX_MONEY:
            case WIDX_GUESTS:
            case WIDX_PARK_RATING:
            case WIDX_DATE:
                gTooltipTimeout = 2000;
                break;
        }
        return cursorId;
    }

    /**
     *
     *  rct2: 0x0066C6F2
     */
    void OnUnknown5() override
    {
        InvalidateDirtyWidgets();
    }
};

/**
 * Creates the main game bottom toolbar window.
 *  rct2: 0x0066B52F (part of 0x0066B3E8)
 */
WindowBase* WindowGameBottomToolbarOpen()
{
    int32_t screenWidth = ContextGetWidth();
    int32_t screenHeight = ContextGetHeight();

    // Figure out how much line height we have to work with.
    uint32_t line_height = FontGetLineHeight(FontStyle::Medium);
    uint32_t toolbar_height = line_height * 2 + 12;

    GameBottomToolbar* window = WindowCreate<GameBottomToolbar>(
        WindowClass::BottomToolbar,
        ScreenCoordsXY(0, screenHeight - toolbar_height),
        screenWidth,
        toolbar_height,
        WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_BACKGROUND);

    window->widgets = window_game_bottom_toolbar_widgets;

    window->frame_no = 0;
    WindowInitScrollWidgets(*window);

    // Reset the middle widget to not show by default.
    // If it is required to be shown news_update will reshow it.
    window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].type = WindowWidgetType::Empty;

    return window;
}

/**
 *
 *  rct2: 0x0066BB79
 */
void WindowGameBottomToolbarInvalidateNewsItem()
{
    if (gScreenFlags == SCREEN_FLAGS_PLAYING)
    {
        WidgetInvalidateByClass(WindowClass::BottomToolbar, WIDX_MIDDLE_OUTSET);
    }
}

