/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Game.h"

#include "Cheats.h"
#include "Context.h"
#include "Editor.h"
#include "FileClassifier.h"
#include "GameStateSnapshots.h"
#include "Input.h"
#include "OpenRCT2.h"
#include "ParkImporter.h"
#include "ReplayManager.h"
#include "actions/LoadOrQuitAction.hpp"
#include "audio/audio.h"
#include "config/Config.h"
#include "core/FileScanner.h"
#include "interface/Screenshot.h"
#include "interface/Viewport.h"
#include "interface/Window.h"
#include "localisation/Localisation.h"
#include "management/Finance.h"
#include "management/Marketing.h"
#include "management/NewsItem.h"
#include "management/Research.h"
#include "network/network.h"
#include "object/Object.h"
#include "object/ObjectList.h"
#include "peep/Peep.h"
#include "peep/Staff.h"
#include "platform/platform.h"
#include "rct1/RCT1.h"
#include "ride/Ride.h"
#include "ride/RideRatings.h"
#include "ride/Station.h"
#include "ride/Track.h"
#include "ride/TrackDesign.h"
#include "ride/Vehicle.h"
#include "scenario/Scenario.h"
#include "title/TitleScreen.h"
#include "ui/UiContext.h"
#include "ui/WindowManager.h"
#include "util/SawyerCoding.h"
#include "util/Util.h"
#include "windows/Intent.h"
#include "world/Banner.h"
#include "world/Climate.h"
#include "world/Entrance.h"
#include "world/Footpath.h"
#include "world/Map.h"
#include "world/MapAnimation.h"
#include "world/Park.h"
#include "world/Scenery.h"
#include "world/Sprite.h"
#include "world/Surface.h"
#include "world/Water.h"

#include <algorithm>
#include <iterator>
#include <memory>

uint16_t gCurrentDeltaTime;
uint8_t gGamePaused = 0;
int32_t gGameSpeed = 1;
bool gDoSingleUpdate = false;
float gDayNightCycle = 0;
bool gInUpdateCode = false;
bool gInMapInitCode = false;
std::string gCurrentLoadedPath;

bool gLoadKeepWindowsOpen = false;

uint32_t gCurrentTicks;
uint32_t gCurrentRealTimeTicks;

rct_string_id gGameCommandErrorTitle;
rct_string_id gGameCommandErrorText;

using namespace OpenRCT2;

void game_increase_game_speed()
{
    gGameSpeed = std::min(gConfigGeneral.debugging_tools ? 5 : 4, gGameSpeed + 1);
    if (gGameSpeed == 5)
        gGameSpeed = 8;
    window_invalidate_by_class(WC_TOP_TOOLBAR);
}

void game_reduce_game_speed()
{
    gGameSpeed = std::max(1, gGameSpeed - 1);
    if (gGameSpeed == 7)
        gGameSpeed = 4;
    window_invalidate_by_class(WC_TOP_TOOLBAR);
}

/**
 *
 *  rct2: 0x0066B5C0 (part of 0x0066B3E8)
 */
void game_create_windows()
{
    context_open_window(WC_MAIN_WINDOW);
    context_open_window(WC_TOP_TOOLBAR);
    context_open_window(WC_BOTTOM_TOOLBAR);
    window_resize_gui(context_get_width(), context_get_height());
}

enum
{
    SPR_GAME_PALETTE_DEFAULT = 1532,
    SPR_GAME_PALETTE_WATER = 1533,
    SPR_GAME_PALETTE_WATER_DARKER_1 = 1534,
    SPR_GAME_PALETTE_WATER_DARKER_2 = 1535,
    SPR_GAME_PALETTE_3 = 1536,
    SPR_GAME_PALETTE_3_DARKER_1 = 1537,
    SPR_GAME_PALETTE_3_DARKER_2 = 1538,
    SPR_GAME_PALETTE_4 = 1539,
    SPR_GAME_PALETTE_4_DARKER_1 = 1540,
    SPR_GAME_PALETTE_4_DARKER_2 = 1541,
};

/**
 *
 *  rct2: 0x006838BD
 */
void update_palette_effects()
{
    auto water_type = (rct_water_type*)object_entry_get_chunk(OBJECT_TYPE_WATER, 0);

    if (gClimateLightningFlash == 1)
    {
        // Change palette to lighter colour during lightning
        int32_t palette = SPR_GAME_PALETTE_DEFAULT;

        if (water_type != nullptr)
        {
            palette = water_type->image_id;
        }
        const rct_g1_element* g1 = gfx_get_g1_element(palette);
        if (g1 != nullptr)
        {
            int32_t xoffset = g1->x_offset;
            xoffset = xoffset * 4;
            uint8_t* paletteOffset = gGamePalette + xoffset;
            for (int32_t i = 0; i < g1->width; i++)
            {
                paletteOffset[(i * 4) + 0] = -((0xFF - g1->offset[(i * 3) + 0]) / 2) - 1;
                paletteOffset[(i * 4) + 1] = -((0xFF - g1->offset[(i * 3) + 1]) / 2) - 1;
                paletteOffset[(i * 4) + 2] = -((0xFF - g1->offset[(i * 3) + 2]) / 2) - 1;
            }
            platform_update_palette(gGamePalette, 10, 236);
        }
        gClimateLightningFlash++;
    }
    else
    {
        if (gClimateLightningFlash == 2)
        {
            // Change palette back to normal after lightning
            int32_t palette = SPR_GAME_PALETTE_DEFAULT;

            if (water_type != nullptr)
            {
                palette = water_type->image_id;
            }

            const rct_g1_element* g1 = gfx_get_g1_element(palette);
            if (g1 != nullptr)
            {
                int32_t xoffset = g1->x_offset;
                xoffset = xoffset * 4;
                uint8_t* paletteOffset = gGamePalette + xoffset;
                for (int32_t i = 0; i < g1->width; i++)
                {
                    paletteOffset[(i * 4) + 0] = g1->offset[(i * 3) + 0];
                    paletteOffset[(i * 4) + 1] = g1->offset[(i * 3) + 1];
                    paletteOffset[(i * 4) + 2] = g1->offset[(i * 3) + 2];
                }
            }
        }

        // Animate the water/lava/chain movement palette
        uint32_t shade = 0;
        if (gConfigGeneral.render_weather_gloom)
        {
            auto paletteId = climate_get_weather_gloom_palette_id(gClimateCurrent);
            if (paletteId != PALETTE_NULL)
            {
                shade = 1;
                if (paletteId != PALETTE_DARKEN_1)
                {
                    shade = 2;
                }
            }
        }
        uint32_t j = gPaletteEffectFrame;
        j = (((uint16_t)((~j / 2) * 128) * 15) >> 16);
        uint32_t waterId = SPR_GAME_PALETTE_WATER;
        if (water_type != nullptr)
        {
            waterId = water_type->palette_index_1;
        }
        const rct_g1_element* g1 = gfx_get_g1_element(shade + waterId);
        if (g1 != nullptr)
        {
            uint8_t* vs = &g1->offset[j * 3];
            uint8_t* vd = &gGamePalette[230 * 4];
            int32_t n = 5;
            for (int32_t i = 0; i < n; i++)
            {
                vd[0] = vs[0];
                vd[1] = vs[1];
                vd[2] = vs[2];
                vs += 9;
                if (vs >= &g1->offset[9 * n])
                {
                    vs -= 9 * n;
                }
                vd += 4;
            }
        }

        waterId = SPR_GAME_PALETTE_3;
        if (water_type != nullptr)
        {
            waterId = water_type->palette_index_2;
        }
        g1 = gfx_get_g1_element(shade + waterId);
        if (g1 != nullptr)
        {
            uint8_t* vs = &g1->offset[j * 3];
            uint8_t* vd = &gGamePalette[235 * 4];
            int32_t n = 5;
            for (int32_t i = 0; i < n; i++)
            {
                vd[0] = vs[0];
                vd[1] = vs[1];
                vd[2] = vs[2];
                vs += 9;
                if (vs >= &g1->offset[9 * n])
                {
                    vs -= 9 * n;
                }
                vd += 4;
            }
        }

        j = ((uint16_t)(gPaletteEffectFrame * -960) * 3) >> 16;
        waterId = SPR_GAME_PALETTE_4;
        g1 = gfx_get_g1_element(shade + waterId);
        if (g1 != nullptr)
        {
            uint8_t* vs = &g1->offset[j * 3];
            uint8_t* vd = &gGamePalette[243 * 4];
            int32_t n = 3;
            for (int32_t i = 0; i < n; i++)
            {
                vd[0] = vs[0];
                vd[1] = vs[1];
                vd[2] = vs[2];
                vs += 3;
                if (vs >= &g1->offset[3 * n])
                {
                    vs -= 3 * n;
                }
                vd += 4;
            }
        }

        platform_update_palette(gGamePalette, 230, 16);
        if (gClimateLightningFlash == 2)
        {
            platform_update_palette(gGamePalette, 10, 236);
            gClimateLightningFlash = 0;
        }
    }
}

void pause_toggle()
{
    gGamePaused ^= GAME_PAUSED_NORMAL;
    window_invalidate_by_class(WC_TOP_TOOLBAR);
    if (gGamePaused & GAME_PAUSED_NORMAL)
    {
        audio_stop_all_music_and_sounds();
    }
}

bool game_is_paused()
{
    return gGamePaused != 0;
}

bool game_is_not_paused()
{
    return gGamePaused == 0;
}

/**
 *
 *  rct2: 0x0066DC0F
 */
static void load_landscape()
{
    auto intent = Intent(WC_LOADSAVE);
    intent.putExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_LOAD | LOADSAVETYPE_LANDSCAPE);
    context_open_intent(&intent);
}

void utf8_to_rct2_self(char* buffer, size_t length)
{
    auto temp = utf8_to_rct2(buffer);

    size_t i = 0;
    const char* src = temp.data();
    char* dst = buffer;
    while (*src != 0 && i < length - 1)
    {
        if (*src == (char)(uint8_t)0xFF)
        {
            if (i < length - 3)
            {
                *dst++ = *src++;
                *dst++ = *src++;
                *dst++ = *src++;
            }
            else
            {
                break;
            }
            i += 3;
        }
        else
        {
            *dst++ = *src++;
            i++;
        }
    }
    do
    {
        *dst++ = '\0';
        i++;
    } while (i < length);
}

void rct2_to_utf8_self(char* buffer, size_t length)
{
    if (length > 0)
    {
        auto temp = rct2_to_utf8(buffer, RCT2_LANGUAGE_ID_ENGLISH_UK);
        safe_strcpy(buffer, temp.data(), length);
    }
}

/**
 * Converts all the user strings and news item strings to UTF-8.
 */
void game_convert_strings_to_utf8()
{
    // Scenario details
    gScenarioCompletedBy = rct2_to_utf8(gScenarioCompletedBy, RCT2_LANGUAGE_ID_ENGLISH_UK);
    gScenarioName = rct2_to_utf8(gScenarioName, RCT2_LANGUAGE_ID_ENGLISH_UK);
    gScenarioDetails = rct2_to_utf8(gScenarioDetails, RCT2_LANGUAGE_ID_ENGLISH_UK);

    // News items
    game_convert_news_items_to_utf8();
}

void game_convert_news_items_to_utf8()
{
    for (int32_t i = 0; i < MAX_NEWS_ITEMS; i++)
    {
        NewsItem* newsItem = news_item_get(i);

        if (!str_is_null_or_empty(newsItem->Text))
        {
            rct2_to_utf8_self(newsItem->Text, sizeof(newsItem->Text));
        }
    }
}

/**
 * Converts all the user strings and news item strings to RCT2 encoding.
 */
void game_convert_strings_to_rct2(rct_s6_data* s6)
{
    // Scenario details
    utf8_to_rct2_self(s6->scenario_completed_name, sizeof(s6->scenario_completed_name));
    utf8_to_rct2_self(s6->scenario_name, sizeof(s6->scenario_name));
    utf8_to_rct2_self(s6->scenario_description, sizeof(s6->scenario_description));

    // User strings
    for (auto* userString : s6->custom_strings)
    {
        if (!str_is_null_or_empty(userString))
        {
            utf8_to_rct2_self(userString, RCT12_USER_STRING_MAX_LENGTH);
        }
    }

    // News items
    for (auto& newsItem : s6->news_items)
    {
        if (!str_is_null_or_empty(newsItem.Text))
        {
            utf8_to_rct2_self(newsItem.Text, sizeof(newsItem.Text));
        }
    }
}

// OpenRCT2 workaround to recalculate some values which are saved redundantly in the save to fix corrupted files.
// For example recalculate guest count by looking at all the guests instead of trusting the value in the file.
void game_fix_save_vars()
{
    // Recalculates peep count after loading a save to fix corrupted files
    Peep* peep;
    uint16_t spriteIndex;
    uint16_t peepCount = 0;
    FOR_ALL_GUESTS (spriteIndex, peep)
    {
        if (!peep->outside_of_park)
            peepCount++;
    }

    gNumGuestsInPark = peepCount;

    peep_sort();

    // Peeps to remove have to be cached here, as removing them from within the loop breaks iteration
    std::vector<Peep*> peepsToRemove;

    // Fix possibly invalid field values
    FOR_ALL_GUESTS (spriteIndex, peep)
    {
        if (peep->current_ride_station >= MAX_STATIONS)
        {
            const uint8_t srcStation = peep->current_ride_station;
            const uint8_t rideIdx = peep->current_ride;
            if (rideIdx == RIDE_ID_NULL)
            {
                continue;
            }
            Ride* ride = get_ride(rideIdx);
            if (ride == nullptr)
            {
                log_warning("Couldn't find ride %u, resetting ride on peep %u", rideIdx, spriteIndex);
                peep->current_ride = RIDE_ID_NULL;
                continue;
            }
            set_format_arg(0, uint32_t, peep->id);
            auto curName = peep->GetName();
            log_warning(
                "Peep %u (%s) has invalid ride station = %u for ride %u.", spriteIndex, curName.c_str(), srcStation, rideIdx);
            auto station = ride_get_first_valid_station_exit(ride);
            if (station == STATION_INDEX_NULL)
            {
                log_warning("Couldn't find station, removing peep %u", spriteIndex);
                peepsToRemove.push_back(peep);
            }
            else
            {
                log_warning("Amending ride station to %u.", station);
                peep->current_ride_station = station;
            }
        }
    }

    if (!peepsToRemove.empty())
    {
        // Some broken saves have broken spatial indexes
        reset_sprite_spatial_index();
    }

    for (auto ptr : peepsToRemove)
    {
        ptr->Remove();
    }

    // Fixes broken saves where a surface element could be null
    // and broken saves with incorrect invisible map border tiles
    for (int32_t y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; y++)
    {
        for (int32_t x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; x++)
        {
            auto* surfaceElement = map_get_surface_element_at(TileCoordsXY{ x, y }.ToCoordsXY());

            if (surfaceElement == nullptr)
            {
                log_error("Null map element at x = %d and y = %d. Fixing...", x, y);
                auto tileElement = tile_element_insert(TileCoordsXYZ{ x, y, 14 }.ToCoordsXYZ(), 0b0000);
                if (tileElement == nullptr)
                {
                    log_error("Unable to fix: Map element limit reached.");
                    return;
                }
                surfaceElement = tileElement->AsSurface();
            }

            // Fix the invisible border tiles.
            // At this point, we can be sure that surfaceElement is not NULL.
            if (x == 0 || x == gMapSize - 1 || y == 0 || y == gMapSize - 1)
            {
                surfaceElement->SetBaseZ(MINIMUM_LAND_HEIGHT_BIG);
                surfaceElement->SetClearanceZ(MINIMUM_LAND_HEIGHT_BIG);
                surfaceElement->SetSlope(0);
                surfaceElement->SetWaterHeight(0);
            }
        }
    }

    research_fix();

    // Fix banner list pointing to NULL map elements
    banner_reset_broken_index();

    // Fix banners which share their index
    fix_duplicated_banners();

    // Fix invalid vehicle sprite sizes, thus preventing visual corruption of sprites
    fix_invalid_vehicle_sprite_sizes();

    // Fix gParkEntrance locations for which the tile_element no longer exists
    fix_park_entrance_locations();
}

void game_load_init()
{
    rct_window* mainWindow;

    IGameStateSnapshots* snapshots = GetContext()->GetGameStateSnapshots();
    snapshots->Reset();

    gScreenFlags = SCREEN_FLAGS_PLAYING;
    audio_stop_all_music_and_sounds();
    if (!gLoadKeepWindowsOpen)
    {
        viewport_init_all();
        game_create_windows();
        mainWindow = window_get_main();
    }
    else
    {
        mainWindow = window_get_main();
        window_unfollow_sprite(mainWindow);
    }

    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    windowManager->SetMainView(gSavedView, gSavedViewZoom, gSavedViewRotation);

    if (network_get_mode() != NETWORK_MODE_CLIENT)
    {
        GameActions::ClearQueue();
        reset_sprite_spatial_index();
    }
    reset_all_sprite_quadrant_placements();
    scenery_set_default_placement_configuration();

    auto intent = Intent(INTENT_ACTION_REFRESH_NEW_RIDES);
    context_broadcast_intent(&intent);

    gWindowUpdateTicks = 0;

    load_palette();

    if (!gOpenRCT2Headless)
    {
        intent = Intent(INTENT_ACTION_CLEAR_TILE_INSPECTOR_CLIPBOARD);
        context_broadcast_intent(&intent);
        window_update_all();
    }

    audio_stop_title_music();
    gGameSpeed = 1;
}

/**
 *
 *  rct2: 0x0069E9A7
 * Call after a rotation or loading of a save to reset sprite quadrants
 */
void reset_all_sprite_quadrant_placements()
{
    for (size_t i = 0; i < MAX_SPRITES; i++)
    {
        rct_sprite* spr = get_sprite(i);
        if (spr->generic.sprite_identifier != SPRITE_IDENTIFIER_NULL)
        {
            sprite_move(spr->generic.x, spr->generic.y, spr->generic.z, &spr->generic);
        }
    }
}

void save_game()
{
    if (!gFirstTimeSaving)
    {
        save_game_with_name(gScenarioSavePath.c_str());
    }
    else
    {
        save_game_as();
    }
}

void save_game_cmd(const utf8* name /* = nullptr */)
{
    if (name == nullptr)
    {
        save_game_with_name(gScenarioSavePath.c_str());
    }
    else
    {
        char savePath[MAX_PATH];
        platform_get_user_directory(savePath, "save", sizeof(savePath));
        safe_strcat_path(savePath, name, sizeof(savePath));
        path_append_extension(savePath, ".sv6", sizeof(savePath));
        save_game_with_name(savePath);
    }
}

void save_game_with_name(const utf8* name)
{
    log_verbose("Saving to %s", name);
    if (scenario_save(name, 0x80000000 | (gConfigGeneral.save_plugin_data ? 1 : 0)))
    {
        log_verbose("Saved to %s", name);
        gCurrentLoadedPath = name;
        gScreenAge = 0;
    }
}

void* create_save_game_as_intent()
{
    char name[MAX_PATH];
    safe_strcpy(name, path_get_filename(gScenarioSavePath.c_str()), MAX_PATH);
    path_remove_extension(name);

    Intent* intent = new Intent(WC_LOADSAVE);
    intent->putExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_SAVE | LOADSAVETYPE_GAME);
    intent->putExtra(INTENT_EXTRA_PATH, std::string{ name });

    return intent;
}

void save_game_as()
{
    auto* intent = (Intent*)create_save_game_as_intent();
    context_open_intent(intent);
    delete intent;
}

static int32_t compare_autosave_file_paths(const void* a, const void* b)
{
    return strcmp(*(char**)a, *(char**)b);
}

static void limit_autosave_count(const size_t numberOfFilesToKeep, bool processLandscapeFolder)
{
    size_t autosavesCount = 0;
    size_t numAutosavesToDelete = 0;

    utf8 filter[MAX_PATH];

    utf8** autosaveFiles = nullptr;

    if (processLandscapeFolder)
    {
        platform_get_user_directory(filter, "landscape", sizeof(filter));
        safe_strcat_path(filter, "autosave", sizeof(filter));
        safe_strcat_path(filter, "autosave_*.sc6", sizeof(filter));
    }
    else
    {
        platform_get_user_directory(filter, "save", sizeof(filter));
        safe_strcat_path(filter, "autosave", sizeof(filter));
        safe_strcat_path(filter, "autosave_*.sv6", sizeof(filter));
    }

    // At first, count how many autosaves there are
    {
        auto scanner = std::unique_ptr<IFileScanner>(Path::ScanDirectory(filter, false));
        while (scanner->Next())
        {
            autosavesCount++;
        }
    }

    // If there are fewer autosaves than the number of files to keep we don't need to delete anything
    if (autosavesCount <= numberOfFilesToKeep)
    {
        return;
    }

    autosaveFiles = (utf8**)malloc(sizeof(utf8*) * autosavesCount);

    {
        auto scanner = std::unique_ptr<IFileScanner>(Path::ScanDirectory(filter, false));
        for (size_t i = 0; i < autosavesCount; i++)
        {
            autosaveFiles[i] = (utf8*)malloc(sizeof(utf8) * MAX_PATH);
            std::memset(autosaveFiles[i], 0, sizeof(utf8) * MAX_PATH);

            if (scanner->Next())
            {
                if (processLandscapeFolder)
                {
                    platform_get_user_directory(autosaveFiles[i], "landscape", sizeof(utf8) * MAX_PATH);
                }
                else
                {
                    platform_get_user_directory(autosaveFiles[i], "save", sizeof(utf8) * MAX_PATH);
                }
                safe_strcat_path(autosaveFiles[i], "autosave", sizeof(utf8) * MAX_PATH);
                safe_strcat_path(autosaveFiles[i], scanner->GetPathRelative(), sizeof(utf8) * MAX_PATH);
            }
        }
    }

    qsort(autosaveFiles, autosavesCount, sizeof(char*), compare_autosave_file_paths);

    // Calculate how many saves we need to delete.
    numAutosavesToDelete = autosavesCount - numberOfFilesToKeep;

    for (size_t i = 0; numAutosavesToDelete > 0; i++, numAutosavesToDelete--)
    {
        platform_file_delete(autosaveFiles[i]);
    }

    for (size_t i = 0; i < autosavesCount; i++)
    {
        free(autosaveFiles[i]);
    }

    free(autosaveFiles);
}

void game_autosave()
{
    const char* subDirectory = "save";
    const char* fileExtension = ".sv6";
    uint32_t saveFlags = 0x80000000;
    if (gScreenFlags & SCREEN_FLAGS_EDITOR)
    {
        subDirectory = "landscape";
        fileExtension = ".sc6";
        saveFlags |= 2;
    }

    // Retrieve current time
    rct2_date currentDate;
    platform_get_date_local(&currentDate);
    rct2_time currentTime;
    platform_get_time_local(&currentTime);

    utf8 timeName[44];
    snprintf(
        timeName, sizeof(timeName), "autosave_%04u-%02u-%02u_%02u-%02u-%02u%s", currentDate.year, currentDate.month,
        currentDate.day, currentTime.hour, currentTime.minute, currentTime.second, fileExtension);

    int32_t autosavesToKeep = gConfigGeneral.autosave_amount;
    limit_autosave_count(autosavesToKeep - 1, (gScreenFlags & SCREEN_FLAGS_EDITOR));

    utf8 path[MAX_PATH];
    utf8 backupPath[MAX_PATH];
    platform_get_user_directory(path, subDirectory, sizeof(path));
    safe_strcat_path(path, "autosave", sizeof(path));
    platform_ensure_directory_exists(path);
    safe_strcpy(backupPath, path, sizeof(backupPath));
    safe_strcat_path(path, timeName, sizeof(path));
    safe_strcat_path(backupPath, "autosave", sizeof(backupPath));
    safe_strcat(backupPath, fileExtension, sizeof(backupPath));
    safe_strcat(backupPath, ".bak", sizeof(backupPath));

    if (platform_file_exists(path))
    {
        platform_file_copy(path, backupPath, true);
    }

    scenario_save(path, saveFlags);
}

static void game_load_or_quit_no_save_prompt_callback(int32_t result, const utf8* path)
{
    if (result == MODAL_RESULT_OK)
    {
        window_close_by_class(WC_EDITOR_OBJECT_SELECTION);
        context_load_park_from_file(path);
    }
}

/**
 *
 *  rct2: 0x0066DB79
 */
void game_load_or_quit_no_save_prompt()
{
    switch (gSavePromptMode)
    {
        case PM_SAVE_BEFORE_LOAD:
        {
            auto loadOrQuitAction = LoadOrQuitAction(LoadOrQuitModes::CloseSavePrompt);
            GameActions::Execute(&loadOrQuitAction);
            tool_cancel();
            if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
            {
                load_landscape();
            }
            else
            {
                auto intent = Intent(WC_LOADSAVE);
                intent.putExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_LOAD | LOADSAVETYPE_GAME);
                intent.putExtra(INTENT_EXTRA_CALLBACK, (void*)game_load_or_quit_no_save_prompt_callback);
                context_open_intent(&intent);
            }
            break;
        }
        case PM_SAVE_BEFORE_QUIT:
        {
            auto loadOrQuitAction = LoadOrQuitAction(LoadOrQuitModes::CloseSavePrompt);
            GameActions::Execute(&loadOrQuitAction);
            tool_cancel();
            if (input_test_flag(INPUT_FLAG_5))
            {
                input_set_flag(INPUT_FLAG_5, false);
            }
            gGameSpeed = 1;
            gFirstTimeSaving = true;
            title_load();
            break;
        }
        default:
            openrct2_finish();
            break;
    }
}
