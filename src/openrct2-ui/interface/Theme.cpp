/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma warning(disable : 4706) // assignment within conditional expression

#include "Theme.h"

#include "../UiStringIds.h"
#include "Window.h"

#include <memory>
#include <openrct2/Context.h>
#include <openrct2/Diagnostic.h>
#include <openrct2/PlatformEnvironment.h>
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

namespace OpenRCT2::Ui
{
    static constexpr uint8_t kCurrentThemeVersion = 1;

    struct WindowThemeDesc;

    /**
     * Represents a window theming style such as the colour scheme.
     */
    struct WindowTheme
    {
        ColourWithFlags Colours[6];
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
        static UIThemeWindowEntry FromJson(const WindowThemeDesc* wtDesc, json_t& json, uint8_t version);
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
        static UITheme CreatePredefined(const std::string& name, std::span<const UIThemeWindowEntry> entries, uint8_t flags);
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

    static constexpr ColourWithFlags opaque(colour_t colour)
    {
        return ColourWithFlags{ colour };
    }
    static constexpr ColourWithFlags translucent(colour_t colour)
    {
        return ColourWithFlags{ colour, EnumToFlag(ColourFlag::translucent) };
    }

    static constexpr WindowThemeDesc kWindowThemeDescriptors[] =
    {
        // WindowClass                              WindowClassSZ                        WindowName                                        NumColours, DefaultTheme
        { WindowClass::topToolbar,                  "WC_TOP_TOOLBAR",                    STR_THEMES_WINDOW_TOP_TOOLBAR,                    COLOURS_4(opaque(COLOUR_LIGHT_BLUE),               opaque(COLOUR_DARK_GREEN),               opaque(COLOUR_DARK_BROWN),             opaque(COLOUR_GREY)         ) },
        { WindowClass::bottomToolbar,               "WC_BOTTOM_TOOLBAR",                 STR_THEMES_WINDOW_BOTTOM_TOOLBAR,                 COLOURS_4(translucent(COLOUR_DARK_GREEN),          translucent(COLOUR_DARK_GREEN),          opaque(COLOUR_BLACK),                  opaque(COLOUR_BRIGHT_GREEN) ) },
        { WindowClass::ride,                        "WC_RIDE",                           STR_THEMES_WINDOW_RIDE,                           COLOURS_3(opaque(COLOUR_GREY),                     opaque(COLOUR_BORDEAUX_RED),             opaque(COLOUR_SATURATED_GREEN)                             ) },
        { WindowClass::rideConstruction,            "WC_RIDE_CONSTRUCTION",              STR_THEMES_WINDOW_RIDE_CONSTRUCTION,              COLOURS_3(opaque(COLOUR_DARK_BROWN),               opaque(COLOUR_DARK_BROWN),               opaque(COLOUR_DARK_BROWN)                                  ) },
        { WindowClass::rideList,                    "WC_RIDE_LIST",                      STR_THEMES_WINDOW_RIDE_LIST,                      COLOURS_3(opaque(COLOUR_GREY),                     opaque(COLOUR_BORDEAUX_RED),             opaque(COLOUR_BORDEAUX_RED)                                ) },
        { WindowClass::savePrompt,                  "WC_SAVE_PROMPT",                    STR_THEMES_WINDOW_SAVE_PROMPT,                    COLOURS_1(translucent(COLOUR_BORDEAUX_RED)                                                                                                             ) },
        { WindowClass::constructRide,               "WC_CONSTRUCT_RIDE",                 STR_THEMES_WINDOW_CONSTRUCT_RIDE,                 COLOURS_3(opaque(COLOUR_DARK_BROWN),               opaque(COLOUR_BORDEAUX_RED),             opaque(COLOUR_BORDEAUX_RED)                                ) },
        { WindowClass::demolishRidePrompt,          "WC_DEMOLISH_RIDE_PROMPT",           STR_THEMES_WINDOW_DEMOLISH_RIDE_PROMPT,           COLOURS_1(translucent(COLOUR_BORDEAUX_RED)                                                                                                             ) },
        { WindowClass::error,                       "WC_ERROR",                          STR_THEMES_WINDOW_ERROR,                          COLOURS_1(translucent(COLOUR_BORDEAUX_RED)                                                                                                             ) },
        { WindowClass::scenery,                     "WC_SCENERY",                        STR_THEMES_WINDOW_SCENERY,                        COLOURS_3(opaque(COLOUR_DARK_BROWN),               opaque(COLOUR_DARK_GREEN),               opaque(COLOUR_DARK_GREEN)                                  ) },
        { WindowClass::sceneryScatter,              "WC_SCENERY_SCATTER",                STR_THEMES_WINDOW_SCENERY_SCATTER,                COLOURS_2(opaque(COLOUR_DARK_GREEN),               opaque(COLOUR_DARK_GREEN)                                                                           ) },
        { WindowClass::options,                     "WC_OPTIONS",                        STR_THEMES_WINDOW_OPTIONS,                        COLOURS_3(opaque(COLOUR_GREY),                     opaque(COLOUR_LIGHT_BLUE),               opaque(COLOUR_LIGHT_BLUE)                                  ) },
        { WindowClass::assetPacks,                  "WC_ASSET_PACKS",                    STR_ASSET_PACKS,                                  COLOURS_3(opaque(COLOUR_LIGHT_BLUE),               opaque(COLOUR_LIGHT_BLUE),               opaque(COLOUR_LIGHT_BLUE)                                  ) },
        { WindowClass::footpath,                    "WC_FOOTPATH",                       STR_THEMES_WINDOW_FOOTPATH,                       COLOURS_3(opaque(COLOUR_DARK_BROWN),               opaque(COLOUR_DARK_BROWN),               opaque(COLOUR_DARK_BROWN)                                  ) },
        { WindowClass::land,                        "WC_LAND",                           STR_THEMES_WINDOW_LAND,                           COLOURS_3(opaque(COLOUR_DARK_BROWN),               opaque(COLOUR_DARK_BROWN),               opaque(COLOUR_DARK_BROWN)                                  ) },
        { WindowClass::water,                       "WC_WATER",                          STR_THEMES_WINDOW_WATER,                          COLOURS_3(opaque(COLOUR_DARK_BROWN),               opaque(COLOUR_DARK_BROWN),               opaque(COLOUR_DARK_BROWN)                                  ) },
        { WindowClass::peep,                        "WC_PEEP",                           STR_THEMES_WINDOW_PEEP,                           COLOURS_3(opaque(COLOUR_GREY),                     opaque(COLOUR_OLIVE_GREEN),              opaque(COLOUR_OLIVE_GREEN)                                 ) },
        { WindowClass::guestList,                   "WC_GUEST_LIST",                     STR_THEMES_WINDOW_GUEST_LIST,                     COLOURS_3(opaque(COLOUR_GREY),                     opaque(COLOUR_OLIVE_GREEN),              opaque(COLOUR_OLIVE_GREEN)                                 ) },
        { WindowClass::staffList,                   "WC_STAFF_LIST",                     STR_THEMES_WINDOW_STAFF_LIST,                     COLOURS_3(opaque(COLOUR_GREY),                     opaque(COLOUR_LIGHT_PURPLE),             opaque(COLOUR_LIGHT_PURPLE)                                ) },
        { WindowClass::firePrompt,                  "WC_FIRE_PROMPT",                    STR_THEMES_WINDOW_FIRE_PROMPT,                    COLOURS_1(translucent(COLOUR_BORDEAUX_RED)                                                                                                             ) },
        { WindowClass::parkInformation,             "WC_PARK_INFORMATION",               STR_THEMES_WINDOW_PARK_INFORMATION,               COLOURS_3(opaque(COLOUR_GREY),                     opaque(COLOUR_DARK_YELLOW),              opaque(COLOUR_DARK_YELLOW)                                 ) },
        { WindowClass::finances,                    "WC_FINANCES",                       STR_THEMES_WINDOW_FINANCES,                       COLOURS_3(opaque(COLOUR_GREY),                     opaque(COLOUR_DARK_YELLOW),              opaque(COLOUR_DARK_YELLOW)                                 ) },
        { WindowClass::titleMenu,                   "WC_TITLE_MENU",                     STR_THEMES_WINDOW_TITLE_MENU_BUTTONS,             COLOURS_3(translucent(COLOUR_DARK_GREEN),          translucent(COLOUR_DARK_GREEN),          translucent(COLOUR_DARK_GREEN)                             ) },
        { WindowClass::titleExit,                   "WC_TITLE_EXIT",                     STR_THEMES_WINDOW_TITLE_MENU_EXIT,                COLOURS_3(translucent(COLOUR_DARK_GREEN),          translucent(COLOUR_DARK_GREEN),          translucent(COLOUR_DARK_GREEN)                             ) },
        { WindowClass::recentNews,                  "WC_RECENT_NEWS",                    STR_THEMES_WINDOW_RECENT_NEWS,                    COLOURS_3(opaque(COLOUR_DARK_GREEN),               opaque(COLOUR_GREY),                     opaque(COLOUR_BLACK)                                       ) },
        { WindowClass::scenarioSelect,              "WC_SCENARIO_SELECT",                STR_THEMES_WINDOW_TITLE_MENU_SCENARIO_SELECTION,  COLOURS_3(opaque(COLOUR_GREY),                     opaque(COLOUR_BORDEAUX_RED),             opaque(COLOUR_BORDEAUX_RED)                                ) },
        { WindowClass::trackDesignList,             "WC_TRACK_DESIGN_LIST",              STR_THEMES_WINDOW_TRACK_DESIGN_LIST,              COLOURS_3(opaque(COLOUR_BORDEAUX_RED),             opaque(COLOUR_BORDEAUX_RED),             opaque(COLOUR_BORDEAUX_RED)                                ) },
        { WindowClass::trackDesignPlace,            "WC_TRACK_DESIGN_PLACE",             STR_THEMES_WINDOW_TRACK_DESIGN_PLACE,             COLOURS_3(opaque(COLOUR_DARK_BROWN),               opaque(COLOUR_DARK_BROWN),               opaque(COLOUR_DARK_BROWN)                                  ) },
        { WindowClass::newCampaign,                 "WC_NEW_CAMPAIGN",                   STR_THEMES_WINDOW_NEW_CAMPAIGN,                   COLOURS_3(opaque(COLOUR_DARK_YELLOW),              opaque(COLOUR_DARK_YELLOW),              opaque(COLOUR_DARK_YELLOW)                                 ) },
        { WindowClass::keyboardShortcutList,        "WC_KEYBOARD_SHORTCUT_LIST",         STR_THEMES_WINDOW_KEYBOARD_SHORTCUT_LIST,         COLOURS_3(opaque(COLOUR_LIGHT_BLUE),               opaque(COLOUR_LIGHT_BLUE),               opaque(COLOUR_LIGHT_BLUE)                                  ) },
        { WindowClass::changeKeyboardShortcut,      "WC_CHANGE_KEYBOARD_SHORTCUT",       STR_THEMES_WINDOW_CHANGE_KEYBOARD_SHORTCUT,       COLOURS_3(opaque(COLOUR_LIGHT_BLUE),               opaque(COLOUR_LIGHT_BLUE),               opaque(COLOUR_LIGHT_BLUE)                                  ) },
        { WindowClass::resetShortcutKeysPrompt,     "WC_RESET_SHORTCUT_KEYS_PROMPT",     STR_SHORTCUT_ACTION_RESET,                        COLOURS_1(translucent(COLOUR_BORDEAUX_RED)                                                                                                             ) },
        { WindowClass::map,                         "WC_MAP",                            STR_THEMES_WINDOW_MAP,                            COLOURS_2(opaque(COLOUR_DARK_GREEN),               opaque(COLOUR_DARK_BROWN)                                                                           ) },
        { WindowClass::banner,                      "WC_BANNER",                         STR_THEMES_WINDOW_BANNER,                         COLOURS_3(opaque(COLOUR_DARK_BROWN),               opaque(COLOUR_DARK_BROWN),               opaque(COLOUR_DARK_BROWN)                                  ) },
        { WindowClass::editorObjectSelection,       "WC_EDITOR_OBJECT_SELECTION",        STR_THEMES_WINDOW_EDITOR_OBJECT_SELECTION,        COLOURS_3(opaque(COLOUR_LIGHT_PURPLE),             opaque(COLOUR_GREY),                     opaque(COLOUR_GREY)                                        ) },
        { WindowClass::editorInventionList,         "WC_EDITOR_INVENTION_LIST",          STR_THEMES_WINDOW_EDITOR_INVENTION_LIST,          COLOURS_3(opaque(COLOUR_LIGHT_PURPLE),             opaque(COLOUR_GREY),                     opaque(COLOUR_GREY)                                        ) },
        { WindowClass::editorScenarioOptions,       "WC_EDITOR_SCENARIO_OPTIONS",        STR_THEMES_WINDOW_EDITOR_SCENARIO_OPTIONS,        COLOURS_3(opaque(COLOUR_LIGHT_PURPLE),             opaque(COLOUR_GREY),                     opaque(COLOUR_GREY)                                        ) },
        { WindowClass::manageTrackDesign,           "WC_MANAGE_TRACK_DESIGN",            STR_THEMES_WINDOW_MANAGE_TRACK_DESIGN,            COLOURS_3(opaque(COLOUR_GREY),                     opaque(COLOUR_GREY),                     opaque(COLOUR_GREY)                                        ) },
        { WindowClass::trackDeletePrompt,           "WC_TRACK_DELETE_PROMPT",            STR_THEMES_WINDOW_TRACK_DELETE_PROMPT,            COLOURS_3(opaque(COLOUR_BORDEAUX_RED),             opaque(COLOUR_BORDEAUX_RED),             opaque(COLOUR_BORDEAUX_RED)                                ) },
        { WindowClass::installTrack,                "WC_INSTALL_TRACK",                  STR_THEMES_WINDOW_INSTALL_TRACK,                  COLOURS_3(opaque(COLOUR_BORDEAUX_RED),             opaque(COLOUR_BORDEAUX_RED),             opaque(COLOUR_BORDEAUX_RED)                                ) },
        { WindowClass::clearScenery,                "WC_CLEAR_SCENERY",                  STR_THEMES_WINDOW_CLEAR_SCENERY,                  COLOURS_3(opaque(COLOUR_DARK_BROWN),               opaque(COLOUR_DARK_BROWN),               opaque(COLOUR_DARK_BROWN)                                  ) },
        { WindowClass::cheats,                      "WC_CHEATS",                         STR_CHEAT_TITLE,                                  COLOURS_2(opaque(COLOUR_GREY),                     opaque(COLOUR_DARK_YELLOW)                                                                          ) },
        { WindowClass::research,                    "WC_RESEARCH",                       STR_THEMES_WINDOW_RESEARCH,                       COLOURS_3(opaque(COLOUR_GREY),                     opaque(COLOUR_DARK_YELLOW),              opaque(COLOUR_DARK_YELLOW)                                 ) },
        { WindowClass::viewport,                    "WC_VIEWPORT",                       STR_THEMES_WINDOW_VIEWPORT,                       COLOURS_3(opaque(COLOUR_DARK_BROWN),               opaque(COLOUR_DARK_BROWN),               opaque(COLOUR_DARK_BROWN)                                  ) },
        { WindowClass::mapgen,                      "WC_MAPGEN",                         STR_THEMES_WINDOW_MAPGEN,                         COLOURS_3(opaque(COLOUR_DARK_GREEN),               opaque(COLOUR_DARK_BROWN),               opaque(COLOUR_DARK_BROWN)                                  ) },
        { WindowClass::loadsave,                    "WC_LOADSAVE",                       STR_THEMES_WINDOW_LOADSAVE,                       COLOURS_3(opaque(COLOUR_DARK_GREEN),               opaque(COLOUR_DARK_GREEN),               opaque(COLOUR_DARK_GREEN)                                  ) },
        { WindowClass::loadsaveOverwritePrompt,     "WC_LOADSAVE_OVERWRITE_PROMPT",      STR_THEMES_WINDOW_LOADSAVE_OVERWRITE_PROMPT,      COLOURS_1(translucent(COLOUR_BORDEAUX_RED)                                                                                                             ) },
        { WindowClass::titleOptions,                "WC_TITLE_OPTIONS",                  STR_THEMES_WINDOW_TITLE_MENU_OPTIONS,             COLOURS_3(translucent(COLOUR_DARK_GREEN),          translucent(COLOUR_DARK_GREEN),          translucent(COLOUR_DARK_GREEN)                             ) },
        { WindowClass::landRights,                  "WC_LAND_RIGHTS",                    STR_THEMES_WINDOW_LAND_RIGHTS,                    COLOURS_3(opaque(COLOUR_DARK_YELLOW),              opaque(COLOUR_DARK_YELLOW),              opaque(COLOUR_DARK_YELLOW)                                 ) },
        { WindowClass::themes,                      "WC_THEMES",                         STR_THEMES_WINDOW_THEMES,                         COLOURS_3(opaque(COLOUR_GREY),                     opaque(COLOUR_DARK_GREEN),               opaque(COLOUR_DARK_GREEN)                                  ) },
        { WindowClass::staff,                       "WC_STAFF",                          STR_THEMES_WINDOW_STAFF,                          COLOURS_3(opaque(COLOUR_GREY),                     opaque(COLOUR_LIGHT_PURPLE),             opaque(COLOUR_LIGHT_PURPLE)                                ) },
        { WindowClass::editorTrackBottomToolbar,    "WC_EDITOR_TRACK_BOTTOM_TOOLBAR",    STR_THEMES_WINDOW_BOTTOM_TOOLBAR_TRACK_EDITOR,    COLOURS_3(translucent(COLOUR_LIGHT_BLUE),          translucent(COLOUR_LIGHT_BLUE),          translucent(COLOUR_LIGHT_BLUE)                             ) },
        { WindowClass::editorScenarioBottomToolbar, "WC_EDITOR_SCENARIO_BOTTOM_TOOLBAR", STR_THEMES_WINDOW_BOTTOM_TOOLBAR_SCENARIO_EDITOR, COLOURS_3(translucent(COLOUR_LIGHT_BROWN),         translucent(COLOUR_LIGHT_BROWN),         translucent(COLOUR_MOSS_GREEN)                             ) },
        { WindowClass::tileInspector,               "WC_TILE_INSPECTOR",                 STR_TILE_INSPECTOR_TITLE,                         COLOURS_2(opaque(COLOUR_LIGHT_BLUE),               opaque(COLOUR_LIGHT_BLUE)                                                                           ) },
        { WindowClass::viewClipping,                "WC_VIEW_CLIPPING",                  STR_VIEW_CLIPPING_TITLE,                          COLOURS_1(opaque(COLOUR_DARK_GREEN)                                                                                                                    ) },
        { WindowClass::patrolArea,                  "WC_PATROL_AREA",                    STR_SET_PATROL_AREA,                              COLOURS_3(opaque(COLOUR_LIGHT_PURPLE),             opaque(COLOUR_LIGHT_PURPLE),             opaque(COLOUR_LIGHT_PURPLE)                                ) },
        { WindowClass::transparency,                "WC_TRANSPARENCY",                   STR_TRANSPARENCY_OPTIONS_TITLE,                   COLOURS_3(opaque(COLOUR_LIGHT_BLUE),               opaque(COLOUR_LIGHT_BLUE),               opaque(COLOUR_LIGHT_BLUE)                                  ) },
        { WindowClass::about,                       "WC_ABOUT",                          STR_ABOUT,                                        COLOURS_2(opaque(COLOUR_GREY),                     opaque(COLOUR_LIGHT_BLUE)                                                                           ) },
        { WindowClass::changelog,                   "WC_CHANGELOG",                      STR_CHANGELOG_TITLE,                              COLOURS_2(opaque(COLOUR_LIGHT_BLUE),               opaque(COLOUR_LIGHT_BLUE)                                                                           ) },
        { WindowClass::multiplayer,                 "WC_MULTIPLAYER",                    STR_MULTIPLAYER,                                  COLOURS_3(opaque(COLOUR_LIGHT_BLUE),               opaque(COLOUR_LIGHT_BLUE),               opaque(COLOUR_LIGHT_BLUE)                                  ) },
        { WindowClass::player,                      "WC_PLAYER",                         STR_THEMES_WINDOW_PLAYER,                         COLOURS_3(opaque(COLOUR_LIGHT_BLUE),               opaque(COLOUR_LIGHT_BLUE),               opaque(COLOUR_LIGHT_BLUE)                                  ) },
        { WindowClass::networkStatus,               "WC_NETWORK_STATUS",                 STR_THEMES_WINDOW_NETWORK_STATUS,                 COLOURS_1(opaque(COLOUR_BLACK)                                                                                                                         ) },
        { WindowClass::serverList,                  "WC_SERVER_LIST",                    STR_SERVER_LIST,                                  COLOURS_2(opaque(COLOUR_LIGHT_BLUE),               opaque(COLOUR_LIGHT_BLUE)                                                                           ) },
        { WindowClass::chat,                        "WC_CHAT",                           STR_CHAT,                                         COLOURS_1(translucent(COLOUR_GREY)                                                                                                                     ) },
        { WindowClass::console,                     "WC_CONSOLE",                        STR_CONSOLE,                                      COLOURS_2(translucent(COLOUR_LIGHT_BLUE),          opaque(COLOUR_WHITE)                                                                                ) },
        { WindowClass::progressWindow,              "WC_PROGRESS_WINDOW",                STR_THEME_LOADING_WINDOW,                         COLOURS_1(opaque(COLOUR_BLACK)                                                                                                                         ) },
        { WindowClass::editorParkEntrance,          "WC_EDITOR_PARK_ENTRANCE",           STR_OBJECT_SELECTION_PARK_ENTRANCE,               COLOURS_2(opaque(COLOUR_DARK_GREEN),               opaque(COLOUR_DARK_BROWN)                                                                           ) },
    };

#pragma endregion

#pragma region Pre-defined Themes

    static constexpr std::array kPredefinedThemeRCT1Entries = std::to_array<UIThemeWindowEntry>({
        { WindowClass::topToolbar,             { opaque(COLOUR_GREY),             opaque(COLOUR_GREY),             opaque(COLOUR_GREY),                opaque(COLOUR_GREY),     opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
        { WindowClass::bottomToolbar,          { translucent(COLOUR_GREY),        translucent(COLOUR_GREY),        opaque(COLOUR_VOID),                opaque(COLOUR_YELLOW),   opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
        { WindowClass::ride,                   { opaque(COLOUR_BORDEAUX_RED),     opaque(COLOUR_GREY),             opaque(COLOUR_SATURATED_GREEN),     opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
        { WindowClass::rideList,               { opaque(COLOUR_BORDEAUX_RED),     opaque(COLOUR_GREY),             opaque(COLOUR_GREY),                opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
        { WindowClass::constructRide,          { opaque(COLOUR_BORDEAUX_RED),     opaque(COLOUR_GREY),             opaque(COLOUR_GREY),                opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
        { WindowClass::peep,                   { opaque(COLOUR_LIGHT_BROWN),      opaque(COLOUR_BORDEAUX_RED),     opaque(COLOUR_BORDEAUX_RED),        opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
        { WindowClass::guestList,              { opaque(COLOUR_LIGHT_BROWN),      opaque(COLOUR_BORDEAUX_RED),     opaque(COLOUR_BORDEAUX_RED),        opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
        { WindowClass::staffList,              { opaque(COLOUR_DARK_GREEN),       opaque(COLOUR_LIGHT_PURPLE),     opaque(COLOUR_LIGHT_PURPLE),        opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
        { WindowClass::finances,               { opaque(COLOUR_LIGHT_PURPLE),     opaque(COLOUR_GREY),             opaque(COLOUR_GREY),                opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
        { WindowClass::titleMenu,              { translucent(COLOUR_GREY),        translucent(COLOUR_GREY),        translucent(COLOUR_GREY),           opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
        { WindowClass::titleExit,              { translucent(COLOUR_GREY),        translucent(COLOUR_GREY),        translucent(COLOUR_GREY),           opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
        { WindowClass::newCampaign,            { opaque(COLOUR_LIGHT_PURPLE),     opaque(COLOUR_LIGHT_PURPLE),     opaque(COLOUR_GREY),                opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
        { WindowClass::titleOptions,           { translucent(COLOUR_GREY),        translucent(COLOUR_GREY),        translucent(COLOUR_GREY),           opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
        { WindowClass::staff,                  { opaque(COLOUR_DARK_GREEN),       opaque(COLOUR_LIGHT_PURPLE),     opaque(COLOUR_LIGHT_PURPLE),        opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
        { WindowClass::options,                { opaque(COLOUR_GREY),             opaque(COLOUR_DARK_BROWN),       opaque(COLOUR_DARK_BROWN),          opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
        { WindowClass::assetPacks,             { opaque(COLOUR_DARK_BROWN),       opaque(COLOUR_DARK_BROWN),       opaque(COLOUR_DARK_BROWN),          opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
        { WindowClass::keyboardShortcutList,   { opaque(COLOUR_GREY),             opaque(COLOUR_DARK_BROWN),       opaque(COLOUR_DARK_BROWN),          opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
        { WindowClass::changeKeyboardShortcut, { opaque(COLOUR_DARK_BROWN),       opaque(COLOUR_DARK_BROWN),       opaque(COLOUR_DARK_BROWN),          opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
        { WindowClass::recentNews,             { opaque(COLOUR_DARK_BROWN),       opaque(COLOUR_GREY),             opaque(COLOUR_GREY),                opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
        { WindowClass::trackDesignList,        { opaque(COLOUR_DARK_BROWN),       opaque(COLOUR_DARK_BROWN),       opaque(COLOUR_DARK_BROWN),          opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
        { WindowClass::map,                    { opaque(COLOUR_DARK_BROWN),       opaque(COLOUR_GREY),             opaque(COLOUR_GREY),                opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
        { WindowClass::about,                  { opaque(COLOUR_GREY),             opaque(COLOUR_DARK_BROWN),       opaque(COLOUR_WHITE),               opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
        { WindowClass::changelog,              { opaque(COLOUR_DARK_BROWN),       opaque(COLOUR_DARK_BROWN),       opaque(COLOUR_WHITE),               opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK),    opaque(COLOUR_BLACK) } },
    });
    // clang-format on

    static constexpr std::array<UIThemeWindowEntry, 0> kPredefinedThemeRCT2Entries = {};

    const UITheme kPredefinedThemeRCT1 = UITheme::CreatePredefined(
        "*RCT1", kPredefinedThemeRCT1Entries,
        UITHEME_FLAG_USE_LIGHTS_RIDE | UITHEME_FLAG_USE_LIGHTS_PARK | UITHEME_FLAG_USE_ALTERNATIVE_SCENARIO_SELECT_FONT
            | UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR);

    const UITheme kPredefinedThemeRCT2 = UITheme::CreatePredefined("*RCT2", kPredefinedThemeRCT2Entries, 0);

    struct PredefinedTheme
    {
        const UITheme* Theme;
        StringId Name;
    };

    static constexpr PredefinedTheme kPredefinedThemes[] = {
        { &kPredefinedThemeRCT1, STR_TITLE_SEQUENCE_RCT1 },
        { &kPredefinedThemeRCT2, STR_TITLE_SEQUENCE_RCT2 },
    };

#pragma endregion

    static const WindowThemeDesc* GetWindowThemeDescriptor(WindowClass windowClass)
    {
        for (const auto& desc : kWindowThemeDescriptors)
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
        for (const auto& desc : kWindowThemeDescriptors)
        {
            if (String::equals(desc.WindowClassSZ, windowClassSZ))
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
            json_t jsonEntry = { { "colour", Colour::ToString(Theme.Colours[i].colour) },
                                 { "translucent", Theme.Colours[i].hasFlag(ColourFlag::translucent) } };

            jsonColours.emplace_back(jsonEntry);
        }

        json_t colourSettingsEntry = {
            { "colours", jsonColours },
        };

        return colourSettingsEntry;
    }

    UIThemeWindowEntry UIThemeWindowEntry::FromJson(const WindowThemeDesc* wtDesc, json_t& jsonData, uint8_t version)
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
            if (version == 0)
            {
                auto number = Json::GetNumber<uint8_t>(jsonColours[i]);
                result.Theme.Colours[i] = ColourWithFlags::fromLegacy(number);
            }
            else
            {
                auto colourObject = Json::AsObject(jsonColours[i]);
                auto colour = Colour::FromString(Json::GetString(colourObject["colour"]), COLOUR_BLACK);
                auto isTranslucent = Json::GetBoolean(colourObject["translucent"], false);
                uint8_t flags = isTranslucent ? EnumToFlag(ColourFlag::translucent) : 0;

                result.Theme.Colours[i] = { colour, flags };
            }
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
            { "version", kCurrentThemeVersion },
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
        auto version = Json::GetNumber<uint8_t>(jsonObj["version"], 0);

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

                        UIThemeWindowEntry entry = UIThemeWindowEntry::FromJson(wtDesc, jsonValue, version);
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

    UITheme UITheme::CreatePredefined(const std::string& name, std::span<const UIThemeWindowEntry> entries, uint8_t flags)
    {
        auto theme = UITheme(name);
        theme.Flags = flags | UITHEME_FLAG_PREDEFINED;
        theme.Entries.assign(entries.begin(), entries.end());
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
            for (auto predefinedTheme : kPredefinedThemes)
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
                theme = const_cast<UITheme*>(&kPredefinedThemeRCT2);
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
                if (String::equals(name, theme.Name))
                {
                    if (theme.Path.empty())
                    {
                        LoadTheme(const_cast<UITheme*>(kPredefinedThemes[i].Theme));
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
            LoadTheme(const_cast<UITheme*>(&kPredefinedThemeRCT2));
            ActiveAvailableThemeIndex = 1;

            bool configValid = false;
            if (!Config::Get().interface.currentThemePreset.empty())
            {
                if (LoadThemeByConfigName(Config::Get().interface.currentThemePreset.c_str()))
                {
                    configValid = true;
                }
            }

            if (!configValid)
            {
                Config::Get().interface.currentThemePreset = ThemeManagerGetAvailableThemeConfigName(1);
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
            auto& env = context->GetPlatformEnvironment();
            return env.GetDirectoryPath(DirBase::user, DirId::themes);
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
            return LanguageGetString(kPredefinedThemes[index].Name);
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
            ThemeManager::LoadTheme(const_cast<UITheme*>(kPredefinedThemes[index].Theme));
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
        Config::Get().interface.currentThemePreset = ThemeManagerGetAvailableThemeConfigName(index);

        ColourSchemeUpdateAll();
    }

    size_t ThemeGetIndexForName(const utf8* name)
    {
        size_t count = ThemeManager::AvailableThemes.size();
        for (size_t i = 0; i < count; i++)
        {
            const utf8* tn = ThemeManagerGetAvailableThemeName(i);
            if (String::iequals(tn, name))
            {
                return i;
            }
        }
        return SIZE_MAX;
    }

    ColourWithFlags ThemeGetColour(WindowClass wc, uint8_t index)
    {
        const UIThemeWindowEntry* entry = ThemeManager::CurrentTheme->GetEntry(wc);
        if (entry == nullptr)
        {
            const WindowThemeDesc* desc = GetWindowThemeDescriptor(wc);
            if (desc == nullptr)
            {
                return {};
            }
            return desc->DefaultTheme.Colours[index];
        }

        return entry->Theme.Colours[index];
    }

    void ThemeSetColour(WindowClass wc, uint8_t index, ColourWithFlags colour)
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
                Config::Get().interface.currentThemePreset = ThemeManagerGetAvailableThemeConfigName(1);
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
                Config::Get().interface.currentThemePreset = ThemeManagerGetAvailableThemeConfigName(i);
                break;
            }
        }
    }

    void ThemeDelete()
    {
        File::Delete(ThemeManager::CurrentThemePath);
        ThemeManager::LoadTheme(const_cast<UITheme*>(&kPredefinedThemeRCT2));
        ThemeManager::ActiveAvailableThemeIndex = 1;
        Config::Get().interface.currentThemePreset = ThemeManagerGetAvailableThemeConfigName(1);
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
            return kStringIdEmpty;
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
        window->flags |= WindowFlag::transparent;
    }
} // namespace OpenRCT2::Ui
