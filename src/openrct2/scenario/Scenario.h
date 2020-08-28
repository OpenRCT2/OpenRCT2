/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _SCENARIO_H_
#define _SCENARIO_H_

#include "../common.h"
#include "../core/Random.hpp"
#include "../management/Finance.h"
#include "../management/Research.h"
#include "../object/Object.h"
#include "../rct12/RCT12.h"
#include "../rct2/RCT2.h"
#include "../ride/Ride.h"
#include "../ride/RideRatings.h"
#include "../world/Banner.h"
#include "../world/Climate.h"
#include "../world/EntityList.h"
#include "../world/Map.h"
#include "../world/MapAnimation.h"

using random_engine_t = Random::Rct2::Engine;

enum class EditorStep : uint8_t;

struct ParkLoadResult;

#pragma pack(push, 1)
/**
 * SV6/SC6 header chunk
 * size: 0x20
 */
struct rct_s6_header
{
    uint8_t type;                // 0x00
    uint8_t classic_flag;        // 0x01
    uint16_t num_packed_objects; // 0x02
    uint32_t version;            // 0x04
    uint32_t magic_number;       // 0x08
    uint8_t pad_0C[0x14];
};
assert_struct_size(rct_s6_header, 0x20);

/**
 * SC6 information chunk
 * size: 0x198
 */
struct rct_s6_info
{
    EditorStep editor_step;
    uint8_t category;        // 0x01
    uint8_t objective_type;  // 0x02
    uint8_t objective_arg_1; // 0x03
    int32_t objective_arg_2; // 0x04
    int16_t objective_arg_3; // 0x08
    uint8_t pad_00A[0x3E];
    char name[64];          // 0x48
    char details[256];      // 0x88
    rct_object_entry entry; // 0x188
};
assert_struct_size(rct_s6_info, 0x198);

enum class ScenarioSource : uint8_t
{
    RCT1,
    RCT1_AA,
    RCT1_LL,
    RCT2,
    RCT2_WW,
    RCT2_TT,
    Real,
    Other
};

struct rct_stex_entry
{
    rct_string_id scenario_name; // 0x00
    rct_string_id park_name;     // 0x02
    rct_string_id details;       // 0x04
    uint8_t var_06;
};
assert_struct_size(rct_stex_entry, 7);

// This will be useful for backwards compatibility
struct rct_s6_data
{
    // SC6[0]
    rct_s6_header header;

    // SC6[1]
    rct_s6_info info;

    // SC6[2]
    // packed objects

    // SC6[3]
    rct_object_entry objects[RCT2_OBJECT_ENTRY_COUNT];

    // SC6[4]
    uint16_t elapsed_months;
    uint16_t current_day;
    uint32_t scenario_ticks;
    uint32_t scenario_srand_0;
    uint32_t scenario_srand_1;

    // SC6[5]
    RCT12TileElement tile_elements[RCT2_MAX_TILE_ELEMENTS];

    // SC6[6]
    uint32_t next_free_tile_element_pointer_index;
    RCT2Sprite sprites[RCT2_MAX_SPRITES];
    uint16_t sprite_lists_head[static_cast<uint8_t>(EntityListId::Count)];
    uint16_t sprite_lists_count[static_cast<uint8_t>(EntityListId::Count)];
    rct_string_id park_name;
    uint8_t pad_013573D6[2];
    uint32_t park_name_args;
    money32 initial_cash;
    money32 current_loan;
    uint32_t park_flags;
    money16 park_entrance_fee;
    uint16_t rct1_park_entrance_x;
    uint16_t rct1_park_entrance_y;
    uint8_t pad_013573EE[2];
    uint8_t rct1_park_entrance_z;
    uint8_t pad_013573F1;
    rct12_peep_spawn peep_spawns[RCT12_MAX_PEEP_SPAWNS];
    uint8_t guest_count_change_modifier;
    uint8_t current_research_level;
    uint8_t pad_01357400[4];
    uint32_t researched_ride_types[RCT2_MAX_RESEARCHED_RIDE_TYPE_QUADS];
    uint32_t researched_ride_entries[RCT2_MAX_RESEARCHED_RIDE_ENTRY_QUADS];
    uint32_t researched_track_types_a[128];
    uint32_t researched_track_types_b[128];

    // SC6[7]
    uint16_t guests_in_park;
    uint16_t guests_heading_for_park;

    // Ignored in scenario
    money32 expenditure_table[RCT12_EXPENDITURE_TABLE_MONTH_COUNT][RCT12_EXPENDITURE_TYPE_COUNT];

    // SC6[8]
    uint16_t last_guests_in_park;
    uint8_t pad_01357BCA[3];
    uint8_t handyman_colour;
    uint8_t mechanic_colour;
    uint8_t security_colour;

    // Ignored in scenario
    uint32_t researched_scenery_items[RCT2_MAX_RESEARCHED_SCENERY_ITEM_QUADS];

    // SC6[9]
    uint16_t park_rating;

    // Ignored in scenario
    uint8_t park_rating_history[32];
    uint8_t guests_in_park_history[32];

    // SC6[10]
    uint8_t active_research_types;
    uint8_t research_progress_stage;
    uint32_t last_researched_item_subject;
    uint8_t pad_01357CF8[1000];
    uint32_t next_research_item;
    uint16_t research_progress;
    uint8_t next_research_category;
    uint8_t next_research_expected_day;
    uint8_t next_research_expected_month;
    uint8_t guest_initial_happiness;
    uint16_t park_size;
    uint16_t guest_generation_probability;
    uint16_t total_ride_value_for_money;
    money32 maximum_loan;
    money16 guest_initial_cash;
    uint8_t guest_initial_hunger;
    uint8_t guest_initial_thirst;
    uint8_t objective_type;
    uint8_t objective_year;
    uint8_t pad_013580FA[2];
    money32 objective_currency;
    uint16_t objective_guests;
    uint8_t campaign_weeks_left[20];
    uint8_t campaign_ride_index[22];

    // Ignored in scenario
    money32 balance_history[RCT12_FINANCE_GRAPH_SIZE];

    // SC6[11]
    money32 current_expenditure;
    money32 current_profit;
    uint32_t weekly_profit_average_dividend;
    uint16_t weekly_profit_average_divisor;
    uint8_t pad_0135833A[2];

    // Ignored in scenario
    money32 weekly_profit_history[RCT12_FINANCE_GRAPH_SIZE];

    // SC6[12]
    money32 park_value;

    // Ignored in scenario
    money32 park_value_history[RCT12_FINANCE_GRAPH_SIZE];

    // SC6[13]
    money32 completed_company_value;
    uint32_t total_admissions;
    money32 income_from_admissions;
    money32 company_value;
    uint8_t peep_warning_throttle[16];
    rct12_award awards[RCT12_MAX_AWARDS];
    money16 land_price;
    money16 construction_rights_price;
    uint16_t word_01358774;
    uint8_t pad_01358776[2];
    uint32_t cd_key;
    uint8_t pad_0135877C[64];
    uint32_t game_version_number;
    money32 completed_company_value_record;
    uint32_t loan_hash;
    uint16_t ride_count;
    uint8_t pad_013587CA[6];
    money32 historical_profit;
    uint8_t pad_013587D4[4];
    char scenario_completed_name[32];
    money32 cash;
    uint8_t pad_013587FC[50];
    uint16_t park_rating_casualty_penalty;
    uint16_t map_size_units;
    uint16_t map_size_minus_2;
    uint16_t map_size;
    uint16_t map_max_xy;
    uint32_t same_price_throughout;
    uint16_t suggested_max_guests;
    uint16_t park_rating_warning_days;
    uint8_t last_entrance_style;
    uint8_t rct1_water_colour;
    uint8_t pad_01358842[2];
    RCT12ResearchItem research_items[MAX_RESEARCH_ITEMS];
    uint16_t map_base_z;
    char scenario_name[64];
    char scenario_description[256];
    uint8_t current_interest_rate;
    uint8_t pad_0135934B;
    uint32_t same_price_throughout_extended;
    int16_t park_entrance_x[RCT12_MAX_PARK_ENTRANCES];
    int16_t park_entrance_y[RCT12_MAX_PARK_ENTRANCES];
    int16_t park_entrance_z[RCT12_MAX_PARK_ENTRANCES];
    uint8_t park_entrance_direction[RCT12_MAX_PARK_ENTRANCES];
    char scenario_filename[256];
    uint8_t saved_expansion_pack_names[3256];
    RCT12Banner banners[RCT2_MAX_BANNERS_IN_PARK];
    char custom_strings[RCT12_MAX_USER_STRINGS][RCT12_USER_STRING_MAX_LENGTH];
    uint32_t game_ticks_1;
    rct2_ride rides[RCT12_MAX_RIDES_IN_PARK];
    uint16_t saved_age;
    int16_t saved_view_x;
    int16_t saved_view_y;
    uint8_t saved_view_zoom;
    uint8_t saved_view_rotation;
    RCT12MapAnimation map_animations[RCT2_MAX_ANIMATED_OBJECTS];
    uint16_t num_map_animations;
    uint8_t pad_0138B582[2];
    RCT2RideRatingCalculationData ride_ratings_calc_data;
    uint8_t pad_0138B5D0[60];
    RCT12RideMeasurement ride_measurements[8];
    uint32_t next_guest_index;
    uint16_t grass_and_scenery_tilepos;
    uint32_t patrol_areas[(RCT2_MAX_STAFF + RCT12_STAFF_TYPE_COUNT) * RCT12_PATROL_AREA_SIZE];
    uint8_t staff_modes[RCT2_MAX_STAFF + RCT12_STAFF_TYPE_COUNT];
    uint8_t pad_13CA73E;
    uint8_t pad_13CA73F;
    uint8_t byte_13CA740;
    uint8_t pad_13CA741;
    uint8_t byte_13CA742[4]; // unused
    uint8_t climate;
    uint8_t pad_013CA747;
    uint16_t climate_update_timer;
    uint8_t current_weather;
    uint8_t next_weather;
    uint8_t temperature;
    uint8_t next_temperature;
    uint8_t current_weather_effect;
    uint8_t next_weather_effect;
    uint8_t current_weather_gloom;
    uint8_t next_weather_gloom;
    uint8_t current_weather_level;
    uint8_t next_weather_level;
    rct12_news_item news_items[RCT12_MAX_NEWS_ITEMS];
    char rct1_scenario_name[62];       // Unused in RCT2
    uint16_t rct1_scenario_slot_index; // Unused in RCT2
    uint32_t rct1_scenario_flags;      // Unused in RCT2
    uint16_t wide_path_tile_loop_x;
    uint16_t wide_path_tile_loop_y;
    uint8_t pad_13CE778[434];
};
assert_struct_size(rct_s6_data, 0x46b44a);
#pragma pack(pop)

enum
{
    SCENARIO_FLAGS_VISIBLE = (1 << 0),
    SCENARIO_FLAGS_COMPLETED = (1 << 1),
    SCENARIO_FLAGS_SIXFLAGS = (1 << 2)
};

enum
{
    S6_TYPE_SAVEDGAME,
    S6_TYPE_SCENARIO
};

#define S6_RCT2_VERSION 120001
#define S6_MAGIC_NUMBER 0x00031144

enum SCENARIO_CATEGORY
{
    // RCT2 categories (keep order)
    SCENARIO_CATEGORY_BEGINNER,
    SCENARIO_CATEGORY_CHALLENGING,
    SCENARIO_CATEGORY_EXPERT,
    SCENARIO_CATEGORY_REAL,
    SCENARIO_CATEGORY_OTHER,

    // OpenRCT2 categories
    SCENARIO_CATEGORY_DLC,
    SCENARIO_CATEGORY_BUILD_YOUR_OWN,

    SCENARIO_CATEGORY_COUNT
};

enum
{
    OBJECTIVE_NONE,
    OBJECTIVE_GUESTS_BY,
    OBJECTIVE_PARK_VALUE_BY,
    OBJECTIVE_HAVE_FUN,
    OBJECTIVE_BUILD_THE_BEST,
    OBJECTIVE_10_ROLLERCOASTERS,
    OBJECTIVE_GUESTS_AND_RATING,
    OBJECTIVE_MONTHLY_RIDE_INCOME,
    OBJECTIVE_10_ROLLERCOASTERS_LENGTH,
    OBJECTIVE_FINISH_5_ROLLERCOASTERS,
    OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE,
    OBJECTIVE_MONTHLY_FOOD_INCOME,

    OBJECTIVE_COUNT
};

bool ObjectiveNeedsMoney(const uint8_t objective);

enum class ObjectiveStatus : uint8_t
{
    Undecided,
    Success,
    Failure,
};

struct Objective
{
    uint8_t Type;
    uint8_t Year;
    union
    {
        uint16_t NumGuests;
        rct_string_id RideId;
        uint16_t MinimumLength; // For the "Build 10 coasters of minimum length" objective.
    };
    union
    {
        money32 Currency;
        uint16_t MinimumExcitement; // For the "Finish 5 coaster with a minimum excitement rating" objective.
    };

    bool NeedsMoney() const
    {
        return ObjectiveNeedsMoney(Type);
    }

    bool IsValid(bool useMoney, bool canAskMoneyForRides) const
    {
        const bool objectiveAllowedByMoneyUsage = useMoney || !NeedsMoney();
        // This objective can only work if the player can ask money for rides.
        const bool objectiveAllowedByPaymentSettings = (Type != OBJECTIVE_MONTHLY_RIDE_INCOME) || canAskMoneyForRides;
        return objectiveAllowedByMoneyUsage && objectiveAllowedByPaymentSettings;
    }

    ObjectiveStatus Check() const;

private:
    ObjectiveStatus CheckGuestsBy() const;
    ObjectiveStatus CheckParkValueBy() const;
    ObjectiveStatus Check10RollerCoasters() const;
    ObjectiveStatus CheckGuestsAndRating() const;
    ObjectiveStatus CheckMonthlyRideIncome() const;
    ObjectiveStatus Check10RollerCoastersLength() const;
    ObjectiveStatus CheckFinish5RollerCoasters() const;
    ObjectiveStatus CheckRepayLoanAndParkValue() const;
    ObjectiveStatus CheckMonthlyFoodIncome() const;
};

enum
{
    SCENARIO_SELECT_MODE_DIFFICULTY,
    SCENARIO_SELECT_MODE_ORIGIN,
};

enum
{
    AUTOSAVE_EVERY_MINUTE,
    AUTOSAVE_EVERY_5MINUTES,
    AUTOSAVE_EVERY_15MINUTES,
    AUTOSAVE_EVERY_30MINUTES,
    AUTOSAVE_EVERY_HOUR,
    AUTOSAVE_NEVER
};

#define AUTOSAVE_PAUSE 0
#define DEFAULT_NUM_AUTOSAVES_TO_KEEP 10

static constexpr money32 COMPANY_VALUE_ON_FAILED_OBJECTIVE = 0x80000001;

extern const rct_string_id ScenarioCategoryStringIds[SCENARIO_CATEGORY_COUNT];

extern uint32_t gScenarioTicks;
extern random_engine_t gScenarioRand;

extern Objective gScenarioObjective;
extern bool gAllowEarlyCompletionInNetworkPlay;
extern uint16_t gScenarioParkRatingWarningDays;
extern money32 gScenarioCompletedCompanyValue;
extern money32 gScenarioCompanyValueRecord;

extern rct_s6_info gS6Info;
extern std::string gScenarioName;
extern std::string gScenarioDetails;
extern std::string gScenarioCompletedBy;
extern std::string gScenarioSavePath;
extern bool gFirstTimeSaving;
extern uint16_t gSavedAge;
extern uint32_t gLastAutoSaveUpdate;

extern char gScenarioFileName[260];

void load_from_sc6(const char* path);
void scenario_begin();
void scenario_update();
bool scenario_create_ducks();
bool AllowEarlyCompletion();

const random_engine_t::state_type& scenario_rand_state();
void scenario_rand_seed(random_engine_t::result_type s0, random_engine_t::result_type s1);
random_engine_t::result_type scenario_rand();
uint32_t scenario_rand_max(uint32_t max);

bool scenario_prepare_for_save();
int32_t scenario_save(const utf8* path, int32_t flags);
void scenario_remove_trackless_rides(rct_s6_data* s6);
void scenario_fix_ghosts(rct_s6_data* s6);
void scenario_failure();
void scenario_success();
void scenario_success_submit_name(const char* name);
void scenario_autosave_check();

#endif
