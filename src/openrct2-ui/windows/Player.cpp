/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Viewport.h"

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Input.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/PlayerKickAction.h>
#include <openrct2/actions/PlayerSetGroupAction.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/network/Network.h>
#include <openrct2/network/NetworkAction.h>
#include <openrct2/ui/WindowManager.h>
#include <utility>

namespace OpenRCT2::Ui::Windows
{
    enum WindowPlayerPage
    {
        WINDOW_PLAYER_PAGE_OVERVIEW,
        WINDOW_PLAYER_PAGE_STATISTICS,
    };

#pragma region Widgets

    enum WindowPlayerWidgetIdx
    {
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

    static constexpr ScreenSize kWindowSize = { 192, 157 };
    // clang-format off
    
    static constexpr auto kCommonPlayerWidgets = makeWidgets(
        makeWindowShim(STR_STRING, kWindowSize),
        makeWidget({ 0, 43}, {192, 114}, WidgetType::resize, WindowColour::secondary),
        makeTab   ({ 3, 17}                                                         ),
        makeTab   ({34, 17}                                                         )
    );

    static constexpr auto window_player_overview_widgets = makeWidgets(
        kCommonPlayerWidgets,
        makeWidget({  3, 46}, {175, 12}, WidgetType::dropdownMenu, WindowColour::secondary                                              ), // Permission group
        makeWidget({167, 47}, { 11, 10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH                          ),
        makeWidget({179, 45}, { 12, 24}, WidgetType::flatBtn,      WindowColour::secondary, ImageId(SPR_LOCATE),   STR_LOCATE_PLAYER_TIP), // Locate button
        makeWidget({179, 69}, { 12, 24}, WidgetType::flatBtn,      WindowColour::secondary, ImageId(SPR_DEMOLISH), STR_KICK_PLAYER_TIP  ), // Kick button
        makeWidget({  3, 60}, {175, 61}, WidgetType::viewport,     WindowColour::secondary                                              )  // Viewport
    );

    static constexpr auto window_player_statistics_widgets = makeWidgets(
        kCommonPlayerWidgets
    );

    static constexpr std::span<const Widget> window_player_page_widgets[] = {
        window_player_overview_widgets,
        window_player_statistics_widgets,
    };
    // clang-format on

#pragma endregion

    class PlayerWindow final : public Window
    {
        int16_t _previousRotation = -1;
        bool _drawViewport = true;

    public:
        void Init(const uint8_t id)
        {
            number = id;
            InitScrollWidgets();
            SetPage(WINDOW_PLAYER_PAGE_OVERVIEW);
        }

#pragma region Events

        void OnOpen() override
        {
            page = 0;
            frame_no = 0;
            list_information_type = 0;

            WindowSetResize(*this, { 210, 134 }, { 500, 450 });

            hold_down_widgets = 0;
            pressed_widgets = 0;
            SetPage(WINDOW_PLAYER_PAGE_OVERVIEW);
        }

        void OnResize() override
        {
            switch (page)
            {
                case WINDOW_PLAYER_PAGE_OVERVIEW:
                    OnResizeOverview();
                    break;

                case WINDOW_PLAYER_PAGE_STATISTICS:
                    OnResizeStatistics();
                    break;
            }
        }

        void OnUpdate() override
        {
            switch (page)
            {
                case WINDOW_PLAYER_PAGE_OVERVIEW:
                    OnUpdateOverview();
                    break;

                case WINDOW_PLAYER_PAGE_STATISTICS:
                    OnUpdateStatistics();
                    break;
            }
        }

        void OnPrepareDraw() override
        {
            switch (page)
            {
                case WINDOW_PLAYER_PAGE_OVERVIEW:
                    OnPrepareDrawOverview();
                    break;

                case WINDOW_PLAYER_PAGE_STATISTICS:
                    OnPrepareDrawStatistics();
                    break;
            }
        }

        void OnDraw(RenderTarget& rt) override
        {
            switch (page)
            {
                case WINDOW_PLAYER_PAGE_OVERVIEW:
                    OnDrawOverview(rt);
                    break;

                case WINDOW_PLAYER_PAGE_STATISTICS:
                    OnDrawStatistics(rt);
                    break;
            }
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
        {
            switch (page)
            {
                case WINDOW_PLAYER_PAGE_OVERVIEW:
                    OnMouseDownOverview(widgetIndex);
                    break;
            }
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    Close();
                    return;
                case WIDX_TAB_1:
                case WIDX_TAB_2:
                    SetPage(widgetIndex - WIDX_TAB_1);
                    return;
            }

            switch (page)
            {
                case WINDOW_PLAYER_PAGE_OVERVIEW:
                    OnMouseUpOverview(widgetIndex);
                    break;
            }
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            switch (page)
            {
                case WINDOW_PLAYER_PAGE_OVERVIEW:
                    OnDropdownOverview(widgetIndex, selectedIndex);
                    break;
            }
        }

#pragma endregion

    private:
        void SetPage(int32_t newPage)
        {
            // Skip setting page if we're already on this page, unless we're initialising the window
            if (page == newPage && !widgets.empty())
                return;

            int32_t originalPage = page;

            page = newPage;
            frame_no = 0;

            hold_down_widgets = 0;
            pressed_widgets = 0;
            SetWidgets(window_player_page_widgets[newPage]);
            Invalidate();
            OnResize();
            OnPrepareDraw();
            InitScrollWidgets();
            Invalidate();

            if (page == WINDOW_PLAYER_PAGE_OVERVIEW)
            {
                if (viewport == nullptr)
                {
                    const auto viewportFocus = Focus(TileCoordsXYZ(128, 128, 0).ToCoordsXYZ());
                    ViewportCreate(this, windowPos, width, height, viewportFocus);
                    flags |= WF_NO_SCROLLING;
                    OnPrepareDraw();
                    UpdateViewport(false);
                }
                else if (originalPage != page)
                {
                    OnPrepareDraw();
                    UpdateViewport(false);
                }
            }
            else
            {
                RemoveViewport();
            }
        }

        void DrawTabImages(RenderTarget& rt)
        {
            Widget* widget;

            // Tab 1
            if (!IsWidgetDisabled(WIDX_TAB_1))
            {
                widget = &this->widgets[WIDX_TAB_1];
                auto screenCoords = windowPos + ScreenCoordsXY{ widget->left, widget->top };
                GfxDrawSprite(rt, ImageId(SPR_PEEP_LARGE_FACE_NORMAL), screenCoords);
            }

            // Tab 2
            if (!IsWidgetDisabled(WIDX_TAB_2))
            {
                widget = &this->widgets[WIDX_TAB_2];
                auto screenCoords = windowPos + ScreenCoordsXY{ widget->left, widget->top };
                int32_t imageId = SPR_TAB_FINANCES_SUMMARY_0;

                if (page == WINDOW_PLAYER_PAGE_STATISTICS)
                {
                    imageId += (frame_no / 2) & 7;
                }

                GfxDrawSprite(rt, ImageId(imageId), screenCoords);
            }
        }

        void UpdateViewport(bool scroll)
        {
            int32_t playerIndex = NetworkGetPlayerIndex(static_cast<uint8_t>(number));
            if (playerIndex == -1)
            {
                return;
            }

            if (viewport != nullptr)
            {
                auto coord = NetworkGetPlayerLastActionCoord(playerIndex);
                if (coord.x != 0 || coord.y != 0 || coord.z != 0)
                {
                    auto centreLoc = centre_2d_coordinates(coord, viewport);
                    if (!centreLoc.has_value())
                    {
                        return;
                    }
                    // Don't scroll if the view was originally undefined
                    if (!_drawViewport)
                    {
                        scroll = false;
                    }

                    if (!scroll || savedViewPos != centreLoc.value())
                    {
                        flags |= WF_SCROLLING_TO_LOCATION;
                        savedViewPos = centreLoc.value();
                        if (!scroll)
                        {
                            viewport->viewPos = centreLoc.value();
                        }
                        InvalidateWidget(WIDX_VIEWPORT);
                    }

                    // Draw the viewport
                    _drawViewport = true;
                }
                else
                {
                    // Don't draw the viewport
                    _drawViewport = false;
                }
            }
        }

        void UpdateTitle()
        {
            auto ft = Formatter::Common();
            int32_t player = NetworkGetPlayerIndex(static_cast<uint8_t>(number));
            if (player != -1)
            {
                ft.Add<const char*>(NetworkGetPlayerName(player)); // set title caption to player name
            }
            else
            {
                ft.Add<const char*>("");
            }
        }

#pragma region Overview

        void OnResizeOverview()
        {
            WindowSetResize(*this, { 240, 170 }, { 500, 300 });
        }

        void OnUpdateOverview()
        {
            frame_no++;
            InvalidateWidget(WIDX_TAB_1 + page);

            if (NetworkGetPlayerIndex(static_cast<uint8_t>(number)) == -1)
            {
                Close();
                return;
            }

            // Update viewport
            bool scroll = true;

            // Use this spare window field for rotation check
            if (_previousRotation != GetCurrentRotation())
            {
                _previousRotation = GetCurrentRotation();
                scroll = false;
            }
            UpdateViewport(scroll);
        }

        void OnPrepareDrawOverview()
        {
            int32_t playerIndex = NetworkGetPlayerIndex(static_cast<uint8_t>(number));
            if (playerIndex == -1)
            {
                return;
            }

            pressed_widgets &= ~(WIDX_TAB_1);
            pressed_widgets &= ~(WIDX_TAB_2);
            pressed_widgets |= 1uLL << (page + WIDX_TAB_1);

            UpdateTitle();

            widgets[WIDX_LOCATE].right = width - 2;
            widgets[WIDX_LOCATE].left = width - 25;
            widgets[WIDX_KICK].right = width - 2;
            widgets[WIDX_KICK].left = width - 25;
            widgets[WIDX_VIEWPORT].right = width - 26;
            widgets[WIDX_VIEWPORT].bottom = height - 14;

            int32_t groupDropdownWidth = widgets[WIDX_GROUP].width();
            widgets[WIDX_GROUP].left = (width - groupDropdownWidth) / 2;
            widgets[WIDX_GROUP].right = widgets[WIDX_GROUP].left + groupDropdownWidth;
            widgets[WIDX_GROUP_DROPDOWN].left = widgets[WIDX_GROUP].right - 10;
            widgets[WIDX_GROUP_DROPDOWN].right = widgets[WIDX_GROUP].right;

            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_2);

            if (viewport != nullptr)
            {
                Widget* viewportWidget = &widgets[WIDX_VIEWPORT];

                viewport->pos = windowPos + ScreenCoordsXY{ viewportWidget->left, viewportWidget->top };
                viewport->width = viewportWidget->width();
                viewport->height = viewportWidget->height();
            }

            // Only enable kick button for other players
            const bool canKick = NetworkCanPerformAction(NetworkGetCurrentPlayerGroupIndex(), NetworkPermission::KickPlayer);
            const bool isServer = NetworkGetPlayerFlags(playerIndex) & NETWORK_PLAYER_FLAG_ISSERVER;
            const bool isOwnWindow = (NetworkGetCurrentPlayerId() == number);
            widgetSetEnabled(*this, WIDX_KICK, canKick && !isOwnWindow && !isServer);
        }

        void OnDrawOverview(RenderTarget& rt)
        {
            DrawWidgets(rt);
            DrawTabImages(rt);

            int32_t player = NetworkGetPlayerIndex(static_cast<uint8_t>(number));
            if (player == -1)
            {
                return;
            }

            // Draw current group
            int32_t groupindex = NetworkGetGroupIndex(NetworkGetPlayerGroup(player));
            if (groupindex != -1)
            {
                Widget* widget = &widgets[WIDX_GROUP];

                thread_local std::string _buffer;
                _buffer.assign("{WINDOW_COLOUR_2}");
                _buffer += NetworkGetGroupName(groupindex);
                auto ft = Formatter();
                ft.Add<const char*>(_buffer.c_str());

                DrawTextEllipsised(
                    rt, windowPos + ScreenCoordsXY{ widget->midX() - 5, widget->top }, widget->width() - 8, STR_STRING, ft,
                    { TextAlignment::CENTRE });
            }

            // Draw ping
            auto screenCoords = windowPos + ScreenCoordsXY{ 90, widgets[WIDX_TAB_1].midY() };

            auto ft = Formatter();
            ft.Add<StringId>(STR_PING);
            DrawTextBasic(rt, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
            char ping[64];
            snprintf(ping, 64, "%d ms", NetworkGetPlayerPing(player));
            DrawText(rt, screenCoords + ScreenCoordsXY(30, 0), { colours[2] }, ping);

            // Draw last action
            screenCoords = windowPos + ScreenCoordsXY{ width / 2, height - 13 };
            int32_t updatedWidth = this->width - 8;
            int32_t lastaction = NetworkGetPlayerLastAction(player, 0);
            ft = Formatter();
            if (lastaction != -999)
            {
                ft.Add<StringId>(NetworkGetActionNameStringID(lastaction));
            }
            else
            {
                ft.Add<StringId>(STR_ACTION_NA);
            }
            DrawTextEllipsised(rt, screenCoords, updatedWidth, STR_LAST_ACTION_RAN, ft, { TextAlignment::CENTRE });

            if (viewport != nullptr && _drawViewport)
            {
                WindowDrawViewport(rt, *this);
            }
        }

        void OnMouseDownOverview(WidgetIndex widgetIndex)
        {
            auto* widget = &widgets[widgetIndex];
            switch (widgetIndex)
            {
                case WIDX_GROUP_DROPDOWN:
                    ShowGroupDropdownOverview(widget);
                    break;
            }
        }

        void OnMouseUpOverview(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_LOCATE:
                {
                    WindowBase* mainWindow = WindowGetMain();
                    if (mainWindow != nullptr)
                    {
                        int32_t player = NetworkGetPlayerIndex(static_cast<uint8_t>(number));
                        if (player == -1)
                        {
                            return;
                        }
                        auto coord = NetworkGetPlayerLastActionCoord(player);
                        if (coord.x || coord.y || coord.z)
                        {
                            WindowScrollToLocation(*mainWindow, coord);
                        }
                    }
                }
                break;
                case WIDX_KICK:
                {
                    auto kickPlayerAction = PlayerKickAction(number);
                    GameActions::Execute(&kickPlayerAction);
                }
                break;
            }
        }

        void OnDropdownOverview(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            const auto playerId = static_cast<uint8_t>(number);
            const auto playerIdx = NetworkGetPlayerIndex(playerId);
            if (playerIdx == -1)
            {
                return;
            }
            if (dropdownIndex == -1)
            {
                return;
            }
            const auto groupId = NetworkGetGroupID(dropdownIndex);
            const auto windowHandle = std::make_pair(classification, number);
            auto playerSetGroupAction = PlayerSetGroupAction(playerId, groupId);
            playerSetGroupAction.SetCallback([windowHandle](const GameAction* ga, const GameActions::Result* result) {
                if (result->Error == GameActions::Status::Ok)
                {
                    auto* windowMgr = Ui::GetWindowManager();
                    windowMgr->InvalidateByNumber(windowHandle.first, windowHandle.second);
                }
            });
            GameActions::Execute(&playerSetGroupAction);
        }

        void ShowGroupDropdownOverview(Widget* widget)
        {
            Widget* dropdownWidget;
            int32_t numItems, i;
            int32_t player = NetworkGetPlayerIndex(static_cast<uint8_t>(number));
            if (player == -1)
            {
                return;
            }

            dropdownWidget = widget - 1;

            numItems = NetworkGetNumGroups();

            WindowDropdownShowTextCustomWidth(
                { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
                colours[1], 0, 0, numItems, widget->right - dropdownWidget->left);

            for (i = 0; i < NetworkGetNumGroups(); i++)
            {
                gDropdownItems[i].Format = STR_OPTIONS_DROPDOWN_ITEM;
                gDropdownItems[i].Args = reinterpret_cast<uintptr_t>(NetworkGetGroupName(i));
            }

            Dropdown::SetChecked(NetworkGetGroupIndex(NetworkGetPlayerGroup(player)), true);
        }

#pragma endregion

#pragma region Statistics

        void OnResizeStatistics()
        {
            WindowSetResize(*this, { 210, 80 }, { 210, 80 });
        }

        void OnUpdateStatistics()
        {
            frame_no++;
            InvalidateWidget(WIDX_TAB_1 + page);

            if (NetworkGetPlayerIndex(static_cast<uint8_t>(number)) == -1)
            {
                Close();
            }
        }

        void OnPrepareDrawStatistics()
        {
            pressed_widgets &= ~(WIDX_TAB_1);
            pressed_widgets &= ~(WIDX_TAB_2);
            pressed_widgets |= 1uLL << (page + WIDX_TAB_1);

            UpdateTitle();

            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_2);
        }

        void OnDrawStatistics(RenderTarget& rt)
        {
            DrawWidgets(rt);
            DrawTabImages(rt);

            int32_t player = NetworkGetPlayerIndex(static_cast<uint8_t>(number));
            if (player == -1)
            {
                return;
            }

            auto screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_PAGE_BACKGROUND].left + 4, widgets[WIDX_PAGE_BACKGROUND].top + 4 };

            auto ft = Formatter();
            ft.Add<uint32_t>(NetworkGetPlayerCommandsRan(player));
            DrawTextBasic(rt, screenCoords, STR_COMMANDS_RAN, ft);

            screenCoords.y += kListRowHeight;

            ft = Formatter();
            ft.Add<uint32_t>(NetworkGetPlayerMoneySpent(player));
            DrawTextBasic(rt, screenCoords, STR_MONEY_SPENT, ft);
        }

#pragma endregion
    };

    WindowBase* PlayerOpen(uint8_t id)
    {
        auto* windowMgr = GetWindowManager();
        auto* window = static_cast<PlayerWindow*>(windowMgr->BringToFrontByNumber(WindowClass::Player, id));
        if (window == nullptr)
        {
            window = windowMgr->Create<PlayerWindow>(WindowClass::Player, { 240, 170 }, WF_RESIZABLE);
        }

        window->Init(id);

        return window;
    }
} // namespace OpenRCT2::Ui::Windows
