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

#include <mutex>
#include <openrct2/config/Config.h>
#include <openrct2/network/network.h>
#include <openrct2/network/ServerList.h>
#include <openrct2/Context.h>
#include <openrct2-ui/windows/Window.h>

extern "C"
{
    #include <openrct2/interface/widget.h>
    #include <openrct2/localisation/localisation.h>
    #include <openrct2/network/http.h>
    #include <openrct2/sprites.h>
    #include <openrct2/util/util.h>
    #include <openrct2/windows/dropdown.h>
    #include <openrct2/windows/tooltip.h>
    #include <openrct2/windows/error.h>
}

#define WWIDTH_MIN 500
#define WHEIGHT_MIN 300
#define WWIDTH_MAX 1200
#define WHEIGHT_MAX 800
#define ITEM_HEIGHT (3 + 9 + 3)

static char _playerName[32 + 1];
static server_entry *_serverEntries = NULL;
static sint32 _numServerEntries = 0;
static std::mutex _mutex;
static uint32 _numPlayersOnline = 0;

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
    { WWT_FRAME,            0,  0,      340,    0,      90,     0xFFFFFFFF,                 STR_NONE },                 // panel / background
    { WWT_CAPTION,          0,  1,      338,    1,      14,     STR_SERVER_LIST,            STR_WINDOW_TITLE_TIP },     // title bar
    { WWT_CLOSEBOX,         0,  327,    337,    2,      13,     STR_CLOSE_X,                STR_CLOSE_WINDOW_TIP },     // close x button
    { WWT_TEXT_BOX,         1,  100,    344,    20,     31,     STR_NONE,                   STR_NONE },                 // player name text box
    { WWT_SCROLL,           1,  6,      337,    37,     50,     STR_NONE,                   STR_NONE },                 // server list
    { WWT_DROPDOWN_BUTTON,  1,  6,      106,    53,     64,     STR_FETCH_SERVERS,          STR_NONE },                 // fetch servers button
    { WWT_DROPDOWN_BUTTON,  1,  112,    212,    53,     64,     STR_ADD_SERVER,             STR_NONE },                 // add server button
    { WWT_DROPDOWN_BUTTON,  1,  218,    318,    53,     64,     STR_START_SERVER,           STR_NONE },                 // start server button
    { WIDGETS_END },
};

static void window_server_list_close(rct_window *w);
static void window_server_list_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_server_list_resize(rct_window *w);
static void window_server_list_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_server_list_update(rct_window *w);
static void window_server_list_scroll_getsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height);
static void window_server_list_scroll_mousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_server_list_scroll_mouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_server_list_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_server_list_invalidate(rct_window *w);
static void window_server_list_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_server_list_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex);

static rct_window_event_list window_server_list_events = {
    window_server_list_close,
    window_server_list_mouseup,
    window_server_list_resize,
    NULL,
    window_server_list_dropdown,
    NULL,
    window_server_list_update,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_server_list_scroll_getsize,
    window_server_list_scroll_mousedown,
    NULL,
    window_server_list_scroll_mouseover,
    window_server_list_textinput,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_server_list_invalidate,
    window_server_list_paint,
    window_server_list_scrollpaint
};

enum {
    DDIDX_JOIN,
    DDIDX_FAVOURITE
};

static sint32 _hoverButtonIndex = -1;
static char * _version = NULL;

static void server_list_get_item_button(sint32 buttonIndex, sint32 x, sint32 y, sint32 width, sint32 *outX, sint32 *outY);
static void server_list_load_server_entries();
static void server_list_save_server_entries();
static void dispose_server_entry_list();
static void dispose_server_entry(server_entry *serverInfo);
static server_entry* add_server_entry(char *address);
static void remove_server_entry(sint32 index);
static void sort_servers();
static void join_server(char *address);
static void fetch_servers();
#ifndef DISABLE_HTTP
static void fetch_servers_callback(http_response_t* response);
#endif

rct_window * window_server_list_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_SERVER_LIST);
    if (window != NULL)
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
    window->no_list_items = _numServerEntries;

    fetch_servers();

    return window;
}

static void window_server_list_close(rct_window *w)
{
    std::lock_guard<std::mutex> guard(_mutex);
    dispose_server_entry_list();
}

static void window_server_list_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_PLAYER_NAME_INPUT:
        window_start_textbox(w, widgetIndex, STR_STRING, _playerName, 63);
        break;
    case WIDX_LIST:{
        sint32 serverIndex = w->selected_list_item;
        if (serverIndex >= 0 && serverIndex < _numServerEntries) {
            if (strcmp(_serverEntries[serverIndex].version, NETWORK_STREAM_ID) != 0 && strcmp(_serverEntries[serverIndex].version, "") != 0) {
                set_format_arg(0, void *, _serverEntries[serverIndex].version);
                window_error_open(STR_UNABLE_TO_CONNECT_TO_SERVER, STR_MULTIPLAYER_INCORRECT_SOFTWARE_VERSION);
                break;
            }
            char *serverAddress = _serverEntries[serverIndex].address;
            join_server(serverAddress);
        }
        }break;
    case WIDX_FETCH_SERVERS:
        fetch_servers();
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

static void window_server_list_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
    sint32 serverIndex = w->selected_list_item;
    if (serverIndex < 0) return;
    if (serverIndex >= _numServerEntries) return;

    char *serverAddress = _serverEntries[serverIndex].address;

    switch (dropdownIndex) {
    case DDIDX_JOIN:
        if (strcmp(_serverEntries[serverIndex].version, NETWORK_STREAM_ID) != 0 && strcmp(_serverEntries[serverIndex].version, "") != 0) {
            set_format_arg(0, void *, _serverEntries[serverIndex].version);
            window_error_open(STR_UNABLE_TO_CONNECT_TO_SERVER, STR_MULTIPLAYER_INCORRECT_SOFTWARE_VERSION);
            break;
        }
        join_server(serverAddress);
        break;
    case DDIDX_FAVOURITE:
        {
            std::lock_guard<std::mutex> guard(_mutex);
            _serverEntries[serverIndex].favourite = !_serverEntries[serverIndex].favourite;
            server_list_save_server_entries();
        }
        break;
    }
}

static void window_server_list_update(rct_window *w)
{
    if (gCurrentTextBox.window.classification == w->classification && gCurrentTextBox.window.number == w->number) {
        window_update_textbox_caret();
        widget_invalidate(w, WIDX_PLAYER_NAME_INPUT);
    }
}

static void window_server_list_scroll_getsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height)
{
    *width = 0;
    *height = w->no_list_items * ITEM_HEIGHT;
}

static void window_server_list_scroll_mousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
    sint32 serverIndex = w->selected_list_item;
    if (serverIndex < 0) return;
    if (serverIndex >= _numServerEntries) return;

    rct_widget *listWidget = &w->widgets[WIDX_LIST];
    sint32 ddx = w->x + listWidget->left + x + 2 - w->scrolls[0].h_left;
    sint32 ddy = w->y + listWidget->top + y + 2 - w->scrolls[0].v_top;

    gDropdownItemsFormat[0] = STR_JOIN_GAME;
    if (_serverEntries[serverIndex].favourite) {
        gDropdownItemsFormat[1] = STR_REMOVE_FROM_FAVOURITES;
    } else {
        gDropdownItemsFormat[1] = STR_ADD_TO_FAVOURITES;
    }
    window_dropdown_show_text(ddx, ddy, 0, COLOUR_GREY, 0, 2);
}

static void window_server_list_scroll_mouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
    // Item
    sint32 index = y / ITEM_HEIGHT;
    if (index < 0 || index >= w->no_list_items) {
        index = -1;
    }

    sint32 hoverButtonIndex = -1;
    if (index != -1) {
        sint32 width = w->widgets[WIDX_LIST].right - w->widgets[WIDX_LIST].left;
        sint32 sy = index * ITEM_HEIGHT;
        for (sint32 i = 0; i < 2; i++) {
            sint32 bx, by;

            server_list_get_item_button(i, 0, sy, width, &bx, &by);
            if (x >= bx && y >= by && x < bx + 24 && y < by + 24) {
                hoverButtonIndex = i;
                break;
            }
        }
    }

    sint32 width = w->widgets[WIDX_LIST].right - w->widgets[WIDX_LIST].left;
    sint32 right = width - 3 - 14 - 10;
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
    if (text == NULL || text[0] == 0) return;

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
            add_server_entry(text);
            sort_servers();
            server_list_save_server_entries();
        }
        window_invalidate(w);
        break;
    }
}

static void window_server_list_invalidate(rct_window *w)
{
    set_format_arg(0, char *, _version);
    window_server_list_widgets[WIDX_BACKGROUND].right = w->width - 1;
    window_server_list_widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    window_server_list_widgets[WIDX_TITLE].right = w->width - 2;
    window_server_list_widgets[WIDX_CLOSE].left = w->width - 2 - 11;
    window_server_list_widgets[WIDX_CLOSE].right = w->width - 2 - 11 + 10;

    sint32 margin = 6;
    sint32 buttonHeight = 11;
    sint32 buttonTop = w->height - margin - buttonHeight - 13;
    sint32 buttonBottom = buttonTop + buttonHeight;
    sint32 listBottom = buttonTop - margin;

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

    w->no_list_items = _numServerEntries;
}

static void window_server_list_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);

    gfx_draw_string_left(dpi, STR_PLAYER_NAME, NULL, COLOUR_WHITE, w->x + 6, w->y + w->widgets[WIDX_PLAYER_NAME_INPUT].top);
    const char * version = NETWORK_STREAM_ID;
    gfx_draw_string_left(dpi, STR_NETWORK_VERSION, (void*)&version, COLOUR_WHITE, w->x + 324, w->y + w->widgets[WIDX_START_SERVER].top);

    gfx_draw_string_left(dpi, STR_X_PLAYERS_ONLINE, (void*)&_numPlayersOnline, COLOUR_WHITE, w->x + 8, w->y + w->height - 15);
}

static void window_server_list_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex)
{
    std::lock_guard<std::mutex> guard(_mutex);

    uint8 paletteIndex = ColourMapA[w->colours[1]].mid_light;
    gfx_clear(dpi, paletteIndex);

    sint32 width = w->widgets[WIDX_LIST].right - w->widgets[WIDX_LIST].left;

    sint32 y = 0;
    w->widgets[WIDX_LIST].tooltip = STR_NONE;
    for (sint32 i = 0; i < w->no_list_items; i++) {
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

        sint32 colour = w->colours[1];
        if (serverDetails->favourite) {
            colour = COLOUR_YELLOW;
        }

        // Draw server information
        if (highlighted && !str_is_null_or_empty(serverDetails->description)) {
            gfx_draw_string(dpi, serverDetails->description, colour, 3, y + 3);
        } else {
            gfx_draw_string(dpi, serverDetails->name, colour, 3, y + 3);
        }

        sint32 right = width - 3 - 14;

        // Draw compatibility icon
        right -= 10;
        sint32 compatibilitySpriteId;
        if (str_is_null_or_empty(serverDetails->version)) {
            // Server not online...
            compatibilitySpriteId = SPR_G2_RCT1_CLOSE_BUTTON_0;
        } else {
            // Server online... check version
            bool correctVersion = strcmp(serverDetails->version, NETWORK_STREAM_ID) == 0;
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
        sint32 numPlayersStringWidth = gfx_get_string_width(players);
        gfx_draw_string(dpi, players, w->colours[1], right - numPlayersStringWidth, y + 3);

        y += ITEM_HEIGHT;
    }
}

static void server_list_get_item_button(sint32 buttonIndex, sint32 x, sint32 y, sint32 width, sint32 *outX, sint32 *outY)
{
    *outX = width - 3 - 36 - (30 * buttonIndex);
    *outY = y + 2;
}

static void server_list_load_server_entries()
{
    uint32 numEntries;
    server_entry * entries;
    if (server_list_read(&numEntries, &entries)) {
        std::lock_guard<std::mutex> guard(_mutex);
        dispose_server_entry_list();
        _numServerEntries = numEntries;
        _serverEntries = entries;
        sort_servers();
    }
}

static void server_list_save_server_entries()
{
    // Get total number of favourite servers
    sint32 count = 0;
    for (sint32 i = 0; i < _numServerEntries; i++) {
        server_entry *serverInfo = &_serverEntries[i];
        if (serverInfo->favourite) {
            count++;
        }
    }

    // Create temporary list of just favourite servers
    server_entry * entries = (server_entry *)calloc(count, sizeof(server_entry));
    sint32 eindex = 0;
    for (sint32 i = 0; i < _numServerEntries; i++) {
        server_entry *serverInfo = &_serverEntries[i];
        if (serverInfo->favourite) {
            entries[eindex++] = *serverInfo;
        }
    }

    // Save servers
    server_list_write(count, entries);

    // Free temporary list
    free(entries);
}

static void dispose_server_entry_list()
{
    if (_serverEntries != NULL) {
        for (sint32 i = 0; i < _numServerEntries; i++) {
            dispose_server_entry(&_serverEntries[i]);
        }
        SafeFree(_serverEntries);
    }
    _numServerEntries = 0;
}

static void dispose_server_entry(server_entry *serverInfo)
{
    SafeFree(serverInfo->address);
    SafeFree(serverInfo->name);
    SafeFree(serverInfo->description);
    SafeFree(serverInfo->version);
}

static server_entry* add_server_entry(char *address)
{
    for (sint32 i = 0; i < _numServerEntries; i++) {
        if (strcmp(_serverEntries[i].address, address) == 0) {
            return &_serverEntries[i];
        }
    }

    _numServerEntries++;
    if (_serverEntries == NULL) {
        _serverEntries = (server_entry *)malloc(_numServerEntries * sizeof(server_entry));
    } else {
        _serverEntries = (server_entry *)realloc(_serverEntries, _numServerEntries * sizeof(server_entry));
    }

    sint32 index = _numServerEntries - 1;
    server_entry* newserver = &_serverEntries[index];
    newserver->address = _strdup(address);
    newserver->name = _strdup(address);
    newserver->requiresPassword = false;
    newserver->description = _strdup("");
    newserver->version = _strdup("");
    newserver->favourite = false;
    newserver->players = 0;
    newserver->maxplayers = 0;
    return newserver;
}

static void remove_server_entry(sint32 index)
{
    if (_numServerEntries > index) {
        sint32 serversToMove = _numServerEntries - index - 1;
        memmove(&_serverEntries[index], &_serverEntries[index + 1], serversToMove * sizeof(server_entry));

        _numServerEntries--;
        _serverEntries = (server_entry *)realloc(_serverEntries, _numServerEntries * sizeof(server_entry));
    }
}

static sint32 server_compare(const void *a, const void *b)
{
    const server_entry *serverA = (const server_entry*)a;
    const server_entry *serverB = (const server_entry*)b;

    // Order by favourite
    if (serverA->favourite != serverB->favourite) {
        if (serverA->favourite) return -1;
        else return 1;
    }

    // Then by version
    bool serverACompatible = strcmp(serverA->version, NETWORK_STREAM_ID) == 0;
    bool serverBCompatible = strcmp(serverB->version, NETWORK_STREAM_ID) == 0;
    if (serverACompatible != serverBCompatible) {
        if (serverACompatible) return -1;
        else return 1;
    }

    // Then by password protection
    if (serverA->requiresPassword != serverB->requiresPassword) {
        if (!serverA->requiresPassword) return -1;
        else return 1;
    }

    // Then by name
    return _strcmpi(serverA->name, serverB->name);
}

static void sort_servers()
{
    if (_serverEntries == NULL) {
        return;
    }
    qsort(_serverEntries, _numServerEntries, sizeof(server_entry), server_compare);
}

static char *substr(char *start, sint32 length)
{
    char *result = (char *)malloc(length + 1);
    memcpy(result, start, length);
    result[length] = 0;
    return result;
}

static void join_server(char *address)
{
    sint32 port = gConfigNetwork.default_port;

    bool addresscopied = false;

    char *endbracket = strrchr(address, ']');
    char *startbracket = strrchr(address, '[');
    char *dot = strchr(address, '.');

    char *colon = strrchr(address, ':');
    if (colon != NULL && (endbracket != NULL || dot != NULL)) {
        address = substr(address, (sint32)(colon - address));
        sscanf(colon + 1, "%d", &port);
        addresscopied = true;
    }

    if (startbracket && endbracket) {
        address = substr(startbracket + 1, (sint32)(endbracket - startbracket - 1));
        addresscopied = true;
    }

    if (!network_begin_client(address, port)) {
        window_error_open(STR_UNABLE_TO_CONNECT_TO_SERVER, STR_NONE);
    }

    if (addresscopied) {
        free(address);
    }
}

static uint32 get_total_player_count()
{
    uint32 numPlayers = 0;
    for (sint32 i = 0; i < _numServerEntries; i++) {
        server_entry *serverDetails = &_serverEntries[i];
        numPlayers += serverDetails->players;
    }
    return numPlayers;
}

static void fetch_servers()
{
#ifndef DISABLE_HTTP
    const char *masterServerUrl = OPENRCT2_MASTER_SERVER_URL;
    if (!str_is_null_or_empty(gConfigNetwork.master_server_url)) {
        masterServerUrl = gConfigNetwork.master_server_url;
    }

    {
        std::lock_guard<std::mutex> guard(_mutex);
        for (sint32 i = 0; i < _numServerEntries; i++) {
            if (!_serverEntries[i].favourite) {
                remove_server_entry(i);
                i = 0;
            }
        }
        sort_servers();
    }

    http_request_t request = {};
    request.url = masterServerUrl;
    request.method = HTTP_METHOD_GET;
    request.body = NULL;
    request.type = HTTP_DATA_JSON;
    http_request_async(&request, fetch_servers_callback);
#endif
}

#ifndef DISABLE_HTTP
static void fetch_servers_callback(http_response_t* response)
{
    if (response == NULL) {
        log_warning("Unable to connect to master server");
        return;
    }

    json_t *jsonStatus = json_object_get(response->root, "status");
    if (!json_is_number(jsonStatus)) {
        http_request_dispose(response);
        log_warning("Invalid response from master server");
        return;
    }

    sint32 status = (sint32)json_integer_value(jsonStatus);
    if (status != 200) {
        http_request_dispose(response);
        log_warning("Master server failed to return servers");
        return;
    }

    json_t *jsonServers = json_object_get(response->root, "servers");
    if (!json_is_array(jsonServers)) {
        http_request_dispose(response);
        log_warning("Invalid response from master server");
        return;
    }

    sint32 count = (sint32)json_array_size(jsonServers);
    for (sint32 i = 0; i < count; i++) {
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

        if (name == NULL || version == NULL)
        {
            log_verbose("Cowardly refusing to add server without name or version specified.");
            continue;
        }

        char address[256];
        snprintf(address, sizeof(address), "%s:%d", json_string_value(addressIp), (sint32)json_integer_value(port));

        {
            std::lock_guard<std::mutex> guard(_mutex);
            server_entry* newserver = add_server_entry(address);
            SafeFree(newserver->name);
            SafeFree(newserver->description);
            SafeFree(newserver->version);
            newserver->name = _strdup(json_string_value(name));
            newserver->requiresPassword = json_is_true(requiresPassword);
            newserver->description = _strdup(description == NULL ? "" : json_string_value(description));
            newserver->version = _strdup(json_string_value(version));
            newserver->players = (uint8)json_integer_value(players);
            newserver->maxplayers = (uint8)json_integer_value(maxPlayers);
        }
    }
    http_request_dispose(response);

    sort_servers();
    _numPlayersOnline = get_total_player_count();

    rct_window *window = window_find_by_class(WC_SERVER_LIST);
    if (window != NULL) {
        window_invalidate(window);
    }
}
#endif
