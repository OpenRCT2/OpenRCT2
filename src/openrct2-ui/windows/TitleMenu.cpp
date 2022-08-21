/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/scripting/CustomMenu.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Editor.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/ParkImporter.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/actions/LoadOrQuitAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/UiContext.h>

// clang-format off
enum {
    WIDX_START_NEW_GAME,
    WIDX_CONTINUE_SAVED_GAME,
    WIDX_MULTIPLAYER,
    WIDX_GAME_TOOLS,
    WIDX_NEW_VERSION,
};

enum
{
    DDIDX_SCENARIO_EDITOR,
    DDIDX_CONVERT_SAVED_GAME,
    DDIDX_TRACK_DESIGNER,
    DDIDX_TRACK_MANAGER,
    DDIDX_OPEN_CONTENT_FOLDER,
    DDIDX_CUSTOM_BEGIN = 6,
};

static ScreenRect _filterRect;
static constexpr ScreenSize MenuButtonDims = { 82, 82 };
static constexpr ScreenSize UpdateButtonDims = { MenuButtonDims.width * 4, 28 };

static rct_widget window_title_menu_widgets[] = {
    MakeWidget({0, UpdateButtonDims.height}, MenuButtonDims,   WindowWidgetType::ImgBtn, WindowColour::Tertiary,  SPR_MENU_NEW_GAME,       STR_START_NEW_GAME_TIP),
    MakeWidget({0, UpdateButtonDims.height}, MenuButtonDims,   WindowWidgetType::ImgBtn, WindowColour::Tertiary,  SPR_MENU_LOAD_GAME,      STR_CONTINUE_SAVED_GAME_TIP),
    MakeWidget({0, UpdateButtonDims.height}, MenuButtonDims,   WindowWidgetType::ImgBtn, WindowColour::Tertiary,  SPR_G2_MENU_MULTIPLAYER, STR_SHOW_MULTIPLAYER_TIP),
    MakeWidget({0, UpdateButtonDims.height}, MenuButtonDims,   WindowWidgetType::ImgBtn, WindowColour::Tertiary,  SPR_MENU_TOOLBOX,        STR_GAME_TOOLS_TIP),
    MakeWidget({0,                       0}, UpdateButtonDims, WindowWidgetType::Empty,  WindowColour::Secondary, STR_UPDATE_AVAILABLE),
    WIDGETS_END,
};

static void WindowTitleMenuMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowTitleMenuMousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void WindowTitleMenuDropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void WindowTitleMenuCursor(rct_window *w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords, CursorID *cursorId);
static void WindowTitleMenuInvalidate(rct_window *w);
static void WindowTitleMenuPaint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_title_menu_events([](auto& events)
{
    events.mouse_up = &WindowTitleMenuMouseup;
    events.mouse_down = &WindowTitleMenuMousedown;
    events.dropdown = &WindowTitleMenuDropdown;
    events.cursor = &WindowTitleMenuCursor;
    events.invalidate = &WindowTitleMenuInvalidate;
    events.paint = &WindowTitleMenuPaint;
});
// clang-format on

/**
 * Creates the window containing the menu buttons on the title screen.
 *  rct2: 0x0066B5C0 (part of 0x0066B3E8)
 */
rct_window* WindowTitleMenuOpen()
{
    rct_window* window;

    const uint16_t windowHeight = MenuButtonDims.height + UpdateButtonDims.height;
    window = WindowCreate(
        ScreenCoordsXY(0, context_get_height() - 182), 0, windowHeight, &window_title_menu_events, WindowClass::TitleMenu,
        WF_STICK_TO_BACK | WF_TRANSPARENT | WF_NO_BACKGROUND);

    window->widgets = window_title_menu_widgets;

#ifdef DISABLE_NETWORK
    window->widgets[WIDX_MULTIPLAYER].type = WindowWidgetType::Empty;
#endif

    rct_widgetindex i = 0;
    int32_t x = 0;
    for (rct_widget* widget = window->widgets; widget != &window->widgets[WIDX_NEW_VERSION]; widget++)
    {
        if (widget->type != WindowWidgetType::Empty)
        {
            widget->left = x;
            widget->right = x + MenuButtonDims.width - 1;

            x += MenuButtonDims.width;
        }
        i++;
    }
    window->width = x;
    window->widgets[WIDX_NEW_VERSION].right = window->width;
    window->windowPos.x = (context_get_width() - window->width) / 2;
    window->colours[1] = TRANSLUCENT(COLOUR_LIGHT_ORANGE);

    WindowInitScrollWidgets(*window);

    return window;
}

static void WindowTitleMenuScenarioselectCallback(const utf8* path)
{
    game_notify_map_change();
    OpenRCT2::GetContext()->LoadParkFromFile(path, false, true);
    game_load_scripts();
    game_notify_map_changed();
}

static void WindowTitleMenuMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    rct_window* windowToOpen = nullptr;

    switch (widgetIndex)
    {
        case WIDX_START_NEW_GAME:
            windowToOpen = window_find_by_class(WindowClass::ScenarioSelect);
            if (windowToOpen != nullptr)
            {
                window_bring_to_front(*windowToOpen);
            }
            else
            {
                window_close_by_class(WindowClass::Loadsave);
                window_close_by_class(WindowClass::ServerList);
                WindowScenarioselectOpen(WindowTitleMenuScenarioselectCallback, false);
            }
            break;
        case WIDX_CONTINUE_SAVED_GAME:
            windowToOpen = window_find_by_class(WindowClass::Loadsave);
            if (windowToOpen != nullptr)
            {
                window_bring_to_front(*windowToOpen);
            }
            else
            {
                window_close_by_class(WindowClass::ScenarioSelect);
                window_close_by_class(WindowClass::ServerList);
                auto loadOrQuitAction = LoadOrQuitAction(LoadOrQuitModes::OpenSavePrompt);
                GameActions::Execute(&loadOrQuitAction);
            }
            break;
        case WIDX_MULTIPLAYER:
            windowToOpen = window_find_by_class(WindowClass::ServerList);
            if (windowToOpen != nullptr)
            {
                window_bring_to_front(*windowToOpen);
            }
            else
            {
                window_close_by_class(WindowClass::ScenarioSelect);
                window_close_by_class(WindowClass::Loadsave);
                context_open_window(WindowClass::ServerList);
            }
            break;
        case WIDX_NEW_VERSION:
            context_open_window_view(WV_NEW_VERSION_INFO);
            break;
    }
}

static void WindowTitleMenuMousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    if (widgetIndex == WIDX_GAME_TOOLS)
    {
        int32_t i = 0;
        gDropdownItems[i++].Format = STR_SCENARIO_EDITOR;
        gDropdownItems[i++].Format = STR_CONVERT_SAVED_GAME_TO_SCENARIO;
        gDropdownItems[i++].Format = STR_ROLLER_COASTER_DESIGNER;
        gDropdownItems[i++].Format = STR_TRACK_DESIGNS_MANAGER;
        gDropdownItems[i++].Format = STR_OPEN_USER_CONTENT_FOLDER;

#ifdef ENABLE_SCRIPTING
        auto hasCustomItems = false;
        const auto& customMenuItems = OpenRCT2::Scripting::CustomMenuItems;
        if (!customMenuItems.empty())
        {
            for (const auto& item : customMenuItems)
            {
                if (item.Kind == OpenRCT2::Scripting::CustomToolbarMenuItemKind::Toolbox)
                {
                    // Add seperator
                    if (!hasCustomItems)
                    {
                        hasCustomItems = true;
                        gDropdownItems[i++].Format = STR_EMPTY;
                    }

                    gDropdownItems[i].Format = STR_STRING;
                    auto sz = item.Text.c_str();
                    std::memcpy(&gDropdownItems[i].Args, &sz, sizeof(const char*));
                    i++;
                }
            }
        }
#endif

        int32_t yOffset = 0;
        if (i > 5)
        {
            yOffset = -(widget->height() + 5 + (i * 12));
        }

        WindowDropdownShowText(
            { w->windowPos.x + widget->left, w->windowPos.y + widget->top + yOffset }, widget->height() + 1,
            TRANSLUCENT(w->colours[0]), Dropdown::Flag::StayOpen, i);
    }
}

static void InvokeCustomToolboxMenuItem(size_t index)
{
#ifdef ENABLE_SCRIPTING
    const auto& customMenuItems = OpenRCT2::Scripting::CustomMenuItems;
    size_t i = 0;
    for (const auto& item : customMenuItems)
    {
        if (item.Kind == OpenRCT2::Scripting::CustomToolbarMenuItemKind::Toolbox)
        {
            if (i == index)
            {
                item.Invoke();
                break;
            }
            i++;
        }
    }
#endif
}

static void WindowTitleMenuDropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (widgetIndex == WIDX_GAME_TOOLS)
    {
        switch (dropdownIndex)
        {
            case DDIDX_SCENARIO_EDITOR:
                Editor::Load();
                break;
            case DDIDX_CONVERT_SAVED_GAME:
                Editor::ConvertSaveToScenario();
                break;
            case DDIDX_TRACK_DESIGNER:
                Editor::LoadTrackDesigner();
                break;
            case DDIDX_TRACK_MANAGER:
                Editor::LoadTrackManager();
                break;
            case DDIDX_OPEN_CONTENT_FOLDER:
            {
                auto context = OpenRCT2::GetContext();
                auto env = context->GetPlatformEnvironment();
                auto uiContext = context->GetUiContext();
                uiContext->OpenFolder(env->GetDirectoryPath(OpenRCT2::DIRBASE::USER));
                break;
            }
            default:
                InvokeCustomToolboxMenuItem(dropdownIndex - DDIDX_CUSTOM_BEGIN);
                break;
        }
    }
}

static void WindowTitleMenuCursor(
    rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords, CursorID* cursorId)
{
    gTooltipTimeout = 2000;
}

static void WindowTitleMenuInvalidate(rct_window* w)
{
    _filterRect = { w->windowPos.x, w->windowPos.y + UpdateButtonDims.height, w->windowPos.x + w->width - 1,
                    w->windowPos.y + MenuButtonDims.height + UpdateButtonDims.height - 1 };
    if (OpenRCT2::GetContext()->HasNewVersionInfo())
    {
        w->widgets[WIDX_NEW_VERSION].type = WindowWidgetType::Button;
        _filterRect.Point1.y = w->windowPos.y;
    }
}

static void WindowTitleMenuPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    gfx_filter_rect(dpi, _filterRect, FilterPaletteID::Palette51);
    WindowDrawWidgets(*w, dpi);
}
