/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/ParkImporter.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/LoadOrQuitAction.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/interface/ColourWithFlags.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Ui::Windows
{
    using namespace OpenRCT2::Drawing;

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
    static constexpr auto _titleMenuWidgets = makeWidgets(
        makeWidget({0, UpdateButtonDims.height}, MenuButtonDims,   WidgetType::imgBtn, WindowColour::tertiary,  ImageId(SPR_MENU_NEW_GAME),       STR_START_NEW_GAME_TIP),
        makeWidget({0, UpdateButtonDims.height}, MenuButtonDims,   WidgetType::imgBtn, WindowColour::tertiary,  ImageId(SPR_MENU_LOAD_GAME),      STR_CONTINUE_SAVED_GAME_TIP),
        makeWidget({0, UpdateButtonDims.height}, MenuButtonDims,   WidgetType::imgBtn, WindowColour::tertiary,  ImageId(SPR_G2_MENU_MULTIPLAYER), STR_SHOW_MULTIPLAYER_TIP),
        makeWidget({0, UpdateButtonDims.height}, MenuButtonDims,   WidgetType::imgBtn, WindowColour::tertiary,  ImageId(SPR_MENU_TOOLBOX),        STR_GAME_TOOLS_TIP),
        makeWidget({0,                       0}, UpdateButtonDims, WidgetType::empty,  WindowColour::secondary, STR_UPDATE_AVAILABLE)
    );
    // clang-format on

    static void WindowTitleMenuScenarioselectCallback(const utf8* path)
    {
        GameNotifyMapChange();
        GetContext()->LoadParkFromFile(path, false, true);
        GameLoadScripts();
        GameNotifyMapChanged();
    }

    static void InvokeCustomToolboxMenuItem(size_t index)
    {
#ifdef ENABLE_SCRIPTING
        const auto& customMenuItems = Scripting::CustomMenuItems;
        size_t i = 0;
        for (const auto& item : customMenuItems)
        {
            if (item.Kind == Scripting::CustomToolbarMenuItemKind::Toolbox)
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
        void onOpen() override
        {
            setWidgets(_titleMenuWidgets);

#ifdef DISABLE_NETWORK
            widgets[WIDX_MULTIPLAYER].type = WidgetType::empty;
#endif

            int32_t x = 0;
            for (Widget* widget = widgets.data(); widget != &widgets[WIDX_NEW_VERSION]; widget++)
            {
                if (widget->type != WidgetType::empty)
                {
                    widget->left = x;
                    widget->right = x + MenuButtonDims.width - 1;

                    x += MenuButtonDims.width;
                }
            }
            width = x;
            widgets[WIDX_NEW_VERSION].right = width;
            windowPos.x = (ContextGetWidth() - width) / 2;
            colours[1] = ColourWithFlags{ Drawing::Colour::lightOrange }.withFlag(ColourFlag::translucent, true);

            initScrollWidgets();
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            WindowBase* windowToOpen = nullptr;

            auto* windowMgr = GetWindowManager();

            switch (widgetIndex)
            {
                case WIDX_START_NEW_GAME:
                    windowToOpen = windowMgr->FindByClass(WindowClass::scenarioSelect);
                    if (windowToOpen != nullptr)
                    {
                        windowMgr->BringToFront(*windowToOpen);
                    }
                    else
                    {
                        windowMgr->CloseByClass(WindowClass::loadsave);
                        windowMgr->CloseByClass(WindowClass::serverList);
                        ScenarioselectOpen(WindowTitleMenuScenarioselectCallback);
                    }
                    break;
                case WIDX_CONTINUE_SAVED_GAME:
                    windowToOpen = windowMgr->FindByClass(WindowClass::loadsave);
                    if (windowToOpen != nullptr)
                    {
                        windowMgr->BringToFront(*windowToOpen);
                    }
                    else
                    {
                        windowMgr->CloseByClass(WindowClass::scenarioSelect);
                        windowMgr->CloseByClass(WindowClass::serverList);
                        auto loadOrQuitAction = GameActions::LoadOrQuitAction(GameActions::LoadOrQuitModes::OpenSavePrompt);
                        GameActions::Execute(&loadOrQuitAction, getGameState());
                    }
                    break;
                case WIDX_MULTIPLAYER:
                    windowToOpen = windowMgr->FindByClass(WindowClass::serverList);
                    if (windowToOpen != nullptr)
                    {
                        windowMgr->BringToFront(*windowToOpen);
                    }
                    else
                    {
                        windowMgr->CloseByClass(WindowClass::scenarioSelect);
                        windowMgr->CloseByClass(WindowClass::loadsave);
                        ContextOpenWindow(WindowClass::serverList);
                    }
                    break;
                case WIDX_NEW_VERSION:
                    ContextOpenWindowView(WindowView::newVersionInfo);
                    break;
            }
        }

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            if (widgetIndex == WIDX_GAME_TOOLS)
            {
                int32_t i = 0;
                gDropdown.items[i++] = Dropdown::PlainMenuLabel(STR_SCENARIO_EDITOR);
                gDropdown.items[i++] = Dropdown::PlainMenuLabel(STR_CONVERT_SAVED_GAME_TO_SCENARIO);
                gDropdown.items[i++] = Dropdown::PlainMenuLabel(STR_ROLLER_COASTER_DESIGNER);
                gDropdown.items[i++] = Dropdown::PlainMenuLabel(STR_TRACK_DESIGNS_MANAGER);
                gDropdown.items[i++] = Dropdown::PlainMenuLabel(STR_OPEN_USER_CONTENT_FOLDER);

#ifdef ENABLE_SCRIPTING
                auto hasCustomItems = false;
                const auto& customMenuItems = Scripting::CustomMenuItems;
                if (!customMenuItems.empty())
                {
                    for (const auto& item : customMenuItems)
                    {
                        if (item.Kind == Scripting::CustomToolbarMenuItemKind::Toolbox)
                        {
                            if (!hasCustomItems)
                            {
                                hasCustomItems = true;
                                gDropdown.items[i++] = Dropdown::Separator();
                            }

                            gDropdown.items[i] = Dropdown::PlainMenuLabel(item.Text.c_str());
                            i++;
                        }
                    }
                }
#endif

                Widget* widget = &widgets[widgetIndex];
                int32_t yOffset = 0;
                if (i > 5)
                {
                    yOffset = -(widget->height() - 1 + 5 + (i * 12));
                }

                WindowDropdownShowText(
                    windowPos + ScreenCoordsXY{ widget->left, widget->top + yOffset }, widget->height(),
                    colours[0].withFlag(ColourFlag::translucent, true), Dropdown::Flag::StayOpen, i);
            }
        }

        void onDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
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
                        auto context = GetContext();
                        auto& env = context->GetPlatformEnvironment();
                        auto& uiContext = context->GetUiContext();
                        uiContext.OpenFolder(env.GetDirectoryPath(DirBase::user));
                        break;
                    }
                    default:
                        InvokeCustomToolboxMenuItem(selectedIndex - DDIDX_CUSTOM_BEGIN);
                        break;
                }
            }
        }

        CursorID onCursor(WidgetIndex, const ScreenCoordsXY&, CursorID cursorId) override
        {
            gTooltipCloseTimeout = gCurrentRealTimeTicks + 2000;
            return cursorId;
        }

        void onPrepareDraw() override
        {
            _filterRect = { windowPos + ScreenCoordsXY{ 0, UpdateButtonDims.height },
                            windowPos + ScreenCoordsXY{ width - 1, MenuButtonDims.height + UpdateButtonDims.height - 1 } };
            if (GetContext()->HasNewVersionInfo())
            {
                widgets[WIDX_NEW_VERSION].type = WidgetType::button;
                _filterRect.Point1.y = windowPos.y;
            }
        }

        void onDraw(RenderTarget& rt) override
        {
            Rectangle::filter(rt, _filterRect, FilterPaletteID::palette51);
            drawWidgets(rt);
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
            WindowClass::titleMenu, ScreenCoordsXY(0, ContextGetHeight() - 182), { 0, windowHeight },
            { WindowFlag::stickToBack, WindowFlag::transparent, WindowFlag::noBackground, WindowFlag::noTitleBar });
    }
} // namespace OpenRCT2::Ui::Windows
