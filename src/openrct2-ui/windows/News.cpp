/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/core/Math.hpp>
#include <openrct2/Context.h>
#include <openrct2-ui/windows/Window.h>

#include <openrct2/audio/audio.h>
#include <openrct2/management/NewsItem.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/world/Sprite.h>
#include <openrct2/peep/Staff.h>
#include <openrct2/sprites.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2/drawing/Drawing.h>

// clang-format off
enum WINDOW_NEWS_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_SETTINGS,
    WIDX_SCROLL
};

static rct_widget window_news_widgets[] = {
    { WWT_FRAME,            0,  0,          399,    0,      299,    0xFFFFFFFF,             STR_NONE },             // panel / background
    { WWT_CAPTION,          0,  1,          398,    1,      14,     STR_RECENT_MESSAGES,    STR_WINDOW_TITLE_TIP }, // title bar
    { WWT_CLOSEBOX,         0,  387,        397,    2,      13,     STR_CLOSE_X,            STR_CLOSE_WINDOW_TIP }, // close x button
    { WWT_FLATBTN,          0,  372,        395,    18,     41,     SPR_TAB_GEARS_0,        STR_NONE },             // settings
    { WWT_SCROLL,           0,  4,          395,    44,     295,    SCROLL_VERTICAL,                        STR_NONE },             // scroll
    { WIDGETS_END },
};

static void window_news_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_news_update(rct_window *w);
static void window_news_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_news_scrollmousedown(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y);
static void window_news_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId);
static void window_news_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_news_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static rct_window_event_list window_news_events = {
    nullptr,
    window_news_mouseup,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_news_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_news_scrollgetsize,
    window_news_scrollmousedown,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_news_tooltip,
    nullptr,
    nullptr,
    nullptr,
    window_news_paint,
    window_news_scrollpaint
};
// clang-format on

/**
 *
 *  rct2: 0x0066E464
 */
rct_window * window_news_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_RECENT_NEWS);
    if (window == nullptr) {
        window = window_create_auto_pos(
            400,
            300,
            &window_news_events,
            WC_RECENT_NEWS,
            0
        );
        window->widgets = window_news_widgets;
        window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_SETTINGS);
        window_init_scroll_widgets(window);
        window->news.var_480 = -1;
    }

// sub_66E4BA:
    rct_widget *widget;

    int32_t width = 0;
    int32_t height = 0;
    window_get_scroll_size(window, 0, &width, &height);
    widget = &window_news_widgets[WIDX_SCROLL];
    window->scrolls[0].v_top = std::max(0, height - (widget->bottom - widget->top - 1));
    widget_scroll_update_thumbs(window, WIDX_SCROLL);

    return window;
}

static int32_t window_news_get_item_height()
{
    return 4 * font_get_line_height(gCurrentFontSpriteBase) + 2;
}

/**
 *
 *  rct2: 0x0066D4D5
 */
static void window_news_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_SETTINGS:
        context_open_window(WC_NOTIFICATION_OPTIONS);
        break;
    }
}

/**
 *
 *  rct2: 0x0066EAB8
 */
static void window_news_update(rct_window *w)
{
    int32_t i, j, x, y, z;

    if (w->news.var_480 == -1 ||
        --w->news.var_484 != 0)
    {
        return;
    }

    window_invalidate(w);
    audio_play_sound(SOUND_CLICK_2, 0, w->x + (w->width / 2));

    j = w->news.var_480;
    w->news.var_480 = -1;
    for (i = 11; i < 61; i++)
    {
        if (news_item_is_empty(i))
            return;

        if (j == 0)
        {
            NewsItem * const newsItem = news_item_get(i);
            if (newsItem->Flags & NEWS_FLAG_HAS_BUTTON)
                return;
            if (w->news.var_482 == 1)
            {
                news_item_open_subject(newsItem->Type, newsItem->Assoc);
                return;
            }
            else if (w->news.var_482 > 1)
            {
                news_item_get_subject_location(newsItem->Type, newsItem->Assoc, &x, &y, &z);
                if (x != LOCATION_NULL && (w = window_get_main()) != nullptr)
                {
                    window_scroll_to_location(w, x, y, z);
                }
                return;
            }
        }
        j--;
    }
}

/**
 *
 *  rct2: 0x0066EA3C
 */
static void window_news_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height)
{
    int32_t itemHeight = window_news_get_item_height();

    *height = 0;
    for (int32_t i = 11; i < 61; i++)
    {
        if (news_item_is_empty(i))
            break;

        *height += itemHeight;
    }
}

/**
 *
 *  rct2: 0x0066EA5C
 */
static void window_news_scrollmousedown(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y)
{
    int32_t itemHeight = window_news_get_item_height();
    int32_t i, buttonIndex;

    buttonIndex = 0;
    for (i = 11; i < 61; i++)
    {
        if (news_item_is_empty(i))
            break;

        if (y < itemHeight)
        {
            NewsItem * const newsItem = news_item_get(i);
            if (newsItem->Flags & NEWS_FLAG_HAS_BUTTON ||
                y < 14 || y >= 38 ||
                x < 328)
            {
                buttonIndex = 0;
                break;
            }
            else if (x < 351 && news_type_properties[newsItem->Type] & NEWS_TYPE_HAS_SUBJECT)
            {
                buttonIndex = 1;
                break;
            }
            else if (x < 376 && news_type_properties[newsItem->Type] & NEWS_TYPE_HAS_LOCATION)
            {
                buttonIndex = 2;
                break;
            }
        }
        y -= itemHeight;
    }

    if (buttonIndex != 0)
    {
        w->news.var_480 = i - 11;
        w->news.var_482 = buttonIndex;
        w->news.var_484 = 4;
        window_invalidate(w);
        audio_play_sound(SOUND_CLICK_1, 0, w->x + (w->width / 2));
    }
}

/**
 *
 *  rct2: 0x0066EAAE
 */
static void window_news_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId)
{
    set_format_arg(0, rct_string_id, STR_LIST);
}

/**
 *
 *  rct2: 0x0066E4E8
 */
static void window_news_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
}

/**
 *
 *  rct2: 0x0066E4EE
 */
static void window_news_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex)
{
    int32_t lineHeight = font_get_line_height(gCurrentFontSpriteBase);
    int32_t itemHeight = window_news_get_item_height();
    int32_t i, x, y, yy, press;

    y = 0;
    for (i = 11; i < 61; i++)
    {
        NewsItem * const newsItem = news_item_get(i);
        if (news_item_is_empty(i))
            break;
        if (y >= dpi->y + dpi->height)
            break;
        if (y + itemHeight < dpi->y)
        {
            y += itemHeight;
            continue;
        }

        // Background
        gfx_fill_rect_inset(dpi, -1, y, 383, y + itemHeight - 1, w->colours[1], (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_GREY));

        // Date text
        set_format_arg(0, rct_string_id, DateDayNames[newsItem->Day - 1]);
        set_format_arg(2, rct_string_id, DateGameMonthNames[date_get_month(newsItem->MonthYear)]);
        gfx_draw_string_left(dpi, STR_NEWS_DATE_FORMAT, gCommonFormatArgs, COLOUR_WHITE, 2, y);

        // Item text
        utf8 *text = newsItem->Text;
        gfx_draw_string_left_wrapped(dpi, &text, 2, y + lineHeight, 325, STR_BOTTOM_TOOLBAR_NEWS_TEXT, COLOUR_BRIGHT_GREEN);

        // Subject button
        if ((news_type_properties[newsItem->Type] & NEWS_TYPE_HAS_SUBJECT) && !(newsItem->Flags & NEWS_FLAG_HAS_BUTTON))
        {
            x = 328;
            yy = y + lineHeight + 4;

            press = 0;
            if (w->news.var_480 != -1) {
                const uint8_t idx = 11 + w->news.var_480;
                news_item_is_valid_idx(idx);
                if (i == idx && w->news.var_482 == 1)
                    press = INSET_RECT_FLAG_BORDER_INSET;
            }
            gfx_fill_rect_inset(dpi, x, yy, x + 23, yy + 23, w->colours[2], press);

            switch (newsItem->Type) {
            case NEWS_ITEM_RIDE:
                gfx_draw_sprite(dpi, SPR_RIDE, x, yy, 0);
                break;
            case NEWS_ITEM_PEEP:
            case NEWS_ITEM_PEEP_ON_RIDE:
            {
                rct_drawpixelinfo cliped_dpi;
                if (!clip_drawpixelinfo(&cliped_dpi, dpi, x + 1, yy + 1, 22, 22)) {
                    break;
                }

                rct_peep* peep = GET_PEEP(newsItem->Assoc);
                int32_t clip_x = 10, clip_y = 19;

                // If normal peep set sprite to normal (no food)
                // If staff set sprite to staff sprite
                int32_t sprite_type = 0;
                if (peep->type == PEEP_TYPE_STAFF){
                    sprite_type = peep->sprite_type;
                    if (peep->staff_type == STAFF_TYPE_ENTERTAINER){
                        clip_y += 3;
                    }
                }

                uint32_t image_id = g_peep_animation_entries[sprite_type].sprite_animation->base_image;
                image_id += 0xA0000001;
                image_id |= (peep->tshirt_colour << 19) | (peep->trousers_colour << 24);

                gfx_draw_sprite(&cliped_dpi, image_id, clip_x, clip_y, 0);
                break;
            }
            case NEWS_ITEM_MONEY:
                gfx_draw_sprite(dpi, SPR_FINANCE, x, yy, 0);
                break;
            case NEWS_ITEM_RESEARCH:
                gfx_draw_sprite(dpi, newsItem->Assoc < 0x10000 ? SPR_NEW_SCENERY : SPR_NEW_RIDE, x, yy, 0);
                break;
            case NEWS_ITEM_PEEPS:
                gfx_draw_sprite(dpi, SPR_GUESTS, x, yy, 0);
                break;
            case NEWS_ITEM_AWARD:
                gfx_draw_sprite(dpi, SPR_AWARD, x, yy, 0);
                break;
            case NEWS_ITEM_GRAPH:
                gfx_draw_sprite(dpi, SPR_GRAPH, x, yy, 0);
                break;
            }
        }

        // Location button
        if ((news_type_properties[newsItem->Type] & NEWS_TYPE_HAS_LOCATION) && !(newsItem->Flags & NEWS_FLAG_HAS_BUTTON))
        {
            x = 352;
            yy = y + lineHeight + 4;

            press = 0;
            if (w->news.var_480 != -1) {
                const uint8_t idx = 11 + w->news.var_480;
                news_item_is_valid_idx(idx);
                if (i == idx && w->news.var_482 == 2)
                    press = 0x20;
            }
            gfx_fill_rect_inset(dpi, x, yy, x + 23, yy + 23, w->colours[2], press);
            gfx_draw_sprite(dpi, SPR_LOCATE, x, yy, 0);
        }

        y += itemHeight;
    }
}
