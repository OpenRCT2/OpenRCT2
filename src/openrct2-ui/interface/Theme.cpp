/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma warning(disable : 4706) // assignment within conditional expression

#include "Theme.h"

#include "Window.h"

#include <algorithm>
#include <memory>
#include <openrct2/Context.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/common.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/File.h>
#include <openrct2/core/FileScanner.h>
#include <openrct2/core/Guard.hpp>
#include <openrct2/core/Json.hpp>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/localisation/Language.h>
#include <openrct2/localisation/StringIds.h>
#include <stdexcept>
#include <vector>

using namespace OpenRCT2;

struct WindowThemeDesc;

/**
 * Represents a window theming style such as the colour scheme.
 */
struct WindowTheme
{
    colour_t Colours[6];
};

/**
 * Represents the style for a particular type of window.
 */
struct UIThemeWindowEntry
{
    WindowClass Class;
    WindowTheme Theme;

    json_t ToJson() const;
    /**
     * @note json is deliberately left non-const: json_t behaviour changes when const
     */
    static UIThemeWindowEntry FromJson(const WindowThemeDesc* wtDesc, json_t& json);
};

/**
 * Represents a user interface theme. Contains window colour schemes and appearance features.
 */
class UITheme
{
public:
    std::string Name;
    std::vector<UIThemeWindowEntry> Entries;
    uint8_t Flags = 0;

    explicit UITheme(const std::string& name)
        : Name(name)
    {
    }

    const UIThemeWindowEntry* GetEntry(WindowClass windowClass) const;
    void SetEntry(const UIThemeWindowEntry* entry);
    void RemoveEntry(WindowClass windowClass);

    json_t ToJson() const;
    bool WriteToFile(const std::string& path) const;

    /**
     * @note json is deliberately left non-const: json_t behaviour changes when const
     */
    static UITheme* FromJson(json_t& json);
    static UITheme* FromFile(const std::string& path);
    static UITheme CreatePredefined(const std::string& name, const UIThemeWindowEntry* entries, uint8_t flags);
};

/**
 * Represents the theme descriptor for a specific window type including the default colour scheme.
 */
struct WindowThemeDesc
{
    ::WindowClass WindowClass;
    const utf8* WindowClassSZ;
    StringId WindowName;
    uint8_t NumColours;
    WindowTheme DefaultTheme;
};

#pragma region Window Theme Descriptors

// clang-format off
#define COLOURS_1(c0) 1, { { (c0), 0, 0, 0, 0, 0 } }
#define COLOURS_2(c0, c1) 2, { { (c0), (c1), 0, 0, 0, 0 } }
#define COLOURS_3(c0, c1, c2) 3, { { (c0), (c1), (c2), 0, 0, 0 } }
#define COLOURS_4(c0, c1, c2, c3) 4, { { (c0), (c1), (c2), (c3), 0, 0 } }
#define COLOURS_5(c0, c1, c2, c3, c4) 5, { { (c0), (c1), (c2), (c3), (c4), 0 } }
#define COLOURS_6(c0, c1, c2, c3, c4, c5) 6, { { (c0), (c1), (c2), (c3), (c4), (c5) } }

#define THEME_DEF_END { WindowClass::Null, { 0, 0, 0, 0, 0, 0 } }

#define TWINDOW(window_class, window_name, window_string_id, theme) { window_class, window_name, window_string_id, theme }

static constexpr const WindowThemeDesc WindowThemeDescriptors[] =
{
    // WindowClass                              WindowClassSZ                        WindowName                                        NumColours, DefaultTheme
    { WindowClass::TopToolbar,                  "WC_TOP_TOOLBAR",                    STR_THEMES_WINDOW_TOP_TOOLBAR,                    COLOURS_4(COLOUR_LIGHT_BLUE,               COLOUR_DARK_GREEN,               COLOUR_DARK_BROWN,             COLOUR_GREY         ) },
    { WindowClass::BottomToolbar,               "WC_BOTTOM_TOOLBAR",                 STR_THEMES_WINDOW_BOTTOM_TOOLBAR,                 COLOURS_4(TRANSLUCENT(COLOUR_DARK_GREEN),  TRANSLUCENT(COLOUR_DARK_GREEN),  COLOUR_BLACK,                  COLOUR_BRIGHT_GREEN ) },
    { WindowClass::Ride,                        "WC_RIDE",                           STR_THEMES_WINDOW_RIDE,                           COLOURS_3(COLOUR_GREY,                     COLOUR_BORDEAUX_RED,             COLOUR_SATURATED_GREEN                             ) },
    { WindowClass::RideConstruction,            "WC_RIDE_CONSTRUCTION",              STR_THEMES_WINDOW_RIDE_CONSTRUCTION,              COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { WindowClass::RideList,                    "WC_RIDE_LIST",                      STR_THEMES_WINDOW_RIDE_LIST,                      COLOURS_3(COLOUR_GREY,                     COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED                                ) },
    { WindowClass::SavePrompt,                  "WC_SAVE_PROMPT",                    STR_THEMES_WINDOW_SAVE_PROMPT,                    COLOURS_1(TRANSLUCENT(COLOUR_BORDEAUX_RED)                                                                                     ) },
    { WindowClass::ConstructRide,               "WC_CONSTRUCT_RIDE",                 STR_THEMES_WINDOW_CONSTRUCT_RIDE,                 COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED                                ) },
    { WindowClass::DemolishRidePrompt,          "WC_DEMOLISH_RIDE_PROMPT",           STR_THEMES_WINDOW_DEMOLISH_RIDE_PROMPT,           COLOURS_1(TRANSLUCENT(COLOUR_BORDEAUX_RED)                                                                                     ) },
    { WindowClass::Scenery,                     "WC_SCENERY",                        STR_THEMES_WINDOW_SCENERY,                        COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_GREEN,               COLOUR_DARK_GREEN                                  ) },
    { WindowClass::SceneryScatter,              "WC_SCENERY_SCATTER",                STR_THEMES_WINDOW_SCENERY_SCATTER,                COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_GREEN,               COLOUR_DARK_GREEN                                  ) },
    { WindowClass::Options,                     "WC_OPTIONS",                        STR_THEMES_WINDOW_OPTIONS,                        COLOURS_3(COLOUR_GREY,                     COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                  ) },
    { WindowClass::AssetPacks,                  "WC_ASSET_PACKS",                    STR_ASSET_PACKS,                                  COLOURS_3(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                  ) },
    { WindowClass::Footpath,                    "WC_FOOTPATH",                       STR_THEMES_WINDOW_FOOTPATH,                       COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { WindowClass::Land,                        "WC_LAND",                           STR_THEMES_WINDOW_LAND,                           COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { WindowClass::Water,                       "WC_WATER",                          STR_THEMES_WINDOW_WATER,                          COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { WindowClass::Peep,                        "WC_PEEP",                           STR_THEMES_WINDOW_PEEP,                           COLOURS_3(COLOUR_GREY,                     COLOUR_OLIVE_GREEN,              COLOUR_OLIVE_GREEN                                 ) },
    { WindowClass::GuestList,                   "WC_GUEST_LIST",                     STR_THEMES_WINDOW_GUEST_LIST,                     COLOURS_3(COLOUR_GREY,                     COLOUR_OLIVE_GREEN,              COLOUR_OLIVE_GREEN                                 ) },
    { WindowClass::StaffList,                   "WC_STAFF_LIST",                     STR_THEMES_WINDOW_STAFF_LIST,                     COLOURS_3(COLOUR_GREY,                     COLOUR_LIGHT_PURPLE,             COLOUR_LIGHT_PURPLE                                ) },
    { WindowClass::FirePrompt,                  "WC_FIRE_PROMPT",                    STR_THEMES_WINDOW_FIRE_PROMPT,                    COLOURS_1(TRANSLUCENT(COLOUR_BORDEAUX_RED)                                                                                     ) },
    { WindowClass::ParkInformation,             "WC_PARK_INFORMATION",               STR_THEMES_WINDOW_PARK_INFORMATION,               COLOURS_3(COLOUR_GREY,                     COLOUR_DARK_YELLOW,              COLOUR_DARK_YELLOW                                 ) },
    { WindowClass::Finances,                    "WC_FINANCES",                       STR_THEMES_WINDOW_FINANCES,                       COLOURS_3(COLOUR_GREY,                     COLOUR_DARK_YELLOW,              COLOUR_DARK_YELLOW                                 ) },
    { WindowClass::TitleMenu,                   "WC_TITLE_MENU",                     STR_THEMES_WINDOW_TITLE_MENU_BUTTONS,             COLOURS_3(TRANSLUCENT(COLOUR_DARK_GREEN),  TRANSLUCENT(COLOUR_DARK_GREEN),  TRANSLUCENT(COLOUR_DARK_GREEN)                     ) },
    { WindowClass::TitleExit,                   "WC_TITLE_EXIT",                     STR_THEMES_WINDOW_TITLE_MENU_EXIT,                COLOURS_3(TRANSLUCENT(COLOUR_DARK_GREEN),  TRANSLUCENT(COLOUR_DARK_GREEN),  TRANSLUCENT(COLOUR_DARK_GREEN)                     ) },
    { WindowClass::RecentNews,                  "WC_RECENT_NEWS",                    STR_THEMES_WINDOW_RECENT_NEWS,                    COLOURS_3(COLOUR_GREY,                     COLOUR_GREY,                     COLOUR_BLACK                                       ) },
    { WindowClass::ScenarioSelect,              "WC_SCENARIO_SELECT",                STR_THEMES_WINDOW_TITLE_MENU_SCENARIO_SELECTION,  COLOURS_3(COLOUR_GREY,                     COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED                                ) },
    { WindowClass::TrackDesignList,             "WC_TRACK_DESIGN_LIST",              STR_THEMES_WINDOW_TRACK_DESIGN_LIST,              COLOURS_3(COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED                                ) },
    { WindowClass::TrackDesignPlace,            "WC_TRACK_DESIGN_PLACE",             STR_THEMES_WINDOW_TRACK_DESIGN_PLACE,             COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { WindowClass::NewCampaign,                 "WC_NEW_CAMPAIGN",                   STR_THEMES_WINDOW_NEW_CAMPAIGN,                   COLOURS_3(COLOUR_DARK_YELLOW,              COLOUR_DARK_YELLOW,              COLOUR_DARK_YELLOW                                 ) },
    { WindowClass::KeyboardShortcutList,        "WC_KEYBOARD_SHORTCUT_LIST",         STR_THEMES_WINDOW_KEYBOARD_SHORTCUT_LIST,         COLOURS_3(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                  ) },
    { WindowClass::ChangeKeyboardShortcut,      "WC_CHANGE_KEYBOARD_SHORTCUT",       STR_THEMES_WINDOW_CHANGE_KEYBOARD_SHORTCUT,       COLOURS_3(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                  ) },
    { WindowClass::ResetShortcutKeysPrompt,     "WC_RESET_SHORTCUT_KEYS_PROMPT",     STR_SHORTCUT_ACTION_RESET,                        COLOURS_1(TRANSLUCENT(COLOUR_BORDEAUX_RED)                                                                                     ) },
    { WindowClass::Map,                         "WC_MAP",                            STR_THEMES_WINDOW_MAP,                            COLOURS_2(COLOUR_DARK_GREEN,               COLOUR_DARK_BROWN                                                                   ) },
    { WindowClass::Banner,                      "WC_BANNER",                         STR_THEMES_WINDOW_BANNER,                         COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { WindowClass::EditorObjectSelection,       "WC_EDITOR_OBJECT_SELECTION",        STR_THEMES_WINDOW_EDITOR_OBJECT_SELECTION,        COLOURS_3(COLOUR_LIGHT_PURPLE,             COLOUR_GREY,                     COLOUR_GREY                                        ) },
    { WindowClass::EditorInventionList,         "WC_EDITOR_INVENTION_LIST",          STR_THEMES_WINDOW_EDITOR_INVENTION_LIST,          COLOURS_3(COLOUR_LIGHT_PURPLE,             COLOUR_GREY,                     COLOUR_GREY                                        ) },
    { WindowClass::EditorScenarioOptions,       "WC_EDITOR_SCENARIO_OPTIONS",        STR_THEMES_WINDOW_EDITOR_SCENARIO_OPTIONS,        COLOURS_3(COLOUR_LIGHT_PURPLE,             COLOUR_GREY,                     COLOUR_GREY                                        ) },
    { WindowClass::EditorObjectiveOptions,      "WC_EDITOR_OBJECTIVE_OPTIONS",       STR_THEMES_WINDOW_EDTIOR_OBJECTIVE_OPTIONS,       COLOURS_3(COLOUR_LIGHT_PURPLE,             COLOUR_GREY,                     COLOUR_GREY                                        ) },
    { WindowClass::ManageTrackDesign,           "WC_MANAGE_TRACK_DESIGN",            STR_THEMES_WINDOW_MANAGE_TRACK_DESIGN,            COLOURS_3(COLOUR_GREY,                     COLOUR_GREY,                     COLOUR_GREY                                        ) },
    { WindowClass::TrackDeletePrompt,           "WC_TRACK_DELETE_PROMPT",            STR_THEMES_WINDOW_TRACK_DELETE_PROMPT,            COLOURS_3(COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED                                ) },
    { WindowClass::InstallTrack,                "WC_INSTALL_TRACK",                  STR_THEMES_WINDOW_INSTALL_TRACK,                  COLOURS_3(COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED                                ) },
    { WindowClass::ClearScenery,                "WC_CLEAR_SCENERY",                  STR_THEMES_WINDOW_CLEAR_SCENERY,                  COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { WindowClass::Cheats,                      "WC_CHEATS",                         STR_CHEAT_TITLE,                                  COLOURS_2(COLOUR_GREY,                     COLOUR_DARK_YELLOW                                                                  ) },
    { WindowClass::Research,                    "WC_RESEARCH",                       STR_THEMES_WINDOW_RESEARCH,                       COLOURS_3(COLOUR_GREY,                     COLOUR_DARK_YELLOW,              COLOUR_DARK_YELLOW                                 ) },
    { WindowClass::Viewport,                    "WC_VIEWPORT",                       STR_THEMES_WINDOW_VIEWPORT,                       COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { WindowClass::Mapgen,                      "WC_MAPGEN",                         STR_THEMES_WINDOW_MAPGEN,                         COLOURS_3(COLOUR_DARK_GREEN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { WindowClass::Loadsave,                    "WC_LOADSAVE",                       STR_THEMES_WINDOW_LOADSAVE,                       COLOURS_3(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                  ) },
    { WindowClass::LoadsaveOverwritePrompt,     "WC_LOADSAVE_OVERWRITE_PROMPT",      STR_THEMES_WINDOW_LOADSAVE_OVERWRITE_PROMPT,      COLOURS_1(TRANSLUCENT(COLOUR_BORDEAUX_RED)                                                                                     ) },
    { WindowClass::TitleOptions,                "WC_TITLE_OPTIONS",                  STR_THEMES_WINDOW_TITLE_MENU_OPTIONS,             COLOURS_3(TRANSLUCENT(COLOUR_DARK_GREEN),  TRANSLUCENT(COLOUR_DARK_GREEN),  TRANSLUCENT(COLOUR_DARK_GREEN)                     ) },
    { WindowClass::LandRights,                  "WC_LAND_RIGHTS",                    STR_THEMES_WINDOW_LAND_RIGHTS,                    COLOURS_3(COLOUR_DARK_YELLOW,              COLOUR_DARK_YELLOW,              COLOUR_DARK_YELLOW                                 ) },
    { WindowClass::Themes,                      "WC_THEMES",                         STR_THEMES_WINDOW_THEMES,                         COLOURS_3(COLOUR_GREY,                     COLOUR_DARK_GREEN,               COLOUR_DARK_GREEN                                  ) },
    { WindowClass::Staff,                       "WC_STAFF",                          STR_THEMES_WINDOW_STAFF,                          COLOURS_3(COLOUR_GREY,                     COLOUR_LIGHT_PURPLE,             COLOUR_LIGHT_PURPLE                                ) },
    { WindowClass::EditorTrackBottomToolbar,    "WC_EDITOR_TRACK_BOTTOM_TOOLBAR",    STR_THEMES_WINDOW_BOTTOM_TOOLBAR_TRACK_EDITOR,    COLOURS_3(TRANSLUCENT(COLOUR_LIGHT_BLUE),  TRANSLUCENT(COLOUR_LIGHT_BLUE),  TRANSLUCENT(COLOUR_LIGHT_BLUE)                     ) },
    { WindowClass::EditorScenarioBottomToolbar, "WC_EDITOR_SCENARIO_BOTTOM_TOOLBAR", STR_THEMES_WINDOW_BOTTOM_TOOLBAR_SCENARIO_EDITOR, COLOURS_3(TRANSLUCENT(COLOUR_LIGHT_BROWN), TRANSLUCENT(COLOUR_LIGHT_BROWN), TRANSLUCENT(COLOUR_MOSS_GREEN)                     ) },
    { WindowClass::TileInspector,               "WC_TILE_INSPECTOR",                 STR_TILE_INSPECTOR_TITLE,                         COLOURS_2(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                                                   ) },
    { WindowClass::ViewClipping,                "WC_VIEW_CLIPPING",                  STR_VIEW_CLIPPING_TITLE,                          COLOURS_1(COLOUR_DARK_GREEN                                                                                                    ) },
    { WindowClass::PatrolArea,                  "WC_PATROL_AREA",                    STR_SET_PATROL_AREA,                              COLOURS_3(COLOUR_LIGHT_PURPLE,             COLOUR_LIGHT_PURPLE,             COLOUR_LIGHT_PURPLE                                ) },
    { WindowClass::Transparency,                "WC_TRANSPARENCY",                   STR_TRANSPARENCY_OPTIONS_TITLE,                   COLOURS_3(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                  ) },
    { WindowClass::About,                       "WC_ABOUT",                          STR_ABOUT,                                        COLOURS_2(COLOUR_GREY,                     COLOUR_LIGHT_BLUE                                                                   ) },
    { WindowClass::Changelog,                   "WC_CHANGELOG",                      STR_CHANGELOG_TITLE,                              COLOURS_2(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                                                   ) },
    { WindowClass::Multiplayer,                 "WC_MULTIPLAYER",                    STR_MULTIPLAYER,                                  COLOURS_3(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                  ) },
    { WindowClass::Player,                      "WC_PLAYER",                         STR_THEMES_WINDOW_PLAYER,                         COLOURS_3(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                  ) },
    { WindowClass::NetworkStatus,               "WC_NETWORK_STATUS",                 STR_THEMES_WINDOW_NETWORK_STATUS,                 COLOURS_1(COLOUR_LIGHT_BLUE                                                                                                    ) },
    { WindowClass::ServerList,                  "WC_SERVER_LIST",                    STR_SERVER_LIST,                                  COLOURS_2(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                                                   ) },
    { WindowClass::Chat,                        "WC_CHAT",                           STR_CHAT,                                         COLOURS_1(TRANSLUCENT(COLOUR_GREY)                                                                                             ) },
    { WindowClass::Console,                     "WC_CONSOLE",                        STR_CONSOLE,                                      COLOURS_2(TRANSLUCENT(COLOUR_LIGHT_BLUE),  COLOUR_WHITE                                                                        ) },
};

#pragma endregion

#pragma region Pre-defined Themes

#define COLOURS_RCT1(c0, c1, c2, c3, c4, c5) { { (c0), (c1), (c2), (c3), (c4), (c5) } }

static constexpr const UIThemeWindowEntry PredefinedThemeRCT1_Entries[] =
{
    { WindowClass::TopToolbar,             COLOURS_RCT1(COLOUR_GREY,               COLOUR_GREY,                COLOUR_GREY,                COLOUR_GREY,     COLOUR_BLACK,    COLOUR_BLACK)    },
    { WindowClass::BottomToolbar,          COLOURS_RCT1(TRANSLUCENT(COLOUR_GREY),  TRANSLUCENT(COLOUR_GREY),   COLOUR_BLACK,               COLOUR_YELLOW,   COLOUR_BLACK,    COLOUR_BLACK)    },
    { WindowClass::Ride,                   COLOURS_RCT1(COLOUR_BORDEAUX_RED,       COLOUR_GREY,                COLOUR_SATURATED_GREEN,     COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WindowClass::RideList,               COLOURS_RCT1(COLOUR_BORDEAUX_RED,       COLOUR_GREY,                COLOUR_GREY,                COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WindowClass::ConstructRide,          COLOURS_RCT1(COLOUR_BORDEAUX_RED,       COLOUR_GREY,                COLOUR_GREY,                COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WindowClass::Peep,                   COLOURS_RCT1(COLOUR_LIGHT_BROWN,        COLOUR_BORDEAUX_RED,        COLOUR_BORDEAUX_RED,        COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WindowClass::GuestList,              COLOURS_RCT1(COLOUR_LIGHT_BROWN,        COLOUR_BORDEAUX_RED,        COLOUR_BORDEAUX_RED,        COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WindowClass::StaffList,              COLOURS_RCT1(COLOUR_DARK_GREEN,         COLOUR_LIGHT_PURPLE,        COLOUR_LIGHT_PURPLE,        COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WindowClass::Finances,               COLOURS_RCT1(COLOUR_LIGHT_PURPLE,       COLOUR_GREY,                COLOUR_GREY,                COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WindowClass::TitleMenu,              COLOURS_RCT1(TRANSLUCENT(COLOUR_GREY),  TRANSLUCENT(COLOUR_GREY),   TRANSLUCENT(COLOUR_GREY),   COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WindowClass::TitleExit,              COLOURS_RCT1(TRANSLUCENT(COLOUR_GREY),  TRANSLUCENT(COLOUR_GREY),   TRANSLUCENT(COLOUR_GREY),   COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WindowClass::NewCampaign,            COLOURS_RCT1(COLOUR_LIGHT_PURPLE,       COLOUR_LIGHT_PURPLE,        COLOUR_GREY,                COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WindowClass::TitleOptions,           COLOURS_RCT1(TRANSLUCENT(COLOUR_GREY),  TRANSLUCENT(COLOUR_GREY),   TRANSLUCENT(COLOUR_GREY),   COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WindowClass::Staff,                  COLOURS_RCT1(COLOUR_DARK_GREEN,         COLOUR_LIGHT_PURPLE,        COLOUR_LIGHT_PURPLE,        COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WindowClass::Options,                COLOURS_RCT1(COLOUR_GREY,               COLOUR_DARK_BROWN,          COLOUR_DARK_BROWN,          COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WindowClass::AssetPacks,             COLOURS_RCT1(COLOUR_DARK_BROWN,         COLOUR_DARK_BROWN,          COLOUR_DARK_BROWN,          COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WindowClass::KeyboardShortcutList,   COLOURS_RCT1(COLOUR_DARK_BROWN,         COLOUR_DARK_BROWN,          COLOUR_DARK_BROWN,          COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WindowClass::ChangeKeyboardShortcut, COLOURS_RCT1(COLOUR_DARK_BROWN,         COLOUR_DARK_BROWN,          COLOUR_DARK_BROWN,          COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WindowClass::TrackDesignList,        COLOURS_RCT1(COLOUR_DARK_BROWN,         COLOUR_DARK_BROWN,          COLOUR_DARK_BROWN,          COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WindowClass::Map,                    COLOURS_RCT1(COLOUR_DARK_BROWN,         COLOUR_GREY,                COLOUR_GREY,                COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WindowClass::About,                  COLOURS_RCT1(COLOUR_GREY,               COLOUR_DARK_BROWN,          COLOUR_WHITE,               COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WindowClass::Changelog,              COLOURS_RCT1(COLOUR_DARK_BROWN,         COLOUR_DARK_BROWN,          COLOUR_WHITE,               COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    THEME_DEF_END,
};
// clang-format on

static constexpr const UIThemeWindowEntry PredefinedThemeRCT2_Entries[] = {
    THEME_DEF_END,
};

const UITheme PredefinedThemeRCT1 = UITheme::CreatePredefined(
    "*RCT1", PredefinedThemeRCT1_Entries,
    UITHEME_FLAG_USE_LIGHTS_RIDE | UITHEME_FLAG_USE_LIGHTS_PARK | UITHEME_FLAG_USE_ALTERNATIVE_SCENARIO_SELECT_FONT
        | UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR);

const UITheme PredefinedThemeRCT2 = UITheme::CreatePredefined("*RCT2", PredefinedThemeRCT2_Entries, 0);

struct PredefinedTheme
{
    const UITheme* Theme;
    StringId Name;
};

static constexpr const PredefinedTheme PredefinedThemes[] = {
    { &PredefinedThemeRCT1, STR_TITLE_SEQUENCE_RCT1 },
    { &PredefinedThemeRCT2, STR_TITLE_SEQUENCE_RCT2 },
};

#pragma endregion

static const WindowThemeDesc* GetWindowThemeDescriptor(WindowClass windowClass)
{
    for (const auto& desc : WindowThemeDescriptors)
    {
        if (desc.WindowClass == windowClass)
        {
            return &desc;
        }
    }
    return nullptr;
}

static const WindowThemeDesc* GetWindowThemeDescriptor(const utf8* windowClassSZ)
{
    for (const auto& desc : WindowThemeDescriptors)
    {
        if (String::Equals(desc.WindowClassSZ, windowClassSZ))
        {
            return &desc;
        }
    }
    return nullptr;
}

static void ThrowThemeLoadException()
{
    throw std::runtime_error("Invalid JSON UI theme entry.");
}

#pragma region UIThemeEntry

json_t UIThemeWindowEntry::ToJson() const
{
    const WindowThemeDesc* wtDesc = GetWindowThemeDescriptor(Class);
    if (wtDesc == nullptr)
    {
        return nullptr;
    }

    json_t jsonColours = json_t::array();
    for (uint8_t i = 0; i < wtDesc->NumColours; i++)
    {
        colour_t colour = Theme.Colours[i];
        jsonColours.emplace_back(colour);
    }

    json_t jsonEntry = {
        { "colours", jsonColours },
    };

    return jsonEntry;
}

UIThemeWindowEntry UIThemeWindowEntry::FromJson(const WindowThemeDesc* wtDesc, json_t& jsonData)
{
    Guard::Assert(jsonData.is_object(), "UIThemeWindowEntry::FromJson expects parameter jsonData to be object");

    auto jsonColours = Json::AsArray(jsonData["colours"]);

    if (jsonColours.empty())
    {
        ThrowThemeLoadException();
    }

    UIThemeWindowEntry result{};
    result.Class = wtDesc->WindowClass;
    result.Theme = wtDesc->DefaultTheme;

    // result.Theme.Colours only has 6 values
    size_t colourCount = std::min(jsonColours.size(), static_cast<size_t>(wtDesc->NumColours));

    for (size_t i = 0; i < colourCount; i++)
    {
        result.Theme.Colours[i] = Json::GetNumber<colour_t>(jsonColours[i]);
    }

    return result;
}

#pragma endregion

#pragma region UITheme

const UIThemeWindowEntry* UITheme::GetEntry(WindowClass windowClass) const
{
    for (const auto& entry : Entries)
    {
        if (entry.Class == windowClass)
        {
            return &entry;
        }
    }
    return nullptr;
}

void UITheme::SetEntry(const UIThemeWindowEntry* newEntry)
{
    // Try to replace existing entry
    for (auto& entry : Entries)
    {
        if (entry.Class == newEntry->Class)
        {
            entry = *newEntry;
            return;
        }
    }

    Entries.push_back(*newEntry);
}

void UITheme::RemoveEntry(WindowClass windowClass)
{
    // Remove existing entry
    for (size_t i = 0; i < Entries.size(); i++)
    {
        UIThemeWindowEntry* entry = &Entries[i];
        if (entry->Class == windowClass)
        {
            Entries.erase(Entries.begin() + i);
            break;
        }
    }
}

json_t UITheme::ToJson() const
{
    // Create entries
    json_t jsonEntries;
    for (const UIThemeWindowEntry& entry : Entries)
    {
        const WindowThemeDesc* wtDesc = GetWindowThemeDescriptor(entry.Class);
        if (wtDesc == nullptr)
        {
            return nullptr;
        }
        jsonEntries[wtDesc->WindowClassSZ] = entry.ToJson();
    }

    // Create theme object
    json_t jsonTheme = {
        { "name", Name },
        { "entries", jsonEntries },
        { "useLightsRide", (Flags & UITHEME_FLAG_USE_LIGHTS_RIDE) != 0 },
        { "useLightsPark", (Flags & UITHEME_FLAG_USE_LIGHTS_PARK) != 0 },
        { "useAltScenarioSelectFont", (Flags & UITHEME_FLAG_USE_ALTERNATIVE_SCENARIO_SELECT_FONT) != 0 },
        { "useFullBottomToolbar", (Flags & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR) != 0 },
    };

    return jsonTheme;
}

bool UITheme::WriteToFile(const std::string& path) const
{
    auto jsonTheme = ToJson();
    bool result;
    try
    {
        Json::WriteToFile(path.c_str(), jsonTheme);
        result = true;
    }
    catch (const std::exception& ex)
    {
        LOG_ERROR("Unable to save %s: %s", path.c_str(), ex.what());
        result = false;
    }

    return result;
}

UITheme* UITheme::FromJson(json_t& jsonObj)
{
    Guard::Assert(jsonObj.is_object(), "UITheme::FromJson expects parameter jsonObj to be object");

    const std::string themeName = Json::GetString(jsonObj["name"]);
    if (themeName.empty())
    {
        ThrowThemeLoadException();
    }

    json_t jsonEntries = jsonObj["entries"];

    UITheme* result = nullptr;
    try
    {
        result = new UITheme(themeName);

        result->Flags = Json::GetFlags<uint8_t>(
            jsonObj,
            {
                { "useLightsRide", UITHEME_FLAG_USE_LIGHTS_RIDE },
                { "useLightsPark", UITHEME_FLAG_USE_LIGHTS_PARK },
                { "useAltScenarioSelectFont", UITHEME_FLAG_USE_ALTERNATIVE_SCENARIO_SELECT_FONT },
                { "useFullBottomToolbar", UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR },
            });

        if (jsonEntries.is_object())
        {
            for (auto& [jsonKey, jsonValue] : jsonEntries.items())
            {
                if (jsonValue.is_object())
                {
                    const WindowThemeDesc* wtDesc = GetWindowThemeDescriptor(jsonKey.data());
                    if (wtDesc == nullptr)
                    {
                        continue;
                    }

                    UIThemeWindowEntry entry = UIThemeWindowEntry::FromJson(wtDesc, jsonValue);
                    result->SetEntry(&entry);
                }
            }
        }

        return result;
    }
    catch (const std::exception&)
    {
        delete result;
        throw;
    }
}

UITheme* UITheme::FromFile(const std::string& path)
{
    UITheme* result = nullptr;
    json_t json;
    try
    {
        json = Json::ReadFromFile(path.c_str());
        result = UITheme::FromJson(json);
    }
    catch (const std::exception&)
    {
        LOG_ERROR("Unable to read theme: %s", path.c_str());
        result = nullptr;
    }
    return result;
}

UITheme UITheme::CreatePredefined(const std::string& name, const UIThemeWindowEntry* entries, uint8_t flags)
{
    auto theme = UITheme(name);
    theme.Flags = flags | UITHEME_FLAG_PREDEFINED;

    size_t numEntries = 0;
    for (const UIThemeWindowEntry* entry = entries; entry->Class != WindowClass::Null; entry++)
    {
        numEntries++;
    }

    theme.Entries = std::vector<UIThemeWindowEntry>(entries, entries + numEntries);
    return theme;
}

#pragma endregion

namespace ThemeManager
{
    struct AvailableTheme
    {
        std::string Path;
        std::string Name;
    };

    static std::string CurrentThemePath;
    static UITheme* CurrentTheme;
    static std::vector<AvailableTheme> AvailableThemes;
    static size_t ActiveAvailableThemeIndex = SIZE_MAX;
    static size_t NumPredefinedThemes = 0;

    std::string GetThemeFileName(const std::string& name);
    bool EnsureThemeDirectoryExists();
    std::string GetThemePath();

    static void GetAvailableThemes(std::vector<AvailableTheme>* outThemes)
    {
        Guard::ArgumentNotNull(outThemes, GUARD_LINE);

        outThemes->clear();

        NumPredefinedThemes = 0;
        for (auto predefinedTheme : PredefinedThemes)
        {
            AvailableTheme theme{};
            theme.Name = predefinedTheme.Theme->Name;
            outThemes->push_back(std::move(theme));

            NumPredefinedThemes++;
        }

        auto themesPattern = Path::Combine(GetThemePath(), u8"*.json");
        auto scanner = Path::ScanDirectory(themesPattern, true);
        while (scanner->Next())
        {
            const auto& fileInfo = scanner->GetFileInfo();
            auto name = Path::GetFileNameWithoutExtension(fileInfo.Name);

            AvailableTheme theme{};
            theme.Name = name;
            theme.Path = GetThemeFileName(name);
            outThemes->push_back(std::move(theme));

            if (Path::Equals(CurrentThemePath, scanner->GetPath()))
            {
                ActiveAvailableThemeIndex = outThemes->size() - 1;
            }
        }
    }

    static void LoadTheme(UITheme* theme)
    {
        if (CurrentTheme == theme)
        {
            return;
        }

        if (CurrentTheme != nullptr)
        {
            if (!(CurrentTheme->Flags & UITHEME_FLAG_PREDEFINED))
            {
                delete CurrentTheme;
            }
        }

        CurrentTheme = theme;
        CurrentThemePath.clear();

        GfxInvalidateScreen();
    }

    static void LoadTheme(const std::string& path)
    {
        auto theme = UITheme::FromFile(path);
        if (theme == nullptr)
        {
            // Fall-back to default
            theme = const_cast<UITheme*>(&PredefinedThemeRCT2);
            LoadTheme(theme);
        }
        else
        {
            LoadTheme(theme);
            CurrentThemePath = path;
        }
    }

    static bool LoadThemeByConfigName(const utf8* name)
    {
        for (size_t i = 0; i < ThemeManager::AvailableThemes.size(); i++)
        {
            const auto& theme = ThemeManager::AvailableThemes[i];
            if (String::Equals(name, theme.Name))
            {
                if (theme.Path.empty())
                {
                    LoadTheme(const_cast<UITheme*>(PredefinedThemes[i].Theme));
                }
                else
                {
                    LoadTheme(theme.Path);
                }
                ActiveAvailableThemeIndex = i;
                return true;
            }
        }
        return false;
    }

    static void Initialise()
    {
        ThemeManager::GetAvailableThemes(&ThemeManager::AvailableThemes);
        LoadTheme(const_cast<UITheme*>(&PredefinedThemeRCT2));
        ActiveAvailableThemeIndex = 1;

        bool configValid = false;
        if (!gConfigInterface.CurrentThemePreset.empty())
        {
            if (LoadThemeByConfigName(gConfigInterface.CurrentThemePreset.c_str()))
            {
                configValid = true;
            }
        }

        if (!configValid)
        {
            gConfigInterface.CurrentThemePreset = ThemeManagerGetAvailableThemeConfigName(1);
        }
    }

    std::string GetThemeFileName(const std::string& name)
    {
        auto themeDirectory = GetThemePath();
        auto themePath = Path::Combine(themeDirectory, name + u8".json");
        return themePath;
    }

    bool EnsureThemeDirectoryExists()
    {
        try
        {
            auto path = GetThemePath();
            Path::CreateDirectory(path);
            return true;
        }
        catch (const std::exception&)
        {
            return false;
        }
    }

    std::string GetThemePath()
    {
        auto context = GetContext();
        auto env = context->GetPlatformEnvironment();
        return env->GetDirectoryPath(DIRBASE::USER, DIRID::THEME);
    }
} // namespace ThemeManager

void ThemeManagerLoadAvailableThemes()
{
    ThemeManager::GetAvailableThemes(&ThemeManager::AvailableThemes);
}

size_t ThemeManagerGetNumAvailableThemes()
{
    return ThemeManager::AvailableThemes.size();
}

const utf8* ThemeManagerGetAvailableThemePath(size_t index)
{
    return ThemeManager::AvailableThemes[index].Path.c_str();
}

const utf8* ThemeManagerGetAvailableThemeConfigName(size_t index)
{
    return ThemeManager::AvailableThemes[index].Name.c_str();
}
const utf8* ThemeManagerGetAvailableThemeName(size_t index)
{
    if (index < ThemeManager::NumPredefinedThemes)
        return LanguageGetString(PredefinedThemes[index].Name);
    return ThemeManager::AvailableThemes[index].Name.c_str();
}

size_t ThemeManagerGetAvailableThemeIndex()
{
    return ThemeManager::ActiveAvailableThemeIndex;
}

void ThemeManagerSetActiveAvailableTheme(size_t index)
{
    if (index < ThemeManager::NumPredefinedThemes)
    {
        ThemeManager::LoadTheme(const_cast<UITheme*>(PredefinedThemes[index].Theme));
    }
    else
    {
        auto path = ThemeManager::AvailableThemes[index].Path;
        ThemeManager::LoadTheme(path);

        // HACK Check if theme load failed and fell back to RCT2
        if (ThemeManager::CurrentThemePath.empty())
        {
            index = 1;
        }
    }
    ThemeManager::ActiveAvailableThemeIndex = index;
    gConfigInterface.CurrentThemePreset = ThemeManagerGetAvailableThemeConfigName(index);

    ColourSchemeUpdateAll();
}

size_t ThemeGetIndexForName(const utf8* name)
{
    size_t count = ThemeManager::AvailableThemes.size();
    for (size_t i = 0; i < count; i++)
    {
        const utf8* tn = ThemeManagerGetAvailableThemeName(i);
        if (String::Equals(tn, name, true))
        {
            return i;
        }
    }
    return SIZE_MAX;
}

uint8_t ThemeGetColour(WindowClass wc, uint8_t index)
{
    const UIThemeWindowEntry* entry = ThemeManager::CurrentTheme->GetEntry(wc);
    if (entry == nullptr)
    {
        const WindowThemeDesc* desc = GetWindowThemeDescriptor(wc);
        if (desc == nullptr)
        {
            return 0;
        }
        return desc->DefaultTheme.Colours[index];
    }

    return entry->Theme.Colours[index];
}

void ThemeSetColour(WindowClass wc, uint8_t index, colour_t colour)
{
    UIThemeWindowEntry entry{};
    entry.Class = wc;

    auto currentEntry = const_cast<UIThemeWindowEntry*>(ThemeManager::CurrentTheme->GetEntry(wc));
    if (currentEntry != nullptr)
    {
        entry.Theme = currentEntry->Theme;
    }
    else
    {
        const WindowThemeDesc* desc = GetWindowThemeDescriptor(wc);
        if (desc == nullptr)
        {
            return;
        }
        entry.Theme = desc->DefaultTheme;
    }

    entry.Theme.Colours[index] = colour;
    ThemeManager::CurrentTheme->SetEntry(&entry);

    ThemeSave();
}

// Quick and dirty mapping for new colours to original colours, until flags are extracted from colour upper bits
colour_t ThemeOverrideExtendedColour(colour_t inputColour)
{
    switch (inputColour)
    {
        case COLOUR_DARK_OLIVE_DARK:
        case COLOUR_DARK_OLIVE_LIGHT:
            return COLOUR_DARK_OLIVE_GREEN;
        case COLOUR_SATURATED_BROWN_LIGHT:
            return COLOUR_LIGHT_BROWN;
        case COLOUR_BORDEAUX_RED_DARK:
        case COLOUR_BORDEAUX_RED_LIGHT:
            return COLOUR_BORDEAUX_RED;
        case COLOUR_GRASS_GREEN_DARK:
        case COLOUR_GRASS_GREEN_LIGHT:
            return COLOUR_MOSS_GREEN;
        case COLOUR_OLIVE_DARK:
        case COLOUR_OLIVE_LIGHT:
            return COLOUR_OLIVE_GREEN;
        case COLOUR_SATURATED_GREEN_LIGHT:
            return COLOUR_BRIGHT_GREEN;
        case COLOUR_TAN_DARK:
        case COLOUR_TAN_LIGHT:
            return COLOUR_SALMON_PINK;
        case COLOUR_DULL_PURPLE_LIGHT:
            return COLOUR_LIGHT_PURPLE;
        case COLOUR_DULL_GREEN_DARK:
        case COLOUR_DULL_GREEN_LIGHT:
            return COLOUR_DARK_GREEN;
        case COLOUR_SATURATED_PURPLE_DARK:
        case COLOUR_SATURATED_PURPLE_LIGHT:
            return COLOUR_BRIGHT_PURPLE;
        case COLOUR_ORANGE_LIGHT:
            return COLOUR_LIGHT_ORANGE;
        case COLOUR_AQUA_DARK:
            return COLOUR_AQUAMARINE;
        case COLOUR_MAGENTA_LIGHT:
            return COLOUR_BRIGHT_PINK;
        case COLOUR_DULL_BROWN_DARK:
        case COLOUR_DULL_BROWN_LIGHT:
            return COLOUR_DARK_BROWN;
        case COLOUR_INVISIBLE:
        case COLOUR_VOID:
            return COLOUR_BLACK;
        default:
            return inputColour;
    }
}

uint8_t ThemeGetFlags()
{
    return ThemeManager::CurrentTheme->Flags;
}

void ThemeSetFlags(uint8_t flags)
{
    ThemeManager::CurrentTheme->Flags = flags;
    ThemeSave();
}

void ThemeSave()
{
    ThemeManager::EnsureThemeDirectoryExists();
    ThemeManager::CurrentTheme->WriteToFile(ThemeManager::CurrentThemePath);
}

void ThemeRename(const utf8* name)
{
    const auto oldPath = ThemeManager::CurrentThemePath;

    ThemeManager::EnsureThemeDirectoryExists();
    auto newPath = ThemeManager::GetThemeFileName(name);
    File::Move(oldPath, newPath);
    ThemeManager::CurrentThemePath = newPath;

    ThemeManager::CurrentTheme->Name = name;
    ThemeManager::CurrentTheme->WriteToFile(ThemeManager::CurrentThemePath);

    ThemeManagerLoadAvailableThemes();
    for (size_t i = 0; i < ThemeManager::AvailableThemes.size(); i++)
    {
        if (Path::Equals(newPath, ThemeManager::AvailableThemes[i].Path))
        {
            ThemeManager::ActiveAvailableThemeIndex = i;
            gConfigInterface.CurrentThemePreset = ThemeManagerGetAvailableThemeConfigName(1);
            break;
        }
    }
}

void ThemeDuplicate(const utf8* name)
{
    ThemeManager::EnsureThemeDirectoryExists();
    auto newPath = ThemeManager::GetThemeFileName(name);

    // Copy the theme, save it and then load it back in
    auto newTheme = std::make_unique<UITheme>(*ThemeManager::CurrentTheme);
    newTheme->Name = name;
    newTheme->Flags &= ~UITHEME_FLAG_PREDEFINED;
    newTheme->WriteToFile(newPath);

    ThemeManager::LoadTheme(newPath);

    ThemeManagerLoadAvailableThemes();
    for (size_t i = 0; i < ThemeManager::AvailableThemes.size(); i++)
    {
        if (Path::Equals(newPath, ThemeManager::AvailableThemes[i].Path))
        {
            ThemeManager::ActiveAvailableThemeIndex = i;
            gConfigInterface.CurrentThemePreset = ThemeManagerGetAvailableThemeConfigName(i);
            break;
        }
    }
}

void ThemeDelete()
{
    File::Delete(ThemeManager::CurrentThemePath);
    ThemeManager::LoadTheme(const_cast<UITheme*>(&PredefinedThemeRCT2));
    ThemeManager::ActiveAvailableThemeIndex = 1;
    gConfigInterface.CurrentThemePreset = ThemeManagerGetAvailableThemeConfigName(1);
}

void ThemeManagerInitialise()
{
    ThemeManager::Initialise();
}

uint8_t ThemeDescGetNumColours(WindowClass wc)
{
    const WindowThemeDesc* desc = GetWindowThemeDescriptor(wc);
    if (desc == nullptr)
    {
        return 0;
    }
    return desc->NumColours;
}

StringId ThemeDescGetName(WindowClass wc)
{
    const WindowThemeDesc* desc = GetWindowThemeDescriptor(wc);
    if (desc == nullptr)
    {
        return STR_EMPTY;
    }
    return desc->WindowName;
}

void ColourSchemeUpdateAll()
{
    WindowVisitEach([](WindowBase* w) { ColourSchemeUpdate(w); });
}

void ColourSchemeUpdate(WindowBase* window)
{
    ColourSchemeUpdateByClass(window, window->classification);
}

void ColourSchemeUpdateByClass(WindowBase* window, WindowClass classification)
{
    const WindowTheme* windowTheme;
    const UIThemeWindowEntry* entry = ThemeManager::CurrentTheme->GetEntry(classification);
    if (entry != nullptr)
    {
        windowTheme = &entry->Theme;
    }
    else
    {
        const WindowThemeDesc* desc = GetWindowThemeDescriptor(classification);

        // Some windows don't have a theme set (e.g. main window, title screen)
        if (desc == nullptr)
        {
            return;
        }

        windowTheme = &desc->DefaultTheme;
    }

    for (int32_t i = 0; i < 6; i++)
    {
        window->colours[i] = windowTheme->Colours[i];
    }
    // Some windows need to be transparent even if the colours aren't.
    // There doesn't seem to be any side-effects for all windows being transparent
    window->flags |= WF_TRANSPARENT;
}
