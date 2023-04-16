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

static bool IsServerPlayerInvisible()
{
    return NetworkIsServerPlayerInvisible() && !gConfigGeneral.DebuggingTools;
}

class MultiplayerWindow final : public Window
{
private:
    std::optional<ScreenCoordsXY> _windowInformationSize;
    uint8_t _selectedGroup{ 0 };

private:
    void ResetPressedWidgets();

    void InformationPaint(DrawPixelInfo& dpi);
    void PlayersPaint(DrawPixelInfo& dpi);
    void GroupsPaint(DrawPixelInfo& dpi);

    void DrawTabImage(DrawPixelInfo& dpi, int32_t page_number, int32_t spriteIndex);
    void DrawTabImages(DrawPixelInfo& dpi);

    void PlayersScrollPaint(int32_t scrollIndex, DrawPixelInfo& dpi) const;
    void GroupsScrollPaint(int32_t scrollIndex, DrawPixelInfo& dpi) const;

    void ShowGroupDropdown(WidgetIndex widgetIndex);
    ScreenCoordsXY InformationGetSize();

public:
    MultiplayerWindow();

    void SetPage(int32_t page_number);

    void OnMouseUp(WidgetIndex widgetIndex) override;
    void OnResize() override;
    void OnUpdate() override;
    void OnPrepareDraw() override;
    void OnDraw(DrawPixelInfo& dpi) override;

    void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override;
    void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override;
    void OnMouseDown(WidgetIndex widgetIndex) override;

    ScreenSize OnScrollGetSize(int32_t scrollIndex) override;
    void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override;
    void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override;
    void OnScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi) override;
};

WindowBase* WindowMultiplayerOpen()
{
    // Check if window is already open
    WindowBase* window = WindowBringToFrontByClass(WindowClass::Multiplayer);
    if (window == nullptr)
    {
        window = WindowCreate<MultiplayerWindow>(WindowClass::Multiplayer, 320, 144, WF_10 | WF_RESIZABLE | WF_AUTO_POSITION);
    }

    return window;
}

MultiplayerWindow::MultiplayerWindow()
{
    SetPage(WINDOW_MULTIPLAYER_PAGE_INFORMATION);
}

void MultiplayerWindow::SetPage(int32_t page_number)
{
    _windowInformationSize.reset();

    page = page_number;
    frame_no = 0;
    no_list_items = 0;
    selected_list_item = -1;

    hold_down_widgets = 0;
    pressed_widgets = 0;
    widgets = window_multiplayer_page_widgets[page];
    widgets[WIDX_TITLE].text = WindowMultiplayerPageTitles[page];

    WindowEventResizeCall(this);
    WindowEventInvalidateCall(this);
    InitScrollWidgets();
    Invalidate();
}

void MultiplayerWindow::OnMouseUp(WidgetIndex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            Close();
            break;
        case WIDX_TAB1:
        case WIDX_TAB2:
        case WIDX_TAB3:
        case WIDX_TAB4:
            if (page != widgetIndex - WIDX_TAB1)
            {
                SetPage(widgetIndex - WIDX_TAB1);
            }
            break;
    }
    switch (page)
    {
        case WINDOW_MULTIPLAYER_PAGE_GROUPS:
        {
            switch (widgetIndex)
            {
                case WIDX_ADD_GROUP:
                {
                    auto networkModifyGroup = NetworkModifyGroupAction(ModifyGroupType::AddGroup);
                    GameActions::Execute(&networkModifyGroup);
                    break;
                }
                case WIDX_REMOVE_GROUP:
                {
                    auto networkModifyGroup = NetworkModifyGroupAction(ModifyGroupType::RemoveGroup, _selectedGroup);
                    GameActions::Execute(&networkModifyGroup);
                    break;
                }
                case WIDX_RENAME_GROUP:
                {
                    int32_t groupIndex = NetworkGetGroupIndex(_selectedGroup);
                    const utf8* groupName = NetworkGetGroupName(groupIndex);
                    WindowTextInputRawOpen(
                        this, widgetIndex, STR_GROUP_NAME, STR_ENTER_NEW_NAME_FOR_THIS_GROUP, {}, groupName, 32);
                    break;
                }
            }
            break;
        }
        case WINDOW_MULTIPLAYER_PAGE_OPTIONS:
        {
            switch (widgetIndex)
            {
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
            break;
        }
    }
}

ScreenCoordsXY MultiplayerWindow::InformationGetSize()
{
    assert(!_windowInformationSize.has_value());

    int32_t lineHeight = FontGetLineHeight(FontStyle::Medium);

    // Base dimensions.
    const int32_t baseWidth = 450;
    int32_t baseHeight = 55;

    // Server name is displayed word-wrapped, so figure out how high it will be.
    {
        int32_t numLines;
        GfxWrapString(NetworkGetServerName(), baseWidth, FontStyle::Medium, nullptr, &numLines);
        baseHeight += (numLines + 1) * lineHeight + (LIST_ROW_HEIGHT / 2);
    }

    // Likewise, for the optional server description -- which can be a little longer.
    const auto& descString = NetworkGetServerDescription();
    if (!descString.empty())
    {
        int32_t numLines;
        GfxWrapString(descString, baseWidth, FontStyle::Medium, nullptr, &numLines);
        baseHeight += (numLines + 1) * lineHeight + (LIST_ROW_HEIGHT / 2);
    }

    // Finally, account for provider info, if present.
    {
        const auto& providerName = NetworkGetServerProviderName();
        if (!providerName.empty())
            baseHeight += LIST_ROW_HEIGHT;

        const auto& providerEmail = NetworkGetServerProviderEmail();
        if (!providerEmail.empty())
            baseHeight += LIST_ROW_HEIGHT;

        const auto& providerWebsite = NetworkGetServerProviderWebsite();
        if (!providerWebsite.empty())
            baseHeight += LIST_ROW_HEIGHT;
    }

    // TODO: Are these casts still neccessary?
    _windowInformationSize = { static_cast<int16_t>(baseWidth), static_cast<int16_t>(baseHeight) };
    return _windowInformationSize.value();
}

void MultiplayerWindow::OnResize()
{
    switch (page)
    {
        case WINDOW_MULTIPLAYER_PAGE_INFORMATION:
        {
            auto size = _windowInformationSize ? _windowInformationSize.value() : InformationGetSize();
            WindowSetResize(*this, size.x, size.y, size.x, size.y);
            break;
        }
        case WINDOW_MULTIPLAYER_PAGE_PLAYERS:
        {
            WindowSetResize(*this, 420, 124, 500, 450);

            no_list_items = (IsServerPlayerInvisible() ? NetworkGetNumVisiblePlayers() : NetworkGetNumPlayers());
            list_item_positions[0] = 0;

            widgets[WIDX_HEADER_PING].right = width - 5;

            selected_list_item = -1;
            Invalidate();
            break;
        }
        case WINDOW_MULTIPLAYER_PAGE_GROUPS:
        {
            WindowSetResize(*this, 320, 200, 320, 500);

            no_list_items = NetworkGetNumActions();
            list_item_positions[0] = 0;

            selected_list_item = -1;
            Invalidate();
            break;
        }
        case WINDOW_MULTIPLAYER_PAGE_OPTIONS:
        {
            WindowSetResize(*this, 300, 100, 300, 100);
            break;
        }
    }
}

void MultiplayerWindow::OnUpdate()
{
    frame_no++;
    InvalidateWidget(WIDX_TAB1 + page);
}

void MultiplayerWindow::ResetPressedWidgets()
{
    for (int32_t i = WIDX_TAB1; i <= WIDX_TAB4; i++)
    {
        SetWidgetPressed(i, false);
    }
}

void MultiplayerWindow::OnPrepareDraw()
{
    ResetPressedWidgets();
    SetWidgetPressed(WIDX_TAB1 + page, true);
    ResizeFrameWithPage();
    switch (page)
    {
        case WINDOW_MULTIPLAYER_PAGE_INFORMATION:
        {
            WindowAlignTabs(this, WIDX_TAB1, WIDX_TAB4);
            break;
        }
        case WINDOW_MULTIPLAYER_PAGE_PLAYERS:
        {
            window_multiplayer_players_widgets[WIDX_LIST].right = width - 4;
            window_multiplayer_players_widgets[WIDX_LIST].bottom = height - 0x0F;
            WindowAlignTabs(this, WIDX_TAB1, WIDX_TAB4);
            break;
        }
        case WINDOW_MULTIPLAYER_PAGE_GROUPS:
        {
            window_multiplayer_groups_widgets[WIDX_PERMISSIONS_LIST].right = width - 4;
            window_multiplayer_groups_widgets[WIDX_PERMISSIONS_LIST].bottom = height - 0x0F;
            WindowAlignTabs(this, WIDX_TAB1, WIDX_TAB4);

            // select other group if one is removed
            while (NetworkGetGroupIndex(_selectedGroup) == -1 && _selectedGroup > 0)
            {
                _selectedGroup--;
            }
            break;
        }
        case WINDOW_MULTIPLAYER_PAGE_OPTIONS:
        {
            WindowAlignTabs(this, WIDX_TAB1, WIDX_TAB4);

            if (NetworkGetMode() == NETWORK_MODE_CLIENT)
            {
                widgets[WIDX_KNOWN_KEYS_ONLY_CHECKBOX].type = WindowWidgetType::Empty;
            }

            SetCheckboxValue(WIDX_LOG_CHAT_CHECKBOX, gConfigNetwork.LogChat);
            SetCheckboxValue(WIDX_LOG_SERVER_ACTIONS_CHECKBOX, gConfigNetwork.LogServerActions);
            SetCheckboxValue(WIDX_KNOWN_KEYS_ONLY_CHECKBOX, gConfigNetwork.KnownKeysOnly);
            break;
        }
    }
}

void MultiplayerWindow::OnDraw(DrawPixelInfo& dpi)
{
    DrawWidgets(dpi);
    DrawTabImages(dpi);
    switch (page)
    {
        case WINDOW_MULTIPLAYER_PAGE_INFORMATION:
        {
            std::cout << "Here" << std::endl;
            InformationPaint(dpi);
            break;
        }
        case WINDOW_MULTIPLAYER_PAGE_PLAYERS:
        {
            std::cout << "THere" << std::endl;
            PlayersPaint(dpi);
            break;
        }
        case WINDOW_MULTIPLAYER_PAGE_GROUPS:
        {
            std::cout << "Somewhere" << std::endl;
            GroupsPaint(dpi);
            break;
        }
    }
    std::cout << "None?" << std::endl;
}

void MultiplayerWindow::OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex)
{
    switch (page)
    {
        case WINDOW_MULTIPLAYER_PAGE_GROUPS:
        {
            if (selectedIndex == -1)
            {
                return;
            }

            switch (widgetIndex)
            {
                case WIDX_DEFAULT_GROUP_DROPDOWN:
                {
                    auto networkModifyGroup = NetworkModifyGroupAction(
                        ModifyGroupType::SetDefault, NetworkGetGroupID(selectedIndex));
                    GameActions::Execute(&networkModifyGroup);
                    break;
                }
                case WIDX_SELECTED_GROUP_DROPDOWN:
                {
                    _selectedGroup = NetworkGetGroupID(selectedIndex);
                    break;
                }
            }
            Invalidate();
            break;
        }
    }
}

void MultiplayerWindow::OnTextInput(WidgetIndex widgetIndex, std::string_view text)
{
    switch (page)
    {
        case WINDOW_MULTIPLAYER_PAGE_GROUPS:
        {
            if (widgetIndex != WIDX_RENAME_GROUP)
                return;

            if (text.empty())
                return;

            auto networkModifyGroup = NetworkModifyGroupAction(ModifyGroupType::SetName, _selectedGroup, std::string(text));
            GameActions::Execute(&networkModifyGroup);
            break;
        }
    }
}

void MultiplayerWindow::ShowGroupDropdown(WidgetIndex widgetIndex)
{
    auto widget = &widgets[widgetIndex];
    Widget* dropdownWidget = widget - 1;
    auto numItems = NetworkGetNumGroups();

    WindowDropdownShowTextCustomWidth(
        windowPos + ScreenCoordsXY{ dropdownWidget->left, dropdownWidget->top }, dropdownWidget->height() + 1, colours[1], 0, 0,
        numItems, widget->right - dropdownWidget->left);

    for (auto i = 0; i < NetworkGetNumGroups(); i++)
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

void MultiplayerWindow::OnMouseDown(WidgetIndex widgetIndex)
{
    switch (page)
    {
        case WINDOW_MULTIPLAYER_PAGE_GROUPS:
        {
            switch (widgetIndex)
            {
                case WIDX_DEFAULT_GROUP_DROPDOWN:
                case WIDX_SELECTED_GROUP_DROPDOWN:
                    ShowGroupDropdown(widgetIndex);
                    break;
            }
            break;
        }
    }
}

ScreenSize MultiplayerWindow::OnScrollGetSize(int32_t scrollIndex)
{
    ScreenSize screenSize{};
    switch (page)
    {
        case WINDOW_MULTIPLAYER_PAGE_PLAYERS:
        {
            if (selected_list_item != -1)
            {
                selected_list_item = -1;
                Invalidate();
            }

            screenSize = { 0, NetworkGetNumPlayers() * SCROLLABLE_ROW_HEIGHT };
            int32_t i = screenSize.height - window_multiplayer_players_widgets[WIDX_LIST].bottom
                + window_multiplayer_players_widgets[WIDX_LIST].top + 21;
            if (i < 0)
                i = 0;
            if (i < scrolls[0].v_top)
            {
                scrolls[0].v_top = i;
                Invalidate();
            }
            break;
        }

        case WINDOW_MULTIPLAYER_PAGE_GROUPS:
        {
            if (selected_list_item != -1)
            {
                selected_list_item = -1;
                Invalidate();
            }

            screenSize = { 0, NetworkGetNumActions() * SCROLLABLE_ROW_HEIGHT };
            int32_t i = screenSize.height - window_multiplayer_groups_widgets[WIDX_LIST].bottom
                + window_multiplayer_groups_widgets[WIDX_LIST].top + 21;
            if (i < 0)
                i = 0;
            if (i < scrolls[0].v_top)
            {
                scrolls[0].v_top = i;
                Invalidate();
            }
            break;
        }
    }
    return screenSize;
}

void MultiplayerWindow::OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    switch (page)
    {
        case WINDOW_MULTIPLAYER_PAGE_PLAYERS:
        {
            int32_t index = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
            if (index >= no_list_items)
                return;

            selected_list_item = index;
            Invalidate();

            int32_t player = (IsServerPlayerInvisible() ? index + 1 : index);
            WindowPlayerOpen(NetworkGetPlayerID(player));
            break;
        }

        case WINDOW_MULTIPLAYER_PAGE_GROUPS:
        {
            int32_t index = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
            if (index >= no_list_items)
                return;

            selected_list_item = index;
            Invalidate();

            auto networkModifyGroup = NetworkModifyGroupAction(
                ModifyGroupType::SetPermissions, _selectedGroup, "", index, PermissionState::Toggle);
            GameActions::Execute(&networkModifyGroup);
            break;
        }
    }
}

void MultiplayerWindow::OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    switch (page)
    {
        case WINDOW_MULTIPLAYER_PAGE_PLAYERS:
        case WINDOW_MULTIPLAYER_PAGE_GROUPS:
        {
            int32_t index = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
            if (index >= no_list_items)
                return;

            selected_list_item = index;
            Invalidate();
            break;
        }
    }
}

void MultiplayerWindow::OnScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi)
{
    switch (page)
    {
        case WINDOW_MULTIPLAYER_PAGE_PLAYERS:
            PlayersScrollPaint(scrollIndex, dpi);
            break;

        case WINDOW_MULTIPLAYER_PAGE_GROUPS:
            GroupsScrollPaint(scrollIndex, dpi);
            break;
    }
}

void MultiplayerWindow::InformationPaint(DrawPixelInfo& dpi)
{
    DrawPixelInfo clippedDPI;
    if (ClipDrawPixelInfo(clippedDPI, dpi, windowPos, width, height))
    {
        auto screenCoords = ScreenCoordsXY{ 3, 50 };
        int32_t newWidth = width - 6;

        const auto& name = NetworkGetServerName();
        {
            auto ft = Formatter();
            ft.Add<const char*>(name.c_str());
            screenCoords.y += DrawTextWrapped(clippedDPI, screenCoords, newWidth, STR_STRING, ft, { colours[1] });
            screenCoords.y += LIST_ROW_HEIGHT / 2;
        }

        const auto& description = NetworkGetServerDescription();
        if (!description.empty())
        {
            auto ft = Formatter();
            ft.Add<const char*>(description.c_str());
            screenCoords.y += DrawTextWrapped(clippedDPI, screenCoords, newWidth, STR_STRING, ft, { colours[1] });
            screenCoords.y += LIST_ROW_HEIGHT / 2;
        }

        const auto& providerName = NetworkGetServerProviderName();
        if (!providerName.empty())
        {
            auto ft = Formatter();
            ft.Add<const char*>(providerName.c_str());
            DrawTextBasic(clippedDPI, screenCoords, STR_PROVIDER_NAME, ft);
            screenCoords.y += LIST_ROW_HEIGHT;
        }

        const auto& providerEmail = NetworkGetServerProviderEmail();
        if (!providerEmail.empty())
        {
            auto ft = Formatter();
            ft.Add<const char*>(providerEmail.c_str());
            DrawTextBasic(clippedDPI, screenCoords, STR_PROVIDER_EMAIL, ft);
            screenCoords.y += LIST_ROW_HEIGHT;
        }

        const auto& providerWebsite = NetworkGetServerProviderWebsite();
        if (!providerWebsite.empty())
        {
            auto ft = Formatter();
            ft.Add<const char*>(providerWebsite.c_str());
            DrawTextBasic(clippedDPI, screenCoords, STR_PROVIDER_WEBSITE, ft);
        }
    }
}

void MultiplayerWindow::PlayersPaint(DrawPixelInfo& dpi)
{
    // Number of players
    StringId stringId = no_list_items == 1 ? STR_MULTIPLAYER_PLAYER_COUNT : STR_MULTIPLAYER_PLAYER_COUNT_PLURAL;
    auto screenCoords = windowPos + ScreenCoordsXY{ 4, widgets[WIDX_LIST].bottom + 2 };
    auto ft = Formatter();
    ft.Add<uint16_t>(no_list_items);
    DrawTextBasic(dpi, screenCoords, stringId, ft, { colours[2] });
}

void MultiplayerWindow::PlayersScrollPaint(int32_t scrollIndex, DrawPixelInfo& dpi) const
{
    ScreenCoordsXY screenCoords;
    screenCoords.y = 0;

    const int32_t firstPlayerInList = (IsServerPlayerInvisible() ? 1 : 0);
    int32_t listPosition = 0;

    for (int32_t player = firstPlayerInList; player < NetworkGetNumPlayers(); player++)
    {
        if (screenCoords.y > dpi.y + dpi.height)
        {
            break;
        }

        if (screenCoords.y + SCROLLABLE_ROW_HEIGHT + 1 >= dpi.y)
        {
            thread_local std::string _buffer;
            _buffer.reserve(512);
            _buffer.clear();

            // Draw player name
            colour_t colour = COLOUR_BLACK;
            if (listPosition == selected_list_item)
            {
                GfxFilterRect(
                    dpi, { 0, screenCoords.y, 800, screenCoords.y + SCROLLABLE_ROW_HEIGHT - 1 },
                    FilterPaletteID::PaletteDarken1);
                _buffer += NetworkGetPlayerName(player);
                colour = colours[2];
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
            GfxDrawString(dpi, screenCoords, _buffer.c_str(), { colour });

            // Draw group name
            _buffer.resize(0);
            int32_t group = NetworkGetGroupIndex(NetworkGetPlayerGroup(player));
            if (group != -1)
            {
                _buffer += "{BLACK}";
                screenCoords.x = 173;
                _buffer += NetworkGetGroupName(group);
                GfxClipString(_buffer.data(), 80, FontStyle::Medium);
                GfxDrawString(dpi, screenCoords, _buffer.c_str(), { colour });
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
            DrawTextEllipsised(dpi, { 256, screenCoords.y }, 100, STR_BLACK_STRING, ft);

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
            GfxDrawString(dpi, screenCoords, _buffer.c_str(), { colour });
        }
        screenCoords.y += SCROLLABLE_ROW_HEIGHT;
        listPosition++;
    }
}

void MultiplayerWindow::GroupsPaint(DrawPixelInfo& dpi)
{
    thread_local std::string _buffer;

    Widget* widget = &window_multiplayer_groups_widgets[WIDX_DEFAULT_GROUP];
    int32_t group = NetworkGetGroupIndex(NetworkGetDefaultGroup());
    if (group != -1)
    {
        _buffer.assign("{WINDOW_COLOUR_2}");
        _buffer += NetworkGetGroupName(group);

        auto ft = Formatter();
        ft.Add<const char*>(_buffer.c_str());
        DrawTextEllipsised(
            dpi, windowPos + ScreenCoordsXY{ widget->midX() - 5, widget->top }, widget->width() - 8, STR_STRING, ft,
            { TextAlignment::CENTRE });
    }

    auto screenPos = windowPos
        + ScreenCoordsXY{ window_multiplayer_groups_widgets[WIDX_CONTENT_PANEL].left + 4,
                          window_multiplayer_groups_widgets[WIDX_CONTENT_PANEL].top + 4 };

    DrawTextBasic(dpi, screenPos, STR_DEFAULT_GROUP, {}, { colours[2] });

    screenPos.y += 20;

    GfxFillRectInset(
        dpi, { screenPos - ScreenCoordsXY{ 0, 6 }, screenPos + ScreenCoordsXY{ 310, -5 } }, colours[1],
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
            dpi, windowPos + ScreenCoordsXY{ widget->midX() - 5, widget->top }, widget->width() - 8, STR_STRING, ft,
            { TextAlignment::CENTRE });
    }
}

void MultiplayerWindow::GroupsScrollPaint(int32_t scrollIndex, DrawPixelInfo& dpi) const
{
    auto screenCoords = ScreenCoordsXY{ 0, 0 };

    auto dpiCoords = ScreenCoordsXY{ dpi.x, dpi.y };
    GfxFillRect(
        dpi, { dpiCoords, dpiCoords + ScreenCoordsXY{ dpi.width - 1, dpi.height - 1 } }, ColourMapA[colours[1]].mid_light);

    for (int32_t i = 0; i < NetworkGetNumActions(); i++)
    {
        if (i == selected_list_item)
        {
            GfxFilterRect(
                dpi, { 0, screenCoords.y, 800, screenCoords.y + SCROLLABLE_ROW_HEIGHT - 1 }, FilterPaletteID::PaletteDarken1);
        }
        if (screenCoords.y > dpi.y + dpi.height)
        {
            break;
        }

        if (screenCoords.y + SCROLLABLE_ROW_HEIGHT + 1 >= dpi.y)
        {
            int32_t groupindex = NetworkGetGroupIndex(_selectedGroup);
            if (groupindex != -1)
            {
                if (NetworkCanPerformAction(groupindex, static_cast<NetworkPermission>(i)))
                {
                    screenCoords.x = 0;
                    GfxDrawString(dpi, screenCoords, u8"{WINDOW_COLOUR_2}✓", {});
                }
            }

            // Draw action name
            auto ft = Formatter();
            ft.Add<uint16_t>(NetworkGetActionNameStringID(i));
            DrawTextBasic(dpi, { 10, screenCoords.y }, STR_WINDOW_COLOUR_2_STRINGID, ft);
        }
        screenCoords.y += SCROLLABLE_ROW_HEIGHT;
    }
}

void MultiplayerWindow::DrawTabImage(DrawPixelInfo& dpi, int32_t page_number, int32_t spriteIndex)
{
    WidgetIndex widgetIndex = WIDX_TAB1 + page_number;

    if (!IsWidgetDisabled(widgetIndex))
    {
        if (page == page_number)
        {
            int32_t numFrames = window_multiplayer_animation_frames[page];
            if (numFrames > 1)
            {
                int32_t frame = frame_no / window_multiplayer_animation_divisor[page];
                spriteIndex += (frame % numFrames);
            }
        }

        GfxDrawSprite(
            dpi, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widgets[widgetIndex].left, widgets[widgetIndex].top });
    }
}

void MultiplayerWindow::DrawTabImages(DrawPixelInfo& dpi)
{
    DrawTabImage(dpi, WINDOW_MULTIPLAYER_PAGE_INFORMATION, SPR_TAB_KIOSKS_AND_FACILITIES_0);
    DrawTabImage(dpi, WINDOW_MULTIPLAYER_PAGE_PLAYERS, SPR_TAB_GUESTS_0);
    DrawTabImage(dpi, WINDOW_MULTIPLAYER_PAGE_GROUPS, SPR_TAB_STAFF_OPTIONS_0);
    DrawTabImage(dpi, WINDOW_MULTIPLAYER_PAGE_OPTIONS, SPR_TAB_GEARS_0);
}
