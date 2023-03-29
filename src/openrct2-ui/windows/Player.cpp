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
#include <openrct2/Input.h>
#include <openrct2/actions/PlayerKickAction.h>
#include <openrct2/actions/PlayerSetGroupAction.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
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

// clang-format on

class PlayerWindow final : public Window
{
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
        picked_peep_frame = 0;
        min_width = 210;
        min_height = 134;
        max_width = 500;
        max_height = 450;
        no_list_items = 0;
        selected_list_item = -1;

        Invalidate();

        widgets = window_player_page_widgets[WINDOW_PLAYER_PAGE_OVERVIEW];
        hold_down_widgets = 0;
        pressed_widgets = 0;
    }

    void OnClose() override
    {
        switch (page)
        {
            case WINDOW_PLAYER_PAGE_STATISTICS:
                OnCloseStatistics();
                break;
        }
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

    void OnDraw(DrawPixelInfo& dpi) override
    {
        switch (page)
        {
            case WINDOW_PLAYER_PAGE_OVERVIEW:
                OnDrawOverview(&dpi);
                break;

            case WINDOW_PLAYER_PAGE_STATISTICS:
                OnDrawStatistics(&dpi);
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
        switch(widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
                SetPage(widgetIndex - WIDX_TAB_1);
                break;
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
        int32_t originalPage = page;

        page = newPage;
        frame_no = 0;
        no_list_items = 0;
        selected_list_item = -1;

        hold_down_widgets = 0;
        pressed_widgets = 0;
        widgets = window_player_page_widgets[newPage];
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

    void DrawTabImages(DrawPixelInfo* dpi)
    {
        Widget* widget;

        // Tab 1
        if (!IsWidgetDisabled(WIDX_TAB_1))
        {
            widget = &this->widgets[WIDX_TAB_1];
            auto screenCoords = windowPos + ScreenCoordsXY{ widget->left, widget->top };
            GfxDrawSprite(dpi, ImageId(SPR_PEEP_LARGE_FACE_NORMAL), screenCoords);
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

            GfxDrawSprite(dpi, ImageId(imageId), screenCoords);
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
                if (var_492 == -1)
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
                var_492 = 0;
            }
            else
            {
                // Don't draw the viewport
                var_492 = -1;
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
        WindowSetResize(*this, 240, 170, 500, 300);
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
        if (var_4AE != GetCurrentRotation())
        {
            var_4AE = GetCurrentRotation();
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

        if (window_player_page_widgets[page] != widgets)
        {
            widgets = window_player_page_widgets[page];
            InitScrollWidgets();
        }

        pressed_widgets &= ~(WIDX_TAB_1);
        pressed_widgets &= ~(WIDX_TAB_2);
        pressed_widgets |= 1uLL << (page + WIDX_TAB_1);

        UpdateTitle();

        ResizeFrameWithPage();
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
            Widget* viewportWidget = &window_player_overview_widgets[WIDX_VIEWPORT];

            viewport->pos = windowPos + ScreenCoordsXY{ viewportWidget->left, viewportWidget->top };
            viewport->width = viewportWidget->width();
            viewport->height = viewportWidget->height();
            viewport->view_width = viewport->zoom.ApplyTo(viewport->width);
            viewport->view_height = viewport->zoom.ApplyTo(viewport->height);
        }

        // Only enable kick button for other players
        const bool canKick = NetworkCanPerformAction(NetworkGetCurrentPlayerGroupIndex(), NetworkPermission::KickPlayer);
        const bool isServer = NetworkGetPlayerFlags(playerIndex) & NETWORK_PLAYER_FLAG_ISSERVER;
        const bool isOwnWindow = (NetworkGetCurrentPlayerId() == number);
        WidgetSetEnabled(*this, WIDX_KICK, canKick && !isOwnWindow && !isServer);
    }

    void OnDrawOverview(DrawPixelInfo* dpi)
    {
        DrawWidgets(*dpi);
        DrawTabImages(dpi);

        int32_t player = NetworkGetPlayerIndex(static_cast<uint8_t>(number));
        if (player == -1)
        {
            return;
        }

        // Draw current group
        int32_t groupindex = NetworkGetGroupIndex(NetworkGetPlayerGroup(player));
        if (groupindex != -1)
        {
            Widget* widget = &window_player_overview_widgets[WIDX_GROUP];

            thread_local std::string _buffer;
            _buffer.assign("{WINDOW_COLOUR_2}");
            _buffer += NetworkGetGroupName(groupindex);
            auto ft = Formatter();
            ft.Add<const char*>(_buffer.c_str());

            DrawTextEllipsised(
                *dpi, windowPos + ScreenCoordsXY{ widget->midX() - 5, widget->top }, widget->width() - 8, STR_STRING, ft,
                { TextAlignment::CENTRE });
        }

        // Draw ping
        auto screenCoords = windowPos + ScreenCoordsXY{ 90, 24 };

        auto ft = Formatter();
        ft.Add<StringId>(STR_PING);
        DrawTextBasic(*dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
        char ping[64];
        snprintf(ping, 64, "%d ms", NetworkGetPlayerPing(player));
        GfxDrawString(*dpi, screenCoords + ScreenCoordsXY(30, 0), ping, { colours[2] });

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
        DrawTextEllipsised(*dpi, screenCoords, updatedWidth, STR_LAST_ACTION_RAN, ft, { TextAlignment::CENTRE });

        if (viewport != nullptr && var_492 != -1)
        {
            WindowDrawViewport(dpi, *this);
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
                WindowInvalidateByNumber(windowHandle.first, windowHandle.second);
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
            { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1, colours[1],
            0, 0, numItems, widget->right - dropdownWidget->left);

        for (i = 0; i < NetworkGetNumGroups(); i++)
        {
            gDropdownItems[i].Format = STR_OPTIONS_DROPDOWN_ITEM;
            gDropdownItems[i].Args = reinterpret_cast<uintptr_t>(NetworkGetGroupName(i));
        }

        Dropdown::SetChecked(NetworkGetGroupIndex(NetworkGetPlayerGroup(player)), true);
    }

#pragma endregion

#pragma region Statistics

    void OnCloseStatistics()
    {
        if (error.var_480)
        {
            error.var_480 = 0;
        }
    }

    void OnResizeStatistics()
    {
        WindowSetResize(*this, 210, 80, 210, 80);
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
        if (window_player_page_widgets[page] != widgets)
        {
            widgets = window_player_page_widgets[page];
            InitScrollWidgets();
        }

        pressed_widgets &= ~(WIDX_TAB_1);
        pressed_widgets &= ~(WIDX_TAB_2);
        pressed_widgets |= 1uLL << (page + WIDX_TAB_1);

        UpdateTitle();

        ResizeFrameWithPage();

        WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_2);
    }

    void OnDrawStatistics(DrawPixelInfo* dpi)
    {
        DrawWidgets(*dpi);
        DrawTabImages(dpi);

        int32_t player = NetworkGetPlayerIndex(static_cast<uint8_t>(number));
        if (player == -1)
        {
            return;
        }

        auto screenCoords = windowPos
            + ScreenCoordsXY{ window_player_overview_widgets[WIDX_PAGE_BACKGROUND].left + 4,
                              window_player_overview_widgets[WIDX_PAGE_BACKGROUND].top + 4 };

        auto ft = Formatter();
        ft.Add<uint32_t>(NetworkGetPlayerCommandsRan(player));
        DrawTextBasic(*dpi, screenCoords, STR_COMMANDS_RAN, ft);

        screenCoords.y += LIST_ROW_HEIGHT;

        ft = Formatter();
        ft.Add<uint32_t>(NetworkGetPlayerMoneySpent(player));
        DrawTextBasic(*dpi, screenCoords, STR_MONEY_SPENT, ft);
    }

#pragma endregion
};

WindowBase* WindowPlayerOpen(uint8_t id)
{
    auto* window = static_cast<PlayerWindow*>(WindowBringToFrontByNumber(WindowClass::Player, id));
    if (window == nullptr)
    {
        window = WindowCreate<PlayerWindow>(WindowClass::Player, 240, 170, WF_RESIZABLE);
    }

    window->Init(id);

    return window;
}
