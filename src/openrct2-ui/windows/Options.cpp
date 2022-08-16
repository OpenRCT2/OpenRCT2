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
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/actions/ScenarioSetSettingAction.h>
#include <openrct2/audio/AudioContext.h>
#include <openrct2/audio/AudioMixer.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/File.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/localisation/Currency.h>
#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Language.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/network/network.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/ride/RideAudio.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/sprites.h>
#include <openrct2/title/TitleScreen.h>
#include <openrct2/title/TitleSequenceManager.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/util/Util.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;

// clang-format off
enum WindowOptionsPage {
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

enum WindowOptionsWidgetIdx {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PAGE_BACKGROUND,
    WIDX_FIRST_TAB,
    WIDX_TAB_DISPLAY = WIDX_FIRST_TAB,
    WIDX_TAB_RENDERING,
    WIDX_TAB_CULTURE,
    WIDX_TAB_AUDIO,
    WIDX_TAB_CONTROLS_AND_INTERFACE,
    WIDX_TAB_MISC,
    WIDX_TAB_ADVANCED,

    WIDX_PAGE_START,

    // Display
    WIDX_HARDWARE_GROUP = WIDX_PAGE_START,
    WIDX_FULLSCREEN_LABEL,
    WIDX_FULLSCREEN,
    WIDX_FULLSCREEN_DROPDOWN,
    WIDX_RESOLUTION_LABEL,
    WIDX_RESOLUTION,
    WIDX_RESOLUTION_DROPDOWN,
    WIDX_SCALE_LABEL,
    WIDX_SCALE,
    WIDX_SCALE_UP,
    WIDX_SCALE_DOWN,
    WIDX_DRAWING_ENGINE_LABEL,
    WIDX_DRAWING_ENGINE,
    WIDX_DRAWING_ENGINE_DROPDOWN,
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
    WIDX_LANGUAGE_LABEL = WIDX_PAGE_START,
    WIDX_LANGUAGE,
    WIDX_LANGUAGE_DROPDOWN,
    WIDX_CURRENCY_LABEL,
    WIDX_CURRENCY,
    WIDX_CURRENCY_DROPDOWN,
    WIDX_DISTANCE_LABEL,
    WIDX_DISTANCE,
    WIDX_DISTANCE_DROPDOWN,
    WIDX_TEMPERATURE_LABEL,
    WIDX_TEMPERATURE,
    WIDX_TEMPERATURE_DROPDOWN,
    WIDX_HEIGHT_LABELS_LABEL,
    WIDX_HEIGHT_LABELS,
    WIDX_HEIGHT_LABELS_DROPDOWN,
    WIDX_DATE_FORMAT_LABEL,
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
    WIDX_THEMES_LABEL,
    WIDX_THEMES,
    WIDX_THEMES_DROPDOWN,
    WIDX_THEMES_BUTTON,
    WIDX_TOOLBAR_BUTTONS_GROUP,
    WIDX_TOOLBAR_BUTTONS_SHOW_FOR_LABEL,
    WIDX_TOOLBAR_SHOW_FINANCES,
    WIDX_TOOLBAR_SHOW_RESEARCH,
    WIDX_TOOLBAR_SHOW_CHEATS,
    WIDX_TOOLBAR_SHOW_NEWS,
    WIDX_TOOLBAR_SHOW_MUTE,
    WIDX_TOOLBAR_SHOW_CHAT,
    WIDX_TOOLBAR_SHOW_ZOOM,

    // Misc
    WIDX_TITLE_SEQUENCE_GROUP = WIDX_PAGE_START,
    WIDX_TITLE_SEQUENCE,
    WIDX_TITLE_SEQUENCE_DROPDOWN,
    WIDX_SCENARIO_GROUP,
    WIDX_SCENARIO_GROUPING_LABEL,
    WIDX_SCENARIO_GROUPING,
    WIDX_SCENARIO_GROUPING_DROPDOWN,
    WIDX_SCENARIO_UNLOCKING,
    WIDX_SCENARIO_OPTIONS_GROUP,
    WIDX_ALLOW_EARLY_COMPLETION,
    WIDX_TWEAKS_GROUP,
    WIDX_REAL_NAME_CHECKBOX,
    WIDX_AUTO_STAFF_PLACEMENT,
    WIDX_AUTO_OPEN_SHOPS,
    WIDX_DEFAULT_INSPECTION_INTERVAL_LABEL,
    WIDX_DEFAULT_INSPECTION_INTERVAL,
    WIDX_DEFAULT_INSPECTION_INTERVAL_DROPDOWN,

    // Advanced
    WIDX_DEBUGGING_TOOLS = WIDX_PAGE_START,
    WIDX_ALLOW_LOADING_WITH_INCORRECT_CHECKSUM,
    WIDX_SAVE_PLUGIN_DATA_CHECKBOX,
    WIDX_STAY_CONNECTED_AFTER_DESYNC,
    WIDX_ALWAYS_NATIVE_LOADSAVE,
    WIDX_AUTOSAVE_FREQUENCY_LABEL,
    WIDX_AUTOSAVE_FREQUENCY,
    WIDX_AUTOSAVE_FREQUENCY_DROPDOWN,
    WIDX_AUTOSAVE_AMOUNT_LABEL,
    WIDX_AUTOSAVE_AMOUNT,
    WIDX_AUTOSAVE_AMOUNT_UP,
    WIDX_AUTOSAVE_AMOUNT_DOWN,
    WIDX_PATH_TO_RCT1_TEXT,
    WIDX_PATH_TO_RCT1_BUTTON,
    WIDX_PATH_TO_RCT1_CLEAR,
};

static constexpr const StringId WINDOW_TITLE = STR_OPTIONS_TITLE;
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
    MakeWidget        ({  5,  53}, {300, 170}, WindowWidgetType::Groupbox,     WindowColour::Secondary, STR_HARDWARE_GROUP                                                              ), // Hardware group
    MakeWidget        ({ 10,  67}, {145,  12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_FULLSCREEN_MODE,                   STR_FULLSCREEN_MODE_TIP                  ), // Fullscreen
    MakeWidget        ({155,  68}, {145,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                                                  ),
    MakeWidget        ({288,  69}, { 11,  10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,                    STR_FULLSCREEN_MODE_TIP                  ),
    MakeWidget        ({ 24,  82}, {145,  12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_DISPLAY_RESOLUTION,                STR_DISPLAY_RESOLUTION_TIP               ), // Resolution
    MakeWidget        ({155,  83}, {145,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_ARG_16_RESOLUTION_X_BY_Y                                                    ),
    MakeWidget        ({288,  84}, { 11,  10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,                    STR_DISPLAY_RESOLUTION_TIP               ),
    MakeWidget        ({ 10,  98}, {145,  12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_UI_SCALING_DESC,                   STR_WINDOW_SCALE_TIP                     ), // Scale
    MakeSpinnerWidgets({155,  98}, {145,  12}, WindowWidgetType::Spinner,      WindowColour::Secondary, STR_NONE,                              STR_WINDOW_SCALE_TIP                     ), // Scale spinner (3 widgets)
    MakeWidget        ({ 10, 113}, {145,  12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_DRAWING_ENGINE,                    STR_DRAWING_ENGINE_TIP                   ), // Drawing engine
    MakeWidget        ({155, 113}, {145,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                                                  ), // Drawing engine
    MakeWidget        ({288, 114}, { 11,  10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,                    STR_DRAWING_ENGINE_TIP                   ),
    MakeWidget        ({ 11, 144}, {280,  12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_STEAM_OVERLAY_PAUSE,               STR_STEAM_OVERLAY_PAUSE_TIP              ), // Pause on steam overlay
    MakeWidget        ({ 11, 161}, {143,  12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_UNCAP_FPS,                         STR_UNCAP_FPS_TIP                        ), // Uncap fps
    MakeWidget        ({155, 161}, {136,  12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_SHOW_FPS,                          STR_SHOW_FPS_TIP                         ), // Show fps
    MakeWidget        ({155, 176}, {136,  12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_MULTITHREADING,                    STR_MULTITHREADING_TIP                   ), // Multithreading
    MakeWidget        ({ 11, 176}, {143,  12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_USE_VSYNC,                         STR_USE_VSYNC_TIP                        ), // Use vsync
    MakeWidget        ({ 11, 191}, {280,  12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_MINIMISE_FULLSCREEN_ON_FOCUS_LOSS, STR_MINIMISE_FULLSCREEN_ON_FOCUS_LOSS_TIP), // Minimise fullscreen focus loss
    MakeWidget        ({ 11, 206}, {280,  12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_DISABLE_SCREENSAVER,               STR_DISABLE_SCREENSAVER_TIP              ), // Disable screensaver
    WIDGETS_END,
};

static rct_widget window_options_rendering_widgets[] = {
    MAIN_OPTIONS_WIDGETS,
#define FRAME_RENDERING_START 53
    MakeWidget({  5,  FRAME_RENDERING_START + 0}, {300, 108}, WindowWidgetType::Groupbox,     WindowColour::Secondary, STR_RENDERING_GROUP                                       ), // Rendering group
    MakeWidget({ 10, FRAME_RENDERING_START + 15}, {281,  12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_TILE_SMOOTHING,         STR_TILE_SMOOTHING_TIP        ), // Landscape smoothing
    MakeWidget({ 10, FRAME_RENDERING_START + 30}, {281,  12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_GRIDLINES,              STR_GRIDLINES_TIP             ), // Gridlines
    MakeWidget({ 10, FRAME_RENDERING_START + 45}, {281,  12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_UPPERCASE_BANNERS,      STR_UPPERCASE_BANNERS_TIP     ), // Uppercase banners
    MakeWidget({ 10, FRAME_RENDERING_START + 60}, {281,  12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_SHOW_GUEST_PURCHASES,   STR_SHOW_GUEST_PURCHASES_TIP  ), // Guest purchases
    MakeWidget({ 10, FRAME_RENDERING_START + 75}, {281,  12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_TRANSPARENT_SCREENSHOT, STR_TRANSPARENT_SCREENSHOT_TIP), // Transparent screenshot
    MakeWidget({ 10, FRAME_RENDERING_START + 90}, {281,  12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_VIRTUAL_FLOOR_STYLE,    STR_VIRTUAL_FLOOR_STYLE_TIP   ), // Virtual floor
    MakeWidget({155, FRAME_RENDERING_START + 90}, {145,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_NONE,                   STR_VIRTUAL_FLOOR_STYLE_TIP   ), // Virtual floor dropdown
    MakeWidget({288, FRAME_RENDERING_START + 91}, { 11,  10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,         STR_VIRTUAL_FLOOR_STYLE_TIP   ), // Virtual floor dropdown
#undef FRAME_RENDERING_START
#define FRAME_EFFECTS_START 163
    MakeWidget({ 5,  FRAME_EFFECTS_START + 0}, {300, 94}, WindowWidgetType::Groupbox, WindowColour::Secondary, STR_EFFECTS_GROUP                                             ), // Rendering group
    MakeWidget({10, FRAME_EFFECTS_START + 15}, {281, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CYCLE_DAY_NIGHT,          STR_CYCLE_DAY_NIGHT_TIP         ), // Cycle day-night
    MakeWidget({25, FRAME_EFFECTS_START + 30}, {266, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_ENABLE_LIGHTING_EFFECTS,  STR_ENABLE_LIGHTING_EFFECTS_TIP ), // Enable light fx
    MakeWidget({40, FRAME_EFFECTS_START + 45}, {251, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_ENABLE_LIGHTING_VEHICLES, STR_ENABLE_LIGHTING_VEHICLES_TIP), // Enable light fx for vehicles
    MakeWidget({10, FRAME_EFFECTS_START + 60}, {281, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_RENDER_WEATHER_EFFECTS,   STR_RENDER_WEATHER_EFFECTS_TIP  ), // Render weather effects
    MakeWidget({25, FRAME_EFFECTS_START + 75}, {266, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_DISABLE_LIGHTNING_EFFECT, STR_DISABLE_LIGHTNING_EFFECT_TIP), // Disable lightning effect
#undef FRAME_EFFECTS_START
    WIDGETS_END,
};

static rct_widget window_options_culture_widgets[] = {
    MAIN_OPTIONS_WIDGETS,
    MakeWidget({ 10,  53}, {145, 12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_OPTIONS_LANGUAGE,   STR_LANGUAGE_TIP           ), // language
    MakeWidget({155,  53}, {145, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_STRING                                         ),
    MakeWidget({288,  54}, { 11, 10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,     STR_LANGUAGE_TIP           ),
    MakeWidget({ 10,  68}, {145, 12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_CURRENCY,           STR_CURRENCY_TIP           ), // Currency
    MakeWidget({155,  68}, {145, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                     ),
    MakeWidget({288,  69}, { 11, 10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,     STR_CURRENCY_TIP           ),
    MakeWidget({ 10,  83}, {145, 12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_DISTANCE_AND_SPEED, STR_DISTANCE_AND_SPEED_TIP ), // Distance and speed
    MakeWidget({155,  83}, {145, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                     ),
    MakeWidget({288,  84}, { 11, 10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,     STR_DISTANCE_AND_SPEED_TIP ),
    MakeWidget({ 10,  98}, {145, 12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_TEMPERATURE,        STR_TEMPERATURE_FORMAT_TIP ), // Temperature
    MakeWidget({155,  98}, {145, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                     ),
    MakeWidget({288,  99}, { 11, 10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,     STR_TEMPERATURE_FORMAT_TIP ),
    MakeWidget({ 10, 113}, {145, 12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_HEIGHT_LABELS,      STR_HEIGHT_LABELS_UNITS_TIP), // Height labels
    MakeWidget({155, 113}, {145, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                     ),
    MakeWidget({288, 114}, { 11, 10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,     STR_HEIGHT_LABELS_UNITS_TIP),
    MakeWidget({ 10, 128}, {145, 12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_DATE_FORMAT,        STR_DATE_FORMAT_TIP        ), // Date format
    MakeWidget({155, 128}, {145, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                     ),
    MakeWidget({288, 129}, { 11, 10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,     STR_DATE_FORMAT_TIP        ),
    WIDGETS_END,
};

static rct_widget window_options_audio_widgets[] = {
    MAIN_OPTIONS_WIDGETS,
    MakeWidget({ 10,  53}, {290, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                ), // Audio device
    MakeWidget({288,  54}, { 11, 10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,      STR_AUDIO_DEVICE_TIP ),
    MakeWidget({ 10,  69}, {220, 12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_MASTER_VOLUME,       STR_MASTER_VOLUME_TIP), // Enable / disable master sound
    MakeWidget({ 10,  84}, {220, 12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_SOUND_EFFECTS,       STR_SOUND_EFFECTS_TIP), // Enable / disable sound effects
    MakeWidget({ 10,  99}, {220, 12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_RIDE_MUSIC,          STR_RIDE_MUSIC_TIP   ), // Enable / disable ride music
    MakeWidget({ 10, 113}, {290, 13}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_AUDIO_FOCUS,         STR_AUDIO_FOCUS_TIP  ), // Enable / disable audio disabled on focus lost
    MakeWidget({ 10, 128}, {145, 13}, WindowWidgetType::Label,        WindowColour::Secondary, STR_OPTIONS_MUSIC_LABEL, STR_TITLE_MUSIC_TIP  ), // Title music label
    MakeWidget({155, 127}, {145, 13}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                ), // Title music
    MakeWidget({288, 128}, { 11, 11}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,      STR_TITLE_MUSIC_TIP  ),
    MakeWidget({155,  68}, {145, 13}, WindowWidgetType::Scroll,       WindowColour::Secondary, SCROLL_HORIZONTAL                             ), // Master volume
    MakeWidget({155,  83}, {145, 13}, WindowWidgetType::Scroll,       WindowColour::Secondary, SCROLL_HORIZONTAL                             ), // Sound effect volume
    MakeWidget({155,  98}, {145, 13}, WindowWidgetType::Scroll,       WindowColour::Secondary, SCROLL_HORIZONTAL                             ), // Music volume
    WIDGETS_END,
};

static rct_widget window_options_controls_and_interface_widgets[] = {
    MAIN_OPTIONS_WIDGETS,
#define CONTROLS_GROUP_START 53
    MakeWidget({  5, CONTROLS_GROUP_START +  0}, {300, 92}, WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CONTROLS_GROUP                                          ), // Controls group
    MakeWidget({ 10, CONTROLS_GROUP_START + 13}, {290, 14}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_SCREEN_EDGE_SCROLLING,   STR_SCREEN_EDGE_SCROLLING_TIP  ), // Edge scrolling
    MakeWidget({ 10, CONTROLS_GROUP_START + 30}, {290, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_TRAP_MOUSE,              STR_TRAP_MOUSE_TIP             ), // Trap mouse
    MakeWidget({ 10, CONTROLS_GROUP_START + 45}, {290, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_INVERT_RIGHT_MOUSE_DRAG, STR_INVERT_RIGHT_MOUSE_DRAG_TIP), // Invert right mouse dragging
    MakeWidget({ 10, CONTROLS_GROUP_START + 60}, {290, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_ZOOM_TO_CURSOR,          STR_ZOOM_TO_CURSOR_TIP         ), // Zoom to cursor
    MakeWidget({155, CONTROLS_GROUP_START + 75}, {145, 13}, WindowWidgetType::Button,   WindowColour::Secondary, STR_HOTKEY,                  STR_HOTKEY_TIP                 ), // Set hotkeys buttons
#undef CONTROLS_GROUP_START
#define THEMES_GROUP_START 148
    MakeWidget({  5, THEMES_GROUP_START +  0}, {300, 48}, WindowWidgetType::Groupbox,     WindowColour::Secondary, STR_THEMES_GROUP                                          ), // Themes group
    MakeWidget({ 10, THEMES_GROUP_START + 14}, {145, 12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_THEMES_LABEL_CURRENT_THEME, STR_CURRENT_THEME_TIP     ), // Themes
    MakeWidget({155, THEMES_GROUP_START + 14}, {145, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_STRING                                                ),
    MakeWidget({288, THEMES_GROUP_START + 15}, { 11, 10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,             STR_CURRENT_THEME_TIP     ),
    MakeWidget({155, THEMES_GROUP_START + 30}, {145, 13}, WindowWidgetType::Button,       WindowColour::Secondary, STR_EDIT_THEMES_BUTTON,         STR_EDIT_THEMES_BUTTON_TIP), // Themes button
#undef THEMES_GROUP_START
#define TOOLBAR_GROUP_START 200
    MakeWidget({  5, TOOLBAR_GROUP_START +  0}, {300, 92}, WindowWidgetType::Groupbox, WindowColour::Secondary, STR_TOOLBAR_BUTTONS_GROUP                                                   ), // Toolbar buttons group
    MakeWidget({ 10, TOOLBAR_GROUP_START + 14}, {280, 12}, WindowWidgetType::Label,    WindowColour::Secondary, STR_SHOW_TOOLBAR_BUTTONS_FOR                                                ),
    MakeWidget({ 24, TOOLBAR_GROUP_START + 31}, {122, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_FINANCES_BUTTON_ON_TOOLBAR,      STR_FINANCES_BUTTON_ON_TOOLBAR_TIP     ), // Finances
    MakeWidget({ 24, TOOLBAR_GROUP_START + 46}, {122, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_RESEARCH_BUTTON_ON_TOOLBAR,      STR_RESEARCH_BUTTON_ON_TOOLBAR_TIP     ), // Research
    MakeWidget({155, TOOLBAR_GROUP_START + 31}, {145, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_CHEATS_BUTTON_ON_TOOLBAR,        STR_CHEATS_BUTTON_ON_TOOLBAR_TIP       ), // Cheats
    MakeWidget({155, TOOLBAR_GROUP_START + 46}, {145, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_SHOW_RECENT_MESSAGES_ON_TOOLBAR, STR_SHOW_RECENT_MESSAGES_ON_TOOLBAR_TIP), // Recent messages
    MakeWidget({ 24, TOOLBAR_GROUP_START + 61}, {162, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_MUTE_BUTTON_ON_TOOLBAR,          STR_MUTE_BUTTON_ON_TOOLBAR_TIP         ), // Mute
    MakeWidget({155, TOOLBAR_GROUP_START + 61}, {145, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_CHAT_BUTTON_ON_TOOLBAR,          STR_CHAT_BUTTON_ON_TOOLBAR_TIP         ), // Chat
    MakeWidget({ 24, TOOLBAR_GROUP_START + 76}, {122, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_ZOOM_BUTTON_ON_TOOLBAR,          STR_ZOOM_BUTTON_ON_TOOLBAR_TIP         ), // Zoom
    WIDGETS_END,
#undef TOOLBAR_GROUP_START
};

#define TITLE_SEQUENCE_START 53
#define SCENARIO_START (TITLE_SEQUENCE_START + 35)
#define SCENARIO_OPTIONS_START (SCENARIO_START + 55)
#define TWEAKS_START (SCENARIO_OPTIONS_START + 39)

static rct_widget window_options_misc_widgets[] = {
    MAIN_OPTIONS_WIDGETS,
    MakeWidget(         {  5, TITLE_SEQUENCE_START +  0}, {300, 31}, WindowWidgetType::Groupbox,     WindowColour::Secondary, STR_OPTIONS_TITLE_SEQUENCE                        ),
    MakeDropdownWidgets({ 10, TITLE_SEQUENCE_START + 15}, {290, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_STRINGID,               STR_TITLE_SEQUENCE_TIP), // Title sequence dropdown

    MakeWidget({  5,  SCENARIO_START + 0}, {300, 51}, WindowWidgetType::Groupbox,     WindowColour::Secondary, STR_OPTIONS_SCENARIO_SELECTION                            ),
    MakeWidget({ 10, SCENARIO_START + 16}, {165, 12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_OPTIONS_SCENARIO_GROUPING,  STR_SCENARIO_GROUPING_TIP ),
    MakeWidget({175, SCENARIO_START + 15}, {125, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                            ), // Scenario select mode
    MakeWidget({288, SCENARIO_START + 16}, { 11, 10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,             STR_SCENARIO_GROUPING_TIP ),
    MakeWidget({ 25, SCENARIO_START + 30}, {275, 16}, WindowWidgetType::Checkbox,     WindowColour::Tertiary , STR_OPTIONS_SCENARIO_UNLOCKING, STR_SCENARIO_UNLOCKING_TIP), // Unlocking of scenarios

    MakeWidget({ 5,  SCENARIO_OPTIONS_START + 0}, {300, 35}, WindowWidgetType::Groupbox, WindowColour::Secondary, STR_SCENARIO_OPTIONS                                ),
    MakeWidget({10, SCENARIO_OPTIONS_START + 15}, {290, 15}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_ALLOW_EARLY_COMPLETION, STR_EARLY_COMPLETION_TIP), // Allow early scenario completion

    MakeWidget({  5,  TWEAKS_START + 0}, {300, 81}, WindowWidgetType::Groupbox,     WindowColour::Secondary, STR_OPTIONS_TWEAKS                                                  ),
    MakeWidget({ 10, TWEAKS_START + 15}, {290, 15}, WindowWidgetType::Checkbox,     WindowColour::Tertiary , STR_REAL_NAME,            STR_REAL_NAME_TIP                         ), // Show 'real' names of guests
    MakeWidget({ 10, TWEAKS_START + 30}, {290, 15}, WindowWidgetType::Checkbox,     WindowColour::Tertiary , STR_AUTO_STAFF_PLACEMENT, STR_AUTO_STAFF_PLACEMENT_TIP              ), // Auto staff placement
    MakeWidget({ 10, TWEAKS_START + 45}, {290, 15}, WindowWidgetType::Checkbox,     WindowColour::Tertiary , STR_AUTO_OPEN_SHOPS,      STR_AUTO_OPEN_SHOPS_TIP                   ), // Automatically open shops & stalls
    MakeWidget({ 10, TWEAKS_START + 62}, {165, 12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_DEFAULT_INSPECTION_INTERVAL, STR_DEFAULT_INSPECTION_INTERVAL_TIP),
    MakeWidget({175, TWEAKS_START + 61}, {125, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                                      ), // Default inspection time dropdown
    MakeWidget({288, TWEAKS_START + 62}, { 11, 10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,       STR_DEFAULT_INSPECTION_INTERVAL_TIP       ), // Default inspection time dropdown button
    WIDGETS_END,
};

#undef TWEAKS_START
#undef SCENARIO_OPTIONS_START
#undef SCENARIO_START
#undef TITLE_SEQUENCE_START

static rct_widget window_options_advanced_widgets[] = {
    MAIN_OPTIONS_WIDGETS,
    MakeWidget        ({ 10,  54}, {290, 12}, WindowWidgetType::Checkbox,     WindowColour::Tertiary , STR_ENABLE_DEBUGGING_TOOLS,                STR_ENABLE_DEBUGGING_TOOLS_TIP               ), // Enable debugging tools
    MakeWidget        ({ 10,  69}, {290, 12}, WindowWidgetType::Checkbox,     WindowColour::Tertiary , STR_ALLOW_LOADING_WITH_INCORRECT_CHECKSUM, STR_ALLOW_LOADING_WITH_INCORRECT_CHECKSUM_TIP), // Allow loading with incorrect checksum
    MakeWidget        ({ 10,  84}, {290, 12}, WindowWidgetType::Checkbox,     WindowColour::Tertiary , STR_SAVE_PLUGIN_DATA,                      STR_SAVE_PLUGIN_DATA_TIP                     ), // Export plug-in objects with saved games
    MakeWidget        ({ 10,  99}, {290, 12}, WindowWidgetType::Checkbox,     WindowColour::Tertiary , STR_STAY_CONNECTED_AFTER_DESYNC,           STR_STAY_CONNECTED_AFTER_DESYNC_TIP          ), // Do not disconnect after the client desynchronises with the server
    MakeWidget        ({ 10, 114}, {290, 12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_ALWAYS_NATIVE_LOADSAVE,                STR_ALWAYS_NATIVE_LOADSAVE_TIP               ), // Use native load/save window
    MakeWidget        ({ 24, 131}, {135, 12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_OPTIONS_AUTOSAVE_FREQUENCY_LABEL,      STR_AUTOSAVE_FREQUENCY_TIP                   ),
    MakeWidget        ({165, 130}, {135, 13}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                                                          ), // Autosave dropdown
    MakeWidget        ({288, 131}, { 11, 11}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,                        STR_AUTOSAVE_FREQUENCY_TIP                   ), // Autosave dropdown button
    MakeWidget        ({ 24, 151}, {135, 12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_AUTOSAVE_AMOUNT,                       STR_AUTOSAVE_AMOUNT_TIP                      ),
    MakeSpinnerWidgets({165, 150}, {135, 12}, WindowWidgetType::Spinner,      WindowColour::Secondary, STR_NONE,                                  STR_AUTOSAVE_AMOUNT_TIP                      ), // Autosave amount spinner
    MakeWidget        ({ 23, 169}, {276, 12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_PATH_TO_RCT1,                          STR_PATH_TO_RCT1_TIP                         ), // RCT 1 path text
    MakeWidget        ({ 24, 184}, {266, 14}, WindowWidgetType::Button,       WindowColour::Secondary, STR_NONE,                                  STR_STRING_TOOLTIP                           ), // RCT 1 path button
    MakeWidget        ({289, 184}, { 11, 14}, WindowWidgetType::Button,       WindowColour::Secondary, STR_CLOSE_X,                               STR_PATH_TO_RCT1_CLEAR_TIP                   ), // RCT 1 path clear button
    WIDGETS_END,
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
// clang-format on

class OptionsWindow final : public Window
{
public:
    void OnOpen() override
    {
        widgets = window_options_display_widgets;
        page = WINDOW_OPTIONS_PAGE_DISPLAY;
        frame_no = 0;
        InitScrollWidgets();
    }

    void OnMouseUp(rct_widgetindex widgetIndex) override
    {
        if (widgetIndex < WIDX_PAGE_START)
            CommonMouseUp(widgetIndex);
        else
        {
            switch (page)
            {
                case WINDOW_OPTIONS_PAGE_DISPLAY:
                    DisplayMouseUp(widgetIndex);
                    break;
                case WINDOW_OPTIONS_PAGE_RENDERING:
                    RenderingMouseUp(widgetIndex);
                    break;
                case WINDOW_OPTIONS_PAGE_AUDIO:
                    AudioMouseUp(widgetIndex);
                    break;
                case WINDOW_OPTIONS_PAGE_CONTROLS_AND_INTERFACE:
                    ControlsMouseUp(widgetIndex);
                    break;
                case WINDOW_OPTIONS_PAGE_MISC:
                    MiscMouseUp(widgetIndex);
                    break;
                case WINDOW_OPTIONS_PAGE_ADVANCED:
                    AdvancedMouseUp(widgetIndex);
                    break;
                case WINDOW_OPTIONS_PAGE_CULTURE:
                default:
                    break;
            }
        }
    }

    void OnMouseDown(rct_widgetindex widgetIndex) override
    {
        switch (page)
        {
            case WINDOW_OPTIONS_PAGE_DISPLAY:
                DisplayMouseDown(widgetIndex);
                break;
            case WINDOW_OPTIONS_PAGE_RENDERING:
                RenderingMouseDown(widgetIndex);
                break;
            case WINDOW_OPTIONS_PAGE_CULTURE:
                CultureMouseDown(widgetIndex);
                break;
            case WINDOW_OPTIONS_PAGE_AUDIO:
                AudioMouseDown(widgetIndex);
                break;
            case WINDOW_OPTIONS_PAGE_CONTROLS_AND_INTERFACE:
                ControlsMouseDown(widgetIndex);
                break;
            case WINDOW_OPTIONS_PAGE_MISC:
                MiscMouseDown(widgetIndex);
                break;
            case WINDOW_OPTIONS_PAGE_ADVANCED:
                AdvancedMouseDown(widgetIndex);
                break;
            default:
                break;
        }
    }

    void OnDropdown(rct_widgetindex widgetIndex, int32_t dropdownIndex) override
    {
        if (dropdownIndex == -1)
            return;

        switch (page)
        {
            case WINDOW_OPTIONS_PAGE_DISPLAY:
                DisplayDropdown(widgetIndex, dropdownIndex);
                break;
            case WINDOW_OPTIONS_PAGE_RENDERING:
                RenderingDropdown(widgetIndex, dropdownIndex);
                break;
            case WINDOW_OPTIONS_PAGE_CULTURE:
                CultureDropdown(widgetIndex, dropdownIndex);
                break;
            case WINDOW_OPTIONS_PAGE_AUDIO:
                AudioDropdown(widgetIndex, dropdownIndex);
                break;
            case WINDOW_OPTIONS_PAGE_CONTROLS_AND_INTERFACE:
                ControlsDropdown(widgetIndex, dropdownIndex);
                break;
            case WINDOW_OPTIONS_PAGE_MISC:
                MiscDropdown(widgetIndex, dropdownIndex);
                break;
            case WINDOW_OPTIONS_PAGE_ADVANCED:
                AdvancedDropdown(widgetIndex, dropdownIndex);
                break;
            default:
                break;
        }
    }

    void OnPrepareDraw() override
    {
        CommonPrepareDrawBefore();

        switch (page)
        {
            case WINDOW_OPTIONS_PAGE_DISPLAY:
                DisplayPrepareDraw();
                break;
            case WINDOW_OPTIONS_PAGE_RENDERING:
                RenderingPrepareDraw();
                break;
            case WINDOW_OPTIONS_PAGE_CULTURE:
                CulturePrepareDraw();
                break;
            case WINDOW_OPTIONS_PAGE_AUDIO:
                AudioPrepareDraw();
                break;
            case WINDOW_OPTIONS_PAGE_CONTROLS_AND_INTERFACE:
                ControlsPrepareDraw();
                break;
            case WINDOW_OPTIONS_PAGE_MISC:
                MiscPrepareDraw();
                break;
            case WINDOW_OPTIONS_PAGE_ADVANCED:
                AdvancedPrepareDraw();
                break;
            default:
                break;
        }

        CommonPrepareDrawAfter();
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        DrawWidgets(dpi);
        DrawTabImages(&dpi);

        switch (page)
        {
            case WINDOW_OPTIONS_PAGE_DISPLAY:
                DisplayDraw(&dpi);
                break;
            case WINDOW_OPTIONS_PAGE_ADVANCED:
                AdvancedDraw(&dpi);
                break;
            default:
                break;
        }
    }

    void OnUpdate() override
    {
        CommonUpdate();

        switch (page)
        {
            case WINDOW_OPTIONS_PAGE_AUDIO:
                AudioUpdate();
                break;
            case WINDOW_OPTIONS_PAGE_DISPLAY:
            case WINDOW_OPTIONS_PAGE_RENDERING:
            case WINDOW_OPTIONS_PAGE_CULTURE:
            case WINDOW_OPTIONS_PAGE_CONTROLS_AND_INTERFACE:
            case WINDOW_OPTIONS_PAGE_MISC:
            case WINDOW_OPTIONS_PAGE_ADVANCED:
            default:
                break;
        }
    }

    ScreenSize OnScrollGetSize(int32_t scrollIndex) override
    {
        switch (page)
        {
            case WINDOW_OPTIONS_PAGE_AUDIO:
                return AudioScrollGetSize(scrollIndex);
            case WINDOW_OPTIONS_PAGE_DISPLAY:
            case WINDOW_OPTIONS_PAGE_RENDERING:
            case WINDOW_OPTIONS_PAGE_CULTURE:
            case WINDOW_OPTIONS_PAGE_CONTROLS_AND_INTERFACE:
            case WINDOW_OPTIONS_PAGE_MISC:
            case WINDOW_OPTIONS_PAGE_ADVANCED:
            default:
                return { WW, WH };
        }
    }

    OpenRCT2String OnTooltip(rct_widgetindex widgetIndex, StringId fallback) override
    {
        if (page == WINDOW_OPTIONS_PAGE_ADVANCED)
            return AdvancedTooltip(widgetIndex, fallback);

        return rct_window::OnTooltip(widgetIndex, fallback);
    }

private:
#pragma region Common events
    void CommonMouseUp(rct_widgetindex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                window_close(*this);
                break;
            case WIDX_TAB_DISPLAY:
            case WIDX_TAB_RENDERING:
            case WIDX_TAB_CULTURE:
            case WIDX_TAB_AUDIO:
            case WIDX_TAB_CONTROLS_AND_INTERFACE:
            case WIDX_TAB_MISC:
            case WIDX_TAB_ADVANCED:
                SetPage(widgetIndex - WIDX_FIRST_TAB);
                break;
        }
    }

    void CommonPrepareDrawBefore()
    {
        if (window_options_page_widgets[page] != widgets)
        {
            widgets = window_options_page_widgets[page];
            InitScrollWidgets();
        }
        SetPressedTab();

        disabled_widgets = 0;
        auto hasFilePicker = OpenRCT2::GetContext()->GetUiContext()->HasFilePicker();
        if (!hasFilePicker)
        {
            disabled_widgets |= (1ULL << WIDX_ALWAYS_NATIVE_LOADSAVE);
            widgets[WIDX_ALWAYS_NATIVE_LOADSAVE].type = WindowWidgetType::Empty;
        }
    }

    void CommonPrepareDrawAfter()
    {
        // Automatically adjust window height to fit widgets
        int32_t y = 0;
        for (auto widget = &widgets[WIDX_PAGE_START]; widget->type != WindowWidgetType::Last; widget++)
        {
            y = std::max<int32_t>(y, widget->bottom);
        }
        height = y + 6;
        widgets[WIDX_BACKGROUND].bottom = height - 1;
        widgets[WIDX_PAGE_BACKGROUND].bottom = height - 1;
    }

    void CommonUpdate()
    {
        // Tab animation
        frame_no++;
        InvalidateWidget(WIDX_FIRST_TAB + page);
    }
#pragma endregion

#pragma region Display tab events
    void DisplayMouseUp(rct_widgetindex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_UNCAP_FPS_CHECKBOX:
                gConfigGeneral.uncap_fps ^= 1;
                drawing_engine_set_vsync(gConfigGeneral.use_vsync);
                config_save_default();
                Invalidate();
                break;
            case WIDX_USE_VSYNC_CHECKBOX:
                gConfigGeneral.use_vsync ^= 1;
                drawing_engine_set_vsync(gConfigGeneral.use_vsync);
                config_save_default();
                Invalidate();
                break;
            case WIDX_SHOW_FPS_CHECKBOX:
                gConfigGeneral.show_fps ^= 1;
                config_save_default();
                Invalidate();
                break;
            case WIDX_MULTITHREADING_CHECKBOX:
                gConfigGeneral.multithreading ^= 1;
                config_save_default();
                Invalidate();
                break;
            case WIDX_MINIMIZE_FOCUS_LOSS:
                gConfigGeneral.minimize_fullscreen_focus_loss ^= 1;
                RefreshVideo(false);
                config_save_default();
                Invalidate();
                break;
            case WIDX_STEAM_OVERLAY_PAUSE:
                gConfigGeneral.steam_overlay_pause ^= 1;
                config_save_default();
                Invalidate();
                break;
            case WIDX_DISABLE_SCREENSAVER_LOCK:
                gConfigGeneral.disable_screensaver ^= 1;
                ApplyScreenSaverLockSetting();
                config_save_default();
                Invalidate();
                break;
        }
    }

    void DisplayMouseDown(rct_widgetindex widgetIndex)
    {
        rct_widget* widget = &widgets[widgetIndex - 1];

        switch (widgetIndex)
        {
            case WIDX_RESOLUTION_DROPDOWN:
            {
                const auto& resolutions = OpenRCT2::GetContext()->GetUiContext()->GetFullscreenResolutions();

                int32_t selectedResolution = -1;
                for (size_t i = 0; i < resolutions.size(); i++)
                {
                    const Resolution& resolution = resolutions[i];

                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;

                    uint16_t* args = reinterpret_cast<uint16_t*>(&gDropdownItems[i].Args);
                    args[0] = STR_RESOLUTION_X_BY_Y;
                    args[1] = resolution.Width;
                    args[2] = resolution.Height;

                    if (resolution.Width == gConfigGeneral.fullscreen_width
                        && resolution.Height == gConfigGeneral.fullscreen_height)
                    {
                        selectedResolution = static_cast<int32_t>(i);
                    }
                }

                ShowDropdown(widget, static_cast<int32_t>(resolutions.size()));

                if (selectedResolution != -1 && selectedResolution < 32)
                {
                    Dropdown::SetChecked(selectedResolution, true);
                }
            }

            break;
            case WIDX_FULLSCREEN_DROPDOWN:
                gDropdownItems[0].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[1].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[2].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[0].Args = STR_OPTIONS_DISPLAY_WINDOWED;
                gDropdownItems[1].Args = STR_OPTIONS_DISPLAY_FULLSCREEN;
                gDropdownItems[2].Args = STR_OPTIONS_DISPLAY_FULLSCREEN_BORDERLESS;

                ShowDropdown(widget, 3);

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
                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[i].Args = DrawingEngineStringIds[i];
                }
                ShowDropdown(widget, numItems);
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
        }
    }

    void DisplayDropdown(rct_widgetindex widgetIndex, int32_t dropdownIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_RESOLUTION_DROPDOWN:
            {
                const auto& resolutions = OpenRCT2::GetContext()->GetUiContext()->GetFullscreenResolutions();

                const Resolution& resolution = resolutions[dropdownIndex];
                if (resolution.Width != gConfigGeneral.fullscreen_width
                    || resolution.Height != gConfigGeneral.fullscreen_height)
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
                    RefreshVideo(recreate_window);
                    config_save_default();
                    Invalidate();
                }
                break;
        }
    }

    void DisplayPrepareDraw()
    {
        // Resolution dropdown caption.
        auto ft = Formatter::Common();
        ft.Increment(16);
        ft.Add<uint16_t>(static_cast<uint16_t>(gConfigGeneral.fullscreen_width));
        ft.Add<uint16_t>(static_cast<uint16_t>(gConfigGeneral.fullscreen_height));

        // Disable resolution dropdown on "Windowed" and "Fullscreen (desktop)"
        if (gConfigGeneral.fullscreen_mode != static_cast<int32_t>(OpenRCT2::Ui::FULLSCREEN_MODE::FULLSCREEN))
        {
            disabled_widgets |= (1ULL << WIDX_RESOLUTION_DROPDOWN);
            disabled_widgets |= (1ULL << WIDX_RESOLUTION);
            disabled_widgets |= (1ULL << WIDX_RESOLUTION_LABEL);
        }
        else
        {
            disabled_widgets &= ~(1ULL << WIDX_RESOLUTION_DROPDOWN);
            disabled_widgets &= ~(1ULL << WIDX_RESOLUTION);
            disabled_widgets &= ~(1ULL << WIDX_RESOLUTION_LABEL);
        }

        // Disable Steam Overlay checkbox when using software rendering.
        if (gConfigGeneral.drawing_engine == DrawingEngine::Software)
        {
            disabled_widgets |= (1ULL << WIDX_STEAM_OVERLAY_PAUSE);
        }
        else
        {
            disabled_widgets &= ~(1ULL << WIDX_STEAM_OVERLAY_PAUSE);
        }

        // Disable changing VSync for Software engine, as we can't control its use of VSync
        if (gConfigGeneral.drawing_engine == DrawingEngine::Software)
        {
            disabled_widgets |= (1ULL << WIDX_USE_VSYNC_CHECKBOX);
        }
        else
        {
            disabled_widgets &= ~(1ULL << WIDX_USE_VSYNC_CHECKBOX);
        }

        SetCheckboxValue(WIDX_UNCAP_FPS_CHECKBOX, gConfigGeneral.uncap_fps);
        SetCheckboxValue(WIDX_USE_VSYNC_CHECKBOX, gConfigGeneral.use_vsync);
        SetCheckboxValue(WIDX_SHOW_FPS_CHECKBOX, gConfigGeneral.show_fps);
        SetCheckboxValue(WIDX_MULTITHREADING_CHECKBOX, gConfigGeneral.multithreading);
        SetCheckboxValue(WIDX_MINIMIZE_FOCUS_LOSS, gConfigGeneral.minimize_fullscreen_focus_loss);
        SetCheckboxValue(WIDX_STEAM_OVERLAY_PAUSE, gConfigGeneral.steam_overlay_pause);
        SetCheckboxValue(WIDX_DISABLE_SCREENSAVER_LOCK, gConfigGeneral.disable_screensaver);

        // Dropdown captions for straightforward strings.
        widgets[WIDX_FULLSCREEN].text = FullscreenModeNames[gConfigGeneral.fullscreen_mode];
        widgets[WIDX_DRAWING_ENGINE].text = DrawingEngineStringIds[EnumValue(gConfigGeneral.drawing_engine)];
    }

    void DisplayDraw(rct_drawpixelinfo* dpi)
    {
        auto ft = Formatter();
        ft.Add<int32_t>(static_cast<int32_t>(gConfigGeneral.window_scale * 100));
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ widgets[WIDX_SCALE].left + 1, widgets[WIDX_SCALE].top + 1 },
            STR_WINDOW_COLOUR_2_COMMA2DP32, ft, { colours[1] });
    }
#pragma endregion

#pragma region Rendering tab events
    void RenderingMouseUp(rct_widgetindex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_TILE_SMOOTHING_CHECKBOX:
                gConfigGeneral.landscape_smoothing ^= 1;
                config_save_default();
                gfx_invalidate_screen();
                break;
            case WIDX_GRIDLINES_CHECKBOX:
            {
                gConfigGeneral.always_show_gridlines ^= 1;
                config_save_default();
                gfx_invalidate_screen();
                rct_window* mainWindow = window_get_main();
                if (mainWindow != nullptr)
                {
                    if (gConfigGeneral.always_show_gridlines)
                        mainWindow->viewport->flags |= VIEWPORT_FLAG_GRIDLINES;
                    else
                        mainWindow->viewport->flags &= ~VIEWPORT_FLAG_GRIDLINES;
                }
                break;
            }
            case WIDX_DAY_NIGHT_CHECKBOX:
                gConfigGeneral.day_night_cycle ^= 1;
                config_save_default();
                Invalidate();
                break;
            case WIDX_ENABLE_LIGHT_FX_CHECKBOX:
                gConfigGeneral.enable_light_fx ^= 1;
                config_save_default();
                Invalidate();
                break;
            case WIDX_ENABLE_LIGHT_FX_FOR_VEHICLES_CHECKBOX:
                gConfigGeneral.enable_light_fx_for_vehicles ^= 1;
                config_save_default();
                Invalidate();
                break;
            case WIDX_UPPER_CASE_BANNERS_CHECKBOX:
                gConfigGeneral.upper_case_banners ^= 1;
                config_save_default();
                Invalidate();
                scrolling_text_invalidate();
                break;
            case WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX:
                gConfigGeneral.disable_lightning_effect ^= 1;
                config_save_default();
                Invalidate();
                break;
            case WIDX_RENDER_WEATHER_EFFECTS_CHECKBOX:
                gConfigGeneral.render_weather_effects ^= 1;
                gConfigGeneral.render_weather_gloom = gConfigGeneral.render_weather_effects;
                config_save_default();
                Invalidate();
                gfx_invalidate_screen();
                break;
            case WIDX_SHOW_GUEST_PURCHASES_CHECKBOX:
                gConfigGeneral.show_guest_purchases ^= 1;
                config_save_default();
                Invalidate();
                break;
            case WIDX_TRANSPARENT_SCREENSHOTS_CHECKBOX:
                gConfigGeneral.transparent_screenshot ^= 1;
                config_save_default();
                Invalidate();
                break;
        }
    }

    void RenderingMouseDown(rct_widgetindex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_VIRTUAL_FLOOR_DROPDOWN:
                gDropdownItems[0].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[1].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[2].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[0].Args = STR_VIRTUAL_FLOOR_STYLE_DISABLED;
                gDropdownItems[1].Args = STR_VIRTUAL_FLOOR_STYLE_TRANSPARENT;
                gDropdownItems[2].Args = STR_VIRTUAL_FLOOR_STYLE_GLASSY;

                rct_widget* widget = &widgets[widgetIndex - 1];
                ShowDropdown(widget, 3);

                Dropdown::SetChecked(static_cast<int32_t>(gConfigGeneral.virtual_floor_style), true);
                break;
        }
    }

    void RenderingDropdown(rct_widgetindex widgetIndex, int32_t dropdownIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_VIRTUAL_FLOOR_DROPDOWN:
                gConfigGeneral.virtual_floor_style = static_cast<VirtualFloorStyles>(dropdownIndex);
                config_save_default();
                break;
        }
    }

    void RenderingPrepareDraw()
    {
        SetCheckboxValue(WIDX_TILE_SMOOTHING_CHECKBOX, gConfigGeneral.landscape_smoothing);
        SetCheckboxValue(WIDX_GRIDLINES_CHECKBOX, gConfigGeneral.always_show_gridlines);
        SetCheckboxValue(WIDX_DAY_NIGHT_CHECKBOX, gConfigGeneral.day_night_cycle);
        SetCheckboxValue(WIDX_SHOW_GUEST_PURCHASES_CHECKBOX, gConfigGeneral.show_guest_purchases);
        SetCheckboxValue(WIDX_TRANSPARENT_SCREENSHOTS_CHECKBOX, gConfigGeneral.transparent_screenshot);
        SetCheckboxValue(WIDX_UPPER_CASE_BANNERS_CHECKBOX, gConfigGeneral.upper_case_banners);

        static constexpr StringId _virtualFloorStyleStrings[] = {
            STR_VIRTUAL_FLOOR_STYLE_DISABLED,
            STR_VIRTUAL_FLOOR_STYLE_TRANSPARENT,
            STR_VIRTUAL_FLOOR_STYLE_GLASSY,
        };

        widgets[WIDX_VIRTUAL_FLOOR].text = _virtualFloorStyleStrings[static_cast<int32_t>(gConfigGeneral.virtual_floor_style)];

        SetCheckboxValue(WIDX_ENABLE_LIGHT_FX_CHECKBOX, gConfigGeneral.enable_light_fx);
        if (gConfigGeneral.day_night_cycle && gConfigGeneral.drawing_engine == DrawingEngine::SoftwareWithHardwareDisplay)
        {
            disabled_widgets &= ~(1ULL << WIDX_ENABLE_LIGHT_FX_CHECKBOX);
        }
        else
        {
            disabled_widgets |= (1ULL << WIDX_ENABLE_LIGHT_FX_CHECKBOX);
            gConfigGeneral.enable_light_fx = false;
        }

        SetCheckboxValue(WIDX_ENABLE_LIGHT_FX_FOR_VEHICLES_CHECKBOX, gConfigGeneral.enable_light_fx_for_vehicles);
        if (gConfigGeneral.day_night_cycle && gConfigGeneral.drawing_engine == DrawingEngine::SoftwareWithHardwareDisplay
            && gConfigGeneral.enable_light_fx)
        {
            disabled_widgets &= ~(1ULL << WIDX_ENABLE_LIGHT_FX_FOR_VEHICLES_CHECKBOX);
        }
        else
        {
            disabled_widgets |= (1ULL << WIDX_ENABLE_LIGHT_FX_FOR_VEHICLES_CHECKBOX);
            gConfigGeneral.enable_light_fx_for_vehicles = false;
        }

        WidgetSetCheckboxValue(
            *this, WIDX_RENDER_WEATHER_EFFECTS_CHECKBOX,
            gConfigGeneral.render_weather_effects || gConfigGeneral.render_weather_gloom);
        SetCheckboxValue(WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX, gConfigGeneral.disable_lightning_effect);
        if (!gConfigGeneral.render_weather_effects && !gConfigGeneral.render_weather_gloom)
        {
            SetCheckboxValue(WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX, true);
            disabled_widgets |= (1ULL << WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX);
        }
        else
        {
            disabled_widgets &= ~(1ULL << WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX);
        }
    }

#pragma endregion

#pragma region Culture tab events
    void CultureMouseDown(rct_widgetindex widgetIndex)
    {
        rct_widget* widget = &widgets[widgetIndex - 1];

        switch (widgetIndex)
        {
            case WIDX_HEIGHT_LABELS_DROPDOWN:
                gDropdownItems[0].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[1].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[0].Args = STR_HEIGHT_IN_UNITS;
                gDropdownItems[1].Args = STR_REAL_VALUES;

                ShowDropdown(widget, 2);

                Dropdown::SetChecked(gConfigGeneral.show_height_as_units ? 0 : 1, true);
                break;
            case WIDX_CURRENCY_DROPDOWN:
            {
                // All the currencies plus the separator
                constexpr auto numItems = EnumValue(CurrencyType::Count) + 1;

                // All the currencies except custom currency
                size_t numOrdinaryCurrencies = EnumValue(CurrencyType::Count) - 1;

                for (size_t i = 0; i < numOrdinaryCurrencies; i++)
                {
                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[i].Args = CurrencyDescriptors[i].stringId;
                }

                gDropdownItems[numOrdinaryCurrencies].Format = Dropdown::SeparatorString;

                gDropdownItems[numOrdinaryCurrencies + 1].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[numOrdinaryCurrencies + 1].Args = CurrencyDescriptors[EnumValue(CurrencyType::Custom)].stringId;

                ShowDropdown(widget, numItems);

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
                gDropdownItems[0].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[1].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[2].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[0].Args = STR_IMPERIAL;
                gDropdownItems[1].Args = STR_METRIC;
                gDropdownItems[2].Args = STR_SI;

                ShowDropdown(widget, 3);

                Dropdown::SetChecked(static_cast<int32_t>(gConfigGeneral.measurement_format), true);
                break;
            case WIDX_TEMPERATURE_DROPDOWN:
                gDropdownItems[0].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[1].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[0].Args = STR_CELSIUS;
                gDropdownItems[1].Args = STR_FAHRENHEIT;

                ShowDropdown(widget, 2);

                Dropdown::SetChecked(static_cast<int32_t>(gConfigGeneral.temperature_format), true);
                break;
            case WIDX_LANGUAGE_DROPDOWN:
                for (size_t i = 1; i < LANGUAGE_COUNT; i++)
                {
                    gDropdownItems[i - 1].Format = STR_OPTIONS_DROPDOWN_ITEM;
                    gDropdownItems[i - 1].Args = reinterpret_cast<uintptr_t>(LanguagesDescriptors[i].native_name);
                }
                ShowDropdown(widget, LANGUAGE_COUNT - 1);
                Dropdown::SetChecked(LocalisationService_GetCurrentLanguage() - 1, true);
                break;
            case WIDX_DATE_FORMAT_DROPDOWN:
                for (size_t i = 0; i < 4; i++)
                {
                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[i].Args = DateFormatStringIds[i];
                }
                ShowDropdown(widget, 4);
                Dropdown::SetChecked(gConfigGeneral.date_format, true);
                break;
        }
    }

    void CultureDropdown(rct_widgetindex widgetIndex, int32_t dropdownIndex)
    {
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
                UpdateHeightMarkers();
                break;
            case WIDX_CURRENCY_DROPDOWN:
                if (dropdownIndex == EnumValue(CurrencyType::Custom) + 1)
                { // Add 1 because the separator occupies a position
                    gConfigGeneral.currency_format = static_cast<CurrencyType>(dropdownIndex - 1);
                    context_open_window(WindowClass::CustomCurrencyConfig);
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
                UpdateHeightMarkers();
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

    void CulturePrepareDraw()
    {
        // Language
        auto ft = Formatter::Common();
        ft.Add<char*>(LanguagesDescriptors[LocalisationService_GetCurrentLanguage()].native_name);

        // Currency: pounds, dollars, etc. (10 total)
        widgets[WIDX_CURRENCY].text = CurrencyDescriptors[EnumValue(gConfigGeneral.currency_format)].stringId;

        // Distance: metric / imperial / si
        {
            StringId stringId = STR_NONE;
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
            widgets[WIDX_DISTANCE].text = stringId;
        }

        // Date format
        widgets[WIDX_DATE_FORMAT].text = DateFormatStringIds[gConfigGeneral.date_format];

        // Temperature: celsius/fahrenheit
        widgets[WIDX_TEMPERATURE].text = gConfigGeneral.temperature_format == TemperatureUnit::Fahrenheit ? STR_FAHRENHEIT
                                                                                                          : STR_CELSIUS;

        // Height: units/real values
        widgets[WIDX_HEIGHT_LABELS].text = gConfigGeneral.show_height_as_units ? STR_HEIGHT_IN_UNITS : STR_REAL_VALUES;
    }

#pragma endregion

#pragma region Audio tab events
    void AudioMouseUp(rct_widgetindex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_SOUND_CHECKBOX:
                gConfigSound.sound_enabled = !gConfigSound.sound_enabled;
                config_save_default();
                Invalidate();
                break;

            case WIDX_MASTER_SOUND_CHECKBOX:
                gConfigSound.master_sound_enabled = !gConfigSound.master_sound_enabled;
                if (!gConfigSound.master_sound_enabled)
                    OpenRCT2::Audio::Pause();
                else
                    OpenRCT2::Audio::Resume();
                window_invalidate_by_class(WindowClass::TopToolbar);
                config_save_default();
                Invalidate();
                break;

            case WIDX_MUSIC_CHECKBOX:
                gConfigSound.ride_music_enabled = !gConfigSound.ride_music_enabled;
                if (!gConfigSound.ride_music_enabled)
                {
                    OpenRCT2::RideAudio::StopAllChannels();
                }
                config_save_default();
                Invalidate();
                break;

            case WIDX_AUDIO_FOCUS_CHECKBOX:
                gConfigSound.audio_focus = !gConfigSound.audio_focus;
                config_save_default();
                Invalidate();
                break;
        }
    }

    void AudioMouseDown(rct_widgetindex widgetIndex)
    {
        rct_widget* widget = &widgets[widgetIndex - 1];

        switch (widgetIndex)
        {
            case WIDX_SOUND_DROPDOWN:
                OpenRCT2::Audio::PopulateDevices();

                // populate the list with the sound devices
                for (int32_t i = 0; i < OpenRCT2::Audio::GetDeviceCount(); i++)
                {
                    gDropdownItems[i].Format = STR_OPTIONS_DROPDOWN_ITEM;
                    gDropdownItems[i].Args = reinterpret_cast<uintptr_t>(OpenRCT2::Audio::GetDeviceName(i).c_str());
                }

                ShowDropdown(widget, OpenRCT2::Audio::GetDeviceCount());

                Dropdown::SetChecked(OpenRCT2::Audio::GetCurrentDeviceIndex(), true);
                break;
            case WIDX_TITLE_MUSIC_DROPDOWN:
            {
                if (!IsRCT1TitleMusicAvailable())
                {
                    // Only show None and RCT2
                    int32_t numItems{};
                    gDropdownItems[numItems].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[numItems++].Args = TitleMusicNames[0];
                    gDropdownItems[numItems].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[numItems++].Args = TitleMusicNames[2];
                    ShowDropdown(widget, numItems);
                    if (gConfigSound.title_music == TitleMusicKind::None)
                        Dropdown::SetChecked(0, true);
                    else if (gConfigSound.title_music == TitleMusicKind::Rct2)
                        Dropdown::SetChecked(1, true);
                }
                else
                {
                    // Show None, RCT1, RCT2 and random
                    int32_t numItems{};
                    for (auto musicName : TitleMusicNames)
                    {
                        gDropdownItems[numItems].Format = STR_DROPDOWN_MENU_LABEL;
                        gDropdownItems[numItems++].Args = musicName;
                    }
                    ShowDropdown(widget, numItems);
                    Dropdown::SetChecked(EnumValue(gConfigSound.title_music), true);
                }
                break;
            }
        }
    }

    void AudioDropdown(rct_widgetindex widgetIndex, int32_t dropdownIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_SOUND_DROPDOWN:
                OpenRCT2::Audio::InitRideSounds(dropdownIndex);
                if (dropdownIndex < OpenRCT2::Audio::GetDeviceCount())
                {
                    auto audioContext = GetContext()->GetAudioContext();
                    if (dropdownIndex == 0)
                    {
                        audioContext->SetOutputDevice("");
                        gConfigSound.device = "";
                    }
                    else
                    {
                        const auto& deviceName = GetDeviceName(dropdownIndex);
                        audioContext->SetOutputDevice(deviceName);
                        gConfigSound.device = deviceName;
                    }
                    config_save_default();
                    OpenRCT2::Audio::PlayTitleMusic();
                }
                Invalidate();
                break;
            case WIDX_TITLE_MUSIC_DROPDOWN:
            {
                auto titleMusic = static_cast<TitleMusicKind>(dropdownIndex);
                if (!IsRCT1TitleMusicAvailable() && dropdownIndex != 0)
                {
                    titleMusic = TitleMusicKind::Rct2;
                }

                gConfigSound.title_music = titleMusic;
                config_save_default();
                Invalidate();

                OpenRCT2::Audio::StopTitleMusic();
                if (titleMusic != TitleMusicKind::None)
                {
                    OpenRCT2::Audio::PlayTitleMusic();
                }
                break;
            }
        }
    }

    void AudioUpdate()
    {
        const auto& masterVolumeWidget = widgets[WIDX_MASTER_VOLUME];
        const auto& masterVolumeScroll = scrolls[0];
        uint8_t masterVolume = GetScrollPercentage(masterVolumeWidget, masterVolumeScroll);
        if (masterVolume != gConfigSound.master_volume)
        {
            gConfigSound.master_volume = masterVolume;
            config_save_default();
            InvalidateWidget(WIDX_MASTER_VOLUME);
        }

        const auto& soundVolumeWidget = widgets[WIDX_MASTER_VOLUME];
        const auto& soundVolumeScroll = scrolls[1];
        uint8_t soundVolume = GetScrollPercentage(soundVolumeWidget, soundVolumeScroll);
        if (soundVolume != gConfigSound.sound_volume)
        {
            gConfigSound.sound_volume = soundVolume;
            config_save_default();
            InvalidateWidget(WIDX_SOUND_VOLUME);
        }

        const auto& musicVolumeWidget = widgets[WIDX_MASTER_VOLUME];
        const auto& musicVolumeScroll = scrolls[2];
        uint8_t rideMusicVolume = GetScrollPercentage(musicVolumeWidget, musicVolumeScroll);
        if (rideMusicVolume != gConfigSound.ride_music_volume)
        {
            gConfigSound.ride_music_volume = rideMusicVolume;
            config_save_default();
            InvalidateWidget(WIDX_MUSIC_VOLUME);
        }
    }

    ScreenSize AudioScrollGetSize(int32_t scrollIndex)
    {
        return { 500, 0 };
    }

    StringId GetTitleMusicName()
    {
        auto index = EnumValue(gConfigSound.title_music);
        if (index < 0 || static_cast<size_t>(index) >= std::size(TitleMusicNames))
        {
            index = EnumValue(TitleMusicKind::None);
        }
        return TitleMusicNames[index];
    }

    void AudioPrepareDraw()
    {
        // Sound device
        StringId audioDeviceStringId = STR_OPTIONS_SOUND_VALUE_DEFAULT;
        const char* audioDeviceName = nullptr;
        const int32_t currentDeviceIndex = OpenRCT2::Audio::GetCurrentDeviceIndex();
        if (currentDeviceIndex == -1 || OpenRCT2::Audio::GetDeviceCount() == 0)
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

        widgets[WIDX_SOUND].text = audioDeviceStringId;
        auto ft = Formatter::Common();
        ft.Add<char*>(audioDeviceName);

        widgets[WIDX_TITLE_MUSIC].text = GetTitleMusicName();

        SetCheckboxValue(WIDX_SOUND_CHECKBOX, gConfigSound.sound_enabled);
        SetCheckboxValue(WIDX_MASTER_SOUND_CHECKBOX, gConfigSound.master_sound_enabled);
        SetCheckboxValue(WIDX_MUSIC_CHECKBOX, gConfigSound.ride_music_enabled);
        SetCheckboxValue(WIDX_AUDIO_FOCUS_CHECKBOX, gConfigSound.audio_focus);
        WidgetSetEnabled(*this, WIDX_SOUND_CHECKBOX, gConfigSound.master_sound_enabled);
        WidgetSetEnabled(*this, WIDX_MUSIC_CHECKBOX, gConfigSound.master_sound_enabled);

        // Initialize only on first frame, otherwise the scrollbars won't be able to be modified
        if (frame_no == 0)
        {
            InitializeScrollPosition(WIDX_MASTER_VOLUME, 0, gConfigSound.master_volume);
            InitializeScrollPosition(WIDX_SOUND_VOLUME, 1, gConfigSound.sound_volume);
            InitializeScrollPosition(WIDX_MUSIC_VOLUME, 2, gConfigSound.ride_music_volume);
        }
    }

#pragma endregion

#pragma region Controls tab events
    void ControlsMouseUp(rct_widgetindex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_HOTKEY_DROPDOWN:
                context_open_window(WindowClass::KeyboardShortcutList);
                break;
            case WIDX_SCREEN_EDGE_SCROLLING:
                gConfigGeneral.edge_scrolling ^= 1;
                config_save_default();
                Invalidate();
                break;
            case WIDX_TRAP_CURSOR:
                gConfigGeneral.trap_cursor ^= 1;
                config_save_default();
                context_set_cursor_trap(gConfigGeneral.trap_cursor);
                Invalidate();
                break;
            case WIDX_ZOOM_TO_CURSOR:
                gConfigGeneral.zoom_to_cursor ^= 1;
                config_save_default();
                Invalidate();
                break;
            case WIDX_TOOLBAR_SHOW_FINANCES:
                gConfigInterface.toolbar_show_finances ^= 1;
                config_save_default();
                Invalidate();
                window_invalidate_by_class(WindowClass::TopToolbar);
                break;
            case WIDX_TOOLBAR_SHOW_RESEARCH:
                gConfigInterface.toolbar_show_research ^= 1;
                config_save_default();
                Invalidate();
                window_invalidate_by_class(WindowClass::TopToolbar);
                break;
            case WIDX_TOOLBAR_SHOW_CHEATS:
                gConfigInterface.toolbar_show_cheats ^= 1;
                config_save_default();
                Invalidate();
                window_invalidate_by_class(WindowClass::TopToolbar);
                break;
            case WIDX_TOOLBAR_SHOW_NEWS:
                gConfigInterface.toolbar_show_news ^= 1;
                config_save_default();
                Invalidate();
                window_invalidate_by_class(WindowClass::TopToolbar);
                break;
            case WIDX_TOOLBAR_SHOW_MUTE:
                gConfigInterface.toolbar_show_mute ^= 1;
                config_save_default();
                Invalidate();
                window_invalidate_by_class(WindowClass::TopToolbar);
                break;
            case WIDX_TOOLBAR_SHOW_CHAT:
                gConfigInterface.toolbar_show_chat ^= 1;
                config_save_default();
                Invalidate();
                window_invalidate_by_class(WindowClass::TopToolbar);
                break;
            case WIDX_TOOLBAR_SHOW_ZOOM:
                gConfigInterface.toolbar_show_zoom ^= 1;
                config_save_default();
                Invalidate();
                window_invalidate_by_class(WindowClass::TopToolbar);
                break;
            case WIDX_INVERT_DRAG:
                gConfigGeneral.invert_viewport_drag ^= 1;
                config_save_default();
                Invalidate();
                break;
            case WIDX_THEMES_BUTTON:
                context_open_window(WindowClass::Themes);
                Invalidate();
                break;
        }
    }

    void ControlsMouseDown(rct_widgetindex widgetIndex)
    {
        rct_widget* widget = &widgets[widgetIndex - 1];

        switch (widgetIndex)
        {
            case WIDX_THEMES_DROPDOWN:
                uint32_t numItems = static_cast<uint32_t>(ThemeManagerGetNumAvailableThemes());

                for (size_t i = 0; i < numItems; i++)
                {
                    gDropdownItems[i].Format = STR_OPTIONS_DROPDOWN_ITEM;
                    gDropdownItems[i].Args = reinterpret_cast<uintptr_t>(ThemeManagerGetAvailableThemeName(i));
                }

                WindowDropdownShowTextCustomWidth(
                    { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height() + 1, colours[1], 0,
                    Dropdown::Flag::StayOpen, numItems, widget->width() - 3);

                Dropdown::SetChecked(static_cast<int32_t>(ThemeManagerGetAvailableThemeIndex()), true);
                InvalidateWidget(WIDX_THEMES_DROPDOWN);
                break;
        }
    }

    void ControlsDropdown(rct_widgetindex widgetIndex, int32_t dropdownIndex)
    {
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

    void ControlsPrepareDraw()
    {
        SetCheckboxValue(WIDX_SCREEN_EDGE_SCROLLING, gConfigGeneral.edge_scrolling);
        SetCheckboxValue(WIDX_TRAP_CURSOR, gConfigGeneral.trap_cursor);
        SetCheckboxValue(WIDX_INVERT_DRAG, gConfigGeneral.invert_viewport_drag);
        SetCheckboxValue(WIDX_ZOOM_TO_CURSOR, gConfigGeneral.zoom_to_cursor);
        SetCheckboxValue(WIDX_TOOLBAR_SHOW_FINANCES, gConfigInterface.toolbar_show_finances);
        SetCheckboxValue(WIDX_TOOLBAR_SHOW_RESEARCH, gConfigInterface.toolbar_show_research);
        SetCheckboxValue(WIDX_TOOLBAR_SHOW_CHEATS, gConfigInterface.toolbar_show_cheats);
        SetCheckboxValue(WIDX_TOOLBAR_SHOW_NEWS, gConfigInterface.toolbar_show_news);
        SetCheckboxValue(WIDX_TOOLBAR_SHOW_MUTE, gConfigInterface.toolbar_show_mute);
        SetCheckboxValue(WIDX_TOOLBAR_SHOW_CHAT, gConfigInterface.toolbar_show_chat);
        SetCheckboxValue(WIDX_TOOLBAR_SHOW_ZOOM, gConfigInterface.toolbar_show_zoom);

        size_t activeAvailableThemeIndex = ThemeManagerGetAvailableThemeIndex();
        const utf8* activeThemeName = ThemeManagerGetAvailableThemeName(activeAvailableThemeIndex);
        auto ft = Formatter::Common();
        ft.Add<utf8*>(activeThemeName);
    }

#pragma endregion

#pragma region Misc tab events
    void MiscMouseUp(rct_widgetindex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_REAL_NAME_CHECKBOX:
                gConfigGeneral.show_real_names_of_guests ^= 1;
                config_save_default();
                Invalidate();
                peep_update_names(gConfigGeneral.show_real_names_of_guests);
                break;
            case WIDX_AUTO_STAFF_PLACEMENT:
                gConfigGeneral.auto_staff_placement ^= 1;
                config_save_default();
                Invalidate();
                break;
            case WIDX_SCENARIO_UNLOCKING:
                gConfigGeneral.scenario_unlocking_enabled ^= 1;
                config_save_default();
                window_close_by_class(WindowClass::ScenarioSelect);
                break;
            case WIDX_AUTO_OPEN_SHOPS:
                gConfigGeneral.auto_open_shops = !gConfigGeneral.auto_open_shops;
                config_save_default();
                Invalidate();
                break;
            case WIDX_ALLOW_EARLY_COMPLETION:
                gConfigGeneral.allow_early_completion ^= 1;
                // Only the server can control this setting and needs to send the
                // current value of allow_early_completion to all clients
                if (network_get_mode() == NETWORK_MODE_SERVER)
                {
                    auto setAllowEarlyCompletionAction = ScenarioSetSettingAction(
                        ScenarioSetSetting::AllowEarlyCompletion, gConfigGeneral.allow_early_completion);
                    GameActions::Execute(&setAllowEarlyCompletionAction);
                }
                config_save_default();
                Invalidate();
                break;
        }
    }

    void MiscMouseDown(rct_widgetindex widgetIndex)
    {
        rct_widget* widget = &widgets[widgetIndex - 1];

        switch (widgetIndex)
        {
            case WIDX_TITLE_SEQUENCE_DROPDOWN:
            {
                uint32_t numItems = static_cast<int32_t>(title_sequence_manager_get_count());
                for (size_t i = 0; i < numItems; i++)
                {
                    gDropdownItems[i].Format = STR_OPTIONS_DROPDOWN_ITEM;
                    gDropdownItems[i].Args = reinterpret_cast<uintptr_t>(title_sequence_manager_get_name(i));
                }

                gDropdownItems[numItems].Format = 0;
                numItems++;
                gDropdownItems[numItems].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[numItems].Args = STR_TITLE_SEQUENCE_RANDOM;
                numItems++;

                WindowDropdownShowText(
                    { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height() + 1, colours[1],
                    Dropdown::Flag::StayOpen, numItems);

                auto selectedIndex = gConfigInterface.random_title_sequence
                    ? numItems - 1
                    : static_cast<int32_t>(title_get_current_sequence());
                Dropdown::SetChecked(selectedIndex, true);
                break;
            }
            case WIDX_SCENARIO_GROUPING_DROPDOWN:
            {
                uint32_t numItems = 2;

                gDropdownItems[0].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[0].Args = STR_OPTIONS_SCENARIO_DIFFICULTY;
                gDropdownItems[1].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[1].Args = STR_OPTIONS_SCENARIO_ORIGIN;

                WindowDropdownShowTextCustomWidth(
                    { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height() + 1, colours[1], 0,
                    Dropdown::Flag::StayOpen, numItems, widget->width() - 3);

                Dropdown::SetChecked(gConfigGeneral.scenario_select_mode, true);
                break;
            }
            case WIDX_DEFAULT_INSPECTION_INTERVAL_DROPDOWN:
                for (size_t i = 0; i < 7; i++)
                {
                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[i].Args = RideInspectionIntervalNames[i];
                }

                ShowDropdown(widget, 7);
                Dropdown::SetChecked(gConfigGeneral.default_inspection_interval, true);
                break;
        }
    }

    void MiscDropdown(rct_widgetindex widgetIndex, int32_t dropdownIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_TITLE_SEQUENCE_DROPDOWN:
            {
                auto numItems = static_cast<int32_t>(title_sequence_manager_get_count());
                if (dropdownIndex < numItems && dropdownIndex != static_cast<int32_t>(title_get_current_sequence()))
                {
                    gConfigInterface.random_title_sequence = false;
                    title_sequence_change_preset(static_cast<size_t>(dropdownIndex));
                    config_save_default();
                    Invalidate();
                }
                else if (dropdownIndex == numItems + 1)
                {
                    gConfigInterface.random_title_sequence = true;
                    config_save_default();
                    Invalidate();
                }
                break;
            }
            case WIDX_DEFAULT_INSPECTION_INTERVAL_DROPDOWN:
                if (dropdownIndex != gConfigGeneral.default_inspection_interval)
                {
                    gConfigGeneral.default_inspection_interval = static_cast<uint8_t>(dropdownIndex);
                    config_save_default();
                    Invalidate();
                }
                break;
            case WIDX_SCENARIO_GROUPING_DROPDOWN:
                if (dropdownIndex != gConfigGeneral.scenario_select_mode)
                {
                    gConfigGeneral.scenario_select_mode = dropdownIndex;
                    gConfigInterface.scenarioselect_last_tab = 0;
                    config_save_default();
                    Invalidate();
                    window_close_by_class(WindowClass::ScenarioSelect);
                }
                break;
        }
    }

    void MiscPrepareDraw()
    {
        auto ft = Formatter::Common();
        if (gConfigInterface.random_title_sequence)
        {
            ft.Add<StringId>(STR_TITLE_SEQUENCE_RANDOM);
        }
        else
        {
            auto name = title_sequence_manager_get_name(title_get_config_sequence());
            ft.Add<StringId>(STR_STRING);
            ft.Add<utf8*>(name);
        }

        // The real name setting of clients is fixed to that of the server
        // and the server cannot change the setting during gameplay to prevent desyncs
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            disabled_widgets |= (1ULL << WIDX_REAL_NAME_CHECKBOX);
            widgets[WIDX_REAL_NAME_CHECKBOX].tooltip = STR_OPTION_DISABLED_DURING_NETWORK_PLAY;
            // Disable the use of the allow_early_completion option during network play on clients.
            // This is to prevent confusion on clients because changing this setting during network play wouldn't change
            // the way scenarios are completed during this network-session
            if (network_get_mode() == NETWORK_MODE_CLIENT)
            {
                disabled_widgets |= (1ULL << WIDX_ALLOW_EARLY_COMPLETION);
                widgets[WIDX_ALLOW_EARLY_COMPLETION].tooltip = STR_OPTION_DISABLED_DURING_NETWORK_PLAY;
            }
        }

        SetCheckboxValue(WIDX_REAL_NAME_CHECKBOX, gConfigGeneral.show_real_names_of_guests);
        SetCheckboxValue(WIDX_AUTO_STAFF_PLACEMENT, gConfigGeneral.auto_staff_placement);
        SetCheckboxValue(WIDX_AUTO_OPEN_SHOPS, gConfigGeneral.auto_open_shops);
        SetCheckboxValue(WIDX_ALLOW_EARLY_COMPLETION, gConfigGeneral.allow_early_completion);

        if (gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_DIFFICULTY)
            widgets[WIDX_SCENARIO_GROUPING].text = STR_OPTIONS_SCENARIO_DIFFICULTY;
        else
            widgets[WIDX_SCENARIO_GROUPING].text = STR_OPTIONS_SCENARIO_ORIGIN;

        SetCheckboxValue(WIDX_SCENARIO_UNLOCKING, gConfigGeneral.scenario_unlocking_enabled);

        if (gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_ORIGIN)
        {
            disabled_widgets &= ~(1ULL << WIDX_SCENARIO_UNLOCKING);
        }
        else
        {
            disabled_widgets |= (1ULL << WIDX_SCENARIO_UNLOCKING);
        }

        widgets[WIDX_DEFAULT_INSPECTION_INTERVAL].text = RideInspectionIntervalNames[gConfigGeneral
                                                                                         .default_inspection_interval];
    }

#pragma endregion

#pragma region Advanced tab events
    void AdvancedMouseUp(rct_widgetindex widgetIndex)
    {
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
                Invalidate();
                break;
            case WIDX_SAVE_PLUGIN_DATA_CHECKBOX:
                gConfigGeneral.save_plugin_data ^= 1;
                config_save_default();
                Invalidate();
                break;
            case WIDX_STAY_CONNECTED_AFTER_DESYNC:
                gConfigNetwork.stay_connected = !gConfigNetwork.stay_connected;
                config_save_default();
                Invalidate();
                break;
            case WIDX_ALWAYS_NATIVE_LOADSAVE:
                gConfigGeneral.use_native_browse_dialog = !gConfigGeneral.use_native_browse_dialog;
                config_save_default();
                Invalidate();
                break;
            case WIDX_PATH_TO_RCT1_BUTTON:
            {
                auto rct1path = OpenRCT2::GetContext()->GetUiContext()->ShowDirectoryDialog(
                    language_get_string(STR_PATH_TO_RCT1_BROWSER));
                if (!rct1path.empty())
                {
                    // Check if this directory actually contains RCT1
                    if (Csg1datPresentAtLocation(rct1path))
                    {
                        if (Csg1idatPresentAtLocation(rct1path))
                        {
                            if (CsgAtLocationIsUsable(rct1path))
                            {
                                gConfigGeneral.rct1_path = std::move(rct1path);
                                gConfigInterface.scenarioselect_last_tab = 0;
                                config_save_default();
                                context_show_error(STR_RESTART_REQUIRED, STR_NONE, {});
                            }
                            else
                            {
                                context_show_error(STR_PATH_TO_RCT1_IS_WRONG_VERSION, STR_NONE, {});
                            }
                        }
                        else
                        {
                            context_show_error(STR_PATH_TO_RCT1_DOES_NOT_CONTAIN_CSG1I_DAT, STR_NONE, {});
                        }
                    }
                    else
                    {
                        context_show_error(STR_PATH_TO_RCT1_WRONG_ERROR, STR_NONE, {});
                    }
                }
                Invalidate();
                break;
            }
            case WIDX_PATH_TO_RCT1_CLEAR:
                if (!gConfigGeneral.rct1_path.empty())
                {
                    gConfigGeneral.rct1_path.clear();
                    config_save_default();
                }
                Invalidate();
                break;
        }
    }

    void AdvancedMouseDown(rct_widgetindex widgetIndex)
    {
        rct_widget* widget = &widgets[widgetIndex - 1];

        switch (widgetIndex)
        {
            case WIDX_AUTOSAVE_FREQUENCY_DROPDOWN:
                for (size_t i = AUTOSAVE_EVERY_MINUTE; i <= AUTOSAVE_NEVER; i++)
                {
                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[i].Args = AutosaveNames[i];
                }

                ShowDropdown(widget, AUTOSAVE_NEVER + 1);
                Dropdown::SetChecked(gConfigGeneral.autosave_frequency, true);
                break;
            case WIDX_AUTOSAVE_AMOUNT_UP:
                gConfigGeneral.autosave_amount += 1;
                config_save_default();
                InvalidateWidget(WIDX_AUTOSAVE_FREQUENCY);
                InvalidateWidget(WIDX_AUTOSAVE_FREQUENCY_DROPDOWN);
                InvalidateWidget(WIDX_AUTOSAVE_AMOUNT);
                break;
            case WIDX_AUTOSAVE_AMOUNT_DOWN:
                if (gConfigGeneral.autosave_amount > 1)
                {
                    gConfigGeneral.autosave_amount -= 1;
                    config_save_default();
                    InvalidateWidget(WIDX_AUTOSAVE_FREQUENCY);
                    InvalidateWidget(WIDX_AUTOSAVE_FREQUENCY_DROPDOWN);
                    InvalidateWidget(WIDX_AUTOSAVE_AMOUNT);
                }
        }
    }

    void AdvancedDropdown(rct_widgetindex widgetIndex, int32_t dropdownIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_AUTOSAVE_FREQUENCY_DROPDOWN:
                if (dropdownIndex != gConfigGeneral.autosave_frequency)
                {
                    gConfigGeneral.autosave_frequency = static_cast<uint8_t>(dropdownIndex);
                    config_save_default();
                    Invalidate();
                }
                break;
        }
    }

    void AdvancedPrepareDraw()
    {
        SetCheckboxValue(WIDX_DEBUGGING_TOOLS, gConfigGeneral.debugging_tools);
        WidgetSetCheckboxValue(
            *this, WIDX_ALLOW_LOADING_WITH_INCORRECT_CHECKSUM, gConfigGeneral.allow_loading_with_incorrect_checksum);
        SetCheckboxValue(WIDX_SAVE_PLUGIN_DATA_CHECKBOX, gConfigGeneral.save_plugin_data);
        SetCheckboxValue(WIDX_STAY_CONNECTED_AFTER_DESYNC, gConfigNetwork.stay_connected);
        SetCheckboxValue(WIDX_ALWAYS_NATIVE_LOADSAVE, gConfigGeneral.use_native_browse_dialog);
        widgets[WIDX_AUTOSAVE_FREQUENCY].text = AutosaveNames[gConfigGeneral.autosave_frequency];
    }

    void AdvancedDraw(rct_drawpixelinfo* dpi)
    {
        auto ft = Formatter();
        ft.Add<int32_t>(static_cast<int32_t>(gConfigGeneral.autosave_amount));
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ widgets[WIDX_AUTOSAVE_AMOUNT].left + 1, widgets[WIDX_AUTOSAVE_AMOUNT].top + 1 },
            STR_WINDOW_COLOUR_2_COMMA16, ft, { colours[1] });

        const auto normalisedPath = Platform::StrDecompToPrecomp(gConfigGeneral.rct1_path);
        ft = Formatter();
        ft.Add<const utf8*>(normalisedPath.c_str());

        rct_widget pathWidget = widgets[WIDX_PATH_TO_RCT1_BUTTON];

        // Apply vertical alignment if appropriate.
        int32_t widgetHeight = pathWidget.bottom - pathWidget.top;
        int32_t lineHeight = font_get_line_height(FontSpriteBase::MEDIUM);
        uint32_t padding = widgetHeight > lineHeight ? (widgetHeight - lineHeight) / 2 : 0;
        ScreenCoordsXY screenCoords = { windowPos.x + pathWidget.left + 1,
                                        windowPos.y + pathWidget.top + static_cast<int32_t>(padding) };
        DrawTextEllipsised(dpi, screenCoords, 277, STR_STRING, ft, { colours[1] });
    }

    OpenRCT2String AdvancedTooltip(rct_widgetindex widgetIndex, StringId fallback)
    {
        if (widgetIndex == WIDX_PATH_TO_RCT1_BUTTON)
        {
            if (gConfigGeneral.rct1_path.empty())
            {
                // No tooltip if the path is empty
                return { STR_NONE, {} };
            }

            auto ft = Formatter();
            ft.Add<utf8*>(gConfigGeneral.rct1_path.c_str());
            return { fallback, ft };
        }
        return { fallback, {} };
    }

#pragma endregion

    void SetPage(int32_t p)
    {
        page = p;
        frame_no = 0;
        pressed_widgets = 0;
        widgets = window_options_page_widgets[page];

        Invalidate();
        window_event_resize_call(this);
        window_event_invalidate_call(this);
        InitScrollWidgets();
        Invalidate();
    }

    void SetPressedTab()
    {
        for (int32_t i = 0; i < WINDOW_OPTIONS_PAGE_COUNT; i++)
            pressed_widgets &= ~(1 << (WIDX_FIRST_TAB + i));
        pressed_widgets |= 1LL << (WIDX_FIRST_TAB + page);
    }

    void ShowDropdown(rct_widget* widget, int32_t num_items)
    {
        // helper function, all dropdown boxes have similar properties
        WindowDropdownShowTextCustomWidth(
            { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height() + 1, colours[1], 0,
            Dropdown::Flag::StayOpen, num_items, widget->width() - 3);
    }

    void DrawTabImages(rct_drawpixelinfo* dpi)
    {
        DrawTabImage(dpi, WINDOW_OPTIONS_PAGE_DISPLAY, SPR_TAB_PAINT_0);
        DrawTabImage(dpi, WINDOW_OPTIONS_PAGE_RENDERING, SPR_G2_TAB_TREE);
        DrawTabImage(dpi, WINDOW_OPTIONS_PAGE_CULTURE, SPR_TAB_TIMER_0);
        DrawTabImage(dpi, WINDOW_OPTIONS_PAGE_AUDIO, SPR_TAB_MUSIC_0);
        DrawTabImage(dpi, WINDOW_OPTIONS_PAGE_CONTROLS_AND_INTERFACE, SPR_TAB_GEARS_0);
        DrawTabImage(dpi, WINDOW_OPTIONS_PAGE_MISC, SPR_TAB_RIDE_0);
        DrawTabImage(dpi, WINDOW_OPTIONS_PAGE_ADVANCED, SPR_TAB_WRENCH_0);
    }

    void DrawTabImage(rct_drawpixelinfo* dpi, int32_t p, int32_t spriteIndex)
    {
        rct_widgetindex widgetIndex = WIDX_FIRST_TAB + p;
        rct_widget* widget = &widgets[widgetIndex];

        auto screenCoords = windowPos + ScreenCoordsXY{ widget->left, widget->top };

        if (!WidgetIsDisabled(*this, widgetIndex))
        {
            if (page == p)
            {
                int32_t frame = frame_no / TabAnimationDivisor[page];
                spriteIndex += (frame % TabAnimationFrames[page]);
            }

            // Draw normal, enabled sprite.
            gfx_draw_sprite(dpi, ImageId(spriteIndex), screenCoords);
        }
        else
        {
            // Get the window background colour
            uint8_t window_colour = NOT_TRANSLUCENT(colours[widget->colour]);

            // Draw greyed out (light border bottom right shadow)
            gfx_draw_sprite_solid(dpi, spriteIndex, screenCoords + ScreenCoordsXY{ 1, 1 }, ColourMapA[window_colour].lighter);

            // Draw greyed out (dark)
            gfx_draw_sprite_solid(dpi, spriteIndex, screenCoords, ColourMapA[window_colour].mid_light);
        }
    }

    void UpdateHeightMarkers()
    {
        config_save_default();
        gfx_invalidate_screen();
    }

    uint8_t GetScrollPercentage(const rct_widget& widget, const rct_scroll& scroll)
    {
        uint8_t w = widget.width() - 1;
        return static_cast<float>(scroll.h_left) / (scroll.h_right - w) * 100;
    }

    void InitializeScrollPosition(rct_widgetindex widgetIndex, int32_t scrollId, uint8_t volume)
    {
        const auto& widget = widgets[widgetIndex];
        auto& scroll = scrolls[scrollId];

        int32_t widgetSize = scroll.h_right - (widget.width() - 1);
        scroll.h_left = ceil(volume / 100.0f * widgetSize);

        WidgetScrollUpdateThumbs(*this, widgetIndex);
    }

    static bool IsRCT1TitleMusicAvailable()
    {
        auto env = GetContext()->GetPlatformEnvironment();
        auto rct1path = env->GetDirectoryPath(DIRBASE::RCT1);
        return !rct1path.empty();
    }

    static constexpr StringId AutosaveNames[] = {
        STR_SAVE_EVERY_MINUTE,    STR_SAVE_EVERY_5MINUTES, STR_SAVE_EVERY_15MINUTES,
        STR_SAVE_EVERY_30MINUTES, STR_SAVE_EVERY_HOUR,     STR_SAVE_NEVER,
    };

    static constexpr StringId TitleMusicNames[] = {
        STR_OPTIONS_MUSIC_VALUE_NONE,
        STR_ROLLERCOASTER_TYCOON_1_DROPDOWN,
        STR_ROLLERCOASTER_TYCOON_2_DROPDOWN,
        STR_OPTIONS_MUSIC_VALUE_RANDOM,
    };

    static constexpr StringId FullscreenModeNames[] = {
        STR_OPTIONS_DISPLAY_WINDOWED,
        STR_OPTIONS_DISPLAY_FULLSCREEN,
        STR_OPTIONS_DISPLAY_FULLSCREEN_BORDERLESS,
    };

    static constexpr int32_t TabAnimationDivisor[] = {
        4, // WINDOW_OPTIONS_PAGE_DISPLAY,
        1, // WINDOW_OPTIONS_PAGE_RENDERING,
        8, // WINDOW_OPTIONS_PAGE_CULTURE,
        2, // WINDOW_OPTIONS_PAGE_AUDIO,
        2, // WINDOW_OPTIONS_PAGE_CONTROLS_AND_INTERFACE,
        4, // WINDOW_OPTIONS_PAGE_MISC,
        2, // WINDOW_OPTIONS_PAGE_ADVANCED,
    };

    static constexpr int32_t TabAnimationFrames[] = {
        8,  // WINDOW_OPTIONS_PAGE_DISPLAY,
        1,  // WINDOW_OPTIONS_PAGE_RENDERING,
        8,  // WINDOW_OPTIONS_PAGE_CULTURE,
        16, // WINDOW_OPTIONS_PAGE_AUDIO,
        4,  // WINDOW_OPTIONS_PAGE_CONTROLS_AND_INTERFACE,
        16, // WINDOW_OPTIONS_PAGE_MISC,
        16, // WINDOW_OPTIONS_PAGE_ADVANCED,
    };
};

/**
 *
 *  rct2: 0x006BAC5B
 */
rct_window* WindowOptionsOpen()
{
    return WindowFocusOrCreate<OptionsWindow>(WindowClass::Options, WW, WH, WF_CENTRE_SCREEN);
}
