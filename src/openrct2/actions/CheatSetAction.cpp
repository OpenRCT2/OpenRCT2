/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CheatSetAction.h"

#include "../Cheats.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../config/Config.h"
#include "../core/EnumUtils.hpp"
#include "../core/Guard.hpp"
#include "../core/String.hpp"
#include "../drawing/Colour.h"
#include "../drawing/Drawing.h"
#include "../entity/Duck.h"
#include "../entity/EntityList.h"
#include "../entity/EntityRegistry.h"
#include "../entity/Staff.h"
#include "../localisation/StringIds.h"
#include "../network/Network.h"
#include "../object/PathAdditionEntry.h"
#include "../ride/Ride.h"
#include "../ride/RideManager.hpp"
#include "../ride/Vehicle.h"
#include "../scenario/Scenario.h"
#include "../scenario/ScenarioObjective.h"
#include "../ui/WindowManager.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Climate.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/SmallSceneryElement.h"
#include "../world/tile_element/SurfaceElement.h"
#include "ParkSetLoanAction.h"
#include "ParkSetParameterAction.h"

namespace OpenRCT2::GameActions
{
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
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void CheatSetAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_cheatType) << DS_TAG(_param1) << DS_TAG(_param2);
    }

    Result CheatSetAction::Query(GameState_t& gameState) const
    {
        if (static_cast<uint32_t>(_cheatType) >= static_cast<uint32_t>(CheatType::count))
        {
            LOG_ERROR("Invalid cheat type %u", _cheatType);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        ParametersRange validRange = GetParameterRange(static_cast<CheatType>(_cheatType.id));

        if (_param1 < validRange.first.first || _param1 > validRange.first.second)
        {
            LOG_ERROR(
                "The first cheat parameter is out of range. Value = %d, min = %d, max = %d", _param1, validRange.first.first,
                validRange.first.second);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
        }
        if (_param2 < validRange.second.first || _param2 > validRange.second.second)
        {
            LOG_ERROR(
                "The second cheat parameter is out of range. Value = %d, min = %d, max = %d", _param2, validRange.second.first,
                validRange.second.second);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        return Result();
    }

    Result CheatSetAction::Execute(GameState_t& gameState) const
    {
        auto* windowMgr = Ui::GetWindowManager();

        switch (static_cast<CheatType>(_cheatType.id))
        {
            case CheatType::sandboxMode:
                gameState.cheats.sandboxMode = _param1 != 0;
                windowMgr->InvalidateByClass(WindowClass::map);
                windowMgr->InvalidateByClass(WindowClass::footpath);
                break;
            case CheatType::disableClearanceChecks:
                gameState.cheats.disableClearanceChecks = _param1 != 0;
                // Required to update the clearance checks overlay on the Cheats button.
                windowMgr->InvalidateByClass(WindowClass::topToolbar);
                break;
            case CheatType::disableSupportLimits:
                gameState.cheats.disableSupportLimits = _param1 != 0;
                break;
            case CheatType::showAllOperatingModes:
                gameState.cheats.showAllOperatingModes = _param1 != 0;
                break;
            case CheatType::showVehiclesFromOtherTrackTypes:
                gameState.cheats.showVehiclesFromOtherTrackTypes = _param1 != 0;
                break;
            case CheatType::fastLiftHill:
                gameState.cheats.unlockOperatingLimits = _param1 != 0;
                break;
            case CheatType::disableBrakesFailure:
                gameState.cheats.disableBrakesFailure = _param1 != 0;
                break;
            case CheatType::disableAllBreakdowns:
                gameState.cheats.disableAllBreakdowns = _param1 != 0;
                break;
            case CheatType::disableTrainLengthLimit:
                gameState.cheats.disableTrainLengthLimit = _param1 != 0;
                break;
            case CheatType::enableChainLiftOnAllTrack:
                gameState.cheats.enableChainLiftOnAllTrack = _param1 != 0;
                break;
            case CheatType::buildInPauseMode:
                gameState.cheats.buildInPauseMode = _param1 != 0;
                break;
            case CheatType::ignoreRideIntensity:
                gameState.cheats.ignoreRideIntensity = _param1 != 0;
                break;
            case CheatType::ignorePrice:
                gameState.cheats.ignorePrice = _param1 != 0;
                break;
            case CheatType::disableVandalism:
                gameState.cheats.disableVandalism = _param1 != 0;
                break;
            case CheatType::disableLittering:
                gameState.cheats.disableLittering = _param1 != 0;
                break;
            case CheatType::noMoney:
                SetScenarioNoMoney(gameState, _param1 != 0);
                break;
            case CheatType::addMoney:
                AddMoney(gameState, _param1);
                break;
            case CheatType::setMoney:
                SetMoney(gameState, _param1);
                break;
            case CheatType::clearLoan:
                ClearLoan(gameState);
                break;
            case CheatType::setGuestParameter:
                SetGuestParameter(_param1, _param2);
                break;
            case CheatType::generateGuests:
                GenerateGuests(_param1);
                break;
            case CheatType::removeAllGuests:
                RemoveAllGuests(gameState);
                break;
            case CheatType::giveAllGuests:
                GiveObjectToGuests(_param1);
                break;
            case CheatType::setGrassLength:
                SetGrassLength(gameState, _param1);
                break;
            case CheatType::waterPlants:
                WaterPlants();
                break;
            case CheatType::fixVandalism:
                FixVandalism();
                break;
            case CheatType::removeLitter:
                RemoveLitter(gameState);
                break;
            case CheatType::disablePlantAging:
                gameState.cheats.disablePlantAging = _param1 != 0;
                break;
            case CheatType::setStaffSpeed:
                SetStaffSpeed(_param1);
                break;
            case CheatType::renewRides:
                RenewRides(gameState);
                break;
            case CheatType::makeDestructible:
                gameState.cheats.makeAllDestructible = _param1 != 0;
                windowMgr->InvalidateByClass(WindowClass::ride);
                break;
            case CheatType::fixRides:
                FixBrokenRides(gameState);
                break;
            case CheatType::resetCrashStatus:
                ResetRideCrashStatus(gameState);
                break;
            case CheatType::tenMinuteInspections:
                Set10MinuteInspection(gameState);
                break;
            case CheatType::winScenario:
                ScenarioSuccess(gameState);
                break;
            case CheatType::forceWeather:
                // Todo - make sure this is safe
                ClimateForceWeather(WeatherType{ static_cast<uint8_t>(_param1) });
                break;
            case CheatType::freezeWeather:
                gameState.cheats.freezeWeather = _param1 != 0;
                break;
            case CheatType::neverendingMarketing:
                gameState.cheats.neverendingMarketing = _param1 != 0;
                break;
            case CheatType::openClosePark:
                ParkSetOpen(!Park::IsOpen(gameState.park), gameState);
                break;
            case CheatType::haveFun:
                gameState.scenarioOptions.objective.Type = Scenario::ObjectiveType::haveFun;
                break;
            case CheatType::setForcedParkRating:
                Park::SetForcedRating(_param1);
                break;
            case CheatType::allowArbitraryRideTypeChanges:
                gameState.cheats.allowArbitraryRideTypeChanges = _param1 != 0;
                windowMgr->InvalidateByClass(WindowClass::ride);
                break;
            case CheatType::ownAllLand:
                OwnAllLand(gameState);
                break;
            case CheatType::disableRideValueAging:
                gameState.cheats.disableRideValueAging = _param1 != 0;
                break;
            case CheatType::ignoreResearchStatus:
                gameState.cheats.ignoreResearchStatus = _param1 != 0;
                break;
            case CheatType::enableAllDrawableTrackPieces:
                gameState.cheats.enableAllDrawableTrackPieces = _param1 != 0;
                break;
            case CheatType::createDucks:
                CreateDucks(_param1);
                break;
            case CheatType::removeDucks:
                Duck::RemoveAll();
                break;
            case CheatType::allowTrackPlaceInvalidHeights:
                gameState.cheats.allowTrackPlaceInvalidHeights = _param1 != 0;
                break;
            case CheatType::allowRegularPathAsQueue:
                gameState.cheats.allowRegularPathAsQueue = _param1 != 0;
                break;
            case CheatType::allowSpecialColourSchemes:
                gameState.cheats.allowSpecialColourSchemes = static_cast<bool>(_param1);
                break;
            case CheatType::removeParkFences:
                RemoveParkFences();
                break;
            default:
            {
                LOG_ERROR("Invalid cheat type %d", _cheatType.id);
                return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
            }
        }

        if (Network::GetMode() == Network::Mode::none)
        {
            Config::Save();
        }

        windowMgr->InvalidateByClass(WindowClass::cheats);
        return Result();
    }

    ParametersRange CheatSetAction::GetParameterRange(CheatType cheatType) const
    {
        switch (static_cast<CheatType>(_cheatType.id))
        {
            case CheatType::sandboxMode:
                [[fallthrough]];
            case CheatType::disableClearanceChecks:
                [[fallthrough]];
            case CheatType::disableSupportLimits:
                [[fallthrough]];
            case CheatType::showAllOperatingModes:
                [[fallthrough]];
            case CheatType::showVehiclesFromOtherTrackTypes:
                [[fallthrough]];
            case CheatType::fastLiftHill:
                [[fallthrough]];
            case CheatType::disableBrakesFailure:
                [[fallthrough]];
            case CheatType::disableAllBreakdowns:
                [[fallthrough]];
            case CheatType::disableTrainLengthLimit:
                [[fallthrough]];
            case CheatType::enableChainLiftOnAllTrack:
                [[fallthrough]];
            case CheatType::buildInPauseMode:
                [[fallthrough]];
            case CheatType::ignoreRideIntensity:
                [[fallthrough]];
            case CheatType::ignorePrice:
                [[fallthrough]];
            case CheatType::disableVandalism:
                [[fallthrough]];
            case CheatType::disableLittering:
                [[fallthrough]];
            case CheatType::noMoney:
                [[fallthrough]];
            case CheatType::disablePlantAging:
                [[fallthrough]];
            case CheatType::freezeWeather:
                [[fallthrough]];
            case CheatType::neverendingMarketing:
                [[fallthrough]];
            case CheatType::allowArbitraryRideTypeChanges:
                [[fallthrough]];
            case CheatType::disableRideValueAging:
                [[fallthrough]];
            case CheatType::ignoreResearchStatus:
                [[fallthrough]];
            case CheatType::enableAllDrawableTrackPieces:
                [[fallthrough]];
            case CheatType::allowRegularPathAsQueue:
                [[fallthrough]];
            case CheatType::unlockAllPrices:
                [[fallthrough]];
            case CheatType::makeDestructible:
                [[fallthrough]];
            case CheatType::allowSpecialColourSchemes:
                [[fallthrough]];
            case CheatType::allowTrackPlaceInvalidHeights:
                [[fallthrough]];
            case CheatType::openClosePark:
                return { { 0, 1 }, { 0, 0 } };
            case CheatType::addMoney:
                [[fallthrough]];
            case CheatType::setMoney:
                return { { std::numeric_limits<money64>::min(), std::numeric_limits<money64>::max() }, { 0, 0 } };
            case CheatType::setGuestParameter:
                switch (_param1)
                {
                    case GUEST_PARAMETER_HAPPINESS:
                        return { { GUEST_PARAMETER_HAPPINESS, GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY },
                                 { 0, kPeepMaxHappiness } };
                    case GUEST_PARAMETER_ENERGY:
                        return { { GUEST_PARAMETER_HAPPINESS, GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY },
                                 { kPeepMinEnergy, kPeepMaxEnergy } };
                    case GUEST_PARAMETER_HUNGER:
                        return { { GUEST_PARAMETER_HAPPINESS, GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY },
                                 { 0, kPeepMaxHunger } };
                    case GUEST_PARAMETER_THIRST:
                        return { { GUEST_PARAMETER_HAPPINESS, GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY },
                                 { 0, kPeepMaxThirst } };
                    case GUEST_PARAMETER_NAUSEA:
                        return { { GUEST_PARAMETER_HAPPINESS, GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY },
                                 { 0, kPeepMaxNausea } };
                    case GUEST_PARAMETER_NAUSEA_TOLERANCE:
                        return { { GUEST_PARAMETER_HAPPINESS, GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY },
                                 { EnumValue(PeepNauseaTolerance::None), EnumValue(PeepNauseaTolerance::High) } };
                    case GUEST_PARAMETER_TOILET:
                        return { { GUEST_PARAMETER_HAPPINESS, GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY },
                                 { 0, kPeepMaxToilet } };
                    case GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY:
                        return { { GUEST_PARAMETER_HAPPINESS, GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY }, { 0, 255 } };
                    default:
                        return { { 0, 0 }, { 0, 0 } };
                }
            case CheatType::generateGuests:
                return { { 1, 10000 }, { 0, 0 } };
            case CheatType::giveAllGuests:
                return { { OBJECT_MONEY, OBJECT_UMBRELLA }, { 0, 0 } };
            case CheatType::setGrassLength:
                return { { 0, 7 }, { 0, 0 } };
            case CheatType::setStaffSpeed:
                return { { 0, 255 }, { 0, 0 } };
            case CheatType::forceWeather:
                return { { 0, EnumValue(WeatherType::Count) - 1 }, { 0, 0 } };
            case CheatType::setForcedParkRating:
                return { { -1, 999 }, { 0, 0 } };
            case CheatType::createDucks:
                return { { 0, 100 }, { 0, 0 } };
            case CheatType::removeDucks:
                [[fallthrough]];
            case CheatType::clearLoan:
                [[fallthrough]];
            case CheatType::removeAllGuests:
                [[fallthrough]];
            case CheatType::ownAllLand:
                [[fallthrough]];
            case CheatType::winScenario:
                [[fallthrough]];
            case CheatType::haveFun:
                [[fallthrough]];
            case CheatType::fixVandalism:
                [[fallthrough]];
            case CheatType::waterPlants:
                [[fallthrough]];
            case CheatType::fixRides:
                [[fallthrough]];
            case CheatType::renewRides:
                [[fallthrough]];
            case CheatType::tenMinuteInspections:
                [[fallthrough]];
            case CheatType::resetCrashStatus:
                [[fallthrough]];
            case CheatType::noCapOnQueueLengthDummy:
                [[fallthrough]];
            case CheatType::removeLitter:
                [[fallthrough]];
            case CheatType::removeParkFences:
                return { { 0, 0 }, { 0, 0 } };
            case CheatType::count:
                break;
        }

        // Should be unreachable unless new cheats are added, still have to return something
        // to avoid compiler warnings.
        Guard::Assert(false, "Missing validation parameter for cheat: %d", _cheatType.id);
        return { { 0, 0 }, { 0, 0 } };
    }

    void CheatSetAction::SetGrassLength(GameState_t& gameState, int32_t length) const
    {
        for (int32_t y = 0; y < gameState.mapSize.y; y++)
        {
            for (int32_t x = 0; x < gameState.mapSize.x; x++)
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

    void CheatSetAction::RemoveLitter(GameState_t& gameState) const
    {
        for (auto litter : EntityList<Litter>())
        {
            gameState.entities.EntityRemove(litter);
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

    void CheatSetAction::FixBrokenRides(GameState_t& gameState) const
    {
        for (auto& ride : RideManager(gameState))
        {
            if (ride.lifecycleFlags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN))
            {
                auto mechanic = RideGetAssignedMechanic(ride);

                if (mechanic != nullptr)
                {
                    if (ride.mechanicStatus == MechanicStatus::fixing)
                    {
                        mechanic->RideSubState = PeepRideSubState::approachExit;
                    }
                    else if (ride.mechanicStatus == MechanicStatus::calling || ride.mechanicStatus == MechanicStatus::heading)
                    {
                        mechanic->RemoveFromRide();
                    }
                }

                RideFixBreakdown(ride, 0);
                ride.windowInvalidateFlags.set(RideInvalidateFlag::main, RideInvalidateFlag::list);
            }
        }
    }

    void CheatSetAction::RenewRides(GameState_t& gameState) const
    {
        for (auto& ride : RideManager(gameState))
        {
            ride.renew();
        }
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByClass(WindowClass::ride);
    }

    void CheatSetAction::ResetRideCrashStatus(GameState_t& gameState) const
    {
        for (auto& ride : RideManager(gameState))
        {
            // Reset crash status and history
            ride.lifecycleFlags &= ~RIDE_LIFECYCLE_CRASHED;
            ride.lastCrashType = RIDE_CRASH_TYPE_NONE;
        }
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByClass(WindowClass::ride);
    }

    void CheatSetAction::Set10MinuteInspection(GameState_t& gameState) const
    {
        for (auto& ride : RideManager(gameState))
        {
            // Set inspection interval to 10 minutes
            ride.inspectionInterval = RideInspection::every10Minutes;
        }
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByClass(WindowClass::ride);
    }

    void CheatSetAction::SetScenarioNoMoney(GameState_t& gameState, bool enabled) const
    {
        auto& park = gameState.park;
        if (enabled)
        {
            park.flags |= PARK_FLAGS_NO_MONEY;
        }
        else
        {
            park.flags &= ~PARK_FLAGS_NO_MONEY;
        }

        // Invalidate all windows that have anything to do with finance
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByClass(WindowClass::ride);
        windowMgr->InvalidateByClass(WindowClass::peep);
        windowMgr->InvalidateByClass(WindowClass::parkInformation);
        windowMgr->InvalidateByClass(WindowClass::finances);
        windowMgr->InvalidateByClass(WindowClass::bottomToolbar);
        windowMgr->InvalidateByClass(WindowClass::topToolbar);
        windowMgr->InvalidateByClass(WindowClass::cheats);
    }

    void CheatSetAction::SetMoney(GameState_t& gameState, money64 amount) const
    {
        gameState.park.cash = amount;

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByClass(WindowClass::finances);
        windowMgr->InvalidateByClass(WindowClass::bottomToolbar);
    }

    void CheatSetAction::AddMoney(GameState_t& gameState, money64 amount) const
    {
        auto& park = gameState.park;
        park.cash = AddClamp(park.cash, amount);

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByClass(WindowClass::finances);
        windowMgr->InvalidateByClass(WindowClass::bottomToolbar);
    }

    void CheatSetAction::ClearLoan(GameState_t& gameState) const
    {
        // First give money
        AddMoney(gameState, gameState.park.bankLoan);

        // Then pay the loan
        auto gameAction = ParkSetLoanAction(0.00_GBP);
        ExecuteNested(&gameAction, gameState);
    }

    void CheatSetAction::GenerateGuests(int32_t count) const
    {
        for (int32_t i = 0; i < count; i++)
        {
            Park::GenerateGuest();
        }

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByClass(WindowClass::bottomToolbar);
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
                    peep->GiveItem(ShopItem::map);
                    break;
                case OBJECT_BALLOON:
                    peep->GiveItem(ShopItem::balloon);
                    peep->BalloonColour = static_cast<Drawing::Colour>(ScenarioRandMax(Drawing::kColourNumNormal));
                    peep->UpdateAnimationGroup();
                    break;
                case OBJECT_UMBRELLA:
                    peep->GiveItem(ShopItem::umbrella);
                    peep->UmbrellaColour = static_cast<Drawing::Colour>(ScenarioRandMax(Drawing::kColourNumNormal));
                    peep->UpdateAnimationGroup();
                    break;
            }
        }

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByClass(WindowClass::peep);
    }

    void CheatSetAction::RemoveAllGuests(GameState_t& gameState) const
    {
        for (auto& ride : RideManager(gameState))
        {
            ride.numRiders = 0;
            ride.slideInUse = 0;

            for (auto& station : ride.getStations())
            {
                station.QueueLength = 0;
                station.LastPeepInQueue = EntityId::GetNull();
            }

            for (auto trainIndex : ride.vehicles)
            {
                for (Vehicle* vehicle = gameState.entities.TryGetEntity<Vehicle>(trainIndex); vehicle != nullptr;
                     vehicle = gameState.entities.TryGetEntity<Vehicle>(vehicle->next_vehicle_on_train))
                {
                    auto i = 0;
                    for (auto& peepInTrainIndex : vehicle->peep)
                    {
                        if (i >= vehicle->num_peeps)
                            break;

                        auto peep = gameState.entities.TryGetEntity<Guest>(peepInTrainIndex);
                        if (peep != nullptr && peep->CurrentRide == ride.id)
                        {
                            if ((peep->State == PeepState::onRide && peep->RideSubState == PeepRideSubState::onRide)
                                || (peep->State == PeepState::leavingRide
                                    && peep->RideSubState == PeepRideSubState::leaveVehicle))
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
            // Frozen peeps are usually placed and tweaked for artistic purposes,
            // so exempt them from being removed.
            if (guest->PeepFlags & PEEP_FLAGS_POSITION_FROZEN)
                continue;

            guest->Remove();
        }

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByClass(WindowClass::ride);
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

    void CheatSetAction::OwnAllLand(GameState_t& gameState) const
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
        for (const auto& spawn : gameState.peepSpawns)
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

    void CheatSetAction::ParkSetOpen(bool isOpen, GameState_t& gameState) const
    {
        auto parkSetParameter = ParkSetParameterAction(isOpen ? ParkParameter::Open : ParkParameter::Close);
        ExecuteNested(&parkSetParameter, gameState);
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
} // namespace OpenRCT2::GameActions
