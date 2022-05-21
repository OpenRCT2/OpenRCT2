/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Guest.h"

#include "../Context.h"
#include "../Game.h"
#include "../OpenRCT2.h"
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../core/DataSerialiser.h"
#include "../core/Guard.hpp"
#include "../core/Numerics.hpp"
#include "../entity/Balloon.h"
#include "../entity/EntityRegistry.h"
#include "../entity/MoneyEffect.h"
#include "../entity/Particle.h"
#include "../interface/Window_internal.h"
#include "../localisation/Formatter.h"
#include "../localisation/Localisation.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../network/network.h"
#include "../object/MusicObject.h"
#include "../peep/GuestPathfinding.h"
#include "../peep/RideUseSystem.h"
#include "../rct2/RCT2.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/ShopItem.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../ride/Vehicle.h"
#include "../scenario/Scenario.h"
#include "../scripting/HookEngine.h"
#include "../scripting/ScriptEngine.h"
#include "../util/Math.hpp"
#include "../windows/Intent.h"
#include "../world/Climate.h"
#include "../world/Footpath.h"
#include "../world/LargeScenery.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Surface.h"
#include "../world/TileElementsView.h"
#include "Peep.h"
#include "Staff.h"

#include <algorithm>
#include <functional>
#include <iterator>

using namespace OpenRCT2;

// Locations of the spiral slide platform that a peep walks from the entrance of the ride to the
// entrance of the slide. Up to 4 waypoints for each 4 sides that an ride entrance can be located
// and 4 different rotations of the ride. 4 * 4 * 4 = 64 locations.
// clang-format off
static constexpr const CoordsXY SpiralSlideWalkingPath[64] = {
    {  56,   8 },
    {   8,   8 },
    {   8,  32 },
    {  32,  32 },
    {   8,   8 },
    {   8,   8 },
    {   8,  32 },
    {  32,  32 },
    {   8,  32 },
    {   8,  32 },
    {   8,  32 },
    {  32,  32 },
    {   8,  56 },
    {   8,  32 },
    {   8,  32 },
    {  32,  32 },
    {  56,  24 },
    {  32,  24 },
    {  32,  24 },
    {  32,   0 },
    {  56, -24 },
    {  56,  24 },
    {  32,  24 },
    {  32,   0 },
    {   8,  24 },
    {  32,  24 },
    {  32,  24 },
    {  32,   0 },
    {  32,  24 },
    {  32,  24 },
    {  32,  24 },
    {  32,   0 },
    {  24,   0 },
    {  24,   0 },
    {  24,   0 },
    {   0,   0 },
    {  24, -24 },
    {  24,   0 },
    {  24,   0 },
    {   0,   0 },
    { -24, -24 },
    {  24, -24 },
    {  24,   0 },
    {   0,   0 },
    {  24,  24 },
    {  24,   0 },
    {  24,   0 },
    {   0,   0 },
    {  24,   8 },
    {   0,   8 },
    {   0,   8 },
    {   0,  32 },
    {   0,   8 },
    {   0,   8 },
    {   0,   8 },
    {   0,  32 },
    { -24,   8 },
    {   0,   8 },
    {   0,   8 },
    {   0,  32 },
    { -24,  56 },
    { -24,   8 },
    {   0,   8 },
    {   0,  32 },
};

/** rct2: 0x00981F4C, 0x00981F4E */
static constexpr const CoordsXY _WatchingPositionOffsets[] = {
    {  7,  5 },
    {  5, 25 },
    { 25,  5 },
    {  5,  7 },
    {  7,  9 },
    {  9, 25 },
    { 25,  9 },
    {  9,  7 },
    {  7, 23 },
    { 23, 25 },
    { 25, 23 },
    { 23,  7 },
    {  7, 27 },
    { 27, 25 },
    { 25, 27 },
    { 27,  7 },
    {  7,  0 },
    {  0, 25 },
    { 25,  0 },
    {  0,  7 },
    {  7,  0 },
    {  0, 25 },
    { 25,  0 },
    {  0,  7 },
    {  7,  0 },
    {  0, 25 },
    { 25,  0 },
    {  0,  7 },
    {  7,  0 },
    {  0, 25 },
    { 25,  0 },
    {  0,  7 },
};

static constexpr const ride_rating NauseaMaximumThresholds[] = {
    300,
    600,
    800,
    1000,
};

/** rct2: 009823AC */
static constexpr const PeepThoughtType crowded_thoughts[] = {
    PeepThoughtType::Lost,
    PeepThoughtType::Tired,
    PeepThoughtType::BadLitter,
    PeepThoughtType::Hungry,
    PeepThoughtType::Thirsty,
    PeepThoughtType::VeryClean,
    PeepThoughtType::Crowded,
    PeepThoughtType::Scenery,
    PeepThoughtType::VeryClean,
    PeepThoughtType::Music,
    PeepThoughtType::Watched,
    PeepThoughtType::NotHungry,
    PeepThoughtType::NotThirsty,
    PeepThoughtType::Toilet,
    PeepThoughtType::None,
    PeepThoughtType::None,
};

static constexpr const char *gPeepEasterEggNames[] = {
    "MICHAEL SCHUMACHER",
    "JACQUES VILLENEUVE",
    "DAMON HILL",
    "MR BEAN",
    "CHRIS SAWYER",
    "KATIE BRAYSHAW",
    "MELANIE WARN",
    "SIMON FOSTER",
    "JOHN WARDLEY",
    "LISA STIRLING",
    "DONALD MACRAE",
    "KATHERINE MCGOWAN",
    "FRANCES MCGOWAN",
    "CORINA MASSOURA",
    "CAROL YOUNG",
    "MIA SHERIDAN",
    "KATIE RODGER",
    "EMMA GARRELL",
    "JOANNE BARTON",
    "FELICITY ANDERSON",
    "KATIE SMITH",
    "EILIDH BELL",
    "NANCY STILLWAGON",
    "DAVID ELLIS",
};
// clang-format on

// Flags used by PeepThoughtToActionMap
enum PeepThoughtToActionFlag : uint8_t
{
    PEEP_THOUGHT_ACTION_NO_FLAGS = 0,
    PEEP_THOUGHT_ACTION_FLAG_RIDE = (1 << 0),
    PEEP_THOUGHT_ACTION_FLAG_SHOP_ITEM_SINGULAR = (1 << 1),
    PEEP_THOUGHT_ACTION_FLAG_SHOP_ITEM_INDEFINITE = (1 << 2),
};

/** rct2: 0x00981DB0 */
static struct
{
    PeepActionType action;
    PeepThoughtToActionFlag flags;
} PeepThoughtToActionMap[] = {
    { PeepActionType::ShakeHead, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::EmptyPockets, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Wow, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_SHOP_ITEM_SINGULAR },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_SHOP_ITEM_INDEFINITE },
    { PeepActionType::ShakeHead, PEEP_THOUGHT_ACTION_FLAG_SHOP_ITEM_INDEFINITE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Wave, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Joy, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::CheckTime, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Wave, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Wave, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Disgust, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::BeingWatched, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::ShakeHead, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Joy, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
};

// These arrays contain the base minimum and maximum nausea ratings for peeps, based on their nausea tolerance level.
static constexpr const ride_rating NauseaMinimumThresholds[] = {
    0,
    0,
    200,
    400,
};

static bool peep_has_voucher_for_free_ride(Guest* peep, Ride* ride);
static void peep_ride_is_too_intense(Guest* peep, Ride* ride, bool peepAtRide);
static void peep_reset_ride_heading(Guest* peep);
static void peep_tried_to_enter_full_queue(Guest* peep, Ride* ride);
static int16_t peep_calculate_ride_satisfaction(Guest* peep, Ride* ride);
static void peep_update_favourite_ride(Guest* peep, Ride* ride);
static int16_t peep_calculate_ride_value_satisfaction(Guest* peep, Ride* ride);
static int16_t peep_calculate_ride_intensity_nausea_satisfaction(Guest* peep, Ride* ride);
static void peep_update_ride_nausea_growth(Guest* peep, Ride* ride);
static bool peep_should_go_on_ride_again(Guest* peep, Ride* ride);
static bool peep_should_preferred_intensity_increase(Guest* peep);
static bool peep_really_liked_ride(Guest* peep, Ride* ride);
static PeepThoughtType peep_assess_surroundings(int16_t centre_x, int16_t centre_y, int16_t centre_z);
static void peep_update_hunger(Guest* peep);
static void peep_decide_whether_to_leave_park(Guest* peep);
static void peep_leave_park(Guest* peep);
static void peep_head_for_nearest_ride_type(Guest* peep, int32_t rideType);
static void PeepHeadForNearestRideWithFlags(Guest* peep, int64_t rideTypeFlags);
bool loc_690FD0(Peep* peep, RideId* rideToView, uint8_t* rideSeatToView, TileElement* tileElement);

template<> bool EntityBase::Is<Guest>() const
{
    return Type == EntityType::Guest;
}

static bool IsValidLocation(const CoordsXYZ& coords)
{
    if (coords.x != LOCATION_NULL)
    {
        if (map_is_location_valid(coords))
        {
            return true;
        }
    }

    return false;
}

template<void (Guest::*EasterEggFunc)(Guest*), bool applyToSelf> static void ApplyEasterEggToNearbyGuests(Guest* guest)
{
    const auto guestLoc = guest->GetLocation();
    if (!IsValidLocation(guestLoc))
        return;

    for (auto* otherGuest : EntityTileList<Guest>(guestLoc))
    {
        if constexpr (!applyToSelf)
        {
            if (otherGuest == guest)
            {
                // Can not apply effect on self.
                continue;
            }
        }
        auto zDiff = std::abs(otherGuest->z - guestLoc.z);
        if (zDiff <= 32)
        {
            std::invoke(EasterEggFunc, *guest, otherGuest);
        }
    }
}

void Guest::GivePassingPeepsPurpleClothes(Guest* passingPeep)
{
    passingPeep->TshirtColour = COLOUR_BRIGHT_PURPLE;
    passingPeep->TrousersColour = COLOUR_BRIGHT_PURPLE;
    passingPeep->Invalidate();
}

void Guest::GivePassingPeepsPizza(Guest* passingPeep)
{
    if (passingPeep->HasItem(ShopItem::Pizza))
        return;

    passingPeep->GiveItem(ShopItem::Pizza);

    int32_t peepDirection = (sprite_direction >> 3) ^ 2;
    int32_t otherPeepOppositeDirection = passingPeep->sprite_direction >> 3;
    if (peepDirection == otherPeepOppositeDirection)
    {
        if (passingPeep->IsActionInterruptable())
        {
            passingPeep->Action = PeepActionType::Wave2;
            passingPeep->ActionFrame = 0;
            passingPeep->ActionSpriteImageOffset = 0;
            passingPeep->UpdateCurrentActionSpriteType();
        }
    }
}

void Guest::MakePassingPeepsSick(Guest* passingPeep)
{
    if (passingPeep->State != PeepState::Walking)
        return;

    if (passingPeep->IsActionInterruptable())
    {
        passingPeep->Action = PeepActionType::ThrowUp;
        passingPeep->ActionFrame = 0;
        passingPeep->ActionSpriteImageOffset = 0;
        passingPeep->UpdateCurrentActionSpriteType();
    }
}

void Guest::GivePassingPeepsIceCream(Guest* passingPeep)
{
    if (passingPeep->HasItem(ShopItem::IceCream))
        return;

    passingPeep->GiveItem(ShopItem::IceCream);
    passingPeep->UpdateSpriteType();
}

/**
 *
 *  rct2: 0x0068FD3A
 */
void Guest::UpdateEasterEggInteractions()
{
    if (PeepFlags & PEEP_FLAGS_PURPLE)
    {
        ApplyEasterEggToNearbyGuests<&Guest::GivePassingPeepsPurpleClothes, true>(this);
    }

    if (PeepFlags & PEEP_FLAGS_PIZZA)
    {
        ApplyEasterEggToNearbyGuests<&Guest::GivePassingPeepsPizza, true>(this);
    }

    if (PeepFlags & PEEP_FLAGS_CONTAGIOUS)
    {
        ApplyEasterEggToNearbyGuests<&Guest::MakePassingPeepsSick, false>(this);
    }

    if (PeepFlags & PEEP_FLAGS_ICE_CREAM)
    {
        ApplyEasterEggToNearbyGuests<&Guest::GivePassingPeepsIceCream, false>(this);
    }

    if (PeepFlags & PEEP_FLAGS_JOY)
    {
        if ((scenario_rand() & 0xFFFF) <= 1456)
        {
            if (IsActionInterruptable())
            {
                Action = PeepActionType::Joy;
                ActionFrame = 0;
                ActionSpriteImageOffset = 0;
                UpdateCurrentActionSpriteType();
            }
        }
    }
}

int32_t Guest::GetEasterEggNameId() const
{
    char buffer[256]{};

    Formatter ft;
    FormatNameTo(ft);
    format_string(buffer, sizeof(buffer), STR_STRINGID, ft.Data());

    for (uint32_t i = 0; i < std::size(gPeepEasterEggNames); i++)
        if (_stricmp(buffer, gPeepEasterEggNames[i]) == 0)
            return static_cast<int32_t>(i);

    return -1;
}

void Guest::HandleEasterEggName()
{
    PeepFlags &= ~PEEP_FLAGS_WAVING;
    if (CheckEasterEggName(EASTEREGG_PEEP_NAME_KATIE_BRAYSHAW))
    {
        PeepFlags |= PEEP_FLAGS_WAVING;
    }

    PeepFlags &= ~PEEP_FLAGS_PHOTO;
    if (CheckEasterEggName(EASTEREGG_PEEP_NAME_CHRIS_SAWYER))
    {
        PeepFlags |= PEEP_FLAGS_PHOTO;
    }

    PeepFlags &= ~PEEP_FLAGS_PAINTING;
    if (CheckEasterEggName(EASTEREGG_PEEP_NAME_SIMON_FOSTER))
    {
        PeepFlags |= PEEP_FLAGS_PAINTING;
    }

    PeepFlags &= ~PEEP_FLAGS_WOW;
    if (CheckEasterEggName(EASTEREGG_PEEP_NAME_JOHN_WARDLEY))
    {
        PeepFlags |= PEEP_FLAGS_WOW;
    }

    if (CheckEasterEggName(EASTEREGG_PEEP_NAME_MELANIE_WARN))
    {
        Happiness = 250;
        HappinessTarget = 250;
        Energy = 127;
        EnergyTarget = 127;
        Nausea = 0;
        NauseaTarget = 0;
    }

    PeepFlags &= ~PEEP_FLAGS_LITTER;
    if (CheckEasterEggName(EASTEREGG_PEEP_NAME_LISA_STIRLING))
    {
        PeepFlags |= PEEP_FLAGS_LITTER;
    }

    PeepFlags &= ~PEEP_FLAGS_LOST;
    if (CheckEasterEggName(EASTEREGG_PEEP_NAME_DONALD_MACRAE))
    {
        PeepFlags |= PEEP_FLAGS_LOST;
    }

    PeepFlags &= ~PEEP_FLAGS_HUNGER;
    if (CheckEasterEggName(EASTEREGG_PEEP_NAME_KATHERINE_MCGOWAN))
    {
        PeepFlags |= PEEP_FLAGS_HUNGER;
    }

    PeepFlags &= ~PEEP_FLAGS_TOILET;
    if (CheckEasterEggName(EASTEREGG_PEEP_NAME_FRANCES_MCGOWAN))
    {
        PeepFlags |= PEEP_FLAGS_TOILET;
    }

    PeepFlags &= ~PEEP_FLAGS_CROWDED;
    if (CheckEasterEggName(EASTEREGG_PEEP_NAME_CORINA_MASSOURA))
    {
        PeepFlags |= PEEP_FLAGS_CROWDED;
    }

    PeepFlags &= ~PEEP_FLAGS_HAPPINESS;
    if (CheckEasterEggName(EASTEREGG_PEEP_NAME_CAROL_YOUNG))
    {
        PeepFlags |= PEEP_FLAGS_HAPPINESS;
    }

    PeepFlags &= ~PEEP_FLAGS_NAUSEA;
    if (CheckEasterEggName(EASTEREGG_PEEP_NAME_MIA_SHERIDAN))
    {
        PeepFlags |= PEEP_FLAGS_NAUSEA;
    }

    if (CheckEasterEggName(EASTEREGG_PEEP_NAME_KATIE_RODGER))
    {
        PeepFlags |= PEEP_FLAGS_LEAVING_PARK;
        PeepFlags &= ~PEEP_FLAGS_PARK_ENTRANCE_CHOSEN;
    }

    PeepFlags &= ~PEEP_FLAGS_PURPLE;
    if (CheckEasterEggName(EASTEREGG_PEEP_NAME_EMMA_GARRELL))
    {
        PeepFlags |= PEEP_FLAGS_PURPLE;
    }

    PeepFlags &= ~PEEP_FLAGS_PIZZA;
    if (CheckEasterEggName(EASTEREGG_PEEP_NAME_JOANNE_BARTON))
    {
        PeepFlags |= PEEP_FLAGS_PIZZA;
    }

    PeepFlags &= ~PEEP_FLAGS_CONTAGIOUS;
    if (CheckEasterEggName(EASTEREGG_PEEP_NAME_FELICITY_ANDERSON))
    {
        PeepFlags |= PEEP_FLAGS_CONTAGIOUS;
    }

    PeepFlags &= ~PEEP_FLAGS_JOY;
    if (CheckEasterEggName(EASTEREGG_PEEP_NAME_KATIE_SMITH))
    {
        PeepFlags |= PEEP_FLAGS_JOY;
    }

    PeepFlags &= ~PEEP_FLAGS_ANGRY;
    if (CheckEasterEggName(EASTEREGG_PEEP_NAME_EILIDH_BELL))
    {
        PeepFlags |= PEEP_FLAGS_ANGRY;
    }

    PeepFlags &= ~PEEP_FLAGS_ICE_CREAM;
    if (CheckEasterEggName(EASTEREGG_PEEP_NAME_NANCY_STILLWAGON))
    {
        PeepFlags |= PEEP_FLAGS_ICE_CREAM;
    }

    PeepFlags &= ~PEEP_FLAGS_HERE_WE_ARE;
    if (CheckEasterEggName(EASTEREGG_PEEP_NAME_DAVID_ELLIS))
    {
        PeepFlags |= PEEP_FLAGS_HERE_WE_ARE;
    }
}

/**
 *
 *  rct2: 0x0069A5A0
 * tests if a peep's name matches a cheat code, normally returns using a register flag
 */
int32_t Guest::CheckEasterEggName(int32_t index) const
{
    char buffer[256]{};

    Formatter ft;
    FormatNameTo(ft);
    format_string(buffer, sizeof(buffer), STR_STRINGID, ft.Data());

    return _stricmp(buffer, gPeepEasterEggNames[index]) == 0;
}

void Guest::loc_68F9F3()
{
    // Idle peep happiness tends towards 127 (50%).
    if (HappinessTarget >= 128)
        HappinessTarget--;
    else
        HappinessTarget++;

    NauseaTarget = std::max(NauseaTarget - 2, 0);

    if (Energy <= 50)
    {
        Energy = std::max(Energy - 2, 0);
    }

    if (Hunger < 10)
    {
        Hunger = std::max(Hunger - 1, 0);
    }

    if (Thirst < 10)
    {
        Thirst = std::max(Thirst - 1, 0);
    }

    if (Toilet >= 195)
    {
        Toilet--;
    }

    if (State == PeepState::Walking && NauseaTarget >= 128)
    {
        if ((scenario_rand() & 0xFF) <= static_cast<uint8_t>((Nausea - 128) / 2))
        {
            if (IsActionInterruptable())
            {
                Action = PeepActionType::ThrowUp;
                ActionFrame = 0;
                ActionSpriteImageOffset = 0;
                UpdateCurrentActionSpriteType();
            }
        }
    }
}

void Guest::loc_68FA89()
{
    // 68FA89
    if (TimeToConsume == 0 && HasFoodOrDrink())
    {
        TimeToConsume += 3;
    }

    if (TimeToConsume != 0 && State != PeepState::OnRide)
    {
        TimeToConsume = std::max(TimeToConsume - 3, 0);

        if (HasDrink())
        {
            Thirst = std::min(Thirst + 7, 255);
        }
        else
        {
            Hunger = std::min(Hunger + 7, 255);
            Thirst = std::max(Thirst - 3, 0);
            Toilet = std::min(Toilet + 2, 255);
        }

        if (TimeToConsume == 0)
        {
            int32_t chosen_food = bitscanforward(GetFoodOrDrinkFlags());
            if (chosen_food != -1)
            {
                ShopItem food = ShopItem(chosen_food);
                RemoveItem(food);

                auto discardContainer = GetShopItemDescriptor(food).DiscardContainer;
                if (discardContainer != ShopItem::None)
                {
                    GiveItem(discardContainer);
                }

                WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                UpdateSpriteType();
            }
        }
    }

    uint8_t newEnergy = Energy;
    uint8_t newTargetEnergy = EnergyTarget;
    if (newEnergy >= newTargetEnergy)
    {
        newEnergy -= 2;
        if (newEnergy < newTargetEnergy)
            newEnergy = newTargetEnergy;
    }
    else
    {
        newEnergy = std::min(PEEP_MAX_ENERGY_TARGET, newEnergy + 4);
        if (newEnergy > newTargetEnergy)
            newEnergy = newTargetEnergy;
    }

    if (newEnergy < PEEP_MIN_ENERGY)
        newEnergy = PEEP_MIN_ENERGY;

    /* Previous code here suggested maximum energy is 128. */
    newEnergy = std::min(static_cast<uint8_t>(PEEP_MAX_ENERGY), newEnergy);

    if (newEnergy != Energy)
    {
        Energy = newEnergy;
        WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_2;
    }

    uint8_t newHappiness = Happiness;
    uint8_t newHappinessGrowth = HappinessTarget;
    if (newHappiness >= newHappinessGrowth)
    {
        newHappiness = std::max(newHappiness - 4, 0);
        if (newHappiness < newHappinessGrowth)
            newHappiness = newHappinessGrowth;
    }
    else
    {
        newHappiness = std::min(255, newHappiness + 4);
        if (newHappiness > newHappinessGrowth)
            newHappiness = newHappinessGrowth;
    }

    if (newHappiness != Happiness)
    {
        Happiness = newHappiness;
        WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_2;
    }

    uint8_t newNausea = Nausea;
    uint8_t newNauseaGrowth = NauseaTarget;
    if (newNausea >= newNauseaGrowth)
    {
        newNausea = std::max(newNausea - 4, 0);
        if (newNausea < newNauseaGrowth)
            newNausea = newNauseaGrowth;
    }
    else
    {
        newNausea = std::min(255, newNausea + 4);
        if (newNausea > newNauseaGrowth)
            newNausea = newNauseaGrowth;
    }

    if (newNausea != Nausea)
    {
        Nausea = newNausea;
        WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_2;
    }
}

void Guest::Tick128UpdateGuest(int32_t index)
{
    if (static_cast<uint32_t>(index & 0x1FF) == (gCurrentTicks & 0x1FF))
    {
        /* Effect of masking with 0x1FF here vs mask 0x7F,
         * which is the condition for calling this function, is
         * to reduce how often the content in this conditional
         * is executed to once every four calls. */
        if (PeepFlags & PEEP_FLAGS_CROWDED)
        {
            PeepThoughtType thought_type = crowded_thoughts[scenario_rand() & 0xF];
            if (thought_type != PeepThoughtType::None)
            {
                InsertNewThought(thought_type);
            }
        }

        if (PeepFlags & PEEP_FLAGS_EXPLODE && x != LOCATION_NULL)
        {
            if (State == PeepState::Walking || State == PeepState::Sitting)
            {
                OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::Crash, GetLocation());

                ExplosionCloud::Create({ x, y, z + 16 });
                ExplosionFlare::Create({ x, y, z + 16 });

                Remove();
                return;
            }

            PeepFlags &= ~PEEP_FLAGS_EXPLODE;
        }

        if (PeepFlags & PEEP_FLAGS_HUNGER)
        {
            if (Hunger >= 15)
                Hunger -= 15;
        }

        if (PeepFlags & PEEP_FLAGS_TOILET)
        {
            if (Toilet <= 180)
                Toilet += 50;
        }

        if (PeepFlags & PEEP_FLAGS_HAPPINESS)
        {
            HappinessTarget = 5;
        }

        if (PeepFlags & PEEP_FLAGS_NAUSEA)
        {
            NauseaTarget = 200;
            if (Nausea <= 130)
                Nausea = 130;
        }

        if (Angriness != 0)
            Angriness--;

        if (State == PeepState::Walking || State == PeepState::Sitting)
        {
            SurroundingsThoughtTimeout++;
            if (SurroundingsThoughtTimeout >= 18)
            {
                SurroundingsThoughtTimeout = 0;
                if (x != LOCATION_NULL)
                {
                    PeepThoughtType thought_type = peep_assess_surroundings(x & 0xFFE0, y & 0xFFE0, z);

                    if (thought_type != PeepThoughtType::None)
                    {
                        InsertNewThought(thought_type);
                        HappinessTarget = std::min(PEEP_MAX_HAPPINESS, HappinessTarget + 45);
                    }
                }
            }
        }

        UpdateSpriteType();

        if (State == PeepState::OnRide || State == PeepState::EnteringRide)
        {
            GuestTimeOnRide = std::min(255, GuestTimeOnRide + 1);

            if (PeepFlags & PEEP_FLAGS_WOW)
            {
                InsertNewThought(PeepThoughtType::Wow2);
            }

            if (GuestTimeOnRide > 15)
            {
                HappinessTarget = std::max(0, HappinessTarget - 5);

                if (GuestTimeOnRide > 22)
                {
                    auto ride = get_ride(CurrentRide);
                    if (ride != nullptr)
                    {
                        PeepThoughtType thought_type = ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IN_RIDE)
                            ? PeepThoughtType::GetOut
                            : PeepThoughtType::GetOff;

                        InsertNewThought(thought_type, CurrentRide);
                    }
                }
            }
        }

        if (State == PeepState::Walking && !OutsideOfPark && !(PeepFlags & PEEP_FLAGS_LEAVING_PARK) && GuestNumRides == 0
            && GuestHeadingToRideId.IsNull())
        {
            uint32_t time_duration = gCurrentTicks - ParkEntryTime;
            time_duration /= 2048;

            if (time_duration >= 5)
            {
                PickRideToGoOn();

                if (GuestHeadingToRideId.IsNull())
                {
                    HappinessTarget = std::max(HappinessTarget - 128, 0);
                    peep_leave_park(this);
                    peep_update_hunger(this);
                    loc_68F9F3();
                    loc_68FA89();
                    return;
                }
            }
        }

        if ((scenario_rand() & 0xFFFF) <= ((HasItem(ShopItem::Map)) ? 8192U : 2184U))
        {
            PickRideToGoOn();
        }

        if (static_cast<uint32_t>(index & 0x3FF) == (gCurrentTicks & 0x3FF))
        {
            /* Effect of masking with 0x3FF here vs mask 0x1FF,
             * which is used in the encompassing conditional, is
             * to reduce how often the content in this conditional
             * is executed to once every second time the encompassing
             * conditional executes. */

            if (!OutsideOfPark && (State == PeepState::Walking || State == PeepState::Sitting))
            {
                uint8_t num_thoughts = 0;
                PeepThoughtType possible_thoughts[5];

                if (PeepFlags & PEEP_FLAGS_LEAVING_PARK)
                {
                    possible_thoughts[num_thoughts++] = PeepThoughtType::GoHome;
                }
                else
                {
                    if (Energy <= 70 && Happiness < 128)
                    {
                        possible_thoughts[num_thoughts++] = PeepThoughtType::Tired;
                    }

                    if (Hunger <= 10 && !HasFoodOrDrink())
                    {
                        possible_thoughts[num_thoughts++] = PeepThoughtType::Hungry;
                    }

                    if (Thirst <= 25 && !HasFoodOrDrink())
                    {
                        possible_thoughts[num_thoughts++] = PeepThoughtType::Thirsty;
                    }

                    if (Toilet >= 160)
                    {
                        possible_thoughts[num_thoughts++] = PeepThoughtType::Toilet;
                    }

                    if (!(gParkFlags & PARK_FLAGS_NO_MONEY) && CashInPocket <= 9.00_GBP && Happiness >= 105 && Energy >= 70)
                    {
                        /* The energy check was originally a second check on happiness.
                         * This was superfluous so should probably check something else.
                         * Guessed that this should really be checking energy, since
                         * the addresses for happiness and energy are quite close,
                         * 70 is also the threshold for tired thoughts (see above) and
                         * it makes sense that a tired peep might not think about getting
                         * more money. */
                        possible_thoughts[num_thoughts++] = PeepThoughtType::RunningOut;
                    }
                }

                if (num_thoughts != 0)
                {
                    PeepThoughtType chosen_thought = possible_thoughts[scenario_rand() % num_thoughts];

                    InsertNewThought(chosen_thought);

                    switch (chosen_thought)
                    {
                        case PeepThoughtType::Hungry:
                            PeepHeadForNearestRideWithFlags(this, RIDE_TYPE_FLAG_SELLS_FOOD);
                            break;
                        case PeepThoughtType::Thirsty:
                            PeepHeadForNearestRideWithFlags(this, RIDE_TYPE_FLAG_SELLS_DRINKS);
                            break;
                        case PeepThoughtType::Toilet:
                            PeepHeadForNearestRideWithFlags(this, RIDE_TYPE_FLAG_IS_TOILET);
                            break;
                        case PeepThoughtType::RunningOut:
                            PeepHeadForNearestRideWithFlags(this, RIDE_TYPE_FLAG_IS_CASH_MACHINE);
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        else
        {
            /* This branch of the conditional is executed on the
             * remaining times the encompassing conditional is
             * executed (which is also every second time, but
             * the alternate time to the true branch). */
            if (Nausea >= 140)
            {
                PeepThoughtType thought_type = PeepThoughtType::Sick;
                if (Nausea >= 200)
                {
                    thought_type = PeepThoughtType::VerySick;
                    peep_head_for_nearest_ride_type(this, RIDE_TYPE_FIRST_AID);
                }
                InsertNewThought(thought_type);
            }
        }

        switch (State)
        {
            case PeepState::Walking:
            case PeepState::LeavingPark:
            case PeepState::EnteringPark:
                peep_decide_whether_to_leave_park(this);
                peep_update_hunger(this);
                break;

            case PeepState::Sitting:
                if (EnergyTarget <= 135)
                    EnergyTarget += 5;

                if (Thirst >= 5)
                {
                    Thirst -= 4;
                    Toilet = std::min(255, Toilet + 3);
                }

                if (NauseaTarget >= 50)
                    NauseaTarget -= 6;

                // In the original this branched differently
                // but it would mean setting the peep happiness from
                // a thought type entry which i think is incorrect.
                peep_update_hunger(this);
                break;

            case PeepState::Queuing:
                if (TimeInQueue >= 2000)
                {
                    /* Peep happiness is affected once the peep has been waiting
                     * too long in a queue. */
                    bool found = false;
                    for (auto* pathElement : TileElementsView<PathElement>(NextLoc))
                    {
                        if (pathElement->GetBaseZ() != NextLoc.z)
                            continue;

                        // Check if the footpath has a queue line TV monitor on it
                        if (pathElement->HasAddition() && !pathElement->AdditionIsGhost())
                        {
                            auto* pathAddEntry = pathElement->GetAdditionEntry();
                            if (pathAddEntry != nullptr && (pathAddEntry->flags & PATH_BIT_FLAG_IS_QUEUE_SCREEN))
                            {
                                found = true;
                            }
                        }
                        break;
                    }

                    if (found)
                    {
                        /* Queue line TV monitors make the peeps waiting in the queue
                         * slowly happier, up to a certain level. */
                        /* Why don't queue line TV monitors start affecting the peeps
                         * as soon as they join the queue?? */
                        if (HappinessTarget < 90)
                            HappinessTarget = 90;

                        if (HappinessTarget < 165)
                            HappinessTarget += 2;
                    }
                    else
                    {
                        /* Without a queue line TV monitor peeps waiting too long
                         * in a queue get less happy. */
                        HappinessTarget = std::max(HappinessTarget - 4, 0);
                    }
                }
                peep_update_hunger(this);
                break;
            case PeepState::EnteringRide:
                if (SubState == 17 || SubState == 15)
                {
                    peep_decide_whether_to_leave_park(this);
                }
                peep_update_hunger(this);
                break;
            default:
                break;
        }

        loc_68F9F3();
    }

    loc_68FA89();
}

/**
 *
 *  rct2: 0x00691677
 */
void Guest::TryGetUpFromSitting()
{
    // Eats all food first
    if (HasFoodOrDrink())
        return;

    TimeToSitdown--;
    if (TimeToSitdown)
        return;

    SetState(PeepState::Walking);

    // Set destination to the centre of the tile.
    auto destination = GetLocation().ToTileCentre();
    SetDestination(destination, 5);
    UpdateCurrentActionSpriteType();
}

/**
 *
 *  rct2: 0x0069152B
 */
void Guest::UpdateSitting()
{
    if (SittingSubState == PeepSittingSubState::TryingToSit)
    {
        if (!CheckForPath())
            return;
        // 691541

        uint8_t pathingResult;
        PerformNextAction(pathingResult);
        if (!(pathingResult & PATHING_DESTINATION_REACHED))
            return;

        auto loc = GetLocation().ToTileStart() + CoordsXYZ{ BenchUseOffsets[Var37 & 0x7], 0 };

        MoveTo(loc);

        sprite_direction = ((Var37 + 2) & 3) * 8;
        Action = PeepActionType::Idle;
        NextActionSpriteType = PeepActionSpriteType::SittingIdle;
        SwitchNextActionSpriteType();

        SittingSubState = PeepSittingSubState::SatDown;

        // Sets time to sit on seat
        TimeToSitdown = (129 - Energy) * 16 + 50;
    }
    else if (SittingSubState == PeepSittingSubState::SatDown)
    {
        if (!IsActionInterruptable())
        {
            UpdateAction();
            if (!IsActionWalking())
                return;

            Action = PeepActionType::Idle;
            TryGetUpFromSitting();
            return;
        }

        if ((PeepFlags & PEEP_FLAGS_LEAVING_PARK))
        {
            SetState(PeepState::Walking);

            // Set destination to the centre of the tile
            auto destination = GetLocation().ToTileCentre();
            SetDestination(destination, 5);
            UpdateCurrentActionSpriteType();
            return;
        }

        if (SpriteType == PeepSpriteType::Umbrella)
        {
            TryGetUpFromSitting();
            return;
        }

        if (HasFoodOrDrink())
        {
            if ((scenario_rand() & 0xFFFF) > 1310)
            {
                TryGetUpFromSitting();
                return;
            }
            Action = PeepActionType::SittingEatFood;
            ActionFrame = 0;
            ActionSpriteImageOffset = 0;
            UpdateCurrentActionSpriteType();
            return;
        }

        int32_t rand = scenario_rand();
        if ((rand & 0xFFFF) > 131)
        {
            TryGetUpFromSitting();
            return;
        }
        if (SpriteType == PeepSpriteType::Balloon || SpriteType == PeepSpriteType::Hat)
        {
            TryGetUpFromSitting();
            return;
        }

        Action = PeepActionType::SittingLookAroundLeft;
        if (rand & 0x80000000)
        {
            Action = PeepActionType::SittingLookAroundRight;
        }

        if (rand & 0x40000000)
        {
            Action = PeepActionType::SittingCheckWatch;
        }
        ActionFrame = 0;
        ActionSpriteImageOffset = 0;
        UpdateCurrentActionSpriteType();
        return;
    }
}

/**
 * To simplify check of 0x36BA3E0 and 0x11FF78
 * returns false on no food.
 */
int64_t Guest::GetFoodOrDrinkFlags() const
{
    return GetItemFlags() & (ShopItemsGetAllFoods() | ShopItemsGetAllDrinks());
}

int64_t Guest::GetEmptyContainerFlags() const
{
    return GetItemFlags() & ShopItemsGetAllContainers();
}

bool Guest::HasFoodOrDrink() const
{
    return GetFoodOrDrinkFlags() != 0;
}

/**
 * To simplify check of NOT(0x12BA3C0 and 0x118F48)
 * returns 0 on no food.
 */
bool Guest::HasDrink() const
{
    return GetItemFlags() & ShopItemsGetAllDrinks();
}

bool Guest::HasEmptyContainer() const
{
    return GetEmptyContainerFlags() != 0;
}

/**
 *
 *  rct2: 0x69C308
 * Check if lost.
 */
void Guest::CheckIfLost()
{
    if (!(PeepFlags & PEEP_FLAGS_LOST))
    {
        if (ride_get_count() < 2)
            return;
        PeepFlags ^= PEEP_FLAGS_21;

        if (!(PeepFlags & PEEP_FLAGS_21))
            return;

        TimeLost++;
        if (TimeLost != 254)
            return;
        TimeLost = 230;
    }
    InsertNewThought(PeepThoughtType::Lost);

    HappinessTarget = std::max(HappinessTarget - 30, 0);
}

/**
 *
 *  rct2: 0x69C26B
 * Check if can't find ride.
 */
void Guest::CheckCantFindRide()
{
    if (GuestHeadingToRideId.IsNull())
        return;

    // Peeps will think "I can't find ride X" twice before giving up completely.
    if (GuestIsLostCountdown == 30 || GuestIsLostCountdown == 60)
    {
        InsertNewThought(PeepThoughtType::CantFind, GuestHeadingToRideId);
        HappinessTarget = std::max(HappinessTarget - 30, 0);
    }

    GuestIsLostCountdown--;
    if (GuestIsLostCountdown != 0)
        return;

    GuestHeadingToRideId = RideId::GetNull();
    rct_window* w = window_find_by_number(WC_PEEP, sprite_index);

    if (w != nullptr)
    {
        window_event_invalidate_call(w);
    }

    window_invalidate_by_number(WC_PEEP, sprite_index);
}

/**
 *
 *  rct2: 0x69C2D0
 * Check if can't find exit.
 */
void Guest::CheckCantFindExit()
{
    if (!(PeepFlags & PEEP_FLAGS_LEAVING_PARK))
        return;

    // Peeps who can't find the park exit will continue to get less happy until they find it.
    if (GuestIsLostCountdown == 1)
    {
        InsertNewThought(PeepThoughtType::CantFindExit);
        HappinessTarget = std::max(HappinessTarget - 30, 0);
    }

    if (--GuestIsLostCountdown == 0)
        GuestIsLostCountdown = 90;
}

/** Main logic to decide whether a peep should buy an item in question
 *
 * Also handles the purchase as well, so once it returns, the peep will have the
 * item and the money will have been deducted.
 *
 * eax: shopItem | (rideIndex << 8)
 * ecx: price
 * esi: *peep
 *
 * Returns 0 or 1 depending on if the peep decided to buy the item
 *
 *  rct2: 0x0069AF1E
 */
bool Guest::DecideAndBuyItem(Ride* ride, ShopItem shopItem, money32 price)
{
    money32 itemValue;

    bool hasVoucher = false;

    bool isRainingAndUmbrella = shopItem == ShopItem::Umbrella && climate_is_raining();

    if ((HasItem(ShopItem::Voucher)) && (VoucherType == VOUCHER_TYPE_FOOD_OR_DRINK_FREE) && (VoucherShopItem == shopItem))
    {
        hasVoucher = true;
    }

    if (HasItem(shopItem))
    {
        InsertNewThought(PeepThoughtType::AlreadyGot, shopItem);
        return false;
    }

    if (GetShopItemDescriptor(shopItem).IsFoodOrDrink())
    {
        int32_t food = bitscanforward(GetFoodOrDrinkFlags());
        if (food != -1)
        {
            InsertNewThought(PeepThoughtType::HaventFinished, static_cast<ShopItem>(food));
            return false;
        }

        if (Nausea >= 145)
            return false;
    }

    if ((shopItem == ShopItem::Balloon || shopItem == ShopItem::IceCream || shopItem == ShopItem::Candyfloss
         || shopItem == ShopItem::Sunglasses)
        && climate_is_raining())
    {
        return false;
    }

    if ((shopItem == ShopItem::Sunglasses || shopItem == ShopItem::IceCream) && gClimateCurrent.Temperature < 12)
    {
        return false;
    }

    if (GetShopItemDescriptor(shopItem).IsFood() && (Hunger > 75))
    {
        InsertNewThought(PeepThoughtType::NotHungry);
        return false;
    }

    if (GetShopItemDescriptor(shopItem).IsDrink() && (Thirst > 75))
    {
        InsertNewThought(PeepThoughtType::NotThirsty);
        return false;
    }

    if (!isRainingAndUmbrella && (shopItem != ShopItem::Map) && GetShopItemDescriptor(shopItem).IsSouvenir() && !hasVoucher)
    {
        if (((scenario_rand() & 0x7F) + 0x73) > Happiness || GuestNumRides < 3)
            return false;
    }

    if (!hasVoucher)
    {
        if (price != 0 && !(gParkFlags & PARK_FLAGS_NO_MONEY))
        {
            if (CashInPocket == 0)
            {
                InsertNewThought(PeepThoughtType::SpentMoney);
                return false;
            }
            if (price > CashInPocket)
            {
                InsertNewThought(PeepThoughtType::CantAffordItem, shopItem);
                return false;
            }
        }

        if (gClimateCurrent.Temperature >= 21)
            itemValue = GetShopItemDescriptor(shopItem).HotValue;
        else if (gClimateCurrent.Temperature <= 11)
            itemValue = GetShopItemDescriptor(shopItem).ColdValue;
        else
            itemValue = GetShopItemDescriptor(shopItem).BaseValue;

        if (itemValue < price)
        {
            itemValue -= price;

            if (!isRainingAndUmbrella)
            {
                itemValue = -itemValue;
                if (Happiness >= 128)
                {
                    itemValue /= 2;
                    if (Happiness >= 180)
                        itemValue /= 2;
                }
                if (itemValue > (static_cast<money16>(scenario_rand() & 0x07)))
                {
                    // "I'm not paying that much for x"
                    InsertNewThought(GetShopItemDescriptor(shopItem).TooMuchThought, ride->id);
                    return false;
                }
            }
        }
        else
        {
            itemValue -= price;
            itemValue = std::max(8, itemValue);

            if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
            {
                if (itemValue >= static_cast<money32>(scenario_rand() & 0x07))
                {
                    // "This x is a really good value"
                    InsertNewThought(GetShopItemDescriptor(shopItem).GoodValueThought, ride->id);
                }
            }

            int32_t happinessGrowth = itemValue * 4;
            HappinessTarget = std::min((HappinessTarget + happinessGrowth), PEEP_MAX_HAPPINESS);
            Happiness = std::min((Happiness + happinessGrowth), PEEP_MAX_HAPPINESS);
        }

        // reset itemValue for satisfaction calculation
        if (gClimateCurrent.Temperature >= 21)
            itemValue = GetShopItemDescriptor(shopItem).HotValue;
        else if (gClimateCurrent.Temperature <= 11)
            itemValue = GetShopItemDescriptor(shopItem).ColdValue;
        else
            itemValue = GetShopItemDescriptor(shopItem).BaseValue;
        itemValue -= price;
        uint8_t satisfaction = 0;
        if (itemValue > -8)
        {
            satisfaction++;
            if (itemValue > -3)
            {
                satisfaction++;
                if (itemValue > 3)
                    satisfaction++;
            }
        }
        ride->UpdateSatisfaction(satisfaction);
    }

    // The peep has now decided to buy the item (or, specifically, has not been
    // dissuaded so far).
    GiveItem(shopItem);

    if (shopItem == ShopItem::TShirt)
        TshirtColour = ride->track_colour[0].main;

    if (shopItem == ShopItem::Hat)
        HatColour = ride->track_colour[0].main;

    if (shopItem == ShopItem::Balloon)
        BalloonColour = ride->track_colour[0].main;

    if (shopItem == ShopItem::Umbrella)
        UmbrellaColour = ride->track_colour[0].main;

    if (shopItem == ShopItem::Map)
        ResetPathfindGoal();

    uint16_t consumptionTime = GetShopItemDescriptor(shopItem).ConsumptionTime;
    TimeToConsume = std::min((TimeToConsume + consumptionTime), 255);

    if (shopItem == ShopItem::Photo)
        Photo1RideRef = ride->id;

    if (shopItem == ShopItem::Photo2)
        Photo2RideRef = ride->id;

    if (shopItem == ShopItem::Photo3)
        Photo3RideRef = ride->id;

    if (shopItem == ShopItem::Photo4)
        Photo4RideRef = ride->id;

    WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
    UpdateSpriteType();
    if (PeepFlags & PEEP_FLAGS_TRACKING)
    {
        auto ft = Formatter();
        FormatNameTo(ft);
        ft.Add<rct_string_id>(GetShopItemDescriptor(shopItem).Naming.Indefinite);
        if (gConfigNotifications.guest_bought_item)
        {
            News::AddItemToQueue(News::ItemType::PeepOnRide, STR_PEEP_TRACKING_NOTIFICATION_BOUGHT_X, sprite_index, ft);
        }
    }

    if (GetShopItemDescriptor(shopItem).IsFood())
        AmountOfFood++;

    if (GetShopItemDescriptor(shopItem).IsDrink())
        AmountOfDrinks++;

    if (GetShopItemDescriptor(shopItem).IsSouvenir())
        AmountOfSouvenirs++;

    money16* expend_type = &PaidOnSouvenirs;
    ExpenditureType expenditure = ExpenditureType::ShopStock;

    if (GetShopItemDescriptor(shopItem).IsFood())
    {
        expend_type = &PaidOnFood;
        expenditure = ExpenditureType::FoodDrinkStock;
    }

    if (GetShopItemDescriptor(shopItem).IsDrink())
    {
        expend_type = &PaidOnDrink;
        expenditure = ExpenditureType::FoodDrinkStock;
    }

    if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
        finance_payment(GetShopItemDescriptor(shopItem).Cost, expenditure);

    // Sets the expenditure type to *_FOODDRINK_SALES or *_SHOP_SALES appropriately.
    expenditure = static_cast<ExpenditureType>(static_cast<int32_t>(expenditure) - 1);
    if (hasVoucher)
    {
        RemoveItem(ShopItem::Voucher);
        WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
    }
    else if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
    {
        SpendMoney(*expend_type, price, expenditure);
    }
    ride->total_profit += (price - GetShopItemDescriptor(shopItem).Cost);
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
    ride->cur_num_customers++;
    ride->total_customers++;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;

    return true;
}

/**
 * Updates various peep stats upon entering a ride, as well as updating the
 * ride's satisfaction value.
 *  rct2: 0x0069545B
 */
void Guest::OnEnterRide(Ride* ride)
{
    if (ride == nullptr)
        return;

    // Calculate how satisfying the ride is for the peep. Can range from -140 to +105.
    int16_t satisfaction = peep_calculate_ride_satisfaction(this, ride);

    // Update the satisfaction stat of the ride.
    uint8_t rideSatisfaction = 0;
    if (satisfaction >= 40)
        rideSatisfaction = 3;
    else if (satisfaction >= 20)
        rideSatisfaction = 2;
    else if (satisfaction >= 0)
        rideSatisfaction = 1;

    ride->UpdateSatisfaction(rideSatisfaction);

    // Update various peep stats.
    if (GuestNumRides < 255)
        GuestNumRides++;

    SetHasRidden(ride);
    peep_update_favourite_ride(this, ride);
    HappinessTarget = std::clamp(HappinessTarget + satisfaction, 0, PEEP_MAX_HAPPINESS);
    peep_update_ride_nausea_growth(this, ride);
}

/**
 *
 *  rct2: 0x0069576E
 */
void Guest::OnExitRide(Ride* ride)
{
    if (PeepFlags & PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE)
    {
        PeepFlags &= ~PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE;
        FavouriteRide = ride->id;
        // TODO fix this flag name or add another one
        WindowInvalidateFlags |= PEEP_INVALIDATE_STAFF_STATS;
    }
    Happiness = HappinessTarget;
    Nausea = NauseaTarget;
    WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_STATS;

    if (PeepFlags & PEEP_FLAGS_LEAVING_PARK)
        PeepFlags &= ~(PEEP_FLAGS_PARK_ENTRANCE_CHOSEN);

    if (ride != nullptr && peep_should_go_on_ride_again(this, ride))
    {
        GuestHeadingToRideId = ride->id;
        GuestIsLostCountdown = 200;
        ResetPathfindGoal();
        WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_ACTION;
    }

    if (peep_should_preferred_intensity_increase(this))
    {
        if (Intensity.GetMaximum() < 15)
        {
            Intensity = Intensity.WithMaximum(Intensity.GetMaximum() + 1);
        }
    }

    if (ride != nullptr && peep_really_liked_ride(this, ride))
    {
        InsertNewThought(PeepThoughtType::WasGreat, ride->id);

        static constexpr OpenRCT2::Audio::SoundId laughs[3] = {
            OpenRCT2::Audio::SoundId::Laugh1,
            OpenRCT2::Audio::SoundId::Laugh2,
            OpenRCT2::Audio::SoundId::Laugh3,
        };
        int32_t laughType = scenario_rand() & 7;
        if (laughType < 3)
        {
            OpenRCT2::Audio::Play3D(laughs[laughType], GetLocation());
        }
    }

    if (ride != nullptr)
    {
        ride->total_customers++;
        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;
    }
}

/**
 *
 *  rct2: 0x00695DD2
 */
void Guest::PickRideToGoOn()
{
    if (State != PeepState::Walking)
        return;
    if (!GuestHeadingToRideId.IsNull())
        return;
    if (PeepFlags & PEEP_FLAGS_LEAVING_PARK)
        return;
    if (HasFoodOrDrink())
        return;
    if (x == LOCATION_NULL)
        return;

    auto ride = FindBestRideToGoOn();
    if (ride != nullptr)
    {
        // Head to that ride
        GuestHeadingToRideId = ride->id;
        GuestIsLostCountdown = 200;
        ResetPathfindGoal();
        WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_ACTION;

        // Make peep look at their map if they have one
        if (HasItem(ShopItem::Map))
        {
            ReadMap();
        }
    }
}

Ride* Guest::FindBestRideToGoOn()
{
    // Pick the most exciting ride
    auto rideConsideration = FindRidesToGoOn();
    Ride* mostExcitingRide = nullptr;
    for (auto& ride : GetRideManager())
    {
        const auto rideIndex = ride.id.ToUnderlying();
        if (rideConsideration.size() > rideIndex && rideConsideration[rideIndex])
        {
            if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_QUEUE_FULL))
            {
                if (ShouldGoOnRide(&ride, StationIndex::FromUnderlying(0), false, true) && ride_has_ratings(&ride))
                {
                    if (mostExcitingRide == nullptr || ride.excitement > mostExcitingRide->excitement)
                    {
                        mostExcitingRide = &ride;
                    }
                }
            }
        }
    }
    return mostExcitingRide;
}

BitSet<OpenRCT2::Limits::MaxRidesInPark> Guest::FindRidesToGoOn()
{
    BitSet<OpenRCT2::Limits::MaxRidesInPark> rideConsideration;

    // FIX  Originally checked for a toy, likely a mistake and should be a map,
    //      but then again this seems to only allow the peep to go on
    //      rides they haven't been on before.
    if (HasItem(ShopItem::Map))
    {
        // Consider rides that peep hasn't been on yet
        for (auto& ride : GetRideManager())
        {
            if (!HasRidden(&ride))
            {
                rideConsideration[ride.id.ToUnderlying()] = true;
            }
        }
    }
    else
    {
        // Take nearby rides into consideration
        constexpr auto radius = 10 * 32;
        int32_t cx = floor2(x, 32);
        int32_t cy = floor2(y, 32);
        for (int32_t tileX = cx - radius; tileX <= cx + radius; tileX += COORDS_XY_STEP)
        {
            for (int32_t tileY = cy - radius; tileY <= cy + radius; tileY += COORDS_XY_STEP)
            {
                auto location = CoordsXY{ tileX, tileY };
                if (!map_is_location_valid(location))
                    continue;

                for (auto* trackElement : TileElementsView<TrackElement>(location))
                {
                    auto rideIndex = trackElement->GetRideIndex();
                    if (!rideIndex.IsNull())
                    {
                        rideConsideration[rideIndex.ToUnderlying()] = true;
                    }
                }
            }
        }

        // Always take the tall rides into consideration (realistic as you can usually see them from anywhere in the park)
        for (auto& ride : GetRideManager())
        {
            if (ride.highest_drop_height > 66 || ride.excitement >= RIDE_RATING(8, 00))
            {
                rideConsideration[ride.id.ToUnderlying()] = true;
            }
        }
    }

    return rideConsideration;
}

/**
 * This function is called whenever a peep is deciding whether or not they want
 * to go on a ride or visit a shop. They may be physically present at the
 * ride/shop, or they may just be thinking about it.
 *  rct2: 0x006960AB
 */
bool Guest::ShouldGoOnRide(Ride* ride, StationIndex entranceNum, bool atQueue, bool thinking)
{
    // Indicates whether a peep is physically at the ride, or is just thinking about going on the ride.
    bool peepAtRide = !thinking;

    if (ride->status == RideStatus::Open && !(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN))
    {
        // Peeps that are leaving the park will refuse to go on any rides, with the exception of free transport rides.
        assert(ride->type < std::size(RideTypeDescriptors));
        if (!ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_TRANSPORT_RIDE) || ride->value == RIDE_VALUE_UNDEFINED
            || ride_get_price(ride) != 0)
        {
            if (PeepFlags & PEEP_FLAGS_LEAVING_PARK)
            {
                ChoseNotToGoOnRide(ride, peepAtRide, false);
                return false;
            }
        }

        if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IS_SHOP))
        {
            return ShouldGoToShop(ride, peepAtRide);
        }

        // This used to check !(flags & 2), but the function is only ever called with flags = 0, 1 or 6.
        // This means we can use the existing !(flags & 4) check.
        if (peepAtRide)
        {
            auto& station = ride->GetStation(entranceNum);

            // Rides without queues can only have one peep waiting at a time.
            if (!atQueue)
            {
                if (!station.LastPeepInQueue.IsNull())
                {
                    peep_tried_to_enter_full_queue(this, ride);
                    return false;
                }
            }
            else
            {
                // Check if there's room in the queue for the peep to enter.
                Guest* lastPeepInQueue = GetEntity<Guest>(station.LastPeepInQueue);
                if (lastPeepInQueue != nullptr && (abs(lastPeepInQueue->z - z) <= 6))
                {
                    int32_t dx = abs(lastPeepInQueue->x - x);
                    int32_t dy = abs(lastPeepInQueue->y - y);
                    int32_t maxD = std::max(dx, dy);

                    // Unlike normal paths, peeps cannot overlap when queueing for a ride.
                    // This check enforces a minimum distance between peeps entering the queue.
                    if (maxD < 8)
                    {
                        peep_tried_to_enter_full_queue(this, ride);
                        return false;
                    }

                    // This checks if there's a peep standing still at the very end of the queue.
                    if (maxD <= 13 && lastPeepInQueue->TimeInQueue > 10)
                    {
                        peep_tried_to_enter_full_queue(this, ride);
                        return false;
                    }
                }
            }
        }

        // Assuming the queue conditions are met, peeps will always go on free transport rides.
        // Ride ratings, recent crashes and weather will all be ignored.
        money16 ridePrice = ride_get_price(ride);
        if (!ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_TRANSPORT_RIDE) || ride->value == RIDE_VALUE_UNDEFINED
            || ridePrice != 0)
        {
            if (PreviousRide == ride->id)
            {
                ChoseNotToGoOnRide(ride, peepAtRide, false);
                return false;
            }

            // Basic price checks
            if (ridePrice != 0 && !peep_has_voucher_for_free_ride(this, ride) && !(gParkFlags & PARK_FLAGS_NO_MONEY))
            {
                if (ridePrice > CashInPocket)
                {
                    if (peepAtRide)
                    {
                        if (CashInPocket <= 0)
                        {
                            InsertNewThought(PeepThoughtType::SpentMoney);
                        }
                        else
                        {
                            InsertNewThought(PeepThoughtType::CantAffordRide, ride->id);
                        }
                    }
                    ChoseNotToGoOnRide(ride, peepAtRide, true);
                    return false;
                }
            }

            // If happy enough, peeps will ignore the fact that a ride has recently crashed.
            if (ride->last_crash_type != RIDE_CRASH_TYPE_NONE && Happiness < 225)
            {
                if (peepAtRide)
                {
                    InsertNewThought(PeepThoughtType::NotSafe, ride->id);
                    if (HappinessTarget >= 64)
                    {
                        HappinessTarget -= 8;
                    }
                    ride->UpdatePopularity(0);
                }
                ChoseNotToGoOnRide(ride, peepAtRide, true);
                return false;
            }

            if (ride_has_ratings(ride))
            {
                // If a peep has already decided that they're going to go on a ride, they'll skip the weather and
                // excitement check and will only do a basic intensity check when they arrive at the ride itself.
                if (ride->id == GuestHeadingToRideId)
                {
                    if (ride->intensity > RIDE_RATING(10, 00) && !gCheatsIgnoreRideIntensity)
                    {
                        peep_ride_is_too_intense(this, ride, peepAtRide);
                        return false;
                    }
                }

                if (climate_is_raining() && !ShouldRideWhileRaining(*ride))
                {
                    if (peepAtRide)
                    {
                        InsertNewThought(PeepThoughtType::NotWhileRaining, ride->id);
                        if (HappinessTarget >= 64)
                        {
                            HappinessTarget -= 8;
                        }
                        ride->UpdatePopularity(0);
                    }
                    ChoseNotToGoOnRide(ride, peepAtRide, true);
                    return false;
                }

                if (!gCheatsIgnoreRideIntensity)
                {
                    // Intensity calculations. Even though the max intensity can go up to 15, it's capped
                    // at 10.0 (before happiness calculations). A full happiness bar will increase the max
                    // intensity and decrease the min intensity by about 2.5.
                    ride_rating maxIntensity = std::min(Intensity.GetMaximum() * 100, 1000) + Happiness;
                    ride_rating minIntensity = (Intensity.GetMinimum() * 100) - Happiness;
                    if (ride->intensity < minIntensity)
                    {
                        if (peepAtRide)
                        {
                            InsertNewThought(PeepThoughtType::MoreThrilling, ride->id);
                            if (HappinessTarget >= 64)
                            {
                                HappinessTarget -= 8;
                            }
                            ride->UpdatePopularity(0);
                        }
                        ChoseNotToGoOnRide(ride, peepAtRide, true);
                        return false;
                    }
                    if (ride->intensity > maxIntensity)
                    {
                        peep_ride_is_too_intense(this, ride, peepAtRide);
                        return false;
                    }

                    // Nausea calculations.
                    ride_rating maxNausea = NauseaMaximumThresholds[(EnumValue(NauseaTolerance) & 3)] + Happiness;

                    if (ride->nausea > maxNausea)
                    {
                        if (peepAtRide)
                        {
                            InsertNewThought(PeepThoughtType::Sickening, ride->id);
                            if (HappinessTarget >= 64)
                            {
                                HappinessTarget -= 8;
                            }
                            ride->UpdatePopularity(0);
                        }
                        ChoseNotToGoOnRide(ride, peepAtRide, true);
                        return false;
                    }

                    // Very nauseous peeps will only go on very gentle rides.
                    if (ride->nausea >= FIXED_2DP(1, 40) && Nausea > 160)
                    {
                        ChoseNotToGoOnRide(ride, peepAtRide, false);
                        return false;
                    }
                }
            }

            // If the ride has not yet been rated and is capable of having g-forces,
            // there's a 90% chance that the peep will ignore it.
            if (!ride_has_ratings(ride) && ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES))
            {
                if ((scenario_rand() & 0xFFFF) > 0x1999U)
                {
                    ChoseNotToGoOnRide(ride, peepAtRide, false);
                    return false;
                }

                if (!gCheatsIgnoreRideIntensity)
                {
                    if (ride->max_positive_vertical_g > FIXED_2DP(5, 00) || ride->max_negative_vertical_g < FIXED_2DP(-4, 00)
                        || ride->max_lateral_g > FIXED_2DP(4, 00))
                    {
                        ChoseNotToGoOnRide(ride, peepAtRide, false);
                        return false;
                    }
                }
            }

            uint32_t value = ride->value;

            // If the value of the ride hasn't yet been calculated, peeps will be willing to pay any amount for the ride.
            if (value != 0xFFFF && !peep_has_voucher_for_free_ride(this, ride) && !(gParkFlags & PARK_FLAGS_NO_MONEY))
            {
                // The amount peeps are willing to pay is decreased by 75% if they had to pay to enter the park.
                if (PeepFlags & PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY)
                    value /= 4;

                // Peeps won't pay more than twice the value of the ride.
                ridePrice = ride_get_price(ride);
                if (ridePrice > static_cast<money16>(value * 2))
                {
                    if (peepAtRide)
                    {
                        InsertNewThought(PeepThoughtType::BadValue, ride->id);
                        if (HappinessTarget >= 60)
                        {
                            HappinessTarget -= 16;
                        }
                        ride->UpdatePopularity(0);
                    }
                    ChoseNotToGoOnRide(ride, peepAtRide, true);
                    return false;
                }

                // A ride is good value if the price is 50% or less of the ride value and the peep didn't pay to enter the park.
                if (ridePrice <= static_cast<money16>(value / 2) && peepAtRide)
                {
                    if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
                    {
                        if (!(PeepFlags & PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY))
                        {
                            InsertNewThought(PeepThoughtType::GoodValue, ride->id);
                        }
                    }
                }
            }
        }

        // At this point, the peep has decided to go on the ride.
        if (peepAtRide)
        {
            ride->UpdatePopularity(1);
        }

        if (ride->id == GuestHeadingToRideId)
        {
            peep_reset_ride_heading(this);
        }

        ride->lifecycle_flags &= ~RIDE_LIFECYCLE_QUEUE_FULL;
        return true;
    }

    ChoseNotToGoOnRide(ride, peepAtRide, false);
    return false;
}

bool Guest::ShouldGoToShop(Ride* ride, bool peepAtShop)
{
    // Peeps won't go to the same shop twice in a row.
    if (ride->id == PreviousRide)
    {
        ChoseNotToGoOnRide(ride, peepAtShop, true);
        return false;
    }

    if (ride->type == RIDE_TYPE_TOILETS)
    {
        if (Toilet < 70)
        {
            ChoseNotToGoOnRide(ride, peepAtShop, true);
            return false;
        }

        // The amount that peeps are willing to pay to use the Toilets scales with their toilet stat.
        // It effectively has a minimum of $0.10 (due to the check above) and a maximum of $0.60.
        if (ride_get_price(ride) * 40 > Toilet)
        {
            if (peepAtShop)
            {
                InsertNewThought(PeepThoughtType::NotPaying, ride->id);
                if (HappinessTarget >= 60)
                {
                    HappinessTarget -= 16;
                }
                ride->UpdatePopularity(0);
            }
            ChoseNotToGoOnRide(ride, peepAtShop, true);
            return false;
        }
    }

    if (ride->type == RIDE_TYPE_FIRST_AID)
    {
        if (Nausea < 128)
        {
            ChoseNotToGoOnRide(ride, peepAtShop, true);
            return false;
        }
    }

    // Basic price checks
    auto ridePrice = ride_get_price(ride);
    if (ridePrice != 0 && ridePrice > CashInPocket)
    {
        if (peepAtShop)
        {
            if (CashInPocket <= 0)
            {
                InsertNewThought(PeepThoughtType::SpentMoney);
            }
            else
            {
                InsertNewThought(PeepThoughtType::CantAffordRide, ride->id);
            }
        }
        ChoseNotToGoOnRide(ride, peepAtShop, true);
        return false;
    }

    if (peepAtShop)
    {
        ride->UpdatePopularity(1);
        if (ride->id == GuestHeadingToRideId)
        {
            peep_reset_ride_heading(this);
        }
    }
    return true;
}

// Used when no logging to an expend type required
void Guest::SpendMoney(money32 amount, ExpenditureType expenditure)
{
    money16 unused;
    SpendMoney(unused, amount, expenditure);
}

/**
 *
 *  rct2: 0x0069926C
 * Expend type was previously an offset saved in 0x00F1AEC0
 */
void Guest::SpendMoney(money16& peep_expend_type, money32 amount, ExpenditureType expenditure)
{
    assert(!(gParkFlags & PARK_FLAGS_NO_MONEY));

    CashInPocket = std::max(0, CashInPocket - amount);
    CashSpent += amount;

    peep_expend_type += static_cast<money16>(amount);

    window_invalidate_by_number(WC_PEEP, sprite_index);

    finance_payment(-amount, expenditure);

    if (gConfigGeneral.show_guest_purchases && !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
    {
        // HACK Currently disabled for multiplayer due to limitation of all sprites
        //      needing to be synchronised
        if (network_get_mode() == NETWORK_MODE_NONE && !gOpenRCT2Headless)
        {
            MoneyEffect::CreateAt(amount, GetLocation(), true);
        }
    }

    OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::Purchase, GetLocation());
}

void Guest::SetHasRidden(const Ride* ride)
{
    OpenRCT2::RideUse::GetHistory().Add(sprite_index, ride->id);

    SetHasRiddenRideType(ride->type);
}

bool Guest::HasRidden(const Ride* ride) const
{
    return OpenRCT2::RideUse::GetHistory().Contains(sprite_index, ride->id);
}

void Guest::SetHasRiddenRideType(int32_t rideType)
{
    OpenRCT2::RideUse::GetTypeHistory().Add(sprite_index, rideType);
}

bool Guest::HasRiddenRideType(int32_t rideType) const
{
    return OpenRCT2::RideUse::GetTypeHistory().Contains(sprite_index, rideType);
}

void Guest::SetParkEntryTime(int32_t entryTime)
{
    ParkEntryTime = entryTime;
}

int32_t Guest::GetParkEntryTime() const
{
    return ParkEntryTime;
}

bool Guest::ShouldRideWhileRaining(const Ride& ride)
{
    // Peeps will go on rides that are sufficiently undercover while it's raining.
    // The threshold is fairly low and only requires about 10-15% of the ride to be undercover.
    if (ride.sheltered_eighths >= 3)
    {
        return true;
    }

    // Peeps with umbrellas will go on rides where they can use their umbrella on it (like the Maze) 50% of the time
    if (HasItem(ShopItem::Umbrella) && ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_PEEP_CAN_USE_UMBRELLA)
        && (scenario_rand() & 2) == 0)
    {
        return true;
    }

    return false;
}

void Guest::ChoseNotToGoOnRide(Ride* ride, bool peepAtRide, bool updateLastRide)
{
    if (peepAtRide && updateLastRide)
    {
        PreviousRide = ride->id;
        PreviousRideTimeOut = 0;
    }

    if (ride->id == GuestHeadingToRideId)
    {
        peep_reset_ride_heading(this);
    }
}

void Guest::ReadMap()
{
    if (IsActionInterruptable())
    {
        Action = PeepActionType::ReadMap;
        ActionFrame = 0;
        ActionSpriteImageOffset = 0;
        UpdateCurrentActionSpriteType();
    }
}

static bool peep_has_voucher_for_free_ride(Guest* peep, Ride* ride)
{
    return peep->HasItem(ShopItem::Voucher) && peep->VoucherType == VOUCHER_TYPE_RIDE_FREE && peep->VoucherRideId == ride->id;
}

/**
 * When the queue is full, peeps will ignore the ride when thinking about what to go on next.
 * Does not effect peeps that walk up to the queue entrance.
 * This flag is reset the next time a peep successfully joins the queue.
 */
static void peep_tried_to_enter_full_queue(Guest* peep, Ride* ride)
{
    ride->lifecycle_flags |= RIDE_LIFECYCLE_QUEUE_FULL;
    peep->PreviousRide = ride->id;
    peep->PreviousRideTimeOut = 0;
    // Change status "Heading to" to "Walking" if queue is full
    if (ride->id == peep->GuestHeadingToRideId)
    {
        peep_reset_ride_heading(peep);
    }
}

static void peep_reset_ride_heading(Guest* peep)
{
    peep->GuestHeadingToRideId = RideId::GetNull();
    peep->WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_ACTION;
}

static void peep_ride_is_too_intense(Guest* peep, Ride* ride, bool peepAtRide)
{
    if (peepAtRide)
    {
        peep->InsertNewThought(PeepThoughtType::Intense, ride->id);
        if (peep->HappinessTarget >= 64)
        {
            peep->HappinessTarget -= 8;
        }
        ride->UpdatePopularity(0);
    }
    peep->ChoseNotToGoOnRide(ride, peepAtRide, true);
}

/**
 *
 *  rct2: 0x00691C6E
 */
static Vehicle* peep_choose_car_from_ride(Peep* peep, Ride* ride, std::vector<uint8_t>& car_array)
{
    uint8_t chosen_car = scenario_rand();
    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_G_FORCES) && ((chosen_car & 0xC) != 0xC))
    {
        chosen_car = (scenario_rand() & 1) ? 0 : static_cast<uint8_t>(car_array.size()) - 1;
    }
    else
    {
        chosen_car = (chosen_car * static_cast<uint16_t>(car_array.size())) >> 8;
    }

    peep->CurrentCar = car_array[chosen_car];

    Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[peep->CurrentTrain]);
    if (vehicle == nullptr)
    {
        return nullptr;
    }
    return vehicle->GetCar(peep->CurrentCar);
}

/**
 *
 *  rct2: 0x00691CD1
 */
static void peep_choose_seat_from_car(Peep* peep, Ride* ride, Vehicle* vehicle)
{
    using namespace OpenRCT2::RideModes;
    const auto* rideMode = GetRideMode(ride->mode);
    rideMode->PeepChooseSeatFromCar(peep, ride, vehicle);
}

/**
 *
 *  rct2: 0x00691D27
 */
void Guest::GoToRideEntrance(Ride* ride)
{
    const auto& station = ride->GetStation(CurrentRideStation);
    if (station.Entrance.IsNull())
    {
        RemoveFromQueue();
        return;
    }

    auto location = station.Entrance.ToCoordsXYZD().ToTileCentre();
    int16_t x_shift = DirectionOffsets[location.direction].x;
    int16_t y_shift = DirectionOffsets[location.direction].y;

    uint8_t shift_multiplier = 21;
    rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);
    if (rideEntry != nullptr)
    {
        if (rideEntry->vehicles[rideEntry->default_vehicle].flags & VEHICLE_ENTRY_FLAG_MINI_GOLF
            || rideEntry->vehicles[rideEntry->default_vehicle].flags
                & (VEHICLE_ENTRY_FLAG_CHAIRLIFT | VEHICLE_ENTRY_FLAG_GO_KART))
        {
            shift_multiplier = 32;
        }
    }

    x_shift *= shift_multiplier;
    y_shift *= shift_multiplier;

    location.x += x_shift;
    location.y += y_shift;

    SetDestination(location, 2);
    SetState(PeepState::EnteringRide);
    RideSubState = PeepRideSubState::InEntrance;

    RejoinQueueTimeout = 0;
    GuestTimeOnRide = 0;

    RemoveFromQueue();
}

bool Guest::FindVehicleToEnter(Ride* ride, std::vector<uint8_t>& car_array)
{
    using namespace OpenRCT2::RideModes;
    const auto* rideMode = GetRideMode(ride->mode);
    return rideMode->FindVehicleToEnter(this, ride, car_array);
}

static void peep_update_ride_at_entrance_try_leave(Guest* peep)
{
    // Destination Tolerance is zero when peep has completely
    // entered entrance
    if (peep->DestinationTolerance == 0)
    {
        peep->RemoveFromQueue();
        peep->SetState(PeepState::Falling);
    }
}

static bool peep_check_ride_price_at_entrance(Guest* peep, Ride* ride, money32 ridePrice)
{
    if ((peep->HasItem(ShopItem::Voucher)) && peep->VoucherType == VOUCHER_TYPE_RIDE_FREE
        && peep->VoucherRideId == peep->CurrentRide)
        return true;

    if (peep->CashInPocket <= 0 && !(gParkFlags & PARK_FLAGS_NO_MONEY))
    {
        peep->InsertNewThought(PeepThoughtType::SpentMoney);
        peep_update_ride_at_entrance_try_leave(peep);
        return false;
    }

    if (ridePrice > peep->CashInPocket)
    {
        peep->InsertNewThought(PeepThoughtType::CantAffordRide, peep->CurrentRide);
        peep_update_ride_at_entrance_try_leave(peep);
        return false;
    }

    uint16_t value = ride->value;
    if (value != RIDE_VALUE_UNDEFINED)
    {
        if (value * 2 < ridePrice)
        {
            peep->InsertNewThought(PeepThoughtType::BadValue, peep->CurrentRide);
            peep_update_ride_at_entrance_try_leave(peep);
            return false;
        }
    }
    return true;
}

/**
 * The satisfaction values calculated here are used to determine how happy the peep is with the ride,
 * and also affects the satisfaction stat of the ride itself. The factors that affect satisfaction include:
 * - The price of the ride compared to the ride's value
 * - How closely the intensity and nausea of the ride matches the peep's preferences
 * - How long the peep was waiting in the queue
 * - If the peep has been on the ride before, or on another ride of the same type
 */
static int16_t peep_calculate_ride_satisfaction(Guest* peep, Ride* ride)
{
    int16_t satisfaction = peep_calculate_ride_value_satisfaction(peep, ride);
    satisfaction += peep_calculate_ride_intensity_nausea_satisfaction(peep, ride);

    // Calculate satisfaction based on how long the peep has been in the queue for.
    // (For comparison: peeps start thinking "I've been queueing for a long time" at 3500 and
    // start leaving the queue at 4300.)
    if (peep->TimeInQueue >= 4500)
        satisfaction -= 35;
    else if (peep->TimeInQueue >= 2250)
        satisfaction -= 10;
    else if (peep->TimeInQueue <= 750)
        satisfaction += 10;

    // Peeps get a small boost in satisfaction if they've been on a ride of the same type before,
    // and this boost is doubled if they've already been on this particular ride.
    if (peep->HasRiddenRideType(ride->type))
        satisfaction += 10;

    if (peep->HasRidden(get_ride(peep->CurrentRide)))
        satisfaction += 10;

    return satisfaction;
}

/**
 * Check to see if the specified ride should become the peep's favourite.
 * For this, a "ride rating" is calculated based on the excitement of the ride and the peep's current happiness.
 * As this value cannot exceed 255, the happier the peep is, the more irrelevant the ride's excitement becomes.
 * Due to the minimum happiness requirement, an excitement rating of more than 3.8 has no further effect.
 *
 * If the ride rating is higher than any ride the peep has already been on and the happiness criteria is met,
 * the ride becomes the peep's favourite. (This doesn't happen right away, but will be updated once the peep
 * exits the ride.)
 */
static void peep_update_favourite_ride(Guest* peep, Ride* ride)
{
    peep->PeepFlags &= ~PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE;
    uint8_t peepRideRating = std::clamp((ride->excitement / 4) + peep->Happiness, 0, PEEP_MAX_HAPPINESS);
    if (peepRideRating >= peep->FavouriteRideRating)
    {
        if (peep->Happiness >= 160 && peep->HappinessTarget >= 160)
        {
            peep->FavouriteRideRating = peepRideRating;
            peep->PeepFlags |= PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE;
        }
    }
}

/* rct2: 0x00695555 */
static int16_t peep_calculate_ride_value_satisfaction(Guest* peep, Ride* ride)
{
    if (gParkFlags & PARK_FLAGS_NO_MONEY)
    {
        return -30;
    }

    if (ride->value == RIDE_VALUE_UNDEFINED)
    {
        return -30;
    }

    money16 ridePrice = ride_get_price(ride);
    if (ride->value >= ridePrice)
    {
        return -5;
    }

    if ((ride->value + ((ride->value * peep->Happiness) / 256)) >= ridePrice)
    {
        return -30;
    }

    return 0;
}

/**
 * Calculate satisfaction based on the intensity and nausea of the ride.
 * The best possible score from this section is achieved by having the intensity and nausea
 * of the ride fall exactly within the peep's preferences, but lower scores can still be achieved
 * if the peep's happiness is enough to offset it.
 */
static int16_t peep_calculate_ride_intensity_nausea_satisfaction(Guest* peep, Ride* ride)
{
    if (!ride_has_ratings(ride))
    {
        return 70;
    }

    uint8_t intensitySatisfaction = 3;
    uint8_t nauseaSatisfaction = 3;
    ride_rating maxIntensity = peep->Intensity.GetMaximum() * 100;
    ride_rating minIntensity = peep->Intensity.GetMinimum() * 100;
    if (minIntensity <= ride->intensity && maxIntensity >= ride->intensity)
    {
        intensitySatisfaction--;
    }
    minIntensity -= peep->Happiness * 2;
    maxIntensity += peep->Happiness;
    if (minIntensity <= ride->intensity && maxIntensity >= ride->intensity)
    {
        intensitySatisfaction--;
    }
    minIntensity -= peep->Happiness * 2;
    maxIntensity += peep->Happiness;
    if (minIntensity <= ride->intensity && maxIntensity >= ride->intensity)
    {
        intensitySatisfaction--;
    }

    // Although it's not shown in the interface, a peep with Average or High nausea tolerance
    // has a minimum preferred nausea value. (For peeps with None or Low, this is set to zero.)
    ride_rating minNausea = NauseaMinimumThresholds[(EnumValue(peep->NauseaTolerance) & 3)];
    ride_rating maxNausea = NauseaMaximumThresholds[(EnumValue(peep->NauseaTolerance) & 3)];
    if (minNausea <= ride->nausea && maxNausea >= ride->nausea)
    {
        nauseaSatisfaction--;
    }
    minNausea -= peep->Happiness * 2;
    maxNausea += peep->Happiness;
    if (minNausea <= ride->nausea && maxNausea >= ride->nausea)
    {
        nauseaSatisfaction--;
    }
    minNausea -= peep->Happiness * 2;
    maxNausea += peep->Happiness;
    if (minNausea <= ride->nausea && maxNausea >= ride->nausea)
    {
        nauseaSatisfaction--;
    }

    uint8_t highestSatisfaction = std::max(intensitySatisfaction, nauseaSatisfaction);
    uint8_t lowestSatisfaction = std::min(intensitySatisfaction, nauseaSatisfaction);

    switch (highestSatisfaction)
    {
        default:
        case 0:
            return 70;
        case 1:
            switch (lowestSatisfaction)
            {
                default:
                case 0:
                    return 50;
                case 1:
                    return 35;
            }
        case 2:
            switch (lowestSatisfaction)
            {
                default:
                case 0:
                    return 35;
                case 1:
                    return 20;
                case 2:
                    return 10;
            }
        case 3:
            switch (lowestSatisfaction)
            {
                default:
                case 0:
                    return -35;
                case 1:
                    return -50;
                case 2:
                    return -60;
                case 3:
                    return -60;
            }
    }
}

/**
 * Update the nausea growth of the peep based on a ride. This is calculated based on:
 * - The nausea rating of the ride
 * - Their new happiness growth rate (the higher, the less nauseous)
 * - How hungry the peep is (+0% nausea at 50% hunger up to +100% nausea at 100% hunger)
 * - The peep's nausea tolerance (Final modifier: none: 100%, low: 50%, average: 25%, high: 12.5%)
 */
static void peep_update_ride_nausea_growth(Guest* peep, Ride* ride)
{
    uint32_t nauseaMultiplier = std::clamp(256 - peep->HappinessTarget, 64, 200);
    uint32_t nauseaGrowthRateChange = (ride->nausea * nauseaMultiplier) / 512;
    nauseaGrowthRateChange *= std::max(static_cast<uint8_t>(128), peep->Hunger) / 64;
    nauseaGrowthRateChange >>= (EnumValue(peep->NauseaTolerance) & 3);
    peep->NauseaTarget = static_cast<uint8_t>(std::min(peep->NauseaTarget + nauseaGrowthRateChange, 255u));
}

static bool peep_should_go_on_ride_again(Guest* peep, Ride* ride)
{
    if (!ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_PEEP_WILL_RIDE_AGAIN))
        return false;
    if (!ride_has_ratings(ride))
        return false;
    if (ride->intensity > RIDE_RATING(10, 00) && !gCheatsIgnoreRideIntensity)
        return false;
    if (peep->Happiness < 180)
        return false;
    if (peep->Energy < 100)
        return false;
    if (peep->Nausea > 160)
        return false;
    if (peep->Hunger < 30)
        return false;
    if (peep->Thirst < 20)
        return false;
    if (peep->Toilet > 170)
        return false;

    uint8_t r = (scenario_rand() & 0xFF);
    if (r <= 128)
    {
        if (peep->GuestNumRides > 7)
            return false;
        if (r > 64)
            return false;
    }

    return true;
}

static bool peep_should_preferred_intensity_increase(Guest* peep)
{
    if (gParkFlags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES)
        return false;
    if (peep->Happiness < 200)
        return false;

    return (scenario_rand() & 0xFF) >= static_cast<uint8_t>(peep->Intensity);
}

static bool peep_really_liked_ride(Guest* peep, Ride* ride)
{
    if (peep->Happiness < 215)
        return false;
    if (peep->Nausea > 120)
        return false;
    if (!ride_has_ratings(ride))
        return false;
    if (ride->intensity > RIDE_RATING(10, 00) && !gCheatsIgnoreRideIntensity)
        return false;
    return true;
}

/**
 *
 *  rct2: 0x0069BC9A
 */
static PeepThoughtType peep_assess_surroundings(int16_t centre_x, int16_t centre_y, int16_t centre_z)
{
    if ((tile_element_height({ centre_x, centre_y })) > centre_z)
        return PeepThoughtType::None;

    uint16_t num_scenery = 0;
    uint16_t num_fountains = 0;
    uint16_t nearby_music = 0;
    uint16_t num_rubbish = 0;

    int16_t initial_x = std::max(centre_x - 160, 0);
    int16_t initial_y = std::max(centre_y - 160, 0);
    int16_t final_x = std::min(centre_x + 160, MAXIMUM_MAP_SIZE_BIG);
    int16_t final_y = std::min(centre_y + 160, MAXIMUM_MAP_SIZE_BIG);

    for (int16_t x = initial_x; x < final_x; x += COORDS_XY_STEP)
    {
        for (int16_t y = initial_y; y < final_y; y += COORDS_XY_STEP)
        {
            for (auto* tileElement : TileElementsView({ x, y }))
            {
                Ride* ride;

                switch (tileElement->GetType())
                {
                    case TileElementType::Path:
                    {
                        if (!tileElement->AsPath()->HasAddition())
                            break;

                        auto* pathAddEntry = tileElement->AsPath()->GetAdditionEntry();
                        if (pathAddEntry == nullptr)
                        {
                            return PeepThoughtType::None;
                        }
                        if (tileElement->AsPath()->AdditionIsGhost())
                            break;

                        if (pathAddEntry->flags & (PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER | PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW))
                        {
                            num_fountains++;
                            break;
                        }
                        if (tileElement->AsPath()->IsBroken())
                        {
                            num_rubbish++;
                        }
                        break;
                    }
                    case TileElementType::LargeScenery:
                    case TileElementType::SmallScenery:
                        num_scenery++;
                        break;
                    case TileElementType::Track:
                    {
                        ride = get_ride(tileElement->AsTrack()->GetRideIndex());
                        if (ride == nullptr)
                            break;

                        bool isPlayingMusic = ride->lifecycle_flags & RIDE_LIFECYCLE_MUSIC && ride->status != RideStatus::Closed
                            && !(ride->lifecycle_flags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED));
                        if (!isPlayingMusic)
                            break;

                        const auto* musicObject = ride->GetMusicObject();
                        if (musicObject == nullptr)
                            break;

                        if (musicObject->GetNiceFactor() == MusicNiceFactor::Nice)
                        {
                            nearby_music |= 1;
                        }
                        else if (musicObject->GetNiceFactor() == MusicNiceFactor::Overbearing)
                        {
                            nearby_music |= 2;
                        }

                        break;
                    }
                    default:
                        break;
                }
            }
        }
    }

    for (auto litter : EntityList<Litter>())
    {
        int16_t dist_x = abs(litter->x - centre_x);
        int16_t dist_y = abs(litter->y - centre_y);
        if (std::max(dist_x, dist_y) <= 160)
        {
            num_rubbish++;
        }
    }

    if (num_fountains >= 5 && num_rubbish < 20)
        return PeepThoughtType::Fountains;

    if (num_scenery >= 40 && num_rubbish < 8)
        return PeepThoughtType::Scenery;

    if (nearby_music == 1 && num_rubbish < 20)
        return PeepThoughtType::Music;

    if (num_rubbish < 2 && !gCheatsDisableLittering)
        // if disable littering cheat is enabled, peeps will not have the "clean and tidy park" thought
        return PeepThoughtType::VeryClean;

    return PeepThoughtType::None;
}

/**
 *
 *  rct2: 0x0068F9A9
 */
static void peep_update_hunger(Guest* peep)
{
    if (peep->Hunger >= 3)
    {
        peep->Hunger -= 2;

        peep->EnergyTarget = std::min(peep->EnergyTarget + 2, PEEP_MAX_ENERGY_TARGET);
        peep->Toilet = std::min(peep->Toilet + 1, 255);
    }
}

/**
 * Main purpose is to decide when peeps leave the park due to
 * low happiness, low energy and (if appropriate) low money.
 *
 *  rct2: 0x0068F8CD
 */
static void peep_decide_whether_to_leave_park(Guest* peep)
{
    if (peep->EnergyTarget >= 33)
    {
        peep->EnergyTarget -= 2;
    }

    if (gClimateCurrent.Temperature >= 21 && peep->Thirst >= 5)
    {
        peep->Thirst--;
    }

    if (peep->OutsideOfPark)
    {
        return;
    }

    /* Peeps that are happy enough, have enough energy and
     * (if appropriate) have enough money will always stay
     * in the park. */
    if (!(peep->PeepFlags & PEEP_FLAGS_LEAVING_PARK))
    {
        if (gParkFlags & PARK_FLAGS_NO_MONEY)
        {
            if (peep->Energy >= 70 && peep->Happiness >= 60)
            {
                return;
            }
        }
        else
        {
            if (peep->Energy >= 55 && peep->Happiness >= 45 && peep->CashInPocket >= 5.00_GBP)
            {
                return;
            }
        }
    }

    // Approx 95% chance of staying in the park
    if ((scenario_rand() & 0xFFFF) > 3276)
    {
        return;
    }

    // In the remaining 5% chance the peep leaves the park.
    peep_leave_park(peep);
}

/**
 *
 *  rct2: 0x0068F93E
 */
static void peep_leave_park(Guest* peep)
{
    peep->GuestHeadingToRideId = RideId::GetNull();
    if (peep->PeepFlags & PEEP_FLAGS_LEAVING_PARK)
    {
        if (peep->GuestIsLostCountdown < 60)
        {
            return;
        }
    }
    else
    {
        peep->GuestIsLostCountdown = 254;
        peep->PeepFlags |= PEEP_FLAGS_LEAVING_PARK;
        peep->PeepFlags &= ~PEEP_FLAGS_PARK_ENTRANCE_CHOSEN;
    }

    peep->InsertNewThought(PeepThoughtType::GoHome);

    rct_window* w = window_find_by_number(WC_PEEP, peep->sprite_index);
    if (w != nullptr)
        window_event_invalidate_call(w);
    window_invalidate_by_number(WC_PEEP, peep->sprite_index);
}

template<typename T> static void peep_head_for_nearest_ride(Guest* peep, bool considerOnlyCloseRides, T predicate)
{
    if (peep->State != PeepState::Sitting && peep->State != PeepState::Watching && peep->State != PeepState::Walking)
    {
        return;
    }
    if (peep->PeepFlags & PEEP_FLAGS_LEAVING_PARK)
        return;
    if (peep->x == LOCATION_NULL)
        return;
    if (!peep->GuestHeadingToRideId.IsNull())
    {
        auto ride = get_ride(peep->GuestHeadingToRideId);
        if (ride != nullptr && predicate(*ride))
        {
            return;
        }
    }

    BitSet<OpenRCT2::Limits::MaxRidesInPark> rideConsideration;
    if (!considerOnlyCloseRides && (peep->HasItem(ShopItem::Map)))
    {
        // Consider all rides in the park
        for (const auto& ride : GetRideManager())
        {
            if (predicate(ride))
            {
                rideConsideration[ride.id.ToUnderlying()] = true;
            }
        }
    }
    else
    {
        // Take nearby rides into consideration
        constexpr auto searchRadius = 10 * 32;
        int32_t cx = floor2(peep->x, 32);
        int32_t cy = floor2(peep->y, 32);
        for (auto x = cx - searchRadius; x <= cx + searchRadius; x += COORDS_XY_STEP)
        {
            for (auto y = cy - searchRadius; y <= cy + searchRadius; y += COORDS_XY_STEP)
            {
                auto location = CoordsXY{ x, y };
                if (!map_is_location_valid(location))
                    continue;

                for (auto* trackElement : TileElementsView<TrackElement>(location))
                {
                    auto rideIndex = trackElement->GetRideIndex();
                    auto ride = get_ride(rideIndex);
                    if (ride == nullptr)
                        continue;

                    if (!predicate(*ride))
                        continue;

                    rideConsideration[ride->id.ToUnderlying()] = true;
                }
            }
        }
    }

    // Filter the considered rides
    RideId potentialRides[OpenRCT2::Limits::MaxRidesInPark];
    size_t numPotentialRides = 0;
    for (auto& ride : GetRideManager())
    {
        if (rideConsideration[ride.id.ToUnderlying()])
        {
            if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_QUEUE_FULL))
            {
                if (peep->ShouldGoOnRide(&ride, StationIndex::FromUnderlying(0), false, true))
                {
                    potentialRides[numPotentialRides++] = ride.id;
                }
            }
        }
    }

    // Pick the closest ride
    Ride* closestRide{};
    auto closestRideDistance = std::numeric_limits<int32_t>::max();
    for (size_t i = 0; i < numPotentialRides; i++)
    {
        auto ride = get_ride(potentialRides[i]);
        if (ride != nullptr)
        {
            auto rideLocation = ride->GetStation().Start;
            int32_t distance = abs(rideLocation.x - peep->x) + abs(rideLocation.y - peep->y);
            if (distance < closestRideDistance)
            {
                closestRide = ride;
                closestRideDistance = distance;
            }
        }
    }
    if (closestRide != nullptr)
    {
        // Head to that ride
        peep->GuestHeadingToRideId = closestRide->id;
        peep->GuestIsLostCountdown = 200;
        peep->ResetPathfindGoal();
        peep->WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_ACTION;
        peep->TimeLost = 0;
    }
}

static void peep_head_for_nearest_ride_type(Guest* peep, int32_t rideType)
{
    auto considerOnlyCloseRides = rideType == RIDE_TYPE_FIRST_AID;
    return peep_head_for_nearest_ride(
        peep, considerOnlyCloseRides, [rideType](const Ride& ride) { return ride.type == rideType; });
}

static void PeepHeadForNearestRideWithFlags(Guest* peep, int64_t rideTypeFlags)
{
    if ((rideTypeFlags & RIDE_TYPE_FLAG_IS_TOILET) && peep->HasFoodOrDrink())
    {
        return;
    }
    peep_head_for_nearest_ride(
        peep, false, [rideTypeFlags](const Ride& ride) { return ride.GetRideTypeDescriptor().HasFlag(rideTypeFlags); });
}

/**
 *
 *  rct2: 0x00699FE3
 * Stops peeps that are having thoughts
 * such as "I'm hungry" after visiting a food shop.
 * Works for Thirst/Hungry/Low Money/Toilet
 */
void Guest::StopPurchaseThought(uint8_t ride_type)
{
    auto thoughtType = PeepThoughtType::Hungry;

    const auto& rtd = GetRideTypeDescriptor(ride_type);
    if (!rtd.HasFlag(RIDE_TYPE_FLAG_SELLS_FOOD))
    {
        thoughtType = PeepThoughtType::Thirsty;
        if (!rtd.HasFlag(RIDE_TYPE_FLAG_SELLS_DRINKS))
        {
            thoughtType = PeepThoughtType::RunningOut;
            if (!rtd.HasFlag(RIDE_TYPE_FLAG_IS_CASH_MACHINE))
            {
                thoughtType = PeepThoughtType::Toilet;
                if (!rtd.HasFlag(RIDE_TYPE_FLAG_IS_TOILET))
                {
                    return;
                }
            }
        }
    }

    // Remove the related thought
    for (int32_t i = 0; i < PEEP_MAX_THOUGHTS; ++i)
    {
        PeepThought* thought = &Thoughts[i];

        if (thought->type == PeepThoughtType::None)
            break;

        if (thought->type != thoughtType)
            continue;

        if (i < PEEP_MAX_THOUGHTS - 1)
        {
            memmove(thought, thought + 1, sizeof(PeepThought) * (PEEP_MAX_THOUGHTS - i - 1));
        }

        Thoughts[PEEP_MAX_THOUGHTS - 1].type = PeepThoughtType::None;

        WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_THOUGHTS;
        i--;
    }
}

/**
 *
 *  rct2: 0x0069AEB7
 */
static bool peep_should_use_cash_machine(Guest* peep, RideId rideIndex)
{
    if (gParkFlags & PARK_FLAGS_NO_MONEY)
        return false;
    if (peep->PeepFlags & PEEP_FLAGS_LEAVING_PARK)
        return false;
    if (peep->CashInPocket > 20.00_GBP)
        return false;
    if (115 + (scenario_rand() % 128) > peep->Happiness)
        return false;
    if (peep->Energy < 80)
        return false;

    auto ride = get_ride(rideIndex);
    if (ride != nullptr)
    {
        ride->UpdateSatisfaction(peep->Happiness >> 6);
        ride->cur_num_customers++;
        ride->total_customers++;
        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;
    }
    return true;
}

/**
 *
 *  rct2: 0x006912A3
 */
void Guest::UpdateBuying()
{
    if (!CheckForPath())
        return;

    auto ride = get_ride(CurrentRide);
    if (ride == nullptr || ride->status != RideStatus::Open)
    {
        SetState(PeepState::Falling);
        return;
    }

    if (SubState == 1)
    {
        if (!IsActionWalking())
        {
            UpdateAction();
            Invalidate();
            return;
        }

        const auto& rtd = GetRideTypeDescriptor(ride->type);
        if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_CASH_MACHINE))
        {
            if (CurrentRide != PreviousRide)
            {
                CashInPocket += 50.00_GBP;
            }
            window_invalidate_by_number(WC_PEEP, sprite_index);
        }
        sprite_direction ^= 0x10;

        auto destination = CoordsXY{ 16, 16 } + NextLoc;
        SetDestination(destination);
        PeepDirection = direction_reverse(PeepDirection);

        SetState(PeepState::Walking);
        return;
    }

    bool item_bought = false;

    if (CurrentRide != PreviousRide)
    {
        const auto& rtd = GetRideTypeDescriptor(ride->type);
        if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_CASH_MACHINE))
        {
            item_bought = peep_should_use_cash_machine(this, CurrentRide);
            if (!item_bought)
            {
                PreviousRide = CurrentRide;
                PreviousRideTimeOut = 0;
            }
            else
            {
                Action = PeepActionType::WithdrawMoney;
                ActionFrame = 0;
                ActionSpriteImageOffset = 0;

                UpdateCurrentActionSpriteType();

                ride->no_primary_items_sold++;
            }
        }
        else
        {
            rct_ride_entry* ride_type = get_ride_entry(ride->subtype);
            if (ride_type == nullptr)
            {
                return;
            }
            if (ride_type->shop_item[1] != ShopItem::None)
            {
                money16 price = ride->price[1];

                item_bought = DecideAndBuyItem(ride, ride_type->shop_item[1], price);
                if (item_bought)
                {
                    ride->no_secondary_items_sold++;
                }
            }

            if (!item_bought && ride_type->shop_item[0] != ShopItem::None)
            {
                money16 price = ride->price[0];

                item_bought = DecideAndBuyItem(ride, ride_type->shop_item[0], price);
                if (item_bought)
                {
                    ride->no_primary_items_sold++;
                }
            }
        }
    }

    if (item_bought)
    {
        ride->UpdatePopularity(1);

        StopPurchaseThought(ride->type);
    }
    else
    {
        ride->UpdatePopularity(0);
    }
    SubState = 1;
}

/**
 *
 *  rct2: 0x00691A3B
 */
void Guest::UpdateRideAtEntrance()
{
    auto ride = get_ride(CurrentRide);
    if (ride == nullptr)
        return;

    // The peep will keep advancing in the entranceway
    // whilst in this state. When it has reached the very
    // front of the queue destination tolerance is set to
    // zero to indicate it is final decision time (try_leave will pass).
    // When a peep has to return to the queue without getting on a ride
    // this is the state it will return to.
    if (DestinationTolerance != 0)
    {
        int16_t xy_distance;
        if (auto loc = UpdateAction(xy_distance); loc.has_value())
        {
            int16_t actionZ = z;
            if (xy_distance < 16)
            {
                const auto& station = ride->GetStation(CurrentRideStation);
                auto entrance = station.Entrance.ToCoordsXYZ();
                actionZ = entrance.z + 2;
            }
            MoveTo({ loc.value(), actionZ });
        }
        else
        {
            DestinationTolerance = 0;
            sprite_direction ^= (1 << 4);
            Invalidate();
        }
    }

    std::vector<uint8_t> carArray;

    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES))
    {
        if (ride->num_riders >= ride->operation_option)
            return;
    }
    else
    {
        if (!FindVehicleToEnter(ride, carArray))
            return;
    }

    if (ride->status != RideStatus::Open || ride->vehicle_change_timeout != 0)
    {
        peep_update_ride_at_entrance_try_leave(this);
        return;
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
        return;

    money16 ridePrice = ride_get_price(ride);
    if (ridePrice != 0)
    {
        if (!peep_check_ride_price_at_entrance(this, ride, ridePrice))
            return;
    }

    if (!ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES))
    {
        Vehicle* vehicle = peep_choose_car_from_ride(this, ride, carArray);
        peep_choose_seat_from_car(this, ride, vehicle);
    }
    GoToRideEntrance(ride);
}

/** rct2: 0x00981FD4, 0x00981FD6 */
static constexpr const CoordsXY _MazeEntranceStart[] = {
    { 8, 8 },
    { 8, 24 },
    { 24, 24 },
    { 24, 8 },
};

static void peep_update_ride_leave_entrance_maze(Guest* peep, Ride* ride, CoordsXYZD& entrance_loc)
{
    peep->MazeLastEdge = entrance_loc.direction + 1;

    entrance_loc.x += CoordsDirectionDelta[entrance_loc.direction].x;
    entrance_loc.y += CoordsDirectionDelta[entrance_loc.direction].y;

    uint8_t direction = entrance_loc.direction * 4 + 11;
    if (scenario_rand() & 0x40)
    {
        direction += 4;
        peep->MazeLastEdge += 2;
    }

    direction &= 0xF;
    // Direction is 11, 15, 3, or 7
    peep->Var37 = direction;
    peep->MazeLastEdge &= 3;

    entrance_loc.x += _MazeEntranceStart[direction / 4].x;
    entrance_loc.y += _MazeEntranceStart[direction / 4].y;

    peep->SetDestination(entrance_loc, 3);

    ride->cur_num_customers++;
    peep->OnEnterRide(ride);
    peep->RideSubState = PeepRideSubState::MazePathfinding;
}

static void peep_update_ride_leave_entrance_spiral_slide(Guest* peep, Ride* ride, CoordsXYZD& entrance_loc)
{
    entrance_loc = { ride->GetStation(peep->CurrentRideStation).GetStart(), entrance_loc.direction };

    TileElement* tile_element = ride_get_station_start_track_element(ride, peep->CurrentRideStation);

    uint8_t direction_track = (tile_element == nullptr ? 0 : tile_element->GetDirection());

    peep->Var37 = (entrance_loc.direction << 2) | (direction_track << 4);

    entrance_loc += SpiralSlideWalkingPath[peep->Var37];

    peep->SetDestination(entrance_loc);
    peep->CurrentCar = 0;

    ride->cur_num_customers++;
    peep->OnEnterRide(ride);
    peep->RideSubState = PeepRideSubState::ApproachSpiralSlide;
}

uint8_t Guest::GetWaypointedSeatLocation(const Ride& ride, rct_ride_entry_vehicle* vehicle_type, uint8_t track_direction) const
{
    // The seatlocation can be split into segments around the ride base
    // to decide the segment first split off the segmentable seat location
    // from the fixed section
    uint8_t seatLocationSegment = CurrentSeat & 0x7;
    uint8_t seatLocationFixed = CurrentSeat & 0xF8;

    // Enterprise has more segments (8) compared to the normal (4)
    if (ride.type != RIDE_TYPE_ENTERPRISE)
        track_direction *= 2;

    // Type 1 loading doesn't do segments and all peeps go to the same
    // location on the ride
    if (vehicle_type->peep_loading_waypoint_segments == 0)
    {
        track_direction /= 2;
        seatLocationSegment = 0;
        seatLocationFixed = 0;
    }
    seatLocationSegment += track_direction;
    seatLocationSegment &= 0x7;
    return seatLocationSegment + seatLocationFixed;
}

void Guest::UpdateRideLeaveEntranceWaypoints(const Ride& ride)
{
    const auto& station = ride.GetStation(CurrentRideStation);
    Guard::Assert(!station.Entrance.IsNull());
    uint8_t direction_entrance = station.Entrance.direction;

    CoordsXY waypoint = ride.GetStation(CurrentRideStation).Start.ToTileCentre();

    TileElement* tile_element = ride_get_station_start_track_element(&ride, CurrentRideStation);

    uint8_t direction_track = (tile_element == nullptr ? 0 : tile_element->GetDirection());

    auto vehicle = GetEntity<Vehicle>(ride.vehicles[CurrentTrain]);
    if (vehicle == nullptr)
    {
        // TODO: Goto ride exit on failure.
        return;
    }
    auto ride_entry = vehicle->GetRideEntry();
    auto vehicle_type = &ride_entry->vehicles[vehicle->vehicle_type];

    Var37 = (direction_entrance | GetWaypointedSeatLocation(ride, vehicle_type, direction_track) * 4) * 4;

    if (ride.type == RIDE_TYPE_ENTERPRISE)
    {
        waypoint.x = vehicle->x;
        waypoint.y = vehicle->y;
    }

    const auto waypointIndex = Var37 / 4;
    Guard::Assert(vehicle_type->peep_loading_waypoints.size() >= static_cast<size_t>(waypointIndex));
    waypoint.x += vehicle_type->peep_loading_waypoints[waypointIndex][0].x;
    waypoint.y += vehicle_type->peep_loading_waypoints[waypointIndex][0].y;

    SetDestination(waypoint);
    RideSubState = PeepRideSubState::ApproachVehicleWaypoints;
}

/**
 *
 *  rct2: 0x006921D3
 */
void Guest::UpdateRideAdvanceThroughEntrance()
{
    auto ride = get_ride(CurrentRide);
    if (ride == nullptr)
        return;

    int16_t actionZ, xy_distance;

    auto ride_entry = ride->GetRideEntry();

    if (auto loc = UpdateAction(xy_distance); loc.has_value())
    {
        uint16_t distanceThreshold = 16;
        if (ride_entry != nullptr)
        {
            uint8_t vehicle = ride_entry->default_vehicle;
            if (ride_entry->vehicles[vehicle].flags & VEHICLE_ENTRY_FLAG_MINI_GOLF
                || ride_entry->vehicles[vehicle].flags & (VEHICLE_ENTRY_FLAG_CHAIRLIFT | VEHICLE_ENTRY_FLAG_GO_KART))
            {
                distanceThreshold = 28;
            }
        }

        if (RideSubState == PeepRideSubState::InEntrance && xy_distance < distanceThreshold)
        {
            RideSubState = PeepRideSubState::FreeVehicleCheck;
        }

        actionZ = ride->GetStation(CurrentRideStation).GetBaseZ();

        distanceThreshold += 4;
        if (xy_distance < distanceThreshold)
        {
            actionZ += ride->GetRideTypeDescriptor().Heights.PlatformHeight;
        }

        MoveTo({ loc.value(), actionZ });
        return;
    }

    if (RideSubState == PeepRideSubState::InEntrance)
    {
        RideSubState = PeepRideSubState::FreeVehicleCheck;
        return;
    }

    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES))
    {
        const auto& station = ride->GetStation(CurrentRideStation);
        auto entranceLocation = station.Entrance.ToCoordsXYZD();
        Guard::Assert(!entranceLocation.IsNull());

        if (ride->type == RIDE_TYPE_MAZE)
        {
            peep_update_ride_leave_entrance_maze(this, ride, entranceLocation);
            return;
        }
        if (ride->type == RIDE_TYPE_SPIRAL_SLIDE)
        {
            peep_update_ride_leave_entrance_spiral_slide(this, ride, entranceLocation);
            return;
        }

        // If the ride type was changed guests will become stuck.
        // Inform the player about this if its a new issue or hasn't been addressed within 120 seconds.
        if ((ride->current_issues & RIDE_ISSUE_GUESTS_STUCK) == 0 || gCurrentTicks - ride->last_issue_time > 3000)
        {
            ride->current_issues |= RIDE_ISSUE_GUESTS_STUCK;
            ride->last_issue_time = gCurrentTicks;

            auto ft = Formatter();
            ride->FormatNameTo(ft);
            if (gConfigNotifications.ride_warnings)
            {
                News::AddItemToQueue(News::ItemType::Ride, STR_GUESTS_GETTING_STUCK_ON_RIDE, CurrentRide.ToUnderlying(), ft);
            }
        }

        return;
    }

    Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[CurrentTrain]);
    if (vehicle == nullptr)
    {
        return;
    }

    vehicle = vehicle->GetCar(CurrentCar);
    if (vehicle == nullptr)
    {
        return;
    }

    ride_entry = vehicle->GetRideEntry();
    if (ride_entry == nullptr)
    {
        return;
    }

    rct_ride_entry_vehicle* vehicle_type = &ride_entry->vehicles[vehicle->vehicle_type];

    if (vehicle_type->flags & VEHICLE_ENTRY_FLAG_LOADING_WAYPOINTS)
    {
        UpdateRideLeaveEntranceWaypoints(*ride);
        return;
    }

    if (vehicle_type->flags & VEHICLE_ENTRY_FLAG_DODGEM_CAR_PLACEMENT)
    {
        SetDestination(vehicle->GetLocation(), 15);
        RideSubState = PeepRideSubState::ApproachVehicle;
        return;
    }

    int8_t load_position = 0;
    // Safe, in case current seat > number of loading positions
    uint16_t numSeatPositions = static_cast<uint16_t>(vehicle_type->peep_loading_positions.size());
    if (numSeatPositions != 0)
    {
        size_t loadPositionIndex = numSeatPositions - 1;
        if (CurrentSeat < numSeatPositions)
        {
            loadPositionIndex = CurrentSeat;
        }
        load_position = vehicle_type->peep_loading_positions[loadPositionIndex];
    }

    auto destination = GetDestination();
    switch (vehicle->sprite_direction / 8)
    {
        case 0:
            destination.x = vehicle->x - load_position;
            break;
        case 1:
            destination.y = vehicle->y + load_position;
            break;
        case 2:
            destination.x = vehicle->x + load_position;
            break;
        case 3:
            destination.y = vehicle->y - load_position;
            break;
    }
    SetDestination(destination);

    RideSubState = PeepRideSubState::ApproachVehicle;
}

/**
 *
 *  rct2: 0x0069321D
 */
static void peep_go_to_ride_exit(Peep* peep, Ride* ride, int16_t x, int16_t y, int16_t z, uint8_t exit_direction)
{
    z += ride->GetRideTypeDescriptor().Heights.PlatformHeight;

    peep->MoveTo({ x, y, z });

    Guard::Assert(peep->CurrentRideStation.ToUnderlying() < OpenRCT2::Limits::MaxStationsPerRide);
    auto exit = ride->GetStation(peep->CurrentRideStation).Exit;
    Guard::Assert(!exit.IsNull());
    x = exit.x;
    y = exit.y;
    x *= 32;
    y *= 32;
    x += 16;
    y += 16;

    int16_t x_shift = DirectionOffsets[exit_direction].x;
    int16_t y_shift = DirectionOffsets[exit_direction].y;

    int16_t shift_multiplier = 20;

    rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);
    if (rideEntry != nullptr)
    {
        rct_ride_entry_vehicle* vehicle_entry = &rideEntry->vehicles[rideEntry->default_vehicle];
        if (vehicle_entry->flags & VEHICLE_ENTRY_FLAG_MINI_GOLF
            || vehicle_entry->flags & (VEHICLE_ENTRY_FLAG_CHAIRLIFT | VEHICLE_ENTRY_FLAG_GO_KART))
        {
            shift_multiplier = 32;
        }
    }

    x_shift *= shift_multiplier;
    y_shift *= shift_multiplier;

    x -= x_shift;
    y -= y_shift;

    peep->SetDestination({ x, y }, 2);

    peep->sprite_direction = exit_direction * 8;
    peep->RideSubState = PeepRideSubState::ApproachExit;
}

/**
 *
 *  rct2: 0x006920B4
 */
void Guest::UpdateRideFreeVehicleEnterRide(Ride* ride)
{
    money16 ridePrice = ride_get_price(ride);
    if (ridePrice != 0)
    {
        if ((HasItem(ShopItem::Voucher)) && (VoucherType == VOUCHER_TYPE_RIDE_FREE) && (VoucherRideId == CurrentRide))
        {
            RemoveItem(ShopItem::Voucher);
            WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
        }
        else
        {
            ride->total_profit += ridePrice;
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
            SpendMoney(PaidOnRides, ridePrice, ExpenditureType::ParkRideTickets);
        }
    }

    RideSubState = PeepRideSubState::LeaveEntrance;
    uint8_t queueTime = DaysInQueue;
    if (queueTime < 253)
        queueTime += 3;

    queueTime /= 2;
    auto& station = ride->GetStation(CurrentRideStation);
    if (queueTime != station.QueueTime)
    {
        station.QueueTime = queueTime;
        window_invalidate_by_number(WC_RIDE, CurrentRide.ToUnderlying());
    }

    if (PeepFlags & PEEP_FLAGS_TRACKING)
    {
        auto ft = Formatter();
        FormatNameTo(ft);
        ride->FormatNameTo(ft);

        rct_string_id msg_string;
        if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IN_RIDE))
            msg_string = STR_PEEP_TRACKING_PEEP_IS_IN_X;
        else
            msg_string = STR_PEEP_TRACKING_PEEP_IS_ON_X;

        if (gConfigNotifications.guest_on_ride)
        {
            News::AddItemToQueue(News::ItemType::PeepOnRide, msg_string, sprite_index, ft);
        }
    }

    if (ride->type == RIDE_TYPE_SPIRAL_SLIDE)
    {
        SwitchToSpecialSprite(1);
    }

    UpdateRideAdvanceThroughEntrance();
}

/**
 *
 *  rct2: 0x00691FD4
 */
void peep_update_ride_no_free_vehicle_rejoin_queue(Guest* peep, Ride* ride)
{
    TileCoordsXYZD entranceLocation = ride->GetStation(peep->CurrentRideStation).Entrance;

    int32_t x = entranceLocation.x * 32;
    int32_t y = entranceLocation.y * 32;
    x += 16 - DirectionOffsets[entranceLocation.direction].x * 20;
    y += 16 - DirectionOffsets[entranceLocation.direction].y * 20;

    peep->SetDestination({ x, y }, 2);
    peep->SetState(PeepState::QueuingFront);
    peep->RideSubState = PeepRideSubState::AtEntrance;

    ride->QueueInsertGuestAtFront(peep->CurrentRideStation, peep);
}

/**
 *
 *  rct2: 0x00691E42
 * Note: Before this was the entry
 * point for sub state 1 and 3. The
 * check has been removed that would
 * branch it out to 1 and 3. Now uses
 * separate functions.
 */
void Guest::UpdateRideFreeVehicleCheck()
{
    auto ride = get_ride(CurrentRide);
    if (ride == nullptr)
        return;

    using namespace OpenRCT2::RideModes;
    const auto* rideMode = GetRideMode(ride->mode);
    rideMode->UpdateRideFreeVehicleCheck(this);
}

void Guest::UpdateRideApproachVehicle()
{
    if (auto loc = UpdateAction(); loc.has_value())
    {
        MoveTo({ loc.value(), z });
        return;
    }
    RideSubState = PeepRideSubState::EnterVehicle;
}

void Guest::UpdateRideEnterVehicle()
{
    auto* ride = get_ride(CurrentRide);
    if (ride != nullptr)
    {
        auto* vehicle = GetEntity<Vehicle>(ride->vehicles[CurrentTrain]);
        if (vehicle != nullptr)
        {
            vehicle = vehicle->GetCar(CurrentCar);
            if (vehicle == nullptr)
            {
                return;
            }

            if (ride->mode != RideMode::ForwardRotation && ride->mode != RideMode::BackwardRotation)
            {
                if (CurrentSeat != vehicle->num_peeps)
                    return;
            }

            if (vehicle->IsUsedInPairs())
            {
                auto* seatedGuest = GetEntity<Guest>(vehicle->peep[CurrentSeat ^ 1]);
                if (seatedGuest != nullptr)
                {
                    if (seatedGuest->RideSubState != PeepRideSubState::EnterVehicle)
                        return;

                    vehicle->num_peeps++;
                    ride->cur_num_customers++;

                    vehicle->ApplyMass(seatedGuest->Mass);
                    seatedGuest->MoveTo({ LOCATION_NULL, 0, 0 });
                    seatedGuest->SetState(PeepState::OnRide);
                    seatedGuest->GuestTimeOnRide = 0;
                    seatedGuest->RideSubState = PeepRideSubState::OnRide;
                    seatedGuest->OnEnterRide(ride);
                }
            }

            vehicle->num_peeps++;
            ride->cur_num_customers++;

            vehicle->ApplyMass(Mass);
            vehicle->Invalidate();

            MoveTo({ LOCATION_NULL, 0, 0 });

            SetState(PeepState::OnRide);

            GuestTimeOnRide = 0;
            RideSubState = PeepRideSubState::OnRide;
            OnEnterRide(ride);
        }
    }
}

/**
 *
 *  rct2: 0x00693028
 */
void Guest::UpdateRideLeaveVehicle()
{
    auto ride = get_ride(CurrentRide);
    if (ride == nullptr)
        return;

    Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[CurrentTrain]);
    if (vehicle == nullptr)
        return;

    StationIndex ride_station = vehicle->current_station;
    vehicle = vehicle->GetCar(CurrentCar);
    if (vehicle == nullptr)
    {
        return;
    }

    // Check if ride is NOT Ferris Wheel.
    if (ride->mode != RideMode::ForwardRotation && ride->mode != RideMode::BackwardRotation)
    {
        if (vehicle->num_peeps - 1 != CurrentSeat)
            return;
    }

    ActionSpriteImageOffset++;
    if (ActionSpriteImageOffset & 3)
        return;

    ActionSpriteImageOffset = 0;

    vehicle->num_peeps--;
    vehicle->ApplyMass(-Mass);
    vehicle->Invalidate();

    if (ride_station.ToUnderlying() >= OpenRCT2::Limits::MaxStationsPerRide)
    {
        // HACK #5658: Some parks have hacked rides which end up in this state
        auto bestStationIndex = ride_get_first_valid_station_exit(ride);
        if (bestStationIndex.IsNull())
        {
            bestStationIndex = StationIndex::FromUnderlying(0);
        }
        ride_station = bestStationIndex;
    }
    CurrentRideStation = ride_station;
    rct_ride_entry* rideEntry = vehicle->GetRideEntry();
    if (rideEntry == nullptr)
    {
        return;
    }

    rct_ride_entry_vehicle* vehicle_entry = &rideEntry->vehicles[vehicle->vehicle_type];

    assert(CurrentRideStation.ToUnderlying() < OpenRCT2::Limits::MaxStationsPerRide);
    auto& station = ride->GetStation(CurrentRideStation);

    if (!(vehicle_entry->flags & VEHICLE_ENTRY_FLAG_LOADING_WAYPOINTS))
    {
        TileCoordsXYZD exitLocation = station.Exit;
        CoordsXYZD platformLocation;
        platformLocation.z = station.GetBaseZ();

        platformLocation.direction = direction_reverse(exitLocation.direction);

        if (!ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_VEHICLE_IS_INTEGRAL))
        {
            for (; vehicle != nullptr && !vehicle->IsHead(); vehicle = GetEntity<Vehicle>(vehicle->prev_vehicle_on_ride))
            {
                auto trackType = vehicle->GetTrackType();
                if (trackType == TrackElemType::Flat || trackType > TrackElemType::MiddleStation)
                    continue;

                bool foundStation = false;
                for (auto* trackElement : TileElementsView<TrackElement>(vehicle->TrackLocation))
                {
                    if (trackElement->GetBaseZ() != vehicle->TrackLocation.z)
                        continue;

                    if (trackElement->GetStationIndex() != CurrentRideStation)
                        continue;

                    foundStation = true;
                    break;
                }

                if (foundStation)
                    break;
            }

            if (vehicle == nullptr)
            {
                return;
            }
            uint8_t shiftMultiplier = 12;
            uint8_t specialDirection = platformLocation.direction;

            rideEntry = get_ride_entry(ride->subtype);

            if (rideEntry != nullptr)
            {
                vehicle_entry = &rideEntry->vehicles[rideEntry->default_vehicle];

                if (vehicle_entry->flags & VEHICLE_ENTRY_FLAG_GO_KART)
                {
                    shiftMultiplier = 9;
                }

                if (vehicle_entry->flags & (VEHICLE_ENTRY_FLAG_CHAIRLIFT | VEHICLE_ENTRY_FLAG_GO_KART))
                {
                    specialDirection = ((vehicle->sprite_direction + 3) / 8) + 1;
                    specialDirection &= 3;

                    if (vehicle->TrackSubposition == VehicleTrackSubposition::GoKartsRightLane)
                        specialDirection = direction_reverse(specialDirection);
                }
            }

            int16_t xShift = DirectionOffsets[specialDirection].x;
            int16_t yShift = DirectionOffsets[specialDirection].y;

            platformLocation.x = vehicle->x + xShift * shiftMultiplier;
            platformLocation.y = vehicle->y + yShift * shiftMultiplier;

            peep_go_to_ride_exit(
                this, ride, platformLocation.x, platformLocation.y, platformLocation.z, platformLocation.direction);
            return;
        }

        platformLocation.x = vehicle->x + DirectionOffsets[platformLocation.direction].x * 12;
        platformLocation.y = vehicle->y + DirectionOffsets[platformLocation.direction].y * 12;

        // This can evaluate to false with buggy custom rides.
        if (CurrentSeat < vehicle_entry->peep_loading_positions.size())
        {
            int8_t loadPosition = vehicle_entry->peep_loading_positions[CurrentSeat];

            switch (vehicle->sprite_direction / 8)
            {
                case 0:
                    platformLocation.x -= loadPosition;
                    break;
                case 1:
                    platformLocation.y += loadPosition;
                    break;
                case 2:
                    platformLocation.x += loadPosition;
                    break;
                case 3:
                    platformLocation.y -= loadPosition;
                    break;
            }
        }
        else
        {
            log_verbose(
                "CurrentSeat %d is too large! (Vehicle entry has room for %d.)", CurrentSeat,
                vehicle_entry->peep_loading_positions.size());
        }

        platformLocation.z = station.GetBaseZ();

        peep_go_to_ride_exit(
            this, ride, platformLocation.x, platformLocation.y, platformLocation.z, platformLocation.direction);
        return;
    }

    auto exitLocation = station.Exit.ToCoordsXYZD();
    Guard::Assert(!exitLocation.IsNull());

    auto waypointLoc = CoordsXYZ{ station.Start.ToTileCentre(),
                                  exitLocation.z + ride->GetRideTypeDescriptor().Heights.PlatformHeight };

    TileElement* trackElement = ride_get_station_start_track_element(ride, CurrentRideStation);

    Direction station_direction = (trackElement == nullptr ? 0 : trackElement->GetDirection());

    vehicle = GetEntity<Vehicle>(ride->vehicles[CurrentTrain]);
    if (vehicle == nullptr)
    {
        return;
    }

    rideEntry = vehicle->GetRideEntry();
    rct_ride_entry_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle->vehicle_type];
    if (vehicleEntry == nullptr)
        return;

    Var37 = ((exitLocation.direction | GetWaypointedSeatLocation(*ride, vehicleEntry, station_direction) * 4) * 4) | 1;

    if (ride->type == RIDE_TYPE_ENTERPRISE)
    {
        waypointLoc.x = vehicle->x;
        waypointLoc.y = vehicle->y;
    }

    Guard::Assert(vehicleEntry->peep_loading_waypoints.size() >= static_cast<size_t>(Var37 / 4));
    CoordsXYZ exitWaypointLoc = waypointLoc;

    exitWaypointLoc.x += vehicleEntry->peep_loading_waypoints[Var37 / 4][2].x;
    exitWaypointLoc.y += vehicleEntry->peep_loading_waypoints[Var37 / 4][2].y;

    if (ride->type == RIDE_TYPE_MOTION_SIMULATOR)
        exitWaypointLoc.z += 15;

    MoveTo(exitWaypointLoc);

    waypointLoc.x += vehicleEntry->peep_loading_waypoints[Var37 / 4][1].x;
    waypointLoc.y += vehicleEntry->peep_loading_waypoints[Var37 / 4][1].y;

    SetDestination(waypointLoc, 2);
    RideSubState = PeepRideSubState::ApproachExitWaypoints;
}

/**
 *
 *  rct2: 0x0069376A
 */
void Guest::UpdateRidePrepareForExit()
{
    auto ride = get_ride(CurrentRide);
    if (ride == nullptr || CurrentRideStation.ToUnderlying() >= std::size(ride->GetStations()))
        return;

    auto exit = ride->GetStation(CurrentRideStation).Exit;
    auto newDestination = exit.ToCoordsXY().ToTileCentre();
    auto xShift = DirectionOffsets[exit.direction].x;
    auto yShift = DirectionOffsets[exit.direction].y;

    int16_t shiftMultiplier = 20;

    rct_ride_entry* rideEntry = ride->GetRideEntry();
    if (rideEntry != nullptr)
    {
        rct_ride_entry_vehicle* vehicleEntry = &rideEntry->vehicles[rideEntry->default_vehicle];
        if (vehicleEntry->flags & (VEHICLE_ENTRY_FLAG_CHAIRLIFT | VEHICLE_ENTRY_FLAG_GO_KART))
        {
            shiftMultiplier = 32;
        }
    }

    xShift *= shiftMultiplier;
    yShift *= shiftMultiplier;

    newDestination.x -= xShift;
    newDestination.y -= yShift;

    SetDestination(newDestination, 2);
    RideSubState = PeepRideSubState::InExit;
}

/**
 *
 *  rct2: 0x0069374F
 */
void Guest::UpdateRideApproachExit()
{
    if (auto loc = UpdateAction(); loc.has_value())
    {
        MoveTo({ loc.value(), z });
        return;
    }

    UpdateRidePrepareForExit();
}

/**
 *
 *  rct2: 0x0069382E
 */
void Guest::UpdateRideInExit()
{
    auto ride = get_ride(CurrentRide);
    if (ride == nullptr)
        return;

    int16_t xy_distance;

    if (auto loc = UpdateAction(xy_distance); loc.has_value())
    {
        if (xy_distance >= 16)
        {
            int16_t actionZ = ride->GetStation(CurrentRideStation).GetBaseZ();

            actionZ += ride->GetRideTypeDescriptor().Heights.PlatformHeight;
            MoveTo({ loc.value(), actionZ });
            return;
        }

        SwitchToSpecialSprite(0);
        MoveTo({ loc.value(), z });
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO)
    {
        ShopItem secondaryItem = ride->GetRideTypeDescriptor().PhotoItem;
        if (DecideAndBuyItem(ride, secondaryItem, ride->price[1]))
        {
            ride->no_secondary_items_sold++;
        }
    }
    RideSubState = PeepRideSubState::LeaveExit;
}
#pragma warning(default : 6011)
/**
 *
 *  rct2: 0x006926AD
 */
void Guest::UpdateRideApproachVehicleWaypoints()
{
    auto ride = get_ride(CurrentRide);
    if (ride == nullptr)
        return;

    int16_t xy_distance;
    uint8_t waypoint = Var37 & 3;

    if (auto loc = UpdateAction(xy_distance); loc.has_value())
    {
        int16_t actionZ;
        // Motion simulators have steps this moves the peeps up the steps
        if (ride->type == RIDE_TYPE_MOTION_SIMULATOR)
        {
            actionZ = ride->GetStation(CurrentRideStation).GetBaseZ() + 2;

            if (waypoint == 2)
            {
                xy_distance -= 12;
                if (xy_distance < 0)
                    xy_distance = 0;

                if (xy_distance <= 15)
                {
                    actionZ += 15 - xy_distance;
                }
            }
        }
        else
        {
            actionZ = z;
        }
        MoveTo({ loc.value(), actionZ });
        return;
    }

    if (waypoint == 2)
    {
        RideSubState = PeepRideSubState::EnterVehicle;
        return;
    }

    waypoint++;
    // This is incrementing the actual peep waypoint
    Var37++;

    Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[CurrentTrain]);
    if (vehicle == nullptr)
    {
        return;
    }

    CoordsXY targetLoc = ride->GetStation(CurrentRideStation).Start.ToTileCentre();

    if (ride->type == RIDE_TYPE_ENTERPRISE)
    {
        targetLoc.x = vehicle->x;
        targetLoc.y = vehicle->y;
    }

    rct_ride_entry* ride_entry = vehicle->GetRideEntry();
    if (ride_entry == nullptr)
    {
        return;
    }

    rct_ride_entry_vehicle* vehicle_type = &ride_entry->vehicles[vehicle->vehicle_type];
    Guard::Assert(waypoint < 3);
    targetLoc.x += vehicle_type->peep_loading_waypoints[Var37 / 4][waypoint].x;
    targetLoc.y += vehicle_type->peep_loading_waypoints[Var37 / 4][waypoint].y;

    SetDestination(targetLoc);
}

/**
 *
 *  rct2: 0x0069357D
 */
void Guest::UpdateRideApproachExitWaypoints()
{
    auto ride = get_ride(CurrentRide);
    if (ride == nullptr)
        return;

    int16_t xy_distance;

    if (auto loc = UpdateAction(xy_distance); loc.has_value())
    {
        int16_t actionZ;
        if (ride->type == RIDE_TYPE_MOTION_SIMULATOR)
        {
            actionZ = ride->GetStation(CurrentRideStation).GetBaseZ() + 2;

            if ((Var37 & 3) == 1)
            {
                if (xy_distance > 15)
                    xy_distance = 15;

                actionZ += xy_distance;
            }
        }
        else
        {
            actionZ = z;
        }
        MoveTo({ loc.value(), actionZ });
        return;
    }

    if ((Var37 & 3) != 0)
    {
        if ((Var37 & 3) == 3)
        {
            UpdateRidePrepareForExit();
            return;
        }

        Var37--;
        Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[CurrentTrain]);
        if (vehicle == nullptr)
        {
            return;
        }
        CoordsXY targetLoc = ride->GetStation(CurrentRideStation).Start.ToTileCentre();

        if (ride->type == RIDE_TYPE_ENTERPRISE)
        {
            targetLoc.x = vehicle->x;
            targetLoc.y = vehicle->y;
        }

        rct_ride_entry* rideEntry = vehicle->GetRideEntry();
        rct_ride_entry_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle->vehicle_type];

        Guard::Assert((Var37 & 3) < 3);
        targetLoc.x += vehicleEntry->peep_loading_waypoints[Var37 / 4][Var37 & 3].x;
        targetLoc.y += vehicleEntry->peep_loading_waypoints[Var37 / 4][Var37 & 3].y;

        SetDestination(targetLoc);
        return;
    }

    Var37 |= 3;

    auto targetLoc = ride->GetStation(CurrentRideStation).Exit.ToCoordsXYZD().ToTileCentre();
    uint8_t exit_direction = direction_reverse(targetLoc.direction);

    int16_t x_shift = DirectionOffsets[exit_direction].x;
    int16_t y_shift = DirectionOffsets[exit_direction].y;

    int16_t shift_multiplier = 20;

    auto rideEntry = get_ride_entry(ride->subtype);
    if (rideEntry != nullptr)
    {
        auto vehicleEntry = &rideEntry->vehicles[rideEntry->default_vehicle];
        if (vehicleEntry->flags & (VEHICLE_ENTRY_FLAG_CHAIRLIFT | VEHICLE_ENTRY_FLAG_GO_KART))
        {
            shift_multiplier = 32;
        }
    }

    x_shift *= shift_multiplier;
    y_shift *= shift_multiplier;

    targetLoc.x -= x_shift;
    targetLoc.y -= y_shift;

    SetDestination(targetLoc);
}

/**
 *
 *  rct2: 0x006927B3
 */
void Guest::UpdateRideApproachSpiralSlide()
{
    auto ride = get_ride(CurrentRide);
    if (ride == nullptr)
        return;

    if (auto loc = UpdateAction(); loc.has_value())
    {
        MoveTo({ loc.value(), z });
        return;
    }

    uint8_t waypoint = Var37 & 3;

    if (waypoint == 3)
    {
        SubState = 15;
        SetDestination({ 0, 0 });
        Var37 = (Var37 / 4) & 0xC;
        MoveTo({ LOCATION_NULL, y, z });
        return;
    }

    if (waypoint == 2)
    {
        bool lastRide = false;
        if (ride->status != RideStatus::Open)
            lastRide = true;
        else if (CurrentCar++ != 0)
        {
            if (ride->mode == RideMode::SingleRidePerAdmission)
                lastRide = true;
            if (static_cast<uint8_t>(CurrentCar - 1) > (scenario_rand() & 0xF))
                lastRide = true;
        }

        if (lastRide)
        {
            auto exit = ride->GetStation(CurrentRideStation).Exit;
            waypoint = 1;
            Var37 = (exit.direction * 4) | (Var37 & 0x30) | waypoint;
            CoordsXY targetLoc = ride->GetStation(CurrentRideStation).Start;

            assert(ride->type == RIDE_TYPE_SPIRAL_SLIDE);
            targetLoc += SpiralSlideWalkingPath[Var37];

            SetDestination(targetLoc);
            RideSubState = PeepRideSubState::LeaveSpiralSlide;
            return;
        }
    }
    waypoint++;
    // Actually increment the real peep waypoint
    Var37++;

    CoordsXY targetLoc = ride->GetStation(CurrentRideStation).Start;

    assert(ride->type == RIDE_TYPE_SPIRAL_SLIDE);
    targetLoc += SpiralSlideWalkingPath[Var37];

    SetDestination(targetLoc);
}

/** rct2: 0x00981F0C, 0x00981F0E */
static constexpr const CoordsXY _SpiralSlideEnd[] = {
    { 25, 56 },
    { 56, 7 },
    { 7, -24 },
    { -24, 25 },
};

/** rct2: 0x00981F1C, 0x00981F1E */
static constexpr const CoordsXY _SpiralSlideEndWaypoint[] = {
    { 8, 56 },
    { 56, 24 },
    { 24, -24 },
    { -24, 8 },
};

/**
 *
 *  rct2: 0x00692D83
 */
void Guest::UpdateRideOnSpiralSlide()
{
    auto ride = get_ride(CurrentRide);
    if (ride == nullptr || ride->type != RIDE_TYPE_SPIRAL_SLIDE)
        return;

    auto destination = GetDestination();
    if ((Var37 & 3) == 0)
    {
        switch (destination.x)
        {
            case 0:
                destination.y++;
                if (destination.y >= 30)
                    destination.x++;

                SetDestination(destination);
                return;
            case 1:
                if (ride->slide_in_use != 0)
                    return;

                ride->slide_in_use++;
                ride->slide_peep = sprite_index;
                ride->slide_peep_t_shirt_colour = TshirtColour;
                ride->spiral_slide_progress = 0;
                destination.x++;

                SetDestination(destination);
                return;
            case 2:
                return;
            case 3:
            {
                auto newLocation = ride->GetStation(CurrentRideStation).Start;
                uint8_t dir = (Var37 / 4) & 3;

                // Set the location that the peep walks to go on slide again
                destination = newLocation + _SpiralSlideEndWaypoint[dir];
                SetDestination(destination);

                // Move the peep sprite to just at the end of the slide
                newLocation.x += _SpiralSlideEnd[dir].x;
                newLocation.y += _SpiralSlideEnd[dir].y;

                MoveTo({ newLocation, z });

                sprite_direction = (Var37 & 0xC) * 2;

                Var37++;
                return;
            }
            default:
                return;
        }
    }

    if (auto loc = UpdateAction(); loc.has_value())
    {
        MoveTo({ loc.value(), z });
        return;
    }

    uint8_t waypoint = 2;
    Var37 = (Var37 * 4 & 0x30) + waypoint;

    CoordsXY targetLoc = ride->GetStation(CurrentRideStation).Start;

    assert(ride->type == RIDE_TYPE_SPIRAL_SLIDE);
    targetLoc += SpiralSlideWalkingPath[Var37];

    SetDestination(targetLoc);
    RideSubState = PeepRideSubState::ApproachSpiralSlide;
}

/**
 *
 *  rct2: 0x00692C6B
 */
void Guest::UpdateRideLeaveSpiralSlide()
{
    // Iterates through the spiral slide waypoints until it reaches
    // waypoint 0. Then it readies to leave the ride by the entrance.
    if (auto loc = UpdateAction(); loc.has_value())
    {
        MoveTo({ loc.value(), z });
        return;
    }

    auto ride = get_ride(CurrentRide);
    if (ride == nullptr)
        return;

    uint8_t waypoint = Var37 & 3;

    if (waypoint != 0)
    {
        if (waypoint == 3)
        {
            UpdateRidePrepareForExit();
            return;
        }

        waypoint--;
        // Actually decrement the peep waypoint
        Var37--;
        CoordsXY targetLoc = ride->GetStation(CurrentRideStation).Start;

        assert(ride->type == RIDE_TYPE_SPIRAL_SLIDE);
        targetLoc += SpiralSlideWalkingPath[Var37];

        SetDestination(targetLoc);
        return;
    }

    // Actually force the final waypoint
    Var37 |= 3;

    auto targetLoc = ride->GetStation(CurrentRideStation).Exit.ToCoordsXYZD().ToTileCentre();

    int16_t xShift = DirectionOffsets[direction_reverse(targetLoc.direction)].x;
    int16_t yShift = DirectionOffsets[direction_reverse(targetLoc.direction)].y;

    int16_t shiftMultiplier = 20;

    xShift *= shiftMultiplier;
    yShift *= shiftMultiplier;

    targetLoc.x -= xShift;
    targetLoc.y -= yShift;

    SetDestination(targetLoc);
}

/** rct2: 0x00981FE4 */
static constexpr const uint8_t _MazeGetNewDirectionFromEdge[][4] = {
    { 15, 7, 15, 7 },
    { 11, 3, 11, 3 },
    { 7, 15, 7, 15 },
    { 3, 11, 3, 11 },
};

/** rct2: 0x00981FF4 */
static constexpr const uint8_t _MazeCurrentDirectionToOpenHedge[][4] = {
    { 1, 2, 14, 0 },
    { 4, 5, 6, 2 },
    { 6, 8, 9, 10 },
    { 14, 10, 12, 13 },
};

/**
 *
 *  rct2: 0x00692A83
 */
void Guest::UpdateRideMazePathfinding()
{
    if (auto loc = UpdateAction(); loc.has_value())
    {
        MoveTo({ loc.value(), z });
        return;
    }

    auto ride = get_ride(CurrentRide);
    if (ride == nullptr)
        return;

    if (Var37 == 16)
    {
        UpdateRidePrepareForExit();
        return;
    }

    if (IsActionInterruptable())
    {
        if (Energy > 80 && !(PeepFlags & PEEP_FLAGS_SLOW_WALK) && !climate_is_raining() && (scenario_rand() & 0xFFFF) <= 2427)
        {
            Action = PeepActionType::Jump;
            ActionFrame = 0;
            ActionSpriteImageOffset = 0;
            UpdateCurrentActionSpriteType();
        }
    }

    auto targetLoc = GetDestination().ToTileStart();

    auto stationBaseZ = ride->GetStation().GetBaseZ();

    // Find the station track element
    auto trackElement = map_get_track_element_at({ targetLoc, stationBaseZ });
    if (trackElement == nullptr)
    {
        return;
    }

    uint16_t mazeEntry = trackElement->GetMazeEntry();
    // Var37 is 3, 7, 11 or 15
    uint8_t hedges[4]{ 0xFF, 0xFF, 0xFF, 0xFF };
    uint8_t openCount = 0;
    uint8_t mazeReverseLastEdge = direction_reverse(MazeLastEdge);
    for (uint8_t i = 0; i < 4; ++i)
    {
        if (!(mazeEntry & (1 << _MazeCurrentDirectionToOpenHedge[Var37 / 4][i])) && i != mazeReverseLastEdge)
        {
            hedges[openCount++] = i;
        }
    }

    if (openCount == 0)
    {
        if ((mazeEntry & (1 << _MazeCurrentDirectionToOpenHedge[Var37 / 4][mazeReverseLastEdge])))
        {
            return;
        }
        hedges[openCount++] = mazeReverseLastEdge;
    }

    uint8_t chosenEdge = hedges[scenario_rand() % openCount];
    assert(chosenEdge != 0xFF);

    targetLoc = GetDestination() + CoordsDirectionDelta[chosenEdge] / 2;

    enum class maze_type
    {
        invalid,
        hedge,
        entrance_or_exit
    };
    maze_type mazeType = maze_type::invalid;

    auto tileElement = map_get_first_element_at(targetLoc);
    if (tileElement == nullptr)
        return;
    do
    {
        if (stationBaseZ != tileElement->GetBaseZ())
            continue;

        if (tileElement->GetType() == TileElementType::Track)
        {
            mazeType = maze_type::hedge;
            break;
        }

        if (tileElement->GetType() == TileElementType::Entrance
            && tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_RIDE_EXIT)
        {
            mazeType = maze_type::entrance_or_exit;
            break;
        }
    } while (!(tileElement++)->IsLastForTile());

    switch (mazeType)
    {
        case maze_type::invalid:
            MazeLastEdge++;
            MazeLastEdge &= 3;
            return;
        case maze_type::hedge:
            SetDestination(targetLoc);
            Var37 = _MazeGetNewDirectionFromEdge[Var37 / 4][chosenEdge];
            MazeLastEdge = chosenEdge;
            break;
        case maze_type::entrance_or_exit:
            targetLoc = GetDestination();
            if (chosenEdge & 1)
            {
                targetLoc.x = targetLoc.ToTileCentre().x;
            }
            else
            {
                targetLoc.y = targetLoc.ToTileCentre().y;
            }
            SetDestination(targetLoc);
            Var37 = 16;
            MazeLastEdge = chosenEdge;
            break;
    }

    if (auto loc = UpdateAction(); loc.has_value())
    {
        MoveTo({ loc.value(), z });
        return;
    }
}

/**
 *
 *  rct2: 0x006938D2
 */
void Guest::UpdateRideLeaveExit()
{
    auto ride = get_ride(CurrentRide);

    if (auto loc = UpdateAction(); loc.has_value())
    {
        if (ride != nullptr)
        {
            MoveTo({ loc.value(), ride->GetStation(CurrentRideStation).GetBaseZ() });
        }
        return;
    }

    OnExitRide(ride);

    if (ride != nullptr && (PeepFlags & PEEP_FLAGS_TRACKING))
    {
        auto ft = Formatter();
        FormatNameTo(ft);
        ride->FormatNameTo(ft);

        if (gConfigNotifications.guest_left_ride)
        {
            News::AddItemToQueue(News::ItemType::PeepOnRide, STR_PEEP_TRACKING_LEFT_RIDE_X, sprite_index, ft);
        }
    }

    InteractionRideIndex = RideId::GetNull();
    SetState(PeepState::Falling);

    CoordsXY targetLoc = { x, y };

    // Find the station track element
    for (auto* pathElement : TileElementsView<PathElement>(targetLoc))
    {
        int16_t height = map_height_from_slope(targetLoc, pathElement->GetSlopeDirection(), pathElement->IsSloped());
        height += pathElement->GetBaseZ();

        int16_t z_diff = z - height;
        if (z_diff > 0 || z_diff < -16)
            continue;

        MoveTo({ x, y, height });
        return;
    }
}

/**
 *
 *  rct2: 0x0069299C
 */
void Guest::UpdateRideShopApproach()
{
    if (auto loc = UpdateAction(); loc.has_value())
    {
        MoveTo({ loc.value(), z });
        return;
    }

    RideSubState = PeepRideSubState::InteractShop;
}

/**
 *
 *  rct2: 0x006929BB
 */
void Guest::UpdateRideShopInteract()
{
    auto ride = get_ride(CurrentRide);
    if (ride == nullptr)
        return;

    const int16_t tileCentreX = NextLoc.x + 16;
    const int16_t tileCentreY = NextLoc.y + 16;
    if (ride->type == RIDE_TYPE_FIRST_AID)
    {
        if (Nausea <= 35)
        {
            RideSubState = PeepRideSubState::LeaveShop;

            SetDestination({ tileCentreX, tileCentreY }, 3);
            HappinessTarget = std::min(HappinessTarget + 30, PEEP_MAX_HAPPINESS);
            Happiness = HappinessTarget;
        }
        else
        {
            Nausea--;
            NauseaTarget = Nausea;
        }
        return;
    }

    if (Toilet != 0)
    {
        Toilet--;
        return;
    }

    // Do not play toilet flush sound on title screen as it's considered loud and annoying
    if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
    {
        OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::ToiletFlush, GetLocation());
    }

    RideSubState = PeepRideSubState::LeaveShop;

    SetDestination({ tileCentreX, tileCentreY }, 3);

    HappinessTarget = std::min(HappinessTarget + 30, PEEP_MAX_HAPPINESS);
    Happiness = HappinessTarget;
    StopPurchaseThought(ride->type);
}

/**
 *
 *  rct2: 0x00692935
 */
void Guest::UpdateRideShopLeave()
{
    if (auto loc = UpdateAction(); loc.has_value())
    {
        const auto curLoc = GetLocation();
        MoveTo({ loc.value(), curLoc.z });

        const auto newLoc = GetLocation().ToTileStart();
        if (newLoc.x != NextLoc.x)
            return;
        if (newLoc.y != NextLoc.y)
            return;
    }

    //#11758 Previously SetState(PeepState::Walking) caused Peeps to double-back to exit point of shop
    SetState(PeepState::Falling);

    auto ride = get_ride(CurrentRide);
    if (ride != nullptr)
    {
        ride->total_customers++;
        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;
        ride->UpdateSatisfaction(Happiness / 64);
    }
}

void Guest::UpdateGuest()
{
    switch (State)
    {
        case PeepState::QueuingFront:
            UpdateRide();
            break;
        case PeepState::LeavingRide:
            UpdateRide();
            break;
        case PeepState::Walking:
            UpdateWalking();
            break;
        case PeepState::Queuing:
            UpdateQueuing();
            break;
        case PeepState::EnteringRide:
            UpdateRide();
            break;
        case PeepState::Sitting:
            UpdateSitting();
            break;
        case PeepState::EnteringPark:
            UpdateEnteringPark();
            break;
        case PeepState::LeavingPark:
            UpdateLeavingPark();
            break;
        case PeepState::Buying:
            UpdateBuying();
            break;
        case PeepState::Watching:
            UpdateWatching();
            break;
        case PeepState::UsingBin:
            UpdateUsingBin();
            break;
        default:
            // TODO reset to default state
            assert(false);
            break;
    }
}

/**
 *
 *  rct2: 0x691A30
 * Used by entering_ride and queueing_front */
void Guest::UpdateRide()
{
    NextFlags &= ~PEEP_NEXT_FLAG_IS_SLOPED;

    switch (RideSubState)
    {
        case PeepRideSubState::AtEntrance:
            UpdateRideAtEntrance();
            break;
        case PeepRideSubState::InEntrance:
            UpdateRideAdvanceThroughEntrance();
            break;
        case PeepRideSubState::FreeVehicleCheck:
            UpdateRideFreeVehicleCheck();
            break;
        case PeepRideSubState::LeaveEntrance:
            UpdateRideAdvanceThroughEntrance();
            break;
        case PeepRideSubState::ApproachVehicle:
            UpdateRideApproachVehicle();
            break;
        case PeepRideSubState::EnterVehicle:
            UpdateRideEnterVehicle();
            break;
        case PeepRideSubState::OnRide:
            // No action, on ride.
            break;
        case PeepRideSubState::LeaveVehicle:
            UpdateRideLeaveVehicle();
            break;
        case PeepRideSubState::ApproachExit:
            UpdateRideApproachExit();
            break;
        case PeepRideSubState::InExit:
            UpdateRideInExit();
            break;
        case PeepRideSubState::ApproachVehicleWaypoints:
            UpdateRideApproachVehicleWaypoints();
            break;
        case PeepRideSubState::ApproachExitWaypoints:
            UpdateRideApproachExitWaypoints();
            break;
        case PeepRideSubState::ApproachSpiralSlide:
            UpdateRideApproachSpiralSlide();
            break;
        case PeepRideSubState::OnSpiralSlide:
            UpdateRideOnSpiralSlide();
            break;
        case PeepRideSubState::LeaveSpiralSlide:
            UpdateRideLeaveSpiralSlide();
            break;
        case PeepRideSubState::MazePathfinding:
            UpdateRideMazePathfinding();
            break;
        case PeepRideSubState::LeaveExit:
            UpdateRideLeaveExit();
            break;
        case PeepRideSubState::ApproachShop:
            UpdateRideShopApproach();
            break;
        case PeepRideSubState::InteractShop:
            UpdateRideShopInteract();
            break;
        case PeepRideSubState::LeaveShop:
            UpdateRideShopLeave();
            break;
        default:
            // Invalid peep sub-state
            assert(false);
            break;
    }
}

static void peep_update_walking_break_scenery(Guest* peep);
static bool peep_find_ride_to_look_at(Peep* peep, uint8_t edge, RideId* rideToView, uint8_t* rideSeatToView);

/**
 *
 *  rct2: 0x0069030A
 */
void Guest::UpdateWalking()
{
    if (!CheckForPath())
        return;

    if (PeepFlags & PEEP_FLAGS_WAVING)
    {
        if (IsActionInterruptable())
        {
            if ((0xFFFF & scenario_rand()) < 936)
            {
                Action = PeepActionType::Wave2;
                ActionFrame = 0;
                ActionSpriteImageOffset = 0;

                UpdateCurrentActionSpriteType();
            }
        }
    }

    if (PeepFlags & PEEP_FLAGS_PHOTO)
    {
        if (IsActionInterruptable())
        {
            if ((0xFFFF & scenario_rand()) < 936)
            {
                Action = PeepActionType::TakePhoto;
                ActionFrame = 0;
                ActionSpriteImageOffset = 0;

                UpdateCurrentActionSpriteType();
            }
        }
    }

    if (PeepFlags & PEEP_FLAGS_PAINTING)
    {
        if (IsActionInterruptable())
        {
            if ((0xFFFF & scenario_rand()) < 936)
            {
                Action = PeepActionType::DrawPicture;
                ActionFrame = 0;
                ActionSpriteImageOffset = 0;

                UpdateCurrentActionSpriteType();
            }
        }
    }

    if (PeepFlags & PEEP_FLAGS_LITTER)
    {
        if (!GetNextIsSurface())
        {
            if ((0xFFFF & scenario_rand()) <= 4096)
            {
                static constexpr const Litter::Type litter_types[] = {
                    Litter::Type::EmptyCan,
                    Litter::Type::Rubbish,
                    Litter::Type::BurgerBox,
                    Litter::Type::EmptyCup,
                };
                auto litterType = litter_types[scenario_rand() & 0x3];
                const auto loc = GetLocation();
                int32_t litterX = loc.x + (scenario_rand() & 0x7) - 3;
                int32_t litterY = loc.y + (scenario_rand() & 0x7) - 3;
                Direction litterDirection = (scenario_rand() & 0x3);

                Litter::Create({ litterX, litterY, loc.z, litterDirection }, litterType);
            }
        }
    }
    else if (HasEmptyContainer())
    {
        if ((!GetNextIsSurface()) && (static_cast<uint32_t>(sprite_index.ToUnderlying() & 0x1FF) == (gCurrentTicks & 0x1FF))
            && ((0xFFFF & scenario_rand()) <= 4096))
        {
            int32_t container = bitscanforward(GetEmptyContainerFlags());
            auto litterType = Litter::Type::Vomit;

            if (container != -1)
            {
                auto item = static_cast<ShopItem>(container);
                RemoveItem(item);
                litterType = Litter::Type(GetShopItemDescriptor(item).Type);
            }

            WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
            UpdateSpriteType();

            const auto loc = GetLocation();
            int32_t litterX = loc.x + (scenario_rand() & 0x7) - 3;
            int32_t litterY = loc.y + (scenario_rand() & 0x7) - 3;
            Direction litterDirection = (scenario_rand() & 0x3);

            Litter::Create({ litterX, litterY, loc.z, litterDirection }, litterType);
        }
    }

    // Check if vehicle is blocking the destination tile
    auto curPos = TileCoordsXYZ(GetLocation());
    auto dstPos = TileCoordsXYZ(CoordsXYZ{ GetDestination(), NextLoc.z });
    if (curPos.x != dstPos.x || curPos.y != dstPos.y)
    {
        if (footpath_is_blocked_by_vehicle(dstPos))
        {
            // Wait for vehicle to pass
            return;
        }
    }

    uint8_t pathingResult;
    PerformNextAction(pathingResult);
    if (!(pathingResult & PATHING_DESTINATION_REACHED))
        return;

    if (GetNextIsSurface())
    {
        auto surfaceElement = map_get_surface_element_at(NextLoc);

        if (surfaceElement != nullptr)
        {
            int32_t water_height = surfaceElement->GetWaterHeight();
            if (water_height > 0)
            {
                MoveTo({ x, y, water_height });
                SetState(PeepState::Falling);
                return;
            }
        }
    }

    CheckIfLost();
    CheckCantFindRide();
    CheckCantFindExit();

    if (UpdateWalkingFindBench())
        return;

    if (UpdateWalkingFindBin())
        return;

    peep_update_walking_break_scenery(this);

    if (State != PeepState::Walking)
        return;

    if (PeepFlags & PEEP_FLAGS_LEAVING_PARK)
        return;

    if (Nausea > 140)
        return;

    if (Happiness < 120)
        return;

    if (Toilet > 140)
        return;

    uint16_t chance = HasFoodOrDrink() ? 13107 : 2849;

    if ((scenario_rand() & 0xFFFF) > chance)
        return;

    if (GetNextIsSurface() || GetNextIsSloped())
        return;

    TileElement* tileElement = map_get_first_element_at(NextLoc);
    if (tileElement == nullptr)
        return;

    for (;; tileElement++)
    {
        if (tileElement->GetType() == TileElementType::Path)
        {
            if (NextLoc.z == tileElement->GetBaseZ())
                break;
        }
        if (tileElement->IsLastForTile())
        {
            return;
        }
    }

    int32_t positions_free = 15;

    if (tileElement->AsPath()->HasAddition())
    {
        if (!tileElement->AsPath()->AdditionIsGhost())
        {
            auto* pathAddEntry = tileElement->AsPath()->GetAdditionEntry();
            if (pathAddEntry == nullptr)
            {
                return;
            }

            if (!(pathAddEntry->flags & PATH_BIT_FLAG_IS_BENCH))
                positions_free = 9;
        }
    }

    int32_t edges = (tileElement->AsPath()->GetEdges()) ^ 0xF;
    if (edges == 0)
        return;

    uint8_t chosen_edge = scenario_rand() & 0x3;

    for (; !(edges & (1 << chosen_edge));)
        chosen_edge = (chosen_edge + 1) & 3;

    RideId ride_to_view;
    uint8_t ride_seat_to_view;
    if (!peep_find_ride_to_look_at(this, chosen_edge, &ride_to_view, &ride_seat_to_view))
        return;

    // Check if there is a peep watching (and if there is place for us)
    for (auto peep : EntityTileList<Peep>({ x, y }))
    {
        if (peep->State != PeepState::Watching)
            continue;

        if (z != peep->z)
            continue;

        if ((peep->Var37 & 0x3) != chosen_edge)
            continue;

        positions_free &= ~(1 << ((peep->Var37 & 0x1C) >> 2));
    }

    if (!positions_free)
        return;

    uint8_t chosen_position = scenario_rand() & 0x3;

    for (; !(positions_free & (1 << chosen_position));)
        chosen_position = (chosen_position + 1) & 3;

    CurrentRide = ride_to_view;
    CurrentSeat = ride_seat_to_view;
    Var37 = chosen_edge | (chosen_position << 2);

    SetState(PeepState::Watching);
    SubState = 0;

    int32_t destX = (x & 0xFFE0) + _WatchingPositionOffsets[Var37 & 0x1F].x;
    int32_t destY = (y & 0xFFE0) + _WatchingPositionOffsets[Var37 & 0x1F].y;

    SetDestination({ destX, destY }, 3);

    if (CurrentSeat & 1)
    {
        InsertNewThought(PeepThoughtType::NewRide);
    }
    if (CurrentRide.IsNull())
    {
        InsertNewThought(PeepThoughtType::Scenery);
    }
}

/**
 *
 *  rct2: 0x69185D
 */
void Guest::UpdateQueuing()
{
    if (!CheckForPath())
    {
        RemoveFromQueue();
        return;
    }
    auto ride = get_ride(CurrentRide);
    if (ride == nullptr || ride->status != RideStatus::Open)
    {
        RemoveFromQueue();
        SetState(PeepState::One);
        return;
    }

    // If not in the queue then at front of queue
    if (RideSubState != PeepRideSubState::InQueue)
    {
        bool is_front = true;
        // Fix #4819: Occasionally the peep->GuestNextInQueue is incorrectly set
        // to prevent this from causing the peeps to enter a loop
        // first check if the next in queue is actually nearby
        // if they are not then it's safe to assume that this is
        // the front of the queue.
        Peep* nextGuest = GetEntity<Guest>(GuestNextInQueue);
        if (nextGuest != nullptr)
        {
            if (abs(nextGuest->x - x) < 32 && abs(nextGuest->y - y) < 32)
            {
                is_front = false;
            }
        }

        if (is_front)
        {
            // Happens every time peep goes onto ride.
            DestinationTolerance = 0;
            SetState(PeepState::QueuingFront);
            RideSubState = PeepRideSubState::AtEntrance;
            return;
        }

        // Give up queueing for the ride
        sprite_direction ^= (1 << 4);
        Invalidate();
        RemoveFromQueue();
        SetState(PeepState::One);
        return;
    }

    uint8_t pathingResult;
    PerformNextAction(pathingResult);
    if (!IsActionInterruptable())
        return;
    if (SpriteType == PeepSpriteType::Normal)
    {
        if (TimeInQueue >= 2000 && (0xFFFF & scenario_rand()) <= 119)
        {
            // Eat Food/Look at watch
            Action = PeepActionType::EatFood;
            ActionFrame = 0;
            ActionSpriteImageOffset = 0;
            UpdateCurrentActionSpriteType();
        }
        if (TimeInQueue >= 3500 && (0xFFFF & scenario_rand()) <= 93)
        {
            // Create the I have been waiting in line ages thought
            InsertNewThought(PeepThoughtType::QueuingAges, CurrentRide);
        }
    }
    else
    {
        if (!(TimeInQueue & 0x3F) && IsActionIdle() && NextActionSpriteType == PeepActionSpriteType::WatchRide)
        {
            switch (SpriteType)
            {
                case PeepSpriteType::IceCream:
                case PeepSpriteType::Chips:
                case PeepSpriteType::Burger:
                case PeepSpriteType::Drink:
                case PeepSpriteType::Candyfloss:
                case PeepSpriteType::Pizza:
                case PeepSpriteType::Popcorn:
                case PeepSpriteType::HotDog:
                case PeepSpriteType::Tentacle:
                case PeepSpriteType::ToffeeApple:
                case PeepSpriteType::Doughnut:
                case PeepSpriteType::Coffee:
                case PeepSpriteType::Chicken:
                case PeepSpriteType::Lemonade:
                case PeepSpriteType::Pretzel:
                case PeepSpriteType::SuJongkwa:
                case PeepSpriteType::Juice:
                case PeepSpriteType::FunnelCake:
                case PeepSpriteType::Noodles:
                case PeepSpriteType::Sausage:
                case PeepSpriteType::Soup:
                case PeepSpriteType::Sandwich:
                    // Eat food
                    Action = PeepActionType::EatFood;
                    ActionFrame = 0;
                    ActionSpriteImageOffset = 0;
                    UpdateCurrentActionSpriteType();
                    break;
                default:
                    break;
            }
        }
    }
    if (TimeInQueue < 4300)
        return;

    if (Happiness <= 65 && (0xFFFF & scenario_rand()) < 2184)
    {
        // Give up queueing for the ride
        sprite_direction ^= (1 << 4);
        Invalidate();
        RemoveFromQueue();
        SetState(PeepState::One);
    }
}

/**
 * rct2: 0x691451
 */
void Guest::UpdateEnteringPark()
{
    if (Var37 != 1)
    {
        uint8_t pathingResult;
        PerformNextAction(pathingResult);
        if ((pathingResult & PATHING_OUTSIDE_PARK))
        {
            decrement_guests_heading_for_park();
            peep_sprite_remove(this);
        }
        return;
    }
    if (auto loc = UpdateAction(); loc.has_value())
    {
        MoveTo({ loc.value(), z });
        return;
    }
    SetState(PeepState::Falling);

    OutsideOfPark = false;
    ParkEntryTime = gCurrentTicks;
    increment_guests_in_park();
    decrement_guests_heading_for_park();
    auto intent = Intent(INTENT_ACTION_UPDATE_GUEST_COUNT);
    context_broadcast_intent(&intent);
}

/**
 *
 *  rct2: 0x6914CD
 */
void Guest::UpdateLeavingPark()
{
    if (Var37 != 0)
    {
        uint8_t pathingResult;
        PerformNextAction(pathingResult);
        if (!(pathingResult & PATHING_OUTSIDE_PARK))
            return;
        peep_sprite_remove(this);
        return;
    }

    if (auto loc = UpdateAction(); loc.has_value())
    {
        MoveTo({ loc.value(), z });
        return;
    }

    OutsideOfPark = true;
    DestinationTolerance = 5;
    decrement_guests_in_park();
    auto intent = Intent(INTENT_ACTION_UPDATE_GUEST_COUNT);
    context_broadcast_intent(&intent);
    Var37 = 1;

    window_invalidate_by_class(WC_GUEST_LIST);
    uint8_t pathingResult;
    PerformNextAction(pathingResult);
    if (!(pathingResult & PATHING_OUTSIDE_PARK))
        return;
    Remove();
}

/**
 *
 *  rct2: 0x6916D6
 */
void Guest::UpdateWatching()
{
    if (SubState == 0)
    {
        if (!CheckForPath())
            return;
        uint8_t pathingResult;
        PerformNextAction(pathingResult);
        if (!(pathingResult & PATHING_DESTINATION_REACHED))
            return;

        SetDestination(GetLocation());

        sprite_direction = (Var37 & 3) * 8;

        Action = PeepActionType::Idle;
        NextActionSpriteType = PeepActionSpriteType::WatchRide;

        SwitchNextActionSpriteType();

        SubState++;

        TimeToStand = std::clamp(((129 - Energy) * 16 + 50) / 2, 0, 255);
        UpdateSpriteType();
    }
    else if (SubState == 1)
    {
        if (!IsActionInterruptable())
        {
            // 6917F6
            UpdateAction();
            Invalidate();
            if (!IsActionWalking())
                return;
            Action = PeepActionType::Idle;
        }
        else
        {
            if (HasFoodOrDrink())
            {
                if ((scenario_rand() & 0xFFFF) <= 1310)
                {
                    Action = PeepActionType::EatFood;
                    ActionFrame = 0;
                    ActionSpriteImageOffset = 0;
                    UpdateCurrentActionSpriteType();
                    return;
                }
            }

            if ((scenario_rand() & 0xFFFF) <= 655)
            {
                Action = PeepActionType::TakePhoto;
                ActionFrame = 0;
                ActionSpriteImageOffset = 0;
                UpdateCurrentActionSpriteType();
                return;
            }

            if ((StandingFlags & 1))
            {
                if ((scenario_rand() & 0xFFFF) <= 655)
                {
                    Action = PeepActionType::Wave;
                    ActionFrame = 0;
                    ActionSpriteImageOffset = 0;
                    UpdateCurrentActionSpriteType();
                    return;
                }
            }
        }

        StandingFlags ^= (1 << 7);
        if (!(StandingFlags & (1 << 7)))
            return;

        TimeToStand--;
        if (TimeToStand != 0)
            return;

        SetState(PeepState::Walking);
        UpdateSpriteType();
        // Send peep to the centre of current tile.

        auto destination = GetLocation().ToTileCentre();
        SetDestination(destination, 5);
        UpdateCurrentActionSpriteType();
    }
}

/**
 *
 *  rct2: 0x00691089
 */
void Guest::UpdateUsingBin()
{
    switch (UsingBinSubState)
    {
        case PeepUsingBinSubState::WalkingToBin:
        {
            if (!CheckForPath())
                return;

            uint8_t pathingResult;
            PerformNextAction(pathingResult);
            if (pathingResult & PATHING_DESTINATION_REACHED)
            {
                UsingBinSubState = PeepUsingBinSubState::GoingBack;
            }
            break;
        }
        case PeepUsingBinSubState::GoingBack:
        {
            if (!IsActionWalking())
            {
                UpdateAction();
                Invalidate();
                return;
            }

            PathElement* foundElement = nullptr;
            for (auto* pathElement : TileElementsView<PathElement>(NextLoc))
            {
                if (pathElement->GetBaseZ() != NextLoc.z)
                    continue;

                if (!pathElement->HasAddition())
                    break;

                auto* pathAddEntry = pathElement->GetAdditionEntry();
                if (!(pathAddEntry->flags & PATH_BIT_FLAG_IS_BIN))
                    break;

                if (pathElement->IsBroken())
                    break;

                if (pathElement->AdditionIsGhost())
                    break;

                foundElement = pathElement;
                break;
            }

            if (foundElement == nullptr)
            {
                StateReset();
                return;
            }

            // Bin selection is one of 4 corners
            uint8_t selectedBin = Var37 * 2;

            // This counts down 2 = No rubbish, 0 = full
            uint8_t spaceLeftInBin = 0x3 & (foundElement->GetAdditionStatus() >> selectedBin);
            uint64_t emptyContainers = GetEmptyContainerFlags();

            for (uint8_t curContainer = 0; curContainer < 64; curContainer++)
            {
                if (!(emptyContainers & (1ULL << curContainer)))
                    continue;

                auto item = ShopItem(curContainer);
                if (spaceLeftInBin != 0)
                {
                    // OpenRCT2 modification: This previously used
                    // the tick count as a simple random function
                    // switched to scenario_rand as it is more reliable
                    if ((scenario_rand() & 7) == 0)
                        spaceLeftInBin--;
                    RemoveItem(item);
                    WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                    UpdateSpriteType();
                    continue;
                }

                auto litterType = Litter::Type(GetShopItemDescriptor(item).Type);

                int32_t litterX = x + (scenario_rand() & 7) - 3;
                int32_t litterY = y + (scenario_rand() & 7) - 3;

                Litter::Create({ litterX, litterY, z, static_cast<Direction>(scenario_rand() & 3) }, litterType);
                RemoveItem(item);
                WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;

                UpdateSpriteType();
            }

            uint8_t additionStatus = foundElement->GetAdditionStatus();
            // Place new amount in bin by first clearing the value
            additionStatus &= ~(3 << selectedBin);
            // Then placing the new value.
            additionStatus |= spaceLeftInBin << selectedBin;
            foundElement->SetAdditionStatus(additionStatus);

            map_invalidate_tile_zoom0({ NextLoc, foundElement->GetBaseZ(), foundElement->GetClearanceZ() });
            StateReset();
            break;
        }
        default:
            Guard::Assert(false, "Invalid sub state");
            break;
    }
}

/* Simplifies 0x690582. Returns true if should find bench*/
bool Guest::ShouldFindBench()
{
    if (PeepFlags & PEEP_FLAGS_LEAVING_PARK)
    {
        return false;
    }

    if (HasFoodOrDrink())
    {
        if (Hunger < 128 || Happiness < 128)
        {
            if (!GetNextIsSurface() && !GetNextIsSloped())
            {
                return true;
            }
        }
    }

    if (Nausea <= 170 && Energy > 50)
    {
        return false;
    }

    return !GetNextIsSurface() && !GetNextIsSloped();
}

static PathElement* FindBench(const CoordsXYZ& loc)
{
    for (auto* pathElement : TileElementsView<PathElement>(loc))
    {
        if (pathElement->GetBaseZ() != loc.z)
            continue;

        if (!pathElement->HasAddition())
            continue;

        auto* pathAddEntry = pathElement->GetAdditionEntry();
        if (pathAddEntry == nullptr || !(pathAddEntry->flags & PATH_BIT_FLAG_IS_BENCH))
            continue;

        if (pathElement->IsBroken())
            continue;

        if (pathElement->AdditionIsGhost())
            continue;

        return pathElement;
    }

    return nullptr;
}

/**
 *
 *  rct2: 0x00690582
 * Returns true when the guest wants to sit down and has found a bench to sit on
 */
bool Guest::UpdateWalkingFindBench()
{
    if (!ShouldFindBench())
        return false;

    auto* pathElement = FindBench(NextLoc);
    if (pathElement == nullptr)
        return false;

    int32_t edges = pathElement->GetEdges() ^ 0xF;
    if (edges == 0)
        return false;
    uint8_t chosen_edge = scenario_rand() & 0x3;

    for (; !(edges & (1 << chosen_edge));)
        chosen_edge = (chosen_edge + 1) & 0x3;

    uint8_t free_edge = 3;

    // Check if there is no peep sitting in chosen_edge
    for (auto peep : EntityTileList<Peep>({ x, y }))
    {
        if (peep->State != PeepState::Sitting)
            continue;

        if (z != peep->z)
            continue;

        if ((peep->Var37 & 0x3) != chosen_edge)
            continue;

        free_edge &= ~(1 << ((peep->Var37 & 0x4) >> 2));
    }

    if (!free_edge)
        return false;

    free_edge ^= 0x3;
    if (!free_edge)
    {
        if (scenario_rand() & 0x8000000)
            free_edge = 1;
    }

    Var37 = ((free_edge & 1) << 2) | chosen_edge;

    SetState(PeepState::Sitting);

    SittingSubState = PeepSittingSubState::TryingToSit;

    int32_t benchX = (x & 0xFFE0) + BenchUseOffsets[Var37 & 0x7].x;
    int32_t benchY = (y & 0xFFE0) + BenchUseOffsets[Var37 & 0x7].y;

    SetDestination({ benchX, benchY }, 3);

    return true;
}

static PathElement* FindBin(const CoordsXYZ& loc)
{
    for (auto* pathElement : TileElementsView<PathElement>(loc))
    {
        if (pathElement->GetBaseZ() != loc.z)
            continue;

        if (!pathElement->HasAddition())
            continue;

        auto* pathAddEntry = pathElement->GetAdditionEntry();
        if (pathAddEntry == nullptr || !(pathAddEntry->flags & PATH_BIT_FLAG_IS_BIN))
            continue;

        if (pathElement->IsBroken())
            continue;

        if (pathElement->AdditionIsGhost())
            continue;

        return pathElement;
    }

    return nullptr;
}

bool Guest::UpdateWalkingFindBin()
{
    auto peep = this;
    if (!peep->HasEmptyContainer())
        return false;

    if (peep->GetNextIsSurface())
        return false;

    auto* pathElement = FindBin(peep->NextLoc);
    if (pathElement == nullptr)
        return false;

    int32_t edges = (pathElement->GetEdges()) ^ 0xF;
    if (edges == 0)
        return false;

    uint8_t chosen_edge = scenario_rand() & 0x3;

    // Note: Bin quantity is inverted 0 = full, 3 = empty
    uint8_t bin_quantities = pathElement->GetAdditionStatus();

    // Rotate the bin to the correct edge. Makes it easier for next calc.
    bin_quantities = Numerics::ror8(Numerics::ror8(bin_quantities, chosen_edge), chosen_edge);

    for (uint8_t free_edge = 4; free_edge != 0; free_edge--)
    {
        // If not full
        if (bin_quantities & 0x3)
        {
            if (edges & (1 << chosen_edge))
                break;
        }
        chosen_edge = (chosen_edge + 1) & 0x3;
        bin_quantities = Numerics::ror8(bin_quantities, 2);
        if ((free_edge - 1) == 0)
            return 0;
    }

    peep->Var37 = chosen_edge;

    peep->SetState(PeepState::UsingBin);
    peep->UsingBinSubState = PeepUsingBinSubState::WalkingToBin;

    int32_t binX = (peep->x & 0xFFE0) + BinUseOffsets[peep->Var37 & 0x3].x;
    int32_t binY = (peep->y & 0xFFE0) + BinUseOffsets[peep->Var37 & 0x3].y;

    peep->SetDestination({ binX, binY }, 3);

    return true;
}

static PathElement* FindBreakableElement(const CoordsXYZ& loc)
{
    for (auto* pathElement : TileElementsView<PathElement>(loc))
    {
        if (pathElement->GetBaseZ() != loc.z)
            continue;

        if (!pathElement->HasAddition())
            continue;

        auto* pathAddEntry = pathElement->GetAdditionEntry();
        if (pathAddEntry == nullptr || !(pathAddEntry->flags & PATH_BIT_FLAG_BREAKABLE))
            continue;

        if (pathElement->IsBroken())
            continue;

        if (pathElement->AdditionIsGhost())
            continue;

        return pathElement;
    }

    return nullptr;
}

/**
 *
 *  rct2: 0x00690848
 */
static void peep_update_walking_break_scenery(Guest* peep)
{
    if (gCheatsDisableVandalism)
        return;

    if (!(peep->PeepFlags & PEEP_FLAGS_ANGRY))
    {
        if (peep->Happiness >= 48)
            return;
        if (peep->Energy < 85)
            return;
        if (peep->State != PeepState::Walking)
            return;

        if ((peep->LitterCount & 0xC0) != 0xC0 && (peep->DisgustingCount & 0xC0) != 0xC0)
            return;

        if ((scenario_rand() & 0xFFFF) > 3276)
            return;
    }

    if (peep->GetNextIsSurface())
        return;

    auto* tileElement = FindBreakableElement(peep->NextLoc);
    if (tileElement == nullptr)
        return;

    int32_t edges = tileElement->GetEdges();
    if (edges == 0xF)
        return;

    // Check if a peep is already sitting on the bench. If so, do not vandalise it.
    for (auto peep2 : EntityTileList<Peep>({ peep->x, peep->y }))
    {
        if ((peep2->State != PeepState::Sitting) || (peep->z != peep2->z))
        {
            continue;
        }

        return;
    }

    for (auto inner_peep : EntityList<Staff>())
    {
        if (inner_peep->AssignedStaffType != StaffType::Security)
            continue;

        if (inner_peep->x == LOCATION_NULL)
            continue;

        int32_t x_diff = abs(inner_peep->x - peep->x);
        int32_t y_diff = abs(inner_peep->y - peep->y);

        if (std::max(x_diff, y_diff) < 224)
        {
            inner_peep->StaffVandalsStopped++;
            return;
        }
    }

    tileElement->SetIsBroken(true);

    map_invalidate_tile_zoom1({ peep->NextLoc, tileElement->GetBaseZ(), tileElement->GetBaseZ() + 32 });

    peep->Angriness = 16;
}

/**
 * rct2: 0x0069101A
 *
 * @return (CF)
 */
static bool peep_should_watch_ride(TileElement* tileElement)
{
    // Ghosts are purely this-client-side and should not cause any interaction,
    // as that may lead to a desync.
    if (network_get_mode() != NETWORK_MODE_NONE)
    {
        if (tileElement->IsGhost())
            return false;
    }

    auto ride = get_ride(tileElement->AsTrack()->GetRideIndex());
    if (ride == nullptr || !ride->IsRide())
    {
        return false;
    }

    // This is most likely to have peeps watch new rides
    if (ride->excitement == RIDE_RATING_UNDEFINED)
    {
        return true;
    }

    if (ride->excitement >= RIDE_RATING(4, 70))
    {
        return true;
    }

    if (ride->intensity >= RIDE_RATING(4, 50))
    {
        return true;
    }

    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_INTERESTING_TO_LOOK_AT))
    {
        if ((scenario_rand() & 0xFFFF) > 0x3333)
        {
            return false;
        }
    }
    else if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_SLIGHTLY_INTERESTING_TO_LOOK_AT))
    {
        if ((scenario_rand() & 0xFFFF) > 0x1000)
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool loc_690FD0(Peep* peep, RideId* rideToView, uint8_t* rideSeatToView, TileElement* tileElement)
{
    auto ride = get_ride(tileElement->AsTrack()->GetRideIndex());
    if (ride == nullptr)
        return false;

    *rideToView = ride->id;
    if (ride->excitement == RIDE_RATING_UNDEFINED)
    {
        *rideSeatToView = 1;
        if (ride->status != RideStatus::Open)
        {
            if (tileElement->GetClearanceZ() > peep->NextLoc.z + (8 * COORDS_Z_STEP))
            {
                *rideSeatToView |= (1 << 1);
            }

            return true;
        }
    }
    else
    {
        *rideSeatToView = 0;
        if (ride->status == RideStatus::Open && !(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN))
        {
            if (tileElement->GetClearanceZ() > peep->NextLoc.z + (8 * COORDS_Z_STEP))
            {
                *rideSeatToView = 0x02;
            }

            return true;
        }
    }

    return false;
}

/**
 *
 *  rct2: 0x00690B99
 *
 * @param edge (eax)
 * @param peep (esi)
 * @param[out] rideToView (cl)
 * @param[out] rideSeatToView (ch)
 * @return !CF
 */
static bool peep_find_ride_to_look_at(Peep* peep, uint8_t edge, RideId* rideToView, uint8_t* rideSeatToView)
{
    TileElement* tileElement;

    auto surfaceElement = map_get_surface_element_at(peep->NextLoc);

    tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    if (tileElement == nullptr)
    {
        return false;
    }

    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetType() != TileElementType::Wall)
            continue;
        if (tileElement->GetDirection() != edge)
            continue;
        auto wallEntry = tileElement->AsWall()->GetEntry();
        if (wallEntry == nullptr || (wallEntry->flags2 & WALL_SCENERY_2_IS_OPAQUE))
            continue;
        if (peep->NextLoc.z + (4 * COORDS_Z_STEP) <= tileElement->GetBaseZ())
            continue;
        if (peep->NextLoc.z + (1 * COORDS_Z_STEP) >= tileElement->GetClearanceZ())
            continue;

        return false;
    } while (!(tileElement++)->IsLastForTile());

    uint16_t x = peep->NextLoc.x + CoordsDirectionDelta[edge].x;
    uint16_t y = peep->NextLoc.y + CoordsDirectionDelta[edge].y;
    if (!map_is_location_valid({ x, y }))
    {
        return false;
    }

    surfaceElement = map_get_surface_element_at(CoordsXY{ x, y });

    tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    if (tileElement == nullptr)
    {
        return false;
    }

    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetType() != TileElementType::Wall)
            continue;
        if (direction_reverse(tileElement->GetDirection()) != edge)
            continue;
        auto wallEntry = tileElement->AsWall()->GetEntry();
        if (wallEntry == nullptr || (wallEntry->flags2 & WALL_SCENERY_2_IS_OPAQUE))
            continue;
        // TODO: Check whether this shouldn't be <=, as the other loops use. If so, also extract as loop A.
        if (peep->NextLoc.z + (4 * COORDS_Z_STEP) >= tileElement->GetBaseZ())
            continue;
        if (peep->NextLoc.z + (1 * COORDS_Z_STEP) >= tileElement->GetClearanceZ())
            continue;

        return false;
    } while (!(tileElement++)->IsLastForTile());

    // TODO: Extract loop B
    tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }

        if (tileElement->GetClearanceZ() + (1 * COORDS_Z_STEP) < peep->NextLoc.z)
            continue;
        if (peep->NextLoc.z + (6 * COORDS_Z_STEP) < tileElement->GetBaseZ())
            continue;

        if (tileElement->GetType() == TileElementType::Track)
        {
            if (peep_should_watch_ride(tileElement))
            {
                return loc_690FD0(peep, rideToView, rideSeatToView, tileElement);
            }
        }

        if (tileElement->GetType() == TileElementType::LargeScenery)
        {
            const auto* sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
            if (sceneryEntry == nullptr || !(sceneryEntry->flags & LARGE_SCENERY_FLAG_PHOTOGENIC))
            {
                continue;
            }

            *rideSeatToView = 0;
            if (tileElement->GetClearanceZ() >= peep->NextLoc.z + (8 * COORDS_Z_STEP))
            {
                *rideSeatToView = 0x02;
            }

            *rideToView = RideId::GetNull();

            return true;
        }
    } while (!(tileElement++)->IsLastForTile());

    // TODO: Extract loop C
    tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetClearanceZ() + (1 * COORDS_Z_STEP) < peep->NextLoc.z)
            continue;
        if (peep->NextLoc.z + (6 * COORDS_Z_STEP) < tileElement->GetBaseZ())
            continue;
        if (tileElement->GetType() == TileElementType::Surface)
            continue;
        if (tileElement->GetType() == TileElementType::Path)
            continue;

        if (tileElement->GetType() == TileElementType::Wall)
        {
            auto wallEntry = tileElement->AsWall()->GetEntry();
            if (wallEntry == nullptr || (wallEntry->flags2 & WALL_SCENERY_2_IS_OPAQUE))
            {
                continue;
            }
        }

        return false;
    } while (!(tileElement++)->IsLastForTile());

    x += CoordsDirectionDelta[edge].x;
    y += CoordsDirectionDelta[edge].y;
    if (!map_is_location_valid({ x, y }))
    {
        return false;
    }

    surfaceElement = map_get_surface_element_at(CoordsXY{ x, y });

    // TODO: extract loop A
    tileElement = reinterpret_cast<TileElement*>(surfaceElement);

    if (tileElement == nullptr)
    {
        return false;
    }

    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetType() != TileElementType::Wall)
            continue;
        if (direction_reverse(tileElement->GetDirection()) != edge)
            continue;
        auto wallEntry = tileElement->AsWall()->GetEntry();
        if (wallEntry == nullptr || (wallEntry->flags2 & WALL_SCENERY_2_IS_OPAQUE))
            continue;
        if (peep->NextLoc.z + (6 * COORDS_Z_STEP) <= tileElement->GetBaseZ())
            continue;
        if (peep->NextLoc.z >= tileElement->GetClearanceZ())
            continue;

        return false;
    } while (!(tileElement++)->IsLastForTile());

    // TODO: Extract loop B
    tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetClearanceZ() + (1 * COORDS_Z_STEP) < peep->NextLoc.z)
            continue;
        if (peep->NextLoc.z + (8 * COORDS_Z_STEP) < tileElement->GetBaseZ())
            continue;

        if (tileElement->GetType() == TileElementType::Track)
        {
            if (peep_should_watch_ride(tileElement))
            {
                return loc_690FD0(peep, rideToView, rideSeatToView, tileElement);
            }
        }

        if (tileElement->GetType() == TileElementType::LargeScenery)
        {
            auto* sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
            if (!(sceneryEntry == nullptr || sceneryEntry->flags & LARGE_SCENERY_FLAG_PHOTOGENIC))
            {
                continue;
            }

            *rideSeatToView = 0;
            if (tileElement->GetClearanceZ() >= peep->NextLoc.z + (8 * COORDS_Z_STEP))
            {
                *rideSeatToView = 0x02;
            }

            *rideToView = RideId::GetNull();

            return true;
        }
    } while (!(tileElement++)->IsLastForTile());

    // TODO: Extract loop C
    tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetClearanceZ() + (1 * COORDS_Z_STEP) < peep->NextLoc.z)
            continue;
        if (peep->NextLoc.z + (8 * COORDS_Z_STEP) < tileElement->GetBaseZ())
            continue;
        if (tileElement->GetType() == TileElementType::Surface)
            continue;
        if (tileElement->GetType() == TileElementType::Path)
            continue;

        if (tileElement->GetType() == TileElementType::Wall)
        {
            auto wallEntry = tileElement->AsWall()->GetEntry();
            if (wallEntry == nullptr || (wallEntry->flags2 & WALL_SCENERY_2_IS_OPAQUE))
            {
                continue;
            }
        }

        return false;
    } while (!(tileElement++)->IsLastForTile());

    x += CoordsDirectionDelta[edge].x;
    y += CoordsDirectionDelta[edge].y;
    if (!map_is_location_valid({ x, y }))
    {
        return false;
    }

    surfaceElement = map_get_surface_element_at(CoordsXY{ x, y });

    // TODO: extract loop A
    tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    if (tileElement == nullptr)
    {
        return false;
    }

    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetType() != TileElementType::Wall)
            continue;
        if (direction_reverse(tileElement->GetDirection()) != edge)
            continue;
        auto wallEntry = tileElement->AsWall()->GetEntry();
        if (wallEntry == nullptr || (wallEntry->flags2 & WALL_SCENERY_2_IS_OPAQUE))
            continue;
        if (peep->NextLoc.z + (8 * COORDS_Z_STEP) <= tileElement->GetBaseZ())
            continue;
        if (peep->NextLoc.z >= tileElement->GetClearanceZ())
            continue;

        return false;
    } while (!(tileElement++)->IsLastForTile());

    // TODO: Extract loop B
    tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetClearanceZ() + (1 * COORDS_Z_STEP) < peep->NextLoc.z)
            continue;
        if (peep->NextLoc.z + (10 * COORDS_Z_STEP) < tileElement->GetBaseZ())
            continue;

        if (tileElement->GetType() == TileElementType::Track)
        {
            if (peep_should_watch_ride(tileElement))
            {
                return loc_690FD0(peep, rideToView, rideSeatToView, tileElement);
            }
        }

        if (tileElement->GetType() == TileElementType::LargeScenery)
        {
            const auto* sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
            if (sceneryEntry == nullptr || !(sceneryEntry->flags & LARGE_SCENERY_FLAG_PHOTOGENIC))
            {
                continue;
            }

            *rideSeatToView = 0;
            if (tileElement->GetClearanceZ() >= peep->NextLoc.z + (8 * COORDS_Z_STEP))
            {
                *rideSeatToView = 0x02;
            }

            *rideToView = RideId::GetNull();

            return true;
        }
    } while (!(tileElement++)->IsLastForTile());

    return false;
}

/* Part of 0x0069B8CC rct2: 0x0069BC31 */
void Guest::SetSpriteType(PeepSpriteType new_sprite_type)
{
    if (SpriteType == new_sprite_type)
        return;

    SpriteType = new_sprite_type;
    ActionSpriteImageOffset = 0;
    WalkingFrameNum = 0;

    if (IsActionInterruptable())
        Action = PeepActionType::Walking;

    PeepFlags &= ~PEEP_FLAGS_SLOW_WALK;
    Guard::Assert(EnumValue(new_sprite_type) < std::size(gSpriteTypeToSlowWalkMap));
    if (gSpriteTypeToSlowWalkMap[EnumValue(new_sprite_type)])
    {
        PeepFlags |= PEEP_FLAGS_SLOW_WALK;
    }

    ActionSpriteType = PeepActionSpriteType::Invalid;
    UpdateCurrentActionSpriteType();

    if (State == PeepState::Sitting)
    {
        Action = PeepActionType::Idle;
        NextActionSpriteType = PeepActionSpriteType::SittingIdle;
        SwitchNextActionSpriteType();
    }
    if (State == PeepState::Watching)
    {
        Action = PeepActionType::Idle;
        NextActionSpriteType = PeepActionSpriteType::WatchRide;
        SwitchNextActionSpriteType();
    }
}

struct item_pref_t
{
    ShopItem item;
    PeepSpriteType sprite_type;
};

// clang-format off
static item_pref_t item_order_preference[] = {
    { ShopItem::IceCream,         PeepSpriteType::IceCream    },
    { ShopItem::Chips,            PeepSpriteType::Chips       },
    { ShopItem::Pizza,            PeepSpriteType::Pizza       },
    { ShopItem::Burger,           PeepSpriteType::Burger      },
    { ShopItem::Drink,            PeepSpriteType::Drink       },
    { ShopItem::Coffee,           PeepSpriteType::Coffee      },
    { ShopItem::Chicken,          PeepSpriteType::Chicken     },
    { ShopItem::Lemonade,         PeepSpriteType::Lemonade    },
    { ShopItem::Candyfloss,       PeepSpriteType::Candyfloss  },
    { ShopItem::Popcorn,          PeepSpriteType::Popcorn     },
    { ShopItem::HotDog,           PeepSpriteType::HotDog      },
    { ShopItem::Tentacle,         PeepSpriteType::Tentacle    },
    { ShopItem::ToffeeApple,      PeepSpriteType::ToffeeApple },
    { ShopItem::Doughnut,         PeepSpriteType::Doughnut    },
    { ShopItem::Pretzel,          PeepSpriteType::Pretzel     },
    { ShopItem::Cookie,           PeepSpriteType::Pretzel     },
    { ShopItem::Chocolate,        PeepSpriteType::Coffee      },
    { ShopItem::IcedTea,          PeepSpriteType::Coffee      },
    { ShopItem::FunnelCake,       PeepSpriteType::FunnelCake  },
    { ShopItem::BeefNoodles,      PeepSpriteType::Noodles     },
    { ShopItem::FriedRiceNoodles, PeepSpriteType::Noodles     },
    { ShopItem::WontonSoup,       PeepSpriteType::Soup        },
    { ShopItem::MeatballSoup,     PeepSpriteType::Soup        },
    { ShopItem::FruitJuice,       PeepSpriteType::Juice       },
    { ShopItem::SoybeanMilk,      PeepSpriteType::SuJongkwa   },
    { ShopItem::Sujeonggwa,       PeepSpriteType::SuJongkwa   },
    { ShopItem::SubSandwich,      PeepSpriteType::Sandwich    },
    { ShopItem::RoastSausage,     PeepSpriteType::Sausage     },
    { ShopItem::Balloon,          PeepSpriteType::Balloon     },
    { ShopItem::Hat,              PeepSpriteType::Hat         },
    { ShopItem::Sunglasses,       PeepSpriteType::Sunglasses  },
};
// clang-format on

/**
 *
 *  rct2: 0x0069B8CC
 */
void Guest::UpdateSpriteType()
{
    if (SpriteType == PeepSpriteType::Balloon && (scenario_rand() & 0xFFFF) <= 327)
    {
        bool isBalloonPopped = false;
        if (x != LOCATION_NULL)
        {
            if ((scenario_rand() & 0xFFFF) <= 13107)
            {
                isBalloonPopped = true;
                OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::BalloonPop, { x, y, z });
            }
            Balloon::Create({ x, y, z + 9 }, BalloonColour, isBalloonPopped);
        }
        RemoveItem(ShopItem::Balloon);
        WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
    }

    if (climate_is_raining() && (HasItem(ShopItem::Umbrella)) && x != LOCATION_NULL)
    {
        CoordsXY loc = { x, y };
        if (map_is_location_valid(loc.ToTileStart()))
        {
            TileElement* tileElement = map_get_first_element_at(loc);
            while (true)
            {
                if (tileElement == nullptr)
                    break;
                if (z < tileElement->GetBaseZ())
                    break;

                if (tileElement->IsLastForTile())
                {
                    SetSpriteType(PeepSpriteType::Umbrella);
                    return;
                }
                tileElement++;
            }
        }
    }

    for (auto& itemPref : item_order_preference)
    {
        if (HasItem(itemPref.item))
        {
            SetSpriteType(itemPref.sprite_type);
            return;
        }
    }

    if (State == PeepState::Watching && StandingFlags & (1 << 1))
    {
        SetSpriteType(PeepSpriteType::Watching);
        return;
    }

    if (Nausea > 170)
    {
        SetSpriteType(PeepSpriteType::VeryNauseous);
        return;
    }

    if (Nausea > 140)
    {
        SetSpriteType(PeepSpriteType::Nauseous);
        return;
    }

    if (Energy <= 64 && Happiness < 128)
    {
        SetSpriteType(PeepSpriteType::HeadDown);
        return;
    }

    if (Energy <= 80 && Happiness < 128)
    {
        SetSpriteType(PeepSpriteType::ArmsCrossed);
        return;
    }

    if (Toilet > 220)
    {
        SetSpriteType(PeepSpriteType::RequireToilet);
        return;
    }

    SetSpriteType(PeepSpriteType::Normal);
}

bool Guest::HeadingForRideOrParkExit() const
{
    return (PeepFlags & PEEP_FLAGS_LEAVING_PARK) || !GuestHeadingToRideId.IsNull();
}

/**
 * rct2: 0x00698342
 * thought.item (eax)
 * thought.type (ebx)
 * argument_1 (esi & ebx)
 * argument_2 (esi+2)
 */
void peep_thought_set_format_args(const PeepThought* thought, Formatter& ft)
{
    ft.Add<rct_string_id>(PeepThoughts[EnumValue(thought->type)]);

    PeepThoughtToActionFlag flags = PeepThoughtToActionMap[EnumValue(thought->type)].flags;
    if (flags & PEEP_THOUGHT_ACTION_FLAG_RIDE)
    {
        auto ride = get_ride(thought->rideId);
        if (ride != nullptr)
        {
            ride->FormatNameTo(ft);
        }
        else
        {
            ft.Add<rct_string_id>(STR_NONE);
        }
    }
    else if (flags & PEEP_THOUGHT_ACTION_FLAG_SHOP_ITEM_SINGULAR)
    {
        ft.Add<rct_string_id>(GetShopItemDescriptor(thought->shopItem).Naming.Singular);
    }
    else if (flags & PEEP_THOUGHT_ACTION_FLAG_SHOP_ITEM_INDEFINITE)
    {
        ft.Add<rct_string_id>(GetShopItemDescriptor(thought->shopItem).Naming.Indefinite);
    }
}

void Guest::InsertNewThought(PeepThoughtType thought_type)
{
    InsertNewThought(thought_type, PeepThoughtItemNone);
}

void Guest::InsertNewThought(PeepThoughtType thought_type, ShopItem shopItem)
{
    InsertNewThought(thought_type, static_cast<uint16_t>(shopItem));
}

void Guest::InsertNewThought(PeepThoughtType thought_type, RideId rideId)
{
    InsertNewThought(thought_type, rideId.ToUnderlying());
}
/**
 *
 *  rct2: 0x699F5A
 * al:thoughtType
 * ah:thoughtArguments
 * esi: peep
 */
void Guest::InsertNewThought(PeepThoughtType thoughtType, uint16_t thoughtArguments)
{
    PeepActionType newAction = PeepThoughtToActionMap[EnumValue(thoughtType)].action;
    if (newAction != PeepActionType::Walking && IsActionInterruptable())
    {
        Action = newAction;
        ActionFrame = 0;
        ActionSpriteImageOffset = 0;
        UpdateCurrentActionSpriteType();
    }

    for (int32_t i = 0; i < PEEP_MAX_THOUGHTS; ++i)
    {
        PeepThought* thought = &Thoughts[i];
        // Remove the oldest thought by setting it to NONE.
        if (thought->type == PeepThoughtType::None)
            break;

        if (thought->type == thoughtType && thought->item == thoughtArguments)
        {
            // If the thought type has not changed then we need to move
            // it to the top of the thought list. This is done by first removing the
            // existing thought and placing it at the top.
            if (i < PEEP_MAX_THOUGHTS - 2)
            {
                memmove(thought, thought + 1, sizeof(PeepThought) * (PEEP_MAX_THOUGHTS - i - 1));
            }
            break;
        }
    }

    memmove(&std::get<1>(Thoughts), &std::get<0>(Thoughts), sizeof(PeepThought) * (PEEP_MAX_THOUGHTS - 1));

    auto& thought = std::get<0>(Thoughts);
    thought.type = thoughtType;
    thought.item = thoughtArguments;
    thought.freshness = 0;
    thought.fresh_timeout = 0;

    WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_THOUGHTS;
}

// clang-format off
/** rct2: 0x009823A0 */
static constexpr const PeepNauseaTolerance nausea_tolerance_distribution[] = {
    PeepNauseaTolerance::None,
    PeepNauseaTolerance::Low, PeepNauseaTolerance::Low,
    PeepNauseaTolerance::Average, PeepNauseaTolerance::Average, PeepNauseaTolerance::Average,
    PeepNauseaTolerance::High, PeepNauseaTolerance::High, PeepNauseaTolerance::High, PeepNauseaTolerance::High, PeepNauseaTolerance::High, PeepNauseaTolerance::High,
};

/** rct2: 0x009823BC */
static constexpr const uint8_t trouser_colours[] = {
    COLOUR_BLACK,
    COLOUR_GREY,
    COLOUR_LIGHT_BROWN,
    COLOUR_SATURATED_BROWN,
    COLOUR_DARK_BROWN,
    COLOUR_SALMON_PINK,
    COLOUR_BLACK,
    COLOUR_GREY,
    COLOUR_LIGHT_BROWN,
    COLOUR_SATURATED_BROWN,
    COLOUR_DARK_BROWN,
    COLOUR_SALMON_PINK,
    COLOUR_BLACK,
    COLOUR_GREY,
    COLOUR_LIGHT_BROWN,
    COLOUR_SATURATED_BROWN,
    COLOUR_DARK_BROWN,
    COLOUR_SALMON_PINK,
    COLOUR_DARK_PURPLE,
    COLOUR_LIGHT_PURPLE,
    COLOUR_DARK_BLUE,
    COLOUR_SATURATED_GREEN,
    COLOUR_SATURATED_RED,
    COLOUR_DARK_ORANGE,
    COLOUR_BORDEAUX_RED,
};

/** rct2: 0x009823D5 */
static constexpr const uint8_t tshirt_colours[] = {
    COLOUR_BLACK,
    COLOUR_GREY,
    COLOUR_LIGHT_BROWN,
    COLOUR_SATURATED_BROWN,
    COLOUR_DARK_BROWN,
    COLOUR_SALMON_PINK,
    COLOUR_BLACK,
    COLOUR_GREY,
    COLOUR_LIGHT_BROWN,
    COLOUR_SATURATED_BROWN,
    COLOUR_DARK_BROWN,
    COLOUR_SALMON_PINK,
    COLOUR_DARK_PURPLE,
    COLOUR_LIGHT_PURPLE,
    COLOUR_DARK_BLUE,
    COLOUR_SATURATED_GREEN,
    COLOUR_SATURATED_RED,
    COLOUR_DARK_ORANGE,
    COLOUR_BORDEAUX_RED,
    COLOUR_WHITE,
    COLOUR_BRIGHT_PURPLE,
    COLOUR_LIGHT_BLUE,
    COLOUR_TEAL,
    COLOUR_DARK_GREEN,
    COLOUR_MOSS_GREEN,
    COLOUR_BRIGHT_GREEN,
    COLOUR_OLIVE_GREEN,
    COLOUR_DARK_OLIVE_GREEN,
    COLOUR_YELLOW,
    COLOUR_LIGHT_ORANGE,
    COLOUR_BRIGHT_RED,
    COLOUR_DARK_PINK,
    COLOUR_BRIGHT_PINK,
};
// clang-format on

/**
 *
 *  rct2: 0x0069A05D
 */
Guest* Guest::Generate(const CoordsXYZ& coords)
{
    if (GetNumFreeEntities() < 400)
        return nullptr;

    Guest* peep = CreateEntity<Guest>();
    peep->SpriteType = PeepSpriteType::Normal;
    peep->OutsideOfPark = true;
    peep->State = PeepState::Falling;
    peep->Action = PeepActionType::Walking;
    peep->SpecialSprite = 0;
    peep->ActionSpriteImageOffset = 0;
    peep->WalkingFrameNum = 0;
    peep->ActionSpriteType = PeepActionSpriteType::None;
    peep->PeepFlags = 0;
    peep->FavouriteRide = RideId::GetNull();
    peep->FavouriteRideRating = 0;

    const rct_sprite_bounds* spriteBounds = &GetSpriteBounds(peep->SpriteType, peep->ActionSpriteType);
    peep->sprite_width = spriteBounds->sprite_width;
    peep->sprite_height_negative = spriteBounds->sprite_height_negative;
    peep->sprite_height_positive = spriteBounds->sprite_height_positive;

    peep->MoveTo(coords);
    peep->sprite_direction = 0;
    peep->Mass = (scenario_rand() & 0x1F) + 45;
    peep->PathCheckOptimisation = 0;
    peep->InteractionRideIndex = RideId::GetNull();
    peep->PreviousRide = RideId::GetNull();
    std::get<0>(peep->Thoughts).type = PeepThoughtType::None;
    peep->WindowInvalidateFlags = 0;

    uint8_t intensityHighest = (scenario_rand() & 0x7) + 3;
    uint8_t intensityLowest = std::min(intensityHighest, static_cast<uint8_t>(7)) - 3;

    if (intensityHighest >= 7)
        intensityHighest = 15;

    /* Check which intensity boxes are enabled
     * and apply the appropriate intensity settings. */
    if (gParkFlags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES)
    {
        if (gParkFlags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES)
        {
            intensityLowest = 0;
            intensityHighest = 15;
        }
        else
        {
            intensityLowest = 0;
            intensityHighest = 4;
        }
    }
    else if (gParkFlags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES)
    {
        intensityLowest = 9;
        intensityHighest = 15;
    }

    peep->Intensity = IntensityRange(intensityLowest, intensityHighest);

    uint8_t nauseaTolerance = scenario_rand() & 0x7;
    if (gParkFlags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES)
    {
        nauseaTolerance += 4;
    }

    peep->NauseaTolerance = nausea_tolerance_distribution[nauseaTolerance];

    /* Scenario editor limits initial guest happiness to between 37..253.
     * To be on the safe side, assume the value could have been hacked
     * to any value 0..255. */
    peep->Happiness = gGuestInitialHappiness;
    /* Assume a default initial happiness of 0 is wrong and set
     * to 128 (50%) instead. */
    if (gGuestInitialHappiness == 0)
        peep->Happiness = 128;
    /* Initial value will vary by -15..16 */
    int8_t happinessDelta = (scenario_rand() & 0x1F) - 15;
    /* Adjust by the delta, clamping at min=0 and max=255. */
    peep->Happiness = std::clamp(peep->Happiness + happinessDelta, 0, PEEP_MAX_HAPPINESS);
    peep->HappinessTarget = peep->Happiness;
    peep->Nausea = 0;
    peep->NauseaTarget = 0;

    /* Scenario editor limits initial guest hunger to between 37..253.
     * To be on the safe side, assume the value could have been hacked
     * to any value 0..255. */
    peep->Hunger = gGuestInitialHunger;
    /* Initial value will vary by -15..16 */
    int8_t hungerDelta = (scenario_rand() & 0x1F) - 15;
    /* Adjust by the delta, clamping at min=0 and max=255. */
    peep->Hunger = std::clamp(peep->Hunger + hungerDelta, 0, PEEP_MAX_HUNGER);

    /* Scenario editor limits initial guest thirst to between 37..253.
     * To be on the safe side, assume the value could have been hacked
     * to any value 0..255. */
    peep->Thirst = gGuestInitialThirst;
    /* Initial value will vary by -15..16 */
    int8_t thirstDelta = (scenario_rand() & 0x1F) - 15;
    /* Adjust by the delta, clamping at min=0 and max=255. */
    peep->Thirst = std::clamp(peep->Thirst + thirstDelta, 0, PEEP_MAX_THIRST);

    peep->Toilet = 0;
    peep->TimeToConsume = 0;

    peep->GuestNumRides = 0;
    peep->Id = gNextGuestNumber++;
    peep->Name = nullptr;

    money32 cash = (scenario_rand() & 0x3) * 100 - 100 + gGuestInitialCash;
    if (cash < 0)
        cash = 0;

    if (gGuestInitialCash == 0)
    {
        cash = 500;
    }

    if (gParkFlags & PARK_FLAGS_NO_MONEY)
    {
        cash = 0;
    }

    if (gGuestInitialCash == MONEY16_UNDEFINED)
    {
        cash = 0;
    }

    peep->CashInPocket = cash;
    peep->CashSpent = 0;
    peep->ParkEntryTime = -1;
    peep->ResetPathfindGoal();
    peep->RemoveAllItems();
    peep->GuestHeadingToRideId = RideId::GetNull();
    peep->LitterCount = 0;
    peep->DisgustingCount = 0;
    peep->VandalismSeen = 0;
    peep->PaidToEnter = 0;
    peep->PaidOnRides = 0;
    peep->PaidOnFood = 0;
    peep->PaidOnDrink = 0;
    peep->PaidOnSouvenirs = 0;
    peep->AmountOfFood = 0;
    peep->AmountOfDrinks = 0;
    peep->AmountOfSouvenirs = 0;
    peep->SurroundingsThoughtTimeout = 0;
    peep->Angriness = 0;
    peep->TimeLost = 0;

    uint8_t tshirtColour = static_cast<uint8_t>(scenario_rand() % std::size(tshirt_colours));
    peep->TshirtColour = tshirt_colours[tshirtColour];

    uint8_t trousersColour = static_cast<uint8_t>(scenario_rand() % std::size(trouser_colours));
    peep->TrousersColour = trouser_colours[trousersColour];

    /* Minimum energy is capped at 32 and maximum at 128, so this initialises
     * a peep with approx 34%-100% energy. (65 - 32) / (128 - 32) ≈ 34% */
    uint8_t energy = (scenario_rand() % 64) + 65;
    peep->Energy = energy;
    peep->EnergyTarget = energy;

    increment_guests_heading_for_park();

#ifdef ENABLE_SCRIPTING
    auto& hookEngine = OpenRCT2::GetContext()->GetScriptEngine().GetHookEngine();
    if (hookEngine.HasSubscriptions(OpenRCT2::Scripting::HOOK_TYPE::GUEST_GENERATION))
    {
        auto ctx = OpenRCT2::GetContext()->GetScriptEngine().GetContext();

        // Create event args object
        auto obj = OpenRCT2::Scripting::DukObject(ctx);
        obj.Set("id", peep->sprite_index.ToUnderlying());

        // Call the subscriptions
        auto e = obj.Take();
        hookEngine.Call(OpenRCT2::Scripting::HOOK_TYPE::GUEST_GENERATION, e, true);
    }
#endif

    return peep;
}

enum
{
    PEEP_FACE_OFFSET_ANGRY = 0,
    PEEP_FACE_OFFSET_VERY_VERY_SICK,
    PEEP_FACE_OFFSET_VERY_SICK,
    PEEP_FACE_OFFSET_SICK,
    PEEP_FACE_OFFSET_VERY_TIRED,
    PEEP_FACE_OFFSET_TIRED,
    PEEP_FACE_OFFSET_VERY_VERY_UNHAPPY,
    PEEP_FACE_OFFSET_VERY_UNHAPPY,
    PEEP_FACE_OFFSET_UNHAPPY,
    PEEP_FACE_OFFSET_NORMAL,
    PEEP_FACE_OFFSET_HAPPY,
    PEEP_FACE_OFFSET_VERY_HAPPY,
    PEEP_FACE_OFFSET_VERY_VERY_HAPPY,
};

static constexpr const int32_t face_sprite_small[] = {
    SPR_PEEP_SMALL_FACE_ANGRY,
    SPR_PEEP_SMALL_FACE_VERY_VERY_SICK,
    SPR_PEEP_SMALL_FACE_VERY_SICK,
    SPR_PEEP_SMALL_FACE_SICK,
    SPR_PEEP_SMALL_FACE_VERY_TIRED,
    SPR_PEEP_SMALL_FACE_TIRED,
    SPR_PEEP_SMALL_FACE_VERY_VERY_UNHAPPY,
    SPR_PEEP_SMALL_FACE_VERY_UNHAPPY,
    SPR_PEEP_SMALL_FACE_UNHAPPY,
    SPR_PEEP_SMALL_FACE_NORMAL,
    SPR_PEEP_SMALL_FACE_HAPPY,
    SPR_PEEP_SMALL_FACE_VERY_HAPPY,
    SPR_PEEP_SMALL_FACE_VERY_VERY_HAPPY,
};

static constexpr const int32_t face_sprite_large[] = {
    SPR_PEEP_LARGE_FACE_ANGRY_0,
    SPR_PEEP_LARGE_FACE_VERY_VERY_SICK_0,
    SPR_PEEP_LARGE_FACE_VERY_SICK_0,
    SPR_PEEP_LARGE_FACE_SICK,
    SPR_PEEP_LARGE_FACE_VERY_TIRED,
    SPR_PEEP_LARGE_FACE_TIRED,
    SPR_PEEP_LARGE_FACE_VERY_VERY_UNHAPPY,
    SPR_PEEP_LARGE_FACE_VERY_UNHAPPY,
    SPR_PEEP_LARGE_FACE_UNHAPPY,
    SPR_PEEP_LARGE_FACE_NORMAL,
    SPR_PEEP_LARGE_FACE_HAPPY,
    SPR_PEEP_LARGE_FACE_VERY_HAPPY,
    SPR_PEEP_LARGE_FACE_VERY_VERY_HAPPY,
};

static int32_t get_face_sprite_offset(Guest* peep)
{
    // ANGRY
    if (peep->Angriness > 0)
        return PEEP_FACE_OFFSET_ANGRY;

    // VERY_VERY_SICK
    if (peep->Nausea > 200)
        return PEEP_FACE_OFFSET_VERY_VERY_SICK;

    // VERY_SICK
    if (peep->Nausea > 170)
        return PEEP_FACE_OFFSET_VERY_SICK;

    // SICK
    if (peep->Nausea > 140)
        return PEEP_FACE_OFFSET_SICK;

    // VERY_TIRED
    if (peep->Energy < 46)
        return PEEP_FACE_OFFSET_VERY_TIRED;

    // TIRED
    if (peep->Energy < 70)
        return PEEP_FACE_OFFSET_TIRED;

    int32_t offset = PEEP_FACE_OFFSET_VERY_VERY_UNHAPPY;
    // There are 7 different happiness based faces
    for (int32_t i = 37; peep->Happiness >= i; i += 37)
    {
        offset++;
    }

    return offset;
}

/**
 * Function split into large and small sprite
 *  rct2: 0x00698721
 */
int32_t get_peep_face_sprite_small(Guest* peep)
{
    return face_sprite_small[get_face_sprite_offset(peep)];
}

/**
 * Function split into large and small sprite
 *  rct2: 0x00698721
 */
int32_t get_peep_face_sprite_large(Guest* peep)
{
    return face_sprite_large[get_face_sprite_offset(peep)];
}

/**
 *
 *  rct2: 0x00693CBB
 */
bool Guest::UpdateQueuePosition(PeepActionType previous_action)
{
    TimeInQueue++;

    auto* guestNext = GetEntity<Guest>(GuestNextInQueue);
    if (guestNext == nullptr)
    {
        return false;
    }

    int16_t x_diff = abs(guestNext->x - x);
    int16_t y_diff = abs(guestNext->y - y);
    int16_t z_diff = abs(guestNext->z - z);

    if (z_diff > 10)
        return false;

    if (x_diff < y_diff)
    {
        int16_t temp_x = x_diff;
        x_diff = y_diff;
        y_diff = temp_x;
    }

    x_diff += y_diff / 2;
    if (x_diff > 7)
    {
        if (x_diff > 13)
        {
            if ((x & 0xFFE0) != (guestNext->x & 0xFFE0) || (y & 0xFFE0) != (guestNext->y & 0xFFE0))
                return false;
        }

        if (sprite_direction != guestNext->sprite_direction)
            return false;

        switch (guestNext->sprite_direction / 8)
        {
            case 0:
                if (x >= guestNext->x)
                    return false;
                break;
            case 1:
                if (y <= guestNext->y)
                    return false;
                break;
            case 2:
                if (x <= guestNext->x)
                    return false;
                break;
            case 3:
                if (y >= guestNext->y)
                    return false;
                break;
        }
    }

    if (!IsActionInterruptable())
        UpdateAction();

    if (!IsActionWalking())
        return true;

    Action = PeepActionType::Idle;
    NextActionSpriteType = PeepActionSpriteType::WatchRide;
    if (previous_action != PeepActionType::Idle)
        Invalidate();
    return true;
}

/**
 *
 *  rct2: 0x006966A9
 */
void Guest::RemoveFromQueue()
{
    auto ride = get_ride(CurrentRide);
    if (ride == nullptr)
        return;

    auto& station = ride->GetStation(CurrentRideStation);
    // Make sure we don't underflow, building while paused might reset it to 0 where peeps have
    // not yet left the queue.
    if (station.QueueLength > 0)
    {
        station.QueueLength--;
    }

    if (sprite_index == station.LastPeepInQueue)
    {
        station.LastPeepInQueue = GuestNextInQueue;
        return;
    }

    auto* otherGuest = GetEntity<Guest>(station.LastPeepInQueue);
    if (otherGuest == nullptr)
    {
        log_error("Invalid Guest Queue list!");
        return;
    }
    for (; otherGuest != nullptr; otherGuest = GetEntity<Guest>(otherGuest->GuestNextInQueue))
    {
        if (sprite_index == otherGuest->GuestNextInQueue)
        {
            otherGuest->GuestNextInQueue = GuestNextInQueue;
            return;
        }
    }
}

uint64_t Guest::GetItemFlags() const
{
    return ItemFlags;
}

void Guest::SetItemFlags(uint64_t itemFlags)
{
    ItemFlags = itemFlags;
}

void Guest::RemoveAllItems()
{
    ItemFlags = 0;
}

void Guest::RemoveItem(ShopItem item)
{
    ItemFlags &= ~EnumToFlag(item);
}

void Guest::GiveItem(ShopItem item)
{
    ItemFlags |= EnumToFlag(item);
}

bool Guest::HasItem(ShopItem peepItem) const
{
    return GetItemFlags() & EnumToFlag(peepItem);
}

static bool IsThoughtShopItemRelated(const PeepThoughtType type)
{
    switch (type)
    {
        case PeepThoughtType::AlreadyGot:
        case PeepThoughtType::HaventFinished:
        case PeepThoughtType::CantAffordItem:
            return true;
        default:
            break;
    }
    return false;
}

void Guest::RemoveRideFromMemory(RideId rideId)
{
    if (State == PeepState::Watching)
    {
        if (CurrentRide == rideId)
        {
            CurrentRide = RideId::GetNull();
            if (TimeToStand >= 50)
            {
                // make peep stop watching the ride
                TimeToStand = 50;
            }
        }
    }

    // remove any free voucher for this ride from peep
    if (HasItem(ShopItem::Voucher))
    {
        if (VoucherType == VOUCHER_TYPE_RIDE_FREE && VoucherRideId == rideId)
        {
            RemoveItem(ShopItem::Voucher);
        }
    }

    // remove any photos of this ride from peep
    if (HasItem(ShopItem::Photo))
    {
        if (Photo1RideRef == rideId)
        {
            RemoveItem(ShopItem::Photo);
        }
    }
    if (HasItem(ShopItem::Photo2))
    {
        if (Photo2RideRef == rideId)
        {
            RemoveItem(ShopItem::Photo2);
        }
    }
    if (HasItem(ShopItem::Photo3))
    {
        if (Photo3RideRef == rideId)
        {
            RemoveItem(ShopItem::Photo3);
        }
    }
    if (HasItem(ShopItem::Photo4))
    {
        if (Photo4RideRef == rideId)
        {
            RemoveItem(ShopItem::Photo4);
        }
    }

    if (GuestHeadingToRideId == rideId)
    {
        GuestHeadingToRideId = RideId::GetNull();
    }
    if (FavouriteRide == rideId)
    {
        FavouriteRide = RideId::GetNull();
    }

    // Erase all thoughts that contain the ride.
    for (auto it = std::begin(Thoughts); it != std::end(Thoughts);)
    {
        const auto& entry = *it;
        if (entry.type == PeepThoughtType::None)
            break;

        // Ride ids and shop item ids might have the same value, look only for ride thoughts.
        if (IsThoughtShopItemRelated(entry.type) || entry.rideId != rideId)
        {
            it++;
            continue;
        }

        if (auto itNext = std::next(it); itNext != std::end(Thoughts))
        {
            // Overwrite this entry by shifting all entries that follow.
            std::rotate(it, itNext, std::end(Thoughts));
        }

        // Last slot is now free.
        auto& lastEntry = Thoughts.back();
        lastEntry.type = PeepThoughtType::None;
        lastEntry.item = PeepThoughtItemNone;
    }
}

void Guest::Serialise(DataSerialiser& stream)
{
    Peep::Serialise(stream);
    stream << GuestNumRides;
    stream << GuestNextInQueue;
    stream << ParkEntryTime;
    stream << GuestHeadingToRideId;
    stream << GuestIsLostCountdown;
    stream << GuestTimeOnRide;
    stream << PaidToEnter;
    stream << PaidOnRides;
    stream << PaidOnFood;
    stream << PaidOnDrink;
    stream << PaidOnSouvenirs;
    stream << OutsideOfPark;
    stream << Happiness;
    stream << HappinessTarget;
    stream << Nausea;
    stream << NauseaTarget;
    stream << Hunger;
    stream << Thirst;
    stream << Toilet;
    stream << TimeToConsume;
    stream << Intensity;
    stream << NauseaTolerance;
    stream << TimeInQueue;
    stream << CashInPocket;
    stream << CashSpent;
    stream << Photo1RideRef;
    stream << Photo2RideRef;
    stream << Photo3RideRef;
    stream << Photo4RideRef;
    stream << RejoinQueueTimeout;
    stream << PreviousRide;
    stream << PreviousRideTimeOut;
    stream << Thoughts;
    stream << LitterCount;
    stream << DisgustingCount;
    stream << AmountOfFood;
    stream << AmountOfDrinks;
    stream << AmountOfSouvenirs;
    stream << VandalismSeen;
    stream << VoucherType;
    stream << VoucherRideId;
    stream << SurroundingsThoughtTimeout;
    stream << Angriness;
    stream << TimeLost;
    stream << DaysInQueue;
    stream << BalloonColour;
    stream << UmbrellaColour;
    stream << HatColour;
    stream << FavouriteRide;
    stream << FavouriteRideRating;
    stream << ItemFlags;
}
