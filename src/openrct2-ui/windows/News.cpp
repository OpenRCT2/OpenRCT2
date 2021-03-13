/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#include <openrct2/audio/audio.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/management/NewsItem.h>
#include <openrct2/peep/Peep.h>
#include <openrct2/peep/Staff.h>
#include <openrct2/sprites.h>
#include <openrct2/world/Sprite.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_RECENT_MESSAGES;
static constexpr const int32_t WH = 300;
static constexpr const int32_t WW = 400;

// clang-format off
enum WINDOW_NEWS_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_SETTINGS,
    WIDX_SCROLL
};

static rct_widget window_news_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({372, 18}, { 24,  24}, WindowWidgetType::FlatBtn, WindowColour::Primary, SPR_TAB_GEARS_0), // settings
    MakeWidget({  4, 44}, {392, 252}, WindowWidgetType::Scroll,  WindowColour::Primary, SCROLL_VERTICAL), // scroll
    { WIDGETS_END },
};

static void window_news_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_news_update(rct_window *w);
static void window_news_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_news_scrollmousedown(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_news_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_news_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static rct_window_event_list window_news_events([](auto& events)
{
    events.mouse_up = &window_news_mouseup;
    events.update = &window_news_update;
    events.get_scroll_size = &window_news_scrollgetsize;
    events.scroll_mousedown = &window_news_scrollmousedown;
    events.paint = &window_news_paint;
    events.scroll_paint = &window_news_scrollpaint;
});
// clang-format on

/**
 *
 *  rct2: 0x0066E464
 */
rct_window* window_news_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_RECENT_NEWS);
    if (window == nullptr)
    {
        window = WindowCreateAutoPos(400, 300, &window_news_events, WC_RECENT_NEWS, 0);
        window->widgets = window_news_widgets;
        window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_SETTINGS);
        WindowInitScrollWidgets(window);
        window->news.var_480 = -1;
    }

    // sub_66E4BA:
    rct_widget* widget;

    int32_t width = 0;
    int32_t height = 0;
    window_get_scroll_size(window, 0, &width, &height);
    widget = &window_news_widgets[WIDX_SCROLL];
    window->scrolls[0].v_top = std::max(0, height - (widget->height() - 1));
    WidgetScrollUpdateThumbs(window, WIDX_SCROLL);

    return window;
}

static int32_t window_news_get_item_height()
{
    return 4 * font_get_line_height(FontSpriteBase::SMALL) + 2;
}

/**
 *
 *  rct2: 0x0066D4D5
 */
static void window_news_mouseup(rct_window* w, rct_widgetindex widgetIndex)
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
static void window_news_update(rct_window* w)
{
    if (w->news.var_480 == -1 || --w->news.var_484 != 0)
    {
        return;
    }

    w->Invalidate();
    OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Click2, 0, w->windowPos.x + (w->width / 2));

    size_t j = w->news.var_480;
    w->news.var_480 = -1;
    if (j >= gNewsItems.GetArchived().size())
        return;

    const auto& newsItem = gNewsItems.GetArchived()[j];
    if (newsItem.HasButton())
        return;
    if (w->news.var_482 == 1)
    {
        News::OpenSubject(newsItem.Type, newsItem.Assoc);
    }
    else if (w->news.var_482 > 1)
    {
        auto subjectLoc = News::GetSubjectLocation(newsItem.Type, newsItem.Assoc);
        if (subjectLoc != std::nullopt && (w = window_get_main()) != nullptr)
        {
            window_scroll_to_location(w, *subjectLoc);
        }
    }
}

/**
 *
 *  rct2: 0x0066EA3C
 */
static void window_news_scrollgetsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    *height = static_cast<int32_t>(gNewsItems.GetArchived().size()) * window_news_get_item_height();
}

/**
 *
 *  rct2: 0x0066EA5C
 */
static void window_news_scrollmousedown(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    int32_t itemHeight = window_news_get_item_height();

    int32_t i = 0;
    int32_t buttonIndex = 0;
    auto mutableScreenCoords = screenCoords;
    for (const auto& newsItem : gNewsItems.GetArchived())
    {
        if (mutableScreenCoords.y < itemHeight)
        {
            if (newsItem.HasButton() || mutableScreenCoords.y < 14 || mutableScreenCoords.y >= 38
                || mutableScreenCoords.x < 328)
            {
                buttonIndex = 0;
                break;
            }
            else if (mutableScreenCoords.x < 351 && newsItem.TypeHasSubject())
            {
                buttonIndex = 1;
                break;
            }
            else if (mutableScreenCoords.x < 376 && newsItem.TypeHasLocation())
            {
                buttonIndex = 2;
                break;
            }
        }
        mutableScreenCoords.y -= itemHeight;
        i++;
    }

    if (buttonIndex != 0)
    {
        w->news.var_480 = i;
        w->news.var_482 = buttonIndex;
        w->news.var_484 = 4;
        w->Invalidate();
        OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Click1, 0, w->windowPos.x + (w->width / 2));
    }
}

/**
 *
 *  rct2: 0x0066E4E8
 */
static void window_news_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
}

/**
 *
 *  rct2: 0x0066E4EE
 */
static void window_news_scrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    int32_t lineHeight = font_get_line_height(FontSpriteBase::SMALL);
    int32_t itemHeight = window_news_get_item_height();

    int32_t y = 0;
    int32_t i = 0;
    for (const auto& newsItem : gNewsItems.GetArchived())
    {
        if (y >= dpi->y + dpi->height)
            break;
        if (y + itemHeight < dpi->y)
        {
            y += itemHeight;
            i++;
            continue;
        }

        // Background
        gfx_fill_rect_inset(
            dpi, { -1, y, 383, y + itemHeight - 1 }, w->colours[1], (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_GREY));

        // Date text
        {
            auto ft = Formatter();
            ft.Add<rct_string_id>(DateDayNames[newsItem.Day - 1]);
            ft.Add<rct_string_id>(DateGameMonthNames[date_get_month(newsItem.MonthYear)]);
            DrawTextBasic(dpi, { 2, y }, STR_NEWS_DATE_FORMAT, ft, { COLOUR_WHITE, FontSpriteBase::SMALL });
        }
        // Item text
        {
            auto ft = Formatter();
            ft.Add<const char*>(newsItem.Text.c_str());
            DrawTextWrapped(dpi, { 2, y + lineHeight }, 325, STR_BOTTOM_TOOLBAR_NEWS_TEXT, ft, { FontSpriteBase::SMALL });
        }
        // Subject button
        if ((newsItem.TypeHasSubject()) && !(newsItem.HasButton()))
        {
            auto screenCoords = ScreenCoordsXY{ 328, y + lineHeight + 4 };

            int32_t press = 0;
            if (w->news.var_480 != -1)
            {
                News::IsValidIndex(w->news.var_480 + News::ItemHistoryStart);
                if (i == w->news.var_480 && w->news.var_482 == 1)
                    press = INSET_RECT_FLAG_BORDER_INSET;
            }
            gfx_fill_rect_inset(dpi, { screenCoords, screenCoords + ScreenCoordsXY{ 23, 23 } }, w->colours[2], press);

            switch (newsItem.Type)
            {
                case News::ItemType::Ride:
                    gfx_draw_sprite(dpi, SPR_RIDE, screenCoords, 0);
                    break;
                case News::ItemType::Peep:
                case News::ItemType::PeepOnRide:
                {
                    rct_drawpixelinfo cliped_dpi;
                    if (!clip_drawpixelinfo(&cliped_dpi, dpi, screenCoords + ScreenCoordsXY{ 1, 1 }, 22, 22))
                    {
                        break;
                    }

                    auto peep = TryGetEntity<Peep>(newsItem.Assoc);
                    if (peep == nullptr)
                        break;

                    auto clipCoords = ScreenCoordsXY{ 10, 19 };

                    // If normal peep set sprite to normal (no food)
                    // If staff set sprite to staff sprite
                    auto spriteType = PeepSpriteType::Normal;
                    if (peep->AssignedPeepType == PeepType::Staff)
                    {
                        spriteType = peep->SpriteType;
                        if (peep->AssignedStaffType == StaffType::Entertainer)
                        {
                            clipCoords.y += 3;
                        }
                    }

                    uint32_t image_id = GetPeepAnimation(spriteType).base_image;
                    image_id += 0xA0000001;
                    image_id |= (peep->TshirtColour << 19) | (peep->TrousersColour << 24);

                    gfx_draw_sprite(&cliped_dpi, image_id, clipCoords, 0);
                    break;
                }
                case News::ItemType::Money:
                    gfx_draw_sprite(dpi, SPR_FINANCE, screenCoords, 0);
                    break;
                case News::ItemType::Research:
                    gfx_draw_sprite(dpi, newsItem.Assoc < 0x10000 ? SPR_NEW_SCENERY : SPR_NEW_RIDE, screenCoords, 0);
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

        // Location button
        if ((newsItem.TypeHasLocation()) && !(newsItem.HasButton()))
        {
            auto screenCoords = ScreenCoordsXY{ 352, y + lineHeight + 4 };

            int32_t press = 0;
            if (w->news.var_480 != -1)
            {
                News::IsValidIndex(w->news.var_480 + News::ItemHistoryStart);
                if (i == w->news.var_480 && w->news.var_482 == 2)
                    press = 0x20;
            }
            gfx_fill_rect_inset(dpi, { screenCoords, screenCoords + ScreenCoordsXY{ 23, 23 } }, w->colours[2], press);
            gfx_draw_sprite(dpi, SPR_LOCATE, screenCoords, 0);
        }

        y += itemHeight;
        i++;
    }
}
