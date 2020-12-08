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
#include "SurfaceSetStyleAction.hpp"
#include "TileModifyAction.hpp"
#include "TrackDesignAction.h"
#include "TrackPlaceAction.hpp"
#include "TrackRemoveAction.hpp"
#include "TrackSetBrakeSpeedAction.hpp"
#include "WallPlaceAction.hpp"
#include "WallRemoveAction.hpp"
#include "WallSetColourAction.hpp"
#include "WaterLowerAction.hpp"
#include "WaterRaiseAction.hpp"
#include "WaterSetHeightAction.hpp"

namespace GameActions
{
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
#ifdef ENABLE_SCRIPTING
        Register<CustomAction>();
#endif
    }
} // namespace GameActions
