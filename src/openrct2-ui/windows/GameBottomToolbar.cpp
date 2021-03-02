/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/management/Finance.h>
#include <openrct2/management/NewsItem.h>
#include <openrct2/peep/Staff.h>
#include <openrct2/sprites.h>
#include <openrct2/world/Climate.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Sprite.h>

// clang-format off
enum WINDOW_GAME_BOTTOM_TOOLBAR_WIDGET_IDX
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

static rct_widget window_game_bottom_toolbar_widgets[] =
{
    MakeWidget({  0,  0}, {142, 34}, WindowWidgetType::ImgBtn,      WindowColour::Primary                                                     ), // Left outset panel
    MakeWidget({  2,  2}, {138, 30}, WindowWidgetType::ImgBtn,      WindowColour::Primary                                                     ), // Left inset panel
    MakeWidget({  2,  1}, {138, 12}, WindowWidgetType::FlatBtn,     WindowColour::Primary , 0xFFFFFFFF, STR_PROFIT_PER_WEEK_AND_PARK_VALUE_TIP), // Money window
    MakeWidget({  2, 11}, {138, 12}, WindowWidgetType::FlatBtn,     WindowColour::Primary                                                     ), // Guests window
    MakeWidget({  2, 21}, {138, 11}, WindowWidgetType::FlatBtn,     WindowColour::Primary , 0xFFFFFFFF, STR_PARK_RATING_TIP                   ), // Park rating window

    MakeWidget({142,  0}, {356, 34}, WindowWidgetType::ImgBtn,      WindowColour::Tertiary                                                    ), // Middle outset panel
    MakeWidget({144,  2}, {352, 30}, WindowWidgetType::Placeholder, WindowColour::Tertiary                                                    ), // Middle inset panel
    MakeWidget({147,  5}, { 24, 24}, WindowWidgetType::FlatBtn,     WindowColour::Tertiary, 0xFFFFFFFF, STR_SHOW_SUBJECT_TIP                  ), // Associated news item window
    MakeWidget({469,  5}, { 24, 24}, WindowWidgetType::FlatBtn,     WindowColour::Tertiary, SPR_LOCATE, STR_LOCATE_SUBJECT_TIP                ), // Scroll to news item target

    MakeWidget({498,  0}, {142, 34}, WindowWidgetType::ImgBtn,      WindowColour::Primary                                                     ), // Right outset panel
    MakeWidget({500,  2}, {138, 30}, WindowWidgetType::ImgBtn,      WindowColour::Primary                                                     ), // Right inset panel
    MakeWidget({500,  2}, {138, 12}, WindowWidgetType::FlatBtn,     WindowColour::Primary                                                     ), // Date
    { WIDGETS_END },
};

uint8_t gToolbarDirtyFlags;

static void window_game_bottom_toolbar_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static OpenRCT2String window_game_bottom_toolbar_tooltip(rct_window* w, const rct_widgetindex widgetIndex, const rct_string_id fallback);
static void window_game_bottom_toolbar_invalidate(rct_window *w);
static void window_game_bottom_toolbar_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_game_bottom_toolbar_update(rct_window* w);
static void window_game_bottom_toolbar_cursor(rct_window *w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords, CursorID *cursorId);
static void window_game_bottom_toolbar_unknown05(rct_window *w);

static void window_game_bottom_toolbar_draw_left_panel(rct_drawpixelinfo *dpi, rct_window *w);
static void window_game_bottom_toolbar_draw_park_rating(rct_drawpixelinfo *dpi, rct_window *w, int32_t colour, int32_t x, int32_t y, uint8_t factor);
static void window_game_bottom_toolbar_draw_right_panel(rct_drawpixelinfo *dpi, rct_window *w);
static void window_game_bottom_toolbar_draw_news_item(rct_drawpixelinfo *dpi, rct_window *w);
static void window_game_bottom_toolbar_draw_middle_panel(rct_drawpixelinfo *dpi, rct_window *w);

/**
 *
 *  rct2: 0x0097BFDC
 */
static rct_window_event_list window_game_bottom_toolbar_events([](auto& events)
{
    events.mouse_up = &window_game_bottom_toolbar_mouseup;
    events.unknown_05 = &window_game_bottom_toolbar_unknown05;
    events.update = &window_game_bottom_toolbar_update;
    events.tooltip = &window_game_bottom_toolbar_tooltip;
    events.cursor = &window_game_bottom_toolbar_cursor;
    events.invalidate = &window_game_bottom_toolbar_invalidate;
    events.paint = &window_game_bottom_toolbar_paint;
});
// clang-format on

static void window_game_bottom_toolbar_invalidate_dirty_widgets(rct_window* w);

/**
 * Creates the main game bottom toolbar window.
 *  rct2: 0x0066B52F (part of 0x0066B3E8)
 */
rct_window* window_game_bottom_toolbar_open()
{
    int32_t screenWidth = context_get_width();
    int32_t screenHeight = context_get_height();

    // Figure out how much line height we have to work with.
    uint32_t line_height = font_get_line_height(FontSpriteBase::MEDIUM);
    uint32_t toolbar_height = line_height * 2 + 12;

    rct_window* window = WindowCreate(
        ScreenCoordsXY(0, screenHeight - toolbar_height), screenWidth, toolbar_height, &window_game_bottom_toolbar_events,
        WC_BOTTOM_TOOLBAR, WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_BACKGROUND);
    window->widgets = window_game_bottom_toolbar_widgets;
    window->enabled_widgets |= (1 << WIDX_LEFT_OUTSET) | (1 << WIDX_MONEY) | (1 << WIDX_GUESTS) | (1 << WIDX_PARK_RATING)
        | (1 << WIDX_MIDDLE_OUTSET) | (1 << WIDX_MIDDLE_INSET) | (1 << WIDX_NEWS_SUBJECT) | (1 << WIDX_NEWS_LOCATE)
        | (1 << WIDX_RIGHT_OUTSET) | (1 << WIDX_DATE);

    window->frame_no = 0;
    WindowInitScrollWidgets(window);

    // Reset the middle widget to not show by default.
    // If it is required to be shown news_update will reshow it.
    window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].type = WindowWidgetType::Empty;

    return window;
}

/**
 *
 *  rct2: 0x0066C588
 */
static void window_game_bottom_toolbar_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    News::Item* newsItem;

    switch (widgetIndex)
    {
        case WIDX_LEFT_OUTSET:
        case WIDX_MONEY:
            if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
                context_open_window(WC_FINANCES);
            break;
        case WIDX_GUESTS:
            context_open_window_view(WV_PARK_GUESTS);
            break;
        case WIDX_PARK_RATING:
            context_open_window_view(WV_PARK_RATING);
            break;
        case WIDX_MIDDLE_INSET:
            if (News::IsQueueEmpty())
            {
                context_open_window(WC_RECENT_NEWS);
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

                if (subjectLoc == std::nullopt)
                    break;

                rct_window* mainWindow = window_get_main();
                if (mainWindow != nullptr)
                    window_scroll_to_location(mainWindow, *subjectLoc);
            }
            break;
        case WIDX_RIGHT_OUTSET:
        case WIDX_DATE:
            context_open_window(WC_RECENT_NEWS);
            break;
    }
}

static OpenRCT2String window_game_bottom_toolbar_tooltip(
    rct_window* w, const rct_widgetindex widgetIndex, const rct_string_id fallback)
{
    int32_t month, day;
    auto ft = Formatter();

    switch (widgetIndex)
    {
        case WIDX_MONEY:
            ft.Add<int32_t>(gCurrentProfit);
            ft.Add<int32_t>(gParkValue);
            break;
        case WIDX_PARK_RATING:
            ft.Add<int16_t>(gParkRating);
            break;
        case WIDX_DATE:
            month = date_get_month(gDateMonthsElapsed);
            day = ((gDateMonthTicks * days_in_month[month]) >> 16) & 0xFF;

            ft.Add<rct_string_id>(DateDayNames[day]);
            ft.Add<rct_string_id>(DateGameMonthNames[month]);
            break;
    }
    return { fallback, ft };
}

/**
 *
 *  rct2: 0x0066BBA0
 */
static void window_game_bottom_toolbar_invalidate(rct_window* w)
{
    // Figure out how much line height we have to work with.
    uint32_t line_height = font_get_line_height(FontSpriteBase::MEDIUM);

    // Reset dimensions as appropriate -- in case we're switching languages.
    w->height = line_height * 2 + 12;
    w->windowPos.y = context_get_height() - w->height;

    // Change height of widgets in accordance with line height.
    w->widgets[WIDX_LEFT_OUTSET].bottom = w->widgets[WIDX_MIDDLE_OUTSET].bottom = w->widgets[WIDX_RIGHT_OUTSET].bottom
        = line_height * 3 + 3;
    w->widgets[WIDX_LEFT_INSET].bottom = w->widgets[WIDX_MIDDLE_INSET].bottom = w->widgets[WIDX_RIGHT_INSET].bottom
        = line_height * 3 + 1;

    // Reposition left widgets in accordance with line height... depending on whether there is money in play.
    if (gParkFlags & PARK_FLAGS_NO_MONEY)
    {
        w->widgets[WIDX_MONEY].type = WindowWidgetType::Empty;
        w->widgets[WIDX_GUESTS].top = 1;
        w->widgets[WIDX_GUESTS].bottom = line_height + 7;
        w->widgets[WIDX_PARK_RATING].top = line_height + 8;
        w->widgets[WIDX_PARK_RATING].bottom = w->height - 1;
    }
    else
    {
        w->widgets[WIDX_MONEY].type = WindowWidgetType::FlatBtn;
        w->widgets[WIDX_MONEY].bottom = w->widgets[WIDX_MONEY].top + line_height;
        w->widgets[WIDX_GUESTS].top = w->widgets[WIDX_MONEY].bottom + 1;
        w->widgets[WIDX_GUESTS].bottom = w->widgets[WIDX_GUESTS].top + line_height;
        w->widgets[WIDX_PARK_RATING].top = w->widgets[WIDX_GUESTS].bottom - 1;
        w->widgets[WIDX_PARK_RATING].bottom = w->height - 1;
    }

    // Reposition right widgets in accordance with line height, too.
    w->widgets[WIDX_DATE].bottom = line_height + 1;

    // Anchor the middle and right panel to the right
    int32_t x = context_get_width();
    w->width = x;
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
            window_game_bottom_toolbar_widgets[WIDX_MIDDLE_INSET].type = WindowWidgetType::Placeholder;
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
        window_game_bottom_toolbar_widgets[WIDX_MIDDLE_INSET].type = WindowWidgetType::Placeholder;
        window_game_bottom_toolbar_widgets[WIDX_NEWS_SUBJECT].type = WindowWidgetType::FlatBtn;
        window_game_bottom_toolbar_widgets[WIDX_NEWS_LOCATE].type = WindowWidgetType::FlatBtn;
        window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].colour = 2;
        window_game_bottom_toolbar_widgets[WIDX_MIDDLE_INSET].colour = 2;
        w->disabled_widgets &= ~(1 << WIDX_NEWS_SUBJECT);
        w->disabled_widgets &= ~(1 << WIDX_NEWS_LOCATE);

        // Find out if the news item is no longer valid
        auto subjectLoc = News::GetSubjectLocation(newsItem->Type, newsItem->Assoc);

        if (subjectLoc == std::nullopt)
            w->disabled_widgets |= (1 << WIDX_NEWS_LOCATE);

        if (!(newsItem->TypeHasSubject()))
        {
            w->disabled_widgets |= (1 << WIDX_NEWS_SUBJECT);
            window_game_bottom_toolbar_widgets[WIDX_NEWS_SUBJECT].type = WindowWidgetType::Empty;
        }

        if (newsItem->HasButton())
        {
            w->disabled_widgets |= (1 << WIDX_NEWS_SUBJECT);
            w->disabled_widgets |= (1 << WIDX_NEWS_LOCATE);
        }
    }
}

/**
 *
 *  rct2: 0x0066BB79
 */
void window_game_bottom_toolbar_invalidate_news_item()
{
    if (gScreenFlags == SCREEN_FLAGS_PLAYING)
    {
        widget_invalidate_by_class(WC_BOTTOM_TOOLBAR, WIDX_MIDDLE_OUTSET);
    }
}

/**
 *
 *  rct2: 0x0066BC87
 */
static void window_game_bottom_toolbar_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    // Draw panel grey backgrounds
    gfx_filter_rect(
        dpi, w->windowPos.x + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].left,
        w->windowPos.y + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].top,
        w->windowPos.x + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].right,
        w->windowPos.y + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].bottom, FilterPaletteID::Palette51);
    gfx_filter_rect(
        dpi, w->windowPos.x + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].left,
        w->windowPos.y + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].top,
        w->windowPos.x + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].right,
        w->windowPos.y + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].bottom, FilterPaletteID::Palette51);

    if (ThemeGetFlags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR)
    {
        // Draw grey background
        gfx_filter_rect(
            dpi, w->windowPos.x + window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].left,
            w->windowPos.y + window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].top,
            w->windowPos.x + window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].right,
            w->windowPos.y + window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].bottom, FilterPaletteID::Palette51);
    }

    WindowDrawWidgets(w, dpi);

    window_game_bottom_toolbar_draw_left_panel(dpi, w);
    window_game_bottom_toolbar_draw_right_panel(dpi, w);

    if (!News::IsQueueEmpty())
    {
        window_game_bottom_toolbar_draw_news_item(dpi, w);
    }
    else if (ThemeGetFlags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR)
    {
        window_game_bottom_toolbar_draw_middle_panel(dpi, w);
    }
}

static void window_game_bottom_toolbar_draw_left_panel(rct_drawpixelinfo* dpi, rct_window* w)
{
    // Draw green inset rectangle on panel
    gfx_fill_rect_inset(
        dpi, w->windowPos.x + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].left + 1,
        w->windowPos.y + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].top + 1,
        w->windowPos.x + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].right - 1,
        w->windowPos.y + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].bottom - 1, w->colours[1], INSET_RECT_F_30);

    // Figure out how much line height we have to work with.
    uint32_t line_height = font_get_line_height(FontSpriteBase::MEDIUM);

    // Draw money
    if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
    {
        rct_widget widget = window_game_bottom_toolbar_widgets[WIDX_MONEY];
        auto screenCoords = ScreenCoordsXY{ w->windowPos.x + widget.midX(),
                                            w->windowPos.y + widget.midY() - (line_height == 10 ? 5 : 6) };

        colour_t colour
            = (gHoverWidget.window_classification == WC_BOTTOM_TOOLBAR && gHoverWidget.widget_index == WIDX_MONEY
                   ? COLOUR_WHITE
                   : NOT_TRANSLUCENT(w->colours[0]));
        rct_string_id stringId = gCash < 0 ? STR_BOTTOM_TOOLBAR_CASH_NEGATIVE : STR_BOTTOM_TOOLBAR_CASH;
        auto ft = Formatter();
        ft.Add<money32>(gCash);
        DrawTextBasic(dpi, screenCoords, stringId, ft, { colour, TextAlignment::CENTRE });
    }

    static constexpr const rct_string_id guestCountFormats[] = {
        STR_BOTTOM_TOOLBAR_NUM_GUESTS_STABLE,
        STR_BOTTOM_TOOLBAR_NUM_GUESTS_DECREASE,
        STR_BOTTOM_TOOLBAR_NUM_GUESTS_INCREASE,
    };

    static constexpr const rct_string_id guestCountFormatsSingular[] = {
        STR_BOTTOM_TOOLBAR_NUM_GUESTS_STABLE_SINGULAR,
        STR_BOTTOM_TOOLBAR_NUM_GUESTS_DECREASE_SINGULAR,
        STR_BOTTOM_TOOLBAR_NUM_GUESTS_INCREASE_SINGULAR,
    };

    // Draw guests
    {
        rct_widget widget = window_game_bottom_toolbar_widgets[WIDX_GUESTS];
        auto screenCoords = ScreenCoordsXY{ w->windowPos.x + widget.midX(), w->windowPos.y + widget.midY() - 6 };

        rct_string_id stringId = gNumGuestsInPark == 1 ? guestCountFormatsSingular[gGuestChangeModifier]
                                                       : guestCountFormats[gGuestChangeModifier];
        colour_t colour
            = (gHoverWidget.window_classification == WC_BOTTOM_TOOLBAR && gHoverWidget.widget_index == WIDX_GUESTS
                   ? COLOUR_WHITE
                   : NOT_TRANSLUCENT(w->colours[0]));
        auto ft = Formatter();
        ft.Add<uint32_t>(gNumGuestsInPark);
        DrawTextBasic(dpi, screenCoords, stringId, ft, { colour, TextAlignment::CENTRE });
    }

    // Draw park rating
    {
        rct_widget widget = window_game_bottom_toolbar_widgets[WIDX_PARK_RATING];
        int32_t x = w->windowPos.x + widget.left + 11;
        int32_t y = w->windowPos.y + widget.midY() - 5;

        window_game_bottom_toolbar_draw_park_rating(dpi, w, w->colours[3], x, y, std::max(10, ((gParkRating / 4) * 263) / 256));
    }
}

/**
 *
 *  rct2: 0x0066C76C
 */
static void window_game_bottom_toolbar_draw_park_rating(
    rct_drawpixelinfo* dpi, rct_window* w, int32_t colour, int32_t x, int32_t y, uint8_t factor)
{
    int16_t bar_width;

    bar_width = (factor * 114) / 255;
    gfx_fill_rect_inset(dpi, x + 1, y + 1, x + 114, y + 9, w->colours[1], INSET_RECT_F_30);
    if (!(colour & IMAGE_TYPE_REMAP_2_PLUS) || game_is_paused() || (gCurrentRealTimeTicks & 8))
    {
        if (bar_width > 2)
            gfx_fill_rect_inset(dpi, x + 2, y + 2, x + bar_width - 1, y + 8, colour & 0x7FFFFFFF, 0);
    }

    // Draw thumbs on the sides
    gfx_draw_sprite(dpi, SPR_RATING_LOW, { x - 14, y }, 0);
    gfx_draw_sprite(dpi, SPR_RATING_HIGH, { x + 114, y }, 0);
}

static void window_game_bottom_toolbar_draw_right_panel(rct_drawpixelinfo* dpi, rct_window* w)
{
    // Draw green inset rectangle on panel
    gfx_fill_rect_inset(
        dpi, w->windowPos.x + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].left + 1,
        w->windowPos.y + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].top + 1,
        w->windowPos.x + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].right - 1,
        w->windowPos.y + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].bottom - 1, w->colours[1], INSET_RECT_F_30);

    auto screenCoords = ScreenCoordsXY{ (window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].left
                                         + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].right)
                                                / 2
                                            + w->windowPos.x,
                                        window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].top + w->windowPos.y + 2 };

    // Date
    int32_t year = date_get_year(gDateMonthsElapsed) + 1;
    int32_t month = date_get_month(gDateMonthsElapsed);
    int32_t day = ((gDateMonthTicks * days_in_month[month]) >> 16) & 0xFF;

    colour_t colour
        = (gHoverWidget.window_classification == WC_BOTTOM_TOOLBAR && gHoverWidget.widget_index == WIDX_DATE
               ? COLOUR_WHITE
               : NOT_TRANSLUCENT(w->colours[0]));
    rct_string_id stringId = DateFormatStringFormatIds[gConfigGeneral.date_format];
    auto ft = Formatter();
    ft.Add<rct_string_id>(DateDayNames[day]);
    ft.Add<int16_t>(month);
    ft.Add<int16_t>(year);
    DrawTextBasic(dpi, screenCoords, stringId, ft, { colour, TextAlignment::CENTRE });

    // Figure out how much line height we have to work with.
    uint32_t line_height = font_get_line_height(FontSpriteBase::MEDIUM);

    // Temperature
    screenCoords = { w->windowPos.x + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].left + 15,
                     static_cast<int32_t>(screenCoords.y + line_height + 1) };

    int32_t temperature = gClimateCurrent.Temperature;
    rct_string_id format = STR_CELSIUS_VALUE;
    if (gConfigGeneral.temperature_format == TemperatureUnit::Fahrenheit)
    {
        temperature = climate_celsius_to_fahrenheit(temperature);
        format = STR_FAHRENHEIT_VALUE;
    }
    ft = Formatter();
    ft.Add<int16_t>(temperature);
    DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ 0, 6 }, format, ft);
    screenCoords.x += 30;

    // Current weather
    auto currentWeatherSpriteId = climate_get_weather_sprite_id(gClimateCurrent);
    gfx_draw_sprite(dpi, currentWeatherSpriteId, screenCoords, 0);

    // Next weather
    auto nextWeatherSpriteId = climate_get_weather_sprite_id(gClimateNext);
    if (currentWeatherSpriteId != nextWeatherSpriteId)
    {
        if (gClimateUpdateTimer < 960)
        {
            gfx_draw_sprite(dpi, SPR_NEXT_WEATHER, screenCoords + ScreenCoordsXY{ 27, 5 }, 0);
            gfx_draw_sprite(dpi, nextWeatherSpriteId, screenCoords + ScreenCoordsXY{ 40, 0 }, 0);
        }
    }
}

/**
 *
 *  rct2: 0x0066BFA5
 */
static void window_game_bottom_toolbar_draw_news_item(rct_drawpixelinfo* dpi, rct_window* w)
{
    int32_t width;
    News::Item* newsItem;
    rct_widget* middleOutsetWidget;

    middleOutsetWidget = &window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET];
    newsItem = News::GetItem(0);

    // Current news item
    gfx_fill_rect_inset(
        dpi, w->windowPos.x + middleOutsetWidget->left + 1, w->windowPos.y + middleOutsetWidget->top + 1,
        w->windowPos.x + middleOutsetWidget->right - 1, w->windowPos.y + middleOutsetWidget->bottom - 1, w->colours[2],
        INSET_RECT_F_30);

    // Text
    const auto* newsItemText = newsItem->Text.c_str();
    auto screenCoords = w->windowPos + ScreenCoordsXY{ middleOutsetWidget->midX(), middleOutsetWidget->top + 11 };
    width = middleOutsetWidget->width() - 62;
    DrawNewsTicker(dpi, screenCoords, width, COLOUR_BRIGHT_GREEN, STR_BOTTOM_TOOLBAR_NEWS_TEXT, &newsItemText, newsItem->Ticks);

    screenCoords = w->windowPos
        + ScreenCoordsXY{ window_game_bottom_toolbar_widgets[WIDX_NEWS_SUBJECT].left,
                          window_game_bottom_toolbar_widgets[WIDX_NEWS_SUBJECT].top };
    switch (newsItem->Type)
    {
        case News::ItemType::Ride:
            gfx_draw_sprite(dpi, SPR_RIDE, screenCoords, 0);
            break;
        case News::ItemType::PeepOnRide:
        case News::ItemType::Peep:
        {
            if (newsItem->HasButton())
                break;

            rct_drawpixelinfo cliped_dpi;
            if (!clip_drawpixelinfo(&cliped_dpi, dpi, screenCoords + ScreenCoordsXY{ 1, 1 }, 22, 22))
            {
                break;
            }

            auto peep = TryGetEntity<Peep>(newsItem->Assoc);
            if (peep == nullptr)
                return;

            auto clipCoords = ScreenCoordsXY{ 10, 19 };

            if (peep->AssignedPeepType == PeepType::Staff && peep->AssignedStaffType == StaffType::Entertainer)
            {
                clipCoords.y += 3;
            }

            uint32_t image_id_base = GetPeepAnimation(peep->SpriteType).base_image;
            image_id_base += w->frame_no & 0xFFFFFFFC;
            image_id_base++;

            uint32_t image_id = image_id_base;
            image_id |= SPRITE_ID_PALETTE_COLOUR_2(peep->TshirtColour, peep->TrousersColour);

            gfx_draw_sprite(&cliped_dpi, image_id, clipCoords, 0);

            if (image_id_base >= 0x2A1D && image_id_base < 0x2A3D)
            {
                image_id_base += 32;
                image_id_base |= SPRITE_ID_PALETTE_COLOUR_1(peep->BalloonColour);

                gfx_draw_sprite(&cliped_dpi, image_id_base, clipCoords, 0);
            }
            else if (image_id_base >= 0x2BBD && image_id_base < 0x2BDD)
            {
                image_id_base += 32;
                image_id_base |= SPRITE_ID_PALETTE_COLOUR_1(peep->UmbrellaColour);

                gfx_draw_sprite(&cliped_dpi, image_id_base, clipCoords, 0);
            }
            else if (image_id_base >= 0x29DD && image_id_base < 0x29FD)
            {
                image_id_base += 32;
                image_id_base |= SPRITE_ID_PALETTE_COLOUR_1(peep->HatColour);

                gfx_draw_sprite(&cliped_dpi, image_id_base, clipCoords, 0);
            }
            break;
        }
        case News::ItemType::Money:
            gfx_draw_sprite(dpi, SPR_FINANCE, screenCoords, 0);
            break;
        case News::ItemType::Research:
            gfx_draw_sprite(dpi, (newsItem->Assoc < 0x10000 ? SPR_NEW_SCENERY : SPR_NEW_RIDE), screenCoords, 0);
            break;
        case News::ItemType::Peeps:
            gfx_draw_sprite(dpi, SPR_GUESTS, screenCoords, 0);
            break;
        case News::ItemType::Award:
            gfx_draw_sprite(dpi, SPR_AWARD, screenCoords, 0);
            break;
        case News::ItemType::Graph:
            gfx_draw_sprite(dpi, SPR_GRAPH, screenCoords, 0);
            break;
        case News::ItemType::Null:
        case News::ItemType::Blank:
        case News::ItemType::Count:
            break;
    }
}

static void window_game_bottom_toolbar_draw_middle_panel(rct_drawpixelinfo* dpi, rct_window* w)
{
    rct_widget* middleOutsetWidget = &window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET];

    gfx_fill_rect_inset(
        dpi, w->windowPos.x + middleOutsetWidget->left + 1, w->windowPos.y + middleOutsetWidget->top + 1,
        w->windowPos.x + middleOutsetWidget->right - 1, w->windowPos.y + middleOutsetWidget->bottom - 1, w->colours[1],
        INSET_RECT_F_30);

    // Figure out how much line height we have to work with.
    uint32_t line_height = font_get_line_height(FontSpriteBase::MEDIUM);

    ScreenCoordsXY middleWidgetCoords(
        w->windowPos.x + middleOutsetWidget->midX(), w->windowPos.y + middleOutsetWidget->top + line_height + 1);
    int32_t width = middleOutsetWidget->width() - 62;

    // Check if there is a map tooltip to draw
    rct_string_id stringId;
    auto ft = GetMapTooltip();
    std::memcpy(&stringId, ft.Data(), sizeof(rct_string_id));
    if (stringId == STR_NONE)
    {
        DrawTextWrapped(
            dpi, middleWidgetCoords, width, STR_TITLE_SEQUENCE_OPENRCT2, ft, { w->colours[0], TextAlignment::CENTRE });
    }
    else
    {
        // Show tooltip in bottom toolbar
        DrawTextWrapped(dpi, middleWidgetCoords, width, STR_STRINGID, ft, { w->colours[0], TextAlignment::CENTRE });
    }
}

/**
 *
 *  rct2: 0x0066C6D8
 */
static void window_game_bottom_toolbar_update(rct_window* w)
{
    w->frame_no++;
    if (w->frame_no >= 24)
        w->frame_no = 0;

    window_game_bottom_toolbar_invalidate_dirty_widgets(w);
}

/**
 *
 *  rct2: 0x0066C644
 */
static void window_game_bottom_toolbar_cursor(
    rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords, CursorID* cursorId)
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
}

/**
 *
 *  rct2: 0x0066C6F2
 */
static void window_game_bottom_toolbar_unknown05(rct_window* w)
{
    window_game_bottom_toolbar_invalidate_dirty_widgets(w);
}

/**
 *
 *  rct2: 0x0066C6F2
 */
static void window_game_bottom_toolbar_invalidate_dirty_widgets(rct_window* w)
{
    if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_MONEY)
    {
        gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_MONEY;
        widget_invalidate(w, WIDX_LEFT_INSET);
    }

    if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_DATE)
    {
        gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_DATE;
        widget_invalidate(w, WIDX_RIGHT_INSET);
    }

    if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_PEEP_COUNT)
    {
        gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_PEEP_COUNT;
        widget_invalidate(w, WIDX_LEFT_INSET);
    }

    if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_CLIMATE)
    {
        gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_CLIMATE;
        widget_invalidate(w, WIDX_RIGHT_INSET);
    }

    if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_PARK_RATING)
    {
        gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_PARK_RATING;
        widget_invalidate(w, WIDX_LEFT_INSET);
    }
}
