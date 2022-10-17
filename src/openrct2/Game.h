/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "common.h"
#include "core/String.hpp"

#include <memory>
#include <string>

class Intent;
struct ParkLoadResult;

enum class GameCommand : int32_t
{
    SetRideAppearance,        // GA
    SetLandHeight,            // GA
    TogglePause,              // GA
    PlaceTrack,               // GA
    RemoveTrack,              // GA
    LoadOrQuit,               // GA
    CreateRide,               // GA
    DemolishRide,             // GA
    SetRideStatus,            // GA
    SetRideVehicles,          // GA
    SetRideName,              // GA
    SetRideSetting,           // GA
    PlaceRideEntranceOrExit,  // GA
    RemoveRideEntranceOrExit, // GA
    RemoveScenery,            // GA
    PlaceScenery,             // GA
    SetWaterHeight,           // GA
    PlacePath,                // GA
    PlacePathFromTrack,       // GA
    RemovePath,               // GA
    ChangeSurfaceStyle,       // GA
    SetRidePrice,             // GA
    SetGuestName,             // GA
    SetStaffName,             // GA
    RaiseLand,                // GA
    LowerLand,                // GA
    EditLandSmooth,           // GA
    RaiseWater,               // GA
    LowerWater,               // GA
    SetBrakesSpeed,           // GA
    HireNewStaffMember,       // GA
    SetStaffPatrol,           // GA
    FireStaffMember,          // GA
    SetStaffOrders,           // GA
    SetParkName,              // GA
    SetParkOpen,              // GA
    BuyLandRights,            // GA
    PlaceParkEntrance,        // GA
    RemoveParkEntrance,       // GA
    SetMazeTrack,             // GA
    SetParkEntranceFee,       // GA
    SetStaffColour,           // GA
    PlaceWall,                // GA
    RemoveWall,               // GA
    PlaceLargeScenery,        // GA
    RemoveLargeScenery,       // GA
    SetCurrentLoan,           // GA
    SetResearchFunding,       // GA
    PlaceTrackDesign,         // GA
    StartMarketingCampaign,   // GA
    PlaceMazeDesign,          // GA
    PlaceBanner,              // GA
    RemoveBanner,             // GA
    SetSceneryColour,         // GA
    SetWallColour,            // GA
    SetLargeSceneryColour,    // GA
    SetBannerColour,          // GA
    SetLandOwnership,         // GA
    ClearScenery,             // GA
    SetBannerName,            // GA
    SetSignName,              // GA
    SetBannerStyle,           // GA
    SetSignStyle,             // GA
    SetPlayerGroup,           // GA
    ModifyGroups,             // GA
    KickPlayer,               // GA
    Cheat,                    // GA
    PickupGuest,              // GA
    PickupStaff,              // GA
    BalloonPress,             // GA
    ModifyTile,               // GA
    EditScenarioOptions,      // GA
    PlacePeepSpawn,           // GA
    SetClimate,               // GA
    SetColourScheme,          // GA
    SetStaffCostume,          // GA
    PlaceFootpathAddition,    // GA
    RemoveFootpathAddition,   // GA
    GuestSetFlags,            // GA
    SetDate,                  // GA
    Custom,                   // GA
    ChangeMapSize,
    FreezeRideRating,
    Count,
};

enum : uint32_t
{
    GAME_COMMAND_FLAG_APPLY = (1 << 0),  // If this flag is set, the command is applied, otherwise only the cost is retrieved
    GAME_COMMAND_FLAG_REPLAY = (1 << 1), // Command was issued from replay manager.
    GAME_COMMAND_FLAG_2 = (1 << 2),      // Unused
    GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED = (1 << 3), // Allow while paused
    GAME_COMMAND_FLAG_4 = (1 << 4),                   // Unused
    GAME_COMMAND_FLAG_NO_SPEND = (1 << 5),            // Game command is not networked
    GAME_COMMAND_FLAG_GHOST = (1 << 6),               // Game command is not networked
    GAME_COMMAND_FLAG_TRACK_DESIGN = (1 << 7),
    GAME_COMMAND_FLAG_NETWORKED = (1U << 31) // Game command is coming from network
};

enum
{
    GAME_PAUSED_NORMAL = 1 << 0,
    GAME_PAUSED_MODAL = 1 << 1,
    GAME_PAUSED_SAVING_TRACK = 1 << 2,
};

enum
{
    ERROR_TYPE_NONE = 0,
    ERROR_TYPE_GENERIC = 254,
    ERROR_TYPE_FILE_LOAD = 255
};

extern uint32_t gCurrentTicks;
extern uint32_t gCurrentRealTimeTicks;

extern uint16_t gCurrentDeltaTime;
extern uint8_t gGamePaused;
extern int32_t gGameSpeed;
extern bool gDoSingleUpdate;
extern float gDayNightCycle;
extern bool gInUpdateCode;
extern bool gInMapInitCode;
extern std::string gCurrentLoadedPath;
extern bool gIsAutosave;
extern bool gIsAutosaveLoaded;

extern bool gLoadKeepWindowsOpen;

void game_reset_speed();
void game_increase_game_speed();
void game_reduce_game_speed();

void game_create_windows();
void reset_all_sprite_quadrant_placements();
void update_palette_effects();

void game_load_or_quit_no_save_prompt();
void game_load_init();
void game_load_scripts();
void game_unload_scripts();
void game_notify_map_change();
void game_notify_map_changed();
void pause_toggle();
bool game_is_paused();
bool game_is_not_paused();
void save_game();
std::unique_ptr<Intent> create_save_game_as_intent();
void save_game_as();
void save_game_cmd(u8string_view name = {});
void save_game_with_name(u8string_view name);
void game_autosave();
void rct2_to_utf8_self(char* buffer, size_t length);
void game_fix_save_vars();
void start_silent_record();
bool stop_silent_record();
void PrepareMapForSave();
