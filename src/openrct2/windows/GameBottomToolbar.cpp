#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../config/Config.h"
#include "../Context.h"
#include "../OpenRCT2.h"
#include "../world/Climate.h"
#include "../core/Math.hpp"

#include "../game.h"
#include "../input.h"
#include "../interface/themes.h"
#include "../interface/widget.h"
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "../management/news_item.h"
#include "../peep/staff.h"
#include "../sprites.h"

enum WINDOW_GAME_BOTTOM_TOOLBAR_WIDGET_IDX {
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


// Right panel needs to be a bit bigger than original so dates like "22nd September, Year 126" can fit.
// Left panel size was also increased for symmetry.
#define WIDTH_MOD 22

static rct_widget window_game_bottom_toolbar_widgets[] = {
    { WWT_IMGBTN,   0,  0x0000, 0x0077+WIDTH_MOD,   0,      33,     0xFFFFFFFF, STR_NONE }, // Left outset panel
    { WWT_IMGBTN,   0,  0x0002, 0x0075+WIDTH_MOD,   2,      31,     0xFFFFFFFF, STR_NONE }, // Left inset panel
    { WWT_FLATBTN,  0,  0x0002, 0x0075+WIDTH_MOD,   1,      12,     0xFFFFFFFF, STR_PROFIT_PER_WEEK_AND_PARK_VALUE_TIP },   // Money window
    { WWT_FLATBTN,  0,  0x0002, 0x0075+WIDTH_MOD,   11,     22,     0xFFFFFFFF, STR_NONE }, // Guests window
    { WWT_FLATBTN,  0,  0x0002, 0x0075+WIDTH_MOD,   21,     31,     0xFFFFFFFF, STR_PARK_RATING_TIP },  // Park rating window

    { WWT_IMGBTN,   2,  0x0078+WIDTH_MOD,   0x0207-WIDTH_MOD,   0,      33,     0xFFFFFFFF, STR_NONE }, // Middle outset panel
    { WWT_25,       2,  0x007A+WIDTH_MOD,   0x0205-WIDTH_MOD,   2,      31,     0xFFFFFFFF, STR_NONE }, // Middle inset panel
    { WWT_FLATBTN,  2,  0x007D+WIDTH_MOD,   0x0094+WIDTH_MOD,   5,      28,     0xFFFFFFFF, STR_SHOW_SUBJECT_TIP }, // Associated news item window
    { WWT_FLATBTN,  2,  0x01EB-WIDTH_MOD,   0x0202-WIDTH_MOD,   5,      28,     SPR_LOCATE, STR_LOCATE_SUBJECT_TIP },   // Scroll to news item target

    { WWT_IMGBTN,   0,  0x0208-WIDTH_MOD,   0x027F, 0,      33,     0xFFFFFFFF, STR_NONE }, // Right outset panel
    { WWT_IMGBTN,   0,  0x020A-WIDTH_MOD,   0x027D, 2,      31,     0xFFFFFFFF, STR_NONE }, // Right inset panel
    { WWT_FLATBTN,  0,  0x020A-WIDTH_MOD,   0x027D, 2,      13,     0xFFFFFFFF, STR_NONE }, // Date
    { WIDGETS_END },
};

static void window_game_bottom_toolbar_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_game_bottom_toolbar_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId);
static void window_game_bottom_toolbar_invalidate(rct_window *w);
static void window_game_bottom_toolbar_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_game_bottom_toolbar_update(rct_window* w);
static void window_game_bottom_toolbar_cursor(rct_window *w, rct_widgetindex widgetIndex, sint32 x, sint32 y, sint32 *cursorId);
static void window_game_bottom_toolbar_unknown05(rct_window *w);

static void window_game_bottom_toolbar_draw_left_panel(rct_drawpixelinfo *dpi, rct_window *w);
static void window_game_bottom_toolbar_draw_park_rating(rct_drawpixelinfo *dpi, rct_window *w, sint32 colour, sint32 x, sint32 y, uint8 factor);
static void window_game_bottom_toolbar_draw_right_panel(rct_drawpixelinfo *dpi, rct_window *w);
static void window_game_bottom_toolbar_draw_news_item(rct_drawpixelinfo *dpi, rct_window *w);
static void window_game_bottom_toolbar_draw_middle_panel(rct_drawpixelinfo *dpi, rct_window *w);

/**
 *
 *  rct2: 0x0097BFDC
 */
static rct_window_event_list window_game_bottom_toolbar_events = {
    nullptr,
    window_game_bottom_toolbar_mouseup,
    nullptr,
    nullptr,
    nullptr,
    window_game_bottom_toolbar_unknown05,
    window_game_bottom_toolbar_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_game_bottom_toolbar_tooltip,
    window_game_bottom_toolbar_cursor,
    nullptr,
    window_game_bottom_toolbar_invalidate,
    window_game_bottom_toolbar_paint,
    nullptr
};

static void window_game_bottom_toolbar_invalidate_dirty_widgets(rct_window *w);

/**
 * Creates the main game bottom toolbar window.
 *  rct2: 0x0066B52F (part of 0x0066B3E8)
 */
void window_game_bottom_toolbar_open()
{
    sint32 screenWidth = context_get_width();
    sint32 screenHeight = context_get_height();
    rct_window * window = window_create(
        0, screenHeight - 32,
        screenWidth, 32,
        &window_game_bottom_toolbar_events,
        WC_BOTTOM_TOOLBAR,
        WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_BACKGROUND
    );
    window->widgets = window_game_bottom_toolbar_widgets;
    window->enabled_widgets |=
        (1 << WIDX_LEFT_OUTSET) |
        (1 << WIDX_MONEY) |
        (1 << WIDX_GUESTS) |
        (1 << WIDX_PARK_RATING) |
        (1 << WIDX_MIDDLE_OUTSET) |
        (1 << WIDX_MIDDLE_INSET) |
        (1 << WIDX_NEWS_SUBJECT) |
        (1 << WIDX_NEWS_LOCATE) |
        (1 << WIDX_RIGHT_OUTSET) |
        (1 << WIDX_DATE);

    window->frame_no = 0;
    window_init_scroll_widgets(window);

    // Reset the middle widget to not show by default.
    // If it is required to be shown news_update will reshow it.
    window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].type = WWT_EMPTY;
}

/**
 *
 *  rct2: 0x0066C588
 */
static void window_game_bottom_toolbar_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    NewsItem *newsItem;

    switch (widgetIndex) {
    case WIDX_LEFT_OUTSET:
    case WIDX_MONEY:
        if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
            window_finances_open();
        break;
    case WIDX_GUESTS:
        window_park_guests_open();
        break;
    case WIDX_PARK_RATING:
        window_park_rating_open();
        break;
    case WIDX_MIDDLE_INSET:
        if (news_item_is_queue_empty())
        {
            context_open_window(WC_RECENT_NEWS);
        }
        else
        {
            news_item_close_current();
        }
        break;
    case WIDX_NEWS_SUBJECT:
        newsItem = news_item_get(0);
        news_item_open_subject(newsItem->Type, newsItem->Assoc);
        break;
    case WIDX_NEWS_LOCATE:
        if (news_item_is_queue_empty())
            break;

        {
            newsItem = news_item_get(0);
            sint32 x, y, z;
            sint32 subject = newsItem->Assoc;

            news_item_get_subject_location(newsItem->Type, subject, &x, &y, &z);

            if (x == SPRITE_LOCATION_NULL)
                break;

            rct_window *mainWindow = window_get_main();
            if (mainWindow != nullptr)
                window_scroll_to_location(mainWindow, x, y, z);
        }
        break;
    case WIDX_RIGHT_OUTSET:
    case WIDX_DATE:
        context_open_window(WC_RECENT_NEWS);
        break;
    }
}

static void window_game_bottom_toolbar_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId)
{
    sint32 month, day;

    switch (widgetIndex) {
    case WIDX_MONEY:
        set_format_arg(0, sint32, gCurrentProfit);
        set_format_arg(4, sint32, gParkValue);
        break;
    case WIDX_PARK_RATING:
        set_format_arg(0, sint16, gParkRating);
        break;
    case WIDX_DATE:
        month = gDateMonthsElapsed % 8;
        day = ((gDateMonthTicks * days_in_month[month]) >> 16) & 0xFF;

        set_format_arg(0, rct_string_id, DateDayNames[day]);
        set_format_arg(2, rct_string_id, DateGameMonthNames[month]);
        break;
    }
}

/**
 *
 *  rct2: 0x0066BBA0
 */
static void window_game_bottom_toolbar_invalidate(rct_window *w)
{
    sint32 x;
    NewsItem *newsItem;

    // Anchor the middle and right panel to the right
    x = context_get_width();
    w->width = x;
    x--;
    window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].right = x;
    x -= 2;
    window_game_bottom_toolbar_widgets[WIDX_RIGHT_INSET].right = x;
    x -= (115 + WIDTH_MOD);
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

    window_game_bottom_toolbar_widgets[WIDX_LEFT_INSET].type = WWT_EMPTY;
    window_game_bottom_toolbar_widgets[WIDX_RIGHT_INSET].type = WWT_EMPTY;

    if (news_item_is_queue_empty()) {
        if (!(theme_get_flags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR)) {
            window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].type = WWT_EMPTY;
            window_game_bottom_toolbar_widgets[WIDX_MIDDLE_INSET].type = WWT_EMPTY;
            window_game_bottom_toolbar_widgets[WIDX_NEWS_SUBJECT].type = WWT_EMPTY;
            window_game_bottom_toolbar_widgets[WIDX_NEWS_LOCATE].type = WWT_EMPTY;
        } else {
            window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].type = WWT_IMGBTN;
            window_game_bottom_toolbar_widgets[WIDX_MIDDLE_INSET].type = WWT_25;
            window_game_bottom_toolbar_widgets[WIDX_NEWS_SUBJECT].type = WWT_EMPTY;
            window_game_bottom_toolbar_widgets[WIDX_NEWS_LOCATE].type = WWT_EMPTY;
            window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].colour = 0;
            window_game_bottom_toolbar_widgets[WIDX_MIDDLE_INSET].colour = 0;
        }
    } else {
        newsItem = news_item_get(0);
        window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].type = WWT_IMGBTN;
        window_game_bottom_toolbar_widgets[WIDX_MIDDLE_INSET].type = WWT_25;
        window_game_bottom_toolbar_widgets[WIDX_NEWS_SUBJECT].type = WWT_FLATBTN;
        window_game_bottom_toolbar_widgets[WIDX_NEWS_LOCATE].type = WWT_FLATBTN;
        window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].colour = 2;
        window_game_bottom_toolbar_widgets[WIDX_MIDDLE_INSET].colour = 2;
        w->disabled_widgets &= ~(1 << WIDX_NEWS_SUBJECT);
        w->disabled_widgets &= ~(1 << WIDX_NEWS_LOCATE);

        // Find out if the news item is no longer valid
        sint32 y, z;
        sint32 subject = newsItem->Assoc;
        news_item_get_subject_location(newsItem->Type, subject, &x, &y, &z);

        if (x == SPRITE_LOCATION_NULL)
            w->disabled_widgets |= (1 << WIDX_NEWS_LOCATE);

        if (!(news_type_properties[newsItem->Type] & NEWS_TYPE_HAS_SUBJECT)) {
            w->disabled_widgets |= (1 << WIDX_NEWS_SUBJECT);
            window_game_bottom_toolbar_widgets[WIDX_NEWS_SUBJECT].type = WWT_EMPTY;
        }

        if (newsItem->Flags & NEWS_FLAG_HAS_BUTTON) {
            w->disabled_widgets |= (1 << WIDX_NEWS_SUBJECT);
            w->disabled_widgets |= (1 << WIDX_NEWS_LOCATE);
        }
    }

    // Hide money if there is no money
    if (gParkFlags & PARK_FLAGS_NO_MONEY) {
        window_game_bottom_toolbar_widgets[WIDX_MONEY].type = WWT_EMPTY;
        window_game_bottom_toolbar_widgets[WIDX_GUESTS].top = 1;
        window_game_bottom_toolbar_widgets[WIDX_GUESTS].bottom = 17;
        window_game_bottom_toolbar_widgets[WIDX_PARK_RATING].top = 17;
    } else {
        window_game_bottom_toolbar_widgets[WIDX_MONEY].type = WWT_FLATBTN;
        window_game_bottom_toolbar_widgets[WIDX_GUESTS].top = 11;
        window_game_bottom_toolbar_widgets[WIDX_GUESTS].bottom = 22;
        window_game_bottom_toolbar_widgets[WIDX_PARK_RATING].top = 21;
    }
}

/**
 *
 *  rct2: 0x0066BB79
 */
void window_game_bottom_toolbar_invalidate_news_item()
{
    if (gScreenFlags == SCREEN_FLAGS_PLAYING) {
        widget_invalidate_by_class(WC_BOTTOM_TOOLBAR, WIDX_MIDDLE_OUTSET);
    }
}

/**
 *
 *  rct2: 0x0066BC87
 */
static void window_game_bottom_toolbar_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    // Draw panel grey backgrounds
    gfx_filter_rect(
        dpi,
        w->x + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].left,
        w->y + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].top,
        w->x + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].right,
        w->y + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].bottom,
        PALETTE_51
    );
    gfx_filter_rect(
        dpi,
        w->x + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].left,
        w->y + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].top,
        w->x + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].right,
        w->y + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].bottom,
        PALETTE_51
    );
    if (theme_get_flags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR)
    {
        // Draw grey background
        gfx_filter_rect(
            dpi,
            w->x + window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].left,
            w->y + window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].top,
            w->x + window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].right,
            w->y + window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET].bottom,
            PALETTE_51
        );
    }

    window_draw_widgets(w, dpi);

    window_game_bottom_toolbar_draw_left_panel(dpi, w);
    window_game_bottom_toolbar_draw_right_panel(dpi, w);

    if (!news_item_is_queue_empty())
    {
        window_game_bottom_toolbar_draw_news_item(dpi, w);
    }
    else if (theme_get_flags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR)
    {
        window_game_bottom_toolbar_draw_middle_panel(dpi, w);
    }
}

static void window_game_bottom_toolbar_draw_left_panel(rct_drawpixelinfo *dpi, rct_window *w)
{
    sint32 x, y;

    // Draw green inset rectangle on panel
    gfx_fill_rect_inset(
        dpi,
        w->x + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].left + 1,
        w->y + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].top + 1,
        w->x + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].right - 1,
        w->y + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].bottom - 1,
        w->colours[1],
        INSET_RECT_F_30
    );

    x = (window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].left + window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].right) / 2 + w->x;
    y = window_game_bottom_toolbar_widgets[WIDX_LEFT_OUTSET].top + w->y + 4;

    // Draw money
    if (!(gParkFlags & PARK_FLAGS_NO_MONEY)) {
        money32 cash = DECRYPT_MONEY(gCashEncrypted);
        set_format_arg(0, money32, cash);
        gfx_draw_string_centred(
            dpi,
            (cash < 0 ? STR_BOTTOM_TOOLBAR_CASH_NEGATIVE : STR_BOTTOM_TOOLBAR_CASH),
            x, y - 3,
            (gHoverWidget.window_classification == WC_BOTTOM_TOOLBAR && gHoverWidget.widget_index == WIDX_MONEY ? COLOUR_WHITE : w->colours[0] & 0x7F),
            gCommonFormatArgs
            );
        y += 7;
    }

    static const rct_string_id guestCountFormats[] = {
        STR_BOTTOM_TOOLBAR_NUM_GUESTS_STABLE,
        STR_BOTTOM_TOOLBAR_NUM_GUESTS_DECREASE,
        STR_BOTTOM_TOOLBAR_NUM_GUESTS_INCREASE,
    };

    // Draw guests
    gfx_draw_string_centred(
        dpi,
        guestCountFormats[gGuestChangeModifier],
        x, y,
        (gHoverWidget.window_classification == WC_BOTTOM_TOOLBAR && gHoverWidget.widget_index == WIDX_GUESTS ? COLOUR_WHITE : NOT_TRANSLUCENT(w->colours[0])),
        &gNumGuestsInPark
    );

    // Draw park rating
    window_game_bottom_toolbar_draw_park_rating(
        dpi,
        w,
        w->colours[3],
        w->x + window_game_bottom_toolbar_widgets[WIDX_PARK_RATING].left + 11,
        w->y + window_game_bottom_toolbar_widgets[WIDX_PARK_RATING].top,
        Math::Max(10, ((gParkRating / 4) * 263) / 256)
    );
}

/**
 *
 *  rct2: 0x0066C76C
 */
static void window_game_bottom_toolbar_draw_park_rating(rct_drawpixelinfo *dpi, rct_window *w, sint32 colour, sint32 x, sint32 y, uint8 factor)
{
    sint16 bar_width;

    bar_width = (factor * (92 + WIDTH_MOD)) / 255;
    gfx_fill_rect_inset(dpi, x + 1, y + 1, x + (92 + WIDTH_MOD), y + 9, w->colours[1], INSET_RECT_F_30);
    if (!(colour & IMAGE_TYPE_REMAP_2_PLUS) || game_is_paused() || (gCurrentTicks & 8)) {
        if (bar_width > 2)
            gfx_fill_rect_inset(dpi, x + 2, y + 2, x + bar_width - 1, y + 8, colour & 0x7FFFFFFF, 0);
    }

    // Draw thumbs on the sides
    gfx_draw_sprite(dpi, SPR_RATING_LOW, x - 14, y, 0);
    gfx_draw_sprite(dpi, SPR_RATING_HIGH, x + (92 + WIDTH_MOD), y, 0);
}

static void window_game_bottom_toolbar_draw_right_panel(rct_drawpixelinfo *dpi, rct_window *w)
{
    sint32 x, y, temperature;
    rct_string_id format;

    // Draw green inset rectangle on panel
    gfx_fill_rect_inset(
        dpi,
        w->x + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].left + 1,
        w->y + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].top + 1,
        w->x + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].right - 1,
        w->y + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].bottom - 1,
        w->colours[1],
        INSET_RECT_F_30
    );

    x = (window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].left + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].right) / 2 + w->x;
    y = window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].top + w->y + 2;

    // Date
    sint32 year = date_get_year(gDateMonthsElapsed) + 1;
    sint32 month = date_get_month(gDateMonthsElapsed & 7);
    sint32 day = ((gDateMonthTicks * days_in_month[month]) >> 16) & 0xFF;

    rct_string_id stringId = DateFormatStringFormatIds[gConfigGeneral.date_format];
    set_format_arg(0, rct_string_id, DateDayNames[day]);
    set_format_arg(2, sint16, month);
    set_format_arg(4, sint16, year);
    gfx_draw_string_centred(
        dpi,
        stringId,
        x,
        y,
        (gHoverWidget.window_classification == WC_BOTTOM_TOOLBAR && gHoverWidget.widget_index == WIDX_DATE ? COLOUR_WHITE : NOT_TRANSLUCENT(w->colours[0])),
        gCommonFormatArgs
    );

    // Temperature
    x = w->x + window_game_bottom_toolbar_widgets[WIDX_RIGHT_OUTSET].left + 15;
    y += 11;

    temperature = gClimateCurrentTemperature;
    format = STR_CELSIUS_VALUE;
    if (gConfigGeneral.temperature_format == TEMPERATURE_FORMAT_F) {
        temperature = climate_celsius_to_fahrenheit(temperature);
        format = STR_FAHRENHEIT_VALUE;
    }
    set_format_arg(0, sint16, temperature);
    gfx_draw_string_left(dpi, format, gCommonFormatArgs, COLOUR_BLACK, x, y + 6);
    x += 30;

    // Current weather
    gfx_draw_sprite(dpi, ClimateWeatherData[gClimateCurrentWeather].SpriteId, x, y, 0);

    // Next weather
    if (ClimateWeatherData[gClimateCurrentWeather].SpriteId != ClimateWeatherData[gClimateNextWeather].SpriteId) {
        if (gClimateUpdateTimer < 960) {
            gfx_draw_sprite(dpi, SPR_NEXT_WEATHER, x + 27, y + 5, 0);
            gfx_draw_sprite(dpi, ClimateWeatherData[gClimateNextWeather].SpriteId, x + 40, y, 0);
        }
    }
}

/**
 *
 *  rct2: 0x0066BFA5
 */
static void window_game_bottom_toolbar_draw_news_item(rct_drawpixelinfo *dpi, rct_window *w)
{
    sint32 x, y, width;
    NewsItem *newsItem;
    rct_widget *middleOutsetWidget;

    middleOutsetWidget = &window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET];
    newsItem = news_item_get(0);

    // Current news item
    gfx_fill_rect_inset(
        dpi,
        w->x + middleOutsetWidget->left + 1,
        w->y + middleOutsetWidget->top + 1,
        w->x + middleOutsetWidget->right - 1,
        w->y + middleOutsetWidget->bottom - 1,
        w->colours[2],
        INSET_RECT_F_30
    );

    // Text
    utf8 *newsItemText = newsItem->Text;
    x = w->x + (middleOutsetWidget->left + middleOutsetWidget->right) / 2;
    y = w->y + middleOutsetWidget->top + 11;
    width = middleOutsetWidget->right - middleOutsetWidget->left - 62;
    gfx_draw_string_centred_wrapped_partial(dpi, x, y, width, COLOUR_BRIGHT_GREEN, STR_BOTTOM_TOOLBAR_NEWS_TEXT, &newsItemText, newsItem->Ticks);

    x = w->x + window_game_bottom_toolbar_widgets[WIDX_NEWS_SUBJECT].left;
    y = w->y + window_game_bottom_toolbar_widgets[WIDX_NEWS_SUBJECT].top;
    switch (newsItem->Type) {
    case NEWS_ITEM_RIDE:
        gfx_draw_sprite(dpi, SPR_RIDE, x, y, 0);
        break;
    case NEWS_ITEM_PEEP_ON_RIDE:
    case NEWS_ITEM_PEEP:
    {
        if (newsItem->Flags & NEWS_FLAG_HAS_BUTTON)
            break;

        rct_drawpixelinfo cliped_dpi;
        if (!clip_drawpixelinfo(&cliped_dpi, dpi, x + 1, y + 1, 22, 22)) {
            break;
        }

        rct_peep* peep = GET_PEEP(newsItem->Assoc);
        sint32 clip_x = 10, clip_y = 19;

        if (peep->type == PEEP_TYPE_STAFF){
            if (peep->staff_type == STAFF_TYPE_ENTERTAINER){
                clip_y += 3;
            }
        }

        uint32 image_id_base = g_peep_animation_entries[peep->sprite_type].sprite_animation->base_image;
        image_id_base += w->frame_no & 0xFFFFFFFC;
        image_id_base++;

        uint32 image_id = image_id_base;
        image_id |= SPRITE_ID_PALETTE_COLOUR_2(peep->tshirt_colour, peep->trousers_colour);

        gfx_draw_sprite(&cliped_dpi, image_id, clip_x, clip_y, 0);

        if (image_id_base >= 0x2A1D && image_id_base < 0x2A3D){
            image_id_base += 32;
            image_id_base |= SPRITE_ID_PALETTE_COLOUR_1(peep->balloon_colour);

            gfx_draw_sprite(&cliped_dpi, image_id_base, clip_x, clip_y, 0);
        }
        else if (image_id_base >= 0x2BBD && image_id_base < 0x2BDD){
            image_id_base += 32;
            image_id_base |= SPRITE_ID_PALETTE_COLOUR_1(peep->umbrella_colour);

            gfx_draw_sprite(&cliped_dpi, image_id_base, clip_x, clip_y, 0);
        }
        else if (image_id_base >= 0x29DD && image_id_base < 0x29FD){
            image_id_base += 32;
            image_id_base |= SPRITE_ID_PALETTE_COLOUR_1(peep->hat_colour);

            gfx_draw_sprite(&cliped_dpi, image_id_base, clip_x, clip_y, 0);
        }
        break;
    }
    case NEWS_ITEM_MONEY:
        gfx_draw_sprite(dpi, SPR_FINANCE, x, y, 0);
        break;
    case NEWS_ITEM_RESEARCH:
        gfx_draw_sprite(dpi, (newsItem->Assoc < 0x10000 ? SPR_NEW_SCENERY : SPR_NEW_RIDE), x, y, 0);
        break;
    case NEWS_ITEM_PEEPS:
        gfx_draw_sprite(dpi, SPR_GUESTS, x, y, 0);
        break;
    case NEWS_ITEM_AWARD:
        gfx_draw_sprite(dpi, SPR_AWARD, x, y, 0);
        break;
    case NEWS_ITEM_GRAPH:
        gfx_draw_sprite(dpi, SPR_GRAPH, x, y, 0);
        break;
    }
}

static void window_game_bottom_toolbar_draw_middle_panel(rct_drawpixelinfo *dpi, rct_window *w)
{
    rct_widget *middleOutsetWidget = &window_game_bottom_toolbar_widgets[WIDX_MIDDLE_OUTSET];

    gfx_fill_rect_inset(
        dpi,
        w->x + middleOutsetWidget->left + 1,
        w->y + middleOutsetWidget->top + 1,
        w->x + middleOutsetWidget->right - 1,
        w->y + middleOutsetWidget->bottom - 1,
        w->colours[1],
        INSET_RECT_F_30
    );

    sint32 x = w->x + (middleOutsetWidget->left + middleOutsetWidget->right) / 2;
    sint32 y = w->y + middleOutsetWidget->top + 11;
    sint32 width = middleOutsetWidget->right - middleOutsetWidget->left - 62;

    // Check if there is a map tooltip to draw
    rct_string_id stringId;
    memcpy(&stringId, gMapTooltipFormatArgs, sizeof(rct_string_id));
    if (stringId == STR_NONE)
    {
        gfx_draw_string_centred_wrapped(dpi, gMapTooltipFormatArgs, x, y, width, STR_TITLE_SEQUENCE_OPENRCT2, w->colours[0]);
    }
    else
    {
        // Show tooltip in bottom toolbar
        gfx_draw_string_centred_wrapped(dpi, gMapTooltipFormatArgs, x, y, width, STR_STRINGID, w->colours[0]);
    }
}

/**
 *
 *  rct2: 0x0066C6D8
 */
static void window_game_bottom_toolbar_update(rct_window* w){

    w->frame_no++;
    if (w->frame_no >= 24)
        w->frame_no = 0;

    window_game_bottom_toolbar_invalidate_dirty_widgets(w);
}

/**
 *
 *  rct2: 0x0066C644
 */
static void window_game_bottom_toolbar_cursor(rct_window *w, rct_widgetindex widgetIndex, sint32 x, sint32 y, sint32 *cursorId)
{
    switch (widgetIndex) {
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
static void window_game_bottom_toolbar_unknown05(rct_window *w)
{
    window_game_bottom_toolbar_invalidate_dirty_widgets(w);
}

/**
 *
 *  rct2: 0x0066C6F2
 */
static void window_game_bottom_toolbar_invalidate_dirty_widgets(rct_window *w)
{
    if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_MONEY){
        gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_MONEY;
        widget_invalidate(w, WIDX_LEFT_INSET);
    }

    if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_DATE){
        gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_DATE;
        widget_invalidate(w, WIDX_RIGHT_INSET);
    }

    if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_PEEP_COUNT){
        gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_PEEP_COUNT;
        widget_invalidate(w, WIDX_LEFT_INSET);
    }

    if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_CLIMATE){
        gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_CLIMATE;
        widget_invalidate(w, WIDX_RIGHT_INSET);
    }

    if (gToolbarDirtyFlags & BTM_TB_DIRTY_FLAG_PARK_RATING){
        gToolbarDirtyFlags &= ~BTM_TB_DIRTY_FLAG_PARK_RATING;
        widget_invalidate(w, WIDX_LEFT_INSET);
    }
}
