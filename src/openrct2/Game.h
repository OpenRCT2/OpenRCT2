/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

    // The number of logical update / ticks per second.
    constexpr uint32_t kGameUpdateFPS = 40;
    // The maximum amount of updates in case rendering is slower
    constexpr uint32_t kGameMaxUpdates = 4;
    // The game update interval in milliseconds, (1000 / 40fps) = 25ms
    constexpr float kGameUpdateTimeMS = 1.0f / kGameUpdateFPS;
    // The maximum threshold to advance.
    constexpr float kGameUpdateMaxThreshold = kGameUpdateTimeMS * kGameMaxUpdates;

    // The network update runs at a different rate to the game update.
    constexpr uint32_t kNetworkUpdateFPS = 140;
    // The network update interval in milliseconds, (1000 / 140fps) = ~7.14ms
    constexpr float kNetworkUpdateTimeMS = 1.0f / kNetworkUpdateFPS;

    constexpr float kGameMinTimeScale = 0.1f;
    constexpr float kGameMaxTimeScale = 5.0f;
} // namespace OpenRCT2

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
extern uint8_t gGameSpeed;
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
