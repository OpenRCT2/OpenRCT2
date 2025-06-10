/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Theme.h"

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Ui::Windows
{
    enum
    {
        WINDOW_THEMES_TAB_SETTINGS,
        WINDOW_THEMES_TAB_MAIN_UI,
        WINDOW_THEMES_TAB_PARK,
        WINDOW_THEMES_TAB_TOOLS,
        WINDOW_THEMES_TAB_RIDES_PEEPS,
        WINDOW_THEMES_TAB_EDITORS,
        WINDOW_THEMES_TAB_MISC,
        WINDOW_THEMES_TAB_PROMPTS,
        WINDOW_THEMES_TAB_FEATURES,
        WINDOW_THEMES_TAB_COUNT
    };

    enum WindowThemesWidgetIdx
    {
        WIDX_THEMES_BACKGROUND,
        WIDX_THEMES_TITLE,
        WIDX_THEMES_CLOSE,
        WIDX_THEMES_TAB_CONTENT_PANEL,
        WIDX_THEMES_SETTINGS_TAB,
        WIDX_THEMES_MAIN_UI_TAB,
        WIDX_THEMES_PARK_TAB,
        WIDX_THEMES_TOOLS_TAB,
        WIDX_THEMES_RIDE_PEEPS_TAB,
        WIDX_THEMES_EDITORS_TAB,
        WIDX_THEMES_MISC_TAB,
        WIDX_THEMES_PROMPTS_TAB,
        WIDX_THEMES_FEATURES_TAB,
        WIDX_THEMES_HEADER_WINDOW,
        WIDX_THEMES_HEADER_PALETTE,
        WIDX_THEMES_HEADER_TRANSPARENCY,
        WIDX_THEMES_PRESETS,
        WIDX_THEMES_PRESETS_DROPDOWN,
        WIDX_THEMES_DUPLICATE_BUTTON,
        WIDX_THEMES_DELETE_BUTTON,
        WIDX_THEMES_RENAME_BUTTON,
        WIDX_THEMES_COLOURBTN_MASK,
        WIDX_THEMES_LIST,
        WIDX_THEMES_RCT1_RIDE_LIGHTS,
        WIDX_THEMES_RCT1_PARK_LIGHTS,
        WIDX_THEMES_RCT1_SCENARIO_FONT,
        WIDX_THEMES_RCT1_BOTTOM_TOOLBAR
    };

    static constexpr StringId WINDOW_TITLE = STR_THEMES_TITLE;
    static constexpr int32_t WW = 320;
    static constexpr int32_t WH = 107;

    const uint16_t kWindowHeaderWidth = 152;

    // clang-format off
    static constexpr auto _themesWidgets = makeWidgets(
        makeWindowShim(WINDOW_TITLE, WW, WH),
        makeWidget({  0, 43}, {320,  64},               WidgetType::resize,       WindowColour::secondary                                                                                ), // tab content panel
        makeTab   ({  3, 17},                                                                                    STR_THEMES_TAB_SETTINGS_TIP                                                   ), // settings tab
        makeTab   ({ 34, 17},                                                                                    STR_THEMES_TAB_MAIN_TIP                                                       ), // main ui tab
        makeTab   ({ 65, 17},                                                                                    STR_THEMES_TAB_PARK_TIP                                                       ), // park tab
        makeTab   ({ 96, 17},                                                                                    STR_THEMES_TAB_TOOLS_TIP                                                      ), // tools tab
        makeTab   ({127, 17},                                                                                    STR_THEMES_TAB_RIDES_AND_GUESTS_TIP                                           ), // rides and peeps tab
        makeTab   ({158, 17},                                                                                    STR_THEMES_TAB_EDITORS_TIP                                                    ), // editors tab
        makeTab   ({189, 17},                                                                                    STR_THEMES_TAB_MISC_TIP                                                       ), // misc tab
        makeTab   ({220, 17},                                                                                    STR_THEMES_TAB_PROMPTS_TIP                                                    ), // prompts tab
        makeTab   ({251, 17},                                                                                    STR_THEMES_TAB_FEATURES_TIP                                                   ), // features tab
        makeWidget({  5, 46}, {kWindowHeaderWidth, 15}, WidgetType::tableHeader,  WindowColour::secondary, STR_THEMES_HEADER_WINDOW                                                      ), // Window header
        makeWidget({157, 46}, { 79,                15}, WidgetType::tableHeader,  WindowColour::secondary, STR_THEMES_HEADER_PALETTE                                                     ), // Palette header
        makeWidget({236, 46}, { 80,                15}, WidgetType::tableHeader,  WindowColour::secondary, STR_THEMES_HEADER_TRANSPARENCY                                                ), // Transparency header
        makeWidget({125, 60}, {175,                12}, WidgetType::dropdownMenu, WindowColour::secondary                                                                                ), // Preset colour schemes
        makeWidget({288, 61}, { 11,                10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH                                                            ),
        makeWidget({ 10, 82}, { 91,                12}, WidgetType::button,       WindowColour::secondary, STR_THEMES_ACTION_DUPLICATE,                   STR_THEMES_ACTION_DUPLICATE_TIP), // Duplicate button
        makeWidget({110, 82}, { 91,                12}, WidgetType::button,       WindowColour::secondary, STR_TRACK_MANAGE_DELETE,                       STR_THEMES_ACTION_DELETE_TIP   ), // Delete button
        makeWidget({210, 82}, { 91,                12}, WidgetType::button,       WindowColour::secondary, STR_TRACK_MANAGE_RENAME,                       STR_THEMES_ACTION_RENAME_TIP   ), // Rename button
        makeWidget({  0,  0}, {  1,                 1}, WidgetType::colourBtn,    WindowColour::secondary                                                                                ), // colour button mask
        makeWidget({  3, 60}, {314,                44}, WidgetType::scroll,       WindowColour::secondary, SCROLL_VERTICAL                                                               ), // staff list
        makeWidget({ 10, 54}, {290,                12}, WidgetType::checkbox,     WindowColour::secondary, STR_THEMES_OPTION_RCT1_RIDE_CONTROLS                                          ), // rct1 ride lights
        makeWidget({ 10, 69}, {290,                12}, WidgetType::checkbox,     WindowColour::secondary, STR_THEMES_OPTION_RCT1_PARK_CONTROLS                                          ), // rct1 park lights
        makeWidget({ 10, 84}, {290,                12}, WidgetType::checkbox,     WindowColour::secondary, STR_THEMES_OPTION_RCT1_SCENARIO_SELECTION_FONT                                ), // rct1 scenario font
        makeWidget({ 10, 99}, {290,                12}, WidgetType::checkbox,     WindowColour::secondary, STR_THEMES_OPTION_RCT1_BOTTOM_TOOLBAR                                         )  // rct1 bottom toolbar
    );
    // clang-format on

#pragma region Tabs

    // clang-format off
    static int32_t window_themes_tab_animation_loops[] = {
        32,
        32,
        1,
        1,
        64,
        32,
        8,
        14,
        38,
    };
    static int32_t window_themes_tab_animation_divisor[] = {
        4,
        4,
        1,
        1,
        4,
        2,
        2,
        2,
        2,
    };
    static int32_t window_themes_tab_sprites[] = {
        SPR_TAB_PAINT_0,
        SPR_TAB_KIOSKS_AND_FACILITIES_0,
        SPR_TAB_PARK_ENTRANCE,
        SPR_G2_TAB_LAND,
        SPR_TAB_RIDE_0,
        SPR_TAB_WRENCH_0,
        SPR_TAB_GEARS_0,
        SPR_TAB_STAFF_OPTIONS_0,
        SPR_TAB_FINANCES_MARKETING_0,
    };

    static WindowClass window_themes_tab_1_classes[] = {
        WindowClass::TopToolbar,
        WindowClass::BottomToolbar,
        WindowClass::EditorScenarioBottomToolbar,
        WindowClass::EditorTrackBottomToolbar,
        WindowClass::TitleMenu,
        WindowClass::TitleExit,
        WindowClass::TitleOptions,
        WindowClass::ScenarioSelect,
    };

    static WindowClass window_themes_tab_2_classes[] = {
        WindowClass::ParkInformation,
        WindowClass::EditorParkEntrance,
        WindowClass::Finances,
        WindowClass::NewCampaign,
        WindowClass::Research,
        WindowClass::Map,
        WindowClass::Viewport,
        WindowClass::RecentNews,
    };

    static WindowClass window_themes_tab_3_classes[] = {
        WindowClass::Land,
        WindowClass::Water,
        WindowClass::ClearScenery,
        WindowClass::LandRights,
        WindowClass::Scenery,
        WindowClass::SceneryScatter,
        WindowClass::Footpath,
        WindowClass::RideConstruction,
        WindowClass::TrackDesignPlace,
        WindowClass::ConstructRide,
        WindowClass::TrackDesignList,
        WindowClass::PatrolArea,
    };

    static WindowClass window_themes_tab_4_classes[] = {
        WindowClass::Ride,
        WindowClass::RideList,
        WindowClass::Peep,
        WindowClass::GuestList,
        WindowClass::Staff,
        WindowClass::StaffList,
        WindowClass::Banner,
    };

    static WindowClass window_themes_tab_5_classes[] = {
        WindowClass::EditorObjectSelection,
        WindowClass::EditorInventionList,
        WindowClass::EditorScenarioOptions,
        WindowClass::Mapgen,
        WindowClass::ManageTrackDesign,
        WindowClass::InstallTrack,
    };

    static WindowClass window_themes_tab_6_classes[] = {
        WindowClass::Cheats,
        WindowClass::TileInspector,
        WindowClass::ViewClipping,
        WindowClass::Transparency,
        WindowClass::Themes,
        WindowClass::Options,
        WindowClass::KeyboardShortcutList,
        WindowClass::ChangeKeyboardShortcut,
        WindowClass::AssetPacks,
        WindowClass::Loadsave,
        WindowClass::About,
        WindowClass::Changelog,
        WindowClass::ServerList,
        WindowClass::Multiplayer,
        WindowClass::Player,
        WindowClass::Chat,
        WindowClass::Console,
    };

    static WindowClass window_themes_tab_7_classes[] = {
        WindowClass::Error,
        WindowClass::SavePrompt,
        WindowClass::DemolishRidePrompt,
        WindowClass::FirePrompt,
        WindowClass::TrackDeletePrompt,
        WindowClass::LoadsaveOverwritePrompt,
        WindowClass::ProgressWindow,
        WindowClass::NetworkStatus,
    };

    static WindowClass* window_themes_tab_classes[] = {
        nullptr,
        window_themes_tab_1_classes,
        window_themes_tab_2_classes,
        window_themes_tab_3_classes,
        window_themes_tab_4_classes,
        window_themes_tab_5_classes,
        window_themes_tab_6_classes,
        window_themes_tab_7_classes,
    };
    // clang-format on

#pragma endregion

    class ThemesWindow final : public Window
    {
    private:
        uint8_t _selected_tab = 0;
        int16_t _classIndex = -1;
        int8_t _buttonIndex = -1;
        const uint8_t _max_row_height = 56;
        const uint8_t _button_offset_x = 185;
        const uint8_t _button_offset_y = 3;
        const uint8_t _check_offset_y = 3;
        const uint16_t _check_offset_x = 265;
        const uint8_t _button_size = 12;

    public:
#pragma region Window Override Events

        void OnOpen() override
        {
            SetWidgets(_themesWidgets);

            WindowThemesInitVars();
            WindowInitScrollWidgets(*this);
            WindowSetResize(*this, { 320, 107 }, { 320, 107 });

            list_information_type = 0;
            _classIndex = -1;
            _buttonIndex = -1;
        }

        void OnResize() override
        {
            if (_selected_tab == WINDOW_THEMES_TAB_SETTINGS)
            {
                if (WindowSetResize(*this, { 320, 107 }, { 320, 107 }))
                    GfxInvalidateScreen();
            }
            else if (_selected_tab == WINDOW_THEMES_TAB_FEATURES)
            {
                if (WindowSetResize(*this, { 320, 122 }, { 320, 122 }))
                    GfxInvalidateScreen();
            }
            else
            {
                WindowSetResize(*this, { 320, 270 }, { 320, 450 });
            }
        }

        void OnUpdate() override
        {
            frame_no++;
            if (frame_no >= window_themes_tab_animation_loops[_selected_tab])
                frame_no = 0;

            InvalidateWidget(WIDX_THEMES_SETTINGS_TAB + _selected_tab);
        }

        void OnPrepareDraw() override
        {
            int32_t pressedWidgets = pressed_widgets
                & ~((1LL << WIDX_THEMES_SETTINGS_TAB) | (1LL << WIDX_THEMES_MAIN_UI_TAB) | (1LL << WIDX_THEMES_PARK_TAB)
                    | (1LL << WIDX_THEMES_TOOLS_TAB) | (1LL << WIDX_THEMES_RIDE_PEEPS_TAB) | (1LL << WIDX_THEMES_EDITORS_TAB)
                    | (1LL << WIDX_THEMES_MISC_TAB) | (1LL << WIDX_THEMES_PROMPTS_TAB) | (1LL << WIDX_THEMES_FEATURES_TAB));
            WidgetIndex widgetIndex = _selected_tab + WIDX_THEMES_SETTINGS_TAB;

            pressed_widgets = pressedWidgets | (1 << widgetIndex);

            auto* windowMgr = GetWindowManager();
            if (windowMgr->FindByClass(WindowClass::Dropdown) == nullptr)
            {
                _classIndex = -1;
                _buttonIndex = -1;
            }

            widgets[WIDX_THEMES_LIST].right = width - 4;
            widgets[WIDX_THEMES_LIST].bottom = height - 0x0F;

            if (_selected_tab == WINDOW_THEMES_TAB_SETTINGS)
            {
                widgets[WIDX_THEMES_HEADER_WINDOW].type = WidgetType::empty;
                widgets[WIDX_THEMES_HEADER_PALETTE].type = WidgetType::empty;
                widgets[WIDX_THEMES_HEADER_TRANSPARENCY].type = WidgetType::empty;
                widgets[WIDX_THEMES_LIST].type = WidgetType::empty;
                widgets[WIDX_THEMES_RCT1_RIDE_LIGHTS].type = WidgetType::empty;
                widgets[WIDX_THEMES_RCT1_PARK_LIGHTS].type = WidgetType::empty;
                widgets[WIDX_THEMES_RCT1_SCENARIO_FONT].type = WidgetType::empty;
                widgets[WIDX_THEMES_RCT1_BOTTOM_TOOLBAR].type = WidgetType::empty;
                widgets[WIDX_THEMES_DUPLICATE_BUTTON].type = WidgetType::button;
                widgets[WIDX_THEMES_DELETE_BUTTON].type = WidgetType::button;
                widgets[WIDX_THEMES_RENAME_BUTTON].type = WidgetType::button;
                widgets[WIDX_THEMES_PRESETS].type = WidgetType::dropdownMenu;
                widgets[WIDX_THEMES_PRESETS_DROPDOWN].type = WidgetType::button;
                widgets[WIDX_THEMES_COLOURBTN_MASK].type = WidgetType::empty;
            }
            else if (_selected_tab == WINDOW_THEMES_TAB_FEATURES)
            {
                widgets[WIDX_THEMES_HEADER_WINDOW].type = WidgetType::empty;
                widgets[WIDX_THEMES_HEADER_PALETTE].type = WidgetType::empty;
                widgets[WIDX_THEMES_HEADER_TRANSPARENCY].type = WidgetType::empty;
                widgets[WIDX_THEMES_LIST].type = WidgetType::empty;
                widgets[WIDX_THEMES_RCT1_RIDE_LIGHTS].type = WidgetType::checkbox;
                widgets[WIDX_THEMES_RCT1_PARK_LIGHTS].type = WidgetType::checkbox;
                widgets[WIDX_THEMES_RCT1_SCENARIO_FONT].type = WidgetType::checkbox;
                widgets[WIDX_THEMES_RCT1_BOTTOM_TOOLBAR].type = WidgetType::checkbox;
                widgets[WIDX_THEMES_DUPLICATE_BUTTON].type = WidgetType::empty;
                widgets[WIDX_THEMES_DELETE_BUTTON].type = WidgetType::empty;
                widgets[WIDX_THEMES_RENAME_BUTTON].type = WidgetType::empty;
                widgets[WIDX_THEMES_PRESETS].type = WidgetType::empty;
                widgets[WIDX_THEMES_PRESETS_DROPDOWN].type = WidgetType::empty;
                widgets[WIDX_THEMES_COLOURBTN_MASK].type = WidgetType::empty;

                widgetSetCheckboxValue(*this, WIDX_THEMES_RCT1_RIDE_LIGHTS, ThemeGetFlags() & UITHEME_FLAG_USE_LIGHTS_RIDE);
                widgetSetCheckboxValue(*this, WIDX_THEMES_RCT1_PARK_LIGHTS, ThemeGetFlags() & UITHEME_FLAG_USE_LIGHTS_PARK);
                widgetSetCheckboxValue(
                    *this, WIDX_THEMES_RCT1_SCENARIO_FONT, ThemeGetFlags() & UITHEME_FLAG_USE_ALTERNATIVE_SCENARIO_SELECT_FONT);
                widgetSetCheckboxValue(
                    *this, WIDX_THEMES_RCT1_BOTTOM_TOOLBAR, ThemeGetFlags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR);
            }
            else
            {
                widgets[WIDX_THEMES_HEADER_WINDOW].type = WidgetType::tableHeader;
                widgets[WIDX_THEMES_HEADER_PALETTE].type = WidgetType::tableHeader;
                widgets[WIDX_THEMES_HEADER_TRANSPARENCY].type = WidgetType::tableHeader;
                widgets[WIDX_THEMES_LIST].type = WidgetType::scroll;
                widgets[WIDX_THEMES_RCT1_RIDE_LIGHTS].type = WidgetType::empty;
                widgets[WIDX_THEMES_RCT1_PARK_LIGHTS].type = WidgetType::empty;
                widgets[WIDX_THEMES_RCT1_SCENARIO_FONT].type = WidgetType::empty;
                widgets[WIDX_THEMES_RCT1_BOTTOM_TOOLBAR].type = WidgetType::empty;
                widgets[WIDX_THEMES_DUPLICATE_BUTTON].type = WidgetType::empty;
                widgets[WIDX_THEMES_DELETE_BUTTON].type = WidgetType::empty;
                widgets[WIDX_THEMES_RENAME_BUTTON].type = WidgetType::empty;
                widgets[WIDX_THEMES_PRESETS].type = WidgetType::empty;
                widgets[WIDX_THEMES_PRESETS_DROPDOWN].type = WidgetType::empty;
                widgets[WIDX_THEMES_COLOURBTN_MASK].type = WidgetType::empty;
            }
        }

        void OnDraw(RenderTarget& rt) override
        {
            // Widgets
            WindowDrawWidgets(*this, rt);
            WindowThemesDrawTabImages(rt);

            if (_selected_tab == WINDOW_THEMES_TAB_SETTINGS)
            {
                DrawTextBasic(
                    rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_THEMES_PRESETS].top + 1 }, STR_THEMES_LABEL_CURRENT_THEME,
                    {}, { colours[1] });

                size_t activeAvailableThemeIndex = ThemeManagerGetAvailableThemeIndex();
                const utf8* activeThemeName = ThemeManagerGetAvailableThemeName(activeAvailableThemeIndex);
                auto ft = Formatter();
                ft.Add<const utf8*>(activeThemeName);

                auto screenPos = windowPos
                    + ScreenCoordsXY{ widgets[WIDX_THEMES_PRESETS].left + 1, widgets[WIDX_THEMES_PRESETS].top };
                auto newWidth = windowPos.x + widgets[WIDX_THEMES_PRESETS_DROPDOWN].left - widgets[WIDX_THEMES_PRESETS].left
                    - 4;

                DrawTextEllipsised(rt, screenPos, newWidth, STR_STRING, ft, { colours[1] });
            }
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
        {
            auto widget = &widgets[widgetIndex];
            auto* windowMgr = Ui::GetWindowManager();

            switch (widgetIndex)
            {
                case WIDX_THEMES_SETTINGS_TAB:
                case WIDX_THEMES_MAIN_UI_TAB:
                case WIDX_THEMES_PARK_TAB:
                case WIDX_THEMES_TOOLS_TAB:
                case WIDX_THEMES_RIDE_PEEPS_TAB:
                case WIDX_THEMES_EDITORS_TAB:
                case WIDX_THEMES_MISC_TAB:
                case WIDX_THEMES_PROMPTS_TAB:
                case WIDX_THEMES_FEATURES_TAB:
                {
                    auto newSelectedTab = widgetIndex - WIDX_THEMES_SETTINGS_TAB;
                    if (_selected_tab == newSelectedTab)
                        break;
                    _selected_tab = static_cast<uint8_t>(newSelectedTab);
                    scrolls[0].contentOffsetY = 0;
                    frame_no = 0;
                    OnResize();
                    Invalidate();
                    break;
                }
                case WIDX_THEMES_PRESETS_DROPDOWN:
                {
                    ThemeManagerLoadAvailableThemes();
                    auto num_items = static_cast<int32_t>(ThemeManagerGetNumAvailableThemes());

                    widget--;
                    for (int32_t i = 0; i < num_items; i++)
                    {
                        gDropdownItems[i].Format = STR_OPTIONS_DROPDOWN_ITEM;
                        gDropdownItems[i].Args = reinterpret_cast<uintptr_t>(ThemeManagerGetAvailableThemeName(i));
                    }

                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height() + 1, colours[1], 0,
                        Dropdown::Flag::StayOpen, num_items, widget->width() - 3);

                    Dropdown::SetChecked(static_cast<int32_t>(ThemeManagerGetAvailableThemeIndex()), true);
                    break;
                }
                case WIDX_THEMES_RCT1_RIDE_LIGHTS:
                    if (ThemeGetFlags() & UITHEME_FLAG_PREDEFINED)
                    {
                        ContextShowError(STR_THEMES_ERR_CANT_CHANGE_THIS_THEME, kStringIdNone, {});
                    }
                    else
                    {
                        ThemeSetFlags(ThemeGetFlags() ^ UITHEME_FLAG_USE_LIGHTS_RIDE);
                        ThemeSave();
                        windowMgr->InvalidateAll();
                    }
                    break;
                case WIDX_THEMES_RCT1_PARK_LIGHTS:
                    if (ThemeGetFlags() & UITHEME_FLAG_PREDEFINED)
                    {
                        ContextShowError(STR_THEMES_ERR_CANT_CHANGE_THIS_THEME, kStringIdNone, {});
                    }
                    else
                    {
                        ThemeSetFlags(ThemeGetFlags() ^ static_cast<uint8_t>(UITHEME_FLAG_USE_LIGHTS_PARK));
                        ThemeSave();
                        windowMgr->InvalidateAll();
                    }
                    break;
                case WIDX_THEMES_RCT1_SCENARIO_FONT:
                    if (ThemeGetFlags() & UITHEME_FLAG_PREDEFINED)
                    {
                        ContextShowError(STR_THEMES_ERR_CANT_CHANGE_THIS_THEME, kStringIdNone, {});
                    }
                    else
                    {
                        ThemeSetFlags(
                            ThemeGetFlags() ^ static_cast<uint8_t>(UITHEME_FLAG_USE_ALTERNATIVE_SCENARIO_SELECT_FONT));
                        ThemeSave();
                        windowMgr->InvalidateAll();
                    }
                    break;
                case WIDX_THEMES_RCT1_BOTTOM_TOOLBAR:
                    if (ThemeGetFlags() & UITHEME_FLAG_PREDEFINED)
                    {
                        ContextShowError(STR_THEMES_ERR_CANT_CHANGE_THIS_THEME, kStringIdNone, {});
                    }
                    else
                    {
                        ThemeSetFlags(ThemeGetFlags() ^ static_cast<uint8_t>(UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR));
                        ThemeSave();
                        windowMgr->InvalidateAll();
                    }
            }
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            size_t activeAvailableThemeIndex;
            const utf8* activeThemeName;

            switch (widgetIndex)
            {
                case WIDX_THEMES_CLOSE:
                    Close();
                    break;
                case WIDX_THEMES_DUPLICATE_BUTTON:;
                    activeAvailableThemeIndex = ThemeManagerGetAvailableThemeIndex();
                    activeThemeName = ThemeManagerGetAvailableThemeName(activeAvailableThemeIndex);
                    WindowTextInputOpen(
                        this, widgetIndex, STR_THEMES_ACTION_DUPLICATE, STR_THEMES_PROMPT_ENTER_THEME_NAME, {}, STR_STRING,
                        reinterpret_cast<uintptr_t>(activeThemeName), 64);
                    break;
                case WIDX_THEMES_DELETE_BUTTON:
                    if (ThemeGetFlags() & UITHEME_FLAG_PREDEFINED)
                    {
                        ContextShowError(STR_THEMES_ERR_CANT_CHANGE_THIS_THEME, kStringIdNone, {});
                    }
                    else
                    {
                        ThemeDelete();
                    }
                    break;
                case WIDX_THEMES_RENAME_BUTTON:
                    if (ThemeGetFlags() & UITHEME_FLAG_PREDEFINED)
                    {
                        ContextShowError(STR_THEMES_ERR_CANT_CHANGE_THIS_THEME, kStringIdNone, {});
                    }
                    else
                    {
                        activeAvailableThemeIndex = ThemeManagerGetAvailableThemeIndex();
                        activeThemeName = ThemeManagerGetAvailableThemeName(activeAvailableThemeIndex);
                        WindowTextInputOpen(
                            this, widgetIndex, STR_TRACK_MANAGE_RENAME, STR_THEMES_PROMPT_ENTER_THEME_NAME, {}, STR_STRING,
                            reinterpret_cast<uintptr_t>(activeThemeName), 64);
                    }
                    break;
            }
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_THEMES_LIST:
                    if (selectedIndex != -1)
                    {
                        ColourWithFlags newColour = { ColourDropDownIndexToColour(selectedIndex) };
                        WindowClass wc = GetWindowClassTabIndex(_classIndex);
                        auto oldColour = ThemeGetColour(wc, _buttonIndex);
                        newColour.flags = oldColour.flags;
                        ThemeSetColour(wc, _buttonIndex, newColour);
                        ColourSchemeUpdateAll();

                        auto* windowMgr = GetWindowManager();
                        windowMgr->InvalidateAll();

                        _classIndex = -1;
                        _buttonIndex = -1;
                    }
                    break;
                case WIDX_THEMES_PRESETS_DROPDOWN:
                    if (selectedIndex != -1)
                    {
                        ThemeManagerSetActiveAvailableTheme(selectedIndex);
                    }
                    break;
            }
        }

        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (text.empty())
                return;

            switch (widgetIndex)
            {
                case WIDX_THEMES_DUPLICATE_BUTTON:
                case WIDX_THEMES_RENAME_BUTTON:
                    if (Platform::IsFilenameValid(text))
                    {
                        if (ThemeGetIndexForName(std::string(text).c_str()) == SIZE_MAX)
                        {
                            if (widgetIndex == WIDX_THEMES_DUPLICATE_BUTTON)
                            {
                                ThemeDuplicate(std::string(text).c_str());
                            }
                            else
                            {
                                ThemeRename(std::string(text).c_str());
                            }
                            Invalidate();
                        }
                        else
                        {
                            ContextShowError(STR_THEMES_ERR_NAME_ALREADY_EXISTS, kStringIdNone, {});
                        }
                    }
                    else
                    {
                        ContextShowError(STR_ERROR_INVALID_CHARACTERS, kStringIdNone, {});
                    }
                    break;
            }
        }

        ScreenSize OnScrollGetSize(int32_t scrollIndex) override
        {
            if (_selected_tab == WINDOW_THEMES_TAB_SETTINGS || _selected_tab == WINDOW_THEMES_TAB_FEATURES)
                return {};

            int32_t scrollHeight = GetTotalColoursUpTo(GetColourSchemeTabCount()) * (_button_size + 2);
            int32_t i = scrollHeight - widgets[WIDX_THEMES_LIST].bottom + widgets[WIDX_THEMES_LIST].top + 21;
            if (i < 0)
                i = 0;
            if (i < scrolls[0].contentOffsetY)
            {
                scrolls[0].contentOffsetY = i;
                Invalidate();
            }

            return { 420, scrollHeight };
        }

        void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (screenCoords.y / _max_row_height < GetColourSchemeTabCount())
            {
                _classIndex = GetClassIndex(screenCoords.y);

                WindowClass wc = GetWindowClassTabIndex(_classIndex);
                int32_t numColours = ThemeDescGetNumColours(wc);

                numColours = AddGap(numColours, _classIndex);

                // position of y relative to the current class
                int32_t y2 = screenCoords.y - (GetTotalColoursUpTo(_classIndex + 1) - numColours) * (_button_size + 2);
                _buttonIndex = (y2 / _button_size);

                if (_buttonIndex < numColours)
                {
                    if (screenCoords.x >= _button_offset_x && screenCoords.x < _button_offset_x + (_button_size + 2)
                        && y2 >= _button_offset_y && y2 < _button_offset_y + (_button_size + 2) * 6)
                    {
                        if (ThemeGetFlags() & UITHEME_FLAG_PREDEFINED)
                        {
                            ContextShowError(STR_THEMES_ERR_CANT_CHANGE_THIS_THEME, STR_THEMES_DESC_CANT_CHANGE_THIS_THEME, {});
                        }
                        else
                        {
                            widgets[WIDX_THEMES_COLOURBTN_MASK].type = WidgetType::colourBtn;
                            widgets[WIDX_THEMES_COLOURBTN_MASK].left = _button_offset_x + widgets[WIDX_THEMES_LIST].left
                                + _button_size;
                            widgets[WIDX_THEMES_COLOURBTN_MASK].top = GetTotalColoursUpTo(_classIndex) * (_button_size + 2)
                                - _button_offset_y - 4 + _button_size * _buttonIndex - scrolls[0].contentOffsetY
                                + widgets[WIDX_THEMES_LIST].top;
                            widgets[WIDX_THEMES_COLOURBTN_MASK].right = widgets[WIDX_THEMES_COLOURBTN_MASK].left + 12;
                            widgets[WIDX_THEMES_COLOURBTN_MASK].bottom = widgets[WIDX_THEMES_COLOURBTN_MASK].top + 12;

                            auto colour = ThemeGetColour(wc, _buttonIndex);
                            WindowDropdownShowColour(
                                this, &(widgets[WIDX_THEMES_COLOURBTN_MASK]), colours[1], colour.colour, true);
                            InvalidateWidget(WIDX_THEMES_LIST);
                        }
                    }
                    else if (
                        screenCoords.x >= _check_offset_x && screenCoords.x < _check_offset_x + _button_size
                        && y2 >= _check_offset_y && y2 < _check_offset_y + _button_size * 6 - 1)
                    {
                        if (ThemeGetFlags() & UITHEME_FLAG_PREDEFINED)
                        {
                            ContextShowError(STR_THEMES_ERR_CANT_CHANGE_THIS_THEME, STR_THEMES_DESC_CANT_CHANGE_THIS_THEME, {});
                        }
                        else
                        {
                            auto colour = ThemeGetColour(wc, _buttonIndex);
                            colour.setFlag(ColourFlag::translucent, !colour.hasFlag(ColourFlag::translucent));
                            ThemeSetColour(wc, _buttonIndex, colour);
                            ColourSchemeUpdateAll();

                            auto* windowMgr = Ui::GetWindowManager();
                            windowMgr->InvalidateAll();
                        }
                    }
                }
            }
        }

        void OnScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            ScreenCoordsXY screenCoords;

            if (_selected_tab == WINDOW_THEMES_TAB_SETTINGS || _selected_tab == WINDOW_THEMES_TAB_FEATURES)
                return;

            if (!colours[1].hasFlag(ColourFlag::translucent))
                // GfxFillRect(dpi, dpi->x, dpi->y, dpi->x + dpi->width - 1, dpi->y + dpi->height - 1,
                // ColourMapA[colours[1].colour].mid_light);
                GfxClear(rt, ColourMapA[colours[1].colour].mid_light);
            screenCoords.y = 0;
            for (int32_t i = 0; i < GetColourSchemeTabCount(); i++)
            {
                WindowClass wc = GetWindowClassTabIndex(i);
                int32_t numColours = ThemeDescGetNumColours(wc);

                // If the window description wraps, make sure the row is high enough.
                bool emptyRow = false;

                int32_t newNumColours = AddGap(numColours, i);

                if (newNumColours != numColours)
                {
                    numColours = newNumColours;
                    emptyRow = true;
                }

                if (screenCoords.y > rt.y + rt.height)
                {
                    break;
                }
                if (screenCoords.y + _max_row_height >= rt.y)
                {
                    if (i + 1 < GetColourSchemeTabCount())
                    {
                        auto colour = colours[1];

                        int8_t colorOffset = (_button_size + 2) * (4 - numColours);

                        auto leftTop = ScreenCoordsXY{ 0, screenCoords.y + _max_row_height - colorOffset + 1 };
                        auto rightBottom = ScreenCoordsXY{ widgets[WIDX_THEMES_LIST].right,
                                                           screenCoords.y + _max_row_height - colorOffset + 1 };
                        auto yPixelOffset = ScreenCoordsXY{ 0, 1 };

                        if (colour.hasFlag(ColourFlag::translucent))
                        {
                            TranslucentWindowPalette windowPalette = TranslucentWindowPalettes[colour.colour];

                            GfxFilterRect(rt, { leftTop, rightBottom }, windowPalette.highlight);
                            GfxFilterRect(rt, { leftTop + yPixelOffset, rightBottom + yPixelOffset }, windowPalette.shadow);
                        }
                        else
                        {
                            colour = ColourMapA[colours[1].colour].mid_dark;
                            GfxFillRect(rt, { leftTop, rightBottom }, colour.colour);

                            colour = ColourMapA[colours[1].colour].lightest;
                            GfxFillRect(rt, { leftTop + yPixelOffset, rightBottom + yPixelOffset }, colour.colour);
                        }
                    }

                    for (uint8_t j = 0; j < numColours; j++)
                    {
                        DrawTextWrapped(
                            rt, { 2, screenCoords.y + 4 }, kWindowHeaderWidth, ThemeDescGetName(wc), {}, { colours[1] });

                        // Don't draw the empty row
                        if (emptyRow && j == 1)
                        {
                            break;
                        }

                        auto colour = ThemeGetColour(wc, j);
                        const bool isPressed = (i == _classIndex && j == _buttonIndex);
                        auto image = ImageId(isPressed ? SPR_PALETTE_BTN_PRESSED : SPR_PALETTE_BTN, colour.colour);
                        GfxDrawSprite(
                            rt, image, { _button_offset_x, screenCoords.y + _button_offset_y + _button_size * j + 1 });

                        ScreenCoordsXY topLeft{ _check_offset_x, screenCoords.y + _check_offset_y + _button_size * j };
                        ScreenCoordsXY bottomRight{ _check_offset_x + 10,
                                                    screenCoords.y + _check_offset_y + 11 + _button_size * j };
                        GfxFillRectInset(rt, { topLeft, bottomRight }, colours[1], INSET_RECT_F_E0);
                        if (colour.hasFlag(ColourFlag::translucent))
                        {
                            DrawText(
                                rt, topLeft, { colours[1].colour, FontStyle::Medium, TextDarkness::Dark }, kCheckMarkString);
                        }
                    }
                }

                screenCoords.y += ((_button_size + 2) * numColours);
            }
        }

#pragma endregion

        void WindowThemesInitVars()
        {
            _selected_tab = WINDOW_THEMES_TAB_SETTINGS;
        }

        WindowClass GetWindowClassTabIndex(int32_t index)
        {
            WindowClass* classes = window_themes_tab_classes[_selected_tab];
            return classes[index];
        }

        // If the window description wraps, make sure the row is high enough.
        int32_t AddGap(int32_t numColours, int8_t index)
        {
            if (numColours == 1 && GetTextWidth(index) >= kWindowHeaderWidth)
            {
                numColours++;
            }
            return numColours;
        }

        int8_t GetClassIndex(int32_t y)
        {
            int8_t numRows = 0;
            for (int32_t i = 0; i < GetColourSchemeTabCount(); ++i)
            {
                int32_t numColours = ThemeDescGetNumColours(GetWindowClassTabIndex(i));

                numColours = AddGap(numColours, i);

                numRows += numColours;

                if ((numRows * (_button_size + 2)) >= y)
                {
                    return i;
                }
            }
            return -1;
        }

        int32_t GetTextWidth(int8_t index)
        {
            WindowClass wc = GetWindowClassTabIndex(index);
            Formatter ft{};

            const void* args{};
            StringId format = ThemeDescGetName(wc);

            std::string str = FormatStringIDLegacy(format, args);

            return GfxGetStringWidth(str, FontStyle::Medium);
        }

        int8_t GetTotalColoursUpTo(int8_t index)
        {
            int8_t numRows = 0;
            for (int32_t i = 0; i < index; ++i)
            {
                uint8_t numColours = ThemeDescGetNumColours(GetWindowClassTabIndex(i));

                if (numColours == 1 && GetTextWidth(i) >= kWindowHeaderWidth)
                {
                    numColours++;
                }

                numRows += numColours;
            }
            return numRows;
        }

        int32_t GetColourSchemeTabCount()
        {
            switch (_selected_tab)
            {
                case 1:
                    return sizeof(window_themes_tab_1_classes);
                case 2:
                    return sizeof(window_themes_tab_2_classes);
                case 3:
                    return sizeof(window_themes_tab_3_classes);
                case 4:
                    return sizeof(window_themes_tab_4_classes);
                case 5:
                    return sizeof(window_themes_tab_5_classes);
                case 6:
                    return sizeof(window_themes_tab_6_classes);
                case 7:
                    return sizeof(window_themes_tab_7_classes);
            }
            return 0;
        }

        void WindowThemesDrawTabImages(RenderTarget& rt)
        {
            for (int32_t i = 0; i < WINDOW_THEMES_TAB_COUNT; i++)
            {
                int32_t sprite_idx = window_themes_tab_sprites[i];
                if (_selected_tab == i)
                    sprite_idx += frame_no / window_themes_tab_animation_divisor[_selected_tab];
                GfxDrawSprite(
                    rt, ImageId(sprite_idx),
                    windowPos
                        + ScreenCoordsXY{ widgets[WIDX_THEMES_SETTINGS_TAB + i].left,
                                          widgets[WIDX_THEMES_SETTINGS_TAB + i].top });
            }
        }
    };

    WindowBase* ThemesOpen()
    {
        // Check if window is already open
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->BringToFrontByClass(WindowClass::Themes);
        if (window != nullptr)
            return window;

        window = windowMgr->Create<ThemesWindow>(WindowClass::Themes, 320, 107, WF_10 | WF_RESIZABLE);

        return window;
    }
} // namespace OpenRCT2::Ui::Windows
