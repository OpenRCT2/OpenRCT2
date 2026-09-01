/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

    #include <cassert>
    #include <chrono>
    #include <openrct2-ui/interface/Dropdown.h>
    #include <openrct2-ui/interface/Widget.h>
    #include <openrct2-ui/interface/Window.h>
    #include <openrct2-ui/windows/Windows.h>
    #include <openrct2/Context.h>
    #include <openrct2/Diagnostic.h>
    #include <openrct2/SpriteIds.h>
    #include <openrct2/config/Config.h>
    #include <openrct2/competitive/CompetitiveSession.h>
    #include <openrct2/drawing/ColourMap.h>
    #include <openrct2/drawing/Drawing.String.h>
    #include <openrct2/drawing/Drawing.h>
    #include <openrct2/drawing/Rectangle.h>
    #include <openrct2/drawing/RenderTarget.h>
    #include <openrct2/drawing/Text.h>
    #include <openrct2/localisation/Formatter.h>
    #include <openrct2/network/Network.h>
    #include <openrct2/network/ServerList.h>
    #include <openrct2/ui/WindowManager.h>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
    static constexpr ScreenSize kMinimumWindowSize = { 600, 330 };
    static constexpr ScreenSize kMaximumWindowSize = { 1200, 830 };
    static constexpr ScreenSize kWindowSize = kMinimumWindowSize;
    static constexpr int32_t kItemHeight = (3 + 9 + 3);

    static constexpr size_t kMaxPlayerNameLength = 32;

    enum WindowServerListWidgetIdx : WidgetIndex
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_TAB_COOPERATIVE,
        WIDX_TAB_COMPETITIVE,
        WIDX_PLAYER_NAME_INPUT,
        WIDX_LIST,
        WIDX_FETCH_SERVERS,
        WIDX_ADD_SERVER,
        WIDX_START_SERVER,
        WIDX_JOIN,
        WIDX_SPECTATE
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
    static constexpr auto _serverListWidgets = makeWidgets(
        makeWindowShim(STR_SERVER_LIST, kWindowSize),
        makeWidget({  3, 17}, {120,  26}, WidgetType::button,   WindowColour::secondary, kStringIdEmpty                         ), // cooperative tab
        makeWidget({123, 17}, {120,  26}, WidgetType::button,   WindowColour::secondary, kStringIdEmpty                         ), // competitive tab
        makeWidget({100, 47}, {245,  12}, WidgetType::textBox,  WindowColour::secondary                                         ), // player name text box
        makeWidget({  6, 64}, {589, 226}, WidgetType::scroll,   WindowColour::secondary                                         ), // server list
        makeWidget({  6, 53}, {101,  14}, WidgetType::button,   WindowColour::secondary, STR_FETCH_SERVERS                      ), // fetch servers button
        makeWidget({112, 53}, {101,  14}, WidgetType::button,   WindowColour::secondary, STR_ADD_SERVER                         ), // add server button
        makeWidget({218, 53}, {121,  14}, WidgetType::button,   WindowColour::secondary, STR_START_SERVER                       ), // start server button
        makeWidget({344, 53}, {121,  14}, WidgetType::button,   WindowColour::secondary, STR_JOIN_GAME                          ), // join button
        makeWidget({470, 53}, {121,  14}, WidgetType::button,   WindowColour::secondary, kStringIdEmpty                         )  // spectate button
    );
    // clang-format on

    void JoinServer(std::string address);

    class ServerListWindow final : public Window
    {
    private:
        u8string _playerName;
        Network::ServerList _serverList;
        std::vector<size_t> _visibleServerIndices;
        std::future<std::pair<std::vector<Network::ServerListEntry>, StringId>> _fetchFuture;
        uint32_t _numPlayersOnline = 0;
        StringId _statusText = STR_SERVER_LIST_CONNECTING;

        bool _showNetworkVersionTooltip = false;
        std::string _version;
        std::string _cooperativeTabText = "Cooperative";
        std::string _competitiveTabText = "Competitive";
        std::string _hostCompetitiveText = "Host competition";
        std::string _spectateText = "Observe match";

    public:
    #pragma region Window Override Events

        void onOpen() override
        {
            _playerName = Config::Get().network.playerName;
            setWidgets(_serverListWidgets);
            widgets[WIDX_TAB_COOPERATIVE].setString(_cooperativeTabText.c_str());
            widgets[WIDX_TAB_COMPETITIVE].setString(_competitiveTabText.c_str());
            widgets[WIDX_SPECTATE].setString(_spectateText.c_str());
            widgets[WIDX_PLAYER_NAME_INPUT].string = const_cast<utf8*>(_playerName.c_str());
            initScrollWidgets();

            numListItems = 0;
            selectedListItem = -1;
            currentFrame = 0;
            page = 0;
            listInformationType = 0;

            WindowSetResize(*this, kMinimumWindowSize, kMaximumWindowSize);

            UpdateVisibleServers();

            ServerListFetchServersBegin();
        }

        void onClose() override
        {
            _serverList = {};
            _fetchFuture = {};
            Config::Save();
        }

        void ShowCompetitivePage()
        {
            SetPage(1);
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_PLAYER_NAME_INPUT:
                    WindowStartTextbox(*this, widgetIndex, _playerName, kMaxPlayerNameLength);
                    break;
                case WIDX_TAB_COOPERATIVE:
                    SetPage(0);
                    break;
                case WIDX_TAB_COMPETITIVE:
                    SetPage(1);
                    break;
                case WIDX_LIST:
                    break;
                case WIDX_FETCH_SERVERS:
                    ServerListFetchServersBegin();
                    break;
                case WIDX_ADD_SERVER:
                    textInputOpen(widgetIndex, STR_ADD_SERVER, STR_ENTER_HOSTNAME_OR_IP_ADDRESS, {}, kStringIdNone, 0, 128);
                    break;
                case WIDX_START_SERVER:
                    if (page == 0)
                    {
                        ContextOpenWindow(WindowClass::serverStart);
                    }
                    else
                    {
                        ServerStartOpenCompetitive();
                    }
                    break;
                case WIDX_JOIN:
                    JoinSelected(Competitive::Role::player);
                    break;
                case WIDX_SPECTATE:
                    JoinSelected(Competitive::Role::spectator);
                    break;
            }
        }

        void onResize() override
        {
            WindowSetResize(*this, kMinimumWindowSize, kMaximumWindowSize);
        }

        void onDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            if (selectedIndex == -1)
            {
                return;
            }
            auto* server = GetSelectedServer();
            if (server != nullptr)
            {
                switch (selectedIndex)
                {
                    case DDIDX_JOIN:
                        JoinSelected(Competitive::Role::player);
                        break;
                    case DDIDX_FAVOURITE:
                    {
                        server->Favourite = !server->Favourite;
                        _serverList.WriteFavourites();
                    }
                    break;
                }
            }
        }

        void onUpdate() override
        {
            if (GetCurrentTextBox().window.classification == classification && GetCurrentTextBox().window.number == number)
            {
                WindowUpdateTextboxCaret();
                invalidateWidget(WIDX_PLAYER_NAME_INPUT);
            }
            ServerListFetchServersCheck();
        }

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            return { 0, numListItems * kItemHeight };
        }

        void onScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            const auto* server = GetSelectedServer();
            if (server != nullptr)
            {
                const auto& listWidget = widgets[WIDX_LIST];

                std::array<Dropdown::Item, 2> dropdownItems = {
                    Dropdown::PlainMenuLabel(STR_JOIN_GAME),
                    Dropdown::PlainMenuLabel(server->Favourite ? STR_REMOVE_FROM_FAVOURITES : STR_ADD_TO_FAVOURITES),
                };

                auto dropdownPos = ScreenCoordsXY{
                    windowPos.x + listWidget.left + screenCoords.x + 2 - scrolls[0].contentOffsetX,
                    windowPos.y + listWidget.top + screenCoords.y + 2 - scrolls[0].contentOffsetY
                };
                WindowDropdownShowText(dropdownPos, 0, { Drawing::Colour::grey }, { Dropdown::Flag::autoClose }, dropdownItems);
            }
        }

        void onScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto& listWidget = widgets[WIDX_LIST];

            int32_t itemIndex = screenCoords.y / kItemHeight;
            bool showNetworkVersionTooltip = false;
            if (itemIndex < 0 || itemIndex >= numListItems)
            {
                itemIndex = -1;
            }
            else
            {
                const int32_t iconX = listWidget.width() - 1 - kScrollBarWidth - 7 - 10;
                showNetworkVersionTooltip = screenCoords.x > iconX;
            }

            if (selectedListItem != itemIndex || _showNetworkVersionTooltip != showNetworkVersionTooltip)
            {
                selectedListItem = itemIndex;
                _showNetworkVersionTooltip = showNetworkVersionTooltip;

                listWidget.tooltip = showNetworkVersionTooltip ? static_cast<StringId>(STR_NETWORK_VERSION_TIP) : kStringIdNone;
                WindowTooltipClose();

                invalidate();
            }
        }

        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (text.empty())
                return;

            auto temp = u8string{ text };

            switch (widgetIndex)
            {
                case WIDX_PLAYER_NAME_INPUT:
                    if (_playerName == text)
                        return;

                    _playerName = temp;
                    Config::Get().network.playerName = _playerName;
                    widgets[WIDX_PLAYER_NAME_INPUT].string = const_cast<utf8*>(_playerName.c_str());

                    invalidateWidget(WIDX_PLAYER_NAME_INPUT);
                    break;

                case WIDX_ADD_SERVER:
                {
                    Network::ServerListEntry entry;
                    entry.Address = text;
                    entry.Name = text;
                    entry.Favourite = true;
                    entry.Kind = page == 1 ? Network::ServerKind::competitive : Network::ServerKind::cooperative;
                    _serverList.Add(entry);
                    _serverList.WriteFavourites();
                    UpdateVisibleServers();
                    invalidate();
                    break;
                }
            }
        }

        StringWithArgs onTooltip(WidgetIndex widgetIndex, StringId fallback) override
        {
            auto ft = Formatter();
            ft.Add<char*>(_version.c_str());
            return { fallback, ft };
        }

        void onDraw(RenderTarget& rt) override
        {
            drawWidgets(rt);

            drawText(
                rt, windowPos + ScreenCoordsXY{ 6, widgets[WIDX_PLAYER_NAME_INPUT].top }, STR_PLAYER_NAME,
                { Drawing::Colour::white });

            // Keep compatibility information visible without occupying the player-name field.
            std::string version = Network::GetVersion();
            auto ft = Formatter();
            ft.Add<const char*>(version.c_str());
            drawText(
                rt, windowPos + ScreenCoordsXY{ 254, 24 }, STR_NETWORK_VERSION, ft,
                { Drawing::Colour::white });

            ft = Formatter();
            ft.Add<uint32_t>(_numPlayersOnline);
            drawText(rt, windowPos + ScreenCoordsXY{ 8, height - 15 }, _statusText, ft, { Drawing::Colour::white });
        }

        void onScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            auto paletteIndex = getColourMap(colours[1].colour).midLight;
            GfxClear(rt, paletteIndex);

            auto& listWidget = widgets[WIDX_LIST];
            int32_t listWidgetWidth = listWidget.width() - 1;

            ScreenCoordsXY screenCoords;
            screenCoords.y = 0;
            for (int32_t i = 0; i < numListItems; i++)
            {
                if (screenCoords.y >= rt.y + rt.height)
                    continue;

                const auto& serverDetails = _serverList.GetServer(_visibleServerIndices[i]);
                bool highlighted = i == selectedListItem;

                // Draw hover highlight
                if (highlighted)
                {
                    Rectangle::filter(
                        rt, { 0, screenCoords.y, listWidgetWidth, screenCoords.y + kItemHeight },
                        FilterPaletteID::paletteDarken1);
                    _version = serverDetails.Version;
                }

                auto colour = colours[1];
                if (serverDetails.Favourite)
                {
                    colour = Drawing::Colour::yellow;
                }
                else if (serverDetails.Local)
                {
                    colour = Drawing::Colour::mossGreen;
                }

                screenCoords.x = 3;

                // Before we draw the server info, we need to know how much room we'll need for player info.
                char players[32] = { 0 };
                if (serverDetails.MaxPlayers > 0)
                {
                    snprintf(players, sizeof(players), "%d/%d", serverDetails.Players, serverDetails.MaxPlayers);
                }
                const int16_t numPlayersStringWidth = getStringWidth(players, FontStyle::medium);

                // How much space we have for the server info depends on the size of everything rendered after.
                const int16_t spaceAvailableForInfo = listWidgetWidth - numPlayersStringWidth - kScrollBarWidth - 35;

                // Are we showing the server's name or description?
                const char* serverInfoToShow = serverDetails.Name.c_str();
                if (highlighted && !serverDetails.Description.empty())
                {
                    serverInfoToShow = serverDetails.Description.c_str();
                }

                // Finally, draw the server information.
                drawTextEllipsised(
                    rt, screenCoords + ScreenCoordsXY{ 0, 3 }, spaceAvailableForInfo, serverInfoToShow, { colour });

                int32_t right = listWidgetWidth - 7 - kScrollBarWidth;

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
                    bool correctVersion = serverDetails.Version == Network::GetVersion();
                    compatibilitySpriteId = correctVersion ? SPR_G2_RCT1_OPEN_BUTTON_2 : SPR_G2_RCT1_CLOSE_BUTTON_2;
                }
                GfxDrawSprite(rt, ImageId(compatibilitySpriteId), { right, screenCoords.y + 1 });
                right -= 4;

                // Draw lock icon
                right -= 8;
                if (serverDetails.RequiresPassword)
                {
                    GfxDrawSprite(rt, ImageId(SPR_G2_LOCKED), { right, screenCoords.y + 4 });
                }
                right -= 6;

                // Draw number of players
                screenCoords.x = right - numPlayersStringWidth;
                drawText(rt, screenCoords + ScreenCoordsXY{ 0, 3 }, players, { colours[1] });

                screenCoords.y += kItemHeight;
            }
        }

    #pragma endregion

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
                std::vector<Network::ServerListEntry> allEntries;
                try
                {
                    auto entries = lanF.get();
                    allEntries.reserve(entries.size());
                    allEntries.insert(allEntries.end(), entries.begin(), entries.end());
                }
                // TODO: Stop catching all exceptions
                catch (...)
                {
                }

                auto status = kStringIdNone;
                try
                {
                    auto entries = wanF.get();
                    allEntries.reserve(allEntries.capacity() + entries.size());
                    allEntries.insert(allEntries.end(), entries.begin(), entries.end());
                }
                catch (const Network::MasterServerException& e)
                {
                    status = e.StatusText;
                }
                // TODO: Stop catching all exceptions
                catch (...)
                {
                    status = STR_SERVER_LIST_NO_CONNECTION;
                }
                return std::make_pair(allEntries, status);
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
                        _statusText = STR_X_PLAYERS_ONLINE;
                        if (statusText != kStringIdNone)
                        {
                            _statusText = statusText;
                        }
                    }
                    catch (const Network::MasterServerException& e)
                    {
                        _statusText = e.StatusText;
                    }
                    catch (const std::exception& e)
                    {
                        _statusText = STR_SERVER_LIST_NO_CONNECTION;
                        LOG_WARNING("Unable to connect to master server: %s", e.what());
                    }
                    _fetchFuture = {};
                    UpdateVisibleServers();
                    invalidate();
                }
            }
        }

        void onPrepareDraw() override
        {
            int32_t margin = 6;
            int32_t buttonHeight = 13;
            int32_t buttonTop = height - margin - buttonHeight - 13;
            int32_t buttonBottom = buttonTop + buttonHeight;
            int32_t listBottom = buttonTop - margin;

            widgets[WIDX_PLAYER_NAME_INPUT].right = width - 6;
            widgets[WIDX_LIST].left = 6;
            widgets[WIDX_LIST].right = width - 6;
            widgets[WIDX_LIST].bottom = listBottom;
            widgets[WIDX_FETCH_SERVERS].top = buttonTop;
            widgets[WIDX_FETCH_SERVERS].bottom = buttonBottom;
            widgets[WIDX_ADD_SERVER].top = buttonTop;
            widgets[WIDX_ADD_SERVER].bottom = buttonBottom;
            widgets[WIDX_START_SERVER].top = buttonTop;
            widgets[WIDX_START_SERVER].bottom = buttonBottom;
            widgets[WIDX_JOIN].top = buttonTop;
            widgets[WIDX_JOIN].bottom = buttonBottom;
            widgets[WIDX_SPECTATE].top = buttonTop;
            widgets[WIDX_SPECTATE].bottom = buttonBottom;

            const bool competitive = page == 1;
            widgets[WIDX_SPECTATE].setVisible(competitive);
            if (competitive)
                widgets[WIDX_START_SERVER].setString(_hostCompetitiveText.c_str());
            else
                widgets[WIDX_START_SERVER].setString(STR_START_SERVER);

            setWidgetPressed(WIDX_TAB_COOPERATIVE, !competitive);
            setWidgetPressed(WIDX_TAB_COMPETITIVE, competitive);

            constexpr int32_t buttonGap = 5;
            const auto buttonCount = competitive ? 5 : 4;
            const auto buttonWidth = (width - (2 * margin) - ((buttonCount - 1) * buttonGap)) / buttonCount;
            std::array<WidgetIndex, 5> buttonIndices = {
                WIDX_FETCH_SERVERS, WIDX_ADD_SERVER, WIDX_START_SERVER, WIDX_JOIN, WIDX_SPECTATE
            };
            int32_t buttonLeft = margin;
            for (int32_t i = 0; i < buttonCount; i++)
            {
                auto& widget = widgets[buttonIndices[i]];
                widget.left = buttonLeft;
                widget.right = buttonLeft + buttonWidth - 1;
                buttonLeft += buttonWidth + buttonGap;
            }

            const auto* server = GetSelectedServer();
            const bool compatible = server != nullptr && server->IsVersionValid();
            bool mayJoin = compatible;
            if (mayJoin && competitive && server->CompetitionPhase != "lobby" && !server->AllowLateJoin)
            {
                mayJoin = false;
            }
            setWidgetDisabled(WIDX_JOIN, !mayJoin);
            setWidgetDisabled(WIDX_SPECTATE, !compatible);
        }

        void SetPage(uint8_t newPage)
        {
            if (page == newPage)
            {
                return;
            }
            page = newPage;
            selectedListItem = -1;
            _showNetworkVersionTooltip = false;
            UpdateVisibleServers();
            invalidate();
        }

        void UpdateVisibleServers()
        {
            _visibleServerIndices.clear();
            _numPlayersOnline = 0;
            const bool competitive = page == 1;
            for (size_t i = 0; i < _serverList.GetCount(); i++)
            {
                const auto& server = _serverList.GetServer(i);
                if (server.IsCompetitive() == competitive)
                {
                    _visibleServerIndices.push_back(i);
                    _numPlayersOnline += server.Players;
                }
            }
            numListItems = static_cast<uint16_t>(_visibleServerIndices.size());
            if (selectedListItem >= numListItems)
            {
                selectedListItem = -1;
            }
        }

        Network::ServerListEntry* GetSelectedServer()
        {
            if (selectedListItem < 0 || selectedListItem >= static_cast<int32_t>(_visibleServerIndices.size()))
            {
                return nullptr;
            }
            return &_serverList.GetServer(_visibleServerIndices[selectedListItem]);
        }

        static std::pair<std::string, uint16_t> ParseAddress(std::string address, uint16_t defaultPort)
        {
            int32_t port = defaultPort;
            const auto endBracketIndex = address.find(']');
            const auto colonIndex = address.find_last_of(':');
            if (colonIndex != std::string::npos)
            {
                const auto dotIndex = address.find('.');
                if (endBracketIndex != std::string::npos || dotIndex != std::string::npos)
                {
                    int32_t parsedPort{};
                    if (std::sscanf(&address[colonIndex + 1], "%d", &parsedPort) == 1 && parsedPort > 0
                        && parsedPort <= UINT16_MAX)
                    {
                        port = parsedPort;
                        address = address.substr(0, colonIndex);
                    }
                }
            }

            const auto beginBracketIndex = address.find('[');
            if (beginBracketIndex != std::string::npos && endBracketIndex != std::string::npos)
            {
                address = address.substr(beginBracketIndex + 1, endBracketIndex - beginBracketIndex - 1);
            }
            return { address, static_cast<uint16_t>(port) };
        }

        void JoinSelected(Competitive::Role role)
        {
            const auto* server = GetSelectedServer();
            if (server == nullptr)
            {
                return;
            }
            if (!server->IsVersionValid())
            {
                Formatter ft;
                ft.Add<const char*>(server->Version.c_str());
                ContextShowError(STR_UNABLE_TO_CONNECT_TO_SERVER, STR_MULTIPLAYER_INCORRECT_SOFTWARE_VERSION, ft);
                return;
            }

            if (!server->IsCompetitive())
            {
                JoinServer(server->Address);
                return;
            }
            if (server->CompetitiveProtocol != 0 && server->CompetitiveProtocol != Competitive::kProtocolVersion)
            {
                ErrorOpen("Cannot join competition", "This competition uses an incompatible competitive protocol version.");
                return;
            }

            auto [host, port] = ParseAddress(server->Address, Competitive::kDefaultPort);
            Competitive::JoinConfiguration configuration;
            configuration.host = std::move(host);
            configuration.port = port;
            configuration.playerName = _playerName;
            configuration.role = role;
            std::string error;
            if (!Competitive::GetSession().Join(configuration, error))
            {
                ErrorOpen("Cannot join competition", error);
            }
        }
    };

    WindowBase* ServerListOpen()
    {
        // Check if window is already open
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->BringToFrontByClass(WindowClass::serverList);
        if (window != nullptr)
            return window;

        window = windowMgr->Create<ServerListWindow>(
            WindowClass::serverList, kMinimumWindowSize,
            { WindowFlag::higherContrastOnPress, WindowFlag::resizable, WindowFlag::centreScreen });

        return window;
    }

    WindowBase* ServerListOpenCompetitive()
    {
        auto* window = ServerListOpen();
        static_cast<ServerListWindow*>(window)->ShowCompetitivePage();
        return window;
    }

    void JoinServer(std::string address)
    {
        int32_t port = Network::kDefaultPort;
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

        if (auto beginBracketIndex = address.find('[');
            beginBracketIndex != std::string::npos && endBracketIndex != std::string::npos)
        {
            address = address.substr(beginBracketIndex + 1, endBracketIndex - beginBracketIndex - 1);
        }

        if (!Network::BeginClient(address, port))
        {
            ContextShowError(STR_UNABLE_TO_CONNECT_TO_SERVER, kStringIdNone, {});
        }
    }
} // namespace OpenRCT2::Ui::Windows
#endif
