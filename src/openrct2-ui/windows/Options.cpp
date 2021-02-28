/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

/**
 * To better group the options together and allow the window to be scalable with additional OpenRCT2 options,
 * the window has been changed to a tab interface similar to the options window seen in Locomotion.
 */

#include "../interface/Theme.h"

#include <algorithm>
#include <cmath>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/audio/AudioMixer.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/localisation/Currency.h>
#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Language.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/network/network.h>
#include <openrct2/platform/Platform2.h>
#include <openrct2/ride/RideAudio.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/sprites.h>
#include <openrct2/title/TitleScreen.h>
#include <openrct2/title/TitleSequenceManager.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/util/Util.h>

// clang-format off
enum WINDOW_OPTIONS_PAGE {
    WINDOW_OPTIONS_PAGE_DISPLAY,
    WINDOW_OPTIONS_PAGE_RENDERING,
    WINDOW_OPTIONS_PAGE_CULTURE,
    WINDOW_OPTIONS_PAGE_AUDIO,
    WINDOW_OPTIONS_PAGE_CONTROLS_AND_INTERFACE,
    WINDOW_OPTIONS_PAGE_MISC,
    WINDOW_OPTIONS_PAGE_ADVANCED,
    WINDOW_OPTIONS_PAGE_COUNT
};

#pragma region Widgets

enum WINDOW_OPTIONS_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PAGE_BACKGROUND,
    WIDX_TAB_1,
    WIDX_TAB_2,
    WIDX_TAB_3,
    WIDX_TAB_4,
    WIDX_TAB_5,
    WIDX_TAB_6,
    WIDX_TAB_7,

    WIDX_PAGE_START,

    // Display
    WIDX_HARDWARE_GROUP = WIDX_PAGE_START,
    WIDX_FULLSCREEN,
    WIDX_FULLSCREEN_DROPDOWN,
    WIDX_RESOLUTION,
    WIDX_RESOLUTION_DROPDOWN,
    WIDX_SCALE,
    WIDX_SCALE_UP,
    WIDX_SCALE_DOWN,
    WIDX_DRAWING_ENGINE,
    WIDX_DRAWING_ENGINE_DROPDOWN,
    WIDX_SCALE_QUALITY,
    WIDX_SCALE_QUALITY_DROPDOWN,
    WIDX_STEAM_OVERLAY_PAUSE,
    WIDX_UNCAP_FPS_CHECKBOX,
    WIDX_SHOW_FPS_CHECKBOX,
    WIDX_MULTITHREADING_CHECKBOX,
    WIDX_USE_VSYNC_CHECKBOX,
    WIDX_MINIMIZE_FOCUS_LOSS,
    WIDX_DISABLE_SCREENSAVER_LOCK,

    // Rendering
    WIDX_RENDERING_GROUP = WIDX_PAGE_START,
    WIDX_TILE_SMOOTHING_CHECKBOX,
    WIDX_GRIDLINES_CHECKBOX,
    WIDX_UPPER_CASE_BANNERS_CHECKBOX,
    WIDX_SHOW_GUEST_PURCHASES_CHECKBOX,
    WIDX_TRANSPARENT_SCREENSHOTS_CHECKBOX,
    WIDX_VIRTUAL_FLOOR_LABEL,
    WIDX_VIRTUAL_FLOOR,
    WIDX_VIRTUAL_FLOOR_DROPDOWN,
    WIDX_EFFECTS_GROUP,
    WIDX_DAY_NIGHT_CHECKBOX,
    WIDX_ENABLE_LIGHT_FX_CHECKBOX,
    WIDX_ENABLE_LIGHT_FX_FOR_VEHICLES_CHECKBOX,
    WIDX_RENDER_WEATHER_EFFECTS_CHECKBOX,
    WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX,

    // Culture / Units
    WIDX_LANGUAGE = WIDX_PAGE_START,
    WIDX_LANGUAGE_DROPDOWN,
    WIDX_CURRENCY,
    WIDX_CURRENCY_DROPDOWN,
    WIDX_DISTANCE,
    WIDX_DISTANCE_DROPDOWN,
    WIDX_TEMPERATURE,
    WIDX_TEMPERATURE_DROPDOWN,
    WIDX_HEIGHT_LABELS,
    WIDX_HEIGHT_LABELS_DROPDOWN,
    WIDX_DATE_FORMAT,
    WIDX_DATE_FORMAT_DROPDOWN,

    // Audio
    WIDX_SOUND = WIDX_PAGE_START,
    WIDX_SOUND_DROPDOWN,
    WIDX_MASTER_SOUND_CHECKBOX,
    WIDX_SOUND_CHECKBOX,
    WIDX_MUSIC_CHECKBOX,
    WIDX_AUDIO_FOCUS_CHECKBOX,
    WIDX_TITLE_MUSIC_LABEL,
    WIDX_TITLE_MUSIC,
    WIDX_TITLE_MUSIC_DROPDOWN,
    WIDX_MASTER_VOLUME,
    WIDX_SOUND_VOLUME,
    WIDX_MUSIC_VOLUME,

    // Controls and interface
    WIDX_CONTROLS_GROUP = WIDX_PAGE_START,
    WIDX_SCREEN_EDGE_SCROLLING,
    WIDX_TRAP_CURSOR,
    WIDX_INVERT_DRAG,
    WIDX_ZOOM_TO_CURSOR,
    WIDX_HOTKEY_DROPDOWN,
    WIDX_THEMES_GROUP,
    WIDX_THEMES,
    WIDX_THEMES_DROPDOWN,
    WIDX_THEMES_BUTTON,
    WIDX_TOOLBAR_BUTTONS_GROUP,
    WIDX_TOOLBAR_SHOW_FINANCES,
    WIDX_TOOLBAR_SHOW_RESEARCH,
    WIDX_TOOLBAR_SHOW_CHEATS,
    WIDX_TOOLBAR_SHOW_NEWS,
    WIDX_TOOLBAR_SHOW_MUTE,
    WIDX_TOOLBAR_SHOW_CHAT,
    WIDX_TOOLBAR_SHOW_ZOOM,

    // Misc
    WIDX_TITLE_SEQUENCE_GROUP = WIDX_PAGE_START,
    WIDX_TITLE_SEQUENCE_RANDOM,
    WIDX_TITLE_SEQUENCE,
    WIDX_TITLE_SEQUENCE_DROPDOWN,
    WIDX_TITLE_SEQUENCE_BUTTON,
    WIDX_SCENARIO_GROUP,
    WIDX_SCENARIO_GROUPING,
    WIDX_SCENARIO_GROUPING_DROPDOWN,
    WIDX_SCENARIO_UNLOCKING,
    WIDX_SCENARIO_OPTIONS_GROUP,
    WIDX_ALLOW_EARLY_COMPLETION,
    WIDX_TWEAKS_GROUP,
    WIDX_REAL_NAME_CHECKBOX,
    WIDX_AUTO_STAFF_PLACEMENT,
    WIDX_AUTO_OPEN_SHOPS,
    WIDX_DEFAULT_INSPECTION_INTERVAL,
    WIDX_DEFAULT_INSPECTION_INTERVAL_DROPDOWN,

    // Advanced
    WIDX_DEBUGGING_TOOLS = WIDX_PAGE_START,
    WIDX_ALLOW_LOADING_WITH_INCORRECT_CHECKSUM,
    WIDX_SAVE_PLUGIN_DATA_CHECKBOX,
    WIDX_STAY_CONNECTED_AFTER_DESYNC,
    WIDX_ALWAYS_NATIVE_LOADSAVE,
    WIDX_AUTOSAVE,
    WIDX_AUTOSAVE_DROPDOWN,
    WIDX_AUTOSAVE_AMOUNT,
    WIDX_AUTOSAVE_AMOUNT_UP,
    WIDX_AUTOSAVE_AMOUNT_DOWN,
    WIDX_PATH_TO_RCT1_TEXT,
    WIDX_PATH_TO_RCT1_BUTTON,
    WIDX_PATH_TO_RCT1_CLEAR,
};

static constexpr const rct_string_id WINDOW_TITLE = STR_OPTIONS_TITLE;
static constexpr const int32_t WW = 310;
static constexpr const int32_t WH = 332;

#define MAIN_OPTIONS_WIDGETS \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), \
    MakeWidget({  0, 43}, {WW, 289}, WindowWidgetType::Resize, WindowColour::Secondary), \
    MakeTab   ({  3, 17}, STR_OPTIONS_DISPLAY_TIP                       ), \
    MakeTab   ({ 34, 17}, STR_OPTIONS_RENDERING_TIP                     ), \
    MakeTab   ({ 65, 17}, STR_OPTIONS_CULTURE_TIP                       ), \
    MakeTab   ({ 96, 17}, STR_OPTIONS_AUDIO_TIP                         ), \
    MakeTab   ({127, 17}, STR_OPTIONS_CONTROLS_AND_INTERFACE_TIP        ), \
    MakeTab   ({158, 17}, STR_OPTIONS_MISCELLANEOUS_TIP                 ), \
    MakeTab   ({189, 17}, STR_OPTIONS_ADVANCED                          )

static rct_widget window_options_display_widgets[] = {
    MAIN_OPTIONS_WIDGETS,
    MakeWidget        ({  5,  53}, {300, 170}, WindowWidgetType::Groupbox, WindowColour::Secondary, STR_HARDWARE_GROUP                                                              ), // Hardware group
    MakeWidget        ({155,  68}, {145,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                                                  ), // Fullscreen
    MakeWidget        ({288,  69}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH,                    STR_FULLSCREEN_MODE_TIP                  ),
    MakeWidget        ({155,  83}, {145,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_ARG_16_RESOLUTION_X_BY_Y                                                    ), // Resolution
    MakeWidget        ({288,  84}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH,                    STR_DISPLAY_RESOLUTION_TIP               ),
    MakeSpinnerWidgets({155,  98}, {145,  12}, WindowWidgetType::Spinner,  WindowColour::Secondary, STR_NONE,                              STR_WINDOW_SCALE_TIP                     ), // Scale spinner (3 widgets)
    MakeWidget        ({155, 113}, {145,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                                                  ),
    MakeWidget        ({288, 114}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH,                    STR_DRAWING_ENGINE_TIP                   ),
    MakeWidget        ({155, 128}, {145,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                                                  ), // Scaling quality hint
    MakeWidget        ({288, 129}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH,                    STR_SCALE_QUALITY_TIP                    ),
    MakeWidget        ({ 25, 144}, {266,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_STEAM_OVERLAY_PAUSE,               STR_STEAM_OVERLAY_PAUSE_TIP              ), // Pause on steam overlay
    MakeWidget        ({ 11, 161}, {143,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_UNCAP_FPS,                         STR_UNCAP_FPS_TIP                        ), // Uncap fps
    MakeWidget        ({155, 161}, {136,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_SHOW_FPS,                          STR_SHOW_FPS_TIP                         ), // Show fps
    MakeWidget        ({155, 176}, {136,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MULTITHREADING,                    STR_MULTITHREADING_TIP                   ), // Multithreading
    MakeWidget        ({ 11, 176}, {143,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_USE_VSYNC,                         STR_USE_VSYNC_TIP                        ), // Use vsync
    MakeWidget        ({ 11, 191}, {280,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MINIMISE_FULLSCREEN_ON_FOCUS_LOSS, STR_MINIMISE_FULLSCREEN_ON_FOCUS_LOSS_TIP), // Minimise fullscreen focus loss
    MakeWidget        ({ 11, 206}, {280,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_DISABLE_SCREENSAVER,               STR_DISABLE_SCREENSAVER_TIP              ), // Disable screensaver
    { WIDGETS_END },
};

static rct_widget window_options_rendering_widgets[] = {
    MAIN_OPTIONS_WIDGETS,
#define FRAME_RENDERING_START 53
    MakeWidget({  5,  FRAME_RENDERING_START + 0}, {300, 108}, WindowWidgetType::Groupbox, WindowColour::Secondary, STR_RENDERING_GROUP                                       ), // Rendering group
    MakeWidget({ 10, FRAME_RENDERING_START + 15}, {281,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_TILE_SMOOTHING,         STR_TILE_SMOOTHING_TIP        ), // Landscape smoothing
    MakeWidget({ 10, FRAME_RENDERING_START + 30}, {281,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_GRIDLINES,              STR_GRIDLINES_TIP             ), // Gridlines
    MakeWidget({ 10, FRAME_RENDERING_START + 45}, {281,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_UPPERCASE_BANNERS,      STR_UPPERCASE_BANNERS_TIP     ), // Uppercase banners
    MakeWidget({ 10, FRAME_RENDERING_START + 60}, {281,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_SHOW_GUEST_PURCHASES,   STR_SHOW_GUEST_PURCHASES_TIP  ), // Guest purchases
    MakeWidget({ 10, FRAME_RENDERING_START + 75}, {281,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_TRANSPARENT_SCREENSHOT, STR_TRANSPARENT_SCREENSHOT_TIP), // Transparent screenshot
    MakeWidget({ 10, FRAME_RENDERING_START + 90}, {281,  12}, WindowWidgetType::Label,    WindowColour::Secondary, STR_VIRTUAL_FLOOR_STYLE                                   ), // Virtual floor
    MakeWidget({155, FRAME_RENDERING_START + 90}, {145,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_NONE,                   STR_VIRTUAL_FLOOR_STYLE_TIP   ), // Virtual floor dropdown
    MakeWidget({288, FRAME_RENDERING_START + 91}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH,         STR_VIRTUAL_FLOOR_STYLE_TIP   ), // Virtual floor dropdown
#undef FRAME_RENDERING_START
#define FRAME_EFFECTS_START 163
    MakeWidget({ 5,  FRAME_EFFECTS_START + 0}, {300, 94}, WindowWidgetType::Groupbox, WindowColour::Secondary, STR_EFFECTS_GROUP                                             ), // Rendering group
    MakeWidget({10, FRAME_EFFECTS_START + 15}, {281, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CYCLE_DAY_NIGHT,          STR_CYCLE_DAY_NIGHT_TIP         ), // Cycle day-night
    MakeWidget({25, FRAME_EFFECTS_START + 30}, {266, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_ENABLE_LIGHTING_EFFECTS,  STR_ENABLE_LIGHTING_EFFECTS_TIP ), // Enable light fx
    MakeWidget({40, FRAME_EFFECTS_START + 45}, {251, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_ENABLE_LIGHTING_VEHICLES, STR_ENABLE_LIGHTING_VEHICLES_TIP), // Enable light fx for vehicles
    MakeWidget({10, FRAME_EFFECTS_START + 60}, {281, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_RENDER_WEATHER_EFFECTS,   STR_RENDER_WEATHER_EFFECTS_TIP  ), // Render weather effects
    MakeWidget({25, FRAME_EFFECTS_START + 75}, {266, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_DISABLE_LIGHTNING_EFFECT, STR_DISABLE_LIGHTNING_EFFECT_TIP), // Disable lightning effect
#undef FRAME_EFFECTS_START
    { WIDGETS_END },
};

static rct_widget window_options_culture_widgets[] = {
    MAIN_OPTIONS_WIDGETS,
    MakeWidget({155,  53}, {145, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_STRING                                     ), // language
    MakeWidget({288,  54}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_LANGUAGE_TIP           ),
    MakeWidget({155,  68}, {145, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                 ), // Currency
    MakeWidget({288,  69}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_CURRENCY_TIP           ),
    MakeWidget({155,  83}, {145, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                 ), // Distance and speed
    MakeWidget({288,  84}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_DISTANCE_AND_SPEED_TIP ),
    MakeWidget({155,  98}, {145, 13}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                 ), // Temperature
    MakeWidget({288,  99}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_TEMPERATURE_FORMAT_TIP ),
    MakeWidget({155, 113}, {145, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                 ), // Height labels
    MakeWidget({288, 114}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_HEIGHT_LABELS_UNITS_TIP),
    MakeWidget({155, 128}, {145, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                 ), // Date format
    MakeWidget({288, 129}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_DATE_FORMAT_TIP        ),
    { WIDGETS_END },
};

static rct_widget window_options_audio_widgets[] = {
    MAIN_OPTIONS_WIDGETS,
    MakeWidget({ 10,  53}, {290, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                ), // Audio device
    MakeWidget({288,  54}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH,      STR_AUDIO_DEVICE_TIP ),
    MakeWidget({ 10,  69}, {220, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MASTER_VOLUME,       STR_MASTER_VOLUME_TIP), // Enable / disable master sound
    MakeWidget({ 10,  84}, {220, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_SOUND_EFFECTS,       STR_SOUND_EFFECTS_TIP), // Enable / disable sound effects
    MakeWidget({ 10,  99}, {220, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_RIDE_MUSIC,          STR_RIDE_MUSIC_TIP   ), // Enable / disable ride music
    MakeWidget({ 10, 113}, {290, 13}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_AUDIO_FOCUS,         STR_AUDIO_FOCUS_TIP  ), // Enable / disable audio disabled on focus lost
    MakeWidget({ 10, 128}, {145, 13}, WindowWidgetType::Label,    WindowColour::Secondary, STR_OPTIONS_MUSIC_LABEL                       ), // Title music label
    MakeWidget({155, 127}, {145, 13}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                ), // Title music
    MakeWidget({288, 128}, { 11, 11}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH,      STR_TITLE_MUSIC_TIP  ),
    MakeWidget({155,  68}, {145, 13}, WindowWidgetType::Scroll,   WindowColour::Secondary, SCROLL_HORIZONTAL                             ), // Master volume
    MakeWidget({155,  83}, {145, 13}, WindowWidgetType::Scroll,   WindowColour::Secondary, SCROLL_HORIZONTAL                             ), // Sound effect volume
    MakeWidget({155,  98}, {145, 13}, WindowWidgetType::Scroll,   WindowColour::Secondary, SCROLL_HORIZONTAL                             ), // Music volume
    { WIDGETS_END },
};

static rct_widget window_options_controls_and_interface_widgets[] = {
    MAIN_OPTIONS_WIDGETS,
#define CONTROLS_GROUP_START 53
    MakeWidget({  5,  CONTROLS_GROUP_START + 0}, {300, 92}, WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CONTROLS_GROUP                                          ), // Controls group
    MakeWidget({ 10, CONTROLS_GROUP_START + 13}, {290, 14}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_SCREEN_EDGE_SCROLLING,   STR_SCREEN_EDGE_SCROLLING_TIP  ), // Edge scrolling
    MakeWidget({ 10, CONTROLS_GROUP_START + 30}, {290, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_TRAP_MOUSE,              STR_TRAP_MOUSE_TIP             ), // Trap mouse
    MakeWidget({ 10, CONTROLS_GROUP_START + 45}, {290, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_INVERT_RIGHT_MOUSE_DRAG, STR_INVERT_RIGHT_MOUSE_DRAG_TIP), // Invert right mouse dragging
    MakeWidget({ 10, CONTROLS_GROUP_START + 60}, {290, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_ZOOM_TO_CURSOR,          STR_ZOOM_TO_CURSOR_TIP         ), // Zoom to cursor
    MakeWidget({155, CONTROLS_GROUP_START + 75}, {145, 13}, WindowWidgetType::Button,   WindowColour::Secondary, STR_HOTKEY,                  STR_HOTKEY_TIP                 ), // Set hotkeys buttons
#undef CONTROLS_GROUP_START
#define THEMES_GROUP_START 148
    MakeWidget({  5,  THEMES_GROUP_START + 0}, {300, 48}, WindowWidgetType::Groupbox, WindowColour::Secondary, STR_THEMES_GROUP                                  ), // Toolbar buttons group
    MakeWidget({155, THEMES_GROUP_START + 14}, {145, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_STRING                                        ), // Themes
    MakeWidget({288, THEMES_GROUP_START + 15}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH,     STR_CURRENT_THEME_TIP     ),
    MakeWidget({155, THEMES_GROUP_START + 30}, {145, 13}, WindowWidgetType::Button,   WindowColour::Secondary, STR_EDIT_THEMES_BUTTON, STR_EDIT_THEMES_BUTTON_TIP), // Themes button
#undef THEMES_GROUP_START
#define TOOLBAR_GROUP_START 200
    MakeWidget({  5,  TOOLBAR_GROUP_START + 0}, {300, 92}, WindowWidgetType::Groupbox, WindowColour::Secondary, STR_TOOLBAR_BUTTONS_GROUP                                                   ), // Toolbar buttons group
    MakeWidget({ 24, TOOLBAR_GROUP_START + 31}, {122, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_FINANCES_BUTTON_ON_TOOLBAR,      STR_FINANCES_BUTTON_ON_TOOLBAR_TIP     ), // Finances
    MakeWidget({ 24, TOOLBAR_GROUP_START + 46}, {122, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_RESEARCH_BUTTON_ON_TOOLBAR,      STR_RESEARCH_BUTTON_ON_TOOLBAR_TIP     ), // Research
    MakeWidget({155, TOOLBAR_GROUP_START + 31}, {145, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_CHEATS_BUTTON_ON_TOOLBAR,        STR_CHEATS_BUTTON_ON_TOOLBAR_TIP       ), // Cheats
    MakeWidget({155, TOOLBAR_GROUP_START + 46}, {145, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_SHOW_RECENT_MESSAGES_ON_TOOLBAR, STR_SHOW_RECENT_MESSAGES_ON_TOOLBAR_TIP), // Recent messages
    MakeWidget({ 24, TOOLBAR_GROUP_START + 61}, {162, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_MUTE_BUTTON_ON_TOOLBAR,          STR_MUTE_BUTTON_ON_TOOLBAR_TIP         ), // Mute
    MakeWidget({155, TOOLBAR_GROUP_START + 61}, {145, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_CHAT_BUTTON_ON_TOOLBAR,          STR_CHAT_BUTTON_ON_TOOLBAR_TIP         ), // Chat
    MakeWidget({ 24, TOOLBAR_GROUP_START + 76}, {122, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_ZOOM_BUTTON_ON_TOOLBAR,          STR_ZOOM_BUTTON_ON_TOOLBAR_TIP         ), // Zoom
    { WIDGETS_END },
#undef TOOLBAR_GROUP_START
};

static rct_widget window_options_misc_widgets[] = {
    MAIN_OPTIONS_WIDGETS,
#define TITLE_SEQUENCE_START 53
    MakeWidget({  5,  TITLE_SEQUENCE_START + 0}, {300, 65}, WindowWidgetType::Groupbox, WindowColour::Secondary, STR_OPTIONS_TITLE_SEQUENCE                                            ),
    MakeWidget({ 10, TITLE_SEQUENCE_START + 15}, {290, 16}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_OPTIONS_RANDOM_TITLE_SEQUENCE                                     ), // Random Title Sequence
    MakeWidget({135, TITLE_SEQUENCE_START + 32}, {165, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_STRING                                                            ), // Title sequence dropdown
    MakeWidget({288, TITLE_SEQUENCE_START + 33}, { 11, 12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH,                STR_TITLE_SEQUENCE_TIP             ), // Title sequence dropdown button
    MakeWidget({135, TITLE_SEQUENCE_START + 48}, {165, 13}, WindowWidgetType::Button,   WindowColour::Secondary, STR_EDIT_TITLE_SEQUENCES_BUTTON,   STR_EDIT_TITLE_SEQUENCES_BUTTON_TIP), // Edit title sequences button
#undef TITLE_SEQUENCE_START
#define SCENARIO_START 122
    MakeWidget({  5,  SCENARIO_START + 0}, {300, 51}, WindowWidgetType::Groupbox, WindowColour::Secondary, STR_OPTIONS_SCENARIO_SELECTION                            ),
    MakeWidget({175, SCENARIO_START + 15}, {125, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                            ), // Scenario select mode
    MakeWidget({288, SCENARIO_START + 16}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH,             STR_SCENARIO_GROUPING_TIP ),
    MakeWidget({ 25, SCENARIO_START + 30}, {275, 16}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_OPTIONS_SCENARIO_UNLOCKING, STR_SCENARIO_UNLOCKING_TIP), // Unlocking of scenarios
#undef SCENARIO_START
#define SCENARIO_OPTIONS_START 177
    MakeWidget({ 5,  SCENARIO_OPTIONS_START + 0}, {300, 35}, WindowWidgetType::Groupbox, WindowColour::Secondary, STR_SCENARIO_OPTIONS                                ),
    MakeWidget({10, SCENARIO_OPTIONS_START + 15}, {290, 15}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_ALLOW_EARLY_COMPLETION, STR_EARLY_COMPLETION_TIP), // Allow early scenario completion
#undef SCENARIO_OPTIONS_START
#define TWEAKS_START 216
    MakeWidget({  5,  TWEAKS_START + 0}, {300, 81}, WindowWidgetType::Groupbox, WindowColour::Secondary, STR_OPTIONS_TWEAKS                                           ),
    MakeWidget({ 10, TWEAKS_START + 15}, {290, 15}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_REAL_NAME,            STR_REAL_NAME_TIP                  ), // Show 'real' names of guests
    MakeWidget({ 10, TWEAKS_START + 30}, {290, 15}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_AUTO_STAFF_PLACEMENT, STR_AUTO_STAFF_PLACEMENT_TIP       ), // Auto staff placement
    MakeWidget({ 10, TWEAKS_START + 45}, {290, 15}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_AUTO_OPEN_SHOPS,      STR_AUTO_OPEN_SHOPS_TIP            ), // Automatically open shops & stalls
    MakeWidget({175, TWEAKS_START + 61}, {125, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                               ), // Default inspection time dropdown
    MakeWidget({288, TWEAKS_START + 62}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH,       STR_DEFAULT_INSPECTION_INTERVAL_TIP), // Default inspection time dropdown button
#undef TWEAKS_START
    { WIDGETS_END },
};

static rct_widget window_options_advanced_widgets[] = {
    MAIN_OPTIONS_WIDGETS,
    MakeWidget        ({ 10,  54}, {290, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_ENABLE_DEBUGGING_TOOLS,                STR_ENABLE_DEBUGGING_TOOLS_TIP               ), // Enable debugging tools
    MakeWidget        ({ 10,  69}, {290, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_ALLOW_LOADING_WITH_INCORRECT_CHECKSUM, STR_ALLOW_LOADING_WITH_INCORRECT_CHECKSUM_TIP), // Allow loading with incorrect checksum
    MakeWidget        ({ 10,  84}, {290, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_SAVE_PLUGIN_DATA,                      STR_SAVE_PLUGIN_DATA_TIP                     ), // Export plug-in objects with saved games
    MakeWidget        ({ 10,  99}, {290, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_STAY_CONNECTED_AFTER_DESYNC,           STR_STAY_CONNECTED_AFTER_DESYNC_TIP          ), // Do not disconnect after the client desynchronises with the server
    MakeWidget        ({ 10, 114}, {290, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_ALWAYS_NATIVE_LOADSAVE,                STR_ALWAYS_NATIVE_LOADSAVE_TIP               ), // Use native load/save window
    MakeWidget        ({165, 130}, {135, 13}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                                                          ), // Autosave dropdown
    MakeWidget        ({288, 131}, { 11, 11}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH,                        STR_AUTOSAVE_FREQUENCY_TIP                   ), // Autosave dropdown button
    MakeSpinnerWidgets({165, 150}, {135, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary, STR_NONE,                                  STR_AUTOSAVE_AMOUNT_TIP                      ), // Autosave amount spinner
    MakeWidget        ({ 23, 169}, {276, 12}, WindowWidgetType::Label,    WindowColour::Secondary, STR_PATH_TO_RCT1,                          STR_PATH_TO_RCT1_TIP                         ), // RCT 1 path text
    MakeWidget        ({ 24, 184}, {266, 14}, WindowWidgetType::Button,   WindowColour::Secondary, STR_NONE,                                  STR_STRING_TOOLTIP                           ), // RCT 1 path button
    MakeWidget        ({289, 184}, { 11, 14}, WindowWidgetType::Button,   WindowColour::Secondary, STR_CLOSE_X,                               STR_PATH_TO_RCT1_CLEAR_TIP                   ), // RCT 1 path clear button
    { WIDGETS_END },
};

static rct_widget *window_options_page_widgets[] = {
    window_options_display_widgets,
    window_options_rendering_widgets,
    window_options_culture_widgets,
    window_options_audio_widgets,
    window_options_controls_and_interface_widgets,
    window_options_misc_widgets,
    window_options_advanced_widgets,
};

#pragma endregion

static constexpr const rct_string_id window_options_autosave_names[6] = {
    STR_SAVE_EVERY_MINUTE,
    STR_SAVE_EVERY_5MINUTES,
    STR_SAVE_EVERY_15MINUTES,
    STR_SAVE_EVERY_30MINUTES,
    STR_SAVE_EVERY_HOUR,
    STR_SAVE_NEVER,
};

static constexpr const rct_string_id window_options_title_music_names[] = {
    STR_OPTIONS_MUSIC_VALUE_NONE ,
    STR_ROLLERCOASTER_TYCOON_1_DROPDOWN ,
    STR_ROLLERCOASTER_TYCOON_2_DROPDOWN ,
    STR_OPTIONS_MUSIC_VALUE_RANDOM,
};

static constexpr const rct_string_id window_options_scale_quality_names[] = {
    STR_SCALING_QUALITY_LINEAR,
    STR_SCALING_QUALITY_SMOOTH_NN
};

static constexpr const rct_string_id window_options_fullscreen_mode_names[] = {
    STR_OPTIONS_DISPLAY_WINDOWED,
    STR_OPTIONS_DISPLAY_FULLSCREEN,
    STR_OPTIONS_DISPLAY_FULLSCREEN_BORDERLESS,
};

const int32_t window_options_tab_animation_divisor[] =
{
    4, // WINDOW_OPTIONS_PAGE_DISPLAY,
    1, // WINDOW_OPTIONS_PAGE_RENDERING,
    8, // WINDOW_OPTIONS_PAGE_CULTURE,
    2, // WINDOW_OPTIONS_PAGE_AUDIO,
    2, // WINDOW_OPTIONS_PAGE_CONTROLS_AND_INTERFACE,
    4, // WINDOW_OPTIONS_PAGE_MISC,
    2, // WINDOW_OPTIONS_PAGE_ADVANCED,
};
const int32_t window_options_tab_animation_frames[] =
{
     8, // WINDOW_OPTIONS_PAGE_DISPLAY,
     1, // WINDOW_OPTIONS_PAGE_RENDERING,
     8, // WINDOW_OPTIONS_PAGE_CULTURE,
    16, // WINDOW_OPTIONS_PAGE_AUDIO,
     4, // WINDOW_OPTIONS_PAGE_CONTROLS_AND_INTERFACE,
    16, // WINDOW_OPTIONS_PAGE_MISC,
    16, // WINDOW_OPTIONS_PAGE_ADVANCED,
};

static void window_options_set_page(rct_window *w, int32_t page);
static void window_options_set_pressed_tab(rct_window *w);
static void window_options_draw_tab_image(rct_drawpixelinfo *dpi, rct_window *w, int32_t page, int32_t spriteIndex);
static void window_options_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);
static void window_options_show_dropdown(rct_window *w, rct_widget *widget, int32_t num_items);
static void window_options_update_height_markers();

#pragma region Enabled Widgets

#define MAIN_OPTIONS_ENABLED_WIDGETS \
    (1 << WIDX_CLOSE) | \
    (1 << WIDX_TAB_1) | \
    (1 << WIDX_TAB_2) | \
    (1 << WIDX_TAB_3) | \
    (1 << WIDX_TAB_4) | \
    (1 << WIDX_TAB_5) | \
    (1 << WIDX_TAB_6) | \
    (1 << WIDX_TAB_7)

static uint64_t window_options_page_enabled_widgets[] = {
    MAIN_OPTIONS_ENABLED_WIDGETS |
    (1 << WIDX_RESOLUTION) |
    (1 << WIDX_RESOLUTION_DROPDOWN) |
    (1 << WIDX_FULLSCREEN) |
    (1 << WIDX_FULLSCREEN_DROPDOWN) |
    (1 << WIDX_DRAWING_ENGINE) |
    (1 << WIDX_DRAWING_ENGINE_DROPDOWN) |
    (1 << WIDX_UNCAP_FPS_CHECKBOX) |
    (1 << WIDX_USE_VSYNC_CHECKBOX) |
    (1 << WIDX_SHOW_FPS_CHECKBOX) |
    (1 << WIDX_MULTITHREADING_CHECKBOX) |
    (1 << WIDX_MINIMIZE_FOCUS_LOSS) |
    (1 << WIDX_STEAM_OVERLAY_PAUSE) |
    (1 << WIDX_DISABLE_SCREENSAVER_LOCK) |
    (1 << WIDX_SCALE) |
    (1 << WIDX_SCALE_UP) |
    (1 << WIDX_SCALE_DOWN) |
    (1 << WIDX_SCALE_QUALITY) |
    (1 << WIDX_SCALE_QUALITY_DROPDOWN),

    MAIN_OPTIONS_ENABLED_WIDGETS |
    (1 << WIDX_TILE_SMOOTHING_CHECKBOX) |
    (1 << WIDX_GRIDLINES_CHECKBOX) |
    (1 << WIDX_UPPER_CASE_BANNERS_CHECKBOX) |
    (1 << WIDX_SHOW_GUEST_PURCHASES_CHECKBOX) |
    (1 << WIDX_TRANSPARENT_SCREENSHOTS_CHECKBOX) |
    (1 << WIDX_VIRTUAL_FLOOR) |
    (1 << WIDX_VIRTUAL_FLOOR_DROPDOWN) |
    (1 << WIDX_DAY_NIGHT_CHECKBOX) |
    (1 << WIDX_ENABLE_LIGHT_FX_CHECKBOX) |
    (1 << WIDX_ENABLE_LIGHT_FX_FOR_VEHICLES_CHECKBOX) |
    (1 << WIDX_RENDER_WEATHER_EFFECTS_CHECKBOX) |
    (1 << WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX),

    MAIN_OPTIONS_ENABLED_WIDGETS |
    (1 << WIDX_LANGUAGE) |
    (1 << WIDX_LANGUAGE_DROPDOWN) |
    (1 << WIDX_CURRENCY) |
    (1 << WIDX_CURRENCY_DROPDOWN) |
    (1 << WIDX_DISTANCE) |
    (1 << WIDX_DISTANCE_DROPDOWN) |
    (1 << WIDX_TEMPERATURE) |
    (1 << WIDX_TEMPERATURE_DROPDOWN) |
    (1 << WIDX_HEIGHT_LABELS) |
    (1 << WIDX_HEIGHT_LABELS_DROPDOWN) |
    (1 << WIDX_DATE_FORMAT) |
    (1 << WIDX_DATE_FORMAT_DROPDOWN),

    MAIN_OPTIONS_ENABLED_WIDGETS |
    (1 << WIDX_SOUND) |
    (1 << WIDX_SOUND_DROPDOWN) |
    (1 << WIDX_MASTER_SOUND_CHECKBOX) |
    (1 << WIDX_SOUND_CHECKBOX) |
    (1 << WIDX_MUSIC_CHECKBOX) |
    (1 << WIDX_AUDIO_FOCUS_CHECKBOX) |
    (1 << WIDX_TITLE_MUSIC) |
    (1 << WIDX_TITLE_MUSIC_DROPDOWN),

    MAIN_OPTIONS_ENABLED_WIDGETS |
    (1 << WIDX_SCREEN_EDGE_SCROLLING) |
    (1 << WIDX_TRAP_CURSOR) |
    (1 << WIDX_INVERT_DRAG) |
    (1 << WIDX_ZOOM_TO_CURSOR) |
    (1 << WIDX_HOTKEY_DROPDOWN) |
    (1 << WIDX_TOOLBAR_SHOW_FINANCES) |
    (1 << WIDX_TOOLBAR_SHOW_RESEARCH) |
    (1 << WIDX_TOOLBAR_SHOW_CHEATS) |
    (1 << WIDX_TOOLBAR_SHOW_NEWS) |
    (1 << WIDX_THEMES) |
    (1 << WIDX_THEMES_DROPDOWN) |
    (1 << WIDX_THEMES_BUTTON) |
    (1 << WIDX_TOOLBAR_SHOW_MUTE) |
    (1 << WIDX_TOOLBAR_SHOW_CHAT) |
    (1 << WIDX_TOOLBAR_SHOW_ZOOM),

    MAIN_OPTIONS_ENABLED_WIDGETS |
    (1 << WIDX_REAL_NAME_CHECKBOX) |
    (1 << WIDX_AUTO_STAFF_PLACEMENT) |
    (1 << WIDX_TITLE_SEQUENCE) |
    (1 << WIDX_TITLE_SEQUENCE_DROPDOWN) |
    (1 << WIDX_TITLE_SEQUENCE_BUTTON) |
    (1 << WIDX_TITLE_SEQUENCE_RANDOM) |
    (1 << WIDX_SCENARIO_GROUPING) |
    (1 << WIDX_SCENARIO_GROUPING_DROPDOWN) |
    (1 << WIDX_SCENARIO_UNLOCKING) |
    (1 << WIDX_ALLOW_EARLY_COMPLETION) |
    (1 << WIDX_AUTO_OPEN_SHOPS) |
    (1 << WIDX_DEFAULT_INSPECTION_INTERVAL) |
    (1 << WIDX_DEFAULT_INSPECTION_INTERVAL_DROPDOWN),

    MAIN_OPTIONS_ENABLED_WIDGETS |
    (1 << WIDX_DEBUGGING_TOOLS) |
    (1 << WIDX_ALLOW_LOADING_WITH_INCORRECT_CHECKSUM) |
    (1 << WIDX_SAVE_PLUGIN_DATA_CHECKBOX) |
    (1 << WIDX_STAY_CONNECTED_AFTER_DESYNC) |
    (1 << WIDX_ALWAYS_NATIVE_LOADSAVE) |
    (1 << WIDX_AUTOSAVE) |
    (1 << WIDX_AUTOSAVE_DROPDOWN) |
    (1 << WIDX_AUTOSAVE_AMOUNT) |
    (1 << WIDX_AUTOSAVE_AMOUNT_UP) |
    (1 << WIDX_AUTOSAVE_AMOUNT_DOWN) |
    (1 << WIDX_PATH_TO_RCT1_TEXT) |
    (1 << WIDX_PATH_TO_RCT1_BUTTON) |
    (1 << WIDX_PATH_TO_RCT1_CLEAR),
};
// clang-format on

#pragma region Common tab events

static void window_options_common_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            return;
        case WIDX_TAB_1:
        case WIDX_TAB_2:
        case WIDX_TAB_3:
        case WIDX_TAB_4:
        case WIDX_TAB_5:
        case WIDX_TAB_6:
        case WIDX_TAB_7:
            window_options_set_page(w, widgetIndex - WIDX_TAB_1);
            break;
    }
}

static void window_options_common_invalidate_before(rct_window* w)
{
    if (window_options_page_widgets[w->page] != w->widgets)
    {
        w->widgets = window_options_page_widgets[w->page];
        WindowInitScrollWidgets(w);
    }
    window_options_set_pressed_tab(w);

    w->disabled_widgets = 0;
    auto hasFilePicker = OpenRCT2::GetContext()->GetUiContext()->HasFilePicker();
    if (!hasFilePicker)
    {
        w->enabled_widgets &= ~WIDX_ALWAYS_NATIVE_LOADSAVE;
        w->disabled_widgets |= WIDX_ALWAYS_NATIVE_LOADSAVE;
        w->widgets[WIDX_ALWAYS_NATIVE_LOADSAVE].type = WindowWidgetType::Empty;
    }
}

static void window_options_common_invalidate_after(rct_window* w)
{
    // Automatically adjust window height to fit widgets
    int32_t y = 0;
    for (auto widget = &w->widgets[WIDX_PAGE_START]; widget->type != WindowWidgetType::Last; widget++)
    {
        y = std::max<int32_t>(y, widget->bottom);
    }
    w->height = y + 6;
    w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;
}

static void window_options_common_update(rct_window* w)
{
    // Tab animation
    w->frame_no++;
    widget_invalidate(w, WIDX_TAB_1 + w->page);
}

#pragma region Display Tab

static void window_options_display_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    if (widgetIndex < WIDX_PAGE_START)
        return window_options_common_mouseup(w, widgetIndex);

    switch (widgetIndex)
    {
        case WIDX_UNCAP_FPS_CHECKBOX:
            gConfigGeneral.uncap_fps ^= 1;
            drawing_engine_set_vsync(gConfigGeneral.use_vsync);
            config_save_default();
            w->Invalidate();
            break;
        case WIDX_USE_VSYNC_CHECKBOX:
            gConfigGeneral.use_vsync ^= 1;
            drawing_engine_set_vsync(gConfigGeneral.use_vsync);
            config_save_default();
            w->Invalidate();
            break;
        case WIDX_SHOW_FPS_CHECKBOX:
            gConfigGeneral.show_fps ^= 1;
            config_save_default();
            w->Invalidate();
            break;
        case WIDX_MULTITHREADING_CHECKBOX:
            gConfigGeneral.multithreading ^= 1;
            config_save_default();
            w->Invalidate();
            break;
        case WIDX_MINIMIZE_FOCUS_LOSS:
            gConfigGeneral.minimize_fullscreen_focus_loss ^= 1;
            platform_refresh_video(false);
            config_save_default();
            w->Invalidate();
            break;
        case WIDX_STEAM_OVERLAY_PAUSE:
            gConfigGeneral.steam_overlay_pause ^= 1;
            config_save_default();
            w->Invalidate();
            break;
        case WIDX_DISABLE_SCREENSAVER_LOCK:
            gConfigGeneral.disable_screensaver ^= 1;
            ApplyScreenSaverLockSetting();
            config_save_default();
            w->Invalidate();
            break;
    }
}

static void window_options_display_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    widget = &w->widgets[widgetIndex - 1];

    switch (widgetIndex)
    {
        case WIDX_RESOLUTION_DROPDOWN:
        {
            const auto& resolutions = OpenRCT2::GetContext()->GetUiContext()->GetFullscreenResolutions();

            int32_t selectedResolution = -1;
            for (size_t i = 0; i < resolutions.size(); i++)
            {
                const Resolution& resolution = resolutions[i];

                gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;

                uint16_t* args = reinterpret_cast<uint16_t*>(&gDropdownItemsArgs[i]);
                args[0] = STR_RESOLUTION_X_BY_Y;
                args[1] = resolution.Width;
                args[2] = resolution.Height;

                if (resolution.Width == gConfigGeneral.fullscreen_width
                    && resolution.Height == gConfigGeneral.fullscreen_height)
                {
                    selectedResolution = static_cast<int32_t>(i);
                }
            }

            window_options_show_dropdown(w, widget, static_cast<int32_t>(resolutions.size()));

            if (selectedResolution != -1 && selectedResolution < 32)
            {
                Dropdown::SetChecked(selectedResolution, true);
            }
        }

        break;
        case WIDX_FULLSCREEN_DROPDOWN:
            gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsFormat[1] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsFormat[2] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsArgs[0] = STR_OPTIONS_DISPLAY_WINDOWED;
            gDropdownItemsArgs[1] = STR_OPTIONS_DISPLAY_FULLSCREEN;
            gDropdownItemsArgs[2] = STR_OPTIONS_DISPLAY_FULLSCREEN_BORDERLESS;

            window_options_show_dropdown(w, widget, 3);

            Dropdown::SetChecked(gConfigGeneral.fullscreen_mode, true);
            break;
        case WIDX_DRAWING_ENGINE_DROPDOWN:
        {
            int32_t numItems = 3;
#ifdef DISABLE_OPENGL
            numItems = 2;
#endif

            for (int32_t i = 0; i < numItems; i++)
            {
                gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
                gDropdownItemsArgs[i] = DrawingEngineStringIds[i];
            }
            window_options_show_dropdown(w, widget, numItems);
            Dropdown::SetChecked(EnumValue(gConfigGeneral.drawing_engine), true);
            break;
        }
        case WIDX_SCALE_UP:
            gConfigGeneral.window_scale += 0.25f;
            config_save_default();
            gfx_invalidate_screen();
            context_trigger_resize();
            context_update_cursor_scale();
            break;
        case WIDX_SCALE_DOWN:
            gConfigGeneral.window_scale -= 0.25f;
            gConfigGeneral.window_scale = std::max(0.5f, gConfigGeneral.window_scale);
            config_save_default();
            gfx_invalidate_screen();
            context_trigger_resize();
            context_update_cursor_scale();
            break;
        case WIDX_SCALE_QUALITY_DROPDOWN:
            gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsFormat[1] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsArgs[0] = STR_SCALING_QUALITY_LINEAR;
            gDropdownItemsArgs[1] = STR_SCALING_QUALITY_SMOOTH_NN;

            window_options_show_dropdown(w, widget, 2);

            // Note: offset by one to compensate for lack of NN option.
            Dropdown::SetChecked(static_cast<int32_t>(gConfigGeneral.scale_quality) - 1, true);
            break;
    }
}

static void window_options_display_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
        return;

    switch (widgetIndex)
    {
        case WIDX_RESOLUTION_DROPDOWN:
        {
            const auto& resolutions = OpenRCT2::GetContext()->GetUiContext()->GetFullscreenResolutions();

            const Resolution& resolution = resolutions[dropdownIndex];
            if (resolution.Width != gConfigGeneral.fullscreen_width || resolution.Height != gConfigGeneral.fullscreen_height)
            {
                gConfigGeneral.fullscreen_width = resolution.Width;
                gConfigGeneral.fullscreen_height = resolution.Height;

                if (gConfigGeneral.fullscreen_mode == static_cast<int32_t>(OpenRCT2::Ui::FULLSCREEN_MODE::FULLSCREEN))
                    context_set_fullscreen_mode(static_cast<int32_t>(OpenRCT2::Ui::FULLSCREEN_MODE::FULLSCREEN));

                config_save_default();
                gfx_invalidate_screen();
            }
        }
        break;
        case WIDX_FULLSCREEN_DROPDOWN:
            if (dropdownIndex != gConfigGeneral.fullscreen_mode)
            {
                context_set_fullscreen_mode(dropdownIndex);

                gConfigGeneral.fullscreen_mode = static_cast<uint8_t>(dropdownIndex);
                config_save_default();
                gfx_invalidate_screen();
            }
            break;
        case WIDX_DRAWING_ENGINE_DROPDOWN:
            if (dropdownIndex != EnumValue(gConfigGeneral.drawing_engine))
            {
                DrawingEngine srcEngine = drawing_engine_get_type();
                DrawingEngine dstEngine = static_cast<DrawingEngine>(dropdownIndex);

                gConfigGeneral.drawing_engine = dstEngine;
                bool recreate_window = drawing_engine_requires_new_window(srcEngine, dstEngine);
                platform_refresh_video(recreate_window);
                config_save_default();
                w->Invalidate();
            }
            break;
        case WIDX_SCALE_QUALITY_DROPDOWN:
            // Note: offset by one to compensate for lack of NN option.
            if (static_cast<ScaleQuality>(dropdownIndex + 1) != gConfigGeneral.scale_quality)
            {
                gConfigGeneral.scale_quality = static_cast<ScaleQuality>(dropdownIndex + 1);
                config_save_default();
                gfx_invalidate_screen();
                context_trigger_resize();
            }
            break;
    }
}

static void window_options_display_invalidate(rct_window* w)
{
    window_options_common_invalidate_before(w);

    // Resolution dropdown caption.
    auto ft = Formatter::Common();
    ft.Increment(16);
    ft.Add<uint16_t>(static_cast<uint16_t>(gConfigGeneral.fullscreen_width));
    ft.Add<uint16_t>(static_cast<uint16_t>(gConfigGeneral.fullscreen_height));

    // Disable resolution dropdown on "Windowed" and "Fullscreen (desktop)"
    if (gConfigGeneral.fullscreen_mode != static_cast<int32_t>(OpenRCT2::Ui::FULLSCREEN_MODE::FULLSCREEN))
    {
        w->disabled_widgets |= (1 << WIDX_RESOLUTION_DROPDOWN);
        w->disabled_widgets |= (1 << WIDX_RESOLUTION);
    }
    else
    {
        w->disabled_widgets &= ~(1 << WIDX_RESOLUTION_DROPDOWN);
        w->disabled_widgets &= ~(1 << WIDX_RESOLUTION);
    }

    // Disable Steam Overlay checkbox when using software rendering.
    if (gConfigGeneral.drawing_engine == DrawingEngine::Software)
    {
        w->disabled_widgets |= (1 << WIDX_STEAM_OVERLAY_PAUSE);
    }
    else
    {
        w->disabled_widgets &= ~(1 << WIDX_STEAM_OVERLAY_PAUSE);
    }

    // Disable scaling quality dropdown when using software rendering or when using an integer scalar.
    // In the latter case, nearest neighbour rendering will be used to scale.
    if (gConfigGeneral.drawing_engine == DrawingEngine::Software
        || gConfigGeneral.window_scale == std::floor(gConfigGeneral.window_scale))
    {
        w->disabled_widgets |= (1 << WIDX_SCALE_QUALITY);
        w->disabled_widgets |= (1 << WIDX_SCALE_QUALITY_DROPDOWN);
    }
    else
    {
        w->disabled_widgets &= ~(1 << WIDX_SCALE_QUALITY);
        w->disabled_widgets &= ~(1 << WIDX_SCALE_QUALITY_DROPDOWN);
    }

    // Disable changing VSync for Software engine, as we can't control its use of VSync
    if (gConfigGeneral.drawing_engine == DrawingEngine::Software)
    {
        w->disabled_widgets |= (1 << WIDX_USE_VSYNC_CHECKBOX);
    }
    else
    {
        w->disabled_widgets &= ~(1 << WIDX_USE_VSYNC_CHECKBOX);
    }

    WidgetSetCheckboxValue(w, WIDX_UNCAP_FPS_CHECKBOX, gConfigGeneral.uncap_fps);
    WidgetSetCheckboxValue(w, WIDX_USE_VSYNC_CHECKBOX, gConfigGeneral.use_vsync);
    WidgetSetCheckboxValue(w, WIDX_SHOW_FPS_CHECKBOX, gConfigGeneral.show_fps);
    WidgetSetCheckboxValue(w, WIDX_MULTITHREADING_CHECKBOX, gConfigGeneral.multithreading);
    WidgetSetCheckboxValue(w, WIDX_MINIMIZE_FOCUS_LOSS, gConfigGeneral.minimize_fullscreen_focus_loss);
    WidgetSetCheckboxValue(w, WIDX_STEAM_OVERLAY_PAUSE, gConfigGeneral.steam_overlay_pause);
    WidgetSetCheckboxValue(w, WIDX_DISABLE_SCREENSAVER_LOCK, gConfigGeneral.disable_screensaver);

    // Dropdown captions for straightforward strings.
    window_options_display_widgets[WIDX_FULLSCREEN].text = window_options_fullscreen_mode_names[gConfigGeneral.fullscreen_mode];
    window_options_display_widgets[WIDX_DRAWING_ENGINE].text = DrawingEngineStringIds[EnumValue(gConfigGeneral.drawing_engine)];
    window_options_display_widgets[WIDX_SCALE_QUALITY].text = window_options_scale_quality_names
        [static_cast<int32_t>(gConfigGeneral.scale_quality) - 1];

    window_options_common_invalidate_after(w);
}

static void window_options_display_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    window_options_draw_tab_images(dpi, w);

    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 10, window_options_display_widgets[WIDX_FULLSCREEN].top + 1 }, STR_FULLSCREEN_MODE,
        w, { w->colours[1] });

    // Disable resolution dropdown on "Windowed" and "Fullscreen (desktop)"
    colour_t colour = w->colours[1];
    if (gConfigGeneral.fullscreen_mode != static_cast<int32_t>(OpenRCT2::Ui::FULLSCREEN_MODE::FULLSCREEN))
    {
        colour |= COLOUR_FLAG_INSET;
    }
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 10 + 15, window_options_display_widgets[WIDX_RESOLUTION].top + 1 },
        STR_DISPLAY_RESOLUTION, w, { colour });

    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 10, window_options_display_widgets[WIDX_SCALE].top + 1 }, STR_UI_SCALING_DESC, w,
        { w->colours[1] });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 10, window_options_display_widgets[WIDX_DRAWING_ENGINE].top + 1 },
        STR_DRAWING_ENGINE, w, { w->colours[1] });

    int32_t scale = static_cast<int32_t>(gConfigGeneral.window_scale * 100);
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_SCALE].left + 1, w->widgets[WIDX_SCALE].top + 1 },
        STR_WINDOW_OBJECTIVE_VALUE_RATING, &scale, { w->colours[1] });

    colour = w->colours[1];
    if (gConfigGeneral.drawing_engine == DrawingEngine::Software
        || gConfigGeneral.window_scale == std::floor(gConfigGeneral.window_scale))
    {
        colour |= COLOUR_FLAG_INSET;
    }
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 25, window_options_display_widgets[WIDX_SCALE_QUALITY].top + 1 },
        STR_SCALING_QUALITY, w, { colour });
}

#pragma region Rendering Tab

static void window_options_rendering_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    if (widgetIndex < WIDX_PAGE_START)
        return window_options_common_mouseup(w, widgetIndex);

    switch (widgetIndex)
    {
        case WIDX_TILE_SMOOTHING_CHECKBOX:
            gConfigGeneral.landscape_smoothing ^= 1;
            config_save_default();
            gfx_invalidate_screen();
            break;
        case WIDX_GRIDLINES_CHECKBOX:
            gConfigGeneral.always_show_gridlines ^= 1;
            config_save_default();
            gfx_invalidate_screen();
            if ((w = window_get_main()) != nullptr)
            {
                if (gConfigGeneral.always_show_gridlines)
                    w->viewport->flags |= VIEWPORT_FLAG_GRIDLINES;
                else
                    w->viewport->flags &= ~VIEWPORT_FLAG_GRIDLINES;
            }
            break;
        case WIDX_DAY_NIGHT_CHECKBOX:
            gConfigGeneral.day_night_cycle ^= 1;
            config_save_default();
            w->Invalidate();
            break;
        case WIDX_ENABLE_LIGHT_FX_CHECKBOX:
            gConfigGeneral.enable_light_fx ^= 1;
            config_save_default();
            w->Invalidate();
            break;
        case WIDX_ENABLE_LIGHT_FX_FOR_VEHICLES_CHECKBOX:
            gConfigGeneral.enable_light_fx_for_vehicles ^= 1;
            config_save_default();
            w->Invalidate();
            break;
        case WIDX_UPPER_CASE_BANNERS_CHECKBOX:
            gConfigGeneral.upper_case_banners ^= 1;
            config_save_default();
            w->Invalidate();
            break;
        case WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX:
            gConfigGeneral.disable_lightning_effect ^= 1;
            config_save_default();
            w->Invalidate();
            break;
        case WIDX_RENDER_WEATHER_EFFECTS_CHECKBOX:
            gConfigGeneral.render_weather_effects ^= 1;
            gConfigGeneral.render_weather_gloom = gConfigGeneral.render_weather_effects;
            config_save_default();
            w->Invalidate();
            gfx_invalidate_screen();
            break;
        case WIDX_SHOW_GUEST_PURCHASES_CHECKBOX:
            gConfigGeneral.show_guest_purchases ^= 1;
            config_save_default();
            w->Invalidate();
            break;
        case WIDX_TRANSPARENT_SCREENSHOTS_CHECKBOX:
            gConfigGeneral.transparent_screenshot ^= 1;
            config_save_default();
            w->Invalidate();
            break;
    }
}

static void window_options_rendering_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    widget = &w->widgets[widgetIndex - 1];

    switch (widgetIndex)
    {
        case WIDX_VIRTUAL_FLOOR_DROPDOWN:
            gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsFormat[1] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsFormat[2] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsArgs[0] = STR_VIRTUAL_FLOOR_STYLE_DISABLED;
            gDropdownItemsArgs[1] = STR_VIRTUAL_FLOOR_STYLE_TRANSPARENT;
            gDropdownItemsArgs[2] = STR_VIRTUAL_FLOOR_STYLE_GLASSY;

            widget = &w->widgets[widgetIndex - 1];
            window_options_show_dropdown(w, widget, 3);

            Dropdown::SetChecked(static_cast<int32_t>(gConfigGeneral.virtual_floor_style), true);
            break;
    }
}

static void window_options_rendering_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
        return;

    switch (widgetIndex)
    {
        case WIDX_VIRTUAL_FLOOR_DROPDOWN:
            gConfigGeneral.virtual_floor_style = static_cast<VirtualFloorStyles>(dropdownIndex);
            config_save_default();
            break;
    }
}

static void window_options_rendering_invalidate(rct_window* w)
{
    window_options_common_invalidate_before(w);

    WidgetSetCheckboxValue(w, WIDX_TILE_SMOOTHING_CHECKBOX, gConfigGeneral.landscape_smoothing);
    WidgetSetCheckboxValue(w, WIDX_GRIDLINES_CHECKBOX, gConfigGeneral.always_show_gridlines);
    WidgetSetCheckboxValue(w, WIDX_DAY_NIGHT_CHECKBOX, gConfigGeneral.day_night_cycle);
    WidgetSetCheckboxValue(w, WIDX_SHOW_GUEST_PURCHASES_CHECKBOX, gConfigGeneral.show_guest_purchases);
    WidgetSetCheckboxValue(w, WIDX_TRANSPARENT_SCREENSHOTS_CHECKBOX, gConfigGeneral.transparent_screenshot);
    WidgetSetCheckboxValue(w, WIDX_UPPER_CASE_BANNERS_CHECKBOX, gConfigGeneral.upper_case_banners);

    rct_string_id VirtualFloorStyleStrings[] = { STR_VIRTUAL_FLOOR_STYLE_DISABLED, STR_VIRTUAL_FLOOR_STYLE_TRANSPARENT,
                                                 STR_VIRTUAL_FLOOR_STYLE_GLASSY };

    window_options_rendering_widgets[WIDX_VIRTUAL_FLOOR].text = VirtualFloorStyleStrings[static_cast<int32_t>(
        gConfigGeneral.virtual_floor_style)];

    WidgetSetCheckboxValue(w, WIDX_ENABLE_LIGHT_FX_CHECKBOX, gConfigGeneral.enable_light_fx);
    if (gConfigGeneral.day_night_cycle && gConfigGeneral.drawing_engine == DrawingEngine::SoftwareWithHardwareDisplay)
    {
        w->disabled_widgets &= ~(1 << WIDX_ENABLE_LIGHT_FX_CHECKBOX);
    }
    else
    {
        w->disabled_widgets |= (1 << WIDX_ENABLE_LIGHT_FX_CHECKBOX);
        gConfigGeneral.enable_light_fx = false;
    }

    WidgetSetCheckboxValue(w, WIDX_ENABLE_LIGHT_FX_FOR_VEHICLES_CHECKBOX, gConfigGeneral.enable_light_fx_for_vehicles);
    if (gConfigGeneral.day_night_cycle && gConfigGeneral.drawing_engine == DrawingEngine::SoftwareWithHardwareDisplay
        && gConfigGeneral.enable_light_fx)
    {
        w->disabled_widgets &= ~(1 << WIDX_ENABLE_LIGHT_FX_FOR_VEHICLES_CHECKBOX);
    }
    else
    {
        w->disabled_widgets |= (1 << WIDX_ENABLE_LIGHT_FX_FOR_VEHICLES_CHECKBOX);
        gConfigGeneral.enable_light_fx_for_vehicles = false;
    }

    WidgetSetCheckboxValue(
        w, WIDX_RENDER_WEATHER_EFFECTS_CHECKBOX, gConfigGeneral.render_weather_effects || gConfigGeneral.render_weather_gloom);
    WidgetSetCheckboxValue(w, WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX, gConfigGeneral.disable_lightning_effect);
    if (!gConfigGeneral.render_weather_effects && !gConfigGeneral.render_weather_gloom)
    {
        WidgetSetCheckboxValue(w, WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX, true);
        w->enabled_widgets &= ~(1 << WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX);
        w->disabled_widgets |= (1 << WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX);
    }
    else
    {
        w->enabled_widgets |= (1 << WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX);
        w->disabled_widgets &= ~(1 << WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX);
    }

    window_options_common_invalidate_after(w);
}

static void window_options_rendering_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    window_options_draw_tab_images(dpi, w);
}

#pragma region Culture Tab

static void window_options_culture_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    if (widgetIndex < WIDX_PAGE_START)
        return window_options_common_mouseup(w, widgetIndex);
}

static void window_options_culture_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    widget = &w->widgets[widgetIndex - 1];

    switch (widgetIndex)
    {
        case WIDX_HEIGHT_LABELS_DROPDOWN:
            gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsFormat[1] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsArgs[0] = STR_HEIGHT_IN_UNITS;
            gDropdownItemsArgs[1] = STR_REAL_VALUES;

            window_options_show_dropdown(w, widget, 2);

            Dropdown::SetChecked(gConfigGeneral.show_height_as_units ? 0 : 1, true);
            break;
        case WIDX_CURRENCY_DROPDOWN:
        {
            uint32_t num_items = EnumValue(CurrencyType::Count) + 1;             // All the currencies plus the separator
            size_t num_ordinary_currencies = EnumValue(CurrencyType::Count) - 1; // All the currencies except custom currency

            for (size_t i = 0; i < num_ordinary_currencies; i++)
            {
                gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
                gDropdownItemsArgs[i] = CurrencyDescriptors[i].stringId;
            }

            gDropdownItemsFormat[num_ordinary_currencies] = Dropdown::SeparatorString;

            gDropdownItemsFormat[num_ordinary_currencies + 1] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsArgs[num_ordinary_currencies + 1] = CurrencyDescriptors[EnumValue(CurrencyType::Custom)].stringId;

            window_options_show_dropdown(w, widget, num_items);

            if (gConfigGeneral.currency_format == CurrencyType::Custom)
            {
                Dropdown::SetChecked(EnumValue(gConfigGeneral.currency_format) + 1, true);
            }
            else
            {
                Dropdown::SetChecked(EnumValue(gConfigGeneral.currency_format), true);
            }
            break;
        }
        case WIDX_DISTANCE_DROPDOWN:
            gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsFormat[1] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsFormat[2] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsArgs[0] = STR_IMPERIAL;
            gDropdownItemsArgs[1] = STR_METRIC;
            gDropdownItemsArgs[2] = STR_SI;

            window_options_show_dropdown(w, widget, 3);

            Dropdown::SetChecked(static_cast<int32_t>(gConfigGeneral.measurement_format), true);
            break;
        case WIDX_TEMPERATURE_DROPDOWN:
            gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsFormat[1] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsArgs[0] = STR_CELSIUS;
            gDropdownItemsArgs[1] = STR_FAHRENHEIT;

            window_options_show_dropdown(w, widget, 2);

            Dropdown::SetChecked(static_cast<int32_t>(gConfigGeneral.temperature_format), true);
            break;
        case WIDX_LANGUAGE_DROPDOWN:
            for (size_t i = 1; i < LANGUAGE_COUNT; i++)
            {
                gDropdownItemsFormat[i - 1] = STR_OPTIONS_DROPDOWN_ITEM;
                gDropdownItemsArgs[i - 1] = reinterpret_cast<uintptr_t>(LanguagesDescriptors[i].native_name);
            }
            window_options_show_dropdown(w, widget, LANGUAGE_COUNT - 1);
            Dropdown::SetChecked(LocalisationService_GetCurrentLanguage() - 1, true);
            break;
        case WIDX_DATE_FORMAT_DROPDOWN:
            for (size_t i = 0; i < 4; i++)
            {
                gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
                gDropdownItemsArgs[i] = DateFormatStringIds[i];
            }
            window_options_show_dropdown(w, widget, 4);
            Dropdown::SetChecked(gConfigGeneral.date_format, true);
            break;
    }
}

static void window_options_culture_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
        return;

    switch (widgetIndex)
    {
        case WIDX_HEIGHT_LABELS_DROPDOWN:
            // reset flag and set it to 1 if height as units is selected
            gConfigGeneral.show_height_as_units = 0;

            if (dropdownIndex == 0)
            {
                gConfigGeneral.show_height_as_units = 1;
            }
            config_save_default();
            window_options_update_height_markers();
            break;
        case WIDX_CURRENCY_DROPDOWN:
            if (dropdownIndex == EnumValue(CurrencyType::Custom) + 1)
            { // Add 1 because the separator occupies a position
                gConfigGeneral.currency_format = static_cast<CurrencyType>(dropdownIndex - 1);
                context_open_window(WC_CUSTOM_CURRENCY_CONFIG);
            }
            else
            {
                gConfigGeneral.currency_format = static_cast<CurrencyType>(dropdownIndex);
            }
            config_save_default();
            gfx_invalidate_screen();
            break;
        case WIDX_DISTANCE_DROPDOWN:
            gConfigGeneral.measurement_format = static_cast<MeasurementFormat>(dropdownIndex);
            config_save_default();
            window_options_update_height_markers();
            break;
        case WIDX_TEMPERATURE_DROPDOWN:
            if (dropdownIndex != static_cast<int32_t>(gConfigGeneral.temperature_format))
            {
                gConfigGeneral.temperature_format = static_cast<TemperatureUnit>(dropdownIndex);
                config_save_default();
                gfx_invalidate_screen();
            }
            break;
        case WIDX_LANGUAGE_DROPDOWN:
        {
            auto fallbackLanguage = LocalisationService_GetCurrentLanguage();
            if (dropdownIndex != LocalisationService_GetCurrentLanguage() - 1)
            {
                if (!language_open(dropdownIndex + 1))
                {
                    // Failed to open language file, try to recover by falling
                    // back to previously used language
                    if (language_open(fallbackLanguage))
                    {
                        // It worked, so we can say it with error message in-game
                        context_show_error(STR_LANGUAGE_LOAD_FAILED, STR_NONE, {});
                    }
                    // report error to console regardless
                    log_error("Failed to open language file.");
                    dropdownIndex = fallbackLanguage - 1;
                }
                else
                {
                    gConfigGeneral.language = dropdownIndex + 1;
                    config_save_default();
                    gfx_invalidate_screen();
                }
            }
        }
        break;
        case WIDX_DATE_FORMAT_DROPDOWN:
            if (dropdownIndex != gConfigGeneral.date_format)
            {
                gConfigGeneral.date_format = static_cast<uint8_t>(dropdownIndex);
                config_save_default();
                gfx_invalidate_screen();
            }
            break;
    }
}

static void window_options_culture_invalidate(rct_window* w)
{
    window_options_common_invalidate_before(w);

    // Language
    auto ft = Formatter::Common();
    ft.Add<char*>(LanguagesDescriptors[LocalisationService_GetCurrentLanguage()].native_name);

    // Currency: pounds, dollars, etc. (10 total)
    window_options_culture_widgets[WIDX_CURRENCY].text = CurrencyDescriptors[EnumValue(gConfigGeneral.currency_format)]
                                                             .stringId;

    // Distance: metric / imperial / si
    {
        rct_string_id stringId = STR_NONE;
        switch (gConfigGeneral.measurement_format)
        {
            case MeasurementFormat::Imperial:
                stringId = STR_IMPERIAL;
                break;
            case MeasurementFormat::Metric:
                stringId = STR_METRIC;
                break;
            case MeasurementFormat::SI:
                stringId = STR_SI;
                break;
        }
        window_options_culture_widgets[WIDX_DISTANCE].text = stringId;
    }

    // Date format
    window_options_culture_widgets[WIDX_DATE_FORMAT].text = DateFormatStringIds[gConfigGeneral.date_format];

    // Temperature: celsius/fahrenheit
    window_options_culture_widgets[WIDX_TEMPERATURE].text = gConfigGeneral.temperature_format == TemperatureUnit::Fahrenheit
        ? STR_FAHRENHEIT
        : STR_CELSIUS;

    // Height: units/real values
    window_options_culture_widgets[WIDX_HEIGHT_LABELS].text = gConfigGeneral.show_height_as_units ? STR_HEIGHT_IN_UNITS
                                                                                                  : STR_REAL_VALUES;

    window_options_common_invalidate_after(w);
}

static void window_options_culture_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    window_options_draw_tab_images(dpi, w);

    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 10, window_options_culture_widgets[WIDX_LANGUAGE].top + 1 }, STR_OPTIONS_LANGUAGE,
        w, { w->colours[1] });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 10, window_options_culture_widgets[WIDX_CURRENCY].top + 1 }, STR_CURRENCY, w,
        { w->colours[1] });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 10, window_options_culture_widgets[WIDX_DISTANCE].top + 1 }, STR_DISTANCE_AND_SPEED,
        w, { w->colours[1] });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 10, window_options_culture_widgets[WIDX_TEMPERATURE].top + 1 }, STR_TEMPERATURE, w,
        { w->colours[1] });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 10, window_options_culture_widgets[WIDX_HEIGHT_LABELS].top + 1 }, STR_HEIGHT_LABELS,
        w, { w->colours[1] });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 10, window_options_culture_widgets[WIDX_DATE_FORMAT].top + 1 }, STR_DATE_FORMAT, w,
        { w->colours[1] });
}

#pragma region Audio Tab

static void window_options_audio_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    if (widgetIndex < WIDX_PAGE_START)
        return window_options_common_mouseup(w, widgetIndex);

    switch (widgetIndex)
    {
        case WIDX_SOUND_CHECKBOX:
            gConfigSound.sound_enabled = !gConfigSound.sound_enabled;
            config_save_default();
            w->Invalidate();
            break;

        case WIDX_MASTER_SOUND_CHECKBOX:
            gConfigSound.master_sound_enabled = !gConfigSound.master_sound_enabled;
            if (!gConfigSound.master_sound_enabled)
                OpenRCT2::Audio::Pause();
            else
                OpenRCT2::Audio::Resume();
            window_invalidate_by_class(WC_TOP_TOOLBAR);
            config_save_default();
            w->Invalidate();
            break;

        case WIDX_MUSIC_CHECKBOX:
            gConfigSound.ride_music_enabled = !gConfigSound.ride_music_enabled;
            if (!gConfigSound.ride_music_enabled)
            {
                OpenRCT2::RideAudio::StopAllChannels();
            }
            config_save_default();
            w->Invalidate();
            break;

        case WIDX_AUDIO_FOCUS_CHECKBOX:
            gConfigSound.audio_focus = !gConfigSound.audio_focus;
            config_save_default();
            w->Invalidate();
            break;
    }
}

static void window_options_audio_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    widget = &w->widgets[widgetIndex - 1];

    switch (widgetIndex)
    {
        case WIDX_SOUND_DROPDOWN:
            OpenRCT2::Audio::PopulateDevices();

            // populate the list with the sound devices
            for (int32_t i = 0; i < OpenRCT2::Audio::GetDeviceCount(); i++)
            {
                gDropdownItemsFormat[i] = STR_OPTIONS_DROPDOWN_ITEM;
                gDropdownItemsArgs[i] = reinterpret_cast<uintptr_t>(OpenRCT2::Audio::GetDeviceName(i).c_str());
            }

            window_options_show_dropdown(w, widget, OpenRCT2::Audio::GetDeviceCount());

            Dropdown::SetChecked(OpenRCT2::Audio::GetCurrentDeviceIndex(), true);
            break;
        case WIDX_TITLE_MUSIC_DROPDOWN:
            uint32_t num_items = 4;

            for (size_t i = 0; i < num_items; i++)
            {
                gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
                gDropdownItemsArgs[i] = window_options_title_music_names[i];
            }

            window_options_show_dropdown(w, widget, num_items);

            Dropdown::SetChecked(gConfigSound.title_music, true);
            break;
    }
}

static void window_options_audio_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
        return;

    switch (widgetIndex)
    {
        case WIDX_SOUND_DROPDOWN:
            OpenRCT2::Audio::InitRideSounds(dropdownIndex);
            if (dropdownIndex < OpenRCT2::Audio::GetDeviceCount())
            {
                if (dropdownIndex == 0)
                {
                    Mixer_Init(nullptr);
                    gConfigSound.device = nullptr;
                }
                else
                {
                    const char* devicename = OpenRCT2::Audio::GetDeviceName(dropdownIndex).c_str();
                    Mixer_Init(devicename);
                    SafeFree(gConfigSound.device);
                    gConfigSound.device = strndup(devicename, OpenRCT2::Audio::MaxDeviceNameSize);
                }
                config_save_default();
                OpenRCT2::Audio::PlayTitleMusic();
            }
            w->Invalidate();
            break;
        case WIDX_TITLE_MUSIC_DROPDOWN:
            if ((dropdownIndex == 1 || dropdownIndex == 3) && !Platform::FileExists(context_get_path_legacy(PATH_ID_CSS50)))
            {
                context_show_error(STR_OPTIONS_MUSIC_ERR_CSS50_NOT_FOUND, STR_OPTIONS_MUSIC_ERR_CSS50_NOT_FOUND_HINT, {});
            }
            else
            {
                gConfigSound.title_music = static_cast<int8_t>(dropdownIndex);
                config_save_default();
                w->Invalidate();
            }

            OpenRCT2::Audio::StopTitleMusic();
            if (dropdownIndex != 0)
                OpenRCT2::Audio::PlayTitleMusic();
            break;
    }
}

static uint8_t get_scroll_percentage(rct_widget* widget, rct_scroll* scroll)
{
    uint8_t width = widget->width() - 1;
    return static_cast<float>(scroll->h_left) / (scroll->h_right - width) * 100;
}

static void window_options_audio_update(rct_window* w)
{
    window_options_common_update(w);

    if (w->page == WINDOW_OPTIONS_PAGE_AUDIO)
    {
        rct_widget* widget;

        widget = &window_options_audio_widgets[WIDX_MASTER_VOLUME];
        uint8_t master_volume = get_scroll_percentage(widget, &w->scrolls[0]);
        if (master_volume != gConfigSound.master_volume)
        {
            gConfigSound.master_volume = master_volume;
            config_save_default();
            widget_invalidate(w, WIDX_MASTER_VOLUME);
        }

        widget = &window_options_audio_widgets[WIDX_SOUND_VOLUME];
        uint8_t sound_volume = get_scroll_percentage(widget, &w->scrolls[1]);
        if (sound_volume != gConfigSound.sound_volume)
        {
            gConfigSound.sound_volume = sound_volume;
            config_save_default();
            widget_invalidate(w, WIDX_SOUND_VOLUME);
        }

        widget = &window_options_audio_widgets[WIDX_MUSIC_VOLUME];
        uint8_t ride_music_volume = get_scroll_percentage(widget, &w->scrolls[2]);
        if (ride_music_volume != gConfigSound.ride_music_volume)
        {
            gConfigSound.ride_music_volume = ride_music_volume;
            config_save_default();
            widget_invalidate(w, WIDX_MUSIC_VOLUME);
        }
    }
}

static void window_options_audio_scrollgetsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    *width = 500;
}

static void initialize_scroll_position(rct_window* w, rct_widgetindex widget_index, int32_t scroll_id, uint8_t volume)
{
    rct_widget* widget = &window_options_audio_widgets[widget_index];
    rct_scroll* scroll = &w->scrolls[scroll_id];

    int widget_size = scroll->h_right - (widget->width() - 1);
    scroll->h_left = ceil(volume / 100.0f * widget_size);

    WidgetScrollUpdateThumbs(w, widget_index);
}

static void window_options_audio_invalidate(rct_window* w)
{
    window_options_common_invalidate_before(w);

    // Sound device
    rct_string_id audioDeviceStringId = STR_OPTIONS_SOUND_VALUE_DEFAULT;
    const char* audioDeviceName = nullptr;
    const int32_t currentDeviceIndex = OpenRCT2::Audio::GetCurrentDeviceIndex();
    if (currentDeviceIndex == -1)
    {
        audioDeviceStringId = STR_SOUND_NONE;
    }
    else
    {
        audioDeviceStringId = STR_STRING;
#ifndef __linux__
        if (currentDeviceIndex == 0)
        {
            audioDeviceStringId = STR_OPTIONS_SOUND_VALUE_DEFAULT;
        }
#endif // __linux__
        if (audioDeviceStringId == STR_STRING)
        {
            audioDeviceName = OpenRCT2::Audio::GetDeviceName(currentDeviceIndex).c_str();
        }
    }

    window_options_audio_widgets[WIDX_SOUND].text = audioDeviceStringId;
    auto ft = Formatter::Common();
    ft.Add<char*>(audioDeviceName);

    window_options_audio_widgets[WIDX_TITLE_MUSIC].text = window_options_title_music_names[gConfigSound.title_music];

    WidgetSetCheckboxValue(w, WIDX_SOUND_CHECKBOX, gConfigSound.sound_enabled);
    WidgetSetCheckboxValue(w, WIDX_MASTER_SOUND_CHECKBOX, gConfigSound.master_sound_enabled);
    WidgetSetCheckboxValue(w, WIDX_MUSIC_CHECKBOX, gConfigSound.ride_music_enabled);
    WidgetSetCheckboxValue(w, WIDX_AUDIO_FOCUS_CHECKBOX, gConfigSound.audio_focus);
    WidgetSetEnabled(w, WIDX_SOUND_CHECKBOX, gConfigSound.master_sound_enabled);
    WidgetSetEnabled(w, WIDX_MUSIC_CHECKBOX, gConfigSound.master_sound_enabled);

    // Initialize only on first frame, otherwise the scrollbars won't be able to be modified
    if (w->frame_no == 0)
    {
        initialize_scroll_position(w, WIDX_MASTER_VOLUME, 0, gConfigSound.master_volume);
        initialize_scroll_position(w, WIDX_SOUND_VOLUME, 1, gConfigSound.sound_volume);
        initialize_scroll_position(w, WIDX_MUSIC_VOLUME, 2, gConfigSound.ride_music_volume);
    }

    window_options_common_invalidate_after(w);
}

static void window_options_audio_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    window_options_draw_tab_images(dpi, w);
}

#pragma region Controls and Interface Tab

static void window_options_controls_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    if (widgetIndex < WIDX_PAGE_START)
        return window_options_common_mouseup(w, widgetIndex);

    switch (widgetIndex)
    {
        case WIDX_HOTKEY_DROPDOWN:
            context_open_window(WC_KEYBOARD_SHORTCUT_LIST);
            break;
        case WIDX_SCREEN_EDGE_SCROLLING:
            gConfigGeneral.edge_scrolling ^= 1;
            config_save_default();
            w->Invalidate();
            break;
        case WIDX_TRAP_CURSOR:
            gConfigGeneral.trap_cursor ^= 1;
            config_save_default();
            context_set_cursor_trap(gConfigGeneral.trap_cursor);
            w->Invalidate();
            break;
        case WIDX_ZOOM_TO_CURSOR:
            gConfigGeneral.zoom_to_cursor ^= 1;
            config_save_default();
            w->Invalidate();
            break;
        case WIDX_TOOLBAR_SHOW_FINANCES:
            gConfigInterface.toolbar_show_finances ^= 1;
            config_save_default();
            w->Invalidate();
            window_invalidate_by_class(WC_TOP_TOOLBAR);
            break;
        case WIDX_TOOLBAR_SHOW_RESEARCH:
            gConfigInterface.toolbar_show_research ^= 1;
            config_save_default();
            w->Invalidate();
            window_invalidate_by_class(WC_TOP_TOOLBAR);
            break;
        case WIDX_TOOLBAR_SHOW_CHEATS:
            gConfigInterface.toolbar_show_cheats ^= 1;
            config_save_default();
            w->Invalidate();
            window_invalidate_by_class(WC_TOP_TOOLBAR);
            break;
        case WIDX_TOOLBAR_SHOW_NEWS:
            gConfigInterface.toolbar_show_news ^= 1;
            config_save_default();
            w->Invalidate();
            window_invalidate_by_class(WC_TOP_TOOLBAR);
            break;
        case WIDX_TOOLBAR_SHOW_MUTE:
            gConfigInterface.toolbar_show_mute ^= 1;
            config_save_default();
            w->Invalidate();
            window_invalidate_by_class(WC_TOP_TOOLBAR);
            break;
        case WIDX_TOOLBAR_SHOW_CHAT:
            gConfigInterface.toolbar_show_chat ^= 1;
            config_save_default();
            w->Invalidate();
            window_invalidate_by_class(WC_TOP_TOOLBAR);
            break;
        case WIDX_TOOLBAR_SHOW_ZOOM:
            gConfigInterface.toolbar_show_zoom ^= 1;
            config_save_default();
            w->Invalidate();
            window_invalidate_by_class(WC_TOP_TOOLBAR);
            break;
        case WIDX_INVERT_DRAG:
            gConfigGeneral.invert_viewport_drag ^= 1;
            config_save_default();
            w->Invalidate();
            break;
        case WIDX_THEMES_BUTTON:
            context_open_window(WC_THEMES);
            w->Invalidate();
            break;
    }
}

static void window_options_controls_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    widget = &w->widgets[widgetIndex - 1];

    switch (widgetIndex)
    {
        case WIDX_THEMES_DROPDOWN:
            uint32_t num_items = static_cast<uint32_t>(ThemeManagerGetNumAvailableThemes());

            for (size_t i = 0; i < num_items; i++)
            {
                gDropdownItemsFormat[i] = STR_OPTIONS_DROPDOWN_ITEM;
                gDropdownItemsArgs[i] = reinterpret_cast<uintptr_t>(ThemeManagerGetAvailableThemeName(i));
            }

            WindowDropdownShowTextCustomWidth(
                { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[1], 0,
                Dropdown::Flag::StayOpen, num_items, widget->width() - 3);

            Dropdown::SetChecked(static_cast<int32_t>(ThemeManagerGetAvailableThemeIndex()), true);
            widget_invalidate(w, WIDX_THEMES_DROPDOWN);
            break;
    }
}

static void window_options_controls_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
        return;

    switch (widgetIndex)
    {
        case WIDX_THEMES_DROPDOWN:
            if (dropdownIndex != -1)
            {
                ThemeManagerSetActiveAvailableTheme(dropdownIndex);
            }
            config_save_default();
            break;
    }
}

static void window_options_controls_invalidate(rct_window* w)
{
    window_options_common_invalidate_before(w);

    WidgetSetCheckboxValue(w, WIDX_SCREEN_EDGE_SCROLLING, gConfigGeneral.edge_scrolling);
    WidgetSetCheckboxValue(w, WIDX_TRAP_CURSOR, gConfigGeneral.trap_cursor);
    WidgetSetCheckboxValue(w, WIDX_INVERT_DRAG, gConfigGeneral.invert_viewport_drag);
    WidgetSetCheckboxValue(w, WIDX_ZOOM_TO_CURSOR, gConfigGeneral.zoom_to_cursor);
    WidgetSetCheckboxValue(w, WIDX_TOOLBAR_SHOW_FINANCES, gConfigInterface.toolbar_show_finances);
    WidgetSetCheckboxValue(w, WIDX_TOOLBAR_SHOW_RESEARCH, gConfigInterface.toolbar_show_research);
    WidgetSetCheckboxValue(w, WIDX_TOOLBAR_SHOW_CHEATS, gConfigInterface.toolbar_show_cheats);
    WidgetSetCheckboxValue(w, WIDX_TOOLBAR_SHOW_NEWS, gConfigInterface.toolbar_show_news);
    WidgetSetCheckboxValue(w, WIDX_TOOLBAR_SHOW_MUTE, gConfigInterface.toolbar_show_mute);
    WidgetSetCheckboxValue(w, WIDX_TOOLBAR_SHOW_CHAT, gConfigInterface.toolbar_show_chat);
    WidgetSetCheckboxValue(w, WIDX_TOOLBAR_SHOW_ZOOM, gConfigInterface.toolbar_show_zoom);

    size_t activeAvailableThemeIndex = ThemeManagerGetAvailableThemeIndex();
    const utf8* activeThemeName = ThemeManagerGetAvailableThemeName(activeAvailableThemeIndex);
    auto ft = Formatter::Common();
    ft.Add<utf8*>(activeThemeName);

    window_options_common_invalidate_after(w);
}

static void window_options_controls_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    window_options_draw_tab_images(dpi, w);

    DrawTextBasic(
        dpi,
        w->windowPos + ScreenCoordsXY{ 10, window_options_controls_and_interface_widgets[WIDX_TOOLBAR_BUTTONS_GROUP].top + 15 },
        STR_SHOW_TOOLBAR_BUTTONS_FOR, w, { w->colours[1] });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 10, window_options_controls_and_interface_widgets[WIDX_THEMES].top + 1 },
        STR_THEMES_LABEL_CURRENT_THEME, {}, { w->colours[1] });
}

#pragma region Miscellaneous Tab

static void window_options_misc_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    if (widgetIndex < WIDX_PAGE_START)
        return window_options_common_mouseup(w, widgetIndex);

    switch (widgetIndex)
    {
        case WIDX_REAL_NAME_CHECKBOX:
            gConfigGeneral.show_real_names_of_guests ^= 1;
            config_save_default();
            w->Invalidate();
            peep_update_names(gConfigGeneral.show_real_names_of_guests);
            break;
        case WIDX_AUTO_STAFF_PLACEMENT:
            gConfigGeneral.auto_staff_placement ^= 1;
            config_save_default();
            w->Invalidate();
            break;
        case WIDX_TITLE_SEQUENCE_BUTTON:
            window_title_editor_open(0);
            break;
        case WIDX_SCENARIO_UNLOCKING:
            gConfigGeneral.scenario_unlocking_enabled ^= 1;
            config_save_default();
            window_close_by_class(WC_SCENARIO_SELECT);
            break;
        case WIDX_TITLE_SEQUENCE_RANDOM:
            gConfigInterface.random_title_sequence ^= 1;
            config_save_default();
            w->Invalidate();
            break;
        case WIDX_AUTO_OPEN_SHOPS:
            gConfigGeneral.auto_open_shops = !gConfigGeneral.auto_open_shops;
            config_save_default();
            w->Invalidate();
            break;
        case WIDX_ALLOW_EARLY_COMPLETION:
            gConfigGeneral.allow_early_completion ^= 1;
            config_save_default();
            w->Invalidate();
            break;
    }
}

static void window_options_misc_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    uint32_t num_items;

    widget = &w->widgets[widgetIndex - 1];

    switch (widgetIndex)
    {
        case WIDX_TITLE_SEQUENCE_DROPDOWN:
            num_items = static_cast<int32_t>(title_sequence_manager_get_count());
            for (size_t i = 0; i < num_items; i++)
            {
                gDropdownItemsFormat[i] = STR_OPTIONS_DROPDOWN_ITEM;
                gDropdownItemsArgs[i] = reinterpret_cast<uintptr_t>(title_sequence_manager_get_name(i));
            }

            WindowDropdownShowText(
                { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[1],
                Dropdown::Flag::StayOpen, num_items);

            Dropdown::SetChecked(static_cast<int32_t>(title_get_current_sequence()), true);
            break;
        case WIDX_SCENARIO_GROUPING_DROPDOWN:
            num_items = 2;

            gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsArgs[0] = STR_OPTIONS_SCENARIO_DIFFICULTY;
            gDropdownItemsFormat[1] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsArgs[1] = STR_OPTIONS_SCENARIO_ORIGIN;

            WindowDropdownShowTextCustomWidth(
                { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[1], 0,
                Dropdown::Flag::StayOpen, num_items, widget->width() - 3);

            Dropdown::SetChecked(gConfigGeneral.scenario_select_mode, true);
            break;
        case WIDX_DEFAULT_INSPECTION_INTERVAL_DROPDOWN:
            for (size_t i = 0; i < 7; i++)
            {
                gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
                gDropdownItemsArgs[i] = RideInspectionIntervalNames[i];
            }

            window_options_show_dropdown(w, widget, 7);
            Dropdown::SetChecked(gConfigGeneral.default_inspection_interval, true);
            break;
    }
}

static void window_options_misc_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
        return;

    switch (widgetIndex)
    {
        case WIDX_TITLE_SEQUENCE_DROPDOWN:
            if (dropdownIndex != static_cast<int32_t>(title_get_current_sequence()))
            {
                title_sequence_change_preset(static_cast<size_t>(dropdownIndex));
                config_save_default();
                w->Invalidate();
            }
            break;
        case WIDX_DEFAULT_INSPECTION_INTERVAL_DROPDOWN:
            if (dropdownIndex != gConfigGeneral.default_inspection_interval)
            {
                gConfigGeneral.default_inspection_interval = static_cast<uint8_t>(dropdownIndex);
                config_save_default();
                w->Invalidate();
            }
            break;
        case WIDX_SCENARIO_GROUPING_DROPDOWN:
            if (dropdownIndex != gConfigGeneral.scenario_select_mode)
            {
                gConfigGeneral.scenario_select_mode = dropdownIndex;
                gConfigInterface.scenarioselect_last_tab = 0;
                config_save_default();
                w->Invalidate();
                window_close_by_class(WC_SCENARIO_SELECT);
            }
            break;
    }
}

static void window_options_misc_invalidate(rct_window* w)
{
    window_options_common_invalidate_before(w);

    const utf8* name = title_sequence_manager_get_name(title_get_config_sequence());
    auto ft = Formatter::Common();
    ft.Add<utf8*>(name);

    // The real name setting of clients is fixed to that of the server
    // and the server cannot change the setting during gameplay to prevent desyncs
    if (network_get_mode() != NETWORK_MODE_NONE)
    {
        w->disabled_widgets |= (1ULL << WIDX_REAL_NAME_CHECKBOX);
        window_options_misc_widgets[WIDX_REAL_NAME_CHECKBOX].tooltip = STR_OPTION_DISABLED_DURING_NETWORK_PLAY;
    }

    WidgetSetCheckboxValue(w, WIDX_REAL_NAME_CHECKBOX, gConfigGeneral.show_real_names_of_guests);
    WidgetSetCheckboxValue(w, WIDX_AUTO_STAFF_PLACEMENT, gConfigGeneral.auto_staff_placement);
    WidgetSetCheckboxValue(w, WIDX_AUTO_OPEN_SHOPS, gConfigGeneral.auto_open_shops);
    WidgetSetCheckboxValue(w, WIDX_TITLE_SEQUENCE_RANDOM, gConfigInterface.random_title_sequence);
    WidgetSetCheckboxValue(w, WIDX_ALLOW_EARLY_COMPLETION, gConfigGeneral.allow_early_completion);

    // Disable title sequence dropdown if set to random
    if (gConfigInterface.random_title_sequence)
    {
        w->disabled_widgets |= (1 << WIDX_TITLE_SEQUENCE_DROPDOWN);
        w->disabled_widgets |= (1 << WIDX_TITLE_SEQUENCE);
    }
    else
    {
        w->disabled_widgets &= ~(1 << WIDX_TITLE_SEQUENCE_DROPDOWN);
        w->disabled_widgets &= ~(1 << WIDX_TITLE_SEQUENCE);
    }

    if (gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_DIFFICULTY)
        window_options_misc_widgets[WIDX_SCENARIO_GROUPING].text = STR_OPTIONS_SCENARIO_DIFFICULTY;
    else
        window_options_misc_widgets[WIDX_SCENARIO_GROUPING].text = STR_OPTIONS_SCENARIO_ORIGIN;

    WidgetSetCheckboxValue(w, WIDX_SCENARIO_UNLOCKING, gConfigGeneral.scenario_unlocking_enabled);

    if (gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_ORIGIN)
    {
        w->disabled_widgets &= ~(1ULL << WIDX_SCENARIO_UNLOCKING);
    }
    else
    {
        w->disabled_widgets |= (1ULL << WIDX_SCENARIO_UNLOCKING);
    }

    window_options_misc_widgets[WIDX_DEFAULT_INSPECTION_INTERVAL].text = RideInspectionIntervalNames
        [gConfigGeneral.default_inspection_interval];

    window_options_common_invalidate_after(w);
}

static void window_options_misc_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    window_options_draw_tab_images(dpi, w);

    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 10, window_options_misc_widgets[WIDX_TITLE_SEQUENCE].top + 1 }, STR_TITLE_SEQUENCE,
        w, { w->colours[1] });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 10, window_options_misc_widgets[WIDX_SCENARIO_GROUPING].top + 1 },
        STR_OPTIONS_SCENARIO_GROUPING, {}, { w->colours[1] });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 10, window_options_misc_widgets[WIDX_DEFAULT_INSPECTION_INTERVAL].top + 1 },
        STR_DEFAULT_INSPECTION_INTERVAL, w, { w->colours[1] });
}

#pragma region Advanced Tab

static void window_options_advanced_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    if (widgetIndex < WIDX_PAGE_START)
        return window_options_common_mouseup(w, widgetIndex);

    switch (widgetIndex)
    {
        case WIDX_DEBUGGING_TOOLS:
            gConfigGeneral.debugging_tools ^= 1;
            config_save_default();
            gfx_invalidate_screen();
            break;
        case WIDX_ALLOW_LOADING_WITH_INCORRECT_CHECKSUM:
            gConfigGeneral.allow_loading_with_incorrect_checksum = !gConfigGeneral.allow_loading_with_incorrect_checksum;
            config_save_default();
            w->Invalidate();
            break;
        case WIDX_SAVE_PLUGIN_DATA_CHECKBOX:
            gConfigGeneral.save_plugin_data ^= 1;
            config_save_default();
            w->Invalidate();
            break;
        case WIDX_STAY_CONNECTED_AFTER_DESYNC:
            gConfigNetwork.stay_connected = !gConfigNetwork.stay_connected;
            config_save_default();
            w->Invalidate();
            break;
        case WIDX_ALWAYS_NATIVE_LOADSAVE:
            gConfigGeneral.use_native_browse_dialog = !gConfigGeneral.use_native_browse_dialog;
            config_save_default();
            w->Invalidate();
            break;
        case WIDX_PATH_TO_RCT1_BUTTON:
        {
            utf8string rct1path = platform_open_directory_browser(language_get_string(STR_PATH_TO_RCT1_BROWSER));
            if (rct1path)
            {
                // Check if this directory actually contains RCT1
                if (Csg1datPresentAtLocation(rct1path))
                {
                    if (Csg1idatPresentAtLocation(rct1path))
                    {
                        if (CsgAtLocationIsUsable(rct1path))
                        {
                            SafeFree(gConfigGeneral.rct1_path);
                            gConfigGeneral.rct1_path = rct1path;
                            gConfigInterface.scenarioselect_last_tab = 0;
                            config_save_default();
                            context_show_error(STR_RESTART_REQUIRED, STR_NONE, {});
                        }
                        else
                        {
                            SafeFree(rct1path);
                            context_show_error(STR_PATH_TO_RCT1_IS_WRONG_VERSION, STR_NONE, {});
                        }
                    }
                    else
                    {
                        SafeFree(rct1path);
                        context_show_error(STR_PATH_TO_RCT1_DOES_NOT_CONTAIN_CSG1I_DAT, STR_NONE, {});
                    }
                }
                else
                {
                    SafeFree(rct1path);
                    context_show_error(STR_PATH_TO_RCT1_WRONG_ERROR, STR_NONE, {});
                }
            }
            w->Invalidate();
            break;
        }
        case WIDX_PATH_TO_RCT1_CLEAR:
            if (!str_is_null_or_empty(gConfigGeneral.rct1_path))
            {
                SafeFree(gConfigGeneral.rct1_path);
                config_save_default();
            }
            w->Invalidate();
            break;
    }
}

static void window_options_advanced_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    widget = &w->widgets[widgetIndex - 1];

    switch (widgetIndex)
    {
        case WIDX_AUTOSAVE_DROPDOWN:
            for (size_t i = AUTOSAVE_EVERY_MINUTE; i <= AUTOSAVE_NEVER; i++)
            {
                gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
                gDropdownItemsArgs[i] = window_options_autosave_names[i];
            }

            window_options_show_dropdown(w, widget, AUTOSAVE_NEVER + 1);
            Dropdown::SetChecked(gConfigGeneral.autosave_frequency, true);
            break;
        case WIDX_AUTOSAVE_AMOUNT_UP:
            gConfigGeneral.autosave_amount += 1;
            config_save_default();
            widget_invalidate(w, WIDX_AUTOSAVE);
            widget_invalidate(w, WIDX_AUTOSAVE_DROPDOWN);
            widget_invalidate(w, WIDX_AUTOSAVE_AMOUNT);
            break;
        case WIDX_AUTOSAVE_AMOUNT_DOWN:
            if (gConfigGeneral.autosave_amount > 1)
            {
                gConfigGeneral.autosave_amount -= 1;
                config_save_default();
                widget_invalidate(w, WIDX_AUTOSAVE);
                widget_invalidate(w, WIDX_AUTOSAVE_DROPDOWN);
                widget_invalidate(w, WIDX_AUTOSAVE_AMOUNT);
            }
    }
}

static void window_options_advanced_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
        return;

    switch (widgetIndex)
    {
        case WIDX_AUTOSAVE_DROPDOWN:
            if (dropdownIndex != gConfigGeneral.autosave_frequency)
            {
                gConfigGeneral.autosave_frequency = static_cast<uint8_t>(dropdownIndex);
                config_save_default();
                w->Invalidate();
            }
            break;
    }
}

static void window_options_advanced_invalidate(rct_window* w)
{
    window_options_common_invalidate_before(w);

    WidgetSetCheckboxValue(w, WIDX_DEBUGGING_TOOLS, gConfigGeneral.debugging_tools);
    WidgetSetCheckboxValue(w, WIDX_ALLOW_LOADING_WITH_INCORRECT_CHECKSUM, gConfigGeneral.allow_loading_with_incorrect_checksum);
    WidgetSetCheckboxValue(w, WIDX_SAVE_PLUGIN_DATA_CHECKBOX, gConfigGeneral.save_plugin_data);
    WidgetSetCheckboxValue(w, WIDX_STAY_CONNECTED_AFTER_DESYNC, gConfigNetwork.stay_connected);
    WidgetSetCheckboxValue(w, WIDX_ALWAYS_NATIVE_LOADSAVE, gConfigGeneral.use_native_browse_dialog);

    window_options_common_invalidate_after(w);
}

static void window_options_advanced_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    window_options_draw_tab_images(dpi, w);

    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 24, window_options_advanced_widgets[WIDX_AUTOSAVE].top + 1 },
        STR_OPTIONS_AUTOSAVE_FREQUENCY_LABEL, w, { w->colours[1] });
    DrawTextBasic(
        dpi,
        w->windowPos
            + ScreenCoordsXY{ window_options_advanced_widgets[WIDX_AUTOSAVE].left + 1,
                              window_options_advanced_widgets[WIDX_AUTOSAVE].top },
        window_options_autosave_names[gConfigGeneral.autosave_frequency], {}, { w->colours[1] });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 24, window_options_advanced_widgets[WIDX_AUTOSAVE_AMOUNT].top + 1 },
        STR_AUTOSAVE_AMOUNT, w, { w->colours[1] });
    int32_t autosavesToKeep = static_cast<int32_t>(gConfigGeneral.autosave_amount);
    DrawTextBasic(
        dpi,
        w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_AUTOSAVE_AMOUNT].left + 1, w->widgets[WIDX_AUTOSAVE_AMOUNT].top + 1 },
        STR_WINDOW_OBJECTIVE_VALUE_GUEST_COUNT, &autosavesToKeep, { w->colours[1] });

    auto ft = Formatter();
    ft.Add<utf8*>(Platform::StrDecompToPrecomp(gConfigGeneral.rct1_path));

    rct_widget pathWidget = window_options_advanced_widgets[WIDX_PATH_TO_RCT1_BUTTON];

    // Apply vertical alignment if appropriate.
    int32_t widgetHeight = pathWidget.bottom - pathWidget.top;
    int32_t lineHeight = font_get_line_height(FontSpriteBase::MEDIUM);
    uint32_t padding = widgetHeight > lineHeight ? (widgetHeight - lineHeight) / 2 : 0;
    ScreenCoordsXY screenCoords = { w->windowPos.x + pathWidget.left + 1,
                                    w->windowPos.y + pathWidget.top + static_cast<int32_t>(padding) };
    DrawTextEllipsised(dpi, screenCoords, 277, STR_STRING, ft, { w->colours[1] });
}

static OpenRCT2String window_options_advanced_tooltip(
    rct_window* w, const rct_widgetindex widgetIndex, const rct_string_id fallback)
{
    if (widgetIndex == WIDX_PATH_TO_RCT1_BUTTON)
    {
        if (str_is_null_or_empty(gConfigGeneral.rct1_path))
        {
            // No tooltip if the path is empty
            return { STR_NONE, {} };
        }
        else
        {
            auto ft = Formatter();
            ft.Add<utf8*>(gConfigGeneral.rct1_path);
            return { fallback, ft };
        }
    }
    return { fallback, {} };
}

#pragma region Event lists

static rct_window_event_list window_options_events_display([](auto& events) {
    events.mouse_up = &window_options_display_mouseup;
    events.mouse_down = &window_options_display_mousedown;
    events.dropdown = &window_options_display_dropdown;
    events.update = &window_options_common_update;
    events.invalidate = &window_options_display_invalidate;
    events.paint = &window_options_display_paint;
});

static rct_window_event_list window_options_events_rendering([](auto& events) {
    events.mouse_up = &window_options_rendering_mouseup;
    events.mouse_down = &window_options_rendering_mousedown;
    events.dropdown = &window_options_rendering_dropdown;
    events.update = &window_options_common_update;
    events.invalidate = &window_options_rendering_invalidate;
    events.paint = &window_options_rendering_paint;
});

static rct_window_event_list window_options_events_culture([](auto& events) {
    events.mouse_up = &window_options_culture_mouseup;
    events.mouse_down = &window_options_culture_mousedown;
    events.dropdown = &window_options_culture_dropdown;
    events.update = &window_options_common_update;
    events.invalidate = &window_options_culture_invalidate;
    events.paint = &window_options_culture_paint;
});

static rct_window_event_list window_options_events_audio([](auto& events) {
    events.mouse_up = &window_options_audio_mouseup;
    events.mouse_down = &window_options_audio_mousedown;
    events.dropdown = &window_options_audio_dropdown;
    events.update = &window_options_audio_update;
    events.get_scroll_size = &window_options_audio_scrollgetsize;
    events.invalidate = &window_options_audio_invalidate;
    events.paint = &window_options_audio_paint;
});

static rct_window_event_list window_options_events_controls([](auto& events) {
    events.mouse_up = &window_options_controls_mouseup;
    events.mouse_down = &window_options_controls_mousedown;
    events.dropdown = &window_options_controls_dropdown;
    events.update = &window_options_common_update;
    events.invalidate = &window_options_controls_invalidate;
    events.paint = &window_options_controls_paint;
});

static rct_window_event_list window_options_events_misc([](auto& events) {
    events.mouse_up = &window_options_misc_mouseup;
    events.mouse_down = &window_options_misc_mousedown;
    events.dropdown = &window_options_misc_dropdown;
    events.update = &window_options_common_update;
    events.invalidate = &window_options_misc_invalidate;
    events.paint = &window_options_misc_paint;
});

static rct_window_event_list window_options_events_advanced([](auto& events) {
    events.mouse_up = &window_options_advanced_mouseup;
    events.mouse_down = &window_options_advanced_mousedown;
    events.dropdown = &window_options_advanced_dropdown;
    events.update = &window_options_common_update;
    events.tooltip = &window_options_advanced_tooltip;
    events.invalidate = &window_options_advanced_invalidate;
    events.paint = &window_options_advanced_paint;
});

// clang-format off
static rct_window_event_list* window_options_event_lists[] = {
    &window_options_events_display,
    &window_options_events_rendering,
    &window_options_events_culture,
    &window_options_events_audio,
    &window_options_events_controls,
    &window_options_events_misc,
    &window_options_events_advanced,
};
// clang-format on

#pragma region Common

/**
 *
 *  rct2: 0x006BAC5B
 */
rct_window* window_options_open()
{
    rct_window* w;

    // Check if window is already open
    w = window_bring_to_front_by_class(WC_OPTIONS);
    if (w != nullptr)
        return w;

    w = WindowCreateCentred(WW, WH, &window_options_events_display, WC_OPTIONS, 0);
    w->widgets = window_options_display_widgets;
    w->enabled_widgets = window_options_page_enabled_widgets[WINDOW_OPTIONS_PAGE_DISPLAY];
    w->page = WINDOW_OPTIONS_PAGE_DISPLAY;
    w->frame_no = 0;
    WindowInitScrollWidgets(w);

    return w;
}

static void window_options_set_page(rct_window* w, int32_t page)
{
    w->page = page;
    w->frame_no = 0;
    w->event_handlers = window_options_event_lists[page];
    w->enabled_widgets = window_options_page_enabled_widgets[page];
    w->pressed_widgets = 0;
    w->widgets = window_options_page_widgets[page];

    w->Invalidate();
    window_event_resize_call(w);
    window_event_invalidate_call(w);
    WindowInitScrollWidgets(w);
    w->Invalidate();
}

static void window_options_set_pressed_tab(rct_window* w)
{
    int32_t i;
    for (i = 0; i < WINDOW_OPTIONS_PAGE_COUNT; i++)
        w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
    w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

static void window_options_draw_tab_image(rct_drawpixelinfo* dpi, rct_window* w, int32_t page, int32_t spriteIndex)
{
    rct_widgetindex widgetIndex = WIDX_TAB_1 + page;
    rct_widget* widget = &w->widgets[widgetIndex];

    auto screenCoords = w->windowPos + ScreenCoordsXY{ widget->left, widget->top };

    if (!(w->disabled_widgets & (1LL << widgetIndex)))
    {
        if (w->page == page)
        {
            int32_t frame = w->frame_no / window_options_tab_animation_divisor[w->page];
            spriteIndex += (frame % window_options_tab_animation_frames[w->page]);
        }

        // Draw normal, enabled sprite.
        gfx_draw_sprite(dpi, spriteIndex, screenCoords, 0);
    }
    else
    {
        // Get the window background colour
        uint8_t window_colour = NOT_TRANSLUCENT(w->colours[widget->colour]);

        // Draw greyed out (light border bottom right shadow)
        gfx_draw_sprite_solid(dpi, spriteIndex, screenCoords + ScreenCoordsXY{ 1, 1 }, ColourMapA[window_colour].lighter);

        // Draw greyed out (dark)
        gfx_draw_sprite_solid(dpi, spriteIndex, screenCoords, ColourMapA[window_colour].mid_light);
    }
}

static void window_options_draw_tab_images(rct_drawpixelinfo* dpi, rct_window* w)
{
    window_options_draw_tab_image(dpi, w, WINDOW_OPTIONS_PAGE_DISPLAY, SPR_TAB_PAINT_0);
    window_options_draw_tab_image(dpi, w, WINDOW_OPTIONS_PAGE_RENDERING, SPR_G2_TAB_TREE);
    window_options_draw_tab_image(dpi, w, WINDOW_OPTIONS_PAGE_CULTURE, SPR_TAB_TIMER_0);
    window_options_draw_tab_image(dpi, w, WINDOW_OPTIONS_PAGE_AUDIO, SPR_TAB_MUSIC_0);
    window_options_draw_tab_image(dpi, w, WINDOW_OPTIONS_PAGE_CONTROLS_AND_INTERFACE, SPR_TAB_GEARS_0);
    window_options_draw_tab_image(dpi, w, WINDOW_OPTIONS_PAGE_MISC, SPR_TAB_RIDE_0);
    window_options_draw_tab_image(dpi, w, WINDOW_OPTIONS_PAGE_ADVANCED, SPR_TAB_WRENCH_0);
}

// helper function, all dropdown boxes have similar properties
static void window_options_show_dropdown(rct_window* w, rct_widget* widget, int32_t num_items)
{
    WindowDropdownShowTextCustomWidth(
        { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[1], 0,
        Dropdown::Flag::StayOpen, num_items, widget->width() - 3);
}

static void window_options_update_height_markers()
{
    config_save_default();
    gfx_invalidate_screen();
}

#pragma endregion
