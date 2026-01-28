/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <openrct2/interface/ColourWithFlags.h>
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

    struct VariableWindowColours
    {
        uint8_t numColours{};
        WindowTheme defaultTheme{};

        constexpr VariableWindowColours() = default;

        constexpr VariableWindowColours(ColourWithFlags colour1)
        {
            numColours = 1;
            defaultTheme.Colours[0] = colour1;
        }

        constexpr VariableWindowColours(ColourWithFlags colour1, ColourWithFlags colour2)
        {
            numColours = 2;
            defaultTheme.Colours[0] = colour1;
            defaultTheme.Colours[1] = colour2;
        }

        constexpr VariableWindowColours(ColourWithFlags colour1, ColourWithFlags colour2, ColourWithFlags colour3)
        {
            numColours = 3;
            defaultTheme.Colours[0] = colour1;
            defaultTheme.Colours[1] = colour2;
            defaultTheme.Colours[2] = colour3;
        }

        constexpr VariableWindowColours(
            ColourWithFlags colour1, ColourWithFlags colour2, ColourWithFlags colour3, ColourWithFlags colour4)
        {
            numColours = 4;
            defaultTheme.Colours[0] = colour1;
            defaultTheme.Colours[1] = colour2;
            defaultTheme.Colours[2] = colour3;
            defaultTheme.Colours[3] = colour4;
        }
    };

    /**
     * Represents the theme descriptor for a specific window type including the default colour scheme.
     */
    struct WindowThemeDesc
    {
        ::WindowClass WindowClass;
        const utf8* WindowClassSZ;
        StringId WindowName;
        VariableWindowColours windowColours;
    };

#pragma region Window Theme Descriptors

    // clang-format off

    static constexpr ColourWithFlags opaque(Drawing::Colour colour)
    {
        return ColourWithFlags{ colour };
    }
    static constexpr ColourWithFlags translucent(Drawing::Colour colour)
    {
    return ColourWithFlags{ colour, { ColourFlag::translucent } };
    }

    static constexpr WindowThemeDesc kWindowThemeDescriptors[] =
    {
        // WindowClass                              WindowClassSZ                        WindowName                                        windowColours
        { WindowClass::topToolbar,                  "WC_TOP_TOOLBAR",                    STR_THEMES_WINDOW_TOP_TOOLBAR,                    { opaque(Drawing::Colour::lightBlue),               opaque(Drawing::Colour::darkGreen),               opaque(Drawing::Colour::darkBrown),             opaque(Drawing::Colour::grey)         } },
        { WindowClass::bottomToolbar,               "WC_BOTTOM_TOOLBAR",                 STR_THEMES_WINDOW_BOTTOM_TOOLBAR,                 { translucent(Drawing::Colour::darkGreen),          opaque(Drawing::Colour::black),                    opaque(Drawing::Colour::black),                  opaque(Drawing::Colour::brightGreen) } },
        { WindowClass::ride,                        "WC_RIDE",                           STR_THEMES_WINDOW_RIDE,                           { opaque(Drawing::Colour::grey),                     opaque(Drawing::Colour::bordeauxRed),             opaque(Drawing::Colour::saturatedGreen)                                     } },
        { WindowClass::rideConstruction,            "WC_RIDE_CONSTRUCTION",              STR_THEMES_WINDOW_RIDE_CONSTRUCTION,              { opaque(Drawing::Colour::darkBrown),               opaque(Drawing::Colour::darkBrown),               opaque(Drawing::Colour::darkBrown)                                          } },
        { WindowClass::rideList,                    "WC_RIDE_LIST",                      STR_THEMES_WINDOW_RIDE_LIST,                      { opaque(Drawing::Colour::grey),                     opaque(Drawing::Colour::bordeauxRed),             opaque(Drawing::Colour::bordeauxRed)                                        } },
        { WindowClass::savePrompt,                  "WC_SAVE_PROMPT",                    STR_THEMES_WINDOW_SAVE_PROMPT,                    { translucent(Drawing::Colour::bordeauxRed)                                                                                                                     } },
        { WindowClass::constructRide,               "WC_CONSTRUCT_RIDE",                 STR_THEMES_WINDOW_CONSTRUCT_RIDE,                 { opaque(Drawing::Colour::darkBrown),               opaque(Drawing::Colour::bordeauxRed),             opaque(Drawing::Colour::bordeauxRed)                                        } },
        { WindowClass::demolishRidePrompt,          "WC_DEMOLISH_RIDE_PROMPT",           STR_THEMES_WINDOW_DEMOLISH_RIDE_PROMPT,           { translucent(Drawing::Colour::bordeauxRed)                                                                                                                     } },
        { WindowClass::error,                       "WC_ERROR",                          STR_THEMES_WINDOW_ERROR,                          { translucent(Drawing::Colour::bordeauxRed)                                                                                                                     } },
        { WindowClass::scenery,                     "WC_SCENERY",                        STR_THEMES_WINDOW_SCENERY,                        { opaque(Drawing::Colour::darkBrown),               opaque(Drawing::Colour::darkGreen),               opaque(Drawing::Colour::darkGreen)                                          } },
        { WindowClass::sceneryScatter,              "WC_SCENERY_SCATTER",                STR_THEMES_WINDOW_SCENERY_SCATTER,                { opaque(Drawing::Colour::darkGreen),               opaque(Drawing::Colour::darkGreen)                                                                                   } },
        { WindowClass::options,                     "WC_OPTIONS",                        STR_THEMES_WINDOW_OPTIONS,                        { opaque(Drawing::Colour::grey),                     opaque(Drawing::Colour::lightBlue),               opaque(Drawing::Colour::lightBlue)                                          } },
        { WindowClass::assetPacks,                  "WC_ASSET_PACKS",                    STR_ASSET_PACKS,                                  { opaque(Drawing::Colour::lightBlue),               opaque(Drawing::Colour::lightBlue),               opaque(Drawing::Colour::lightBlue)                                          } },
        { WindowClass::footpath,                    "WC_FOOTPATH",                       STR_THEMES_WINDOW_FOOTPATH,                       { opaque(Drawing::Colour::darkBrown),               opaque(Drawing::Colour::darkBrown),               opaque(Drawing::Colour::darkBrown)                                          } },
        { WindowClass::land,                        "WC_LAND",                           STR_THEMES_WINDOW_LAND,                           { opaque(Drawing::Colour::darkBrown),               opaque(Drawing::Colour::darkBrown),               opaque(Drawing::Colour::darkBrown)                                          } },
        { WindowClass::water,                       "WC_WATER",                          STR_THEMES_WINDOW_WATER,                          { opaque(Drawing::Colour::darkBrown),               opaque(Drawing::Colour::darkBrown),               opaque(Drawing::Colour::darkBrown)                                          } },
        { WindowClass::peep,                        "WC_PEEP",                           STR_THEMES_WINDOW_PEEP,                           { opaque(Drawing::Colour::grey),                     opaque(Drawing::Colour::oliveGreen),              opaque(Drawing::Colour::oliveGreen)                                         } },
        { WindowClass::guestList,                   "WC_GUEST_LIST",                     STR_THEMES_WINDOW_GUEST_LIST,                     { opaque(Drawing::Colour::grey),                     opaque(Drawing::Colour::oliveGreen),              opaque(Drawing::Colour::oliveGreen)                                         } },
        { WindowClass::staffList,                   "WC_STAFF_LIST",                     STR_THEMES_WINDOW_STAFF_LIST,                     { opaque(Drawing::Colour::grey),                     opaque(Drawing::Colour::lightPurple),             opaque(Drawing::Colour::lightPurple)                                        } },
        { WindowClass::firePrompt,                  "WC_FIRE_PROMPT",                    STR_THEMES_WINDOW_FIRE_PROMPT,                    { translucent(Drawing::Colour::bordeauxRed)                                                                                                                     } },
        { WindowClass::parkInformation,             "WC_PARK_INFORMATION",               STR_THEMES_WINDOW_PARK_INFORMATION,               { opaque(Drawing::Colour::grey),                     opaque(Drawing::Colour::darkYellow),              opaque(Drawing::Colour::darkYellow)                                         } },
        { WindowClass::finances,                    "WC_FINANCES",                       STR_THEMES_WINDOW_FINANCES,                       { opaque(Drawing::Colour::grey),                     opaque(Drawing::Colour::darkYellow),              opaque(Drawing::Colour::darkYellow)                                         } },
        { WindowClass::titleMenu,                   "WC_TITLE_MENU",                     STR_THEMES_WINDOW_TITLE_MENU_BUTTONS,             { translucent(Drawing::Colour::darkGreen),          translucent(Drawing::Colour::darkGreen),          translucent(Drawing::Colour::darkGreen)                                     } },
        { WindowClass::titleExit,                   "WC_TITLE_EXIT",                     STR_THEMES_WINDOW_TITLE_MENU_EXIT,                { translucent(Drawing::Colour::darkGreen),          translucent(Drawing::Colour::darkGreen),          translucent(Drawing::Colour::darkGreen)                                     } },
        { WindowClass::recentNews,                  "WC_RECENT_NEWS",                    STR_THEMES_WINDOW_RECENT_NEWS,                    { opaque(Drawing::Colour::darkGreen),               opaque(Drawing::Colour::grey),                     opaque(Drawing::Colour::black),                  opaque(Drawing::Colour::black)        } },
        { WindowClass::scenarioSelect,              "WC_SCENARIO_SELECT",                STR_THEMES_WINDOW_TITLE_MENU_SCENARIO_SELECTION,  { opaque(Drawing::Colour::grey),                     opaque(Drawing::Colour::bordeauxRed),             opaque(Drawing::Colour::bordeauxRed)                                        } },
        { WindowClass::trackDesignList,             "WC_TRACK_DESIGN_LIST",              STR_THEMES_WINDOW_TRACK_DESIGN_LIST,              { opaque(Drawing::Colour::bordeauxRed),             opaque(Drawing::Colour::bordeauxRed),             opaque(Drawing::Colour::bordeauxRed)                                        } },
        { WindowClass::trackDesignPlace,            "WC_TRACK_DESIGN_PLACE",             STR_THEMES_WINDOW_TRACK_DESIGN_PLACE,             { opaque(Drawing::Colour::darkBrown),               opaque(Drawing::Colour::darkBrown),               opaque(Drawing::Colour::darkBrown)                                          } },
        { WindowClass::newCampaign,                 "WC_NEW_CAMPAIGN",                   STR_THEMES_WINDOW_NEW_CAMPAIGN,                   { opaque(Drawing::Colour::darkYellow),              opaque(Drawing::Colour::darkYellow),              opaque(Drawing::Colour::darkYellow)                                         } },
        { WindowClass::keyboardShortcutList,        "WC_KEYBOARD_SHORTCUT_LIST",         STR_THEMES_WINDOW_KEYBOARD_SHORTCUT_LIST,         { opaque(Drawing::Colour::lightBlue),               opaque(Drawing::Colour::lightBlue)                                                                                   } },
        { WindowClass::changeKeyboardShortcut,      "WC_CHANGE_KEYBOARD_SHORTCUT",       STR_THEMES_WINDOW_CHANGE_KEYBOARD_SHORTCUT,       { opaque(Drawing::Colour::lightBlue),               opaque(Drawing::Colour::lightBlue),               opaque(Drawing::Colour::lightBlue)                                          } },
        { WindowClass::resetShortcutKeysPrompt,     "WC_RESET_SHORTCUT_KEYS_PROMPT",     STR_SHORTCUT_ACTION_RESET,                        { translucent(Drawing::Colour::bordeauxRed)                                                                                                                     } },
        { WindowClass::map,                         "WC_MAP",                            STR_THEMES_WINDOW_MAP,                            { opaque(Drawing::Colour::darkGreen),               opaque(Drawing::Colour::darkBrown)                                                                                   } },
        { WindowClass::banner,                      "WC_BANNER",                         STR_THEMES_WINDOW_BANNER,                         { opaque(Drawing::Colour::darkBrown),               opaque(Drawing::Colour::darkBrown),               opaque(Drawing::Colour::darkBrown)                                          } },
        { WindowClass::editorObjectSelection,       "WC_EDITOR_OBJECT_SELECTION",        STR_THEMES_WINDOW_EDITOR_OBJECT_SELECTION,        { opaque(Drawing::Colour::lightPurple),             opaque(Drawing::Colour::grey),                     opaque(Drawing::Colour::grey)                                                } },
        { WindowClass::editorInventionList,         "WC_EDITOR_INVENTION_LIST",          STR_THEMES_WINDOW_EDITOR_INVENTION_LIST,          { opaque(Drawing::Colour::lightPurple),             opaque(Drawing::Colour::grey),                     opaque(Drawing::Colour::grey)                                                } },
        { WindowClass::editorScenarioOptions,       "WC_EDITOR_SCENARIO_OPTIONS",        STR_THEMES_WINDOW_EDITOR_SCENARIO_OPTIONS,        { opaque(Drawing::Colour::lightPurple),             opaque(Drawing::Colour::grey),                     opaque(Drawing::Colour::grey)                                                } },
        { WindowClass::manageTrackDesign,           "WC_MANAGE_TRACK_DESIGN",            STR_THEMES_WINDOW_MANAGE_TRACK_DESIGN,            { opaque(Drawing::Colour::grey),                     opaque(Drawing::Colour::grey),                     opaque(Drawing::Colour::grey)                                                } },
        { WindowClass::trackDeletePrompt,           "WC_TRACK_DELETE_PROMPT",            STR_THEMES_WINDOW_TRACK_DELETE_PROMPT,            { opaque(Drawing::Colour::bordeauxRed),             opaque(Drawing::Colour::bordeauxRed),             opaque(Drawing::Colour::bordeauxRed)                                        } },
        { WindowClass::installTrack,                "WC_INSTALL_TRACK",                  STR_THEMES_WINDOW_INSTALL_TRACK,                  { opaque(Drawing::Colour::bordeauxRed),             opaque(Drawing::Colour::bordeauxRed),             opaque(Drawing::Colour::bordeauxRed)                                        } },
        { WindowClass::clearScenery,                "WC_CLEAR_SCENERY",                  STR_THEMES_WINDOW_CLEAR_SCENERY,                  { opaque(Drawing::Colour::darkBrown),               opaque(Drawing::Colour::darkBrown),               opaque(Drawing::Colour::darkBrown)                                          } },
        { WindowClass::cheats,                      "WC_CHEATS",                         STR_CHEAT_TITLE,                                  { opaque(Drawing::Colour::grey),                     opaque(Drawing::Colour::darkYellow)                                                                                  } },
        { WindowClass::research,                    "WC_RESEARCH",                       STR_THEMES_WINDOW_RESEARCH,                       { opaque(Drawing::Colour::grey),                     opaque(Drawing::Colour::darkYellow),              opaque(Drawing::Colour::darkYellow)                                         } },
        { WindowClass::viewport,                    "WC_VIEWPORT",                       STR_THEMES_WINDOW_VIEWPORT,                       { opaque(Drawing::Colour::darkBrown),               opaque(Drawing::Colour::darkBrown),               opaque(Drawing::Colour::darkBrown)                                          } },
        { WindowClass::mapgen,                      "WC_MAPGEN",                         STR_THEMES_WINDOW_MAPGEN,                         { opaque(Drawing::Colour::darkGreen),               opaque(Drawing::Colour::darkBrown),               opaque(Drawing::Colour::darkBrown)                                          } },
        { WindowClass::loadsave,                    "WC_LOADSAVE",                       STR_THEMES_WINDOW_LOADSAVE,                       { opaque(Drawing::Colour::darkGreen),               opaque(Drawing::Colour::darkGreen),               opaque(Drawing::Colour::darkGreen)                                          } },
        { WindowClass::loadsaveOverwritePrompt,     "WC_LOADSAVE_OVERWRITE_PROMPT",      STR_THEMES_WINDOW_LOADSAVE_OVERWRITE_PROMPT,      { translucent(Drawing::Colour::bordeauxRed)                                                                                                                     } },
        { WindowClass::titleOptions,                "WC_TITLE_OPTIONS",                  STR_THEMES_WINDOW_TITLE_MENU_OPTIONS,             { translucent(Drawing::Colour::darkGreen),          translucent(Drawing::Colour::darkGreen),          translucent(Drawing::Colour::darkGreen)                                     } },
        { WindowClass::landRights,                  "WC_LAND_RIGHTS",                    STR_THEMES_WINDOW_LAND_RIGHTS,                    { opaque(Drawing::Colour::darkYellow),              opaque(Drawing::Colour::darkYellow),              opaque(Drawing::Colour::darkYellow)                                         } },
        { WindowClass::themes,                      "WC_THEMES",                         STR_THEMES_WINDOW_THEMES,                         { opaque(Drawing::Colour::grey),                     opaque(Drawing::Colour::darkGreen),               opaque(Drawing::Colour::darkGreen)                                          } },
        { WindowClass::staff,                       "WC_STAFF",                          STR_THEMES_WINDOW_STAFF,                          { opaque(Drawing::Colour::grey),                     opaque(Drawing::Colour::lightPurple),             opaque(Drawing::Colour::lightPurple)                                        } },
        { WindowClass::editorTrackBottomToolbar,    "WC_EDITOR_TRACK_BOTTOM_TOOLBAR",    STR_THEMES_WINDOW_BOTTOM_TOOLBAR_TRACK_EDITOR,    { translucent(Drawing::Colour::lightBlue),          translucent(Drawing::Colour::lightBlue),          translucent(Drawing::Colour::lightBlue)                                     } },
        { WindowClass::editorScenarioBottomToolbar, "WC_EDITOR_SCENARIO_BOTTOM_TOOLBAR", STR_THEMES_WINDOW_BOTTOM_TOOLBAR_SCENARIO_EDITOR, { translucent(Drawing::Colour::lightBrown),         translucent(Drawing::Colour::lightBrown),         translucent(Drawing::Colour::mossGreen)                                     } },
        { WindowClass::tileInspector,               "WC_TILE_INSPECTOR",                 STR_TILE_INSPECTOR_TITLE,                         { opaque(Drawing::Colour::lightBlue),               opaque(Drawing::Colour::lightBlue)                                                                                   } },
        { WindowClass::viewClipping,                "WC_VIEW_CLIPPING",                  STR_VIEW_CLIPPING_TITLE,                          { opaque(Drawing::Colour::darkGreen)                                                                                                                            } },
        { WindowClass::patrolArea,                  "WC_PATROL_AREA",                    STR_SET_PATROL_AREA,                              { opaque(Drawing::Colour::lightPurple),             opaque(Drawing::Colour::lightPurple),             opaque(Drawing::Colour::lightPurple)                                        } },
        { WindowClass::transparency,                "WC_TRANSPARENCY",                   STR_TRANSPARENCY_OPTIONS_TITLE,                   { opaque(Drawing::Colour::lightBlue),               opaque(Drawing::Colour::lightBlue),               opaque(Drawing::Colour::lightBlue)                                          } },
        { WindowClass::about,                       "WC_ABOUT",                          STR_ABOUT,                                        { opaque(Drawing::Colour::grey),                     opaque(Drawing::Colour::lightBlue)                                                                                   } },
        { WindowClass::changelog,                   "WC_CHANGELOG",                      STR_CHANGELOG_TITLE,                              { opaque(Drawing::Colour::lightBlue),               opaque(Drawing::Colour::lightBlue)                                                                                   } },
        { WindowClass::multiplayer,                 "WC_MULTIPLAYER",                    STR_MULTIPLAYER,                                  { opaque(Drawing::Colour::lightBlue),               opaque(Drawing::Colour::lightBlue),               opaque(Drawing::Colour::lightBlue)                                          } },
        { WindowClass::player,                      "WC_PLAYER",                         STR_THEMES_WINDOW_PLAYER,                         { opaque(Drawing::Colour::lightBlue),               opaque(Drawing::Colour::lightBlue),               opaque(Drawing::Colour::lightBlue)                                          } },
        { WindowClass::networkStatus,               "WC_NETWORK_STATUS",                 STR_THEMES_WINDOW_NETWORK_STATUS,                 { opaque(Drawing::Colour::black)                                                                                                                                 } },
        { WindowClass::serverList,                  "WC_SERVER_LIST",                    STR_SERVER_LIST,                                  { opaque(Drawing::Colour::lightBlue),               opaque(Drawing::Colour::lightBlue)                                                                                   } },
        { WindowClass::chat,                        "WC_CHAT",                           STR_CHAT,                                         { translucent(Drawing::Colour::grey)                                                                                                                             } },
        { WindowClass::console,                     "WC_CONSOLE",                        STR_CONSOLE,                                      { translucent(Drawing::Colour::lightBlue),          opaque(Drawing::Colour::white)                                                                                        } },
        { WindowClass::progressWindow,              "WC_PROGRESS_WINDOW",                STR_THEME_LOADING_WINDOW,                         { opaque(Drawing::Colour::black)                                                                                                                                 } },
        { WindowClass::editorParkEntrance,          "WC_EDITOR_PARK_ENTRANCE",           STR_OBJECT_SELECTION_PARK_ENTRANCE,               { opaque(Drawing::Colour::darkGreen),               opaque(Drawing::Colour::darkBrown)                                                                                   } },
    };

#pragma endregion

#pragma region Pre-defined Themes

    static constexpr std::array kPredefinedThemeRCT1Entries = std::to_array<UIThemeWindowEntry>({
        { WindowClass::topToolbar,             { opaque(Drawing::Colour::grey),             opaque(Drawing::Colour::grey),             opaque(Drawing::Colour::grey),                opaque(Drawing::Colour::grey),     opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
        { WindowClass::bottomToolbar,          { translucent(Drawing::Colour::grey),        opaque(Drawing::Colour::grey),             opaque(Drawing::Colour::voidBackground),                opaque(Drawing::Colour::yellow),   opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
        { WindowClass::ride,                   { opaque(Drawing::Colour::bordeauxRed),     opaque(Drawing::Colour::grey),             opaque(Drawing::Colour::saturatedGreen),     opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
        { WindowClass::rideList,               { opaque(Drawing::Colour::bordeauxRed),     opaque(Drawing::Colour::grey),             opaque(Drawing::Colour::grey),                opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
        { WindowClass::constructRide,          { opaque(Drawing::Colour::bordeauxRed),     opaque(Drawing::Colour::grey),             opaque(Drawing::Colour::grey),                opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
        { WindowClass::peep,                   { opaque(Drawing::Colour::lightBrown),      opaque(Drawing::Colour::bordeauxRed),     opaque(Drawing::Colour::bordeauxRed),        opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
        { WindowClass::guestList,              { opaque(Drawing::Colour::lightBrown),      opaque(Drawing::Colour::bordeauxRed),     opaque(Drawing::Colour::bordeauxRed),        opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
        { WindowClass::staffList,              { opaque(Drawing::Colour::darkGreen),       opaque(Drawing::Colour::lightPurple),     opaque(Drawing::Colour::lightPurple),        opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
        { WindowClass::finances,               { opaque(Drawing::Colour::lightPurple),     opaque(Drawing::Colour::grey),             opaque(Drawing::Colour::grey),                opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
        { WindowClass::titleMenu,              { translucent(Drawing::Colour::grey),        translucent(Drawing::Colour::grey),        translucent(Drawing::Colour::grey),           opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
        { WindowClass::titleExit,              { translucent(Drawing::Colour::grey),        translucent(Drawing::Colour::grey),        translucent(Drawing::Colour::grey),           opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
        { WindowClass::newCampaign,            { opaque(Drawing::Colour::lightPurple),     opaque(Drawing::Colour::lightPurple),     opaque(Drawing::Colour::grey),                opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
        { WindowClass::titleOptions,           { translucent(Drawing::Colour::grey),        translucent(Drawing::Colour::grey),        translucent(Drawing::Colour::grey),           opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
        { WindowClass::staff,                  { opaque(Drawing::Colour::darkGreen),       opaque(Drawing::Colour::lightPurple),     opaque(Drawing::Colour::lightPurple),        opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
        { WindowClass::options,                { opaque(Drawing::Colour::grey),             opaque(Drawing::Colour::darkBrown),       opaque(Drawing::Colour::darkBrown),          opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
        { WindowClass::assetPacks,             { opaque(Drawing::Colour::darkBrown),       opaque(Drawing::Colour::darkBrown),       opaque(Drawing::Colour::darkBrown),          opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
        { WindowClass::keyboardShortcutList,   { opaque(Drawing::Colour::grey),             opaque(Drawing::Colour::darkBrown),       opaque(Drawing::Colour::darkBrown),          opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
        { WindowClass::changeKeyboardShortcut, { opaque(Drawing::Colour::darkBrown),       opaque(Drawing::Colour::darkBrown),       opaque(Drawing::Colour::darkBrown),          opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
        { WindowClass::recentNews,             { opaque(Drawing::Colour::darkBrown),       opaque(Drawing::Colour::grey),             opaque(Drawing::Colour::grey),                opaque(Drawing::Colour::voidBackground),     opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
        { WindowClass::trackDesignList,        { opaque(Drawing::Colour::darkBrown),       opaque(Drawing::Colour::darkBrown),       opaque(Drawing::Colour::darkBrown),          opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
        { WindowClass::map,                    { opaque(Drawing::Colour::darkBrown),       opaque(Drawing::Colour::grey),             opaque(Drawing::Colour::grey),                opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
        { WindowClass::about,                  { opaque(Drawing::Colour::grey),             opaque(Drawing::Colour::darkBrown),       opaque(Drawing::Colour::white),               opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
        { WindowClass::changelog,              { opaque(Drawing::Colour::darkBrown),       opaque(Drawing::Colour::darkBrown),       opaque(Drawing::Colour::white),               opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black),    opaque(Drawing::Colour::black) } },
    });
    // clang-format on

    static constexpr std::array<UIThemeWindowEntry, 0> kPredefinedThemeRCT2Entries = {};

    const UITheme kPredefinedThemeRCT1 = UITheme::CreatePredefined(
        "*RCT1", kPredefinedThemeRCT1Entries,
        UITHEME_FLAG_USE_LIGHTS_RIDE | UITHEME_FLAG_USE_LIGHTS_PARK | UITHEME_FLAG_USE_ALTERNATIVE_SCENARIO_SELECT_FONT
            | UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR | UITHEME_FLAG_USE_3D_IMAGE_BUTTONS);

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
        for (uint8_t i = 0; i < wtDesc->windowColours.numColours; i++)
        {
            json_t jsonEntry = { { "colour", colourToString(Theme.Colours[i].colour) },
                                 { "translucent", Theme.Colours[i].flags.has(ColourFlag::translucent) } };

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
        result.Theme = wtDesc->windowColours.defaultTheme;

        // result.Theme.Colours only has 6 values
        size_t colourCount = std::min(jsonColours.size(), static_cast<size_t>(wtDesc->windowColours.numColours));

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
                auto colour = colourFromString(Json::GetString(colourObject["colour"]), Drawing::Colour::black);
                auto isTranslucent = Json::GetBoolean(colourObject["translucent"], false);
                ColourFlags flags{};
                flags.set(ColourFlag::translucent, isTranslucent);

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
            { "use3dImageButtons", (Flags & UITHEME_FLAG_USE_3D_IMAGE_BUTTONS) != 0 },
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
                    { "use3dImageButtons", UITHEME_FLAG_USE_3D_IMAGE_BUTTONS },
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
            result = FromJson(json);
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
            for (size_t i = 0; i < AvailableThemes.size(); i++)
            {
                const auto& theme = AvailableThemes[i];
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
            GetAvailableThemes(&AvailableThemes);
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
            return desc->windowColours.defaultTheme.Colours[index];
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
            entry.Theme = desc->windowColours.defaultTheme;
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
        return desc->windowColours.numColours;
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

            windowTheme = &desc->windowColours.defaultTheme;
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
