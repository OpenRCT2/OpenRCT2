/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

#    include <algorithm>
#    include <chrono>
#    include <openrct2-ui/interface/Dropdown.h>
#    include <openrct2-ui/interface/Widget.h>
#    include <openrct2-ui/windows/Window.h>
#    include <openrct2/Context.h>
#    include <openrct2/config/Config.h>
#    include <openrct2/core/Json.hpp>
#    include <openrct2/core/String.hpp>
#    include <openrct2/drawing/Drawing.h>
#    include <openrct2/interface/Colour.h>
#    include <openrct2/localisation/Localisation.h>
#    include <openrct2/network/ServerList.h>
#    include <openrct2/network/network.h>
#    include <openrct2/platform/platform.h>
#    include <openrct2/sprites.h>
#    include <openrct2/util/Util.h>
#    include <tuple>

#    define WWIDTH_MIN 500
#    define WHEIGHT_MIN 300
#    define WWIDTH_MAX 1200
#    define WHEIGHT_MAX 800
#    define ITEM_HEIGHT (3 + 9 + 3)

static char _playerName[32 + 1];
static ServerList _serverList;
static std::future<std::tuple<std::vector<ServerListEntry>, rct_string_id>> _fetchFuture;
static uint32_t _numPlayersOnline = 0;
static rct_string_id _statusText = STR_SERVER_LIST_CONNECTING;

// clang-format off
enum {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PLAYER_NAME_INPUT,
    WIDX_LIST,
    WIDX_FETCH_SERVERS,
    WIDX_ADD_SERVER,
    WIDX_START_SERVER
};

enum {
    WIDX_LIST_REMOVE,
    WIDX_LIST_SPECTATE
};

static rct_widget window_server_list_widgets[] = {
    MakeWidget({  0,  0}, {341, 91}, WindowWidgetType::Frame,    WindowColour::Primary                                           ), // panel / background
    MakeWidget({  1,  1}, {338, 14}, WindowWidgetType::Caption,  WindowColour::Primary  , STR_SERVER_LIST,   STR_WINDOW_TITLE_TIP), // title bar
    MakeWidget({327,  2}, { 11, 12}, WindowWidgetType::CloseBox, WindowColour::Primary  , STR_CLOSE_X,       STR_CLOSE_WINDOW_TIP), // close x button
    MakeWidget({100, 20}, {245, 12}, WindowWidgetType::TextBox, WindowColour::Secondary                                         ), // player name text box
    MakeWidget({  6, 37}, {332, 14}, WindowWidgetType::Scroll,   WindowColour::Secondary                                         ), // server list
    MakeWidget({  6, 53}, {101, 14}, WindowWidgetType::Button,   WindowColour::Secondary, STR_FETCH_SERVERS                      ), // fetch servers button
    MakeWidget({112, 53}, {101, 14}, WindowWidgetType::Button,   WindowColour::Secondary, STR_ADD_SERVER                         ), // add server button
    MakeWidget({218, 53}, {101, 14}, WindowWidgetType::Button,   WindowColour::Secondary, STR_START_SERVER                       ), // start server button
    { WIDGETS_END },
};

static void window_server_list_close(rct_window *w);
static void window_server_list_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_server_list_resize(rct_window *w);
static void window_server_list_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_server_list_update(rct_window *w);
static void window_server_list_scroll_getsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_server_list_scroll_mousedown(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_server_list_scroll_mouseover(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_server_list_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static OpenRCT2String window_server_list_tooltip(rct_window* const w, const rct_widgetindex widgetIndex, rct_string_id fallback);
static void window_server_list_invalidate(rct_window *w);
static void window_server_list_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_server_list_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static rct_window_event_list window_server_list_events([](auto& events)
{
    events.close = &window_server_list_close;
    events.mouse_up = &window_server_list_mouseup;
    events.resize = &window_server_list_resize;
    events.dropdown = &window_server_list_dropdown;
    events.update = &window_server_list_update;
    events.get_scroll_size = &window_server_list_scroll_getsize;
    events.scroll_mousedown = &window_server_list_scroll_mousedown;
    events.scroll_mouseover = &window_server_list_scroll_mouseover;
    events.text_input = &window_server_list_textinput;
    events.tooltip = &window_server_list_tooltip;
    events.invalidate = &window_server_list_invalidate;
    events.paint = &window_server_list_paint;
    events.scroll_paint = &window_server_list_scrollpaint;
});
// clang-format on

enum
{
    DDIDX_JOIN,
    DDIDX_FAVOURITE
};

static int32_t _hoverButtonIndex = -1;
static std::string _version;

static void server_list_get_item_button(int32_t buttonIndex, int32_t x, int32_t y, int32_t width, int32_t* outX, int32_t* outY);
static void join_server(std::string address);
static void server_list_fetch_servers_begin();
static void server_list_fetch_servers_check(rct_window* w);

rct_window* window_server_list_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_SERVER_LIST);
    if (window != nullptr)
        return window;

    window = WindowCreateCentred(WWIDTH_MIN, WHEIGHT_MIN, &window_server_list_events, WC_SERVER_LIST, WF_10 | WF_RESIZABLE);

    window_server_list_widgets[WIDX_PLAYER_NAME_INPUT].string = _playerName;
    window->widgets = window_server_list_widgets;
    window->enabled_widgets
        = ((1 << WIDX_CLOSE) | (1 << WIDX_PLAYER_NAME_INPUT) | (1 << WIDX_FETCH_SERVERS) | (1 << WIDX_ADD_SERVER)
           | (1 << WIDX_START_SERVER));
    WindowInitScrollWidgets(window);
    window->no_list_items = 0;
    window->selected_list_item = -1;
    window->frame_no = 0;
    window->min_width = 320;
    window->min_height = 90;
    window->max_width = window->min_width;
    window->max_height = window->min_height;

    window->page = 0;
    window->list_information_type = 0;

    window_set_resize(window, WWIDTH_MIN, WHEIGHT_MIN, WWIDTH_MAX, WHEIGHT_MAX);

    safe_strcpy(_playerName, gConfigNetwork.player_name.c_str(), sizeof(_playerName));

    _serverList.ReadAndAddFavourites();
    window->no_list_items = static_cast<uint16_t>(_serverList.GetCount());

    server_list_fetch_servers_begin();

    return window;
}

static void window_server_list_close(rct_window* w)
{
    _serverList = {};
    _fetchFuture = {};
}

static void window_server_list_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_PLAYER_NAME_INPUT:
            window_start_textbox(w, widgetIndex, STR_STRING, _playerName, 63);
            break;
        case WIDX_LIST:
        {
            int32_t serverIndex = w->selected_list_item;
            if (serverIndex >= 0 && serverIndex < static_cast<int32_t>(_serverList.GetCount()))
            {
                const auto& server = _serverList.GetServer(serverIndex);
                if (server.IsVersionValid())
                {
                    join_server(server.Address);
                }
                else
                {
                    Formatter ft;
                    ft.Add<const char*>(server.Version.c_str());
                    context_show_error(STR_UNABLE_TO_CONNECT_TO_SERVER, STR_MULTIPLAYER_INCORRECT_SOFTWARE_VERSION, ft);
                }
            }
            break;
        }
        case WIDX_FETCH_SERVERS:
            server_list_fetch_servers_begin();
            break;
        case WIDX_ADD_SERVER:
            window_text_input_open(w, widgetIndex, STR_ADD_SERVER, STR_ENTER_HOSTNAME_OR_IP_ADDRESS, STR_NONE, 0, 128);
            break;
        case WIDX_START_SERVER:
            context_open_window(WC_SERVER_START);
            break;
    }
}

static void window_server_list_resize(rct_window* w)
{
    window_set_resize(w, WWIDTH_MIN, WHEIGHT_MIN, WWIDTH_MAX, WHEIGHT_MAX);
}

static void window_server_list_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    auto serverIndex = w->selected_list_item;
    if (serverIndex >= 0 && serverIndex < static_cast<int32_t>(_serverList.GetCount()))
    {
        auto& server = _serverList.GetServer(serverIndex);
        switch (dropdownIndex)
        {
            case DDIDX_JOIN:
                if (server.IsVersionValid())
                {
                    join_server(server.Address);
                }
                else
                {
                    Formatter ft;
                    ft.Add<const char*>(server.Version.c_str());
                    context_show_error(STR_UNABLE_TO_CONNECT_TO_SERVER, STR_MULTIPLAYER_INCORRECT_SOFTWARE_VERSION, ft);
                }
                break;
            case DDIDX_FAVOURITE:
            {
                server.Favourite = !server.Favourite;
                _serverList.WriteFavourites();
            }
            break;
        }
    }
}

static void window_server_list_update(rct_window* w)
{
    if (gCurrentTextBox.window.classification == w->classification && gCurrentTextBox.window.number == w->number)
    {
        window_update_textbox_caret();
        widget_invalidate(w, WIDX_PLAYER_NAME_INPUT);
    }
    server_list_fetch_servers_check(w);
}

static void window_server_list_scroll_getsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    *width = 0;
    *height = w->no_list_items * ITEM_HEIGHT;
}

static void window_server_list_scroll_mousedown(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    int32_t serverIndex = w->selected_list_item;
    if (serverIndex >= 0 && serverIndex < static_cast<int32_t>(_serverList.GetCount()))
    {
        const auto& server = _serverList.GetServer(serverIndex);

        auto listWidget = &w->widgets[WIDX_LIST];

        gDropdownItemsFormat[0] = STR_JOIN_GAME;
        if (server.Favourite)
        {
            gDropdownItemsFormat[1] = STR_REMOVE_FROM_FAVOURITES;
        }
        else
        {
            gDropdownItemsFormat[1] = STR_ADD_TO_FAVOURITES;
        }
        auto dropdownPos = ScreenCoordsXY{ w->windowPos.x + listWidget->left + screenCoords.x + 2 - w->scrolls[0].h_left,
                                           w->windowPos.y + listWidget->top + screenCoords.y + 2 - w->scrolls[0].v_top };
        WindowDropdownShowText(dropdownPos, 0, COLOUR_GREY, 0, 2);
    }
}

static void window_server_list_scroll_mouseover(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    // Item
    int32_t index = screenCoords.y / ITEM_HEIGHT;
    if (index < 0 || index >= w->no_list_items)
    {
        index = -1;
    }

    int32_t hoverButtonIndex = -1;
    if (index != -1)
    {
        int32_t width = w->widgets[WIDX_LIST].width();
        int32_t sy = index * ITEM_HEIGHT;
        for (int32_t i = 0; i < 2; i++)
        {
            int32_t bx, by;

            server_list_get_item_button(i, 0, sy, width, &bx, &by);
            if (screenCoords.x >= bx && screenCoords.y >= by && screenCoords.x < bx + 24 && screenCoords.y < by + 24)
            {
                hoverButtonIndex = i;
                break;
            }
        }
    }

    int32_t width = w->widgets[WIDX_LIST].width();
    int32_t right = width - 3 - 14 - 10;
    if (screenCoords.x < right)
    {
        w->widgets[WIDX_LIST].tooltip = STR_NONE;
        window_tooltip_close();
    }

    if (w->selected_list_item != index || _hoverButtonIndex != hoverButtonIndex)
    {
        w->selected_list_item = index;
        _hoverButtonIndex = hoverButtonIndex;
        window_tooltip_close();
        w->Invalidate();
    }
}

static void window_server_list_textinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    if (text == nullptr || text[0] == 0)
        return;

    switch (widgetIndex)
    {
        case WIDX_PLAYER_NAME_INPUT:
            if (strcmp(_playerName, text) == 0)
                return;

            std::fill_n(_playerName, sizeof(_playerName), 0x00);
            if (strlen(text) > 0)
            {
                safe_strcpy(_playerName, text, sizeof(_playerName));
            }

            if (strlen(_playerName) > 0)
            {
                gConfigNetwork.player_name = _playerName;
                config_save_default();
            }

            widget_invalidate(w, WIDX_PLAYER_NAME_INPUT);
            break;

        case WIDX_ADD_SERVER:
        {
            ServerListEntry entry;
            entry.Address = text;
            entry.Name = text;
            entry.Favourite = true;
            _serverList.Add(entry);
            _serverList.WriteFavourites();
            w->Invalidate();
            break;
        }
    }
}

static OpenRCT2String window_server_list_tooltip(rct_window* const w, const rct_widgetindex widgetIndex, rct_string_id fallback)
{
    auto ft = Formatter();
    ft.Add<char*>(_version.c_str());
    return { fallback, ft };
}

static void window_server_list_invalidate(rct_window* w)
{
    window_server_list_widgets[WIDX_BACKGROUND].right = w->width - 1;
    window_server_list_widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    window_server_list_widgets[WIDX_TITLE].right = w->width - 2;
    window_server_list_widgets[WIDX_CLOSE].left = w->width - 2 - 11;
    window_server_list_widgets[WIDX_CLOSE].right = w->width - 2 - 11 + 10;

    int32_t margin = 6;
    int32_t buttonHeight = 13;
    int32_t buttonTop = w->height - margin - buttonHeight - 13;
    int32_t buttonBottom = buttonTop + buttonHeight;
    int32_t listBottom = buttonTop - margin;

    window_server_list_widgets[WIDX_PLAYER_NAME_INPUT].right = w->width - 6;
    window_server_list_widgets[WIDX_LIST].left = 6;
    window_server_list_widgets[WIDX_LIST].right = w->width - 6;
    window_server_list_widgets[WIDX_LIST].bottom = listBottom;
    window_server_list_widgets[WIDX_FETCH_SERVERS].top = buttonTop;
    window_server_list_widgets[WIDX_FETCH_SERVERS].bottom = buttonBottom;
    window_server_list_widgets[WIDX_ADD_SERVER].top = buttonTop;
    window_server_list_widgets[WIDX_ADD_SERVER].bottom = buttonBottom;
    window_server_list_widgets[WIDX_START_SERVER].top = buttonTop;
    window_server_list_widgets[WIDX_START_SERVER].bottom = buttonBottom;

    w->no_list_items = static_cast<uint16_t>(_serverList.GetCount());
}

static void window_server_list_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);

    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 6, w->widgets[WIDX_PLAYER_NAME_INPUT].top }, STR_PLAYER_NAME, nullptr,
        { COLOUR_WHITE });

    // Draw version number
    std::string version = network_get_version();
    const char* versionCStr = version.c_str();
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 324, w->widgets[WIDX_START_SERVER].top + 1 }, STR_NETWORK_VERSION,
        static_cast<void*>(&versionCStr), { COLOUR_WHITE });

    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 8, w->height - 15 }, _statusText, static_cast<void*>(&_numPlayersOnline),
        { COLOUR_WHITE });
}

static void window_server_list_scrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    uint8_t paletteIndex = ColourMapA[w->colours[1]].mid_light;
    gfx_clear(dpi, paletteIndex);

    int32_t width = w->widgets[WIDX_LIST].width();

    ScreenCoordsXY screenCoords;
    screenCoords.y = 0;
    w->widgets[WIDX_LIST].tooltip = STR_NONE;
    for (int32_t i = 0; i < w->no_list_items; i++)
    {
        if (screenCoords.y >= dpi->y + dpi->height)
            continue;

        const auto& serverDetails = _serverList.GetServer(i);
        bool highlighted = i == w->selected_list_item;

        // Draw hover highlight
        if (highlighted)
        {
            gfx_filter_rect(dpi, 0, screenCoords.y, width, screenCoords.y + ITEM_HEIGHT, FilterPaletteID::PaletteDarken1);
            _version = serverDetails.Version;
            w->widgets[WIDX_LIST].tooltip = STR_NETWORK_VERSION_TIP;
        }

        colour_t colour = w->colours[1];
        if (serverDetails.Favourite)
        {
            colour = COLOUR_YELLOW;
        }
        else if (serverDetails.Local)
        {
            colour = COLOUR_MOSS_GREEN;
        }

        screenCoords.x = 3;

        // Before we draw the server info, we need to know how much room we'll need for player info.
        char players[32] = { 0 };
        if (serverDetails.MaxPlayers > 0)
        {
            snprintf(players, sizeof(players), "%d/%d", serverDetails.Players, serverDetails.MaxPlayers);
        }
        const int16_t numPlayersStringWidth = gfx_get_string_width(players, FontSpriteBase::MEDIUM);

        // How much space we have for the server info depends on the size of everything rendered after.
        const int16_t spaceAvailableForInfo = width - numPlayersStringWidth - SCROLLBAR_WIDTH - 35;

        // Are we showing the server's name or description?
        const char* serverInfoToShow = serverDetails.Name.c_str();
        if (highlighted && !serverDetails.Description.empty())
        {
            serverInfoToShow = serverDetails.Description.c_str();
        }

        // Finally, draw the server information.
        auto ft = Formatter();
        ft.Add<const char*>(serverInfoToShow);
        DrawTextEllipsised(dpi, screenCoords + ScreenCoordsXY{ 0, 3 }, spaceAvailableForInfo, STR_STRING, ft, { colour });

        int32_t right = width - 7 - SCROLLBAR_WIDTH;

        // Draw compatibility icon
        right -= 10;
        int32_t compatibilitySpriteId;
        if (serverDetails.Version.empty())
        {
            // Server not online...
            compatibilitySpriteId = SPR_G2_RCT1_CLOSE_BUTTON_0;
        }
        else
        {
            // Server online... check version
            bool correctVersion = serverDetails.Version == network_get_version();
            compatibilitySpriteId = correctVersion ? SPR_G2_RCT1_OPEN_BUTTON_2 : SPR_G2_RCT1_CLOSE_BUTTON_2;
        }
        gfx_draw_sprite(dpi, ImageId(compatibilitySpriteId), { right, screenCoords.y + 1 });
        right -= 4;

        // Draw lock icon
        right -= 8;
        if (serverDetails.RequiresPassword)
        {
            gfx_draw_sprite(dpi, ImageId(SPR_G2_LOCKED), { right, screenCoords.y + 4 });
        }
        right -= 6;

        // Draw number of players
        screenCoords.x = right - numPlayersStringWidth;
        gfx_draw_string(dpi, screenCoords + ScreenCoordsXY{ 0, 3 }, players, { w->colours[1] });

        screenCoords.y += ITEM_HEIGHT;
    }
}

static void server_list_get_item_button(int32_t buttonIndex, int32_t x, int32_t y, int32_t width, int32_t* outX, int32_t* outY)
{
    *outX = width - 3 - 36 - (30 * buttonIndex);
    *outY = y + 2;
}

static void join_server(std::string address)
{
    int32_t port = NETWORK_DEFAULT_PORT;
    auto beginBracketIndex = address.find('[');
    auto endBracketIndex = address.find(']');
    auto dotIndex = address.find('.');
    auto colonIndex = address.find_last_of(':');
    if (colonIndex != std::string::npos)
    {
        if (endBracketIndex != std::string::npos || dotIndex != std::string::npos)
        {
            auto ret = std::sscanf(&address[colonIndex + 1], "%d", &port);
            assert(ret);
            if (ret > 0)
            {
                address = address.substr(0, colonIndex);
            }
        }
    }

    if (beginBracketIndex != std::string::npos && endBracketIndex != std::string::npos)
    {
        address = address.substr(beginBracketIndex + 1, endBracketIndex - beginBracketIndex - 1);
    }

    if (!network_begin_client(address.c_str(), port))
    {
        context_show_error(STR_UNABLE_TO_CONNECT_TO_SERVER, STR_NONE, {});
    }
}

static void server_list_fetch_servers_begin()
{
    if (_fetchFuture.valid())
    {
        // A fetch is already in progress
        return;
    }

    _serverList.Clear();
    _serverList.ReadAndAddFavourites();
    _statusText = STR_SERVER_LIST_CONNECTING;

    _fetchFuture = std::async(std::launch::async, [] {
        // Spin off background fetches
        auto lanF = _serverList.FetchLocalServerListAsync();
        auto wanF = _serverList.FetchOnlineServerListAsync();

        // Merge or deal with errors
        std::vector<ServerListEntry> allEntries;
        try
        {
            auto entries = lanF.get();
            allEntries.insert(allEntries.end(), entries.begin(), entries.end());
        }
        catch (...)
        {
        }

        auto status = STR_NONE;
        try
        {
            auto entries = wanF.get();
            allEntries.insert(allEntries.end(), entries.begin(), entries.end());
        }
        catch (const MasterServerException& e)
        {
            status = e.StatusText;
        }
        catch (...)
        {
            status = STR_SERVER_LIST_NO_CONNECTION;
        }
        return std::make_tuple(allEntries, status);
    });
}

static void server_list_fetch_servers_check(rct_window* w)
{
    if (_fetchFuture.valid())
    {
        auto status = _fetchFuture.wait_for(std::chrono::seconds::zero());
        if (status == std::future_status::ready)
        {
            try
            {
                auto [entries, statusText] = _fetchFuture.get();
                _serverList.AddRange(entries);
                _numPlayersOnline = _serverList.GetTotalPlayerCount();
                _statusText = STR_X_PLAYERS_ONLINE;
                if (statusText != STR_NONE)
                {
                    _statusText = statusText;
                }
            }
            catch (const MasterServerException& e)
            {
                _statusText = e.StatusText;
            }
            catch (const std::exception& e)
            {
                _statusText = STR_SERVER_LIST_NO_CONNECTION;
                log_warning("Unable to connect to master server: %s", e.what());
            }
            _fetchFuture = {};
            w->Invalidate();
        }
    }
}

#endif
