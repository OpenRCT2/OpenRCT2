/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Guest.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../Game.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../SpriteIds.h"
#include "../audio/Audio.h"
#include "../config/Config.h"
#include "../core/DataSerialiser.h"
#include "../core/Guard.hpp"
#include "../core/Numerics.hpp"
#include "../core/String.hpp"
#include "../entity/Balloon.h"
#include "../entity/EntityList.h"
#include "../entity/EntityRegistry.h"
#include "../entity/MoneyEffect.h"
#include "../entity/Particle.h"
#include "../interface/WindowBase.h"
#include "../localisation/Formatter.h"
#include "../localisation/Formatting.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../network/Network.h"
#include "../object/ClimateObject.h"
#include "../object/LargeSceneryEntry.h"
#include "../object/MusicObject.h"
#include "../object/ObjectManager.h"
#include "../object/PathAdditionEntry.h"
#include "../object/PeepAnimationsObject.h"
#include "../object/WallSceneryEntry.h"
#include "../peep/GuestPathfinding.h"
#include "../peep/PeepAnimations.h"
#include "../peep/PeepThoughts.h"
#include "../peep/RideUseSystem.h"
#include "../rct2/RCT2.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/RideManager.hpp"
#include "../ride/ShopItem.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../ride/Vehicle.h"
#include "../scenario/Scenario.h"
#include "../scripting/HookEngine.h"
#include "../scripting/ScriptEngine.h"
#include "../ui/WindowManager.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Climate.h"
#include "../world/Footpath.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/TileElementsView.h"
#include "../world/tile_element/EntranceElement.h"
#include "../world/tile_element/LargeSceneryElement.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TrackElement.h"
#include "../world/tile_element/WallElement.h"
#include "Peep.h"
#include "Staff.h"

#include <cassert>
#include <functional>
#include <iterator>
#include <sfl/static_vector.hpp>
#include <span>

using namespace OpenRCT2;

static const uint8_t kTicksToGoUpSpiralSlide = 30;

// Locations of the spiral slide platform that a peep walks from the entrance of the ride to the
// entrance of the slide. Up to 4 waypoints for each 4 sides that an ride entrance can be located
// and 4 different rotations of the ride. 4 * 4 * 4 = 64 locations.
// clang-format off
static constexpr CoordsXY kSpiralSlideWalkingPath[64] = {
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
static constexpr CoordsXY kWatchingPositionOffsets[] = {
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

static constexpr RideRating_t NauseaMaximumThresholds[] = {
    300,
    600,
    800,
    1000,
};

/** rct2: 009823AC */
static constexpr PeepThoughtType kCrowdedThoughts[] = {
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
    { PeepActionType::shakeHead, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::emptyPockets, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::wow, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_SHOP_ITEM_SINGULAR },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_SHOP_ITEM_INDEFINITE },
    { PeepActionType::shakeHead, PEEP_THOUGHT_ACTION_FLAG_SHOP_ITEM_INDEFINITE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::wave, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::joy, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::checkTime, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::wave, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::wave, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::disgust, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::beingWatched, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::shakeHead, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::joy, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::walking, PEEP_THOUGHT_ACTION_FLAG_RIDE },
};

// These arrays contain the base minimum and maximum nausea ratings for peeps, based on their nausea tolerance level.
static constexpr RideRating_t kNauseaMinimumThresholds[] = {
    0,
    0,
    200,
    400,
};

static bool GuestHasVoucherForFreeRide(Guest& guest, const Ride& ride);
static void GuestRideIsTooIntense(Guest& guest, Ride& ride, bool peepAtRide);
static void GuestResetRideHeading(Guest& guest);
static void GuestTriedToEnterFullQueue(Guest& guest, Ride& ride);
static int16_t GuestCalculateRideSatisfaction(Guest& guest, const Ride& ride);
static void GuestUpdateFavouriteRide(Guest& guest, const Ride& ride, uint8_t satisfaction);
static int16_t GuestCalculateRideValueSatisfaction(Guest& guest, const Ride& ride);
static int16_t GuestCalculateRideIntensityNauseaSatisfaction(Guest& guest, const Ride& ride);
static void GuestUpdateRideNauseaGrowth(Guest& guest, const Ride& ride);
static bool GuestShouldGoOnRideAgain(Guest& guest, const Ride& ride);
static bool GuestShouldPreferredIntensityIncrease(Guest& guest);
static bool GuestReallyLikedRide(Guest& guest, const Ride& ride);
static PeepThoughtType GuestAssessSurroundings(int16_t centre_x, int16_t centre_y, int16_t centre_z);
static void GuestUpdateHunger(Guest& guest);
static void GuestDecideWhetherToLeavePark(Guest& guest);
static void GuestLeavePark(Guest& guest);
static void GuestHeadForNearestRideWithFlag(Guest& guest, bool considerOnlyCloseRides, RtdFlag rtdFlag);
static void GuestHeadForNearestRideWithSpecialType(Guest& guest, bool considerOnlyCloseRides, RtdSpecialType specialType);
static bool Loc690FD0(Guest& guest, RideId* rideToView, uint8_t* rideSeatToView, TileElement* tileElement);
static void GuestUpdateWalkingBreakScenery(Guest& guest);
static bool GuestFindRideToLookAt(Guest& guest, uint8_t edge, RideId* rideToView, uint8_t* rideSeatToView);
static bool GuestShouldGoToShop(Guest& guest, Ride& ride, bool peepAtShop);
static bool GuestShouldRideWhileRaining(Guest& guest, const Ride& ride);
static void GuestPickRideToGoOn(Guest& guest);
static Ride* GuestFindBestRideToGoOn(Guest& guest);

template<>
bool EntityBase::Is<Guest>() const
{
    return Type == EntityType::guest;
}

static bool IsValidLocation(const CoordsXYZ& coords)
{
    if (coords.x != kLocationNull)
    {
        if (MapIsLocationValid(coords))
        {
            return true;
        }
    }

    return false;
}

template<void (Guest::*EasterEggFunc)(Guest&), bool applyToSelf>
static void ApplyEasterEggToNearbyGuests(Guest& guest)
{
    const auto guestLoc = guest.GetLocation();
    if (!IsValidLocation(guestLoc))
        return;

    for (auto* otherGuest : EntityTileList<Guest>(guestLoc))
    {
        if constexpr (!applyToSelf)
        {
            if (otherGuest == &guest)
            {
                // Can not apply effect on self.
                continue;
            }
        }
        auto zDiff = std::abs(otherGuest->z - guestLoc.z);
        if (zDiff <= 32)
        {
            std::invoke(EasterEggFunc, guest, *otherGuest);
        }
    }
}

void Guest::GivePassingGuestPurpleClothes(Guest& passingPeep)
{
    passingPeep.TshirtColour = Drawing::Colour::brightPurple;
    passingPeep.TrousersColour = Drawing::Colour::brightPurple;
    passingPeep.Invalidate();
}

void Guest::GivePassingGuestPizza(Guest& passingPeep)
{
    if (passingPeep.HasItem(ShopItem::pizza))
        return;

    passingPeep.GiveItem(ShopItem::pizza);

    int32_t peepDirection = (Orientation >> 3) ^ 2;
    int32_t otherPeepOppositeDirection = passingPeep.Orientation >> 3;
    if (peepDirection == otherPeepOppositeDirection)
    {
        if (passingPeep.IsActionInterruptableSafely())
        {
            passingPeep.Action = PeepActionType::wave2;
            passingPeep.AnimationFrameNum = 0;
            passingPeep.AnimationImageIdOffset = 0;
            passingPeep.UpdateCurrentAnimationType();
        }
    }
}

void Guest::MakePassingGuestSick(Guest& passingPeep)
{
    if (passingPeep.State != PeepState::walking)
        return;

    if (passingPeep.IsActionInterruptableSafely())
    {
        passingPeep.Action = PeepActionType::throwUp;
        passingPeep.AnimationFrameNum = 0;
        passingPeep.AnimationImageIdOffset = 0;
        passingPeep.UpdateCurrentAnimationType();
    }
}

void Guest::GivePassingPeepsIceCream(Guest& passingPeep)
{
    if (passingPeep.HasItem(ShopItem::iceCream))
        return;

    passingPeep.GiveItem(ShopItem::iceCream);
    passingPeep.UpdateAnimationGroup();
}

/**
 *
 *  rct2: 0x0068FD3A
 */
void Guest::UpdateEasterEggInteractions()
{
    if (PeepFlags & PEEP_FLAGS_PURPLE)
    {
        ApplyEasterEggToNearbyGuests<&Guest::GivePassingGuestPurpleClothes, true>(*this);
    }

    if (PeepFlags & PEEP_FLAGS_PIZZA)
    {
        ApplyEasterEggToNearbyGuests<&Guest::GivePassingGuestPizza, true>(*this);
    }

    if (PeepFlags & PEEP_FLAGS_CONTAGIOUS)
    {
        ApplyEasterEggToNearbyGuests<&Guest::MakePassingGuestSick, false>(*this);
    }

    if (PeepFlags & PEEP_FLAGS_ICE_CREAM)
    {
        ApplyEasterEggToNearbyGuests<&Guest::GivePassingPeepsIceCream, false>(*this);
    }

    if (PeepFlags & PEEP_FLAGS_JOY)
    {
        if ((ScenarioRand() & 0xFFFF) <= 1456)
        {
            if (IsActionInterruptableSafely())
            {
                Action = PeepActionType::joy;
                AnimationFrameNum = 0;
                AnimationImageIdOffset = 0;
                UpdateCurrentAnimationType();
            }
        }
    }
}

int32_t Guest::GetEasterEggNameId() const
{
    char buffer[256]{};

    Formatter ft;
    FormatNameTo(ft);
    FormatStringLegacy(buffer, sizeof(buffer), STR_STRINGID, ft.Data());

    for (uint32_t i = 0; i < std::size(gPeepEasterEggNames); i++)
    {
        if (String::iequals(buffer, gPeepEasterEggNames[i]))
            return static_cast<int32_t>(i);
    }

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
    FormatStringLegacy(buffer, sizeof(buffer), STR_STRINGID, ft.Data());

    return String::iequals(buffer, gPeepEasterEggNames[index]);
}

void Guest::UpdateMotivesIdle()
{
    // Idle peep happiness tends towards 127 (50%).
    if (HappinessTarget >= 128)
        HappinessTarget--;
    else
        HappinessTarget++;

    NauseaTarget = std::max(NauseaTarget - 2, 0);

    if (Energy <= 50)
    {
        HappinessTarget = std::max(HappinessTarget - 2, 0);
    }

    if (Hunger < 10)
    {
        HappinessTarget = std::max(HappinessTarget - 1, 0);
    }

    if (Thirst < 10)
    {
        HappinessTarget = std::max(HappinessTarget - 1, 0);
    }

    if (Toilet >= 195)
    {
        HappinessTarget = std::max(HappinessTarget - 1, 0);
    }

    if (State == PeepState::walking && NauseaTarget >= 128)
    {
        if ((ScenarioRand() & 0xFF) <= static_cast<uint8_t>((Nausea - 128) / 2))
        {
            if (IsActionInterruptableSafely())
            {
                Action = PeepActionType::throwUp;
                AnimationFrameNum = 0;
                AnimationImageIdOffset = 0;
                UpdateCurrentAnimationType();
            }
        }
    }
}

/* rct2: 0x0068FA89*/
void Guest::UpdateConsumptionMotives()
{
    if (TimeToConsume == 0 && HasFoodOrDrink())
    {
        TimeToConsume += 3;
    }

    if (TimeToConsume != 0 && State != PeepState::onRide)
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
            int32_t chosen_food = Numerics::bitScanForward(GetFoodOrDrinkFlags());
            if (chosen_food != -1)
            {
                ShopItem food = ShopItem(chosen_food);
                RemoveItem(food);

                auto discardContainer = GetShopItemDescriptor(food).DiscardContainer;
                if (discardContainer != ShopItem::none)
                {
                    GiveItem(discardContainer);
                }

                WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                UpdateAnimationGroup();
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
        newEnergy = std::min<uint16_t>(kPeepMaxEnergyTarget, newEnergy + 4);
        if (newEnergy > newTargetEnergy)
            newEnergy = newTargetEnergy;
    }

    /* Previous code here suggested maximum energy is 128. */
    newEnergy = std::clamp(newEnergy, kPeepMinEnergy, kPeepMaxEnergy);

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

void Guest::Tick128UpdateGuest(uint32_t index)
{
    const auto currentTicks = getGameState().currentTicks;
    const bool ticksMatchIndex = (index & 0x1FF) != (currentTicks & 0x1FF);
    if (ticksMatchIndex && !(PeepFlags & PEEP_FLAGS_POSITION_FROZEN))
    {
        UpdateConsumptionMotives();
        return;
    }

    /* Effect of masking with 0x1FF here vs mask 0x7F,
     * which is the condition for calling this function, is
     * to reduce how often the content in this conditional
     * is executed to once every four calls. */
    if (PeepFlags & PEEP_FLAGS_CROWDED)
    {
        PeepThoughtType thought_type = kCrowdedThoughts[ScenarioRand() & 0xF];
        if (thought_type != PeepThoughtType::None)
        {
            InsertNewThought(thought_type);
        }
    }

    if (PeepFlags & PEEP_FLAGS_EXPLODE && x != kLocationNull)
    {
        if (State == PeepState::walking || State == PeepState::sitting)
        {
            Audio::Play3D(Audio::SoundId::crash, GetLocation());

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

    if (State == PeepState::walking || State == PeepState::sitting)
    {
        SurroundingsThoughtTimeout++;
        if (SurroundingsThoughtTimeout >= 18)
        {
            SurroundingsThoughtTimeout = 0;
            if (x != kLocationNull)
            {
                PeepThoughtType thought_type = GuestAssessSurroundings(x & 0xFFE0, y & 0xFFE0, z);

                if (thought_type != PeepThoughtType::None)
                {
                    InsertNewThought(thought_type);
                    HappinessTarget = std::min(kPeepMaxHappiness, HappinessTarget + 45);
                }
            }
        }
    }

    if (!(PeepFlags & PEEP_FLAGS_ANIMATION_FROZEN))
    {
        UpdateAnimationGroup();
    }

    if (State == PeepState::onRide || State == PeepState::enteringRide)
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
                auto ride = GetRide(CurrentRide);
                if (ride != nullptr)
                {
                    PeepThoughtType thought_type = ride->getRideTypeDescriptor().HasFlag(RtdFlag::describeAsInside)
                        ? PeepThoughtType::GetOut
                        : PeepThoughtType::GetOff;

                    InsertNewThought(thought_type, CurrentRide);
                }
            }
        }
    }

    if (PeepFlags & PEEP_FLAGS_POSITION_FROZEN)
    {
        return;
    }

    if (State == PeepState::walking && !OutsideOfPark && !(PeepFlags & PEEP_FLAGS_LEAVING_PARK) && GuestNumRides == 0
        && GuestHeadingToRideId.IsNull())
    {
        uint32_t time_duration = currentTicks - ParkEntryTime;
        time_duration /= 2048;

        if (time_duration >= 5)
        {
            GuestPickRideToGoOn(*this);

            if (GuestHeadingToRideId.IsNull())
            {
                HappinessTarget = std::max(HappinessTarget - 128, 0);
                GuestLeavePark(*this);
                GuestUpdateHunger(*this);
                UpdateMotivesIdle();
                UpdateConsumptionMotives();
                return;
            }
        }
    }

    if ((ScenarioRand() & 0xFFFF) <= ((HasItem(ShopItem::map)) ? 8192u : 2184u))
    {
        GuestPickRideToGoOn(*this);
    }

    if ((index & 0x3FF) == (currentTicks & 0x3FF))
    {
        /* Effect of masking with 0x3FF here vs mask 0x1FF,
         * which is used in the encompassing conditional, is
         * to reduce how often the content in this conditional
         * is executed to once every second time the encompassing
         * conditional executes. */

        if (!OutsideOfPark && (State == PeepState::walking || State == PeepState::sitting))
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

                if (!(getGameState().park.flags & PARK_FLAGS_NO_MONEY) && CashInPocket <= 9.00_GBP && Happiness >= 105
                    && Energy >= 70)
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
                PeepThoughtType chosen_thought = possible_thoughts[ScenarioRand() % num_thoughts];

                InsertNewThought(chosen_thought);

                switch (chosen_thought)
                {
                    case PeepThoughtType::Hungry:
                        GuestHeadForNearestRideWithFlag(*this, false, RtdFlag::sellsFood);
                        break;
                    case PeepThoughtType::Thirsty:
                        GuestHeadForNearestRideWithFlag(*this, false, RtdFlag::sellsDrinks);
                        break;
                    case PeepThoughtType::Toilet:
                        GuestHeadForNearestRideWithSpecialType(*this, false, RtdSpecialType::toilet);
                        break;
                    case PeepThoughtType::RunningOut:
                        GuestHeadForNearestRideWithSpecialType(*this, false, RtdSpecialType::cashMachine);
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
                GuestHeadForNearestRideWithSpecialType(*this, true, RtdSpecialType::firstAid);
            }
            InsertNewThought(thought_type);
        }
    }

    switch (State)
    {
        case PeepState::walking:
        case PeepState::leavingPark:
        case PeepState::enteringPark:
            GuestDecideWhetherToLeavePark(*this);
            GuestUpdateHunger(*this);
            break;

        case PeepState::sitting:
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
            GuestUpdateHunger(*this);
            break;

        case PeepState::queuing:
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
                        if (pathAddEntry != nullptr && (pathAddEntry->flags & PATH_ADDITION_FLAG_IS_QUEUE_SCREEN))
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

                    // This is +2 as UpdateMotivesIdle (that is called later in the function)
                    // will -1. We want to gradually increase happiness to 165
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
            GuestUpdateHunger(*this);
            break;
        case PeepState::enteringRide:
            if (SubState == 17 || SubState == 15)
            {
                GuestDecideWhetherToLeavePark(*this);
            }
            GuestUpdateHunger(*this);
            break;
        default:
            break;
    }

    UpdateMotivesIdle();
    UpdateConsumptionMotives();
}

/**
 *
 *  rct2: 0x00691677
 */
static void GuestTryGetUpFromSitting(Guest& guest)
{
    // Eats all food first
    if (guest.HasFoodOrDrink())
        return;

    guest.TimeToSitdown--;
    if (guest.TimeToSitdown)
        return;

    guest.SetState(PeepState::walking);

    // Set destination to the centre of the tile.
    const auto destination = guest.GetLocation().ToTileCentre();
    guest.SetDestination(destination, 5);
    guest.UpdateCurrentAnimationType();
}

/**
 *
 *  rct2: 0x0069152B
 */
void Guest::UpdateSitting()
{
    if (SittingSubState == PeepSittingSubState::tryingToSit)
    {
        if (!CheckForPath())
            return;
        // 691541

        const auto [pathingResult, _] = PerformNextAction();
        if (!(pathingResult & PATHING_DESTINATION_REACHED))
            return;

        auto loc = GetLocation().ToTileStart() + CoordsXYZ{ BenchUseOffsets[Var37 & 0x7], 0 };

        MoveTo(loc);

        Orientation = ((Var37 + 2) & 3) * 8;
        Action = PeepActionType::idle;
        NextAnimationType = PeepAnimationType::sittingIdle;
        SwitchNextAnimationType();

        SittingSubState = PeepSittingSubState::satDown;

        // Sets time to sit on seat
        TimeToSitdown = (129 - Energy) * 16 + 50;
    }
    else if (SittingSubState == PeepSittingSubState::satDown)
    {
        if (!IsActionInterruptable())
        {
            UpdateAction();
            if (!IsActionWalking())
                return;

            Action = PeepActionType::idle;
            GuestTryGetUpFromSitting(*this);
            return;
        }

        if ((PeepFlags & PEEP_FLAGS_LEAVING_PARK))
        {
            SetState(PeepState::walking);

            // Set destination to the centre of the tile
            auto destination = GetLocation().ToTileCentre();
            SetDestination(destination, 5);
            UpdateCurrentAnimationType();
            return;
        }

        if (AnimationGroup == PeepAnimationGroup::umbrella)
        {
            GuestTryGetUpFromSitting(*this);
            return;
        }

        if (HasFoodOrDrink())
        {
            if ((ScenarioRand() & 0xFFFFu) > 1310u)
            {
                GuestTryGetUpFromSitting(*this);
                return;
            }
            Action = PeepActionType::sittingEatFood;
            AnimationFrameNum = 0;
            AnimationImageIdOffset = 0;
            UpdateCurrentAnimationType();
            return;
        }

        const auto rand = ScenarioRand();
        if ((rand & 0xFFFFu) > 131u)
        {
            GuestTryGetUpFromSitting(*this);
            return;
        }
        if (AnimationGroup == PeepAnimationGroup::balloon || AnimationGroup == PeepAnimationGroup::hat)
        {
            GuestTryGetUpFromSitting(*this);
            return;
        }

        Action = PeepActionType::sittingLookAroundLeft;
        if (rand & 0x80000000u)
        {
            Action = PeepActionType::sittingLookAroundRight;
        }

        if (rand & 0x40000000u)
        {
            Action = PeepActionType::sittingCheckWatch;
        }
        AnimationFrameNum = 0;
        AnimationImageIdOffset = 0;
        UpdateCurrentAnimationType();
    }
}

/**
 * To simplify check of 0x36BA3E0 and 0x11FF78
 * returns false on no food.
 */
uint64_t Guest::GetFoodOrDrinkFlags() const
{
    return GetItemFlags() & (ShopItemsGetAllFoods() | ShopItemsGetAllDrinks());
}

uint64_t Guest::GetEmptyContainerFlags() const
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
        if (RideGetCount() < 2)
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

    auto* windowMgr = Ui::GetWindowManager();
    WindowBase* w = windowMgr->FindByNumber(WindowClass::peep, Id);

    if (w != nullptr)
    {
        w->onPrepareDraw();
    }

    windowMgr->InvalidateByNumber(WindowClass::peep, Id);
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

static money64 getItemValue(const ShopItemDescriptor& shopItemDescriptor)
{
    auto& objManager = GetContext()->GetObjectManager();
    auto* climateObj = objManager.GetLoadedObject<ClimateObject>(0);
    if (climateObj == nullptr)
        return shopItemDescriptor.BaseValue;

    const auto& thresholds = climateObj->getItemThresholds();
    const auto& gameState = getGameState();

    if (gameState.weatherCurrent.temperature >= thresholds.warm)
        return shopItemDescriptor.HotValue;
    else if (gameState.weatherCurrent.temperature <= thresholds.cold)
        return shopItemDescriptor.ColdValue;
    else
        return shopItemDescriptor.BaseValue;
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
static bool GuestDecideAndBuyItem(Guest& guest, Ride& ride, const ShopItem shopItem, money64 price)
{
    const bool isPrecipitating = ClimateIsPrecipitating();
    const bool isPrecipitatingAndUmbrella = isPrecipitating && (shopItem == ShopItem::umbrella);

    bool hasVoucher = false;
    if ((guest.HasItem(ShopItem::voucher)) && (guest.VoucherType == VOUCHER_TYPE_FOOD_OR_DRINK_FREE)
        && (guest.VoucherShopItem == shopItem))
    {
        hasVoucher = true;
    }

    if (guest.HasItem(shopItem))
    {
        guest.InsertNewThought(PeepThoughtType::AlreadyGot, shopItem);
        return false;
    }

    const auto& shopItemDescriptor = GetShopItemDescriptor(shopItem);
    if (shopItemDescriptor.IsFoodOrDrink())
    {
        int32_t food = Numerics::bitScanForward(guest.GetFoodOrDrinkFlags());
        if (food != -1)
        {
            guest.InsertNewThought(PeepThoughtType::HaventFinished, static_cast<ShopItem>(food));
            return false;
        }

        if (guest.Nausea >= 145)
            return false;
    }

    if ((shopItem == ShopItem::balloon || shopItem == ShopItem::iceCream || shopItem == ShopItem::candyfloss
         || shopItem == ShopItem::sunglasses)
        && isPrecipitating)
    {
        return false;
    }

    auto& gameState = getGameState();
    if ((shopItem == ShopItem::sunglasses || shopItem == ShopItem::iceCream) && gameState.weatherCurrent.temperature < 12)
    {
        return false;
    }

    if (shopItemDescriptor.IsFood() && (guest.Hunger > 75))
    {
        guest.InsertNewThought(PeepThoughtType::NotHungry);
        return false;
    }

    if (shopItemDescriptor.IsDrink() && (guest.Thirst > 75))
    {
        guest.InsertNewThought(PeepThoughtType::NotThirsty);
        return false;
    }

    if (!isPrecipitatingAndUmbrella && (shopItem != ShopItem::map) && shopItemDescriptor.IsSouvenir() && !hasVoucher)
    {
        if (((ScenarioRand() & 0x7F) + 0x73) > guest.Happiness || guest.GuestNumRides < 3)
            return false;
    }

    if (!hasVoucher)
    {
        if (price != 0 && !(gameState.park.flags & PARK_FLAGS_NO_MONEY))
        {
            if (guest.CashInPocket == 0)
            {
                guest.InsertNewThought(PeepThoughtType::SpentMoney);
                return false;
            }
            if (price > guest.CashInPocket)
            {
                guest.InsertNewThought(PeepThoughtType::CantAffordItem, shopItem);
                return false;
            }
        }

        money64 itemValue = getItemValue(shopItemDescriptor);
        if (itemValue < price)
        {
            itemValue -= price;

            if (!isPrecipitatingAndUmbrella)
            {
                itemValue = -itemValue;
                if (guest.Happiness >= 128)
                {
                    itemValue /= 2;
                    if (guest.Happiness >= 180)
                        itemValue /= 2;
                }
                if (itemValue > (static_cast<money64>(ScenarioRand() & 0x07)) && !(gameState.cheats.ignorePrice))
                {
                    // "I'm not paying that much for x"
                    guest.InsertNewThought(shopItemDescriptor.TooMuchThought, ride.id);
                    return false;
                }
            }
        }
        else
        {
            itemValue -= price;
            itemValue = std::max(0.80_GBP, itemValue);

            if (!(gameState.park.flags & PARK_FLAGS_NO_MONEY))
            {
                if (itemValue >= static_cast<money64>(ScenarioRand() & 0x07))
                {
                    // "This x is a really good value"
                    guest.InsertNewThought(shopItemDescriptor.GoodValueThought, ride.id);
                }
            }

            int32_t happinessGrowth = itemValue * 4;
            guest.HappinessTarget = std::min((guest.HappinessTarget + happinessGrowth), kPeepMaxHappiness);
            guest.Happiness = std::min((guest.Happiness + happinessGrowth), kPeepMaxHappiness);
        }

        // reset itemValue for satisfaction calculation
        itemValue = getItemValue(shopItemDescriptor);
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
        ride.updateSatisfaction(satisfaction);
    }

    // The peep has now decided to buy the item (or, specifically, has not been
    // dissuaded so far).
    guest.GiveItem(shopItem);
    const auto hasRandomShopColour = ride.hasLifecycleFlag(RIDE_LIFECYCLE_RANDOM_SHOP_COLOURS);

    switch (shopItem)
    {
        case ShopItem::tShirt:
            guest.TshirtColour = hasRandomShopColour ? static_cast<Drawing::Colour>(ScenarioRandMax(Drawing::kColourNumNormal))
                                                     : ride.trackColours[0].main;
            break;
        case ShopItem::hat:
            guest.HatColour = hasRandomShopColour ? static_cast<Drawing::Colour>(ScenarioRandMax(Drawing::kColourNumNormal))
                                                  : ride.trackColours[0].main;
            break;
        case ShopItem::balloon:
            guest.BalloonColour = hasRandomShopColour ? static_cast<Drawing::Colour>(ScenarioRandMax(Drawing::kColourNumNormal))
                                                      : ride.trackColours[0].main;
            break;
        case ShopItem::umbrella:
            guest.UmbrellaColour = hasRandomShopColour
                ? static_cast<Drawing::Colour>(ScenarioRandMax(Drawing::kColourNumNormal))
                : ride.trackColours[0].main;
            break;
        case ShopItem::map:
            guest.ResetPathfindGoal();
            break;
        case ShopItem::photo:
            guest.Photo1RideRef = ride.id;
            break;
        case ShopItem::photo2:
            guest.Photo2RideRef = ride.id;
            break;
        case ShopItem::photo3:
            guest.Photo3RideRef = ride.id;
            break;
        case ShopItem::photo4:
            guest.Photo4RideRef = ride.id;
            break;
        default:
            break;
    }

    guest.WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
    guest.UpdateAnimationGroup();
    if (guest.PeepFlags & PEEP_FLAGS_TRACKING)
    {
        auto ft = Formatter();
        guest.FormatNameTo(ft);
        ft.Add<StringId>(shopItemDescriptor.Naming.Indefinite);
        if (Config::Get().notifications.guestBoughtItem)
        {
            News::AddItemToQueue(News::ItemType::peepOnRide, STR_PEEP_TRACKING_NOTIFICATION_BOUGHT_X, guest.Id, ft);
        }
    }

    money64* expend_type = &guest.PaidOnSouvenirs;
    ExpenditureType expenditure = ExpenditureType::shopStock;

    if (shopItemDescriptor.IsFood())
    {
        guest.AmountOfFood++;
        expend_type = &guest.PaidOnFood;
        expenditure = ExpenditureType::foodDrinkStock;
    }
    else if (shopItemDescriptor.IsDrink())
    {
        guest.AmountOfDrinks++;
        expend_type = &guest.PaidOnDrink;
        expenditure = ExpenditureType::foodDrinkStock;
    }
    else if (shopItemDescriptor.IsSouvenir())
    {
        guest.AmountOfSouvenirs++;
    }

    if (!(gameState.park.flags & PARK_FLAGS_NO_MONEY))
        FinancePayment(shopItemDescriptor.Cost, expenditure);

    // Sets the expenditure type to *_FOODDRINK_SALES or *_SHOP_SALES appropriately.
    expenditure = static_cast<ExpenditureType>(static_cast<int32_t>(expenditure) - 1);
    if (hasVoucher)
    {
        guest.RemoveItem(ShopItem::voucher);
        guest.WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
    }
    else if (!(gameState.park.flags & PARK_FLAGS_NO_MONEY))
    {
        guest.SpendMoney(*expend_type, price, expenditure);
    }
    ride.totalProfit = AddClamp(ride.totalProfit, price - shopItemDescriptor.Cost);
    ride.windowInvalidateFlags.set(RideInvalidateFlag::income);
    ride.curNumCustomers++;
    ride.totalCustomers = AddClamp(ride.totalCustomers, 1u);
    ride.windowInvalidateFlags.set(RideInvalidateFlag::customers);

    return true;
}

/**
 * Updates various peep stats upon entering a ride, as well as updating the
 * ride's satisfaction value.
 *  rct2: 0x0069545B
 */
void Guest::OnEnterRide(Ride& ride)
{
    // Calculate how satisfying the ride is for the peep. Can range from -140 to +105.
    int16_t satisfaction = GuestCalculateRideSatisfaction(*this, ride);

    // Update the satisfaction stat of the ride.
    uint8_t rideSatisfaction = 0;
    if (satisfaction >= 40)
        rideSatisfaction = 3;
    else if (satisfaction >= 20)
        rideSatisfaction = 2;
    else if (satisfaction >= 0)
        rideSatisfaction = 1;

    ride.updateSatisfaction(rideSatisfaction);

    // Update various peep stats.
    if (GuestNumRides < 255)
        GuestNumRides++;

    SetHasRidden(ride);
    GuestUpdateFavouriteRide(*this, ride, satisfaction);
    HappinessTarget = std::clamp(HappinessTarget + satisfaction, 0, kPeepMaxHappiness);
    GuestUpdateRideNauseaGrowth(*this, ride);
}

/**
 *
 *  rct2: 0x0069576E
 */
void Guest::OnExitRide(Ride& ride)
{
    if (PeepFlags & PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE)
    {
        PeepFlags &= ~PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE;
        FavouriteRide = ride.id;
        // TODO fix this flag name or add another one
        WindowInvalidateFlags |= PEEP_INVALIDATE_STAFF_STATS;
    }
    Happiness = HappinessTarget;
    Nausea = NauseaTarget;
    WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_STATS;

    if (PeepFlags & PEEP_FLAGS_LEAVING_PARK)
        PeepFlags &= ~(PEEP_FLAGS_PARK_ENTRANCE_CHOSEN);

    if (GuestShouldGoOnRideAgain(*this, ride))
    {
        GuestHeadingToRideId = ride.id;
        GuestIsLostCountdown = 200;
        ResetPathfindGoal();
        WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_ACTION;
    }

    if (GuestShouldPreferredIntensityIncrease(*this))
    {
        if (Intensity.GetMaximum() < 15)
        {
            Intensity = Intensity.WithMaximum(Intensity.GetMaximum() + 1);
        }
    }

    if (GuestReallyLikedRide(*this, ride))
    {
        InsertNewThought(PeepThoughtType::WasGreat, ride.id);

        static constexpr Audio::SoundId laughs[3] = {
            Audio::SoundId::laugh1,
            Audio::SoundId::laugh2,
            Audio::SoundId::laugh3,
        };
        int32_t laughType = ScenarioRand() & 7;
        if (laughType < 3)
        {
            Audio::Play3D(laughs[laughType], GetLocation());
        }
    }

    ride.totalCustomers = AddClamp(ride.totalCustomers, 1u);
    ride.windowInvalidateFlags.set(RideInvalidateFlag::customers);
}

/**
 *
 *  rct2: 0x00695DD2
 */
static void GuestPickRideToGoOn(Guest& guest)
{
    if (guest.State != PeepState::walking)
        return;
    if (!guest.GuestHeadingToRideId.IsNull())
        return;
    if (guest.PeepFlags & PEEP_FLAGS_LEAVING_PARK)
        return;
    if (guest.HasFoodOrDrink())
        return;
    if (guest.x == kLocationNull)
        return;

    auto* ride = GuestFindBestRideToGoOn(guest);
    if (ride != nullptr)
    {
        // Head to that ride
        guest.GuestHeadingToRideId = ride->id;
        guest.GuestIsLostCountdown = 200;
        guest.ResetPathfindGoal();
        guest.WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_ACTION;

        // Make peep look at their map if they have one
        if (guest.HasItem(ShopItem::map))
        {
            guest.ReadMap();
        }
    }
}

static OpenRCT2::BitSet<Limits::kMaxRidesInPark> GuestFindRidesToGoOn(Guest& guest)
{
    OpenRCT2::BitSet<Limits::kMaxRidesInPark> rideConsideration;

    // FIX  Originally checked for a toy, likely a mistake and should be a map,
    //      but then again this seems to only allow the peep to go on
    //      rides they haven't been on before.
    if (guest.HasItem(ShopItem::map))
    {
        // Consider rides that peep hasn't been on yet
        auto& gameState = getGameState();
        for (auto& ride : RideManager(gameState))
        {
            if (!guest.HasRidden(ride))
            {
                rideConsideration[ride.id.ToUnderlying()] = true;
            }
        }
    }
    else
    {
        // Take nearby rides into consideration
        constexpr auto radius = 10 * 32;
        int32_t cx = Numerics::floor2(guest.x, 32);
        int32_t cy = Numerics::floor2(guest.y, 32);
        for (int32_t tileX = cx - radius; tileX <= cx + radius; tileX += kCoordsXYStep)
        {
            for (int32_t tileY = cy - radius; tileY <= cy + radius; tileY += kCoordsXYStep)
            {
                auto location = CoordsXY{ tileX, tileY };
                if (!MapIsLocationValid(location))
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
        auto& gameState = getGameState();
        for (auto& ride : RideManager(gameState))
        {
            if (ride.highestDropHeight > 66 || ride.ratings.excitement >= RideRating::make(8, 00))
            {
                rideConsideration[ride.id.ToUnderlying()] = true;
            }
        }
    }

    return rideConsideration;
}

static Ride* GuestFindBestRideToGoOn(Guest& guest)
{
    // Pick the most exciting ride
    auto rideConsideration = GuestFindRidesToGoOn(guest);
    Ride* mostExcitingRide = nullptr;

    auto& gameState = getGameState();
    for (auto& ride : RideManager(gameState))
    {
        const auto rideIndex = ride.id.ToUnderlying();
        if (rideConsideration.size() > rideIndex && rideConsideration[rideIndex])
        {
            if (!(ride.lifecycleFlags & RIDE_LIFECYCLE_QUEUE_FULL))
            {
                if (guest.ShouldGoOnRide(ride, StationIndex::FromUnderlying(0), false, true) && RideHasRatings(ride))
                {
                    if (mostExcitingRide == nullptr || ride.ratings.excitement > mostExcitingRide->ratings.excitement)
                    {
                        mostExcitingRide = &ride;
                    }
                }
            }
        }
    }
    return mostExcitingRide;
}

/**
 * This function is called whenever a peep is deciding whether or not they want
 * to go on a ride or visit a shop. They may be physically present at the
 * ride/shop, or they may just be thinking about it.
 *  rct2: 0x006960AB
 */
bool Guest::ShouldGoOnRide(Ride& ride, StationIndex entranceNum, bool atQueue, bool thinking)
{
    // Indicates whether a peep is physically at the ride, or is just thinking about going on the ride.
    bool peepAtRide = !thinking;

    if (ride.status == RideStatus::open && !(ride.lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN))
    {
        // Peeps that are leaving the park will refuse to go on any rides, with the exception of free transport rides.
        assert(ride.type < std::size(kRideTypeDescriptors));
        if (!ride.getRideTypeDescriptor().HasFlag(RtdFlag::isTransportRide) || ride.value == kRideValueUndefined
            || RideGetPrice(ride) != 0)
        {
            if (PeepFlags & PEEP_FLAGS_LEAVING_PARK)
            {
                ChoseNotToGoOnRide(ride, peepAtRide, false);
                return false;
            }
        }

        if (ride.getRideTypeDescriptor().HasFlag(RtdFlag::isShopOrFacility))
        {
            return GuestShouldGoToShop(*this, ride, peepAtRide);
        }

        // This used to check !(flags & 2), but the function is only ever called with flags = 0, 1 or 6.
        // This means we can use the existing !(flags & 4) check.
        if (peepAtRide)
        {
            auto& station = ride.getStation(entranceNum);

            // Rides without queues can only have one peep waiting at a time.
            if (!atQueue)
            {
                if (!station.LastPeepInQueue.IsNull())
                {
                    GuestTriedToEnterFullQueue(*this, ride);
                    return false;
                }
            }
            else
            {
                // Check if there's room in the queue for the peep to enter.
                Guest* lastPeepInQueue = getGameState().entities.GetEntity<Guest>(station.LastPeepInQueue);
                if (lastPeepInQueue != nullptr && (abs(lastPeepInQueue->z - z) <= 6))
                {
                    int32_t dx = abs(lastPeepInQueue->x - x);
                    int32_t dy = abs(lastPeepInQueue->y - y);
                    int32_t maxD = std::max(dx, dy);

                    // Unlike normal paths, peeps cannot overlap when queueing for a ride.
                    // This check enforces a minimum distance between peeps entering the queue.
                    if (maxD < 8)
                    {
                        GuestTriedToEnterFullQueue(*this, ride);
                        return false;
                    }

                    // This checks if there's a peep standing still at the very end of the queue.
                    if (maxD <= 13 && lastPeepInQueue->TimeInQueue > 10)
                    {
                        GuestTriedToEnterFullQueue(*this, ride);
                        return false;
                    }
                }
            }
        }

        // Assuming the queue conditions are met, peeps will always go on free transport rides.
        // Ride ratings, recent crashes and weather will all be ignored.
        auto ridePrice = RideGetPrice(ride);
        if (!ride.getRideTypeDescriptor().HasFlag(RtdFlag::isTransportRide) || ride.value == kRideValueUndefined
            || ridePrice != 0)
        {
            if (PreviousRide == ride.id)
            {
                ChoseNotToGoOnRide(ride, peepAtRide, false);
                return false;
            }

            auto& gameState = getGameState();
            // Basic price checks
            if (ridePrice != 0 && !GuestHasVoucherForFreeRide(*this, ride) && !(gameState.park.flags & PARK_FLAGS_NO_MONEY))
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
                            InsertNewThought(PeepThoughtType::CantAffordRide, ride.id);
                        }
                    }
                    ChoseNotToGoOnRide(ride, peepAtRide, true);
                    return false;
                }
            }

            // If happy enough, peeps will ignore the fact that a ride has recently crashed.
            if (ride.lastCrashType != RIDE_CRASH_TYPE_NONE && Happiness < 225)
            {
                if (peepAtRide)
                {
                    InsertNewThought(PeepThoughtType::NotSafe, ride.id);
                    if (HappinessTarget >= 64)
                    {
                        HappinessTarget -= 8;
                    }
                    ride.updatePopularity(0);
                }
                ChoseNotToGoOnRide(ride, peepAtRide, true);
                return false;
            }

            if (RideHasRatings(ride))
            {
                // If a peep has already decided that they're going to go on a ride, they'll skip the weather and
                // excitement check and will only do a basic intensity check when they arrive at the ride itself.
                if (ride.id == GuestHeadingToRideId)
                {
                    if (ride.ratings.intensity > RideRating::make(10, 00) && !gameState.cheats.ignoreRideIntensity)
                    {
                        GuestRideIsTooIntense(*this, ride, peepAtRide);
                        return false;
                    }
                }
                else
                {
                    const bool isPrecipitating = ClimateIsPrecipitating();
                    if (isPrecipitating && !GuestShouldRideWhileRaining(*this, ride))
                    {
                        if (peepAtRide)
                        {
                            InsertNewThought(PeepThoughtType::NotWhileRaining, ride.id);
                            if (HappinessTarget >= 64)
                            {
                                HappinessTarget -= 8;
                            }
                            ride.updatePopularity(0);
                        }
                        ChoseNotToGoOnRide(ride, peepAtRide, true);
                        return false;
                    }
                    // If it is raining and the ride provides shelter skip the
                    // ride intensity check and get me on a sheltered ride!
                    if (!isPrecipitating || !GuestShouldRideWhileRaining(*this, ride))
                    {
                        if (!gameState.cheats.ignoreRideIntensity)
                        {
                            // Intensity calculations. Even though the max intensity can go up to 15, it's capped
                            // at 10.0 (before happiness calculations). A full happiness bar will increase the max
                            // intensity and decrease the min intensity by about 2.5.
                            RideRating_t maxIntensity = std::min(Intensity.GetMaximum() * 100, 1000) + Happiness;
                            RideRating_t minIntensity = (Intensity.GetMinimum() * 100) - Happiness;
                            if (ride.ratings.intensity < minIntensity)
                            {
                                if (peepAtRide)
                                {
                                    InsertNewThought(PeepThoughtType::MoreThrilling, ride.id);
                                    if (HappinessTarget >= 64)
                                    {
                                        HappinessTarget -= 8;
                                    }
                                    ride.updatePopularity(0);
                                }
                                ChoseNotToGoOnRide(ride, peepAtRide, true);
                                return false;
                            }
                            if (ride.ratings.intensity > maxIntensity)
                            {
                                GuestRideIsTooIntense(*this, ride, peepAtRide);
                                return false;
                            }

                            // Nausea calculations.
                            RideRating_t maxNausea = NauseaMaximumThresholds[(EnumValue(NauseaTolerance) & 3)] + Happiness;

                            if (ride.ratings.nausea > maxNausea)
                            {
                                if (peepAtRide)
                                {
                                    InsertNewThought(PeepThoughtType::Sickening, ride.id);
                                    if (HappinessTarget >= 64)
                                    {
                                        HappinessTarget -= 8;
                                    }
                                    ride.updatePopularity(0);
                                }
                                ChoseNotToGoOnRide(ride, peepAtRide, true);
                                return false;
                            }

                            // Very nauseous peeps will only go on very gentle rides.
                            if (ride.ratings.nausea >= RideRating::make(1, 40) && Nausea > 160)
                            {
                                ChoseNotToGoOnRide(ride, peepAtRide, false);
                                return false;
                            }
                        }
                    }
                }
            }

            // If the ride has not yet been rated and is capable of having g-forces,
            // there's a 90% chance that the peep will ignore it.
            if (!RideHasRatings(ride) && ride.getRideTypeDescriptor().HasFlag(RtdFlag::checkGForces))
            {
                if ((ScenarioRand() & 0xFFFF) > 0x1999u)
                {
                    ChoseNotToGoOnRide(ride, peepAtRide, false);
                    return false;
                }

                if (!gameState.cheats.ignoreRideIntensity)
                {
                    if (ride.maxPositiveVerticalG > MakeFixed16_2dp(5, 00)
                        || ride.maxNegativeVerticalG < MakeFixed16_2dp(-4, 00) || ride.maxLateralG > MakeFixed16_2dp(4, 00))
                    {
                        ChoseNotToGoOnRide(ride, peepAtRide, false);
                        return false;
                    }
                }
            }

            money64 value = ride.value;

            // If the value of the ride hasn't yet been calculated, peeps will be willing to pay any amount for the ride.
            if (value != kRideValueUndefined && !GuestHasVoucherForFreeRide(*this, ride)
                && !(gameState.park.flags & PARK_FLAGS_NO_MONEY))
            {
                // The amount peeps are willing to pay is decreased by 75% if they had to pay to enter the park.
                if (PeepFlags & PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY)
                    value /= 4;

                // Peeps won't pay more than twice the value of the ride.
                ridePrice = RideGetPrice(ride);
                if ((ridePrice > (value * 2)) && !(gameState.cheats.ignorePrice))
                {
                    if (peepAtRide)
                    {
                        InsertNewThought(PeepThoughtType::BadValue, ride.id);
                        if (HappinessTarget >= 60)
                        {
                            HappinessTarget -= 16;
                        }
                        ride.updatePopularity(0);
                    }
                    ChoseNotToGoOnRide(ride, peepAtRide, true);
                    return false;
                }

                // A ride is good value if the price is 50% or less of the ride value and the peep didn't pay to enter the park.
                if (ridePrice <= (value / 2) && peepAtRide)
                {
                    if (!(gameState.park.flags & PARK_FLAGS_NO_MONEY))
                    {
                        if (!(PeepFlags & PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY))
                        {
                            InsertNewThought(PeepThoughtType::GoodValue, ride.id);
                        }
                    }
                }
            }
        }

        // At this point, the peep has decided to go on the ride.
        if (peepAtRide)
        {
            ride.updatePopularity(1);
        }

        if (ride.id == GuestHeadingToRideId)
        {
            GuestResetRideHeading(*this);
        }

        ride.lifecycleFlags &= ~RIDE_LIFECYCLE_QUEUE_FULL;
        return true;
    }

    ChoseNotToGoOnRide(ride, peepAtRide, false);
    return false;
}

static bool GuestShouldGoToShop(Guest& guest, Ride& ride, bool peepAtShop)
{
    // Peeps won't go to the same shop twice in a row.
    if (ride.id == guest.PreviousRide)
    {
        guest.ChoseNotToGoOnRide(ride, peepAtShop, true);
        return false;
    }

    const auto& rtd = ride.getRideTypeDescriptor();
    if (rtd.specialType == RtdSpecialType::toilet)
    {
        if (guest.Toilet < 70)
        {
            guest.ChoseNotToGoOnRide(ride, peepAtShop, true);
            return false;
        }

        // The amount that peeps are willing to pay to use the Toilets scales with their toilet stat.
        // It effectively has a minimum of $0.10 (due to the check above) and a maximum of $0.60.
        if ((RideGetPrice(ride) * 40 > guest.Toilet) && !getGameState().cheats.ignorePrice)
        {
            if (peepAtShop)
            {
                guest.InsertNewThought(PeepThoughtType::NotPaying, ride.id);
                if (guest.HappinessTarget >= 60)
                {
                    guest.HappinessTarget -= 16;
                }
                ride.updatePopularity(0);
            }
            guest.ChoseNotToGoOnRide(ride, peepAtShop, true);
            return false;
        }
    }

    if (rtd.specialType == RtdSpecialType::firstAid)
    {
        if (guest.Nausea < 128)
        {
            guest.ChoseNotToGoOnRide(ride, peepAtShop, true);
            return false;
        }
    }

    // Basic price checks
    auto ridePrice = RideGetPrice(ride);
    if (ridePrice != 0 && ridePrice > guest.CashInPocket)
    {
        if (peepAtShop)
        {
            if (guest.CashInPocket <= 0)
            {
                guest.InsertNewThought(PeepThoughtType::SpentMoney);
            }
            else
            {
                guest.InsertNewThought(PeepThoughtType::CantAffordRide, ride.id);
            }
        }
        guest.ChoseNotToGoOnRide(ride, peepAtShop, true);
        return false;
    }

    if (peepAtShop)
    {
        ride.updatePopularity(1);
        if (ride.id == guest.GuestHeadingToRideId)
        {
            GuestResetRideHeading(guest);
        }
    }
    return true;
}

// Used when no logging to an expend type required
void Guest::SpendMoney(money64 amount, ExpenditureType expenditure)
{
    money64 unused;
    SpendMoney(unused, amount, expenditure);
}

/**
 *
 *  rct2: 0x0069926C
 * Expend type was previously an offset saved in 0x00F1AEC0
 */
void Guest::SpendMoney(money64& peep_expend_type, money64 amount, ExpenditureType expenditure)
{
    assert(!(getGameState().park.flags & PARK_FLAGS_NO_MONEY));

    CashInPocket = std::max(0.00_GBP, CashInPocket - amount);
    CashSpent += amount;

    peep_expend_type += amount;

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByNumber(WindowClass::peep, Id);

    FinancePayment(-amount, expenditure);

    MoneyEffect::CreateAt(amount, GetLocation(), true);

    Audio::Play3D(Audio::SoundId::purchase, GetLocation());
}

void Guest::SetHasRidden(const Ride& ride)
{
    RideUse::GetHistory().Add(Id, ride.id);

    SetHasRiddenRideType(ride.type);
}

bool Guest::HasRidden(const Ride& ride) const
{
    return RideUse::GetHistory().Contains(Id, ride.id);
}

void Guest::SetHasRiddenRideType(ride_type_t rideType)
{
    RideUse::GetTypeHistory().Add(Id, rideType);
}

bool Guest::HasRiddenRideType(ride_type_t rideType) const
{
    return RideUse::GetTypeHistory().Contains(Id, rideType);
}

void Guest::SetParkEntryTime(int32_t entryTime)
{
    ParkEntryTime = entryTime;
}

int32_t Guest::GetParkEntryTime() const
{
    return ParkEntryTime;
}

static bool GuestShouldRideWhileRaining(Guest& guest, const Ride& ride)
{
    // Peeps will go on rides that are sufficiently undercover while it's raining.
    // The threshold is fairly low and only requires about 10-15% of the ride to be undercover.
    if (ride.shelteredEighths >= 3)
    {
        return true;
    }

    // Peeps with umbrellas will go on rides where they can use their umbrella on it (like the Maze) 50% of the time
    if (guest.HasItem(ShopItem::umbrella) && ride.getRideTypeDescriptor().HasFlag(RtdFlag::guestsCanUseUmbrella)
        && (ScenarioRand() & 2) == 0)
    {
        return true;
    }

    return false;
}

void Guest::ChoseNotToGoOnRide(const Ride& ride, bool peepAtRide, bool updateLastRide)
{
    if (peepAtRide && updateLastRide)
    {
        PreviousRide = ride.id;
        PreviousRideTimeOut = 0;
    }

    if (ride.id == GuestHeadingToRideId)
    {
        GuestResetRideHeading(*this);
    }
}

void Guest::ReadMap()
{
    if (IsActionInterruptableSafely())
    {
        Action = PeepActionType::readMap;
        AnimationFrameNum = 0;
        AnimationImageIdOffset = 0;
        UpdateCurrentAnimationType();
    }
}

static bool GuestHasVoucherForFreeRide(Guest& guest, const Ride& ride)
{
    return guest.HasItem(ShopItem::voucher) && guest.VoucherType == VOUCHER_TYPE_RIDE_FREE && guest.VoucherRideId == ride.id;
}

/**
 * When the queue is full, peeps will ignore the ride when thinking about what to go on next.
 * Does not effect peeps that walk up to the queue entrance.
 * This flag is reset the next time a peep successfully joins the queue.
 */
static void GuestTriedToEnterFullQueue(Guest& guest, Ride& ride)
{
    ride.lifecycleFlags |= RIDE_LIFECYCLE_QUEUE_FULL;
    guest.PreviousRide = ride.id;
    guest.PreviousRideTimeOut = 0;
    // Change status "Heading to" to "Walking" if queue is full
    if (ride.id == guest.GuestHeadingToRideId)
    {
        GuestResetRideHeading(guest);
    }
}

static void GuestResetRideHeading(Guest& guest)
{
    guest.GuestHeadingToRideId = RideId::GetNull();
    guest.WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_ACTION;
}

static void GuestRideIsTooIntense(Guest& guest, Ride& ride, bool peepAtRide)
{
    if (peepAtRide)
    {
        guest.InsertNewThought(PeepThoughtType::Intense, ride.id);
        if (guest.HappinessTarget >= 64)
        {
            guest.HappinessTarget -= 8;
        }
        ride.updatePopularity(0);
    }
    guest.ChoseNotToGoOnRide(ride, peepAtRide, true);
}

/**
 *
 *  rct2: 0x00691C6E
 */
static Vehicle* PeepChooseCarFromRide(Guest& guest, const Ride& ride, std::span<const uint8_t> carArray)
{
    uint8_t chosen_car = ScenarioRand();
    if (ride.getRideTypeDescriptor().HasFlag(RtdFlag::hasGForces) && ((chosen_car & 0xC) != 0xC))
    {
        chosen_car = (ScenarioRand() & 1) ? 0 : static_cast<uint8_t>(carArray.size()) - 1;
    }
    else
    {
        chosen_car = (chosen_car * static_cast<uint16_t>(carArray.size())) >> 8;
    }

    guest.CurrentCar = carArray[chosen_car];

    Vehicle* vehicle = getGameState().entities.GetEntity<Vehicle>(ride.vehicles[guest.CurrentTrain]);
    if (vehicle == nullptr)
    {
        return nullptr;
    }
    return vehicle->GetCar(guest.CurrentCar);
}

/**
 *
 *  rct2: 0x00691CD1
 */
static void PeepChooseSeatFromCar(Guest* guest, const Ride& ride, Vehicle* vehicle)
{
    if (vehicle == nullptr)
    {
        return;
    }
    uint8_t chosen_seat = vehicle->next_free_seat;

    if (ride.mode == RideMode::forwardRotation || ride.mode == RideMode::backwardRotation)
    {
        chosen_seat = (((~vehicle->flatRideAnimationFrame + 1) >> 3) & 0xF) * 2;
        if (vehicle->next_free_seat & 1)
        {
            chosen_seat++;
        }
    }
    guest->CurrentSeat = chosen_seat;
    vehicle->next_free_seat++;

    vehicle->peep[guest->CurrentSeat] = guest->Id;
    vehicle->peep_tshirt_colours[guest->CurrentSeat] = guest->TshirtColour;
}

/**
 *
 *  rct2: 0x00691D27
 */
void Guest::GoToRideEntrance(const Ride& ride)
{
    const auto& station = ride.getStation(CurrentRideStation);
    if (station.Entrance.IsNull())
    {
        RemoveFromQueue();
        return;
    }

    auto location = station.Entrance.ToCoordsXYZD().ToTileCentre();
    int16_t x_shift = DirectionOffsets[location.direction].x;
    int16_t y_shift = DirectionOffsets[location.direction].y;

    uint8_t shift_multiplier = 21;
    const auto* rideEntry = GetRideEntryByIndex(ride.subtype);
    if (rideEntry != nullptr)
    {
        if (rideEntry->Cars[rideEntry->DefaultCar].flags.hasAny(
                CarEntryFlag::isMiniGolf, CarEntryFlag::isChairlift, CarEntryFlag::isGoKart))
        {
            shift_multiplier = 32;
        }
    }

    x_shift *= shift_multiplier;
    y_shift *= shift_multiplier;

    location.x += x_shift;
    location.y += y_shift;

    SetDestination(location, 2);
    SetState(PeepState::enteringRide);
    RideSubState = PeepRideSubState::inEntrance;

    RejoinQueueTimeout = 0;
    GuestTimeOnRide = 0;

    RemoveFromQueue();
}

static bool FindVehicleToEnter(
    Guest& guest, const Ride& ride, sfl::static_vector<uint8_t, Limits::kMaxTrainsPerRide>& car_array)
{
    uint8_t chosen_train = RideStation::kNoTrain;

    if (ride.mode == RideMode::dodgems || ride.mode == RideMode::race)
    {
        if (ride.lifecycleFlags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
            return false;

        for (int32_t i = 0; i < ride.numTrains; ++i)
        {
            Vehicle* vehicle = getGameState().entities.GetEntity<Vehicle>(ride.vehicles[i]);
            if (vehicle == nullptr)
                continue;

            if (vehicle->next_free_seat >= vehicle->num_seats)
                continue;

            if (vehicle->status != Vehicle::Status::waitingForPassengers)
                continue;
            chosen_train = i;
            break;
        }
    }
    else
    {
        chosen_train = ride.getStation(guest.CurrentRideStation).TrainAtStation;
    }
    if (chosen_train >= Limits::kMaxTrainsPerRide)
    {
        return false;
    }

    guest.CurrentTrain = chosen_train;

    int32_t i = 0;

    auto vehicle_id = ride.vehicles[chosen_train];
    for (Vehicle* vehicle = getGameState().entities.GetEntity<Vehicle>(vehicle_id); vehicle != nullptr;
         vehicle = getGameState().entities.GetEntity<Vehicle>(vehicle->next_vehicle_on_train), ++i)
    {
        uint8_t num_seats = vehicle->num_seats;
        if (vehicle->IsUsedInPairs())
        {
            if (vehicle->next_free_seat & 1)
            {
                car_array.clear();
                car_array.push_back(i);
                return true;
            }
            num_seats &= kVehicleSeatNumMask;
        }
        if (num_seats == vehicle->next_free_seat)
            continue;

        if (ride.mode == RideMode::forwardRotation || ride.mode == RideMode::backwardRotation)
        {
            uint8_t position = (((~vehicle->flatRideAnimationFrame + 1) >> 3) & 0xF) * 2;
            if (!vehicle->peep[position].IsNull())
                continue;
        }
        car_array.push_back(i);
    }

    return !car_array.empty();
}

static void PeepUpdateRideAtEntranceTryLeave(Guest& guest)
{
    // Destination Tolerance is zero when peep has completely
    // entered entrance
    if (guest.DestinationTolerance == 0)
    {
        guest.RemoveFromQueue();
        guest.SetState(PeepState::falling);
    }
}

static bool PeepCheckRidePriceAtEntrance(Guest& guest, const Ride& ride, money64 ridePrice)
{
    if ((guest.HasItem(ShopItem::voucher)) && guest.VoucherType == VOUCHER_TYPE_RIDE_FREE
        && guest.VoucherRideId == guest.CurrentRide)
        return true;

    if (guest.CashInPocket <= 0 && !(getGameState().park.flags & PARK_FLAGS_NO_MONEY))
    {
        guest.InsertNewThought(PeepThoughtType::SpentMoney);
        PeepUpdateRideAtEntranceTryLeave(guest);
        return false;
    }

    if (ridePrice > guest.CashInPocket)
    {
        // Prevent looping of same thought / animation since Destination Tolerance
        // is only 0 exactly at entrance and will immediately change as guest
        // tries to leave hereafter
        if (guest.DestinationTolerance == 0)
        {
            guest.InsertNewThought(PeepThoughtType::CantAffordRide, guest.CurrentRide);
        }
        PeepUpdateRideAtEntranceTryLeave(guest);
        return false;
    }

    auto value = ride.value;
    if (value != kRideValueUndefined)
    {
        if (((value * 2) < ridePrice) && !(getGameState().cheats.ignorePrice))
        {
            guest.InsertNewThought(PeepThoughtType::BadValue, guest.CurrentRide);
            PeepUpdateRideAtEntranceTryLeave(guest);
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
static int16_t GuestCalculateRideSatisfaction(Guest& guest, const Ride& ride)
{
    int16_t satisfaction = GuestCalculateRideValueSatisfaction(guest, ride);
    satisfaction += GuestCalculateRideIntensityNauseaSatisfaction(guest, ride);

    // Calculate satisfaction based on how long the peep has been in the queue for.
    // (For comparison: peeps start thinking "I've been queueing for a long time" at 3500 and
    // start leaving the queue at 4300.)
    if (guest.TimeInQueue >= 4500)
        satisfaction -= 35;
    else if (guest.TimeInQueue >= 2250)
        satisfaction -= 10;
    else if (guest.TimeInQueue <= 750)
        satisfaction += 10;

    // Peeps get a small boost in satisfaction if they've been on a ride of the same type before,
    // and this boost is doubled if they've already been on this particular ride.
    if (guest.HasRiddenRideType(ride.type))
        satisfaction += 10;

    if (guest.HasRidden(*GetRide(guest.CurrentRide)))
        satisfaction += 10;

    return satisfaction;
}

/**
 * Check to see if the specified ride should become the peep's favourite.
 * For this, a "ride rating" is calculated based on the excitement of the ride and the satisfaction of the ride.
 * As this value cannot exceed 255, the more satisfied the peep is, the more irrelevant the ride's excitement becomes.
 * Due to the minimum happiness requirement, an excitement rating of more than 3.8 has no further effect.
 *
 * If the ride rating is higher than any ride the peep has already been on and the happiness criteria is met,
 * the ride becomes the peep's favourite. (This doesn't happen right away, but will be updated once the peep
 * exits the ride.)
 */
static void GuestUpdateFavouriteRide(Guest& guest, const Ride& ride, uint8_t satisfaction)
{
    guest.PeepFlags &= ~PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE;
    uint8_t peepRideRating = std::clamp((ride.ratings.excitement / 4) + satisfaction, 0, kPeepMaxHappiness);
    if (peepRideRating >= guest.FavouriteRideRating)
    {
        if (guest.Happiness >= 160 && guest.HappinessTarget >= 160)
        {
            guest.FavouriteRideRating = peepRideRating;
            guest.PeepFlags |= PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE;
        }
    }
}

/* rct2: 0x00695555 */
static int16_t GuestCalculateRideValueSatisfaction(Guest& guest, const Ride& ride)
{
    if (getGameState().park.flags & PARK_FLAGS_NO_MONEY)
    {
        return -30;
    }

    if (ride.value == kRideValueUndefined)
    {
        return -30;
    }

    auto ridePrice = RideGetPrice(ride);
    if (ride.value >= ridePrice)
    {
        return -5;
    }

    if ((ride.value + ((ride.value * guest.Happiness) / 256)) >= ridePrice)
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
static int16_t GuestCalculateRideIntensityNauseaSatisfaction(Guest& guest, const Ride& ride)
{
    if (!RideHasRatings(ride))
    {
        return 70;
    }

    uint8_t intensitySatisfaction = 3;
    uint8_t nauseaSatisfaction = 3;
    RideRating_t maxIntensity = guest.Intensity.GetMaximum() * 100;
    RideRating_t minIntensity = guest.Intensity.GetMinimum() * 100;
    if (minIntensity <= ride.ratings.intensity && maxIntensity >= ride.ratings.intensity)
    {
        intensitySatisfaction--;
    }
    minIntensity -= guest.Happiness * 2;
    maxIntensity += guest.Happiness;
    if (minIntensity <= ride.ratings.intensity && maxIntensity >= ride.ratings.intensity)
    {
        intensitySatisfaction--;
    }
    minIntensity -= guest.Happiness * 2;
    maxIntensity += guest.Happiness;
    if (minIntensity <= ride.ratings.intensity && maxIntensity >= ride.ratings.intensity)
    {
        intensitySatisfaction--;
    }

    // Although it's not shown in the interface, a peep with Average or High nausea tolerance
    // has a minimum preferred nausea value. (For peeps with None or Low, this is set to zero.)
    RideRating_t minNausea = kNauseaMinimumThresholds[(EnumValue(guest.NauseaTolerance) & 3)];
    RideRating_t maxNausea = NauseaMaximumThresholds[(EnumValue(guest.NauseaTolerance) & 3)];
    if (minNausea <= ride.ratings.nausea && maxNausea >= ride.ratings.nausea)
    {
        nauseaSatisfaction--;
    }
    minNausea -= guest.Happiness * 2;
    maxNausea += guest.Happiness;
    if (minNausea <= ride.ratings.nausea && maxNausea >= ride.ratings.nausea)
    {
        nauseaSatisfaction--;
    }
    minNausea -= guest.Happiness * 2;
    maxNausea += guest.Happiness;
    if (minNausea <= ride.ratings.nausea && maxNausea >= ride.ratings.nausea)
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
static void GuestUpdateRideNauseaGrowth(Guest& guest, const Ride& ride)
{
    const auto nauseaMultiplier = std::clamp(256 - guest.HappinessTarget, 64, 200);
    const auto rideGeneratedNausea = (ride.ratings.nausea * nauseaMultiplier) / 512;
    const auto hungerAdjustedNausea = ((rideGeneratedNausea * std::max<uint8_t>(128, guest.Hunger)) / 128) * 2;
    const auto nauseaGrowthRateChange = hungerAdjustedNausea >> (EnumValue(guest.NauseaTolerance) & 3);
    guest.NauseaTarget = static_cast<uint8_t>(std::min<int32_t>(guest.NauseaTarget + nauseaGrowthRateChange, 255));
}

static bool GuestShouldGoOnRideAgain(Guest& guest, const Ride& ride)
{
    if (!ride.getRideTypeDescriptor().HasFlag(RtdFlag::guestsWillRideAgain))
        return false;
    if (!RideHasRatings(ride))
        return false;
    if (ride.ratings.intensity > RideRating::make(10, 00) && !getGameState().cheats.ignoreRideIntensity)
        return false;
    if (guest.Happiness < 180)
        return false;
    if (guest.Energy < 100)
        return false;
    if (guest.Nausea > 160)
        return false;
    if (guest.Hunger < 30)
        return false;
    if (guest.Thirst < 20)
        return false;
    if (guest.Toilet > 170)
        return false;

    uint8_t r = (ScenarioRand() & 0xFF);
    if (r <= 128)
    {
        if (guest.GuestNumRides > 7)
            return false;
        if (r > 64)
            return false;
    }

    return true;
}

static bool GuestShouldPreferredIntensityIncrease(Guest& guest)
{
    if (getGameState().park.flags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES)
        return false;
    if (guest.Happiness < 200)
        return false;

    return (ScenarioRand() & 0xFF) >= static_cast<uint8_t>(guest.Intensity);
}

static bool GuestReallyLikedRide(Guest& guest, const Ride& ride)
{
    if (guest.Happiness < 215)
        return false;
    if (guest.Nausea > 120)
        return false;
    if (!RideHasRatings(ride))
        return false;
    if (ride.ratings.intensity > RideRating::make(10, 00) && !getGameState().cheats.ignoreRideIntensity)
        return false;
    return true;
}

/**
 *
 *  rct2: 0x0069BC9A
 */
static PeepThoughtType GuestAssessSurroundings(int16_t centre_x, int16_t centre_y, int16_t centre_z)
{
    if ((TileElementHeight({ centre_x, centre_y })) > centre_z)
        return PeepThoughtType::None;

    uint16_t num_scenery = 0;
    uint16_t num_fountains = 0;
    uint16_t nearby_music = 0;
    uint16_t num_rubbish = 0;

    // TODO: Refactor this to step as tiles, 160 units is 5 tiles.
    int16_t initial_x = std::max(centre_x - 160, 0);
    int16_t initial_y = std::max(centre_y - 160, 0);
    int16_t final_x = std::min(centre_x + 160, kMaximumMapSizeBig);
    int16_t final_y = std::min(centre_y + 160, kMaximumMapSizeBig);

    for (int16_t x = initial_x; x < final_x; x += kCoordsXYStep)
    {
        for (int16_t y = initial_y; y < final_y; y += kCoordsXYStep)
        {
            for (auto* tileElement : TileElementsView(CoordsXY{ x, y }))
            {
                if (tileElement->IsGhost())
                {
                    continue;
                }

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

                        if (pathAddEntry->flags
                            & (PATH_ADDITION_FLAG_JUMPING_FOUNTAIN_WATER | PATH_ADDITION_FLAG_JUMPING_FOUNTAIN_SNOW))
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
                        auto* ride = GetRide(tileElement->AsTrack()->GetRideIndex());
                        if (ride == nullptr)
                            break;

                        bool isPlayingMusic = ride->lifecycleFlags & RIDE_LIFECYCLE_MUSIC && ride->status != RideStatus::closed
                            && !(ride->lifecycleFlags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED));
                        if (!isPlayingMusic)
                            break;

                        const auto* musicObject = ride->getMusicObject();
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

    if (num_rubbish < 2 && !getGameState().cheats.disableLittering)
        // if disable littering cheat is enabled, peeps will not have the "clean and tidy park" thought
        return PeepThoughtType::VeryClean;

    return PeepThoughtType::None;
}

/**
 *
 *  rct2: 0x0068F9A9
 */
static void GuestUpdateHunger(Guest& guest)
{
    if (guest.Hunger >= 3)
    {
        guest.Hunger -= 2;

        guest.EnergyTarget = std::min<uint16_t>(guest.EnergyTarget + 2, kPeepMaxEnergyTarget);
        guest.Toilet = std::min(guest.Toilet + 1, 255);
    }
}

/**
 * Main purpose is to decide when peeps leave the park due to
 * low happiness, low energy and (if appropriate) low money.
 *
 *  rct2: 0x0068F8CD
 */
static void GuestDecideWhetherToLeavePark(Guest& guest)
{
    if (guest.EnergyTarget >= 33)
    {
        guest.EnergyTarget -= 2;
    }

    if (getGameState().weatherCurrent.temperature >= 21 && guest.Thirst >= 5)
    {
        guest.Thirst--;
    }

    if (guest.OutsideOfPark)
    {
        return;
    }

    /* Peeps that are happy enough, have enough energy and
     * (if appropriate) have enough money will always stay
     * in the park. */
    if (!(guest.PeepFlags & PEEP_FLAGS_LEAVING_PARK))
    {
        if (getGameState().park.flags & PARK_FLAGS_NO_MONEY)
        {
            if (guest.Energy >= 70 && guest.Happiness >= 60)
            {
                return;
            }
        }
        else
        {
            if (guest.Energy >= 55 && guest.Happiness >= 45 && guest.CashInPocket >= 5.00_GBP)
            {
                return;
            }
        }
    }

    // Approx 95% chance of staying in the park
    if ((ScenarioRand() & 0xFFFF) > 3276)
    {
        return;
    }

    // In the remaining 5% chance the peep leaves the park.
    GuestLeavePark(guest);
}

/**
 *
 *  rct2: 0x0068F93E
 */
static void GuestLeavePark(Guest& guest)
{
    guest.GuestHeadingToRideId = RideId::GetNull();
    if (guest.PeepFlags & PEEP_FLAGS_LEAVING_PARK)
    {
        if (guest.GuestIsLostCountdown < 60)
        {
            return;
        }
    }
    else
    {
        guest.GuestIsLostCountdown = 254;
        guest.PeepFlags |= PEEP_FLAGS_LEAVING_PARK;
        guest.PeepFlags &= ~PEEP_FLAGS_PARK_ENTRANCE_CHOSEN;
    }

    guest.InsertNewThought(PeepThoughtType::GoHome);

    auto* windowMgr = Ui::GetWindowManager();
    WindowBase* w = windowMgr->FindByNumber(WindowClass::peep, guest.Id);
    if (w != nullptr)
        w->onPrepareDraw();
    windowMgr->InvalidateByNumber(WindowClass::peep, guest.Id);
}

template<typename T>
static void PeepHeadForNearestRide(Guest& guest, bool considerOnlyCloseRides, T predicate)
{
    if (guest.State != PeepState::sitting && guest.State != PeepState::watching && guest.State != PeepState::walking)
    {
        return;
    }
    if (guest.PeepFlags & PEEP_FLAGS_LEAVING_PARK)
        return;
    if (guest.x == kLocationNull)
        return;
    if (!guest.GuestHeadingToRideId.IsNull())
    {
        auto ride = GetRide(guest.GuestHeadingToRideId);
        if (ride != nullptr && predicate(*ride))
        {
            return;
        }
    }

    OpenRCT2::BitSet<Limits::kMaxRidesInPark> rideConsideration;
    if (!considerOnlyCloseRides && (guest.HasItem(ShopItem::map)))
    {
        // Consider all rides in the park
        auto& gameState = getGameState();
        for (const auto& ride : RideManager(gameState))
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
        constexpr auto kSearchRadius = 10 * 32;
        int32_t cx = Numerics::floor2(guest.x, 32);
        int32_t cy = Numerics::floor2(guest.y, 32);
        for (auto x = cx - kSearchRadius; x <= cx + kSearchRadius; x += kCoordsXYStep)
        {
            for (auto y = cy - kSearchRadius; y <= cy + kSearchRadius; y += kCoordsXYStep)
            {
                auto location = CoordsXY{ x, y };
                if (!MapIsLocationValid(location))
                    continue;

                for (auto* trackElement : TileElementsView<TrackElement>(location))
                {
                    auto rideIndex = trackElement->GetRideIndex();
                    auto ride = GetRide(rideIndex);
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
    RideId potentialRides[Limits::kMaxRidesInPark];
    size_t numPotentialRides = 0;

    auto& gameState = getGameState();
    for (auto& ride : RideManager(gameState))
    {
        if (rideConsideration[ride.id.ToUnderlying()])
        {
            if (!(ride.lifecycleFlags & RIDE_LIFECYCLE_QUEUE_FULL))
            {
                if (guest.ShouldGoOnRide(ride, StationIndex::FromUnderlying(0), false, true))
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
        auto ride = GetRide(potentialRides[i]);
        if (ride != nullptr)
        {
            auto rideLocation = ride->getStation().Start;
            int32_t distance = abs(rideLocation.x - guest.x) + abs(rideLocation.y - guest.y);
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
        guest.GuestHeadingToRideId = closestRide->id;
        guest.GuestIsLostCountdown = 200;
        guest.ResetPathfindGoal();
        guest.WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_ACTION;
        guest.TimeLost = 0;
    }
}

static void GuestHeadForNearestRideWithFlag(Guest& guest, bool considerOnlyCloseRides, RtdFlag rtdFlag)
{
    PeepHeadForNearestRide(
        guest, considerOnlyCloseRides, [rtdFlag](const Ride& ride) { return ride.getRideTypeDescriptor().HasFlag(rtdFlag); });
}

static void GuestHeadForNearestRideWithSpecialType(Guest& guest, bool considerOnlyCloseRides, RtdSpecialType specialType)
{
    if ((specialType == RtdSpecialType::toilet) && guest.HasFoodOrDrink())
    {
        return;
    }
    PeepHeadForNearestRide(guest, considerOnlyCloseRides, [specialType](const Ride& ride) {
        return ride.getRideTypeDescriptor().specialType == specialType;
    });
}

/**
 *
 *  rct2: 0x00699FE3
 * Stops peeps that are having thoughts
 * such as "I'm hungry" after visiting a food shop.
 * Works for Thirst/Hungry/Low Money/Toilet
 */
static void GuestStopPurchaseThought(Guest& guest, ride_type_t rideType)
{
    auto thoughtType = PeepThoughtType::Hungry;

    const auto& rtd = GetRideTypeDescriptor(rideType);
    if (!rtd.HasFlag(RtdFlag::sellsFood))
    {
        thoughtType = PeepThoughtType::Thirsty;
        if (!rtd.HasFlag(RtdFlag::sellsDrinks))
        {
            thoughtType = PeepThoughtType::RunningOut;
            if (rtd.specialType != RtdSpecialType::cashMachine)
            {
                thoughtType = PeepThoughtType::Toilet;
                if (rtd.specialType != RtdSpecialType::toilet)
                {
                    return;
                }
            }
        }
    }

    // Remove the related thought
    for (int32_t i = 0; i < kPeepMaxThoughts; ++i)
    {
        PeepThought* thought = &guest.Thoughts[i];

        if (thought->type == PeepThoughtType::None)
            break;

        if (thought->type != thoughtType)
            continue;

        if (i < kPeepMaxThoughts - 1)
        {
            memmove(thought, thought + 1, sizeof(PeepThought) * (kPeepMaxThoughts - i - 1));
        }

        guest.Thoughts[kPeepMaxThoughts - 1].type = PeepThoughtType::None;

        guest.WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_THOUGHTS;
        i--;
    }
}

/**
 *
 *  rct2: 0x0069AEB7
 */
static bool PeepShouldUseCashMachine(Guest& guest, RideId rideIndex)
{
    if (getGameState().park.flags & PARK_FLAGS_NO_MONEY)
        return false;
    if (guest.PeepFlags & PEEP_FLAGS_LEAVING_PARK)
        return false;
    if (guest.CashInPocket > 20.00_GBP)
        return false;
    if (115 + (ScenarioRand() % 128) > guest.Happiness)
        return false;
    if (guest.Energy < 80)
        return false;

    auto ride = GetRide(rideIndex);
    if (ride != nullptr)
    {
        ride->updateSatisfaction(guest.Happiness >> 6);
        ride->curNumCustomers++;
        ride->totalCustomers = AddClamp(ride->totalCustomers, 1u);
        ride->windowInvalidateFlags.set(RideInvalidateFlag::customers);
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

    auto ride = GetRide(CurrentRide);
    if (ride == nullptr || ride->status != RideStatus::open)
    {
        SetState(PeepState::falling);
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
        if (rtd.specialType == RtdSpecialType::cashMachine)
        {
            if (CurrentRide != PreviousRide)
            {
                CashInPocket += 50.00_GBP;
            }
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->InvalidateByNumber(WindowClass::peep, Id);
        }
        Orientation ^= 0x10;

        auto destination = CoordsXY{ 16, 16 } + NextLoc;
        SetDestination(destination);
        PeepDirection = DirectionReverse(PeepDirection);

        SetState(PeepState::walking);
        return;
    }

    bool item_bought = false;

    if (CurrentRide != PreviousRide)
    {
        const auto& rtd = GetRideTypeDescriptor(ride->type);
        if (rtd.specialType == RtdSpecialType::cashMachine)
        {
            item_bought = PeepShouldUseCashMachine(*this, CurrentRide);
            if (!item_bought)
            {
                PreviousRide = CurrentRide;
                PreviousRideTimeOut = 0;
            }
            else
            {
                Action = PeepActionType::withdrawMoney;
                AnimationFrameNum = 0;
                AnimationImageIdOffset = 0;

                UpdateCurrentAnimationType();

                ride->numPrimaryItemsSold = AddClamp(ride->numPrimaryItemsSold, 1u);
            }
        }
        else
        {
            const auto* ride_type = GetRideEntryByIndex(ride->subtype);
            if (ride_type == nullptr)
            {
                return;
            }
            if (ride_type->shop_item[1] != ShopItem::none)
            {
                auto price = ride->price[1];

                item_bought = GuestDecideAndBuyItem(*this, *ride, ride_type->shop_item[1], price);
                if (item_bought)
                {
                    ride->numSecondaryItemsSold = AddClamp(ride->numSecondaryItemsSold, 1u);
                }
            }

            if (!item_bought && ride_type->shop_item[0] != ShopItem::none)
            {
                auto price = ride->price[0];

                item_bought = GuestDecideAndBuyItem(*this, *ride, ride_type->shop_item[0], price);
                if (item_bought)
                {
                    ride->numPrimaryItemsSold = AddClamp(ride->numPrimaryItemsSold, 1u);
                }
            }
        }
    }

    if (item_bought)
    {
        ride->updatePopularity(1);

        GuestStopPurchaseThought(*this, ride->type);
    }
    else
    {
        ride->updatePopularity(0);
    }
    SubState = 1;
}

/**
 *
 *  rct2: 0x00691A3B
 */
void Guest::UpdateRideAtEntrance()
{
    auto ride = GetRide(CurrentRide);
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
                const auto& station = ride->getStation(CurrentRideStation);
                auto entrance = station.Entrance.ToCoordsXYZ();
                actionZ = entrance.z + 2;
            }
            MoveTo({ loc.value(), actionZ });
        }
        else
        {
            DestinationTolerance = 0;
            Orientation ^= (1 << 4);
            Invalidate();
        }
    }

    sfl::static_vector<uint8_t, Limits::kMaxTrainsPerRide> carArray;

    if (ride->getRideTypeDescriptor().HasFlag(RtdFlag::noVehicles))
    {
        if (ride->numRiders >= ride->operationOption)
            return;
    }
    else
    {
        if (!FindVehicleToEnter(*this, *ride, carArray))
            return;
    }

    if (ride->status != RideStatus::open || ride->vehicleChangeTimeout != 0)
    {
        PeepUpdateRideAtEntranceTryLeave(*this);
        return;
    }

    if (ride->lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN)
        return;

    auto ridePrice = RideGetPrice(*ride);
    if (ridePrice != 0)
    {
        if (!PeepCheckRidePriceAtEntrance(*this, *ride, ridePrice))
            return;
    }

    if (!ride->getRideTypeDescriptor().HasFlag(RtdFlag::noVehicles))
    {
        Vehicle* vehicle = PeepChooseCarFromRide(*this, *ride, carArray);
        PeepChooseSeatFromCar(this, *ride, vehicle);
    }
    GoToRideEntrance(*ride);
}

/** rct2: 0x00981FD4, 0x00981FD6 */
static constexpr CoordsXY kMazeEntranceStart[] = {
    { 8, 8 },
    { 8, 24 },
    { 24, 24 },
    { 24, 8 },
};

void PeepUpdateRideLeaveEntranceMaze(Guest& guest, Ride& ride, CoordsXYZD& entrance_loc)
{
    guest.MazeLastEdge = entrance_loc.direction + 1;

    entrance_loc.x += CoordsDirectionDelta[entrance_loc.direction].x;
    entrance_loc.y += CoordsDirectionDelta[entrance_loc.direction].y;

    uint8_t direction = entrance_loc.direction * 4 + 11;
    if (ScenarioRand() & 0x40)
    {
        direction += 4;
        guest.MazeLastEdge += 2;
    }

    direction &= 0xF;
    // Direction is 11, 15, 3, or 7
    guest.Var37 = direction;
    guest.MazeLastEdge &= 3;

    entrance_loc.x += kMazeEntranceStart[direction / 4].x;
    entrance_loc.y += kMazeEntranceStart[direction / 4].y;

    guest.SetDestination(entrance_loc, 3);

    ride.curNumCustomers++;
    guest.OnEnterRide(ride);
    guest.RideSubState = PeepRideSubState::mazePathfinding;
}

void PeepUpdateRideLeaveEntranceSpiralSlide(Guest& guest, Ride& ride, CoordsXYZD& entrance_loc)
{
    entrance_loc = { ride.getStation(guest.CurrentRideStation).GetStart(), entrance_loc.direction };

    TileElement* tile_element = RideGetStationStartTrackElement(ride, guest.CurrentRideStation);

    uint8_t direction_track = (tile_element == nullptr ? 0 : tile_element->GetDirection());

    guest.Var37 = (entrance_loc.direction << 2) | (direction_track << 4);

    entrance_loc += kSpiralSlideWalkingPath[guest.Var37];

    guest.SetDestination(entrance_loc);
    guest.timesSlidDown = 0;

    ride.curNumCustomers++;
    guest.OnEnterRide(ride);
    guest.RideSubState = PeepRideSubState::approachSpiralSlide;
}

void PeepUpdateRideLeaveEntranceDefault(Guest& guest, Ride& ride, CoordsXYZD& entrance_loc)
{
    const auto currentTicks = getGameState().currentTicks;

    // If the ride type was changed guests will become stuck.
    // Inform the player about this if its a new issue or hasn't been addressed within 120 seconds.
    if ((ride.currentIssues & RIDE_ISSUE_GUESTS_STUCK) == 0 || currentTicks - ride.lastIssueTime > 3000)
    {
        ride.currentIssues |= RIDE_ISSUE_GUESTS_STUCK;
        ride.lastIssueTime = currentTicks;

        auto ft = Formatter();
        ride.formatNameTo(ft);
        if (Config::Get().notifications.rideWarnings)
        {
            News::AddItemToQueue(News::ItemType::ride, STR_GUESTS_GETTING_STUCK_ON_RIDE, guest.CurrentRide.ToUnderlying(), ft);
        }
    }
}

uint8_t Guest::GetWaypointedSeatLocation(const Ride& ride, const CarEntry* vehicle_type, uint8_t track_direction) const
{
    // The seatlocation can be split into segments around the ride base
    // to decide the segment first split off the segmentable seat location
    // from the fixed section
    uint8_t seatLocationSegment = CurrentSeat & 0x7;
    uint8_t seatLocationFixed = CurrentSeat & 0xF8;

    // Enterprise has more segments (8) compared to the normal (4)
    if (ride.getRideTypeDescriptor().specialType != RtdSpecialType::enterprise)
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
    const auto& station = ride.getStation(CurrentRideStation);
    if (station.Entrance.IsNull())
    {
        return;
    }
    uint8_t direction_entrance = station.Entrance.direction;

    TileElement* tile_element = RideGetStationStartTrackElement(ride, CurrentRideStation);

    uint8_t direction_track = (tile_element == nullptr ? 0 : tile_element->GetDirection());

    auto vehicle = getGameState().entities.GetEntity<Vehicle>(ride.vehicles[CurrentTrain]);
    if (vehicle == nullptr)
    {
        // TODO: Goto ride exit on failure.
        return;
    }
    const auto* rideEntry = vehicle->GetRideEntry();
    const auto* carEntry = &rideEntry->Cars[vehicle->vehicle_type];

    Var37 = (direction_entrance | GetWaypointedSeatLocation(ride, carEntry, direction_track) * 4) * 4;

    const auto& rtd = ride.getRideTypeDescriptor();
    CoordsXY waypoint = rtd.GetGuestWaypointLocation(*vehicle, ride, CurrentRideStation);

    const auto waypointIndex = Var37 / 4u;
    if (waypointIndex < carEntry->peep_loading_waypoints.size())
    {
        Guard::Assert(carEntry->peep_loading_waypoints.size() >= static_cast<size_t>(waypointIndex));
        waypoint.x += carEntry->peep_loading_waypoints[waypointIndex][0].x;
        waypoint.y += carEntry->peep_loading_waypoints[waypointIndex][0].y;
    }

    SetDestination(waypoint);
    RideSubState = PeepRideSubState::approachVehicleWaypoints;
}

/**
 *
 *  rct2: 0x006921D3
 */
void Guest::UpdateRideAdvanceThroughEntrance()
{
    auto ride = GetRide(CurrentRide);
    if (ride == nullptr)
        return;

    int16_t actionZ, xy_distance;

    const auto* rideEntry = ride->getRideEntry();

    if (auto loc = UpdateAction(xy_distance); loc.has_value())
    {
        uint16_t distanceThreshold = 16;
        if (rideEntry != nullptr)
        {
            uint8_t vehicle = rideEntry->DefaultCar;
            if (rideEntry->Cars[vehicle].flags.hasAny(
                    CarEntryFlag::isMiniGolf, CarEntryFlag::isChairlift, CarEntryFlag::isGoKart))
            {
                distanceThreshold = 28;
            }
        }

        if (RideSubState == PeepRideSubState::inEntrance && xy_distance < distanceThreshold)
        {
            RideSubState = PeepRideSubState::freeVehicleCheck;
        }

        actionZ = ride->getStation(CurrentRideStation).GetBaseZ();

        distanceThreshold += 4;
        if (xy_distance < distanceThreshold)
        {
            actionZ += ride->getRideTypeDescriptor().Heights.PlatformHeight;
        }

        MoveTo({ loc.value(), actionZ });
        return;
    }

    if (RideSubState == PeepRideSubState::inEntrance)
    {
        RideSubState = PeepRideSubState::freeVehicleCheck;
        return;
    }

    if (ride->getRideTypeDescriptor().HasFlag(RtdFlag::noVehicles))
    {
        const auto& station = ride->getStation(CurrentRideStation);
        auto entranceLocation = station.Entrance.ToCoordsXYZD();
        if (entranceLocation.IsNull())
        {
            return;
        }

        const auto& rtd = GetRideTypeDescriptor(ride->type);
        rtd.UpdateLeaveEntrance(*this, *ride, entranceLocation);
        return;
    }

    Vehicle* vehicle = getGameState().entities.GetEntity<Vehicle>(ride->vehicles[CurrentTrain]);
    if (vehicle == nullptr)
    {
        return;
    }

    vehicle = vehicle->GetCar(CurrentCar);
    if (vehicle == nullptr)
    {
        return;
    }

    rideEntry = vehicle->GetRideEntry();
    if (rideEntry == nullptr)
    {
        return;
    }

    const auto* vehicle_type = &rideEntry->Cars[vehicle->vehicle_type];

    if (vehicle_type->flags.has(CarEntryFlag::loadingWaypoints))
    {
        UpdateRideLeaveEntranceWaypoints(*ride);
        return;
    }

    if (vehicle_type->flags.has(CarEntryFlag::useDodgemCarPlacement))
    {
        SetDestination(vehicle->GetLocation(), 15);
        RideSubState = PeepRideSubState::approachVehicle;
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
    auto loadPositionWithReversal = (vehicle->HasFlag(VehicleFlags::CarIsReversed)) ? -load_position : load_position;
    switch (vehicle->Orientation / 8)
    {
        case 0:
            destination.x = vehicle->x - loadPositionWithReversal;
            break;
        case 1:
            destination.y = vehicle->y + loadPositionWithReversal;
            break;
        case 2:
            destination.x = vehicle->x + loadPositionWithReversal;
            break;
        case 3:
            destination.y = vehicle->y - loadPositionWithReversal;
            break;
    }
    SetDestination(destination);

    RideSubState = PeepRideSubState::approachVehicle;
}

/**
 *
 *  rct2: 0x0069321D
 */
static void PeepGoToRideExit(Guest& guest, const Ride& ride, int16_t x, int16_t y, int16_t z, uint8_t exit_direction)
{
    z += ride.getRideTypeDescriptor().Heights.PlatformHeight;

    guest.MoveTo({ x, y, z });

    Guard::Assert(guest.CurrentRideStation.ToUnderlying() < Limits::kMaxStationsPerRide);
    auto exit = ride.getStation(guest.CurrentRideStation).Exit;
    x = exit.x;
    y = exit.y;
    x *= 32;
    y *= 32;
    x += 16;
    y += 16;

    auto [x_shift, y_shift] = [exit_direction]() {
        if (exit_direction < DirectionOffsets.size())
        {
            return std::pair(DirectionOffsets[exit_direction].x, DirectionOffsets[exit_direction].y);
        }
        else
        {
            return std::pair(0, 0);
        }
    }();

    int16_t shift_multiplier = 20;

    const auto* rideEntry = GetRideEntryByIndex(ride.subtype);
    if (rideEntry != nullptr)
    {
        const CarEntry& carEntry = rideEntry->Cars[rideEntry->DefaultCar];
        if (carEntry.flags.hasAny(CarEntryFlag::isMiniGolf, CarEntryFlag::isChairlift, CarEntryFlag::isGoKart))
        {
            shift_multiplier = 32;
        }
    }

    x_shift *= shift_multiplier;
    y_shift *= shift_multiplier;

    x -= x_shift;
    y -= y_shift;

    guest.SetDestination({ x, y }, 2);

    guest.Orientation = exit_direction * 8;
    guest.RideSubState = PeepRideSubState::approachExit;
}

/**
 *
 *  rct2: 0x006920B4
 */
void Guest::UpdateRideFreeVehicleEnterRide(Ride& ride)
{
    auto ridePrice = RideGetPrice(ride);
    if (ridePrice != 0)
    {
        if ((HasItem(ShopItem::voucher)) && (VoucherType == VOUCHER_TYPE_RIDE_FREE) && (VoucherRideId == CurrentRide))
        {
            RemoveItem(ShopItem::voucher);
            WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
        }
        else
        {
            ride.totalProfit = AddClamp<money64>(ride.totalProfit, ridePrice);
            ride.windowInvalidateFlags.set(RideInvalidateFlag::income);
            SpendMoney(PaidOnRides, ridePrice, ExpenditureType::parkRideTickets);
        }
    }

    RideSubState = PeepRideSubState::leaveEntrance;
    uint8_t queueTime = DaysInQueue;
    if (queueTime < 253)
        queueTime += 3;

    queueTime /= 2;
    auto& station = ride.getStation(CurrentRideStation);
    if (queueTime != station.QueueTime)
    {
        station.QueueTime = queueTime;
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByNumber(WindowClass::ride, CurrentRide.ToUnderlying());
    }

    if (PeepFlags & PEEP_FLAGS_TRACKING)
    {
        auto ft = Formatter();
        FormatNameTo(ft);
        ride.formatNameTo(ft);

        StringId msg_string;
        if (ride.getRideTypeDescriptor().HasFlag(RtdFlag::describeAsInside))
            msg_string = STR_PEEP_TRACKING_PEEP_IS_IN_X;
        else
            msg_string = STR_PEEP_TRACKING_PEEP_IS_ON_X;

        if (Config::Get().notifications.guestOnRide)
        {
            News::AddItemToQueue(News::ItemType::peepOnRide, msg_string, Id, ft);
        }
    }

    const auto& rtd = ride.getRideTypeDescriptor();
    if (rtd.specialType == RtdSpecialType::spiralSlide)
    {
        SwitchToSpecialSprite(1);
    }

    UpdateRideAdvanceThroughEntrance();
}

/**
 *
 *  rct2: 0x00691FD4
 */
static void PeepUpdateRideNoFreeVehicleRejoinQueue(Guest& guest, Ride& ride)
{
    TileCoordsXYZD entranceLocation = ride.getStation(guest.CurrentRideStation).Entrance;

    int32_t x = entranceLocation.x * 32;
    int32_t y = entranceLocation.y * 32;
    if (entranceLocation.direction < DirectionOffsets.size())
    {
        x += 16 - DirectionOffsets[entranceLocation.direction].x * 20;
        y += 16 - DirectionOffsets[entranceLocation.direction].y * 20;
    }

    guest.SetDestination({ x, y }, 2);
    guest.SetState(PeepState::queuingFront);
    guest.RideSubState = PeepRideSubState::atEntrance;

    ride.queueInsertGuestAtFront(guest.CurrentRideStation, &guest);
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
    auto ride = GetRide(CurrentRide);
    if (ride == nullptr)
        return;

    if (ride->getRideTypeDescriptor().HasFlag(RtdFlag::noVehicles))
    {
        if (ride->status != RideStatus::open || ride->vehicleChangeTimeout != 0 || (++RejoinQueueTimeout) == 0)
        {
            PeepUpdateRideNoFreeVehicleRejoinQueue(*this, *ride);
            return;
        }

        UpdateRideFreeVehicleEnterRide(*ride);
        return;
    }

    auto& gameState = getGameState();
    Vehicle* vehicle = gameState.entities.GetEntity<Vehicle>(ride->vehicles[CurrentTrain]);
    if (vehicle == nullptr)
    {
        // TODO: Leave ride on failure goes for all returns on nullptr in this function
        return;
    }
    vehicle = vehicle->GetCar(CurrentCar);
    if (vehicle == nullptr)
        return;

    const auto* rideEntry = vehicle->GetRideEntry();
    if (rideEntry == nullptr)
    {
        return;
    }

    if (rideEntry->Cars[0].flags.has(CarEntryFlag::isMiniGolf))
    {
        vehicle->mini_golf_flags &= ~MiniGolfFlag::Flag5;

        for (size_t i = 0; i < ride->numTrains; ++i)
        {
            Vehicle* train = gameState.entities.GetEntity<Vehicle>(ride->vehicles[i]);
            if (train == nullptr)
                continue;

            Vehicle* second_vehicle = gameState.entities.GetEntity<Vehicle>(train->next_vehicle_on_train);
            if (second_vehicle == nullptr)
                continue;

            if (second_vehicle->num_peeps == 0)
                continue;

            if (second_vehicle->mini_golf_flags & MiniGolfFlag::Flag5)
                continue;

            return;
        }
    }

    if (!vehicle->IsUsedInPairs())
    {
        UpdateRideFreeVehicleEnterRide(*ride);
        return;
    }

    if (ride->mode == RideMode::forwardRotation || ride->mode == RideMode::backwardRotation)
    {
        if (CurrentSeat & 1 || !(vehicle->next_free_seat & 1))
        {
            UpdateRideFreeVehicleEnterRide(*ride);
            return;
        }
    }
    else
    {
        uint8_t seat = CurrentSeat | 1;
        if (seat < vehicle->next_free_seat)
        {
            UpdateRideFreeVehicleEnterRide(*ride);
            return;
        }
    }

    Vehicle* currentTrain = gameState.entities.GetEntity<Vehicle>(ride->vehicles[CurrentTrain]);
    if (currentTrain == nullptr)
    {
        return;
    }
    if (ride->status == RideStatus::open && ++RejoinQueueTimeout != 0 && !currentTrain->HasFlag(VehicleFlags::ReadyToDepart))
    {
        return;
    }

    if (ride->mode != RideMode::forwardRotation && ride->mode != RideMode::backwardRotation)
    {
        if (vehicle->next_free_seat - 1 != CurrentSeat)
            return;
    }

    vehicle->next_free_seat--;
    vehicle->peep[CurrentSeat] = EntityId::GetNull();

    PeepUpdateRideNoFreeVehicleRejoinQueue(*this, *ride);
}

void Guest::UpdateRideApproachVehicle()
{
    if (auto loc = UpdateAction(); loc.has_value())
    {
        MoveTo({ loc.value(), z });
        return;
    }
    RideSubState = PeepRideSubState::enterVehicle;
}

void Guest::UpdateRideEnterVehicle()
{
    auto& gameState = getGameState();
    auto* ride = GetRide(CurrentRide);
    if (ride != nullptr)
    {
        auto* vehicle = gameState.entities.GetEntity<Vehicle>(ride->vehicles[CurrentTrain]);
        if (vehicle != nullptr)
        {
            vehicle = vehicle->GetCar(CurrentCar);
            if (vehicle == nullptr)
            {
                return;
            }

            if (ride->mode != RideMode::forwardRotation && ride->mode != RideMode::backwardRotation)
            {
                if (CurrentSeat != vehicle->num_peeps)
                    return;
            }

            if (vehicle->IsUsedInPairs())
            {
                auto* seatedGuest = gameState.entities.GetEntity<Guest>(vehicle->peep[CurrentSeat ^ 1]);
                if (seatedGuest != nullptr)
                {
                    if (seatedGuest->RideSubState != PeepRideSubState::enterVehicle)
                        return;

                    vehicle->num_peeps++;
                    ride->curNumCustomers++;

                    vehicle->ApplyMass(seatedGuest->Mass);
                    seatedGuest->MoveTo({ kLocationNull, 0, 0 });
                    seatedGuest->SetState(PeepState::onRide);
                    seatedGuest->GuestTimeOnRide = 0;
                    seatedGuest->RideSubState = PeepRideSubState::onRide;
                    seatedGuest->OnEnterRide(*ride);
                }
            }

            vehicle->num_peeps++;
            ride->curNumCustomers++;

            vehicle->ApplyMass(Mass);
            vehicle->Invalidate();

            MoveTo({ kLocationNull, 0, 0 });

            SetState(PeepState::onRide);

            GuestTimeOnRide = 0;
            RideSubState = PeepRideSubState::onRide;
            OnEnterRide(*ride);
        }
    }
}

/**
 *
 *  rct2: 0x00693028
 */
void Guest::UpdateRideLeaveVehicle()
{
    auto& gameState = getGameState();
    auto ride = GetRide(CurrentRide);
    if (ride == nullptr)
        return;

    Vehicle* vehicle = gameState.entities.GetEntity<Vehicle>(ride->vehicles[CurrentTrain]);
    if (vehicle == nullptr)
        return;

    StationIndex ride_station = vehicle->current_station;
    vehicle = vehicle->GetCar(CurrentCar);
    if (vehicle == nullptr)
    {
        return;
    }

    // Check if ride is NOT Ferris Wheel.
    if (ride->mode != RideMode::forwardRotation && ride->mode != RideMode::backwardRotation)
    {
        if (vehicle->num_peeps - 1 != CurrentSeat)
            return;
    }

    AnimationImageIdOffset++;
    if (AnimationImageIdOffset & 3)
        return;

    AnimationImageIdOffset = 0;

    vehicle->num_peeps--;
    vehicle->ApplyMass(-Mass);
    vehicle->Invalidate();

    if (ride_station.ToUnderlying() >= Limits::kMaxStationsPerRide)
    {
        // HACK #5658: Some parks have hacked rides which end up in this state
        auto bestStationIndex = RideGetFirstValidStationExit(*ride);
        if (bestStationIndex.IsNull())
        {
            bestStationIndex = StationIndex::FromUnderlying(0);
        }
        ride_station = bestStationIndex;
    }
    CurrentRideStation = ride_station;
    const auto* rideEntry = vehicle->GetRideEntry();
    if (rideEntry == nullptr)
    {
        return;
    }

    const auto* carEntry = &rideEntry->Cars[vehicle->vehicle_type];

    assert(CurrentRideStation.ToUnderlying() < Limits::kMaxStationsPerRide);
    auto& station = ride->getStation(CurrentRideStation);

    if (!carEntry->flags.has(CarEntryFlag::loadingWaypoints))
    {
        TileCoordsXYZD exitLocation = station.Exit;
        CoordsXYZD platformLocation;
        platformLocation.z = station.GetBaseZ();

        platformLocation.direction = DirectionReverse(exitLocation.direction);

        if (!ride->getRideTypeDescriptor().HasFlag(RtdFlag::vehicleIsIntegral))
        {
            for (; vehicle != nullptr && !vehicle->IsHead();
                 vehicle = gameState.entities.GetEntity<Vehicle>(vehicle->prev_vehicle_on_ride))
            {
                auto trackType = vehicle->GetTrackType();
                if (trackType == TrackElemType::flat || trackType > TrackElemType::middleStation)
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

            rideEntry = GetRideEntryByIndex(ride->subtype);

            if (rideEntry != nullptr)
            {
                carEntry = &rideEntry->Cars[rideEntry->DefaultCar];

                if (carEntry->flags.has(CarEntryFlag::isGoKart))
                {
                    shiftMultiplier = 9;
                }

                if (carEntry->flags.hasAny(CarEntryFlag::isChairlift, CarEntryFlag::isGoKart))
                {
                    specialDirection = ((vehicle->Orientation + 3) / 8) + 1;
                    specialDirection &= 3;

                    if (vehicle->TrackSubposition == VehicleTrackSubposition::GoKartsRightLane)
                        specialDirection = DirectionReverse(specialDirection);
                }
            }

            auto [xShift, yShift] = [specialDirection]() {
                if (specialDirection < DirectionOffsets.size())
                {
                    return std::pair(DirectionOffsets[specialDirection].x, DirectionOffsets[specialDirection].y);
                }
                else
                {
                    return std::pair(0, 0);
                }
            }();

            platformLocation.x = vehicle->x + xShift * shiftMultiplier;
            platformLocation.y = vehicle->y + yShift * shiftMultiplier;

            PeepGoToRideExit(
                *this, *ride, platformLocation.x, platformLocation.y, platformLocation.z, platformLocation.direction);
            return;
        }

        platformLocation.x = vehicle->x + DirectionOffsets[platformLocation.direction].x * 12;
        platformLocation.y = vehicle->y + DirectionOffsets[platformLocation.direction].y * 12;

        // This can evaluate to false with buggy custom rides.
        if (CurrentSeat < carEntry->peep_loading_positions.size())
        {
            int8_t loadPosition = carEntry->peep_loading_positions[CurrentSeat];

            switch (vehicle->Orientation / 8)
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
            LOG_VERBOSE(
                "CurrentSeat %d is too large! (Vehicle entry has room for %d.)", CurrentSeat,
                carEntry->peep_loading_positions.size());
        }

        platformLocation.z = station.GetBaseZ();

        PeepGoToRideExit(*this, *ride, platformLocation.x, platformLocation.y, platformLocation.z, platformLocation.direction);
        return;
    }

    auto exitLocation = station.Exit.ToCoordsXYZD();
    if (exitLocation.IsNull())
    {
        return;
    }

    TileElement* trackElement = RideGetStationStartTrackElement(*ride, CurrentRideStation);

    Direction station_direction = (trackElement == nullptr ? 0 : trackElement->GetDirection());

    vehicle = gameState.entities.GetEntity<Vehicle>(ride->vehicles[CurrentTrain]);
    if (vehicle == nullptr)
    {
        return;
    }

    const auto& rtd = ride->getRideTypeDescriptor();
    CoordsXYZ waypointLoc = { rtd.GetGuestWaypointLocation(*vehicle, *ride, CurrentRideStation),
                              exitLocation.z + ride->getRideTypeDescriptor().Heights.PlatformHeight };

    rideEntry = vehicle->GetRideEntry();
    carEntry = &rideEntry->Cars[vehicle->vehicle_type];
    if (carEntry == nullptr)
        return;

    Var37 = ((exitLocation.direction | GetWaypointedSeatLocation(*ride, carEntry, station_direction) * 4) * 4) | 1;

    CoordsXYZ exitWaypointLoc = waypointLoc;

    const auto waypointIndex = Var37 / 4u;
    if (waypointIndex < carEntry->peep_loading_waypoints.size())
    {
        exitWaypointLoc.x += carEntry->peep_loading_waypoints[waypointIndex][2].x;
        exitWaypointLoc.y += carEntry->peep_loading_waypoints[waypointIndex][2].y;
    }

    if (ride->getRideTypeDescriptor().specialType == RtdSpecialType::motionSimulator)
        exitWaypointLoc.z += 15;

    MoveTo(exitWaypointLoc);

    if (waypointIndex < carEntry->peep_loading_waypoints.size())
    {
        waypointLoc.x += carEntry->peep_loading_waypoints[waypointIndex][1].x;
        waypointLoc.y += carEntry->peep_loading_waypoints[waypointIndex][1].y;
    }

    SetDestination(waypointLoc, 2);
    RideSubState = PeepRideSubState::approachExitWaypoints;
}

/**
 *
 *  rct2: 0x0069376A
 */
void Guest::UpdateRidePrepareForExit()
{
    const auto* ride = GetRide(CurrentRide);
    if (ride == nullptr || CurrentRideStation.ToUnderlying() >= std::size(ride->getStations()))
        return;

    auto exit = ride->getStation(CurrentRideStation).Exit;
    auto newDestination = exit.ToCoordsXY().ToTileCentre();

    auto [xShift, yShift] = [exit]() {
        if (exit.direction < DirectionOffsets.size())
        {
            return std::pair(DirectionOffsets[exit.direction].x, DirectionOffsets[exit.direction].y);
        }
        else
        {
            return std::pair(0, 0);
        }
    }();

    int16_t shiftMultiplier = 20;

    const auto* rideEntry = ride->getRideEntry();
    if (rideEntry != nullptr)
    {
        const auto& carEntry = rideEntry->Cars[rideEntry->DefaultCar];
        if (carEntry.flags.hasAny(CarEntryFlag::isChairlift, CarEntryFlag::isGoKart))
        {
            shiftMultiplier = 32;
        }
    }

    xShift *= shiftMultiplier;
    yShift *= shiftMultiplier;

    newDestination.x -= xShift;
    newDestination.y -= yShift;

    SetDestination(newDestination, 2);
    RideSubState = PeepRideSubState::inExit;
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
    auto ride = GetRide(CurrentRide);
    if (ride == nullptr)
        return;

    int16_t xy_distance;

    if (auto loc = UpdateAction(xy_distance); loc.has_value())
    {
        if (xy_distance >= 16)
        {
            int16_t actionZ = ride->getStation(CurrentRideStation).GetBaseZ();

            actionZ += ride->getRideTypeDescriptor().Heights.PlatformHeight;
            MoveTo({ loc.value(), actionZ });
            return;
        }

        SwitchToSpecialSprite(0);
        MoveTo({ loc.value(), z });
    }

    if (ride->lifecycleFlags & RIDE_LIFECYCLE_ON_RIDE_PHOTO)
    {
        ShopItem secondaryItem = ride->getRideTypeDescriptor().PhotoItem;
        if (GuestDecideAndBuyItem(*this, *ride, secondaryItem, ride->price[1]))
        {
            ride->numSecondaryItemsSold = AddClamp(ride->numSecondaryItemsSold, 1u);
        }
    }
    RideSubState = PeepRideSubState::leaveExit;
}
#pragma warning(default : 6011)

CoordsXY GetGuestWaypointLocationDefault(const Vehicle& vehicle, const Ride& ride, const StationIndex& CurrentRideStation)
{
    return ride.getStation(CurrentRideStation).Start.ToTileCentre();
}

CoordsXY GetGuestWaypointLocationEnterprise(const Vehicle& vehicle, const Ride& ride, const StationIndex& CurrentRideStation)
{
    return { vehicle.x, vehicle.y };
}

/**
 *
 *  rct2: 0x006926AD
 */
void Guest::UpdateRideApproachVehicleWaypoints()
{
    auto ride = GetRide(CurrentRide);
    if (ride == nullptr)
        return;

    int16_t xy_distance;
    uint8_t waypoint = Var37 & 3;

    const auto& rtd = ride->getRideTypeDescriptor();
    if (auto loc = UpdateAction(xy_distance); loc.has_value())
    {
        rtd.UpdateRideApproachVehicleWaypoints(*this, loc.value(), xy_distance);
        return;
    }

    if (waypoint == 2)
    {
        RideSubState = PeepRideSubState::enterVehicle;
        return;
    }

    waypoint++;
    // This is incrementing the actual peep waypoint
    Var37++;

    Vehicle* vehicle = getGameState().entities.GetEntity<Vehicle>(ride->vehicles[CurrentTrain]);
    if (vehicle == nullptr)
    {
        return;
    }

    CoordsXY targetLoc = rtd.GetGuestWaypointLocation(*vehicle, *ride, CurrentRideStation);

    const auto* rideEntry = vehicle->GetRideEntry();
    if (rideEntry == nullptr)
    {
        return;
    }

    const auto& vehicle_type = rideEntry->Cars[vehicle->vehicle_type];
    const auto waypointIndex = Var37 / 4u;
    if (waypointIndex < vehicle_type.peep_loading_waypoints.size())
    {
        Guard::Assert(waypoint < 3);
        targetLoc.x += vehicle_type.peep_loading_waypoints[waypointIndex][waypoint].x;
        targetLoc.y += vehicle_type.peep_loading_waypoints[waypointIndex][waypoint].y;
    }

    SetDestination(targetLoc);
}

void UpdateRideApproachVehicleWaypointsMotionSimulator(Guest& guest, const CoordsXY& loc, int16_t& xy_distance)
{
    auto ride = GetRide(guest.CurrentRide);
    // Motion simulators have steps. This moves the peeps up the steps.
    int16_t actionZ = ride->getStation(guest.CurrentRideStation).GetBaseZ() + 2;

    uint8_t waypoint = guest.Var37 & 3;
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
    guest.MoveTo({ loc, actionZ });
}

void UpdateRideApproachVehicleWaypointsDefault(Guest& guest, const CoordsXY& loc, int16_t& xy_distance)
{
    guest.MoveTo({ loc, guest.z });
}

/**
 *
 *  rct2: 0x0069357D
 */
void Guest::UpdateRideApproachExitWaypoints()
{
    auto ride = GetRide(CurrentRide);
    if (ride == nullptr)
        return;

    int16_t xy_distance;

    if (auto loc = UpdateAction(xy_distance); loc.has_value())
    {
        int16_t actionZ;

        if (ride->getRideTypeDescriptor().specialType == RtdSpecialType::motionSimulator)
        {
            actionZ = ride->getStation(CurrentRideStation).GetBaseZ() + 2;

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
        Vehicle* vehicle = getGameState().entities.GetEntity<Vehicle>(ride->vehicles[CurrentTrain]);
        if (vehicle == nullptr)
        {
            return;
        }

        const auto* rideEntry = vehicle->GetRideEntry();
        if (rideEntry == nullptr)
            return;

        if (vehicle->vehicle_type >= std::size(rideEntry->Cars))
            return;

        const CarEntry& carEntry = rideEntry->Cars[vehicle->vehicle_type];

        const size_t carPosition = Var37 / 4;
        if (carPosition >= carEntry.peep_loading_waypoints.size())
            return;

        const auto waypoint = Var37 & 3;
        Guard::Assert(waypoint < 3);

        const auto& rtd = ride->getRideTypeDescriptor();

        CoordsXY targetLoc = rtd.GetGuestWaypointLocation(*vehicle, *ride, CurrentRideStation);
        targetLoc += carEntry.peep_loading_waypoints[carPosition][waypoint];
        SetDestination(targetLoc);
        return;
    }

    Var37 |= 3;

    auto targetLoc = ride->getStation(CurrentRideStation).Exit.ToCoordsXYZD().ToTileCentre();
    uint8_t exit_direction = DirectionReverse(targetLoc.direction);

    int16_t x_shift = DirectionOffsets[exit_direction].x;
    int16_t y_shift = DirectionOffsets[exit_direction].y;

    int16_t shift_multiplier = 20;

    auto rideEntry = GetRideEntryByIndex(ride->subtype);
    if (rideEntry != nullptr)
    {
        auto carEntry = &rideEntry->Cars[rideEntry->DefaultCar];
        if (carEntry->flags.hasAny(CarEntryFlag::isChairlift, CarEntryFlag::isGoKart))
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
    auto ride = GetRide(CurrentRide);
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
        spiralSlideSubstate = PeepSpiralSlideSubState::goingUp;
        spiralSlideGoingUpTimer = 0;
        Var37 = (Var37 / 4) & 0xC;
        MoveTo({ kLocationNull, y, z });
        return;
    }

    [[maybe_unused]] const auto& rtd = ride->getRideTypeDescriptor();
    if (waypoint == 2)
    {
        bool lastRide = false;
        if (ride->status != RideStatus::open)
            lastRide = true;
        else if (timesSlidDown++ != 0)
        {
            if (ride->mode == RideMode::singleRidePerAdmission)
                lastRide = true;
            if (static_cast<uint8_t>(timesSlidDown - 1) > (ScenarioRand() & 0xF))
                lastRide = true;
        }

        if (lastRide)
        {
            auto exit = ride->getStation(CurrentRideStation).Exit;
            waypoint = 1;
            auto directionTemp = exit.direction;
            if (exit.direction == kInvalidDirection)
            {
                directionTemp = 0;
            }
            Var37 = (directionTemp * 4) | (Var37 & 0x30) | waypoint;
            CoordsXY targetLoc = ride->getStation(CurrentRideStation).Start;

            assert(rtd.specialType == RtdSpecialType::spiralSlide);
            targetLoc += kSpiralSlideWalkingPath[Var37];

            SetDestination(targetLoc);
            RideSubState = PeepRideSubState::leaveSpiralSlide;
            return;
        }
    }
    waypoint++;
    // Actually increment the real peep waypoint
    Var37++;

    CoordsXY targetLoc = ride->getStation(CurrentRideStation).Start;

    assert(rtd.specialType == RtdSpecialType::spiralSlide);
    targetLoc += kSpiralSlideWalkingPath[Var37];

    SetDestination(targetLoc);
}

/** rct2: 0x00981F0C, 0x00981F0E */
static constexpr CoordsXY kSpiralSlideEnd[] = {
    { 25, 56 },
    { 56, 7 },
    { 7, -24 },
    { -24, 25 },
};

/** rct2: 0x00981F1C, 0x00981F1E */
static constexpr CoordsXY kSpiralSlideEndWaypoint[] = {
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
    auto ride = GetRide(CurrentRide);

    if (ride == nullptr)
        return;

    const auto& rtd = ride->getRideTypeDescriptor();
    if (rtd.specialType != RtdSpecialType::spiralSlide)
        return;

    if ((Var37 & 3) == 0)
    {
        switch (spiralSlideSubstate)
        {
            case PeepSpiralSlideSubState::goingUp:
                spiralSlideGoingUpTimer++;
                if (spiralSlideGoingUpTimer >= kTicksToGoUpSpiralSlide)
                    spiralSlideSubstate = PeepSpiralSlideSubState::prepareToSlide;

                return;
            case PeepSpiralSlideSubState::prepareToSlide:
                if (ride->slideInUse || ride->lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN)
                    return;

                ride->slideInUse = 1;
                ride->slidePeep = Id;
                ride->slidePeepTShirtColour = TshirtColour;
                ride->spiralSlideProgress = 0;
                spiralSlideSubstate = PeepSpiralSlideSubState::slidingDown;

                return;
            case PeepSpiralSlideSubState::finishedSliding:
            {
                auto newLocation = ride->getStation(CurrentRideStation).Start;
                uint8_t dir = (Var37 / 4) & 3;

                // Set the location that the guest walks to go on slide again
                auto destination = newLocation + kSpiralSlideEndWaypoint[dir];
                SetDestination(destination);

                // Move the guest sprite to just at the end of the slide
                newLocation.x += kSpiralSlideEnd[dir].x;
                newLocation.y += kSpiralSlideEnd[dir].y;

                MoveTo({ newLocation, z });

                Orientation = (Var37 & 0xC) * 2;

                Var37++;
                return;
            }
            case PeepSpiralSlideSubState::slidingDown: // Handled by updateSpiralSlide in Ride.cpp
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

    CoordsXY targetLoc = ride->getStation(CurrentRideStation).Start;

    targetLoc += kSpiralSlideWalkingPath[Var37];

    SetDestination(targetLoc);
    RideSubState = PeepRideSubState::approachSpiralSlide;
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

    auto ride = GetRide(CurrentRide);
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
        CoordsXY targetLoc = ride->getStation(CurrentRideStation).Start;

        [[maybe_unused]] const auto& rtd = ride->getRideTypeDescriptor();
        assert(rtd.specialType == RtdSpecialType::spiralSlide);
        targetLoc += kSpiralSlideWalkingPath[Var37];

        SetDestination(targetLoc);
        return;
    }

    // Actually force the final waypoint
    Var37 |= 3;

    auto targetLoc = ride->getStation(CurrentRideStation).Exit.ToCoordsXYZD().ToTileCentre();

    int16_t xShift = DirectionOffsets[DirectionReverse(targetLoc.direction)].x;
    int16_t yShift = DirectionOffsets[DirectionReverse(targetLoc.direction)].y;

    int16_t shiftMultiplier = 20;

    xShift *= shiftMultiplier;
    yShift *= shiftMultiplier;

    targetLoc.x -= xShift;
    targetLoc.y -= yShift;

    SetDestination(targetLoc);
}

/** rct2: 0x00981FE4 */
static constexpr uint8_t kMazeGetNewDirectionFromEdge[][4] = {
    { 15, 7, 15, 7 },
    { 11, 3, 11, 3 },
    { 7, 15, 7, 15 },
    { 3, 11, 3, 11 },
};

/** rct2: 0x00981FF4 */
static constexpr uint8_t kMazeCurrentDirectionToOpenHedge[][4] = {
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

    auto ride = GetRide(CurrentRide);
    if (ride == nullptr)
        return;

    if (Var37 == 16)
    {
        UpdateRidePrepareForExit();
        return;
    }

    if (IsActionInterruptable())
    {
        if (Energy > 80 && !(PeepFlags & PEEP_FLAGS_SLOW_WALK) && !ClimateIsPrecipitating()
            && (ScenarioRand() & 0xFFFF) <= 2427)
        {
            Action = PeepActionType::jump;
            AnimationFrameNum = 0;
            AnimationImageIdOffset = 0;
            UpdateCurrentAnimationType();
        }
    }

    auto targetLoc = GetDestination().ToTileStart();

    auto stationBaseZ = ride->getStation().GetBaseZ();

    // Find the station track element
    auto trackElement = MapGetTrackElementAt({ targetLoc, stationBaseZ });
    if (trackElement == nullptr)
    {
        return;
    }

    uint16_t mazeEntry = trackElement->GetMazeEntry();
    // Var37 is 3, 7, 11 or 15
    uint8_t hedges[4]{ 0xFF, 0xFF, 0xFF, 0xFF };
    uint8_t openCount = 0;
    uint8_t mazeReverseLastEdge = DirectionReverse(MazeLastEdge);
    for (uint8_t i = 0; i < kNumOrthogonalDirections; ++i)
    {
        if (!(mazeEntry & (1 << kMazeCurrentDirectionToOpenHedge[Var37 / 4][i])) && i != mazeReverseLastEdge)
        {
            hedges[openCount++] = i;
        }
    }

    if (openCount == 0)
    {
        if ((mazeEntry & (1 << kMazeCurrentDirectionToOpenHedge[Var37 / 4][mazeReverseLastEdge])))
        {
            return;
        }
        hedges[openCount++] = mazeReverseLastEdge;
    }

    uint8_t chosenEdge = hedges[ScenarioRand() % openCount];
    assert(chosenEdge != 0xFF);

    targetLoc = GetDestination() + CoordsDirectionDelta[chosenEdge] / 2;

    enum class maze_type
    {
        invalid,
        hedge,
        entrance_or_exit
    };
    maze_type mazeType = maze_type::invalid;

    auto tileElement = MapGetFirstElementAt(targetLoc);
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
            Var37 = kMazeGetNewDirectionFromEdge[Var37 / 4][chosenEdge];
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
    }
}

/**
 *
 *  rct2: 0x006938D2
 */
void Guest::UpdateRideLeaveExit()
{
    auto ride = GetRide(CurrentRide);

    if (auto loc = UpdateAction(); loc.has_value())
    {
        if (ride != nullptr)
        {
            MoveTo({ loc.value(), ride->getStation(CurrentRideStation).GetBaseZ() });
        }
        return;
    }

    if (ride != nullptr)
    {
        OnExitRide(*ride);

        if (PeepFlags & PEEP_FLAGS_TRACKING)
        {
            auto ft = Formatter();
            FormatNameTo(ft);
            ride->formatNameTo(ft);

            if (Config::Get().notifications.guestLeftRide)
            {
                News::AddItemToQueue(News::ItemType::peepOnRide, STR_PEEP_TRACKING_LEFT_RIDE_X, Id, ft);
            }
        }
    }

    InteractionRideIndex = RideId::GetNull();
    SetState(PeepState::falling);

    CoordsXY targetLoc = { x, y };

    // Find the station track element
    for (auto* pathElement : TileElementsView<PathElement>(targetLoc))
    {
        int16_t height = MapHeightFromSlope(targetLoc, pathElement->GetSlopeDirection(), pathElement->IsSloped());
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

    RideSubState = PeepRideSubState::interactShop;
}

/**
 *
 *  rct2: 0x006929BB
 */
void Guest::UpdateRideShopInteract()
{
    auto ride = GetRide(CurrentRide);
    if (ride == nullptr)
        return;

    const int16_t tileCentreX = NextLoc.x + 16;
    const int16_t tileCentreY = NextLoc.y + 16;

    const auto& rtd = ride->getRideTypeDescriptor();
    if (rtd.specialType == RtdSpecialType::firstAid)
    {
        if (Nausea <= 35)
        {
            RideSubState = PeepRideSubState::leaveShop;

            SetDestination({ tileCentreX, tileCentreY }, 3);
            HappinessTarget = std::min(HappinessTarget + 30, kPeepMaxHappiness);
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
    if (gLegacyScene != LegacyScene::titleSequence)
    {
        Audio::Play3D(Audio::SoundId::toiletFlush, GetLocation());
    }

    RideSubState = PeepRideSubState::leaveShop;

    SetDestination({ tileCentreX, tileCentreY }, 3);

    HappinessTarget = std::min(HappinessTarget + 30, kPeepMaxHappiness);
    Happiness = HappinessTarget;
    GuestStopPurchaseThought(*this, ride->type);
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

    // #11758 Previously SetState(PeepState::walking) caused Peeps to double-back to exit point of shop
    SetState(PeepState::falling);

    auto ride = GetRide(CurrentRide);
    if (ride != nullptr)
    {
        ride->totalCustomers = AddClamp(ride->totalCustomers, 1u);
        ride->windowInvalidateFlags.set(RideInvalidateFlag::customers);
        ride->updateSatisfaction(Happiness / 64);
    }
}

/* From peep_update */
static void GuestUpdateThoughts(Guest& guest)
{
    // Thoughts must always have a gap of at least
    // 220 ticks in age between them. In order to
    // allow this when a thought is new it enters
    // a holding zone. Before it becomes fresh.
    int32_t add_fresh = 1;
    int32_t fresh_thought = -1;
    for (int32_t i = 0; i < kPeepMaxThoughts; i++)
    {
        if (guest.Thoughts[i].type == PeepThoughtType::None)
            break;

        if (guest.Thoughts[i].freshness == 1)
        {
            add_fresh = 0;
            // If thought is fresh we wait 220 ticks
            // before allowing a new thought to become fresh.
            if (++guest.Thoughts[i].fresh_timeout >= 220)
            {
                guest.Thoughts[i].fresh_timeout = 0;
                // Thought is no longer fresh
                guest.Thoughts[i].freshness++;
                add_fresh = 1;
            }
        }
        else if (guest.Thoughts[i].freshness > 1)
        {
            if (++guest.Thoughts[i].fresh_timeout == 0)
            {
                // When thought is older than ~6900 ticks remove it
                if (++guest.Thoughts[i].freshness >= 28)
                {
                    guest.WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_THOUGHTS;

                    // Clear top thought, push others up
                    if (i < kPeepMaxThoughts - 2)
                    {
                        memmove(&guest.Thoughts[i], &guest.Thoughts[i + 1], sizeof(PeepThought) * (kPeepMaxThoughts - i - 1));
                    }
                    guest.Thoughts[kPeepMaxThoughts - 1].type = PeepThoughtType::None;
                }
            }
        }
        else
        {
            fresh_thought = i;
        }
    }
    // If there are no fresh thoughts
    // a previously new thought can become
    // fresh.
    if (add_fresh && fresh_thought != -1)
    {
        guest.Thoughts[fresh_thought].freshness = 1;
        guest.WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_THOUGHTS;
    }
}

void Guest::Update()
{
    if (PeepFlags & PEEP_FLAGS_POSITION_FROZEN)
    {
        if (!(PeepFlags & PEEP_FLAGS_ANIMATION_FROZEN))
        {
            // This is circumventing other logic, so only update every few ticks
            if ((getGameState().currentTicks & 3) == 0)
            {
                if (IsActionWalking())
                    UpdateWalkingAnimation();
                else
                    UpdateActionAnimation();
                Invalidate();
            }
        }
        return;
    }
    else if (PeepFlags & PEEP_FLAGS_ANIMATION_FROZEN)
    {
        // Animation is frozen while position is not. This allows a peep to walk
        // around without its sprite being updated, which looks very glitchy.
        // We'll just remove the flag and continue as normal, in this case.
        PeepFlags &= ~PEEP_FLAGS_ANIMATION_FROZEN;
    }

    if (!PreviousRide.IsNull())
    {
        if (++PreviousRideTimeOut >= 720)
        {
            PreviousRide = RideId::GetNull();
        }
    }

    GuestUpdateThoughts(*this);

    // Walking speed logic
    const auto stepsToTake = GetStepsToTake();
    const auto carryCheck = StepProgress + stepsToTake;
    StepProgress = carryCheck;

    if (carryCheck <= 255)
    {
        UpdateEasterEggInteractions();
    }
    else
    {
        // Loc68FD2F
        switch (State)
        {
            case PeepState::falling:
                UpdateFalling();
                break;
            case PeepState::one:
                Update1();
                break;
            case PeepState::onRide:
                // No action
                break;
            case PeepState::picked:
                UpdatePicked();
                break;
            case PeepState::queuingFront:
                UpdateRide();
                break;
            case PeepState::leavingRide:
                UpdateRide();
                break;
            case PeepState::walking:
                UpdateWalking();
                break;
            case PeepState::queuing:
                UpdateQueuing();
                break;
            case PeepState::enteringRide:
                UpdateRide();
                break;
            case PeepState::sitting:
                UpdateSitting();
                break;
            case PeepState::enteringPark:
                UpdateEnteringPark();
                break;
            case PeepState::leavingPark:
                UpdateLeavingPark();
                break;
            case PeepState::buying:
                UpdateBuying();
                break;
            case PeepState::watching:
                UpdateWatching();
                break;
            case PeepState::usingBin:
                UpdateUsingBin();
                break;
            default:
                // TODO reset to default state
                assert(false);
                break;
        }
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
        case PeepRideSubState::atEntrance:
            UpdateRideAtEntrance();
            break;
        case PeepRideSubState::inEntrance:
            UpdateRideAdvanceThroughEntrance();
            break;
        case PeepRideSubState::freeVehicleCheck:
            UpdateRideFreeVehicleCheck();
            break;
        case PeepRideSubState::leaveEntrance:
            UpdateRideAdvanceThroughEntrance();
            break;
        case PeepRideSubState::approachVehicle:
            UpdateRideApproachVehicle();
            break;
        case PeepRideSubState::enterVehicle:
            UpdateRideEnterVehicle();
            break;
        case PeepRideSubState::onRide:
            // No action, on ride.
            break;
        case PeepRideSubState::leaveVehicle:
            UpdateRideLeaveVehicle();
            break;
        case PeepRideSubState::approachExit:
            UpdateRideApproachExit();
            break;
        case PeepRideSubState::inExit:
            UpdateRideInExit();
            break;
        case PeepRideSubState::approachVehicleWaypoints:
            UpdateRideApproachVehicleWaypoints();
            break;
        case PeepRideSubState::approachExitWaypoints:
            UpdateRideApproachExitWaypoints();
            break;
        case PeepRideSubState::approachSpiralSlide:
            UpdateRideApproachSpiralSlide();
            break;
        case PeepRideSubState::onSpiralSlide:
            UpdateRideOnSpiralSlide();
            break;
        case PeepRideSubState::leaveSpiralSlide:
            UpdateRideLeaveSpiralSlide();
            break;
        case PeepRideSubState::mazePathfinding:
            UpdateRideMazePathfinding();
            break;
        case PeepRideSubState::leaveExit:
            UpdateRideLeaveExit();
            break;
        case PeepRideSubState::approachShop:
            UpdateRideShopApproach();
            break;
        case PeepRideSubState::interactShop:
            UpdateRideShopInteract();
            break;
        case PeepRideSubState::leaveShop:
            UpdateRideShopLeave();
            break;
        default:
            // Invalid peep sub-state
            assert(false);
            break;
    }
}

/**
 *
 *  rct2: 0x0069030A
 */
void Guest::UpdateWalking()
{
    if (!CheckForPath())
        return;

    const auto currentTicks = getGameState().currentTicks;

    if (IsActionInterruptableSafely())
    {
        PeepActionType NewAction = Action;

        if (PeepFlags & PEEP_FLAGS_WAVING && (0xFFFF & ScenarioRand()) < 936)
            NewAction = PeepActionType::wave2;
        else if (PeepFlags & PEEP_FLAGS_PHOTO && (0xFFFF & ScenarioRand()) < 936)
            NewAction = PeepActionType::takePhoto;
        else if (PeepFlags & PEEP_FLAGS_PAINTING && (0xFFFF & ScenarioRand()) < 936)
            NewAction = PeepActionType::drawPicture;

        if (NewAction != Action)
        {
            Action = NewAction;
            AnimationFrameNum = 0;
            AnimationImageIdOffset = 0;
            UpdateCurrentAnimationType();
        }
    }

    if (PeepFlags & PEEP_FLAGS_LITTER)
    {
        if (!GetNextIsSurface())
        {
            if ((0xFFFF & ScenarioRand()) <= 4096)
            {
                static constexpr Litter::Type litter_types[] = {
                    Litter::Type::emptyCan,
                    Litter::Type::rubbish,
                    Litter::Type::burgerBox,
                    Litter::Type::emptyCup,
                };
                auto litterType = litter_types[ScenarioRand() & 0x3];
                const auto loc = GetLocation();
                int32_t litterX = loc.x + (ScenarioRand() & 0x7) - 3;
                int32_t litterY = loc.y + (ScenarioRand() & 0x7) - 3;
                Direction litterDirection = (ScenarioRand() & 0x3);

                Litter::Create({ litterX, litterY, loc.z, litterDirection }, litterType);
            }
        }
    }
    else if (HasEmptyContainer())
    {
        if ((!GetNextIsSurface()) && (static_cast<uint32_t>(Id.ToUnderlying() & 0x1FF) == (currentTicks & 0x1FF))
            && ((0xFFFF & ScenarioRand()) <= 4096))
        {
            int32_t container = Numerics::bitScanForward(GetEmptyContainerFlags());
            auto litterType = Litter::Type::vomit;

            if (container != -1)
            {
                auto item = static_cast<ShopItem>(container);
                RemoveItem(item);
                litterType = Litter::Type(GetShopItemDescriptor(item).Type);
            }

            WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
            UpdateAnimationGroup();

            const auto loc = GetLocation();
            int32_t litterX = loc.x + (ScenarioRand() & 0x7) - 3;
            int32_t litterY = loc.y + (ScenarioRand() & 0x7) - 3;
            Direction litterDirection = (ScenarioRand() & 0x3);

            Litter::Create({ litterX, litterY, loc.z, litterDirection }, litterType);
        }
    }

    if (ShouldWaitForLevelCrossing())
    {
        // Wait for vehicle to pass
        UpdateWaitingAtCrossing();

        return;
    }

    const auto [pathingResult, _] = PerformNextAction();
    if (!(pathingResult & PATHING_DESTINATION_REACHED))
        return;

    if (GetNextIsSurface())
    {
        auto surfaceElement = MapGetSurfaceElementAt(NextLoc);

        if (surfaceElement != nullptr)
        {
            int32_t water_height = surfaceElement->GetWaterHeight();
            if (water_height > 0)
            {
                MoveTo({ x, y, water_height });
                SetState(PeepState::falling);
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

    GuestUpdateWalkingBreakScenery(*this);

    if (State != PeepState::walking)
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

    if ((ScenarioRand() & 0xFFFF) > chance)
        return;

    if (GetNextIsSurface() || GetNextIsSloped())
        return;

    TileElement* tileElement = MapGetFirstElementAt(NextLoc);
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

            if (!(pathAddEntry->flags & PATH_ADDITION_FLAG_IS_BENCH))
                positions_free = 9;
        }
    }

    int32_t edges = (tileElement->AsPath()->GetEdges()) ^ 0xF;
    if (edges == 0)
        return;

    uint8_t chosen_edge = ScenarioRand() & 0x3;

    while (!(edges & (1 << chosen_edge)))
        chosen_edge = (chosen_edge + 1) & 3;

    RideId ride_to_view;
    uint8_t ride_seat_to_view;
    if (IsOnLevelCrossing() || !GuestFindRideToLookAt(*this, chosen_edge, &ride_to_view, &ride_seat_to_view))
        return;

    // Check if there is a peep watching (and if there is place for us)
    for (auto peep : EntityTileList<Peep>({ x, y }))
    {
        if (peep->State != PeepState::watching)
            continue;

        if (z != peep->z)
            continue;

        if ((peep->Var37 & 0x3) != chosen_edge)
            continue;

        positions_free &= ~(1 << ((peep->Var37 & 0x1C) >> 2));
    }

    if (!positions_free)
        return;

    uint8_t chosen_position = ScenarioRand() & 0x3;

    while (!(positions_free & (1 << chosen_position)))
        chosen_position = (chosen_position + 1) & 3;

    CurrentRide = ride_to_view;
    CurrentSeat = ride_seat_to_view;
    Var37 = chosen_edge | (chosen_position << 2);

    SetState(PeepState::watching);
    SubState = 0;

    int32_t destX = (x & 0xFFE0) + kWatchingPositionOffsets[Var37 & 0x1F].x;
    int32_t destY = (y & 0xFFE0) + kWatchingPositionOffsets[Var37 & 0x1F].y;

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
    auto ride = GetRide(CurrentRide);
    if (ride == nullptr || ride->status != RideStatus::open)
    {
        RemoveFromQueue();
        SetState(PeepState::one);
        return;
    }

    // If not in the queue then at front of queue
    if (RideSubState != PeepRideSubState::inQueue)
    {
        bool is_front = true;
        // Fix #4819: Occasionally the peep->GuestNextInQueue is incorrectly set
        // to prevent this from causing the peeps to enter a loop
        // first check if the next in queue is actually nearby
        // if they are not then it's safe to assume that this is
        // the front of the queue.
        Peep* nextGuest = getGameState().entities.GetEntity<Guest>(GuestNextInQueue);
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
            SetState(PeepState::queuingFront);
            RideSubState = PeepRideSubState::atEntrance;
        }

        return;
    }

    PerformNextAction();
    if (!IsActionInterruptable())
        return;
    if (AnimationGroup == PeepAnimationGroup::normal)
    {
        if (TimeInQueue >= 2000 && (0xFFFF & ScenarioRand()) <= 119)
        {
            // Eat Food/Look at watch
            Action = PeepActionType::eatFood;
            AnimationFrameNum = 0;
            AnimationImageIdOffset = 0;
            UpdateCurrentAnimationType();
        }
        if (TimeInQueue >= 3500 && (0xFFFF & ScenarioRand()) <= 93)
        {
            // Create the I have been waiting in line ages thought
            InsertNewThought(PeepThoughtType::QueuingAges, CurrentRide);
        }
    }
    else
    {
        if (!(TimeInQueue & 0x3F) && IsActionIdle() && NextAnimationType == PeepAnimationType::watchRide)
        {
            switch (AnimationGroup)
            {
                case PeepAnimationGroup::iceCream:
                case PeepAnimationGroup::chips:
                case PeepAnimationGroup::burger:
                case PeepAnimationGroup::drink:
                case PeepAnimationGroup::candyfloss:
                case PeepAnimationGroup::pizza:
                case PeepAnimationGroup::popcorn:
                case PeepAnimationGroup::hotDog:
                case PeepAnimationGroup::tentacle:
                case PeepAnimationGroup::toffeeApple:
                case PeepAnimationGroup::doughnut:
                case PeepAnimationGroup::coffee:
                case PeepAnimationGroup::chicken:
                case PeepAnimationGroup::lemonade:
                case PeepAnimationGroup::pretzel:
                case PeepAnimationGroup::sujeonggwa:
                case PeepAnimationGroup::juice:
                case PeepAnimationGroup::funnelCake:
                case PeepAnimationGroup::noodles:
                case PeepAnimationGroup::sausage:
                case PeepAnimationGroup::soup:
                case PeepAnimationGroup::sandwich:
                    // Eat food
                    Action = PeepActionType::eatFood;
                    AnimationFrameNum = 0;
                    AnimationImageIdOffset = 0;
                    UpdateCurrentAnimationType();
                    break;
                default:
                    break;
            }
        }
    }
    if (TimeInQueue < 4300)
        return;

    if (Happiness <= 65 && (0xFFFF & ScenarioRand()) < 2184)
    {
        // Give up queueing for the ride
        Orientation ^= (1 << 4);
        Invalidate();
        RemoveFromQueue();
        SetState(PeepState::one);
    }
}

/**
 * rct2: 0x691451
 */
void Guest::UpdateEnteringPark()
{
    if (Var37 != 1)
    {
        const auto [pathingResult, _] = PerformNextAction();
        if ((pathingResult & PATHING_OUTSIDE_PARK))
        {
            DecrementGuestsHeadingForPark();
            PeepEntityRemove(this);
        }
        return;
    }
    if (auto loc = UpdateAction(); loc.has_value())
    {
        MoveTo({ loc.value(), z });
        return;
    }
    SetState(PeepState::falling);

    OutsideOfPark = false;
    ParkEntryTime = getGameState().currentTicks;
    IncrementGuestsInPark();
    DecrementGuestsHeadingForPark();
    auto intent = Intent(INTENT_ACTION_UPDATE_GUEST_COUNT);
    ContextBroadcastIntent(&intent);
}

/**
 *
 *  rct2: 0x6914CD
 */
void Guest::UpdateLeavingPark()
{
    if (Var37 != 0)
    {
        const auto [pathingResult, _] = PerformNextAction();
        if (!(pathingResult & PATHING_OUTSIDE_PARK))
            return;
        PeepEntityRemove(this);
        return;
    }

    if (auto loc = UpdateAction(); loc.has_value())
    {
        MoveTo({ loc.value(), z });
        return;
    }

    OutsideOfPark = true;
    DestinationTolerance = 5;
    DecrementGuestsInPark();
    auto intent = Intent(INTENT_ACTION_UPDATE_GUEST_COUNT);
    ContextBroadcastIntent(&intent);
    Var37 = 1;

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByClass(WindowClass::guestList);

    const auto [pathingResult, _] = PerformNextAction();
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

        const auto [pathingResult, _] = PerformNextAction();
        if (!(pathingResult & PATHING_DESTINATION_REACHED))
            return;

        SetDestination(GetLocation());

        Orientation = (Var37 & 3) * 8;

        Action = PeepActionType::idle;
        NextAnimationType = PeepAnimationType::watchRide;

        SwitchNextAnimationType();

        SubState++;

        TimeToStand = std::clamp(((129 - Energy) * 16 + 50) / 2, 0, 255);
        UpdateAnimationGroup();
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
            Action = PeepActionType::idle;
        }
        else
        {
            if (HasFoodOrDrink())
            {
                if ((ScenarioRand() & 0xFFFF) <= 1310)
                {
                    Action = PeepActionType::eatFood;
                    AnimationFrameNum = 0;
                    AnimationImageIdOffset = 0;
                    UpdateCurrentAnimationType();
                    return;
                }
            }

            if ((ScenarioRand() & 0xFFFF) <= 655)
            {
                Action = PeepActionType::takePhoto;
                AnimationFrameNum = 0;
                AnimationImageIdOffset = 0;
                UpdateCurrentAnimationType();
                return;
            }

            if ((StandingFlags & 1))
            {
                if ((ScenarioRand() & 0xFFFF) <= 655)
                {
                    Action = PeepActionType::wave;
                    AnimationFrameNum = 0;
                    AnimationImageIdOffset = 0;
                    UpdateCurrentAnimationType();
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

        SetState(PeepState::walking);
        UpdateAnimationGroup();
        // Send peep to the centre of current tile.

        auto destination = GetLocation().ToTileCentre();
        SetDestination(destination, 5);
        UpdateCurrentAnimationType();
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
        case PeepUsingBinSubState::walkingToBin:
        {
            if (!CheckForPath())
                return;

            const auto [pathingResult, _] = PerformNextAction();
            if (pathingResult & PATHING_DESTINATION_REACHED)
            {
                UsingBinSubState = PeepUsingBinSubState::goingBack;
            }
            break;
        }
        case PeepUsingBinSubState::goingBack:
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
                if (!(pathAddEntry->flags & PATH_ADDITION_FLAG_IS_BIN))
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
                if (!(emptyContainers & (1uLL << curContainer)))
                    continue;

                auto item = ShopItem(curContainer);
                if (spaceLeftInBin != 0)
                {
                    // OpenRCT2 modification: This previously used
                    // the tick count as a simple random function
                    // switched to scenario_rand as it is more reliable
                    if ((ScenarioRand() & 7) == 0)
                        spaceLeftInBin--;
                    RemoveItem(item);
                    WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                    UpdateAnimationGroup();
                    continue;
                }

                auto litterType = Litter::Type(GetShopItemDescriptor(item).Type);

                int32_t litterX = x + (ScenarioRand() & 7) - 3;
                int32_t litterY = y + (ScenarioRand() & 7) - 3;

                Litter::Create({ litterX, litterY, z, static_cast<Direction>(ScenarioRand() & 3) }, litterType);
                RemoveItem(item);
                WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;

                UpdateAnimationGroup();
            }

            uint8_t additionStatus = foundElement->GetAdditionStatus();
            // Place new amount in bin by first clearing the value
            additionStatus &= ~(3 << selectedBin);
            // Then placing the new value.
            additionStatus |= spaceLeftInBin << selectedBin;
            foundElement->SetAdditionStatus(additionStatus);

            MapInvalidateTileZoom0({ NextLoc, foundElement->GetBaseZ(), foundElement->GetClearanceZ() });
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
        if (pathAddEntry == nullptr || !(pathAddEntry->flags & PATH_ADDITION_FLAG_IS_BENCH))
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
    uint8_t chosen_edge = ScenarioRand() & 0x3;

    while (!(edges & (1 << chosen_edge)))
        chosen_edge = (chosen_edge + 1) & 0x3;

    uint8_t free_edge = 3;

    // Check if there is no peep sitting in chosen_edge
    for (auto peep : EntityTileList<Peep>({ x, y }))
    {
        if (peep->State != PeepState::sitting)
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
        if (ScenarioRand() & 0x8000000)
            free_edge = 1;
    }

    Var37 = ((free_edge & 1) << 2) | chosen_edge;

    SetState(PeepState::sitting);

    SittingSubState = PeepSittingSubState::tryingToSit;

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
        if (pathAddEntry == nullptr || !(pathAddEntry->flags & PATH_ADDITION_FLAG_IS_BIN))
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

    uint8_t chosen_edge = ScenarioRand() & 0x3;

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
            return false;
    }

    peep->Var37 = chosen_edge;

    peep->SetState(PeepState::usingBin);
    peep->UsingBinSubState = PeepUsingBinSubState::walkingToBin;

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
        if (pathAddEntry == nullptr || !(pathAddEntry->flags & PATH_ADDITION_FLAG_BREAKABLE))
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
static void GuestUpdateWalkingBreakScenery(Guest& guest)
{
    if (getGameState().cheats.disableVandalism)
        return;

    if (!(guest.PeepFlags & PEEP_FLAGS_ANGRY))
    {
        if (guest.Happiness >= 48)
            return;
        if (guest.Energy < 85)
            return;
        if (guest.State != PeepState::walking)
            return;

        if ((guest.LitterCount & 0xC0) != 0xC0 && (guest.DisgustingCount & 0xC0) != 0xC0)
            return;

        if ((ScenarioRand() & 0xFFFF) > 3276)
            return;
    }

    if (guest.GetNextIsSurface())
        return;

    auto* tileElement = FindBreakableElement(guest.NextLoc);
    if (tileElement == nullptr)
        return;

    int32_t edges = tileElement->GetEdges();
    if (edges == 0xF)
        return;

    // Check if a peep is already sitting on the bench. If so, do not vandalise it.
    for (auto* otherGuest : EntityTileList<Peep>({ guest.x, guest.y }))
    {
        if ((otherGuest->State != PeepState::sitting) || (guest.z != otherGuest->z))
        {
            continue;
        }

        return;
    }

    for (auto innerPeep : EntityList<Staff>())
    {
        if (innerPeep->AssignedStaffType != StaffType::security || innerPeep->x == kLocationNull)
            continue;

        int32_t xDist = abs(innerPeep->x - guest.x);
        int32_t yDist = abs(innerPeep->y - guest.y);

        if (std::max(xDist, yDist) < 224)
        {
            innerPeep->StaffVandalsStopped = AddClamp(innerPeep->StaffVandalsStopped, 1u);
            innerPeep->WindowInvalidateFlags |= PEEP_INVALIDATE_STAFF_STATS;
            return;
        }
    }

    tileElement->SetIsBroken(true);

    MapInvalidateTileZoom1({ guest.NextLoc, tileElement->GetBaseZ(), tileElement->GetBaseZ() + 32 });

    guest.Angriness = 16;
}

/**
 * rct2: 0x0069101A
 *
 * @return (CF)
 */
static bool PeepShouldWatchRide(TileElement* tileElement)
{
    // Ghosts are purely this-client-side and should not cause any interaction,
    // as that may lead to a desync.
    if (Network::GetMode() != Network::Mode::none)
    {
        if (tileElement->IsGhost())
            return false;
    }

    auto ride = GetRide(tileElement->AsTrack()->GetRideIndex());
    if (ride == nullptr || !ride->isRide())
    {
        return false;
    }

    // This is most likely to have peeps watch new rides
    if (ride->ratings.isNull())
    {
        return true;
    }

    if (ride->ratings.excitement >= RideRating::make(4, 70))
    {
        return true;
    }

    if (ride->ratings.intensity >= RideRating::make(4, 50))
    {
        return true;
    }

    if (ride->getRideTypeDescriptor().HasFlag(RtdFlag::interestingToLookAt))
    {
        if ((ScenarioRand() & 0xFFFF) > 0x3333)
        {
            return false;
        }
    }
    else if (ride->getRideTypeDescriptor().HasFlag(RtdFlag::slightlyInterestingToLookAt))
    {
        if ((ScenarioRand() & 0xFFFF) > 0x1000)
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

bool Loc690FD0(Guest& guest, RideId* rideToView, uint8_t* rideSeatToView, TileElement* tileElement)
{
    auto ride = GetRide(tileElement->AsTrack()->GetRideIndex());
    if (ride == nullptr)
        return false;

    *rideToView = ride->id;
    if (ride->ratings.isNull())
    {
        *rideSeatToView = 1;
        if (ride->status != RideStatus::open)
        {
            if (tileElement->GetClearanceZ() > guest.NextLoc.z + (8 * kCoordsZStep))
            {
                *rideSeatToView |= (1 << 1);
            }

            return true;
        }
    }
    else
    {
        *rideSeatToView = 0;
        if (ride->status == RideStatus::open && !(ride->lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN))
        {
            if (tileElement->GetClearanceZ() > guest.NextLoc.z + (8 * kCoordsZStep))
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
static bool GuestFindRideToLookAt(Guest& guest, uint8_t edge, RideId* rideToView, uint8_t* rideSeatToView)
{
    auto surfaceElement = MapGetSurfaceElementAt(guest.NextLoc);
    TileElement* tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    if (tileElement == nullptr)
    {
        return false;
    }

    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (Network::GetMode() != Network::Mode::none)
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
        if (guest.NextLoc.z + (4 * kCoordsZStep) <= tileElement->GetBaseZ())
            continue;
        if (guest.NextLoc.z + (1 * kCoordsZStep) >= tileElement->GetClearanceZ())
            continue;

        return false;
    } while (!(tileElement++)->IsLastForTile());

    uint16_t x = guest.NextLoc.x + CoordsDirectionDelta[edge].x;
    uint16_t y = guest.NextLoc.y + CoordsDirectionDelta[edge].y;
    if (!MapIsLocationValid(CoordsXY{ x, y }))
    {
        return false;
    }

    surfaceElement = MapGetSurfaceElementAt(CoordsXY{ x, y });

    tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    if (tileElement == nullptr)
    {
        return false;
    }

    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (Network::GetMode() != Network::Mode::none)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetType() != TileElementType::Wall)
            continue;
        if (DirectionReverse(tileElement->GetDirection()) != edge)
            continue;
        auto wallEntry = tileElement->AsWall()->GetEntry();
        if (wallEntry == nullptr || (wallEntry->flags2 & WALL_SCENERY_2_IS_OPAQUE))
            continue;
        // TODO: Check whether this shouldn't be <=, as the other loops use. If so, also extract as loop A.
        if (guest.NextLoc.z + (4 * kCoordsZStep) >= tileElement->GetBaseZ())
            continue;
        if (guest.NextLoc.z + (1 * kCoordsZStep) >= tileElement->GetClearanceZ())
            continue;

        return false;
    } while (!(tileElement++)->IsLastForTile());

    // TODO: Extract loop B
    tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (Network::GetMode() != Network::Mode::none)
        {
            if (tileElement->IsGhost())
                continue;
        }

        if (tileElement->GetClearanceZ() + (1 * kCoordsZStep) < guest.NextLoc.z)
            continue;
        if (guest.NextLoc.z + (6 * kCoordsZStep) < tileElement->GetBaseZ())
            continue;

        if (tileElement->GetType() == TileElementType::Track)
        {
            if (PeepShouldWatchRide(tileElement))
            {
                return Loc690FD0(guest, rideToView, rideSeatToView, tileElement);
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
            if (tileElement->GetClearanceZ() >= guest.NextLoc.z + (8 * kCoordsZStep))
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
        if (Network::GetMode() != Network::Mode::none)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetClearanceZ() + (1 * kCoordsZStep) < guest.NextLoc.z)
            continue;
        if (guest.NextLoc.z + (6 * kCoordsZStep) < tileElement->GetBaseZ())
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
    if (!MapIsLocationValid(CoordsXY{ x, y }))
    {
        return false;
    }

    surfaceElement = MapGetSurfaceElementAt(CoordsXY{ x, y });

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
        if (Network::GetMode() != Network::Mode::none)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetType() != TileElementType::Wall)
            continue;
        if (DirectionReverse(tileElement->GetDirection()) != edge)
            continue;
        auto wallEntry = tileElement->AsWall()->GetEntry();
        if (wallEntry == nullptr || (wallEntry->flags2 & WALL_SCENERY_2_IS_OPAQUE))
            continue;
        if (guest.NextLoc.z + (6 * kCoordsZStep) <= tileElement->GetBaseZ())
            continue;
        if (guest.NextLoc.z >= tileElement->GetClearanceZ())
            continue;

        return false;
    } while (!(tileElement++)->IsLastForTile());

    // TODO: Extract loop B
    tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (Network::GetMode() != Network::Mode::none)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetClearanceZ() + (1 * kCoordsZStep) < guest.NextLoc.z)
            continue;
        if (guest.NextLoc.z + (8 * kCoordsZStep) < tileElement->GetBaseZ())
            continue;

        if (tileElement->GetType() == TileElementType::Track)
        {
            if (PeepShouldWatchRide(tileElement))
            {
                return Loc690FD0(guest, rideToView, rideSeatToView, tileElement);
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
            if (tileElement->GetClearanceZ() >= guest.NextLoc.z + (8 * kCoordsZStep))
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
        if (Network::GetMode() != Network::Mode::none)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetClearanceZ() + (1 * kCoordsZStep) < guest.NextLoc.z)
            continue;
        if (guest.NextLoc.z + (8 * kCoordsZStep) < tileElement->GetBaseZ())
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
    if (!MapIsLocationValid(CoordsXY{ x, y }))
    {
        return false;
    }

    surfaceElement = MapGetSurfaceElementAt(CoordsXY{ x, y });

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
        if (Network::GetMode() != Network::Mode::none)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetType() != TileElementType::Wall)
            continue;
        if (DirectionReverse(tileElement->GetDirection()) != edge)
            continue;
        auto wallEntry = tileElement->AsWall()->GetEntry();
        if (wallEntry == nullptr || (wallEntry->flags2 & WALL_SCENERY_2_IS_OPAQUE))
            continue;
        if (guest.NextLoc.z + (8 * kCoordsZStep) <= tileElement->GetBaseZ())
            continue;
        if (guest.NextLoc.z >= tileElement->GetClearanceZ())
            continue;

        return false;
    } while (!(tileElement++)->IsLastForTile());

    // TODO: Extract loop B
    tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (Network::GetMode() != Network::Mode::none)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetClearanceZ() + (1 * kCoordsZStep) < guest.NextLoc.z)
            continue;
        if (guest.NextLoc.z + (10 * kCoordsZStep) < tileElement->GetBaseZ())
            continue;

        if (tileElement->GetType() == TileElementType::Track)
        {
            if (PeepShouldWatchRide(tileElement))
            {
                return Loc690FD0(guest, rideToView, rideSeatToView, tileElement);
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
            if (tileElement->GetClearanceZ() >= guest.NextLoc.z + (8 * kCoordsZStep))
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
void Guest::SetAnimationGroup(PeepAnimationGroup new_sprite_type)
{
    if (AnimationGroup == new_sprite_type)
        return;

    AnimationGroup = new_sprite_type;
    AnimationImageIdOffset = 0;
    WalkingAnimationFrameNum = 0;

    if (IsActionInterruptable())
        Action = PeepActionType::walking;

    auto& objManager = GetContext()->GetObjectManager();
    auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(AnimationObjectIndex);

    PeepFlags &= ~PEEP_FLAGS_SLOW_WALK;
    if (animObj->IsSlowWalking(new_sprite_type))
    {
        PeepFlags |= PEEP_FLAGS_SLOW_WALK;
    }

    AnimationType = PeepAnimationType::invalid;
    UpdateCurrentAnimationType();

    if (State == PeepState::sitting)
    {
        Action = PeepActionType::idle;
        NextAnimationType = PeepAnimationType::sittingIdle;
        SwitchNextAnimationType();
    }
    if (State == PeepState::watching)
    {
        Action = PeepActionType::idle;
        NextAnimationType = PeepAnimationType::watchRide;
        SwitchNextAnimationType();
    }
}

struct ItemPref
{
    ShopItem item;
    PeepAnimationGroup sprite_type;
};

// clang-format off
static ItemPref item_order_preference[] = {
    { ShopItem::iceCream,         PeepAnimationGroup::iceCream    },
    { ShopItem::chips,            PeepAnimationGroup::chips       },
    { ShopItem::pizza,            PeepAnimationGroup::pizza       },
    { ShopItem::burger,           PeepAnimationGroup::burger      },
    { ShopItem::drink,            PeepAnimationGroup::drink       },
    { ShopItem::coffee,           PeepAnimationGroup::coffee      },
    { ShopItem::chicken,          PeepAnimationGroup::chicken     },
    { ShopItem::lemonade,         PeepAnimationGroup::lemonade    },
    { ShopItem::candyfloss,       PeepAnimationGroup::candyfloss  },
    { ShopItem::popcorn,          PeepAnimationGroup::popcorn     },
    { ShopItem::hotDog,           PeepAnimationGroup::hotDog      },
    { ShopItem::tentacle,         PeepAnimationGroup::tentacle    },
    { ShopItem::toffeeApple,      PeepAnimationGroup::toffeeApple },
    { ShopItem::doughnut,         PeepAnimationGroup::doughnut    },
    { ShopItem::pretzel,          PeepAnimationGroup::pretzel     },
    { ShopItem::cookie,           PeepAnimationGroup::pretzel     },
    { ShopItem::chocolate,        PeepAnimationGroup::coffee      },
    { ShopItem::icedTea,          PeepAnimationGroup::coffee      },
    { ShopItem::funnelCake,       PeepAnimationGroup::funnelCake  },
    { ShopItem::beefNoodles,      PeepAnimationGroup::noodles     },
    { ShopItem::friedRiceNoodles, PeepAnimationGroup::noodles     },
    { ShopItem::wontonSoup,       PeepAnimationGroup::soup        },
    { ShopItem::meatballSoup,     PeepAnimationGroup::soup        },
    { ShopItem::fruitJuice,       PeepAnimationGroup::juice       },
    { ShopItem::soybeanMilk,      PeepAnimationGroup::sujeonggwa   },
    { ShopItem::sujeonggwa,       PeepAnimationGroup::sujeonggwa   },
    { ShopItem::subSandwich,      PeepAnimationGroup::sandwich    },
    { ShopItem::roastSausage,     PeepAnimationGroup::sausage     },
    { ShopItem::balloon,          PeepAnimationGroup::balloon     },
    { ShopItem::hat,              PeepAnimationGroup::hat         },
    { ShopItem::sunglasses,       PeepAnimationGroup::sunglasses  },
};
// clang-format on

/**
 *
 *  rct2: 0x0069B8CC
 */
void Guest::UpdateAnimationGroup()
{
    if (AnimationGroup == PeepAnimationGroup::balloon && (ScenarioRand() & 0xFFFF) <= 327)
    {
        bool isBalloonPopped = false;
        if (x != kLocationNull)
        {
            if ((ScenarioRand() & 0xFFFF) <= 13107)
            {
                isBalloonPopped = true;
                Audio::Play3D(Audio::SoundId::balloonPop, { x, y, z });
            }
            Balloon::Create({ x, y, z + 9 }, BalloonColour, isBalloonPopped);
        }
        RemoveItem(ShopItem::balloon);
        WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
    }

    if (ClimateIsPrecipitating() && (HasItem(ShopItem::umbrella)) && x != kLocationNull)
    {
        CoordsXY loc = { x, y };
        if (MapIsLocationValid(loc.ToTileStart()))
        {
            TileElement* tileElement = MapGetFirstElementAt(loc);
            while (true)
            {
                if (tileElement == nullptr)
                    break;
                if (z < tileElement->GetBaseZ())
                    break;

                if (tileElement->IsLastForTile())
                {
                    SetAnimationGroup(PeepAnimationGroup::umbrella);
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
            SetAnimationGroup(itemPref.sprite_type);
            return;
        }
    }

    if (State == PeepState::watching && StandingFlags & (1 << 1))
    {
        SetAnimationGroup(PeepAnimationGroup::watching);
        return;
    }

    if (Nausea > 170)
    {
        SetAnimationGroup(PeepAnimationGroup::veryNauseous);
        return;
    }

    if (Nausea > 140)
    {
        SetAnimationGroup(PeepAnimationGroup::nauseous);
        return;
    }

    if (Energy <= 64 && Happiness < 128)
    {
        SetAnimationGroup(PeepAnimationGroup::headDown);
        return;
    }

    if (Energy <= 80 && Happiness < 128)
    {
        SetAnimationGroup(PeepAnimationGroup::armsCrossed);
        return;
    }

    if (Toilet > 220)
    {
        SetAnimationGroup(PeepAnimationGroup::requireToilet);
        return;
    }

    SetAnimationGroup(PeepAnimationGroup::normal);
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
void PeepThoughtSetFormatArgs(const PeepThought* thought, Formatter& ft)
{
    ft.Add<StringId>(kPeepThoughtIds[EnumValue(thought->type)]);

    PeepThoughtToActionFlag flags = PeepThoughtToActionMap[EnumValue(thought->type)].flags;
    if (flags & PEEP_THOUGHT_ACTION_FLAG_RIDE)
    {
        auto ride = GetRide(thought->rideId);
        if (ride != nullptr)
        {
            ride->formatNameTo(ft);
        }
        else
        {
            ft.Add<StringId>(kStringIdNone);
        }
    }
    else if (flags & PEEP_THOUGHT_ACTION_FLAG_SHOP_ITEM_SINGULAR)
    {
        ft.Add<StringId>(GetShopItemDescriptor(thought->shopItem).Naming.Singular);
    }
    else if (flags & PEEP_THOUGHT_ACTION_FLAG_SHOP_ITEM_INDEFINITE)
    {
        ft.Add<StringId>(GetShopItemDescriptor(thought->shopItem).Naming.Indefinite);
    }
}

void Guest::InsertNewThought(PeepThoughtType thought_type)
{
    InsertNewThought(thought_type, kPeepThoughtItemNone);
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
    if (newAction != PeepActionType::walking && IsActionInterruptableSafely())
    {
        Action = newAction;
        AnimationFrameNum = 0;
        AnimationImageIdOffset = 0;
        UpdateCurrentAnimationType();
    }

    for (int32_t i = 0; i < kPeepMaxThoughts; ++i)
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
            if (i < kPeepMaxThoughts - 2)
            {
                memmove(thought, thought + 1, sizeof(PeepThought) * (kPeepMaxThoughts - i - 1));
            }
            break;
        }
    }

    memmove(&std::get<1>(Thoughts), &std::get<0>(Thoughts), sizeof(PeepThought) * (kPeepMaxThoughts - 1));

    auto& thought = std::get<0>(Thoughts);
    thought.type = thoughtType;
    thought.item = thoughtArguments;
    thought.freshness = 0;
    thought.fresh_timeout = 0;

    WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_THOUGHTS;
}

// clang-format off
/** rct2: 0x009823A0 */
static constexpr PeepNauseaTolerance nausea_tolerance_distribution[] = {
    PeepNauseaTolerance::None,
    PeepNauseaTolerance::Low, PeepNauseaTolerance::Low,
    PeepNauseaTolerance::Average, PeepNauseaTolerance::Average, PeepNauseaTolerance::Average,
    PeepNauseaTolerance::High, PeepNauseaTolerance::High, PeepNauseaTolerance::High, PeepNauseaTolerance::High, PeepNauseaTolerance::High, PeepNauseaTolerance::High,
};

/** rct2: 0x009823BC */
static constexpr Drawing::Colour kTrouserColours[] = {
    Drawing::Colour::black,
    Drawing::Colour::grey,
    Drawing::Colour::lightBrown,
    Drawing::Colour::saturatedBrown,
    Drawing::Colour::darkBrown,
    Drawing::Colour::salmonPink,
    Drawing::Colour::black,
    Drawing::Colour::grey,
    Drawing::Colour::lightBrown,
    Drawing::Colour::saturatedBrown,
    Drawing::Colour::darkBrown,
    Drawing::Colour::salmonPink,
    Drawing::Colour::black,
    Drawing::Colour::grey,
    Drawing::Colour::lightBrown,
    Drawing::Colour::saturatedBrown,
    Drawing::Colour::darkBrown,
    Drawing::Colour::salmonPink,
    Drawing::Colour::darkPurple,
    Drawing::Colour::lightPurple,
    Drawing::Colour::darkBlue,
    Drawing::Colour::saturatedGreen,
    Drawing::Colour::saturatedRed,
    Drawing::Colour::darkOrange,
    Drawing::Colour::bordeauxRed,
    Drawing::Colour::armyGreen,
    Drawing::Colour::hunterGreen,
    Drawing::Colour::deepWater,
    Drawing::Colour::umber,
};

/** rct2: 0x009823D5 */
static constexpr Drawing::Colour kTshirtColours[] = {
    Drawing::Colour::black,
    Drawing::Colour::grey,
    Drawing::Colour::lightBrown,
    Drawing::Colour::saturatedBrown,
    Drawing::Colour::darkBrown,
    Drawing::Colour::salmonPink,
    Drawing::Colour::black,
    Drawing::Colour::grey,
    Drawing::Colour::lightBrown,
    Drawing::Colour::saturatedBrown,
    Drawing::Colour::darkBrown,
    Drawing::Colour::salmonPink,
    Drawing::Colour::darkPurple,
    Drawing::Colour::lightPurple,
    Drawing::Colour::darkBlue,
    Drawing::Colour::saturatedGreen,
    Drawing::Colour::saturatedRed,
    Drawing::Colour::darkOrange,
    Drawing::Colour::bordeauxRed,
    Drawing::Colour::white,
    Drawing::Colour::brightPurple,
    Drawing::Colour::lightBlue,
    Drawing::Colour::darkWater,
    Drawing::Colour::darkGreen,
    Drawing::Colour::mossGreen,
    Drawing::Colour::brightGreen,
    Drawing::Colour::oliveGreen,
    Drawing::Colour::darkOliveGreen,
    Drawing::Colour::yellow,
    Drawing::Colour::lightOrange,
    Drawing::Colour::brightRed,
    Drawing::Colour::darkPink,
    Drawing::Colour::brightPink,
    Drawing::Colour::armyGreen,
    Drawing::Colour::honeyDew,
    Drawing::Colour::tan,
    Drawing::Colour::maroon,
    Drawing::Colour::coralPink,
    Drawing::Colour::forestGreen,
    Drawing::Colour::chartreuse,
    Drawing::Colour::hunterGreen,
    Drawing::Colour::celadon,
    Drawing::Colour::limeGreen,
    Drawing::Colour::sepia,
    Drawing::Colour::peach,
    Drawing::Colour::periwinkle,
    Drawing::Colour::viridian,
    Drawing::Colour::seafoamGreen,
    Drawing::Colour::violet,
    Drawing::Colour::lavender,
    Drawing::Colour::pastelOrange,
    Drawing::Colour::deepWater,
    Drawing::Colour::pastelPink,
    Drawing::Colour::umber,
    Drawing::Colour::beige,
};
// clang-format on

/**
 *
 *  rct2: 0x0069A05D
 */
Guest* Guest::Generate(const CoordsXYZ& coords)
{
    auto& gameState = getGameState();
    if (gameState.entities.GetNumFreeEntities() < 400)
        return nullptr;

    Guest* peep = gameState.entities.CreateEntity<Guest>();

    peep->AnimationObjectIndex = findPeepAnimationsIndexForType(AnimationPeepType::guest);
    peep->AnimationGroup = PeepAnimationGroup::normal;
    peep->OutsideOfPark = true;
    peep->State = PeepState::falling;
    peep->Action = PeepActionType::walking;
    peep->SpecialSprite = 0;
    peep->AnimationImageIdOffset = 0;
    peep->WalkingAnimationFrameNum = 0;
    peep->AnimationType = PeepAnimationType::walking;
    peep->PeepFlags = 0;
    peep->FavouriteRide = RideId::GetNull();
    peep->FavouriteRideRating = 0;

    auto& objManager = GetContext()->GetObjectManager();
    auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep->AnimationObjectIndex);

    const auto& spriteBounds = animObj->GetSpriteBounds(peep->AnimationGroup, peep->AnimationType);
    peep->SpriteData.Width = spriteBounds.spriteWidth;
    peep->SpriteData.HeightMin = spriteBounds.spriteHeightNegative;
    peep->SpriteData.HeightMax = spriteBounds.spriteHeightPositive;
    peep->Orientation = 0;

    peep->MoveTo(coords);
    peep->Mass = (ScenarioRand() & 0x1F) + 45;
    peep->PathCheckOptimisation = 0;
    peep->InteractionRideIndex = RideId::GetNull();
    peep->PreviousRide = RideId::GetNull();
    std::get<0>(peep->Thoughts).type = PeepThoughtType::None;
    peep->WindowInvalidateFlags = 0;

    uint8_t intensityHighest = (ScenarioRand() & 0x7) + 3;
    uint8_t intensityLowest = std::min(intensityHighest, static_cast<uint8_t>(7)) - 3;

    if (intensityHighest >= 7)
        intensityHighest = 15;

    /* Check which intensity boxes are enabled
     * and apply the appropriate intensity settings. */
    if (gameState.park.flags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES)
    {
        if (gameState.park.flags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES)
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
    else if (gameState.park.flags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES)
    {
        intensityLowest = 9;
        intensityHighest = 15;
    }

    peep->Intensity = IntensityRange(intensityLowest, intensityHighest);

    uint8_t nauseaTolerance = ScenarioRand() & 0x7;
    if (gameState.park.flags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES)
    {
        nauseaTolerance += 4;
    }

    peep->NauseaTolerance = nausea_tolerance_distribution[nauseaTolerance];

    /* Scenario editor limits initial guest happiness to between 37..253.
     * To be on the safe side, assume the value could have been hacked
     * to any value 0..255. */
    peep->Happiness = gameState.scenarioOptions.guestInitialHappiness;
    /* Assume a default initial happiness of 0 is wrong and set
     * to 128 (50%) instead. */
    if (gameState.scenarioOptions.guestInitialHappiness == 0)
        peep->Happiness = 128;
    /* Initial value will vary by -15..16 */
    int8_t happinessDelta = (ScenarioRand() & 0x1F) - 15;
    /* Adjust by the delta, clamping at min=0 and max=255. */
    peep->Happiness = std::clamp(peep->Happiness + happinessDelta, 0, kPeepMaxHappiness);
    peep->HappinessTarget = peep->Happiness;
    peep->Nausea = 0;
    peep->NauseaTarget = 0;

    /* Scenario editor limits initial guest hunger to between 37..253.
     * To be on the safe side, assume the value could have been hacked
     * to any value 0..255. */
    peep->Hunger = gameState.scenarioOptions.guestInitialHunger;
    /* Initial value will vary by -15..16 */
    int8_t hungerDelta = (ScenarioRand() & 0x1F) - 15;
    /* Adjust by the delta, clamping at min=0 and max=255. */
    peep->Hunger = std::clamp(peep->Hunger + hungerDelta, 0, kPeepMaxHunger);

    /* Scenario editor limits initial guest thirst to between 37..253.
     * To be on the safe side, assume the value could have been hacked
     * to any value 0..255. */
    peep->Thirst = gameState.scenarioOptions.guestInitialThirst;
    /* Initial value will vary by -15..16 */
    int8_t thirstDelta = (ScenarioRand() & 0x1F) - 15;
    /* Adjust by the delta, clamping at min=0 and max=255. */
    peep->Thirst = std::clamp(peep->Thirst + thirstDelta, 0, kPeepMaxThirst);

    peep->Toilet = 0;
    peep->TimeToConsume = 0;

    peep->GuestNumRides = 0;
    peep->PeepId = gameState.nextGuestNumber++;
    peep->Name = nullptr;

    money64 cash = (static_cast<money64>(ScenarioRand() & 0x3) * 100) - 100 + gameState.scenarioOptions.guestInitialCash;
    if (cash < 0)
        cash = 0;

    if (gameState.scenarioOptions.guestInitialCash == 0.00_GBP)
    {
        cash = 500;
    }

    if (gameState.park.flags & PARK_FLAGS_NO_MONEY)
    {
        cash = 0;
    }

    if (gameState.scenarioOptions.guestInitialCash == kMoney64Undefined)
    {
        cash = 0;
    }

    peep->CashInPocket = cash;
    peep->CashSpent = 0;
    peep->ParkEntryTime = -1;
    peep->ResetPathfindGoal();
    peep->RemoveAllItems();
    peep->GuestHeadingToRideId = RideId::GetNull();
    peep->GuestNextInQueue = EntityId::GetNull();
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

    uint8_t tshirtColour = static_cast<uint8_t>(ScenarioRand() % std::size(kTshirtColours));
    peep->TshirtColour = kTshirtColours[tshirtColour];

    uint8_t trousersColour = static_cast<uint8_t>(ScenarioRand() % std::size(kTrouserColours));
    peep->TrousersColour = kTrouserColours[trousersColour];

    /* Minimum energy is capped at 32 and maximum at 128, so this initialises
     * a peep with approx 34%-100% energy. (65 - 32) / (128 - 32) ≈ 34% */
    uint8_t energy = (ScenarioRand() % 64) + 65;
    peep->Energy = energy;
    peep->EnergyTarget = energy;

    IncrementGuestsHeadingForPark();

#ifdef ENABLE_SCRIPTING
    auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
    if (hookEngine.HasSubscriptions(Scripting::HookType::guestGeneration))
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();

        // Create event args object
        auto obj = Scripting::DukObject(ctx);
        obj.Set("id", peep->Id.ToUnderlying());

        // Call the subscriptions
        auto e = obj.Take();
        hookEngine.Call(Scripting::HookType::guestGeneration, e, true);
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

static constexpr int32_t kFaceSpriteSmall[] = {
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

static constexpr int32_t kFaceSpriteLarge[] = {
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

static int32_t GetFaceSpriteOffset(Guest* peep)
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
int32_t GetPeepFaceSpriteSmall(Guest* peep)
{
    return kFaceSpriteSmall[GetFaceSpriteOffset(peep)];
}

/**
 * Function split into large and small sprite
 *  rct2: 0x00698721
 */
int32_t GetPeepFaceSpriteLarge(Guest* peep)
{
    return kFaceSpriteLarge[GetFaceSpriteOffset(peep)];
}

/**
 *
 *  rct2: 0x00693CBB
 */
bool Guest::UpdateQueuePosition(PeepActionType previous_action)
{
    TimeInQueue++;

    auto* guestNext = getGameState().entities.GetEntity<Guest>(GuestNextInQueue);
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

        if (PeepDirection != guestNext->PeepDirection)
            return false;

        switch (guestNext->PeepDirection)
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

    Action = PeepActionType::idle;
    NextAnimationType = PeepAnimationType::watchRide;
    if (previous_action != PeepActionType::idle)
        Invalidate();
    return true;
}

/**
 *
 *  rct2: 0x006966A9
 */
void Guest::RemoveFromQueue()
{
    auto ride = GetRide(CurrentRide);
    if (ride == nullptr)
        return;

    auto& station = ride->getStation(CurrentRideStation);
    // Make sure we don't underflow, building while paused might reset it to 0 where peeps have
    // not yet left the queue.
    if (station.QueueLength > 0)
    {
        station.QueueLength--;
    }

    if (Id == station.LastPeepInQueue)
    {
        station.LastPeepInQueue = GuestNextInQueue;
        return;
    }

    auto& gameState = getGameState();
    auto* otherGuest = gameState.entities.GetEntity<Guest>(station.LastPeepInQueue);
    if (otherGuest == nullptr)
    {
        LOG_ERROR("Invalid Guest Queue list!");
        return;
    }
    for (; otherGuest != nullptr; otherGuest = gameState.entities.GetEntity<Guest>(otherGuest->GuestNextInQueue))
    {
        if (Id == otherGuest->GuestNextInQueue)
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
    TimeToConsume = 0;
}

void Guest::RemoveItem(ShopItem item)
{
    ItemFlags &= ~EnumToFlag(item);
    TimeToConsume = 0;
}

void Guest::GiveItem(ShopItem item)
{
    ItemFlags |= EnumToFlag(item);

    const auto& shopItemDescriptor = GetShopItemDescriptor(item);
    uint16_t consumptionTime = shopItemDescriptor.ConsumptionTime;
    TimeToConsume = std::min((TimeToConsume + consumptionTime), 255);
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
    if (State == PeepState::watching)
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
    if (HasItem(ShopItem::voucher))
    {
        if (VoucherType == VOUCHER_TYPE_RIDE_FREE && VoucherRideId == rideId)
        {
            RemoveItem(ShopItem::voucher);
        }
    }

    // remove any photos of this ride from peep
    if (HasItem(ShopItem::photo))
    {
        if (Photo1RideRef == rideId)
        {
            RemoveItem(ShopItem::photo);
        }
    }
    if (HasItem(ShopItem::photo2))
    {
        if (Photo2RideRef == rideId)
        {
            RemoveItem(ShopItem::photo2);
        }
    }
    if (HasItem(ShopItem::photo3))
    {
        if (Photo3RideRef == rideId)
        {
            RemoveItem(ShopItem::photo3);
        }
    }
    if (HasItem(ShopItem::photo4))
    {
        if (Photo4RideRef == rideId)
        {
            RemoveItem(ShopItem::photo4);
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
        lastEntry.item = kPeepThoughtItemNone;
    }
}

void Guest::ThrowUp()
{
    Hunger /= 2;
    NauseaTarget /= 2;

    if (Nausea < 30)
        Nausea = 0;
    else
        Nausea -= 30;

    WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_2;

    const auto curLoc = GetLocation();
    Litter::Create({ curLoc, Orientation }, (Id.ToUnderlying() & 1) ? Litter::Type::vomitAlt : Litter::Type::vomit);

    static constexpr Audio::SoundId coughs[4] = {
        Audio::SoundId::cough1,
        Audio::SoundId::cough2,
        Audio::SoundId::cough3,
        Audio::SoundId::cough4,
    };
    auto soundId = coughs[ScenarioRand() & 3];
    Audio::Play3D(soundId, curLoc);
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
