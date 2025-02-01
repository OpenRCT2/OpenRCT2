/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Park.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Date.h"
#include "../Game.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../actions/ParkSetParameterAction.h"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "../entity/Litter.h"
#include "../entity/Peep.h"
#include "../entity/Staff.h"
#include "../interface/Colour.h"
#include "../management/Award.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/Research.h"
#include "../network/network.h"
#include "../profiling/Profiling.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/RideManager.hpp"
#include "../ride/ShopItem.h"
#include "../scenario/Scenario.h"
#include "../scripting/ScriptEngine.h"
#include "../ui/WindowManager.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "Entrance.h"
#include "Map.h"
#include "tile_element/EntranceElement.h"
#include "tile_element/SurfaceElement.h"

#include <limits>
#include <type_traits>

using namespace OpenRCT2;
using namespace OpenRCT2::Scripting;

namespace OpenRCT2::Park
{
    static money64 calculateRideValue(const Ride& ride);
    static money64 calculateTotalRideValueForMoney();
    static uint32_t calculateSuggestedMaxGuests();
    static uint32_t calculateGuestGenerationProbability();

    static void generateGuests(GameState_t& gameState);
    static Guest* generateGuestFromCampaign(int32_t campaign);

    /**
     * Choose a random peep spawn and iterates through until defined spawn is found.
     */
    static PeepSpawn* GetRandomPeepSpawn()
    {
        auto& gameState = GetGameState();
        if (!gameState.PeepSpawns.empty())
        {
            return &gameState.PeepSpawns[ScenarioRand() % gameState.PeepSpawns.size()];
        }

        return nullptr;
    }

    static money64 calculateRideValue(const Ride& ride)
    {
        money64 result = 0;
        if (ride.value != RIDE_VALUE_UNDEFINED)
        {
            const auto& rtd = ride.GetRideTypeDescriptor();
            result = (ride.value * 10) * (static_cast<money64>(RideCustomersInLast5Minutes(ride)) + rtd.BonusValue * 4LL);
        }
        return result;
    }

    static money64 calculateTotalRideValueForMoney()
    {
        money64 totalRideValue = 0;
        bool ridePricesUnlocked = RidePricesUnlocked() && !(GetGameState().Park.Flags & PARK_FLAGS_NO_MONEY);
        for (auto& ride : GetRideManager())
        {
            if (ride.status != RideStatus::Open)
                continue;
            if (ride.lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
                continue;
            if (ride.lifecycle_flags & RIDE_LIFECYCLE_CRASHED)
                continue;

            // Add ride value
            if (ride.value != RIDE_VALUE_UNDEFINED)
            {
                money64 rideValue = ride.value;
                if (ridePricesUnlocked)
                {
                    rideValue -= ride.price[0];
                }
                if (rideValue > 0)
                {
                    totalRideValue += rideValue * 2;
                }
            }
        }
        return totalRideValue;
    }

    static uint32_t calculateSuggestedMaxGuests()
    {
        uint32_t suggestedMaxGuests = 0;
        uint32_t difficultGenerationBonus = 0;

        auto& gameState = GetGameState();

        for (auto& ride : GetRideManager())
        {
            if (ride.status != RideStatus::Open)
                continue;
            if (ride.lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
                continue;
            if (ride.lifecycle_flags & RIDE_LIFECYCLE_CRASHED)
                continue;

            // Add guest score for ride type
            suggestedMaxGuests += ride.GetRideTypeDescriptor().BonusValue;

            // If difficult guest generation, extra guests are available for good rides
            if (gameState.Park.Flags & PARK_FLAGS_DIFFICULT_GUEST_GENERATION)
            {
                if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
                    continue;
                if (!ride.GetRideTypeDescriptor().HasFlag(RtdFlag::hasTrack))
                    continue;
                if (!ride.GetRideTypeDescriptor().HasFlag(RtdFlag::hasDataLogging))
                    continue;
                if (ride.GetStation().SegmentLength < (600 << 16))
                    continue;
                if (ride.ratings.excitement < MakeRideRating(6, 00))
                    continue;

                // Bonus guests for good ride
                difficultGenerationBonus += ride.GetRideTypeDescriptor().BonusValue * 2;
            }
        }

        if (gameState.Park.Flags & PARK_FLAGS_DIFFICULT_GUEST_GENERATION)
        {
            suggestedMaxGuests = std::min<uint32_t>(suggestedMaxGuests, 1000);
            suggestedMaxGuests += difficultGenerationBonus;
        }

        suggestedMaxGuests = std::min<uint32_t>(suggestedMaxGuests, 65535);

#ifdef ENABLE_SCRIPTING
        auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
        if (hookEngine.HasSubscriptions(HOOK_TYPE::PARK_CALCULATE_GUEST_CAP))
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto obj = DukObject(ctx);
            obj.Set("suggestedGuestMaximum", suggestedMaxGuests);
            auto e = obj.Take();
            hookEngine.Call(HOOK_TYPE::PARK_CALCULATE_GUEST_CAP, e, true);

            suggestedMaxGuests = AsOrDefault(e["suggestedGuestMaximum"], static_cast<int32_t>(suggestedMaxGuests));
            suggestedMaxGuests = std::clamp<uint16_t>(suggestedMaxGuests, 0, UINT16_MAX);
        }
#endif
        return suggestedMaxGuests;
    }

    static uint32_t calculateGuestGenerationProbability()
    {
        auto& gameState = GetGameState();

        // Begin with 50 + park rating
        uint32_t probability = 50 + std::clamp(gameState.Park.Rating - 200, 0, 650);

        // The more guests, the lower the chance of a new one
        uint32_t numGuests = gameState.NumGuestsInPark + gameState.NumGuestsHeadingForPark;
        if (numGuests > gameState.SuggestedGuestMaximum)
        {
            probability /= 4;
            // Even lower for difficult guest generation
            if (gameState.Park.Flags & PARK_FLAGS_DIFFICULT_GUEST_GENERATION)
            {
                probability /= 4;
            }
        }

        // Reduces chance for any more than 7000 guests
        if (numGuests > 7000)
        {
            probability /= 4;
        }

        // Penalty for overpriced entrance fee relative to total ride value
        auto entranceFee = GetEntranceFee();
        if (entranceFee > gameState.TotalRideValueForMoney)
        {
            probability /= 4;
            // Extra penalty for very overpriced entrance fee
            if (entranceFee / 2 > gameState.TotalRideValueForMoney)
            {
                probability /= 4;
            }
        }

        // Reward or penalties for park awards
        for (const auto& award : GetGameState().CurrentAwards)
        {
            // +/- 0.25% of the probability
            if (AwardIsPositive(award.Type))
            {
                probability += probability / 4;
            }
            else
            {
                probability -= probability / 4;
            }
        }

        return probability;
    }

    static void generateGuests(GameState_t& gameState)
    {
        // Generate a new guest for some probability
        if (static_cast<int32_t>(ScenarioRand() & 0xFFFF) < gameState.GuestGenerationProbability)
        {
            bool difficultGeneration = (gameState.Park.Flags & PARK_FLAGS_DIFFICULT_GUEST_GENERATION) != 0;
            if (!difficultGeneration || gameState.SuggestedGuestMaximum + 150 >= gameState.NumGuestsInPark)
            {
                GenerateGuest();
            }
        }

        // Extra guests generated by advertising campaigns
        for (const auto& campaign : gameState.MarketingCampaigns)
        {
            // Random chance of guest generation
            auto probability = MarketingGetCampaignGuestGenerationProbability(campaign.Type);
            auto random = ScenarioRandMax(std::numeric_limits<uint16_t>::max());
            if (random < probability)
            {
                generateGuestFromCampaign(campaign.Type);
            }
        }
    }

    static Guest* generateGuestFromCampaign(int32_t campaign)
    {
        auto peep = GenerateGuest();
        if (peep != nullptr)
        {
            MarketingSetGuestCampaign(peep, campaign);
        }
        return peep;
    }

    template<typename T, size_t TSize>
    static void HistoryPushRecord(T history[TSize], T newItem)
    {
        for (size_t i = TSize - 1; i > 0; i--)
        {
            history[i] = history[i - 1];
        }
        history[0] = newItem;
    }

    void Initialise(GameState_t& gameState)
    {
        gameState.Park.Name = LanguageGetString(STR_UNNAMED_PARK);
        gameState.PluginStorage = {};
        gameState.StaffHandymanColour = COLOUR_BRIGHT_RED;
        gameState.StaffMechanicColour = COLOUR_LIGHT_BLUE;
        gameState.StaffSecurityColour = COLOUR_YELLOW;
        gameState.NumGuestsInPark = 0;
        gameState.NumGuestsInParkLastWeek = 0;
        gameState.NumGuestsHeadingForPark = 0;
        gameState.GuestChangeModifier = 0;
        gameState.Park.Rating = 0;
        gameState.GuestGenerationProbability = 0;
        gameState.TotalRideValueForMoney = 0;
        gameState.SuggestedGuestMaximum = 0;
        gameState.ResearchLastItem = std::nullopt;
        gameState.MarketingCampaigns.clear();

        ResearchResetItems(gameState);
        FinanceInit();

        SetEveryRideTypeNotInvented();

        SetAllSceneryItemsInvented();

        gameState.Park.EntranceFee = 10.00_GBP;

        gameState.PeepSpawns.clear();
        ParkEntranceReset();

        gameState.ResearchPriorities = EnumsToFlags(
            ResearchCategory::Transport, ResearchCategory::Gentle, ResearchCategory::Rollercoaster, ResearchCategory::Thrill,
            ResearchCategory::Water, ResearchCategory::Shop, ResearchCategory::SceneryGroup);
        gameState.ResearchFundingLevel = RESEARCH_FUNDING_NORMAL;

        gameState.GuestInitialCash = 50.00_GBP;
        gameState.GuestInitialHappiness = Park::CalculateGuestInitialHappiness(50);
        gameState.GuestInitialHunger = 200;
        gameState.GuestInitialThirst = 200;
        gameState.ScenarioObjective.Type = OBJECTIVE_GUESTS_BY;
        gameState.ScenarioObjective.Year = 4;
        gameState.ScenarioObjective.NumGuests = 1000;
        gameState.LandPrice = 90.00_GBP;
        gameState.ConstructionRightsPrice = 40.00_GBP;
        gameState.Park.Flags = PARK_FLAGS_NO_MONEY | PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
        ResetHistories(gameState);
        FinanceResetHistory();
        AwardReset();

        gameState.ScenarioName.clear();
        gameState.ScenarioDetails = String::toStd(LanguageGetString(STR_NO_DETAILS_YET));
    }

    void Update(GameState_t& gameState, const Date& date)
    {
        PROFILED_FUNCTION();

        // Every new week
        if (date.IsWeekStart())
        {
            UpdateHistories(gameState);
        }

        const auto currentTicks = gameState.CurrentTicks;
        auto* windowMgr = Ui::GetWindowManager();

        // Every ~13 seconds
        if (currentTicks % 512 == 0)
        {
            gameState.Park.Rating = CalculateParkRating();
            gameState.Park.Value = Park::CalculateParkValue();
            gameState.CompanyValue = CalculateCompanyValue();
            gameState.TotalRideValueForMoney = calculateTotalRideValueForMoney();
            gameState.SuggestedGuestMaximum = calculateSuggestedMaxGuests();
            gameState.GuestGenerationProbability = calculateGuestGenerationProbability();

            windowMgr->InvalidateByClass(WindowClass::Finances);
            auto intent = Intent(INTENT_ACTION_UPDATE_PARK_RATING);
            ContextBroadcastIntent(&intent);
        }

        // Every ~102 seconds
        if (currentTicks % 4096 == 0)
        {
            gameState.Park.Size = CalculateParkSize();
            windowMgr->InvalidateByClass(WindowClass::ParkInformation);
        }

        generateGuests(gameState);
    }

    uint32_t CalculateParkSize()
    {
        uint32_t tiles = 0;
        TileElementIterator it;
        TileElementIteratorBegin(&it);
        do
        {
            if (it.element->GetType() == TileElementType::Surface)
            {
                if (it.element->AsSurface()->GetOwnership() & (OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED | OWNERSHIP_OWNED))
                {
                    tiles++;
                }
            }
        } while (TileElementIteratorNext(&it));

        auto& gameState = GetGameState();
        if (tiles != gameState.Park.Size)
        {
            gameState.Park.Size = tiles;
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->InvalidateByClass(WindowClass::ParkInformation);
        }

        return tiles;
    }

    int32_t CalculateParkRating()
    {
        auto& gameState = GetGameState();

        if (gameState.Cheats.forcedParkRating != kForcedParkRatingDisabled)
        {
            return gameState.Cheats.forcedParkRating;
        }

        int32_t result = 1150;
        if (gameState.Park.Flags & PARK_FLAGS_DIFFICULT_PARK_RATING)
        {
            result = 1050;
        }

        // Guests
        {
            // -150 to +3 based on a range of guests from 0 to 2000
            result -= 150 - (std::min<int32_t>(2000, gameState.NumGuestsInPark) / 13);

            // Find the number of happy peeps and the number of peeps who can't find the park exit
            uint32_t happyGuestCount = 0;
            uint32_t lostGuestCount = 0;
            for (auto peep : EntityList<Guest>())
            {
                if (!peep->OutsideOfPark)
                {
                    if (peep->Happiness > 128)
                    {
                        happyGuestCount++;
                    }
                    if ((peep->PeepFlags & PEEP_FLAGS_LEAVING_PARK) && (peep->GuestIsLostCountdown < 90))
                    {
                        lostGuestCount++;
                    }
                }
            }

            // Peep happiness -500 to +0
            result -= 500;
            if (gameState.NumGuestsInPark > 0)
            {
                result += 2 * std::min(250u, (happyGuestCount * 300) / gameState.NumGuestsInPark);
            }

            // Up to 25 guests can be lost without affecting the park rating.
            if (lostGuestCount > 25)
            {
                result -= (lostGuestCount - 25) * 7;
            }
        }

        // Rides
        {
            int32_t rideCount = 0;
            int32_t excitingRideCount = 0;
            int32_t totalRideUptime = 0;
            int32_t totalRideIntensity = 0;
            int32_t totalRideExcitement = 0;
            for (auto& ride : GetRideManager())
            {
                totalRideUptime += 100 - ride.downtime;
                if (RideHasRatings(ride))
                {
                    totalRideExcitement += ride.ratings.excitement / 8;
                    totalRideIntensity += ride.ratings.intensity / 8;
                    excitingRideCount++;
                }
                rideCount++;
            }
            result -= 200;
            if (rideCount > 0)
            {
                result += (totalRideUptime / rideCount) * 2;
            }
            result -= 100;
            if (excitingRideCount > 0)
            {
                int32_t averageExcitement = totalRideExcitement / excitingRideCount;
                int32_t averageIntensity = totalRideIntensity / excitingRideCount;

                averageExcitement -= 46;
                if (averageExcitement < 0)
                {
                    averageExcitement = -averageExcitement;
                }

                averageIntensity -= 65;
                if (averageIntensity < 0)
                {
                    averageIntensity = -averageIntensity;
                }

                averageExcitement = std::min(averageExcitement / 2, 50);
                averageIntensity = std::min(averageIntensity / 2, 50);
                result += 100 - averageExcitement - averageIntensity;
            }

            totalRideExcitement = std::min<int32_t>(1000, totalRideExcitement);
            totalRideIntensity = std::min<int32_t>(1000, totalRideIntensity);
            result -= 200 - ((totalRideExcitement + totalRideIntensity) / 10);
        }

        // Litter
        {
            // Counts the amount of litter whose age is min. 7680 ticks (5~ min) old.
            const auto litterList = EntityList<Litter>();
            const auto litterCount = std::count_if(
                litterList.begin(), litterList.end(), [](auto* litter) { return litter->GetAge() >= 7680; });

            result -= 600 - (4 * (150 - std::min<int32_t>(150, litterCount)));
        }

        result -= gameState.Park.RatingCasualtyPenalty;
        result = std::clamp(result, 0, 999);
        return result;
    }

    money64 CalculateParkValue()
    {
        // Sum ride values
        money64 result = 0;
        for (const auto& ride : GetRideManager())
        {
            result += calculateRideValue(ride);
        }

        // +7.00 per guest
        result += static_cast<money64>(GetGameState().NumGuestsInPark) * 7.00_GBP;

        return result;
    }

    money64 CalculateCompanyValue()
    {
        const auto& gameState = GetGameState();

        auto result = gameState.Park.Value - gameState.BankLoan;

        // Clamp addition to prevent overflow
        result = AddClamp<money64>(result, FinanceGetCurrentCash());

        return result;
    }

    uint8_t CalculateGuestInitialHappiness(uint8_t percentage)
    {
        percentage = std::clamp<uint8_t>(percentage, 15, 98);

        // The percentages follow this sequence:
        //   15 17 18 20 21 23 25 26 28 29 31 32 34 36 37 39 40 42 43 45 47 48 50 51 53...
        // This sequence can be defined as PI*(9+n)/2 (the value is floored)
        for (uint8_t n = 1; n < 55; n++)
        {
            // Avoid floating point math by rescaling PI up.
            constexpr int32_t SCALE = 100000;
            constexpr int32_t PI_SCALED = 314159; // PI * SCALE;
            if (((PI_SCALED * (9 + n)) / SCALE) / 2 >= percentage)
            {
                return (9 + n) * 4;
            }
        }

        // This is the lowest possible value:
        return 40;
    }

    Guest* GenerateGuest()
    {
        Guest* peep = nullptr;
        const auto spawn = GetRandomPeepSpawn();
        if (spawn != nullptr)
        {
            auto direction = DirectionReverse(spawn->direction);
            peep = Guest::Generate({ spawn->x, spawn->y, spawn->z });
            if (peep != nullptr)
            {
                peep->Orientation = direction << 3;

                auto destination = peep->GetLocation().ToTileCentre();
                peep->SetDestination(destination, 5);
                peep->PeepDirection = direction;
                peep->Var37 = 0;
                peep->State = PeepState::EnteringPark;
            }
        }
        return peep;
    }

    void ResetHistories(GameState_t& gameState)
    {
        std::fill(
            std::begin(gameState.Park.RatingHistory), std::end(gameState.Park.RatingHistory), kParkRatingHistoryUndefined);
        std::fill(
            std::begin(gameState.GuestsInParkHistory), std::end(gameState.GuestsInParkHistory), kGuestsInParkHistoryUndefined);
    }

    void UpdateHistories(GameState_t& gameState)
    {
        uint8_t guestChangeModifier = 1;
        int32_t changeInGuestsInPark = static_cast<int32_t>(gameState.NumGuestsInPark)
            - static_cast<int32_t>(gameState.NumGuestsInParkLastWeek);
        if (changeInGuestsInPark > -20)
        {
            guestChangeModifier++;
            if (changeInGuestsInPark < 20)
            {
                guestChangeModifier = 0;
            }
        }
        gameState.GuestChangeModifier = guestChangeModifier;
        gameState.NumGuestsInParkLastWeek = gameState.NumGuestsInPark;

        // Update park rating, guests in park and current cash history
        constexpr auto ratingHistorySize = std::extent_v<decltype(ParkData::RatingHistory)>;
        HistoryPushRecord<uint16_t, ratingHistorySize>(gameState.Park.RatingHistory, gameState.Park.Rating);
        constexpr auto numGuestsHistorySize = std::extent_v<decltype(GameState_t::GuestsInParkHistory)>;
        HistoryPushRecord<uint32_t, numGuestsHistorySize>(gameState.GuestsInParkHistory, gameState.NumGuestsInPark);

        constexpr auto cashHistorySize = std::extent_v<decltype(GameState_t::CashHistory)>;
        HistoryPushRecord<money64, cashHistorySize>(gameState.CashHistory, FinanceGetCurrentCash() - gameState.BankLoan);

        // Update weekly profit history
        auto currentWeeklyProfit = gameState.WeeklyProfitAverageDividend;
        if (gameState.WeeklyProfitAverageDivisor != 0)
        {
            currentWeeklyProfit /= gameState.WeeklyProfitAverageDivisor;
        }
        constexpr auto profitHistorySize = std::extent_v<decltype(GameState_t::WeeklyProfitHistory)>;
        HistoryPushRecord<money64, profitHistorySize>(gameState.WeeklyProfitHistory, currentWeeklyProfit);
        gameState.WeeklyProfitAverageDividend = 0;
        gameState.WeeklyProfitAverageDivisor = 0;

        // Update park value history
        constexpr auto parkValueHistorySize = std::extent_v<decltype(GameState_t::WeeklyProfitHistory)>;
        HistoryPushRecord<money64, parkValueHistorySize>(gameState.Park.ValueHistory, gameState.Park.Value);

        // Invalidate relevant windows
        auto intent = Intent(INTENT_ACTION_UPDATE_GUEST_COUNT);
        ContextBroadcastIntent(&intent);

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByClass(WindowClass::ParkInformation);
        windowMgr->InvalidateByClass(WindowClass::Finances);
    }

    uint32_t UpdateSize(GameState_t& gameState)
    {
        auto tiles = CalculateParkSize();
        if (tiles != gameState.Park.Size)
        {
            gameState.Park.Size = tiles;

            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->InvalidateByClass(WindowClass::ParkInformation);
        }
        return tiles;
    }

    void SetOpen(bool open)
    {
        auto parkSetParameter = ParkSetParameterAction(open ? ParkParameter::Open : ParkParameter::Close);
        GameActions::Execute(&parkSetParameter);
    }

    /**
     *
     *  rct2: 0x00664D05
     */
    void UpdateFences(const CoordsXY& coords)
    {
        if (MapIsEdge(coords))
            return;

        auto surfaceElement = MapGetSurfaceElementAt(coords);
        if (surfaceElement == nullptr)
            return;

        uint8_t newFences = 0;
        if ((surfaceElement->GetOwnership() & OWNERSHIP_OWNED) == 0)
        {
            bool fenceRequired = true;

            TileElement* tileElement = MapGetFirstElementAt(coords);
            if (tileElement == nullptr)
                return;
            // If an entrance element do not place flags around surface
            do
            {
                if (tileElement->GetType() != TileElementType::Entrance)
                    continue;

                if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_PARK_ENTRANCE)
                    continue;

                if (!(tileElement->IsGhost()))
                {
                    fenceRequired = false;
                    break;
                }
            } while (!(tileElement++)->IsLastForTile());

            if (fenceRequired)
            {
                if (MapIsLocationInPark({ coords.x - kCoordsXYStep, coords.y }))
                {
                    newFences |= 0x8;
                }

                if (MapIsLocationInPark({ coords.x, coords.y - kCoordsXYStep }))
                {
                    newFences |= 0x4;
                }

                if (MapIsLocationInPark({ coords.x + kCoordsXYStep, coords.y }))
                {
                    newFences |= 0x2;
                }

                if (MapIsLocationInPark({ coords.x, coords.y + kCoordsXYStep }))
                {
                    newFences |= 0x1;
                }
            }
        }

        if (surfaceElement->GetParkFences() != newFences)
        {
            int32_t baseZ = surfaceElement->GetBaseZ();
            int32_t clearZ = baseZ + 16;
            MapInvalidateTile({ coords, baseZ, clearZ });
            surfaceElement->SetParkFences(newFences);
        }
    }

    void UpdateFencesAroundTile(const CoordsXY& coords)
    {
        UpdateFences(coords);
        UpdateFences({ coords.x + kCoordsXYStep, coords.y });
        UpdateFences({ coords.x - kCoordsXYStep, coords.y });
        UpdateFences({ coords.x, coords.y + kCoordsXYStep });
        UpdateFences({ coords.x, coords.y - kCoordsXYStep });
    }

    void SetForcedRating(int32_t rating)
    {
        auto& gameState = GetGameState();
        gameState.Cheats.forcedParkRating = rating;
        gameState.Park.Rating = CalculateParkRating();

        auto intent = Intent(INTENT_ACTION_UPDATE_PARK_RATING);
        ContextBroadcastIntent(&intent);
    }

    int32_t GetForcedRating()
    {
        return GetGameState().Cheats.forcedParkRating;
    }

    money64 GetEntranceFee()
    {
        const auto& gameState = GetGameState();
        if (gameState.Park.Flags & PARK_FLAGS_NO_MONEY)
        {
            return 0;
        }
        if (!EntranceFeeUnlocked())
        {
            return 0;
        }
        return gameState.Park.EntranceFee;
    }

    bool RidePricesUnlocked()
    {
        const auto& gameState = GetGameState();
        if (gameState.Park.Flags & PARK_FLAGS_UNLOCK_ALL_PRICES)
        {
            return true;
        }
        if (gameState.Park.Flags & PARK_FLAGS_PARK_FREE_ENTRY)
        {
            return true;
        }
        return false;
    }

    bool EntranceFeeUnlocked()
    {
        const auto& gameState = GetGameState();
        if (gameState.Park.Flags & PARK_FLAGS_UNLOCK_ALL_PRICES)
        {
            return true;
        }
        if (!(gameState.Park.Flags & PARK_FLAGS_PARK_FREE_ENTRY))
        {
            return true;
        }
        return false;
    }

    bool ParkData::IsOpen() const
    {
        return (Flags & PARK_FLAGS_PARK_OPEN) != 0;
    }
} // namespace OpenRCT2::Park
