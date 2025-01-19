/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "core/StringTypes.h"

#include <memory>

namespace OpenRCT2
{
    class Intent;
}

struct ParkLoadResult;

enum class GameCommand : int32_t
{
    SetRideAppearance,
    SetLandHeight,
    TogglePause,
    PlaceTrack,
    RemoveTrack,
    LoadOrQuit,
    CreateRide,
    DemolishRide,
    SetRideStatus,
    SetRideVehicles,
    SetRideName,
    SetRideSetting,
    PlaceRideEntranceOrExit,
    RemoveRideEntranceOrExit,
    RemoveScenery,
    PlaceScenery,
    SetWaterHeight,
    PlacePath,
    PlacePathLayout,
    RemovePath,
    ChangeSurfaceStyle,
    SetRidePrice,
    SetGuestName,
    SetStaffName,
    RaiseLand,
    LowerLand,
    EditLandSmooth,
    RaiseWater,
    LowerWater,
    SetBrakesSpeed,
    HireNewStaffMember,
    SetStaffPatrol,
    FireStaffMember,
    SetStaffOrders,
    SetParkName,
    SetParkOpen,
    BuyLandRights,
    PlaceParkEntrance,
    RemoveParkEntrance,
    SetMazeTrack,
    SetParkEntranceFee,
    SetStaffColour,
    PlaceWall,
    RemoveWall,
    PlaceLargeScenery,
    RemoveLargeScenery,
    SetCurrentLoan,
    SetResearchFunding,
    PlaceTrackDesign,
    StartMarketingCampaign,
    PlaceMazeDesign,
    PlaceBanner,
    RemoveBanner,
    SetSceneryColour,
    SetWallColour,
    SetLargeSceneryColour,
    SetBannerColour,
    SetLandOwnership,
    ClearScenery,
    SetBannerName,
    SetSignName,
    SetBannerStyle,
    SetSignStyle,
    SetPlayerGroup,
    ModifyGroups,
    KickPlayer,
    Cheat,
    PickupGuest,
    PickupStaff,
    BalloonPress,
    ModifyTile,
    EditScenarioOptions,
    PlacePeepSpawn,
    SetClimate,
    SetColourScheme,
    SetStaffCostume,
    PlaceFootpathAddition,
    RemoveFootpathAddition,
    GuestSetFlags,
    SetDate,
    Custom,
    ChangeMapSize,
    FreezeRideRating,
    SetGameSpeed,
    SetRestrictedScenery,
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
    GAME_COMMAND_FLAG_NETWORKED = (1u << 31) // Game command is coming from network
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

void GameResetSpeed();
void GameIncreaseGameSpeed();
void GameReduceGameSpeed();

void GameCreateWindows();
void ResetAllSpriteQuadrantPlacements();

void GameLoadOrQuitNoSavePrompt();
void GameLoadInit();
void GameLoadScripts();
void GameUnloadScripts();
void GameNotifyMapChange();
void GameNotifyMapChanged();
void PauseToggle();
bool GameIsPaused();
bool GameIsNotPaused();
void SaveGame();
std::unique_ptr<OpenRCT2::Intent> CreateSaveGameAsIntent();
void SaveGameAs();
void SaveGameCmd(u8string_view name = {});
void SaveGameWithName(u8string_view name);
void GameAutosave();
void RCT2StringToUTF8Self(char* buffer, size_t length);
void GameFixSaveVars();
void StartSilentRecord();
bool StopSilentRecord();
void PrepareMapForSave();
