/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <openrct2/GameState.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/ScenarioSetSettingAction.h>
#include <openrct2/audio/Audio.h>
#include <openrct2/audio/AudioContext.h>
#include <openrct2/audio/AudioMixer.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/EnumUtils.hpp>
#include <openrct2/core/File.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/drawing/ScrollingText.h>
#include <openrct2/localisation/Currency.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Language.h>
#include <openrct2/localisation/Localisation.Date.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/network/Network.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/rct1/Csg.h>
#include <openrct2/ride/RideAudio.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/scenes/title/TitleScene.h>
#include <openrct2/scenes/title/TitleSequenceManager.h>
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
using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
    enum WindowOptionsPage
    {
        WINDOW_OPTIONS_PAGE_DISPLAY,
        WINDOW_OPTIONS_PAGE_RENDERING,
        WINDOW_OPTIONS_PAGE_CULTURE,
        WINDOW_OPTIONS_PAGE_AUDIO,
        WINDOW_OPTIONS_PAGE_INTERFACE,
        WINDOW_OPTIONS_PAGE_CONTROLS,
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
        WIDX_TAB_INTERFACE,
        WIDX_TAB_CONTROLS,
        WIDX_TAB_MISC,
        WIDX_TAB_ADVANCED,

        WIDX_PAGE_START,

        // Display
        WIDX_WINDOW_GROUP = WIDX_PAGE_START,
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

        WIDX_RENDERING_GROUP,
        WIDX_DRAWING_ENGINE_LABEL,
        WIDX_DRAWING_ENGINE,
        WIDX_DRAWING_ENGINE_DROPDOWN,
        WIDX_FRAME_RATE_LIMIT_LABEL,
        WIDX_FRAME_RATE_LIMIT,
        WIDX_FRAME_RATE_LIMIT_DROPDOWN,
        WIDX_SHOW_FPS_CHECKBOX,
        WIDX_MULTITHREADING_CHECKBOX,

        WIDX_BEHAVIOUR_GROUP,
        WIDX_MINIMIZE_FOCUS_LOSS,
        WIDX_DISABLE_SCREENSAVER_LOCK,

        // Map rendering
        WIDX_MAP_RENDERING_GROUP = WIDX_PAGE_START,
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

        // Interface
        WIDX_THEMES_GROUP = WIDX_PAGE_START,
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

        // Controls
        WIDX_CONTROLS_GROUP = WIDX_PAGE_START,
        WIDX_SCREEN_EDGE_SCROLLING,
        WIDX_TRAP_CURSOR,
        WIDX_INVERT_DRAG,
        WIDX_ZOOM_TO_CURSOR,
        WIDX_WINDOW_BUTTONS_ON_THE_LEFT,
        WIDX_ENLARGED_UI,
        WIDX_TOUCH_ENHANCEMENTS,
        WIDX_HOTKEY_DROPDOWN,

        // Gamepad
        WIDX_GAMEPAD_GROUP,
        WIDX_GAMEPAD_DEADZONE_LABEL,
        WIDX_GAMEPAD_DEADZONE,
        WIDX_GAMEPAD_SENSITIVITY_LABEL,
        WIDX_GAMEPAD_SENSITIVITY,

        // Misc
        WIDX_TITLE_SEQUENCE_GROUP = WIDX_PAGE_START,
        WIDX_TITLE_SEQUENCE,
        WIDX_TITLE_SEQUENCE_DROPDOWN,
        WIDX_SCENARIO_OPTIONS_GROUP,
        WIDX_SCENARIO_PREVIEWS_LABEL,
        WIDX_SCENARIO_PREVIEWS,
        WIDX_SCENARIO_PREVIEWS_DROPDOWN,
        WIDX_SCENARIO_UNLOCKING,
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
        WIDX_GROUP_RCT1_PATH = WIDX_PAGE_START,
        WIDX_PATH_TO_RCT1_LABEL,
        WIDX_PATH_TO_RCT1_PATH,
        WIDX_PATH_TO_RCT1_BROWSE,
        WIDX_PATH_TO_RCT1_CLEAR,

        WIDX_GROUP_SAVING,
        WIDX_EXPORT_CUSTOM_OBJECTS_CHECKBOX,
        WIDX_ALWAYS_NATIVE_LOADSAVE,
        WIDX_AUTOSAVE_FREQUENCY_LABEL,
        WIDX_AUTOSAVE_FREQUENCY,
        WIDX_AUTOSAVE_FREQUENCY_DROPDOWN,
        WIDX_AUTOSAVE_AMOUNT_LABEL,
        WIDX_AUTOSAVE_AMOUNT,
        WIDX_AUTOSAVE_AMOUNT_UP,
        WIDX_AUTOSAVE_AMOUNT_DOWN,

        WIDX_GROUP_ADVANCED,
        WIDX_DEBUGGING_TOOLS,
        WIDX_STAY_CONNECTED_AFTER_DESYNC,
#ifdef __EMSCRIPTEN__
        WIDX_EXPORT_EMSCRIPTEN_DATA,
        WIDX_IMPORT_EMSCRIPTEN_DATA,
#endif
        WIDX_ASSET_PACKS,
    };

    // clang-format off
    static constexpr StringId kWindowTitle = STR_OPTIONS_TITLE;
    static constexpr ScreenSize kWindowSize = { 310, 332 };

    static constexpr auto kMainOptionsWidgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget({   0, 43 }, { kWindowSize.width, 289 }, WidgetType::resize, WindowColour::secondary),
        makeTab   ({   3, 17 }, STR_OPTIONS_DISPLAY_TIP),
        makeTab   ({  34, 17 }, STR_OPTIONS_RENDERING_TIP),
        makeTab   ({  65, 17 }, STR_OPTIONS_CULTURE_TIP),
        makeTab   ({  96, 17 }, STR_OPTIONS_AUDIO_TIP),
        makeTab   ({ 127, 17 }, STR_OPTIONS_INTERFACE_TIP),
        makeTab   ({ 158, 17 }, STR_OPTIONS_CONTROLS_TIP),
        makeTab   ({ 189, 17 }, STR_OPTIONS_MISCELLANEOUS_TIP),
        makeTab   ({ 220, 17 }, STR_OPTIONS_ADVANCED)
    );

    static constexpr auto window_options_display_widgets = makeWidgets(
        kMainOptionsWidgets,
        makeWidget        ({  5,  53}, {300,  64}, WidgetType::groupbox,     WindowColour::secondary, STR_GROUP_WINDOW                                                                ), // Window group
        makeWidget        ({ 10,  67}, {145,  12}, WidgetType::label,        WindowColour::secondary, STR_FULLSCREEN_MODE,                   STR_FULLSCREEN_MODE_TIP                  ), // Fullscreen
        makeWidget        ({155,  68}, {145,  12}, WidgetType::dropdownMenu, WindowColour::secondary                                                                                  ),
        makeWidget        ({288,  69}, { 11,  10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,                    STR_FULLSCREEN_MODE_TIP                  ),
        makeWidget        ({ 24,  82}, {145,  12}, WidgetType::label,        WindowColour::secondary, STR_DISPLAY_RESOLUTION,                STR_DISPLAY_RESOLUTION_TIP               ), // Resolution
        makeWidget        ({155,  83}, {145,  12}, WidgetType::dropdownMenu, WindowColour::secondary, STR_ARG_16_RESOLUTION_X_BY_Y                                                    ),
        makeWidget        ({288,  84}, { 11,  10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,                    STR_DISPLAY_RESOLUTION_TIP               ),
        makeWidget        ({ 10,  98}, {145,  12}, WidgetType::label,        WindowColour::secondary, STR_UI_SCALING_DESC,                   STR_WINDOW_SCALE_TIP                     ), // Scale
        makeSpinnerWidgets({155,  98}, {145,  12}, WidgetType::spinner,      WindowColour::secondary, kStringIdNone,                         STR_WINDOW_SCALE_TIP                     ), // Scale spinner (3 widgets)

        makeWidget        ({  5, 121}, {300,  64}, WidgetType::groupbox,     WindowColour::secondary, STR_GROUP_RENDERING                                                             ), // Rendering group
        makeWidget        ({ 10, 135}, {145,  12}, WidgetType::label,        WindowColour::secondary, STR_DRAWING_ENGINE,                    STR_DRAWING_ENGINE_TIP                   ), // Drawing engine (label)
        makeWidget        ({155, 135}, {145,  12}, WidgetType::dropdownMenu, WindowColour::secondary                                                                                  ), // Drawing engine (dropdown label)
        makeWidget        ({288, 136}, { 11,  10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,                    STR_DRAWING_ENGINE_TIP                   ), // Drawing engine (chevron)
        makeWidget        ({ 10, 150}, {145,  12}, WidgetType::label,        WindowColour::secondary, STR_FRAME_RATE_LIMIT_LABEL                                                      ), // Frame rate limit (label)
        makeWidget        ({155, 150}, {145,  12}, WidgetType::dropdownMenu, WindowColour::secondary                                                                                  ), // Frame rate limit (dropdown label)
        makeWidget        ({288, 151}, { 11,  10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH                                                              ), // Frame rate limit (chevron)
        makeWidget        ({ 10, 166}, {136,  12}, WidgetType::checkbox,     WindowColour::secondary, STR_SHOW_FPS,                          STR_SHOW_FPS_TIP                         ), // Show fps
        makeWidget        ({155, 166}, {136,  12}, WidgetType::checkbox,     WindowColour::secondary, STR_MULTITHREADING,                    STR_MULTITHREADING_TIP                   ), // Multithreading

        makeWidget        ({  5, 188}, {300,  49}, WidgetType::groupbox,     WindowColour::secondary, STR_GROUP_BEHAVIOUR                                                             ), // Behaviour group
        makeWidget        ({ 11, 203}, {280,  12}, WidgetType::checkbox,     WindowColour::secondary, STR_MINIMISE_FULLSCREEN_ON_FOCUS_LOSS, STR_MINIMISE_FULLSCREEN_ON_FOCUS_LOSS_TIP), // Minimise fullscreen focus loss
        makeWidget        ({ 11, 218}, {280,  12}, WidgetType::checkbox,     WindowColour::secondary, STR_DISABLE_SCREENSAVER,               STR_DISABLE_SCREENSAVER_TIP              )  // Disable screensaver
    );

    constexpr int32_t kFrameRenderingStart = 53;
    constexpr int32_t kFrameEffectStart = 163;

    static constexpr auto window_options_rendering_widgets = makeWidgets(
        kMainOptionsWidgets,
        makeWidget({  5,  kFrameRenderingStart + 0}, {300, 108}, WidgetType::groupbox,     WindowColour::secondary, STR_RENDERING_GROUP                                       ), // Rendering group
        makeWidget({ 10, kFrameRenderingStart + 15}, {281,  12}, WidgetType::checkbox,     WindowColour::secondary, STR_TILE_SMOOTHING,         STR_TILE_SMOOTHING_TIP        ), // Landscape smoothing
        makeWidget({ 10, kFrameRenderingStart + 30}, {281,  12}, WidgetType::checkbox,     WindowColour::secondary, STR_GRIDLINES,              STR_GRIDLINES_TIP             ), // Gridlines
        makeWidget({ 10, kFrameRenderingStart + 45}, {281,  12}, WidgetType::checkbox,     WindowColour::secondary, STR_UPPERCASE_BANNERS,      STR_UPPERCASE_BANNERS_TIP     ), // Uppercase banners
        makeWidget({ 10, kFrameRenderingStart + 60}, {281,  12}, WidgetType::checkbox,     WindowColour::secondary, STR_SHOW_GUEST_PURCHASES,   STR_SHOW_GUEST_PURCHASES_TIP  ), // Guest purchases
        makeWidget({ 10, kFrameRenderingStart + 75}, {281,  12}, WidgetType::checkbox,     WindowColour::secondary, STR_TRANSPARENT_SCREENSHOT, STR_TRANSPARENT_SCREENSHOT_TIP), // Transparent screenshot
        makeWidget({ 10, kFrameRenderingStart + 90}, {281,  12}, WidgetType::label,        WindowColour::secondary, STR_VIRTUAL_FLOOR_STYLE,    STR_VIRTUAL_FLOOR_STYLE_TIP   ), // Virtual floor
        makeWidget({155, kFrameRenderingStart + 90}, {145,  12}, WidgetType::dropdownMenu, WindowColour::secondary, kStringIdNone,              STR_VIRTUAL_FLOOR_STYLE_TIP   ), // Virtual floor dropdown
        makeWidget({288, kFrameRenderingStart + 91}, { 11,  10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,         STR_VIRTUAL_FLOOR_STYLE_TIP   ), // Virtual floor dropdown

        makeWidget({ 5,  kFrameEffectStart + 0}, {300, 94}, WidgetType::groupbox, WindowColour::secondary, STR_EFFECTS_GROUP                                             ), // Rendering group
        makeWidget({10, kFrameEffectStart + 15}, {281, 12}, WidgetType::checkbox, WindowColour::secondary, STR_CYCLE_DAY_NIGHT,          STR_CYCLE_DAY_NIGHT_TIP         ), // Cycle day-night
        makeWidget({25, kFrameEffectStart + 30}, {266, 12}, WidgetType::checkbox, WindowColour::secondary, STR_ENABLE_LIGHTING_EFFECTS,  STR_ENABLE_LIGHTING_EFFECTS_TIP ), // Enable light fx
        makeWidget({40, kFrameEffectStart + 45}, {251, 12}, WidgetType::checkbox, WindowColour::secondary, STR_ENABLE_LIGHTING_VEHICLES, STR_ENABLE_LIGHTING_VEHICLES_TIP), // Enable light fx for vehicles
        makeWidget({10, kFrameEffectStart + 60}, {281, 12}, WidgetType::checkbox, WindowColour::secondary, STR_RENDER_WEATHER_EFFECTS,   STR_RENDER_WEATHER_EFFECTS_TIP  ), // Render weather effects
        makeWidget({25, kFrameEffectStart + 75}, {266, 12}, WidgetType::checkbox, WindowColour::secondary, STR_DISABLE_LIGHTNING_EFFECT, STR_DISABLE_LIGHTNING_EFFECT_TIP)  // Disable lightning effect
    );

    static constexpr auto window_options_culture_widgets = makeWidgets(
        kMainOptionsWidgets,
        makeWidget({ 10,  53}, {145, 12}, WidgetType::label,        WindowColour::secondary, STR_OPTIONS_LANGUAGE,   STR_LANGUAGE_TIP           ), // language
        makeWidget({155,  53}, {145, 12}, WidgetType::dropdownMenu, WindowColour::secondary, STR_STRING                                         ),
        makeWidget({288,  54}, { 11, 10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,     STR_LANGUAGE_TIP           ),
        makeWidget({ 10,  68}, {145, 12}, WidgetType::label,        WindowColour::secondary, STR_CURRENCY,           STR_CURRENCY_TIP           ), // Currency
        makeWidget({155,  68}, {145, 12}, WidgetType::dropdownMenu, WindowColour::secondary                                                     ),
        makeWidget({288,  69}, { 11, 10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,     STR_CURRENCY_TIP           ),
        makeWidget({ 10,  83}, {145, 12}, WidgetType::label,        WindowColour::secondary, STR_DISTANCE_AND_SPEED, STR_DISTANCE_AND_SPEED_TIP ), // Distance and speed
        makeWidget({155,  83}, {145, 12}, WidgetType::dropdownMenu, WindowColour::secondary                                                     ),
        makeWidget({288,  84}, { 11, 10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,     STR_DISTANCE_AND_SPEED_TIP ),
        makeWidget({ 10,  98}, {145, 12}, WidgetType::label,        WindowColour::secondary, STR_TEMPERATURE,        STR_TEMPERATURE_FORMAT_TIP ), // Temperature
        makeWidget({155,  98}, {145, 12}, WidgetType::dropdownMenu, WindowColour::secondary                                                     ),
        makeWidget({288,  99}, { 11, 10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,     STR_TEMPERATURE_FORMAT_TIP ),
        makeWidget({ 10, 113}, {145, 12}, WidgetType::label,        WindowColour::secondary, STR_HEIGHT_LABELS,      STR_HEIGHT_LABELS_UNITS_TIP), // Height labels
        makeWidget({155, 113}, {145, 12}, WidgetType::dropdownMenu, WindowColour::secondary                                                     ),
        makeWidget({288, 114}, { 11, 10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,     STR_HEIGHT_LABELS_UNITS_TIP),
        makeWidget({ 10, 128}, {145, 12}, WidgetType::label,        WindowColour::secondary, STR_DATE_FORMAT,        STR_DATE_FORMAT_TIP        ), // Date format
        makeWidget({155, 128}, {145, 12}, WidgetType::dropdownMenu, WindowColour::secondary                                                     ),
        makeWidget({288, 129}, { 11, 10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,     STR_DATE_FORMAT_TIP        )
    );

    static constexpr auto window_options_audio_widgets = makeWidgets(
        kMainOptionsWidgets,
        makeWidget({ 10,  53}, {290, 12}, WidgetType::dropdownMenu, WindowColour::secondary                                                ), // Audio device
        makeWidget({288,  54}, { 11, 10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,      STR_AUDIO_DEVICE_TIP ),
        makeWidget({ 10,  69}, {220, 12}, WidgetType::checkbox,     WindowColour::secondary, STR_MASTER_VOLUME,       STR_MASTER_VOLUME_TIP), // Enable / disable master sound
        makeWidget({ 10,  84}, {220, 12}, WidgetType::checkbox,     WindowColour::secondary, STR_SOUND_EFFECTS,       STR_SOUND_EFFECTS_TIP), // Enable / disable sound effects
        makeWidget({ 10,  99}, {220, 12}, WidgetType::checkbox,     WindowColour::secondary, STR_RIDE_MUSIC,          STR_RIDE_MUSIC_TIP   ), // Enable / disable ride music
        makeWidget({ 10, 113}, {290, 13}, WidgetType::checkbox,     WindowColour::secondary, STR_AUDIO_FOCUS,         STR_AUDIO_FOCUS_TIP  ), // Enable / disable audio disabled on focus lost
        makeWidget({ 10, 128}, {145, 13}, WidgetType::label,        WindowColour::secondary, STR_OPTIONS_MUSIC_LABEL, STR_TITLE_MUSIC_TIP  ), // Title music label
        makeWidget({155, 127}, {145, 13}, WidgetType::dropdownMenu, WindowColour::secondary                                                ), // Title music
        makeWidget({288, 128}, { 11, 11}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,      STR_TITLE_MUSIC_TIP  ),
        makeWidget({155,  68}, {145, 13}, WidgetType::scroll,       WindowColour::secondary, SCROLL_HORIZONTAL                             ), // Master volume
        makeWidget({155,  83}, {145, 13}, WidgetType::scroll,       WindowColour::secondary, SCROLL_HORIZONTAL                             ), // Sound effect volume
        makeWidget({155,  98}, {145, 13}, WidgetType::scroll,       WindowColour::secondary, SCROLL_HORIZONTAL                             )  // Music volume
    );

    constexpr int32_t kControlsGroupStart = 53;
    constexpr int32_t kGamepadGroupStart = kControlsGroupStart + 150;

    static constexpr auto window_options_controls_widgets = makeWidgets(
        kMainOptionsWidgets,
        makeWidget({  5, kControlsGroupStart +  0},  {300,137}, WidgetType::groupbox, WindowColour::secondary, STR_CONTROLS_GROUP                                                ), // Controls group
        makeWidget({ 10, kControlsGroupStart + 13},  {290, 14}, WidgetType::checkbox, WindowColour::tertiary , STR_SCREEN_EDGE_SCROLLING,      STR_SCREEN_EDGE_SCROLLING_TIP     ), // Edge scrolling
        makeWidget({ 10, kControlsGroupStart + 30},  {290, 12}, WidgetType::checkbox, WindowColour::tertiary , STR_TRAP_MOUSE,                 STR_TRAP_MOUSE_TIP                ), // Trap mouse
        makeWidget({ 10, kControlsGroupStart + 45},  {290, 12}, WidgetType::checkbox, WindowColour::tertiary , STR_INVERT_RIGHT_MOUSE_DRAG,    STR_INVERT_RIGHT_MOUSE_DRAG_TIP   ), // Invert right mouse dragging
        makeWidget({ 10, kControlsGroupStart + 60},  {290, 12}, WidgetType::checkbox, WindowColour::tertiary , STR_ZOOM_TO_CURSOR,             STR_ZOOM_TO_CURSOR_TIP            ), // Zoom to cursor
        makeWidget({ 10, kControlsGroupStart + 75},  {290, 12}, WidgetType::checkbox, WindowColour::tertiary,  STR_WINDOW_BUTTONS_ON_THE_LEFT, STR_WINDOW_BUTTONS_ON_THE_LEFT_TIP), // Window buttons on the left
        makeWidget({ 10, kControlsGroupStart + 90},  {290, 12}, WidgetType::checkbox, WindowColour::tertiary,  STR_ENLARGED_UI,                STR_ENLARGED_UI_TIP               ),
        makeWidget({ 25, kControlsGroupStart + 105}, {275, 12}, WidgetType::checkbox, WindowColour::tertiary,  STR_TOUCH_ENHANCEMENTS,         STR_TOUCH_ENHANCEMENTS_TIP        ),
        makeWidget({155, kControlsGroupStart + 120}, {145, 13}, WidgetType::button,   WindowColour::secondary, STR_HOTKEY,                     STR_HOTKEY_TIP                    ), // Set hotkeys buttons

        // Gamepad group
        makeWidget({  5, kGamepadGroupStart +  0},   {300, 45}, WidgetType::groupbox, WindowColour::secondary, STR_GAMEPAD_GROUP                                                 ), // Gamepad group
        makeWidget({ 10, kGamepadGroupStart + 13},   { 90, 12}, WidgetType::label,    WindowColour::secondary, STR_GAMEPAD_DEADZONE_LABEL,     STR_GAMEPAD_DEADZONE_TIP          ), // Deadzone label
        makeWidget({105, kGamepadGroupStart + 13},   {190, 13}, WidgetType::scroll,   WindowColour::secondary, SCROLL_HORIZONTAL,              STR_GAMEPAD_DEADZONE_TOOLTIP_FORMAT), // Deadzone slider
        makeWidget({ 10, kGamepadGroupStart + 28},   { 90, 12}, WidgetType::label,    WindowColour::secondary, STR_GAMEPAD_SENSITIVITY_LABEL,  STR_GAMEPAD_SENSITIVITY_TIP       ), // Sensitivity label
        makeWidget({105, kGamepadGroupStart + 28},   {190, 13}, WidgetType::scroll,   WindowColour::secondary, SCROLL_HORIZONTAL,              STR_GAMEPAD_SENSITIVITY_TOOLTIP_FORMAT)  // Sensitivity slider
    );

    constexpr int32_t kThemesGroupStart = 53;
    constexpr int32_t kToolbarGroupStart = kThemesGroupStart + 52;

    static constexpr auto window_options_interface_widgets = makeWidgets(
        kMainOptionsWidgets,
        makeWidget({  5, kThemesGroupStart +  0}, {300, 48}, WidgetType::groupbox,     WindowColour::secondary, STR_THEMES_GROUP                                          ), // Themes group
        makeWidget({ 10, kThemesGroupStart + 14}, {145, 12}, WidgetType::label,        WindowColour::secondary, STR_THEMES_LABEL_CURRENT_THEME, STR_CURRENT_THEME_TIP     ), // Themes
        makeWidget({155, kThemesGroupStart + 14}, {145, 12}, WidgetType::dropdownMenu, WindowColour::secondary, STR_STRING                                                ),
        makeWidget({288, kThemesGroupStart + 15}, { 11, 10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,             STR_CURRENT_THEME_TIP     ),
        makeWidget({155, kThemesGroupStart + 30}, {145, 13}, WidgetType::button,       WindowColour::secondary, STR_EDIT_THEMES_BUTTON,         STR_EDIT_THEMES_BUTTON_TIP), // Themes button

        makeWidget({  5, kToolbarGroupStart +  0}, {300,107}, WidgetType::groupbox, WindowColour::secondary, STR_TOOLBAR_BUTTONS_GROUP                                                         ), // Toolbar buttons group
        makeWidget({ 10, kToolbarGroupStart + 14}, {280, 12}, WidgetType::checkbox, WindowColour::tertiary,  STR_OPTIONS_TOOLBAR_BUTTONS_CENTRED, STR_OPTIONS_TOOLBAR_BUTTONS_CENTRED_TIP      ),
        makeWidget({ 10, kToolbarGroupStart + 31}, {280, 12}, WidgetType::label,    WindowColour::secondary, STR_SHOW_TOOLBAR_BUTTONS_FOR                                                      ),
        makeWidget({ 24, kToolbarGroupStart + 46}, {122, 12}, WidgetType::checkbox, WindowColour::tertiary , STR_FINANCES_BUTTON_ON_TOOLBAR,      STR_FINANCES_BUTTON_ON_TOOLBAR_TIP           ), // Finances
        makeWidget({ 24, kToolbarGroupStart + 61}, {122, 12}, WidgetType::checkbox, WindowColour::tertiary , STR_RESEARCH_BUTTON_ON_TOOLBAR,      STR_RESEARCH_BUTTON_ON_TOOLBAR_TIP           ), // Research
        makeWidget({155, kToolbarGroupStart + 46}, {145, 12}, WidgetType::checkbox, WindowColour::tertiary , STR_CHEATS_BUTTON_ON_TOOLBAR,        STR_CHEATS_BUTTON_ON_TOOLBAR_TIP             ), // Cheats
        makeWidget({155, kToolbarGroupStart + 61}, {145, 12}, WidgetType::checkbox, WindowColour::tertiary , STR_SHOW_RECENT_MESSAGES_ON_TOOLBAR, STR_SHOW_RECENT_MESSAGES_ON_TOOLBAR_TIP      ), // Recent messages
        makeWidget({ 24, kToolbarGroupStart + 76}, {162, 12}, WidgetType::checkbox, WindowColour::tertiary , STR_MUTE_BUTTON_ON_TOOLBAR,          STR_MUTE_BUTTON_ON_TOOLBAR_TIP               ), // Mute
        makeWidget({155, kToolbarGroupStart + 76}, {145, 12}, WidgetType::checkbox, WindowColour::tertiary , STR_CHAT_BUTTON_ON_TOOLBAR,          STR_CHAT_BUTTON_ON_TOOLBAR_TIP               ), // Chat
        makeWidget({ 24, kToolbarGroupStart + 91}, {122, 12}, WidgetType::checkbox, WindowColour::tertiary , STR_ZOOM_BUTTON_ON_TOOLBAR,          STR_ZOOM_BUTTON_ON_TOOLBAR_TIP               ), // Zoom
        makeWidget({155, kToolbarGroupStart + 91}, {145, 12}, WidgetType::checkbox, WindowColour::tertiary , STR_ROTATE_ANTI_CLOCKWISE,           STR_ROTATE_VIEW_ANTI_CLOCKWISE_IN_TOOLBAR_TIP)  // Rotate anti-clockwise
    );

    constexpr int32_t kTitleSequenceStart = 53;
    constexpr int32_t kScenarioOptionsGroupStart = kTitleSequenceStart + 35;
    constexpr int32_t kTweaksStart = kScenarioOptionsGroupStart + 70;

    static constexpr auto window_options_misc_widgets = makeWidgets(
        kMainOptionsWidgets,
        makeWidget(         {  5, kTitleSequenceStart +  0}, {300, 31}, WidgetType::groupbox,     WindowColour::secondary, STR_OPTIONS_TITLE_SEQUENCE                              ),
        makeDropdownWidgets({ 10, kTitleSequenceStart + 15}, {290, 12}, WidgetType::dropdownMenu, WindowColour::secondary, STR_STRINGID,                 STR_TITLE_SEQUENCE_TIP    ), // Title sequence dropdown

        makeWidget({ 5,  kScenarioOptionsGroupStart +  0}, {300, 66}, WidgetType::groupbox,     WindowColour::secondary, STR_SCENARIO_OPTIONS                                      ),
        makeWidget({ 10, kScenarioOptionsGroupStart + 16}, {165, 12}, WidgetType::label,        WindowColour::secondary, STR_SCENARIO_PREVIEWS_LABEL,    STR_SCENARIO_PREVIEWS_TIP ),
        makeWidget({175, kScenarioOptionsGroupStart + 15}, {125, 12}, WidgetType::dropdownMenu, WindowColour::secondary                                                            ), // Scenario previews
        makeWidget({288, kScenarioOptionsGroupStart + 16}, { 11, 10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,             STR_SCENARIO_PREVIEWS_TIP ),
        makeWidget({ 10, kScenarioOptionsGroupStart + 30}, {275, 16}, WidgetType::checkbox,     WindowColour::tertiary,  STR_OPTIONS_SCENARIO_UNLOCKING, STR_SCENARIO_UNLOCKING_TIP), // Unlocking of scenarios
        makeWidget({ 10, kScenarioOptionsGroupStart + 45}, {290, 15}, WidgetType::checkbox,     WindowColour::tertiary,  STR_ALLOW_EARLY_COMPLETION,     STR_EARLY_COMPLETION_TIP  ), // Allow early scenario completion

        makeWidget({  5,  kTweaksStart + 0}, {300, 96}, WidgetType::groupbox,     WindowColour::secondary, STR_OPTIONS_TWEAKS                                                  ),
        makeWidget({ 10, kTweaksStart + 15}, {290, 15}, WidgetType::checkbox,     WindowColour::tertiary , STR_REAL_NAME_GUESTS,     STR_REAL_NAME_GUESTS_TIP                  ), // Show 'real' names of guests
        makeWidget({ 10, kTweaksStart + 30}, {290, 15}, WidgetType::checkbox,     WindowColour::tertiary , STR_REAL_NAME_STAFF,      STR_REAL_NAME_STAFF_TIP                   ), // Show 'real' names of staff
        makeWidget({ 10, kTweaksStart + 45}, {290, 15}, WidgetType::checkbox,     WindowColour::tertiary , STR_AUTO_STAFF_PLACEMENT, STR_AUTO_STAFF_PLACEMENT_TIP              ), // Auto staff placement
        makeWidget({ 10, kTweaksStart + 60}, {290, 15}, WidgetType::checkbox,     WindowColour::tertiary , STR_AUTO_OPEN_SHOPS,      STR_AUTO_OPEN_SHOPS_TIP                   ), // Automatically open shops & stalls
        makeWidget({ 10, kTweaksStart + 77}, {165, 12}, WidgetType::label,        WindowColour::secondary, STR_DEFAULT_INSPECTION_INTERVAL, STR_DEFAULT_INSPECTION_INTERVAL_TIP),
        makeWidget({175, kTweaksStart + 76}, {125, 12}, WidgetType::dropdownMenu, WindowColour::secondary                                                                      ), // Default inspection time dropdown
        makeWidget({288, kTweaksStart + 77}, { 11, 10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,       STR_DEFAULT_INSPECTION_INTERVAL_TIP       )  // Default inspection time dropdown button
    );

    constexpr int32_t kRCT1Start = 53;
    constexpr int32_t kSavingStart = kRCT1Start + 54;
    constexpr int32_t kAdvancedStart = kSavingStart + 84;

    static constexpr auto window_options_advanced_widgets = makeWidgets(
        kMainOptionsWidgets,
        makeWidget        ({  5, kRCT1Start +  0}, {300, 50}, WidgetType::groupbox,     WindowColour::secondary, STR_GROUP_RCT1                                                                          ),
        makeWidget        ({ 10, kRCT1Start + 16}, {276, 12}, WidgetType::label,        WindowColour::secondary, STR_PATH_TO_RCT1,                          STR_PATH_TO_RCT1_TIP                         ), // RCT 1 path label
        makeWidget        ({ 10, kRCT1Start + 30}, {290, 14}, WidgetType::label,        WindowColour::secondary, kStringIdNone,                             STR_STRING_TOOLTIP                           ), // RCT 1 path path
        makeWidget        ({239, kRCT1Start + 15}, { 60, 14}, WidgetType::button,       WindowColour::secondary, STR_BROWSE                                                                              ), // RCT 1 path browse
        makeWidget        ({249, kRCT1Start + 15}, { 50, 14}, WidgetType::button,       WindowColour::secondary, STR_CLEAR_BUTTON,                          STR_PATH_TO_RCT1_CLEAR_TIP                   ), // RCT 1 path clear

        makeWidget        ({  5, kSavingStart +  0}, {300, 80}, WidgetType::groupbox,     WindowColour::secondary, STR_GROUP_SAVING                                                                        ),
        makeWidget        ({ 10, kSavingStart + 16}, {290, 12}, WidgetType::checkbox,     WindowColour::tertiary,  STR_SAVE_PLUGIN_DATA,                      STR_SAVE_PLUGIN_DATA_TIP                     ), // Export plug-in objects with saved games
        makeWidget        ({ 10, kSavingStart + 30}, {290, 12}, WidgetType::checkbox,     WindowColour::secondary, STR_ALWAYS_NATIVE_LOADSAVE,                STR_ALWAYS_NATIVE_LOADSAVE_TIP               ), // Use native load/save window
        makeWidget        ({ 23, kSavingStart + 46}, {135, 12}, WidgetType::label,        WindowColour::secondary, STR_OPTIONS_AUTOSAVE_FREQUENCY_LABEL,      STR_AUTOSAVE_FREQUENCY_TIP                   ),
        makeWidget        ({165, kSavingStart + 45}, {135, 13}, WidgetType::dropdownMenu, WindowColour::secondary                                                                                          ), // Autosave dropdown
        makeWidget        ({288, kSavingStart + 46}, { 11, 11}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,                        STR_AUTOSAVE_FREQUENCY_TIP                   ), // Autosave dropdown button
        makeWidget        ({ 23, kSavingStart + 60}, {135, 12}, WidgetType::label,        WindowColour::secondary, STR_AUTOSAVE_AMOUNT,                       STR_AUTOSAVE_AMOUNT_TIP                      ),
        makeSpinnerWidgets({165, kSavingStart + 60}, {135, 12}, WidgetType::spinner,      WindowColour::secondary, kStringIdNone,                             STR_AUTOSAVE_AMOUNT_TIP                      ), // Autosave amount spinner

        makeWidget        ({  5, kAdvancedStart +  0}, {300, 97}, WidgetType::groupbox,     WindowColour::secondary, STR_GROUP_ADVANCED                                                                      ),
        makeWidget        ({ 10, kAdvancedStart + 16}, {295, 12}, WidgetType::checkbox,     WindowColour::tertiary,  STR_ENABLE_DEBUGGING_TOOLS,                STR_ENABLE_DEBUGGING_TOOLS_TIP               ), // Enable debugging tools
        makeWidget        ({ 10, kAdvancedStart + 30}, {295, 12}, WidgetType::checkbox,     WindowColour::tertiary,  STR_STAY_CONNECTED_AFTER_DESYNC,           STR_STAY_CONNECTED_AFTER_DESYNC_TIP          ), // Do not disconnect after the client desynchronises with the server
#ifdef __EMSCRIPTEN__
        makeWidget        ({ 10, kAdvancedStart + 46}, {135, 14}, WidgetType::button,       WindowColour::secondary, STR_EXPORT_EMSCRIPTEN,                     kStringIdNone                                ), // Emscripten data export
        makeWidget        ({150, kAdvancedStart + 46}, {150, 14}, WidgetType::button,       WindowColour::secondary, STR_IMPORT_EMSCRIPTEN,                     kStringIdNone                                ), // Emscripten data import
#endif
        makeWidget        ({150, kAdvancedStart + 64}, {150, 14}, WidgetType::button,       WindowColour::secondary, STR_EDIT_ASSET_PACKS_BUTTON,               kStringIdNone                                )  // Asset packs
    );

    static constexpr std::span<const Widget> window_options_page_widgets[] = {
        window_options_display_widgets,
        window_options_rendering_widgets,
        window_options_culture_widgets,
        window_options_audio_widgets,
        window_options_interface_widgets,
        window_options_controls_widgets,
        window_options_misc_widgets,
        window_options_advanced_widgets,
    };
    // clang-format on

    static constexpr StringId kDrawingEngineStringIds[] = {
        STR_DRAWING_ENGINE_SOFTWARE,
#ifndef DISABLE_OPENGL
        STR_DRAWING_ENGINE_OPENGL,
#endif
    };

#pragma endregion

    class OptionsWindow final : public Window
    {
    public:
        void onOpen() override
        {
            setPage(WINDOW_OPTIONS_PAGE_DISPLAY);
        }

        void onMouseUp(WidgetIndex widgetIndex) override
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
                    case WINDOW_OPTIONS_PAGE_CONTROLS:
                        ControlsMouseUp(widgetIndex);
                        break;
                    case WINDOW_OPTIONS_PAGE_INTERFACE:
                        InterfaceMouseUp(widgetIndex);
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

        void onMouseDown(WidgetIndex widgetIndex) override
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
                case WINDOW_OPTIONS_PAGE_INTERFACE:
                    InterfaceMouseDown(widgetIndex);
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

        void onDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
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
                case WINDOW_OPTIONS_PAGE_INTERFACE:
                    InterfaceDropdown(widgetIndex, dropdownIndex);
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

        void onPrepareDraw() override
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
                case WINDOW_OPTIONS_PAGE_CONTROLS:
                    ControlsPrepareDraw();
                    break;
                case WINDOW_OPTIONS_PAGE_INTERFACE:
                    InterfacePrepareDraw();
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

        void onDraw(RenderTarget& rt) override
        {
            drawWidgets(rt);
            DrawTabImages(rt);

            switch (page)
            {
                case WINDOW_OPTIONS_PAGE_DISPLAY:
                    DisplayDraw(rt);
                    break;
                case WINDOW_OPTIONS_PAGE_ADVANCED:
                    AdvancedDraw(rt);
                    break;
                default:
                    break;
            }
        }

        void onUpdate() override
        {
            CommonUpdate();

            switch (page)
            {
                case WINDOW_OPTIONS_PAGE_AUDIO:
                    AudioUpdate();
                    break;
                case WINDOW_OPTIONS_PAGE_CONTROLS:
                    ControlsUpdate();
                    break;
                case WINDOW_OPTIONS_PAGE_DISPLAY:
                case WINDOW_OPTIONS_PAGE_RENDERING:
                case WINDOW_OPTIONS_PAGE_CULTURE:
                case WINDOW_OPTIONS_PAGE_MISC:
                case WINDOW_OPTIONS_PAGE_ADVANCED:
                default:
                    break;
            }
        }

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            switch (page)
            {
                case WINDOW_OPTIONS_PAGE_AUDIO:
                    return AudioScrollGetSize(scrollIndex);
                case WINDOW_OPTIONS_PAGE_CONTROLS:
                    return ControlsScrollGetSize(scrollIndex);
                case WINDOW_OPTIONS_PAGE_DISPLAY:
                case WINDOW_OPTIONS_PAGE_RENDERING:
                case WINDOW_OPTIONS_PAGE_CULTURE:
                case WINDOW_OPTIONS_PAGE_MISC:
                case WINDOW_OPTIONS_PAGE_ADVANCED:
                default:
                    return kWindowSize;
            }
        }

        OpenRCT2String onTooltip(WidgetIndex widgetIndex, StringId fallback) override
        {
            if (page == WINDOW_OPTIONS_PAGE_ADVANCED)
                return AdvancedTooltip(widgetIndex, fallback);

            if (page == WINDOW_OPTIONS_PAGE_CONTROLS)
            {
                if (widgetIndex == WIDX_GAMEPAD_DEADZONE)
                {
                    const int32_t deadzone = Config::Get().general.gamepadDeadzone;
                    const int32_t deadzonePercent = static_cast<int32_t>((deadzone / 32767.0f) * 100);

                    auto ft = Formatter();
                    ft.Add<int32_t>(deadzonePercent);
                    return { STR_GAMEPAD_DEADZONE_TOOLTIP_FORMAT, ft };
                }
                else if (widgetIndex == WIDX_GAMEPAD_SENSITIVITY)
                {
                    const float sensitivity = Config::Get().general.gamepadSensitivity;
                    const int32_t sensitivityDisplay = static_cast<int32_t>(sensitivity * 100);

                    auto ft = Formatter();
                    ft.Add<int32_t>(sensitivityDisplay);
                    return { STR_GAMEPAD_SENSITIVITY_TOOLTIP_FORMAT, ft };
                }
            }

            return WindowBase::onTooltip(widgetIndex, fallback);
        }

    private:
#pragma region Common events
        void CommonMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_TAB_DISPLAY:
                case WIDX_TAB_RENDERING:
                case WIDX_TAB_CULTURE:
                case WIDX_TAB_AUDIO:
                case WIDX_TAB_CONTROLS:
                case WIDX_TAB_INTERFACE:
                case WIDX_TAB_MISC:
                case WIDX_TAB_ADVANCED:
                    setPage(widgetIndex - WIDX_FIRST_TAB);
                    break;
            }
        }

        void CommonPrepareDrawBefore()
        {
            SetPressedTab();

            disabledWidgets = 0;
            auto hasFilePicker = GetContext()->GetUiContext().HasFilePicker();
            const bool advancedTabSelected = (WIDX_FIRST_TAB + page) == WIDX_TAB_ADVANCED;
            if (!hasFilePicker && advancedTabSelected)
            {
                disabledWidgets |= (1uLL << WIDX_ALWAYS_NATIVE_LOADSAVE);
                widgets[WIDX_ALWAYS_NATIVE_LOADSAVE].type = WidgetType::empty;
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
            y += 6;

            if (height != y)
            {
                invalidate();
                height = y;
                resizeFrame();
                invalidate();
            }
        }

        void CommonUpdate()
        {
            // Tab animation
            currentFrame++;
            invalidateWidget(WIDX_FIRST_TAB + page);
        }
#pragma endregion

#pragma region Display tab events
        void DisplayMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_SHOW_FPS_CHECKBOX:
                    Config::Get().general.showFPS ^= 1;
                    Config::Save();
                    invalidate();
                    break;
                case WIDX_MULTITHREADING_CHECKBOX:
                    Config::Get().general.multiThreading ^= 1;
                    Config::Save();
                    invalidate();
                    break;
                case WIDX_MINIMIZE_FOCUS_LOSS:
                    Config::Get().general.minimizeFullscreenFocusLoss ^= 1;
                    Config::Save();
                    invalidate();
                    break;
                case WIDX_DISABLE_SCREENSAVER_LOCK:
                    Config::Get().general.disableScreensaver ^= 1;
                    ApplyScreenSaverLockSetting();
                    Config::Save();
                    invalidate();
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
                    const auto& resolutions = GetContext()->GetUiContext().GetFullscreenResolutions();

                    int32_t selectedResolution = -1;
                    for (size_t i = 0; i < resolutions.size(); i++)
                    {
                        const Resolution& resolution = resolutions[i];

                        Formatter ft;
                        ft.Add<uint16_t>(resolution.Width);
                        ft.Add<uint16_t>(resolution.Height);
                        gDropdown.items[i] = Dropdown::MenuLabel(STR_RESOLUTION_X_BY_Y, ft);

                        if (resolution.Width == Config::Get().general.fullscreenWidth
                            && resolution.Height == Config::Get().general.fullscreenHeight)
                        {
                            selectedResolution = static_cast<int32_t>(i);
                        }
                    }

                    ShowDropdown(widget, static_cast<int32_t>(resolutions.size()));

                    if (selectedResolution != -1 && selectedResolution < 32)
                    {
                        gDropdown.items[selectedResolution].setChecked(true);
                    }
                }

                break;
                case WIDX_FULLSCREEN_DROPDOWN:
                    gDropdown.items[0] = Dropdown::MenuLabel(STR_OPTIONS_DISPLAY_WINDOWED);
                    gDropdown.items[1] = Dropdown::MenuLabel(STR_OPTIONS_DISPLAY_FULLSCREEN);
                    gDropdown.items[2] = Dropdown::MenuLabel(STR_OPTIONS_DISPLAY_FULLSCREEN_BORDERLESS);

                    ShowDropdown(widget, 3);

                    gDropdown.items[Config::Get().general.fullscreenMode].setChecked(true);
                    break;
                case WIDX_DRAWING_ENGINE_DROPDOWN:
                {
                    const auto numItems = static_cast<int32_t>(std::size(kDrawingEngineStringIds));
                    for (int32_t i = 0; i < numItems; i++)
                    {
                        gDropdown.items[i] = Dropdown::MenuLabel(kDrawingEngineStringIds[i]);
                    }
                    ShowDropdown(widget, numItems);
                    gDropdown.items[EnumValue(Config::Get().general.drawingEngine)].setChecked(true);
                    break;
                }
                case WIDX_SCALE_UP:
                    Config::Get().general.windowScale += 0.25f;
                    Config::Save();
                    GfxInvalidateScreen();
                    ContextTriggerResize();
                    ContextUpdateCursorScale();
                    break;
                case WIDX_SCALE_DOWN:
                    Config::Get().general.windowScale -= 0.25f;
                    Config::Get().general.windowScale = std::max(0.5f, Config::Get().general.windowScale);
                    Config::Save();
                    GfxInvalidateScreen();
                    ContextTriggerResize();
                    ContextUpdateCursorScale();
                    break;
                case WIDX_FRAME_RATE_LIMIT_DROPDOWN:
                {
                    gDropdown.items[0] = Dropdown::MenuLabel(STR_FRAME_RATE_LIMIT_DEFAULT);
                    gDropdown.items[1] = Dropdown::MenuLabel(STR_FRAME_RATE_LIMIT_VSYNC);
                    gDropdown.items[2] = Dropdown::MenuLabel(STR_FRAME_RATE_LIMIT_UNRESTRICTED);

                    ShowDropdown(widget, 3);

                    auto& config = Config::Get().general;
                    auto activeItem = 0;
                    if (config.uncapFPS)
                    {
                        activeItem = config.useVSync ? 1 : 2;
                    }

                    gDropdown.items[activeItem].setChecked(true);
                    break;
                }
            }
        }

        void DisplayDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_RESOLUTION_DROPDOWN:
                {
                    const auto& resolutions = GetContext()->GetUiContext().GetFullscreenResolutions();

                    const Resolution& resolution = resolutions[dropdownIndex];
                    if (resolution.Width != Config::Get().general.fullscreenWidth
                        || resolution.Height != Config::Get().general.fullscreenHeight)
                    {
                        Config::Get().general.fullscreenWidth = resolution.Width;
                        Config::Get().general.fullscreenHeight = resolution.Height;

                        if (Config::Get().general.fullscreenMode == static_cast<int32_t>(FullscreenMode::fullscreen))
                            ContextSetFullscreenMode(static_cast<int32_t>(FullscreenMode::fullscreen));

                        Config::Save();
                        GfxInvalidateScreen();
                    }
                    break;
                }
                case WIDX_FULLSCREEN_DROPDOWN:
                    if (dropdownIndex != Config::Get().general.fullscreenMode)
                    {
                        ContextSetFullscreenMode(dropdownIndex);

                        Config::Get().general.fullscreenMode = static_cast<uint8_t>(dropdownIndex);
                        Config::Save();
                        GfxInvalidateScreen();
                    }
                    break;
                case WIDX_DRAWING_ENGINE_DROPDOWN:
                    if (dropdownIndex != EnumValue(Config::Get().general.drawingEngine))
                    {
                        DrawingEngine dstEngine = static_cast<DrawingEngine>(dropdownIndex);

                        Config::Get().general.drawingEngine = dstEngine;
                        RefreshVideo();
                        Config::Save();
                        invalidate();
                    }
                    break;
                case WIDX_FRAME_RATE_LIMIT_DROPDOWN:
                {
                    auto& config = Config::Get().general;
                    switch (dropdownIndex)
                    {
                        case 0: // vanilla
                            config.uncapFPS = false;
                            config.useVSync = false;
                            break;
                        case 1: // vsync
                            config.uncapFPS = true;
                            config.useVSync = true;
                            break;
                        case 2: // uncapped
                            config.uncapFPS = true;
                            config.useVSync = false;
                            break;
                    }

                    DrawingEngineSetVSync(config.useVSync);
                    Config::Save();
                    invalidate();
                    break;
                }
            }
        }

        void DisplayPrepareDraw()
        {
            // Resolution dropdown caption.
            auto ft = Formatter::Common();
            ft.Increment(16);
            ft.Add<uint16_t>(static_cast<uint16_t>(Config::Get().general.fullscreenWidth));
            ft.Add<uint16_t>(static_cast<uint16_t>(Config::Get().general.fullscreenHeight));

            // Disable resolution dropdown on "Windowed" and "Fullscreen (desktop)"
            if (Config::Get().general.fullscreenMode != static_cast<int32_t>(FullscreenMode::fullscreen))
            {
                disabledWidgets |= (1uLL << WIDX_RESOLUTION_DROPDOWN);
                disabledWidgets |= (1uLL << WIDX_RESOLUTION);
                disabledWidgets |= (1uLL << WIDX_RESOLUTION_LABEL);
            }
            else
            {
                disabledWidgets &= ~(1uLL << WIDX_RESOLUTION_DROPDOWN);
                disabledWidgets &= ~(1uLL << WIDX_RESOLUTION);
                disabledWidgets &= ~(1uLL << WIDX_RESOLUTION_LABEL);
            }

            setCheckboxValue(WIDX_SHOW_FPS_CHECKBOX, Config::Get().general.showFPS);
            setCheckboxValue(WIDX_MULTITHREADING_CHECKBOX, Config::Get().general.multiThreading);
            setCheckboxValue(WIDX_MINIMIZE_FOCUS_LOSS, Config::Get().general.minimizeFullscreenFocusLoss);
            setCheckboxValue(WIDX_DISABLE_SCREENSAVER_LOCK, Config::Get().general.disableScreensaver);

            // Dropdown captions for straightforward strings.
            widgets[WIDX_FULLSCREEN].text = FullscreenModeNames[Config::Get().general.fullscreenMode];
            widgets[WIDX_DRAWING_ENGINE].text = kDrawingEngineStringIds[EnumValue(Config::Get().general.drawingEngine)];

            static constexpr StringId kFrameRateLimitStringIds[] = {
                STR_FRAME_RATE_LIMIT_DEFAULT,
                STR_FRAME_RATE_LIMIT_VSYNC,
                STR_FRAME_RATE_LIMIT_UNRESTRICTED,
            };

            auto& config = Config::Get().general;
            auto activeItem = 0;
            if (config.uncapFPS)
            {
                activeItem = config.useVSync ? 1 : 2;
            }
            widgets[WIDX_FRAME_RATE_LIMIT].text = kFrameRateLimitStringIds[activeItem];
        }

        void DisplayDraw(RenderTarget& rt)
        {
            auto ft = Formatter();
            ft.Add<int32_t>(static_cast<int32_t>(Config::Get().general.windowScale * 100));
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ widgets[WIDX_SCALE].left + 1, widgets[WIDX_SCALE].top + 1 },
                STR_WINDOW_COLOUR_2_COMMA2DP32, ft, { colours[1] });
        }
#pragma endregion

#pragma region Rendering tab events
        void RenderingMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_TILE_SMOOTHING_CHECKBOX:
                    Config::Get().general.landscapeSmoothing ^= 1;
                    Config::Save();
                    GfxInvalidateScreen();
                    break;
                case WIDX_GRIDLINES_CHECKBOX:
                {
                    Config::Get().general.alwaysShowGridlines ^= 1;
                    Config::Save();
                    GfxInvalidateScreen();
                    WindowBase* mainWindow = WindowGetMain();
                    if (mainWindow != nullptr)
                    {
                        if (Config::Get().general.alwaysShowGridlines)
                            mainWindow->viewport->flags |= VIEWPORT_FLAG_GRIDLINES;
                        else
                            mainWindow->viewport->flags &= ~VIEWPORT_FLAG_GRIDLINES;
                    }
                    break;
                }
                case WIDX_DAY_NIGHT_CHECKBOX:
                    Config::Get().general.dayNightCycle ^= 1;
                    Config::Save();
                    invalidate();
                    break;
                case WIDX_ENABLE_LIGHT_FX_CHECKBOX:
                    Config::Get().general.enableLightFx ^= 1;
                    Config::Save();
                    invalidate();
                    break;
                case WIDX_ENABLE_LIGHT_FX_FOR_VEHICLES_CHECKBOX:
                    Config::Get().general.enableLightFxForVehicles ^= 1;
                    Config::Save();
                    invalidate();
                    break;
                case WIDX_UPPER_CASE_BANNERS_CHECKBOX:
                    Config::Get().general.upperCaseBanners ^= 1;
                    Config::Save();
                    invalidate();
                    ScrollingText::invalidate();
                    break;
                case WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX:
                    Config::Get().general.disableLightningEffect ^= 1;
                    Config::Save();
                    invalidate();
                    break;
                case WIDX_RENDER_WEATHER_EFFECTS_CHECKBOX:
                    Config::Get().general.renderWeatherEffects ^= 1;
                    Config::Get().general.renderWeatherGloom = Config::Get().general.renderWeatherEffects;
                    Config::Save();
                    invalidate();
                    GfxInvalidateScreen();
                    break;
                case WIDX_SHOW_GUEST_PURCHASES_CHECKBOX:
                    Config::Get().general.showGuestPurchases ^= 1;
                    Config::Save();
                    invalidate();
                    break;
                case WIDX_TRANSPARENT_SCREENSHOTS_CHECKBOX:
                    Config::Get().general.transparentScreenshot ^= 1;
                    Config::Save();
                    invalidate();
                    break;
            }
        }

        void RenderingMouseDown(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_VIRTUAL_FLOOR_DROPDOWN:
                    gDropdown.items[0] = Dropdown::MenuLabel(STR_VIRTUAL_FLOOR_STYLE_DISABLED);
                    gDropdown.items[1] = Dropdown::MenuLabel(STR_VIRTUAL_FLOOR_STYLE_TRANSPARENT);
                    gDropdown.items[2] = Dropdown::MenuLabel(STR_VIRTUAL_FLOOR_STYLE_GLASSY);

                    Widget* widget = &widgets[widgetIndex - 1];
                    ShowDropdown(widget, 3);

                    gDropdown.items[static_cast<int32_t>(Config::Get().general.virtualFloorStyle)].setChecked(true);
                    break;
            }
        }

        void RenderingDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_VIRTUAL_FLOOR_DROPDOWN:
                    Config::Get().general.virtualFloorStyle = static_cast<VirtualFloorStyles>(dropdownIndex);
                    Config::Save();
                    break;
            }
        }

        void RenderingPrepareDraw()
        {
            setCheckboxValue(WIDX_TILE_SMOOTHING_CHECKBOX, Config::Get().general.landscapeSmoothing);
            setCheckboxValue(WIDX_GRIDLINES_CHECKBOX, Config::Get().general.alwaysShowGridlines);
            setCheckboxValue(WIDX_DAY_NIGHT_CHECKBOX, Config::Get().general.dayNightCycle);
            setCheckboxValue(WIDX_SHOW_GUEST_PURCHASES_CHECKBOX, Config::Get().general.showGuestPurchases);
            setCheckboxValue(WIDX_TRANSPARENT_SCREENSHOTS_CHECKBOX, Config::Get().general.transparentScreenshot);
            setCheckboxValue(WIDX_UPPER_CASE_BANNERS_CHECKBOX, Config::Get().general.upperCaseBanners);

            static constexpr StringId _virtualFloorStyleStrings[] = {
                STR_VIRTUAL_FLOOR_STYLE_DISABLED,
                STR_VIRTUAL_FLOOR_STYLE_TRANSPARENT,
                STR_VIRTUAL_FLOOR_STYLE_GLASSY,
            };

            widgets[WIDX_VIRTUAL_FLOOR].text = _virtualFloorStyleStrings[EnumValue(Config::Get().general.virtualFloorStyle)];

            setCheckboxValue(WIDX_ENABLE_LIGHT_FX_CHECKBOX, Config::Get().general.enableLightFx);
            if (Config::Get().general.dayNightCycle
                && Config::Get().general.drawingEngine == DrawingEngine::SoftwareWithHardwareDisplay)
            {
                disabledWidgets &= ~(1uLL << WIDX_ENABLE_LIGHT_FX_CHECKBOX);
            }
            else
            {
                disabledWidgets |= (1uLL << WIDX_ENABLE_LIGHT_FX_CHECKBOX);
                Config::Get().general.enableLightFx = false;
            }

            setCheckboxValue(WIDX_ENABLE_LIGHT_FX_FOR_VEHICLES_CHECKBOX, Config::Get().general.enableLightFxForVehicles);
            if (Config::Get().general.dayNightCycle
                && Config::Get().general.drawingEngine == DrawingEngine::SoftwareWithHardwareDisplay
                && Config::Get().general.enableLightFx)
            {
                disabledWidgets &= ~(1uLL << WIDX_ENABLE_LIGHT_FX_FOR_VEHICLES_CHECKBOX);
            }
            else
            {
                disabledWidgets |= (1uLL << WIDX_ENABLE_LIGHT_FX_FOR_VEHICLES_CHECKBOX);
                Config::Get().general.enableLightFxForVehicles = false;
            }

            setCheckboxValue(
                WIDX_RENDER_WEATHER_EFFECTS_CHECKBOX,
                Config::Get().general.renderWeatherEffects || Config::Get().general.renderWeatherGloom);
            setCheckboxValue(WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX, Config::Get().general.disableLightningEffect);
            if (!Config::Get().general.renderWeatherEffects && !Config::Get().general.renderWeatherGloom)
            {
                setCheckboxValue(WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX, true);
                disabledWidgets |= (1uLL << WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX);
            }
            else
            {
                disabledWidgets &= ~(1uLL << WIDX_DISABLE_LIGHTNING_EFFECT_CHECKBOX);
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
                    gDropdown.items[0] = Dropdown::MenuLabel(STR_HEIGHT_IN_UNITS);
                    gDropdown.items[1] = Dropdown::MenuLabel(STR_REAL_VALUES);

                    ShowDropdown(widget, 2);

                    gDropdown.items[Config::Get().general.showHeightAsUnits ? 0 : 1].setChecked(true);
                    break;
                case WIDX_CURRENCY_DROPDOWN:
                {
                    // All the currencies plus the separator
                    constexpr auto numItems = EnumValue(CurrencyType::count) + 1;

                    // All the currencies except custom currency
                    constexpr size_t numOrdinaryCurrencies = EnumValue(CurrencyType::count) - 1;

                    for (size_t i = 0; i < numOrdinaryCurrencies; i++)
                    {
                        gDropdown.items[i] = Dropdown::MenuLabel(CurrencyDescriptors[i].stringId);
                    }

                    gDropdown.items[numOrdinaryCurrencies] = Dropdown::Separator();

                    gDropdown.items[numOrdinaryCurrencies + 1] = Dropdown::MenuLabel(
                        CurrencyDescriptors[EnumValue(CurrencyType::custom)].stringId);

                    ShowDropdown(widget, numItems);

                    if (Config::Get().general.currencyFormat == CurrencyType::custom)
                    {
                        gDropdown.items[EnumValue(Config::Get().general.currencyFormat) + 1].setChecked(true);
                    }
                    else
                    {
                        gDropdown.items[EnumValue(Config::Get().general.currencyFormat)].setChecked(true);
                    }
                    break;
                }
                case WIDX_DISTANCE_DROPDOWN:
                    gDropdown.items[0] = Dropdown::MenuLabel(STR_IMPERIAL);
                    gDropdown.items[1] = Dropdown::MenuLabel(STR_METRIC);
                    gDropdown.items[2] = Dropdown::MenuLabel(STR_SI);

                    ShowDropdown(widget, 3);

                    gDropdown.items[static_cast<int32_t>(Config::Get().general.measurementFormat)].setChecked(true);
                    break;
                case WIDX_TEMPERATURE_DROPDOWN:
                    gDropdown.items[0] = Dropdown::MenuLabel(STR_CELSIUS);
                    gDropdown.items[1] = Dropdown::MenuLabel(STR_FAHRENHEIT);

                    ShowDropdown(widget, 2);

                    gDropdown.items[static_cast<int32_t>(Config::Get().general.temperatureFormat)].setChecked(true);
                    break;
                case WIDX_LANGUAGE_DROPDOWN:
                    for (size_t i = 1; i < LANGUAGE_COUNT; i++)
                    {
                        gDropdown.items[i - 1] = Dropdown::MenuLabel(LanguagesDescriptors[i].native_name);
                    }
                    ShowDropdown(widget, LANGUAGE_COUNT - 1);
                    gDropdown.items[LocalisationService_GetCurrentLanguage() - 1].setChecked(true);
                    break;
                case WIDX_DATE_FORMAT_DROPDOWN:
                    for (size_t i = 0; i < 4; i++)
                    {
                        gDropdown.items[i] = Dropdown::MenuLabel(DateFormatStringIDs[i]);
                    }
                    ShowDropdown(widget, 4);
                    gDropdown.items[Config::Get().general.dateFormat].setChecked(true);
                    break;
            }
        }

        void CultureDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_HEIGHT_LABELS_DROPDOWN:
                    // reset flag and set it to 1 if height as units is selected
                    Config::Get().general.showHeightAsUnits = false;

                    if (dropdownIndex == 0)
                    {
                        Config::Get().general.showHeightAsUnits = true;
                    }
                    Config::Save();
                    UpdateHeightMarkers();
                    break;
                case WIDX_CURRENCY_DROPDOWN:
                    if (dropdownIndex == EnumValue(CurrencyType::custom) + 1)
                    { // Add 1 because the separator occupies a position
                        Config::Get().general.currencyFormat = static_cast<CurrencyType>(dropdownIndex - 1);
                        ContextOpenWindow(WindowClass::customCurrencyConfig);
                    }
                    else
                    {
                        Config::Get().general.currencyFormat = static_cast<CurrencyType>(dropdownIndex);
                    }
                    Config::Save();
                    GfxInvalidateScreen();
                    break;
                case WIDX_DISTANCE_DROPDOWN:
                    Config::Get().general.measurementFormat = static_cast<MeasurementFormat>(dropdownIndex);
                    Config::Save();
                    UpdateHeightMarkers();
                    break;
                case WIDX_TEMPERATURE_DROPDOWN:
                    if (dropdownIndex != static_cast<int32_t>(Config::Get().general.temperatureFormat))
                    {
                        Config::Get().general.temperatureFormat = static_cast<TemperatureUnit>(dropdownIndex);
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
                            Config::Get().general.language = dropdownIndex + 1;
                            Config::Save();
                            GfxInvalidateScreen();
                        }
                    }
                }
                break;
                case WIDX_DATE_FORMAT_DROPDOWN:
                    if (dropdownIndex != Config::Get().general.dateFormat)
                    {
                        Config::Get().general.dateFormat = static_cast<uint8_t>(dropdownIndex);
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
            widgets[WIDX_CURRENCY].text = CurrencyDescriptors[EnumValue(Config::Get().general.currencyFormat)].stringId;

            // Distance: metric / imperial / si
            {
                StringId stringId = kStringIdNone;
                switch (Config::Get().general.measurementFormat)
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
            widgets[WIDX_DATE_FORMAT].text = DateFormatStringIDs[Config::Get().general.dateFormat];

            // Temperature: celsius/fahrenheit
            widgets[WIDX_TEMPERATURE].text = Config::Get().general.temperatureFormat == TemperatureUnit::Fahrenheit
                ? STR_FAHRENHEIT
                : STR_CELSIUS;

            // Height: units/real values
            widgets[WIDX_HEIGHT_LABELS].text = Config::Get().general.showHeightAsUnits ? STR_HEIGHT_IN_UNITS : STR_REAL_VALUES;
        }

#pragma endregion

#pragma region Audio tab events
        void AudioMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_SOUND_CHECKBOX:
                    Config::Get().sound.soundEnabled = !Config::Get().sound.soundEnabled;
                    Config::Save();
                    invalidate();
                    break;

                case WIDX_MASTER_SOUND_CHECKBOX:
                {
                    Config::Get().sound.masterSoundEnabled = !Config::Get().sound.masterSoundEnabled;
                    if (!Config::Get().sound.masterSoundEnabled)
                        Pause();
                    else
                        Resume();

                    auto* windowMgr = GetWindowManager();
                    windowMgr->InvalidateByClass(WindowClass::topToolbar);
                    Config::Save();
                    invalidate();
                    break;
                }

                case WIDX_MUSIC_CHECKBOX:
                    Config::Get().sound.rideMusicEnabled = !Config::Get().sound.rideMusicEnabled;
                    if (!Config::Get().sound.rideMusicEnabled)
                    {
                        RideAudio::StopAllChannels();
                    }
                    Config::Save();
                    invalidate();
                    break;

                case WIDX_AUDIO_FOCUS_CHECKBOX:
                    Config::Get().sound.audioFocus = !Config::Get().sound.audioFocus;
                    Config::Save();
                    invalidate();
                    break;
            }
        }

        void AudioMouseDown(WidgetIndex widgetIndex)
        {
            Widget* widget = &widgets[widgetIndex - 1];

            switch (widgetIndex)
            {
                case WIDX_SOUND_DROPDOWN:
                    PopulateDevices();

                    // populate the list with the sound devices
                    for (int32_t i = 0; i < GetDeviceCount(); i++)
                    {
                        gDropdown.items[i] = Dropdown::MenuLabel(GetDeviceName(i).c_str());
                    }

                    ShowDropdown(widget, GetDeviceCount());

                    gDropdown.items[GetCurrentDeviceIndex()].setChecked(true);
                    break;
                case WIDX_TITLE_MUSIC_DROPDOWN:
                {
                    const bool rct1MusicThemeIsAvailable = isRCT1TitleMusicAvailable();
                    int32_t numItems{};
                    int32_t checkedIndex{};
                    for (auto theme : TitleThemeOptions)
                    {
                        if (theme.Kind == TitleMusicKind::RCT1 && !rct1MusicThemeIsAvailable)
                            continue;

                        if (Config::Get().sound.titleMusic == theme.Kind)
                            checkedIndex = numItems;

                        gDropdown.items[numItems++] = Dropdown::MenuLabel(theme.Name);
                    }
                    ShowDropdown(widget, numItems);
                    gDropdown.items[checkedIndex].setChecked(true);
                    break;
                }
            }
        }

        void AudioDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_SOUND_DROPDOWN:
                    InitRideSounds(dropdownIndex);
                    if (dropdownIndex < GetDeviceCount())
                    {
                        auto& audioContext = GetContext()->GetAudioContext();
                        if (dropdownIndex == 0)
                        {
                            audioContext.SetOutputDevice("");
                            Config::Get().sound.device = "";
                        }
                        else
                        {
                            const auto& deviceName = GetDeviceName(dropdownIndex);
                            audioContext.SetOutputDevice(deviceName);
                            Config::Get().sound.device = deviceName;
                        }
                        Config::Save();
                        PlayTitleMusic();
                    }
                    invalidate();
                    break;
                case WIDX_TITLE_MUSIC_DROPDOWN:
                {
                    // HACK: When RCT1 is not available, it's not in the dropdown, so indices higher than it should be
                    // incremented
                    const bool rct1MusicThemeIsAvailable = isRCT1TitleMusicAvailable();
                    for (size_t i = 0; i < std::size(TitleThemeOptions) && static_cast<int32_t>(i) <= dropdownIndex; i++)
                    {
                        if (TitleThemeOptions[i].Kind == TitleMusicKind::RCT1 && !rct1MusicThemeIsAvailable)
                            dropdownIndex++;
                    }

                    Config::Get().sound.titleMusic = TitleThemeOptions[dropdownIndex].Kind;
                    Config::Save();
                    invalidate();

                    StopTitleMusic();
                    if (Config::Get().sound.titleMusic != TitleMusicKind::None)
                    {
                        PlayTitleMusic();
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
            if (masterVolume != Config::Get().sound.masterVolume)
            {
                Config::Get().sound.masterVolume = masterVolume;
                Config::Save();
                invalidateWidget(WIDX_MASTER_VOLUME);
            }

            const auto& soundVolumeWidget = widgets[WIDX_MASTER_VOLUME];
            const auto& soundVolumeScroll = scrolls[1];
            uint8_t soundVolume = GetScrollPercentage(soundVolumeWidget, soundVolumeScroll);
            if (soundVolume != Config::Get().sound.soundVolume)
            {
                Config::Get().sound.soundVolume = soundVolume;
                Config::Save();
                invalidateWidget(WIDX_SOUND_VOLUME);
            }

            const auto& musicVolumeWidget = widgets[WIDX_MASTER_VOLUME];
            const auto& musicVolumeScroll = scrolls[2];
            uint8_t rideMusicVolume = GetScrollPercentage(musicVolumeWidget, musicVolumeScroll);
            if (rideMusicVolume != Config::Get().sound.rideMusicVolume)
            {
                Config::Get().sound.rideMusicVolume = rideMusicVolume;
                Config::Save();
                invalidateWidget(WIDX_MUSIC_VOLUME);
            }
        }

        void ControlsUpdate()
        {
            const auto& deadzoneWidget = widgets[WIDX_GAMEPAD_DEADZONE];
            const auto& deadzoneScroll = scrolls[0];
            uint8_t deadzonePercent = GetScrollPercentage(deadzoneWidget, deadzoneScroll);
            int32_t deadzoneValue = static_cast<int32_t>((deadzonePercent / 100.0f) * 32767);
            if (deadzoneValue != Config::Get().general.gamepadDeadzone)
            {
                Config::Get().general.gamepadDeadzone = deadzoneValue;
                Config::Save();
            }

            const auto& sensitivityWidget = widgets[WIDX_GAMEPAD_SENSITIVITY];
            const auto& sensitivityScroll = scrolls[1];
            uint8_t sensitivityPercent = GetScrollPercentage(sensitivityWidget, sensitivityScroll);
            float sensitivityValue = 0.5f + (sensitivityPercent / 100.0f) * 2.5f; // Map 0-100% to 0.5-3.0
            if (std::abs(sensitivityValue - Config::Get().general.gamepadSensitivity) > 0.01f)
            {
                Config::Get().general.gamepadSensitivity = sensitivityValue;
                Config::Save();
            }
        }

        ScreenSize ControlsScrollGetSize(int32_t scrollIndex)
        {
            switch (scrollIndex)
            {
                case 0:                // Deadzone slider
                    return { 500, 0 }; // Range 0-500 (same as audio sliders)
                case 1:                // Sensitivity slider
                    return { 500, 0 }; // Range 0-500 (same as audio sliders)
                default:
                    return { 0, 0 };
            }
        }

        ScreenSize AudioScrollGetSize(int32_t scrollIndex)
        {
            return { 500, 0 };
        }

        StringId GetTitleMusicName() const
        {
            auto theme = std::find_if(std::begin(TitleThemeOptions), std::end(TitleThemeOptions), [](auto&& option) {
                return Config::Get().sound.titleMusic == option.Kind;
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
            const int32_t currentDeviceIndex = GetCurrentDeviceIndex();
            if (currentDeviceIndex == -1 || GetDeviceCount() == 0)
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
                    audioDeviceName = GetDeviceName(currentDeviceIndex).c_str();
                }
            }

            widgets[WIDX_SOUND].text = audioDeviceStringId;
            auto ft = Formatter::Common();
            ft.Add<char*>(audioDeviceName);

            widgets[WIDX_TITLE_MUSIC].text = GetTitleMusicName();

            setCheckboxValue(WIDX_SOUND_CHECKBOX, Config::Get().sound.soundEnabled);
            setCheckboxValue(WIDX_MASTER_SOUND_CHECKBOX, Config::Get().sound.masterSoundEnabled);
            setCheckboxValue(WIDX_MUSIC_CHECKBOX, Config::Get().sound.rideMusicEnabled);
            setCheckboxValue(WIDX_AUDIO_FOCUS_CHECKBOX, Config::Get().sound.audioFocus);
            widgetSetEnabled(*this, WIDX_SOUND_CHECKBOX, Config::Get().sound.masterSoundEnabled);
            widgetSetEnabled(*this, WIDX_MUSIC_CHECKBOX, Config::Get().sound.masterSoundEnabled);

            // Initialize only on first frame, otherwise the scrollbars won't be able to be modified
            if (currentFrame == 0)
            {
                initialiseScrollPosition(WIDX_MASTER_VOLUME, 0, Config::Get().sound.masterVolume);
                initialiseScrollPosition(WIDX_SOUND_VOLUME, 1, Config::Get().sound.soundVolume);
                initialiseScrollPosition(WIDX_MUSIC_VOLUME, 2, Config::Get().sound.rideMusicVolume);
            }
        }

#pragma endregion

#pragma region Controls tab events
        void ControlsMouseUp(WidgetIndex widgetIndex)
        {
            auto* windowMgr = GetWindowManager();

            switch (widgetIndex)
            {
                case WIDX_HOTKEY_DROPDOWN:
                    ContextOpenWindow(WindowClass::keyboardShortcutList);
                    break;
                case WIDX_SCREEN_EDGE_SCROLLING:
                    Config::Get().general.edgeScrolling ^= 1;
                    Config::Save();
                    invalidate();
                    break;
                case WIDX_TRAP_CURSOR:
                    Config::Get().general.trapCursor ^= 1;
                    Config::Save();
                    ContextSetCursorTrap(Config::Get().general.trapCursor);
                    invalidate();
                    break;
                case WIDX_ZOOM_TO_CURSOR:
                    Config::Get().general.zoomToCursor ^= 1;
                    Config::Save();
                    invalidate();
                    break;
                case WIDX_WINDOW_BUTTONS_ON_THE_LEFT:
                    Config::Get().interface.windowButtonsOnTheLeft ^= 1;
                    Config::Save();
                    invalidate();
                    windowMgr->InvalidateAll();
                    WindowVisitEach([](WindowBase* w) { w->resizeFrame(); });
                    break;
                case WIDX_ENLARGED_UI:
                    Config::Get().interface.enlargedUi ^= 1;
                    if (Config::Get().interface.enlargedUi == false)
                        Config::Get().interface.touchEnhancements = false;
                    Config::Save();
                    invalidate();
                    windowMgr->InvalidateAll();
                    WindowVisitEach([](WindowBase* w) { w->resizeFrame(); });
                    break;
                case WIDX_TOUCH_ENHANCEMENTS:
                    Config::Get().interface.touchEnhancements ^= 1;
                    Config::Save();
                    invalidate();
                    windowMgr->InvalidateAll();
                    break;
                case WIDX_INVERT_DRAG:
                    Config::Get().general.invertViewportDrag ^= 1;
                    Config::Save();
                    invalidate();
                    break;
            }
        }

        void ControlsPrepareDraw()
        {
            setCheckboxValue(WIDX_SCREEN_EDGE_SCROLLING, Config::Get().general.edgeScrolling);
            setCheckboxValue(WIDX_TRAP_CURSOR, Config::Get().general.trapCursor);
            setCheckboxValue(WIDX_INVERT_DRAG, Config::Get().general.invertViewportDrag);
            setCheckboxValue(WIDX_ZOOM_TO_CURSOR, Config::Get().general.zoomToCursor);
            setCheckboxValue(WIDX_WINDOW_BUTTONS_ON_THE_LEFT, Config::Get().interface.windowButtonsOnTheLeft);
            setCheckboxValue(WIDX_ENLARGED_UI, Config::Get().interface.enlargedUi);
            setCheckboxValue(WIDX_TOUCH_ENHANCEMENTS, Config::Get().interface.touchEnhancements);

            widgetSetEnabled(*this, WIDX_TOUCH_ENHANCEMENTS, Config::Get().interface.enlargedUi);

            // Initialize scroll positions for sliders only on first frame
            if (currentFrame == 0)
            {
                // Convert deadzone (0-32767) to percentage (0-100), then to scroll position (0-500)
                uint8_t deadzonePercent = static_cast<uint8_t>((Config::Get().general.gamepadDeadzone / 32767.0f) * 100);
                initialiseScrollPosition(WIDX_GAMEPAD_DEADZONE, 0, deadzonePercent);

                // Convert sensitivity (0.5-3.0) to percentage (0-100), then to scroll position (0-500)
                uint8_t sensitivityPercent = static_cast<uint8_t>(
                    ((Config::Get().general.gamepadSensitivity - 0.5f) / 2.5f) * 100);
                initialiseScrollPosition(WIDX_GAMEPAD_SENSITIVITY, 1, sensitivityPercent);
            }
        }

#pragma endregion

#pragma region Interface tab events
        void ToggleToolbarSetting(bool& setting)
        {
            setting ^= true;
            Config::Save();
            invalidate();

            auto* windowMgr = GetWindowManager();
            windowMgr->InvalidateByClass(WindowClass::topToolbar);
        }

        void InterfaceMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_TOOLBAR_BUTTONS_CENTRED:
                    ToggleToolbarSetting(Config::Get().interface.toolbarButtonsCentred);
                    break;
                case WIDX_TOOLBAR_SHOW_FINANCES:
                    ToggleToolbarSetting(Config::Get().interface.toolbarShowFinances);
                    break;
                case WIDX_TOOLBAR_SHOW_RESEARCH:
                    ToggleToolbarSetting(Config::Get().interface.toolbarShowResearch);
                    break;
                case WIDX_TOOLBAR_SHOW_CHEATS:
                    ToggleToolbarSetting(Config::Get().interface.toolbarShowCheats);
                    break;
                case WIDX_TOOLBAR_SHOW_NEWS:
                    ToggleToolbarSetting(Config::Get().interface.toolbarShowNews);
                    break;
                case WIDX_TOOLBAR_SHOW_MUTE:
                    ToggleToolbarSetting(Config::Get().interface.toolbarShowMute);
                    break;
                case WIDX_TOOLBAR_SHOW_CHAT:
                    ToggleToolbarSetting(Config::Get().interface.toolbarShowChat);
                    break;
                case WIDX_TOOLBAR_SHOW_ZOOM:
                    ToggleToolbarSetting(Config::Get().interface.toolbarShowZoom);
                    break;
                case WIDX_TOOLBAR_SHOW_ROTATE_ANTI_CLOCKWISE:
                    ToggleToolbarSetting(Config::Get().interface.toolbarShowRotateAnticlockwise);
                    break;
                case WIDX_THEMES_BUTTON:
                    ContextOpenWindow(WindowClass::themes);
                    invalidate();
                    break;
            }
        }

        void InterfaceMouseDown(WidgetIndex widgetIndex)
        {
            Widget* widget = &widgets[widgetIndex - 1];

            switch (widgetIndex)
            {
                case WIDX_THEMES_DROPDOWN:
                    uint32_t numItems = static_cast<uint32_t>(ThemeManagerGetNumAvailableThemes());

                    for (size_t i = 0; i < numItems; i++)
                    {
                        gDropdown.items[i] = Dropdown::MenuLabel(ThemeManagerGetAvailableThemeName(i));
                    }

                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height(), colours[1], 0,
                        Dropdown::Flag::StayOpen, numItems, widget->width() - 4);

                    gDropdown.items[static_cast<int32_t>(ThemeManagerGetAvailableThemeIndex())].setChecked(true);
                    invalidateWidget(WIDX_THEMES_DROPDOWN);
                    break;
            }
        }

        void InterfaceDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
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

        void InterfacePrepareDraw()
        {
            setCheckboxValue(WIDX_TOOLBAR_BUTTONS_CENTRED, Config::Get().interface.toolbarButtonsCentred);
            setCheckboxValue(WIDX_TOOLBAR_SHOW_FINANCES, Config::Get().interface.toolbarShowFinances);
            setCheckboxValue(WIDX_TOOLBAR_SHOW_RESEARCH, Config::Get().interface.toolbarShowResearch);
            setCheckboxValue(WIDX_TOOLBAR_SHOW_CHEATS, Config::Get().interface.toolbarShowCheats);
            setCheckboxValue(WIDX_TOOLBAR_SHOW_NEWS, Config::Get().interface.toolbarShowNews);
            setCheckboxValue(WIDX_TOOLBAR_SHOW_MUTE, Config::Get().interface.toolbarShowMute);
            setCheckboxValue(WIDX_TOOLBAR_SHOW_CHAT, Config::Get().interface.toolbarShowChat);
            setCheckboxValue(WIDX_TOOLBAR_SHOW_ZOOM, Config::Get().interface.toolbarShowZoom);
            setCheckboxValue(WIDX_TOOLBAR_SHOW_ROTATE_ANTI_CLOCKWISE, Config::Get().interface.toolbarShowRotateAnticlockwise);

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
                    Config::Get().general.showRealNamesOfGuests ^= 1;
                    Config::Save();
                    invalidate();
                    PeepUpdateNames();
                    break;
                case WIDX_REAL_NAMES_STAFF_CHECKBOX:
                    Config::Get().general.showRealNamesOfStaff ^= 1;
                    Config::Save();
                    invalidate();
                    PeepUpdateNames();
                    break;
                case WIDX_AUTO_STAFF_PLACEMENT:
                    Config::Get().general.autoStaffPlacement ^= 1;
                    Config::Save();
                    invalidate();
                    break;
                case WIDX_SCENARIO_UNLOCKING:
                {
                    Config::Get().general.scenarioUnlockingEnabled ^= 1;
                    Config::Save();
                    auto* windowMgr = GetWindowManager();
                    windowMgr->InvalidateByClass(WindowClass::scenarioSelect);
                    break;
                }
                case WIDX_AUTO_OPEN_SHOPS:
                    Config::Get().general.autoOpenShops = !Config::Get().general.autoOpenShops;
                    Config::Save();
                    invalidate();
                    break;
                case WIDX_ALLOW_EARLY_COMPLETION:
                    Config::Get().general.allowEarlyCompletion ^= 1;
                    // only the server can control this setting and needs to send the
                    // current value of allow_early_completion to all clients
                    if (Network::GetMode() == Network::Mode::server)
                    {
                        auto setAllowEarlyCompletionAction = GameActions::ScenarioSetSettingAction(
                            GameActions::ScenarioSetSetting::AllowEarlyCompletion, Config::Get().general.allowEarlyCompletion);
                        GameActions::Execute(&setAllowEarlyCompletionAction, getGameState());
                    }
                    Config::Save();
                    invalidate();
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
                        gDropdown.items[i] = Dropdown::MenuLabel(TitleSequenceManager::GetName(i));
                    }

                    gDropdown.items[numItems] = Dropdown::Separator();
                    numItems++;
                    gDropdown.items[numItems] = Dropdown::MenuLabel(STR_TITLE_SEQUENCE_RANDOM);
                    numItems++;

                    WindowDropdownShowText(
                        { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height(), colours[1],
                        Dropdown::Flag::StayOpen, numItems);

                    auto selectedIndex = Config::Get().interface.randomTitleSequence
                        ? numItems - 1
                        : static_cast<int32_t>(TitleGetCurrentSequence());
                    gDropdown.items[selectedIndex].setChecked(true);
                    break;
                }
                case WIDX_SCENARIO_PREVIEWS_DROPDOWN:
                {
                    uint32_t numItems = 2;

                    gDropdown.items[0] = Dropdown::MenuLabel(STR_SCENARIO_PREVIEWS_MINIMAPS);
                    gDropdown.items[1] = Dropdown::MenuLabel(STR_SCENARIO_PREVIEWS_SCREENSHOTS);

                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height(), colours[1], 0,
                        Dropdown::Flag::StayOpen, numItems, widget->width() - 4);

                    gDropdown.items[Config::Get().interface.scenarioPreviewScreenshots].setChecked(true);
                    break;
                }
                case WIDX_DEFAULT_INSPECTION_INTERVAL_DROPDOWN:
                    for (size_t i = 0; i < 7; i++)
                    {
                        gDropdown.items[i] = Dropdown::MenuLabel(kRideInspectionIntervalNames[i]);
                    }

                    ShowDropdown(widget, 7);
                    auto selectedIndex = EnumValue(Config::Get().general.defaultInspectionInterval);
                    gDropdown.items[selectedIndex].setChecked(true);
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
                        Config::Get().interface.randomTitleSequence = false;
                        TitleSequenceChangePreset(static_cast<size_t>(dropdownIndex));
                        Config::Save();
                        invalidate();
                    }
                    else if (dropdownIndex == numItems + 1)
                    {
                        Config::Get().interface.randomTitleSequence = true;
                        Config::Save();
                        invalidate();
                    }
                    break;
                }
                case WIDX_DEFAULT_INSPECTION_INTERVAL_DROPDOWN:
                    if (dropdownIndex != EnumValue(Config::Get().general.defaultInspectionInterval))
                    {
                        Config::Get().general.defaultInspectionInterval = static_cast<RideInspection>(dropdownIndex);
                        Config::Save();
                        invalidate();
                    }
                    break;
                case WIDX_SCENARIO_PREVIEWS_DROPDOWN:
                    if (dropdownIndex != static_cast<int32_t>(Config::Get().interface.scenarioPreviewScreenshots))
                    {
                        Config::Get().interface.scenarioPreviewScreenshots = dropdownIndex;
                        Config::Save();
                        invalidate();
                        auto* windowMgr = GetWindowManager();
                        windowMgr->InvalidateByClass(WindowClass::scenarioSelect);
                    }
                    break;
            }
        }

        void MiscPrepareDraw()
        {
            auto ft = Formatter::Common();
            if (Config::Get().interface.randomTitleSequence)
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
            if (Network::GetMode() != Network::Mode::none)
            {
                disabledWidgets |= (1uLL << WIDX_REAL_NAMES_GUESTS_CHECKBOX) | (1uLL << WIDX_REAL_NAMES_STAFF_CHECKBOX);
                widgets[WIDX_REAL_NAMES_GUESTS_CHECKBOX].tooltip = STR_OPTION_DISABLED_DURING_NETWORK_PLAY;
                widgets[WIDX_REAL_NAMES_STAFF_CHECKBOX].tooltip = STR_OPTION_DISABLED_DURING_NETWORK_PLAY;

                // Disable the use of the allow_early_completion option during network play on clients.
                // This is to prevent confusion on clients because changing this setting during network play wouldn't change
                // the way scenarios are completed during this network-session
                if (Network::GetMode() == Network::Mode::client)
                {
                    disabledWidgets |= (1uLL << WIDX_ALLOW_EARLY_COMPLETION);
                    widgets[WIDX_ALLOW_EARLY_COMPLETION].tooltip = STR_OPTION_DISABLED_DURING_NETWORK_PLAY;
                }
            }

            setCheckboxValue(WIDX_REAL_NAMES_GUESTS_CHECKBOX, Config::Get().general.showRealNamesOfGuests);
            setCheckboxValue(WIDX_REAL_NAMES_STAFF_CHECKBOX, Config::Get().general.showRealNamesOfStaff);
            setCheckboxValue(WIDX_AUTO_STAFF_PLACEMENT, Config::Get().general.autoStaffPlacement);
            setCheckboxValue(WIDX_AUTO_OPEN_SHOPS, Config::Get().general.autoOpenShops);
            setCheckboxValue(WIDX_ALLOW_EARLY_COMPLETION, Config::Get().general.allowEarlyCompletion);

            if (Config::Get().interface.scenarioPreviewScreenshots)
                widgets[WIDX_SCENARIO_PREVIEWS].text = STR_SCENARIO_PREVIEWS_SCREENSHOTS;
            else
                widgets[WIDX_SCENARIO_PREVIEWS].text = STR_SCENARIO_PREVIEWS_MINIMAPS;

            setCheckboxValue(WIDX_SCENARIO_UNLOCKING, Config::Get().general.scenarioUnlockingEnabled);

            auto selectedIndex = EnumValue(Config::Get().general.defaultInspectionInterval);
            widgets[WIDX_DEFAULT_INSPECTION_INTERVAL].text = kRideInspectionIntervalNames[selectedIndex];
        }

#pragma endregion

#pragma region Advanced tab events
        void AdvancedMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_DEBUGGING_TOOLS:
                    Config::Get().general.debuggingTools ^= 1;
                    Config::Save();
                    GfxInvalidateScreen();
                    break;
                case WIDX_EXPORT_CUSTOM_OBJECTS_CHECKBOX:
                    Config::Get().general.savePluginData ^= 1;
                    Config::Save();
                    invalidate();
                    break;
                case WIDX_STAY_CONNECTED_AFTER_DESYNC:
                    Config::Get().network.stayConnected = !Config::Get().network.stayConnected;
                    Config::Save();
                    invalidate();
                    break;
                case WIDX_ALWAYS_NATIVE_LOADSAVE:
                    Config::Get().general.useNativeBrowseDialog = !Config::Get().general.useNativeBrowseDialog;
                    Config::Save();
                    invalidate();
                    break;
                case WIDX_PATH_TO_RCT1_BROWSE:
                {
                    auto rct1path = GetContext()->GetUiContext().ShowDirectoryDialog(
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
                                    Config::Get().general.rct1Path = std::move(rct1path);
                                    Config::Get().interface.scenarioSelectLastTab = 0;
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
                    invalidate();
                    break;
                }
                case WIDX_PATH_TO_RCT1_CLEAR:
                    if (!Config::Get().general.rct1Path.empty())
                    {
                        Config::Get().general.rct1Path.clear();
                        Config::Save();
                    }
                    invalidate();
                    break;
                case WIDX_ASSET_PACKS:
                    ContextOpenWindow(WindowClass::assetPacks);
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
                        gDropdown.items[i] = Dropdown::MenuLabel(AutosaveNames[i]);
                    }

                    ShowDropdown(widget, AUTOSAVE_NEVER + 1);
                    gDropdown.items[Config::Get().general.autosaveFrequency].setChecked(true);
                    break;
                case WIDX_AUTOSAVE_AMOUNT_UP:
                    Config::Get().general.autosaveAmount += 1;
                    Config::Save();
                    invalidateWidget(WIDX_AUTOSAVE_FREQUENCY);
                    invalidateWidget(WIDX_AUTOSAVE_FREQUENCY_DROPDOWN);
                    invalidateWidget(WIDX_AUTOSAVE_AMOUNT);
                    break;
                case WIDX_AUTOSAVE_AMOUNT_DOWN:
                    if (Config::Get().general.autosaveAmount > 1)
                    {
                        Config::Get().general.autosaveAmount -= 1;
                        Config::Save();
                        invalidateWidget(WIDX_AUTOSAVE_FREQUENCY);
                        invalidateWidget(WIDX_AUTOSAVE_FREQUENCY_DROPDOWN);
                        invalidateWidget(WIDX_AUTOSAVE_AMOUNT);
                    }
            }
        }

        void AdvancedDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_AUTOSAVE_FREQUENCY_DROPDOWN:
                    if (dropdownIndex != Config::Get().general.autosaveFrequency)
                    {
                        Config::Get().general.autosaveFrequency = static_cast<uint8_t>(dropdownIndex);
                        Config::Save();
                        invalidate();
                    }
                    break;
            }
        }

        void AdvancedPrepareDraw()
        {
            if (!Config::Get().general.rct1Path.empty())
            {
                widgets[WIDX_PATH_TO_RCT1_PATH].type = WidgetType::label;
                widgets[WIDX_PATH_TO_RCT1_BROWSE].type = WidgetType::empty;
                widgets[WIDX_PATH_TO_RCT1_CLEAR].type = WidgetType::button;

                // Get 'Clear' button string width
                auto clearLabel = LanguageGetString(STR_CLEAR_BUTTON);
                auto clearLabelWidth = GfxGetStringWidth(clearLabel, FontStyle::medium) + 12;

                widgets[WIDX_PATH_TO_RCT1_CLEAR].right = widgets[WIDX_PAGE_BACKGROUND].right - 12;
                widgets[WIDX_PATH_TO_RCT1_CLEAR].left = widgets[WIDX_PATH_TO_RCT1_BROWSE].right - clearLabelWidth;
            }
            else
            {
                widgets[WIDX_PATH_TO_RCT1_PATH].type = WidgetType::empty;
                widgets[WIDX_PATH_TO_RCT1_BROWSE].type = WidgetType::button;
                widgets[WIDX_PATH_TO_RCT1_CLEAR].type = WidgetType::empty;

                // Get 'Browse' button string width
                auto browseLabel = LanguageGetString(STR_BROWSE);
                auto browseLabelWidth = GfxGetStringWidth(browseLabel, FontStyle::medium) + 12;

                widgets[WIDX_PATH_TO_RCT1_BROWSE].right = widgets[WIDX_PAGE_BACKGROUND].right - 12;
                widgets[WIDX_PATH_TO_RCT1_BROWSE].left = widgets[WIDX_PATH_TO_RCT1_BROWSE].right - browseLabelWidth;
            }

            setCheckboxValue(WIDX_EXPORT_CUSTOM_OBJECTS_CHECKBOX, Config::Get().general.savePluginData);
            setCheckboxValue(WIDX_ALWAYS_NATIVE_LOADSAVE, Config::Get().general.useNativeBrowseDialog);
            widgets[WIDX_AUTOSAVE_FREQUENCY].text = AutosaveNames[Config::Get().general.autosaveFrequency];

            setCheckboxValue(WIDX_DEBUGGING_TOOLS, Config::Get().general.debuggingTools);
            setCheckboxValue(WIDX_STAY_CONNECTED_AFTER_DESYNC, Config::Get().network.stayConnected);

#ifdef __EMSCRIPTEN__
            widgets[WIDX_GROUP_ADVANCED].bottom = kAdvancedStart + 84 + getTitleBarDiffNormal();
#else
            widgets[WIDX_GROUP_ADVANCED].bottom = kAdvancedStart + 64 + getTitleBarDiffNormal();
#endif

            widgets[WIDX_ASSET_PACKS].top = widgets[WIDX_GROUP_ADVANCED].bottom - 20;
            widgets[WIDX_ASSET_PACKS].bottom = widgets[WIDX_GROUP_ADVANCED].bottom - 6;
        }

        void AdvancedDraw(RenderTarget& rt)
        {
            auto ft = Formatter();
            ft.Add<int32_t>(Config::Get().general.autosaveAmount);
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ widgets[WIDX_AUTOSAVE_AMOUNT].left + 1, widgets[WIDX_AUTOSAVE_AMOUNT].top + 1 },
                STR_WINDOW_COLOUR_2_COMMA32, ft, { colours[1] });

            // Format RCT1 path
            const auto normalisedPath = Platform::StrDecompToPrecomp(Config::Get().general.rct1Path);
            ft = Formatter();
            ft.Add<StringId>(STR_STRING);
            ft.Add<const utf8*>(normalisedPath.c_str());

            auto& pathWidget = widgets[WIDX_PATH_TO_RCT1_PATH];
            int32_t widgetHeight = pathWidget.bottom - pathWidget.top;

            // Apply vertical alignment if font height requires it
            int32_t lineHeight = FontGetLineHeight(FontStyle::medium);
            int32_t padding = widgetHeight > lineHeight ? (widgetHeight - lineHeight) / 2 : 0;

            auto screenCoords = windowPos + ScreenCoordsXY{ pathWidget.left + 1, pathWidget.top + padding };
            DrawTextEllipsised(rt, screenCoords, pathWidget.width() - 1, STR_BLACK_STRING, ft);
        }

        OpenRCT2String AdvancedTooltip(WidgetIndex widgetIndex, StringId fallback)
        {
            if (widgetIndex == WIDX_PATH_TO_RCT1_PATH)
            {
                if (Config::Get().general.rct1Path.empty())
                {
                    // No tooltip if the path is empty
                    return { kStringIdNone, {} };
                }

                auto ft = Formatter();
                ft.Add<utf8*>(Config::Get().general.rct1Path.c_str());
                return { fallback, ft };
            }
            return { fallback, {} };
        }

#pragma endregion

        void setPage(int32_t p)
        {
            // Skip setting page if we're already on this page, unless we're initialising the window
            if (page == p && !widgets.empty())
                return;

            page = p;
            currentFrame = 0;
            pressedWidgets = 0;
            setWidgets(window_options_page_widgets[page]);

            invalidate();
            onPrepareDraw();
            onResize();
            initScrollWidgets();
            invalidate();
        }

        void SetPressedTab()
        {
            for (int32_t i = 0; i < WINDOW_OPTIONS_PAGE_COUNT; i++)
                pressedWidgets &= ~(1 << (WIDX_FIRST_TAB + i));
            pressedWidgets |= 1LL << (WIDX_FIRST_TAB + page);
        }

        void ShowDropdown(Widget* widget, int32_t num_items)
        {
            // helper function, all dropdown boxes have similar properties
            WindowDropdownShowTextCustomWidth(
                { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height(), colours[1], 0,
                Dropdown::Flag::StayOpen, num_items, widget->width() - 4);
        }

        void DrawTabImages(RenderTarget& rt)
        {
            DrawTabImage(rt, WINDOW_OPTIONS_PAGE_DISPLAY, SPR_G2_MONITOR_TAB_START);
            DrawTabImage(rt, WINDOW_OPTIONS_PAGE_RENDERING, SPR_G2_TAB_TREE);
            DrawTabImage(rt, WINDOW_OPTIONS_PAGE_CULTURE, SPR_TAB_TIMER_0);
            DrawTabImage(rt, WINDOW_OPTIONS_PAGE_AUDIO, SPR_TAB_MUSIC_0);
            DrawTabImage(rt, WINDOW_OPTIONS_PAGE_INTERFACE, SPR_TAB_PAINT_0);
            DrawTabImage(rt, WINDOW_OPTIONS_PAGE_CONTROLS, SPR_G2_CONTROLS_TAB_START);
            DrawTabImage(rt, WINDOW_OPTIONS_PAGE_MISC, SPR_TAB_RIDE_0);
            DrawTabImage(rt, WINDOW_OPTIONS_PAGE_ADVANCED, SPR_TAB_WRENCH_0);
        }

        void DrawTabImage(RenderTarget& rt, int32_t p, int32_t spriteIndex)
        {
            WidgetIndex widgetIndex = WIDX_FIRST_TAB + p;
            Widget* widget = &widgets[widgetIndex];

            auto screenCoords = windowPos + ScreenCoordsXY{ widget->left, widget->top };

            if (!widgetIsDisabled(*this, widgetIndex))
            {
                if (page == p)
                {
                    int32_t frame = currentFrame / TabAnimationDivisor[page];
                    spriteIndex += (frame % TabAnimationFrames[page]);
                }

                // Draw normal, enabled sprite.
                GfxDrawSprite(rt, ImageId(spriteIndex), screenCoords);
            }
            else
            {
                // Get the window background colour
                auto windowColour = colours[widget->colour].colour;

                // Draw greyed out (light border bottom right shadow)
                GfxDrawSpriteSolid(
                    rt, ImageId(spriteIndex), screenCoords + ScreenCoordsXY{ 1, 1 }, getColourMap(windowColour).lighter);

                // Draw greyed out (dark)
                GfxDrawSpriteSolid(rt, ImageId(spriteIndex), screenCoords, getColourMap(windowColour).midLight);
            }
        }

        void UpdateHeightMarkers()
        {
            Config::Save();
            GfxInvalidateScreen();
        }

        uint8_t GetScrollPercentage(const Widget& widget, const ScrollArea& scroll)
        {
            uint8_t w = widget.width() - 2;
            return static_cast<float>(scroll.contentOffsetX) / (scroll.contentWidth - w) * 100;
        }

        void initialiseScrollPosition(WidgetIndex widgetIndex, int32_t scrollId, uint8_t volume)
        {
            const auto& widget = widgets[widgetIndex];
            auto& scroll = scrolls[scrollId];

            int32_t widgetSize = scroll.contentWidth - (widget.width() - 2);
            scroll.contentOffsetX = ceil(volume / 100.0f * widgetSize);

            widgetScrollUpdateThumbs(*this, widgetIndex);
        }

        static bool isRCT1TitleMusicAvailable()
        {
            auto& env = GetContext()->GetPlatformEnvironment();
            auto rct1path = env.GetDirectoryPath(DirBase::rct1);
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
            4, // WINDOW_OPTIONS_PAGE_INTERFACE,
            8, // WINDOW_OPTIONS_PAGE_CONTROLS,
            4, // WINDOW_OPTIONS_PAGE_MISC,
            2, // WINDOW_OPTIONS_PAGE_ADVANCED,
        };

        static constexpr int32_t TabAnimationFrames[] = {
            SPR_G2_MONITOR_TAB_END - SPR_G2_MONITOR_TAB_START, // WINDOW_OPTIONS_PAGE_DISPLAY,
            1,                                                 // WINDOW_OPTIONS_PAGE_RENDERING,
            8,                                                 // WINDOW_OPTIONS_PAGE_CULTURE,
            16,                                                // WINDOW_OPTIONS_PAGE_AUDIO,
            8,                                                 // WINDOW_OPTIONS_PAGE_INTERFACE,
            8,                                                 // WINDOW_OPTIONS_PAGE_CONTROLS,
            16,                                                // WINDOW_OPTIONS_PAGE_MISC,
            16,                                                // WINDOW_OPTIONS_PAGE_ADVANCED,
        };
    };

    /**
     *
     *  rct2: 0x006BAC5B
     */
    WindowBase* OptionsOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<OptionsWindow>(WindowClass::options, kWindowSize, WindowFlag::centreScreen);
    }
} // namespace OpenRCT2::Ui::Windows
