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

#include <array>

namespace GameActions
{
    static std::array<GameActionFactory, EnumValue(GameCommand::Count)> _actions{};
    static std::array<const char*, EnumValue(GameCommand::Count)> _actionNames{};

    static void Register(GameCommand id, GameActionFactory factory, const char* name)
    {
        const auto idx = static_cast<size_t>(id);

        Guard::Assert(idx < std::size(_actions));
        Guard::ArgumentNotNull(factory);

        _actions[idx] = factory;
        _actionNames[idx] = name;
    }

    template<typename T> static void Register(const char* name)
    {
        GameActionFactory factory = []() -> GameAction* { return new T(); };
        Register(T::TYPE, factory, name);
    }

    void Register()
    {
#define REGISTER_ACTION(type) Register<type>(#type)

        REGISTER_ACTION(BalloonPressAction);
        REGISTER_ACTION(BannerPlaceAction);
        REGISTER_ACTION(BannerRemoveAction);
        REGISTER_ACTION(BannerSetColourAction);
        REGISTER_ACTION(BannerSetNameAction);
        REGISTER_ACTION(BannerSetStyleAction);
        REGISTER_ACTION(ClimateSetAction);
        REGISTER_ACTION(FootpathPlaceAction);
        REGISTER_ACTION(FootpathPlaceFromTrackAction);
        REGISTER_ACTION(FootpathRemoveAction);
        REGISTER_ACTION(FootpathAdditionPlaceAction);
        REGISTER_ACTION(FootpathAdditionRemoveAction);
        REGISTER_ACTION(GuestSetNameAction);
        REGISTER_ACTION(MazePlaceTrackAction);
        REGISTER_ACTION(MazeSetTrackAction);
        REGISTER_ACTION(NetworkModifyGroupAction);
        REGISTER_ACTION(ParkMarketingAction);
        REGISTER_ACTION(ParkEntranceRemoveAction);
        REGISTER_ACTION(ParkSetLoanAction);
        REGISTER_ACTION(ParkSetNameAction);
        REGISTER_ACTION(ParkSetParameterAction);
        REGISTER_ACTION(ParkSetResearchFundingAction);
        REGISTER_ACTION(PeepPickupAction);
        REGISTER_ACTION(PlaceParkEntranceAction);
        REGISTER_ACTION(PlacePeepSpawnAction);
        REGISTER_ACTION(PlayerKickAction);
        REGISTER_ACTION(PlayerSetGroupAction);
        REGISTER_ACTION(RideCreateAction);
        REGISTER_ACTION(RideDemolishAction);
        REGISTER_ACTION(RideEntranceExitPlaceAction);
        REGISTER_ACTION(RideEntranceExitRemoveAction);
        REGISTER_ACTION(RideSetColourSchemeAction);
        REGISTER_ACTION(RideSetNameAction);
        REGISTER_ACTION(RideSetPriceAction);
        REGISTER_ACTION(RideSetStatusAction);
        REGISTER_ACTION(RideSetAppearanceAction);
        REGISTER_ACTION(RideSetVehicleAction);
        REGISTER_ACTION(RideSetSettingAction);
        REGISTER_ACTION(ScenarioSetSettingAction);
        REGISTER_ACTION(SetParkEntranceFeeAction);
        REGISTER_ACTION(SignSetNameAction);
        REGISTER_ACTION(SignSetStyleAction);
        REGISTER_ACTION(StaffFireAction);
        REGISTER_ACTION(StaffHireNewAction);
        REGISTER_ACTION(StaffSetColourAction);
        REGISTER_ACTION(StaffSetNameAction);
        REGISTER_ACTION(StaffSetOrdersAction);
        REGISTER_ACTION(StaffSetCostumeAction);
        REGISTER_ACTION(StaffSetPatrolAreaAction);
        REGISTER_ACTION(SurfaceSetStyleAction);
        REGISTER_ACTION(WallPlaceAction);
        REGISTER_ACTION(WallRemoveAction);
        REGISTER_ACTION(WallSetColourAction);
        REGISTER_ACTION(SmallSceneryPlaceAction);
        REGISTER_ACTION(SmallSceneryRemoveAction);
        REGISTER_ACTION(SmallScenerySetColourAction);
        REGISTER_ACTION(LargeSceneryPlaceAction);
        REGISTER_ACTION(LargeSceneryRemoveAction);
        REGISTER_ACTION(LargeScenerySetColourAction);
        REGISTER_ACTION(LandBuyRightsAction);
        REGISTER_ACTION(LandLowerAction);
        REGISTER_ACTION(LandRaiseAction);
        REGISTER_ACTION(LandSetHeightAction);
        REGISTER_ACTION(LandSetRightsAction);
        REGISTER_ACTION(LandSmoothAction);
        REGISTER_ACTION(TileModifyAction);
        REGISTER_ACTION(TrackDesignAction);
        REGISTER_ACTION(TrackPlaceAction);
        REGISTER_ACTION(TrackRemoveAction);
        REGISTER_ACTION(TrackSetBrakeSpeedAction);
        REGISTER_ACTION(ClearAction);
        REGISTER_ACTION(PauseToggleAction);
        REGISTER_ACTION(LoadOrQuitAction);
        REGISTER_ACTION(WaterSetHeightAction);
        REGISTER_ACTION(WaterLowerAction);
        REGISTER_ACTION(WaterRaiseAction);
        REGISTER_ACTION(GuestSetFlagsAction);
        REGISTER_ACTION(ParkSetDateAction);
        REGISTER_ACTION(SetCheatAction);
        REGISTER_ACTION(ChangeMapSizeAction);
#ifdef ENABLE_SCRIPTING
        REGISTER_ACTION(CustomAction);
#endif

#undef REGISTER_ACTION
    }

    const char* GetName(GameCommand id)
    {
        Initialize();

        const auto idx = static_cast<size_t>(id);
        Guard::IndexInRange(idx, _actionNames);

        return _actionNames[idx];
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
        Guard::ArgumentNotNull(result, "Attempting to create unregistered game action: %u", id);
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

    void Initialize()
    {
        static bool initialized = false;
        if (initialized)
            return;

        Register();
        initialized = true;
    }

} // namespace GameActions
