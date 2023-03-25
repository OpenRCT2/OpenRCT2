/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

#    include <chrono>
#    include <openrct2-ui/interface/Dropdown.h>
#    include <openrct2-ui/interface/Widget.h>
#    include <openrct2-ui/windows/Window.h>
#    include <openrct2/Context.h>
#    include <openrct2/config/Config.h>
#    include <openrct2/core/Json.hpp>
#    include <openrct2/drawing/Drawing.h>
#    include <openrct2/interface/Colour.h>
#    include <openrct2/localisation/Formatter.h>
#    include <openrct2/localisation/Localisation.h>
#    include <openrct2/network/ServerList.h>
#    include <openrct2/network/network.h>
#    include <openrct2/platform/Platform.h>
#    include <openrct2/sprites.h>
#    include <openrct2/util/Util.h>
#    include <tuple>

#    define WWIDTH_MIN 500
#    define WHEIGHT_MIN 300
#    define WWIDTH_MAX 1200
#    define WHEIGHT_MAX 800
#    define ITEM_HEIGHT (3 + 9 + 3)

enum
{
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PLAYER_NAME_INPUT,
    WIDX_LIST,
    WIDX_FETCH_SERVERS,
    WIDX_ADD_SERVER,
    WIDX_START_SERVER
};

enum
{
    WIDX_LIST_REMOVE,
    WIDX_LIST_SPECTATE
};

enum
{
    DDIDX_JOIN,
    DDIDX_FAVOURITE
};

// clang-format off
static Widget window_server_list_widgets[] = {
    MakeWidget({  0,  0}, {341, 91}, WindowWidgetType::Frame,    WindowColour::Primary                                           ), // panel / background
    MakeWidget({  1,  1}, {338, 14}, WindowWidgetType::Caption,  WindowColour::Primary,   STR_SERVER_LIST,   STR_WINDOW_TITLE_TIP), // title bar
    MakeWidget({327,  2}, { 11, 12}, WindowWidgetType::CloseBox, WindowColour::Primary,   STR_CLOSE_X,       STR_CLOSE_WINDOW_TIP), // close x button
    MakeWidget({100, 20}, {245, 12}, WindowWidgetType::TextBox,  WindowColour::Secondary                                         ), // player name text box
    MakeWidget({  6, 37}, {332, 14}, WindowWidgetType::Scroll,   WindowColour::Secondary                                         ), // server list
    MakeWidget({  6, 53}, {101, 14}, WindowWidgetType::Button,   WindowColour::Secondary, STR_FETCH_SERVERS                      ), // fetch servers button
    MakeWidget({112, 53}, {101, 14}, WindowWidgetType::Button,   WindowColour::Secondary, STR_ADD_SERVER                         ), // add server button
    MakeWidget({218, 53}, {101, 14}, WindowWidgetType::Button,   WindowColour::Secondary, STR_START_SERVER                       ), // start server button
    WIDGETS_END,
};
// clang-format on

void JoinServer(std::string address)
{
    int32_t port = NETWORK_DEFAULT_PORT;
    auto endBracketIndex = address.find(']');
    auto colonIndex = address.find_last_of(':');
    if (colonIndex != std::string::npos)
    {
        if (auto dotIndex = address.find('.'); endBracketIndex != std::string::npos || dotIndex != std::string::npos)
        {
            auto ret = std::sscanf(&address[colonIndex + 1], "%d", &port);
            assert(ret);
            if (ret > 0)
            {
                address = address.substr(0, colonIndex);
            }
        }
    }

    if (auto beginBracketIndex = address.find('['); beginBracketIndex != std::string::npos && endBracketIndex != std::string::npos)
    {
        address = address.substr(beginBracketIndex + 1, endBracketIndex - beginBracketIndex - 1);
    }

    if (!NetworkBeginClient(address, port))
    {
        ContextShowError(STR_UNABLE_TO_CONNECT_TO_SERVER, STR_NONE, {});
    }
}


class ServerListWindow final : public Window
{
private:
    char _playerName[32 + 1] = {};
    ServerList _serverList;
    std::future<std::tuple<std::vector<ServerListEntry>, StringId>> _fetchFuture;
    uint32_t _numPlayersOnline = 0;
    StringId _statusText = STR_SERVER_LIST_CONNECTING;

    bool _showNetworkVersionTooltip = false;
    std::string _version;

public:
#    pragma region Window Override Events

    void OnOpen() override
    {
        window_server_list_widgets[WIDX_PLAYER_NAME_INPUT].string = _playerName;
        widgets = window_server_list_widgets;
        InitScrollWidgets();
        no_list_items = 0;
        selected_list_item = -1;
        frame_no = 0;
        min_width = 320;
        min_height = 90;
        max_width = min_width;
        max_height = min_height;

        page = 0;
        list_information_type = 0;

        WindowSetResize(*this, WWIDTH_MIN, WHEIGHT_MIN, WWIDTH_MAX, WHEIGHT_MAX);

        SafeStrCpy(_playerName, gConfigNetwork.PlayerName.c_str(), sizeof(_playerName));

        no_list_items = static_cast<uint16_t>(_serverList.GetCount());

        ServerListFetchServersBegin();
    }

    void OnClose() override
    {
        _serverList = {};
        _fetchFuture = {};
    }

    void OnMouseUp(WidgetIndex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
            case WIDX_PLAYER_NAME_INPUT:
                WindowStartTextbox(*this, widgetIndex, STR_STRING, _playerName, 63);
                break;
            case WIDX_LIST:
            {
                int32_t serverIndex = selected_list_item;
                if (serverIndex >= 0 && serverIndex < static_cast<int32_t>(_serverList.GetCount()))
                {
                    const auto& server = _serverList.GetServer(serverIndex);
                    if (server.IsVersionValid())
                    {
                        JoinServer(server.Address);
                    }
                    else
                    {
                        Formatter ft;
                        ft.Add<const char*>(server.Version.c_str());
                        ContextShowError(STR_UNABLE_TO_CONNECT_TO_SERVER, STR_MULTIPLAYER_INCORRECT_SOFTWARE_VERSION, ft);
                    }
                }
                break;
            }
            case WIDX_FETCH_SERVERS:
                ServerListFetchServersBegin();
                break;
            case WIDX_ADD_SERVER:
                TextInputOpen(widgetIndex, STR_ADD_SERVER, STR_ENTER_HOSTNAME_OR_IP_ADDRESS, {}, STR_NONE, 0, 128);
                break;
            case WIDX_START_SERVER:
                ContextOpenWindow(WindowClass::ServerStart);
                break;
        }
    }

    void OnResize() override
    {
        WindowSetResize(*this, WWIDTH_MIN, WHEIGHT_MIN, WWIDTH_MAX, WHEIGHT_MAX);
    }

    void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
    {
        auto serverIndex = selected_list_item;
        if (serverIndex >= 0 && serverIndex < static_cast<int32_t>(_serverList.GetCount()))
        {
            auto& server = _serverList.GetServer(serverIndex);
            switch (selectedIndex)
            {
                case DDIDX_JOIN:
                    if (server.IsVersionValid())
                    {
                        JoinServer(server.Address);
                    }
                    else
                    {
                        Formatter ft;
                        ft.Add<const char*>(server.Version.c_str());
                        ContextShowError(STR_UNABLE_TO_CONNECT_TO_SERVER, STR_MULTIPLAYER_INCORRECT_SOFTWARE_VERSION, ft);
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

    void OnUpdate() override
    {
        if (gCurrentTextBox.window.classification == classification && gCurrentTextBox.window.number == number)
        {
            WindowUpdateTextboxCaret();
            InvalidateWidget(WIDX_PLAYER_NAME_INPUT);
        }
        ServerListFetchServersCheck();
    }

    ScreenSize OnScrollGetSize(int32_t scrollIndex) override
    {
        return { 0, no_list_items * ITEM_HEIGHT };
    }

    void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        int32_t serverIndex = selected_list_item;
        if (serverIndex >= 0 && serverIndex < static_cast<int32_t>(_serverList.GetCount()))
        {
            const auto& server = _serverList.GetServer(serverIndex);

            const auto& listWidget = widgets[WIDX_LIST];

            gDropdownItems[0].Format = STR_JOIN_GAME;
            if (server.Favourite)
            {
                gDropdownItems[1].Format = STR_REMOVE_FROM_FAVOURITES;
            }
            else
            {
                gDropdownItems[1].Format = STR_ADD_TO_FAVOURITES;
            }
            auto dropdownPos = ScreenCoordsXY{ windowPos.x + listWidget.left + screenCoords.x + 2 - scrolls[0].h_left,
                                               windowPos.y + listWidget.top + screenCoords.y + 2 - scrolls[0].v_top };
            WindowDropdownShowText(dropdownPos, 0, COLOUR_GREY, 0, 2);
        }
    }

    void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        auto& listWidget = widgets[WIDX_LIST];

        int32_t itemIndex = screenCoords.y / ITEM_HEIGHT;
        bool showNetworkVersionTooltip = false;
        if (itemIndex < 0 || itemIndex >= no_list_items)
        {
            itemIndex = -1;
        }
        else
        {
            const int32_t iconX = listWidget.width() - SCROLLBAR_WIDTH - 7 - 10;
            showNetworkVersionTooltip = screenCoords.x > iconX;
        }

        if (selected_list_item != itemIndex || _showNetworkVersionTooltip != showNetworkVersionTooltip)
        {
            selected_list_item = itemIndex;
            _showNetworkVersionTooltip = showNetworkVersionTooltip;

            listWidget.tooltip = showNetworkVersionTooltip ? static_cast<StringId>(STR_NETWORK_VERSION_TIP) : STR_NONE;
            WindowTooltipClose();

            Invalidate();
        }
    }

    void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
    {
        if (text.empty())
            return;

        std::string temp = static_cast<std::string>(text);

        switch (widgetIndex)
        {
            case WIDX_PLAYER_NAME_INPUT:
                if (_playerName == text)
                    return;

                text.copy(_playerName, sizeof(_playerName));

                // Don't allow empty player names
                if (_playerName[0] != '\0')
                {
                    gConfigNetwork.PlayerName = _playerName;
                    ConfigSaveDefault();
                }

                InvalidateWidget(WIDX_PLAYER_NAME_INPUT);
                break;

            case WIDX_ADD_SERVER:
            {
                ServerListEntry entry;
                entry.Address = text;
                entry.Name = text;
                entry.Favourite = true;
                _serverList.Add(entry);
                _serverList.WriteFavourites();
                Invalidate();
                break;
            }
        }
    }

    OpenRCT2String OnTooltip(WidgetIndex widgetIndex, StringId fallback) override
    {
        auto ft = Formatter();
        ft.Add<char*>(_version.c_str());
        return { fallback, ft };
    }

    void OnDraw(DrawPixelInfo& dpi) override
    {
        DrawWidgets(dpi);

        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ 6, widgets[WIDX_PLAYER_NAME_INPUT].top }, STR_PLAYER_NAME, {}, { COLOUR_WHITE });

        // Draw version number
        std::string version = NetworkGetVersion();
        auto ft = Formatter();
        ft.Add<const char*>(version.c_str());
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ 324, widgets[WIDX_START_SERVER].top + 1 }, STR_NETWORK_VERSION, ft,
            { COLOUR_WHITE });

        ft = Formatter();
        ft.Add<uint32_t>(_numPlayersOnline);
        DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ 8, height - 15 }, _statusText, ft, { COLOUR_WHITE });
    }

    void OnScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi) override
    {
        uint8_t paletteIndex = ColourMapA[colours[1]].mid_light;
        GfxClear(&dpi, paletteIndex);

        auto& listWidget = widgets[WIDX_LIST];
        int32_t listWidgetWidth = listWidget.width();

        ScreenCoordsXY screenCoords;
        screenCoords.y = 0;
        for (int32_t i = 0; i < no_list_items; i++)
        {
            if (screenCoords.y >= dpi.y + dpi.height)
                continue;

            const auto& serverDetails = _serverList.GetServer(i);
            bool highlighted = i == selected_list_item;

            // Draw hover highlight
            if (highlighted)
            {
                GfxFilterRect(
                    &dpi, { 0, screenCoords.y, listWidgetWidth, screenCoords.y + ITEM_HEIGHT },
                    FilterPaletteID::PaletteDarken1);
                _version = serverDetails.Version;
            }

            colour_t colour = colours[1];
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
            const int16_t numPlayersStringWidth = GfxGetStringWidth(players, FontStyle::Medium);

            // How much space we have for the server info depends on the size of everything rendered after.
            const int16_t spaceAvailableForInfo = listWidgetWidth - numPlayersStringWidth - SCROLLBAR_WIDTH - 35;

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

            int32_t right = listWidgetWidth - 7 - SCROLLBAR_WIDTH;

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
                bool correctVersion = serverDetails.Version == NetworkGetVersion();
                compatibilitySpriteId = correctVersion ? SPR_G2_RCT1_OPEN_BUTTON_2 : SPR_G2_RCT1_CLOSE_BUTTON_2;
            }
            GfxDrawSprite(&dpi, ImageId(compatibilitySpriteId), { right, screenCoords.y + 1 });
            right -= 4;

            // Draw lock icon
            right -= 8;
            if (serverDetails.RequiresPassword)
            {
                GfxDrawSprite(&dpi, ImageId(SPR_G2_LOCKED), { right, screenCoords.y + 4 });
            }
            right -= 6;

            // Draw number of players
            screenCoords.x = right - numPlayersStringWidth;
            GfxDrawString(dpi, screenCoords + ScreenCoordsXY{ 0, 3 }, players, { colours[1] });

            screenCoords.y += ITEM_HEIGHT;
        }
    }

#    pragma endregion

private:
    void ServerListFetchServersBegin()
    {
        if (_fetchFuture.valid())
        {
            // A fetch is already in progress
            return;
        }

        _serverList.Clear();
        _serverList.ReadAndAddFavourites();
        _statusText = STR_SERVER_LIST_CONNECTING;

        _fetchFuture = std::async(std::launch::async, [this] {
            // Spin off background fetches
            auto lanF = _serverList.FetchLocalServerListAsync();
            auto wanF = _serverList.FetchOnlineServerListAsync();

            // Merge or deal with errors
            std::vector<ServerListEntry> allEntries;
            try
            {
                auto entries = lanF.get();
                allEntries.reserve(entries.size());
                allEntries.insert(allEntries.end(), entries.begin(), entries.end());
            }
            catch (...)
            {
            }

            auto status = STR_NONE;
            try
            {
                auto entries = wanF.get();
                allEntries.reserve(allEntries.capacity() + entries.size());
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

    void ServerListFetchServersCheck()
    {
        if (_fetchFuture.valid())
        {
            auto status = _fetchFuture.wait_for(std::chrono::seconds::zero());
            if (status == std::future_status::ready)
            {
                try
                {
                    auto [entries, statusText] = _fetchFuture.get();
                    _serverList.AddOrUpdateRange(entries);
                    _serverList.WriteFavourites(); // Update favourites in case favourited server info changes
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
                    LOG_WARNING("Unable to connect to master server: %s", e.what());
                }
                _fetchFuture = {};
                Invalidate();
            }
        }
    }

    void OnPrepareDraw() override
    {
        ResizeFrame();

        int32_t margin = 6;
        int32_t buttonHeight = 13;
        int32_t buttonTop = height - margin - buttonHeight - 13;
        int32_t buttonBottom = buttonTop + buttonHeight;
        int32_t listBottom = buttonTop - margin;

        window_server_list_widgets[WIDX_PLAYER_NAME_INPUT].right = width - 6;
        window_server_list_widgets[WIDX_LIST].left = 6;
        window_server_list_widgets[WIDX_LIST].right = width - 6;
        window_server_list_widgets[WIDX_LIST].bottom = listBottom;
        window_server_list_widgets[WIDX_FETCH_SERVERS].top = buttonTop;
        window_server_list_widgets[WIDX_FETCH_SERVERS].bottom = buttonBottom;
        window_server_list_widgets[WIDX_ADD_SERVER].top = buttonTop;
        window_server_list_widgets[WIDX_ADD_SERVER].bottom = buttonBottom;
        window_server_list_widgets[WIDX_START_SERVER].top = buttonTop;
        window_server_list_widgets[WIDX_START_SERVER].bottom = buttonBottom;

        no_list_items = static_cast<uint16_t>(_serverList.GetCount());
    }
};

WindowBase* WindowServerListOpen()
{
    // Check if window is already open
    auto* window = WindowBringToFrontByClass(WindowClass::ServerList);
    if (window != nullptr)
        return window;

    window = WindowCreate<ServerListWindow>(
        WindowClass::ServerList, WWIDTH_MIN, WHEIGHT_MIN, WF_10 | WF_RESIZABLE | WF_CENTRE_SCREEN);

    return window;
}

#endif
