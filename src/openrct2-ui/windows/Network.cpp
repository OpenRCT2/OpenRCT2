/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <cmath>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Game.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/CircularBuffer.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/network/network.h>
#include <openrct2/platform/platform.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>

// clang-format off
enum {
    WINDOW_NETWORK_PAGE_INFORMATION,
};

static constexpr const int32_t WW = 450;
static constexpr const int32_t WH = 210;

enum WINDOW_NETWORK_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_RESIZE,
    WIDX_TAB1,
};

static rct_widget window_network_information_widgets[] = {
    WINDOW_SHIM(STR_NONE, WW, WH),
    MakeWidget({  0, 43}, {450, 167}, WWT_RESIZE,  WindowColour::Secondary), // content panel
    MakeTab   ({  3, 17}, STR_SHOW_SERVER_INFO_TIP                        ), // tab
    { WIDGETS_END }
};

static rct_widget *window_network_page_widgets[] = {
    window_network_information_widgets,
};

static constexpr const uint64_t window_network_page_enabled_widgets[] = {
    (1 << WIDX_CLOSE) | (1 << WIDX_TAB1),
};

static constexpr rct_string_id WindowNetworkPageTitles[] = {
    STR_NETWORK_INFORMATION_TITLE,
};

static void window_network_information_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_network_information_resize(rct_window *w);
static void window_network_information_update(rct_window *w);
static void window_network_information_invalidate(rct_window *w);
static void window_network_information_paint(rct_window *w, rct_drawpixelinfo *dpi);

struct NetworkHistory_t
{
    std::array<uint16_t, NETWORK_STATISTICS_GROUP_MAX> deltaBytesReceived;
    std::array<uint16_t, NETWORK_STATISTICS_GROUP_MAX> deltaBytesSent;
};

static NetworkStats_t _networkStats;
static NetworkHistory_t _networkLastDeltaStats;
static NetworkHistory_t _networkAccumulatedStats;

static float _graphMaxIn;
static float _graphMaxOut;

static float _bytesInSec;
static float _bytesOutSec;
static uint32_t _bytesIn;
static uint32_t _bytesOut;

static uint32_t _lastGraphUpdateTime;
static uint32_t _lastStatsUpdateTime;

static CircularBuffer<NetworkHistory_t, 128> _networkHistory;

static constexpr int32_t NetworkTrafficGroupColors[NETWORK_STATISTICS_GROUP_MAX] = {
    PALETTE_INDEX_21,
    PALETTE_INDEX_102,
    PALETTE_INDEX_138,
    PALETTE_INDEX_171,
};

static constexpr int32_t NetworkTrafficGroupNames[NETWORK_STATISTICS_GROUP_MAX] = {
    STR_NETWORK,
    STR_NETWORK_LEGEND_BASE,
    STR_NETWORK_LEGEND_COMMANDS,
    STR_NETWORK_LEGEND_MAPDATA,
};

static rct_window_event_list window_network_information_events = {
    nullptr,
    window_network_information_mouseup,
    window_network_information_resize,
    nullptr,
    nullptr,
    nullptr,
    window_network_information_update,
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
    nullptr,
    nullptr,
    nullptr,
    window_network_information_invalidate,
    window_network_information_paint,
    nullptr
};

static rct_window_event_list *window_network_page_events[] = {
    &window_network_information_events,
};
// clang-format on

static constexpr const int32_t window_network_animation_divisor[] = { 4, 4, 2, 2 };
static constexpr const int32_t window_network_animation_frames[] = { 8, 8, 7, 4 };

static void window_network_draw_tab_images(rct_window* w, rct_drawpixelinfo* dpi);
static void window_network_set_page(rct_window* w, int32_t page);

rct_window* window_network_open()
{
    // Check if window is already open
    rct_window* window = window_bring_to_front_by_class(WC_NETWORK);
    if (window == nullptr)
    {
        window = window_create_auto_pos(320, 144, &window_network_information_events, WC_NETWORK, WF_10 | WF_RESIZABLE);
        window_network_set_page(window, WINDOW_NETWORK_PAGE_INFORMATION);

        // Fill the buffer so it will start scrolling in.
        _networkHistory.clear();
        for (size_t i = 0; i < _networkHistory.capacity(); i++)
        {
            _networkHistory.push_back(NetworkHistory_t{});
        }
    }

    _networkStats = network_get_stats();
    _networkAccumulatedStats = {};

    return window;
}

static void window_network_set_page(rct_window* w, int32_t page)
{
    w->page = page;
    w->frame_no = 0;
    w->no_list_items = 0;
    w->selected_list_item = -1;

    w->enabled_widgets = window_network_page_enabled_widgets[page];
    w->hold_down_widgets = 0;
    w->event_handlers = window_network_page_events[page];
    w->pressed_widgets = 0;
    w->widgets = window_network_page_widgets[page];
    w->widgets[WIDX_TITLE].text = WindowNetworkPageTitles[page];

    window_event_resize_call(w);
    window_event_invalidate_call(w);
    window_init_scroll_widgets(w);
    w->Invalidate();
}

static void window_network_anchor_border_widgets(rct_window* w)
{
    w->widgets[WIDX_BACKGROUND].right = w->width - 1;
    w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_TITLE].right = w->width - 2;
    w->widgets[WIDX_RESIZE].right = w->width - 1;
    w->widgets[WIDX_RESIZE].bottom = w->height - 1;
    w->widgets[WIDX_CLOSE].left = w->width - 13;
    w->widgets[WIDX_CLOSE].right = w->width - 3;
}

static void window_network_set_pressed_tab(rct_window* w)
{
    for (int32_t i = 0; i < 2; i++)
    {
        w->pressed_widgets &= ~(1 << (WIDX_TAB1 + i));
    }
    w->pressed_widgets |= 1LL << (WIDX_TAB1 + w->page);
}

#pragma region Information page

static void window_network_information_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_TAB1:
            if (w->page != widgetIndex - WIDX_TAB1)
            {
                window_network_set_page(w, widgetIndex - WIDX_TAB1);
            }
            break;
    }
}

static void window_network_information_resize(rct_window* w)
{
    window_set_resize(w, WW, WH, WW * 4, WH * 4);
    window_network_anchor_border_widgets(w);
}

static void window_network_information_update(rct_window* w)
{
    w->frame_no++;
    widget_invalidate(w, WIDX_TAB1 + w->page);
    w->Invalidate();

    NetworkStats_t curStats = network_get_stats();

    uint32_t currentTicks = platform_get_ticks();

    float graphTimeElapsed = (currentTicks - _lastGraphUpdateTime) / 1000.0f;
    _lastGraphUpdateTime = currentTicks;

    for (int i = 0; i < NETWORK_STATISTICS_GROUP_MAX; i++)
    {
        uint32_t deltaBytesReceived = curStats.bytesReceived[i] - _networkStats.bytesReceived[i];
        uint32_t deltaBytesSent = curStats.bytesSent[i] - _networkStats.bytesSent[i];

        _networkLastDeltaStats.deltaBytesReceived[i] = deltaBytesReceived;
        _networkLastDeltaStats.deltaBytesSent[i] = deltaBytesSent;

        _networkAccumulatedStats.deltaBytesReceived[i] += deltaBytesReceived;
        _networkAccumulatedStats.deltaBytesSent[i] += deltaBytesSent;
    }

    float graphMaxIn = 0.0f;
    float graphMaxOut = 0.0f;

    for (size_t i = 0; i < _networkHistory.size(); i++)
    {
        const NetworkHistory_t& history = _networkHistory[i];
        for (int n = 1; n < NETWORK_STATISTICS_GROUP_MAX; n++)
        {
            graphMaxIn = static_cast<float>(std::max<uint32_t>(history.deltaBytesReceived[n], graphMaxIn));
            graphMaxOut = static_cast<float>(std::max<uint32_t>(history.deltaBytesSent[n], graphMaxOut));
        }
    }

    _graphMaxIn = flerp(_graphMaxIn, graphMaxIn, graphTimeElapsed * 4.0f);
    _graphMaxOut = flerp(_graphMaxOut, graphMaxOut, graphTimeElapsed * 4.0f);

    // Compute readable statistics.
    if (currentTicks - _lastStatsUpdateTime >= 1000)
    {
        float statsTimeElapsed = (currentTicks - _lastStatsUpdateTime) / 1000.0f;
        _lastStatsUpdateTime = currentTicks;

        _bytesIn = _networkAccumulatedStats.deltaBytesReceived[NETWORK_STATISTICS_GROUP_TOTAL];
        _bytesOut = _networkAccumulatedStats.deltaBytesSent[NETWORK_STATISTICS_GROUP_TOTAL];
        _bytesInSec = static_cast<double>(_bytesIn) / statsTimeElapsed;
        _bytesOutSec = static_cast<double>(_bytesOut) / statsTimeElapsed;

        _networkAccumulatedStats = {};
    }

    _networkStats = curStats;
    _networkHistory.push_back(_networkLastDeltaStats);
}

static void window_network_information_invalidate(rct_window* w)
{
    window_network_set_pressed_tab(w);
    window_network_anchor_border_widgets(w);
    window_align_tabs(w, WIDX_TAB1, WIDX_TAB1);
}

static void graph_draw_bar(rct_drawpixelinfo* dpi, int32_t x, int32_t y, int32_t height, int32_t width, int32_t colour)
{
    auto coords = ScreenCoordsXY{ x, y };
    gfx_fill_rect(dpi, { coords, coords + ScreenCoordsXY{ width, height } }, colour);
}

static void window_network_draw_graph(
    rct_window* w, rct_drawpixelinfo* dpi, int32_t x, int32_t y, int32_t height, int32_t width, int32_t barWidth, bool received)
{
    float dataMax = received ? _graphMaxIn : _graphMaxOut;

    // Draw box.
    auto right1 = ScreenCoordsXY{ x, y };
    auto right2 = ScreenCoordsXY{ x, y + height };
    gfx_draw_line(dpi, { right1, right2 }, COLOUR_BLACK);

    auto left1 = ScreenCoordsXY{ x, y + height };
    auto left2 = ScreenCoordsXY{ x + width, y + height };
    gfx_draw_line(dpi, { left1, left2 }, COLOUR_BLACK);

    auto bottom1 = ScreenCoordsXY{ x, y };
    auto bottom2 = ScreenCoordsXY{ x + width, y };
    gfx_draw_line(dpi, { bottom1, bottom2 }, COLOUR_BLACK);

    auto top1 = ScreenCoordsXY{ x + width, y };
    auto top2 = ScreenCoordsXY{ x + width, y + height };
    gfx_draw_line(dpi, { top1, top2 }, COLOUR_BLACK);

    // Draw graph inside box
    x = x + 1;
    y = y + 1;
    width = width - 2;
    height = height - 2;

    rct_drawpixelinfo clippedDPI;
    if (!clip_drawpixelinfo(&clippedDPI, dpi, { x, y }, width, height))
        return;

    dpi = &clippedDPI;

    for (size_t i = 0; i < _networkHistory.size(); i++)
    {
        NetworkHistory_t history = _networkHistory[i];
        // std::sort(history.deltaBytesReceived.begin(), history.deltaBytesReceived.end(), std::greater<uint16_t>());

        // NOTE: Capacity is not a mistake, we always want the full length.
        uint32_t curX = std::round((static_cast<float>(i) / static_cast<float>(_networkHistory.capacity())) * barWidth * width);

        float totalSum = 0.0f;
        for (int n = 1; n < NETWORK_STATISTICS_GROUP_MAX; n++)
        {
            if (received)
                totalSum += static_cast<float>(history.deltaBytesReceived[n]);
            else
                totalSum += static_cast<float>(history.deltaBytesSent[n]);
        }

        int32_t yOffset = height;
        for (int n = 1; n < NETWORK_STATISTICS_GROUP_MAX; n++)
        {
            float totalHeight;
            float singleHeight;

            if (received)
            {
                totalHeight = (static_cast<float>(history.deltaBytesReceived[n]) / dataMax) * height;
                singleHeight = (static_cast<float>(history.deltaBytesReceived[n]) / totalSum) * totalHeight;
            }
            else
            {
                totalHeight = (static_cast<float>(history.deltaBytesSent[n]) / dataMax) * height;
                singleHeight = (static_cast<float>(history.deltaBytesSent[n]) / totalSum) * totalHeight;
            }

            uint32_t lineHeight = std::ceil(singleHeight);
            lineHeight = std::min<uint32_t>(lineHeight, height);

            if (lineHeight > 0)
            {
                graph_draw_bar(dpi, curX, yOffset - lineHeight, lineHeight, barWidth, NetworkTrafficGroupColors[n]);
            }

            yOffset -= lineHeight;
        }
    }
}

static void window_network_information_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    char textBuffer[200] = {};

    window_draw_widgets(w, dpi);
    window_network_draw_tab_images(w, dpi);

    constexpr int32_t padding = 5;
    constexpr int32_t heightTab = 43;
    constexpr int32_t textHeight = 12;
    const int32_t graphBarWidth = std::min(1, w->width / WH);
    const int32_t totalHeight = w->height;
    const int32_t totalHeightText = (textHeight + (padding * 2)) * 3;
    const int32_t graphHeight = (totalHeight - totalHeightText - heightTab) / 2;

    rct_drawpixelinfo clippedDPI;
    if (clip_drawpixelinfo(&clippedDPI, dpi, w->windowPos, w->width, w->height))
    {
        dpi = &clippedDPI;

        auto screenCoords = ScreenCoordsXY{ padding, heightTab + padding };

        // Received stats.
        {
            gfx_draw_string_left(dpi, STR_NETWORK_RECEIVE, nullptr, PALETTE_INDEX_10, screenCoords);

            format_readable_speed(textBuffer, sizeof(textBuffer), _bytesInSec);
            gfx_draw_string(dpi, textBuffer, PALETTE_INDEX_10, screenCoords + ScreenCoordsXY(70, 0));

            gfx_draw_string_left(
                dpi, STR_NETWORK_TOTAL_RECEIVED, nullptr, PALETTE_INDEX_10, screenCoords + ScreenCoordsXY{ 200, 0 });

            format_readable_size(textBuffer, sizeof(textBuffer), _networkStats.bytesReceived[NETWORK_STATISTICS_GROUP_TOTAL]);
            gfx_draw_string(dpi, textBuffer, PALETTE_INDEX_10, screenCoords + ScreenCoordsXY(300, 0));
            screenCoords.y += textHeight + padding;

            window_network_draw_graph(
                w, dpi, screenCoords.x, screenCoords.y, graphHeight, w->width - (padding * 2), graphBarWidth, true);
            screenCoords.y += graphHeight + padding;
        }

        // Sent stats.
        {
            gfx_draw_string_left(dpi, STR_NETWORK_SEND, nullptr, PALETTE_INDEX_10, screenCoords);

            format_readable_speed(textBuffer, sizeof(textBuffer), _bytesOutSec);
            gfx_draw_string(dpi, textBuffer, PALETTE_INDEX_10, screenCoords + ScreenCoordsXY(70, 0));

            gfx_draw_string_left(
                dpi, STR_NETWORK_TOTAL_SENT, nullptr, PALETTE_INDEX_10, screenCoords + ScreenCoordsXY{ 200, 0 });

            format_readable_size(textBuffer, sizeof(textBuffer), _networkStats.bytesSent[NETWORK_STATISTICS_GROUP_TOTAL]);
            gfx_draw_string(dpi, textBuffer, PALETTE_INDEX_10, screenCoords + ScreenCoordsXY(300, 0));
            screenCoords.y += textHeight + padding;

            window_network_draw_graph(
                w, dpi, screenCoords.x, screenCoords.y, graphHeight, w->width - (padding * 2), graphBarWidth, false);
            screenCoords.y += graphHeight + padding;
        }

        // Draw legend
        {
            for (int i = 1; i < NETWORK_STATISTICS_GROUP_MAX; i++)
            {
                format_string(textBuffer, sizeof(textBuffer), NetworkTrafficGroupNames[i], nullptr);

                // Draw color stripe.
                gfx_fill_rect(
                    dpi, { screenCoords + ScreenCoordsXY{ 0, 4 }, screenCoords + ScreenCoordsXY{ 4, 6 } },
                    NetworkTrafficGroupColors[i]);

                // Draw text.
                gfx_draw_string(dpi, textBuffer, PALETTE_INDEX_10, screenCoords + ScreenCoordsXY(10, 0));

                gfx_get_string_width(textBuffer);

                screenCoords.x += gfx_get_string_width(textBuffer) + 20;
            }
        }
    }
}

#pragma endregion

static void window_network_draw_tab_image(rct_window* w, rct_drawpixelinfo* dpi, int32_t page, int32_t spriteIndex)
{
    rct_widgetindex widgetIndex = WIDX_TAB1 + page;

    if (!widget_is_disabled(w, widgetIndex))
    {
        if (w->page == page)
        {
            int32_t numFrames = window_network_animation_frames[w->page];
            if (numFrames > 1)
            {
                int32_t frame = w->frame_no / window_network_animation_divisor[w->page];
                spriteIndex += (frame % numFrames);
            }
        }

        gfx_draw_sprite(
            dpi, spriteIndex, w->windowPos + ScreenCoordsXY{ w->widgets[widgetIndex].left, w->widgets[widgetIndex].top }, 0);
    }
}

static void window_network_draw_tab_images(rct_window* w, rct_drawpixelinfo* dpi)
{
    window_network_draw_tab_image(w, dpi, WINDOW_NETWORK_PAGE_INFORMATION, SPR_TAB_KIOSKS_AND_FACILITIES_0);
}
