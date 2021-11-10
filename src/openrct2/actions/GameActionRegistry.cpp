/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "BalloonPressAction.h"
#include "BannerPlaceAction.h"
#include "BannerRemoveAction.h"
#include "BannerSetColourAction.h"
#include "BannerSetNameAction.h"
#include "BannerSetStyleAction.h"
#include "ChangeMapSizeAction.h"
#include "ClearAction.h"
#include "ClimateSetAction.h"
#include "CustomAction.h"
#include "FootpathAdditionPlaceAction.h"
#include "FootpathAdditionRemoveAction.h"
#include "FootpathPlaceAction.h"
#include "FootpathPlaceFromTrackAction.h"
#include "FootpathRemoveAction.h"
#include "GameAction.h"
#include "GuestSetFlagsAction.h"
#include "GuestSetNameAction.h"
#include "LandBuyRightsAction.h"
#include "LandLowerAction.h"
#include "LandRaiseAction.h"
#include "LandSetHeightAction.h"
#include "LandSetRightsAction.h"
#include "LandSmoothAction.h"
#include "LargeSceneryPlaceAction.h"
#include "LargeSceneryRemoveAction.h"
#include "LargeScenerySetColourAction.h"
#include "LoadOrQuitAction.h"
#include "MazePlaceTrackAction.h"
#include "MazeSetTrackAction.h"
#include "NetworkModifyGroupAction.h"
#include "ParkEntranceRemoveAction.h"
#include "ParkMarketingAction.h"
#include "ParkSetDateAction.h"
#include "ParkSetLoanAction.h"
#include "ParkSetNameAction.h"
#include "ParkSetParameterAction.h"
#include "ParkSetResearchFundingAction.h"
#include "PauseToggleAction.h"
#include "PeepPickupAction.h"
#include "PlaceParkEntranceAction.h"
#include "PlacePeepSpawnAction.h"
#include "PlayerKickAction.h"
#include "PlayerSetGroupAction.h"
#include "RideCreateAction.h"
#include "RideDemolishAction.h"
#include "RideEntranceExitPlaceAction.h"
#include "RideEntranceExitRemoveAction.h"
#include "RideSetAppearanceAction.h"
#include "RideSetColourSchemeAction.h"
#include "RideSetNameAction.h"
#include "RideSetPriceAction.h"
#include "RideSetSettingAction.h"
#include "RideSetStatusAction.h"
#include "RideSetVehicleAction.h"
#include "ScenarioSetSettingAction.h"
#include "SetCheatAction.h"
#include "SetParkEntranceFeeAction.h"
#include "SignSetNameAction.h"
#include "SignSetStyleAction.h"
#include "SmallSceneryPlaceAction.h"
#include "SmallSceneryRemoveAction.h"
#include "SmallScenerySetColourAction.h"
#include "StaffFireAction.h"
#include "StaffHireNewAction.h"
#include "StaffSetColourAction.h"
#include "StaffSetCostumeAction.h"
#include "StaffSetNameAction.h"
#include "StaffSetOrdersAction.h"
#include "StaffSetPatrolAreaAction.h"
#include "SurfaceSetStyleAction.h"
#include "TileModifyAction.h"
#include "TrackDesignAction.h"
#include "TrackPlaceAction.h"
#include "TrackRemoveAction.h"
#include "TrackSetBrakeSpeedAction.h"
#include "WallPlaceAction.h"
#include "WallRemoveAction.h"
#include "WallSetColourAction.h"
#include "WaterLowerAction.h"
#include "WaterRaiseAction.h"
#include "WaterSetHeightAction.h"

namespace GameActions
{
    static GameActionFactory _actions[EnumValue(GameCommand::Count)];

    GameActionFactory Register(GameCommand id, GameActionFactory factory)
    {
        const auto idx = static_cast<size_t>(id);

        Guard::Assert(idx < std::size(_actions));
        Guard::ArgumentNotNull(factory);

        _actions[idx] = factory;
        return factory;
    }

    void Register()
    {
        Register<BalloonPressAction>();
        Register<BannerPlaceAction>();
        Register<BannerRemoveAction>();
        Register<BannerSetColourAction>();
        Register<BannerSetNameAction>();
        Register<BannerSetStyleAction>();
        Register<ClimateSetAction>();
        Register<FootpathPlaceAction>();
        Register<FootpathPlaceFromTrackAction>();
        Register<FootpathRemoveAction>();
        Register<FootpathAdditionPlaceAction>();
        Register<FootpathAdditionRemoveAction>();
        Register<GuestSetNameAction>();
        Register<MazePlaceTrackAction>();
        Register<MazeSetTrackAction>();
        Register<NetworkModifyGroupAction>();
        Register<ParkMarketingAction>();
        Register<ParkEntranceRemoveAction>();
        Register<ParkSetLoanAction>();
        Register<ParkSetNameAction>();
        Register<ParkSetParameterAction>();
        Register<ParkSetResearchFundingAction>();
        Register<PeepPickupAction>();
        Register<PlaceParkEntranceAction>();
        Register<PlacePeepSpawnAction>();
        Register<PlayerKickAction>();
        Register<PlayerSetGroupAction>();
        Register<RideCreateAction>();
        Register<RideDemolishAction>();
        Register<RideEntranceExitPlaceAction>();
        Register<RideEntranceExitRemoveAction>();
        Register<RideSetColourSchemeAction>();
        Register<RideSetNameAction>();
        Register<RideSetPriceAction>();
        Register<RideSetStatusAction>();
        Register<RideSetAppearanceAction>();
        Register<RideSetVehicleAction>();
        Register<RideSetSettingAction>();
        Register<ScenarioSetSettingAction>();
        Register<SetParkEntranceFeeAction>();
        Register<SignSetNameAction>();
        Register<SignSetStyleAction>();
        Register<StaffFireAction>();
        Register<StaffHireNewAction>();
        Register<StaffSetColourAction>();
        Register<StaffSetNameAction>();
        Register<StaffSetOrdersAction>();
        Register<StaffSetCostumeAction>();
        Register<StaffSetPatrolAreaAction>();
        Register<SurfaceSetStyleAction>();
        Register<WallPlaceAction>();
        Register<WallRemoveAction>();
        Register<WallSetColourAction>();
        Register<SmallSceneryPlaceAction>();
        Register<SmallSceneryRemoveAction>();
        Register<SmallScenerySetColourAction>();
        Register<LargeSceneryPlaceAction>();
        Register<LargeSceneryRemoveAction>();
        Register<LargeScenerySetColourAction>();
        Register<LandBuyRightsAction>();
        Register<LandLowerAction>();
        Register<LandRaiseAction>();
        Register<LandSetHeightAction>();
        Register<LandSetRightsAction>();
        Register<LandSmoothAction>();
        Register<TileModifyAction>();
        Register<TrackDesignAction>();
        Register<TrackPlaceAction>();
        Register<TrackRemoveAction>();
        Register<TrackSetBrakeSpeedAction>();
        Register<ClearAction>();
        Register<PauseToggleAction>();
        Register<LoadOrQuitAction>();
        Register<WaterSetHeightAction>();
        Register<WaterLowerAction>();
        Register<WaterRaiseAction>();
        Register<GuestSetFlagsAction>();
        Register<ParkSetDateAction>();
        Register<SetCheatAction>();
        Register<ChangeMapSizeAction>();
#ifdef ENABLE_SCRIPTING
        Register<CustomAction>();
#endif
    }

    const char* GetName(GameCommand id)
    {
        switch (id)
        {
            case GameCommand::SetRideAppearance:
                return "SetRideAppearance";
            case GameCommand::SetLandHeight:
                return "SetLandHeight";
            case GameCommand::TogglePause:
                return "TogglePause";
            case GameCommand::PlaceTrack:
                return "PlaceTrack";
            case GameCommand::RemoveTrack:
                return "RemoveTrack";
            case GameCommand::LoadOrQuit:
                return "LoadOrQuit";
            case GameCommand::CreateRide:
                return "CreateRide";
            case GameCommand::DemolishRide:
                return "DemolishRide";
            case GameCommand::SetRideStatus:
                return "SetRideStatus";
            case GameCommand::SetRideVehicles:
                return "SetRideVehicles";
            case GameCommand::SetRideName:
                return "SetRideName";
            case GameCommand::SetRideSetting:
                return "SetRideSetting";
            case GameCommand::PlaceRideEntranceOrExit:
                return "PlaceRideEntranceOrExit";
            case GameCommand::RemoveRideEntranceOrExit:
                return "RemoveRideEntranceOrExit";
            case GameCommand::RemoveScenery:
                return "RemoveScenery";
            case GameCommand::PlaceScenery:
                return "PlaceScenery";
            case GameCommand::SetWaterHeight:
                return "SetWaterHeight";
            case GameCommand::PlacePath:
                return "PlacePath";
            case GameCommand::PlacePathFromTrack:
                return "PlacePathFromTrack";
            case GameCommand::RemovePath:
                return "RemovePath";
            case GameCommand::ChangeSurfaceStyle:
                return "ChangeSurfaceStyle";
            case GameCommand::SetRidePrice:
                return "SetRidePrice";
            case GameCommand::SetGuestName:
                return "SetGuestName";
            case GameCommand::SetStaffName:
                return "SetStaffName";
            case GameCommand::RaiseLand:
                return "RaiseLand";
            case GameCommand::LowerLand:
                return "LowerLand";
            case GameCommand::EditLandSmooth:
                return "EditLandSmooth";
            case GameCommand::RaiseWater:
                return "RaiseWater";
            case GameCommand::LowerWater:
                return "LowerWater";
            case GameCommand::SetBrakesSpeed:
                return "SetBrakesSpeed";
            case GameCommand::HireNewStaffMember:
                return "HireNewStaffMember";
            case GameCommand::SetStaffPatrol:
                return "SetStaffPatrol";
            case GameCommand::FireStaffMember:
                return "FireStaffMember";
            case GameCommand::SetStaffOrders:
                return "SetStaffOrders";
            case GameCommand::SetParkName:
                return "SetParkName";
            case GameCommand::SetParkOpen:
                return "SetParkOpen";
            case GameCommand::BuyLandRights:
                return "BuyLandRights";
            case GameCommand::PlaceParkEntrance:
                return "PlaceParkEntrance";
            case GameCommand::RemoveParkEntrance:
                return "RemoveParkEntrance";
            case GameCommand::SetMazeTrack:
                return "SetMazeTrack";
            case GameCommand::SetParkEntranceFee:
                return "SetParkEntranceFee";
            case GameCommand::SetStaffColour:
                return "SetStaffColour";
            case GameCommand::PlaceWall:
                return "PlaceWall";
            case GameCommand::RemoveWall:
                return "RemoveWall";
            case GameCommand::PlaceLargeScenery:
                return "PlaceLargeScenery";
            case GameCommand::RemoveLargeScenery:
                return "RemoveLargeScenery";
            case GameCommand::SetCurrentLoan:
                return "SetCurrentLoan";
            case GameCommand::SetResearchFunding:
                return "SetResearchFunding";
            case GameCommand::PlaceTrackDesign:
                return "PlaceTrackDesign";
            case GameCommand::StartMarketingCampaign:
                return "StartMarketingCampaign";
            case GameCommand::PlaceMazeDesign:
                return "PlaceMazeDesign";
            case GameCommand::PlaceBanner:
                return "PlaceBanner";
            case GameCommand::RemoveBanner:
                return "RemoveBanner";
            case GameCommand::SetSceneryColour:
                return "SetSceneryColour";
            case GameCommand::SetWallColour:
                return "SetWallColour";
            case GameCommand::SetLargeSceneryColour:
                return "SetLargeSceneryColour";
            case GameCommand::SetBannerColour:
                return "SetBannerColour";
            case GameCommand::SetLandOwnership:
                return "SetLandOwnership";
            case GameCommand::ClearScenery:
                return "ClearScenery";
            case GameCommand::SetBannerName:
                return "SetBannerName";
            case GameCommand::SetSignName:
                return "SetSignName";
            case GameCommand::SetBannerStyle:
                return "SetBannerStyle";
            case GameCommand::SetSignStyle:
                return "SetSignStyle";
            case GameCommand::SetPlayerGroup:
                return "SetPlayerGroup";
            case GameCommand::ModifyGroups:
                return "ModifyGroups";
            case GameCommand::KickPlayer:
                return "KickPlayer";
            case GameCommand::Cheat:
                return "Cheat";
            case GameCommand::PickupGuest:
                return "PickupGuest";
            case GameCommand::PickupStaff:
                return "PickupStaff";
            case GameCommand::BalloonPress:
                return "BalloonPress";
            case GameCommand::ModifyTile:
                return "ModifyTile";
            case GameCommand::EditScenarioOptions:
                return "EditScenarioOptions";
            case GameCommand::PlacePeepSpawn:
                return "PlacePeepSpawn";
            case GameCommand::SetClimate:
                return "SetClimate";
            case GameCommand::SetColourScheme:
                return "SetColourScheme";
            case GameCommand::SetStaffCostume:
                return "SetStaffCostume";
            case GameCommand::PlaceFootpathAddition:
                return "PlaceFootpathAddition";
            case GameCommand::RemoveFootpathAddition:
                return "RemoveFootpathAddition";
            case GameCommand::GuestSetFlags:
                return "GuestSetFlags";
            case GameCommand::SetDate:
                return "SetDate";
            case GameCommand::Custom:
                return "Custom";
            case GameCommand::ChangeMapSize:
                return "ChangeMapSize";
            default:
                break;
        }
        return "<error>";
    }

    std::unique_ptr<GameAction> Create(GameCommand id)
    {
        Initialize();

        const auto idx = static_cast<size_t>(id);

        GameAction* result = nullptr;
        if (idx < std::size(_actions))
        {
            GameActionFactory factory = _actions[idx];
            if (factory != nullptr)
            {
                result = factory();
            }
        }
#ifdef _DEBUG
        Guard::ArgumentNotNull(result, "Attempting to create unregistered gameaction: %u", id);
#endif
        return std::unique_ptr<GameAction>(result);
    }

    bool IsValidId(uint32_t id)
    {
        if (id < std::size(_actions))
        {
            return _actions[id] != nullptr;
        }
        return false;
    }

} // namespace GameActions
