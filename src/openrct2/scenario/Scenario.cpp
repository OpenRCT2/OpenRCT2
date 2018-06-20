/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../audio/audio.h"
#include "../Cheats.h"
#include "../config/Config.h"
#include "../FileClassifier.h"
#include "../Game.h"
#include "../GameState.h"
#include "../interface/Viewport.h"
#include "../localisation/Date.h"
#include "../localisation/Localisation.h"
#include "../management/Award.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../management/Research.h"
#include "../network/network.h"
#include "../object/Object.h"
#include "../object/ObjectList.h"
#include "../OpenRCT2.h"
#include "../ParkImporter.h"
#include "../peep/Staff.h"
#include "../platform/platform.h"
#include "../rct1/RCT1.h"
#include "../ride/Ride.h"
#include "../util/SawyerCoding.h"
#include "../util/Util.h"
#include "../world/Climate.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Sprite.h"
#include "../world/Water.h"
#include "ScenarioRepository.h"
#include "ScenarioSources.h"
#include "Scenario.h"
#include "../Context.h"
#include "../ride/Track.h"
#include "../windows/Intent.h"

const rct_string_id ScenarioCategoryStringIds[SCENARIO_CATEGORY_COUNT] = {
    STR_BEGINNER_PARKS,
    STR_CHALLENGING_PARKS,
    STR_EXPERT_PARKS,
    STR_REAL_PARKS,
    STR_OTHER_PARKS,

    STR_DLC_PARKS,
    STR_BUILD_YOUR_OWN_PARKS,
};

rct_s6_info gS6Info;
char gScenarioName[64];
char gScenarioDetails[256];
char gScenarioCompletedBy[32];
char gScenarioSavePath[MAX_PATH];
char gScenarioExpansionPacks[3256];
bool gFirstTimeSaving = true;
uint16_t gSavedAge;
uint32_t gLastAutoSaveUpdate = 0;

uint32_t gScenarioTicks;
uint32_t gScenarioSrand0;
uint32_t gScenarioSrand1;

uint8_t gScenarioObjectiveType;
uint8_t gScenarioObjectiveYear;
uint16_t gScenarioObjectiveNumGuests;
money32 gScenarioObjectiveCurrency;

uint16_t gScenarioParkRatingWarningDays;
money32 gScenarioCompletedCompanyValue;
money32 gScenarioCompanyValueRecord;

char gScenarioFileName[MAX_PATH];

static int32_t scenario_create_ducks();
static void scenario_objective_check();

using namespace OpenRCT2;

void scenario_begin()
{
    game_load_init();

    // Set the scenario pseudo-random seeds
    gScenarioSrand0 ^= platform_get_ticks();
    gScenarioSrand1 ^= platform_get_ticks();


    gParkFlags &= ~PARK_FLAGS_NO_MONEY;
    if (gParkFlags & PARK_FLAGS_NO_MONEY_SCENARIO)
        gParkFlags |= PARK_FLAGS_NO_MONEY;
    research_reset_current_item();
    scenery_set_default_placement_configuration();
    news_item_init_queue();
    if (gScenarioObjectiveType != OBJECTIVE_NONE && !gLoadKeepWindowsOpen)
        context_open_window_view(WV_PARK_OBJECTIVE);

    auto& park = GetContext()->GetGameState()->GetPark();
    gParkRating = park.CalculateParkRating();
    gParkValue = park.CalculateParkValue();
    gCompanyValue = park.CalculateCompanyValue();
    gHistoricalProfit = gInitialCash - gBankLoan;
    gCash = gInitialCash;

    safe_strcpy(gScenarioDetails, gS6Info.details, 256);
    safe_strcpy(gScenarioName, gS6Info.name, 64);

    {
        utf8 normalisedName[64];
        scenario_normalise_name(normalisedName, sizeof(normalisedName), gS6Info.name);

        rct_string_id localisedStringIds[3];
        if (language_get_localised_scenario_strings(normalisedName, localisedStringIds)) {
            if (localisedStringIds[0] != STR_NONE) {
                safe_strcpy(gScenarioName, language_get_string(localisedStringIds[0]), 32);
            }
            if (localisedStringIds[1] != STR_NONE) {
                park_set_name(language_get_string(localisedStringIds[1]));
            }
            if (localisedStringIds[2] != STR_NONE) {
                safe_strcpy(gScenarioDetails, language_get_string(localisedStringIds[2]), 256);
            }
        }
        else {
            auto stex = (rct_stex_entry *)object_entry_get_chunk(OBJECT_TYPE_SCENARIO_TEXT, 0);
            if (stex != nullptr) {
                char *buffer = gCommonStringFormatBuffer;

                // Set localised park name
                format_string(buffer, 256, stex->park_name, nullptr);
                park_set_name(buffer);

                // Set localised scenario name
                format_string(buffer, 256, stex->scenario_name, nullptr);
                safe_strcpy(gScenarioName, buffer, 64);

                // Set localised scenario details
                format_string(buffer, 256, stex->details, nullptr);
                safe_strcpy(gScenarioDetails, buffer, 256);
            }
        }
    }

    // Set the last saved game path
    char parkName[128];
    format_string(parkName, 128, gParkName, &gParkNameArgs);

    platform_get_user_directory(gScenarioSavePath, "save", sizeof(gScenarioSavePath));
    safe_strcat_path(gScenarioSavePath, parkName, sizeof(gScenarioSavePath));
    path_append_extension(gScenarioSavePath, ".sv6", sizeof(gScenarioSavePath));

    gCurrentExpenditure = 0;
    gCurrentProfit = 0;
    gWeeklyProfitAverageDividend = 0;
    gWeeklyProfitAverageDivisor = 0;
    gScenarioCompletedCompanyValue = MONEY32_UNDEFINED;
    gTotalAdmissions = 0;
    gTotalIncomeFromAdmissions = 0;
    safe_strcpy(gScenarioCompletedBy, "?", sizeof(gScenarioCompletedBy));
    park.ResetHistories();
    finance_reset_history();
    award_reset();
    reset_all_ride_build_dates();
    date_reset();
    duck_remove_all();
    park_calculate_size();
    map_count_remaining_land_rights();
    staff_reset_stats();
    gLastEntranceStyle = RIDE_ENTRANCE_STYLE_PLAIN;
    memset(gMarketingCampaignDaysLeft, 0, 20);
    gParkRatingCasualtyPenalty = 0;

    // Open park with free entry when there is no money
    if (gParkFlags & PARK_FLAGS_NO_MONEY) {
        gParkFlags |= PARK_FLAGS_PARK_OPEN;
        gParkEntranceFee = 0;
    }

    gParkFlags |= PARK_FLAGS_SPRITES_INITIALISED;

    gScreenAge = 0;
}

static void scenario_end()
{
    window_close_by_class(WC_DROPDOWN);
    window_close_all_except_flags(WF_STICK_TO_BACK | WF_STICK_TO_FRONT);
    context_open_window_view(WV_PARK_OBJECTIVE);
}

/**
 *
 *  rct2: 0x0066A752
 */
void scenario_failure()
{
    gScenarioCompletedCompanyValue = 0x80000001;
    scenario_end();
}

/**
 *
 *  rct2: 0x0066A75E
 */
void scenario_success()
{
    const money32 companyValue = gCompanyValue;

    gScenarioCompletedCompanyValue = companyValue;
    peep_applause();

    if (scenario_repository_try_record_highscore(gScenarioFileName, companyValue, nullptr))
    {
        // Allow name entry
        gParkFlags |= PARK_FLAGS_SCENARIO_COMPLETE_NAME_INPUT;
        gScenarioCompanyValueRecord = companyValue;
    }
    scenario_end();
}

/**
 *
 *  rct2: 0x006695E8
 */
void scenario_success_submit_name(const char *name)
{
    if (scenario_repository_try_record_highscore(gScenarioFileName, gScenarioCompanyValueRecord, name))
    {
        safe_strcpy(gScenarioCompletedBy, name, 32);
    }
    gParkFlags &= ~PARK_FLAGS_SCENARIO_COMPLETE_NAME_INPUT;
}

/**
 * Send a warning when entrance price is too high.
 *  rct2: 0x0066A80E
 */
static void scenario_entrance_fee_too_high_check()
{
    uint16_t x = 0, y = 0;
    money16 totalRideValueForMoney = gTotalRideValueForMoney;
    money16 max_fee = totalRideValueForMoney + (totalRideValueForMoney / 2);

    if ((gParkFlags & PARK_FLAGS_PARK_OPEN) && park_get_entrance_fee() > max_fee) {
        for (int32_t i = 0; i < MAX_PARK_ENTRANCES && gParkEntrances[i].x != LOCATION_NULL; i++) {
            x = gParkEntrances[i].x + 16;
            y = gParkEntrances[i].y + 16;
        }

        uint32_t packed_xy = (y << 16) | x;
        if (gConfigNotifications.park_warnings) {
            news_item_add_to_queue(NEWS_ITEM_BLANK, STR_ENTRANCE_FEE_TOO_HI, packed_xy);
        }
    }
}

void scenario_autosave_check()
{
    if (gLastAutoSaveUpdate == AUTOSAVE_PAUSE) return;

    // Milliseconds since last save
    uint32_t timeSinceSave = platform_get_ticks() - gLastAutoSaveUpdate;

    bool shouldSave = false;
    switch (gConfigGeneral.autosave_frequency) {
    case AUTOSAVE_EVERY_MINUTE:
        shouldSave = timeSinceSave >= 1 * 60 * 1000;
        break;
    case AUTOSAVE_EVERY_5MINUTES:
        shouldSave = timeSinceSave >= 5 * 60 * 1000;
        break;
    case AUTOSAVE_EVERY_15MINUTES:
        shouldSave = timeSinceSave >= 15 * 60 * 1000;
        break;
    case AUTOSAVE_EVERY_30MINUTES:
        shouldSave = timeSinceSave >= 30 * 60 * 1000;
        break;
    case AUTOSAVE_EVERY_HOUR:
        shouldSave = timeSinceSave >= 60 * 60 * 1000;
        break;
    }

    if (shouldSave) {
        gLastAutoSaveUpdate = AUTOSAVE_PAUSE;
        game_autosave();
    }
}

static void scenario_day_update()
{
    finance_update_daily_profit();
    peep_update_days_in_queue();
    switch (gScenarioObjectiveType) {
    case OBJECTIVE_10_ROLLERCOASTERS:
    case OBJECTIVE_GUESTS_AND_RATING:
    case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
    case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
    case OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE:
        scenario_objective_check();
        break;
    default:
        if (gConfigGeneral.allow_early_completion)
            scenario_objective_check();
        break;
    }

    // Lower the casualty penalty
    uint16_t casualtyPenaltyModifier = (gParkFlags & PARK_FLAGS_NO_MONEY) ? 40 : 7;
    gParkRatingCasualtyPenalty = std::max(0, gParkRatingCasualtyPenalty - casualtyPenaltyModifier);

    auto intent = Intent(INTENT_ACTION_UPDATE_DATE);
    context_broadcast_intent(&intent);
}

static void scenario_week_update()
{
    int32_t month = date_get_month(gDateMonthsElapsed);

    finance_pay_wages();
    finance_pay_research();
    finance_pay_interest();
    marketing_update();
    peep_problem_warnings_update();
    ride_check_all_reachable();
    ride_update_favourited_stat();

    auto water_type = (rct_water_type *)object_entry_get_chunk(OBJECT_TYPE_WATER, 0);

    if (month <= MONTH_APRIL && water_type != nullptr && water_type->flags & WATER_FLAGS_ALLOW_DUCKS) {
        // 100 attempts at finding some water to create a few ducks at
        for (int32_t i = 0; i < 100; i++) {
            if (scenario_create_ducks())
                break;
        }
    }
}

static void scenario_fortnight_update()
{
    finance_pay_ride_upkeep();
}

static void scenario_month_update()
{
    finance_shift_expenditure_table();
    scenario_objective_check();
    scenario_entrance_fee_too_high_check();
    award_update_all();
}

static void scenario_update_daynight_cycle()
{
    float currentDayNightCycle = gDayNightCycle;
    gDayNightCycle = 0;

    if (gScreenFlags == SCREEN_FLAGS_PLAYING && gConfigGeneral.day_night_cycle) {
        float monthFraction = gDateMonthTicks / (float)0x10000;
        if (monthFraction < (1 / 8.0f)) {
            gDayNightCycle = 0.0f;
        } else if (monthFraction < (3 / 8.0f)) {
            gDayNightCycle = (monthFraction - (1 / 8.0f)) / (2 / 8.0f);
        } else if (monthFraction < (5 / 8.0f)) {
            gDayNightCycle = 1.0f;
        } else if (monthFraction < (7 / 8.0f)) {
            gDayNightCycle = 1.0f - ((monthFraction - (5 / 8.0f)) / (2 / 8.0f));
        } else {
            gDayNightCycle = 0.0f;
        }
    }

    // Only update palette if day / night cycle has changed
    if (gDayNightCycle != currentDayNightCycle) {
        platform_update_palette(gGamePalette, 10, 236);
    }
}

/**
 * Scenario and finance related update iteration.
 *  rct2: 0x006C44B1
 */
void scenario_update()
{
    if (gScreenFlags == SCREEN_FLAGS_PLAYING)
    {
        if (date_is_day_start(gDateMonthTicks))
        {
            scenario_day_update();
        }
        if (date_is_week_start(gDateMonthTicks))
        {
            scenario_week_update();
        }
        if (date_is_fortnight_start(gDateMonthTicks))
        {
            scenario_fortnight_update();
        }
        if (date_is_month_start(gDateMonthTicks))
        {
            scenario_month_update();
        }
    }
    scenario_update_daynight_cycle();
}

/**
 *
 *  rct2: 0x006744A9
 */
static int32_t scenario_create_ducks()
{
    int32_t i, j, r, c, x, y, waterZ, centreWaterZ, x2, y2;

    r = scenario_rand();
    x = ((r >> 16) & 0xFFFF) & 0x7F;
    y = (r & 0xFFFF) & 0x7F;
    x = (x + 64) * 32;
    y = (y + 64) * 32;

    if (!map_is_location_in_park({x, y}))
        return 0;

    centreWaterZ = (tile_element_height(x, y) >> 16) & 0xFFFF;
    if (centreWaterZ == 0)
        return 0;

    // Check 7x7 area around centre tile
    x2 = x - (32 * 3);
    y2 = y - (32 * 3);
    c = 0;
    for (i = 0; i < 7; i++) {
        for (j = 0; j < 7; j++) {
            waterZ = (tile_element_height(x2, y2) >> 16) & 0xFFFF;
            if (waterZ == centreWaterZ)
                c++;

            x2 += 32;
        }
        x2 -= 224;
        y2 += 32;
    }

    // Must be at least 25 water tiles of the same height in 7x7 area
    if (c < 25)
        return 0;

    // Set x, y to the centre of the tile
    x += 16;
    y += 16;
    c = (scenario_rand() & 3) + 2;
    for (i = 0; i < c; i++) {
        r = scenario_rand();
        x2 = (r >> 16) & 0x7F;
        y2 = (r & 0xFFFF) & 0x7F;
        create_duck(x + x2 - 64, y + y2 - 64);
    }

    return 1;
}

/**
 *
 *  rct2: 0x006E37D2
 *
 * @return eax
 */
#ifndef DEBUG_DESYNC
uint32_t scenario_rand()
#else
static FILE *fp = nullptr;
static const char *realm = "LC";

uint32_t dbg_scenario_rand(const char *file, const char *function, const uint32_t line, const void *data)
#endif
{
    uint32_t originalSrand0 = gScenarioSrand0;
    gScenarioSrand0 += ror32(gScenarioSrand1 ^ 0x1234567F, 7);
    gScenarioSrand1 = ror32(originalSrand0, 3);

#ifdef DEBUG_DESYNC
    if (fp == nullptr)
    {
        if (network_get_mode() == NETWORK_MODE_SERVER)
        {
            fp = fopen("server_rand.txt", "wt");
            realm = "SV";
        }
        else if (network_get_mode() == NETWORK_MODE_CLIENT)
        {
            fp = fopen("client_rand.txt", "wt");
            realm = "CL";
        }
        else
        {
            if (fp)
                fclose(fp);
            fp = nullptr;
            realm = "LC";
        }
    }
    if (fp)
    {
        fprintf(fp, "Tick: %d, Rand: %08X - REF: %s:%u %s (%p)\n", gCurrentTicks, gScenarioSrand1, file, line, function, data);
    }
    if (!gInUpdateCode && !gInMapInitCode) {
        log_warning("scenario_rand called from outside game update");
        assert(false);
    }
#endif

    return gScenarioSrand1;
}

#ifdef DEBUG_DESYNC
void dbg_report_desync(uint32_t tick, uint32_t srand0, uint32_t server_srand0, const char *clientHash, const char *serverHash)
{
    if (fp == nullptr)
    {
        if (network_get_mode() == NETWORK_MODE_SERVER)
        {
            fp = fopen("server_rand.txt", "wt");
            realm = "SV";
        }
        else if (network_get_mode() == NETWORK_MODE_CLIENT)
        {
            fp = fopen("client_rand.txt", "wt");
            realm = "CL";
        }
    }
    if (fp)
    {
        const bool sprites_mismatch = serverHash[0] != '\0' && strcmp(clientHash, serverHash);

        fprintf(fp, "[%s] !! DESYNC !! Tick: %d, Client Hash: %s, Server Hash: %s, Client Rand: %08X, Server Rand: %08X - %s\n", realm,
                tick,
                clientHash,
                ( (serverHash[0] != '\0') ? serverHash : "<NONE:0>" ),
                srand0,
                server_srand0,
                (sprites_mismatch ? "Sprite hash mismatch" : "scenario rand mismatch"));
    }
}
#endif

uint32_t scenario_rand_max(uint32_t max)
{
    if (max < 2) return 0;
    if ((max & (max - 1)) == 0)
        return scenario_rand() & (max - 1);
    uint32_t rand, cap = ~((uint32_t)0) - (~((uint32_t)0) % max) - 1;
    do {
        rand = scenario_rand();
    } while (rand > cap);
    return rand % max;
}

/**
 * Prepare rides, for the finish five rollercoasters objective.
 *  rct2: 0x006788F7
 */
static bool scenario_prepare_rides_for_save()
{
    int32_t isFiveCoasterObjective = gScenarioObjectiveType == OBJECTIVE_FINISH_5_ROLLERCOASTERS;
    int32_t i;
    Ride * ride;
    uint8_t rcs = 0;

    FOR_ALL_RIDES(i, ride)
    {
        const rct_ride_entry * rideEntry = get_ride_entry(ride->subtype);
        
        // If there are more than 5 roller coasters, only mark the first five.
        if (isFiveCoasterObjective &&
            rideEntry != nullptr &&
            (ride_entry_has_category(rideEntry, RIDE_CATEGORY_ROLLERCOASTER) &&
             rcs < 5))
        {
            ride->lifecycle_flags |= RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK;
            rcs++;
        }
        else
        {
            ride->lifecycle_flags &= ~RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK;
        }
    }

    if (isFiveCoasterObjective && rcs < 5)
    {
        gGameCommandErrorText = STR_NOT_ENOUGH_ROLLER_COASTERS;
        return false;
    }

    bool markTrackAsIndestructible;
    tile_element_iterator it;
    tile_element_iterator_begin(&it);
    do
    {
        if (it.element->GetType() == TILE_ELEMENT_TYPE_TRACK)
        {
            markTrackAsIndestructible = false;

            if (isFiveCoasterObjective)
            {
                ride = get_ride(track_element_get_ride_index(it.element));

                // In the previous step, this flag was set on the first five roller coasters.
                if (ride != nullptr && ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)
                {
                    markTrackAsIndestructible = true;
                }
            }

            if (markTrackAsIndestructible)
            {
                it.element->flags |= TILE_ELEMENT_FLAG_INDESTRUCTIBLE_TRACK_PIECE;
            }
            else
            {
                it.element->flags &= ~TILE_ELEMENT_FLAG_INDESTRUCTIBLE_TRACK_PIECE;
            }
        }
    }
    while (tile_element_iterator_next(&it));

    return true;
}

/**
 *
 *  rct2: 0x006726C7
 */
bool scenario_prepare_for_save()
{
    gS6Info.entry.flags = 255;

    auto stex = (rct_stex_entry *)object_entry_get_chunk(OBJECT_TYPE_SCENARIO_TEXT, 0);
    if (stex != nullptr) {
        char buffer[256];
        format_string(buffer, 256, stex->scenario_name, nullptr);
        safe_strcpy(gS6Info.name, buffer, sizeof(gS6Info.name));

        memcpy(&gS6Info.entry, object_entry_get_entry(OBJECT_TYPE_SCENARIO_TEXT, 0), sizeof(rct_object_entry));
    }

    if (gS6Info.name[0] == 0)
        format_string(gS6Info.name, 64, gParkName, &gParkNameArgs);

    gS6Info.objective_type = gScenarioObjectiveType;
    gS6Info.objective_arg_1 = gScenarioObjectiveYear;
    gS6Info.objective_arg_2 = gScenarioObjectiveCurrency;
    gS6Info.objective_arg_3 = gScenarioObjectiveNumGuests;

    // This can return false if the goal is 'Finish 5 roller coaster' and there are too few.
    if (!scenario_prepare_rides_for_save())
    {
        return false;
    }

    if (gScenarioObjectiveType == OBJECTIVE_GUESTS_AND_RATING)
        gParkFlags |= PARK_FLAGS_PARK_OPEN;

    // Fix #2385: saved scenarios did not initialise temperatures to selected climate
    climate_reset(gClimate);

    return true;
}

/**
 * Modifies the given S6 data so that ghost elements, rides with no track elements or unused banners / user strings are saved.
 */
void scenario_fix_ghosts(rct_s6_data *s6)
{
    // Remove all ghost elements
    rct_tile_element *destinationElement = s6->tile_elements;

    for (int32_t y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; y++) {
        for (int32_t x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; x++) {
            rct_tile_element *originalElement = map_get_first_element_at(x, y);
            do {
                if (originalElement->flags & TILE_ELEMENT_FLAG_GHOST) {
                    BannerIndex bannerIndex = tile_element_get_banner_index(originalElement);
                    if (bannerIndex != BANNER_INDEX_NULL) {
                        rct_banner *banner = &s6->banners[bannerIndex];
                        if (banner->type != BANNER_NULL)
                        {
                            banner->type = BANNER_NULL;
                            if (is_user_string_id(banner->string_idx))
                                s6->custom_strings[(banner->string_idx % RCT12_MAX_USER_STRINGS)][0] = 0;
                        }
                    }
                } else {
                    *destinationElement++ = *originalElement;
                }
            } while (!(originalElement++)->IsLastForTile());

            // Set last element flag in case the original last element was never added
            (destinationElement - 1)->flags |= TILE_ELEMENT_FLAG_LAST_TILE;
        }
    }
}

void scenario_remove_trackless_rides(rct_s6_data *s6)
{
    bool rideHasTrack[MAX_RIDES];
    ride_all_has_any_track_elements(rideHasTrack);
    for (int32_t i = 0; i < RCT12_MAX_RIDES_IN_PARK; i++) {
        rct2_ride * ride = &s6->rides[i];

        if (rideHasTrack[i] || ride->type == RIDE_TYPE_NULL) {
            continue;
        }

        ride->type = RIDE_TYPE_NULL;
        if (is_user_string_id(ride->name))
        {
            s6->custom_strings[(ride->name % RCT12_MAX_USER_STRINGS)][0] = 0;
        }
    }
}

static void scenario_objective_check_guests_by()
{
    uint8_t objectiveYear = gScenarioObjectiveYear;
    int16_t parkRating = gParkRating;
    int16_t guestsInPark = gNumGuestsInPark;
    int16_t objectiveGuests = gScenarioObjectiveNumGuests;
    int16_t currentMonthYear = gDateMonthsElapsed;

    if (currentMonthYear == MONTH_COUNT * objectiveYear || gConfigGeneral.allow_early_completion) {
        if (parkRating >= 600 && guestsInPark >= objectiveGuests) {
            scenario_success();
        }
        else if (currentMonthYear == MONTH_COUNT * objectiveYear) {
            scenario_failure();
        }
    }
}

static void scenario_objective_check_park_value_by()
{
    uint8_t objectiveYear = gScenarioObjectiveYear;
    int16_t currentMonthYear = gDateMonthsElapsed;
    money32 objectiveParkValue = gScenarioObjectiveCurrency;
    money32 parkValue = gParkValue;

    if (currentMonthYear == MONTH_COUNT * objectiveYear || gConfigGeneral.allow_early_completion) {
        if (parkValue >= objectiveParkValue) {
            scenario_success();
        }
        else if (currentMonthYear == MONTH_COUNT * objectiveYear) {
            scenario_failure();
        }
    }
}

/**
* Checks if there are 10 rollercoasters of different subtype with
* excitement >= 600 .
* rct2:
**/
static void scenario_objective_check_10_rollercoasters()
{
    int32_t i, rcs = 0;
    uint8_t type_already_counted[256];
    Ride* ride;

    memset(type_already_counted, 0, 256);

    FOR_ALL_RIDES(i, ride) {
        uint8_t subtype_id = ride->subtype;
        rct_ride_entry *rideEntry = get_ride_entry(subtype_id);
        if (rideEntry == nullptr) {
            continue;
        }

        if (rideEntry != nullptr &&
            ride_entry_has_category(rideEntry, RIDE_CATEGORY_ROLLERCOASTER) &&
            ride->status == RIDE_STATUS_OPEN &&
            ride->excitement >= RIDE_RATING(6,00) && type_already_counted[subtype_id] == 0){
            type_already_counted[subtype_id]++;
            rcs++;
        }
    }

    if (rcs >= 10)
        scenario_success();
}

/**
 *
 *  rct2: 0x0066A13C
 */
static void scenario_objective_check_guests_and_rating()
{
    if (gParkRating < 700 && gDateMonthsElapsed >= 1) {
        gScenarioParkRatingWarningDays++;
        if (gScenarioParkRatingWarningDays == 1) {
            if (gConfigNotifications.park_rating_warnings) {
                news_item_add_to_queue(NEWS_ITEM_GRAPH, STR_PARK_RATING_WARNING_4_WEEKS_REMAINING, 0);
            }
        } else if (gScenarioParkRatingWarningDays == 8) {
            if (gConfigNotifications.park_rating_warnings) {
                news_item_add_to_queue(NEWS_ITEM_GRAPH, STR_PARK_RATING_WARNING_3_WEEKS_REMAINING, 0);
            }
        } else if (gScenarioParkRatingWarningDays == 15) {
            if (gConfigNotifications.park_rating_warnings) {
                news_item_add_to_queue(NEWS_ITEM_GRAPH, STR_PARK_RATING_WARNING_2_WEEKS_REMAINING, 0);
            }
        } else if (gScenarioParkRatingWarningDays == 22) {
            if (gConfigNotifications.park_rating_warnings) {
                news_item_add_to_queue(NEWS_ITEM_GRAPH, STR_PARK_RATING_WARNING_1_WEEK_REMAINING, 0);
            }
        } else if (gScenarioParkRatingWarningDays == 29) {
            news_item_add_to_queue(NEWS_ITEM_GRAPH, STR_PARK_HAS_BEEN_CLOSED_DOWN, 0);
            gParkFlags &= ~PARK_FLAGS_PARK_OPEN;
            scenario_failure();
            gGuestInitialHappiness = 50;
        }
    } else if (gScenarioCompletedCompanyValue != (money32)0x80000001) {
        gScenarioParkRatingWarningDays = 0;
    }

    if (gParkRating >= 700)
        if (gNumGuestsInPark >= gScenarioObjectiveNumGuests)
            scenario_success();
}

static void scenario_objective_check_monthly_ride_income()
{
    money32 lastMonthRideIncome = gExpenditureTable[1][RCT_EXPENDITURE_TYPE_PARK_RIDE_TICKETS];
    if (lastMonthRideIncome >= gScenarioObjectiveCurrency) {
        scenario_success();
    }
}

/**
 * Checks if there are 10 rollercoasters of different subtype with
 * excitement > 700 and a minimum length;
 *  rct2: 0x0066A6B5
 */
static void scenario_objective_check_10_rollercoasters_length()
{
    int32_t i, rcs = 0;
    uint8_t type_already_counted[256];
    int16_t objective_length = gScenarioObjectiveNumGuests;
    Ride* ride;

    memset(type_already_counted, 0, 256);

    FOR_ALL_RIDES(i, ride) {
        uint8_t subtype_id = ride->subtype;
        rct_ride_entry *rideEntry = get_ride_entry(subtype_id);
        if (rideEntry == nullptr) {
            continue;
        }
        if (ride_entry_has_category(rideEntry, RIDE_CATEGORY_ROLLERCOASTER) &&
            ride->status == RIDE_STATUS_OPEN &&
            ride->excitement >= RIDE_RATING(7,00) && type_already_counted[subtype_id] == 0){

            if ((ride_get_total_length(ride) >> 16) > objective_length) {
                type_already_counted[subtype_id]++;
                rcs++;
            }
        }
    }

    if (rcs >= 10)
        scenario_success();
}

static void scenario_objective_check_finish_5_rollercoasters()
{
    money32 objectiveRideExcitement = gScenarioObjectiveCurrency;

    // Originally, this did not check for null rides, neither did it check if
    // the rides are even rollercoasters, never mind the right rollercoasters to be finished.
    int32_t i;
    Ride * ride;
    int32_t rcs = 0;
    FOR_ALL_RIDES(i, ride)
    {
        const rct_ride_entry * rideEntry = get_ride_entry(ride->subtype);
        if (rideEntry == nullptr)
        {
            continue;
        }

        if (ride->status != RIDE_STATUS_CLOSED &&
            ride->excitement >= objectiveRideExcitement &&
            (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK) && // Set on partially finished coasters
            ride_entry_has_category(rideEntry, RIDE_CATEGORY_ROLLERCOASTER))
            rcs++;
    }

    if (rcs >= 5)
        scenario_success();
}

static void scenario_objective_check_replay_loan_and_park_value()
{
    money32 objectiveParkValue = gScenarioObjectiveCurrency;
    money32 parkValue = gParkValue;
    money32 currentLoan = gBankLoan;

    if (currentLoan <= 0 && parkValue >= objectiveParkValue)
        scenario_success();
}

static void scenario_objective_check_monthly_food_income()
{
    money32 * lastMonthExpenditure = gExpenditureTable[1];
    int32_t lastMonthProfit =
       lastMonthExpenditure[RCT_EXPENDITURE_TYPE_SHOP_SHOP_SALES] +
       lastMonthExpenditure[RCT_EXPENDITURE_TYPE_SHOP_STOCK] +
       lastMonthExpenditure[RCT_EXPENDITURE_TYPE_FOODDRINK_SALES] +
       lastMonthExpenditure[RCT_EXPENDITURE_TYPE_FOODDRINK_STOCK];

    if (lastMonthProfit >= gScenarioObjectiveCurrency) {
        scenario_success();
    }
}

/**
 * Checks the win/lose conditions of the current objective.
 *  rct2: 0x0066A4B2
 */
static void scenario_objective_check()
{
    if (gScenarioCompletedCompanyValue != MONEY32_UNDEFINED) {
        return;
    }

    switch (gScenarioObjectiveType) {
    case OBJECTIVE_GUESTS_BY:
        scenario_objective_check_guests_by();
        break;
    case OBJECTIVE_PARK_VALUE_BY:
        scenario_objective_check_park_value_by();
        break;
    case OBJECTIVE_10_ROLLERCOASTERS:
        scenario_objective_check_10_rollercoasters();
        break;
    case OBJECTIVE_GUESTS_AND_RATING:
        scenario_objective_check_guests_and_rating();
        break;
    case OBJECTIVE_MONTHLY_RIDE_INCOME:
        scenario_objective_check_monthly_ride_income();
        break;
    case OBJECTIVE_10_ROLLERCOASTERS_LENGTH:
        scenario_objective_check_10_rollercoasters_length();
        break;
    case OBJECTIVE_FINISH_5_ROLLERCOASTERS:
        scenario_objective_check_finish_5_rollercoasters();
        break;
    case OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE:
        scenario_objective_check_replay_loan_and_park_value();
        break;
    case OBJECTIVE_MONTHLY_FOOD_INCOME:
        scenario_objective_check_monthly_food_income();
        break;
    }
}

