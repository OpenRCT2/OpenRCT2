/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <mutex>
#include <numeric>
#include <vector>
#include <openrct2/config/Config.h>
#include <openrct2/core/String.hpp>
#include <openrct2/Context.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/network/Http.h>
#include <openrct2/network/network.h>
#include <openrct2/network/ServerList.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/drawing/Drawing.h>

#ifndef DISABLE_HTTP
using namespace OpenRCT2::Network;
#endif

#define WWIDTH_MIN 500
#define WHEIGHT_MIN 300
#define WWIDTH_MAX 1200
#define WHEIGHT_MAX 800
#define ITEM_HEIGHT (3 + 9 + 3)

static char _playerName[32 + 1];
static std::vector<server_entry> _serverEntries;
static std::mutex _mutex;
static uint32_t _numPlayersOnline = 0;
static rct_string_id status_text = STR_SERVER_LIST_CONNECTING;

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
    { WWT_FRAME,            0,  0,      340,    0,      90,     STR_NONE,                   STR_NONE },                 // panel / background
    { WWT_CAPTION,          0,  1,      338,    1,      14,     STR_SERVER_LIST,            STR_WINDOW_TITLE_TIP },     // title bar
    { WWT_CLOSEBOX,         0,  327,    337,    2,      13,     STR_CLOSE_X,                STR_CLOSE_WINDOW_TIP },     // close x button
    { WWT_TEXT_BOX,         1,  100,    344,    20,     31,     STR_NONE,                   STR_NONE },                 // player name text box
    { WWT_SCROLL,           1,  6,      337,    37,     50,     STR_NONE,                   STR_NONE },                 // server list
    { WWT_BUTTON,           1,  6,      106,    53,     66,     STR_FETCH_SERVERS,          STR_NONE },                 // fetch servers button
    { WWT_BUTTON,           1,  112,    212,    53,     66,     STR_ADD_SERVER,             STR_NONE },                 // add server button
    { WWT_BUTTON,           1,  218,    318,    53,     66,     STR_START_SERVER,           STR_NONE },                 // start server button
    { WIDGETS_END },
};

static void window_server_list_close(rct_window *w);
static void window_server_list_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_server_list_resize(rct_window *w);
static void window_server_list_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_server_list_update(rct_window *w);
static void window_server_list_scroll_getsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_server_list_scroll_mousedown(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y);
static void window_server_list_scroll_mouseover(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y);
static void window_server_list_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_server_list_invalidate(rct_window *w);
static void window_server_list_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_server_list_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static rct_window_event_list window_server_list_events = {
    window_server_list_close,
    window_server_list_mouseup,
    window_server_list_resize,
    nullptr,
    window_server_list_dropdown,
    nullptr,
    window_server_list_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_server_list_scroll_getsize,
    window_server_list_scroll_mousedown,
    nullptr,
    window_server_list_scroll_mouseover,
    window_server_list_textinput,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_server_list_invalidate,
    window_server_list_paint,
    window_server_list_scrollpaint
};
// clang-format on

enum {
    DDIDX_JOIN,
    DDIDX_FAVOURITE
};

static int32_t _hoverButtonIndex = -1;
static std::string _version;

static void server_list_get_item_button(int32_t buttonIndex, int32_t x, int32_t y, int32_t width, int32_t *outX, int32_t *outY);
static void server_list_load_server_entries();
static void server_list_save_server_entries();
static void dispose_server_entry_list();
static server_entry & add_server_entry(const std::string &address);
static void sort_servers();
static void join_server(std::string address);
#ifndef DISABLE_HTTP
static void fetch_servers();
static void fetch_servers_callback(Http::Response & response);
#endif
static bool is_version_valid(const std::string &version);

rct_window * window_server_list_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_SERVER_LIST);
    if (window != nullptr)
        return window;

    window = window_create_centred(WWIDTH_MIN, WHEIGHT_MIN, &window_server_list_events, WC_SERVER_LIST, WF_10 | WF_RESIZABLE);

    window_server_list_widgets[WIDX_PLAYER_NAME_INPUT].string = _playerName;
    window->widgets = window_server_list_widgets;
    window->enabled_widgets = (
        (1 << WIDX_CLOSE) |
        (1 << WIDX_PLAYER_NAME_INPUT) |
        (1 << WIDX_FETCH_SERVERS) |
        (1 << WIDX_ADD_SERVER) |
        (1 << WIDX_START_SERVER)
    );
    window_init_scroll_widgets(window);
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

    safe_strcpy(_playerName, gConfigNetwork.player_name, sizeof(_playerName));

    server_list_load_server_entries();
    window->no_list_items = (uint16_t)_serverEntries.size();

#ifndef DISABLE_HTTP
    fetch_servers();
#endif

    return window;
}

static void window_server_list_close(rct_window *w)
{
    std::lock_guard<std::mutex> guard(_mutex);
    dispose_server_entry_list();
}

static void window_server_list_mouseup(rct_window *w, rct_widgetindex widgetIndex)
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
            if (serverIndex >= 0 && serverIndex < (int32_t)_serverEntries.size())
            {
                const auto &server = _serverEntries[serverIndex];
                if (is_version_valid(server.version))
                {
                    join_server(server.address);
                }
                else
                {
                    set_format_arg(0, void *, server.version.c_str());
                    context_show_error(STR_UNABLE_TO_CONNECT_TO_SERVER, STR_MULTIPLAYER_INCORRECT_SOFTWARE_VERSION);
                }
            }
            break;
        }
    case WIDX_FETCH_SERVERS:
#ifndef DISABLE_HTTP
        fetch_servers();
#endif
        break;
    case WIDX_ADD_SERVER:
        window_text_input_open(w, widgetIndex, STR_ADD_SERVER, STR_ENTER_HOSTNAME_OR_IP_ADDRESS, STR_NONE, 0, 128);
        break;
    case WIDX_START_SERVER:
        context_open_window(WC_SERVER_START);
        break;
    }
}

static void window_server_list_resize(rct_window *w)
{
    window_set_resize(w, WWIDTH_MIN, WHEIGHT_MIN, WWIDTH_MAX, WHEIGHT_MAX);
}

static void window_server_list_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    auto serverIndex = w->selected_list_item;
    if (serverIndex >= 0 && serverIndex < (int32_t)_serverEntries.size())
    {
        auto &server = _serverEntries[serverIndex];
        switch (dropdownIndex)
        {
        case DDIDX_JOIN:
            if (is_version_valid(server.version))
            {
                join_server(server.address);
            }
            else
            {
                set_format_arg(0, void *, _serverEntries[serverIndex].version.c_str());
                context_show_error(STR_UNABLE_TO_CONNECT_TO_SERVER, STR_MULTIPLAYER_INCORRECT_SOFTWARE_VERSION);
            }
            break;
        case DDIDX_FAVOURITE:
            {
                std::lock_guard<std::mutex> guard(_mutex);
                server.favourite = !server.favourite;
                server_list_save_server_entries();
            }
            break;
        }
    }
}

static void window_server_list_update(rct_window *w)
{
    if (gCurrentTextBox.window.classification == w->classification && gCurrentTextBox.window.number == w->number) {
        window_update_textbox_caret();
        widget_invalidate(w, WIDX_PLAYER_NAME_INPUT);
    }
}

static void window_server_list_scroll_getsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height)
{
    *width = 0;
    *height = w->no_list_items * ITEM_HEIGHT;
}

static void window_server_list_scroll_mousedown(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y)
{
    int32_t serverIndex = w->selected_list_item;
    if (serverIndex < 0) return;
    if (serverIndex >= (int32_t)_serverEntries.size()) return;

    rct_widget *listWidget = &w->widgets[WIDX_LIST];
    int32_t ddx = w->x + listWidget->left + x + 2 - w->scrolls[0].h_left;
    int32_t ddy = w->y + listWidget->top + y + 2 - w->scrolls[0].v_top;

    gDropdownItemsFormat[0] = STR_JOIN_GAME;
    if (_serverEntries[serverIndex].favourite) {
        gDropdownItemsFormat[1] = STR_REMOVE_FROM_FAVOURITES;
    } else {
        gDropdownItemsFormat[1] = STR_ADD_TO_FAVOURITES;
    }
    window_dropdown_show_text(ddx, ddy, 0, COLOUR_GREY, 0, 2);
}

static void window_server_list_scroll_mouseover(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y)
{
    // Item
    int32_t index = y / ITEM_HEIGHT;
    if (index < 0 || index >= w->no_list_items) {
        index = -1;
    }

    int32_t hoverButtonIndex = -1;
    if (index != -1) {
        int32_t width = w->widgets[WIDX_LIST].right - w->widgets[WIDX_LIST].left;
        int32_t sy = index * ITEM_HEIGHT;
        for (int32_t i = 0; i < 2; i++) {
            int32_t bx, by;

            server_list_get_item_button(i, 0, sy, width, &bx, &by);
            if (x >= bx && y >= by && x < bx + 24 && y < by + 24) {
                hoverButtonIndex = i;
                break;
            }
        }
    }

    int32_t width = w->widgets[WIDX_LIST].right - w->widgets[WIDX_LIST].left;
    int32_t right = width - 3 - 14 - 10;
    if (x < right)
    {
        w->widgets[WIDX_LIST].tooltip = STR_NONE;
        window_tooltip_close();
    }

    if (w->selected_list_item != index || _hoverButtonIndex != hoverButtonIndex) {
        w->selected_list_item = index;
        _hoverButtonIndex = hoverButtonIndex;
        window_tooltip_close();
        window_invalidate(w);
    }
}

static void window_server_list_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text)
{
    if (text == nullptr || text[0] == 0) return;

    switch (widgetIndex) {
    case WIDX_PLAYER_NAME_INPUT:
        if (strcmp(_playerName, text) == 0)
            return;

        if (strlen(text) == 0) {
            memset(_playerName, 0, sizeof(_playerName));
        } else {
            memset(_playerName, 0, sizeof(_playerName));
            safe_strcpy(_playerName, text, sizeof(_playerName));
        }

        if (strlen(_playerName) > 0) {
            SafeFree(gConfigNetwork.player_name);
            gConfigNetwork.player_name = _strdup(_playerName);
            config_save_default();
        }

        widget_invalidate(w, WIDX_PLAYER_NAME_INPUT);
        break;

    case WIDX_ADD_SERVER:
        {
            std::lock_guard<std::mutex> guard(_mutex);
            auto &entry = add_server_entry(text);
            entry.favourite = true;
            sort_servers();
            server_list_save_server_entries();
        }
        window_invalidate(w);
        break;
    }
}

static void window_server_list_invalidate(rct_window *w)
{
    set_format_arg(0, char *, _version.c_str());
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

    w->no_list_items = (uint16_t)_serverEntries.size();
}

static void window_server_list_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);

    gfx_draw_string_left(dpi, STR_PLAYER_NAME, nullptr, COLOUR_WHITE, w->x + 6, w->y + w->widgets[WIDX_PLAYER_NAME_INPUT].top);

    // Draw version number
    std::string version = network_get_version();
    const char * versionCStr = version.c_str();
    gfx_draw_string_left(dpi, STR_NETWORK_VERSION, (void*)&versionCStr, COLOUR_WHITE, w->x + 324, w->y + w->widgets[WIDX_START_SERVER].top + 1);

    gfx_draw_string_left(dpi, status_text, (void *)&_numPlayersOnline, COLOUR_WHITE, w->x + 8, w->y + w->height - 15);
}

static void window_server_list_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex)
{
    std::lock_guard<std::mutex> guard(_mutex);

    uint8_t paletteIndex = ColourMapA[w->colours[1]].mid_light;
    gfx_clear(dpi, paletteIndex);

    int32_t width = w->widgets[WIDX_LIST].right - w->widgets[WIDX_LIST].left;

    int32_t y = 0;
    w->widgets[WIDX_LIST].tooltip = STR_NONE;
    for (int32_t i = 0; i < w->no_list_items; i++) {
        if (y >= dpi->y + dpi->height) continue;
        // if (y + ITEM_HEIGHT < dpi->y) continue;

        server_entry *serverDetails = &_serverEntries[i];
        bool highlighted = i == w->selected_list_item;

        // Draw hover highlight
        if (highlighted) {
            gfx_filter_rect(dpi, 0, y, width, y + ITEM_HEIGHT, PALETTE_DARKEN_1);
            _version = serverDetails->version;
            w->widgets[WIDX_LIST].tooltip = STR_NETWORK_VERSION_TIP;
        }

        int32_t colour = w->colours[1];
        if (serverDetails->favourite) {
            colour = COLOUR_YELLOW;
        }

        // Draw server information
        if (highlighted && !serverDetails->description.empty()) {
            gfx_draw_string(dpi, serverDetails->description.c_str(), colour, 3, y + 3);
        } else {
            gfx_draw_string(dpi, serverDetails->name.c_str(), colour, 3, y + 3);
        }

        int32_t right = width - 3 - 14;

        // Draw compatibility icon
        right -= 10;
        int32_t compatibilitySpriteId;
        if (serverDetails->version.empty()) {
            // Server not online...
            compatibilitySpriteId = SPR_G2_RCT1_CLOSE_BUTTON_0;
        } else {
            // Server online... check version
            bool correctVersion = serverDetails->version == network_get_version();
            compatibilitySpriteId = correctVersion ? SPR_G2_RCT1_OPEN_BUTTON_2 : SPR_G2_RCT1_CLOSE_BUTTON_2;
        }
        gfx_draw_sprite(dpi, compatibilitySpriteId, right, y + 1, 0);
        right -= 4;

        // Draw lock icon
        right -= 8;
        if (serverDetails->requiresPassword) {
            gfx_draw_sprite(dpi, SPR_G2_LOCKED, right, y + 4, 0);
        }
        right -= 6;

        // Draw number of players
        char players[32];
        players[0] = 0;
        if (serverDetails->maxplayers > 0) {
            snprintf(players, 32, "%d/%d", serverDetails->players, serverDetails->maxplayers);
        }
        int32_t numPlayersStringWidth = gfx_get_string_width(players);
        gfx_draw_string(dpi, players, w->colours[1], right - numPlayersStringWidth, y + 3);

        y += ITEM_HEIGHT;
    }
}

static void server_list_get_item_button(int32_t buttonIndex, int32_t x, int32_t y, int32_t width, int32_t *outX, int32_t *outY)
{
    *outX = width - 3 - 36 - (30 * buttonIndex);
    *outY = y + 2;
}

static void server_list_load_server_entries()
{
    auto entries = server_list_read();
    {
        std::lock_guard<std::mutex> guard(_mutex);
        dispose_server_entry_list();
        _serverEntries = entries;
        sort_servers();
    }
}

static void server_list_save_server_entries()
{
    // Save just favourite servers
    std::vector<server_entry> favouriteServers;
    std::copy_if(
        _serverEntries.begin(),
        _serverEntries.end(),
        std::back_inserter(favouriteServers),
        [](const server_entry &entry)
        {
            return entry.favourite;
        });
    server_list_write(favouriteServers);
}

static void dispose_server_entry_list()
{
    _serverEntries.clear();
    _serverEntries.shrink_to_fit();
}

static server_entry & add_server_entry(const std::string &address)
{
    auto entry = std::find_if(
        std::begin(_serverEntries),
        std::end(_serverEntries),
        [address](const server_entry &e)
        {
            return e.address == address;
        });
    if (entry != _serverEntries.end())
    {
        return *entry;
    }

    server_entry newserver;
    newserver.address = address;
    newserver.name = address;
    _serverEntries.push_back(newserver);
    return _serverEntries.back();
}

static bool server_compare(const server_entry &a, const server_entry &b)
{
    // Order by favourite
    if (a.favourite != b.favourite)
    {
        return a.favourite;
    }

    // Then by version
    bool serverACompatible = a.version == network_get_version();
    bool serverBCompatible = b.version == network_get_version();
    if (serverACompatible != serverBCompatible)
    {
        return serverACompatible;
    }

    // Then by password protection
    if (a.requiresPassword != b.requiresPassword)
    {
        return !a.requiresPassword;
    }

    // Then by name
    return String::Compare(a.name, b.name, true) < 0;
}

static void sort_servers()
{
    std::sort(
        _serverEntries.begin(),
        _serverEntries.end(),
        server_compare);
}

static void join_server(std::string address)
{
    int32_t port = gConfigNetwork.default_port;
    auto beginBracketIndex = address.find('[');
    auto endBracketIndex = address.find(']');
    auto dotIndex = address.find('.');
    auto colonIndex = address.find_last_of(':');
    if (colonIndex != std::string::npos)
    {
        if (endBracketIndex != std::string::npos || dotIndex != std::string::npos)
        {
            std::sscanf(&address[colonIndex + 1], "%d", &port);
            address = address.substr(0, colonIndex);
        }
    }

    if (beginBracketIndex != std::string::npos && endBracketIndex != std::string::npos)
    {
        address = address.substr(beginBracketIndex + 1, endBracketIndex - beginBracketIndex - 1);
    }

    if (!network_begin_client(address.c_str(), port))
    {
        context_show_error(STR_UNABLE_TO_CONNECT_TO_SERVER, STR_NONE);
    }
}

#ifndef DISABLE_HTTP
static void fetch_servers()
{
    const char *masterServerUrl = OPENRCT2_MASTER_SERVER_URL;
    if (!str_is_null_or_empty(gConfigNetwork.master_server_url)) {
        masterServerUrl = gConfigNetwork.master_server_url;
    }

    {
        std::lock_guard<std::mutex> guard(_mutex);
        _serverEntries.erase(
            std::remove_if(
                _serverEntries.begin(),
                _serverEntries.end(),
                [](const server_entry &server)
                {
                    return !server.favourite;
                }),
            _serverEntries.end());
        sort_servers();
    }

    Http::Request request;
    request.url = masterServerUrl;
    request.method           = Http::Method::GET;
    request.header["Accept"] = "application/json";
    status_text = STR_SERVER_LIST_CONNECTING;
    Http::DoAsync(request, fetch_servers_callback);
}

static uint32_t get_total_player_count()
{
    return std::accumulate(
        _serverEntries.begin(),
        _serverEntries.end(),
        0,
        [](uint32_t acc, const server_entry &entry)
        {
            return acc + entry.players;
        });
}

static void fetch_servers_callback(Http::Response & response)
{
    if (response.status != Http::Status::OK)
    {
        status_text = STR_SERVER_LIST_NO_CONNECTION;
        window_invalidate_by_class(WC_SERVER_LIST);
        log_warning("Unable to connect to master server");
        return;
    }

    json_t * root       = Json::FromString(response.body);
    json_t * jsonStatus = json_object_get(root, "status");
    if (!json_is_number(jsonStatus)) {
        status_text = STR_SERVER_LIST_INVALID_RESPONSE_JSON_NUMBER;
        window_invalidate_by_class(WC_SERVER_LIST);
        log_warning("Invalid response from master server");
        return;
    }

    int32_t status = (int32_t)json_integer_value(jsonStatus);
    if (status != 200) {
        status_text = STR_SERVER_LIST_MASTER_SERVER_FAILED;
        window_invalidate_by_class(WC_SERVER_LIST);
        log_warning("Master server failed to return servers");
        return;
    }

    json_t * jsonServers = json_object_get(root, "servers");
    if (!json_is_array(jsonServers)) {
        status_text = STR_SERVER_LIST_INVALID_RESPONSE_JSON_ARRAY;
        window_invalidate_by_class(WC_SERVER_LIST);
        log_warning("Invalid response from master server");
        return;
    }

    int32_t count = (int32_t)json_array_size(jsonServers);
    for (int32_t i = 0; i < count; i++) {
        json_t *server = json_array_get(jsonServers, i);
        if (!json_is_object(server)) {
            continue;
        }

        json_t *port = json_object_get(server, "port");
        json_t *name = json_object_get(server, "name");
        json_t *description = json_object_get(server, "description");
        json_t *requiresPassword = json_object_get(server, "requiresPassword");
        json_t *version = json_object_get(server, "version");
        json_t *players = json_object_get(server, "players");
        json_t *maxPlayers = json_object_get(server, "maxPlayers");
        json_t *ip = json_object_get(server, "ip");
        json_t *ip4 = json_object_get(ip, "v4");
        json_t *addressIp = json_array_get(ip4, 0);

        if (name == nullptr || version == nullptr)
        {
            log_verbose("Cowardly refusing to add server without name or version specified.");
            continue;
        }

        auto address = String::StdFormat("%s:%d", json_string_value(addressIp), (int32_t)json_integer_value(port));
        {
            std::lock_guard<std::mutex> guard(_mutex);
            auto &newserver = add_server_entry(address);
            newserver.name = json_string_value(name);
            newserver.requiresPassword = json_is_true(requiresPassword);
            newserver.description = (description == nullptr ? "" : json_string_value(description));
            newserver.version = json_string_value(version);
            newserver.players = (uint8_t)json_integer_value(players);
            newserver.maxplayers = (uint8_t)json_integer_value(maxPlayers);
        }
    }

    sort_servers();
    _numPlayersOnline = get_total_player_count();

    status_text = STR_X_PLAYERS_ONLINE;
    window_invalidate_by_class(WC_SERVER_LIST);
}
#endif

static bool is_version_valid(const std::string &version)
{
    return version.empty() || version == network_get_version();
}
