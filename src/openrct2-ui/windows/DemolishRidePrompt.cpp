/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>

static constexpr const int32_t WW = 200;
static constexpr const int32_t WH = 100;

static money32 _demolishRideCost;

// clang-format off
enum WindowRideDemolishWidgetIdx {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_DEMOLISH = 3,
    WIDX_REFURBISH = 3,
    WIDX_CANCEL
};

// 0x009AEBA0
static rct_widget _windowRideDemolishWidgets[] = {
    WINDOW_SHIM_WHITE(STR_DEMOLISH_RIDE, WW, WH),
    MakeWidget({     10, WH - 22}, {85, 14}, WindowWidgetType::Button, WindowColour::Primary, STR_DEMOLISH          ),
    MakeWidget({WW - 95, WH - 22}, {85, 14}, WindowWidgetType::Button, WindowColour::Primary, STR_SAVE_PROMPT_CANCEL),
    WIDGETS_END,
};

static rct_widget _windowRideRefurbishWidgets[] = {
    WINDOW_SHIM_WHITE(STR_REFURBISH_RIDE, WW, WH),
    MakeWidget({     10, WH - 22}, {85, 14}, WindowWidgetType::Button, WindowColour::Primary, STR_REFURBISH         ),
    MakeWidget({WW - 95, WH - 22}, {85, 14}, WindowWidgetType::Button, WindowColour::Primary, STR_SAVE_PROMPT_CANCEL),
    WIDGETS_END,
};

static void WindowRideDemolishMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowRideDemolishPaint(rct_window *w, rct_drawpixelinfo *dpi);
static void WindowRideRefurbishMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowRideRefurbishPaint(rct_window *w, rct_drawpixelinfo *dpi);

//0x0098E2E4
static rct_window_event_list _windowRideDemolishEvents([](auto& events)
{
    events.mouse_up = &WindowRideDemolishMouseup;
    events.paint = &WindowRideDemolishPaint;
});
// clang-format on

static rct_window_event_list _windowRideRefurbishEvents([](auto& events) {
    events.mouse_up = &WindowRideRefurbishMouseup;
    events.paint = &WindowRideRefurbishPaint;
});

/** Based off of rct2: 0x006B486A */
rct_window* WindowRideDemolishPromptOpen(Ride* ride)
{
    rct_window* w;

    w = window_find_by_class(WC_DEMOLISH_RIDE_PROMPT);
    if (w != nullptr)
    {
        auto windowPos = w->windowPos;
        window_close(w);
        w = WindowCreate(windowPos, WW, WH, &_windowRideDemolishEvents, WC_DEMOLISH_RIDE_PROMPT, WF_TRANSPARENT);
    }
    else
    {
        w = WindowCreateCentred(WW, WH, &_windowRideDemolishEvents, WC_DEMOLISH_RIDE_PROMPT, WF_TRANSPARENT);
    }

    w->widgets = _windowRideDemolishWidgets;
    w->enabled_widgets = (1ULL << WIDX_CLOSE) | (1ULL << WIDX_CANCEL) | (1ULL << WIDX_DEMOLISH);
    WindowInitScrollWidgets(w);
    w->rideId = ride->id;
    _demolishRideCost = -ride_get_refund_price(ride);

    return w;
}

rct_window* WindowRideRefurbishPromptOpen(Ride* ride)
{
    rct_window* w;

    w = window_find_by_class(WC_DEMOLISH_RIDE_PROMPT);
    if (w != nullptr)
    {
        auto windowPos = w->windowPos;
        window_close(w);
        w = WindowCreate(windowPos, WW, WH, &_windowRideRefurbishEvents, WC_DEMOLISH_RIDE_PROMPT, WF_TRANSPARENT);
    }
    else
    {
        w = WindowCreateCentred(WW, WH, &_windowRideRefurbishEvents, WC_DEMOLISH_RIDE_PROMPT, WF_TRANSPARENT);
    }

    w->widgets = _windowRideRefurbishWidgets;
    w->enabled_widgets = (1ULL << WIDX_CLOSE) | (1ULL << WIDX_CANCEL) | (1ULL << WIDX_REFURBISH);
    WindowInitScrollWidgets(w);
    w->rideId = ride->id;
    _demolishRideCost = -ride_get_refund_price(ride);

    return w;
}

/**
 *
 *  rct2: 0x006B4933
 */
static void WindowRideDemolishMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_DEMOLISH:
        {
            auto ride = get_ride(w->rideId);
            ride_action_modify(ride, RIDE_MODIFY_DEMOLISH, GAME_COMMAND_FLAG_APPLY);
            break;
        }
        case WIDX_CANCEL:
        case WIDX_CLOSE:
            window_close(w);
            break;
    }
}

static void WindowRideRefurbishMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_REFURBISH:
        {
            auto ride = get_ride(w->rideId);
            ride_action_modify(ride, RIDE_MODIFY_RENEW, GAME_COMMAND_FLAG_APPLY);
            break;
        }
        case WIDX_CANCEL:
        case WIDX_CLOSE:
            window_close(w);
            break;
    }
}

/**
 *
 *  rct2: 0x006B48E5
 */
static void WindowRideDemolishPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);

    auto ride = get_ride(w->rideId);
    if (ride != nullptr)
    {
        auto stringId = (gParkFlags & PARK_FLAGS_NO_MONEY) ? STR_DEMOLISH_RIDE_ID : STR_DEMOLISH_RIDE_ID_MONEY;
        auto ft = Formatter();
        ride->FormatNameTo(ft);
        ft.Add<money64>(_demolishRideCost);

        ScreenCoordsXY stringCoords(w->windowPos.x + WW / 2, w->windowPos.y + (WH / 2) - 3);
        DrawTextWrapped(dpi, stringCoords, WW - 4, stringId, ft, { TextAlignment::CENTRE });
    }
}

static void WindowRideRefurbishPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);

    auto ride = get_ride(w->rideId);
    if (ride != nullptr)
    {
        auto stringId = (gParkFlags & PARK_FLAGS_NO_MONEY) ? STR_REFURBISH_RIDE_ID_NO_MONEY : STR_REFURBISH_RIDE_ID_MONEY;
        auto ft = Formatter();
        ride->FormatNameTo(ft);
        ft.Add<money64>(_demolishRideCost / 2);

        ScreenCoordsXY stringCoords(w->windowPos.x + WW / 2, w->windowPos.y + (WH / 2) - 3);
        DrawTextWrapped(dpi, stringCoords, WW - 4, stringId, ft, { TextAlignment::CENTRE });
    }
}
