#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#pragma warning(disable : 4996) // GetVersionExA deprecated

#include <time.h>
#include "audio/audio.h"
#include "audio/AudioMixer.h"
#include "config/Config.h"
#include "Context.h"
#include "drawing/drawing.h"
#include "drawing/lightfx.h"
#include "editor.h"
#include "game.h"
#include "input.h"
#include "interface/chat.h"
#include "interface/console.h"
#include "interface/viewport.h"
#include "intro.h"
#include "localisation/date.h"
#include "localisation/localisation.h"
#include "management/news_item.h"
#include "network/network.h"
#include "network/twitch.h"
#include "object.h"
#include "object/ObjectManager.h"
#include "OpenRCT2.h"
#include "ParkImporter.h"
#include "peep/staff.h"
#include "platform/platform.h"
#include "rct1.h"
#include "ride/ride.h"
#include "ride/track.h"
#include "ride/track_design.h"
#include "ride/TrackDesignRepository.h"
#include "scenario/ScenarioRepository.h"
#include "title/TitleScreen.h"
#include "util/util.h"
#include "world/Climate.h"
#include "world/map.h"
#include "world/park.h"
#include "world/scenery.h"
#include "world/sprite.h"

// rct2: 0x0097F67C
static const char * const RCT2FilePaths[PATH_ID_END] = {
    "Data" PATH_SEPARATOR "g1.dat",
    "Data" PATH_SEPARATOR "plugin.dat",
    "Data" PATH_SEPARATOR "css1.dat",
    "Data" PATH_SEPARATOR "css2.dat",
    "Data" PATH_SEPARATOR "css4.dat",
    "Data" PATH_SEPARATOR "css5.dat",
    "Data" PATH_SEPARATOR "css6.dat",
    "Data" PATH_SEPARATOR "css7.dat",
    "Data" PATH_SEPARATOR "css8.dat",
    "Data" PATH_SEPARATOR "css9.dat",
    "Data" PATH_SEPARATOR "css11.dat",
    "Data" PATH_SEPARATOR "css12.dat",
    "Data" PATH_SEPARATOR "css13.dat",
    "Data" PATH_SEPARATOR "css14.dat",
    "Data" PATH_SEPARATOR "css15.dat",
    "Data" PATH_SEPARATOR "css3.dat",
    "Data" PATH_SEPARATOR "css17.dat",
    "Data" PATH_SEPARATOR "css18.dat",
    "Data" PATH_SEPARATOR "css19.dat",
    "Data" PATH_SEPARATOR "css20.dat",
    "Data" PATH_SEPARATOR "css21.dat",
    "Data" PATH_SEPARATOR "css22.dat",
    "Saved Games" PATH_SEPARATOR "scores.dat",
    "Data" PATH_SEPARATOR "css23.dat",
    "Data" PATH_SEPARATOR "css24.dat",
    "Data" PATH_SEPARATOR "css25.dat",
    "Data" PATH_SEPARATOR "css26.dat",
    "Data" PATH_SEPARATOR "css27.dat",
    "Data" PATH_SEPARATOR "css28.dat",
    "Data" PATH_SEPARATOR "css29.dat",
    "Data" PATH_SEPARATOR "css30.dat",
    "Data" PATH_SEPARATOR "css31.dat",
    "Data" PATH_SEPARATOR "css32.dat",
    "Data" PATH_SEPARATOR "css33.dat",
    "Data" PATH_SEPARATOR "css34.dat",
    "Data" PATH_SEPARATOR "css35.dat",
    "Data" PATH_SEPARATOR "css36.dat",
    "Data" PATH_SEPARATOR "css37.dat",
    "Data" PATH_SEPARATOR "css38.dat",
    "Data" PATH_SEPARATOR "CUSTOM1.WAV",
    "Data" PATH_SEPARATOR "CUSTOM2.WAV",
    "Data" PATH_SEPARATOR "css39.dat",
    "Data" PATH_SEPARATOR "css40.dat",
    "Data" PATH_SEPARATOR "css41.dat",
    "Scenarios" PATH_SEPARATOR "Six Flags Magic Mountain.SC6",
    "Data" PATH_SEPARATOR "css42.dat",
    "Data" PATH_SEPARATOR "css43.dat",
    "Data" PATH_SEPARATOR "css44.dat",
    "Data" PATH_SEPARATOR "css45.dat",
    "Data" PATH_SEPARATOR "css46.dat",
    "Data" PATH_SEPARATOR "css50.dat"
};

uint32 gCurrentDrawCount = 0;

uint8 gScreenFlags;
uint32 gScreenAge;
uint8 gSavePromptMode;

char gRCT2AddressAppPath[MAX_PATH];
char gRCT2AddressSavedGamesPath[MAX_PATH];
char gRCT2AddressScenariosPath[MAX_PATH];
char gRCT2AddressLandscapesPath[MAX_PATH];
char gRCT2AddressObjectDataPath[MAX_PATH];
char gRCT2AddressTracksPath[MAX_PATH];

typedef struct tm tm_t;

void print_launch_information();

void rct2_quit()
{
    gSavePromptMode = PM_QUIT;
    window_save_prompt_open();
}

/**
 *
 *  rct2: 0x00683499
 */
sint32 rct2_init_directories()
{
    // windows_get_registry_install_info((rct2_install_info*)0x009AA10C, "RollerCoaster Tycoon 2 Setup", "MS Sans Serif", 0);

    if (str_is_null_or_empty(gCustomRCT2DataPath)) {
        // check install directory
        if (gConfigGeneral.rct2_path == NULL || !platform_original_game_data_exists(gConfigGeneral.rct2_path)) {
            log_verbose("install directory does not exist or invalid directory selected, %s", gConfigGeneral.rct2_path);
            if (!config_find_or_browse_install_directory()) {
                utf8 path[MAX_PATH];
                config_get_default_path(path, sizeof(path));
                fprintf(stderr, "An RCT2 install directory must be specified! Please edit \"game_path\" in %s.\n", path);
                return 0;
            }
        }
        safe_strcpy(gRCT2AddressAppPath, gConfigGeneral.rct2_path, sizeof(gRCT2AddressAppPath));
    } else {
        safe_strcpy(gRCT2AddressAppPath, gCustomRCT2DataPath, sizeof(gRCT2AddressAppPath));
    }
    path_end_with_separator(gRCT2AddressAppPath, sizeof(gRCT2AddressAppPath));

    safe_strcpy(gRCT2AddressSavedGamesPath, gRCT2AddressAppPath, sizeof(gRCT2AddressSavedGamesPath));
    safe_strcat_path(gRCT2AddressSavedGamesPath, "Saved Games", sizeof(gRCT2AddressSavedGamesPath));
    path_end_with_separator(gRCT2AddressSavedGamesPath, sizeof(gRCT2AddressSavedGamesPath));

    safe_strcpy(gRCT2AddressScenariosPath, gRCT2AddressAppPath, sizeof(gRCT2AddressScenariosPath));
    safe_strcat_path(gRCT2AddressScenariosPath, "Scenarios", sizeof(gRCT2AddressScenariosPath));
    safe_strcat_path(gRCT2AddressScenariosPath, "*.SC6", sizeof(gRCT2AddressScenariosPath));

    safe_strcpy(gRCT2AddressLandscapesPath, gRCT2AddressAppPath, sizeof(gRCT2AddressLandscapesPath));
    safe_strcat_path(gRCT2AddressLandscapesPath, "Landscapes", sizeof(gRCT2AddressLandscapesPath));
    safe_strcat_path(gRCT2AddressLandscapesPath, "*.SC6", sizeof(gRCT2AddressLandscapesPath));

    safe_strcpy(gRCT2AddressObjectDataPath, gRCT2AddressAppPath, sizeof(gRCT2AddressObjectDataPath));
    safe_strcat_path(gRCT2AddressObjectDataPath, "ObjData", sizeof(gRCT2AddressObjectDataPath));
    safe_strcat_path(gRCT2AddressObjectDataPath, "*.DAT", sizeof(gRCT2AddressObjectDataPath));

    safe_strcpy(gRCT2AddressTracksPath, gRCT2AddressAppPath, sizeof(gRCT2AddressTracksPath));
    safe_strcat_path(gRCT2AddressTracksPath, "Tracks", sizeof(gRCT2AddressTracksPath));
    safe_strcat_path(gRCT2AddressTracksPath, "*.TD?", sizeof(gRCT2AddressTracksPath));
    return 1;
}

/**
 *
 *  rct2: 0x00674E6C
 */
const utf8 *get_file_path(sint32 pathId)
{
    static utf8 path[MAX_PATH];
    if (pathId == PATH_ID_CSS50 && !str_is_null_or_empty(gConfigGeneral.rct1_path)) {
        safe_strcpy(path, gConfigGeneral.rct1_path, sizeof(path));
        safe_strcat_path(path, RCT2FilePaths[PATH_ID_CSS17], sizeof(path));
    } else {
        safe_strcpy(path, gRCT2AddressAppPath, sizeof(path));
        safe_strcat_path(path, RCT2FilePaths[pathId], sizeof(path));
    }
    return path;
}

uint32 get_file_extension_type(const utf8 *path)
{
    const utf8 *extension = path_get_extension(path);
    if (strcicmp(extension, ".dat") == 0) return FILE_EXTENSION_DAT;
    if (strcicmp(extension, ".sc4") == 0) return FILE_EXTENSION_SC4;
    if (strcicmp(extension, ".sv4") == 0) return FILE_EXTENSION_SV4;
    if (strcicmp(extension, ".td4") == 0) return FILE_EXTENSION_TD4;
    if (strcicmp(extension, ".sc6") == 0) return FILE_EXTENSION_SC6;
    if (strcicmp(extension, ".sv6") == 0) return FILE_EXTENSION_SV6;
    if (strcicmp(extension, ".td6") == 0) return FILE_EXTENSION_TD6;
    return FILE_EXTENSION_UNKNOWN;
}
