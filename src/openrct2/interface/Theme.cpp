#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma warning(disable : 4706) // assignment within conditional expression

#include <vector>
#include <jansson.h>

extern "C"
{
    #include "../common.h"
    #include "../config/Config.h"
    #include "../platform/platform.h"
    #include "themes.h"
    #include "window.h"
}

#include "../core/Json.hpp"
#include "../core/Math.hpp"
#include "../core/Memory.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../core/Util.hpp"
#include "../localisation/string_ids.h"

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
    rct_windowclass WindowClass;
    WindowTheme     Theme;


           json_t *           ToJson() const;
    static UIThemeWindowEntry FromJson(const WindowThemeDesc * wtDesc, const json_t * json);
};

/**
 * Represents a user interface theme. Contains window colour schemes and appearance features.
 */
class UITheme
{
public:
    utf8 *                          Name;
    std::vector<UIThemeWindowEntry> Entries;
    uint8                           Flags;

    explicit UITheme(const utf8 * name);
    UITheme(const UITheme & copy);
    ~UITheme();

    void                       SetName(const utf8 * name);
    const UIThemeWindowEntry * GetEntry(rct_windowclass windowClass) const;
    void                       SetEntry(const UIThemeWindowEntry * entry);
    void                       RemoveEntry(rct_windowclass windowClass);

    json_t * ToJson() const;
    bool     WriteToFile(const utf8 * path) const;

    static UITheme * FromJson(const json_t * json);
    static UITheme * FromFile(const utf8 * path);
    static UITheme   CreatePredefined(const utf8 * name, const UIThemeWindowEntry * entries, uint8 flags);
};

/**
 * Represents the theme descriptor for a specific window type including the default colour scheme.
 */
struct WindowThemeDesc
{
    rct_windowclass WindowClass;
    const utf8 *    WindowClassSZ;
    rct_string_id   WindowName;
    uint8           NumColours;
    WindowTheme     DefaultTheme;
};

#pragma region Window Theme Descriptors

#define COLOURS_1(c0) 1, { { (c0), 0, 0, 0, 0, 0 } }
#define COLOURS_2(c0, c1) 2, { { (c0), (c1), 0, 0, 0, 0 } }
#define COLOURS_3(c0, c1, c2) 3, { { (c0), (c1), (c2), 0, 0, 0 } }
#define COLOURS_4(c0, c1, c2, c3) 4, { { (c0), (c1), (c2), (c3), 0, 0 } }
#define COLOURS_5(c0, c1, c2, c3, c4) 5, { { (c0), (c1), (c2), (c3), (c4), 0 } }
#define COLOURS_6(c0, c1, c2, c3, c4, c5) 6, { { (c0), (c1), (c2), (c3), (c4), (c5) } }

#define THEME_DEF_END { 0xFF, { 0, 0, 0, 0, 0, 0 } }

#define TWINDOW(window_class, window_name, window_string_id, theme) { window_class, window_name, window_string_id, theme }

#define THEME_WC(wc) wc, #wc

WindowThemeDesc WindowThemeDescriptors[] =
{
    // WindowClass, WindowClassSZ                  WindowName                                        NumColours, DefaultTheme
    { THEME_WC(WC_TOP_TOOLBAR),                    STR_THEMES_WINDOW_TOP_TOOLBAR,                    COLOURS_4(COLOUR_LIGHT_BLUE,               COLOUR_DARK_GREEN,               COLOUR_DARK_BROWN,             COLOUR_GREY         ) },
    { THEME_WC(WC_BOTTOM_TOOLBAR),                 STR_THEMES_WINDOW_BOTTOM_TOOLBAR,                 COLOURS_4(TRANSLUCENT(COLOUR_DARK_GREEN),  TRANSLUCENT(COLOUR_DARK_GREEN),  COLOUR_BLACK,                  COLOUR_BRIGHT_GREEN ) },
    { THEME_WC(WC_RIDE),                           STR_THEMES_WINDOW_RIDE,                           COLOURS_3(COLOUR_GREY,                     COLOUR_BORDEAUX_RED,             COLOUR_GREY                                        ) },
    { THEME_WC(WC_RIDE_CONSTRUCTION),              STR_THEMES_WINDOW_RIDE_CONSTRUCTION,              COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { THEME_WC(WC_RIDE_LIST),                      STR_THEMES_WINDOW_RIDE_LIST,                      COLOURS_3(COLOUR_GREY,                     COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED                                ) },
    { THEME_WC(WC_SAVE_PROMPT),                    STR_THEMES_WINDOW_SAVE_PROMPT,                    COLOURS_1(TRANSLUCENT(COLOUR_BORDEAUX_RED)                                                                                     ) },
    { THEME_WC(WC_CONSTRUCT_RIDE),                 STR_THEMES_WINDOW_CONSTRUCT_RIDE,                 COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED                                ) },
    { THEME_WC(WC_DEMOLISH_RIDE_PROMPT),           STR_THEMES_WINDOW_DEMOLISH_RIDE_PROMPT,           COLOURS_1(TRANSLUCENT(COLOUR_BORDEAUX_RED)                                                                                     ) },
    { THEME_WC(WC_SCENERY),                        STR_THEMES_WINDOW_SCENERY,                        COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_GREEN,               COLOUR_DARK_GREEN                                  ) },
    { THEME_WC(WC_OPTIONS),                        STR_THEMES_WINDOW_OPTIONS,                        COLOURS_3(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                  ) },
    { THEME_WC(WC_FOOTPATH),                       STR_THEMES_WINDOW_FOOTPATH,                       COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { THEME_WC(WC_LAND),                           STR_THEMES_WINDOW_LAND,                           COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { THEME_WC(WC_WATER),                          STR_THEMES_WINDOW_WATER,                          COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { THEME_WC(WC_PEEP),                           STR_THEMES_WINDOW_PEEP,                           COLOURS_3(COLOUR_GREY,                     COLOUR_OLIVE_GREEN,              COLOUR_OLIVE_GREEN                                 ) },
    { THEME_WC(WC_GUEST_LIST),                     STR_THEMES_WINDOW_GUEST_LIST,                     COLOURS_3(COLOUR_GREY,                     COLOUR_OLIVE_GREEN,              COLOUR_OLIVE_GREEN                                 ) },
    { THEME_WC(WC_STAFF_LIST),                     STR_THEMES_WINDOW_STAFF_LIST,                     COLOURS_3(COLOUR_GREY,                     COLOUR_LIGHT_PURPLE,             COLOUR_LIGHT_PURPLE                                ) },
    { THEME_WC(WC_FIRE_PROMPT),                    STR_THEMES_WINDOW_FIRE_PROMPT,                    COLOURS_1(TRANSLUCENT(COLOUR_BORDEAUX_RED)                                                                                     ) },
    { THEME_WC(WC_PARK_INFORMATION),               STR_THEMES_WINDOW_PARK_INFORMATION,               COLOURS_3(COLOUR_GREY,                     COLOUR_DARK_YELLOW,              COLOUR_DARK_YELLOW                                 ) },
    { THEME_WC(WC_FINANCES),                       STR_THEMES_WINDOW_FINANCES,                       COLOURS_3(COLOUR_GREY,                     COLOUR_DARK_YELLOW,              COLOUR_DARK_YELLOW                                 ) },
    { THEME_WC(WC_TITLE_MENU),                     STR_THEMES_WINDOW_TITLE_MENU_BUTTONS,             COLOURS_3(TRANSLUCENT(COLOUR_DARK_GREEN),  TRANSLUCENT(COLOUR_DARK_GREEN),  TRANSLUCENT(COLOUR_DARK_GREEN)                     ) },
    { THEME_WC(WC_TITLE_EXIT),                     STR_THEMES_WINDOW_TITLE_MENU_EXIT,                COLOURS_3(TRANSLUCENT(COLOUR_DARK_GREEN),  TRANSLUCENT(COLOUR_DARK_GREEN),  TRANSLUCENT(COLOUR_DARK_GREEN)                     ) },
    { THEME_WC(WC_RECENT_NEWS),                    STR_THEMES_WINDOW_RECENT_NEWS,                    COLOURS_3(COLOUR_GREY,                     COLOUR_GREY,                     COLOUR_BLACK                                       ) },
    { THEME_WC(WC_SCENARIO_SELECT),                STR_THEMES_WINDOW_TITLE_MENU_SCENARIO_SELECTION,  COLOURS_3(COLOUR_GREY,                     COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED                                ) },
    { THEME_WC(WC_TRACK_DESIGN_LIST),              STR_THEMES_WINDOW_TRACK_DESIGN_LIST,              COLOURS_3(COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED                                ) },
    { THEME_WC(WC_TRACK_DESIGN_PLACE),             STR_THEMES_WINDOW_TRACK_DESIGN_PLACE,             COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { THEME_WC(WC_NEW_CAMPAIGN),                   STR_THEMES_WINDOW_NEW_CAMPAIGN,                   COLOURS_3(COLOUR_DARK_YELLOW,              COLOUR_DARK_YELLOW,              COLOUR_DARK_YELLOW                                 ) },
    { THEME_WC(WC_KEYBOARD_SHORTCUT_LIST),         STR_THEMES_WINDOW_KEYBOARD_SHORTCUT_LIST,         COLOURS_3(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                  ) },
    { THEME_WC(WC_CHANGE_KEYBOARD_SHORTCUT),       STR_THEMES_WINDOW_CHANGE_KEYBOARD_SHORTCUT,       COLOURS_3(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                  ) },
    { THEME_WC(WC_MAP),                            STR_THEMES_WINDOW_MAP,                            COLOURS_2(COLOUR_DARK_GREEN,               COLOUR_DARK_BROWN                                                                   ) },
    { THEME_WC(WC_BANNER),                         STR_THEMES_WINDOW_BANNER,                         COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { THEME_WC(WC_EDITOR_OBJECT_SELECTION),        STR_THEMES_WINDOW_EDITOR_OBJECT_SELECTION,        COLOURS_3(COLOUR_LIGHT_PURPLE,             COLOUR_GREY,                     COLOUR_GREY                                        ) },
    { THEME_WC(WC_EDITOR_INVENTION_LIST),          STR_THEMES_WINDOW_EDITOR_INVENTION_LIST,          COLOURS_3(COLOUR_LIGHT_PURPLE,             COLOUR_GREY,                     COLOUR_GREY                                        ) },
    { THEME_WC(WC_EDITOR_SCENARIO_OPTIONS),        STR_THEMES_WINDOW_EDITOR_SCENARIO_OPTIONS,        COLOURS_3(COLOUR_LIGHT_PURPLE,             COLOUR_GREY,                     COLOUR_GREY                                        ) },
    { THEME_WC(WC_EDTIOR_OBJECTIVE_OPTIONS),       STR_THEMES_WINDOW_EDTIOR_OBJECTIVE_OPTIONS,       COLOURS_3(COLOUR_LIGHT_PURPLE,             COLOUR_GREY,                     COLOUR_GREY                                        ) },
    { THEME_WC(WC_MANAGE_TRACK_DESIGN),            STR_THEMES_WINDOW_MANAGE_TRACK_DESIGN,            COLOURS_3(COLOUR_GREY,                     COLOUR_GREY,                     COLOUR_GREY                                        ) },
    { THEME_WC(WC_TRACK_DELETE_PROMPT),            STR_THEMES_WINDOW_TRACK_DELETE_PROMPT,            COLOURS_3(COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED                                ) },
    { THEME_WC(WC_INSTALL_TRACK),                  STR_THEMES_WINDOW_INSTALL_TRACK,                  COLOURS_3(COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED                                ) },
    { THEME_WC(WC_CLEAR_SCENERY),                  STR_THEMES_WINDOW_CLEAR_SCENERY,                  COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { THEME_WC(WC_CHEATS),                         STR_CHEAT_TITLE,                                  COLOURS_2(COLOUR_GREY,                     COLOUR_DARK_YELLOW                                                                  ) },
    { THEME_WC(WC_RESEARCH),                       STR_THEMES_WINDOW_RESEARCH,                       COLOURS_3(COLOUR_GREY,                     COLOUR_DARK_YELLOW,              COLOUR_DARK_YELLOW                                 ) },
    { THEME_WC(WC_VIEWPORT),                       STR_THEMES_WINDOW_VIEWPORT,                       COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { THEME_WC(WC_MAPGEN),                         STR_THEMES_WINDOW_MAPGEN,                         COLOURS_3(COLOUR_DARK_GREEN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { THEME_WC(WC_LOADSAVE),                       STR_THEMES_WINDOW_LOADSAVE,                       COLOURS_3(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                  ) },
    { THEME_WC(WC_LOADSAVE_OVERWRITE_PROMPT),      STR_THEMES_WINDOW_LOADSAVE_OVERWRITE_PROMPT,      COLOURS_1(TRANSLUCENT(COLOUR_BORDEAUX_RED)                                                                                     ) },
    { THEME_WC(WC_TITLE_OPTIONS),                  STR_THEMES_WINDOW_TITLE_MENU_OPTIONS,             COLOURS_3(TRANSLUCENT(COLOUR_DARK_GREEN),  TRANSLUCENT(COLOUR_DARK_GREEN),  TRANSLUCENT(COLOUR_DARK_GREEN)                     ) },
    { THEME_WC(WC_LAND_RIGHTS),                    STR_THEMES_WINDOW_LAND_RIGHTS,                    COLOURS_3(COLOUR_DARK_YELLOW,              COLOUR_DARK_YELLOW,              COLOUR_DARK_YELLOW                                 ) },
    { THEME_WC(WC_THEMES),                         STR_THEMES_WINDOW_THEMES,                         COLOURS_3(COLOUR_GREY,                     COLOUR_DARK_GREEN,               COLOUR_DARK_GREEN                                  ) },
    { THEME_WC(WC_STAFF),                          STR_THEMES_WINDOW_STAFF,                          COLOURS_3(COLOUR_GREY,                     COLOUR_LIGHT_PURPLE,             COLOUR_LIGHT_PURPLE                                ) },
    { THEME_WC(WC_EDITOR_TRACK_BOTTOM_TOOLBAR),    STR_THEMES_WINDOW_BOTTOM_TOOLBAR_TRACK_EDITOR,    COLOURS_3(TRANSLUCENT(COLOUR_LIGHT_BLUE),  TRANSLUCENT(COLOUR_LIGHT_BLUE),  TRANSLUCENT(COLOUR_LIGHT_BLUE)                     ) },
    { THEME_WC(WC_EDITOR_SCENARIO_BOTTOM_TOOLBAR), STR_THEMES_WINDOW_BOTTOM_TOOLBAR_SCENARIO_EDITOR, COLOURS_3(TRANSLUCENT(COLOUR_LIGHT_BROWN), TRANSLUCENT(COLOUR_LIGHT_BROWN), TRANSLUCENT(COLOUR_MOSS_GREEN)                     ) },
    { THEME_WC(WC_TITLE_EDITOR),                   STR_TITLE_EDITOR_TITLE,                           COLOURS_3(COLOUR_GREY,                     COLOUR_OLIVE_GREEN,              COLOUR_OLIVE_GREEN                                 ) },
    { THEME_WC(WC_TILE_INSPECTOR),                 STR_TILE_INSPECTOR_TITLE,                         COLOURS_2(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                                                   ) },
    { THEME_WC(WC_VIEW_CLIPPING),                  STR_VIEW_CLIPPING_TITLE,                          COLOURS_1(COLOUR_DARK_GREEN                                                                                                    ) },
    { THEME_WC(WC_CHANGELOG),                      STR_CHANGELOG_TITLE,                              COLOURS_2(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                                                   ) },
    { THEME_WC(WC_MULTIPLAYER),                    STR_MULTIPLAYER,                                  COLOURS_3(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                  ) },
    { THEME_WC(WC_PLAYER),                         STR_THEMES_WINDOW_PLAYER,                         COLOURS_3(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                  ) },
    { THEME_WC(WC_NETWORK_STATUS),                 STR_THEMES_WINDOW_NETWORK_STATUS,                 COLOURS_1(COLOUR_LIGHT_BLUE                                                                                                    ) },
    { THEME_WC(WC_SERVER_LIST),                    STR_SERVER_LIST,                                  COLOURS_2(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                                                   ) },
    { THEME_WC(WC_CHAT),                           STR_CHAT,                                         COLOURS_1(TRANSLUCENT(COLOUR_GREY)                                                                                             ) },
};

#pragma endregion

#pragma region Pre-defined Themes

#define COLOURS_RCT1(c0, c1, c2, c3, c4, c5) { { (c0), (c1), (c2), (c3), (c4), (c5) } }

UIThemeWindowEntry PredefinedThemeRCT1_Entries[] =
{
    { WC_TOP_TOOLBAR,     COLOURS_RCT1(COLOUR_GREY,               COLOUR_GREY,                COLOUR_GREY,                COLOUR_GREY,     COLOUR_BLACK,    COLOUR_BLACK)    },
    { WC_BOTTOM_TOOLBAR,  COLOURS_RCT1(TRANSLUCENT(COLOUR_GREY),  TRANSLUCENT(COLOUR_GREY),   COLOUR_BLACK,               COLOUR_YELLOW,   COLOUR_BLACK,    COLOUR_BLACK)    },
    { WC_RIDE,            COLOURS_RCT1(COLOUR_BORDEAUX_RED,       COLOUR_GREY,                COLOUR_SATURATED_GREEN,     COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WC_RIDE_LIST,       COLOURS_RCT1(COLOUR_BORDEAUX_RED,       COLOUR_GREY,                COLOUR_GREY,                COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WC_CONSTRUCT_RIDE,  COLOURS_RCT1(COLOUR_BORDEAUX_RED,       COLOUR_GREY,                COLOUR_GREY,                COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WC_PEEP,            COLOURS_RCT1(COLOUR_LIGHT_BROWN,        COLOUR_BORDEAUX_RED,        COLOUR_BORDEAUX_RED,        COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WC_GUEST_LIST,      COLOURS_RCT1(COLOUR_LIGHT_BROWN,        COLOUR_BORDEAUX_RED,        COLOUR_BORDEAUX_RED,        COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WC_STAFF_LIST,      COLOURS_RCT1(COLOUR_DARK_GREEN,         COLOUR_LIGHT_PURPLE,        COLOUR_LIGHT_PURPLE,        COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WC_FINANCES,        COLOURS_RCT1(COLOUR_LIGHT_PURPLE,       COLOUR_GREY,                COLOUR_GREY,                COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WC_TITLE_MENU,      COLOURS_RCT1(TRANSLUCENT(COLOUR_GREY),  TRANSLUCENT(COLOUR_GREY),   TRANSLUCENT(COLOUR_GREY),   COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WC_TITLE_EXIT,      COLOURS_RCT1(TRANSLUCENT(COLOUR_GREY),  TRANSLUCENT(COLOUR_GREY),   TRANSLUCENT(COLOUR_GREY),   COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WC_NEW_CAMPAIGN,    COLOURS_RCT1(COLOUR_LIGHT_PURPLE,       COLOUR_LIGHT_PURPLE,        COLOUR_GREY,                COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WC_TITLE_OPTIONS,   COLOURS_RCT1(TRANSLUCENT(COLOUR_GREY),  TRANSLUCENT(COLOUR_GREY),   TRANSLUCENT(COLOUR_GREY),   COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    { WC_STAFF,           COLOURS_RCT1(COLOUR_DARK_GREEN,         COLOUR_LIGHT_PURPLE,        COLOUR_LIGHT_PURPLE,        COLOUR_BLACK,    COLOUR_BLACK,    COLOUR_BLACK)    },
    THEME_DEF_END
};

UIThemeWindowEntry PredefinedThemeRCT2_Entries[] =
{
    THEME_DEF_END
};

const UITheme PredefinedThemeRCT1 = UITheme::CreatePredefined(
    "RCT1", PredefinedThemeRCT1_Entries, UITHEME_FLAG_USE_LIGHTS_RIDE |
                                         UITHEME_FLAG_USE_LIGHTS_PARK |
                                         UITHEME_FLAG_USE_ALTERNATIVE_SCENARIO_SELECT_FONT);

const UITheme PredefinedThemeRCT2 = UITheme::CreatePredefined(
    "RCT2", PredefinedThemeRCT2_Entries, 0);

const UITheme * PredefinedThemes[] = {
    &PredefinedThemeRCT1,
    &PredefinedThemeRCT2,
    nullptr
};

#pragma endregion

static const WindowThemeDesc * GetWindowThemeDescriptor(rct_windowclass windowClass)
{
    for (size_t i = 0; i < Util::CountOf(WindowThemeDescriptors); i++)
    {
        const WindowThemeDesc * desc = &WindowThemeDescriptors[i];
        if (desc->WindowClass == windowClass)
        {
            return desc;
        }
    }
    return nullptr;
}

static const WindowThemeDesc * GetWindowThemeDescriptor(const utf8 * windowClassSZ)
{
    for (size_t i = 0; i < Util::CountOf(WindowThemeDescriptors); i++)
    {
        const WindowThemeDesc * desc = &WindowThemeDescriptors[i];
        if (String::Equals(desc->WindowClassSZ, windowClassSZ))
        {
            return desc;
        }
    }
    return nullptr;
}

static void ThrowThemeLoadException()
{
    throw Exception("Invalid JSON UI theme entry.");
}

#pragma region UIThemeEntry

json_t * UIThemeWindowEntry::ToJson() const
{
    const WindowThemeDesc * wtDesc = GetWindowThemeDescriptor(WindowClass);
    if (wtDesc == nullptr)
    {
        return nullptr;
    }

    json_t * jsonColours = json_array();
    for (uint8 i = 0; i < wtDesc->NumColours; i++) {
        colour_t colour = Theme.Colours[i];
        json_array_append_new(jsonColours, json_integer(colour));
    }

    json_t * jsonEntry = json_object();
    json_object_set_new(jsonEntry, "colours", jsonColours);

    return jsonEntry;
}

UIThemeWindowEntry UIThemeWindowEntry::FromJson(const WindowThemeDesc * wtDesc, const json_t * json)
{
    json_t * jsonColours = json_object_get(json, "colours");
    if (jsonColours == nullptr)
    {
        ThrowThemeLoadException();
    }

    uint8 numColours = (uint8)json_array_size(jsonColours);
    numColours = Math::Min(numColours, wtDesc->NumColours);

    UIThemeWindowEntry result;
    result.WindowClass = wtDesc->WindowClass;
    result.Theme = wtDesc->DefaultTheme;

    for (uint8 i = 0; i < numColours; i++)
    {
        result.Theme.Colours[i] = (colour_t)json_integer_value(json_array_get(jsonColours, i));
    }

    return result;
}

#pragma endregion

#pragma region UITheme

UITheme::UITheme(const utf8 * name)
{
    Name = String::Duplicate(name);
    Flags = 0;
}

UITheme::UITheme(const UITheme & copy)
{
    Name = String::Duplicate(copy.Name);
    Flags = copy.Flags;
    Entries = copy.Entries;
}

UITheme::~UITheme()
{
    Memory::Free(Name);
}

void UITheme::SetName(const utf8 * name)
{
    String::DiscardDuplicate(&Name, name);
}

const UIThemeWindowEntry * UITheme::GetEntry(rct_windowclass windowClass) const
{
    for (size_t i = 0; i < Entries.size(); i++)
    {
        const UIThemeWindowEntry * entry = &Entries[i];
        if (entry->WindowClass == windowClass)
        {
            return entry;
        }
    }
    return nullptr;
}

void UITheme::SetEntry(const UIThemeWindowEntry * newEntry)
{
    // Try to replace existing entry
    for (size_t i = 0; i < Entries.size(); i++)
    {
        UIThemeWindowEntry * entry = &Entries[i];
        if (entry->WindowClass == newEntry->WindowClass)
        {
            *entry = *newEntry;
            return;
        }
    }

    Entries.push_back(*newEntry);
}

void UITheme::RemoveEntry(rct_windowclass windowClass)
{
    // Remove existing entry
    for (size_t i = 0; i < Entries.size(); i++)
    {
        UIThemeWindowEntry * entry = &Entries[i];
        if (entry->WindowClass == windowClass)
        {
            Entries.erase(Entries.begin() + i);
            break;
        }
    }
}

json_t * UITheme::ToJson() const
{
    // Create entries
    json_t * jsonEntries = json_object();
    for (const UIThemeWindowEntry & entry : Entries)
    {
        const WindowThemeDesc * wtDesc = GetWindowThemeDescriptor(entry.WindowClass);
        if (wtDesc == nullptr)
        {
            return nullptr;
        }
        json_object_set_new(jsonEntries, wtDesc->WindowClassSZ, entry.ToJson());
    }

    // Create theme object
    json_t * jsonTheme = json_object();
    json_object_set_new(jsonTheme, "name", json_string(Name));
    json_object_set_new(jsonTheme, "entries", jsonEntries);

    json_object_set_new(jsonTheme, "useLightsRide", json_boolean(Flags & UITHEME_FLAG_USE_LIGHTS_RIDE));
    json_object_set_new(jsonTheme, "useLightsPark", json_boolean(Flags & UITHEME_FLAG_USE_LIGHTS_PARK));
    json_object_set_new(jsonTheme,
                        "useAltScenarioSelectFont",
                        json_boolean(Flags & UITHEME_FLAG_USE_ALTERNATIVE_SCENARIO_SELECT_FONT));

    return jsonTheme;
}

bool UITheme::WriteToFile(const utf8 * path) const
{
    json_t * jsonTheme = ToJson();
    bool     result;
    try
    {
        Json::WriteToFile(path, jsonTheme, JSON_INDENT(4) | JSON_PRESERVE_ORDER);
        result = true;
    }
    catch (const Exception & ex)
    {
        log_error("Unable to save %s: %s", path, ex.GetMessage());
        result = false;
    }

    json_decref(jsonTheme);
    return result;
}

UITheme * UITheme::FromJson(const json_t * json)
{
    const char * themeName = json_string_value(json_object_get(json, "name"));
    if (themeName == nullptr)
    {
        ThrowThemeLoadException();
    }

    json_t * jsonEntries = json_object_get(json, "entries");

    UITheme * result = nullptr;
    try
    {
        result = new UITheme(themeName);

        if (json_is_true(json_object_get(json, "useLightsRide")))
        {
            result->Flags |= UITHEME_FLAG_USE_LIGHTS_RIDE;
        }
        if (json_is_true(json_object_get(json, "useLightsPark")))
        {
            result->Flags |= UITHEME_FLAG_USE_LIGHTS_PARK;
        }
        if (json_is_true(json_object_get(json, "useAltScenarioSelectFont")))
        {
            result->Flags |= UITHEME_FLAG_USE_ALTERNATIVE_SCENARIO_SELECT_FONT;
        }

        const char * jkey;
        json_t     * jvalue;
        json_object_foreach(jsonEntries, jkey, jvalue)
        {
            const WindowThemeDesc * wtDesc = GetWindowThemeDescriptor(jkey);
            if (wtDesc == nullptr) continue;

            UIThemeWindowEntry entry = UIThemeWindowEntry::FromJson(wtDesc, jvalue);
            result->SetEntry(&entry);
        }

        return result;
    }
    catch (const Exception &)
    {
        delete result;
        throw;
    }
}

UITheme * UITheme::FromFile(const utf8 * path)
{
    json_t  * json = nullptr;
    UITheme * result = nullptr;
    try
    {
        json = Json::ReadFromFile(path);
        result = UITheme::FromJson(json);
    }
    catch (const Exception &)
    {
        log_error("Unable to read theme: %s", path);
        result = nullptr;
    }

    json_decref(json);
    return result;
}

UITheme UITheme::CreatePredefined(const utf8 * name, const UIThemeWindowEntry * entries, uint8 flags)
{
    auto theme = UITheme(name);
    theme.Flags = flags | UITHEME_FLAG_PREDEFINED;

    size_t numEntries = 0;
    for (const UIThemeWindowEntry * entry = entries; entry->WindowClass != 255; entry++)
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
        utf8 Path[MAX_PATH];
        utf8 Name[96];
    };

    utf8 *                      CurrentThemePath;
    UITheme *                   CurrentTheme;
    std::vector<AvailableTheme> AvailableThemes;
    size_t                      ActiveAvailableThemeIndex = SIZE_MAX;
    size_t                      NumPredefinedThemes = 0;

    void GetThemeFileName(utf8 * buffer, size_t bufferSize, const utf8 * name);
    bool EnsureThemeDirectoryExists();
    void GetThemePath(utf8 * buffer, size_t bufferSize);

    static void GetAvailableThemes(std::vector<AvailableTheme> * outThemes)
    {
        Guard::ArgumentNotNull(outThemes, GUARD_LINE);

        outThemes->clear();

        NumPredefinedThemes = 0;
        for (const UITheme * * predefinedTheme = PredefinedThemes; *predefinedTheme != nullptr; predefinedTheme++)
        {
            AvailableTheme theme;
            String::Set(theme.Path, sizeof(theme.Path), String::Empty);
            String::Set(theme.Name, sizeof(theme.Name), (*predefinedTheme)->Name);
            outThemes->push_back(theme);

            NumPredefinedThemes++;
        }

        utf8 themesPattern[MAX_PATH];
        GetThemePath(themesPattern, sizeof(themesPattern));
        Path::Append(themesPattern, sizeof(themesPattern), "*.json");

        sint32 handle = platform_enumerate_files_begin(themesPattern);
        if (handle != INVALID_HANDLE)
        {
            file_info fileInfo;
            while (platform_enumerate_files_next(handle, &fileInfo))
            {
                AvailableTheme theme;
                Path::GetFileNameWithoutExtension(theme.Name, sizeof(theme.Name), fileInfo.path);
                GetThemeFileName(theme.Path, sizeof(theme.Path), theme.Name);

                outThemes->push_back(theme);

                if (Path::Equals(CurrentThemePath, fileInfo.path))
                {
                    ActiveAvailableThemeIndex = outThemes->size() - 1;
                }
            }
            platform_enumerate_files_end(handle);
        }
    }

    static void LoadTheme(UITheme * theme)
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
        String::DiscardUse(&CurrentThemePath, nullptr);

        gfx_invalidate_screen();
    }

    static void LoadTheme(const utf8 * path)
    {
        UITheme * theme = UITheme::FromFile(path);
        if (theme == nullptr)
        {
            // Fall-back to default
            theme = (UITheme *)&PredefinedThemeRCT2;
            LoadTheme(theme);
        }
        else
        {
            LoadTheme(theme);
            String::DiscardDuplicate(&CurrentThemePath, path);
        }
    }

    static bool LoadThemeByName(const utf8 * name)
    {
        for (size_t i = 0; i < ThemeManager::AvailableThemes.size(); i++)
        {
            if (String::Equals(name, ThemeManager::AvailableThemes[i].Name))
            {
                const utf8 * path = ThemeManager::AvailableThemes[i].Path;
                if (String::IsNullOrEmpty(path))
                {
                    LoadTheme((UITheme *)PredefinedThemes[i]);
                }
                else
                {
                    LoadTheme(ThemeManager::AvailableThemes[i].Path);
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
        LoadTheme((UITheme *)&PredefinedThemeRCT2);
        ActiveAvailableThemeIndex = 1;

        bool configValid = false;
        if (!String::IsNullOrEmpty(gConfigInterface.current_theme_preset))
        {
            if (LoadThemeByName(gConfigInterface.current_theme_preset))
            {
                configValid = true;
            }
        }

        if (!configValid)
        {
            String::DiscardDuplicate(&gConfigInterface.current_theme_preset, theme_manager_get_available_theme_name(1));
        }
    }

    void GetThemeFileName(utf8 * buffer, size_t bufferSize, const utf8 * name)
    {
        GetThemePath(buffer, bufferSize);
        Path::Append(buffer, bufferSize, name);
        String::Append(buffer, bufferSize, ".json");
    }

    bool EnsureThemeDirectoryExists()
    {
        utf8 path[MAX_PATH];
        GetThemePath(path, sizeof(path));
        return platform_ensure_directory_exists(path);
    }

    void GetThemePath(utf8 * buffer, size_t bufferSize)
    {
        platform_get_user_directory(buffer, "themes", bufferSize);
    }
}

extern "C"
{
    void theme_manager_load_available_themes()
    {
        ThemeManager::GetAvailableThemes(&ThemeManager::AvailableThemes);
    }

    size_t theme_manager_get_num_available_themes()
    {
        return ThemeManager::AvailableThemes.size();
    }

    const utf8 * theme_manager_get_available_theme_path(size_t index)
    {
        return ThemeManager::AvailableThemes[index].Path;
    }

    const utf8 * theme_manager_get_available_theme_name(size_t index)
    {
        return ThemeManager::AvailableThemes[index].Name;
    }

    size_t theme_manager_get_active_available_theme_index()
    {
        return ThemeManager::ActiveAvailableThemeIndex;
    }

    void theme_manager_set_active_available_theme(size_t index)
    {
        if (index < ThemeManager::NumPredefinedThemes)
        {
            ThemeManager::LoadTheme((UITheme *)PredefinedThemes[index]);
        }
        else
        {
            const utf8 * path = ThemeManager::AvailableThemes[index].Path;
            ThemeManager::LoadTheme(path);

            // HACK Check if theme load failed and fell back to RCT2
            if (ThemeManager::CurrentThemePath == nullptr)
            {
                index = 1;
            }
        }
        ThemeManager::ActiveAvailableThemeIndex = index;
        String::DiscardDuplicate(&gConfigInterface.current_theme_preset, theme_manager_get_available_theme_name(index));

        // Apply the selected theme to all open windows
        for (rct_window *w = g_window_list; w < gWindowNextSlot; w++)
        {
            colour_scheme_update(w);
        }
    }

    uint8 theme_get_colour(rct_windowclass wc, uint8 index)
    {
        const UIThemeWindowEntry * entry = ThemeManager::CurrentTheme->GetEntry(wc);
        if (entry == nullptr)
        {
            const WindowThemeDesc * desc = GetWindowThemeDescriptor(wc);
            if (desc == nullptr)
            {
                return 0;
            }
            return desc->DefaultTheme.Colours[index];
        }
        else
        {
            return entry->Theme.Colours[index];
        }
    }

    void theme_set_colour(rct_windowclass wc, uint8 index, colour_t colour)
    {
        UIThemeWindowEntry entry;
        entry.WindowClass = wc;

        auto currentEntry = (UIThemeWindowEntry *)ThemeManager::CurrentTheme->GetEntry(wc);
        if (currentEntry != nullptr)
        {
            entry.Theme = currentEntry->Theme;
        }
        else
        {
            const WindowThemeDesc * desc = GetWindowThemeDescriptor(wc);
            if (desc == nullptr)
            {
                return;
            }
            entry.Theme = desc->DefaultTheme;
        }

        entry.Theme.Colours[index] = colour;
        ThemeManager::CurrentTheme->SetEntry(&entry);

        theme_save();
    }

    uint8 theme_get_flags()
    {
        return ThemeManager::CurrentTheme->Flags;
    }

    void theme_set_flags(uint8 flags)
    {
        ThemeManager::CurrentTheme->Flags = flags;
        theme_save();
    }

    void theme_save()
    {
        ThemeManager::EnsureThemeDirectoryExists();
        ThemeManager::CurrentTheme->WriteToFile(ThemeManager::CurrentThemePath);
    }

    void theme_rename(const utf8 * name)
    {
        const utf8 * oldPath = ThemeManager::CurrentThemePath;
        utf8         newPath[MAX_PATH];

        ThemeManager::EnsureThemeDirectoryExists();
        ThemeManager::GetThemeFileName(newPath, sizeof(newPath), name);
        platform_file_move(oldPath, newPath);
        String::DiscardDuplicate(&ThemeManager::CurrentThemePath, newPath);

        ThemeManager::CurrentTheme->SetName(name);
        ThemeManager::CurrentTheme->WriteToFile(ThemeManager::CurrentThemePath);

        theme_manager_load_available_themes();
        for (size_t i = 0; i < ThemeManager::AvailableThemes.size(); i++)
        {
            if (Path::Equals(newPath, ThemeManager::AvailableThemes[i].Path))
            {
                ThemeManager::ActiveAvailableThemeIndex = i;
                String::DiscardDuplicate(&gConfigInterface.current_theme_preset, theme_manager_get_available_theme_name(1));
                break;
            }
        }
    }

    void theme_duplicate(const utf8 * name)
    {
        utf8 newPath[MAX_PATH];

        ThemeManager::EnsureThemeDirectoryExists();
        ThemeManager::GetThemeFileName(newPath, sizeof(newPath), name);

        // Copy the theme, save it and then load it back in
        UITheme * newTheme = new UITheme(*ThemeManager::CurrentTheme);
        newTheme->SetName(name);
        newTheme->Flags &= ~UITHEME_FLAG_PREDEFINED;
        newTheme->WriteToFile(newPath);
        delete newTheme;

        ThemeManager::LoadTheme(newPath);

        theme_manager_load_available_themes();
        for (size_t i = 0; i < ThemeManager::AvailableThemes.size(); i++)
        {
            if (Path::Equals(newPath, ThemeManager::AvailableThemes[i].Path))
            {
                ThemeManager::ActiveAvailableThemeIndex = i;
                String::DiscardDuplicate(&gConfigInterface.current_theme_preset, theme_manager_get_available_theme_name(i));
                break;
            }
        }
    }

    void theme_delete()
    {
        platform_file_delete(ThemeManager::CurrentThemePath);
        ThemeManager::LoadTheme((UITheme *)&PredefinedThemeRCT2);
        ThemeManager::ActiveAvailableThemeIndex = 1;
        String::DiscardDuplicate(&gConfigInterface.current_theme_preset, theme_manager_get_available_theme_name(1));
    }

    void theme_manager_initialise()
    {
        ThemeManager::Initialise();
    }

    uint8 theme_desc_get_num_colours(rct_windowclass wc)
    {
        const WindowThemeDesc * desc = GetWindowThemeDescriptor(wc);
        if (desc == nullptr)
        {
            return 0;
        }
        return desc->NumColours;
    }

    rct_string_id theme_desc_get_name(rct_windowclass wc)
    {
        const WindowThemeDesc * desc = GetWindowThemeDescriptor(wc);
        if (desc == nullptr)
        {
            return STR_EMPTY;
        }
        return desc->WindowName;
    }

    void colour_scheme_update(rct_window * window)
    {
        colour_scheme_update_by_class(window, window->classification);
    }

    void colour_scheme_update_by_class(rct_window * window, rct_windowclass classification)
    {
        const WindowTheme *        windowTheme;
        const UIThemeWindowEntry * entry = ThemeManager::CurrentTheme->GetEntry(classification);
        if (entry != nullptr)
        {
            windowTheme = &entry->Theme;
        }
        else
        {
            const WindowThemeDesc * desc = GetWindowThemeDescriptor(classification);

            // Some windows don't have a theme set (e.g. main window, title screen)
            if (desc == nullptr)
            {
                return;
            }

            windowTheme = &desc->DefaultTheme;
        }

        for (sint32 i = 0; i < 6; i++) {
            window->colours[i] = windowTheme->Colours[i];
        }
        // Some windows need to be transparent even if the colours aren't.
        // There doesn't seem to be any side-effects for all windows being transparent
        window->flags |= WF_TRANSPARENT;
    }
}
