#include <jansson.h>

extern "C"
{
    #include "../common.h"
    #include "window.h"
}

#include "../core/FileStream.hpp"
#include "../core/Math.hpp"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "../core/Util.hpp"

struct WindowThemeDesc;

// Don't try to load theme files that exceed 64 MiB
constexpr uint64 MAX_THEME_SIZE = 64 * 1024 * 1024;

enum {
    UITHEME_FLAG_PREDEFINED                            = 1 << 0,
    UITHEME_FLAG_USE_LIGHTS_RIDE                       = 1 << 1,
    UITHEME_FLAG_USE_LIGHTS_PARK                       = 1 << 2,
    UITHEME_FLAG_USE_ALTERNATIVE_SCENARIO_SELECT_FONT  = 1 << 3,
};

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
struct UIThemeEntry
{
    rct_windowclass WindowClass;
    WindowTheme     Theme;


           json_t *       ToJson() const;
    static UIThemeEntry * FromJson(const WindowThemeDesc * wtDesc, const json_t * json);
};

/**
 * Represents a user interface theme. Contains window colour schemes and appearance features.
 */
struct UITheme
{
    utf8 *         Name;
    UIThemeEntry * Entries;
    uint8          Flags;


           void      SetEntry(const UIThemeEntry * entry);
           void      RemoveEntry(rct_windowclass windowClass);
           json_t *  ToJson() const;
           bool      WriteToFile(const utf8 * path) const;
    static UITheme * Create(const utf8 * name);
    static void      Free(UITheme * theme);
    static UITheme * FromJson(const json_t * json);
    static UITheme * FromFile(const utf8 * path);
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
   // WindowClass, WindowClassSZ                   WindowName  NumColours, DefaultTheme
    { THEME_WC(WC_TOP_TOOLBAR),                    5245,       COLOURS_4(COLOUR_LIGHT_BLUE,               COLOUR_DARK_GREEN,               COLOUR_DARK_BROWN,             COLOUR_GREY         ) },
    { THEME_WC(WC_BOTTOM_TOOLBAR),                 5246,       COLOURS_4(TRANSLUCENT(COLOUR_DARK_GREEN),  TRANSLUCENT(COLOUR_DARK_GREEN),  COLOUR_BLACK,                  COLOUR_BRIGHT_GREEN ) },
    { THEME_WC(WC_RIDE),                           5203,       COLOURS_3(COLOUR_GREY,                     COLOUR_BORDEAUX_RED,             COLOUR_GREY                                        ) },
    { THEME_WC(WC_RIDE_CONSTRUCTION),              5199,       COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { THEME_WC(WC_RIDE_LIST),                      5204,       COLOURS_3(COLOUR_GREY,                     COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED                                ) },
    { THEME_WC(WC_SAVE_PROMPT),                    5223,       COLOURS_1(TRANSLUCENT(COLOUR_BORDEAUX_RED)                                                                                     ) },
    { THEME_WC(WC_CONSTRUCT_RIDE),                 5201,       COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED                                ) },
    { THEME_WC(WC_DEMOLISH_RIDE_PROMPT),           5224,       COLOURS_1(TRANSLUCENT(COLOUR_BORDEAUX_RED)                                                                                     ) },
    { THEME_WC(WC_SCENERY),                        5197,       COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_GREEN,               COLOUR_DARK_GREEN                                  ) },
    { THEME_WC(WC_OPTIONS),                        5219,       COLOURS_3(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                  ) },
    { THEME_WC(WC_FOOTPATH),                       5198,       COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { THEME_WC(WC_LAND),                           5193,       COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { THEME_WC(WC_WATER),                          5194,       COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { THEME_WC(WC_PEEP),                           5205,       COLOURS_3(COLOUR_GREY,                     COLOUR_OLIVE_GREEN,              COLOUR_OLIVE_GREEN                                 ) },
    { THEME_WC(WC_GUEST_LIST),                     5206,       COLOURS_3(COLOUR_GREY,                     COLOUR_OLIVE_GREEN,              COLOUR_OLIVE_GREEN                                 ) },
    { THEME_WC(WC_STAFF_LIST),                     5208,       COLOURS_3(COLOUR_GREY,                     COLOUR_LIGHT_PURPLE,             COLOUR_LIGHT_PURPLE                                ) },
    { THEME_WC(WC_FIRE_PROMPT),                    5225,       COLOURS_1(TRANSLUCENT(COLOUR_BORDEAUX_RED)                                                                                     ) },
    { THEME_WC(WC_PARK_INFORMATION),               5253,       COLOURS_3(COLOUR_GREY,                     COLOUR_DARK_YELLOW,              COLOUR_DARK_YELLOW                                 ) },
    { THEME_WC(WC_FINANCES),                       5187,       COLOURS_3(COLOUR_GREY,                     COLOUR_DARK_YELLOW,              COLOUR_DARK_YELLOW                                 ) },
    { THEME_WC(WC_TITLE_MENU),                     5249,       COLOURS_3(TRANSLUCENT(COLOUR_DARK_GREEN),  TRANSLUCENT(COLOUR_DARK_GREEN),  TRANSLUCENT(COLOUR_DARK_GREEN)                     ) },
    { THEME_WC(WC_TITLE_EXIT),                     5250,       COLOURS_3(TRANSLUCENT(COLOUR_DARK_GREEN),  TRANSLUCENT(COLOUR_DARK_GREEN),  TRANSLUCENT(COLOUR_DARK_GREEN)                     ) },
    { THEME_WC(WC_RECENT_NEWS),                    5192,       COLOURS_3(COLOUR_GREY,                     COLOUR_GREY,                     COLOUR_BLACK                                       ) },
    { THEME_WC(WC_SCENARIO_SELECT),                5252,       COLOURS_3(COLOUR_GREY,                     COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED                                ) },
    { THEME_WC(WC_TRACK_DESIGN_LIST),              5202,       COLOURS_3(COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED                                ) },
    { THEME_WC(WC_TRACK_DESIGN_PLACE),             5200,       COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { THEME_WC(WC_NEW_CAMPAIGN),                   5188,       COLOURS_3(COLOUR_DARK_YELLOW,              COLOUR_DARK_YELLOW,              COLOUR_DARK_YELLOW                                 ) },
    { THEME_WC(WC_KEYBOARD_SHORTCUT_LIST),         5220,       COLOURS_3(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                  ) },
    { THEME_WC(WC_CHANGE_KEYBOARD_SHORTCUT),       5221,       COLOURS_3(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                  ) },
    { THEME_WC(WC_MAP),                            5190,       COLOURS_2(COLOUR_DARK_GREEN,               COLOUR_DARK_BROWN                                                                   ) },
    { THEME_WC(WC_BANNER),                         5209,       COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { THEME_WC(WC_EDITOR_OBJECT_SELECTION),        5210,       COLOURS_3(COLOUR_LIGHT_PURPLE,             COLOUR_GREY,                     COLOUR_GREY                                        ) },
    { THEME_WC(WC_EDITOR_INVENTION_LIST),          5211,       COLOURS_3(COLOUR_LIGHT_PURPLE,             COLOUR_GREY,                     COLOUR_GREY                                        ) },
    { THEME_WC(WC_EDITOR_SCENARIO_OPTIONS),        5212,       COLOURS_3(COLOUR_LIGHT_PURPLE,             COLOUR_GREY,                     COLOUR_GREY                                        ) },
    { THEME_WC(WC_EDTIOR_OBJECTIVE_OPTIONS),       5213,       COLOURS_3(COLOUR_LIGHT_PURPLE,             COLOUR_GREY,                     COLOUR_GREY                                        ) },
    { THEME_WC(WC_MANAGE_TRACK_DESIGN),            5215,       COLOURS_3(COLOUR_GREY,                     COLOUR_GREY,                     COLOUR_GREY                                        ) },
    { THEME_WC(WC_TRACK_DELETE_PROMPT),            5226,       COLOURS_3(COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED                                ) },
    { THEME_WC(WC_INSTALL_TRACK),                  5216,       COLOURS_3(COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED,             COLOUR_BORDEAUX_RED                                ) },
    { THEME_WC(WC_CLEAR_SCENERY),                  5195,       COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { THEME_WC(WC_CHEATS),                         5217,       COLOURS_3(COLOUR_GREY,                     COLOUR_DARK_YELLOW,              COLOUR_DARK_YELLOW                                 ) },
    { THEME_WC(WC_RESEARCH),                       5189,       COLOURS_3(COLOUR_GREY,                     COLOUR_DARK_YELLOW,              COLOUR_DARK_YELLOW                                 ) },
    { THEME_WC(WC_VIEWPORT),                       5191,       COLOURS_3(COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { THEME_WC(WC_MAPGEN),                         5214,       COLOURS_3(COLOUR_DARK_GREEN,               COLOUR_DARK_BROWN,               COLOUR_DARK_BROWN                                  ) },
    { THEME_WC(WC_LOADSAVE),                       5222,       COLOURS_3(COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE,               COLOUR_LIGHT_BLUE                                  ) },
    { THEME_WC(WC_LOADSAVE_OVERWRITE_PROMPT),      5227,       COLOURS_1(TRANSLUCENT(COLOUR_BORDEAUX_RED)                                                                                     ) },
    { THEME_WC(WC_TITLE_OPTIONS),                  5251,       COLOURS_3(TRANSLUCENT(COLOUR_DARK_GREEN),  TRANSLUCENT(COLOUR_DARK_GREEN),  TRANSLUCENT(COLOUR_DARK_GREEN)                     ) },
    { THEME_WC(WC_LAND_RIGHTS),                    5196,       COLOURS_3(COLOUR_DARK_YELLOW,              COLOUR_DARK_YELLOW,              COLOUR_DARK_YELLOW                                 ) },
    { THEME_WC(WC_THEMES),                         5218,       COLOURS_3(COLOUR_GREY,                     COLOUR_DARK_GREEN,               COLOUR_DARK_GREEN                                  ) },
    { THEME_WC(WC_STAFF),                          5207,       COLOURS_3(COLOUR_GREY,                     COLOUR_LIGHT_PURPLE,             COLOUR_LIGHT_PURPLE                                ) },
    { THEME_WC(WC_EDITOR_TRACK_BOTTOM_TOOLBAR),    5247,       COLOURS_3(TRANSLUCENT(COLOUR_LIGHT_BLUE),  TRANSLUCENT(COLOUR_LIGHT_BLUE),  TRANSLUCENT(COLOUR_LIGHT_BLUE)                     ) },
    { THEME_WC(WC_EDITOR_SCENARIO_BOTTOM_TOOLBAR), 5248,       COLOURS_3(TRANSLUCENT(COLOUR_LIGHT_BROWN), TRANSLUCENT(COLOUR_LIGHT_BROWN), TRANSLUCENT(COLOUR_MOSS_GREEN)                     ) },
    { THEME_WC(WC_TITLE_EDITOR),                   5433,       COLOURS_3(COLOUR_GREY,                     COLOUR_OLIVE_GREEN,              COLOUR_OLIVE_GREEN                                 ) },
};

#pragma endregion

#pragma region Pre-defined Themes

#define COLOURS_RCT1(c0, c1, c2, c3, c4, c5) { { (c0), (c1), (c2), (c3), (c4), (c5) } }

UIThemeEntry PredefinedThemeRCT1_Entries[] =
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

UIThemeEntry PredefinedThemeRCT2_Entries[] =
{
    THEME_DEF_END
};

const UITheme PredefinedThemeRCT1 = { "RCT1", PredefinedThemeRCT1_Entries, UITHEME_FLAG_PREDEFINED };
const UITheme PredefinedThemeRCT2 = { "RCT2", PredefinedThemeRCT2_Entries, UITHEME_FLAG_PREDEFINED };

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

json_t * UIThemeEntry::ToJson() const
{
    const WindowThemeDesc * wtDesc = GetWindowThemeDescriptor(WindowClass);
        
    json_t * jsonColours = json_array();
    for (uint8 i = 0; i < wtDesc->NumColours; i++) {
        colour_t colour = Theme.Colours[i];
        json_array_append_new(jsonColours, json_integer(colour));
    }

    json_t * jsonEntry = json_object();
    json_object_set_new(jsonEntry, "colours", jsonColours);

    return jsonEntry;
}

UIThemeEntry * UIThemeEntry::FromJson(const WindowThemeDesc * wtDesc, const json_t * json)
{
    json_t * jsonColours = json_object_get(json, "colours");
    if (jsonColours == nullptr)
    {
        ThrowThemeLoadException();
    }

    uint8 numColours = (uint8)json_array_size(jsonColours);
    numColours = Math::Min(numColours, wtDesc->NumColours);

    UIThemeEntry * result = Memory::Allocate<UIThemeEntry>();
    result->WindowClass = wtDesc->WindowClass;
    result->Theme = wtDesc->DefaultTheme;
    
    for (uint8 i = 0; i < numColours; i++)
    {
        result->Theme.Colours[i] = (colour_t)json_integer_value(json_array_get(jsonColours, i));
    }

    return result;
}

UITheme * UITheme::Create(const utf8 * name)
{
    UITheme * theme = Memory::Allocate<UITheme>();
    theme->Name = String::Duplicate(name);
    theme->Entries = Memory::AllocateArray<UIThemeEntry>(1);
    theme->Entries[0].WindowClass = 255;
    theme->Flags = 0;
    return theme;
}

#pragma endregion

#pragma region UITheme

void UITheme::Free(UITheme * theme)
{
    assert(!(theme->Flags & UITHEME_FLAG_PREDEFINED));

    Memory::Free(theme->Name);
    Memory::Free(theme->Entries);
}

void UITheme::SetEntry(const UIThemeEntry * newEntry)
{
    // Try to replace existing entry
    size_t numEntries = 0;
    for (UIThemeEntry * entry = Entries; entry->WindowClass != 0xFF; entry++)
    {
        if (entry->WindowClass == newEntry->WindowClass)
        {
            *entry = *newEntry;
            return;
        }

        numEntries++;
    }

    // Increase size of entry list
    Entries = Memory::ReallocateArray(Entries, numEntries + 2);
    Entries[numEntries] = *newEntry;
    Entries[numEntries + 1].WindowClass = 255;
}

void UITheme::RemoveEntry(rct_windowclass windowClass)
{
    // Remove existing entry
    bool found = false;
    for (UIThemeEntry * entry = Entries; entry->WindowClass != 0xFF; entry++)
    {
        if (entry->WindowClass == windowClass)
        {
            for (UIThemeEntry * entry2 = entry; entry2->WindowClass != 0xFF; entry2++)
            {
                *entry2 = *(entry2 + 1);
            }
            found = true;
            break;
        }
    }
    if (!found) return;

    // Reduce size of entry list
    size_t numEntries = 0;
    for (UIThemeEntry * entry = Entries; entry->WindowClass != 0xFF; entry++)
    {
        numEntries++;
    }
    Entries = Memory::ReallocateArray(Entries, numEntries);
}

json_t * UITheme::ToJson() const
{
    // Create entries
    json_t * jsonEntries = json_object();
    for (UIThemeEntry * entry = Entries; entry->WindowClass != 0xFF; entry++)
    {
        const WindowThemeDesc * wtDesc = GetWindowThemeDescriptor(entry->WindowClass);
        json_object_set_new(jsonEntries, wtDesc->WindowClassSZ, entry->ToJson());
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
    const char * jsonOutput;

    // Serialise theme object as JSON
    json_t * jsonTheme = ToJson();
    jsonOutput = json_dumps(jsonTheme, JSON_INDENT(4));
    json_decref(jsonTheme);

    try
    {
        auto fs = FileStream(path, FILE_MODE_WRITE);
        size_t jsonOutputSize = String::SizeOf(jsonOutput);
        fs.Write(jsonOutput, jsonOutputSize);

        return true;
    }
    catch (Exception ex)
    {
        log_error("Unable to save %s: %s", path, ex.GetMessage());
        return false;
    }
}

UITheme * UITheme::FromJson(const json_t * json)
{
    const char * themeName = json_string_value(json_object_get(json, "name"));
    if (themeName == nullptr)
    {
        ThrowThemeLoadException();
    }

    json_t * jsonEntries = json_object_get(json, "entries");
    size_t numEntries = json_object_size(jsonEntries);
    if (numEntries == 0)
    {
        ThrowThemeLoadException();
    }

    UITheme * result = nullptr;
    try
    {
        result = UITheme::Create(themeName);

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
        size_t       i = 0;
        json_object_foreach(jsonEntries, jkey, jvalue)
        {
            const WindowThemeDesc * wtDesc = GetWindowThemeDescriptor(jkey);
            if (wtDesc == nullptr) continue;

            UIThemeEntry * entry = UIThemeEntry::FromJson(wtDesc, jvalue);
            result->SetEntry(entry);
            Memory::Free(entry);
        }

        return result;
    }
    catch (Exception ex)
    {
        UITheme::Free(result);
        throw ex;
    }
}

UITheme * UITheme::FromFile(const utf8 * path)
{
    json_t  * json = nullptr;
    UITheme * result = nullptr;
    try
    {
        auto fs = FileStream(path, FILE_MODE_OPEN);

        size_t fileLength = (size_t)fs.GetLength();
        if (fileLength > MAX_THEME_SIZE)
        {
            throw IOException("Language file too large.");
        }

        utf8 * fileData = Memory::Allocate<utf8>(fileLength + 1);
        fs.Read(fileData, fileLength);
        fileData[fileLength] = '\0';

        json_error_t jsonLoadError;
        json = json_loads(fileData, 0, &jsonLoadError);
        Memory::Free(fileData);

        if (json != nullptr)
        {
            result = UITheme::FromJson(json);
        }
    }
    catch (Exception ex)
    {
        result = nullptr;
    }

    json_decref(json);
    return result;
}

#pragma endregion

