/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CheatSetAction.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../config/Config.h"
#include "../core/EnumUtils.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../entity/Duck.h"
#include "../entity/EntityRegistry.h"
#include "../entity/Staff.h"
#include "../localisation/StringIds.h"
#include "../network/network.h"
#include "../object/PathAdditionEntry.h"
#include "../ride/Ride.h"
#include "../ride/RideManager.hpp"
#include "../ride/Vehicle.h"
#include "../scenario/Scenario.h"
#include "../ui/WindowManager.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Banner.h"
#include "../world/Climate.h"
#include "../world/Footpath.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/SmallSceneryElement.h"
#include "../world/tile_element/SurfaceElement.h"
#include "ParkSetLoanAction.h"
#include "ParkSetParameterAction.h"

using namespace OpenRCT2;

using ParametersRange = std::pair<std::pair<int64_t, int64_t>, std::pair<int64_t, int64_t>>;

CheatSetAction::CheatSetAction(CheatType cheatType, int64_t param1, int64_t param2)
    : _cheatType(static_cast<int32_t>(cheatType))
    , _param1(param1)
    , _param2(param2)
{
}

void CheatSetAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("type", _cheatType);
    visitor.Visit("param1", _param1);
    visitor.Visit("param2", _param2);
}

uint16_t CheatSetAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void CheatSetAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_cheatType) << DS_TAG(_param1) << DS_TAG(_param2);
}

GameActions::Result CheatSetAction::Query() const
{
    if (static_cast<uint32_t>(_cheatType) >= static_cast<uint32_t>(CheatType::Count))
    {
        LOG_ERROR("Invalid cheat type %u", _cheatType);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
    }

    ParametersRange validRange = GetParameterRange(static_cast<CheatType>(_cheatType.id));

    if (_param1 < validRange.first.first || _param1 > validRange.first.second)
    {
        LOG_ERROR(
            "The first cheat parameter is out of range. Value = %d, min = %d, max = %d", _param1, validRange.first.first,
            validRange.first.second);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
    }
    if (_param2 < validRange.second.first || _param2 > validRange.second.second)
    {
        LOG_ERROR(
            "The second cheat parameter is out of range. Value = %d, min = %d, max = %d", _param2, validRange.second.first,
            validRange.second.second);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
    }

    return GameActions::Result();
}

GameActions::Result CheatSetAction::Execute() const
{
    auto& gameState = GetGameState();
    auto* windowMgr = Ui::GetWindowManager();

    switch (static_cast<CheatType>(_cheatType.id))
    {
        case CheatType::SandboxMode:
            gameState.Cheats.sandboxMode = _param1 != 0;
            windowMgr->InvalidateByClass(WindowClass::Map);
            windowMgr->InvalidateByClass(WindowClass::Footpath);
            break;
        case CheatType::DisableClearanceChecks:
            gameState.Cheats.disableClearanceChecks = _param1 != 0;
            // Required to update the clearance checks overlay on the Cheats button.
            windowMgr->InvalidateByClass(WindowClass::TopToolbar);
            break;
        case CheatType::DisableSupportLimits:
            gameState.Cheats.disableSupportLimits = _param1 != 0;
            break;
        case CheatType::ShowAllOperatingModes:
            gameState.Cheats.showAllOperatingModes = _param1 != 0;
            break;
        case CheatType::ShowVehiclesFromOtherTrackTypes:
            gameState.Cheats.showVehiclesFromOtherTrackTypes = _param1 != 0;
            break;
        case CheatType::FastLiftHill:
            gameState.Cheats.unlockOperatingLimits = _param1 != 0;
            break;
        case CheatType::DisableBrakesFailure:
            gameState.Cheats.disableBrakesFailure = _param1 != 0;
            break;
        case CheatType::DisableAllBreakdowns:
            gameState.Cheats.disableAllBreakdowns = _param1 != 0;
            break;
        case CheatType::DisableTrainLengthLimit:
            gameState.Cheats.disableTrainLengthLimit = _param1 != 0;
            break;
        case CheatType::EnableChainLiftOnAllTrack:
            gameState.Cheats.enableChainLiftOnAllTrack = _param1 != 0;
            break;
        case CheatType::BuildInPauseMode:
            gameState.Cheats.buildInPauseMode = _param1 != 0;
            break;
        case CheatType::IgnoreRideIntensity:
            gameState.Cheats.ignoreRideIntensity = _param1 != 0;
            break;
        case CheatType::IgnorePrice:
            gameState.Cheats.ignorePrice = _param1 != 0;
            break;
        case CheatType::DisableVandalism:
            gameState.Cheats.disableVandalism = _param1 != 0;
            break;
        case CheatType::DisableLittering:
            gameState.Cheats.disableLittering = _param1 != 0;
            break;
        case CheatType::NoMoney:
            SetScenarioNoMoney(_param1 != 0);
            break;
        case CheatType::AddMoney:
            AddMoney(_param1);
            break;
        case CheatType::SetMoney:
            SetMoney(_param1);
            break;
        case CheatType::ClearLoan:
            ClearLoan();
            break;
        case CheatType::SetGuestParameter:
            SetGuestParameter(_param1, _param2);
            break;
        case CheatType::GenerateGuests:
            GenerateGuests(_param1);
            break;
        case CheatType::RemoveAllGuests:
            RemoveAllGuests();
            break;
        case CheatType::GiveAllGuests:
            GiveObjectToGuests(_param1);
            break;
        case CheatType::SetGrassLength:
            SetGrassLength(_param1);
            break;
        case CheatType::WaterPlants:
            WaterPlants();
            break;
        case CheatType::FixVandalism:
            FixVandalism();
            break;
        case CheatType::RemoveLitter:
            RemoveLitter();
            break;
        case CheatType::DisablePlantAging:
            gameState.Cheats.disablePlantAging = _param1 != 0;
            break;
        case CheatType::SetStaffSpeed:
            SetStaffSpeed(_param1);
            break;
        case CheatType::RenewRides:
            RenewRides();
            break;
        case CheatType::MakeDestructible:
            gameState.Cheats.makeAllDestructible = _param1 != 0;
            windowMgr->InvalidateByClass(WindowClass::Ride);
            break;
        case CheatType::FixRides:
            FixBrokenRides();
            break;
        case CheatType::ResetCrashStatus:
            ResetRideCrashStatus();
            break;
        case CheatType::TenMinuteInspections:
            Set10MinuteInspection();
            break;
        case CheatType::WinScenario:
            ScenarioSuccess(gameState);
            break;
        case CheatType::ForceWeather:
            // Todo - make sure this is safe
            ClimateForceWeather(WeatherType{ static_cast<uint8_t>(_param1) });
            break;
        case CheatType::FreezeWeather:
            gameState.Cheats.freezeWeather = _param1 != 0;
            break;
        case CheatType::NeverendingMarketing:
            gameState.Cheats.neverendingMarketing = _param1 != 0;
            break;
        case CheatType::OpenClosePark:
            ParkSetOpen(!gameState.Park.IsOpen());
            break;
        case CheatType::HaveFun:
            gameState.ScenarioObjective.Type = OBJECTIVE_HAVE_FUN;
            break;
        case CheatType::SetForcedParkRating:
            Park::SetForcedRating(_param1);
            break;
        case CheatType::AllowArbitraryRideTypeChanges:
            gameState.Cheats.allowArbitraryRideTypeChanges = _param1 != 0;
            windowMgr->InvalidateByClass(WindowClass::Ride);
            break;
        case CheatType::OwnAllLand:
            OwnAllLand();
            break;
        case CheatType::DisableRideValueAging:
            gameState.Cheats.disableRideValueAging = _param1 != 0;
            break;
        case CheatType::IgnoreResearchStatus:
            gameState.Cheats.ignoreResearchStatus = _param1 != 0;
            break;
        case CheatType::EnableAllDrawableTrackPieces:
            gameState.Cheats.enableAllDrawableTrackPieces = _param1 != 0;
            break;
        case CheatType::CreateDucks:
            CreateDucks(_param1);
            break;
        case CheatType::RemoveDucks:
            Duck::RemoveAll();
            break;
        case CheatType::AllowTrackPlaceInvalidHeights:
            gameState.Cheats.allowTrackPlaceInvalidHeights = _param1 != 0;
            break;
        case CheatType::AllowRegularPathAsQueue:
            gameState.Cheats.allowRegularPathAsQueue = _param1 != 0;
            break;
        case CheatType::AllowSpecialColourSchemes:
            gameState.Cheats.allowSpecialColourSchemes = static_cast<bool>(_param1);
            break;
        case CheatType::RemoveParkFences:
            RemoveParkFences();
            break;
        default:
        {
            LOG_ERROR("Invalid cheat type %d", _cheatType.id);
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
        }
    }

    if (NetworkGetMode() == NETWORK_MODE_NONE)
    {
        Config::Save();
    }

    windowMgr->InvalidateByClass(WindowClass::Cheats);
    return GameActions::Result();
}

ParametersRange CheatSetAction::GetParameterRange(CheatType cheatType) const
{
    switch (static_cast<CheatType>(_cheatType.id))
    {
        case CheatType::SandboxMode:
            [[fallthrough]];
        case CheatType::DisableClearanceChecks:
            [[fallthrough]];
        case CheatType::DisableSupportLimits:
            [[fallthrough]];
        case CheatType::ShowAllOperatingModes:
            [[fallthrough]];
        case CheatType::ShowVehiclesFromOtherTrackTypes:
            [[fallthrough]];
        case CheatType::FastLiftHill:
            [[fallthrough]];
        case CheatType::DisableBrakesFailure:
            [[fallthrough]];
        case CheatType::DisableAllBreakdowns:
            [[fallthrough]];
        case CheatType::DisableTrainLengthLimit:
            [[fallthrough]];
        case CheatType::EnableChainLiftOnAllTrack:
            [[fallthrough]];
        case CheatType::BuildInPauseMode:
            [[fallthrough]];
        case CheatType::IgnoreRideIntensity:
            [[fallthrough]];
        case CheatType::IgnorePrice:
            [[fallthrough]];
        case CheatType::DisableVandalism:
            [[fallthrough]];
        case CheatType::DisableLittering:
            [[fallthrough]];
        case CheatType::NoMoney:
            [[fallthrough]];
        case CheatType::DisablePlantAging:
            [[fallthrough]];
        case CheatType::FreezeWeather:
            [[fallthrough]];
        case CheatType::NeverendingMarketing:
            [[fallthrough]];
        case CheatType::AllowArbitraryRideTypeChanges:
            [[fallthrough]];
        case CheatType::DisableRideValueAging:
            [[fallthrough]];
        case CheatType::IgnoreResearchStatus:
            [[fallthrough]];
        case CheatType::EnableAllDrawableTrackPieces:
            [[fallthrough]];
        case CheatType::AllowRegularPathAsQueue:
            [[fallthrough]];
        case CheatType::UnlockAllPrices:
            [[fallthrough]];
        case CheatType::MakeDestructible:
            [[fallthrough]];
        case CheatType::AllowSpecialColourSchemes:
            [[fallthrough]];
        case CheatType::AllowTrackPlaceInvalidHeights:
            [[fallthrough]];
        case CheatType::OpenClosePark:
            return { { 0, 1 }, { 0, 0 } };
        case CheatType::AddMoney:
            [[fallthrough]];
        case CheatType::SetMoney:
            return { { std::numeric_limits<money64>::min(), std::numeric_limits<money64>::max() }, { 0, 0 } };
        case CheatType::SetGuestParameter:
            switch (_param1)
            {
                case GUEST_PARAMETER_HAPPINESS:
                    return { { GUEST_PARAMETER_HAPPINESS, GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY },
                             { 0, kPeepMaxHappiness } };
                case GUEST_PARAMETER_ENERGY:
                    return { { GUEST_PARAMETER_HAPPINESS, GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY },
                             { kPeepMinEnergy, kPeepMaxEnergy } };
                case GUEST_PARAMETER_HUNGER:
                    return { { GUEST_PARAMETER_HAPPINESS, GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY }, { 0, kPeepMaxHunger } };
                case GUEST_PARAMETER_THIRST:
                    return { { GUEST_PARAMETER_HAPPINESS, GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY }, { 0, kPeepMaxThirst } };
                case GUEST_PARAMETER_NAUSEA:
                    return { { GUEST_PARAMETER_HAPPINESS, GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY }, { 0, kPeepMaxNausea } };
                case GUEST_PARAMETER_NAUSEA_TOLERANCE:
                    return { { GUEST_PARAMETER_HAPPINESS, GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY },
                             { EnumValue(PeepNauseaTolerance::None), EnumValue(PeepNauseaTolerance::High) } };
                case GUEST_PARAMETER_TOILET:
                    return { { GUEST_PARAMETER_HAPPINESS, GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY }, { 0, kPeepMaxToilet } };
                case GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY:
                    return { { GUEST_PARAMETER_HAPPINESS, GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY }, { 0, 255 } };
                default:
                    return { { 0, 0 }, { 0, 0 } };
            }
        case CheatType::GenerateGuests:
            return { { 1, 10000 }, { 0, 0 } };
        case CheatType::GiveAllGuests:
            return { { OBJECT_MONEY, OBJECT_UMBRELLA }, { 0, 0 } };
        case CheatType::SetGrassLength:
            return { { 0, 7 }, { 0, 0 } };
        case CheatType::SetStaffSpeed:
            return { { 0, 255 }, { 0, 0 } };
        case CheatType::ForceWeather:
            return { { 0, EnumValue(WeatherType::Count) - 1 }, { 0, 0 } };
        case CheatType::SetForcedParkRating:
            return { { -1, 999 }, { 0, 0 } };
        case CheatType::CreateDucks:
            return { { 0, 100 }, { 0, 0 } };
        case CheatType::RemoveDucks:
            [[fallthrough]];
        case CheatType::ClearLoan:
            [[fallthrough]];
        case CheatType::RemoveAllGuests:
            [[fallthrough]];
        case CheatType::OwnAllLand:
            [[fallthrough]];
        case CheatType::WinScenario:
            [[fallthrough]];
        case CheatType::HaveFun:
            [[fallthrough]];
        case CheatType::FixVandalism:
            [[fallthrough]];
        case CheatType::WaterPlants:
            [[fallthrough]];
        case CheatType::FixRides:
            [[fallthrough]];
        case CheatType::RenewRides:
            [[fallthrough]];
        case CheatType::TenMinuteInspections:
            [[fallthrough]];
        case CheatType::ResetCrashStatus:
            [[fallthrough]];
        case CheatType::NoCapOnQueueLengthDummy:
            [[fallthrough]];
        case CheatType::RemoveLitter:
            [[fallthrough]];
        case CheatType::RemoveParkFences:
            return { { 0, 0 }, { 0, 0 } };
        case CheatType::Count:
            break;
    }

    // Should be unreachable unless new cheats are added, still have to return something
    // to avoid compiler warnings.
    Guard::Assert(false, "Missing validation parameter for cheat: %d", _cheatType.id);
    return { { 0, 0 }, { 0, 0 } };
}

void CheatSetAction::SetGrassLength(int32_t length) const
{
    auto& gameState = GetGameState();
    for (int32_t y = 0; y < gameState.MapSize.y; y++)
    {
        for (int32_t x = 0; x < gameState.MapSize.x; x++)
        {
            auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
            if (surfaceElement == nullptr)
                continue;

            if (surfaceElement != nullptr && (surfaceElement->GetOwnership() & OWNERSHIP_OWNED)
                && surfaceElement->GetWaterHeight() == 0 && surfaceElement->CanGrassGrow())
            {
                surfaceElement->SetGrassLength(length);
            }
        }
    }

    GfxInvalidateScreen();
}

void CheatSetAction::WaterPlants() const
{
    TileElementIterator it;

    TileElementIteratorBegin(&it);
    do
    {
        if (it.element->GetType() == TileElementType::SmallScenery)
        {
            it.element->AsSmallScenery()->SetAge(0);
        }
    } while (TileElementIteratorNext(&it));

    GfxInvalidateScreen();
}

void CheatSetAction::FixVandalism() const
{
    TileElementIterator it;

    TileElementIteratorBegin(&it);
    do
    {
        if (it.element->GetType() != TileElementType::Path)
            continue;

        if (!(it.element)->AsPath()->HasAddition())
            continue;

        it.element->AsPath()->SetIsBroken(false);
    } while (TileElementIteratorNext(&it));

    GfxInvalidateScreen();
}

void CheatSetAction::RemoveLitter() const
{
    for (auto litter : EntityList<Litter>())
    {
        EntityRemove(litter);
    }

    TileElementIterator it{};
    TileElementIteratorBegin(&it);
    do
    {
        if (it.element->GetType() != TileElementType::Path)
            continue;

        auto* path = it.element->AsPath();
        if (!path->HasAddition())
            continue;

        auto* pathAdditionEntry = path->GetAdditionEntry();
        if (pathAdditionEntry != nullptr && pathAdditionEntry->flags & PATH_ADDITION_FLAG_IS_BIN)
            path->SetAdditionStatus(0xFF);

    } while (TileElementIteratorNext(&it));

    GfxInvalidateScreen();
}

void CheatSetAction::FixBrokenRides() const
{
    for (auto& ride : GetRideManager())
    {
        if (ride.lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN))
        {
            auto mechanic = RideGetAssignedMechanic(ride);

            if (mechanic != nullptr)
            {
                if (ride.mechanic_status == RIDE_MECHANIC_STATUS_FIXING)
                {
                    mechanic->RideSubState = PeepRideSubState::ApproachExit;
                }
                else if (
                    ride.mechanic_status == RIDE_MECHANIC_STATUS_CALLING
                    || ride.mechanic_status == RIDE_MECHANIC_STATUS_HEADING)
                {
                    mechanic->RemoveFromRide();
                }
            }

            RideFixBreakdown(ride, 0);
            ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
        }
    }
}

void CheatSetAction::RenewRides() const
{
    for (auto& ride : GetRideManager())
    {
        ride.Renew();
    }
    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByClass(WindowClass::Ride);
}

void CheatSetAction::ResetRideCrashStatus() const
{
    for (auto& ride : GetRideManager())
    {
        // Reset crash status and history
        ride.lifecycle_flags &= ~RIDE_LIFECYCLE_CRASHED;
        ride.last_crash_type = RIDE_CRASH_TYPE_NONE;
    }
    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByClass(WindowClass::Ride);
}

void CheatSetAction::Set10MinuteInspection() const
{
    for (auto& ride : GetRideManager())
    {
        // Set inspection interval to 10 minutes
        ride.inspection_interval = RIDE_INSPECTION_EVERY_10_MINUTES;
    }
    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByClass(WindowClass::Ride);
}

void CheatSetAction::SetScenarioNoMoney(bool enabled) const
{
    auto& gameState = GetGameState();
    if (enabled)
    {
        gameState.Park.Flags |= PARK_FLAGS_NO_MONEY;
    }
    else
    {
        gameState.Park.Flags &= ~PARK_FLAGS_NO_MONEY;
    }

    // Invalidate all windows that have anything to do with finance
    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByClass(WindowClass::Ride);
    windowMgr->InvalidateByClass(WindowClass::Peep);
    windowMgr->InvalidateByClass(WindowClass::ParkInformation);
    windowMgr->InvalidateByClass(WindowClass::Finances);
    windowMgr->InvalidateByClass(WindowClass::BottomToolbar);
    windowMgr->InvalidateByClass(WindowClass::TopToolbar);
    windowMgr->InvalidateByClass(WindowClass::Cheats);
}

void CheatSetAction::SetMoney(money64 amount) const
{
    GetGameState().Cash = amount;

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByClass(WindowClass::Finances);
    windowMgr->InvalidateByClass(WindowClass::BottomToolbar);
}

void CheatSetAction::AddMoney(money64 amount) const
{
    auto& gameState = GetGameState();
    gameState.Cash = AddClamp<money64>(gameState.Cash, amount);

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByClass(WindowClass::Finances);
    windowMgr->InvalidateByClass(WindowClass::BottomToolbar);
}

void CheatSetAction::ClearLoan() const
{
    // First give money
    AddMoney(GetGameState().BankLoan);

    // Then pay the loan
    auto gameAction = ParkSetLoanAction(0.00_GBP);
    GameActions::ExecuteNested(&gameAction);
}

void CheatSetAction::GenerateGuests(int32_t count) const
{
    for (int32_t i = 0; i < count; i++)
    {
        Park::GenerateGuest();
    }

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByClass(WindowClass::BottomToolbar);
}

void CheatSetAction::SetGuestParameter(int32_t parameter, int32_t value) const
{
    for (auto peep : EntityList<Guest>())
    {
        switch (parameter)
        {
            case GUEST_PARAMETER_HAPPINESS:
                peep->Happiness = value;
                peep->HappinessTarget = value;
                // Clear the 'red-faced with anger' status if we're making the guest happy
                if (value > 0)
                {
                    peep->PeepFlags &= ~PEEP_FLAGS_ANGRY;
                    peep->Angriness = 0;
                }
                break;
            case GUEST_PARAMETER_ENERGY:
                peep->Energy = value;
                peep->EnergyTarget = value;
                break;
            case GUEST_PARAMETER_HUNGER:
                peep->Hunger = value;
                break;
            case GUEST_PARAMETER_THIRST:
                peep->Thirst = value;
                break;
            case GUEST_PARAMETER_NAUSEA:
                peep->Nausea = value;
                peep->NauseaTarget = value;
                break;
            case GUEST_PARAMETER_NAUSEA_TOLERANCE:
                peep->NauseaTolerance = static_cast<PeepNauseaTolerance>(value);
                break;
            case GUEST_PARAMETER_TOILET:
                peep->Toilet = value;
                break;
            case GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY:
                peep->Intensity = IntensityRange(value, 15);
                break;
        }
        peep->UpdateAnimationGroup();
    }
}

void CheatSetAction::GiveObjectToGuests(int32_t object) const
{
    for (auto peep : EntityList<Guest>())
    {
        switch (object)
        {
            case OBJECT_MONEY:
                peep->CashInPocket = 1000.00_GBP;
                break;
            case OBJECT_PARK_MAP:
                peep->GiveItem(ShopItem::Map);
                break;
            case OBJECT_BALLOON:
                peep->GiveItem(ShopItem::Balloon);
                peep->BalloonColour = ScenarioRandMax(kColourNumNormal);
                peep->UpdateAnimationGroup();
                break;
            case OBJECT_UMBRELLA:
                peep->GiveItem(ShopItem::Umbrella);
                peep->UmbrellaColour = ScenarioRandMax(kColourNumOriginal);
                peep->UpdateAnimationGroup();
                break;
        }
    }

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByClass(WindowClass::Peep);
}

void CheatSetAction::RemoveAllGuests() const
{
    for (auto& ride : GetRideManager())
    {
        ride.num_riders = 0;

        for (auto& station : ride.GetStations())
        {
            station.QueueLength = 0;
            station.LastPeepInQueue = EntityId::GetNull();
        }

        for (auto trainIndex : ride.vehicles)
        {
            for (Vehicle* vehicle = TryGetEntity<Vehicle>(trainIndex); vehicle != nullptr;
                 vehicle = TryGetEntity<Vehicle>(vehicle->next_vehicle_on_train))
            {
                auto i = 0;
                for (auto& peepInTrainIndex : vehicle->peep)
                {
                    if (i >= vehicle->num_peeps)
                        break;

                    auto peep = TryGetEntity<Guest>(peepInTrainIndex);
                    if (peep != nullptr && peep->CurrentRide == ride.id)
                    {
                        if ((peep->State == PeepState::OnRide && peep->RideSubState == PeepRideSubState::OnRide)
                            || (peep->State == PeepState::LeavingRide && peep->RideSubState == PeepRideSubState::LeaveVehicle))
                        {
                            vehicle->ApplyMass(-peep->Mass);
                        }
                    }
                    peepInTrainIndex = EntityId::GetNull();
                    i++;
                }

                vehicle->num_peeps = 0;
                vehicle->next_free_seat = 0;
            }
        }
    }

    // Do not use the FOR_ALL_PEEPS macro for this as next sprite index
    // will be fetched on a deleted guest.
    for (auto guest : EntityList<Guest>())
    {
        guest->Remove();
    }

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByClass(WindowClass::Ride);
    GfxInvalidateScreen();
}

void CheatSetAction::SetStaffSpeed(uint8_t value) const
{
    for (auto peep : EntityList<Staff>())
    {
        peep->Energy = value;
        peep->EnergyTarget = value;
    }
}

void CheatSetAction::OwnAllLand() const
{
    const auto min = CoordsXY{ kCoordsXYStep, kCoordsXYStep };
    const auto max = GetMapSizeUnits() - CoordsXY{ kCoordsXYStep, kCoordsXYStep };

    for (CoordsXY coords = min; coords.y <= max.y; coords.y += kCoordsXYStep)
    {
        for (coords.x = min.x; coords.x <= max.x; coords.x += kCoordsXYStep)
        {
            auto* surfaceElement = MapGetSurfaceElementAt(coords);
            if (surfaceElement == nullptr)
                continue;

            // Ignore already owned tiles.
            if (surfaceElement->GetOwnership() & OWNERSHIP_OWNED)
                continue;

            int32_t baseZ = surfaceElement->GetBaseZ();
            int32_t destOwnership = CheckMaxAllowableLandRightsForTile({ coords, baseZ });

            // only own tiles that were not set to 0
            if (destOwnership != OWNERSHIP_UNOWNED)
            {
                surfaceElement->SetOwnership(destOwnership);
                Park::UpdateFencesAroundTile(coords);
                MapInvalidateTile({ coords, baseZ, baseZ + 16 });
            }
        }
    }

    // Completely unown peep spawn points
    for (const auto& spawn : GetGameState().PeepSpawns)
    {
        auto* surfaceElement = MapGetSurfaceElementAt(spawn);
        if (surfaceElement != nullptr)
        {
            surfaceElement->SetOwnership(OWNERSHIP_UNOWNED);
            Park::UpdateFencesAroundTile(spawn);
            uint16_t baseZ = surfaceElement->GetBaseZ();
            MapInvalidateTile({ spawn, baseZ, baseZ + 16 });
        }
    }

    MapCountRemainingLandRights();
}

void CheatSetAction::ParkSetOpen(bool isOpen) const
{
    auto parkSetParameter = ParkSetParameterAction(isOpen ? ParkParameter::Open : ParkParameter::Close);
    GameActions::ExecuteNested(&parkSetParameter);
}

void CheatSetAction::CreateDucks(int count) const
{
    for (int i = 0; i < count; i++)
    {
        // 100 attempts at finding some water to create a few ducks at
        for (int32_t attempts = 0; attempts < 100; attempts++)
        {
            if (ScenarioCreateDucks())
                break;
        }
    }
}

void CheatSetAction::RemoveParkFences() const
{
    TileElementIterator it;
    TileElementIteratorBegin(&it);
    do
    {
        if (it.element->GetType() == TileElementType::Surface)
        {
            // Remove all park fence flags
            it.element->AsSurface()->SetParkFences(0);
        }
    } while (TileElementIteratorNext(&it));

    GfxInvalidateScreen();
}
