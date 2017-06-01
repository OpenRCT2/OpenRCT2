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

#ifndef _RCT2_H_
#define _RCT2_H_

#include "common.h"

typedef struct rct2_install_info {
    uint32 installLevel;
    char title[260];
    char path[260];
    uint32 var_20C;
    uint8 pad_210[256];
    char expansionPackNames[16][128];
    uint32 activeExpansionPacks;        //0xB10
} rct2_install_info;

#pragma pack(push, 1)

#ifdef __cplusplus

/**
 * scores.dat file header.
 * size: 0x10
 */
struct rct_scores_header
{
    uint32 var_0;
    uint32 var_4;
    uint32 var_8;
    uint32 ScenarioCount;
};
assert_struct_size(rct_scores_header, 0x10);

/**
 * An entry of scores.dat
 * size: 0x02B0
 */
struct rct_scores_entry
{
    char    Path[256];
    uint8   Category;
    uint8   pad_0101[0x1F];
    sint8   ObjectiveType;
    sint8   ObjectiveArg1;
    sint32  objectiveArg2;
    sint16  objectiveArg3;
    char    Name[64];
    char    Details[256];
    sint32  Flags;
    money32 CompanyValue;
    char    CompletedBy[64];
};
assert_struct_size(rct_scores_entry, 0x02B0);

#endif // __cplusplus

#pragma pack(pop)

enum {
    // Although this is labeled a flag it actually means when
    // zero the screen is in playing mode.
    SCREEN_FLAGS_PLAYING = 0,
    SCREEN_FLAGS_TITLE_DEMO = 1,
    SCREEN_FLAGS_SCENARIO_EDITOR = 2,
    SCREEN_FLAGS_TRACK_DESIGNER = 4,
    SCREEN_FLAGS_TRACK_MANAGER = 8,
};

#define SCREEN_FLAGS_EDITOR (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)

enum {
    PATH_ID_G1,
    PATH_ID_PLUGIN,
    PATH_ID_CSS1,
    PATH_ID_CSS2,
    PATH_ID_CSS4,
    PATH_ID_CSS5,
    PATH_ID_CSS6,
    PATH_ID_CSS7,
    PATH_ID_CSS8,
    PATH_ID_CSS9,
    PATH_ID_CSS11,
    PATH_ID_CSS12,
    PATH_ID_CSS13,
    PATH_ID_CSS14,
    PATH_ID_CSS15,
    PATH_ID_CSS3,
    PATH_ID_CSS17,
    PATH_ID_CSS18,
    PATH_ID_CSS19,
    PATH_ID_CSS20,
    PATH_ID_CSS21,
    PATH_ID_CSS22,
    PATH_ID_SCORES,
    PATH_ID_CSS23,
    PATH_ID_CSS24,
    PATH_ID_CSS25,
    PATH_ID_CSS26,
    PATH_ID_CSS27,
    PATH_ID_CSS28,
    PATH_ID_CSS29,
    PATH_ID_CSS30,
    PATH_ID_CSS31,
    PATH_ID_CSS32,
    PATH_ID_CSS33,
    PATH_ID_CSS34,
    PATH_ID_CSS35,
    PATH_ID_CSS36,
    PATH_ID_CSS37,
    PATH_ID_CSS38,
    PATH_ID_CUSTOM1,
    PATH_ID_CUSTOM2,
    PATH_ID_CSS39,
    PATH_ID_CSS40,
    PATH_ID_CSS41,
    PATH_ID_SIXFLAGS_MAGICMOUNTAIN,
    PATH_ID_CSS42,
    PATH_ID_CSS43,
    PATH_ID_CSS44,
    PATH_ID_CSS45,
    PATH_ID_CSS46,
    PATH_ID_CSS50,
    PATH_ID_END
};

enum {
    FILE_EXTENSION_UNKNOWN,
    FILE_EXTENSION_DAT,
    FILE_EXTENSION_SC4,
    FILE_EXTENSION_SV4,
    FILE_EXTENSION_TD4,
    FILE_EXTENSION_SC6,
    FILE_EXTENSION_SV6,
    FILE_EXTENSION_TD6,
};

#ifdef __cplusplus
extern "C" {
#endif


extern const char * const RCT2FilePaths[PATH_ID_END];

extern uint32 gCurrentDrawCount;

extern uint8 gScreenFlags;
extern uint32 gScreenAge;
extern uint8 gSavePromptMode;

extern char gRCT2AddressAppPath[];
extern char gRCT2AddressSavedGamesPath[];
extern char gRCT2AddressSavedGamesPath2[];
extern char gRCT2AddressScenariosPath[];
extern char gRCT2AddressLandscapesPath[];
extern char gRCT2AddressObjectDataPath[];
extern char gRCT2AddressTracksPath[];

bool rct2_init();
sint32 rct2_init_directories();
sint32 rct2_startup_checks();
void rct2_dispose();
void rct2_update();
void substitute_path(char *dest, size_t size, const char *path, const char *filename);
sint32 check_mutex();
sint32 check_file_paths();
bool check_file_path(sint32 pathId);
sint32 check_files_integrity();
const char *get_file_path(sint32 pathId);
void rct2_quit();

bool rct2_open_file(const char *path);

uint32 get_file_extension_type(const utf8 *path);
void rct2_copy_original_user_files_over();

#ifdef __cplusplus
}
#endif

#endif
