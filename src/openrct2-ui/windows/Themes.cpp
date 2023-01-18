/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Theme.h"

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/sprites.h>

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

static constexpr const StringId WINDOW_TITLE = STR_THEMES_TITLE;
static constexpr const int32_t WW = 320;
static constexpr const int32_t WH = 107;

// clang-format off
static Widget window_themes_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({  0, 43}, {320,  64}, WindowWidgetType::Resize,       WindowColour::Secondary                                                                                     ), // tab content panel
    MakeTab   ({  3, 17},                                                                                                        STR_THEMES_TAB_SETTINGS_TIP        ), // settings tab
    MakeTab   ({ 34, 17},                                                                                                        STR_THEMES_TAB_MAIN_TIP            ), // main ui tab
    MakeTab   ({ 65, 17},                                                                                                        STR_THEMES_TAB_PARK_TIP            ), // park tab
    MakeTab   ({ 96, 17},                                                                                                        STR_THEMES_TAB_TOOLS_TIP           ), // tools tab
    MakeTab   ({127, 17},                                                                                                        STR_THEMES_TAB_RIDES_AND_GUESTS_TIP), // rides and peeps tab
    MakeTab   ({158, 17},                                                                                                        STR_THEMES_TAB_EDITORS_TIP         ), // editors tab
    MakeTab   ({189, 17},                                                                                                        STR_THEMES_TAB_MISC_TIP            ), // misc tab
    MakeTab   ({220, 17},                                                                                                        STR_THEMES_TAB_PROMPTS_TIP         ), // prompts tab
    MakeTab   ({251, 17},                                                                                                        STR_THEMES_TAB_FEATURES_TIP        ), // features tab
    MakeWidget({  5, 46}, {214,  15}, WindowWidgetType::TableHeader, WindowColour::Secondary, STR_THEMES_HEADER_WINDOW                                                           ), // Window header
    MakeWidget({219, 46}, { 97,  15}, WindowWidgetType::TableHeader, WindowColour::Secondary, STR_THEMES_HEADER_PALETTE                                                          ), // Palette header
    MakeWidget({125, 60}, {175,  12}, WindowWidgetType::DropdownMenu,     WindowColour::Secondary                                                                                     ), // Preset colour schemes
    MakeWidget({288, 61}, { 11,  10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH                                                                 ),
    MakeWidget({ 10, 82}, { 91,  12}, WindowWidgetType::Button,       WindowColour::Secondary, STR_THEMES_ACTION_DUPLICATE,                    STR_THEMES_ACTION_DUPLICATE_TIP    ), // Duplicate button
    MakeWidget({110, 82}, { 91,  12}, WindowWidgetType::Button,       WindowColour::Secondary, STR_TRACK_MANAGE_DELETE,                        STR_THEMES_ACTION_DELETE_TIP       ), // Delete button
    MakeWidget({210, 82}, { 91,  12}, WindowWidgetType::Button,       WindowColour::Secondary, STR_TRACK_MANAGE_RENAME,                        STR_THEMES_ACTION_RENAME_TIP       ), // Rename button
    MakeWidget({  0,  0}, {  1,   1}, WindowWidgetType::ColourBtn,    WindowColour::Secondary                                                                                     ), // colour button mask
    MakeWidget({  3, 60}, {314,  44}, WindowWidgetType::Scroll,       WindowColour::Secondary, SCROLL_VERTICAL                                                                    ), // staff list
    MakeWidget({ 10, 54}, {290,  12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_THEMES_OPTION_RCT1_RIDE_CONTROLS                                               ), // rct1 ride lights
    MakeWidget({ 10, 69}, {290,  12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_THEMES_OPTION_RCT1_PARK_CONTROLS                                               ), // rct1 park lights
    MakeWidget({ 10, 84}, {290,  12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_THEMES_OPTION_RCT1_SCENARIO_SELECTION_FONT                                     ), // rct1 scenario font
    MakeWidget({ 10, 99}, {290,  12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_THEMES_OPTION_RCT1_BOTTOM_TOOLBAR                                              ), // rct1 bottom toolbar
    WIDGETS_END,
};

#pragma region Tabs

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
    WindowClass::EditorObjectiveOptions,
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
    WindowClass::SavePrompt,
    WindowClass::DemolishRidePrompt,
    WindowClass::FirePrompt,
    WindowClass::TrackDeletePrompt,
    WindowClass::LoadsaveOverwritePrompt,
    WindowClass::NetworkStatus,
};
// clang-format on

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

#pragma endregion

class ThemesWindow final : public Window
{
private:
    uint8_t _selected_tab = 0;
    int16_t _colour_index_1 = -1;
    int8_t _colour_index_2 = -1;
    const uint8_t _row_height = 32;
    const uint8_t _button_offset_x = 220;
    const uint8_t _button_offset_y = 3;
    const uint8_t _check_offset_y = 3 + 12 + 2;

public:
#pragma region Window Override Events

    void OnOpen() override
    {
        widgets = window_themes_widgets;

        WindowThemesInitVars();

        WindowInitScrollWidgets(*this);
        list_information_type = 0;
        _colour_index_1 = -1;
        _colour_index_2 = -1;
        min_width = 320;
        min_height = 107;
        max_width = 320;
        max_height = 107;
    }

    void OnResize() override
    {
        if (_selected_tab == WINDOW_THEMES_TAB_SETTINGS)
        {
            min_width = 320;
            min_height = 107;
            max_width = 320;
            max_height = 107;

            if (width < min_width)
            {
                width = min_width;
                GfxInvalidateScreen();
            }
            if (height < min_height)
            {
                height = min_height;
                GfxInvalidateScreen();
            }
            if (width > max_width)
            {
                width = max_width;
                GfxInvalidateScreen();
            }
            if (height > max_height)
            {
                height = max_height;
                GfxInvalidateScreen();
            }
        }
        else if (_selected_tab == WINDOW_THEMES_TAB_FEATURES)
        {
            min_width = 320;
            min_height = 122;
            max_width = 320;
            max_height = 122;

            if (width < min_width)
            {
                width = min_width;
                GfxInvalidateScreen();
            }
            if (height < min_height)
            {
                height = min_height;
                GfxInvalidateScreen();
            }
            if (width > max_width)
            {
                width = max_width;
                GfxInvalidateScreen();
            }
            if (height > max_height)
            {
                height = max_height;
                GfxInvalidateScreen();
            }
        }
        else
        {
            min_width = 320;
            min_height = 270;
            max_width = 320;
            max_height = 450;

            if (width < min_width)
            {
                width = min_width;
                Invalidate();
            }
            if (height < min_height)
            {
                height = min_height;
                Invalidate();
            }
            if (width > max_width)
            {
                width = max_width;
                Invalidate();
            }
            if (height > max_height)
            {
                height = max_height;
                Invalidate();
            }
        }
    }

    void OnUpdate() override
    {
        frame_no++;
        if (frame_no >= window_themes_tab_animation_loops[_selected_tab])
            frame_no = 0;

        WidgetInvalidate(*this, WIDX_THEMES_SETTINGS_TAB + _selected_tab);
    }

    void OnPrepareDraw() override
    {
        int32_t pressedWidgets = pressed_widgets
            & ~((1LL << WIDX_THEMES_SETTINGS_TAB) | (1LL << WIDX_THEMES_MAIN_UI_TAB) | (1LL << WIDX_THEMES_PARK_TAB)
                | (1LL << WIDX_THEMES_TOOLS_TAB) | (1LL << WIDX_THEMES_RIDE_PEEPS_TAB) | (1LL << WIDX_THEMES_EDITORS_TAB)
                | (1LL << WIDX_THEMES_MISC_TAB) | (1LL << WIDX_THEMES_PROMPTS_TAB) | (1LL << WIDX_THEMES_FEATURES_TAB));
        WidgetIndex widgetIndex = _selected_tab + WIDX_THEMES_SETTINGS_TAB;

        pressed_widgets = pressedWidgets | (1 << widgetIndex);

        if (WindowFindByClass(WindowClass::Dropdown) == nullptr)
        {
            _colour_index_1 = -1;
            _colour_index_2 = -1;
        }

        window_themes_widgets[WIDX_THEMES_BACKGROUND].right = width - 1;
        window_themes_widgets[WIDX_THEMES_BACKGROUND].bottom = height - 1;
        window_themes_widgets[WIDX_THEMES_TAB_CONTENT_PANEL].right = width - 1;
        window_themes_widgets[WIDX_THEMES_TAB_CONTENT_PANEL].bottom = height - 1;
        window_themes_widgets[WIDX_THEMES_TITLE].right = width - 2;
        window_themes_widgets[WIDX_THEMES_CLOSE].left = width - 2 - 0x0B;
        window_themes_widgets[WIDX_THEMES_CLOSE].right = width - 2 - 0x0B + 0x0A;
        window_themes_widgets[WIDX_THEMES_LIST].right = width - 4;
        window_themes_widgets[WIDX_THEMES_LIST].bottom = height - 0x0F;

        if (_selected_tab == WINDOW_THEMES_TAB_SETTINGS)
        {
            window_themes_widgets[WIDX_THEMES_HEADER_WINDOW].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_HEADER_PALETTE].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_LIST].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_RCT1_RIDE_LIGHTS].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_RCT1_PARK_LIGHTS].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_RCT1_SCENARIO_FONT].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_RCT1_BOTTOM_TOOLBAR].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_DUPLICATE_BUTTON].type = WindowWidgetType::Button;
            window_themes_widgets[WIDX_THEMES_DELETE_BUTTON].type = WindowWidgetType::Button;
            window_themes_widgets[WIDX_THEMES_RENAME_BUTTON].type = WindowWidgetType::Button;
            window_themes_widgets[WIDX_THEMES_PRESETS].type = WindowWidgetType::DropdownMenu;
            window_themes_widgets[WIDX_THEMES_PRESETS_DROPDOWN].type = WindowWidgetType::Button;
            window_themes_widgets[WIDX_THEMES_COLOURBTN_MASK].type = WindowWidgetType::Empty;
        }
        else if (_selected_tab == WINDOW_THEMES_TAB_FEATURES)
        {
            window_themes_widgets[WIDX_THEMES_HEADER_WINDOW].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_HEADER_PALETTE].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_LIST].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_RCT1_RIDE_LIGHTS].type = WindowWidgetType::Checkbox;
            window_themes_widgets[WIDX_THEMES_RCT1_PARK_LIGHTS].type = WindowWidgetType::Checkbox;
            window_themes_widgets[WIDX_THEMES_RCT1_SCENARIO_FONT].type = WindowWidgetType::Checkbox;
            window_themes_widgets[WIDX_THEMES_RCT1_BOTTOM_TOOLBAR].type = WindowWidgetType::Checkbox;
            window_themes_widgets[WIDX_THEMES_DUPLICATE_BUTTON].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_DELETE_BUTTON].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_RENAME_BUTTON].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_PRESETS].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_PRESETS_DROPDOWN].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_COLOURBTN_MASK].type = WindowWidgetType::Empty;

            WidgetSetCheckboxValue(*this, WIDX_THEMES_RCT1_RIDE_LIGHTS, ThemeGetFlags() & UITHEME_FLAG_USE_LIGHTS_RIDE);
            WidgetSetCheckboxValue(*this, WIDX_THEMES_RCT1_PARK_LIGHTS, ThemeGetFlags() & UITHEME_FLAG_USE_LIGHTS_PARK);
            WidgetSetCheckboxValue(
                *this, WIDX_THEMES_RCT1_SCENARIO_FONT, ThemeGetFlags() & UITHEME_FLAG_USE_ALTERNATIVE_SCENARIO_SELECT_FONT);
            WidgetSetCheckboxValue(
                *this, WIDX_THEMES_RCT1_BOTTOM_TOOLBAR, ThemeGetFlags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR);
        }
        else
        {
            window_themes_widgets[WIDX_THEMES_HEADER_WINDOW].type = WindowWidgetType::TableHeader;
            window_themes_widgets[WIDX_THEMES_HEADER_PALETTE].type = WindowWidgetType::TableHeader;
            window_themes_widgets[WIDX_THEMES_LIST].type = WindowWidgetType::Scroll;
            window_themes_widgets[WIDX_THEMES_RCT1_RIDE_LIGHTS].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_RCT1_PARK_LIGHTS].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_RCT1_SCENARIO_FONT].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_RCT1_BOTTOM_TOOLBAR].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_DUPLICATE_BUTTON].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_DELETE_BUTTON].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_RENAME_BUTTON].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_PRESETS].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_PRESETS_DROPDOWN].type = WindowWidgetType::Empty;
            window_themes_widgets[WIDX_THEMES_COLOURBTN_MASK].type = WindowWidgetType::Empty;
        }
    }

    void OnDraw(DrawPixelInfo& dpi) override
    {
        // Widgets
        WindowDrawWidgets(*this, &dpi);
        WindowThemesDrawTabImages(&dpi);

        if (_selected_tab == WINDOW_THEMES_TAB_SETTINGS)
        {
            DrawTextBasic(
                &dpi, windowPos + ScreenCoordsXY{ 10, window_themes_widgets[WIDX_THEMES_PRESETS].top + 1 },
                STR_THEMES_LABEL_CURRENT_THEME, {}, { colours[1] });

            size_t activeAvailableThemeIndex = ThemeManagerGetAvailableThemeIndex();
            const utf8* activeThemeName = ThemeManagerGetAvailableThemeName(activeAvailableThemeIndex);
            auto ft = Formatter();
            ft.Add<const utf8*>(activeThemeName);

            auto screenPos = windowPos
                + ScreenCoordsXY{ window_themes_widgets[WIDX_THEMES_PRESETS].left + 1,
                                  window_themes_widgets[WIDX_THEMES_PRESETS].top };
            auto newWidth = windowPos.x + window_themes_widgets[WIDX_THEMES_PRESETS_DROPDOWN].left
                - window_themes_widgets[WIDX_THEMES_PRESETS].left - 4;

            DrawTextEllipsised(&dpi, screenPos, newWidth, STR_STRING, ft, { colours[1] });
        }
    }

    void OnMouseDown(WidgetIndex widgetIndex) override
    {
        int16_t newSelectedTab;
        int32_t num_items;
        auto widget = &widgets[widgetIndex];

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
                newSelectedTab = widgetIndex - WIDX_THEMES_SETTINGS_TAB;
                if (_selected_tab == newSelectedTab)
                    break;
                _selected_tab = static_cast<uint8_t>(newSelectedTab);
                scrolls[0].v_top = 0;
                frame_no = 0;
                WindowEventResizeCall(this);
                Invalidate();
                break;
            case WIDX_THEMES_PRESETS_DROPDOWN:
                ThemeManagerLoadAvailableThemes();
                num_items = static_cast<int32_t>(ThemeManagerGetNumAvailableThemes());

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
            case WIDX_THEMES_RCT1_RIDE_LIGHTS:
                if (ThemeGetFlags() & UITHEME_FLAG_PREDEFINED)
                {
                    ContextShowError(STR_THEMES_ERR_CANT_CHANGE_THIS_THEME, STR_NONE, {});
                }
                else
                {
                    ThemeSetFlags(ThemeGetFlags() ^ UITHEME_FLAG_USE_LIGHTS_RIDE);
                    ThemeSave();
                    WindowInvalidateAll();
                }
                break;
            case WIDX_THEMES_RCT1_PARK_LIGHTS:
                if (ThemeGetFlags() & UITHEME_FLAG_PREDEFINED)
                {
                    ContextShowError(STR_THEMES_ERR_CANT_CHANGE_THIS_THEME, STR_NONE, {});
                }
                else
                {
                    ThemeSetFlags(ThemeGetFlags() ^ static_cast<uint8_t>(UITHEME_FLAG_USE_LIGHTS_PARK));
                    ThemeSave();
                    WindowInvalidateAll();
                }
                break;
            case WIDX_THEMES_RCT1_SCENARIO_FONT:
                if (ThemeGetFlags() & UITHEME_FLAG_PREDEFINED)
                {
                    ContextShowError(STR_THEMES_ERR_CANT_CHANGE_THIS_THEME, STR_NONE, {});
                }
                else
                {
                    ThemeSetFlags(ThemeGetFlags() ^ static_cast<uint8_t>(UITHEME_FLAG_USE_ALTERNATIVE_SCENARIO_SELECT_FONT));
                    ThemeSave();
                    WindowInvalidateAll();
                }
                break;
            case WIDX_THEMES_RCT1_BOTTOM_TOOLBAR:
                if (ThemeGetFlags() & UITHEME_FLAG_PREDEFINED)
                {
                    ContextShowError(STR_THEMES_ERR_CANT_CHANGE_THIS_THEME, STR_NONE, {});
                }
                else
                {
                    ThemeSetFlags(ThemeGetFlags() ^ static_cast<uint8_t>(UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR));
                    ThemeSave();
                    WindowInvalidateAll();
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
                    ContextShowError(STR_THEMES_ERR_CANT_CHANGE_THIS_THEME, STR_NONE, {});
                }
                else
                {
                    ThemeDelete();
                }
                break;
            case WIDX_THEMES_RENAME_BUTTON:
                if (ThemeGetFlags() & UITHEME_FLAG_PREDEFINED)
                {
                    ContextShowError(STR_THEMES_ERR_CANT_CHANGE_THIS_THEME, STR_NONE, {});
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
                    WindowClass wc = GetWindowClassTabIndex(_colour_index_1);
                    uint8_t colour = ThemeGetColour(wc, _colour_index_2);
                    colour = (colour & COLOUR_FLAG_TRANSLUCENT) | selectedIndex;
                    ThemeSetColour(wc, _colour_index_2, colour);
                    ColourSchemeUpdateAll();
                    WindowInvalidateAll();
                    _colour_index_1 = -1;
                    _colour_index_2 = -1;
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
                        ContextShowError(STR_THEMES_ERR_NAME_ALREADY_EXISTS, STR_NONE, {});
                    }
                }
                else
                {
                    ContextShowError(STR_ERROR_INVALID_CHARACTERS, STR_NONE, {});
                }
                break;
        }
    }

    ScreenSize OnScrollGetSize(int32_t scrollIndex) override
    {
        if (_selected_tab == WINDOW_THEMES_TAB_SETTINGS || _selected_tab == WINDOW_THEMES_TAB_FEATURES)
            return {};

        int32_t scrollHeight = GetColourSchemeTabCount() * _row_height;
        int32_t i = scrollHeight - window_themes_widgets[WIDX_THEMES_LIST].bottom + window_themes_widgets[WIDX_THEMES_LIST].top
            + 21;
        if (i < 0)
            i = 0;
        if (i < scrolls[0].v_top)
        {
            scrolls[0].v_top = i;
            Invalidate();
        }

        return { 420, scrollHeight };
    }

    void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        if (screenCoords.y / _row_height < GetColourSchemeTabCount())
        {
            int32_t y2 = screenCoords.y % _row_height;
            _colour_index_1 = screenCoords.y / _row_height;
            _colour_index_2 = ((screenCoords.x - _button_offset_x) / 12);

            WindowClass wc = GetWindowClassTabIndex(_colour_index_1);
            int32_t numColours = ThemeDescGetNumColours(wc);
            if (_colour_index_2 < numColours)
            {
                if (screenCoords.x >= _button_offset_x && screenCoords.x < _button_offset_x + 12 * 6 && y2 >= _button_offset_y
                    && y2 < _button_offset_y + 11)
                {
                    if (ThemeGetFlags() & UITHEME_FLAG_PREDEFINED)
                    {
                        ContextShowError(STR_THEMES_ERR_CANT_CHANGE_THIS_THEME, STR_THEMES_DESC_CANT_CHANGE_THIS_THEME, {});
                    }
                    else
                    {
                        window_themes_widgets[WIDX_THEMES_COLOURBTN_MASK].type = WindowWidgetType::ColourBtn;
                        window_themes_widgets[WIDX_THEMES_COLOURBTN_MASK].left = _button_offset_x + _colour_index_2 * 12
                            + window_themes_widgets[WIDX_THEMES_LIST].left;
                        window_themes_widgets[WIDX_THEMES_COLOURBTN_MASK].top = _colour_index_1 * _row_height + _button_offset_y
                            - scrolls[0].v_top + window_themes_widgets[WIDX_THEMES_LIST].top;
                        window_themes_widgets[WIDX_THEMES_COLOURBTN_MASK].right = window_themes_widgets
                                                                                      [WIDX_THEMES_COLOURBTN_MASK]
                                                                                          .left
                            + 12;
                        window_themes_widgets[WIDX_THEMES_COLOURBTN_MASK].bottom = window_themes_widgets
                                                                                       [WIDX_THEMES_COLOURBTN_MASK]
                                                                                           .top
                            + 12;

                        uint8_t colour = ThemeGetColour(wc, _colour_index_2);
                        WindowDropdownShowColour(
                            this, &(window_themes_widgets[WIDX_THEMES_COLOURBTN_MASK]), colours[1], colour);
                        WidgetInvalidate(*this, WIDX_THEMES_LIST);
                    }
                }
                else if (
                    screenCoords.x >= _button_offset_x && screenCoords.x < _button_offset_x + 12 * 6 - 1
                    && y2 >= _check_offset_y && y2 < _check_offset_y + 11)
                {
                    if (ThemeGetFlags() & UITHEME_FLAG_PREDEFINED)
                    {
                        ContextShowError(STR_THEMES_ERR_CANT_CHANGE_THIS_THEME, STR_THEMES_DESC_CANT_CHANGE_THIS_THEME, {});
                    }
                    else
                    {
                        uint8_t colour = ThemeGetColour(wc, _colour_index_2);
                        if (colour & COLOUR_FLAG_TRANSLUCENT)
                        {
                            colour &= ~COLOUR_FLAG_TRANSLUCENT;
                        }
                        else
                        {
                            colour |= COLOUR_FLAG_TRANSLUCENT;
                        }
                        ThemeSetColour(wc, _colour_index_2, colour);
                        ColourSchemeUpdateAll();
                        WindowInvalidateAll();
                    }
                }
            }
        }
    }

    void OnScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi) override
    {
        ScreenCoordsXY screenCoords;

        if (_selected_tab == WINDOW_THEMES_TAB_SETTINGS || _selected_tab == WINDOW_THEMES_TAB_FEATURES)
            return;

        if ((colours[1] & 0x80) == 0)
            // GfxFillRect(dpi, dpi->x, dpi->y, dpi->x + dpi->width - 1, dpi->y + dpi->height - 1,
            // ColourMapA[colours[1]].mid_light);
            GfxClear(&dpi, ColourMapA[colours[1]].mid_light);
        screenCoords.y = 0;
        for (int32_t i = 0; i < GetColourSchemeTabCount(); i++)
        {
            if (screenCoords.y > dpi.y + dpi.height)
            {
                break;
            }
            if (screenCoords.y + _row_height >= dpi.y)
            {
                if (i + 1 < GetColourSchemeTabCount())
                {
                    int32_t colour = colours[1];

                    auto leftTop = ScreenCoordsXY{ 0, screenCoords.y + _row_height - 2 };
                    auto rightBottom = ScreenCoordsXY{ window_themes_widgets[WIDX_THEMES_LIST].right,
                                                       screenCoords.y + _row_height - 2 };
                    auto yPixelOffset = ScreenCoordsXY{ 0, 1 };

                    if (colour & COLOUR_FLAG_TRANSLUCENT)
                    {
                        TranslucentWindowPalette windowPalette = TranslucentWindowPalettes[BASE_COLOUR(colour)];

                        GfxFilterRect(&dpi, { leftTop, rightBottom }, windowPalette.highlight);
                        GfxFilterRect(&dpi, { leftTop + yPixelOffset, rightBottom + yPixelOffset }, windowPalette.shadow);
                    }
                    else
                    {
                        colour = ColourMapA[colours[1]].mid_dark;
                        GfxFillRect(&dpi, { leftTop, rightBottom }, colour);

                        colour = ColourMapA[colours[1]].lightest;
                        GfxFillRect(&dpi, { leftTop + yPixelOffset, rightBottom + yPixelOffset }, colour);
                    }
                }

                WindowClass wc = GetWindowClassTabIndex(i);
                int32_t numColours = ThemeDescGetNumColours(wc);
                for (uint8_t j = 0; j < numColours; j++)
                {
                    DrawTextBasic(&dpi, { 2, screenCoords.y + 4 }, ThemeDescGetName(wc), {}, { colours[1] });

                    uint8_t colour = ThemeGetColour(wc, j);
                    const bool isPressed = (i == _colour_index_1 && j == _colour_index_2);
                    auto image = ImageId(
                        isPressed ? SPR_PALETTE_BTN_PRESSED : SPR_PALETTE_BTN, colour & ~COLOUR_FLAG_TRANSLUCENT);
                    GfxDrawSprite(&dpi, image, { _button_offset_x + 12 * j, screenCoords.y + _button_offset_y });

                    ScreenCoordsXY topLeft{ _button_offset_x + 12 * j, screenCoords.y + _check_offset_y };
                    ScreenCoordsXY bottomRight{ _button_offset_x + 12 * j + 9, screenCoords.y + _check_offset_y + 10 };
                    GfxFillRectInset(&dpi, { topLeft, bottomRight }, colours[1], INSET_RECT_F_E0);
                    if (colour & COLOUR_FLAG_TRANSLUCENT)
                    {
                        GfxDrawString(
                            &dpi, topLeft, static_cast<const char*>(CheckBoxMarkString),
                            { static_cast<colour_t>(colours[1] & 0x7F), FontStyle::Medium, TextDarkness::Dark });
                    }
                }
            }

            screenCoords.y += _row_height;
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

    void WindowThemesDrawTabImages(DrawPixelInfo* dpi)
    {
        for (int32_t i = 0; i < WINDOW_THEMES_TAB_COUNT; i++)
        {
            int32_t sprite_idx = window_themes_tab_sprites[i];
            if (_selected_tab == i)
                sprite_idx += frame_no / window_themes_tab_animation_divisor[_selected_tab];
            GfxDrawSprite(
                dpi, ImageId(sprite_idx),
                windowPos
                    + ScreenCoordsXY{ widgets[WIDX_THEMES_SETTINGS_TAB + i].left, widgets[WIDX_THEMES_SETTINGS_TAB + i].top });
        }
    }
};

rct_window* WindowThemesOpen()
{
    rct_window* window;

    // Check if window is already open
    window = WindowBringToFrontByClass(WindowClass::Themes);
    if (window != nullptr)
        return window;

    window = WindowCreate<ThemesWindow>(WindowClass::Themes, 320, 107, WF_10 | WF_RESIZABLE);

    return window;
}
