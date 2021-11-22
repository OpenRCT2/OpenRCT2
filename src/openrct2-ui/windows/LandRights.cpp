/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/actions/LandBuyRightsAction.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/world/Park.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_LAND_RIGHTS;
static constexpr const int32_t WH = 94;
static constexpr const int32_t WW = 98;

// clang-format off
enum WINDOW_LAND_RIGHTS_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PREVIEW,
    WIDX_DECREMENT,
    WIDX_INCREMENT,
    WIDX_BUY_LAND_RIGHTS,
    WIDX_BUY_CONSTRUCTION_RIGHTS
};

static rct_widget window_land_rights_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget     ({27, 17}, {44, 32}, WindowWidgetType::ImgBtn,  WindowColour::Primary , SPR_LAND_TOOL_SIZE_0                                           ), // preview box
    MakeRemapWidget({28, 18}, {16, 16}, WindowWidgetType::TrnBtn,  WindowColour::Tertiary, SPR_LAND_TOOL_DECREASE,      STR_ADJUST_SMALLER_LAND_RIGHTS_TIP), // decrement size
    MakeRemapWidget({54, 32}, {16, 16}, WindowWidgetType::TrnBtn,  WindowColour::Tertiary, SPR_LAND_TOOL_INCREASE,      STR_ADJUST_LARGER_LAND_RIGHTS_TIP ), // increment size
    MakeRemapWidget({22, 53}, {24, 24}, WindowWidgetType::FlatBtn, WindowColour::Tertiary, SPR_BUY_LAND_RIGHTS,         STR_BUY_LAND_RIGHTS_TIP           ), // land rights
    MakeRemapWidget({52, 53}, {24, 24}, WindowWidgetType::FlatBtn, WindowColour::Tertiary, SPR_BUY_CONSTRUCTION_RIGHTS, STR_BUY_CONSTRUCTION_RIGHTS_TIP   ), // construction rights
    WIDGETS_END,
};

static void WindowLandRightsClose(rct_window *w);
static void WindowLandRightsMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowLandRightsMousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget *widget);
static void WindowLandRightsUpdate(rct_window *w);
static void WindowLandRightsInvalidate(rct_window *w);
static void WindowLandRightsPaint(rct_window *w, rct_drawpixelinfo *dpi);
static void WindowLandRightsTextinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void WindowLandRightsInputsize(rct_window *w);
static void WindowLandRightsToolupdate(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void WindowLandRightsTooldown(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void WindowLandRightsTooldrag(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void WindowLandRightsToolabort(rct_window *w, rct_widgetindex widgetIndex);
static bool LandRightsToolIsActive();


static rct_window_event_list window_land_rights_events([](auto& events)
{
    events.close = &WindowLandRightsClose;
    events.mouse_up = &WindowLandRightsMouseup;
    events.mouse_down = &WindowLandRightsMousedown;
    events.update = &WindowLandRightsUpdate;
    events.tool_update = &WindowLandRightsToolupdate;
    events.tool_down = &WindowLandRightsTooldown;
    events.tool_drag = &WindowLandRightsTooldrag;
    events.tool_abort = &WindowLandRightsToolabort;
    events.text_input = &WindowLandRightsTextinput;
    events.invalidate = &WindowLandRightsInvalidate;
    events.paint = &WindowLandRightsPaint;
});
// clang-format on

constexpr uint8_t LAND_RIGHTS_MODE_BUY_CONSTRUCTION_RIGHTS = 0;
constexpr uint8_t LAND_RIGHTS_MODE_BUY_LAND = 1;

static uint8_t _landRightsMode;
static money32 _landRightsCost;

rct_window* WindowLandRightsOpen()
{
    rct_window* window;

    // Check if window is already open
    window = window_find_by_class(WC_LAND_RIGHTS);
    if (window != nullptr)
        return window;

    window = WindowCreate(ScreenCoordsXY(context_get_width() - 98, 29), 98, 94, &window_land_rights_events, WC_LAND_RIGHTS, 0);
    window->widgets = window_land_rights_widgets;
    window->enabled_widgets = (1ULL << WIDX_CLOSE) | (1ULL << WIDX_DECREMENT) | (1ULL << WIDX_INCREMENT)
        | (1ULL << WIDX_PREVIEW) | (1ULL << WIDX_BUY_LAND_RIGHTS) | (1ULL << WIDX_BUY_CONSTRUCTION_RIGHTS);
    window->hold_down_widgets = (1ULL << WIDX_INCREMENT) | (1ULL << WIDX_DECREMENT);
    WindowInitScrollWidgets(window);
    window_push_others_below(window);

    _landRightsMode = LAND_RIGHTS_MODE_BUY_LAND;
    window->pressed_widgets = (1ULL << WIDX_BUY_LAND_RIGHTS);

    gLandToolSize = 1;

    show_gridlines();
    tool_set(window, WIDX_BUY_LAND_RIGHTS, Tool::UpArrow);
    input_set_flag(INPUT_FLAG_6, true);

    show_land_rights();

    if (gLandRemainingConstructionSales == 0)
    {
        show_construction_rights();
    }

    return window;
}

static void WindowLandRightsClose(rct_window* w)
{
    if (gLandRemainingConstructionSales == 0)
    {
        hide_construction_rights();
    }

    // If the tool wasn't changed, turn tool off
    if (LandRightsToolIsActive())
        tool_cancel();
}

static void WindowLandRightsMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_PREVIEW:
            WindowLandRightsInputsize(w);
            break;
        case WIDX_BUY_LAND_RIGHTS:
            if (_landRightsMode != LAND_RIGHTS_MODE_BUY_LAND)
            {
                tool_set(w, WIDX_BUY_LAND_RIGHTS, Tool::UpArrow);
                _landRightsMode = LAND_RIGHTS_MODE_BUY_LAND;
                show_land_rights();
                w->Invalidate();
            }
            break;
        case WIDX_BUY_CONSTRUCTION_RIGHTS:
            if (_landRightsMode != LAND_RIGHTS_MODE_BUY_CONSTRUCTION_RIGHTS)
            {
                tool_set(w, WIDX_BUY_CONSTRUCTION_RIGHTS, Tool::UpArrow);
                _landRightsMode = LAND_RIGHTS_MODE_BUY_CONSTRUCTION_RIGHTS;
                show_construction_rights();
                w->Invalidate();
            }
            break;
    }
}

static void WindowLandRightsMousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_DECREMENT:
            // Decrement land rights tool size
            gLandToolSize = std::max(MINIMUM_TOOL_SIZE, gLandToolSize - 1);

            // Invalidate the window
            w->Invalidate();
            break;
        case WIDX_INCREMENT:
            // Decrement land rights tool size
            gLandToolSize = std::min(MAXIMUM_TOOL_SIZE, gLandToolSize + 1);

            // Invalidate the window
            w->Invalidate();
            break;
    }
}

static void WindowLandRightsTextinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    int32_t size;
    char* end;

    if (widgetIndex != WIDX_PREVIEW || text == nullptr)
        return;

    size = strtol(text, &end, 10);
    if (*end == '\0')
    {
        size = std::max(MINIMUM_TOOL_SIZE, size);
        size = std::min(MAXIMUM_TOOL_SIZE, size);
        gLandToolSize = size;
        w->Invalidate();
    }
}

static void WindowLandRightsInputsize(rct_window* w)
{
    Formatter ft;
    ft.Add<int16_t>(MINIMUM_TOOL_SIZE);
    ft.Add<int16_t>(MAXIMUM_TOOL_SIZE);
    WindowTextInputOpen(w, WIDX_PREVIEW, STR_SELECTION_SIZE, STR_ENTER_SELECTION_SIZE, ft, STR_NONE, STR_NONE, 3);
}

static void WindowLandRightsUpdate(rct_window* w)
{
    w->frame_no++;
    // Close window if another tool is open
    if (!LandRightsToolIsActive())
        window_close(w);
}

static void WindowLandRightsInvalidate(rct_window* w)
{
    // Set the preview image button to be pressed down
    w->pressed_widgets |= (1ULL << WIDX_PREVIEW)
        | (1ULL << ((_landRightsMode == LAND_RIGHTS_MODE_BUY_LAND) ? WIDX_BUY_LAND_RIGHTS : WIDX_BUY_CONSTRUCTION_RIGHTS));
    w->pressed_widgets &= ~(
        1ULL
        << ((_landRightsMode == LAND_RIGHTS_MODE_BUY_CONSTRUCTION_RIGHTS) ? WIDX_BUY_LAND_RIGHTS
                                                                          : WIDX_BUY_CONSTRUCTION_RIGHTS));

    // Update the preview image
    window_land_rights_widgets[WIDX_PREVIEW].image = LandTool::SizeToSpriteIndex(gLandToolSize);

    // Disable ownership and/or construction buying functions if there are no tiles left for sale
    if (gLandRemainingOwnershipSales == 0)
    {
        w->disabled_widgets |= (1ULL << WIDX_BUY_LAND_RIGHTS);
        window_land_rights_widgets[WIDX_BUY_LAND_RIGHTS].tooltip = STR_NO_LAND_RIGHTS_FOR_SALE_TIP;
    }
    else
    {
        w->disabled_widgets &= ~(1ULL << WIDX_BUY_LAND_RIGHTS);
        window_land_rights_widgets[WIDX_BUY_LAND_RIGHTS].tooltip = STR_BUY_LAND_RIGHTS_TIP;
    }

    if (gLandRemainingConstructionSales == 0)
    {
        w->disabled_widgets |= (1ULL << WIDX_BUY_CONSTRUCTION_RIGHTS);
        window_land_rights_widgets[WIDX_BUY_CONSTRUCTION_RIGHTS].tooltip = STR_NO_CONSTRUCTION_RIGHTS_FOR_SALE_TIP;
    }
    else
    {
        w->disabled_widgets &= ~(1ULL << WIDX_BUY_CONSTRUCTION_RIGHTS);
        window_land_rights_widgets[WIDX_BUY_CONSTRUCTION_RIGHTS].tooltip = STR_BUY_CONSTRUCTION_RIGHTS_TIP;
    }
}

static void WindowLandRightsPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    auto screenCoords = ScreenCoordsXY{ w->windowPos.x + window_land_rights_widgets[WIDX_PREVIEW].midX(),
                                        w->windowPos.y + window_land_rights_widgets[WIDX_PREVIEW].midY() };

    WindowDrawWidgets(w, dpi);
    // Draw number for tool sizes bigger than 7
    if (gLandToolSize > MAX_TOOL_SIZE_WITH_SPRITE)
    {
        auto ft = Formatter();
        ft.Add<uint16_t>(gLandToolSize);
        DrawTextBasic(dpi, screenCoords - ScreenCoordsXY{ 0, 2 }, STR_LAND_TOOL_SIZE_VALUE, ft, { TextAlignment::CENTRE });
    }

    // Draw cost amount
    if (_landRightsCost != MONEY32_UNDEFINED && _landRightsCost != 0 && !(gParkFlags & PARK_FLAGS_NO_MONEY))
    {
        auto ft = Formatter();
        ft.Add<money64>(_landRightsCost);
        screenCoords = { window_land_rights_widgets[WIDX_PREVIEW].midX() + w->windowPos.x,
                         window_land_rights_widgets[WIDX_PREVIEW].bottom + w->windowPos.y + 32 };
        DrawTextBasic(dpi, screenCoords, STR_COST_AMOUNT, ft, { TextAlignment::CENTRE });
    }
}

static void WindowLandRightsToolUpdateLandRights(const ScreenCoordsXY& screenCoords)
{
    map_invalidate_selection_rect();
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;

    auto mapTile = screen_get_map_xy(screenCoords, nullptr);

    if (!mapTile.has_value())
    {
        if (_landRightsCost != MONEY32_UNDEFINED)
        {
            _landRightsCost = MONEY32_UNDEFINED;
            window_invalidate_by_class(WC_CLEAR_SCENERY);
        }
        return;
    }

    uint8_t state_changed = 0;

    if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
    {
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
        state_changed++;
    }

    if (gMapSelectType != MAP_SELECT_TYPE_FULL)
    {
        gMapSelectType = MAP_SELECT_TYPE_FULL;
        state_changed++;
    }

    int16_t tool_size = gLandToolSize;
    if (tool_size == 0)
        tool_size = 1;

    int16_t tool_length = (tool_size - 1) * 32;

    // Move to tool bottom left
    mapTile->x -= (tool_size - 1) * 16;
    mapTile->y -= (tool_size - 1) * 16;
    mapTile = mapTile->ToTileStart();

    if (gMapSelectPositionA.x != mapTile->x)
    {
        gMapSelectPositionA.x = mapTile->x;
        state_changed++;
    }

    if (gMapSelectPositionA.y != mapTile->y)
    {
        gMapSelectPositionA.y = mapTile->y;
        state_changed++;
    }

    mapTile->x += tool_length;
    mapTile->y += tool_length;

    if (gMapSelectPositionB.x != mapTile->x)
    {
        gMapSelectPositionB.x = mapTile->x;
        state_changed++;
    }

    if (gMapSelectPositionB.y != mapTile->y)
    {
        gMapSelectPositionB.y = mapTile->y;
        state_changed++;
    }

    map_invalidate_selection_rect();
    if (!state_changed)
        return;

    auto landBuyRightsAction = LandBuyRightsAction(
        { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
        (_landRightsMode == LAND_RIGHTS_MODE_BUY_LAND) ? LandBuyRightSetting::BuyLand
                                                       : LandBuyRightSetting::BuyConstructionRights);
    auto res = GameActions::Query(&landBuyRightsAction);

    _landRightsCost = res->Error == GameActions::Status::Ok ? res->Cost : MONEY32_UNDEFINED;
}

/**
 *
 *  rct2: 0x0066822A
 */
static void WindowLandRightsToolabort(rct_window* w, rct_widgetindex widgetIndex)
{
    hide_gridlines();
    if (_landRightsMode == LAND_RIGHTS_MODE_BUY_LAND)
    {
        hide_land_rights();
    }
    else
    {
        hide_construction_rights();
    }
}

/**
 *
 *  rct2: 0x006681D1
 */
static void WindowLandRightsToolupdate(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    WindowLandRightsToolUpdateLandRights(screenCoords);
}

/**
 *
 *  rct2: 0x006681E6
 */
static void WindowLandRightsTooldown(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (_landRightsMode == LAND_RIGHTS_MODE_BUY_LAND)
    {
        if (screenCoords.x != LOCATION_NULL)
        {
            auto landBuyRightsAction = LandBuyRightsAction(
                { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
                LandBuyRightSetting::BuyLand);
            GameActions::Execute(&landBuyRightsAction);
        }
    }
    else
    {
        if (screenCoords.x != LOCATION_NULL)
        {
            auto landBuyRightsAction = LandBuyRightsAction(
                { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
                LandBuyRightSetting::BuyConstructionRights);
            GameActions::Execute(&landBuyRightsAction);
        }
    }
}

/**
 *
 *  rct2: 0x006681FB
 */
static void WindowLandRightsTooldrag(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (_landRightsMode == LAND_RIGHTS_MODE_BUY_LAND)
    {
        if (screenCoords.x != LOCATION_NULL)
        {
            auto landBuyRightsAction = LandBuyRightsAction(
                { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
                LandBuyRightSetting::BuyLand);
            GameActions::Execute(&landBuyRightsAction);
        }
    }
    else
    {
        if (screenCoords.x != LOCATION_NULL)
        {
            auto landBuyRightsAction = LandBuyRightsAction(
                { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
                LandBuyRightSetting::BuyConstructionRights);
            GameActions::Execute(&landBuyRightsAction);
        }
    }
}

static bool LandRightsToolIsActive()
{
    if (!(input_test_flag(INPUT_FLAG_TOOL_ACTIVE)))
        return false;
    if (gCurrentToolWidget.window_classification != WC_LAND_RIGHTS)
        return false;
    return true;
}
