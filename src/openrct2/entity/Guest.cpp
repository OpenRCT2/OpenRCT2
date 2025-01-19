/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../core/DataSerialiser.h"
#include "../core/Guard.hpp"
#include "../core/Numerics.hpp"
#include "../core/String.hpp"
#include "../entity/Balloon.h"
#include "../entity/EntityRegistry.h"
#include "../entity/MoneyEffect.h"
#include "../entity/Particle.h"
#include "../interface/Window_internal.h"
#include "../localisation/Formatter.h"
#include "../localisation/Formatting.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../network/network.h"
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
#include "../sprites.h"
#include "../ui/UiContext.h"
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
using namespace OpenRCT2::Numerics;

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

static constexpr ride_rating NauseaMaximumThresholds[] = {
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
static constexpr ride_rating kNauseaMinimumThresholds[] = {
    0,
    0,
    200,
    400,
};

static bool PeepHasVoucherForFreeRide(Guest* peep, const Ride& ride);
static void PeepRideIsTooIntense(Guest* peep, Ride& ride, bool peepAtRide);
static void PeepResetRideHeading(Guest* peep);
static void PeepTriedToEnterFullQueue(Guest* peep, Ride& ride);
static int16_t PeepCalculateRideSatisfaction(Guest* peep, const Ride& ride);
static void GuestUpdateFavouriteRide(Guest& peep, const Ride& ride, const uint8_t satisfaction);
static int16_t PeepCalculateRideValueSatisfaction(Guest* peep, const Ride& ride);
static int16_t PeepCalculateRideIntensityNauseaSatisfaction(Guest* peep, const Ride& ride);
static void PeepUpdateRideNauseaGrowth(Guest* peep, const Ride& ride);
static bool PeepShouldGoOnRideAgain(Guest* peep, const Ride& ride);
static bool PeepShouldPreferredIntensityIncrease(Guest* peep);
static bool PeepReallyLikedRide(Guest* peep, const Ride& ride);
static PeepThoughtType PeepAssessSurroundings(int16_t centre_x, int16_t centre_y, int16_t centre_z);
static void PeepUpdateHunger(Guest* peep);
static void PeepDecideWhetherToLeavePark(Guest* peep);
static void PeepLeavePark(Guest* peep);
static void PeepHeadForNearestRideWithFlag(Guest* peep, bool considerOnlyCloseRides, RtdFlag rtdFlag);
static void GuestHeadForNearestRideWithSpecialType(Guest& guest, bool considerOnlyCloseRides, RtdSpecialType specialType);
bool Loc690FD0(Peep* peep, RideId* rideToView, uint8_t* rideSeatToView, TileElement* tileElement);

template<>
bool EntityBase::Is<Guest>() const
{
    return Type == EntityType::Guest;
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

template<void (Guest::*EasterEggFunc)(Guest*), bool applyToSelf>
static void ApplyEasterEggToNearbyGuests(Guest* guest)
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

    int32_t peepDirection = (Orientation >> 3) ^ 2;
    int32_t otherPeepOppositeDirection = passingPeep->Orientation >> 3;
    if (peepDirection == otherPeepOppositeDirection)
    {
        if (passingPeep->IsActionInterruptable())
        {
            passingPeep->Action = PeepActionType::Wave2;
            passingPeep->AnimationFrameNum = 0;
            passingPeep->AnimationImageIdOffset = 0;
            passingPeep->UpdateCurrentAnimationType();
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
        passingPeep->AnimationFrameNum = 0;
        passingPeep->AnimationImageIdOffset = 0;
        passingPeep->UpdateCurrentAnimationType();
    }
}

void Guest::GivePassingPeepsIceCream(Guest* passingPeep)
{
    if (passingPeep->HasItem(ShopItem::IceCream))
        return;

    passingPeep->GiveItem(ShopItem::IceCream);
    passingPeep->UpdateAnimationGroup();
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
        if ((ScenarioRand() & 0xFFFF) <= 1456)
        {
            if (IsActionInterruptable())
            {
                Action = PeepActionType::Joy;
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
        if ((ScenarioRand() & 0xFF) <= static_cast<uint8_t>((Nausea - 128) / 2))
        {
            if (IsActionInterruptable())
            {
                Action = PeepActionType::ThrowUp;
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
            int32_t chosen_food = Numerics::bitScanForward(GetFoodOrDrinkFlags());
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
    const auto currentTicks = GetGameState().CurrentTicks;
    if ((index & 0x1FF) != (currentTicks & 0x1FF))
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
            if (x != kLocationNull)
            {
                PeepThoughtType thought_type = PeepAssessSurroundings(x & 0xFFE0, y & 0xFFE0, z);

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
                auto ride = GetRide(CurrentRide);
                if (ride != nullptr)
                {
                    PeepThoughtType thought_type = ride->GetRideTypeDescriptor().HasFlag(RtdFlag::describeAsInside)
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

    if (State == PeepState::Walking && !OutsideOfPark && !(PeepFlags & PEEP_FLAGS_LEAVING_PARK) && GuestNumRides == 0
        && GuestHeadingToRideId.IsNull())
    {
        uint32_t time_duration = currentTicks - ParkEntryTime;
        time_duration /= 2048;

        if (time_duration >= 5)
        {
            PickRideToGoOn();

            if (GuestHeadingToRideId.IsNull())
            {
                HappinessTarget = std::max(HappinessTarget - 128, 0);
                PeepLeavePark(this);
                PeepUpdateHunger(this);
                UpdateMotivesIdle();
                UpdateConsumptionMotives();
                return;
            }
        }
    }

    if ((ScenarioRand() & 0xFFFF) <= ((HasItem(ShopItem::Map)) ? 8192u : 2184u))
    {
        PickRideToGoOn();
    }

    if ((index & 0x3FF) == (currentTicks & 0x3FF))
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

                if (!(GetGameState().Park.Flags & PARK_FLAGS_NO_MONEY) && CashInPocket <= 9.00_GBP && Happiness >= 105
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
                        PeepHeadForNearestRideWithFlag(this, false, RtdFlag::sellsFood);
                        break;
                    case PeepThoughtType::Thirsty:
                        PeepHeadForNearestRideWithFlag(this, false, RtdFlag::sellsDrinks);
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
        case PeepState::Walking:
        case PeepState::LeavingPark:
        case PeepState::EnteringPark:
            PeepDecideWhetherToLeavePark(this);
            PeepUpdateHunger(this);
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
            PeepUpdateHunger(this);
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
            PeepUpdateHunger(this);
            break;
        case PeepState::EnteringRide:
            if (SubState == 17 || SubState == 15)
            {
                PeepDecideWhetherToLeavePark(this);
            }
            PeepUpdateHunger(this);
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
    UpdateCurrentAnimationType();
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

        Orientation = ((Var37 + 2) & 3) * 8;
        Action = PeepActionType::Idle;
        NextAnimationType = PeepAnimationType::SittingIdle;
        SwitchNextAnimationType();

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
            UpdateCurrentAnimationType();
            return;
        }

        if (AnimationGroup == PeepAnimationGroup::Umbrella)
        {
            TryGetUpFromSitting();
            return;
        }

        if (HasFoodOrDrink())
        {
            if ((ScenarioRand() & 0xFFFFu) > 1310u)
            {
                TryGetUpFromSitting();
                return;
            }
            Action = PeepActionType::SittingEatFood;
            AnimationFrameNum = 0;
            AnimationImageIdOffset = 0;
            UpdateCurrentAnimationType();
            return;
        }

        const auto rand = ScenarioRand();
        if ((rand & 0xFFFFu) > 131u)
        {
            TryGetUpFromSitting();
            return;
        }
        if (AnimationGroup == PeepAnimationGroup::Balloon || AnimationGroup == PeepAnimationGroup::Hat)
        {
            TryGetUpFromSitting();
            return;
        }

        Action = PeepActionType::SittingLookAroundLeft;
        if (rand & 0x80000000u)
        {
            Action = PeepActionType::SittingLookAroundRight;
        }

        if (rand & 0x40000000u)
        {
            Action = PeepActionType::SittingCheckWatch;
        }
        AnimationFrameNum = 0;
        AnimationImageIdOffset = 0;
        UpdateCurrentAnimationType();
        return;
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
    WindowBase* w = windowMgr->FindByNumber(WindowClass::Peep, Id);

    if (w != nullptr)
    {
        w->OnPrepareDraw();
    }

    WindowInvalidateByNumber(WindowClass::Peep, Id);
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
bool Guest::DecideAndBuyItem(Ride& ride, const ShopItem shopItem, money64 price)
{
    money64 itemValue;

    bool hasVoucher = false;

    const bool isPrecipitating = ClimateIsRaining() || ClimateIsSnowingHeavily();
    const bool isUmbrella = shopItem == ShopItem::Umbrella;
    const bool isRainingAndUmbrella = isPrecipitating && isUmbrella;

    if ((HasItem(ShopItem::Voucher)) && (VoucherType == VOUCHER_TYPE_FOOD_OR_DRINK_FREE) && (VoucherShopItem == shopItem))
    {
        hasVoucher = true;
    }

    if (HasItem(shopItem))
    {
        InsertNewThought(PeepThoughtType::AlreadyGot, shopItem);
        return false;
    }

    const auto& shopItemDescriptor = GetShopItemDescriptor(shopItem);
    if (shopItemDescriptor.IsFoodOrDrink())
    {
        int32_t food = Numerics::bitScanForward(GetFoodOrDrinkFlags());
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
        && isPrecipitating)
    {
        return false;
    }

    auto& gameState = GetGameState();
    if ((shopItem == ShopItem::Sunglasses || shopItem == ShopItem::IceCream) && gameState.ClimateCurrent.Temperature < 12)
    {
        return false;
    }

    if (shopItemDescriptor.IsFood() && (Hunger > 75))
    {
        InsertNewThought(PeepThoughtType::NotHungry);
        return false;
    }

    if (shopItemDescriptor.IsDrink() && (Thirst > 75))
    {
        InsertNewThought(PeepThoughtType::NotThirsty);
        return false;
    }

    if (!isRainingAndUmbrella && (shopItem != ShopItem::Map) && shopItemDescriptor.IsSouvenir() && !hasVoucher)
    {
        if (((ScenarioRand() & 0x7F) + 0x73) > Happiness || GuestNumRides < 3)
            return false;
    }

    if (!hasVoucher)
    {
        if (price != 0 && !(gameState.Park.Flags & PARK_FLAGS_NO_MONEY))
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

        if (gameState.ClimateCurrent.Temperature >= 21)
            itemValue = shopItemDescriptor.HotValue;
        else if (gameState.ClimateCurrent.Temperature <= 11)
            itemValue = shopItemDescriptor.ColdValue;
        else
            itemValue = shopItemDescriptor.BaseValue;

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
                if (itemValue > (static_cast<money64>(ScenarioRand() & 0x07)) && !(GetGameState().Cheats.ignorePrice))
                {
                    // "I'm not paying that much for x"
                    InsertNewThought(shopItemDescriptor.TooMuchThought, ride.id);
                    return false;
                }
            }
        }
        else
        {
            itemValue -= price;
            itemValue = std::max(0.80_GBP, itemValue);

            if (!(gameState.Park.Flags & PARK_FLAGS_NO_MONEY))
            {
                if (itemValue >= static_cast<money64>(ScenarioRand() & 0x07))
                {
                    // "This x is a really good value"
                    InsertNewThought(shopItemDescriptor.GoodValueThought, ride.id);
                }
            }

            int32_t happinessGrowth = itemValue * 4;
            HappinessTarget = std::min((HappinessTarget + happinessGrowth), kPeepMaxHappiness);
            Happiness = std::min((Happiness + happinessGrowth), kPeepMaxHappiness);
        }

        // reset itemValue for satisfaction calculation
        if (gameState.ClimateCurrent.Temperature >= 21)
            itemValue = shopItemDescriptor.HotValue;
        else if (gameState.ClimateCurrent.Temperature <= 11)
            itemValue = shopItemDescriptor.ColdValue;
        else
            itemValue = shopItemDescriptor.BaseValue;
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
        ride.UpdateSatisfaction(satisfaction);
    }

    // The peep has now decided to buy the item (or, specifically, has not been
    // dissuaded so far).
    GiveItem(shopItem);
    const auto hasRandomShopColour = ride.HasLifecycleFlag(RIDE_LIFECYCLE_RANDOM_SHOP_COLOURS);

    if (shopItem == ShopItem::TShirt)
        TshirtColour = hasRandomShopColour ? ScenarioRandMax(kColourNumNormal) : ride.track_colour[0].main;

    if (shopItem == ShopItem::Hat)
        HatColour = hasRandomShopColour ? ScenarioRandMax(kColourNumNormal) : ride.track_colour[0].main;

    if (shopItem == ShopItem::Balloon)
        BalloonColour = hasRandomShopColour ? ScenarioRandMax(kColourNumNormal) : ride.track_colour[0].main;

    if (shopItem == ShopItem::Umbrella)
        UmbrellaColour = hasRandomShopColour ? ScenarioRandMax(kColourNumNormal) : ride.track_colour[0].main;

    if (shopItem == ShopItem::Map)
        ResetPathfindGoal();

    if (shopItem == ShopItem::Photo)
        Photo1RideRef = ride.id;

    if (shopItem == ShopItem::Photo2)
        Photo2RideRef = ride.id;

    if (shopItem == ShopItem::Photo3)
        Photo3RideRef = ride.id;

    if (shopItem == ShopItem::Photo4)
        Photo4RideRef = ride.id;

    WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
    UpdateAnimationGroup();
    if (PeepFlags & PEEP_FLAGS_TRACKING)
    {
        auto ft = Formatter();
        FormatNameTo(ft);
        ft.Add<StringId>(shopItemDescriptor.Naming.Indefinite);
        if (Config::Get().notifications.GuestBoughtItem)
        {
            News::AddItemToQueue(News::ItemType::PeepOnRide, STR_PEEP_TRACKING_NOTIFICATION_BOUGHT_X, Id, ft);
        }
    }

    if (shopItemDescriptor.IsFood())
        AmountOfFood++;

    if (shopItemDescriptor.IsDrink())
        AmountOfDrinks++;

    if (shopItemDescriptor.IsSouvenir())
        AmountOfSouvenirs++;

    money64* expend_type = &PaidOnSouvenirs;
    ExpenditureType expenditure = ExpenditureType::ShopStock;

    if (shopItemDescriptor.IsFood())
    {
        expend_type = &PaidOnFood;
        expenditure = ExpenditureType::FoodDrinkStock;
    }

    if (shopItemDescriptor.IsDrink())
    {
        expend_type = &PaidOnDrink;
        expenditure = ExpenditureType::FoodDrinkStock;
    }

    if (!(gameState.Park.Flags & PARK_FLAGS_NO_MONEY))
        FinancePayment(shopItemDescriptor.Cost, expenditure);

    // Sets the expenditure type to *_FOODDRINK_SALES or *_SHOP_SALES appropriately.
    expenditure = static_cast<ExpenditureType>(static_cast<int32_t>(expenditure) - 1);
    if (hasVoucher)
    {
        RemoveItem(ShopItem::Voucher);
        WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
    }
    else if (!(gameState.Park.Flags & PARK_FLAGS_NO_MONEY))
    {
        SpendMoney(*expend_type, price, expenditure);
    }
    ride.total_profit += (price - shopItemDescriptor.Cost);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
    ride.cur_num_customers++;
    ride.total_customers++;
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;

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
    int16_t satisfaction = PeepCalculateRideSatisfaction(this, ride);

    // Update the satisfaction stat of the ride.
    uint8_t rideSatisfaction = 0;
    if (satisfaction >= 40)
        rideSatisfaction = 3;
    else if (satisfaction >= 20)
        rideSatisfaction = 2;
    else if (satisfaction >= 0)
        rideSatisfaction = 1;

    ride.UpdateSatisfaction(rideSatisfaction);

    // Update various peep stats.
    if (GuestNumRides < 255)
        GuestNumRides++;

    SetHasRidden(ride);
    GuestUpdateFavouriteRide(*this, ride, satisfaction);
    HappinessTarget = std::clamp(HappinessTarget + satisfaction, 0, kPeepMaxHappiness);
    PeepUpdateRideNauseaGrowth(this, ride);
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

    if (PeepShouldGoOnRideAgain(this, ride))
    {
        GuestHeadingToRideId = ride.id;
        GuestIsLostCountdown = 200;
        ResetPathfindGoal();
        WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_ACTION;
    }

    if (PeepShouldPreferredIntensityIncrease(this))
    {
        if (Intensity.GetMaximum() < 15)
        {
            Intensity = Intensity.WithMaximum(Intensity.GetMaximum() + 1);
        }
    }

    if (PeepReallyLikedRide(this, ride))
    {
        InsertNewThought(PeepThoughtType::WasGreat, ride.id);

        static constexpr OpenRCT2::Audio::SoundId laughs[3] = {
            OpenRCT2::Audio::SoundId::Laugh1,
            OpenRCT2::Audio::SoundId::Laugh2,
            OpenRCT2::Audio::SoundId::Laugh3,
        };
        int32_t laughType = ScenarioRand() & 7;
        if (laughType < 3)
        {
            OpenRCT2::Audio::Play3D(laughs[laughType], GetLocation());
        }
    }

    ride.total_customers++;
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;
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
    if (x == kLocationNull)
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
                if (ShouldGoOnRide(ride, StationIndex::FromUnderlying(0), false, true) && RideHasRatings(ride))
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

OpenRCT2::BitSet<OpenRCT2::Limits::kMaxRidesInPark> Guest::FindRidesToGoOn()
{
    OpenRCT2::BitSet<OpenRCT2::Limits::kMaxRidesInPark> rideConsideration;

    // FIX  Originally checked for a toy, likely a mistake and should be a map,
    //      but then again this seems to only allow the peep to go on
    //      rides they haven't been on before.
    if (HasItem(ShopItem::Map))
    {
        // Consider rides that peep hasn't been on yet
        for (auto& ride : GetRideManager())
        {
            if (!HasRidden(ride))
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
        for (auto& ride : GetRideManager())
        {
            if (ride.highest_drop_height > 66 || ride.ratings.excitement >= RIDE_RATING(8, 00))
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
bool Guest::ShouldGoOnRide(Ride& ride, StationIndex entranceNum, bool atQueue, bool thinking)
{
    // Indicates whether a peep is physically at the ride, or is just thinking about going on the ride.
    bool peepAtRide = !thinking;

    if (ride.status == RideStatus::Open && !(ride.lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN))
    {
        // Peeps that are leaving the park will refuse to go on any rides, with the exception of free transport rides.
        assert(ride.type < std::size(RideTypeDescriptors));
        if (!ride.GetRideTypeDescriptor().HasFlag(RtdFlag::isTransportRide) || ride.value == RIDE_VALUE_UNDEFINED
            || RideGetPrice(ride) != 0)
        {
            if (PeepFlags & PEEP_FLAGS_LEAVING_PARK)
            {
                ChoseNotToGoOnRide(ride, peepAtRide, false);
                return false;
            }
        }

        if (ride.GetRideTypeDescriptor().HasFlag(RtdFlag::isShopOrFacility))
        {
            return ShouldGoToShop(ride, peepAtRide);
        }

        // This used to check !(flags & 2), but the function is only ever called with flags = 0, 1 or 6.
        // This means we can use the existing !(flags & 4) check.
        if (peepAtRide)
        {
            auto& station = ride.GetStation(entranceNum);

            // Rides without queues can only have one peep waiting at a time.
            if (!atQueue)
            {
                if (!station.LastPeepInQueue.IsNull())
                {
                    PeepTriedToEnterFullQueue(this, ride);
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
                        PeepTriedToEnterFullQueue(this, ride);
                        return false;
                    }

                    // This checks if there's a peep standing still at the very end of the queue.
                    if (maxD <= 13 && lastPeepInQueue->TimeInQueue > 10)
                    {
                        PeepTriedToEnterFullQueue(this, ride);
                        return false;
                    }
                }
            }
        }

        // Assuming the queue conditions are met, peeps will always go on free transport rides.
        // Ride ratings, recent crashes and weather will all be ignored.
        auto ridePrice = RideGetPrice(ride);
        if (!ride.GetRideTypeDescriptor().HasFlag(RtdFlag::isTransportRide) || ride.value == RIDE_VALUE_UNDEFINED
            || ridePrice != 0)
        {
            if (PreviousRide == ride.id)
            {
                ChoseNotToGoOnRide(ride, peepAtRide, false);
                return false;
            }

            auto& gameState = GetGameState();
            // Basic price checks
            if (ridePrice != 0 && !PeepHasVoucherForFreeRide(this, ride) && !(gameState.Park.Flags & PARK_FLAGS_NO_MONEY))
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
            if (ride.last_crash_type != RIDE_CRASH_TYPE_NONE && Happiness < 225)
            {
                if (peepAtRide)
                {
                    InsertNewThought(PeepThoughtType::NotSafe, ride.id);
                    if (HappinessTarget >= 64)
                    {
                        HappinessTarget -= 8;
                    }
                    ride.UpdatePopularity(0);
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
                    if (ride.ratings.intensity > RIDE_RATING(10, 00) && !GetGameState().Cheats.ignoreRideIntensity)
                    {
                        PeepRideIsTooIntense(this, ride, peepAtRide);
                        return false;
                    }
                }
                else
                {
                    const bool isPrecipitating = ClimateIsRaining() || ClimateIsSnowingHeavily();
                    if (isPrecipitating && !ShouldRideWhileRaining(ride))
                    {
                        if (peepAtRide)
                        {
                            InsertNewThought(PeepThoughtType::NotWhileRaining, ride.id);
                            if (HappinessTarget >= 64)
                            {
                                HappinessTarget -= 8;
                            }
                            ride.UpdatePopularity(0);
                        }
                        ChoseNotToGoOnRide(ride, peepAtRide, true);
                        return false;
                    }
                    // If it is raining and the ride provides shelter skip the
                    // ride intensity check and get me on a sheltered ride!
                    if (!isPrecipitating || !ShouldRideWhileRaining(ride))
                    {
                        if (!GetGameState().Cheats.ignoreRideIntensity)
                        {
                            // Intensity calculations. Even though the max intensity can go up to 15, it's capped
                            // at 10.0 (before happiness calculations). A full happiness bar will increase the max
                            // intensity and decrease the min intensity by about 2.5.
                            ride_rating maxIntensity = std::min(Intensity.GetMaximum() * 100, 1000) + Happiness;
                            ride_rating minIntensity = (Intensity.GetMinimum() * 100) - Happiness;
                            if (ride.ratings.intensity < minIntensity)
                            {
                                if (peepAtRide)
                                {
                                    InsertNewThought(PeepThoughtType::MoreThrilling, ride.id);
                                    if (HappinessTarget >= 64)
                                    {
                                        HappinessTarget -= 8;
                                    }
                                    ride.UpdatePopularity(0);
                                }
                                ChoseNotToGoOnRide(ride, peepAtRide, true);
                                return false;
                            }
                            if (ride.ratings.intensity > maxIntensity)
                            {
                                PeepRideIsTooIntense(this, ride, peepAtRide);
                                return false;
                            }

                            // Nausea calculations.
                            ride_rating maxNausea = NauseaMaximumThresholds[(EnumValue(NauseaTolerance) & 3)] + Happiness;

                            if (ride.ratings.nausea > maxNausea)
                            {
                                if (peepAtRide)
                                {
                                    InsertNewThought(PeepThoughtType::Sickening, ride.id);
                                    if (HappinessTarget >= 64)
                                    {
                                        HappinessTarget -= 8;
                                    }
                                    ride.UpdatePopularity(0);
                                }
                                ChoseNotToGoOnRide(ride, peepAtRide, true);
                                return false;
                            }

                            // Very nauseous peeps will only go on very gentle rides.
                            if (ride.ratings.nausea >= FIXED_2DP(1, 40) && Nausea > 160)
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
            if (!RideHasRatings(ride) && ride.GetRideTypeDescriptor().HasFlag(RtdFlag::checkGForces))
            {
                if ((ScenarioRand() & 0xFFFF) > 0x1999u)
                {
                    ChoseNotToGoOnRide(ride, peepAtRide, false);
                    return false;
                }

                if (!GetGameState().Cheats.ignoreRideIntensity)
                {
                    if (ride.max_positive_vertical_g > FIXED_2DP(5, 00) || ride.max_negative_vertical_g < FIXED_2DP(-4, 00)
                        || ride.max_lateral_g > FIXED_2DP(4, 00))
                    {
                        ChoseNotToGoOnRide(ride, peepAtRide, false);
                        return false;
                    }
                }
            }

            money64 value = ride.value;

            // If the value of the ride hasn't yet been calculated, peeps will be willing to pay any amount for the ride.
            if (value != RIDE_VALUE_UNDEFINED && !PeepHasVoucherForFreeRide(this, ride)
                && !(gameState.Park.Flags & PARK_FLAGS_NO_MONEY))
            {
                // The amount peeps are willing to pay is decreased by 75% if they had to pay to enter the park.
                if (PeepFlags & PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY)
                    value /= 4;

                // Peeps won't pay more than twice the value of the ride.
                ridePrice = RideGetPrice(ride);
                if ((ridePrice > (value * 2)) && !(gameState.Cheats.ignorePrice))
                {
                    if (peepAtRide)
                    {
                        InsertNewThought(PeepThoughtType::BadValue, ride.id);
                        if (HappinessTarget >= 60)
                        {
                            HappinessTarget -= 16;
                        }
                        ride.UpdatePopularity(0);
                    }
                    ChoseNotToGoOnRide(ride, peepAtRide, true);
                    return false;
                }

                // A ride is good value if the price is 50% or less of the ride value and the peep didn't pay to enter the park.
                if (ridePrice <= (value / 2) && peepAtRide)
                {
                    if (!(gameState.Park.Flags & PARK_FLAGS_NO_MONEY))
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
            ride.UpdatePopularity(1);
        }

        if (ride.id == GuestHeadingToRideId)
        {
            PeepResetRideHeading(this);
        }

        ride.lifecycle_flags &= ~RIDE_LIFECYCLE_QUEUE_FULL;
        return true;
    }

    ChoseNotToGoOnRide(ride, peepAtRide, false);
    return false;
}

bool Guest::ShouldGoToShop(Ride& ride, bool peepAtShop)
{
    // Peeps won't go to the same shop twice in a row.
    if (ride.id == PreviousRide)
    {
        ChoseNotToGoOnRide(ride, peepAtShop, true);
        return false;
    }

    const auto& rtd = ride.GetRideTypeDescriptor();
    if (rtd.specialType == RtdSpecialType::toilet)
    {
        if (Toilet < 70)
        {
            ChoseNotToGoOnRide(ride, peepAtShop, true);
            return false;
        }

        // The amount that peeps are willing to pay to use the Toilets scales with their toilet stat.
        // It effectively has a minimum of $0.10 (due to the check above) and a maximum of $0.60.
        if ((RideGetPrice(ride) * 40 > Toilet) && !GetGameState().Cheats.ignorePrice)
        {
            if (peepAtShop)
            {
                InsertNewThought(PeepThoughtType::NotPaying, ride.id);
                if (HappinessTarget >= 60)
                {
                    HappinessTarget -= 16;
                }
                ride.UpdatePopularity(0);
            }
            ChoseNotToGoOnRide(ride, peepAtShop, true);
            return false;
        }
    }

    if (rtd.specialType == RtdSpecialType::firstAid)
    {
        if (Nausea < 128)
        {
            ChoseNotToGoOnRide(ride, peepAtShop, true);
            return false;
        }
    }

    // Basic price checks
    auto ridePrice = RideGetPrice(ride);
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
                InsertNewThought(PeepThoughtType::CantAffordRide, ride.id);
            }
        }
        ChoseNotToGoOnRide(ride, peepAtShop, true);
        return false;
    }

    if (peepAtShop)
    {
        ride.UpdatePopularity(1);
        if (ride.id == GuestHeadingToRideId)
        {
            PeepResetRideHeading(this);
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
    assert(!(GetGameState().Park.Flags & PARK_FLAGS_NO_MONEY));

    CashInPocket = std::max(0.00_GBP, static_cast<money64>(CashInPocket) - amount);
    CashSpent += amount;

    peep_expend_type += amount;

    WindowInvalidateByNumber(WindowClass::Peep, Id);

    FinancePayment(-amount, expenditure);

    MoneyEffect::CreateAt(amount, GetLocation(), true);

    OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::Purchase, GetLocation());
}

void Guest::SetHasRidden(const Ride& ride)
{
    OpenRCT2::RideUse::GetHistory().Add(Id, ride.id);

    SetHasRiddenRideType(ride.type);
}

bool Guest::HasRidden(const Ride& ride) const
{
    return OpenRCT2::RideUse::GetHistory().Contains(Id, ride.id);
}

void Guest::SetHasRiddenRideType(ride_type_t rideType)
{
    OpenRCT2::RideUse::GetTypeHistory().Add(Id, rideType);
}

bool Guest::HasRiddenRideType(ride_type_t rideType) const
{
    return OpenRCT2::RideUse::GetTypeHistory().Contains(Id, rideType);
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
    if (HasItem(ShopItem::Umbrella) && ride.GetRideTypeDescriptor().HasFlag(RtdFlag::guestsCanUseUmbrella)
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
        PeepResetRideHeading(this);
    }
}

void Guest::ReadMap()
{
    if (IsActionInterruptable() && !IsOnLevelCrossing())
    {
        Action = PeepActionType::ReadMap;
        AnimationFrameNum = 0;
        AnimationImageIdOffset = 0;
        UpdateCurrentAnimationType();
    }
}

static bool PeepHasVoucherForFreeRide(Guest* peep, const Ride& ride)
{
    return peep->HasItem(ShopItem::Voucher) && peep->VoucherType == VOUCHER_TYPE_RIDE_FREE && peep->VoucherRideId == ride.id;
}

/**
 * When the queue is full, peeps will ignore the ride when thinking about what to go on next.
 * Does not effect peeps that walk up to the queue entrance.
 * This flag is reset the next time a peep successfully joins the queue.
 */
static void PeepTriedToEnterFullQueue(Guest* peep, Ride& ride)
{
    ride.lifecycle_flags |= RIDE_LIFECYCLE_QUEUE_FULL;
    peep->PreviousRide = ride.id;
    peep->PreviousRideTimeOut = 0;
    // Change status "Heading to" to "Walking" if queue is full
    if (ride.id == peep->GuestHeadingToRideId)
    {
        PeepResetRideHeading(peep);
    }
}

static void PeepResetRideHeading(Guest* peep)
{
    peep->GuestHeadingToRideId = RideId::GetNull();
    peep->WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_ACTION;
}

static void PeepRideIsTooIntense(Guest* peep, Ride& ride, bool peepAtRide)
{
    if (peepAtRide)
    {
        peep->InsertNewThought(PeepThoughtType::Intense, ride.id);
        if (peep->HappinessTarget >= 64)
        {
            peep->HappinessTarget -= 8;
        }
        ride.UpdatePopularity(0);
    }
    peep->ChoseNotToGoOnRide(ride, peepAtRide, true);
}

/**
 *
 *  rct2: 0x00691C6E
 */
static Vehicle* PeepChooseCarFromRide(Peep* peep, const Ride& ride, std::span<const uint8_t> carArray)
{
    uint8_t chosen_car = ScenarioRand();
    if (ride.GetRideTypeDescriptor().HasFlag(RtdFlag::hasGForces) && ((chosen_car & 0xC) != 0xC))
    {
        chosen_car = (ScenarioRand() & 1) ? 0 : static_cast<uint8_t>(carArray.size()) - 1;
    }
    else
    {
        chosen_car = (chosen_car * static_cast<uint16_t>(carArray.size())) >> 8;
    }

    peep->CurrentCar = carArray[chosen_car];

    Vehicle* vehicle = GetEntity<Vehicle>(ride.vehicles[peep->CurrentTrain]);
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
static void PeepChooseSeatFromCar(Peep* peep, const Ride& ride, Vehicle* vehicle)
{
    if (vehicle == nullptr)
    {
        return;
    }
    uint8_t chosen_seat = vehicle->next_free_seat;

    if (ride.mode == RideMode::ForwardRotation || ride.mode == RideMode::BackwardRotation)
    {
        chosen_seat = (((~vehicle->Pitch + 1) >> 3) & 0xF) * 2;
        if (vehicle->next_free_seat & 1)
        {
            chosen_seat++;
        }
    }
    peep->CurrentSeat = chosen_seat;
    vehicle->next_free_seat++;

    vehicle->peep[peep->CurrentSeat] = peep->Id;
    vehicle->peep_tshirt_colours[peep->CurrentSeat] = peep->TshirtColour;
}

/**
 *
 *  rct2: 0x00691D27
 */
void Guest::GoToRideEntrance(const Ride& ride)
{
    const auto& station = ride.GetStation(CurrentRideStation);
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
        if (rideEntry->Cars[rideEntry->DefaultCar].flags & CAR_ENTRY_FLAG_MINI_GOLF
            || rideEntry->Cars[rideEntry->DefaultCar].flags & (CAR_ENTRY_FLAG_CHAIRLIFT | CAR_ENTRY_FLAG_GO_KART))
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

static bool FindVehicleToEnter(
    Guest& guest, const Ride& ride, sfl::static_vector<uint8_t, OpenRCT2::Limits::kMaxTrainsPerRide>& car_array)
{
    uint8_t chosen_train = RideStation::kNoTrain;

    if (ride.mode == RideMode::Dodgems || ride.mode == RideMode::Race)
    {
        if (ride.lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
            return false;

        for (int32_t i = 0; i < ride.NumTrains; ++i)
        {
            Vehicle* vehicle = GetEntity<Vehicle>(ride.vehicles[i]);
            if (vehicle == nullptr)
                continue;

            if (vehicle->next_free_seat >= vehicle->num_seats)
                continue;

            if (vehicle->status != Vehicle::Status::WaitingForPassengers)
                continue;
            chosen_train = i;
            break;
        }
    }
    else
    {
        chosen_train = ride.GetStation(guest.CurrentRideStation).TrainAtStation;
    }
    if (chosen_train >= OpenRCT2::Limits::kMaxTrainsPerRide)
    {
        return false;
    }

    guest.CurrentTrain = chosen_train;

    int32_t i = 0;

    auto vehicle_id = ride.vehicles[chosen_train];
    for (Vehicle* vehicle = GetEntity<Vehicle>(vehicle_id); vehicle != nullptr;
         vehicle = GetEntity<Vehicle>(vehicle->next_vehicle_on_train), ++i)
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

        if (ride.mode == RideMode::ForwardRotation || ride.mode == RideMode::BackwardRotation)
        {
            uint8_t position = (((~vehicle->Pitch + 1) >> 3) & 0xF) * 2;
            if (!vehicle->peep[position].IsNull())
                continue;
        }
        car_array.push_back(i);
    }

    return !car_array.empty();
}

static void PeepUpdateRideAtEntranceTryLeave(Guest* peep)
{
    // Destination Tolerance is zero when peep has completely
    // entered entrance
    if (peep->DestinationTolerance == 0)
    {
        peep->RemoveFromQueue();
        peep->SetState(PeepState::Falling);
    }
}

static bool PeepCheckRidePriceAtEntrance(Guest* peep, const Ride& ride, money64 ridePrice)
{
    if ((peep->HasItem(ShopItem::Voucher)) && peep->VoucherType == VOUCHER_TYPE_RIDE_FREE
        && peep->VoucherRideId == peep->CurrentRide)
        return true;

    if (peep->CashInPocket <= 0 && !(GetGameState().Park.Flags & PARK_FLAGS_NO_MONEY))
    {
        peep->InsertNewThought(PeepThoughtType::SpentMoney);
        PeepUpdateRideAtEntranceTryLeave(peep);
        return false;
    }

    if (ridePrice > peep->CashInPocket)
    {
        // Prevent looping of same thought / animation since Destination Tolerance
        // is only 0 exactly at entrance and will immediately change as guest
        // tries to leave hereafter
        if (peep->DestinationTolerance == 0)
        {
            peep->InsertNewThought(PeepThoughtType::CantAffordRide, peep->CurrentRide);
        }
        PeepUpdateRideAtEntranceTryLeave(peep);
        return false;
    }

    auto value = ride.value;
    if (value != RIDE_VALUE_UNDEFINED)
    {
        if (((value * 2) < ridePrice) && !(GetGameState().Cheats.ignorePrice))
        {
            peep->InsertNewThought(PeepThoughtType::BadValue, peep->CurrentRide);
            PeepUpdateRideAtEntranceTryLeave(peep);
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
static int16_t PeepCalculateRideSatisfaction(Guest* peep, const Ride& ride)
{
    int16_t satisfaction = PeepCalculateRideValueSatisfaction(peep, ride);
    satisfaction += PeepCalculateRideIntensityNauseaSatisfaction(peep, ride);

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
    if (peep->HasRiddenRideType(ride.type))
        satisfaction += 10;

    if (peep->HasRidden(*GetRide(peep->CurrentRide)))
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
static void GuestUpdateFavouriteRide(Guest& peep, const Ride& ride, uint8_t satisfaction)
{
    peep.PeepFlags &= ~PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE;
    uint8_t peepRideRating = std::clamp((ride.ratings.excitement / 4) + satisfaction, 0, kPeepMaxHappiness);
    if (peepRideRating >= peep.FavouriteRideRating)
    {
        if (peep.Happiness >= 160 && peep.HappinessTarget >= 160)
        {
            peep.FavouriteRideRating = peepRideRating;
            peep.PeepFlags |= PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE;
        }
    }
}

/* rct2: 0x00695555 */
static int16_t PeepCalculateRideValueSatisfaction(Guest* peep, const Ride& ride)
{
    if (GetGameState().Park.Flags & PARK_FLAGS_NO_MONEY)
    {
        return -30;
    }

    if (ride.value == RIDE_VALUE_UNDEFINED)
    {
        return -30;
    }

    auto ridePrice = RideGetPrice(ride);
    if (ride.value >= ridePrice)
    {
        return -5;
    }

    if ((ride.value + ((ride.value * peep->Happiness) / 256)) >= ridePrice)
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
static int16_t PeepCalculateRideIntensityNauseaSatisfaction(Guest* peep, const Ride& ride)
{
    if (!RideHasRatings(ride))
    {
        return 70;
    }

    uint8_t intensitySatisfaction = 3;
    uint8_t nauseaSatisfaction = 3;
    ride_rating maxIntensity = peep->Intensity.GetMaximum() * 100;
    ride_rating minIntensity = peep->Intensity.GetMinimum() * 100;
    if (minIntensity <= ride.ratings.intensity && maxIntensity >= ride.ratings.intensity)
    {
        intensitySatisfaction--;
    }
    minIntensity -= peep->Happiness * 2;
    maxIntensity += peep->Happiness;
    if (minIntensity <= ride.ratings.intensity && maxIntensity >= ride.ratings.intensity)
    {
        intensitySatisfaction--;
    }
    minIntensity -= peep->Happiness * 2;
    maxIntensity += peep->Happiness;
    if (minIntensity <= ride.ratings.intensity && maxIntensity >= ride.ratings.intensity)
    {
        intensitySatisfaction--;
    }

    // Although it's not shown in the interface, a peep with Average or High nausea tolerance
    // has a minimum preferred nausea value. (For peeps with None or Low, this is set to zero.)
    ride_rating minNausea = kNauseaMinimumThresholds[(EnumValue(peep->NauseaTolerance) & 3)];
    ride_rating maxNausea = NauseaMaximumThresholds[(EnumValue(peep->NauseaTolerance) & 3)];
    if (minNausea <= ride.ratings.nausea && maxNausea >= ride.ratings.nausea)
    {
        nauseaSatisfaction--;
    }
    minNausea -= peep->Happiness * 2;
    maxNausea += peep->Happiness;
    if (minNausea <= ride.ratings.nausea && maxNausea >= ride.ratings.nausea)
    {
        nauseaSatisfaction--;
    }
    minNausea -= peep->Happiness * 2;
    maxNausea += peep->Happiness;
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
static void PeepUpdateRideNauseaGrowth(Guest* peep, const Ride& ride)
{
    uint32_t nauseaMultiplier = std::clamp(256 - peep->HappinessTarget, 64, 200);
    uint32_t nauseaGrowthRateChange = (ride.ratings.nausea * nauseaMultiplier) / 512;
    nauseaGrowthRateChange *= std::max(static_cast<uint8_t>(128), peep->Hunger) / 64;
    nauseaGrowthRateChange >>= (EnumValue(peep->NauseaTolerance) & 3);
    peep->NauseaTarget = static_cast<uint8_t>(std::min(peep->NauseaTarget + nauseaGrowthRateChange, 255u));
}

static bool PeepShouldGoOnRideAgain(Guest* peep, const Ride& ride)
{
    if (!ride.GetRideTypeDescriptor().HasFlag(RtdFlag::guestsWillRideAgain))
        return false;
    if (!RideHasRatings(ride))
        return false;
    if (ride.ratings.intensity > RIDE_RATING(10, 00) && !GetGameState().Cheats.ignoreRideIntensity)
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

    uint8_t r = (ScenarioRand() & 0xFF);
    if (r <= 128)
    {
        if (peep->GuestNumRides > 7)
            return false;
        if (r > 64)
            return false;
    }

    return true;
}

static bool PeepShouldPreferredIntensityIncrease(Guest* peep)
{
    if (GetGameState().Park.Flags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES)
        return false;
    if (peep->Happiness < 200)
        return false;

    return (ScenarioRand() & 0xFF) >= static_cast<uint8_t>(peep->Intensity);
}

static bool PeepReallyLikedRide(Guest* peep, const Ride& ride)
{
    if (peep->Happiness < 215)
        return false;
    if (peep->Nausea > 120)
        return false;
    if (!RideHasRatings(ride))
        return false;
    if (ride.ratings.intensity > RIDE_RATING(10, 00) && !GetGameState().Cheats.ignoreRideIntensity)
        return false;
    return true;
}

/**
 *
 *  rct2: 0x0069BC9A
 */
static PeepThoughtType PeepAssessSurroundings(int16_t centre_x, int16_t centre_y, int16_t centre_z)
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
    int16_t final_x = std::min(centre_x + 160, MAXIMUM_MAP_SIZE_BIG);
    int16_t final_y = std::min(centre_y + 160, MAXIMUM_MAP_SIZE_BIG);

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

    if (num_rubbish < 2 && !GetGameState().Cheats.disableLittering)
        // if disable littering cheat is enabled, peeps will not have the "clean and tidy park" thought
        return PeepThoughtType::VeryClean;

    return PeepThoughtType::None;
}

/**
 *
 *  rct2: 0x0068F9A9
 */
static void PeepUpdateHunger(Guest* peep)
{
    if (peep->Hunger >= 3)
    {
        peep->Hunger -= 2;

        peep->EnergyTarget = std::min<uint16_t>(peep->EnergyTarget + 2, kPeepMaxEnergyTarget);
        peep->Toilet = std::min(peep->Toilet + 1, 255);
    }
}

/**
 * Main purpose is to decide when peeps leave the park due to
 * low happiness, low energy and (if appropriate) low money.
 *
 *  rct2: 0x0068F8CD
 */
static void PeepDecideWhetherToLeavePark(Guest* peep)
{
    if (peep->EnergyTarget >= 33)
    {
        peep->EnergyTarget -= 2;
    }

    if (GetGameState().ClimateCurrent.Temperature >= 21 && peep->Thirst >= 5)
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
        if (GetGameState().Park.Flags & PARK_FLAGS_NO_MONEY)
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
    if ((ScenarioRand() & 0xFFFF) > 3276)
    {
        return;
    }

    // In the remaining 5% chance the peep leaves the park.
    PeepLeavePark(peep);
}

/**
 *
 *  rct2: 0x0068F93E
 */
static void PeepLeavePark(Guest* peep)
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

    auto* windowMgr = Ui::GetWindowManager();
    WindowBase* w = windowMgr->FindByNumber(WindowClass::Peep, peep->Id);
    if (w != nullptr)
        w->OnPrepareDraw();
    WindowInvalidateByNumber(WindowClass::Peep, peep->Id);
}

template<typename T>
static void PeepHeadForNearestRide(Guest* peep, bool considerOnlyCloseRides, T predicate)
{
    if (peep->State != PeepState::Sitting && peep->State != PeepState::Watching && peep->State != PeepState::Walking)
    {
        return;
    }
    if (peep->PeepFlags & PEEP_FLAGS_LEAVING_PARK)
        return;
    if (peep->x == kLocationNull)
        return;
    if (!peep->GuestHeadingToRideId.IsNull())
    {
        auto ride = GetRide(peep->GuestHeadingToRideId);
        if (ride != nullptr && predicate(*ride))
        {
            return;
        }
    }

    OpenRCT2::BitSet<OpenRCT2::Limits::kMaxRidesInPark> rideConsideration;
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
        constexpr auto kSearchRadius = 10 * 32;
        int32_t cx = floor2(peep->x, 32);
        int32_t cy = floor2(peep->y, 32);
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
    RideId potentialRides[OpenRCT2::Limits::kMaxRidesInPark];
    size_t numPotentialRides = 0;
    for (auto& ride : GetRideManager())
    {
        if (rideConsideration[ride.id.ToUnderlying()])
        {
            if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_QUEUE_FULL))
            {
                if (peep->ShouldGoOnRide(ride, StationIndex::FromUnderlying(0), false, true))
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

static void PeepHeadForNearestRideWithFlag(Guest* peep, bool considerOnlyCloseRides, RtdFlag rtdFlag)
{
    PeepHeadForNearestRide(
        peep, considerOnlyCloseRides, [rtdFlag](const Ride& ride) { return ride.GetRideTypeDescriptor().HasFlag(rtdFlag); });
}

static void GuestHeadForNearestRideWithSpecialType(Guest& guest, bool considerOnlyCloseRides, RtdSpecialType specialType)
{
    if ((specialType == RtdSpecialType::toilet) && guest.HasFoodOrDrink())
    {
        return;
    }
    PeepHeadForNearestRide(&guest, considerOnlyCloseRides, [specialType](const Ride& ride) {
        return ride.GetRideTypeDescriptor().specialType == specialType;
    });
}

/**
 *
 *  rct2: 0x00699FE3
 * Stops peeps that are having thoughts
 * such as "I'm hungry" after visiting a food shop.
 * Works for Thirst/Hungry/Low Money/Toilet
 */
void Guest::StopPurchaseThought(ride_type_t rideType)
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
        PeepThought* thought = &Thoughts[i];

        if (thought->type == PeepThoughtType::None)
            break;

        if (thought->type != thoughtType)
            continue;

        if (i < kPeepMaxThoughts - 1)
        {
            memmove(thought, thought + 1, sizeof(PeepThought) * (kPeepMaxThoughts - i - 1));
        }

        Thoughts[kPeepMaxThoughts - 1].type = PeepThoughtType::None;

        WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_THOUGHTS;
        i--;
    }
}

/**
 *
 *  rct2: 0x0069AEB7
 */
static bool PeepShouldUseCashMachine(Guest* peep, RideId rideIndex)
{
    if (GetGameState().Park.Flags & PARK_FLAGS_NO_MONEY)
        return false;
    if (peep->PeepFlags & PEEP_FLAGS_LEAVING_PARK)
        return false;
    if (peep->CashInPocket > 20.00_GBP)
        return false;
    if (115 + (ScenarioRand() % 128) > peep->Happiness)
        return false;
    if (peep->Energy < 80)
        return false;

    auto ride = GetRide(rideIndex);
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

    auto ride = GetRide(CurrentRide);
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
        if (rtd.specialType == RtdSpecialType::cashMachine)
        {
            if (CurrentRide != PreviousRide)
            {
                CashInPocket += 50.00_GBP;
            }
            WindowInvalidateByNumber(WindowClass::Peep, Id);
        }
        Orientation ^= 0x10;

        auto destination = CoordsXY{ 16, 16 } + NextLoc;
        SetDestination(destination);
        PeepDirection = DirectionReverse(PeepDirection);

        SetState(PeepState::Walking);
        return;
    }

    bool item_bought = false;

    if (CurrentRide != PreviousRide)
    {
        const auto& rtd = GetRideTypeDescriptor(ride->type);
        if (rtd.specialType == RtdSpecialType::cashMachine)
        {
            item_bought = PeepShouldUseCashMachine(this, CurrentRide);
            if (!item_bought)
            {
                PreviousRide = CurrentRide;
                PreviousRideTimeOut = 0;
            }
            else
            {
                Action = PeepActionType::WithdrawMoney;
                AnimationFrameNum = 0;
                AnimationImageIdOffset = 0;

                UpdateCurrentAnimationType();

                ride->no_primary_items_sold++;
            }
        }
        else
        {
            const auto* ride_type = GetRideEntryByIndex(ride->subtype);
            if (ride_type == nullptr)
            {
                return;
            }
            if (ride_type->shop_item[1] != ShopItem::None)
            {
                auto price = ride->price[1];

                item_bought = DecideAndBuyItem(*ride, ride_type->shop_item[1], price);
                if (item_bought)
                {
                    ride->no_secondary_items_sold++;
                }
            }

            if (!item_bought && ride_type->shop_item[0] != ShopItem::None)
            {
                auto price = ride->price[0];

                item_bought = DecideAndBuyItem(*ride, ride_type->shop_item[0], price);
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
                const auto& station = ride->GetStation(CurrentRideStation);
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

    sfl::static_vector<uint8_t, OpenRCT2::Limits::kMaxTrainsPerRide> carArray;

    if (ride->GetRideTypeDescriptor().HasFlag(RtdFlag::noVehicles))
    {
        if (ride->num_riders >= ride->operation_option)
            return;
    }
    else
    {
        if (!FindVehicleToEnter(*this, *ride, carArray))
            return;
    }

    if (ride->status != RideStatus::Open || ride->vehicle_change_timeout != 0)
    {
        PeepUpdateRideAtEntranceTryLeave(this);
        return;
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
        return;

    auto ridePrice = RideGetPrice(*ride);
    if (ridePrice != 0)
    {
        if (!PeepCheckRidePriceAtEntrance(this, *ride, ridePrice))
            return;
    }

    if (!ride->GetRideTypeDescriptor().HasFlag(RtdFlag::noVehicles))
    {
        Vehicle* vehicle = PeepChooseCarFromRide(this, *ride, carArray);
        PeepChooseSeatFromCar(this, *ride, vehicle);
    }
    GoToRideEntrance(*ride);
}

/** rct2: 0x00981FD4, 0x00981FD6 */
static constexpr CoordsXY _MazeEntranceStart[] = {
    { 8, 8 },
    { 8, 24 },
    { 24, 24 },
    { 24, 8 },
};

void PeepUpdateRideLeaveEntranceMaze(Guest* peep, Ride& ride, CoordsXYZD& entrance_loc)
{
    peep->MazeLastEdge = entrance_loc.direction + 1;

    entrance_loc.x += CoordsDirectionDelta[entrance_loc.direction].x;
    entrance_loc.y += CoordsDirectionDelta[entrance_loc.direction].y;

    uint8_t direction = entrance_loc.direction * 4 + 11;
    if (ScenarioRand() & 0x40)
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

    ride.cur_num_customers++;
    peep->OnEnterRide(ride);
    peep->RideSubState = PeepRideSubState::MazePathfinding;
}

void PeepUpdateRideLeaveEntranceSpiralSlide(Guest* peep, Ride& ride, CoordsXYZD& entrance_loc)
{
    entrance_loc = { ride.GetStation(peep->CurrentRideStation).GetStart(), entrance_loc.direction };

    TileElement* tile_element = RideGetStationStartTrackElement(ride, peep->CurrentRideStation);

    uint8_t direction_track = (tile_element == nullptr ? 0 : tile_element->GetDirection());

    peep->Var37 = (entrance_loc.direction << 2) | (direction_track << 4);

    entrance_loc += kSpiralSlideWalkingPath[peep->Var37];

    peep->SetDestination(entrance_loc);
    peep->CurrentCar = 0;

    ride.cur_num_customers++;
    peep->OnEnterRide(ride);
    peep->RideSubState = PeepRideSubState::ApproachSpiralSlide;
}

void PeepUpdateRideLeaveEntranceDefault(Guest* peep, Ride& ride, CoordsXYZD& entrance_loc)
{
    const auto currentTicks = GetGameState().CurrentTicks;

    // If the ride type was changed guests will become stuck.
    // Inform the player about this if its a new issue or hasn't been addressed within 120 seconds.
    if ((ride.current_issues & RIDE_ISSUE_GUESTS_STUCK) == 0 || currentTicks - ride.last_issue_time > 3000)
    {
        ride.current_issues |= RIDE_ISSUE_GUESTS_STUCK;
        ride.last_issue_time = currentTicks;

        auto ft = Formatter();
        ride.FormatNameTo(ft);
        if (Config::Get().notifications.RideWarnings)
        {
            News::AddItemToQueue(News::ItemType::Ride, STR_GUESTS_GETTING_STUCK_ON_RIDE, peep->CurrentRide.ToUnderlying(), ft);
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
    if (station.Entrance.IsNull())
    {
        return;
    }
    uint8_t direction_entrance = station.Entrance.direction;

    TileElement* tile_element = RideGetStationStartTrackElement(ride, CurrentRideStation);

    uint8_t direction_track = (tile_element == nullptr ? 0 : tile_element->GetDirection());

    auto vehicle = GetEntity<Vehicle>(ride.vehicles[CurrentTrain]);
    if (vehicle == nullptr)
    {
        // TODO: Goto ride exit on failure.
        return;
    }
    const auto* rideEntry = vehicle->GetRideEntry();
    const auto* carEntry = &rideEntry->Cars[vehicle->vehicle_type];

    Var37 = (direction_entrance | GetWaypointedSeatLocation(ride, carEntry, direction_track) * 4) * 4;

    const auto& rtd = ride.GetRideTypeDescriptor();
    CoordsXY waypoint = rtd.GetGuestWaypointLocation(*vehicle, ride, CurrentRideStation);

    const auto waypointIndex = Var37 / 4u;
    if (waypointIndex < carEntry->peep_loading_waypoints.size())
    {
        Guard::Assert(carEntry->peep_loading_waypoints.size() >= static_cast<size_t>(waypointIndex));
        waypoint.x += carEntry->peep_loading_waypoints[waypointIndex][0].x;
        waypoint.y += carEntry->peep_loading_waypoints[waypointIndex][0].y;
    }

    SetDestination(waypoint);
    RideSubState = PeepRideSubState::ApproachVehicleWaypoints;
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

    const auto* rideEntry = ride->GetRideEntry();

    if (auto loc = UpdateAction(xy_distance); loc.has_value())
    {
        uint16_t distanceThreshold = 16;
        if (rideEntry != nullptr)
        {
            uint8_t vehicle = rideEntry->DefaultCar;
            if (rideEntry->Cars[vehicle].flags & CAR_ENTRY_FLAG_MINI_GOLF
                || rideEntry->Cars[vehicle].flags & (CAR_ENTRY_FLAG_CHAIRLIFT | CAR_ENTRY_FLAG_GO_KART))
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

    if (ride->GetRideTypeDescriptor().HasFlag(RtdFlag::noVehicles))
    {
        const auto& station = ride->GetStation(CurrentRideStation);
        auto entranceLocation = station.Entrance.ToCoordsXYZD();
        if (entranceLocation.IsNull())
        {
            return;
        }

        const auto& rtd = GetRideTypeDescriptor(ride->type);
        rtd.UpdateLeaveEntrance(this, *ride, entranceLocation);
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

    rideEntry = vehicle->GetRideEntry();
    if (rideEntry == nullptr)
    {
        return;
    }

    const auto* vehicle_type = &rideEntry->Cars[vehicle->vehicle_type];

    if (vehicle_type->flags & CAR_ENTRY_FLAG_LOADING_WAYPOINTS)
    {
        UpdateRideLeaveEntranceWaypoints(*ride);
        return;
    }

    if (vehicle_type->flags & CAR_ENTRY_FLAG_DODGEM_CAR_PLACEMENT)
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

    RideSubState = PeepRideSubState::ApproachVehicle;
}

/**
 *
 *  rct2: 0x0069321D
 */
static void PeepGoToRideExit(Peep* peep, const Ride& ride, int16_t x, int16_t y, int16_t z, uint8_t exit_direction)
{
    z += ride.GetRideTypeDescriptor().Heights.PlatformHeight;

    peep->MoveTo({ x, y, z });

    Guard::Assert(peep->CurrentRideStation.ToUnderlying() < OpenRCT2::Limits::kMaxStationsPerRide);
    auto exit = ride.GetStation(peep->CurrentRideStation).Exit;
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
        if (carEntry.flags & CAR_ENTRY_FLAG_MINI_GOLF || carEntry.flags & (CAR_ENTRY_FLAG_CHAIRLIFT | CAR_ENTRY_FLAG_GO_KART))
        {
            shift_multiplier = 32;
        }
    }

    x_shift *= shift_multiplier;
    y_shift *= shift_multiplier;

    x -= x_shift;
    y -= y_shift;

    peep->SetDestination({ x, y }, 2);

    peep->Orientation = exit_direction * 8;
    peep->RideSubState = PeepRideSubState::ApproachExit;
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
        if ((HasItem(ShopItem::Voucher)) && (VoucherType == VOUCHER_TYPE_RIDE_FREE) && (VoucherRideId == CurrentRide))
        {
            RemoveItem(ShopItem::Voucher);
            WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
        }
        else
        {
            ride.total_profit = AddClamp<money64>(ride.total_profit, ridePrice);
            ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
            SpendMoney(PaidOnRides, ridePrice, ExpenditureType::ParkRideTickets);
        }
    }

    RideSubState = PeepRideSubState::LeaveEntrance;
    uint8_t queueTime = DaysInQueue;
    if (queueTime < 253)
        queueTime += 3;

    queueTime /= 2;
    auto& station = ride.GetStation(CurrentRideStation);
    if (queueTime != station.QueueTime)
    {
        station.QueueTime = queueTime;
        WindowInvalidateByNumber(WindowClass::Ride, CurrentRide.ToUnderlying());
    }

    if (PeepFlags & PEEP_FLAGS_TRACKING)
    {
        auto ft = Formatter();
        FormatNameTo(ft);
        ride.FormatNameTo(ft);

        StringId msg_string;
        if (ride.GetRideTypeDescriptor().HasFlag(RtdFlag::describeAsInside))
            msg_string = STR_PEEP_TRACKING_PEEP_IS_IN_X;
        else
            msg_string = STR_PEEP_TRACKING_PEEP_IS_ON_X;

        if (Config::Get().notifications.GuestOnRide)
        {
            News::AddItemToQueue(News::ItemType::PeepOnRide, msg_string, Id, ft);
        }
    }

    const auto& rtd = ride.GetRideTypeDescriptor();
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
static void PeepUpdateRideNoFreeVehicleRejoinQueue(Guest* peep, Ride& ride)
{
    TileCoordsXYZD entranceLocation = ride.GetStation(peep->CurrentRideStation).Entrance;

    int32_t x = entranceLocation.x * 32;
    int32_t y = entranceLocation.y * 32;
    if (entranceLocation.direction < DirectionOffsets.size())
    {
        x += 16 - DirectionOffsets[entranceLocation.direction].x * 20;
        y += 16 - DirectionOffsets[entranceLocation.direction].y * 20;
    }

    peep->SetDestination({ x, y }, 2);
    peep->SetState(PeepState::QueuingFront);
    peep->RideSubState = PeepRideSubState::AtEntrance;

    ride.QueueInsertGuestAtFront(peep->CurrentRideStation, peep);
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

    if (ride->GetRideTypeDescriptor().HasFlag(RtdFlag::noVehicles))
    {
        if (ride->status != RideStatus::Open || ride->vehicle_change_timeout != 0 || (++RejoinQueueTimeout) == 0)
        {
            PeepUpdateRideNoFreeVehicleRejoinQueue(this, *ride);
            return;
        }

        UpdateRideFreeVehicleEnterRide(*ride);
        return;
    }

    Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[CurrentTrain]);
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

    if (rideEntry->Cars[0].flags & CAR_ENTRY_FLAG_MINI_GOLF)
    {
        vehicle->mini_golf_flags &= ~MiniGolfFlag::Flag5;

        for (size_t i = 0; i < ride->NumTrains; ++i)
        {
            Vehicle* train = GetEntity<Vehicle>(ride->vehicles[i]);
            if (train == nullptr)
                continue;

            Vehicle* second_vehicle = GetEntity<Vehicle>(train->next_vehicle_on_train);
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

    if (ride->mode == RideMode::ForwardRotation || ride->mode == RideMode::BackwardRotation)
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

    Vehicle* currentTrain = GetEntity<Vehicle>(ride->vehicles[CurrentTrain]);
    if (currentTrain == nullptr)
    {
        return;
    }
    if (ride->status == RideStatus::Open && ++RejoinQueueTimeout != 0 && !currentTrain->HasFlag(VehicleFlags::ReadyToDepart))
    {
        return;
    }

    if (ride->mode != RideMode::ForwardRotation && ride->mode != RideMode::BackwardRotation)
    {
        if (vehicle->next_free_seat - 1 != CurrentSeat)
            return;
    }

    vehicle->next_free_seat--;
    vehicle->peep[CurrentSeat] = EntityId::GetNull();

    PeepUpdateRideNoFreeVehicleRejoinQueue(this, *ride);
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
    auto* ride = GetRide(CurrentRide);
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
                    seatedGuest->MoveTo({ kLocationNull, 0, 0 });
                    seatedGuest->SetState(PeepState::OnRide);
                    seatedGuest->GuestTimeOnRide = 0;
                    seatedGuest->RideSubState = PeepRideSubState::OnRide;
                    seatedGuest->OnEnterRide(*ride);
                }
            }

            vehicle->num_peeps++;
            ride->cur_num_customers++;

            vehicle->ApplyMass(Mass);
            vehicle->Invalidate();

            MoveTo({ kLocationNull, 0, 0 });

            SetState(PeepState::OnRide);

            GuestTimeOnRide = 0;
            RideSubState = PeepRideSubState::OnRide;
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
    auto ride = GetRide(CurrentRide);
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

    AnimationImageIdOffset++;
    if (AnimationImageIdOffset & 3)
        return;

    AnimationImageIdOffset = 0;

    vehicle->num_peeps--;
    vehicle->ApplyMass(-Mass);
    vehicle->Invalidate();

    if (ride_station.ToUnderlying() >= OpenRCT2::Limits::kMaxStationsPerRide)
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

    assert(CurrentRideStation.ToUnderlying() < OpenRCT2::Limits::kMaxStationsPerRide);
    auto& station = ride->GetStation(CurrentRideStation);

    if (!(carEntry->flags & CAR_ENTRY_FLAG_LOADING_WAYPOINTS))
    {
        TileCoordsXYZD exitLocation = station.Exit;
        CoordsXYZD platformLocation;
        platformLocation.z = station.GetBaseZ();

        platformLocation.direction = DirectionReverse(exitLocation.direction);

        if (!ride->GetRideTypeDescriptor().HasFlag(RtdFlag::vehicleIsIntegral))
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

            rideEntry = GetRideEntryByIndex(ride->subtype);

            if (rideEntry != nullptr)
            {
                carEntry = &rideEntry->Cars[rideEntry->DefaultCar];

                if (carEntry->flags & CAR_ENTRY_FLAG_GO_KART)
                {
                    shiftMultiplier = 9;
                }

                if (carEntry->flags & (CAR_ENTRY_FLAG_CHAIRLIFT | CAR_ENTRY_FLAG_GO_KART))
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
                this, *ride, platformLocation.x, platformLocation.y, platformLocation.z, platformLocation.direction);
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

        PeepGoToRideExit(this, *ride, platformLocation.x, platformLocation.y, platformLocation.z, platformLocation.direction);
        return;
    }

    auto exitLocation = station.Exit.ToCoordsXYZD();
    if (exitLocation.IsNull())
    {
        return;
    }

    TileElement* trackElement = RideGetStationStartTrackElement(*ride, CurrentRideStation);

    Direction station_direction = (trackElement == nullptr ? 0 : trackElement->GetDirection());

    vehicle = GetEntity<Vehicle>(ride->vehicles[CurrentTrain]);
    if (vehicle == nullptr)
    {
        return;
    }

    CoordsXYZ waypointLoc;
    const auto& rtd = ride->GetRideTypeDescriptor();
    waypointLoc = { rtd.GetGuestWaypointLocation(*vehicle, *ride, CurrentRideStation),
                    exitLocation.z + ride->GetRideTypeDescriptor().Heights.PlatformHeight };

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

    if (ride->type == RIDE_TYPE_MOTION_SIMULATOR)
        exitWaypointLoc.z += 15;

    MoveTo(exitWaypointLoc);

    if (waypointIndex < carEntry->peep_loading_waypoints.size())
    {
        waypointLoc.x += carEntry->peep_loading_waypoints[waypointIndex][1].x;
        waypointLoc.y += carEntry->peep_loading_waypoints[waypointIndex][1].y;
    }

    SetDestination(waypointLoc, 2);
    RideSubState = PeepRideSubState::ApproachExitWaypoints;
}

/**
 *
 *  rct2: 0x0069376A
 */
void Guest::UpdateRidePrepareForExit()
{
    const auto* ride = GetRide(CurrentRide);
    if (ride == nullptr || CurrentRideStation.ToUnderlying() >= std::size(ride->GetStations()))
        return;

    auto exit = ride->GetStation(CurrentRideStation).Exit;
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

    const auto* rideEntry = ride->GetRideEntry();
    if (rideEntry != nullptr)
    {
        const auto& carEntry = rideEntry->Cars[rideEntry->DefaultCar];
        if (carEntry.flags & (CAR_ENTRY_FLAG_CHAIRLIFT | CAR_ENTRY_FLAG_GO_KART))
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
    auto ride = GetRide(CurrentRide);
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
        if (DecideAndBuyItem(*ride, secondaryItem, ride->price[1]))
        {
            ride->no_secondary_items_sold++;
        }
    }
    RideSubState = PeepRideSubState::LeaveExit;
}
#pragma warning(default : 6011)

CoordsXY GetGuestWaypointLocationDefault(const Vehicle& vehicle, const Ride& ride, const StationIndex& CurrentRideStation)
{
    return ride.GetStation(CurrentRideStation).Start.ToTileCentre();
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

    const auto& rtd = ride->GetRideTypeDescriptor();
    if (auto loc = UpdateAction(xy_distance); loc.has_value())
    {
        rtd.UpdateRideApproachVehicleWaypoints(*this, loc.value(), xy_distance);
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
    int16_t actionZ;
    auto ride = GetRide(guest.CurrentRide);
    // Motion simulators have steps this moves the peeps up the steps
    actionZ = ride->GetStation(guest.CurrentRideStation).GetBaseZ() + 2;

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

        const auto& rtd = ride->GetRideTypeDescriptor();

        CoordsXY targetLoc = rtd.GetGuestWaypointLocation(*vehicle, *ride, CurrentRideStation);
        targetLoc += carEntry.peep_loading_waypoints[carPosition][waypoint];
        SetDestination(targetLoc);
        return;
    }

    Var37 |= 3;

    auto targetLoc = ride->GetStation(CurrentRideStation).Exit.ToCoordsXYZD().ToTileCentre();
    uint8_t exit_direction = DirectionReverse(targetLoc.direction);

    int16_t x_shift = DirectionOffsets[exit_direction].x;
    int16_t y_shift = DirectionOffsets[exit_direction].y;

    int16_t shift_multiplier = 20;

    auto rideEntry = GetRideEntryByIndex(ride->subtype);
    if (rideEntry != nullptr)
    {
        auto carEntry = &rideEntry->Cars[rideEntry->DefaultCar];
        if (carEntry->flags & (CAR_ENTRY_FLAG_CHAIRLIFT | CAR_ENTRY_FLAG_GO_KART))
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
        SetDestination({ 0, 0 });
        Var37 = (Var37 / 4) & 0xC;
        MoveTo({ kLocationNull, y, z });
        return;
    }

    [[maybe_unused]] const auto& rtd = ride->GetRideTypeDescriptor();
    if (waypoint == 2)
    {
        bool lastRide = false;
        if (ride->status != RideStatus::Open)
            lastRide = true;
        else if (CurrentCar++ != 0)
        {
            if (ride->mode == RideMode::SingleRidePerAdmission)
                lastRide = true;
            if (static_cast<uint8_t>(CurrentCar - 1) > (ScenarioRand() & 0xF))
                lastRide = true;
        }

        if (lastRide)
        {
            auto exit = ride->GetStation(CurrentRideStation).Exit;
            waypoint = 1;
            auto directionTemp = exit.direction;
            if (exit.direction == INVALID_DIRECTION)
            {
                directionTemp = 0;
            }
            Var37 = (directionTemp * 4) | (Var37 & 0x30) | waypoint;
            CoordsXY targetLoc = ride->GetStation(CurrentRideStation).Start;

            assert(rtd.specialType == RtdSpecialType::spiralSlide);
            targetLoc += kSpiralSlideWalkingPath[Var37];

            SetDestination(targetLoc);
            RideSubState = PeepRideSubState::LeaveSpiralSlide;
            return;
        }
    }
    waypoint++;
    // Actually increment the real peep waypoint
    Var37++;

    CoordsXY targetLoc = ride->GetStation(CurrentRideStation).Start;

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

    const auto& rtd = ride->GetRideTypeDescriptor();
    if (rtd.specialType != RtdSpecialType::spiralSlide)
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
                ride->slide_peep = Id;
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
                destination = newLocation + kSpiralSlideEndWaypoint[dir];
                SetDestination(destination);

                // Move the peep sprite to just at the end of the slide
                newLocation.x += kSpiralSlideEnd[dir].x;
                newLocation.y += kSpiralSlideEnd[dir].y;

                MoveTo({ newLocation, z });

                Orientation = (Var37 & 0xC) * 2;

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

    targetLoc += kSpiralSlideWalkingPath[Var37];

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
        CoordsXY targetLoc = ride->GetStation(CurrentRideStation).Start;

        [[maybe_unused]] const auto& rtd = ride->GetRideTypeDescriptor();
        assert(rtd.specialType == RtdSpecialType::spiralSlide);
        targetLoc += kSpiralSlideWalkingPath[Var37];

        SetDestination(targetLoc);
        return;
    }

    // Actually force the final waypoint
    Var37 |= 3;

    auto targetLoc = ride->GetStation(CurrentRideStation).Exit.ToCoordsXYZD().ToTileCentre();

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
        if (Energy > 80 && !(PeepFlags & PEEP_FLAGS_SLOW_WALK) && !ClimateIsRaining() && (ScenarioRand() & 0xFFFF) <= 2427)
        {
            Action = PeepActionType::Jump;
            AnimationFrameNum = 0;
            AnimationImageIdOffset = 0;
            UpdateCurrentAnimationType();
        }
    }

    auto targetLoc = GetDestination().ToTileStart();

    auto stationBaseZ = ride->GetStation().GetBaseZ();

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
        return;
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
            MoveTo({ loc.value(), ride->GetStation(CurrentRideStation).GetBaseZ() });
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
            ride->FormatNameTo(ft);

            if (Config::Get().notifications.GuestLeftRide)
            {
                News::AddItemToQueue(News::ItemType::PeepOnRide, STR_PEEP_TRACKING_LEFT_RIDE_X, Id, ft);
            }
        }
    }

    InteractionRideIndex = RideId::GetNull();
    SetState(PeepState::Falling);

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

    RideSubState = PeepRideSubState::InteractShop;
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

    const auto& rtd = ride->GetRideTypeDescriptor();
    if (rtd.specialType == RtdSpecialType::firstAid)
    {
        if (Nausea <= 35)
        {
            RideSubState = PeepRideSubState::LeaveShop;

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
    if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
    {
        OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::ToiletFlush, GetLocation());
    }

    RideSubState = PeepRideSubState::LeaveShop;

    SetDestination({ tileCentreX, tileCentreY }, 3);

    HappinessTarget = std::min(HappinessTarget + 30, kPeepMaxHappiness);
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

    // #11758 Previously SetState(PeepState::Walking) caused Peeps to double-back to exit point of shop
    SetState(PeepState::Falling);

    auto ride = GetRide(CurrentRide);
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

static void PeepUpdateWalkingBreakScenery(Guest* peep);
static bool PeepFindRideToLookAt(Peep* peep, uint8_t edge, RideId* rideToView, uint8_t* rideSeatToView);

/**
 *
 *  rct2: 0x0069030A
 */
void Guest::UpdateWalking()
{
    if (!CheckForPath())
        return;

    const auto currentTicks = GetGameState().CurrentTicks;

    if (!IsOnLevelCrossing())
    {
        if (PeepFlags & PEEP_FLAGS_WAVING && IsActionInterruptable() && (0xFFFF & ScenarioRand()) < 936)
        {
            Action = PeepActionType::Wave2;
            AnimationFrameNum = 0;
            AnimationImageIdOffset = 0;

            UpdateCurrentAnimationType();
        }

        if (PeepFlags & PEEP_FLAGS_PHOTO && IsActionInterruptable() && (0xFFFF & ScenarioRand()) < 936)
        {
            Action = PeepActionType::TakePhoto;
            AnimationFrameNum = 0;
            AnimationImageIdOffset = 0;

            UpdateCurrentAnimationType();
        }

        if (PeepFlags & PEEP_FLAGS_PAINTING && IsActionInterruptable() && (0xFFFF & ScenarioRand()) < 936)
        {
            Action = PeepActionType::DrawPicture;
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
                    Litter::Type::EmptyCan,
                    Litter::Type::Rubbish,
                    Litter::Type::BurgerBox,
                    Litter::Type::EmptyCup,
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
            auto litterType = Litter::Type::Vomit;

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

    uint8_t pathingResult;
    PerformNextAction(pathingResult);
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

    PeepUpdateWalkingBreakScenery(this);

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

    for (; !(edges & (1 << chosen_edge));)
        chosen_edge = (chosen_edge + 1) & 3;

    RideId ride_to_view;
    uint8_t ride_seat_to_view;
    if (IsOnLevelCrossing() || !PeepFindRideToLookAt(this, chosen_edge, &ride_to_view, &ride_seat_to_view))
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

    uint8_t chosen_position = ScenarioRand() & 0x3;

    for (; !(positions_free & (1 << chosen_position));)
        chosen_position = (chosen_position + 1) & 3;

    CurrentRide = ride_to_view;
    CurrentSeat = ride_seat_to_view;
    Var37 = chosen_edge | (chosen_position << 2);

    SetState(PeepState::Watching);
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

void Guest::UpdateWaitingAtCrossing()
{
    if (!IsActionInterruptable())
    {
        UpdateAction();
        Invalidate();
        if (!IsActionWalking())
            return;
    }

    Action = PeepActionType::Idle;
    NextAnimationType = PeepAnimationType::WatchRide;
    SwitchNextAnimationType();

    if (HasFoodOrDrink())
    {
        if ((ScenarioRand() & 0xFFFF) <= 1310)
        {
            Action = PeepActionType::EatFood;
            AnimationFrameNum = 0;
            AnimationImageIdOffset = 0;
        }

        UpdateCurrentAnimationType();

        return;
    }

    if ((ScenarioRand() & 0xFFFF) <= 64)
    {
        Action = PeepActionType::Wave2;
        AnimationFrameNum = 0;
        AnimationImageIdOffset = 0;
    }

    UpdateCurrentAnimationType();
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
        }

        return;
    }

    uint8_t pathingResult;
    PerformNextAction(pathingResult);
    if (!IsActionInterruptable())
        return;
    if (AnimationGroup == PeepAnimationGroup::Normal)
    {
        if (TimeInQueue >= 2000 && (0xFFFF & ScenarioRand()) <= 119)
        {
            // Eat Food/Look at watch
            Action = PeepActionType::EatFood;
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
        if (!(TimeInQueue & 0x3F) && IsActionIdle() && NextAnimationType == PeepAnimationType::WatchRide)
        {
            switch (AnimationGroup)
            {
                case PeepAnimationGroup::IceCream:
                case PeepAnimationGroup::Chips:
                case PeepAnimationGroup::Burger:
                case PeepAnimationGroup::Drink:
                case PeepAnimationGroup::Candyfloss:
                case PeepAnimationGroup::Pizza:
                case PeepAnimationGroup::Popcorn:
                case PeepAnimationGroup::HotDog:
                case PeepAnimationGroup::Tentacle:
                case PeepAnimationGroup::ToffeeApple:
                case PeepAnimationGroup::Doughnut:
                case PeepAnimationGroup::Coffee:
                case PeepAnimationGroup::Chicken:
                case PeepAnimationGroup::Lemonade:
                case PeepAnimationGroup::Pretzel:
                case PeepAnimationGroup::Sujeonggwa:
                case PeepAnimationGroup::Juice:
                case PeepAnimationGroup::FunnelCake:
                case PeepAnimationGroup::Noodles:
                case PeepAnimationGroup::Sausage:
                case PeepAnimationGroup::Soup:
                case PeepAnimationGroup::Sandwich:
                    // Eat food
                    Action = PeepActionType::EatFood;
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
    SetState(PeepState::Falling);

    OutsideOfPark = false;
    ParkEntryTime = GetGameState().CurrentTicks;
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
        uint8_t pathingResult;
        PerformNextAction(pathingResult);
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

    WindowInvalidateByClass(WindowClass::GuestList);
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

        Orientation = (Var37 & 3) * 8;

        Action = PeepActionType::Idle;
        NextAnimationType = PeepAnimationType::WatchRide;

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
            Action = PeepActionType::Idle;
        }
        else
        {
            if (HasFoodOrDrink())
            {
                if ((ScenarioRand() & 0xFFFF) <= 1310)
                {
                    Action = PeepActionType::EatFood;
                    AnimationFrameNum = 0;
                    AnimationImageIdOffset = 0;
                    UpdateCurrentAnimationType();
                    return;
                }
            }

            if ((ScenarioRand() & 0xFFFF) <= 655)
            {
                Action = PeepActionType::TakePhoto;
                AnimationFrameNum = 0;
                AnimationImageIdOffset = 0;
                UpdateCurrentAnimationType();
                return;
            }

            if ((StandingFlags & 1))
            {
                if ((ScenarioRand() & 0xFFFF) <= 655)
                {
                    Action = PeepActionType::Wave;
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

        SetState(PeepState::Walking);
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
        if (ScenarioRand() & 0x8000000)
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
static void PeepUpdateWalkingBreakScenery(Guest* peep)
{
    if (GetGameState().Cheats.disableVandalism)
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

        if ((ScenarioRand() & 0xFFFF) > 3276)
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

        if (inner_peep->x == kLocationNull)
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

    MapInvalidateTileZoom1({ peep->NextLoc, tileElement->GetBaseZ(), tileElement->GetBaseZ() + 32 });

    peep->Angriness = 16;
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
    if (NetworkGetMode() != NETWORK_MODE_NONE)
    {
        if (tileElement->IsGhost())
            return false;
    }

    auto ride = GetRide(tileElement->AsTrack()->GetRideIndex());
    if (ride == nullptr || !ride->IsRide())
    {
        return false;
    }

    // This is most likely to have peeps watch new rides
    if (ride->ratings.isNull())
    {
        return true;
    }

    if (ride->ratings.excitement >= RIDE_RATING(4, 70))
    {
        return true;
    }

    if (ride->ratings.intensity >= RIDE_RATING(4, 50))
    {
        return true;
    }

    if (ride->GetRideTypeDescriptor().HasFlag(RtdFlag::interestingToLookAt))
    {
        if ((ScenarioRand() & 0xFFFF) > 0x3333)
        {
            return false;
        }
    }
    else if (ride->GetRideTypeDescriptor().HasFlag(RtdFlag::slightlyInterestingToLookAt))
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

bool Loc690FD0(Peep* peep, RideId* rideToView, uint8_t* rideSeatToView, TileElement* tileElement)
{
    auto ride = GetRide(tileElement->AsTrack()->GetRideIndex());
    if (ride == nullptr)
        return false;

    *rideToView = ride->id;
    if (ride->ratings.isNull())
    {
        *rideSeatToView = 1;
        if (ride->status != RideStatus::Open)
        {
            if (tileElement->GetClearanceZ() > peep->NextLoc.z + (8 * kCoordsZStep))
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
            if (tileElement->GetClearanceZ() > peep->NextLoc.z + (8 * kCoordsZStep))
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
static bool PeepFindRideToLookAt(Peep* peep, uint8_t edge, RideId* rideToView, uint8_t* rideSeatToView)
{
    TileElement* tileElement;

    auto surfaceElement = MapGetSurfaceElementAt(peep->NextLoc);

    tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    if (tileElement == nullptr)
    {
        return false;
    }

    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (NetworkGetMode() != NETWORK_MODE_NONE)
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
        if (peep->NextLoc.z + (4 * kCoordsZStep) <= tileElement->GetBaseZ())
            continue;
        if (peep->NextLoc.z + (1 * kCoordsZStep) >= tileElement->GetClearanceZ())
            continue;

        return false;
    } while (!(tileElement++)->IsLastForTile());

    uint16_t x = peep->NextLoc.x + CoordsDirectionDelta[edge].x;
    uint16_t y = peep->NextLoc.y + CoordsDirectionDelta[edge].y;
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
        if (NetworkGetMode() != NETWORK_MODE_NONE)
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
        if (peep->NextLoc.z + (4 * kCoordsZStep) >= tileElement->GetBaseZ())
            continue;
        if (peep->NextLoc.z + (1 * kCoordsZStep) >= tileElement->GetClearanceZ())
            continue;

        return false;
    } while (!(tileElement++)->IsLastForTile());

    // TODO: Extract loop B
    tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (NetworkGetMode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }

        if (tileElement->GetClearanceZ() + (1 * kCoordsZStep) < peep->NextLoc.z)
            continue;
        if (peep->NextLoc.z + (6 * kCoordsZStep) < tileElement->GetBaseZ())
            continue;

        if (tileElement->GetType() == TileElementType::Track)
        {
            if (PeepShouldWatchRide(tileElement))
            {
                return Loc690FD0(peep, rideToView, rideSeatToView, tileElement);
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
            if (tileElement->GetClearanceZ() >= peep->NextLoc.z + (8 * kCoordsZStep))
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
        if (NetworkGetMode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetClearanceZ() + (1 * kCoordsZStep) < peep->NextLoc.z)
            continue;
        if (peep->NextLoc.z + (6 * kCoordsZStep) < tileElement->GetBaseZ())
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
        if (NetworkGetMode() != NETWORK_MODE_NONE)
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
        if (peep->NextLoc.z + (6 * kCoordsZStep) <= tileElement->GetBaseZ())
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
        if (NetworkGetMode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetClearanceZ() + (1 * kCoordsZStep) < peep->NextLoc.z)
            continue;
        if (peep->NextLoc.z + (8 * kCoordsZStep) < tileElement->GetBaseZ())
            continue;

        if (tileElement->GetType() == TileElementType::Track)
        {
            if (PeepShouldWatchRide(tileElement))
            {
                return Loc690FD0(peep, rideToView, rideSeatToView, tileElement);
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
            if (tileElement->GetClearanceZ() >= peep->NextLoc.z + (8 * kCoordsZStep))
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
        if (NetworkGetMode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetClearanceZ() + (1 * kCoordsZStep) < peep->NextLoc.z)
            continue;
        if (peep->NextLoc.z + (8 * kCoordsZStep) < tileElement->GetBaseZ())
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
        if (NetworkGetMode() != NETWORK_MODE_NONE)
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
        if (peep->NextLoc.z + (8 * kCoordsZStep) <= tileElement->GetBaseZ())
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
        if (NetworkGetMode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetClearanceZ() + (1 * kCoordsZStep) < peep->NextLoc.z)
            continue;
        if (peep->NextLoc.z + (10 * kCoordsZStep) < tileElement->GetBaseZ())
            continue;

        if (tileElement->GetType() == TileElementType::Track)
        {
            if (PeepShouldWatchRide(tileElement))
            {
                return Loc690FD0(peep, rideToView, rideSeatToView, tileElement);
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
            if (tileElement->GetClearanceZ() >= peep->NextLoc.z + (8 * kCoordsZStep))
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
        Action = PeepActionType::Walking;

    auto& objManager = GetContext()->GetObjectManager();
    auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(AnimationObjectIndex);

    PeepFlags &= ~PEEP_FLAGS_SLOW_WALK;
    if (animObj->IsSlowWalking(new_sprite_type))
    {
        PeepFlags |= PEEP_FLAGS_SLOW_WALK;
    }

    AnimationType = PeepAnimationType::Invalid;
    UpdateCurrentAnimationType();

    if (State == PeepState::Sitting)
    {
        Action = PeepActionType::Idle;
        NextAnimationType = PeepAnimationType::SittingIdle;
        SwitchNextAnimationType();
    }
    if (State == PeepState::Watching)
    {
        Action = PeepActionType::Idle;
        NextAnimationType = PeepAnimationType::WatchRide;
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
    { ShopItem::IceCream,         PeepAnimationGroup::IceCream    },
    { ShopItem::Chips,            PeepAnimationGroup::Chips       },
    { ShopItem::Pizza,            PeepAnimationGroup::Pizza       },
    { ShopItem::Burger,           PeepAnimationGroup::Burger      },
    { ShopItem::Drink,            PeepAnimationGroup::Drink       },
    { ShopItem::Coffee,           PeepAnimationGroup::Coffee      },
    { ShopItem::Chicken,          PeepAnimationGroup::Chicken     },
    { ShopItem::Lemonade,         PeepAnimationGroup::Lemonade    },
    { ShopItem::Candyfloss,       PeepAnimationGroup::Candyfloss  },
    { ShopItem::Popcorn,          PeepAnimationGroup::Popcorn     },
    { ShopItem::HotDog,           PeepAnimationGroup::HotDog      },
    { ShopItem::Tentacle,         PeepAnimationGroup::Tentacle    },
    { ShopItem::ToffeeApple,      PeepAnimationGroup::ToffeeApple },
    { ShopItem::Doughnut,         PeepAnimationGroup::Doughnut    },
    { ShopItem::Pretzel,          PeepAnimationGroup::Pretzel     },
    { ShopItem::Cookie,           PeepAnimationGroup::Pretzel     },
    { ShopItem::Chocolate,        PeepAnimationGroup::Coffee      },
    { ShopItem::IcedTea,          PeepAnimationGroup::Coffee      },
    { ShopItem::FunnelCake,       PeepAnimationGroup::FunnelCake  },
    { ShopItem::BeefNoodles,      PeepAnimationGroup::Noodles     },
    { ShopItem::FriedRiceNoodles, PeepAnimationGroup::Noodles     },
    { ShopItem::WontonSoup,       PeepAnimationGroup::Soup        },
    { ShopItem::MeatballSoup,     PeepAnimationGroup::Soup        },
    { ShopItem::FruitJuice,       PeepAnimationGroup::Juice       },
    { ShopItem::SoybeanMilk,      PeepAnimationGroup::Sujeonggwa   },
    { ShopItem::Sujeonggwa,       PeepAnimationGroup::Sujeonggwa   },
    { ShopItem::SubSandwich,      PeepAnimationGroup::Sandwich    },
    { ShopItem::RoastSausage,     PeepAnimationGroup::Sausage     },
    { ShopItem::Balloon,          PeepAnimationGroup::Balloon     },
    { ShopItem::Hat,              PeepAnimationGroup::Hat         },
    { ShopItem::Sunglasses,       PeepAnimationGroup::Sunglasses  },
};
// clang-format on

/**
 *
 *  rct2: 0x0069B8CC
 */
void Guest::UpdateAnimationGroup()
{
    if (AnimationGroup == PeepAnimationGroup::Balloon && (ScenarioRand() & 0xFFFF) <= 327)
    {
        bool isBalloonPopped = false;
        if (x != kLocationNull)
        {
            if ((ScenarioRand() & 0xFFFF) <= 13107)
            {
                isBalloonPopped = true;
                OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::BalloonPop, { x, y, z });
            }
            Balloon::Create({ x, y, z + 9 }, BalloonColour, isBalloonPopped);
        }
        RemoveItem(ShopItem::Balloon);
        WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
    }

    const bool isPrecipitating = ClimateIsRaining() || ClimateIsSnowingHeavily();
    if (isPrecipitating && (HasItem(ShopItem::Umbrella)) && x != kLocationNull)
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
                    SetAnimationGroup(PeepAnimationGroup::Umbrella);
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

    if (State == PeepState::Watching && StandingFlags & (1 << 1))
    {
        SetAnimationGroup(PeepAnimationGroup::Watching);
        return;
    }

    if (Nausea > 170)
    {
        SetAnimationGroup(PeepAnimationGroup::VeryNauseous);
        return;
    }

    if (Nausea > 140)
    {
        SetAnimationGroup(PeepAnimationGroup::Nauseous);
        return;
    }

    if (Energy <= 64 && Happiness < 128)
    {
        SetAnimationGroup(PeepAnimationGroup::HeadDown);
        return;
    }

    if (Energy <= 80 && Happiness < 128)
    {
        SetAnimationGroup(PeepAnimationGroup::ArmsCrossed);
        return;
    }

    if (Toilet > 220)
    {
        SetAnimationGroup(PeepAnimationGroup::RequireToilet);
        return;
    }

    SetAnimationGroup(PeepAnimationGroup::Normal);
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
            ride->FormatNameTo(ft);
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
    if (newAction != PeepActionType::Walking && IsActionInterruptable())
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
static constexpr uint8_t kTrouserColours[] = {
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
    COLOUR_DARK_OLIVE_DARK,
    COLOUR_OLIVE_DARK,
    COLOUR_AQUA_DARK,
    COLOUR_DULL_BROWN_DARK,
};

/** rct2: 0x009823D5 */
static constexpr uint8_t kTshirtColours[] = {
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
    COLOUR_DARK_OLIVE_DARK,
    COLOUR_DARK_OLIVE_LIGHT,
    COLOUR_SATURATED_BROWN_LIGHT,
    COLOUR_BORDEAUX_RED_DARK,
    COLOUR_BORDEAUX_RED_LIGHT,
    COLOUR_GRASS_GREEN_DARK,
    COLOUR_GRASS_GREEN_LIGHT,
    COLOUR_OLIVE_DARK,
    COLOUR_OLIVE_LIGHT,
    COLOUR_SATURATED_GREEN_LIGHT,
    COLOUR_TAN_DARK,
    COLOUR_TAN_LIGHT,
    COLOUR_DULL_PURPLE_LIGHT,
    COLOUR_DULL_GREEN_DARK,
    COLOUR_DULL_GREEN_LIGHT,
    COLOUR_SATURATED_PURPLE_DARK,
    COLOUR_SATURATED_PURPLE_LIGHT,
    COLOUR_ORANGE_LIGHT,
    COLOUR_AQUA_DARK,
    COLOUR_MAGENTA_LIGHT,
    COLOUR_DULL_BROWN_DARK,
    COLOUR_DULL_BROWN_LIGHT,
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

    auto& gameState = GetGameState();
    Guest* peep = CreateEntity<Guest>();

    peep->AnimationObjectIndex = findPeepAnimationsIndexForType(AnimationPeepType::Guest);
    peep->AnimationGroup = PeepAnimationGroup::Normal;
    peep->OutsideOfPark = true;
    peep->State = PeepState::Falling;
    peep->Action = PeepActionType::Walking;
    peep->SpecialSprite = 0;
    peep->AnimationImageIdOffset = 0;
    peep->WalkingAnimationFrameNum = 0;
    peep->AnimationType = PeepAnimationType::Walking;
    peep->PeepFlags = 0;
    peep->FavouriteRide = RideId::GetNull();
    peep->FavouriteRideRating = 0;

    auto& objManager = GetContext()->GetObjectManager();
    auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep->AnimationObjectIndex);

    const auto& spriteBounds = animObj->GetSpriteBounds(peep->AnimationGroup, peep->AnimationType);
    peep->SpriteData.Width = spriteBounds.sprite_width;
    peep->SpriteData.HeightMin = spriteBounds.sprite_height_negative;
    peep->SpriteData.HeightMax = spriteBounds.sprite_height_positive;
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
    if (gameState.Park.Flags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES)
    {
        if (gameState.Park.Flags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES)
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
    else if (gameState.Park.Flags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES)
    {
        intensityLowest = 9;
        intensityHighest = 15;
    }

    peep->Intensity = IntensityRange(intensityLowest, intensityHighest);

    uint8_t nauseaTolerance = ScenarioRand() & 0x7;
    if (gameState.Park.Flags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES)
    {
        nauseaTolerance += 4;
    }

    peep->NauseaTolerance = nausea_tolerance_distribution[nauseaTolerance];

    /* Scenario editor limits initial guest happiness to between 37..253.
     * To be on the safe side, assume the value could have been hacked
     * to any value 0..255. */
    peep->Happiness = gameState.GuestInitialHappiness;
    /* Assume a default initial happiness of 0 is wrong and set
     * to 128 (50%) instead. */
    if (gameState.GuestInitialHappiness == 0)
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
    peep->Hunger = gameState.GuestInitialHunger;
    /* Initial value will vary by -15..16 */
    int8_t hungerDelta = (ScenarioRand() & 0x1F) - 15;
    /* Adjust by the delta, clamping at min=0 and max=255. */
    peep->Hunger = std::clamp(peep->Hunger + hungerDelta, 0, kPeepMaxHunger);

    /* Scenario editor limits initial guest thirst to between 37..253.
     * To be on the safe side, assume the value could have been hacked
     * to any value 0..255. */
    peep->Thirst = gameState.GuestInitialThirst;
    /* Initial value will vary by -15..16 */
    int8_t thirstDelta = (ScenarioRand() & 0x1F) - 15;
    /* Adjust by the delta, clamping at min=0 and max=255. */
    peep->Thirst = std::clamp(peep->Thirst + thirstDelta, 0, kPeepMaxThirst);

    peep->Toilet = 0;
    peep->TimeToConsume = 0;

    peep->GuestNumRides = 0;
    peep->PeepId = gameState.NextGuestNumber++;
    peep->Name = nullptr;

    money64 cash = (static_cast<money64>(ScenarioRand() & 0x3) * 100) - 100 + gameState.GuestInitialCash;
    if (cash < 0)
        cash = 0;

    if (gameState.GuestInitialCash == 0.00_GBP)
    {
        cash = 500;
    }

    if (gameState.Park.Flags & PARK_FLAGS_NO_MONEY)
    {
        cash = 0;
    }

    if (gameState.GuestInitialCash == kMoney64Undefined)
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
    auto& hookEngine = OpenRCT2::GetContext()->GetScriptEngine().GetHookEngine();
    if (hookEngine.HasSubscriptions(OpenRCT2::Scripting::HOOK_TYPE::GUEST_GENERATION))
    {
        auto ctx = OpenRCT2::GetContext()->GetScriptEngine().GetContext();

        // Create event args object
        auto obj = OpenRCT2::Scripting::DukObject(ctx);
        obj.Set("id", peep->Id.ToUnderlying());

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

    Action = PeepActionType::Idle;
    NextAnimationType = PeepAnimationType::WatchRide;
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
    auto ride = GetRide(CurrentRide);
    if (ride == nullptr)
        return;

    auto& station = ride->GetStation(CurrentRideStation);
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

    auto* otherGuest = GetEntity<Guest>(station.LastPeepInQueue);
    if (otherGuest == nullptr)
    {
        LOG_ERROR("Invalid Guest Queue list!");
        return;
    }
    for (; otherGuest != nullptr; otherGuest = GetEntity<Guest>(otherGuest->GuestNextInQueue))
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
        lastEntry.item = kPeepThoughtItemNone;
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
