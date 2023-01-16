/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/actions/PlayerKickAction.h>
#include <openrct2/actions/PlayerSetGroupAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/network/NetworkAction.h>
#include <openrct2/network/network.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>
#include <utility>

// clang-format off
enum WindowPlayerPage {
    WINDOW_PLAYER_PAGE_OVERVIEW,
    WINDOW_PLAYER_PAGE_STATISTICS,
};

#pragma region Widgets

enum WindowPlayerWidgetIdx {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PAGE_BACKGROUND,
    WIDX_TAB_1,
    WIDX_TAB_2,

    WIDX_GROUP = 6,
    WIDX_GROUP_DROPDOWN,
    WIDX_LOCATE,
    WIDX_KICK,
    WIDX_VIEWPORT,
};

#define WINDOW_PLAYER_COMMON_WIDGETS                                                                                                    \
    MakeWidget({  0,  0}, {192, 157}, WindowWidgetType::Frame,    WindowColour::Primary                                     ), /* Panel / Background */ \
    MakeWidget({  1,  1}, {190,  14}, WindowWidgetType::Caption,  WindowColour::Primary  , STR_STRING,  STR_WINDOW_TITLE_TIP), /* Title              */ \
    MakeWidget({179,  2}, { 11,  12}, WindowWidgetType::CloseBox, WindowColour::Primary  , STR_CLOSE_X, STR_CLOSE_WINDOW_TIP), /* Close x button     */ \
    MakeWidget({  0, 43}, {192, 114}, WindowWidgetType::Resize,   WindowColour::Secondary                                   ), /* Resize             */ \
    MakeTab   ({  3, 17}                                                                                      ), /* Tab 1              */ \
    MakeTab   ({ 34, 17}                                                                                      )  /* Tab 2              */

static Widget window_player_overview_widgets[] = {
    WINDOW_PLAYER_COMMON_WIDGETS,
    MakeWidget({  3, 46}, {175, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                           ), // Permission group
    MakeWidget({167, 47}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH                       ),
    MakeWidget({179, 45}, { 12, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_LOCATE),         STR_LOCATE_PLAYER_TIP), // Locate button
    MakeWidget({179, 69}, { 12, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_DEMOLISH),       STR_KICK_PLAYER_TIP  ), // Kick button
    MakeWidget({  3, 60}, {175, 61}, WindowWidgetType::Viewport, WindowColour::Secondary                                           ), // Viewport
    WIDGETS_END,
};

static Widget window_player_statistics_widgets[] = {
    WINDOW_PLAYER_COMMON_WIDGETS,
    WIDGETS_END,
};

static Widget *window_player_page_widgets[] = {
    window_player_overview_widgets,
    window_player_statistics_widgets,
};

#pragma endregion

#pragma region Events

static void WindowPlayerOverviewClose(rct_window *w);
static void WindowPlayerOverviewMouseUp(rct_window *w, WidgetIndex widgetIndex);
static void WindowPlayerOverviewResize(rct_window *w);
static void WindowPlayerOverviewMouseDown(rct_window *w, WidgetIndex widgetIndex, Widget *widget);
static void WindowPlayerOverviewDropdown(rct_window *w, WidgetIndex widgetIndex, int32_t dropdownIndex);
static void WindowPlayerOverviewUpdate(rct_window* w);
static void WindowPlayerOverviewInvalidate(rct_window *w);
static void WindowPlayerOverviewPaint(rct_window *w, rct_drawpixelinfo *dpi);

static WindowEventList window_player_overview_events([](auto& events)
{
    events.close = &WindowPlayerOverviewClose;
    events.mouse_up = &WindowPlayerOverviewMouseUp;
    events.resize = &WindowPlayerOverviewResize;
    events.mouse_down = &WindowPlayerOverviewMouseDown;
    events.dropdown = &WindowPlayerOverviewDropdown;
    events.update = &WindowPlayerOverviewUpdate;
    events.invalidate = &WindowPlayerOverviewInvalidate;
    events.paint = &WindowPlayerOverviewPaint;
});

static void WindowPlayerStatisticsClose(rct_window *w);
static void WindowPlayerStatisticsMouseUp(rct_window *w, WidgetIndex widgetIndex);
static void WindowPlayerStatisticsResize(rct_window *w);
static void WindowPlayerStatisticsUpdate(rct_window* w);
static void WindowPlayerStatisticsInvalidate(rct_window *w);
static void WindowPlayerStatisticsPaint(rct_window *w, rct_drawpixelinfo *dpi);

static WindowEventList window_player_statistics_events([](auto& events)
{
    events.close = &WindowPlayerStatisticsClose;
    events.mouse_up = &WindowPlayerStatisticsMouseUp;
    events.resize = &WindowPlayerStatisticsResize;
    events.update = &WindowPlayerStatisticsUpdate;
    events.invalidate = &WindowPlayerStatisticsInvalidate;
    events.paint = &WindowPlayerStatisticsPaint;
});

static WindowEventList *window_player_page_events[] = {
    &window_player_overview_events,
    &window_player_statistics_events,
};

#pragma endregion

// clang-format on

static void WindowPlayerSetPage(rct_window* w, int32_t page);
static void WindowPlayerDrawTabImages(rct_drawpixelinfo* dpi, rct_window* w);
static void WindowPlayerUpdateViewport(rct_window* w, bool scroll);
static void WindowPlayerUpdateTitle(rct_window* w);

rct_window* WindowPlayerOpen(uint8_t id)
{
    rct_window* window;

    window = WindowBringToFrontByNumber(WindowClass::Player, id);
    if (window == nullptr)
    {
        window = WindowCreateAutoPos(240, 170, &window_player_overview_events, WindowClass::Player, WF_RESIZABLE);
        window->number = id;
        window->page = 0;
        window->frame_no = 0;
        window->list_information_type = 0;
        window->picked_peep_frame = 0;
        window->min_width = 210;
        window->min_height = 134;
        window->max_width = 500;
        window->max_height = 450;

        window->no_list_items = 0;
        window->selected_list_item = -1;
    }

    window->page = 0;
    window->Invalidate();

    window->widgets = window_player_page_widgets[WINDOW_PLAYER_PAGE_OVERVIEW];
    window->hold_down_widgets = 0;
    window->event_handlers = window_player_page_events[WINDOW_PLAYER_PAGE_OVERVIEW];
    window->pressed_widgets = 0;

    WindowInitScrollWidgets(*window);
    WindowPlayerSetPage(window, WINDOW_PLAYER_PAGE_OVERVIEW);

    return window;
}

static void WindowPlayerOverviewShowGroupDropdown(rct_window* w, Widget* widget)
{
    Widget* dropdownWidget;
    int32_t numItems, i;
    int32_t player = network_get_player_index(static_cast<uint8_t>(w->number));
    if (player == -1)
    {
        return;
    }

    dropdownWidget = widget - 1;

    numItems = network_get_num_groups();

    WindowDropdownShowTextCustomWidth(
        { w->windowPos.x + dropdownWidget->left, w->windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
        w->colours[1], 0, 0, numItems, widget->right - dropdownWidget->left);

    for (i = 0; i < network_get_num_groups(); i++)
    {
        gDropdownItems[i].Format = STR_OPTIONS_DROPDOWN_ITEM;
        gDropdownItems[i].Args = reinterpret_cast<uintptr_t>(network_get_group_name(i));
    }

    Dropdown::SetChecked(network_get_group_index(network_get_player_group(player)), true);
}

void WindowPlayerOverviewClose(rct_window* w)
{
}

void WindowPlayerOverviewMouseUp(rct_window* w, WidgetIndex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            WindowClose(*w);
            break;
        case WIDX_TAB_1:
        case WIDX_TAB_2:
            WindowPlayerSetPage(w, widgetIndex - WIDX_TAB_1);
            break;
        case WIDX_LOCATE:
        {
            rct_window* mainWindow = WindowGetMain();
            if (mainWindow != nullptr)
            {
                int32_t player = network_get_player_index(static_cast<uint8_t>(w->number));
                if (player == -1)
                {
                    return;
                }
                auto coord = network_get_player_last_action_coord(player);
                if (coord.x || coord.y || coord.z)
                {
                    WindowScrollToLocation(*mainWindow, coord);
                }
            }
        }
        break;
        case WIDX_KICK:
        {
            auto kickPlayerAction = PlayerKickAction(w->number);
            GameActions::Execute(&kickPlayerAction);
        }
        break;
    }
}

void WindowPlayerOverviewMouseDown(rct_window* w, WidgetIndex widgetIndex, Widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_GROUP_DROPDOWN:
            WindowPlayerOverviewShowGroupDropdown(w, widget);
            break;
    }
}

void WindowPlayerOverviewDropdown(rct_window* w, WidgetIndex widgetIndex, int32_t dropdownIndex)
{
    const auto playerId = static_cast<uint8_t>(w->number);
    const auto playerIdx = network_get_player_index(playerId);
    if (playerIdx == -1)
    {
        return;
    }
    if (dropdownIndex == -1)
    {
        return;
    }
    const auto groupId = network_get_group_id(dropdownIndex);
    const auto windowHandle = std::make_pair(w->classification, w->number);
    auto playerSetGroupAction = PlayerSetGroupAction(playerId, groupId);
    playerSetGroupAction.SetCallback([windowHandle](const GameAction* ga, const GameActions::Result* result) {
        if (result->Error == GameActions::Status::Ok)
        {
            WindowInvalidateByNumber(windowHandle.first, windowHandle.second);
        }
    });
    GameActions::Execute(&playerSetGroupAction);
}

void WindowPlayerOverviewResize(rct_window* w)
{
    WindowSetResize(*w, 240, 170, 500, 300);
}

void WindowPlayerOverviewUpdate(rct_window* w)
{
    w->frame_no++;
    WidgetInvalidate(*w, WIDX_TAB_1 + w->page);

    if (network_get_player_index(static_cast<uint8_t>(w->number)) == -1)
    {
        WindowClose(*w);
        return;
    }

    // Update viewport
    bool scroll = true;

    // Use this spare window field for rotation check
    if (w->var_4AE != GetCurrentRotation())
    {
        w->var_4AE = GetCurrentRotation();
        scroll = false;
    }
    WindowPlayerUpdateViewport(w, scroll);
}

void WindowPlayerOverviewPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(*w, dpi);
    WindowPlayerDrawTabImages(dpi, w);

    int32_t player = network_get_player_index(static_cast<uint8_t>(w->number));
    if (player == -1)
    {
        return;
    }

    // Draw current group
    int32_t groupindex = network_get_group_index(network_get_player_group(player));
    if (groupindex != -1)
    {
        Widget* widget = &window_player_overview_widgets[WIDX_GROUP];

        thread_local std::string _buffer;
        _buffer.assign("{WINDOW_COLOUR_2}");
        _buffer += network_get_group_name(groupindex);
        auto ft = Formatter();
        ft.Add<const char*>(_buffer.c_str());

        DrawTextEllipsised(
            dpi, w->windowPos + ScreenCoordsXY{ widget->midX() - 5, widget->top }, widget->width() - 8, STR_STRING, ft,
            { TextAlignment::CENTRE });
    }

    // Draw ping
    auto screenCoords = w->windowPos + ScreenCoordsXY{ 90, 24 };

    auto ft = Formatter();
    ft.Add<StringId>(STR_PING);
    DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
    char ping[64];
    snprintf(ping, 64, "%d ms", network_get_player_ping(player));
    GfxDrawString(dpi, screenCoords + ScreenCoordsXY(30, 0), ping, { w->colours[2] });

    // Draw last action
    screenCoords = w->windowPos + ScreenCoordsXY{ w->width / 2, w->height - 13 };
    int32_t width = w->width - 8;
    int32_t lastaction = network_get_player_last_action(player, 0);
    ft = Formatter();
    if (lastaction != -999)
    {
        ft.Add<StringId>(network_get_action_name_string_id(lastaction));
    }
    else
    {
        ft.Add<StringId>(STR_ACTION_NA);
    }
    DrawTextEllipsised(dpi, screenCoords, width, STR_LAST_ACTION_RAN, ft, { TextAlignment::CENTRE });

    if (w->viewport != nullptr && w->var_492 != -1)
    {
        WindowDrawViewport(dpi, *w);
    }
}

void WindowPlayerOverviewInvalidate(rct_window* w)
{
    int32_t playerIndex = network_get_player_index(static_cast<uint8_t>(w->number));
    if (playerIndex == -1)
    {
        return;
    }

    if (window_player_page_widgets[w->page] != w->widgets)
    {
        w->widgets = window_player_page_widgets[w->page];
        WindowInitScrollWidgets(*w);
    }

    w->pressed_widgets &= ~(WIDX_TAB_1);
    w->pressed_widgets &= ~(WIDX_TAB_2);
    w->pressed_widgets |= 1uLL << (w->page + WIDX_TAB_1);

    WindowPlayerUpdateTitle(w);

    w->ResizeFrameWithPage();
    w->widgets[WIDX_LOCATE].right = w->width - 2;
    w->widgets[WIDX_LOCATE].left = w->width - 25;
    w->widgets[WIDX_KICK].right = w->width - 2;
    w->widgets[WIDX_KICK].left = w->width - 25;
    w->widgets[WIDX_VIEWPORT].right = w->width - 26;
    w->widgets[WIDX_VIEWPORT].bottom = w->height - 14;

    int32_t groupDropdownWidth = w->widgets[WIDX_GROUP].width();
    w->widgets[WIDX_GROUP].left = (w->width - groupDropdownWidth) / 2;
    w->widgets[WIDX_GROUP].right = w->widgets[WIDX_GROUP].left + groupDropdownWidth;
    w->widgets[WIDX_GROUP_DROPDOWN].left = w->widgets[WIDX_GROUP].right - 10;
    w->widgets[WIDX_GROUP_DROPDOWN].right = w->widgets[WIDX_GROUP].right;

    WindowAlignTabs(w, WIDX_TAB_1, WIDX_TAB_2);

    rct_viewport* viewport = w->viewport;
    if (viewport != nullptr)
    {
        Widget* viewportWidget = &window_player_overview_widgets[WIDX_VIEWPORT];

        viewport->pos = w->windowPos + ScreenCoordsXY{ viewportWidget->left, viewportWidget->top };
        viewport->width = viewportWidget->width();
        viewport->height = viewportWidget->height();
        viewport->view_width = viewport->zoom.ApplyTo(viewport->width);
        viewport->view_height = viewport->zoom.ApplyTo(viewport->height);
    }

    // Only enable kick button for other players
    const bool canKick = network_can_perform_action(network_get_current_player_group_index(), NetworkPermission::KickPlayer);
    const bool isServer = network_get_player_flags(playerIndex) & NETWORK_PLAYER_FLAG_ISSERVER;
    const bool isOwnWindow = (network_get_current_player_id() == w->number);
    WidgetSetEnabled(*w, WIDX_KICK, canKick && !isOwnWindow && !isServer);
}

void WindowPlayerStatisticsClose(rct_window* w)
{
    if (w->error.var_480)
    {
        w->error.var_480 = 0;
    }
}

void WindowPlayerStatisticsMouseUp(rct_window* w, WidgetIndex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            WindowClose(*w);
            break;
        case WIDX_TAB_1:
        case WIDX_TAB_2:
            WindowPlayerSetPage(w, widgetIndex - WIDX_TAB_1);
            break;
    }
}

void WindowPlayerStatisticsResize(rct_window* w)
{
    WindowSetResize(*w, 210, 80, 210, 80);
}

void WindowPlayerStatisticsUpdate(rct_window* w)
{
    w->frame_no++;
    WidgetInvalidate(*w, WIDX_TAB_1 + w->page);

    if (network_get_player_index(static_cast<uint8_t>(w->number)) == -1)
    {
        WindowClose(*w);
    }
}

void WindowPlayerStatisticsInvalidate(rct_window* w)
{
    if (window_player_page_widgets[w->page] != w->widgets)
    {
        w->widgets = window_player_page_widgets[w->page];
        WindowInitScrollWidgets(*w);
    }

    w->pressed_widgets &= ~(WIDX_TAB_1);
    w->pressed_widgets &= ~(WIDX_TAB_2);
    w->pressed_widgets |= 1uLL << (w->page + WIDX_TAB_1);

    WindowPlayerUpdateTitle(w);

    w->ResizeFrameWithPage();

    WindowAlignTabs(w, WIDX_TAB_1, WIDX_TAB_2);
}

void WindowPlayerStatisticsPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(*w, dpi);
    WindowPlayerDrawTabImages(dpi, w);

    int32_t player = network_get_player_index(static_cast<uint8_t>(w->number));
    if (player == -1)
    {
        return;
    }

    auto screenCoords = w->windowPos
        + ScreenCoordsXY{ window_player_overview_widgets[WIDX_PAGE_BACKGROUND].left + 4,
                          window_player_overview_widgets[WIDX_PAGE_BACKGROUND].top + 4 };

    auto ft = Formatter();
    ft.Add<uint32_t>(network_get_player_commands_ran(player));
    DrawTextBasic(dpi, screenCoords, STR_COMMANDS_RAN, ft);

    screenCoords.y += LIST_ROW_HEIGHT;

    ft = Formatter();
    ft.Add<uint32_t>(network_get_player_money_spent(player));
    DrawTextBasic(dpi, screenCoords, STR_MONEY_SPENT, ft);
}

static void WindowPlayerSetPage(rct_window* w, int32_t page)
{
    int32_t originalPage = w->page;

    w->page = page;
    w->frame_no = 0;
    w->no_list_items = 0;
    w->selected_list_item = -1;

    w->hold_down_widgets = 0;
    w->event_handlers = window_player_page_events[page];
    w->pressed_widgets = 0;
    w->widgets = window_player_page_widgets[page];
    w->Invalidate();
    WindowEventResizeCall(w);
    WindowEventInvalidateCall(w);
    WindowInitScrollWidgets(*w);
    w->Invalidate();

    if (page == WINDOW_PLAYER_PAGE_OVERVIEW)
    {
        if (w->viewport == nullptr)
        {
            const auto focus = Focus(TileCoordsXYZ(128, 128, 0).ToCoordsXYZ());
            ViewportCreate(w, w->windowPos, w->width, w->height, focus);
            w->flags |= WF_NO_SCROLLING;
            WindowEventInvalidateCall(w);
            WindowPlayerUpdateViewport(w, false);
        }
        else if (originalPage != page)
        {
            WindowEventInvalidateCall(w);
            WindowPlayerUpdateViewport(w, false);
        }
    }
    else
    {
        w->RemoveViewport();
    }
}

static void WindowPlayerDrawTabImages(rct_drawpixelinfo* dpi, rct_window* w)
{
    Widget* widget;

    // Tab 1
    if (!WidgetIsDisabled(*w, WIDX_TAB_1))
    {
        widget = &w->widgets[WIDX_TAB_1];
        auto screenCoords = w->windowPos + ScreenCoordsXY{ widget->left, widget->top };
        GfxDrawSprite(dpi, ImageId(SPR_PEEP_LARGE_FACE_NORMAL), screenCoords);
    }

    // Tab 2
    if (!WidgetIsDisabled(*w, WIDX_TAB_2))
    {
        widget = &w->widgets[WIDX_TAB_2];
        auto screenCoords = w->windowPos + ScreenCoordsXY{ widget->left, widget->top };
        int32_t imageId = SPR_TAB_FINANCES_SUMMARY_0;

        if (w->page == WINDOW_PLAYER_PAGE_STATISTICS)
        {
            imageId += (w->frame_no / 2) & 7;
        }

        GfxDrawSprite(dpi, ImageId(imageId), screenCoords);
    }
}

static void WindowPlayerUpdateViewport(rct_window* w, bool scroll)
{
    int32_t playerIndex = network_get_player_index(static_cast<uint8_t>(w->number));
    if (playerIndex == -1)
    {
        return;
    }

    rct_viewport* viewport = w->viewport;
    if (viewport != nullptr)
    {
        auto coord = network_get_player_last_action_coord(playerIndex);
        if (coord.x != 0 || coord.y != 0 || coord.z != 0)
        {
            auto centreLoc = centre_2d_coordinates(coord, viewport);
            if (!centreLoc.has_value())
            {
                return;
            }
            // Don't scroll if the view was originally undefined
            if (w->var_492 == -1)
            {
                scroll = false;
            }

            if (!scroll || w->savedViewPos != centreLoc.value())
            {
                w->flags |= WF_SCROLLING_TO_LOCATION;
                w->savedViewPos = centreLoc.value();
                if (!scroll)
                {
                    w->viewport->viewPos = centreLoc.value();
                }
                WidgetInvalidate(*w, WIDX_VIEWPORT);
            }

            // Draw the viewport
            w->var_492 = 0;
        }
        else
        {
            // Don't draw the viewport
            w->var_492 = -1;
        }
    }
}

static void WindowPlayerUpdateTitle(rct_window* w)
{
    auto ft = Formatter::Common();
    int32_t player = network_get_player_index(static_cast<uint8_t>(w->number));
    if (player != -1)
    {
        ft.Add<const char*>(network_get_player_name(player)); // set title caption to player name
    }
    else
    {
        ft.Add<const char*>("");
    }
}
