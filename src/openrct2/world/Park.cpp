/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Cheats.h"
#include "../Context.h"
#include "../config/Config.h"
#include "../core/Math.hpp"
#include "../core/Memory.hpp"
#include "../core/Util.hpp"
#include "../Date.h"
#include "../Game.h"
#include "../GameState.h"
#include "../interface/Colour.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../management/Award.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../management/Research.h"
#include "../network/network.h"
#include "../OpenRCT2.h"
#include "../peep/Peep.h"
#include "../peep/Staff.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/ShopItem.h"
#include "../scenario/Scenario.h"
#include "../windows/Intent.h"
#include "Entrance.h"
#include "Map.h"
#include "Park.h"
#include "Sprite.h"
#include "Surface.h"

using namespace OpenRCT2;

rct_string_id gParkName;
uint32_t gParkNameArgs;
uint32_t gParkFlags;
uint16_t gParkRating;
money16 gParkEntranceFee;
uint16_t gParkSize;
money16 gLandPrice;
money16 gConstructionRightsPrice;

uint32_t gTotalAdmissions;
money32 gTotalIncomeFromAdmissions;

money32 gParkValue;
money32 gCompanyValue;

int16_t gParkRatingCasualtyPenalty;
uint8_t gParkRatingHistory[32];
uint8_t gGuestsInParkHistory[32];

// If this value is more than or equal to 0, the park rating is forced to this value. Used for cheat
static int32_t _forcedParkRating = -1;

/**
 * In a difficult guest generation scenario, no guests will be generated if over this value.
 */
int32_t _suggestedGuestMaximum;

/**
 * Probability out of 65535, of gaining a new guest per game tick.
 * new guests per second = 40 * (probability / 65535)
 * With a full park rating, non-overpriced entrance fee, less guests than the suggested maximum and four positive awards,
 * approximately 1 guest per second can be generated (+60 guests in one minute).
 */
int32_t _guestGenerationProbability;

/**
 *
 *  rct2: 0x00667104
 */
void reset_park_entry()
{
    gParkName = 0;
    reset_park_entrance();
    for (int32_t i = 0; i < MAX_PEEP_SPAWNS; i++) {
        gPeepSpawns[i].x = PEEP_SPAWN_UNDEFINED;
    }
}

/**
 * Choose a random peep spawn and iterates through until defined spawn is found.
 */
static uint32_t get_random_peep_spawn_index()
{
    uint32_t spawnIndexList[MAX_PEEP_SPAWNS];
    uint32_t numSpawns = map_get_available_peep_spawn_index_list(spawnIndexList);
    if (numSpawns > 0) {
        return spawnIndexList[scenario_rand() % numSpawns];
    }
    else {
        return 0;
    }
}

void park_set_open(int32_t open)
{
    game_do_command(0, GAME_COMMAND_FLAG_APPLY, 0, open << 8, GAME_COMMAND_SET_PARK_OPEN, 0, 0);
}

/**
 *
 *  rct2: 0x00669D4A
 */
void game_command_set_park_open(
    [[maybe_unused]] int32_t * eax,
    int32_t *                  ebx,
    [[maybe_unused]] int32_t * ecx,
    int32_t *                  edx,
    [[maybe_unused]] int32_t * esi,
    int32_t *                  edi,
    [[maybe_unused]] int32_t * ebp)
{
    if (!(*ebx & GAME_COMMAND_FLAG_APPLY)) {
        *ebx = 0;
        return;
    }

    int32_t dh = (*edx >> 8) & 0xFF;

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_PARK_ENTRANCE_TICKETS;
    switch (dh) {
    case 0:
        if (gParkFlags & PARK_FLAGS_PARK_OPEN) {
            gParkFlags &= ~PARK_FLAGS_PARK_OPEN;
            window_invalidate_by_class(WC_PARK_INFORMATION);
        }
        break;
    case 1:
        if (!(gParkFlags & PARK_FLAGS_PARK_OPEN)) {
            gParkFlags |= PARK_FLAGS_PARK_OPEN;
            window_invalidate_by_class(WC_PARK_INFORMATION);
        }
        break;
    case 2:
        gSamePriceThroughoutParkA = *edi;
        window_invalidate_by_class(WC_RIDE);
        break;
    case 3:
        gSamePriceThroughoutParkB = *edi;
        window_invalidate_by_class(WC_RIDE);
        break;
    }

    *ebx = 0;
}

/**
*
*  rct2: 0x00664D05
*/
void update_park_fences(const CoordsXY coords)
{
    if (map_is_edge(coords))
        return;

    rct_tile_element* surfaceElement = map_get_surface_element_at(coords);
    if (surfaceElement == nullptr)
        return;

    uint8_t newOwnership = surfaceElement->properties.surface.ownership & 0xF0;
    if ((surfaceElement->properties.surface.ownership & OWNERSHIP_OWNED) == 0) {
        bool fenceRequired = true;

        rct_tile_element* tileElement = map_get_first_element_at(coords.x / 32, coords.y / 32);
        // If an entrance element do not place flags around surface
        do {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
                continue;

            if (tileElement->properties.entrance.type != ENTRANCE_TYPE_PARK_ENTRANCE)
                continue;

            if (!(tileElement->flags & TILE_ELEMENT_FLAG_GHOST)) {
                fenceRequired = false;
                break;
            }
        } while (!(tileElement++)->IsLastForTile());

        if (fenceRequired) {
            // As map_is_location_in_park sets the error text
            // will require to back it up.
            rct_string_id previous_error = gGameCommandErrorText;
            if (map_is_location_in_park({coords.x - 32, coords.y})){
                newOwnership |= 0x8;
            }

            if (map_is_location_in_park({coords.x, coords.y - 32})){
                newOwnership |= 0x4;
            }

            if (map_is_location_in_park({coords.x + 32, coords.y})){
                newOwnership |= 0x2;
            }

            if (map_is_location_in_park({coords.x, coords.y + 32})){
                newOwnership |= 0x1;
            }

            gGameCommandErrorText = previous_error;
        }
    }

    if (surfaceElement->properties.surface.ownership != newOwnership) {
        int32_t z0 = surfaceElement->base_height * 8;
        int32_t z1 = z0 + 16;
        map_invalidate_tile(coords.x, coords.y, z0, z1);
        surfaceElement->properties.surface.ownership = newOwnership;
    }
}

void update_park_fences_around_tile(const CoordsXY coords)
{
    update_park_fences(coords);
    update_park_fences({coords.x + 32, coords.y});
    update_park_fences({coords.x - 32, coords.y});
    update_park_fences({coords.x, coords.y + 32});
    update_park_fences({coords.x, coords.y - 32});
}

void park_set_name(const char *name)
{
    auto nameId = user_string_allocate(USER_STRING_HIGH_ID_NUMBER, name);
    if (nameId != 0)
    {
        user_string_free(gParkName);
        gParkName = nameId;
    }
}

static money32 map_buy_land_rights_for_tile(int32_t x, int32_t y, int32_t setting, int32_t flags) {
    rct_tile_element* surfaceElement = map_get_surface_element_at({x, y});
    if (surfaceElement == nullptr)
        return MONEY32_UNDEFINED;

    switch (setting) {
    case BUY_LAND_RIGHTS_FLAG_BUY_LAND: // 0
        if ((surfaceElement->properties.surface.ownership & OWNERSHIP_OWNED) != 0) { // If the land is already owned
            return 0;
        }

        if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) != 0 || (surfaceElement->properties.surface.ownership & OWNERSHIP_AVAILABLE) == 0) {
            gGameCommandErrorText = STR_LAND_NOT_FOR_SALE;
            return MONEY32_UNDEFINED;
        }
        if (flags & GAME_COMMAND_FLAG_APPLY) {
            surfaceElement->properties.surface.ownership |= OWNERSHIP_OWNED;
            update_park_fences_around_tile({x, y});
        }
        return gLandPrice;
    case BUY_LAND_RIGHTS_FLAG_UNOWN_TILE: // 1
        if (flags & GAME_COMMAND_FLAG_APPLY) {
            surfaceElement->properties.surface.ownership &= ~(OWNERSHIP_OWNED | OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED);
            update_park_fences_around_tile({x, y});
        }
        return 0;
    case BUY_LAND_RIGHTS_FLAG_BUY_CONSTRUCTION_RIGHTS: // 2
        if ((surfaceElement->properties.surface.ownership & (OWNERSHIP_OWNED | OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED)) != 0) { // If the land or construction rights are already owned
            return 0;
        }

        if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) != 0 || (surfaceElement->properties.surface.ownership & OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE) == 0) {
            gGameCommandErrorText = STR_CONSTRUCTION_RIGHTS_NOT_FOR_SALE;
            return MONEY32_UNDEFINED;
        }

        if (flags & GAME_COMMAND_FLAG_APPLY) {
            surfaceElement->properties.surface.ownership |= OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED;
            uint16_t baseHeight = surfaceElement->base_height * 8;
            map_invalidate_tile(x, y, baseHeight, baseHeight + 16);
        }
        return gConstructionRightsPrice;
    case BUY_LAND_RIGHTS_FLAG_UNOWN_CONSTRUCTION_RIGHTS: // 3
        if (flags & GAME_COMMAND_FLAG_APPLY) {
            surfaceElement->properties.surface.ownership &= ~OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED;
            uint16_t baseHeight = surfaceElement->base_height * 8;
            map_invalidate_tile(x, y, baseHeight, baseHeight + 16);
        }
        return 0;
    case BUY_LAND_RIGHTS_FLAG_SET_FOR_SALE: // 4
        if (flags & GAME_COMMAND_FLAG_APPLY) {
            surfaceElement->properties.surface.ownership |= OWNERSHIP_AVAILABLE;
            uint16_t baseHeight = surfaceElement->base_height * 8;
            map_invalidate_tile(x, y, baseHeight, baseHeight + 16);
        }
        return 0;
    case BUY_LAND_RIGHTS_FLAG_SET_CONSTRUCTION_RIGHTS_FOR_SALE: // 5
        if (flags & GAME_COMMAND_FLAG_APPLY) {
            surfaceElement->properties.surface.ownership |= OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE;
            uint16_t baseHeight = surfaceElement->base_height * 8;
            map_invalidate_tile(x, y, baseHeight, baseHeight + 16);
        }
        return 0;
    case BUY_LAND_RIGHTS_FLAG_SET_OWNERSHIP_WITH_CHECKS:
        {
            if (!(gScreenFlags & SCREEN_FLAGS_EDITOR) && !gCheatsSandboxMode) {
                return MONEY32_UNDEFINED;
            }

            if (x <= 0 || y <= 0) {
                gGameCommandErrorText = STR_TOO_CLOSE_TO_EDGE_OF_MAP;
                return MONEY32_UNDEFINED;
            }

            if (x >= gMapSizeUnits || y >= gMapSizeUnits) {
                gGameCommandErrorText = STR_TOO_CLOSE_TO_EDGE_OF_MAP;
                return MONEY32_UNDEFINED;
            }

            uint8_t newOwnership = (flags & 0xFF00) >> 4;
            if (newOwnership == (surfaceElement->properties.surface.ownership & 0xF0)) {
                return 0;
            }

            rct_tile_element* tileElement = map_get_first_element_at(x / 32, y / 32);
            do {
                if (tileElement->GetType() == TILE_ELEMENT_TYPE_ENTRANCE) {
                    // Do not allow ownership of park entrance.
                    if (newOwnership == OWNERSHIP_OWNED || newOwnership == OWNERSHIP_AVAILABLE)
                        return 0;
                    // Allow construction rights available / for sale on park entrances on surface.
                    // There is no need to check the height if newOwnership is 0 (unowned and no rights available).
                    if ((newOwnership == OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED ||
                         newOwnership == OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE) &&
                        (tileElement->base_height - 3 > surfaceElement->base_height ||
                         tileElement->base_height < surfaceElement->base_height))
                        return 0;
                }
            } while (!(tileElement++)->IsLastForTile());

            if (!(flags & GAME_COMMAND_FLAG_APPLY)) {
                return gLandPrice;
            }

            if ((newOwnership & 0xF0) != 0) {
                PeepSpawn *peepSpawns = gPeepSpawns;

                for (uint8_t i = 0; i < MAX_PEEP_SPAWNS; ++i) {
                    if (x == (peepSpawns[i].x & 0xFFE0)) {
                        if (y == (peepSpawns[i].y & 0xFFE0)) {
                            peepSpawns[i].x = PEEP_SPAWN_UNDEFINED;
                        }
                    }
                }
            }
            surfaceElement->properties.surface.ownership &= 0x0F;
            surfaceElement->properties.surface.ownership |= newOwnership;
            update_park_fences_around_tile({x, y});
            gMapLandRightsUpdateSuccess = true;
            return 0;
        }
    default:
        log_warning("Tried calling map_buy_land_rights_for_tile() with an incorrect setting!");
        assert(false);
        return MONEY32_UNDEFINED;
    }
}

int32_t map_buy_land_rights(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t setting, int32_t flags)
{
    int32_t x, y, z;
    money32 totalCost, cost;
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LAND_PURCHASE;

    if (x1 == 0 && y1 == 0) {
        x1 = x0;
        y1 = y0;
    }

    x = (x0 + x1) / 2 + 16;
    y = (y0 + y1) / 2 + 16;
    z = tile_element_height(x, y);
    gCommandPosition.x = x;
    gCommandPosition.y = y;
    gCommandPosition.z = z;

    // Game command modified to accept selection size
    totalCost = 0;
    gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
    if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) != 0 || game_is_not_paused() || gCheatsBuildInPauseMode) {
        for (y = y0; y <= y1; y += 32) {
            for (x = x0; x <= x1; x += 32) {
                cost = map_buy_land_rights_for_tile(x, y, setting, flags);
                if (cost != MONEY32_UNDEFINED)
                {
                    totalCost += cost;
                }
            }
        }
    }

    return totalCost;
}

/**
*
*  rct2: 0x006649BD
*/
void game_command_buy_land_rights(
    int32_t * eax, int32_t * ebx, int32_t * ecx, int32_t * edx, [[maybe_unused]] int32_t * esi, int32_t * edi, int32_t * ebp)
{
    int32_t flags = *ebx & 0xFFFF;

    *ebx = map_buy_land_rights(
        (*eax & 0xFFFF),
        (*ecx & 0xFFFF),
        (*edi & 0xFFFF),
        (*ebp & 0xFFFF),
        (*edx & 0x00FF),
        flags
    );

    // Too expensive to always call in map_buy_land_rights.
    // It's already counted when the park is loaded, after
    // that it should only be called for user actions.
    if (flags & GAME_COMMAND_FLAG_APPLY) {
        map_count_remaining_land_rights();
    }
}

void set_forced_park_rating(int32_t rating)
{
    _forcedParkRating = rating;
    auto& park = GetContext()->GetGameState()->GetPark();
    gParkRating = park.CalculateParkRating();
    auto intent = Intent(INTENT_ACTION_UPDATE_PARK_RATING);
    context_broadcast_intent(&intent);
}

int32_t get_forced_park_rating()
{
    return _forcedParkRating;
}

money16 park_get_entrance_fee()
{
    if (gParkFlags & PARK_FLAGS_NO_MONEY)
    {
        return 0;
    }
    if (!park_entry_price_unlocked())
    {
        return 0;
    }
    return gParkEntranceFee;
}

bool park_ride_prices_unlocked()
{
    if (gParkFlags & PARK_FLAGS_UNLOCK_ALL_PRICES)
    {
        return true;
    }
    if (gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY)
    {
        return true;
    }
    return false;
}

bool park_entry_price_unlocked()
{
    if (gParkFlags & PARK_FLAGS_UNLOCK_ALL_PRICES)
    {
        return true;
    }
    if (!(gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY))
    {
        return true;
    }
    return false;
}

bool Park::IsOpen() const
{
    return (gParkFlags & PARK_FLAGS_PARK_OPEN) != 0;
}

uint16_t Park::GetParkRating() const
{
    return gParkRating;
}

money32 Park::GetParkValue() const
{
    return gParkValue;
}

money32 Park::GetCompanyValue() const
{
    return gCompanyValue;
}

void Park::Initialise()
{
    gUnk13CA740 = 0;
    gParkName = STR_UNNAMED_PARK;
    gStaffHandymanColour = COLOUR_BRIGHT_RED;
    gStaffMechanicColour = COLOUR_LIGHT_BLUE;
    gStaffSecurityColour = COLOUR_YELLOW;
    gNumGuestsInPark = 0;
    gNumGuestsInParkLastWeek = 0;
    gNumGuestsHeadingForPark = 0;
    gGuestChangeModifier = 0;
    gParkRating = 0;
    _guestGenerationProbability = 0;
    gTotalRideValueForMoney = 0;
    gResearchLastItem.rawValue = RESEARCHED_ITEMS_SEPARATOR;

    for (size_t i = 0; i < 20; i++)
    {
        gMarketingCampaignDaysLeft[i] = 0;
    }

    research_reset_items();
    finance_init();

    set_every_ride_type_not_invented();

    set_all_scenery_items_invented();

    gParkEntranceFee = MONEY(10, 00);

    for (auto &peepSpawn : gPeepSpawns)
    {
        peepSpawn.x = PEEP_SPAWN_UNDEFINED;
    }

    gResearchPriorities =
        (1 << RESEARCH_CATEGORY_TRANSPORT) |
        (1 << RESEARCH_CATEGORY_GENTLE) |
        (1 << RESEARCH_CATEGORY_ROLLERCOASTER) |
        (1 << RESEARCH_CATEGORY_THRILL) |
        (1 << RESEARCH_CATEGORY_WATER) |
        (1 << RESEARCH_CATEGORY_SHOP) |
        (1 << RESEARCH_CATEGORY_SCENERY_GROUP);
    gResearchFundingLevel = RESEARCH_FUNDING_NORMAL;

    gGuestInitialCash = MONEY(50,00);
    gGuestInitialHappiness = CalculateGuestInitialHappiness(50);
    gGuestInitialHunger = 200;
    gGuestInitialThirst = 200;
    gScenarioObjectiveType = OBJECTIVE_GUESTS_BY;
    gScenarioObjectiveYear = 4;
    gScenarioObjectiveNumGuests = 1000;
    gLandPrice = MONEY(90,00);
    gConstructionRightsPrice = MONEY(40,00);
    gParkFlags = PARK_FLAGS_NO_MONEY | PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
    ResetHistories();
    finance_reset_history();
    award_reset();

    gS6Info.name[0] = '\0';
    format_string(gS6Info.details, 256, STR_NO_DETAILS_YET, nullptr);
}

void Park::Update(const Date &date)
{
    // Every ~13 seconds
    if (gCurrentTicks % 512 == 0)
    {
        gParkRating = CalculateParkRating();
        gParkValue = CalculateParkValue();
        gCompanyValue = CalculateCompanyValue();
        gTotalRideValueForMoney = CalculateTotalRideValueForMoney();
        _suggestedGuestMaximum = CalculateSuggestedMaxGuests();
        _guestGenerationProbability = CalculateGuestGenerationProbability();

        window_invalidate_by_class(WC_FINANCES);
        auto intent = Intent(INTENT_ACTION_UPDATE_PARK_RATING);
        context_broadcast_intent(&intent);
    }
    // Every ~102 seconds
    if (gCurrentTicks % 4096 == 0)
    {
        gParkSize = CalculateParkSize();
        window_invalidate_by_class(WC_PARK_INFORMATION);
    }
    // Every new week
    if (date.IsWeekStart())
    {
        UpdateHistories();
    }
    GenerateGuests();
}

int32_t Park::CalculateParkSize() const
{
    int32_t tiles;
    tile_element_iterator it;

    tiles = 0;
    tile_element_iterator_begin(&it);
    do {
        if (it.element->GetType() == TILE_ELEMENT_TYPE_SURFACE)
        {
            if (it.element->properties.surface.ownership & (OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED | OWNERSHIP_OWNED))
            {
                tiles++;
            }
        }
    } while (tile_element_iterator_next(&it));

    if (tiles != gParkSize) {
        gParkSize = tiles;
        window_invalidate_by_class(WC_PARK_INFORMATION);
    }

    return tiles;
}

int32_t Park::CalculateParkRating() const
{
    if (_forcedParkRating >= 0)
    {
        return _forcedParkRating;
    }

    int32_t result = 1150;
    if (gParkFlags & PARK_FLAGS_DIFFICULT_PARK_RATING)
    {
        result = 1050;
    }

    // Guests
    {
        // -150 to +3 based on a range of guests from 0 to 2000
        result -= 150 - (std::min<int16_t>(2000, gNumGuestsInPark) / 13);

        // Find the number of happy peeps and the number of peeps who can't find the park exit
        int32_t happyGuestCount = 0;
        int32_t lostGuestCount = 0;
        uint16_t spriteIndex;
        rct_peep * peep;
        FOR_ALL_GUESTS(spriteIndex, peep)
        {
            if (peep->outside_of_park == 0)
            {
                if (peep->happiness > 128)
                {
                    happyGuestCount++;
                }
                if ((peep->peep_flags & PEEP_FLAGS_LEAVING_PARK) &&
                    (peep->peep_is_lost_countdown < 90))
                {
                    lostGuestCount++;
                }
            }
        }

        // Peep happiness -500 to +0
        result -= 500;
        if (gNumGuestsInPark > 0)
        {
            result += 2 * std::min(250, (happyGuestCount * 300) / gNumGuestsInPark);
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

        int32_t i;
        Ride * ride;
        FOR_ALL_RIDES(i, ride)
        {
            totalRideUptime += 100 - ride->downtime;
            if (ride->excitement != RIDE_RATING_UNDEFINED)
            {
                totalRideExcitement += ride->excitement / 8;
                totalRideIntensity += ride->intensity / 8;
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

        totalRideExcitement = std::min<int16_t>(1000, totalRideExcitement);
        totalRideIntensity = std::min<int16_t>(1000, totalRideIntensity);
        result -= 200 - ((totalRideExcitement + totalRideIntensity) / 10);
    }

    // Litter
    {
        rct_litter * litter;
        int32_t litterCount = 0;
        for (uint16_t spriteIndex = gSpriteListHead[SPRITE_LIST_LITTER]; spriteIndex != SPRITE_INDEX_NULL; spriteIndex = litter->next)
        {
            litter = &(get_sprite(spriteIndex)->litter);

            // Ignore recently dropped litter
            if (litter->creationTick - gScenarioTicks >= 7680)
            {
                litterCount++;
            }
        }
        result -= 600 - (4 * (150 - std::min<int32_t>(150, litterCount)));
    }

    result -= gParkRatingCasualtyPenalty;
    result = Math::Clamp(0, result, 999);
    return result;
}

money32 Park::CalculateParkValue() const
{
    money32 result = 0;

    // Sum ride values
    for (int32_t i = 0; i < MAX_RIDES; i++)
    {
        auto ride = get_ride(i);
        result += CalculateRideValue(ride);
    }

    // +7.00 per guest
    result += gNumGuestsInPark * MONEY(7, 00);

    return result;
}

money32 Park::CalculateRideValue(const Ride * ride) const
{
    money32 result = 0;
    if (ride->type != RIDE_TYPE_NULL &&
        ride->value != RIDE_VALUE_UNDEFINED)
    {
        result = (ride->value * 10) * (ride_customers_in_last_5_minutes(ride) + rideBonusValue[ride->type] * 4);
    }
    return result;
}

money32 Park::CalculateCompanyValue() const
{
    return finance_get_current_cash() + gParkValue - gBankLoan;
}

money16 Park::CalculateTotalRideValueForMoney() const
{
    money16 totalRideValue = 0;
    int32_t i;
    Ride * ride;
    FOR_ALL_RIDES(i, ride)
    {
        if (ride->status != RIDE_STATUS_OPEN) continue;
        if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) continue;
        if (ride->lifecycle_flags & RIDE_LIFECYCLE_CRASHED) continue;

        // Add ride value
        if (ride->value != RIDE_VALUE_UNDEFINED)
        {
            money16 rideValue = (money16)(ride->value - ride->price);
            if (rideValue > 0)
            {
                totalRideValue += rideValue * 2;
            }
        }
    }
    return totalRideValue;
}

uint32_t Park::CalculateSuggestedMaxGuests() const
{
    uint32_t suggestedMaxGuests = 0;

    // TODO combine the two ride loops
    int32_t i;
    Ride * ride;
    FOR_ALL_RIDES(i, ride)
    {
        if (ride->status != RIDE_STATUS_OPEN) continue;
        if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) continue;
        if (ride->lifecycle_flags & RIDE_LIFECYCLE_CRASHED) continue;

        // Add guest score for ride type
        suggestedMaxGuests += rideBonusValue[ride->type];
    }

    // If difficult guest generation, extra guests are available for good rides
    if (gParkFlags & PARK_FLAGS_DIFFICULT_GUEST_GENERATION)
    {
        suggestedMaxGuests = std::min<uint32_t>(suggestedMaxGuests, 1000);
        FOR_ALL_RIDES(i, ride)
        {
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_CRASHED) continue;
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) continue;
            if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED)) continue;
            if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_TRACK)) continue;
            if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_DATA_LOGGING)) continue;
            if (ride->length[0] < (600 << 16)) continue;
            if (ride->excitement < RIDE_RATING(6, 00)) continue;

            // Bonus guests for good ride
            suggestedMaxGuests += rideBonusValue[ride->type] * 2;
        }
    }

    suggestedMaxGuests = std::min<uint32_t>(suggestedMaxGuests, 65535);
    return suggestedMaxGuests;
}

uint32_t Park::CalculateGuestGenerationProbability() const
{
    // Begin with 50 + park rating
    uint32_t probability = 50 + Math::Clamp(0, gParkRating - 200, 650);

    // The more guests, the lower the chance of a new one
    int32_t numGuests = gNumGuestsInPark + gNumGuestsHeadingForPark;
    if (numGuests > _suggestedGuestMaximum)
    {
        probability /= 4;
        // Even lower for difficult guest generation
        if (gParkFlags & PARK_FLAGS_DIFFICULT_GUEST_GENERATION)
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
    money16 entranceFee = park_get_entrance_fee();
    if (entranceFee > gTotalRideValueForMoney)
    {
        probability /= 4;
        // Extra penalty for very overpriced entrance fee
        if (entranceFee / 2 > gTotalRideValueForMoney)
        {
            probability /= 4;
        }
    }

    // Reward or penalties for park awards
    for (size_t i = 0; i < MAX_AWARDS; i++)
    {
        const auto award = &gCurrentAwards[i];
        if (award->Time != 0)
        {
            // +/- 0.25% of the probability
            if (award_is_positive(award->Type))
            {
                probability += probability / 4;
            }
            else
            {
                probability -= probability / 4;
            }
        }
    }

    return probability;
}

uint8_t Park::CalculateGuestInitialHappiness(uint8_t percentage)
{
    percentage = Math::Clamp<uint8_t>(15, percentage, 98);

    // The percentages follow this sequence:
    //   15 17 18 20 21 23 25 26 28 29 31 32 34 36 37 39 40 42 43 45 47 48 50 51 53...
    // This sequence can be defined as PI*(9+n)/2 (the value is floored)
    for (uint8_t n = 1; n < 55; n++)
    {
        if ((3.14159 * (9 + n)) / 2 >= percentage)
        {
            return (9 + n) * 4;
        }
    }

    // This is the lowest possible value:
    return 40;
}

void Park::GenerateGuests()
{
    // Generate a new guest for some probability
    if ((int32_t)(scenario_rand() & 0xFFFF) < _guestGenerationProbability)
    {
        bool difficultGeneration = (gParkFlags & PARK_FLAGS_DIFFICULT_GUEST_GENERATION) != 0;
        if (!difficultGeneration || _suggestedGuestMaximum + 150 >= gNumGuestsInPark)
        {
            GenerateGuest();
        }
    }

    // Extra guests generated by advertising campaigns
    for (int32_t campaign = 0; campaign < ADVERTISING_CAMPAIGN_COUNT; campaign++)
    {
        if (gMarketingCampaignDaysLeft[campaign] != 0)
        {
            // Random chance of guest generation
            if ((int32_t)(scenario_rand() & 0xFFFF) < marketing_get_campaign_guest_generation_probability(campaign))
            {
                GenerateGuestFromCampaign(campaign);
            }
        }
    }
}

rct_peep * Park::GenerateGuestFromCampaign(int32_t campaign)
{
    auto peep = GenerateGuest();
    if (peep != nullptr)
    {
        marketing_set_guest_campaign(peep, campaign);
    }
    return peep;
}

rct_peep * Park::GenerateGuest()
{
    rct_peep * peep = nullptr;
    PeepSpawn spawn = gPeepSpawns[get_random_peep_spawn_index()];

    if (spawn.x != PEEP_SPAWN_UNDEFINED)
    {
        spawn.direction ^= 2;
        peep = peep_generate(spawn.x, spawn.y, spawn.z);
        if (peep != nullptr)
        {
            peep->sprite_direction = spawn.direction << 3;

            // Get the centre point of the tile the peep is on
            peep->destination_x = (peep->x & 0xFFE0) + 16;
            peep->destination_y = (peep->y & 0xFFE0) + 16;

            peep->destination_tolerance = 5;
            peep->direction = spawn.direction;
            peep->var_37 = 0;
            peep->state = PEEP_STATE_ENTERING_PARK;
        }
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

void Park::ResetHistories()
{
    for (size_t i = 0; i < 32; i++)
    {
        gParkRatingHistory[i] = 255;
        gGuestsInParkHistory[i] = 255;
    }
}

void Park::UpdateHistories()
{
    uint8_t guestChangeModifier = 1;
    int32_t changeInGuestsInPark = (int32_t)gNumGuestsInPark - (int32_t)gNumGuestsInParkLastWeek;
    if (changeInGuestsInPark > -20)
    {
        guestChangeModifier++;
        if (changeInGuestsInPark < 20)
        {
            guestChangeModifier = 0;
        }
    }
    gGuestChangeModifier = guestChangeModifier;
    gNumGuestsInParkLastWeek = gNumGuestsInPark;

    // Update park rating, guests in park and current cash history
    HistoryPushRecord<uint8_t, 32>(gParkRatingHistory, CalculateParkRating() / 4);
    HistoryPushRecord<uint8_t, 32>(gGuestsInParkHistory, std::min<uint16_t>(gNumGuestsInPark, 5000) / 20);
    HistoryPushRecord<money32, 128>(gCashHistory, finance_get_current_cash() - gBankLoan);

    // Update weekly profit history
    money32 currentWeeklyProfit = gWeeklyProfitAverageDividend;
    if (gWeeklyProfitAverageDivisor != 0)
    {
        currentWeeklyProfit /= gWeeklyProfitAverageDivisor;
    }
    HistoryPushRecord<money32, 128>(gWeeklyProfitHistory, currentWeeklyProfit);
    gWeeklyProfitAverageDividend = 0;
    gWeeklyProfitAverageDivisor = 0;

    // Update park value history
    HistoryPushRecord<money32, 128>(gParkValueHistory, gParkValue);

    // Invalidate relevant windows
    auto intent = Intent(INTENT_ACTION_UPDATE_GUEST_COUNT);
    context_broadcast_intent(&intent);
    window_invalidate_by_class(WC_PARK_INFORMATION);
    window_invalidate_by_class(WC_FINANCES);
}

int32_t park_is_open()
{
    return GetContext()->GetGameState()->GetPark().IsOpen();
}

int32_t park_calculate_size()
{
    auto tiles = GetContext()->GetGameState()->GetPark().CalculateParkSize();
    if (tiles != gParkSize)
    {
        gParkSize = tiles;
        window_invalidate_by_class(WC_PARK_INFORMATION);
    }
    return tiles;
}

uint8_t calculate_guest_initial_happiness(uint8_t percentage)
{
    return Park::CalculateGuestInitialHappiness(percentage);
}
