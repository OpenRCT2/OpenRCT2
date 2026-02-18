/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GameActionRegistry.h"

#include "../core/Guard.hpp"
#include "cheats/CheatSetAction.h"
#include "footpath/FootpathAdditionPlaceAction.h"
#include "footpath/FootpathAdditionRemoveAction.h"
#include "footpath/FootpathLayoutPlaceAction.h"
#include "footpath/FootpathPlaceAction.h"
#include "footpath/FootpathRemoveAction.h"
#include "general/BalloonPressAction.h"
#include "general/CustomAction.h"
#include "general/GameSetSpeedAction.h"
#include "general/LoadOrQuitAction.h"
#include "general/MapChangeSizeAction.h"
#include "general/PauseToggleAction.h"
#include "general/ScenarioSetSettingAction.h"
#include "general/TileModifyAction.h"
#include "network/NetworkModifyGroupAction.h"
#include "network/PlayerKickAction.h"
#include "network/PlayerSetGroupAction.h"
#include "park/LandBuyRightsAction.h"
#include "park/LandSetRightsAction.h"
#include "park/ParkEntrancePlaceAction.h"
#include "park/ParkEntranceRemoveAction.h"
#include "park/ParkMarketingAction.h"
#include "park/ParkSetDateAction.h"
#include "park/ParkSetEntranceFeeAction.h"
#include "park/ParkSetLoanAction.h"
#include "park/ParkSetNameAction.h"
#include "park/ParkSetParameterAction.h"
#include "park/ParkSetResearchFundingAction.h"
#include "peep/GuestSetFlagsAction.h"
#include "peep/GuestSetNameAction.h"
#include "peep/PeepPickupAction.h"
#include "peep/PeepSpawnPlaceAction.h"
#include "peep/StaffFireAction.h"
#include "peep/StaffHireNewAction.h"
#include "peep/StaffSetColourAction.h"
#include "peep/StaffSetCostumeAction.h"
#include "peep/StaffSetNameAction.h"
#include "peep/StaffSetOrdersAction.h"
#include "peep/StaffSetPatrolAreaAction.h"
#include "ride/MazePlaceTrackAction.h"
#include "ride/MazeSetTrackAction.h"
#include "ride/RideCreateAction.h"
#include "ride/RideDemolishAction.h"
#include "ride/RideEntranceExitPlaceAction.h"
#include "ride/RideEntranceExitRemoveAction.h"
#include "ride/RideFreezeRatingAction.h"
#include "ride/RideSetAppearanceAction.h"
#include "ride/RideSetColourSchemeAction.h"
#include "ride/RideSetNameAction.h"
#include "ride/RideSetPriceAction.h"
#include "ride/RideSetSettingAction.h"
#include "ride/RideSetStatusAction.h"
#include "ride/RideSetVehicleAction.h"
#include "ride/RideSetVisibilityAction.h"
#include "scenery/BannerPlaceAction.h"
#include "scenery/BannerRemoveAction.h"
#include "scenery/BannerSetColourAction.h"
#include "scenery/BannerSetNameAction.h"
#include "scenery/BannerSetStyleAction.h"
#include "scenery/LargeSceneryPlaceAction.h"
#include "scenery/LargeSceneryRemoveAction.h"
#include "scenery/LargeScenerySetColourAction.h"
#include "scenery/ScenerySetRestrictedAction.h"
#include "scenery/SignSetNameAction.h"
#include "scenery/SignSetStyleAction.h"
#include "scenery/SmallSceneryPlaceAction.h"
#include "scenery/SmallSceneryRemoveAction.h"
#include "scenery/SmallScenerySetColourAction.h"
#include "scenery/WallPlaceAction.h"
#include "scenery/WallRemoveAction.h"
#include "scenery/WallSetColourAction.h"
#include "terraform/ClearAction.h"
#include "terraform/LandLowerAction.h"
#include "terraform/LandRaiseAction.h"
#include "terraform/LandSetHeightAction.h"
#include "terraform/LandSmoothAction.h"
#include "terraform/SurfaceSetStyleAction.h"
#include "terraform/WaterLowerAction.h"
#include "terraform/WaterRaiseAction.h"
#include "terraform/WaterSetHeightAction.h"
#include "track/TrackDesignAction.h"
#include "track/TrackPlaceAction.h"
#include "track/TrackRemoveAction.h"
#include "track/TrackSetBrakeSpeedAction.h"

#include <array>

namespace OpenRCT2::GameActions
{
    struct GameActionEntry
    {
        GameActionFactory factory{};
        const char* name{};
    };

    using GameActionRegistry = std::array<GameActionEntry, EnumValue(GameCommand::Count)>;

    template<GameCommand TId>
    static constexpr void Register(GameActionRegistry& registry, GameActionFactory factory, const char* name)
    {
        constexpr auto idx = static_cast<size_t>(TId);

        static_assert(idx < EnumValue(GameCommand::Count));

        registry[idx] = { factory, name };
    }

    template<typename T>
    static constexpr void Register(GameActionRegistry& registry, const char* name)
    {
        GameActionFactory factory = []() -> GameAction* { return new T(); };
        Register<T::kType>(registry, factory, name);
    }

    static constexpr GameActionRegistry BuildRegistry()
    {
        GameActionRegistry registry{};

#define REGISTER_ACTION(type) Register<type>(registry, #type)

        REGISTER_ACTION(BalloonPressAction);
        REGISTER_ACTION(BannerPlaceAction);
        REGISTER_ACTION(BannerRemoveAction);
        REGISTER_ACTION(BannerSetColourAction);
        REGISTER_ACTION(BannerSetNameAction);
        REGISTER_ACTION(BannerSetStyleAction);
        REGISTER_ACTION(FootpathPlaceAction);
        REGISTER_ACTION(FootpathLayoutPlaceAction);
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
        REGISTER_ACTION(ParkEntrancePlaceAction);
        REGISTER_ACTION(PeepSpawnPlaceAction);
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
        REGISTER_ACTION(RideFreezeRatingAction);
        REGISTER_ACTION(RideSetAppearanceAction);
        REGISTER_ACTION(RideSetVehicleAction);
        REGISTER_ACTION(RideSetSettingAction);
        REGISTER_ACTION(ScenarioSetSettingAction);
        REGISTER_ACTION(ParkSetEntranceFeeAction);
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
        REGISTER_ACTION(CheatSetAction);
        REGISTER_ACTION(MapChangeSizeAction);
        REGISTER_ACTION(GameSetSpeedAction);
        REGISTER_ACTION(ScenerySetRestrictedAction);
        REGISTER_ACTION(RideSetVisibilityAction);
#ifdef ENABLE_SCRIPTING
        REGISTER_ACTION(CustomAction);
#endif

#undef REGISTER_ACTION

        return registry;
    }

    static constexpr GameActionRegistry _registry = BuildRegistry();

    std::optional<GameActionFactory> getFactory(GameCommand id)
    {
        const auto idx = static_cast<size_t>(id);
        if (idx < std::size(_registry))
        {
            return _registry[idx].factory;
        }
        return std::nullopt;
    }

    const char* GetName(GameCommand id)
    {
        const auto idx = static_cast<size_t>(id);
        Guard::IndexInRange(idx, _registry);

        return _registry[idx].name;
    }

    bool IsValidId(uint32_t id)
    {
        if (id < std::size(_registry))
        {
            return _registry[id].factory != nullptr;
        }
        return false;
    }

} // namespace OpenRCT2::GameActions
