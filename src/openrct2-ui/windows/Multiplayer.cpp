/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Game.h>
#include <openrct2/actions/NetworkModifyGroupAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/network/network.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>

// clang-format off
enum {
    WINDOW_MULTIPLAYER_PAGE_INFORMATION,
    WINDOW_MULTIPLAYER_PAGE_PLAYERS,
    WINDOW_MULTIPLAYER_PAGE_GROUPS,
    WINDOW_MULTIPLAYER_PAGE_OPTIONS
};

enum WindowMultiplayerWidgetIdx {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_CONTENT_PANEL,
    WIDX_TAB1,
    WIDX_TAB2,
    WIDX_TAB3,
    WIDX_TAB4,

    WIDX_HEADER_PLAYER = 8,
    WIDX_HEADER_GROUP,
    WIDX_HEADER_LAST_ACTION,
    WIDX_HEADER_PING,
    WIDX_LIST,

    WIDX_DEFAULT_GROUP = 8,
    WIDX_DEFAULT_GROUP_DROPDOWN,
    WIDX_ADD_GROUP,
    WIDX_REMOVE_GROUP,
    WIDX_RENAME_GROUP,
    WIDX_SELECTED_GROUP,
    WIDX_SELECTED_GROUP_DROPDOWN,
    WIDX_PERMISSIONS_LIST,

    WIDX_LOG_CHAT_CHECKBOX = 8,
    WIDX_LOG_SERVER_ACTIONS_CHECKBOX,
    WIDX_KNOWN_KEYS_ONLY_CHECKBOX,
};

#define MAIN_MULTIPLAYER_WIDGETS \
    MakeWidget({  0,  0}, {340, 240}, WindowWidgetType::Frame,    WindowColour::Primary                                        ), /* panel / background */ \
    MakeWidget({  1,  1}, {338,  14}, WindowWidgetType::Caption,  WindowColour::Primary,  STR_NONE,    STR_WINDOW_TITLE_TIP    ), /* title bar */ \
    MakeWidget({327,  2}, { 11,  12}, WindowWidgetType::CloseBox, WindowColour::Primary,  STR_CLOSE_X, STR_CLOSE_WINDOW_TIP    ), /* close x button */ \
    MakeWidget({  0, 43}, {340, 197}, WindowWidgetType::Resize,   WindowColour::Secondary                                      ), /* content panel */ \
    MakeTab   ({  3, 17},                                                                STR_SHOW_SERVER_INFO_TIP), /* tab */ \
    MakeTab   ({ 34, 17},                                                                STR_PLAYERS_TIP         ), /* tab */ \
    MakeTab   ({ 65, 17},                                                                STR_GROUPS_TIP          ), /* tab */ \
    MakeTab   ({ 96, 17},                                                                STR_OPTIONS_TIP         )  /* tab */

static Widget window_multiplayer_information_widgets[] = {
    MAIN_MULTIPLAYER_WIDGETS,
    WIDGETS_END,
};

static Widget window_multiplayer_players_widgets[] = {
    MAIN_MULTIPLAYER_WIDGETS,
    MakeWidget({  3, 46}, {173,  15}, WindowWidgetType::TableHeader, WindowColour::Primary  , STR_PLAYER     ), // Player name
    MakeWidget({176, 46}, { 83,  15}, WindowWidgetType::TableHeader, WindowColour::Primary  , STR_GROUP      ), // Player name
    MakeWidget({259, 46}, {100,  15}, WindowWidgetType::TableHeader, WindowColour::Primary  , STR_LAST_ACTION), // Player name
    MakeWidget({359, 46}, { 42,  15}, WindowWidgetType::TableHeader, WindowColour::Primary  , STR_PING       ), // Player name
    MakeWidget({  3, 60}, {334, 177}, WindowWidgetType::Scroll,       WindowColour::Secondary, SCROLL_VERTICAL), // list
    WIDGETS_END,
};

static Widget window_multiplayer_groups_widgets[] = {
    MAIN_MULTIPLAYER_WIDGETS,
    MakeWidget({141, 46}, {175,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                    ), // default group
    MakeWidget({305, 47}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH),
    MakeWidget({ 11, 65}, { 92,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_ADD_GROUP     ), // add group button
    MakeWidget({113, 65}, { 92,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_REMOVE_GROUP  ), // remove group button
    MakeWidget({215, 65}, { 92,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_RENAME_GROUP  ), // rename group button
    MakeWidget({ 72, 80}, {175,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                    ), // selected group
    MakeWidget({236, 81}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH),
    MakeWidget({  3, 94}, {314, 207}, WindowWidgetType::Scroll,   WindowColour::Secondary, SCROLL_VERTICAL   ), // permissions list
    WIDGETS_END,
};

static Widget window_multiplayer_options_widgets[] = {
    MAIN_MULTIPLAYER_WIDGETS,
    MakeWidget({3, 50}, {295, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_LOG_CHAT,              STR_LOG_CHAT_TIP             ),
    MakeWidget({3, 64}, {295, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_LOG_SERVER_ACTIONS,    STR_LOG_SERVER_ACTIONS_TIP   ),
    MakeWidget({3, 78}, {295, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_ALLOW_KNOWN_KEYS_ONLY, STR_ALLOW_KNOWN_KEYS_ONLY_TIP),
    WIDGETS_END,
};

static Widget *window_multiplayer_page_widgets[] = {
    window_multiplayer_information_widgets,
    window_multiplayer_players_widgets,
    window_multiplayer_groups_widgets,
    window_multiplayer_options_widgets,
};

static constexpr StringId WindowMultiplayerPageTitles[] = {
    STR_MULTIPLAYER_INFORMATION_TITLE,
    STR_MULTIPLAYER_PLAYERS_TITLE,
    STR_MULTIPLAYER_GROUPS_TITLE,
    STR_MULTIPLAYER_OPTIONS_TITLE,
};

static uint8_t _selectedGroup = 0;

static void WindowMultiplayerInformationMouseup(WindowBase *w, WidgetIndex widgetIndex);
static void WindowMultiplayerInformationResize(WindowBase *w);
static void WindowMultiplayerInformationUpdate(WindowBase *w);
static void WindowMultiplayerInformationInvalidate(WindowBase *w);
static void WindowMultiplayerInformationPaint(WindowBase *w, DrawPixelInfo *dpi);

static void WindowMultiplayerPlayersMouseup(WindowBase *w, WidgetIndex widgetIndex);
static void WindowMultiplayerPlayersResize(WindowBase *w);
static void WindowMultiplayerPlayersUpdate(WindowBase *w);
static void WindowMultiplayerPlayersScrollgetsize(WindowBase *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void WindowMultiplayerPlayersScrollmousedown(WindowBase *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void WindowMultiplayerPlayersScrollmouseover(WindowBase *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void WindowMultiplayerPlayersInvalidate(WindowBase *w);
static void WindowMultiplayerPlayersPaint(WindowBase *w, DrawPixelInfo *dpi);
static void WindowMultiplayerPlayersScrollpaint(WindowBase *w, DrawPixelInfo *dpi, int32_t scrollIndex);

static void WindowMultiplayerGroupsMouseup(WindowBase *w, WidgetIndex widgetIndex);
static void WindowMultiplayerGroupsResize(WindowBase *w);
static void WindowMultiplayerGroupsMousedown(WindowBase *w, WidgetIndex widgetIndex, Widget* widget);
static void WindowMultiplayerGroupsDropdown(WindowBase *w, WidgetIndex widgetIndex, int32_t dropdownIndex);
static void WindowMultiplayerGroupsUpdate(WindowBase *w);
static void WindowMultiplayerGroupsScrollgetsize(WindowBase *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void WindowMultiplayerGroupsScrollmousedown(WindowBase *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void WindowMultiplayerGroupsScrollmouseover(WindowBase *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void WindowMultiplayerGroupsTextInput(WindowBase *w, WidgetIndex widgetIndex, const char *text);
static void WindowMultiplayerGroupsInvalidate(WindowBase *w);
static void WindowMultiplayerGroupsPaint(WindowBase *w, DrawPixelInfo *dpi);
static void WindowMultiplayerGroupsScrollpaint(WindowBase *w, DrawPixelInfo *dpi, int32_t scrollIndex);

static void WindowMultiplayerOptionsMouseup(WindowBase *w, WidgetIndex widgetIndex);
static void WindowMultiplayerOptionsResize(WindowBase *w);
static void WindowMultiplayerOptionsUpdate(WindowBase *w);
static void WindowMultiplayerOptionsInvalidate(WindowBase *w);
static void WindowMultiplayerOptionsPaint(WindowBase *w, DrawPixelInfo *dpi);

static WindowEventList window_multiplayer_information_events([](auto& events)
{
    events.mouse_up = &WindowMultiplayerInformationMouseup;
    events.resize = &WindowMultiplayerInformationResize;
    events.update = &WindowMultiplayerInformationUpdate;
    events.invalidate = &WindowMultiplayerInformationInvalidate;
    events.paint = &WindowMultiplayerInformationPaint;
});

static WindowEventList window_multiplayer_players_events([](auto& events)
{
    events.mouse_up = &WindowMultiplayerPlayersMouseup;
    events.resize = &WindowMultiplayerPlayersResize;
    events.update = &WindowMultiplayerPlayersUpdate;
    events.get_scroll_size = &WindowMultiplayerPlayersScrollgetsize;
    events.scroll_mousedown = &WindowMultiplayerPlayersScrollmousedown;
    events.scroll_mouseover = &WindowMultiplayerPlayersScrollmouseover;
    events.invalidate = &WindowMultiplayerPlayersInvalidate;
    events.paint = &WindowMultiplayerPlayersPaint;
    events.scroll_paint = &WindowMultiplayerPlayersScrollpaint;
});

static WindowEventList window_multiplayer_groups_events([](auto& events)
{
    events.mouse_up = &WindowMultiplayerGroupsMouseup;
    events.resize = &WindowMultiplayerGroupsResize;
    events.mouse_down = &WindowMultiplayerGroupsMousedown;
    events.dropdown = &WindowMultiplayerGroupsDropdown;
    events.update = &WindowMultiplayerGroupsUpdate;
    events.get_scroll_size = &WindowMultiplayerGroupsScrollgetsize;
    events.scroll_mousedown = &WindowMultiplayerGroupsScrollmousedown;
    events.scroll_mouseover = &WindowMultiplayerGroupsScrollmouseover;
    events.text_input = &WindowMultiplayerGroupsTextInput;
    events.invalidate = &WindowMultiplayerGroupsInvalidate;
    events.paint = &WindowMultiplayerGroupsPaint;
    events.scroll_paint = &WindowMultiplayerGroupsScrollpaint;
});

static WindowEventList window_multiplayer_options_events([](auto& events)
{
    events.mouse_up = &WindowMultiplayerOptionsMouseup;
    events.resize = &WindowMultiplayerOptionsResize;
    events.update = &WindowMultiplayerOptionsUpdate;
    events.invalidate = &WindowMultiplayerOptionsInvalidate;
    events.paint = &WindowMultiplayerOptionsPaint;
});

static WindowEventList *window_multiplayer_page_events[] = {
    &window_multiplayer_information_events,
    &window_multiplayer_players_events,
    &window_multiplayer_groups_events,
    &window_multiplayer_options_events,
};
// clang-format on

static constexpr const int32_t window_multiplayer_animation_divisor[] = {
    4,
    4,
    2,
    2,
};
static constexpr const int32_t window_multiplayer_animation_frames[] = {
    8,
    8,
    7,
    4,
};

static void WindowMultiplayerDrawTabImages(WindowBase* w, DrawPixelInfo* dpi);
static void WindowMultiplayerSetPage(WindowBase* w, int32_t page);

static bool _windowInformationSizeDirty;
static ScreenCoordsXY _windowInformationSize;

WindowBase* WindowMultiplayerOpen()
{
    // Check if window is already open
    WindowBase* window = WindowBringToFrontByClass(WindowClass::Multiplayer);
    if (window == nullptr)
    {
        window = WindowCreateAutoPos(
            320, 144, &window_multiplayer_players_events, WindowClass::Multiplayer, WF_10 | WF_RESIZABLE);
        WindowMultiplayerSetPage(window, WINDOW_MULTIPLAYER_PAGE_INFORMATION);
    }

    return window;
}

static void WindowMultiplayerSetPage(WindowBase* w, int32_t page)
{
    _windowInformationSizeDirty = true;

    w->page = page;
    w->frame_no = 0;
    w->no_list_items = 0;
    w->selected_list_item = -1;

    w->hold_down_widgets = 0;
    w->event_handlers = window_multiplayer_page_events[page];
    w->pressed_widgets = 0;
    w->widgets = window_multiplayer_page_widgets[page];
    w->widgets[WIDX_TITLE].text = WindowMultiplayerPageTitles[page];

    WindowEventResizeCall(w);
    WindowEventInvalidateCall(w);
    WindowInitScrollWidgets(*w);
    w->Invalidate();
}

static void WindowMultiplayerAnchorBorderWidgets(WindowBase* w)
{
    w->ResizeFrameWithPage();
}

static void WindowMultiplayerSetPressedTab(WindowBase* w)
{
    for (int32_t i = WIDX_TAB1; i <= WIDX_TAB4; i++)
    {
        w->pressed_widgets &= ~(1 << i);
    }
    w->pressed_widgets |= 1LL << (WIDX_TAB1 + w->page);
}

static void WindowMultiplayerGroupsShowGroupDropdown(WindowBase* w, Widget* widget)
{
    Widget* dropdownWidget;
    int32_t numItems, i;

    dropdownWidget = widget - 1;

    numItems = NetworkGetNumGroups();

    WindowDropdownShowTextCustomWidth(
        { w->windowPos.x + dropdownWidget->left, w->windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
        w->colours[1], 0, 0, numItems, widget->right - dropdownWidget->left);

    for (i = 0; i < NetworkGetNumGroups(); i++)
    {
        gDropdownItems[i].Format = STR_OPTIONS_DROPDOWN_ITEM;
        gDropdownItems[i].Args = reinterpret_cast<uintptr_t>(NetworkGetGroupName(i));
    }
    if (widget == &window_multiplayer_groups_widgets[WIDX_DEFAULT_GROUP_DROPDOWN])
    {
        Dropdown::SetChecked(NetworkGetGroupIndex(NetworkGetDefaultGroup()), true);
    }
    else if (widget == &window_multiplayer_groups_widgets[WIDX_SELECTED_GROUP_DROPDOWN])
    {
        Dropdown::SetChecked(NetworkGetGroupIndex(_selectedGroup), true);
    }
}

#pragma region Information page

static void WindowMultiplayerInformationMouseup(WindowBase* w, WidgetIndex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            WindowClose(*w);
            break;
        case WIDX_TAB1:
        case WIDX_TAB2:
        case WIDX_TAB3:
        case WIDX_TAB4:
            if (w->page != widgetIndex - WIDX_TAB1)
            {
                WindowMultiplayerSetPage(w, widgetIndex - WIDX_TAB1);
            }
            break;
    }
}

static ScreenCoordsXY WindowMultiplayerInformationGetSize()
{
    if (!_windowInformationSizeDirty)
    {
        return _windowInformationSize;
    }

    int32_t lineHeight = FontGetLineHeight(FontStyle::Medium);

    // Base dimensions.
    const int32_t width = 450;
    int32_t height = 55;

    // Server name is displayed word-wrapped, so figure out how high it will be.
    {
        int32_t numLines;
        GfxWrapString(NetworkGetServerName(), width, FontStyle::Medium, nullptr, &numLines);
        height += (numLines + 1) * lineHeight + (LIST_ROW_HEIGHT / 2);
    }

    // Likewise, for the optional server description -- which can be a little longer.
    const auto& descString = NetworkGetServerDescription();
    if (!descString.empty())
    {
        int32_t numLines;
        GfxWrapString(descString, width, FontStyle::Medium, nullptr, &numLines);
        height += (numLines + 1) * lineHeight + (LIST_ROW_HEIGHT / 2);
    }

    // Finally, account for provider info, if present.
    {
        const auto& providerName = NetworkGetServerProviderName();
        if (!providerName.empty())
            height += LIST_ROW_HEIGHT;

        const auto& providerEmail = NetworkGetServerProviderEmail();
        if (!providerEmail.empty())
            height += LIST_ROW_HEIGHT;

        const auto& providerWebsite = NetworkGetServerProviderWebsite();
        if (!providerWebsite.empty())
            height += LIST_ROW_HEIGHT;
    }

    _windowInformationSizeDirty = false;
    _windowInformationSize = { static_cast<int16_t>(width), static_cast<int16_t>(height) };
    return _windowInformationSize;
}

static void WindowMultiplayerInformationResize(WindowBase* w)
{
    auto size = WindowMultiplayerInformationGetSize();
    WindowSetResize(*w, size.x, size.y, size.x, size.y);
}

static void WindowMultiplayerInformationUpdate(WindowBase* w)
{
    w->frame_no++;
    WidgetInvalidate(*w, WIDX_TAB1 + w->page);
}

static void WindowMultiplayerInformationInvalidate(WindowBase* w)
{
    WindowMultiplayerSetPressedTab(w);
    WindowMultiplayerAnchorBorderWidgets(w);
    WindowAlignTabs(w, WIDX_TAB1, WIDX_TAB4);
}

static void WindowMultiplayerInformationPaint(WindowBase* w, DrawPixelInfo* dpi)
{
    WindowDrawWidgets(*w, dpi);
    WindowMultiplayerDrawTabImages(w, dpi);

    DrawPixelInfo clippedDPI;
    if (ClipDrawPixelInfo(&clippedDPI, dpi, w->windowPos, w->width, w->height))
    {
        dpi = &clippedDPI;

        auto screenCoords = ScreenCoordsXY{ 3, 50 };
        int32_t width = w->width - 6;

        const auto& name = NetworkGetServerName();
        {
            auto ft = Formatter();
            ft.Add<const char*>(name.c_str());
            screenCoords.y += DrawTextWrapped(*dpi, screenCoords, width, STR_STRING, ft, { w->colours[1] });
            screenCoords.y += LIST_ROW_HEIGHT / 2;
        }

        const auto& description = NetworkGetServerDescription();
        if (!description.empty())
        {
            auto ft = Formatter();
            ft.Add<const char*>(description.c_str());
            screenCoords.y += DrawTextWrapped(*dpi, screenCoords, width, STR_STRING, ft, { w->colours[1] });
            screenCoords.y += LIST_ROW_HEIGHT / 2;
        }

        const auto& providerName = NetworkGetServerProviderName();
        if (!providerName.empty())
        {
            auto ft = Formatter();
            ft.Add<const char*>(providerName.c_str());
            DrawTextBasic(*dpi, screenCoords, STR_PROVIDER_NAME, ft);
            screenCoords.y += LIST_ROW_HEIGHT;
        }

        const auto& providerEmail = NetworkGetServerProviderEmail();
        if (!providerEmail.empty())
        {
            auto ft = Formatter();
            ft.Add<const char*>(providerEmail.c_str());
            DrawTextBasic(*dpi, screenCoords, STR_PROVIDER_EMAIL, ft);
            screenCoords.y += LIST_ROW_HEIGHT;
        }

        const auto& providerWebsite = NetworkGetServerProviderWebsite();
        if (!providerWebsite.empty())
        {
            auto ft = Formatter();
            ft.Add<const char*>(providerWebsite.c_str());
            DrawTextBasic(*dpi, screenCoords, STR_PROVIDER_WEBSITE, ft);
        }
    }
}

#pragma endregion

#pragma region Players page

static bool IsServerPlayerInvisible()
{
    return NetworkIsServerPlayerInvisible() && !gConfigGeneral.DebuggingTools;
}

static void WindowMultiplayerPlayersMouseup(WindowBase* w, WidgetIndex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            WindowClose(*w);
            break;
        case WIDX_TAB1:
        case WIDX_TAB2:
        case WIDX_TAB3:
        case WIDX_TAB4:
            if (w->page != widgetIndex - WIDX_TAB1)
            {
                WindowMultiplayerSetPage(w, widgetIndex - WIDX_TAB1);
            }
            break;
    }
}

static void WindowMultiplayerPlayersResize(WindowBase* w)
{
    WindowSetResize(*w, 420, 124, 500, 450);

    w->no_list_items = (IsServerPlayerInvisible() ? NetworkGetNumVisiblePlayers() : NetworkGetNumPlayers());
    w->list_item_positions[0] = 0;

    w->widgets[WIDX_HEADER_PING].right = w->width - 5;

    w->selected_list_item = -1;
    w->Invalidate();
}

static void WindowMultiplayerPlayersUpdate(WindowBase* w)
{
    w->frame_no++;
    WidgetInvalidate(*w, WIDX_TAB1 + w->page);
}

static void WindowMultiplayerPlayersScrollgetsize(WindowBase* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    int32_t i;

    if (w->selected_list_item != -1)
    {
        w->selected_list_item = -1;
        w->Invalidate();
    }

    *height = NetworkGetNumPlayers() * SCROLLABLE_ROW_HEIGHT;
    i = *height - window_multiplayer_players_widgets[WIDX_LIST].bottom + window_multiplayer_players_widgets[WIDX_LIST].top + 21;
    if (i < 0)
        i = 0;
    if (i < w->scrolls[0].v_top)
    {
        w->scrolls[0].v_top = i;
        w->Invalidate();
    }
}

static void WindowMultiplayerPlayersScrollmousedown(WindowBase* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    int32_t index;

    index = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
    if (index >= w->no_list_items)
        return;

    w->selected_list_item = index;
    w->Invalidate();

    int32_t player = (IsServerPlayerInvisible() ? index + 1 : index);
    WindowPlayerOpen(NetworkGetPlayerID(player));
}

static void WindowMultiplayerPlayersScrollmouseover(WindowBase* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    int32_t index;

    index = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
    if (index >= w->no_list_items)
        return;

    w->selected_list_item = index;
    w->Invalidate();
}

static void WindowMultiplayerPlayersInvalidate(WindowBase* w)
{
    WindowMultiplayerSetPressedTab(w);
    WindowMultiplayerAnchorBorderWidgets(w);
    window_multiplayer_players_widgets[WIDX_LIST].right = w->width - 4;
    window_multiplayer_players_widgets[WIDX_LIST].bottom = w->height - 0x0F;
    WindowAlignTabs(w, WIDX_TAB1, WIDX_TAB4);
}

static void WindowMultiplayerPlayersPaint(WindowBase* w, DrawPixelInfo* dpi)
{
    StringId stringId;

    WindowDrawWidgets(*w, dpi);
    WindowMultiplayerDrawTabImages(w, dpi);

    // Number of players
    stringId = w->no_list_items == 1 ? STR_MULTIPLAYER_PLAYER_COUNT : STR_MULTIPLAYER_PLAYER_COUNT_PLURAL;
    auto screenCoords = w->windowPos + ScreenCoordsXY{ 4, w->widgets[WIDX_LIST].bottom + 2 };
    auto ft = Formatter();
    ft.Add<uint16_t>(w->no_list_items);
    DrawTextBasic(*dpi, screenCoords, stringId, ft, { w->colours[2] });
}

static void WindowMultiplayerPlayersScrollpaint(WindowBase* w, DrawPixelInfo* dpi, int32_t scrollIndex)
{
    ScreenCoordsXY screenCoords;
    screenCoords.y = 0;

    const int32_t firstPlayerInList = (IsServerPlayerInvisible() ? 1 : 0);
    int32_t listPosition = 0;

    for (int32_t player = firstPlayerInList; player < NetworkGetNumPlayers(); player++)
    {
        if (screenCoords.y > dpi->y + dpi->height)
        {
            break;
        }

        if (screenCoords.y + SCROLLABLE_ROW_HEIGHT + 1 >= dpi->y)
        {
            thread_local std::string _buffer;
            _buffer.reserve(512);
            _buffer.clear();

            // Draw player name
            colour_t colour = COLOUR_BLACK;
            if (listPosition == w->selected_list_item)
            {
                GfxFilterRect(
                    dpi, { 0, screenCoords.y, 800, screenCoords.y + SCROLLABLE_ROW_HEIGHT - 1 },
                    FilterPaletteID::PaletteDarken1);
                _buffer += NetworkGetPlayerName(player);
                colour = w->colours[2];
            }
            else
            {
                if (NetworkGetPlayerFlags(player) & NETWORK_PLAYER_FLAG_ISSERVER)
                {
                    _buffer += "{BABYBLUE}";
                }
                else
                {
                    _buffer += "{BLACK}";
                }
                _buffer += NetworkGetPlayerName(player);
            }
            screenCoords.x = 0;
            GfxClipString(_buffer.data(), 230, FontStyle::Medium);
            GfxDrawString(*dpi, screenCoords, _buffer.c_str(), { colour });

            // Draw group name
            _buffer.resize(0);
            int32_t group = NetworkGetGroupIndex(NetworkGetPlayerGroup(player));
            if (group != -1)
            {
                _buffer += "{BLACK}";
                screenCoords.x = 173;
                _buffer += NetworkGetGroupName(group);
                GfxClipString(_buffer.data(), 80, FontStyle::Medium);
                GfxDrawString(*dpi, screenCoords, _buffer.c_str(), { colour });
            }

            // Draw last action
            int32_t action = NetworkGetPlayerLastAction(player, 2000);
            auto ft = Formatter();
            if (action != -999)
            {
                ft.Add<StringId>(NetworkGetActionNameStringID(action));
            }
            else
            {
                ft.Add<StringId>(STR_ACTION_NA);
            }
            DrawTextEllipsised(*dpi, { 256, screenCoords.y }, 100, STR_BLACK_STRING, ft);

            // Draw ping
            _buffer.resize(0);
            int32_t ping = NetworkGetPlayerPing(player);
            if (ping <= 100)
            {
                _buffer += "{GREEN}";
            }
            else if (ping <= 250)
            {
                _buffer += "{YELLOW}";
            }
            else
            {
                _buffer += "{RED}";
            }

            char pingBuffer[64]{};
            snprintf(pingBuffer, sizeof(pingBuffer), "%d ms", ping);
            _buffer += pingBuffer;

            screenCoords.x = 356;
            GfxDrawString(*dpi, screenCoords, _buffer.c_str(), { colour });
        }
        screenCoords.y += SCROLLABLE_ROW_HEIGHT;
        listPosition++;
    }
}

#pragma endregion

#pragma region Groups page

static void WindowMultiplayerGroupsMouseup(WindowBase* w, WidgetIndex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            WindowClose(*w);
            break;
        case WIDX_TAB1:
        case WIDX_TAB2:
        case WIDX_TAB3:
        case WIDX_TAB4:
            if (w->page != widgetIndex - WIDX_TAB1)
            {
                WindowMultiplayerSetPage(w, widgetIndex - WIDX_TAB1);
            }
            break;
        case WIDX_ADD_GROUP:
        {
            auto networkModifyGroup = NetworkModifyGroupAction(ModifyGroupType::AddGroup);
            GameActions::Execute(&networkModifyGroup);
        }
        break;
        case WIDX_REMOVE_GROUP:
        {
            auto networkModifyGroup = NetworkModifyGroupAction(ModifyGroupType::RemoveGroup, _selectedGroup);
            GameActions::Execute(&networkModifyGroup);
        }
        break;
        case WIDX_RENAME_GROUP:;
            int32_t groupIndex = NetworkGetGroupIndex(_selectedGroup);
            const utf8* groupName = NetworkGetGroupName(groupIndex);
            WindowTextInputRawOpen(w, widgetIndex, STR_GROUP_NAME, STR_ENTER_NEW_NAME_FOR_THIS_GROUP, {}, groupName, 32);
            break;
    }
}

static void WindowMultiplayerGroupsResize(WindowBase* w)
{
    WindowSetResize(*w, 320, 200, 320, 500);

    w->no_list_items = NetworkGetNumActions();
    w->list_item_positions[0] = 0;

    w->selected_list_item = -1;
    w->Invalidate();
}

static void WindowMultiplayerGroupsMousedown(WindowBase* w, WidgetIndex widgetIndex, Widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_DEFAULT_GROUP_DROPDOWN:
            WindowMultiplayerGroupsShowGroupDropdown(w, widget);
            break;
        case WIDX_SELECTED_GROUP_DROPDOWN:
            WindowMultiplayerGroupsShowGroupDropdown(w, widget);
            break;
    }
}

static void WindowMultiplayerGroupsDropdown(WindowBase* w, WidgetIndex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
    {
        return;
    }

    switch (widgetIndex)
    {
        case WIDX_DEFAULT_GROUP_DROPDOWN:
        {
            auto networkModifyGroup = NetworkModifyGroupAction(ModifyGroupType::SetDefault, NetworkGetGroupID(dropdownIndex));
            GameActions::Execute(&networkModifyGroup);
        }
        break;
        case WIDX_SELECTED_GROUP_DROPDOWN:
            _selectedGroup = NetworkGetGroupID(dropdownIndex);
            break;
    }

    w->Invalidate();
}

static void WindowMultiplayerGroupsUpdate(WindowBase* w)
{
    w->frame_no++;
    WidgetInvalidate(*w, WIDX_TAB1 + w->page);
}

static void WindowMultiplayerGroupsScrollgetsize(WindowBase* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    int32_t i;

    if (w->selected_list_item != -1)
    {
        w->selected_list_item = -1;
        w->Invalidate();
    }

    *height = NetworkGetNumActions() * SCROLLABLE_ROW_HEIGHT;
    i = *height - window_multiplayer_groups_widgets[WIDX_LIST].bottom + window_multiplayer_groups_widgets[WIDX_LIST].top + 21;
    if (i < 0)
        i = 0;
    if (i < w->scrolls[0].v_top)
    {
        w->scrolls[0].v_top = i;
        w->Invalidate();
    }
}

static void WindowMultiplayerGroupsScrollmousedown(WindowBase* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    int32_t index;

    index = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
    if (index >= w->no_list_items)
        return;

    w->selected_list_item = index;
    w->Invalidate();

    auto networkModifyGroup = NetworkModifyGroupAction(
        ModifyGroupType::SetPermissions, _selectedGroup, "", index, PermissionState::Toggle);
    GameActions::Execute(&networkModifyGroup);
}

static void WindowMultiplayerGroupsScrollmouseover(WindowBase* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    int32_t index;

    index = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
    if (index >= w->no_list_items)
        return;

    w->selected_list_item = index;
    w->Invalidate();
}

static void WindowMultiplayerGroupsTextInput(WindowBase* w, WidgetIndex widgetIndex, const char* text)
{
    if (widgetIndex != WIDX_RENAME_GROUP)
        return;

    if (text == nullptr)
        return;

    auto networkModifyGroup = NetworkModifyGroupAction(ModifyGroupType::SetName, _selectedGroup, text);
    GameActions::Execute(&networkModifyGroup);
}

static void WindowMultiplayerGroupsInvalidate(WindowBase* w)
{
    WindowMultiplayerSetPressedTab(w);
    WindowMultiplayerAnchorBorderWidgets(w);
    window_multiplayer_groups_widgets[WIDX_PERMISSIONS_LIST].right = w->width - 4;
    window_multiplayer_groups_widgets[WIDX_PERMISSIONS_LIST].bottom = w->height - 0x0F;
    WindowAlignTabs(w, WIDX_TAB1, WIDX_TAB4);

    // select other group if one is removed
    while (NetworkGetGroupIndex(_selectedGroup) == -1 && _selectedGroup > 0)
    {
        _selectedGroup--;
    }
}

static void WindowMultiplayerGroupsPaint(WindowBase* w, DrawPixelInfo* dpi)
{
    thread_local std::string _buffer;

    WindowDrawWidgets(*w, dpi);
    WindowMultiplayerDrawTabImages(w, dpi);

    Widget* widget = &window_multiplayer_groups_widgets[WIDX_DEFAULT_GROUP];
    int32_t group = NetworkGetGroupIndex(NetworkGetDefaultGroup());
    if (group != -1)
    {
        _buffer.assign("{WINDOW_COLOUR_2}");
        _buffer += NetworkGetGroupName(group);

        auto ft = Formatter();
        ft.Add<const char*>(_buffer.c_str());
        DrawTextEllipsised(
            *dpi, w->windowPos + ScreenCoordsXY{ widget->midX() - 5, widget->top }, widget->width() - 8, STR_STRING, ft,
            { TextAlignment::CENTRE });
    }

    auto screenPos = w->windowPos
        + ScreenCoordsXY{ window_multiplayer_groups_widgets[WIDX_CONTENT_PANEL].left + 4,
                          window_multiplayer_groups_widgets[WIDX_CONTENT_PANEL].top + 4 };

    DrawTextBasic(*dpi, screenPos, STR_DEFAULT_GROUP, {}, { w->colours[2] });

    screenPos.y += 20;

    GfxFillRectInset(
        dpi, { screenPos - ScreenCoordsXY{ 0, 6 }, screenPos + ScreenCoordsXY{ 310, -5 } }, w->colours[1],
        INSET_RECT_FLAG_BORDER_INSET);

    widget = &window_multiplayer_groups_widgets[WIDX_SELECTED_GROUP];
    group = NetworkGetGroupIndex(_selectedGroup);
    if (group != -1)
    {
        _buffer.assign("{WINDOW_COLOUR_2}");
        _buffer += NetworkGetGroupName(group);
        auto ft = Formatter();
        ft.Add<const char*>(_buffer.c_str());
        DrawTextEllipsised(
            *dpi, w->windowPos + ScreenCoordsXY{ widget->midX() - 5, widget->top }, widget->width() - 8, STR_STRING, ft,
            { TextAlignment::CENTRE });
    }
}

static void WindowMultiplayerGroupsScrollpaint(WindowBase* w, DrawPixelInfo* dpi, int32_t scrollIndex)
{
    auto screenCoords = ScreenCoordsXY{ 0, 0 };

    auto dpiCoords = ScreenCoordsXY{ dpi->x, dpi->y };
    GfxFillRect(
        dpi, { dpiCoords, dpiCoords + ScreenCoordsXY{ dpi->width - 1, dpi->height - 1 } }, ColourMapA[w->colours[1]].mid_light);

    for (int32_t i = 0; i < NetworkGetNumActions(); i++)
    {
        if (i == w->selected_list_item)
        {
            GfxFilterRect(
                dpi, { 0, screenCoords.y, 800, screenCoords.y + SCROLLABLE_ROW_HEIGHT - 1 }, FilterPaletteID::PaletteDarken1);
        }
        if (screenCoords.y > dpi->y + dpi->height)
        {
            break;
        }

        if (screenCoords.y + SCROLLABLE_ROW_HEIGHT + 1 >= dpi->y)
        {
            int32_t groupindex = NetworkGetGroupIndex(_selectedGroup);
            if (groupindex != -1)
            {
                if (NetworkCanPerformAction(groupindex, static_cast<NetworkPermission>(i)))
                {
                    screenCoords.x = 0;
                    GfxDrawString(*dpi, screenCoords, u8"{WINDOW_COLOUR_2}✓", {});
                }
            }

            // Draw action name
            auto ft = Formatter();
            ft.Add<uint16_t>(NetworkGetActionNameStringID(i));
            DrawTextBasic(*dpi, { 10, screenCoords.y }, STR_WINDOW_COLOUR_2_STRINGID, ft);
        }
        screenCoords.y += SCROLLABLE_ROW_HEIGHT;
    }
}

#pragma endregion

#pragma region Options page

static void WindowMultiplayerOptionsMouseup(WindowBase* w, WidgetIndex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            WindowClose(*w);
            break;
        case WIDX_TAB1:
        case WIDX_TAB2:
        case WIDX_TAB3:
        case WIDX_TAB4:
            if (w->page != widgetIndex - WIDX_TAB1)
            {
                WindowMultiplayerSetPage(w, widgetIndex - WIDX_TAB1);
            }
            break;
        case WIDX_LOG_CHAT_CHECKBOX:
            gConfigNetwork.LogChat = !gConfigNetwork.LogChat;
            ConfigSaveDefault();
            break;
        case WIDX_LOG_SERVER_ACTIONS_CHECKBOX:
            gConfigNetwork.LogServerActions = !gConfigNetwork.LogServerActions;
            ConfigSaveDefault();
            break;
        case WIDX_KNOWN_KEYS_ONLY_CHECKBOX:
            gConfigNetwork.KnownKeysOnly = !gConfigNetwork.KnownKeysOnly;
            ConfigSaveDefault();
            break;
    }
}

static void WindowMultiplayerOptionsResize(WindowBase* w)
{
    WindowSetResize(*w, 300, 100, 300, 100);
}

static void WindowMultiplayerOptionsUpdate(WindowBase* w)
{
    w->frame_no++;
    WidgetInvalidate(*w, WIDX_TAB1 + w->page);
}

static void WindowMultiplayerOptionsInvalidate(WindowBase* w)
{
    WindowMultiplayerSetPressedTab(w);
    WindowMultiplayerAnchorBorderWidgets(w);
    WindowAlignTabs(w, WIDX_TAB1, WIDX_TAB4);

    if (NetworkGetMode() == NETWORK_MODE_CLIENT)
    {
        w->widgets[WIDX_KNOWN_KEYS_ONLY_CHECKBOX].type = WindowWidgetType::Empty;
    }

    WidgetSetCheckboxValue(*w, WIDX_LOG_CHAT_CHECKBOX, gConfigNetwork.LogChat);
    WidgetSetCheckboxValue(*w, WIDX_LOG_SERVER_ACTIONS_CHECKBOX, gConfigNetwork.LogServerActions);
    WidgetSetCheckboxValue(*w, WIDX_KNOWN_KEYS_ONLY_CHECKBOX, gConfigNetwork.KnownKeysOnly);
}

static void WindowMultiplayerOptionsPaint(WindowBase* w, DrawPixelInfo* dpi)
{
    WindowDrawWidgets(*w, dpi);
    WindowMultiplayerDrawTabImages(w, dpi);
}

#pragma endregion

static void WindowMultiplayerDrawTabImage(WindowBase* w, DrawPixelInfo* dpi, int32_t page, int32_t spriteIndex)
{
    WidgetIndex widgetIndex = WIDX_TAB1 + page;

    if (!WidgetIsDisabled(*w, widgetIndex))
    {
        if (w->page == page)
        {
            int32_t numFrames = window_multiplayer_animation_frames[w->page];
            if (numFrames > 1)
            {
                int32_t frame = w->frame_no / window_multiplayer_animation_divisor[w->page];
                spriteIndex += (frame % numFrames);
            }
        }

        GfxDrawSprite(
            dpi, ImageId(spriteIndex),
            w->windowPos + ScreenCoordsXY{ w->widgets[widgetIndex].left, w->widgets[widgetIndex].top });
    }
}

static void WindowMultiplayerDrawTabImages(WindowBase* w, DrawPixelInfo* dpi)
{
    WindowMultiplayerDrawTabImage(w, dpi, WINDOW_MULTIPLAYER_PAGE_INFORMATION, SPR_TAB_KIOSKS_AND_FACILITIES_0);
    WindowMultiplayerDrawTabImage(w, dpi, WINDOW_MULTIPLAYER_PAGE_PLAYERS, SPR_TAB_GUESTS_0);
    WindowMultiplayerDrawTabImage(w, dpi, WINDOW_MULTIPLAYER_PAGE_GROUPS, SPR_TAB_STAFF_OPTIONS_0);
    WindowMultiplayerDrawTabImage(w, dpi, WINDOW_MULTIPLAYER_PAGE_OPTIONS, SPR_TAB_GEARS_0);
}
