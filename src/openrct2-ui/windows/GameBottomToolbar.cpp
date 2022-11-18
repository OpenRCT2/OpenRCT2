/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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

static rct_widget window_game_bottom_toolbar_widgets[] =
{
    MakeWidget({  0,  0}, {142, 34}, WindowWidgetType::ImgBtn,      WindowColour::Primary                                                     ), // Left outset panel
    MakeWidget({  2,  2}, {138, 30}, WindowWidgetType::ImgBtn,      WindowColour::Primary                                                     ), // Left inset panel
    MakeWidget({  2,  1}, {138, 12}, WindowWidgetType::FlatBtn,     WindowColour::Primary , 0xFFFFFFFF, STR_PROFIT_PER_WEEK_AND_PARK_VALUE_TIP), // Money window
    MakeWidget({  2, 11}, {138, 12}, WindowWidgetType::FlatBtn,     WindowColour::Primary                                                     ), // Guests window
    MakeWidget({  2, 21}, {138, 11}, WindowWidgetType::FlatBtn,     WindowColour::Primary , 0xFFFFFFFF, STR_PARK_RATING_TIP                   ), // Park rating window

    MakeWidget({142,  0}, {356, 34}, WindowWidgetType::ImgBtn,      WindowColour::Tertiary                                                    ), // Middle outset panel
    MakeWidget({144,  2}, {352, 30}, WindowWidgetType::FlatBtn,     WindowColour::Tertiary                                                    ), // Middle inset panel
    MakeWidget({147,  5}, { 24, 24}, WindowWidgetType::FlatBtn,     WindowColour::Tertiary, 0xFFFFFFFF, STR_SHOW_SUBJECT_TIP                  ), // Associated news item window
    MakeWidget({469,  5}, { 24, 24}, WindowWidgetType::FlatBtn,     WindowColour::Tertiary, SPR_LOCATE, STR_LOCATE_SUBJECT_TIP                ), // Scroll to news item target

    MakeWidget({498,  0}, {142, 34}, WindowWidgetType::ImgBtn,      WindowColour::Primary                                                     ), // Right outset panel
    MakeWidget({500,  2}, {138, 30}, WindowWidgetType::ImgBtn,      WindowColour::Primary                                                     ), // Right inset panel
    MakeWidget({500,  2}, {138, 12}, WindowWidgetType::FlatBtn,     WindowColour::Primary                                                     ), // Date
    WIDGETS_END,
};

uint8_t gToolbarDirtyFlags;

static void WindowGameBottomToolbarMouseup(rct_window *w, WidgetIndex widgetIndex);
static OpenRCT2String WindowGameBottomToolbarTooltip(rct_window* w, const WidgetIndex widgetIndex, const StringId fallback);
static void WindowGameBottomToolbarInvalidate(rct_window *w);
static void WindowGameBottomToolbarPaint(rct_window *w, rct_drawpixelinfo *dpi);
static void WindowGameBottomToolbarUpdate(rct_window* w);
static void WindowGameBottomToolbarCursor(rct_window *w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords, CursorID *cursorId);
static void WindowGameBottomToolbarUnknown05(rct_window *w);

static void WindowGameBottomToolbarDrawLeftPanel(rct_drawpixelinfo *dpi, rct_window *w);
static void WindowGameBottomToolbarDrawParkRating(rct_drawpixelinfo *dpi, rct_window *w, int32_t colour, const ScreenCoordsXY& coords, uint8_t factor);
static void WindowGameBottomToolbarDrawRightPanel(rct_drawpixelinfo *dpi, rct_window *w);
static void WindowGameBottomToolbarDrawNewsItem(rct_drawpixelinfo *dpi, rct_window *w);
static void WindowGameBottomToolbarDrawMiddlePanel(rct_drawpixelinfo *dpi, rct_window *w);

/**
 *
 *  rct2: 0x0097BFDC
 */
static WindowEventList window_game_bottom_toolbar_events([](auto& events)
{
    events.mouse_up = &WindowGameBottomToolbarMouseup;
    events.unknown_05 = &WindowGameBottomToolbarUnknown05;
    events.update = &WindowGameBottomToolbarUpdate;
    events.tooltip = &WindowGameBottomToolbarTooltip;
    events.cursor = &WindowGameBottomToolbarCursor;
    events.invalidate = &WindowGameBottomToolbarInvalidate;
    events.paint = &WindowGameBottomToolbarPaint;
});
// clang-format on

static void WindowGameBottomToolbarInvalidateDirtyWidgets(rct_window* w);

/**
 * Creates the main game bottom toolbar window.
 *  rct2: 0x0066B52F (part of 0x0066B3E8)
 */
rct_window* WindowGameBottomToolbarOpen()
{
    int32_t screenWidth = ContextGetWidth();
    int32_t screenHeight = ContextGetHeight();

    // Figure out how much line height we have to work with.
    uint32_t line_height = font_get_line_height(FontStyle::Medium);
    uint32_t toolbar_height = line_height * 2 + 12;

    rct_window* window = WindowCreate(
        ScreenCoordsXY(0, screenHeight - toolbar_height), screenWidth, toolbar_height, &window_game_bottom_toolbar_events,
        WindowClass::BottomToolbar, WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_BACKGROUND);
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
 *  rct2: 0x0066C588
 */
static void WindowGameBottomToolbarMouseup(rct_window* w, WidgetIndex widgetIndex)
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

                rct_window* mainWindow = window_get_main();
                if (mainWindow != nullptr)
                    window_scroll_to_location(*mainWindow, subjectLoc.value());
            }
            break;
        case WIDX_RIGHT_OUTSET:
        case WIDX_DATE:
            ContextOpenWindow(WindowClass::RecentNews);
            break;
    }
}

static OpenRCT2String WindowGameBottomToolbarTooltip(rct_window* w, const WidgetIndex widgetIndex, const StringId fallback)
{
    int32_t month, day;
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
        case WIDX_DATE:
            month = date_get_month(gDateMonthsElapsed);
            day = ((gDateMonthTicks * days_in_month[month]) >> 16) & 0xFF;

            ft.Add<StringId>(DateDayNames[day]);
            ft.Add<StringId>(DateGameMonthNames[month]);
            break;
    }
    return { fallback, ft };
}

/**
 *
 *  rct2: 0x0066BBA0
 */
static void WindowGameBottomToolbarInvalidate(rct_window* w)
{
    // Figure out how much line height we have to work with.
    uint32_t line_height = font_get_line_height(FontStyle::Medium);

    // Reset dimensions as appropriate -- in case we're switching languages.
    w->height = line_height * 2 + 12;
    w->windowPos.y = ContextGetHeight() - w->height;

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
    int32_t x = ContextGetWidth();
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
        w->disabled_widgets &= ~(1uLL << WIDX_NEWS_SUBJECT);
        w->disabled_widgets &= ~(1uLL << WIDX_NEWS_LOCATE);

        // Find out if the news item is no longer valid
        auto subjectLoc = News::GetSubjectLocation(newsItem->Type, newsItem->Assoc);

        if (!subjectLoc.has_value())
            w->disabled_widgets |= (1uLL << WIDX_NEWS_LOCATE);

        if (!(newsItem->TypeHasSubject()))
        {
            w->disabled_widgets |= (1uLL << WIDX_NEWS_SUBJECT);
            window_game_bottom_toolbar_widgets[WIDX_NEWS_SUBJECT].type = WindowWidgetType::Empty;
        }

        if (newsItem->HasButton())
        {
            w->disabled_widgets |= (1uLL << WIDX_NEWS_SUBJECT);
            w->disabled_widgets |= (1uLL << WIDX_NEWS_LOCATE);
        }
    }
}

/**
 *
 *  rct2: 0x0066BB79
 */
void WindowGameBottomToolbarInvalidateNewsItem()
{
    if (gScreenFlags == SCREEN_FLAGS_PLAYING)
    {
        widget_invalidate_by_class(WindowClass::BottomToolbar, WIDX_MIDDLE_OUTSET);
    }
}

/**
 *
 *  rct2: 0x0066BC87
 */
static void WindowGameBottomToolbarPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    auto leftWidget = window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET];
    auto rightWidget = window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET];
    auto middleWidget = window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET];

    // Draw panel grey backgrounds
    auto leftTop = w->windowPos + ScreenCoordsXY{ leftWidget.left, leftWidget.top };
    auto rightBottom = w->windowPos + ScreenCoordsXY{ leftWidget.right, leftWidget.bottom };
    gfx_filter_rect(dpi, { leftTop, rightBottom }, FilterPaletteID::Palette51);

    leftTop = w->windowPos + ScreenCoordsXY{ rightWidget.left, rightWidget.top };
    rightBottom = w->windowPos + ScreenCoordsXY{ rightWidget.right, rightWidget.bottom };
    gfx_filter_rect(dpi, { leftTop, rightBottom }, FilterPaletteID::Palette51);

    if (ThemeGetFlags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR)
    {
        // Draw grey background
        leftTop = w->windowPos + ScreenCoordsXY{ middleWidget.left, middleWidget.top };
        rightBottom = w->windowPos + ScreenCoordsXY{ middleWidget.right, middleWidget.bottom };
        gfx_filter_rect(dpi, { leftTop, rightBottom }, FilterPaletteID::Palette51);
    }

    WindowDrawWidgets(*w, dpi);

    WindowGameBottomToolbarDrawLeftPanel(dpi, w);
    WindowGameBottomToolbarDrawRightPanel(dpi, w);

    if (!News::IsQueueEmpty())
    {
        WindowGameBottomToolbarDrawNewsItem(dpi, w);
    }
    else if (ThemeGetFlags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR)
    {
        WindowGameBottomToolbarDrawMiddlePanel(dpi, w);
    }
}

static void WindowGameBottomToolbarDrawLeftPanel(rct_drawpixelinfo* dpi, rct_window* w)
{
    const auto topLeft = w->windowPos
        + ScreenCoordsXY{ window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].left + 1,
                          window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].top + 1 };
    const auto bottomRight = w->windowPos
        + ScreenCoordsXY{ window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].right - 1,
                          window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].bottom - 1 };
    // Draw green inset rectangle on panel
    gfx_fill_rect_inset(dpi, { topLeft, bottomRight }, w->colours[1], INSET_RECT_F_30);

    // Figure out how much line height we have to work with.
    uint32_t line_height = font_get_line_height(FontStyle::Medium);

    // Draw money
    if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
    {
        rct_widget widget = window_game_bottom_toolbar_widgets[WIDX_MONEY];
        auto screenCoords = ScreenCoordsXY{ w->windowPos.x + widget.midX(),
                                            w->windowPos.y + widget.midY() - (line_height == 10 ? 5 : 6) };

        colour_t colour
            = (gHoverWidget.window_classification == WindowClass::BottomToolbar && gHoverWidget.widget_index == WIDX_MONEY
                   ? COLOUR_WHITE
                   : NOT_TRANSLUCENT(w->colours[0]));
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
        rct_widget widget = window_game_bottom_toolbar_widgets[WIDX_GUESTS];
        auto screenCoords = ScreenCoordsXY{ w->windowPos.x + widget.midX(), w->windowPos.y + widget.midY() - 6 };

        StringId stringId = gNumGuestsInPark == 1 ? _guestCountFormatsSingular[gGuestChangeModifier]
                                                  : _guestCountFormats[gGuestChangeModifier];
        colour_t colour
            = (gHoverWidget.window_classification == WindowClass::BottomToolbar && gHoverWidget.widget_index == WIDX_GUESTS
                   ? COLOUR_WHITE
                   : NOT_TRANSLUCENT(w->colours[0]));
        auto ft = Formatter();
        ft.Add<uint32_t>(gNumGuestsInPark);
        DrawTextBasic(dpi, screenCoords, stringId, ft, { colour, TextAlignment::CENTRE });
    }

    // Draw park rating
    {
        rct_widget widget = window_game_bottom_toolbar_widgets[WIDX_PARK_RATING];
        auto screenCoords = w->windowPos + ScreenCoordsXY{ widget.left + 11, widget.midY() - 5 };

        WindowGameBottomToolbarDrawParkRating(
            dpi, w, w->colours[3], screenCoords, std::max(10, ((gParkRating / 4) * 263) / 256));
    }
}

/**
 *
 *  rct2: 0x0066C76C
 */
static void WindowGameBottomToolbarDrawParkRating(
    rct_drawpixelinfo* dpi, rct_window* w, int32_t colour, const ScreenCoordsXY& coords, uint8_t factor)
{
    int16_t bar_width;

    bar_width = (factor * 114) / 255;
    gfx_fill_rect_inset(
        dpi, { coords + ScreenCoordsXY{ 1, 1 }, coords + ScreenCoordsXY{ 114, 9 } }, w->colours[1], INSET_RECT_F_30);
    if (!(colour & BAR_BLINK) || game_is_paused() || (gCurrentRealTimeTicks & 8))
    {
        if (bar_width > 2)
        {
            gfx_fill_rect_inset(
                dpi, { coords + ScreenCoordsXY{ 2, 2 }, coords + ScreenCoordsXY{ bar_width - 1, 8 } }, colour, 0);
        }
    }

    // Draw thumbs on the sides
    gfx_draw_sprite(dpi, ImageId(SPR_RATING_LOW), coords - ScreenCoordsXY{ 14, 0 });
    gfx_draw_sprite(dpi, ImageId(SPR_RATING_HIGH), coords + ScreenCoordsXY{ 114, 0 });
}

static void WindowGameBottomToolbarDrawRightPanel(rct_drawpixelinfo* dpi, rct_window* w)
{
    const auto topLeft = w->windowPos
        + ScreenCoordsXY{ window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].left + 1,
                          window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].top + 1 };
    const auto bottomRight = w->windowPos
        + ScreenCoordsXY{ window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].right - 1,
                          window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].bottom - 1 };
    // Draw green inset rectangle on panel
    gfx_fill_rect_inset(dpi, { topLeft, bottomRight }, w->colours[1], INSET_RECT_F_30);

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
        = (gHoverWidget.window_classification == WindowClass::BottomToolbar && gHoverWidget.widget_index == WIDX_DATE
               ? COLOUR_WHITE
               : NOT_TRANSLUCENT(w->colours[0]));
    StringId stringId = DateFormatStringFormatIds[gConfigGeneral.DateFormat];
    auto ft = Formatter();
    ft.Add<StringId>(DateDayNames[day]);
    ft.Add<int16_t>(month);
    ft.Add<int16_t>(year);
    DrawTextBasic(dpi, screenCoords, stringId, ft, { colour, TextAlignment::CENTRE });

    // Figure out how much line height we have to work with.
    uint32_t line_height = font_get_line_height(FontStyle::Medium);

    // Temperature
    screenCoords = { w->windowPos.x + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].left + 15,
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
    gfx_draw_sprite(dpi, ImageId(currentWeatherSpriteId), screenCoords);

    // Next weather
    auto nextWeatherSpriteId = ClimateGetWeatherSpriteId(gClimateNext);
    if (currentWeatherSpriteId != nextWeatherSpriteId)
    {
        if (gClimateUpdateTimer < 960)
        {
            gfx_draw_sprite(dpi, ImageId(SPR_NEXT_WEATHER), screenCoords + ScreenCoordsXY{ 27, 5 });
            gfx_draw_sprite(dpi, ImageId(nextWeatherSpriteId), screenCoords + ScreenCoordsXY{ 40, 0 });
        }
    }
}

/**
 *
 *  rct2: 0x0066BFA5
 */
static void WindowGameBottomToolbarDrawNewsItem(rct_drawpixelinfo* dpi, rct_window* w)
{
    int32_t width;
    News::Item* newsItem;
    rct_widget* middleOutsetWidget;

    middleOutsetWidget = &window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET];
    newsItem = News::GetItem(0);

    // Current news item
    gfx_fill_rect_inset(
        dpi,
        { w->windowPos + ScreenCoordsXY{ middleOutsetWidget->left + 1, middleOutsetWidget->top + 1 },
          w->windowPos + ScreenCoordsXY{ middleOutsetWidget->right - 1, middleOutsetWidget->bottom - 1 } },
        w->colours[2], INSET_RECT_F_30);

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
            gfx_draw_sprite(dpi, ImageId(SPR_RIDE), screenCoords);
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
            image_id_base += w->frame_no & 0xFFFFFFFC;
            image_id_base++;

            auto image_id = ImageId(image_id_base, peep->TshirtColour, peep->TrousersColour);
            gfx_draw_sprite(&cliped_dpi, image_id, clipCoords);

            auto* guest = peep->As<Guest>();
            if (guest != nullptr)
            {
                if (image_id_base >= 0x2A1D && image_id_base < 0x2A3D)
                {
                    gfx_draw_sprite(&cliped_dpi, ImageId(image_id_base + 32, guest->BalloonColour), clipCoords);
                }
                else if (image_id_base >= 0x2BBD && image_id_base < 0x2BDD)
                {
                    gfx_draw_sprite(&cliped_dpi, ImageId(image_id_base + 32, guest->UmbrellaColour), clipCoords);
                }
                else if (image_id_base >= 0x29DD && image_id_base < 0x29FD)
                {
                    gfx_draw_sprite(&cliped_dpi, ImageId(image_id_base + 32, guest->HatColour), clipCoords);
                }
            }
            break;
        }
        case News::ItemType::Money:
        case News::ItemType::Campaign:
            gfx_draw_sprite(dpi, ImageId(SPR_FINANCE), screenCoords);
            break;
        case News::ItemType::Research:
            gfx_draw_sprite(dpi, ImageId(newsItem->Assoc < 0x10000 ? SPR_NEW_SCENERY : SPR_NEW_RIDE), screenCoords);
            break;
        case News::ItemType::Peeps:
            gfx_draw_sprite(dpi, ImageId(SPR_GUESTS), screenCoords);
            break;
        case News::ItemType::Award:
            gfx_draw_sprite(dpi, ImageId(SPR_AWARD), screenCoords);
            break;
        case News::ItemType::Graph:
            gfx_draw_sprite(dpi, ImageId(SPR_GRAPH), screenCoords);
            break;
        case News::ItemType::Null:
        case News::ItemType::Blank:
        case News::ItemType::Count:
            break;
    }
}

static void WindowGameBottomToolbarDrawMiddlePanel(rct_drawpixelinfo* dpi, rct_window* w)
{
    rct_widget* middleOutsetWidget = &window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET];

    gfx_fill_rect_inset(
        dpi,
        { w->windowPos + ScreenCoordsXY{ middleOutsetWidget->left + 1, middleOutsetWidget->top + 1 },
          w->windowPos + ScreenCoordsXY{ middleOutsetWidget->right - 1, middleOutsetWidget->bottom - 1 } },
        w->colours[1], INSET_RECT_F_30);

    // Figure out how much line height we have to work with.
    uint32_t line_height = font_get_line_height(FontStyle::Medium);

    ScreenCoordsXY middleWidgetCoords(
        w->windowPos.x + middleOutsetWidget->midX(), w->windowPos.y + middleOutsetWidget->top + line_height + 1);
    int32_t width = middleOutsetWidget->width() - 62;

    // Check if there is a map tooltip to draw
    StringId stringId;
    auto ft = GetMapTooltip();
    std::memcpy(&stringId, ft.Data(), sizeof(StringId));
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
static void WindowGameBottomToolbarUpdate(rct_window* w)
{
    w->frame_no++;
    if (w->frame_no >= 24)
        w->frame_no = 0;

    WindowGameBottomToolbarInvalidateDirtyWidgets(w);
}

/**
 *
 *  rct2: 0x0066C644
 */
static void WindowGameBottomToolbarCursor(
    rct_window* w, WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords, CursorID* cursorId)
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
static void WindowGameBottomToolbarUnknown05(rct_window* w)
{
    WindowGameBottomToolbarInvalidateDirtyWidgets(w);
}

/**
 *
 *  rct2: 0x0066C6F2
 */
static void WindowGameBottomToolbarInvalidateDirtyWidgets(rct_window* w)
{
    if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_MONEY)
    {
        gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_MONEY;
        widget_invalidate(*w, WIDX_LEFT_INSET);
    }

    if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_DATE)
    {
        gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_DATE;
        widget_invalidate(*w, WIDX_RIGHT_INSET);
    }

    if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_PEEP_COUNT)
    {
        gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_PEEP_COUNT;
        widget_invalidate(*w, WIDX_LEFT_INSET);
    }

    if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_CLIMATE)
    {
        gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_CLIMATE;
        widget_invalidate(*w, WIDX_RIGHT_INSET);
    }

    if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_PARK_RATING)
    {
        gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_PARK_RATING;
        widget_invalidate(*w, WIDX_LEFT_INSET);
    }
}
