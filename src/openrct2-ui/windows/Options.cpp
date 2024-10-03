/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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

#include <cmath>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Theme.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Diagnostic.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/actions/ScenarioSetSettingAction.h>
#include <openrct2/audio/AudioContext.h>
#include <openrct2/audio/AudioMixer.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/EnumUtils.hpp>
#include <openrct2/core/File.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/localisation/Currency.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Language.h>
#include <openrct2/localisation/Localisation.Date.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/network/network.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/rct1/Csg.h>
#include <openrct2/ride/RideAudio.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/scenes/title/TitleScene.h>
#include <openrct2/scenes/title/TitleSequenceManager.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
extern "C" {
extern void ExportPersistentData();
extern void ImportPersistentData();
}
#endif

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;

namespace OpenRCT2::Ui::Windows
{
    enum WindowOptionsPage
    {
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

    enum WindowOptionsWidgetIdx
    {
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
        WIDX_WINDOW_BUTTONS_ON_THE_LEFT,
        WIDX_ENLARGED_UI,
        WIDX_TOUCH_ENHANCEMENTS,
        WIDX_HOTKEY_DROPDOWN,
        WIDX_THEMES_GROUP,
        WIDX_THEMES_LABEL,
        WIDX_THEMES,
        WIDX_THEMES_DROPDOWN,
        WIDX_THEMES_BUTTON,
        WIDX_TOOLBAR_BUTTONS_GROUP,
        WIDX_TOOLBAR_BUTTONS_CENTRED,
        WIDX_TOOLBAR_BUTTONS_SHOW_FOR_LABEL,
        WIDX_TOOLBAR_SHOW_FINANCES,
        WIDX_TOOLBAR_SHOW_RESEARCH,
        WIDX_TOOLBAR_SHOW_CHEATS,
        WIDX_TOOLBAR_SHOW_NEWS,
        WIDX_TOOLBAR_SHOW_MUTE,
        WIDX_TOOLBAR_SHOW_CHAT,
        WIDX_TOOLBAR_SHOW_ZOOM,
        WIDX_TOOLBAR_SHOW_ROTATE_ANTI_CLOCKWISE,

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
        WIDX_REAL_NAMES_GUESTS_CHECKBOX,
        WIDX_REAL_NAMES_STAFF_CHECKBOX,
        WIDX_AUTO_STAFF_PLACEMENT,
        WIDX_AUTO_OPEN_SHOPS,
        WIDX_DEFAULT_INSPECTION_INTERVAL_LABEL,
        WIDX_DEFAULT_INSPECTION_INTERVAL,
        WIDX_DEFAULT_INSPECTION_INTERVAL_DROPDOWN,

        // Advanced
        WIDX_DEBUGGING_TOOLS = WIDX_PAGE_START,
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
        WIDX_ASSET_PACKS,
#ifdef __EMSCRIPTEN__
        WIDX_EXPORT_EMSCRIPTEN_DATA,
        WIDX_IMPORT_EMSCRIPTEN_DATA,
#endif
    };

    // clang-format off
    static constexpr StringId WINDOW_TITLE = STR_OPTIONS_TITLE;
    static constexpr int32_t WW = 310;
    static constexpr int32_t WH = 332;

    #define MAIN_OPTIONS_WIDGETS \
        WINDOW_SHIM(WINDOW_TITLE, WW, WH), \
        MakeWidget({ 0, 43 }, { WW, 289 }, WindowWidgetType::Resize, WindowColour::Secondary), \
        MakeTab({ 3, 17 }, STR_OPTIONS_DISPLAY_TIP), \
        MakeTab({ 34, 17 }, STR_OPTIONS_RENDERING_TIP), \
        MakeTab({ 65, 17 }, STR_OPTIONS_CULTURE_TIP), \
        MakeTab({ 96, 17 }, STR_OPTIONS_AUDIO_TIP), \
        MakeTab({ 127, 17 }, STR_OPTIONS_CONTROLS_AND_INTERFACE_TIP), \
        MakeTab({ 158, 17 }, STR_OPTIONS_MISCELLANEOUS_TIP), \
        MakeTab({ 189, 17 }, STR_OPTIONS_ADVANCED)

    static constexpr Widget window_options_display_widgets[] = {
        MAIN_OPTIONS_WIDGETS,
        MakeWidget        ({  5,  53}, {300, 170}, WindowWidgetType::Groupbox,     WindowColour::Secondary, STR_HARDWARE_GROUP                                                              ), // Hardware group
        MakeWidget        ({ 10,  67}, {145,  12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_FULLSCREEN_MODE,                   STR_FULLSCREEN_MODE_TIP                  ), // Fullscreen
        MakeWidget        ({155,  68}, {145,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                                                  ),
        MakeWidget        ({288,  69}, { 11,  10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,                    STR_FULLSCREEN_MODE_TIP                  ),
        MakeWidget        ({ 24,  82}, {145,  12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_DISPLAY_RESOLUTION,                STR_DISPLAY_RESOLUTION_TIP               ), // Resolution
        MakeWidget        ({155,  83}, {145,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_ARG_16_RESOLUTION_X_BY_Y                                                    ),
        MakeWidget        ({288,  84}, { 11,  10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,                    STR_DISPLAY_RESOLUTION_TIP               ),
        MakeWidget        ({ 10,  98}, {145,  12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_UI_SCALING_DESC,                   STR_WINDOW_SCALE_TIP                     ), // Scale
        MakeSpinnerWidgets({155,  98}, {145,  12}, WindowWidgetType::Spinner,      WindowColour::Secondary, kStringIdNone,                              STR_WINDOW_SCALE_TIP                     ), // Scale spinner (3 widgets)
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
    };

    constexpr int32_t kFrameRenderingStart = 53;
    constexpr int32_t kFrameEffectStart = 163;

    static constexpr Widget window_options_rendering_widgets[] = {
        MAIN_OPTIONS_WIDGETS,
        MakeWidget({  5,  kFrameRenderingStart + 0}, {300, 108}, WindowWidgetType::Groupbox,     WindowColour::Secondary, STR_RENDERING_GROUP                                       ), // Rendering group
        MakeWidget({ 10, kFrameRenderingStart + 15}, {281,  12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_TILE_SMOOTHING,         STR_TILE_SMOOTHING_TIP        ), // Landscape smoothing
        MakeWidget({ 10, kFrameRenderingStart + 30}, {281,  12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_GRIDLINES,              STR_GRIDLINES_TIP             ), // Gridlines
        MakeWidget({ 10, kFrameRenderingStart + 45}, {281,  12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_UPPERCASE_BANNERS,      STR_UPPERCASE_BANNERS_TIP     ), // Uppercase banners
        MakeWidget({ 10, kFrameRenderingStart + 60}, {281,  12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_SHOW_GUEST_PURCHASES,   STR_SHOW_GUEST_PURCHASES_TIP  ), // Guest purchases
        MakeWidget({ 10, kFrameRenderingStart + 75}, {281,  12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_TRANSPARENT_SCREENSHOT, STR_TRANSPARENT_SCREENSHOT_TIP), // Transparent screenshot
        MakeWidget({ 10, kFrameRenderingStart + 90}, {281,  12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_VIRTUAL_FLOOR_STYLE,    STR_VIRTUAL_FLOOR_STYLE_TIP   ), // Virtual floor
        MakeWidget({155, kFrameRenderingStart + 90}, {145,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, kStringIdNone,                   STR_VIRTUAL_FLOOR_STYLE_TIP   ), // Virtual floor dropdown
        MakeWidget({288, kFrameRenderingStart + 91}, { 11,  10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,         STR_VIRTUAL_FLOOR_STYLE_TIP   ), // Virtual floor dropdown

        MakeWidget({ 5,  kFrameEffectStart + 0}, {300, 94}, WindowWidgetType::Groupbox, WindowColour::Secondary, STR_EFFECTS_GROUP                                             ), // Rendering group
        MakeWidget({10, kFrameEffectStart + 15}, {281, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_CYCLE_DAY_NIGHT,          STR_CYCLE_DAY_NIGHT_TIP         ), // Cycle day-night
        MakeWidget({25, kFrameEffectStart + 30}, {266, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_ENABLE_LIGHTING_EFFECTS,  STR_ENABLE_LIGHTING_EFFECTS_TIP ), // Enable light fx
        MakeWidget({40, kFrameEffectStart + 45}, {251, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_ENABLE_LIGHTING_VEHICLES, STR_ENABLE_LIGHTING_VEHICLES_TIP), // Enable light fx for vehicles
        MakeWidget({10, kFrameEffectStart + 60}, {281, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_RENDER_WEATHER_EFFECTS,   STR_RENDER_WEATHER_EFFECTS_TIP  ), // Render weather effects
        MakeWidget({25, kFrameEffectStart + 75}, {266, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_DISABLE_LIGHTNING_EFFECT, STR_DISABLE_LIGHTNING_EFFECT_TIP), // Disable lightning effect
    };

    static constexpr Widget window_options_culture_widgets[] = {
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
    };

    static constexpr Widget window_options_audio_widgets[] = {
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
    };

    constexpr int32_t kControlsGroupStart = 53;
    constexpr int32_t kThemesGroupStart = 193;
    constexpr int32_t kToolbarGroupStart = 245;

    static constexpr Widget window_options_controls_and_interface_widgets[] = {
        MAIN_OPTIONS_WIDGETS,
        MakeWidget({  5, kControlsGroupStart +  0},  {300,137}, WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CONTROLS_GROUP                                                ), // Controls group
        MakeWidget({ 10, kControlsGroupStart + 13},  {290, 14}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_SCREEN_EDGE_SCROLLING,      STR_SCREEN_EDGE_SCROLLING_TIP     ), // Edge scrolling
        MakeWidget({ 10, kControlsGroupStart + 30},  {290, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_TRAP_MOUSE,                 STR_TRAP_MOUSE_TIP                ), // Trap mouse
        MakeWidget({ 10, kControlsGroupStart + 45},  {290, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_INVERT_RIGHT_MOUSE_DRAG,    STR_INVERT_RIGHT_MOUSE_DRAG_TIP   ), // Invert right mouse dragging
        MakeWidget({ 10, kControlsGroupStart + 60},  {290, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_ZOOM_TO_CURSOR,             STR_ZOOM_TO_CURSOR_TIP            ), // Zoom to cursor
        MakeWidget({ 10, kControlsGroupStart + 75},  {290, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary,  STR_WINDOW_BUTTONS_ON_THE_LEFT, STR_WINDOW_BUTTONS_ON_THE_LEFT_TIP), // Window buttons on the left
        MakeWidget({ 10, kControlsGroupStart + 90},  {290, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary,  STR_ENLARGED_UI,                STR_ENLARGED_UI_TIP               ),
        MakeWidget({ 25, kControlsGroupStart + 105}, {275, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary,  STR_TOUCH_ENHANCEMENTS,         STR_TOUCH_ENHANCEMENTS_TIP        ),
        MakeWidget({155, kControlsGroupStart + 120}, {145, 13}, WindowWidgetType::Button,   WindowColour::Secondary, STR_HOTKEY,                     STR_HOTKEY_TIP                    ), // Set hotkeys buttons

        MakeWidget({  5, kThemesGroupStart +  0}, {300, 48}, WindowWidgetType::Groupbox,     WindowColour::Secondary, STR_THEMES_GROUP                                          ), // Themes group
        MakeWidget({ 10, kThemesGroupStart + 14}, {145, 12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_THEMES_LABEL_CURRENT_THEME, STR_CURRENT_THEME_TIP     ), // Themes
        MakeWidget({155, kThemesGroupStart + 14}, {145, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_STRING                                                ),
        MakeWidget({288, kThemesGroupStart + 15}, { 11, 10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,             STR_CURRENT_THEME_TIP     ),
        MakeWidget({155, kThemesGroupStart + 30}, {145, 13}, WindowWidgetType::Button,       WindowColour::Secondary, STR_EDIT_THEMES_BUTTON,         STR_EDIT_THEMES_BUTTON_TIP), // Themes button

        MakeWidget({  5, kToolbarGroupStart +  0}, {300,107}, WindowWidgetType::Groupbox, WindowColour::Secondary, STR_TOOLBAR_BUTTONS_GROUP                                                         ), // Toolbar buttons group
        MakeWidget({ 10, kToolbarGroupStart + 14}, {280, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary,  STR_OPTIONS_TOOLBAR_BUTTONS_CENTRED, STR_OPTIONS_TOOLBAR_BUTTONS_CENTRED_TIP      ),
        MakeWidget({ 10, kToolbarGroupStart + 31}, {280, 12}, WindowWidgetType::Label,    WindowColour::Secondary, STR_SHOW_TOOLBAR_BUTTONS_FOR                                                      ),
        MakeWidget({ 24, kToolbarGroupStart + 46}, {122, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_FINANCES_BUTTON_ON_TOOLBAR,      STR_FINANCES_BUTTON_ON_TOOLBAR_TIP           ), // Finances
        MakeWidget({ 24, kToolbarGroupStart + 61}, {122, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_RESEARCH_BUTTON_ON_TOOLBAR,      STR_RESEARCH_BUTTON_ON_TOOLBAR_TIP           ), // Research
        MakeWidget({155, kToolbarGroupStart + 46}, {145, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_CHEATS_BUTTON_ON_TOOLBAR,        STR_CHEATS_BUTTON_ON_TOOLBAR_TIP             ), // Cheats
        MakeWidget({155, kToolbarGroupStart + 61}, {145, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_SHOW_RECENT_MESSAGES_ON_TOOLBAR, STR_SHOW_RECENT_MESSAGES_ON_TOOLBAR_TIP      ), // Recent messages
        MakeWidget({ 24, kToolbarGroupStart + 76}, {162, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_MUTE_BUTTON_ON_TOOLBAR,          STR_MUTE_BUTTON_ON_TOOLBAR_TIP               ), // Mute
        MakeWidget({155, kToolbarGroupStart + 76}, {145, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_CHAT_BUTTON_ON_TOOLBAR,          STR_CHAT_BUTTON_ON_TOOLBAR_TIP               ), // Chat
        MakeWidget({ 24, kToolbarGroupStart + 91}, {122, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_ZOOM_BUTTON_ON_TOOLBAR,          STR_ZOOM_BUTTON_ON_TOOLBAR_TIP               ), // Zoom
        MakeWidget({155, kToolbarGroupStart + 91}, {145, 12}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_ROTATE_ANTI_CLOCKWISE,           STR_ROTATE_VIEW_ANTI_CLOCKWISE_IN_TOOLBAR_TIP), // Rotate anti-clockwise
    };

    constexpr int32_t kTitleSequenceStart = 53;
    constexpr int32_t kScenarioGroupStart = kTitleSequenceStart + 35;
    constexpr int32_t kScenarioOptionsGroupStart = kScenarioGroupStart + 55;
    constexpr int32_t kTweaksStart = kScenarioOptionsGroupStart + 39;

    static constexpr Widget window_options_misc_widgets[] = {
        MAIN_OPTIONS_WIDGETS,
        MakeWidget(         {  5, kTitleSequenceStart +  0}, {300, 31}, WindowWidgetType::Groupbox,     WindowColour::Secondary, STR_OPTIONS_TITLE_SEQUENCE                        ),
        MakeDropdownWidgets({ 10, kTitleSequenceStart + 15}, {290, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_STRINGID,               STR_TITLE_SEQUENCE_TIP), // Title sequence dropdown

        MakeWidget({  5,  kScenarioGroupStart + 0}, {300, 51}, WindowWidgetType::Groupbox,     WindowColour::Secondary, STR_OPTIONS_SCENARIO_SELECTION                            ),
        MakeWidget({ 10, kScenarioGroupStart + 16}, {165, 12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_OPTIONS_SCENARIO_GROUPING,  STR_SCENARIO_GROUPING_TIP ),
        MakeWidget({175, kScenarioGroupStart + 15}, {125, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                            ), // Scenario select mode
        MakeWidget({288, kScenarioGroupStart + 16}, { 11, 10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,             STR_SCENARIO_GROUPING_TIP ),
        MakeWidget({ 25, kScenarioGroupStart + 30}, {275, 16}, WindowWidgetType::Checkbox,     WindowColour::Tertiary , STR_OPTIONS_SCENARIO_UNLOCKING, STR_SCENARIO_UNLOCKING_TIP), // Unlocking of scenarios

        MakeWidget({ 5,  kScenarioOptionsGroupStart + 0}, {300, 35}, WindowWidgetType::Groupbox, WindowColour::Secondary, STR_SCENARIO_OPTIONS                                ),
        MakeWidget({10, kScenarioOptionsGroupStart + 15}, {290, 15}, WindowWidgetType::Checkbox, WindowColour::Tertiary , STR_ALLOW_EARLY_COMPLETION, STR_EARLY_COMPLETION_TIP), // Allow early scenario completion

        MakeWidget({  5,  kTweaksStart + 0}, {300, 96}, WindowWidgetType::Groupbox,     WindowColour::Secondary, STR_OPTIONS_TWEAKS                                                  ),
        MakeWidget({ 10, kTweaksStart + 15}, {290, 15}, WindowWidgetType::Checkbox,     WindowColour::Tertiary , STR_REAL_NAME_GUESTS,     STR_REAL_NAME_GUESTS_TIP                  ), // Show 'real' names of guests
        MakeWidget({ 10, kTweaksStart + 30}, {290, 15}, WindowWidgetType::Checkbox,     WindowColour::Tertiary , STR_REAL_NAME_STAFF,      STR_REAL_NAME_STAFF_TIP                   ), // Show 'real' names of staff
        MakeWidget({ 10, kTweaksStart + 45}, {290, 15}, WindowWidgetType::Checkbox,     WindowColour::Tertiary , STR_AUTO_STAFF_PLACEMENT, STR_AUTO_STAFF_PLACEMENT_TIP              ), // Auto staff placement
        MakeWidget({ 10, kTweaksStart + 60}, {290, 15}, WindowWidgetType::Checkbox,     WindowColour::Tertiary , STR_AUTO_OPEN_SHOPS,      STR_AUTO_OPEN_SHOPS_TIP                   ), // Automatically open shops & stalls
        MakeWidget({ 10, kTweaksStart + 77}, {165, 12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_DEFAULT_INSPECTION_INTERVAL, STR_DEFAULT_INSPECTION_INTERVAL_TIP),
        MakeWidget({175, kTweaksStart + 76}, {125, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                                      ), // Default inspection time dropdown
        MakeWidget({288, kTweaksStart + 77}, { 11, 10}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,       STR_DEFAULT_INSPECTION_INTERVAL_TIP       ), // Default inspection time dropdown button
    };

    static constexpr Widget window_options_advanced_widgets[] = {
        MAIN_OPTIONS_WIDGETS,
        MakeWidget        ({ 10,  54}, {290, 12}, WindowWidgetType::Checkbox,     WindowColour::Tertiary,  STR_ENABLE_DEBUGGING_TOOLS,                STR_ENABLE_DEBUGGING_TOOLS_TIP               ), // Enable debugging tools
        MakeWidget        ({ 10,  69}, {290, 12}, WindowWidgetType::Checkbox,     WindowColour::Tertiary,  STR_SAVE_PLUGIN_DATA,                      STR_SAVE_PLUGIN_DATA_TIP                     ), // Export plug-in objects with saved games
        MakeWidget        ({ 10,  84}, {290, 12}, WindowWidgetType::Checkbox,     WindowColour::Tertiary,  STR_STAY_CONNECTED_AFTER_DESYNC,           STR_STAY_CONNECTED_AFTER_DESYNC_TIP          ), // Do not disconnect after the client desynchronises with the server
        MakeWidget        ({ 10,  99}, {290, 12}, WindowWidgetType::Checkbox,     WindowColour::Secondary, STR_ALWAYS_NATIVE_LOADSAVE,                STR_ALWAYS_NATIVE_LOADSAVE_TIP               ), // Use native load/save window
        MakeWidget        ({ 23, 114}, {135, 12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_OPTIONS_AUTOSAVE_FREQUENCY_LABEL,      STR_AUTOSAVE_FREQUENCY_TIP                   ),
        MakeWidget        ({165, 113}, {135, 13}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                                                          ), // Autosave dropdown
        MakeWidget        ({288, 114}, { 11, 11}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH,                        STR_AUTOSAVE_FREQUENCY_TIP                   ), // Autosave dropdown button
        MakeWidget        ({ 23, 130}, {135, 12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_AUTOSAVE_AMOUNT,                       STR_AUTOSAVE_AMOUNT_TIP                      ),
        MakeSpinnerWidgets({165, 130}, {135, 12}, WindowWidgetType::Spinner,      WindowColour::Secondary, kStringIdNone,                             STR_AUTOSAVE_AMOUNT_TIP                      ), // Autosave amount spinner
        MakeWidget        ({ 23, 145}, {276, 12}, WindowWidgetType::Label,        WindowColour::Secondary, STR_PATH_TO_RCT1,                          STR_PATH_TO_RCT1_TIP                         ), // RCT 1 path text
        MakeWidget        ({ 24, 160}, {266, 14}, WindowWidgetType::Button,       WindowColour::Secondary, kStringIdNone,                             STR_STRING_TOOLTIP                           ), // RCT 1 path button
        MakeWidget        ({289, 160}, { 11, 14}, WindowWidgetType::Button,       WindowColour::Secondary, STR_CLOSE_X,                               STR_PATH_TO_RCT1_CLEAR_TIP                   ), // RCT 1 path clear button
        MakeWidget        ({150, 176}, {150, 14}, WindowWidgetType::Button,       WindowColour::Secondary, STR_EDIT_ASSET_PACKS_BUTTON,               kStringIdNone                                ), // Asset packs
#ifdef __EMSCRIPTEN__
        MakeWidget        ({150, 192}, {150, 14}, WindowWidgetType::Button,       WindowColour::Secondary, STR_EXPORT_EMSCRIPTEN,                     kStringIdNone                                ), // Emscripten data export
        MakeWidget        ({150, 208}, {150, 14}, WindowWidgetType::Button,       WindowColour::Secondary, STR_IMPORT_EMSCRIPTEN,                     kStringIdNone                                ), // Emscripten data import
#endif
    };

    static constexpr std::span<const Widget> window_options_page_widgets[] = {
        window_options_display_widgets,
        window_options_rendering_widgets,
        window_options_culture_widgets,
        window_options_audio_widgets,
        window_options_controls_and_interface_widgets,
        window_options_misc_widgets,
        window_options_advanced_widgets,
    };
    // clang-format on

#pragma endregion

    class OptionsWindow final : public Window
    {
    public:
        void OnOpen() override
        {
            SetPage(WINDOW_OPTIONS_PAGE_DISPLAY);
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
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

        void OnMouseDown(WidgetIndex widgetIndex) override
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

        void OnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
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

        void OnDraw(DrawPixelInfo& dpi) override
        {
            DrawWidgets(dpi);
            DrawTabImages(dpi);

            switch (page)
            {
                case WINDOW_OPTIONS_PAGE_DISPLAY:
                    DisplayDraw(dpi);
                    break;
                case WINDOW_OPTIONS_PAGE_ADVANCED:
                    AdvancedDraw(dpi);
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

        OpenRCT2String OnTooltip(WidgetIndex widgetIndex, StringId fallback) override
        {
            if (page == WINDOW_OPTIONS_PAGE_ADVANCED)
                return AdvancedTooltip(widgetIndex, fallback);

            return WindowBase::OnTooltip(widgetIndex, fallback);
        }

    private:
#pragma region Common events
        void CommonMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    Close();
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
            SetPressedTab();

            disabled_widgets = 0;
            auto hasFilePicker = OpenRCT2::GetContext()->GetUiContext()->HasFilePicker();
            const bool controlsTabSelected = (WIDX_FIRST_TAB + page) == WIDX_TAB_CONTROLS_AND_INTERFACE;
            if (!hasFilePicker && controlsTabSelected)
            {
                disabled_widgets |= (1uLL << WIDX_ALWAYS_NATIVE_LOADSAVE);
                widgets[WIDX_ALWAYS_NATIVE_LOADSAVE].type = WindowWidgetType::Empty;
            }
        }

        void CommonPrepareDrawAfter()
        {
            // Automatically adjust window height to fit widgets
            int32_t y = 0;
            for (WidgetIndex widgetIdx = WIDX_PAGE_START; widgetIdx < widgets.size(); widgetIdx++)
            {
                const auto& widget = widgets[widgetIdx];
                y = std::max<int32_t>(y, widget.bottom);
            }
            height = y + 6;
            ResizeFrameWithPage();
        }

        void OnResize() override
        {
            ResizeFrameWithPage();
        }

        void CommonUpdate()
        {
            // Tab animation
            frame_no++;
            InvalidateWidget(WIDX_FIRST_TAB + page);
        }
#pragma endregion

#pragma region Display tab events
        void DisplayMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_UNCAP_FPS_CHECKBOX:
                    Config::Get().general.UncapFPS ^= 1;
                    DrawingEngineSetVSync(Config::Get().general.UseVSync);
                    Config::Save();
                    Invalidate();
                    break;
                case WIDX_USE_VSYNC_CHECKBOX:
                    Config::Get().general.UseVSync ^= 1;
                    DrawingEngineSetVSync(Config::Get().general.UseVSync);
                    Config::Save();
                    Invalidate();
                    break;
                case WIDX_SHOW_FPS_CHECKBOX:
                    Config::Get().general.ShowFPS ^= 1;
                    Config::Save();
                    Invalidate();
                    break;
                case WIDX_MULTITHREADING_CHECKBOX:
                    Config::Get().general.MultiThreading ^= 1;
                    Config::Save();
                    Invalidate();
                    break;
                case WIDX_MINIMIZE_FOCUS_LOSS:
                    Config::Get().general.MinimizeFullscreenFocusLoss ^= 1;
                    Config::Save();
                    Invalidate();
                    break;
                case WIDX_STEAM_OVERLAY_PAUSE:
                    Config::Get().general.SteamOverlayPause ^= 1;
                    Config::Save();
                    Invalidate();
                    break;
                case WIDX_DISABLE_SCREENSAVER_LOCK:
                    Config::Get().general.DisableScreensaver ^= 1;
                    ApplyScreenSaverLockSetting();
                    Config::Save();
                    Invalidate();
                    break;
            }
        }

        void DisplayMouseDown(WidgetIndex widgetIndex)
        {
            Widget* widget = &widgets[widgetIndex - 1];

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

                        if (resolution.Width == Config::Get().general.FullscreenWidth
                            && resolution.Height == Config::Get().general.FullscreenHeight)
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

                    Dropdown::SetChecked(Config::Get().general.FullscreenMode, true);
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
                    Dropdown::SetChecked(EnumValue(Config::Get().general.DrawingEngine), true);
                    break;
                }
                case WIDX_SCALE_UP:
                    Config::Get().general.WindowScale += 0.25f;
                    Config::Save();
                    GfxInvalidateScreen();
                    ContextTriggerResize();
                    ContextUpdateCursorScale();
                    break;
                case WIDX_SCALE_DOWN:
                    Config::Get().general.WindowScale -= 0.25f;
                    Config::Get().general.WindowScale = std::max(0.5f, Config::Get().general.WindowScale);
                    Config::Save();
                    GfxInvalidateScreen();
                    ContextTriggerResize();
                    ContextUpdateCursorScale();
                    break;
            }
        }

        void DisplayDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_RESOLUTION_DROPDOWN:
                {
                    const auto& resolutions = OpenRCT2::GetContext()->GetUiContext()->GetFullscreenResolutions();

                    const Resolution& resolution = resolutions[dropdownIndex];
                    if (resolution.Width != Config::Get().general.FullscreenWidth
                        || resolution.Height != Config::Get().general.FullscreenHeight)
                    {
                        Config::Get().general.FullscreenWidth = resolution.Width;
                        Config::Get().general.FullscreenHeight = resolution.Height;

                        if (Config::Get().general.FullscreenMode
                            == static_cast<int32_t>(OpenRCT2::Ui::FULLSCREEN_MODE::FULLSCREEN))
                            ContextSetFullscreenMode(static_cast<int32_t>(OpenRCT2::Ui::FULLSCREEN_MODE::FULLSCREEN));

                        Config::Save();
                        GfxInvalidateScreen();
                    }
                }
                break;
                case WIDX_FULLSCREEN_DROPDOWN:
                    if (dropdownIndex != Config::Get().general.FullscreenMode)
                    {
                        ContextSetFullscreenMode(dropdownIndex);

                        Config::Get().general.FullscreenMode = static_cast<uint8_t>(dropdownIndex);
                        Config::Save();
                        GfxInvalidateScreen();
                    }
                    break;
                case WIDX_DRAWING_ENGINE_DROPDOWN:
                    if (dropdownIndex != EnumValue(Config::Get().general.DrawingEngine))
                    {
                        DrawingEngine dstEngine = static_cast<DrawingEngine>(dropdownIndex);

                        Config::Get().general.DrawingEngine = dstEngine;
                        RefreshVideo();
                        Config::Save();
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
            ft.Add<uint16_t>(static_cast<uint16_t>(Config::Get().general.FullscreenWidth));
            ft.Add<uint16_t>(static_cast<uint16_t>(Config::Get().general.FullscreenHeight));

            // Disable resolution dropdown on "Windowed" and "Fullscreen (desktop)"
            if (Config::Get().general.FullscreenMode != static_cast<int32_t>(OpenRCT2::Ui::FULLSCREEN_MODE::FULLSCREEN))
            {
                disabled_widgets |= (1uLL << WIDX_RESOLUTION_DROPDOWN);
                disabled_widgets |= (1uLL << WIDX_RESOLUTION);
                disabled_widgets |= (1uLL << WIDX_RESOLUTION_LABEL);
            }
            else
            {
                disabled_widgets &= ~(1uLL << WIDX_RESOLUTION_DROPDOWN);
                disabled_widgets &= ~(1uLL << WIDX_RESOLUTION);
                disabled_widgets &= ~(1uLL << WIDX_RESOLUTION_LABEL);
            }

            // Disable Steam Overlay checkbox when using software or OpenGL rendering.
            if (Config::Get().general.DrawingEngine == DrawingEngine::Software
                || Config::Get().general.DrawingEngine == DrawingEngine::OpenGL)
            {
                disabled_widgets |= (1uLL << WIDX_STEAM_OVERLAY_PAUSE);
            }
            else
            {
                disabled_widgets &= ~(1uLL << WIDX_STEAM_OVERLAY_PAUSE);
            }

            // Disable changing VSync for Software engine, as we can't control its use of VSync
            if (Config::Get().general.DrawingEngine == DrawingEngine::Software)
            {
                disabled_widgets |= (1uLL << WIDX_USE_VSYNC_CHECKBOX);
            }
            else
            {
                disabled_widgets &= ~(1uLL << WIDX_USE_VSYNC_CHECKBOX);
            }

            SetCheckboxValue(WIDX_UNCAP_FPS_CHECKBOX, Config::Get().general.UncapFPS);
            SetCheckboxValue(WIDX_USE_VSYNC_CHECKBOX, Config::Get().general.UseVSync);
            SetCheckboxValue(WIDX_SHOW_FPS_CHECKBOX, Config::Get().general.ShowFPS);
            SetCheckboxValue(WIDX_MULTITHREADING_CHECKBOX, Config::Get().general.MultiThreading);
            SetCheckboxValue(WIDX_MINIMIZE_FOCUS_LOSS, Config::Get().general.MinimizeFullscreenFocusLoss);
            SetCheckboxValue(WIDX_STEAM_OVERLAY_PAUSE, Config::Get().general.SteamOverlayPause);
            SetCheckboxValue(WIDX_DISABLE_SCREENSAVER_LOCK, Config::Get().general.DisableScreensaver);

            // Dropdown captions for straightforward strings.
            widgets[WIDX_FULLSCREEN].text = FullscreenModeNames[Config::Get().general.FullscreenMode];
            widgets[WIDX_DRAWING_ENGINE].text = DrawingEngineStringIds[EnumValue(Config::Get().general.DrawingEngine)];
        }

        void DisplayDraw(DrawPixelInfo& dpi)
        {
            auto ft = Formatter();
            ft.Add<int32_t>(static_cast<int32_t>(Config::Get().general.WindowScale * 100));
            DrawTextBasic(
                dpi, windowPos + ScreenCoordsXY{ widgets[WIDX_SCALE].left + 1, widgets[WIDX_SCALE].top + 1 },
                STR_WINDOW_COLOUR_2_COMMA2DP32, ft, { colours[1] });
        }
#pragma endregion

#pragma region Rendering tab events
        void RenderingMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_TILE_SMOOTHING_CHECKBOX:
                    Config::Get().general.LandscapeSmoothing ^= 1;
                    Config::Save();
                    GfxInvalidateScreen();
                    break;
                case WIDX_GRIDLINES_CHECKBOX:
                {
                    Config::Get().general.AlwaysShowGridlines ^= 1;
                    Config::Save();
                    GfxInvalidateScreen();
                    WindowBase* mainWindow = WindowGetMain();
                    if (mainWindow != nullptr)
                    {
                        if (Config::Get().general.AlwaysShowGridlines)
                            mainWindow->viewport->flags |= VIEWPORT_FLAG_GRIDLINES;
                        else
                            mainWindow->viewport->flags &= ~VIEWPORT_FLAG_GRIDLINES;
                    }
                    break;
                }
                case WIDX_DAY_NIGHT_CHECKBOX:
                    Config::Get().general.DayNightCycle ^= 1;
                    Config::Save();
                    Invalidate();
                    break;
                case WIDX_ENABLE_LIGHT_FX_CHECKBOX:
                    Config::Get().general.EnableLightFx ^= 1;
                    Config::Save();
                    Invalidate();
                    break;
                case WIDX_ENABLE_LIGHT_FX_FOR_VEHICLES_CHECKBOX:
                    Config::Get().general.EnableLightFxForVehicles ^= 1;
                    Config::Save();
                    Invalidate();
                    break;
                case WIDX_UPPER_CASE_BANNERS_CHECKBOX:
                    Config::Get().general.UpperCaseBanners ^= 1;
                    Config::Save();
                    Invalidate();
                    ScrollingTextInvalidate();
                    break;
                case WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX:
                    Config::Get().general.DisableLightningEffect ^= 1;
                    Config::Save();
                    Invalidate();
                    break;
                case WIDX_RENDER_WEATHER_EFFECTS_CHECKBOX:
                    Config::Get().general.RenderWeatherEffects ^= 1;
                    Config::Get().general.RenderWeatherGloom = Config::Get().general.RenderWeatherEffects;
                    Config::Save();
                    Invalidate();
                    GfxInvalidateScreen();
                    break;
                case WIDX_SHOW_GUEST_PURCHASES_CHECKBOX:
                    Config::Get().general.ShowGuestPurchases ^= 1;
                    Config::Save();
                    Invalidate();
                    break;
                case WIDX_TRANSPARENT_SCREENSHOTS_CHECKBOX:
                    Config::Get().general.TransparentScreenshot ^= 1;
                    Config::Save();
                    Invalidate();
                    break;
            }
        }

        void RenderingMouseDown(WidgetIndex widgetIndex)
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

                    Widget* widget = &widgets[widgetIndex - 1];
                    ShowDropdown(widget, 3);

                    Dropdown::SetChecked(static_cast<int32_t>(Config::Get().general.VirtualFloorStyle), true);
                    break;
            }
        }

        void RenderingDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_VIRTUAL_FLOOR_DROPDOWN:
                    Config::Get().general.VirtualFloorStyle = static_cast<VirtualFloorStyles>(dropdownIndex);
                    Config::Save();
                    break;
            }
        }

        void RenderingPrepareDraw()
        {
            SetCheckboxValue(WIDX_TILE_SMOOTHING_CHECKBOX, Config::Get().general.LandscapeSmoothing);
            SetCheckboxValue(WIDX_GRIDLINES_CHECKBOX, Config::Get().general.AlwaysShowGridlines);
            SetCheckboxValue(WIDX_DAY_NIGHT_CHECKBOX, Config::Get().general.DayNightCycle);
            SetCheckboxValue(WIDX_SHOW_GUEST_PURCHASES_CHECKBOX, Config::Get().general.ShowGuestPurchases);
            SetCheckboxValue(WIDX_TRANSPARENT_SCREENSHOTS_CHECKBOX, Config::Get().general.TransparentScreenshot);
            SetCheckboxValue(WIDX_UPPER_CASE_BANNERS_CHECKBOX, Config::Get().general.UpperCaseBanners);

            static constexpr StringId _virtualFloorStyleStrings[] = {
                STR_VIRTUAL_FLOOR_STYLE_DISABLED,
                STR_VIRTUAL_FLOOR_STYLE_TRANSPARENT,
                STR_VIRTUAL_FLOOR_STYLE_GLASSY,
            };

            widgets[WIDX_VIRTUAL_FLOOR].text = _virtualFloorStyleStrings[EnumValue(Config::Get().general.VirtualFloorStyle)];

            SetCheckboxValue(WIDX_ENABLE_LIGHT_FX_CHECKBOX, Config::Get().general.EnableLightFx);
            if (Config::Get().general.DayNightCycle
                && Config::Get().general.DrawingEngine == DrawingEngine::SoftwareWithHardwareDisplay)
            {
                disabled_widgets &= ~(1uLL << WIDX_ENABLE_LIGHT_FX_CHECKBOX);
            }
            else
            {
                disabled_widgets |= (1uLL << WIDX_ENABLE_LIGHT_FX_CHECKBOX);
                Config::Get().general.EnableLightFx = false;
            }

            SetCheckboxValue(WIDX_ENABLE_LIGHT_FX_FOR_VEHICLES_CHECKBOX, Config::Get().general.EnableLightFxForVehicles);
            if (Config::Get().general.DayNightCycle
                && Config::Get().general.DrawingEngine == DrawingEngine::SoftwareWithHardwareDisplay
                && Config::Get().general.EnableLightFx)
            {
                disabled_widgets &= ~(1uLL << WIDX_ENABLE_LIGHT_FX_FOR_VEHICLES_CHECKBOX);
            }
            else
            {
                disabled_widgets |= (1uLL << WIDX_ENABLE_LIGHT_FX_FOR_VEHICLES_CHECKBOX);
                Config::Get().general.EnableLightFxForVehicles = false;
            }

            WidgetSetCheckboxValue(
                *this, WIDX_RENDER_WEATHER_EFFECTS_CHECKBOX,
                Config::Get().general.RenderWeatherEffects || Config::Get().general.RenderWeatherGloom);
            SetCheckboxValue(WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX, Config::Get().general.DisableLightningEffect);
            if (!Config::Get().general.RenderWeatherEffects && !Config::Get().general.RenderWeatherGloom)
            {
                SetCheckboxValue(WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX, true);
                disabled_widgets |= (1uLL << WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX);
            }
            else
            {
                disabled_widgets &= ~(1uLL << WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX);
            }
        }

#pragma endregion

#pragma region Culture tab events
        void CultureMouseDown(WidgetIndex widgetIndex)
        {
            Widget* widget = &widgets[widgetIndex - 1];

            switch (widgetIndex)
            {
                case WIDX_HEIGHT_LABELS_DROPDOWN:
                    gDropdownItems[0].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[1].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[0].Args = STR_HEIGHT_IN_UNITS;
                    gDropdownItems[1].Args = STR_REAL_VALUES;

                    ShowDropdown(widget, 2);

                    Dropdown::SetChecked(Config::Get().general.ShowHeightAsUnits ? 0 : 1, true);
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

                    gDropdownItems[numOrdinaryCurrencies].Format = Dropdown::kSeparatorString;

                    gDropdownItems[numOrdinaryCurrencies + 1].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[numOrdinaryCurrencies + 1].Args = CurrencyDescriptors[EnumValue(CurrencyType::Custom)]
                                                                         .stringId;

                    ShowDropdown(widget, numItems);

                    if (Config::Get().general.CurrencyFormat == CurrencyType::Custom)
                    {
                        Dropdown::SetChecked(EnumValue(Config::Get().general.CurrencyFormat) + 1, true);
                    }
                    else
                    {
                        Dropdown::SetChecked(EnumValue(Config::Get().general.CurrencyFormat), true);
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

                    Dropdown::SetChecked(static_cast<int32_t>(Config::Get().general.MeasurementFormat), true);
                    break;
                case WIDX_TEMPERATURE_DROPDOWN:
                    gDropdownItems[0].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[1].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[0].Args = STR_CELSIUS;
                    gDropdownItems[1].Args = STR_FAHRENHEIT;

                    ShowDropdown(widget, 2);

                    Dropdown::SetChecked(static_cast<int32_t>(Config::Get().general.TemperatureFormat), true);
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
                        gDropdownItems[i].Args = DateFormatStringIDs[i];
                    }
                    ShowDropdown(widget, 4);
                    Dropdown::SetChecked(Config::Get().general.DateFormat, true);
                    break;
            }
        }

        void CultureDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_HEIGHT_LABELS_DROPDOWN:
                    // reset flag and set it to 1 if height as units is selected
                    Config::Get().general.ShowHeightAsUnits = 0;

                    if (dropdownIndex == 0)
                    {
                        Config::Get().general.ShowHeightAsUnits = 1;
                    }
                    Config::Save();
                    UpdateHeightMarkers();
                    break;
                case WIDX_CURRENCY_DROPDOWN:
                    if (dropdownIndex == EnumValue(CurrencyType::Custom) + 1)
                    { // Add 1 because the separator occupies a position
                        Config::Get().general.CurrencyFormat = static_cast<CurrencyType>(dropdownIndex - 1);
                        ContextOpenWindow(WindowClass::CustomCurrencyConfig);
                    }
                    else
                    {
                        Config::Get().general.CurrencyFormat = static_cast<CurrencyType>(dropdownIndex);
                    }
                    Config::Save();
                    GfxInvalidateScreen();
                    break;
                case WIDX_DISTANCE_DROPDOWN:
                    Config::Get().general.MeasurementFormat = static_cast<MeasurementFormat>(dropdownIndex);
                    Config::Save();
                    UpdateHeightMarkers();
                    break;
                case WIDX_TEMPERATURE_DROPDOWN:
                    if (dropdownIndex != static_cast<int32_t>(Config::Get().general.TemperatureFormat))
                    {
                        Config::Get().general.TemperatureFormat = static_cast<TemperatureUnit>(dropdownIndex);
                        Config::Save();
                        GfxInvalidateScreen();
                    }
                    break;
                case WIDX_LANGUAGE_DROPDOWN:
                {
                    auto fallbackLanguage = LocalisationService_GetCurrentLanguage();
                    if (dropdownIndex != LocalisationService_GetCurrentLanguage() - 1)
                    {
                        if (!LanguageOpen(dropdownIndex + 1))
                        {
                            // Failed to open language file, try to recover by falling
                            // back to previously used language
                            if (LanguageOpen(fallbackLanguage))
                            {
                                // It worked, so we can say it with error message in-game
                                ContextShowError(STR_LANGUAGE_LOAD_FAILED, kStringIdNone, {});
                            }
                            // report error to console regardless
                            LOG_ERROR("Failed to open language file.");
                        }
                        else
                        {
                            Config::Get().general.Language = dropdownIndex + 1;
                            Config::Save();
                            GfxInvalidateScreen();
                        }
                    }
                }
                break;
                case WIDX_DATE_FORMAT_DROPDOWN:
                    if (dropdownIndex != Config::Get().general.DateFormat)
                    {
                        Config::Get().general.DateFormat = static_cast<uint8_t>(dropdownIndex);
                        Config::Save();
                        GfxInvalidateScreen();
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
            widgets[WIDX_CURRENCY].text = CurrencyDescriptors[EnumValue(Config::Get().general.CurrencyFormat)].stringId;

            // Distance: metric / imperial / si
            {
                StringId stringId = kStringIdNone;
                switch (Config::Get().general.MeasurementFormat)
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
            widgets[WIDX_DATE_FORMAT].text = DateFormatStringIDs[Config::Get().general.DateFormat];

            // Temperature: celsius/fahrenheit
            widgets[WIDX_TEMPERATURE].text = Config::Get().general.TemperatureFormat == TemperatureUnit::Fahrenheit
                ? STR_FAHRENHEIT
                : STR_CELSIUS;

            // Height: units/real values
            widgets[WIDX_HEIGHT_LABELS].text = Config::Get().general.ShowHeightAsUnits ? STR_HEIGHT_IN_UNITS : STR_REAL_VALUES;
        }

#pragma endregion

#pragma region Audio tab events
        void AudioMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_SOUND_CHECKBOX:
                    Config::Get().sound.SoundEnabled = !Config::Get().sound.SoundEnabled;
                    Config::Save();
                    Invalidate();
                    break;

                case WIDX_MASTER_SOUND_CHECKBOX:
                {
                    Config::Get().sound.MasterSoundEnabled = !Config::Get().sound.MasterSoundEnabled;
                    if (!Config::Get().sound.MasterSoundEnabled)
                        OpenRCT2::Audio::Pause();
                    else
                        OpenRCT2::Audio::Resume();

                    auto* windowMgr = Ui::GetWindowManager();
                    windowMgr->InvalidateByClass(WindowClass::TopToolbar);
                    Config::Save();
                    Invalidate();
                    break;
                }

                case WIDX_MUSIC_CHECKBOX:
                    Config::Get().sound.RideMusicEnabled = !Config::Get().sound.RideMusicEnabled;
                    if (!Config::Get().sound.RideMusicEnabled)
                    {
                        OpenRCT2::RideAudio::StopAllChannels();
                    }
                    Config::Save();
                    Invalidate();
                    break;

                case WIDX_AUDIO_FOCUS_CHECKBOX:
                    Config::Get().sound.audio_focus = !Config::Get().sound.audio_focus;
                    Config::Save();
                    Invalidate();
                    break;
            }
        }

        void AudioMouseDown(WidgetIndex widgetIndex)
        {
            Widget* widget = &widgets[widgetIndex - 1];

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
                    const bool rct1MusicThemeIsAvailable = IsRCT1TitleMusicAvailable();
                    int32_t numItems{};
                    int32_t checkedIndex{};
                    for (auto theme : TitleThemeOptions)
                    {
                        if (theme.Kind == TitleMusicKind::RCT1 && !rct1MusicThemeIsAvailable)
                            continue;

                        if (Config::Get().sound.TitleMusic == theme.Kind)
                            checkedIndex = numItems;

                        gDropdownItems[numItems].Format = STR_DROPDOWN_MENU_LABEL;
                        gDropdownItems[numItems++].Args = theme.Name;
                    }
                    ShowDropdown(widget, numItems);
                    Dropdown::SetChecked(checkedIndex, true);
                    break;
                }
            }
        }

        void AudioDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
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
                            Config::Get().sound.Device = "";
                        }
                        else
                        {
                            const auto& deviceName = GetDeviceName(dropdownIndex);
                            audioContext->SetOutputDevice(deviceName);
                            Config::Get().sound.Device = deviceName;
                        }
                        Config::Save();
                        OpenRCT2::Audio::PlayTitleMusic();
                    }
                    Invalidate();
                    break;
                case WIDX_TITLE_MUSIC_DROPDOWN:
                {
                    // HACK: When RCT1 is not available, it's not in the dropdown, so indices higher than it should be
                    // incremented
                    const bool rct1MusicThemeIsAvailable = IsRCT1TitleMusicAvailable();
                    for (size_t i = 0; i < std::size(TitleThemeOptions) && static_cast<int32_t>(i) <= dropdownIndex; i++)
                    {
                        if (TitleThemeOptions[i].Kind == TitleMusicKind::RCT1 && !rct1MusicThemeIsAvailable)
                            dropdownIndex++;
                    }

                    Config::Get().sound.TitleMusic = TitleThemeOptions[dropdownIndex].Kind;
                    Config::Save();
                    Invalidate();

                    OpenRCT2::Audio::StopTitleMusic();
                    if (Config::Get().sound.TitleMusic != TitleMusicKind::None)
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
            if (masterVolume != Config::Get().sound.MasterVolume)
            {
                Config::Get().sound.MasterVolume = masterVolume;
                Config::Save();
                InvalidateWidget(WIDX_MASTER_VOLUME);
            }

            const auto& soundVolumeWidget = widgets[WIDX_MASTER_VOLUME];
            const auto& soundVolumeScroll = scrolls[1];
            uint8_t soundVolume = GetScrollPercentage(soundVolumeWidget, soundVolumeScroll);
            if (soundVolume != Config::Get().sound.SoundVolume)
            {
                Config::Get().sound.SoundVolume = soundVolume;
                Config::Save();
                InvalidateWidget(WIDX_SOUND_VOLUME);
            }

            const auto& musicVolumeWidget = widgets[WIDX_MASTER_VOLUME];
            const auto& musicVolumeScroll = scrolls[2];
            uint8_t rideMusicVolume = GetScrollPercentage(musicVolumeWidget, musicVolumeScroll);
            if (rideMusicVolume != Config::Get().sound.AudioFocus)
            {
                Config::Get().sound.AudioFocus = rideMusicVolume;
                Config::Save();
                InvalidateWidget(WIDX_MUSIC_VOLUME);
            }
        }

        ScreenSize AudioScrollGetSize(int32_t scrollIndex)
        {
            return { 500, 0 };
        }

        StringId GetTitleMusicName() const
        {
            auto theme = std::find_if(std::begin(TitleThemeOptions), std::end(TitleThemeOptions), [](auto&& option) {
                return Config::Get().sound.TitleMusic == option.Kind;
            });
            if (theme != std::end(TitleThemeOptions))
                return theme->Name;
            return STR_OPENRCT2_DROPDOWN;
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

            SetCheckboxValue(WIDX_SOUND_CHECKBOX, Config::Get().sound.SoundEnabled);
            SetCheckboxValue(WIDX_MASTER_SOUND_CHECKBOX, Config::Get().sound.MasterSoundEnabled);
            SetCheckboxValue(WIDX_MUSIC_CHECKBOX, Config::Get().sound.RideMusicEnabled);
            SetCheckboxValue(WIDX_AUDIO_FOCUS_CHECKBOX, Config::Get().sound.audio_focus);
            WidgetSetEnabled(*this, WIDX_SOUND_CHECKBOX, Config::Get().sound.MasterSoundEnabled);
            WidgetSetEnabled(*this, WIDX_MUSIC_CHECKBOX, Config::Get().sound.MasterSoundEnabled);

            // Initialize only on first frame, otherwise the scrollbars won't be able to be modified
            if (frame_no == 0)
            {
                InitializeScrollPosition(WIDX_MASTER_VOLUME, 0, Config::Get().sound.MasterVolume);
                InitializeScrollPosition(WIDX_SOUND_VOLUME, 1, Config::Get().sound.SoundVolume);
                InitializeScrollPosition(WIDX_MUSIC_VOLUME, 2, Config::Get().sound.AudioFocus);
            }
        }

#pragma endregion

#pragma region Controls tab events
        void ToggleToolbarSetting(bool& setting)
        {
            setting ^= true;
            Config::Save();
            Invalidate();

            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->InvalidateByClass(WindowClass::TopToolbar);
        }

        void ControlsMouseUp(WidgetIndex widgetIndex)
        {
            auto* windowMgr = Ui::GetWindowManager();

            switch (widgetIndex)
            {
                case WIDX_HOTKEY_DROPDOWN:
                    ContextOpenWindow(WindowClass::KeyboardShortcutList);
                    break;
                case WIDX_SCREEN_EDGE_SCROLLING:
                    Config::Get().general.EdgeScrolling ^= 1;
                    Config::Save();
                    Invalidate();
                    break;
                case WIDX_TRAP_CURSOR:
                    Config::Get().general.TrapCursor ^= 1;
                    Config::Save();
                    ContextSetCursorTrap(Config::Get().general.TrapCursor);
                    Invalidate();
                    break;
                case WIDX_ZOOM_TO_CURSOR:
                    Config::Get().general.ZoomToCursor ^= 1;
                    Config::Save();
                    Invalidate();
                    break;
                case WIDX_TOOLBAR_BUTTONS_CENTRED:
                    ToggleToolbarSetting(Config::Get().interface.ToolbarButtonsCentred);
                    break;
                case WIDX_TOOLBAR_SHOW_FINANCES:
                    ToggleToolbarSetting(Config::Get().interface.ToolbarShowFinances);
                    break;
                case WIDX_TOOLBAR_SHOW_RESEARCH:
                    ToggleToolbarSetting(Config::Get().interface.ToolbarShowResearch);
                    break;
                case WIDX_TOOLBAR_SHOW_CHEATS:
                    ToggleToolbarSetting(Config::Get().interface.ToolbarShowCheats);
                    break;
                case WIDX_TOOLBAR_SHOW_NEWS:
                    ToggleToolbarSetting(Config::Get().interface.ToolbarShowNews);
                    break;
                case WIDX_TOOLBAR_SHOW_MUTE:
                    ToggleToolbarSetting(Config::Get().interface.ToolbarShowMute);
                    break;
                case WIDX_TOOLBAR_SHOW_CHAT:
                    ToggleToolbarSetting(Config::Get().interface.ToolbarShowChat);
                    break;
                case WIDX_TOOLBAR_SHOW_ZOOM:
                    ToggleToolbarSetting(Config::Get().interface.ToolbarShowZoom);
                    break;
                case WIDX_TOOLBAR_SHOW_ROTATE_ANTI_CLOCKWISE:
                    ToggleToolbarSetting(Config::Get().interface.ToolbarShowRotateAnticlockwise);
                    break;
                case WIDX_WINDOW_BUTTONS_ON_THE_LEFT:
                    Config::Get().interface.WindowButtonsOnTheLeft ^= 1;
                    Config::Save();
                    Invalidate();
                    windowMgr->InvalidateAll();
                    break;
                case WIDX_ENLARGED_UI:
                    Config::Get().interface.EnlargedUi ^= 1;
                    if (Config::Get().interface.EnlargedUi == false)
                        Config::Get().interface.TouchEnhancements = false;
                    Config::Save();
                    Invalidate();
                    windowMgr->InvalidateAll();
                    break;
                case WIDX_TOUCH_ENHANCEMENTS:
                    Config::Get().interface.TouchEnhancements ^= 1;
                    Config::Save();
                    Invalidate();
                    windowMgr->InvalidateAll();
                    break;
                case WIDX_INVERT_DRAG:
                    Config::Get().general.InvertViewportDrag ^= 1;
                    Config::Save();
                    Invalidate();
                    break;
                case WIDX_THEMES_BUTTON:
                    ContextOpenWindow(WindowClass::Themes);
                    Invalidate();
                    break;
            }
        }

        void ControlsMouseDown(WidgetIndex widgetIndex)
        {
            Widget* widget = &widgets[widgetIndex - 1];

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

        void ControlsDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_THEMES_DROPDOWN:
                    if (dropdownIndex != -1)
                    {
                        ThemeManagerSetActiveAvailableTheme(dropdownIndex);
                    }
                    Config::Save();
                    break;
            }
        }

        void ControlsPrepareDraw()
        {
            SetCheckboxValue(WIDX_SCREEN_EDGE_SCROLLING, Config::Get().general.EdgeScrolling);
            SetCheckboxValue(WIDX_TRAP_CURSOR, Config::Get().general.TrapCursor);
            SetCheckboxValue(WIDX_INVERT_DRAG, Config::Get().general.InvertViewportDrag);
            SetCheckboxValue(WIDX_ZOOM_TO_CURSOR, Config::Get().general.ZoomToCursor);
            SetCheckboxValue(WIDX_TOOLBAR_BUTTONS_CENTRED, Config::Get().interface.ToolbarButtonsCentred);
            SetCheckboxValue(WIDX_TOOLBAR_SHOW_FINANCES, Config::Get().interface.ToolbarShowFinances);
            SetCheckboxValue(WIDX_TOOLBAR_SHOW_RESEARCH, Config::Get().interface.ToolbarShowResearch);
            SetCheckboxValue(WIDX_TOOLBAR_SHOW_CHEATS, Config::Get().interface.ToolbarShowCheats);
            SetCheckboxValue(WIDX_TOOLBAR_SHOW_NEWS, Config::Get().interface.ToolbarShowNews);
            SetCheckboxValue(WIDX_TOOLBAR_SHOW_MUTE, Config::Get().interface.ToolbarShowMute);
            SetCheckboxValue(WIDX_TOOLBAR_SHOW_CHAT, Config::Get().interface.ToolbarShowChat);
            SetCheckboxValue(WIDX_TOOLBAR_SHOW_ZOOM, Config::Get().interface.ToolbarShowZoom);
            SetCheckboxValue(WIDX_TOOLBAR_SHOW_ROTATE_ANTI_CLOCKWISE, Config::Get().interface.ToolbarShowRotateAnticlockwise);
            SetCheckboxValue(WIDX_WINDOW_BUTTONS_ON_THE_LEFT, Config::Get().interface.WindowButtonsOnTheLeft);
            SetCheckboxValue(WIDX_ENLARGED_UI, Config::Get().interface.EnlargedUi);
            SetCheckboxValue(WIDX_TOUCH_ENHANCEMENTS, Config::Get().interface.TouchEnhancements);

            WidgetSetEnabled(*this, WIDX_TOUCH_ENHANCEMENTS, Config::Get().interface.EnlargedUi);

            size_t activeAvailableThemeIndex = ThemeManagerGetAvailableThemeIndex();
            const utf8* activeThemeName = ThemeManagerGetAvailableThemeName(activeAvailableThemeIndex);
            auto ft = Formatter::Common();
            ft.Add<utf8*>(activeThemeName);
        }

#pragma endregion

#pragma region Misc tab events
        void MiscMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_REAL_NAMES_GUESTS_CHECKBOX:
                    Config::Get().general.ShowRealNamesOfGuests ^= 1;
                    Config::Save();
                    Invalidate();
                    PeepUpdateNames();
                    break;
                case WIDX_REAL_NAMES_STAFF_CHECKBOX:
                    Config::Get().general.ShowRealNamesOfStaff ^= 1;
                    Config::Save();
                    Invalidate();
                    PeepUpdateNames();
                    break;
                case WIDX_AUTO_STAFF_PLACEMENT:
                    Config::Get().general.AutoStaffPlacement ^= 1;
                    Config::Save();
                    Invalidate();
                    break;
                case WIDX_SCENARIO_UNLOCKING:
                {
                    Config::Get().general.ScenarioUnlockingEnabled ^= 1;
                    Config::Save();
                    auto* windowMgr = Ui::GetWindowManager();
                    windowMgr->CloseByClass(WindowClass::ScenarioSelect);
                    break;
                }
                case WIDX_AUTO_OPEN_SHOPS:
                    Config::Get().general.AutoOpenShops = !Config::Get().general.AutoOpenShops;
                    Config::Save();
                    Invalidate();
                    break;
                case WIDX_ALLOW_EARLY_COMPLETION:
                    Config::Get().general.AllowEarlyCompletion ^= 1;
                    // Only the server can control this setting and needs to send the
                    // current value of allow_early_completion to all clients
                    if (NetworkGetMode() == NETWORK_MODE_SERVER)
                    {
                        auto setAllowEarlyCompletionAction = ScenarioSetSettingAction(
                            ScenarioSetSetting::AllowEarlyCompletion, Config::Get().general.AllowEarlyCompletion);
                        GameActions::Execute(&setAllowEarlyCompletionAction);
                    }
                    Config::Save();
                    Invalidate();
                    break;
            }
        }

        void MiscMouseDown(WidgetIndex widgetIndex)
        {
            Widget* widget = &widgets[widgetIndex - 1];

            switch (widgetIndex)
            {
                case WIDX_TITLE_SEQUENCE_DROPDOWN:
                {
                    uint32_t numItems = static_cast<int32_t>(TitleSequenceManager::GetCount());
                    for (size_t i = 0; i < numItems; i++)
                    {
                        gDropdownItems[i].Format = STR_OPTIONS_DROPDOWN_ITEM;
                        gDropdownItems[i].Args = reinterpret_cast<uintptr_t>(TitleSequenceManager::GetName(i));
                    }

                    gDropdownItems[numItems].Format = 0;
                    numItems++;
                    gDropdownItems[numItems].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[numItems].Args = STR_TITLE_SEQUENCE_RANDOM;
                    numItems++;

                    WindowDropdownShowText(
                        { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height() + 1, colours[1],
                        Dropdown::Flag::StayOpen, numItems);

                    auto selectedIndex = Config::Get().interface.RandomTitleSequence
                        ? numItems - 1
                        : static_cast<int32_t>(TitleGetCurrentSequence());
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

                    Dropdown::SetChecked(Config::Get().general.ScenarioSelectMode, true);
                    break;
                }
                case WIDX_DEFAULT_INSPECTION_INTERVAL_DROPDOWN:
                    for (size_t i = 0; i < 7; i++)
                    {
                        gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                        gDropdownItems[i].Args = kRideInspectionIntervalNames[i];
                    }

                    ShowDropdown(widget, 7);
                    Dropdown::SetChecked(Config::Get().general.DefaultInspectionInterval, true);
                    break;
            }
        }

        void MiscDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_TITLE_SEQUENCE_DROPDOWN:
                {
                    auto numItems = static_cast<int32_t>(TitleSequenceManager::GetCount());
                    if (dropdownIndex < numItems && dropdownIndex != static_cast<int32_t>(TitleGetCurrentSequence()))
                    {
                        Config::Get().interface.RandomTitleSequence = false;
                        TitleSequenceChangePreset(static_cast<size_t>(dropdownIndex));
                        Config::Save();
                        Invalidate();
                    }
                    else if (dropdownIndex == numItems + 1)
                    {
                        Config::Get().interface.RandomTitleSequence = true;
                        Config::Save();
                        Invalidate();
                    }
                    break;
                }
                case WIDX_DEFAULT_INSPECTION_INTERVAL_DROPDOWN:
                    if (dropdownIndex != Config::Get().general.DefaultInspectionInterval)
                    {
                        Config::Get().general.DefaultInspectionInterval = static_cast<uint8_t>(dropdownIndex);
                        Config::Save();
                        Invalidate();
                    }
                    break;
                case WIDX_SCENARIO_GROUPING_DROPDOWN:
                    if (dropdownIndex != Config::Get().general.ScenarioSelectMode)
                    {
                        Config::Get().general.ScenarioSelectMode = dropdownIndex;
                        Config::Get().interface.ScenarioselectLastTab = 0;
                        Config::Save();
                        Invalidate();
                        auto* windowMgr = Ui::GetWindowManager();
                        windowMgr->CloseByClass(WindowClass::ScenarioSelect);
                    }
                    break;
            }
        }

        void MiscPrepareDraw()
        {
            auto ft = Formatter::Common();
            if (Config::Get().interface.RandomTitleSequence)
            {
                ft.Add<StringId>(STR_TITLE_SEQUENCE_RANDOM);
            }
            else
            {
                auto name = TitleSequenceManager::GetName(TitleGetConfigSequence());
                ft.Add<StringId>(STR_STRING);
                ft.Add<utf8*>(name);
            }

            // The real name setting of clients is fixed to that of the server
            // and the server cannot change the setting during gameplay to prevent desyncs
            if (NetworkGetMode() != NETWORK_MODE_NONE)
            {
                disabled_widgets |= (1uLL << WIDX_REAL_NAMES_GUESTS_CHECKBOX) | (1uLL << WIDX_REAL_NAMES_STAFF_CHECKBOX);
                widgets[WIDX_REAL_NAMES_GUESTS_CHECKBOX].tooltip = STR_OPTION_DISABLED_DURING_NETWORK_PLAY;
                widgets[WIDX_REAL_NAMES_STAFF_CHECKBOX].tooltip = STR_OPTION_DISABLED_DURING_NETWORK_PLAY;

                // Disable the use of the allow_early_completion option during network play on clients.
                // This is to prevent confusion on clients because changing this setting during network play wouldn't change
                // the way scenarios are completed during this network-session
                if (NetworkGetMode() == NETWORK_MODE_CLIENT)
                {
                    disabled_widgets |= (1uLL << WIDX_ALLOW_EARLY_COMPLETION);
                    widgets[WIDX_ALLOW_EARLY_COMPLETION].tooltip = STR_OPTION_DISABLED_DURING_NETWORK_PLAY;
                }
            }

            SetCheckboxValue(WIDX_REAL_NAMES_GUESTS_CHECKBOX, Config::Get().general.ShowRealNamesOfGuests);
            SetCheckboxValue(WIDX_REAL_NAMES_STAFF_CHECKBOX, Config::Get().general.ShowRealNamesOfStaff);
            SetCheckboxValue(WIDX_AUTO_STAFF_PLACEMENT, Config::Get().general.AutoStaffPlacement);
            SetCheckboxValue(WIDX_AUTO_OPEN_SHOPS, Config::Get().general.AutoOpenShops);
            SetCheckboxValue(WIDX_ALLOW_EARLY_COMPLETION, Config::Get().general.AllowEarlyCompletion);

            if (Config::Get().general.ScenarioSelectMode == SCENARIO_SELECT_MODE_DIFFICULTY)
                widgets[WIDX_SCENARIO_GROUPING].text = STR_OPTIONS_SCENARIO_DIFFICULTY;
            else
                widgets[WIDX_SCENARIO_GROUPING].text = STR_OPTIONS_SCENARIO_ORIGIN;

            SetCheckboxValue(WIDX_SCENARIO_UNLOCKING, Config::Get().general.ScenarioUnlockingEnabled);

            if (Config::Get().general.ScenarioSelectMode == SCENARIO_SELECT_MODE_ORIGIN)
            {
                disabled_widgets &= ~(1uLL << WIDX_SCENARIO_UNLOCKING);
            }
            else
            {
                disabled_widgets |= (1uLL << WIDX_SCENARIO_UNLOCKING);
            }

            widgets[WIDX_DEFAULT_INSPECTION_INTERVAL].text = kRideInspectionIntervalNames
                [Config::Get().general.DefaultInspectionInterval];
        }

#pragma endregion

#pragma region Advanced tab events
        void AdvancedMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_DEBUGGING_TOOLS:
                    Config::Get().general.DebuggingTools ^= 1;
                    Config::Save();
                    GfxInvalidateScreen();
                    break;
                case WIDX_SAVE_PLUGIN_DATA_CHECKBOX:
                    Config::Get().general.SavePluginData ^= 1;
                    Config::Save();
                    Invalidate();
                    break;
                case WIDX_STAY_CONNECTED_AFTER_DESYNC:
                    Config::Get().network.StayConnected = !Config::Get().network.StayConnected;
                    Config::Save();
                    Invalidate();
                    break;
                case WIDX_ALWAYS_NATIVE_LOADSAVE:
                    Config::Get().general.UseNativeBrowseDialog = !Config::Get().general.UseNativeBrowseDialog;
                    Config::Save();
                    Invalidate();
                    break;
                case WIDX_PATH_TO_RCT1_BUTTON:
                {
                    auto rct1path = OpenRCT2::GetContext()->GetUiContext()->ShowDirectoryDialog(
                        LanguageGetString(STR_PATH_TO_RCT1_BROWSER));
                    if (!rct1path.empty())
                    {
                        // Check if this directory actually contains RCT1
                        if (Csg1datPresentAtLocation(rct1path))
                        {
                            if (Csg1idatPresentAtLocation(rct1path))
                            {
                                if (CsgAtLocationIsUsable(rct1path))
                                {
                                    Config::Get().general.RCT1Path = std::move(rct1path);
                                    Config::Get().interface.ScenarioselectLastTab = 0;
                                    Config::Save();
                                    ContextShowError(STR_RESTART_REQUIRED, kStringIdNone, {});
                                }
                                else
                                {
                                    ContextShowError(STR_PATH_TO_RCT1_IS_WRONG_VERSION, kStringIdNone, {});
                                }
                            }
                            else
                            {
                                ContextShowError(STR_PATH_TO_RCT1_DOES_NOT_CONTAIN_CSG1I_DAT, kStringIdNone, {});
                            }
                        }
                        else
                        {
                            ContextShowError(STR_PATH_TO_RCT1_WRONG_ERROR, kStringIdNone, {});
                        }
                    }
                    Invalidate();
                    break;
                }
                case WIDX_PATH_TO_RCT1_CLEAR:
                    if (!Config::Get().general.RCT1Path.empty())
                    {
                        Config::Get().general.RCT1Path.clear();
                        Config::Save();
                    }
                    Invalidate();
                    break;
                case WIDX_ASSET_PACKS:
                    ContextOpenWindow(WindowClass::AssetPacks);
                    break;
#ifdef __EMSCRIPTEN__
                case WIDX_EXPORT_EMSCRIPTEN_DATA:
                    ExportPersistentData();
                    break;
                case WIDX_IMPORT_EMSCRIPTEN_DATA:
                    ImportPersistentData();
                    break;
#endif
            }
        }

        void AdvancedMouseDown(WidgetIndex widgetIndex)
        {
            Widget* widget = &widgets[widgetIndex - 1];

            switch (widgetIndex)
            {
                case WIDX_AUTOSAVE_FREQUENCY_DROPDOWN:
                    for (size_t i = AUTOSAVE_EVERY_MINUTE; i <= AUTOSAVE_NEVER; i++)
                    {
                        gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                        gDropdownItems[i].Args = AutosaveNames[i];
                    }

                    ShowDropdown(widget, AUTOSAVE_NEVER + 1);
                    Dropdown::SetChecked(Config::Get().general.AutosaveFrequency, true);
                    break;
                case WIDX_AUTOSAVE_AMOUNT_UP:
                    Config::Get().general.AutosaveAmount += 1;
                    Config::Save();
                    InvalidateWidget(WIDX_AUTOSAVE_FREQUENCY);
                    InvalidateWidget(WIDX_AUTOSAVE_FREQUENCY_DROPDOWN);
                    InvalidateWidget(WIDX_AUTOSAVE_AMOUNT);
                    break;
                case WIDX_AUTOSAVE_AMOUNT_DOWN:
                    if (Config::Get().general.AutosaveAmount > 1)
                    {
                        Config::Get().general.AutosaveAmount -= 1;
                        Config::Save();
                        InvalidateWidget(WIDX_AUTOSAVE_FREQUENCY);
                        InvalidateWidget(WIDX_AUTOSAVE_FREQUENCY_DROPDOWN);
                        InvalidateWidget(WIDX_AUTOSAVE_AMOUNT);
                    }
            }
        }

        void AdvancedDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_AUTOSAVE_FREQUENCY_DROPDOWN:
                    if (dropdownIndex != Config::Get().general.AutosaveFrequency)
                    {
                        Config::Get().general.AutosaveFrequency = static_cast<uint8_t>(dropdownIndex);
                        Config::Save();
                        Invalidate();
                    }
                    break;
            }
        }

        void AdvancedPrepareDraw()
        {
            SetCheckboxValue(WIDX_DEBUGGING_TOOLS, Config::Get().general.DebuggingTools);
            SetCheckboxValue(WIDX_SAVE_PLUGIN_DATA_CHECKBOX, Config::Get().general.SavePluginData);
            SetCheckboxValue(WIDX_STAY_CONNECTED_AFTER_DESYNC, Config::Get().network.StayConnected);
            SetCheckboxValue(WIDX_ALWAYS_NATIVE_LOADSAVE, Config::Get().general.UseNativeBrowseDialog);
            widgets[WIDX_AUTOSAVE_FREQUENCY].text = AutosaveNames[Config::Get().general.AutosaveFrequency];
        }

        void AdvancedDraw(DrawPixelInfo& dpi)
        {
            auto ft = Formatter();
            ft.Add<int32_t>(static_cast<int32_t>(Config::Get().general.AutosaveAmount));
            DrawTextBasic(
                dpi,
                windowPos + ScreenCoordsXY{ widgets[WIDX_AUTOSAVE_AMOUNT].left + 1, widgets[WIDX_AUTOSAVE_AMOUNT].top + 1 },
                STR_WINDOW_COLOUR_2_COMMA32, ft, { colours[1] });

            const auto normalisedPath = Platform::StrDecompToPrecomp(Config::Get().general.RCT1Path);
            ft = Formatter();
            ft.Add<const utf8*>(normalisedPath.c_str());

            Widget pathWidget = widgets[WIDX_PATH_TO_RCT1_BUTTON];

            // Apply vertical alignment if appropriate.
            int32_t widgetHeight = pathWidget.bottom - pathWidget.top;
            int32_t lineHeight = FontGetLineHeight(FontStyle::Medium);
            uint32_t padding = widgetHeight > lineHeight ? (widgetHeight - lineHeight) / 2 : 0;
            ScreenCoordsXY screenCoords = { windowPos.x + pathWidget.left + 1,
                                            windowPos.y + pathWidget.top + static_cast<int32_t>(padding) };
            DrawTextEllipsised(dpi, screenCoords, 277, STR_STRING, ft, { colours[1] });
        }

        OpenRCT2String AdvancedTooltip(WidgetIndex widgetIndex, StringId fallback)
        {
            if (widgetIndex == WIDX_PATH_TO_RCT1_BUTTON)
            {
                if (Config::Get().general.RCT1Path.empty())
                {
                    // No tooltip if the path is empty
                    return { kStringIdNone, {} };
                }

                auto ft = Formatter();
                ft.Add<utf8*>(Config::Get().general.RCT1Path.c_str());
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
            SetWidgets(window_options_page_widgets[page]);

            Invalidate();
            OnResize();
            OnPrepareDraw();
            InitScrollWidgets();
            Invalidate();
        }

        void SetPressedTab()
        {
            for (int32_t i = 0; i < WINDOW_OPTIONS_PAGE_COUNT; i++)
                pressed_widgets &= ~(1 << (WIDX_FIRST_TAB + i));
            pressed_widgets |= 1LL << (WIDX_FIRST_TAB + page);
        }

        void ShowDropdown(Widget* widget, int32_t num_items)
        {
            // helper function, all dropdown boxes have similar properties
            WindowDropdownShowTextCustomWidth(
                { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height() + 1, colours[1], 0,
                Dropdown::Flag::StayOpen, num_items, widget->width() - 3);
        }

        void DrawTabImages(DrawPixelInfo& dpi)
        {
            DrawTabImage(dpi, WINDOW_OPTIONS_PAGE_DISPLAY, SPR_TAB_PAINT_0);
            DrawTabImage(dpi, WINDOW_OPTIONS_PAGE_RENDERING, SPR_G2_TAB_TREE);
            DrawTabImage(dpi, WINDOW_OPTIONS_PAGE_CULTURE, SPR_TAB_TIMER_0);
            DrawTabImage(dpi, WINDOW_OPTIONS_PAGE_AUDIO, SPR_TAB_MUSIC_0);
            DrawTabImage(dpi, WINDOW_OPTIONS_PAGE_CONTROLS_AND_INTERFACE, SPR_TAB_GEARS_0);
            DrawTabImage(dpi, WINDOW_OPTIONS_PAGE_MISC, SPR_TAB_RIDE_0);
            DrawTabImage(dpi, WINDOW_OPTIONS_PAGE_ADVANCED, SPR_TAB_WRENCH_0);
        }

        void DrawTabImage(DrawPixelInfo& dpi, int32_t p, int32_t spriteIndex)
        {
            WidgetIndex widgetIndex = WIDX_FIRST_TAB + p;
            Widget* widget = &widgets[widgetIndex];

            auto screenCoords = windowPos + ScreenCoordsXY{ widget->left, widget->top };

            if (!WidgetIsDisabled(*this, widgetIndex))
            {
                if (page == p)
                {
                    int32_t frame = frame_no / TabAnimationDivisor[page];
                    spriteIndex += (frame % TabAnimationFrames[page]);
                }

                // Draw normal, enabled sprite.
                GfxDrawSprite(dpi, ImageId(spriteIndex), screenCoords);
            }
            else
            {
                // Get the window background colour
                auto windowColour = colours[widget->colour].colour;

                // Draw greyed out (light border bottom right shadow)
                GfxDrawSpriteSolid(
                    dpi, ImageId(spriteIndex), screenCoords + ScreenCoordsXY{ 1, 1 }, ColourMapA[windowColour].lighter);

                // Draw greyed out (dark)
                GfxDrawSpriteSolid(dpi, ImageId(spriteIndex), screenCoords, ColourMapA[windowColour].mid_light);
            }
        }

        void UpdateHeightMarkers()
        {
            Config::Save();
            GfxInvalidateScreen();
        }

        uint8_t GetScrollPercentage(const Widget& widget, const ScrollArea& scroll)
        {
            uint8_t w = widget.width() - 1;
            return static_cast<float>(scroll.contentOffsetX) / (scroll.contentWidth - w) * 100;
        }

        void InitializeScrollPosition(WidgetIndex widgetIndex, int32_t scrollId, uint8_t volume)
        {
            const auto& widget = widgets[widgetIndex];
            auto& scroll = scrolls[scrollId];

            int32_t widgetSize = scroll.contentWidth - (widget.width() - 1);
            scroll.contentOffsetX = ceil(volume / 100.0f * widgetSize);

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

        static constexpr struct
        {
            TitleMusicKind Kind;
            StringId Name;
        } TitleThemeOptions[] = {
            { TitleMusicKind::None, STR_OPTIONS_MUSIC_VALUE_NONE },
            { TitleMusicKind::OpenRCT2, STR_OPENRCT2_DROPDOWN },
            { TitleMusicKind::RCT1, STR_ROLLERCOASTER_TYCOON_1_DROPDOWN },
            { TitleMusicKind::RCT2, STR_ROLLERCOASTER_TYCOON_2_DROPDOWN },
            { TitleMusicKind::Random, STR_OPTIONS_MUSIC_VALUE_RANDOM },
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
    WindowBase* OptionsOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<OptionsWindow>(WindowClass::Options, WW, WH, WF_CENTRE_SCREEN);
    }
} // namespace OpenRCT2::Ui::Windows
