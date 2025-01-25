/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "InteractiveConsole.h"

#include "../Context.h"
#include "../Date.h"
#include "../EditorObjectSelectionSession.h"
#include "../Game.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../PlatformEnvironment.h"
#include "../ReplayManager.h"
#include "../Version.h"
#include "../actions/CheatSetAction.h"
#include "../actions/ClimateSetAction.h"
#include "../actions/GameSetSpeedAction.h"
#include "../actions/ParkSetDateAction.h"
#include "../actions/ParkSetParameterAction.h"
#include "../actions/RideFreezeRatingAction.h"
#include "../actions/RideSetPriceAction.h"
#include "../actions/RideSetSettingAction.h"
#include "../actions/ScenarioSetSettingAction.h"
#include "../actions/StaffSetCostumeAction.h"
#include "../config/Config.h"
#include "../core/Console.hpp"
#include "../core/EnumUtils.hpp"
#include "../core/Guard.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../drawing/Font.h"
#include "../drawing/Image.h"
#include "../entity/Balloon.h"
#include "../entity/EntityList.h"
#include "../entity/EntityRegistry.h"
#include "../entity/Staff.h"
#include "../interface/Chat.h"
#include "../interface/Colour.h"
#include "../interface/Viewport.h"
#include "../interface/Window_internal.h"
#include "../localisation/Formatting.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../management/NewsItem.h"
#include "../management/Research.h"
#include "../network/network.h"
#include "../object/Object.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../object/PeepAnimationsObject.h"
#include "../platform/Platform.h"
#include "../profiling/Profiling.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/RideManager.hpp"
#include "../ride/Vehicle.h"
#include "../ui/WindowManager.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Climate.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "Viewport.h"

#include <array>
#include <cmath>
#include <cstdarg>
#include <cstdlib>
#include <deque>
#include <exception>
#include <string>
#include <thread>
#include <vector>

#ifndef NO_TTF
    #include "../drawing/TTF.h"
#endif

using namespace OpenRCT2;

using arguments_t = std::vector<std::string>;
using OpenRCT2::Date;

static constexpr const char* kClimateNames[] = {
    "cool_and_wet",
    "warm",
    "hot_and_dry",
    "cold",
};

static int32_t ConsoleParseInt(const std::string& src, bool* valid);
static double ConsoleParseDouble(const std::string& src, bool* valid);

static void ConsoleWriteAllCommands(InteractiveConsole& console);
static void ConsoleCommandVariables(InteractiveConsole& console, const arguments_t& argv);
static void ConsoleCommandWindows(InteractiveConsole& console, const arguments_t& argv);
static void ConsoleCommandHelp(InteractiveConsole& console, const arguments_t& argv);

static bool InvalidArguments(bool* invalid, bool arguments);

#define SET_FLAG(variable, flag, value)                                                                                        \
    {                                                                                                                          \
        if (value)                                                                                                             \
            variable |= flag;                                                                                                  \
        else                                                                                                                   \
            variable &= ~(flag);                                                                                               \
    }

static int32_t ConsoleParseInt(const std::string& src, bool* valid)
{
    utf8* end;
    int32_t value;
    value = static_cast<int32_t>(strtol(src.c_str(), &end, 10));
    *valid = (*end == '\0');
    return value;
}

static double ConsoleParseDouble(const std::string& src, bool* valid)
{
    utf8* end;
    double value;
    value = strtod(src.c_str(), &end);
    *valid = (*end == '\0');
    return value;
}

static void ConsoleCommandClear(InteractiveConsole& console, [[maybe_unused]] const arguments_t& argv)
{
    console.Clear();
}

static void ConsoleCommandClose(InteractiveConsole& console, [[maybe_unused]] const arguments_t& argv)
{
    console.Close();
}

static void ConsoleCommandHide(InteractiveConsole& console, [[maybe_unused]] const arguments_t& argv)
{
    console.Hide();
}

static void ConsoleCommandEcho(InteractiveConsole& console, const arguments_t& argv)
{
    if (!argv.empty())
        console.WriteLine(argv[0]);
}

static void ConsoleCommandRides(InteractiveConsole& console, const arguments_t& argv)
{
    if (!argv.empty())
    {
        if (argv[0] == "list")
        {
            for (const auto& ride : GetRideManager())
            {
                auto name = ride.GetName();
                console.WriteFormatLine(
                    "ride: %03d type: %02u subtype %03u operating mode: %02u name: %s", ride.id, ride.type, ride.subtype,
                    ride.mode, name.c_str());
            }
        }
        else if (argv[0] == "set")
        {
            if (argv.size() < 4)
            {
                if (argv.size() > 1 && argv[1] == "mode")
                {
                    console.WriteFormatLine("Ride modes are specified using integer IDs as given below:");
                    for (int32_t i = 0; i < static_cast<uint8_t>(RideMode::Count); i++)
                    {
                        char mode_name[128] = { 0 };
                        StringId mode_string_id = kRideModeNames[i];
                        OpenRCT2::FormatStringLegacy(mode_name, 128, mode_string_id, nullptr);
                        console.WriteFormatLine("%02d - %s", i, mode_name);
                    }
                }
                else
                {
                    console.WriteFormatLine("rides set type <ride id> <ride type>");
                    console.WriteFormatLine("rides set mode [<ride id> <operating mode>]");
                    console.WriteFormatLine("rides set mass <ride id> <mass value>");
                    console.WriteFormatLine("rides set excitement <ride id> <excitement value>");
                    console.WriteFormatLine("rides set intensity <ride id> <intensity value>");
                    console.WriteFormatLine("rides set nausea <ride id> <nausea value>");
                    console.WriteFormatLine("rides set price <ride id / all [type]> <price>");
                }
                return;
            }
            if (argv[1] == "type")
            {
                bool int_valid[2] = { false };
                int32_t ride_index = ConsoleParseInt(argv[2], &int_valid[0]);
                int32_t type = ConsoleParseInt(argv[3], &int_valid[1]);
                if (!int_valid[0] || !int_valid[1])
                {
                    console.WriteFormatLine("This command expects integer arguments");
                }
                else if (ride_index < 0)
                {
                    console.WriteFormatLine("Ride index must not be negative");
                }
                else
                {
                    auto res = SetOperatingSetting(RideId::FromUnderlying(ride_index), RideSetSetting::RideType, type);
                    if (res == kMoney64Undefined)
                    {
                        if (!GetGameState().Cheats.allowArbitraryRideTypeChanges)
                        {
                            console.WriteFormatLine(
                                "That didn't work. Try enabling the 'Allow arbitrary ride type changes' cheat");
                        }
                        else
                        {
                            console.WriteFormatLine("That didn't work");
                        }
                    }
                }
            }
            else if (argv[1] == "mode")
            {
                bool int_valid[2] = { false };
                int32_t ride_index = ConsoleParseInt(argv[2], &int_valid[0]);
                int32_t mode = ConsoleParseInt(argv[3], &int_valid[1]);
                if (!int_valid[0] || !int_valid[1])
                {
                    console.WriteFormatLine("This command expects integer arguments");
                }
                else if (ride_index < 0)
                {
                    console.WriteFormatLine("Ride index must not be negative");
                }
                else
                {
                    auto ride = GetRide(RideId::FromUnderlying(ride_index));
                    if (mode >= static_cast<uint8_t>(RideMode::Count))
                    {
                        console.WriteFormatLine("Invalid ride mode.");
                    }
                    else if (ride == nullptr)
                    {
                        console.WriteFormatLine("No ride found with index %d", ride_index);
                    }
                    else
                    {
                        ride->mode = static_cast<RideMode>(mode & 0xFF);
                        InvalidateTestResults(*ride);
                    }
                }
            }
            else if (argv[1] == "mass")
            {
                bool int_valid[2] = { false };
                int32_t ride_index = ConsoleParseInt(argv[2], &int_valid[0]);
                int32_t mass = ConsoleParseInt(argv[3], &int_valid[1]);

                if (ride_index < 0)
                {
                    console.WriteFormatLine("Ride index must not be negative");
                }
                else if (!int_valid[0] || !int_valid[1])
                {
                    console.WriteFormatLine("This command expects integer arguments");
                }
                else
                {
                    auto ride = GetRide(RideId::FromUnderlying(ride_index));
                    if (mass <= 0)
                    {
                        console.WriteFormatLine("Friction value must be strictly positive");
                    }
                    else if (ride == nullptr)
                    {
                        console.WriteFormatLine("No ride found with index %d", ride_index);
                    }
                    else
                    {
                        for (int32_t i = 0; i < ride->NumTrains; ++i)
                        {
                            for (Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[i]); vehicle != nullptr;
                                 vehicle = GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
                            {
                                vehicle->mass = mass;
                            }
                        }
                    }
                }
            }
            else if (argv[1] == "excitement")
            {
                bool int_valid[2] = { false };
                int32_t ride_index = ConsoleParseInt(argv[2], &int_valid[0]);
                ride_rating excitement = ConsoleParseInt(argv[3], &int_valid[1]);

                if (ride_index < 0)
                {
                    console.WriteFormatLine("Ride index must not be negative");
                }
                else if (!int_valid[0] || !int_valid[1])
                {
                    console.WriteFormatLine("This command expects integer arguments");
                }
                else
                {
                    auto rideIndex = RideId::FromUnderlying(ride_index);
                    auto ride = GetRide(rideIndex);
                    if (excitement <= 0)
                    {
                        console.WriteFormatLine("Excitement value must be strictly positive");
                    }
                    else if (ride == nullptr)
                    {
                        console.WriteFormatLine("No ride found with index %d", ride_index);
                    }
                    else
                    {
                        auto rideAction = RideFreezeRatingAction(rideIndex, RideRatingType::Excitement, excitement);
                        GameActions::Execute(&rideAction);
                    }
                }
            }
            else if (argv[1] == "intensity")
            {
                bool int_valid[2] = { false };
                int32_t ride_index = ConsoleParseInt(argv[2], &int_valid[0]);
                ride_rating intensity = ConsoleParseInt(argv[3], &int_valid[1]);

                if (ride_index < 0)
                {
                    console.WriteFormatLine("Ride index must not be negative");
                }
                else if (!int_valid[0] || !int_valid[1])
                {
                    console.WriteFormatLine("This command expects integer arguments");
                }
                else
                {
                    auto rideIndex = RideId::FromUnderlying(ride_index);
                    auto ride = GetRide(rideIndex);
                    if (intensity <= 0)
                    {
                        console.WriteFormatLine("Intensity value must be strictly positive");
                    }
                    else if (ride == nullptr)
                    {
                        console.WriteFormatLine("No ride found with index %d", ride_index);
                    }
                    else
                    {
                        auto rideAction = RideFreezeRatingAction(rideIndex, RideRatingType::Intensity, intensity);
                        GameActions::Execute(&rideAction);
                    }
                }
            }
            else if (argv[1] == "nausea")
            {
                bool int_valid[2] = { false };
                int32_t ride_index = ConsoleParseInt(argv[2], &int_valid[0]);
                ride_rating nausea = ConsoleParseInt(argv[3], &int_valid[1]);

                if (ride_index < 0)
                {
                    console.WriteFormatLine("Ride index must not be negative");
                }
                else if (!int_valid[0] || !int_valid[1])
                {
                    console.WriteFormatLine("This command expects integer arguments");
                }
                else
                {
                    auto rideIndex = RideId::FromUnderlying(ride_index);
                    auto ride = GetRide(rideIndex);
                    if (nausea <= 0)
                    {
                        console.WriteFormatLine("Nausea value must be strictly positive");
                    }
                    else if (ride == nullptr)
                    {
                        console.WriteFormatLine("No ride found with index %d", ride_index);
                    }
                    else
                    {
                        auto rideAction = RideFreezeRatingAction(rideIndex, RideRatingType::Nausea, nausea);
                        GameActions::Execute(&rideAction);
                    }
                }
            }
            else if (argv[1] == "price")
            {
                bool int_valid[2] = { false };
                if (argv[2] == "all")
                {
                    auto arg1 = ConsoleParseInt(argv[3], &int_valid[0]);
                    if (argv.size() <= 4)
                    {
                        auto price = arg1;
                        if (int_valid[0])
                        {
                            for (const auto& ride : GetRideManager())
                            {
                                auto rideSetPrice = RideSetPriceAction(ride.id, price, true);
                                GameActions::Execute(&rideSetPrice);
                            }
                        }
                        else
                        {
                            console.WriteFormatLine("This command expects one or two integer arguments");
                        }
                    }
                    else
                    {
                        auto rideType = arg1;
                        auto price = ConsoleParseInt(argv[4], &int_valid[1]);

                        if (int_valid[0] && int_valid[1])
                        {
                            for (const auto& ride : GetRideManager())
                            {
                                if (ride.type == rideType)
                                {
                                    auto rideSetPrice = RideSetPriceAction(ride.id, price, true);
                                    GameActions::Execute(&rideSetPrice);
                                }
                            }
                        }
                        else
                        {
                            console.WriteFormatLine("This command expects one or two integer arguments");
                        }
                    }
                }
                else
                {
                    int32_t rideId = ConsoleParseInt(argv[2], &int_valid[0]);
                    money64 price = ConsoleParseInt(argv[3], &int_valid[1]);

                    if (!int_valid[0] || !int_valid[1])
                    {
                        console.WriteFormatLine("This command expects the string all or two integer arguments");
                    }
                    else
                    {
                        auto rideSetPrice = RideSetPriceAction(RideId::FromUnderlying(rideId), price, true);
                        GameActions::Execute(&rideSetPrice);
                    }
                }
            }
        }
    }
    else
    {
        console.WriteFormatLine("subcommands: list, set");
    }
}

static void ConsoleCommandStaff(InteractiveConsole& console, const arguments_t& argv)
{
    if (!argv.empty())
    {
        if (argv[0] == "list")
        {
            for (auto peep : EntityList<Staff>())
            {
                auto name = peep->GetName();
                console.WriteFormatLine(
                    "staff id %03d type: %02u energy %03u name %s", peep->Id, peep->AssignedStaffType, peep->Energy,
                    name.c_str());
            }
        }
        else if (argv[0] == "set")
        {
            if (argv.size() < 4)
            {
                console.WriteFormatLine("staff set energy <staff id> <value 0-255>");
                console.WriteFormatLine("staff set costume <staff id> <costume id>");

                auto _availableCostumeIndexes = findAllPeepAnimationsIndexesForType(AnimationPeepType::Entertainer);
                auto _availableCostumeObjects = findAllPeepAnimationsObjectForType(AnimationPeepType::Entertainer);

                for (auto i = 0u; i < _availableCostumeIndexes.size(); i++)
                {
                    auto index = _availableCostumeIndexes[i];
                    auto name = _availableCostumeObjects[i]->GetCostumeName();
                    console.WriteFormatLine("        costume %i: %s", index, name.c_str());
                }
                return;
            }
            if (argv[1] == "energy")
            {
                int32_t int_val[3];
                bool int_valid[3] = { false };
                int_val[0] = ConsoleParseInt(argv[2], &int_valid[0]);
                int_val[1] = ConsoleParseInt(argv[3], &int_valid[1]);

                if (int_valid[0] && int_valid[1])
                {
                    Peep* peep = GetEntity<Peep>(EntityId::FromUnderlying(int_val[0]));
                    if (peep != nullptr)
                    {
                        peep->Energy = int_val[1];
                        peep->EnergyTarget = int_val[1];
                    }
                }
            }
            else if (argv[1] == "costume")
            {
                int32_t int_val[2];
                bool int_valid[2] = { false };
                int_val[0] = ConsoleParseInt(argv[2], &int_valid[0]);
                int_val[1] = ConsoleParseInt(argv[3], &int_valid[1]);
                if (!int_valid[0])
                {
                    console.WriteLineError("Invalid staff ID");
                    return;
                }
                auto staff = GetEntity<Staff>(EntityId::FromUnderlying(int_val[0]));
                if (staff == nullptr)
                {
                    console.WriteLineError("Invalid staff ID");
                    return;
                }
                if (staff->AssignedStaffType != StaffType::Entertainer)
                {
                    console.WriteLineError("Specified staff is not entertainer");
                    return;
                }
                auto& objManager = GetContext()->GetObjectManager();
                if (!int_valid[1] || int_val[1] < 0 || objManager.GetLoadedObject<PeepAnimationsObject>(int_val[1]) == nullptr)
                {
                    console.WriteLineError("Invalid costume ID");
                    return;
                }

                auto costume = static_cast<ObjectEntryIndex>(int_val[1]);
                auto staffSetCostumeAction = StaffSetCostumeAction(EntityId::FromUnderlying(int_val[0]), costume);
                GameActions::Execute(&staffSetCostumeAction);
            }
        }
    }
    else
    {
        console.WriteFormatLine("subcommands: list, set");
    }
}

static void ConsoleCommandGet(InteractiveConsole& console, const arguments_t& argv)
{
    auto& gameState = GetGameState();

    if (!argv.empty())
    {
        if (argv[0] == "park_rating")
        {
            console.WriteFormatLine("park_rating %d", gameState.Park.Rating);
        }
        else if (argv[0] == "park_value")
        {
            console.WriteLine(FormatString("park_value {CURRENCY2DP}", gameState.Park.Value));
        }
        else if (argv[0] == "company_value")
        {
            console.WriteLine(FormatString("company_value {CURRENCY2DP}", gameState.CompanyValue));
        }
        else if (argv[0] == "money")
        {
            console.WriteLine(FormatString("money {CURRENCY2DP}", gameState.Cash));
        }
        else if (argv[0] == "scenario_initial_cash")
        {
            console.WriteLine(FormatString("scenario_initial_cash {CURRENCY2DP}", gameState.InitialCash));
        }
        else if (argv[0] == "current_loan")
        {
            console.WriteLine(FormatString("current_loan {CURRENCY2DP}", gameState.BankLoan));
        }
        else if (argv[0] == "max_loan")
        {
            console.WriteLine(FormatString("max_loan {CURRENCY2DP}", gameState.MaxBankLoan));
        }
        else if (argv[0] == "guest_initial_cash")
        {
            console.WriteLine(FormatString("guest_initial_cash {CURRENCY2DP}", gameState.GuestInitialCash));
        }
        else if (argv[0] == "land_rights_cost")
        {
            console.WriteLine(FormatString("land_rights_cost {CURRENCY2DP}", gameState.LandPrice));
        }
        else if (argv[0] == "construction_rights_cost")
        {
            console.WriteLine(FormatString("construction_rights_cost {CURRENCY2DP}", gameState.ConstructionRightsPrice));
        }
        else if (argv[0] == "guest_initial_happiness")
        {
            uint32_t current_happiness = gameState.GuestInitialHappiness;
            for (int32_t i = 15; i <= 99; i++)
            {
                if (i == 99)
                {
                    console.WriteFormatLine("guest_initial_happiness %d%%  (%d)", 15, gameState.GuestInitialHappiness);
                }
                else if (current_happiness == Park::CalculateGuestInitialHappiness(i))
                {
                    console.WriteFormatLine("guest_initial_happiness %d%%  (%d)", i, gameState.GuestInitialHappiness);
                    break;
                }
            }
        }
        else if (argv[0] == "guest_initial_hunger")
        {
            console.WriteFormatLine(
                "guest_initial_hunger %d%%  (%d)", ((255 - gameState.GuestInitialHunger) * 100) / 255,
                gameState.GuestInitialHunger);
        }
        else if (argv[0] == "guest_initial_thirst")
        {
            console.WriteFormatLine(
                "guest_initial_thirst %d%%  (%d)", ((255 - gameState.GuestInitialThirst) * 100) / 255,
                gameState.GuestInitialThirst);
        }
        else if (argv[0] == "guest_prefer_less_intense_rides")
        {
            console.WriteFormatLine(
                "guest_prefer_less_intense_rides %d", (gameState.Park.Flags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES) != 0);
        }
        else if (argv[0] == "guest_prefer_more_intense_rides")
        {
            console.WriteFormatLine(
                "guest_prefer_more_intense_rides %d", (gameState.Park.Flags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES) != 0);
        }
        else if (argv[0] == "forbid_marketing_campaigns")
        {
            console.WriteFormatLine(
                "forbid_marketing_campaigns %d", (gameState.Park.Flags & PARK_FLAGS_FORBID_MARKETING_CAMPAIGN) != 0);
        }
        else if (argv[0] == "forbid_landscape_changes")
        {
            console.WriteFormatLine(
                "forbid_landscape_changes %d", (gameState.Park.Flags & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES) != 0);
        }
        else if (argv[0] == "forbid_tree_removal")
        {
            console.WriteFormatLine("forbid_tree_removal %d", (gameState.Park.Flags & PARK_FLAGS_FORBID_TREE_REMOVAL) != 0);
        }
        else if (argv[0] == "forbid_high_construction")
        {
            console.WriteFormatLine(
                "forbid_high_construction %d", (gameState.Park.Flags & PARK_FLAGS_FORBID_HIGH_CONSTRUCTION) != 0);
        }
        else if (argv[0] == "pay_for_rides")
        {
            console.WriteFormatLine("pay_for_rides %d", (gameState.Park.Flags & PARK_FLAGS_PARK_FREE_ENTRY) != 0);
        }
        else if (argv[0] == "no_money")
        {
            console.WriteFormatLine("no_money %d", (gameState.Park.Flags & PARK_FLAGS_NO_MONEY) != 0);
        }
        else if (argv[0] == "difficult_park_rating")
        {
            console.WriteFormatLine("difficult_park_rating %d", (gameState.Park.Flags & PARK_FLAGS_DIFFICULT_PARK_RATING) != 0);
        }
        else if (argv[0] == "difficult_guest_generation")
        {
            console.WriteFormatLine(
                "difficult_guest_generation %d", (gameState.Park.Flags & PARK_FLAGS_DIFFICULT_GUEST_GENERATION) != 0);
        }
        else if (argv[0] == "park_open")
        {
            console.WriteFormatLine("park_open %d", (gameState.Park.Flags & PARK_FLAGS_PARK_OPEN) != 0);
        }
        else if (argv[0] == "climate")
        {
            console.WriteFormatLine(
                "climate %s  (%d)", kClimateNames[EnumValue(gameState.Climate)], EnumValue(gameState.Climate));
        }
        else if (argv[0] == "game_speed")
        {
            console.WriteFormatLine("game_speed %d", gGameSpeed);
        }
        else if (argv[0] == "console_small_font")
        {
            console.WriteFormatLine("console_small_font %d", Config::Get().interface.ConsoleSmallFont);
        }
        else if (argv[0] == "location")
        {
            WindowBase* w = WindowGetMain();
            if (w != nullptr)
            {
                Viewport* viewport = WindowGetViewport(w);
                auto info = GetMapCoordinatesFromPosWindow(
                    w, { viewport->width / 2, viewport->height / 2 }, EnumsToFlags(ViewportInteractionItem::Terrain));

                auto tileMapCoord = TileCoordsXY(info.Loc);
                console.WriteFormatLine("location %d %d", tileMapCoord.x, tileMapCoord.y);
            }
        }
        else if (argv[0] == "window_scale")
        {
            console.WriteFormatLine("window_scale %.3f", Config::Get().general.WindowScale);
        }
        else if (argv[0] == "window_limit")
        {
            console.WriteFormatLine("window_limit %d", Config::Get().general.WindowLimit);
        }
        else if (argv[0] == "render_weather_effects")
        {
            console.WriteFormatLine("render_weather_effects %d", Config::Get().general.RenderWeatherEffects);
        }
        else if (argv[0] == "render_weather_gloom")
        {
            console.WriteFormatLine("render_weather_gloom %d", Config::Get().general.RenderWeatherGloom);
        }
        else if (argv[0] == "cheat_sandbox_mode")
        {
            console.WriteFormatLine("cheat_sandbox_mode %d", GetGameState().Cheats.sandboxMode);
        }
        else if (argv[0] == "cheat_disable_clearance_checks")
        {
            console.WriteFormatLine("cheat_disable_clearance_checks %d", GetGameState().Cheats.disableClearanceChecks);
        }
        else if (argv[0] == "cheat_disable_support_limits")
        {
            console.WriteFormatLine("cheat_disable_support_limits %d", GetGameState().Cheats.disableSupportLimits);
        }
        else if (argv[0] == "current_rotation")
        {
            console.WriteFormatLine("current_rotation %d", GetCurrentRotation());
        }
        else if (argv[0] == "host_timescale")
        {
            console.WriteFormatLine("host_timescale %.02f", OpenRCT2::GetContext()->GetTimeScale());
        }
#ifndef NO_TTF
        else if (argv[0] == "enable_hinting")
        {
            console.WriteFormatLine("enable_hinting %d", Config::Get().fonts.EnableHinting);
        }
#endif
        else
        {
            console.WriteLineWarning("Invalid variable.");
        }
    }
}

template<typename TAction, typename... TArgs>
static void ConsoleSetVariableAction(InteractiveConsole& console, std::string var, TArgs&&... args)
{
    auto action = TAction(std::forward<TArgs>(args)...);
    action.SetCallback([&console, var](const GameAction*, const GameActions::Result* res) {
        if (res->Error != GameActions::Status::Ok)
            console.WriteLineError(String::stdFormat("set %s command failed, likely due to permissions.", var.c_str()));
        else
            console.Execute(String::stdFormat("get %s", var.c_str()));
        console.EndAsyncExecution();
    });
    console.BeginAsyncExecution();
    GameActions::Execute(&action);
}

static void ConsoleCommandSet(InteractiveConsole& console, const arguments_t& argv)
{
    if (argv.size() > 1)
    {
        int32_t int_val[4];
        bool int_valid[4];
        double double_val[4];
        bool double_valid[4];
        bool invalidArgs = false;

        for (uint32_t i = 0; i < std::size(int_val); i++)
        {
            if (i + 1 < argv.size())
            {
                int_val[i] = ConsoleParseInt(argv[i + 1], &int_valid[i]);
                double_val[i] = ConsoleParseDouble(argv[i + 1], &double_valid[i]);
            }
            else
            {
                int_val[i] = 0;
                int_valid[i] = false;
                double_val[i] = 0;
                double_valid[i] = false;
            }
        }

        std::string varName = argv[0];

        auto& gameState = GetGameState();
        if (varName == "money" && InvalidArguments(&invalidArgs, double_valid[0]))
        {
            money64 money = ToMoney64FromGBP(double_val[0]);
            if (gameState.Cash != money)
            {
                ConsoleSetVariableAction<CheatSetAction>(console, varName, CheatType::SetMoney, money);
            }
            else
            {
                console.Execute("get money");
            }
        }
        else if (varName == "scenario_initial_cash" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            ConsoleSetVariableAction<ScenarioSetSettingAction>(
                console, varName, ScenarioSetSetting::InitialCash,
                std::clamp(ToMoney64FromGBP(int_val[0]), 0.00_GBP, 1000000.00_GBP));
        }
        else if (varName == "current_loan" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            auto amount = std::clamp(
                ToMoney64FromGBP(int_val[0]) - ToMoney64FromGBP(int_val[0] % 1000), 0.00_GBP, gameState.MaxBankLoan);
            ConsoleSetVariableAction<ScenarioSetSettingAction>(console, varName, ScenarioSetSetting::InitialLoan, amount);
        }
        else if (varName == "max_loan" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            auto amount = std::clamp(
                ToMoney64FromGBP(int_val[0]) - ToMoney64FromGBP(int_val[0] % 1000), 0.00_GBP, 5000000.00_GBP);
            ConsoleSetVariableAction<ScenarioSetSettingAction>(console, varName, ScenarioSetSetting::MaximumLoanSize, amount);
        }
        else if (varName == "guest_initial_cash" && InvalidArguments(&invalidArgs, double_valid[0]))
        {
            ConsoleSetVariableAction<ScenarioSetSettingAction>(
                console, varName, ScenarioSetSetting::AverageCashPerGuest,
                std::clamp(ToMoney64FromGBP(double_val[0]), 0.00_GBP, 1000.00_GBP));
        }
        else if (varName == "guest_initial_happiness" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            ConsoleSetVariableAction<ScenarioSetSettingAction>(
                console, varName, ScenarioSetSetting::GuestInitialHappiness,
                Park::CalculateGuestInitialHappiness(static_cast<uint8_t>(int_val[0])));
        }
        else if (varName == "guest_initial_hunger" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            ConsoleSetVariableAction<ScenarioSetSettingAction>(
                console, varName, ScenarioSetSetting::GuestInitialHunger,
                (std::clamp(int_val[0], 1, 84) * 255 / 100 - 255) * -1);
        }
        else if (varName == "guest_initial_thirst" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            ConsoleSetVariableAction<ScenarioSetSettingAction>(
                console, varName, ScenarioSetSetting::GuestInitialThirst,
                (std::clamp(int_val[0], 1, 84) * 255 / 100 - 255) * -1);
        }
        else if (varName == "guest_prefer_less_intense_rides" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            ConsoleSetVariableAction<ScenarioSetSettingAction>(
                console, varName, ScenarioSetSetting::GuestsPreferLessIntenseRides, int_val[0]);
        }
        else if (varName == "guest_prefer_more_intense_rides" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            ConsoleSetVariableAction<ScenarioSetSettingAction>(
                console, varName, ScenarioSetSetting::GuestsPreferMoreIntenseRides, int_val[0]);
        }
        else if (varName == "forbid_marketing_campaigns" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            ConsoleSetVariableAction<ScenarioSetSettingAction>(
                console, varName, ScenarioSetSetting::ForbidMarketingCampaigns, int_val[0]);
        }
        else if (varName == "forbid_landscape_changes" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            ConsoleSetVariableAction<ScenarioSetSettingAction>(
                console, varName, ScenarioSetSetting::ForbidLandscapeChanges, int_val[0]);
        }
        else if (varName == "forbid_tree_removal" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            ConsoleSetVariableAction<ScenarioSetSettingAction>(
                console, varName, ScenarioSetSetting::ForbidTreeRemoval, int_val[0]);
        }
        else if (varName == "forbid_high_construction" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            ConsoleSetVariableAction<ScenarioSetSettingAction>(
                console, varName, ScenarioSetSetting::ForbidHighConstruction, int_val[0]);
        }
        else if (varName == "pay_for_rides" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            SET_FLAG(gameState.Park.Flags, PARK_FLAGS_PARK_FREE_ENTRY, int_val[0]);
            console.Execute("get pay_for_rides");
        }
        else if (varName == "no_money" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            ConsoleSetVariableAction<CheatSetAction>(console, varName, CheatType::NoMoney, int_val[0] != 0);
        }
        else if (varName == "difficult_park_rating" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            ConsoleSetVariableAction<ScenarioSetSettingAction>(
                console, varName, ScenarioSetSetting::ParkRatingHigherDifficultyLevel, int_val[0]);
        }
        else if (varName == "difficult_guest_generation" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            ConsoleSetVariableAction<ScenarioSetSettingAction>(
                console, varName, ScenarioSetSetting::GuestGenerationHigherDifficultyLevel, int_val[0]);
        }
        else if (varName == "park_open" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            ConsoleSetVariableAction<ParkSetParameterAction>(
                console, varName, (int_val[0] == 1) ? ParkParameter::Open : ParkParameter::Close);
        }
        else if (varName == "land_rights_cost" && InvalidArguments(&invalidArgs, double_valid[0]))
        {
            ConsoleSetVariableAction<ScenarioSetSettingAction>(
                console, varName, ScenarioSetSetting::CostToBuyLand,
                std::clamp(ToMoney64FromGBP(double_val[0]), 0.00_GBP, 200.00_GBP));
        }
        else if (varName == "construction_rights_cost" && InvalidArguments(&invalidArgs, double_valid[0]))
        {
            ConsoleSetVariableAction<ScenarioSetSettingAction>(
                console, varName, ScenarioSetSetting::CostToBuyConstructionRights,
                std::clamp(ToMoney64FromGBP(double_val[0]), 0.00_GBP, 200.00_GBP));
        }
        else if (varName == "climate")
        {
            uint8_t newClimate = static_cast<uint8_t>(ClimateType::Count);
            invalidArgs = true;

            if (int_valid[0])
            {
                newClimate = static_cast<uint8_t>(int_val[0]);
                invalidArgs = false;
            }
            else
            {
                for (newClimate = 0; newClimate < static_cast<uint8_t>(ClimateType::Count); newClimate++)
                {
                    if (argv[1] == kClimateNames[newClimate])
                    {
                        invalidArgs = false;
                        break;
                    }
                }
            }

            if (invalidArgs)
            {
                console.WriteLine(LanguageGetString(STR_INVALID_CLIMATE_ID));
            }
            else
            {
                ConsoleSetVariableAction<ClimateSetAction>(console, varName, ClimateType{ newClimate });
            }
        }
        else if (varName == "game_speed" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            ConsoleSetVariableAction<GameSetSpeedAction>(console, varName, std::clamp(int_val[0], 1, 8));
        }
        else if (varName == "console_small_font" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            Config::Get().interface.ConsoleSmallFont = (int_val[0] != 0);
            Config::Save();
            console.Execute("get console_small_font");
        }
        else if (varName == "location" && InvalidArguments(&invalidArgs, int_valid[0] && int_valid[1]))
        {
            WindowBase* w = WindowGetMain();
            if (w != nullptr)
            {
                auto location = TileCoordsXYZ(int_val[0], int_val[1], 0).ToCoordsXYZ().ToTileCentre();
                location.z = TileElementHeight(location);
                w->SetLocation(location);
                ViewportUpdatePosition(w);
                console.Execute("get location");
            }
        }
        else if (varName == "window_scale" && InvalidArguments(&invalidArgs, double_valid[0]))
        {
            float newScale = static_cast<float>(0.001 * std::trunc(1000 * double_val[0]));
            Config::Get().general.WindowScale = std::clamp(newScale, 0.5f, 5.0f);
            Config::Save();
            GfxInvalidateScreen();
            ContextTriggerResize();
            ContextUpdateCursorScale();
            console.Execute("get window_scale");
        }
        else if (varName == "window_limit" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            WindowSetWindowLimit(int_val[0]);
            console.Execute("get window_limit");
        }
        else if (varName == "render_weather_effects" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            Config::Get().general.RenderWeatherEffects = (int_val[0] != 0);
            Config::Save();
            console.Execute("get render_weather_effects");
        }
        else if (varName == "render_weather_gloom" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            Config::Get().general.RenderWeatherGloom = (int_val[0] != 0);
            Config::Save();
            console.Execute("get render_weather_gloom");
        }
        else if (varName == "cheat_sandbox_mode" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            if (GetGameState().Cheats.sandboxMode != (int_val[0] != 0))
            {
                ConsoleSetVariableAction<CheatSetAction>(console, varName, CheatType::SandboxMode, int_val[0] != 0);
            }
            else
            {
                console.Execute("get cheat_sandbox_mode");
            }
        }
        else if (varName == "cheat_disable_clearance_checks" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            if (GetGameState().Cheats.disableClearanceChecks != (int_val[0] != 0))
            {
                ConsoleSetVariableAction<CheatSetAction>(console, varName, CheatType::DisableClearanceChecks, int_val[0] != 0);
            }
            else
            {
                console.Execute("get cheat_disable_clearance_checks");
            }
        }
        else if (varName == "cheat_disable_support_limits" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            if (GetGameState().Cheats.disableSupportLimits != (int_val[0] != 0))
            {
                ConsoleSetVariableAction<CheatSetAction>(console, varName, CheatType::DisableSupportLimits, int_val[0] != 0);
            }
            else
            {
                console.Execute("get cheat_disable_support_limits");
            }
        }
        else if (varName == "current_rotation" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            uint8_t currentRotation = GetCurrentRotation();
            int32_t newRotation = int_val[0];
            if (newRotation < 0 || newRotation > 3)
            {
                console.WriteLineError("Invalid argument. Valid rotations are 0-3.");
            }
            else if (newRotation != currentRotation)
            {
                ViewportRotateAll(newRotation - currentRotation);
            }
            console.Execute("get current_rotation");
        }
        else if (varName == "host_timescale" && InvalidArguments(&invalidArgs, double_valid[0]))
        {
            float newScale = static_cast<float>(double_val[0]);

            OpenRCT2::GetContext()->SetTimeScale(newScale);

            console.Execute("get host_timescale");
        }
#ifndef NO_TTF
        else if (varName == "enable_hinting" && InvalidArguments(&invalidArgs, int_valid[0]))
        {
            Config::Get().fonts.EnableHinting = (int_val[0] != 0);
            Config::Save();
            console.Execute("get enable_hinting");
            TTFToggleHinting();
        }
#endif
        else if (invalidArgs)
        {
            console.WriteLineError("Invalid arguments.");
        }
        else
        {
            console.WriteLineError("Invalid variable.");
        }

        GfxInvalidateScreen();
    }
    else
    {
        console.WriteLineError("Value required.");
    }
}

static void ConsoleCommandLoadObject(InteractiveConsole& console, const arguments_t& argv)
{
    if (!argv.empty())
    {
        char name[9] = { 0 };
        std::fill_n(name, 8, ' ');
        std::size_t i = 0;
        for (const char* ch = argv[0].c_str(); *ch != '\0' && i < std::size(name) - 1; ch++)
        {
            name[i++] = *ch;
        }

        const ObjectRepositoryItem* ori = ObjectRepositoryFindObjectByName(name);
        if (ori == nullptr)
        {
            console.WriteLineError("Could not find the object.");
            return;
        }

        const RCTObjectEntry* entry = &ori->ObjectEntry;
        const auto* loadedObject = ObjectManagerGetLoadedObject(ObjectEntryDescriptor(*ori));
        if (loadedObject != nullptr)
        {
            console.WriteLineError("Object is already in scenario.");
            return;
        }

        loadedObject = ObjectManagerLoadObject(entry);
        if (loadedObject == nullptr)
        {
            console.WriteLineError("Unable to load object.");
            return;
        }
        auto groupIndex = ObjectManagerGetLoadedObjectEntryIndex(loadedObject);

        ObjectType objectType = entry->GetType();
        if (objectType == ObjectType::Ride)
        {
            // Automatically research the ride so it's supported by the game.
            const auto* rideEntry = GetRideEntryByIndex(groupIndex);

            for (int32_t j = 0; j < RCT2::ObjectLimits::kMaxRideTypesPerRideEntry; j++)
            {
                auto rideType = rideEntry->ride_type[j];
                if (rideType != kRideTypeNull)
                {
                    ResearchCategory category = GetRideTypeDescriptor(rideType).GetResearchCategory();
                    ResearchInsertRideEntry(rideType, groupIndex, category, true);
                }
            }

            gSilentResearch = true;
            ResearchResetCurrentItem();
            gSilentResearch = false;
        }
        else if (objectType == ObjectType::SceneryGroup)
        {
            ResearchInsertSceneryGroupEntry(groupIndex, true);

            gSilentResearch = true;
            ResearchResetCurrentItem();
            gSilentResearch = false;
        }

        auto sceneryIntent = Intent(INTENT_ACTION_SET_DEFAULT_SCENERY_CONFIG);
        ContextBroadcastIntent(&sceneryIntent);

        auto ridesIntent = Intent(INTENT_ACTION_REFRESH_NEW_RIDES);
        ContextBroadcastIntent(&ridesIntent);

        gWindowUpdateTicks = 0;
        GfxInvalidateScreen();
        console.WriteLine("Object file loaded.");
    }
}

constexpr auto _objectTypeNames = std::to_array<StringId>({
    STR_OBJECT_SELECTION_RIDE_VEHICLES_ATTRACTIONS,
    STR_OBJECT_SELECTION_SMALL_SCENERY,
    STR_OBJECT_SELECTION_LARGE_SCENERY,
    STR_OBJECT_SELECTION_WALLS_FENCES,
    STR_OBJECT_SELECTION_PATH_SIGNS,
    STR_OBJECT_SELECTION_FOOTPATHS,
    STR_OBJECT_SELECTION_PATH_EXTRAS,
    STR_OBJECT_SELECTION_SCENERY_GROUPS,
    STR_OBJECT_SELECTION_PARK_ENTRANCE,
    STR_OBJECT_SELECTION_WATER,
    STR_OBJECT_SELECTION_SCENARIO_TEXTS,
    STR_OBJECT_SELECTION_TERRAIN_SURFACES,
    STR_OBJECT_SELECTION_TERRAIN_EDGES,
    STR_OBJECT_SELECTION_STATIONS,
    STR_OBJECT_SELECTION_MUSIC,
    STR_OBJECT_SELECTION_FOOTPATH_SURFACES,
    STR_OBJECT_SELECTION_FOOTPATH_RAILINGS,
    STR_OBJECT_SELECTION_MUSIC,
    STR_OBJECT_SELECTION_PEEP_NAMES,
    STR_OBJECT_SELECTION_PEEP_ANIMATIONS,
});
static_assert(_objectTypeNames.size() == EnumValue(ObjectType::Count));

static void ConsoleCommandCountObjects(InteractiveConsole& console, [[maybe_unused]] const arguments_t& argv)
{
    for (auto objectType : getAllObjectTypes())
    {
        uint32_t entryGroupIndex = 0;
        for (; entryGroupIndex < getObjectEntryGroupCount(objectType); entryGroupIndex++)
        {
            if (ObjectEntryGetObject(objectType, entryGroupIndex) == nullptr)
            {
                break;
            }
        }

        const auto objectStringId = _objectTypeNames[EnumValue(objectType)];
        const auto* objectString = LanguageGetString(objectStringId);

        console.WriteFormatLine("%s: %d/%d", objectString, entryGroupIndex, getObjectEntryGroupCount(objectType));
    }
}

static void ConsoleCommandOpen(InteractiveConsole& console, const arguments_t& argv)
{
    if (!argv.empty())
    {
        bool title = (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) != 0;
        bool invalidTitle = false;
        if (argv[0] == "object_selection" && InvalidArguments(&invalidTitle, !title))
        {
            if (NetworkGetMode() != NETWORK_MODE_NONE)
            {
                console.WriteLineError("Cannot open this window in multiplayer mode.");
            }
            else
            {
                // Only this window should be open for safety reasons
                auto* windowMgr = Ui::GetWindowManager();
                windowMgr->CloseAll();
                ContextOpenWindow(WindowClass::EditorObjectSelection);
            }
        }
        else if (argv[0] == "inventions_list" && InvalidArguments(&invalidTitle, !title))
        {
            if (NetworkGetMode() != NETWORK_MODE_NONE)
            {
                console.WriteLineError("Cannot open this window in multiplayer mode.");
            }
            else
            {
                ContextOpenWindow(WindowClass::EditorInventionList);
            }
        }
        else if (argv[0] == "scenario_options" && InvalidArguments(&invalidTitle, !title))
        {
            ContextOpenWindow(WindowClass::EditorScenarioOptions);
        }
        else if (argv[0] == "objective_options" && InvalidArguments(&invalidTitle, !title))
        {
            if (NetworkGetMode() != NETWORK_MODE_NONE)
            {
                console.WriteLineError("Cannot open this window in multiplayer mode.");
            }
            else
            {
                ContextOpenWindow(WindowClass::EditorObjectiveOptions);
            }
        }
        else if (argv[0] == "options")
        {
            ContextOpenWindow(WindowClass::Options);
        }
        else if (argv[0] == "themes")
        {
            ContextOpenWindow(WindowClass::Themes);
        }
        else if (invalidTitle)
        {
            console.WriteLineError("Cannot open this window in the title screen.");
        }
        else
        {
            console.WriteLineError("Invalid window.");
        }
    }
}

static void ConsoleCommandRemoveUnusedObjects(InteractiveConsole& console, [[maybe_unused]] const arguments_t& argv)
{
    int32_t result = EditorRemoveUnusedObjects();
    console.WriteFormatLine("%d unused object entries have been removed.", result);
}

static void ConsoleCommandRemoveFloatingObjects(InteractiveConsole& console, const arguments_t& argv)
{
    uint16_t result = RemoveFloatingEntities();
    console.WriteFormatLine("Removed %d flying objects", result);
}

static void ConsoleCommandShowLimits(InteractiveConsole& console, [[maybe_unused]] const arguments_t& argv)
{
    const auto& tileElements = GetTileElements();
    const auto tileElementCount = tileElements.size();

    int32_t rideCount = RideGetCount();
    int32_t spriteCount = 0;
    for (int32_t i = 0; i < static_cast<uint8_t>(EntityType::Count); ++i)
    {
        spriteCount += GetEntityListCount(EntityType(i));
    }

    auto bannerCount = GetNumBanners();

    console.WriteFormatLine("Sprites: %d/%d", spriteCount, kMaxEntities);
    console.WriteFormatLine("Map Elements: %zu/%d", tileElementCount, kMaxTileElements);
    console.WriteFormatLine("Banners: %d/%zu", bannerCount, MAX_BANNERS);
    console.WriteFormatLine("Rides: %d/%d", rideCount, OpenRCT2::Limits::kMaxRidesInPark);
    console.WriteFormatLine("Images: %zu/%zu", ImageListGetUsedCount(), ImageListGetMaximum());
}

static void ConsoleCommandForceDate([[maybe_unused]] InteractiveConsole& console, [[maybe_unused]] const arguments_t& argv)
{
    int32_t year = 0;
    int32_t month = 0;
    int32_t day = 0;
    if (argv.size() < 1 || argv.size() > 3)
    {
        return;
    }

    // All cases involve providing a year, so grab that first
    year = atoi(argv[0].c_str());
    if (year < 1 || year > kMaxYear)
    {
        return;
    }

    // YYYY (no month provided, preserve existing month)
    if (argv.size() == 1)
    {
        month = GetDate().GetMonth() + 1;
    }

    // YYYY MM or YYYY MM DD (month provided)
    if (argv.size() >= 2)
    {
        month = atoi(argv[1].c_str());
        month -= 2;
        if (month < 1 || month > MONTH_COUNT)
        {
            return;
        }
    }

    // YYYY OR YYYY MM (no day provided, preserve existing day)
    if (argv.size() <= 2)
    {
        day = std::clamp(GetDate().GetDay() + 1, 1, static_cast<int>(Date::GetDaysInMonth(month - 1)));
    }

    // YYYY MM DD (year, month, and day provided)
    if (argv.size() == 3)
    {
        day = atoi(argv[2].c_str());
        if (day < 1 || day > Date::GetDaysInMonth(month - 1))
        {
            return;
        }
    }

    auto setDateAction = ParkSetDateAction(year - 1, month - 1, day - 1);
    GameActions::Execute(&setDateAction);

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByClass(WindowClass::BottomToolbar);
}

static void ConsoleCommandLoadPark([[maybe_unused]] InteractiveConsole& console, [[maybe_unused]] const arguments_t& argv)
{
    if (argv.size() < 1)
    {
        console.WriteLine("Parameters required <filename>");
        return;
    }

    u8string savePath = {};
    if (String::indexOf(argv[0].c_str(), '/') == SIZE_MAX && String::indexOf(argv[0].c_str(), '\\') == SIZE_MAX)
    {
        // no / or \ was included. File should be in save dir.
        auto env = OpenRCT2::GetContext()->GetPlatformEnvironment();
        auto directory = env->GetDirectoryPath(OpenRCT2::DIRBASE::USER, OpenRCT2::DIRID::SAVE);
        savePath = Path::Combine(directory, argv[0]);
    }
    else
    {
        savePath = argv[0];
    }
    if (!String::endsWith(savePath, ".sv6", true) && !String::endsWith(savePath, ".sc6", true)
        && !String::endsWith(savePath, ".park", true))
    {
        savePath += ".park";
    }
    if (OpenRCT2::GetContext()->LoadParkFromFile(savePath))
    {
        console.WriteFormatLine("Park %s was loaded successfully", savePath.c_str());
    }
    else
    {
        console.WriteFormatLine("Loading Park %s failed", savePath.c_str());
    }
}

static void ConsoleCommandSavePark([[maybe_unused]] InteractiveConsole& console, [[maybe_unused]] const arguments_t& argv)
{
    if (argv.size() < 1)
    {
        SaveGameCmd();
    }
    else
    {
        SaveGameCmd(argv[0].c_str());
    }
}

static void ConsoleCommandSay(InteractiveConsole& console, const arguments_t& argv)
{
    if (NetworkGetMode() == NETWORK_MODE_NONE || NetworkGetStatus() != NETWORK_STATUS_CONNECTED
        || NetworkGetAuthstatus() != NetworkAuth::Ok)
    {
        console.WriteFormatLine("This command only works in multiplayer mode.");
        return;
    }

    if (!argv.empty())
    {
        NetworkSendChat(argv[0].c_str());
        return;
    }

    console.WriteFormatLine("Input your message");
}

static void ConsoleCommandReplayStartRecord(InteractiveConsole& console, const arguments_t& argv)
{
    if (NetworkGetMode() != NETWORK_MODE_NONE)
    {
        console.WriteFormatLine("This command is currently not supported in multiplayer mode.");
        return;
    }

    if (argv.size() < 1)
    {
        console.WriteFormatLine("Parameters required <replay_name> [<max_ticks = 0xFFFFFFFF>]");
        return;
    }

    std::string name = argv[0];

    if (!String::endsWith(name, ".parkrep", true))
    {
        name += ".parkrep";
    }
    std::string outPath = OpenRCT2::GetContext()->GetPlatformEnvironment()->GetDirectoryPath(
        OpenRCT2::DIRBASE::USER, OpenRCT2::DIRID::REPLAY);
    name = Path::Combine(outPath, name);

    // If ticks are specified by user use that otherwise maximum ticks specified by const.
    uint32_t maxTicks = OpenRCT2::k_MaxReplayTicks;
    if (argv.size() >= 2)
    {
        maxTicks = atol(argv[1].c_str());
    }

    auto* replayManager = OpenRCT2::GetContext()->GetReplayManager();
    if (replayManager->StartRecording(name, maxTicks))
    {
        OpenRCT2::ReplayRecordInfo info;
        replayManager->GetCurrentReplayInfo(info);

        const char* logFmt = "Replay recording started: (%s) %s";
        console.WriteFormatLine(logFmt, info.Name.c_str(), info.FilePath.c_str());
        Console::WriteLine(logFmt, info.Name.c_str(), info.FilePath.c_str());
    }
}

static void ConsoleCommandReplayStopRecord(InteractiveConsole& console, const arguments_t& argv)
{
    if (NetworkGetMode() != NETWORK_MODE_NONE)
    {
        console.WriteFormatLine("This command is currently not supported in multiplayer mode.");
        return;
    }

    auto* replayManager = OpenRCT2::GetContext()->GetReplayManager();
    if (!replayManager->IsRecording() && !replayManager->IsNormalising())
    {
        console.WriteFormatLine("Replay currently not recording");
        return;
    }

    OpenRCT2::ReplayRecordInfo info;
    replayManager->GetCurrentReplayInfo(info);

    if (replayManager->StopRecording())
    {
        const char* logFmt = "Replay recording stopped: (%s) %s\n"
                             "  Ticks: %u\n"
                             "  Commands: %u\n"
                             "  Checksums: %u";

        console.WriteFormatLine(
            logFmt, info.Name.c_str(), info.FilePath.c_str(), info.Ticks, info.NumCommands, info.NumChecksums);
        Console::WriteLine(logFmt, info.Name.c_str(), info.FilePath.c_str(), info.Ticks, info.NumCommands, info.NumChecksums);
    }
}

static void ConsoleCommandReplayStart(InteractiveConsole& console, const arguments_t& argv)
{
    if (NetworkGetMode() != NETWORK_MODE_NONE)
    {
        console.WriteFormatLine("This command is currently not supported in multiplayer mode.");
        return;
    }

    if (argv.size() < 1)
    {
        console.WriteFormatLine("Parameters required <replay_name>");
        return;
    }

    std::string name = argv[0];

    auto* replayManager = OpenRCT2::GetContext()->GetReplayManager();
    if (replayManager->StartPlayback(name))
    {
        OpenRCT2::ReplayRecordInfo info;
        replayManager->GetCurrentReplayInfo(info);

        std::time_t ts = info.TimeRecorded;

        char recordingDate[128] = {};
        std::strftime(recordingDate, sizeof(recordingDate), "%c", std::localtime(&ts));

        const char* logFmt = "Replay playback started: %s\n"
                             "  Date Recorded: %s\n"
                             "  Ticks: %u\n"
                             "  Commands: %u\n"
                             "  Checksums: %u";

        console.WriteFormatLine(logFmt, info.FilePath.c_str(), recordingDate, info.Ticks, info.NumCommands, info.NumChecksums);
        Console::WriteLine(logFmt, info.FilePath.c_str(), recordingDate, info.Ticks, info.NumCommands, info.NumChecksums);
    }
}

static void ConsoleCommandReplayStop(InteractiveConsole& console, const arguments_t& argv)
{
    if (NetworkGetMode() != NETWORK_MODE_NONE)
    {
        console.WriteFormatLine("This command is currently not supported in multiplayer mode.");
        return;
    }

    auto* replayManager = OpenRCT2::GetContext()->GetReplayManager();
    if (replayManager->StopPlayback())
    {
        console.WriteFormatLine("Stopped replay");
    }
}

static void ConsoleCommandReplayNormalise(InteractiveConsole& console, const arguments_t& argv)
{
    if (NetworkGetMode() != NETWORK_MODE_NONE)
    {
        console.WriteFormatLine("This command is currently not supported in multiplayer mode.");
    }

    if (argv.size() < 2)
    {
        console.WriteFormatLine("Parameters required <replay_input> <replay_output>");
    }

    std::string inputFile = argv[0];
    std::string outputFile = argv[1];

    if (!String::endsWith(outputFile, ".parkrep", true))
    {
        outputFile += ".parkrep";
    }
    std::string outPath = OpenRCT2::GetContext()->GetPlatformEnvironment()->GetDirectoryPath(
        OpenRCT2::DIRBASE::USER, OpenRCT2::DIRID::REPLAY);
    outputFile = Path::Combine(outPath, outputFile);

    auto* replayManager = OpenRCT2::GetContext()->GetReplayManager();
    if (replayManager->NormaliseReplay(inputFile, outputFile))
    {
        console.WriteFormatLine("Stopped replay");
    }
}

static void ConsoleCommandMpDesync(InteractiveConsole& console, const arguments_t& argv)
{
    int32_t desyncType = 0;
    if (argv.size() >= 1)
    {
        desyncType = atoi(argv[0].c_str());
    }

    std::vector<Guest*> guests;

    for (auto* guest : EntityList<Guest>())
    {
        guests.push_back(guest);
    }

    switch (desyncType)
    {
        case 0: // Guest t-shirts.
        {
            if (guests.empty())
            {
                console.WriteFormatLine("No guests");
            }
            else
            {
                auto* guest = guests[0];
                if (guests.size() > 1)
                    guest = guests[UtilRand() % guests.size() - 1];
                guest->TshirtColour = UtilRand() & 0xFF;
                guest->Invalidate();
            }
            break;
        }
        case 1: // Remove random guest.
        {
            if (guests.empty())
            {
                console.WriteFormatLine("No guest removed");
            }
            else
            {
                auto* guest = guests[0];
                if (guests.size() > 1)
                    guest = guests[UtilRand() % guests.size() - 1];
                guest->Remove();
            }
            break;
        }
    }
}

#pragma warning(push)
#pragma warning(disable : 4702) // unreachable code
static void ConsoleCommandAbort([[maybe_unused]] InteractiveConsole& console, [[maybe_unused]] const arguments_t& argv)
{
    std::abort();
}

static void ConsoleCommandDereference([[maybe_unused]] InteractiveConsole& console, [[maybe_unused]] const arguments_t& argv)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
    // Dereference a nullptr to induce a crash to be caught by crash handler, on supported platforms
    uint8_t* myptr = nullptr;
    *myptr = 42;
#pragma GCC diagnostic pop
}

static void ConsoleCommandTerminate([[maybe_unused]] InteractiveConsole& console, [[maybe_unused]] const arguments_t& argv)
{
    std::terminate();
}
#pragma warning(pop)

static void ConsoleCommandAssert([[maybe_unused]] InteractiveConsole& console, [[maybe_unused]] const arguments_t& argv)
{
    if (!argv.empty())
        Guard::Assert(false, "%s", argv[0].c_str());
    else
        Guard::Assert(false);
}

static void ConsoleCommandAddNewsItem([[maybe_unused]] InteractiveConsole& console, [[maybe_unused]] const arguments_t& argv)
{
    if (argv.size() < 2)
    {
        console.WriteLineWarning("Too few arguments");
        static_assert(News::ItemTypeCount == 11, "News::ItemType::Count changed, update console command!");
        console.WriteLine("add_news_item <type> <message> [assoc]");
        console.WriteLine("type is one of:");
        console.WriteLine("    0 (News::ItemType::Null)");
        console.WriteLine("    1 (News::ItemType::Ride)");
        console.WriteLine("    2 (News::ItemType::PeepOnRide)");
        console.WriteLine("    3 (News::ItemType::Peep)");
        console.WriteLine("    4 (News::ItemType::Money)");
        console.WriteLine("    5 (News::ItemType::Blank)");
        console.WriteLine("    6 (News::ItemType::Research)");
        console.WriteLine("    7 (News::ItemType::Peeps)");
        console.WriteLine("    8 (News::ItemType::Award)");
        console.WriteLine("    9 (News::ItemType::Graph)");
        console.WriteLine("   10 (News::ItemType::Campaign)");
        console.WriteLine("message is the message to display, wrapped in quotes for multiple words");
        console.WriteLine("assoc is the associated id of ride/peep/tile/etc. If the selected ItemType doesn't need an assoc "
                          "(Null, Money, Award, Graph), you can leave this field blank");
        return;
    }

    auto type = atoi(argv[0].c_str());
    auto msg = argv[1].c_str();
    auto assoc = 0;

    News::ItemType itemType = static_cast<News::ItemType>(type);

    if (argv.size() == 3) // 3 arguments passed, set assoc
    {
        assoc = atoi(argv[2].c_str());
    }
    else
    {
        if (News::CheckIfItemRequiresAssoc(itemType))
        {
            console.WriteLine("Selected ItemType requires an assoc");
            return;
        }
    }

    News::AddItemToQueue(itemType, msg, assoc);
    console.WriteLine("Successfully added News Item");
}

static void ConsoleCommandProfilerReset([[maybe_unused]] InteractiveConsole& console, [[maybe_unused]] const arguments_t& argv)
{
    OpenRCT2::Profiling::ResetData();
}
static void ConsoleCommandProfilerStart([[maybe_unused]] InteractiveConsole& console, [[maybe_unused]] const arguments_t& argv)
{
    if (!OpenRCT2::Profiling::IsEnabled())
        console.WriteLine("Started profiler");
    OpenRCT2::Profiling::Enable();
}

static void ConsoleCommandProfilerExportCSV(
    [[maybe_unused]] InteractiveConsole& console, [[maybe_unused]] const arguments_t& argv)
{
    if (argv.size() < 1)
    {
        console.WriteLineError("Missing argument: <file path>");
    }

    const auto& csvFilePath = argv[0];
    if (!OpenRCT2::Profiling::ExportCSV(csvFilePath))
    {
        console.WriteFormatLine("Unable to export CSV file to %s", csvFilePath.c_str());
    }

    console.WriteFormatLine("Wrote file CSV file: \"%s\"", csvFilePath.c_str());
}

static void ConsoleCommandProfilerStop([[maybe_unused]] InteractiveConsole& console, [[maybe_unused]] const arguments_t& argv)
{
    if (OpenRCT2::Profiling::IsEnabled())
        console.WriteLine("Stopped profiler");
    OpenRCT2::Profiling::Disable();

    // Export to CSV if argument is provided.
    if (argv.size() >= 1)
    {
        return ConsoleCommandProfilerExportCSV(console, argv);
    }
}

static void ConsoleSpawnBalloon(InteractiveConsole& console, const arguments_t& argv)
{
    if (argv.size() < 3)
    {
        console.WriteLineError("Need arguments: <x> <y> <z> <colour>");
        return;
    }
    int32_t x = kCoordsXYStep * atof(argv[0].c_str());
    int32_t y = kCoordsXYStep * atof(argv[1].c_str());
    int32_t z = kCoordsZStep * atof(argv[2].c_str());
    int32_t col = 28;
    if (argv.size() > 3)
        col = atoi(argv[3].c_str());
    Balloon::Create({ x, y, z }, col, false);
}

using console_command_func = void (*)(InteractiveConsole& console, const arguments_t& argv);
struct ConsoleCommand
{
    const utf8* command;
    console_command_func func;
    const utf8* help;
    const utf8* usage;
};

// clang-format off
static constexpr const utf8* console_variable_table[] = {
    "park_rating",
    "park_value",
    "company_value",
    "money",
    "scenario_initial_cash",
    "current_loan",
    "max_loan",
    "guest_initial_cash",
    "guest_initial_happiness",
    "guest_initial_hunger",
    "guest_initial_thirst",
    "guest_prefer_less_intense_rides",
    "guest_prefer_more_intense_rides",
    "forbid_marketing_campaigns",
    "forbid_landscape_changes",
    "forbid_tree_removal",
    "forbid_high_construction",
    "pay_for_rides",
    "no_money",
    "difficult_park_rating",
    "difficult_guest_generation",
    "land_rights_cost",
    "construction_rights_cost",
    "park_open",
    "climate",
    "game_speed",
    "console_small_font",
    "location",
    "window_scale",
    "window_limit",
    "render_weather_effects",
    "render_weather_gloom",
    "cheat_sandbox_mode",
    "cheat_disable_clearance_checks",
    "cheat_disable_support_limits",
    "current_rotation",
};

static constexpr const utf8* console_window_table[] = {
    "object_selection",
    "inventions_list",
    "scenario_options",
    "objective_options",
    "options",
    "themes",
    "title_sequences",
};
// clang-format on

static constexpr ConsoleCommand console_command_table[] = {
    { "abort", ConsoleCommandAbort, "Calls std::abort(), for testing purposes only.", "abort" },
    { "add_news_item", ConsoleCommandAddNewsItem, "Inserts a news item", "add_news_item [<type> <message> <assoc>]" },
    { "assert", ConsoleCommandAssert, "Triggers assertion failure, for testing purposes only", "assert" },
    { "clear", ConsoleCommandClear, "Clears the console.", "clear" },
    { "close", ConsoleCommandClose, "Closes the console.", "close" },
    { "date", ConsoleCommandForceDate, "Sets the date to a given date.", "Format <year>[ <month>[ <day>]]." },
    { "dereference", ConsoleCommandDereference, "Dereferences a nullptr, for testing purposes only", "dereference" },
    { "echo", ConsoleCommandEcho, "Echoes the text to the console.", "echo <text>" },
    { "exit", ConsoleCommandClose, "Closes the console.", "exit" },
    { "get", ConsoleCommandGet, "Gets the value of the specified variable.", "get <variable>" },
    { "help", ConsoleCommandHelp, "Lists commands or info about a command.", "help [command]" },
    { "hide", ConsoleCommandHide, "Hides the console.", "hide" },
    { "load_object", ConsoleCommandLoadObject,
      "Loads the object file into the scenario.\n"
      "Loading a scenery group will not load its associated objects.\n"
      "This is a safer method opposed to \"open object_selection\".",
      "load_object <objectfilenodat>" },
    { "load_park", ConsoleCommandLoadPark, "Load park from save directory or by absolute path", "load_park <filename>" },
    { "object_count", ConsoleCommandCountObjects, "Shows the number of objects of each type in the scenario.", "object_count" },
    { "open", ConsoleCommandOpen, "Opens the window with the give name.", "open <window>." },
    { "quit", ConsoleCommandClose, "Closes the console.", "quit" },
    { "remove_unused_objects", ConsoleCommandRemoveUnusedObjects, "Removes all the unused objects from the object selection.",
      "remove_unused_objects" },
    { "remove_floating_objects", ConsoleCommandRemoveFloatingObjects, "Removes floating objects", "remove_floating_objects" },
    { "rides", ConsoleCommandRides, "Ride management.", "rides <subcommand>" },
    { "save_park", ConsoleCommandSavePark, "Save current state of park. If no name specified default path will be used.",
      "save_park [name]" },
    { "say", ConsoleCommandSay, "Say to other players.", "say <message>" },
    { "set", ConsoleCommandSet, "Sets the variable to the specified value.", "set <variable> <value>" },
    { "show_limits", ConsoleCommandShowLimits, "Shows the map data counts and limits.", "show_limits" },
    { "spawn_balloon", ConsoleSpawnBalloon, "Spawns a balloon.", "spawn_balloon <x> <y> <z> <colour>" },
    { "staff", ConsoleCommandStaff, "Staff management.", "staff <subcommand>" },
    { "terminate", ConsoleCommandTerminate, "Calls std::terminate(), for testing purposes only.", "terminate" },
    { "variables", ConsoleCommandVariables, "Lists all the variables that can be used with get and sometimes set.",
      "variables" },
    { "windows", ConsoleCommandWindows, "Lists all the windows that can be opened.", "windows" },
    { "replay_startrecord", ConsoleCommandReplayStartRecord, "Starts recording a new replay.",
      "replay_startrecord <name> [max_ticks]" },
    { "replay_stoprecord", ConsoleCommandReplayStopRecord, "Stops recording a new replay.", "replay_stoprecord" },
    { "replay_start", ConsoleCommandReplayStart, "Starts a replay", "replay_start <name>" },
    { "replay_stop", ConsoleCommandReplayStop, "Stops the replay", "replay_stop" },
    { "replay_normalise", ConsoleCommandReplayNormalise, "Normalises the replay to remove all gaps",
      "replay_normalise <input file> <output file>" },
    { "mp_desync", ConsoleCommandMpDesync, "Forces a multiplayer desync",
      "ConsoleCommandMpDesync [desync_type, 0 = Random t-shirt color on random guest, 1 = Remove random guest ]" },
    { "profiler_reset", ConsoleCommandProfilerReset, "Resets the profiler data.", "profiler_reset" },
    { "profiler_start", ConsoleCommandProfilerStart, "Starts the profiler.", "profiler_start" },
    { "profiler_stop", ConsoleCommandProfilerStop, "Stops the profiler.", "profiler_stop [<output file>]" },
    { "profiler_exportcsv", ConsoleCommandProfilerExportCSV, "Exports the current profiler data.",
      "profiler_exportcsv <output file>" },
};

static void ConsoleCommandWindows(InteractiveConsole& console, [[maybe_unused]] const arguments_t& argv)
{
    for (auto s : console_window_table)
    {
        console.WriteLine(s);
    }
}

static void ConsoleCommandVariables(InteractiveConsole& console, [[maybe_unused]] const arguments_t& argv)
{
    for (auto s : console_variable_table)
    {
        console.WriteLine(s);
    }
}

static void ConsoleCommandHelp(InteractiveConsole& console, const arguments_t& argv)
{
    if (!argv.empty())
    {
        for (const auto& c : console_command_table)
        {
            if (argv[0] == c.command)
            {
                console.WriteLine(c.help);
                console.WriteFormatLine("\nUsage:   %s", c.usage);
            }
        }
    }
    else
    {
        ConsoleWriteAllCommands(console);
    }
}

static void ConsoleWriteAllCommands(InteractiveConsole& console)
{
    for (const auto& c : console_command_table)
    {
        console.WriteLine(c.command);
    }
}

static bool InvalidArguments(bool* invalid, bool arguments)
{
    if (!arguments)
    {
        *invalid = true;
        return false;
    }
    return true;
}

void InteractiveConsole::Execute(const std::string& s)
{
    arguments_t argv;
    argv.reserve(8);

    const utf8* start = s.c_str();
    const utf8* end;
    bool inQuotes = false;
    do
    {
        while (*start == ' ')
            start++;

        if (*start == '"')
        {
            inQuotes = true;
            start++;
        }
        else
        {
            inQuotes = false;
        }

        end = start;
        while (*end != 0)
        {
            if (*end == ' ' && !inQuotes)
                break;
            if (*end == '"' && inQuotes)
                break;
            end++;
        }
        size_t length = end - start;

        if (length > 0)
        {
            argv.emplace_back(start, length);
        }

        start = end;
    } while (*end != 0);

    if (argv.empty())
        return;

    bool validCommand = false;
    for (const auto& c : console_command_table)
    {
        if (argv[0] == c.command)
        {
            argv.erase(argv.begin());
            c.func(*this, argv);
            validCommand = true;
            break;
        }
    }

    if (!validCommand)
    {
        WriteLineError("Unknown command. Type help to list available commands.");
    }
}

void InteractiveConsole::WriteLine(const std::string& s)
{
    WriteLine(s, FormatToken::ColourWindow2);
}

void InteractiveConsole::WriteLineError(const std::string& s)
{
    WriteLine(s, FormatToken::ColourRed);
}

void InteractiveConsole::WriteLineWarning(const std::string& s)
{
    WriteLine(s, FormatToken::ColourYellow);
}

void InteractiveConsole::WriteFormatLine(const char* format, ...)
{
    va_list list;
    va_start(list, format);
    auto buffer = String::formatVA(format, list);
    va_end(list);
    WriteLine(buffer);
}

void InteractiveConsole::BeginAsyncExecution()
{
    OpenRCT2::Guard::Assert(!_commandExecuting.test_and_set(), "Command already executing asynchronously");
}

void InteractiveConsole::EndAsyncExecution()
{
    _commandExecuting.clear();
}

bool InteractiveConsole::IsExecuting()
{
    return _commandExecuting.test();
}
