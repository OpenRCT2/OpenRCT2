/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/scripting/CustomMenu.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Editor.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/ParkImporter.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/actions/LoadOrQuitAction.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Ui::Windows
{
    enum
    {
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

    static constexpr ScreenSize MenuButtonDims = { 82, 82 };
    static constexpr ScreenSize UpdateButtonDims = { MenuButtonDims.width * 4, 28 };

    // clang-format off
    static constexpr Widget _titleMenuWidgets[] = {
        MakeWidget({0, UpdateButtonDims.height}, MenuButtonDims,   WindowWidgetType::ImgBtn, WindowColour::Tertiary,  ImageId(SPR_MENU_NEW_GAME),       STR_START_NEW_GAME_TIP),
        MakeWidget({0, UpdateButtonDims.height}, MenuButtonDims,   WindowWidgetType::ImgBtn, WindowColour::Tertiary,  ImageId(SPR_MENU_LOAD_GAME),      STR_CONTINUE_SAVED_GAME_TIP),
        MakeWidget({0, UpdateButtonDims.height}, MenuButtonDims,   WindowWidgetType::ImgBtn, WindowColour::Tertiary,  ImageId(SPR_G2_MENU_MULTIPLAYER), STR_SHOW_MULTIPLAYER_TIP),
        MakeWidget({0, UpdateButtonDims.height}, MenuButtonDims,   WindowWidgetType::ImgBtn, WindowColour::Tertiary,  ImageId(SPR_MENU_TOOLBOX),        STR_GAME_TOOLS_TIP),
        MakeWidget({0,                       0}, UpdateButtonDims, WindowWidgetType::Empty,  WindowColour::Secondary, STR_UPDATE_AVAILABLE),
    };
    // clang-format on

    static void WindowTitleMenuScenarioselectCallback(const utf8* path)
    {
        GameNotifyMapChange();
        OpenRCT2::GetContext()->LoadParkFromFile(path, false, true);
        GameLoadScripts();
        GameNotifyMapChanged();
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

    class TitleMenuWindow final : public Window
    {
    private:
        ScreenRect _filterRect;

    public:
        void OnOpen() override
        {
            SetWidgets(_titleMenuWidgets);

#ifdef DISABLE_NETWORK
            widgets[WIDX_MULTIPLAYER].type = WindowWidgetType::Empty;
#endif

            int32_t x = 0;
            for (Widget* widget = widgets.data(); widget != &widgets[WIDX_NEW_VERSION]; widget++)
            {
                if (widget->type != WindowWidgetType::Empty)
                {
                    widget->left = x;
                    widget->right = x + MenuButtonDims.width - 1;

                    x += MenuButtonDims.width;
                }
            }
            width = x;
            widgets[WIDX_NEW_VERSION].right = width;
            windowPos.x = (ContextGetWidth() - width) / 2;
            colours[1] = ColourWithFlags{ COLOUR_LIGHT_ORANGE }.withFlag(ColourFlag::translucent, true);

            InitScrollWidgets();
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            WindowBase* windowToOpen = nullptr;

            auto* windowMgr = GetWindowManager();

            switch (widgetIndex)
            {
                case WIDX_START_NEW_GAME:
                    windowToOpen = windowMgr->FindByClass(WindowClass::ScenarioSelect);
                    if (windowToOpen != nullptr)
                    {
                        windowMgr->BringToFront(*windowToOpen);
                    }
                    else
                    {
                        windowMgr->CloseByClass(WindowClass::Loadsave);
                        windowMgr->CloseByClass(WindowClass::ServerList);
                        ScenarioselectOpen(WindowTitleMenuScenarioselectCallback);
                    }
                    break;
                case WIDX_CONTINUE_SAVED_GAME:
                    windowToOpen = windowMgr->FindByClass(WindowClass::Loadsave);
                    if (windowToOpen != nullptr)
                    {
                        windowMgr->BringToFront(*windowToOpen);
                    }
                    else
                    {
                        windowMgr->CloseByClass(WindowClass::ScenarioSelect);
                        windowMgr->CloseByClass(WindowClass::ServerList);
                        auto loadOrQuitAction = LoadOrQuitAction(LoadOrQuitModes::OpenSavePrompt);
                        GameActions::Execute(&loadOrQuitAction);
                    }
                    break;
                case WIDX_MULTIPLAYER:
                    windowToOpen = windowMgr->FindByClass(WindowClass::ServerList);
                    if (windowToOpen != nullptr)
                    {
                        windowMgr->BringToFront(*windowToOpen);
                    }
                    else
                    {
                        windowMgr->CloseByClass(WindowClass::ScenarioSelect);
                        windowMgr->CloseByClass(WindowClass::Loadsave);
                        ContextOpenWindow(WindowClass::ServerList);
                    }
                    break;
                case WIDX_NEW_VERSION:
                    ContextOpenWindowView(WV_NEW_VERSION_INFO);
                    break;
            }
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
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
                                gDropdownItems[i++].Format = kStringIdEmpty;
                            }

                            gDropdownItems[i].Format = STR_STRING;
                            auto sz = item.Text.c_str();
                            std::memcpy(&gDropdownItems[i].Args, &sz, sizeof(const char*));
                            i++;
                        }
                    }
                }
#endif

                Widget* widget = &widgets[widgetIndex];
                int32_t yOffset = 0;
                if (i > 5)
                {
                    yOffset = -(widget->height() + 5 + (i * 12));
                }

                WindowDropdownShowText(
                    windowPos + ScreenCoordsXY{ widget->left, widget->top + yOffset }, widget->height() + 1,
                    colours[0].withFlag(ColourFlag::translucent, true), Dropdown::Flag::StayOpen, i);
            }
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            if (selectedIndex == -1)
            {
                return;
            }
            if (widgetIndex == WIDX_GAME_TOOLS)
            {
                switch (selectedIndex)
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
                        InvokeCustomToolboxMenuItem(selectedIndex - DDIDX_CUSTOM_BEGIN);
                        break;
                }
            }
        }

        CursorID OnCursor(WidgetIndex, const ScreenCoordsXY&, CursorID cursorId) override
        {
            gTooltipCloseTimeout = gCurrentRealTimeTicks + 2000;
            return cursorId;
        }

        void OnPrepareDraw() override
        {
            _filterRect = { windowPos + ScreenCoordsXY{ 0, UpdateButtonDims.height },
                            windowPos + ScreenCoordsXY{ width - 1, MenuButtonDims.height + UpdateButtonDims.height - 1 } };
            if (OpenRCT2::GetContext()->HasNewVersionInfo())
            {
                widgets[WIDX_NEW_VERSION].type = WindowWidgetType::Button;
                _filterRect.Point1.y = windowPos.y;
            }
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            GfxFilterRect(dpi, _filterRect, FilterPaletteID::Palette51);
            DrawWidgets(dpi);
        }
    };

    /**
     * Creates the window containing the menu buttons on the title screen.
     */
    WindowBase* TitleMenuOpen()
    {
        const uint16_t windowHeight = MenuButtonDims.height + UpdateButtonDims.height;

        auto* windowMgr = GetWindowManager();
        return windowMgr->Create<TitleMenuWindow>(
            WindowClass::TitleMenu, ScreenCoordsXY(0, ContextGetHeight() - 182), 0, windowHeight,
            WF_STICK_TO_BACK | WF_TRANSPARENT | WF_NO_BACKGROUND);
    }
} // namespace OpenRCT2::Ui::Windows
