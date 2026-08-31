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
#include "../peep/PeepAnimations.h"
#include "../peep/PeepThoughts.h"
#include "../peep/RideUseSystem.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/RideManager.hpp"
#include "../ride/ShopItem.h"
#include "../ride/Station.h"
#include "../ride/Vehicle.h"
#include "../scenario/Scenario.h"
#include "../scripting/HookEngine.h"
#include "../scripting/ScriptEngine.h"
#include "../ui/WindowManager.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Footpath.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "../world/TileElementsView.h"
#include "../world/Weather.h"
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

namespace OpenRCT2
{
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
        PeepThoughtType::lost,
        PeepThoughtType::tired,
        PeepThoughtType::badLitter,
        PeepThoughtType::hungry,
        PeepThoughtType::thirsty,
        PeepThoughtType::veryClean,
        PeepThoughtType::crowded,
        PeepThoughtType::scenery,
        PeepThoughtType::veryClean,
        PeepThoughtType::music,
        PeepThoughtType::watched,
        PeepThoughtType::notHungry,
        PeepThoughtType::notThirsty,
        PeepThoughtType::toilet,
        PeepThoughtType::none,
        PeepThoughtType::none,
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
    bool EntityBase::is<Guest>() const
    {
        return type == EntityType::guest;
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
        const auto guestLoc = guest.getLocation();
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

    void Guest::givePassingGuestPurpleClothes(Guest& passingPeep)
    {
        passingPeep.tShirtColour = Drawing::Colour::brightPurple;
        passingPeep.trousersColour = Drawing::Colour::brightPurple;
        passingPeep.invalidate();
    }

    void Guest::givePassingGuestPizza(Guest& passingPeep)
    {
        if (passingPeep.hasItem(ShopItem::pizza))
            return;

        passingPeep.giveItem(ShopItem::pizza);

        int32_t peepDirectionReversed = (orientation >> 3) ^ 2;
        int32_t otherPeepOppositeDirection = passingPeep.orientation >> 3;
        if (peepDirectionReversed == otherPeepOppositeDirection)
        {
            if (passingPeep.isActionInterruptableSafely())
            {
                passingPeep.action = PeepActionType::wave2;
                passingPeep.animationFrameNum = 0;
                passingPeep.animationImageIdOffset = 0;
                passingPeep.updateCurrentAnimationType();
            }
        }
    }

    void Guest::makePassingGuestSick(Guest& passingPeep)
    {
        if (passingPeep.state != PeepState::walking)
            return;

        if (passingPeep.isActionInterruptableSafely())
        {
            passingPeep.action = PeepActionType::throwUp;
            passingPeep.animationFrameNum = 0;
            passingPeep.animationImageIdOffset = 0;
            passingPeep.updateCurrentAnimationType();
        }
    }

    void Guest::givePassingPeepsIceCream(Guest& passingPeep)
    {
        if (passingPeep.hasItem(ShopItem::iceCream))
            return;

        passingPeep.giveItem(ShopItem::iceCream);
        passingPeep.updateAnimationGroup();
    }

    /**
     *
     *  rct2: 0x0068FD3A
     */
    void Guest::updateEasterEggInteractions()
    {
        if (peepFlags.has(PeepFlag::purple))
        {
            ApplyEasterEggToNearbyGuests<&Guest::givePassingGuestPurpleClothes, true>(*this);
        }

        if (peepFlags.has(PeepFlag::pizza))
        {
            ApplyEasterEggToNearbyGuests<&Guest::givePassingGuestPizza, true>(*this);
        }

        if (peepFlags.has(PeepFlag::contagious))
        {
            ApplyEasterEggToNearbyGuests<&Guest::makePassingGuestSick, false>(*this);
        }

        if (peepFlags.has(PeepFlag::iceCream))
        {
            ApplyEasterEggToNearbyGuests<&Guest::givePassingPeepsIceCream, false>(*this);
        }

        if (peepFlags.has(PeepFlag::joy))
        {
            if ((ScenarioRand() & 0xFFFF) <= 1456)
            {
                if (isActionInterruptableSafely())
                {
                    action = PeepActionType::joy;
                    animationFrameNum = 0;
                    animationImageIdOffset = 0;
                    updateCurrentAnimationType();
                }
            }
        }
    }

    int32_t Guest::getEasterEggNameId() const
    {
        char buffer[256]{};

        Formatter ft;
        formatNameTo(ft);
        FormatStringLegacy(buffer, sizeof(buffer), STR_STRINGID, ft.Data());

        for (uint32_t i = 0; i < std::size(gPeepEasterEggNames); i++)
        {
            if (String::iequals(buffer, gPeepEasterEggNames[i]))
                return static_cast<int32_t>(i);
        }

        return -1;
    }

    void Guest::handleEasterEggName()
    {
        peepFlags.set(PeepFlag::waving, checkEasterEggName(EASTEREGG_PEEP_NAME_KATIE_BRAYSHAW));
        peepFlags.set(PeepFlag::photo, checkEasterEggName(EASTEREGG_PEEP_NAME_CHRIS_SAWYER));
        peepFlags.set(PeepFlag::painting, checkEasterEggName(EASTEREGG_PEEP_NAME_SIMON_FOSTER));
        peepFlags.set(PeepFlag::wow, checkEasterEggName(EASTEREGG_PEEP_NAME_JOHN_WARDLEY));

        if (checkEasterEggName(EASTEREGG_PEEP_NAME_MELANIE_WARN))
        {
            happiness = 250;
            happinessTarget = 250;
            energy = 127;
            energyTarget = 127;
            nausea = 0;
            nauseaTarget = 0;
        }

        peepFlags.set(PeepFlag::litter, checkEasterEggName(EASTEREGG_PEEP_NAME_LISA_STIRLING));
        peepFlags.set(PeepFlag::lost, checkEasterEggName(EASTEREGG_PEEP_NAME_DONALD_MACRAE));
        peepFlags.set(PeepFlag::hunger, checkEasterEggName(EASTEREGG_PEEP_NAME_KATHERINE_MCGOWAN));
        peepFlags.set(PeepFlag::toilet, checkEasterEggName(EASTEREGG_PEEP_NAME_FRANCES_MCGOWAN));
        peepFlags.set(PeepFlag::crowded, checkEasterEggName(EASTEREGG_PEEP_NAME_CORINA_MASSOURA));
        peepFlags.set(PeepFlag::happiness, checkEasterEggName(EASTEREGG_PEEP_NAME_CAROL_YOUNG));
        peepFlags.set(PeepFlag::nausea, checkEasterEggName(EASTEREGG_PEEP_NAME_MIA_SHERIDAN));

        if (checkEasterEggName(EASTEREGG_PEEP_NAME_KATIE_RODGER))
        {
            peepFlags.set(PeepFlag::leavingPark);
            peepFlags.unset(PeepFlag::parkEntranceChosen);
        }

        peepFlags.set(PeepFlag::purple, checkEasterEggName(EASTEREGG_PEEP_NAME_EMMA_GARRELL));
        peepFlags.set(PeepFlag::pizza, checkEasterEggName(EASTEREGG_PEEP_NAME_JOANNE_BARTON));
        peepFlags.set(PeepFlag::contagious, checkEasterEggName(EASTEREGG_PEEP_NAME_FELICITY_ANDERSON));
        peepFlags.set(PeepFlag::joy, checkEasterEggName(EASTEREGG_PEEP_NAME_KATIE_SMITH));
        peepFlags.set(PeepFlag::angry, checkEasterEggName(EASTEREGG_PEEP_NAME_EILIDH_BELL));
        peepFlags.set(PeepFlag::iceCream, checkEasterEggName(EASTEREGG_PEEP_NAME_NANCY_STILLWAGON));
        peepFlags.set(PeepFlag::hereWeAre, checkEasterEggName(EASTEREGG_PEEP_NAME_DAVID_ELLIS));
    }

    /**
     *
     *  rct2: 0x0069A5A0
     * tests if a peep's name matches a cheat code, normally returns using a register flag
     */
    bool Guest::checkEasterEggName(int32_t index) const
    {
        char buffer[256]{};

        Formatter ft;
        formatNameTo(ft);
        FormatStringLegacy(buffer, sizeof(buffer), STR_STRINGID, ft.Data());

        return String::iequals(buffer, gPeepEasterEggNames[index]);
    }

    void Guest::updateMotivesIdle()
    {
        // Idle peep happiness tends towards 127 (50%).
        if (happinessTarget >= 128)
            happinessTarget--;
        else
            happinessTarget++;

        nauseaTarget = std::max(nauseaTarget - 2, 0);

        if (energy <= 50)
        {
            happinessTarget = std::max(happinessTarget - 2, 0);
        }

        if (hunger < 10)
        {
            happinessTarget = std::max(happinessTarget - 1, 0);
        }

        if (thirst < 10)
        {
            happinessTarget = std::max(happinessTarget - 1, 0);
        }

        if (toilet >= 195)
        {
            happinessTarget = std::max(happinessTarget - 1, 0);
        }

        if (state == PeepState::walking && nauseaTarget >= 128)
        {
            if ((ScenarioRand() & 0xFF) <= static_cast<uint8_t>((nausea - 128) / 2))
            {
                if (isActionInterruptableSafely())
                {
                    action = PeepActionType::throwUp;
                    animationFrameNum = 0;
                    animationImageIdOffset = 0;
                    updateCurrentAnimationType();
                }
            }
        }
    }

    /* rct2: 0x0068FA89*/
    void Guest::updateConsumptionMotives()
    {
        if (timeToConsume == 0 && hasFoodOrDrink())
        {
            timeToConsume += 3;
        }

        if (timeToConsume != 0 && state != PeepState::onRide)
        {
            timeToConsume = std::max(timeToConsume - 3, 0);

            if (hasDrink())
            {
                thirst = std::min(thirst + 7, 255);
            }
            else
            {
                hunger = std::min(hunger + 7, 255);
                thirst = std::max(thirst - 3, 0);
                toilet = std::min(toilet + 2, 255);
            }

            if (timeToConsume == 0)
            {
                int32_t chosen_food = Numerics::bitScanForward(getFoodOrDrinkFlags());
                if (chosen_food != -1)
                {
                    ShopItem food = ShopItem(chosen_food);
                    removeItem(food);

                    auto discardContainer = GetShopItemDescriptor(food).DiscardContainer;
                    if (discardContainer != ShopItem::none)
                    {
                        giveItem(discardContainer);
                    }

                    windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                    updateAnimationGroup();
                }
            }
        }

        uint8_t newEnergy = energy;
        uint8_t newTargetEnergy = energyTarget;
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

        if (newEnergy != energy)
        {
            energy = newEnergy;
            windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_2;
        }

        uint8_t newHappiness = happiness;
        uint8_t newHappinessGrowth = happinessTarget;
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

        if (newHappiness != happiness)
        {
            happiness = newHappiness;
            windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_2;
        }

        uint8_t newNausea = nausea;
        uint8_t newNauseaGrowth = nauseaTarget;
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

        if (newNausea != nausea)
        {
            nausea = newNausea;
            windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_2;
        }
    }

    void Guest::tick128UpdateGuest(uint32_t index)
    {
        const auto currentTicks = getGameState().currentTicks;
        const bool ticksMatchIndex = (index & 0x1FF) != (currentTicks & 0x1FF);
        if (ticksMatchIndex && !peepFlags.has(PeepFlag::positionFrozen))
        {
            updateConsumptionMotives();
            return;
        }

        /* Effect of masking with 0x1FF here vs mask 0x7F,
         * which is the condition for calling this function, is
         * to reduce how often the content in this conditional
         * is executed to once every four calls. */
        if (peepFlags.has(PeepFlag::crowded))
        {
            PeepThoughtType thought_type = kCrowdedThoughts[ScenarioRand() & 0xF];
            if (thought_type != PeepThoughtType::none)
            {
                insertNewThought(thought_type);
            }
        }

        if (peepFlags.has(PeepFlag::explode) && x != kLocationNull)
        {
            if (state == PeepState::walking || state == PeepState::sitting)
            {
                Audio::Play3D(Audio::SoundId::crash, getLocation());

                ExplosionCloud::create({ x, y, z + 16 });
                ExplosionFlare::create({ x, y, z + 16 });

                remove();
                return;
            }

            peepFlags.unset(PeepFlag::explode);
        }

        if (peepFlags.has(PeepFlag::hunger))
        {
            if (hunger >= 15)
                hunger -= 15;
        }

        if (peepFlags.has(PeepFlag::toilet))
        {
            if (toilet <= 180)
                toilet += 50;
        }

        if (peepFlags.has(PeepFlag::happiness))
        {
            happinessTarget = 5;
        }

        if (peepFlags.has(PeepFlag::nausea))
        {
            nauseaTarget = 200;
            if (nausea <= 130)
                nausea = 130;
        }

        if (angriness != 0)
            angriness--;

        if (state == PeepState::walking || state == PeepState::sitting)
        {
            surroundingsThoughtTimeout++;
            if (surroundingsThoughtTimeout >= 18)
            {
                surroundingsThoughtTimeout = 0;
                if (x != kLocationNull)
                {
                    PeepThoughtType thought_type = GuestAssessSurroundings(x & 0xFFE0, y & 0xFFE0, z);

                    if (thought_type != PeepThoughtType::none)
                    {
                        insertNewThought(thought_type);
                        happinessTarget = std::min(kPeepMaxHappiness, happinessTarget + 45);
                    }
                }
            }
        }

        if (!peepFlags.has(PeepFlag::animationFrozen))
        {
            updateAnimationGroup();
        }

        if (state == PeepState::onRide || state == PeepState::enteringRide)
        {
            guestTimeOnRide = AddClamp<uint8_t>(guestTimeOnRide, 1);

            if (peepFlags.has(PeepFlag::wow))
            {
                insertNewThought(PeepThoughtType::wow2);
            }

            if (guestTimeOnRide > 15)
            {
                happinessTarget = std::max(0, happinessTarget - 5);

                if (guestTimeOnRide > 22)
                {
                    auto ride = GetRide(currentRide);
                    if (ride != nullptr)
                    {
                        PeepThoughtType thought_type = ride->getRideTypeDescriptor().flags.has(RtdFlag::describeAsInside)
                            ? PeepThoughtType::getOut
                            : PeepThoughtType::getOff;

                        insertNewThought(thought_type, currentRide);
                    }
                }
            }
        }

        if (peepFlags.has(PeepFlag::positionFrozen))
        {
            return;
        }

        if (state == PeepState::walking && !outsideOfPark && !peepFlags.has(PeepFlag::leavingPark) && guestNumRides == 0
            && guestHeadingToRideId.IsNull())
        {
            uint32_t time_duration = currentTicks - parkEntryTime;
            time_duration /= 2048;

            if (time_duration >= 5)
            {
                GuestPickRideToGoOn(*this);

                if (guestHeadingToRideId.IsNull())
                {
                    happinessTarget = std::max(happinessTarget - 128, 0);
                    GuestLeavePark(*this);
                    GuestUpdateHunger(*this);
                    updateMotivesIdle();
                    updateConsumptionMotives();
                    return;
                }
            }
        }

        if ((ScenarioRand() & 0xFFFF) <= ((hasItem(ShopItem::map)) ? 8192u : 2184u))
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

            if (!outsideOfPark && (state == PeepState::walking || state == PeepState::sitting))
            {
                uint8_t num_thoughts = 0;
                PeepThoughtType possible_thoughts[5];

                if (peepFlags.has(PeepFlag::leavingPark))
                {
                    possible_thoughts[num_thoughts++] = PeepThoughtType::goHome;
                }
                else
                {
                    if (energy <= 70 && happiness < 128)
                    {
                        possible_thoughts[num_thoughts++] = PeepThoughtType::tired;
                    }

                    if (hunger <= 10 && !hasFoodOrDrink())
                    {
                        possible_thoughts[num_thoughts++] = PeepThoughtType::hungry;
                    }

                    if (thirst <= 25 && !hasFoodOrDrink())
                    {
                        possible_thoughts[num_thoughts++] = PeepThoughtType::thirsty;
                    }

                    if (toilet >= 160)
                    {
                        possible_thoughts[num_thoughts++] = PeepThoughtType::toilet;
                    }

                    if (!getGameState().park.flags.has(ParkFlag::noMoney) && cashInPocket <= 9.00_GBP && happiness >= 105
                        && energy >= 70)
                    {
                        /* The energy check was originally a second check on happiness.
                         * This was superfluous so should probably check something else.
                         * Guessed that this should really be checking energy, since
                         * the addresses for happiness and energy are quite close,
                         * 70 is also the threshold for tired thoughts (see above) and
                         * it makes sense that a tired peep might not think about getting
                         * more money. */
                        possible_thoughts[num_thoughts++] = PeepThoughtType::runningOut;
                    }
                }

                if (num_thoughts != 0)
                {
                    PeepThoughtType chosen_thought = possible_thoughts[ScenarioRand() % num_thoughts];

                    insertNewThought(chosen_thought);

                    switch (chosen_thought)
                    {
                        case PeepThoughtType::hungry:
                            GuestHeadForNearestRideWithFlag(*this, false, RtdFlag::sellsFood);
                            break;
                        case PeepThoughtType::thirsty:
                            GuestHeadForNearestRideWithFlag(*this, false, RtdFlag::sellsDrinks);
                            break;
                        case PeepThoughtType::toilet:
                            GuestHeadForNearestRideWithSpecialType(*this, false, RtdSpecialType::toilet);
                            break;
                        case PeepThoughtType::runningOut:
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
            if (nausea >= 140)
            {
                PeepThoughtType thought_type = PeepThoughtType::sick;
                if (nausea >= 200)
                {
                    thought_type = PeepThoughtType::verySick;
                    GuestHeadForNearestRideWithSpecialType(*this, true, RtdSpecialType::firstAid);
                }
                insertNewThought(thought_type);
            }
        }

        switch (state)
        {
            case PeepState::walking:
            case PeepState::leavingPark:
            case PeepState::enteringPark:
                GuestDecideWhetherToLeavePark(*this);
                GuestUpdateHunger(*this);
                break;

            case PeepState::sitting:
                if (energyTarget <= 135)
                    energyTarget += 5;

                if (thirst >= 5)
                {
                    thirst -= 4;
                    toilet = std::min(255, toilet + 3);
                }

                if (nauseaTarget >= 50)
                    nauseaTarget -= 6;

                // In the original this branched differently
                // but it would mean setting the peep happiness from
                // a thought type entry which i think is incorrect.
                GuestUpdateHunger(*this);
                break;

            case PeepState::queuing:
                if (timeInQueue >= 2000)
                {
                    /* Peep happiness is affected once the peep has been waiting
                     * too long in a queue. */
                    bool found = false;
                    for (auto* pathElement : TileElementsView<PathElement>(nextLoc))
                    {
                        if (pathElement->getBaseZ() != nextLoc.z)
                            continue;

                        // Check if the footpath has a queue line TV monitor on it
                        if (pathElement->hasAddition() && !pathElement->additionIsGhost())
                        {
                            auto* pathAddEntry = pathElement->getAdditionEntry();
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
                        if (happinessTarget < 90)
                            happinessTarget = 90;

                        // This is +2 as UpdateMotivesIdle (that is called later in the function)
                        // will -1. We want to gradually increase happiness to 165
                        if (happinessTarget < 165)
                            happinessTarget += 2;
                    }
                    else
                    {
                        /* Without a queue line TV monitor peeps waiting too long
                         * in a queue get less happy. */
                        happinessTarget = std::max(happinessTarget - 4, 0);
                    }
                }
                GuestUpdateHunger(*this);
                break;
            case PeepState::enteringRide:
                if (subState == 17 || subState == 15)
                {
                    GuestDecideWhetherToLeavePark(*this);
                }
                GuestUpdateHunger(*this);
                break;
            default:
                break;
        }

        updateMotivesIdle();
        updateConsumptionMotives();
    }

    /**
     *
     *  rct2: 0x00691677
     */
    static void GuestTryGetUpFromSitting(Guest& guest)
    {
        // Eats all food first
        if (guest.hasFoodOrDrink())
            return;

        guest.timeToSitdown--;
        if (guest.timeToSitdown)
            return;

        guest.setState(PeepState::walking);

        // Set destination to the centre of the tile.
        const auto destination = guest.getLocation().ToTileCentre();
        guest.setDestination(destination, 5);
        guest.updateCurrentAnimationType();
    }

    /**
     *
     *  rct2: 0x0069152B
     */
    void Guest::updateSitting()
    {
        if (sittingSubState == PeepSittingSubState::tryingToSit)
        {
            if (!checkForPath())
                return;
            // 691541

            const auto [pathingResult, _] = performNextAction();
            if (!(pathingResult & PATHING_DESTINATION_REACHED))
                return;

            auto loc = getLocation().ToTileStart() + CoordsXYZ{ BenchUseOffsets[var37 & 0x7], 0 };

            moveTo(loc);

            orientation = ((var37 + 2) & 3) * 8;
            action = PeepActionType::idle;
            nextAnimationType = PeepAnimationType::sittingIdle;
            switchNextAnimationType();

            sittingSubState = PeepSittingSubState::satDown;

            // Sets time to sit on seat
            timeToSitdown = (129 - energy) * 16 + 50;
        }
        else if (sittingSubState == PeepSittingSubState::satDown)
        {
            if (!isActionInterruptable())
            {
                updateAction();
                if (!isActionWalking())
                    return;

                action = PeepActionType::idle;
                GuestTryGetUpFromSitting(*this);
                return;
            }

            if (peepFlags.has(PeepFlag::leavingPark))
            {
                setState(PeepState::walking);

                // Set destination to the centre of the tile
                auto destination = getLocation().ToTileCentre();
                setDestination(destination, 5);
                updateCurrentAnimationType();
                return;
            }

            if (animationGroup == PeepAnimationGroup::umbrella)
            {
                GuestTryGetUpFromSitting(*this);
                return;
            }

            if (hasFoodOrDrink())
            {
                if ((ScenarioRand() & 0xFFFFu) > 1310u)
                {
                    GuestTryGetUpFromSitting(*this);
                    return;
                }
                action = PeepActionType::sittingEatFood;
                animationFrameNum = 0;
                animationImageIdOffset = 0;
                updateCurrentAnimationType();
                return;
            }

            const auto rand = ScenarioRand();
            if ((rand & 0xFFFFu) > 131u)
            {
                GuestTryGetUpFromSitting(*this);
                return;
            }
            if (animationGroup == PeepAnimationGroup::balloon || animationGroup == PeepAnimationGroup::hat)
            {
                GuestTryGetUpFromSitting(*this);
                return;
            }

            action = PeepActionType::sittingLookAroundLeft;
            if (rand & 0x80000000u)
            {
                action = PeepActionType::sittingLookAroundRight;
            }

            if (rand & 0x40000000u)
            {
                action = PeepActionType::sittingCheckWatch;
            }
            animationFrameNum = 0;
            animationImageIdOffset = 0;
            updateCurrentAnimationType();
        }
    }

    /**
     * To simplify check of 0x36BA3E0 and 0x11FF78
     * returns false on no food.
     */
    uint64_t Guest::getFoodOrDrinkFlags() const
    {
        return getItemFlags() & (ShopItemsGetAllFoods() | ShopItemsGetAllDrinks());
    }

    uint64_t Guest::getEmptyContainerFlags() const
    {
        return getItemFlags() & ShopItemsGetAllContainers();
    }

    bool Guest::hasFoodOrDrink() const
    {
        return getFoodOrDrinkFlags() != 0;
    }

    /**
     * To simplify check of NOT(0x12BA3C0 and 0x118F48)
     * returns 0 on no food.
     */
    bool Guest::hasDrink() const
    {
        return getItemFlags() & ShopItemsGetAllDrinks();
    }

    bool Guest::hasEmptyContainer() const
    {
        return getEmptyContainerFlags() != 0;
    }

    /**
     *
     *  rct2: 0x69C308
     * Check if lost.
     */
    void Guest::checkIfLost()
    {
        if (!peepFlags.has(PeepFlag::lost))
        {
            if (RideGetCount() < 2)
                return;
            peepFlags.flip(PeepFlag::unknown21);

            if (!peepFlags.has(PeepFlag::unknown21))
                return;

            timeLost++;
            if (timeLost != 254)
                return;
            timeLost = 230;
        }
        insertNewThought(PeepThoughtType::lost);

        happinessTarget = std::max(happinessTarget - 30, 0);
    }

    /**
     *
     *  rct2: 0x69C26B
     * Check if can't find ride.
     */
    void Guest::checkCantFindRide()
    {
        if (guestHeadingToRideId.IsNull())
            return;

        // Peeps will think "I can't find ride X" twice before giving up completely.
        if (guestIsLostCountdown == 30 || guestIsLostCountdown == 60)
        {
            insertNewThought(PeepThoughtType::cantFind, guestHeadingToRideId);
            happinessTarget = std::max(happinessTarget - 30, 0);
        }

        guestIsLostCountdown--;
        if (guestIsLostCountdown != 0)
            return;

        guestHeadingToRideId = RideId::GetNull();

        auto* windowMgr = Ui::GetWindowManager();
        WindowBase* w = windowMgr->FindByNumber(WindowClass::peep, id);

        if (w != nullptr)
        {
            w->onPrepareDraw();
        }

        windowMgr->InvalidateByNumber(WindowClass::peep, id);
    }

    /**
     *
     *  rct2: 0x69C2D0
     * Check if can't find exit.
     */
    void Guest::checkCantFindExit()
    {
        if (!peepFlags.has(PeepFlag::leavingPark))
            return;

        // Peeps who can't find the park exit will continue to get less happy until they find it.
        if (guestIsLostCountdown == 1)
        {
            insertNewThought(PeepThoughtType::cantFindExit);
            happinessTarget = std::max(happinessTarget - 30, 0);
        }

        if (--guestIsLostCountdown == 0)
            guestIsLostCountdown = 90;
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
        const bool isPrecipitating = Weather::isPrecipitating();
        const bool isPrecipitatingAndUmbrella = isPrecipitating && (shopItem == ShopItem::umbrella);

        bool hasVoucher = false;
        if ((guest.hasItem(ShopItem::voucher)) && (guest.voucherType == VOUCHER_TYPE_FOOD_OR_DRINK_FREE)
            && (guest.voucherShopItem == shopItem))
        {
            hasVoucher = true;
        }

        if (guest.hasItem(shopItem))
        {
            guest.insertNewThought(PeepThoughtType::alreadyGot, shopItem);
            return false;
        }

        const auto& shopItemDescriptor = GetShopItemDescriptor(shopItem);
        if (shopItemDescriptor.IsFoodOrDrink())
        {
            int32_t food = Numerics::bitScanForward(guest.getFoodOrDrinkFlags());
            if (food != -1)
            {
                guest.insertNewThought(PeepThoughtType::haventFinished, static_cast<ShopItem>(food));
                return false;
            }

            if (guest.nausea >= 145)
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

        if (shopItemDescriptor.IsFood() && (guest.hunger > 75))
        {
            guest.insertNewThought(PeepThoughtType::notHungry);
            return false;
        }

        if (shopItemDescriptor.IsDrink() && (guest.thirst > 75))
        {
            guest.insertNewThought(PeepThoughtType::notThirsty);
            return false;
        }

        if (!isPrecipitatingAndUmbrella && (shopItem != ShopItem::map) && shopItemDescriptor.IsSouvenir() && !hasVoucher)
        {
            if (((ScenarioRand() & 0x7F) + 0x73) > guest.happiness || guest.guestNumRides < 3)
                return false;
        }

        if (!hasVoucher)
        {
            if (price != 0 && !gameState.park.flags.has(ParkFlag::noMoney))
            {
                if (guest.cashInPocket == 0)
                {
                    guest.insertNewThought(PeepThoughtType::spentMoney);
                    return false;
                }
                if (price > guest.cashInPocket)
                {
                    guest.insertNewThought(PeepThoughtType::cantAffordItem, shopItem);
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
                    if (guest.happiness >= 128)
                    {
                        itemValue /= 2;
                        if (guest.happiness >= 180)
                            itemValue /= 2;
                    }
                    if (itemValue > (static_cast<money64>(ScenarioRand() & 0x07)) && !(gameState.cheats.ignorePrice))
                    {
                        // "I'm not paying that much for x"
                        guest.insertNewThought(shopItemDescriptor.TooMuchThought, ride.id);
                        return false;
                    }
                }
            }
            else
            {
                itemValue -= price;
                itemValue = std::max(0.80_GBP, itemValue);

                if (!gameState.park.flags.has(ParkFlag::noMoney))
                {
                    if (itemValue >= static_cast<money64>(ScenarioRand() & 0x07))
                    {
                        // "This x is a really good value"
                        guest.insertNewThought(shopItemDescriptor.GoodValueThought, ride.id);
                    }
                }

                int32_t happinessGrowth = itemValue * 4;
                guest.happinessTarget = std::min((guest.happinessTarget + happinessGrowth), kPeepMaxHappiness);
                guest.happiness = std::min((guest.happiness + happinessGrowth), kPeepMaxHappiness);
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
        guest.giveItem(shopItem);
        const auto hasRandomShopColour = ride.flags.has(RideFlag::randomShopColours);

        switch (shopItem)
        {
            case ShopItem::tShirt:
                guest.tShirtColour = hasRandomShopColour ? Drawing::getRandomColourNetworkSafe() : ride.trackColours[0].main;
                break;
            case ShopItem::hat:
                guest.hatColour = hasRandomShopColour ? Drawing::getRandomColourNetworkSafe() : ride.trackColours[0].main;
                break;
            case ShopItem::balloon:
                guest.balloonColour = hasRandomShopColour ? Drawing::getRandomColourNetworkSafe() : ride.trackColours[0].main;
                break;
            case ShopItem::umbrella:
                guest.umbrellaColour = hasRandomShopColour ? Drawing::getRandomColourNetworkSafe() : ride.trackColours[0].main;
                break;
            case ShopItem::map:
                guest.resetPathfindGoal();
                break;
            case ShopItem::photo:
                guest.photo1RideRef = ride.id;
                break;
            case ShopItem::photo2:
                guest.photo2RideRef = ride.id;
                break;
            case ShopItem::photo3:
                guest.photo3RideRef = ride.id;
                break;
            case ShopItem::photo4:
                guest.photo4RideRef = ride.id;
                break;
            default:
                break;
        }

        guest.windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
        guest.updateAnimationGroup();
        if (guest.peepFlags.has(PeepFlag::tracking))
        {
            auto ft = Formatter();
            guest.formatNameTo(ft);
            ft.Add<StringId>(shopItemDescriptor.Naming.Indefinite);
            if (Config::Get().notifications.guestBoughtItem)
            {
                News::AddItemToQueue(News::ItemType::peepOnRide, STR_PEEP_TRACKING_NOTIFICATION_BOUGHT_X, guest.id, ft);
            }
        }

        money64* expend_type = &guest.paidOnSouvenirs;
        ExpenditureType expenditure = ExpenditureType::shopStock;

        if (shopItemDescriptor.IsFood())
        {
            guest.amountOfFood++;
            expend_type = &guest.paidOnFood;
            expenditure = ExpenditureType::foodDrinkStock;
        }
        else if (shopItemDescriptor.IsDrink())
        {
            guest.amountOfDrinks++;
            expend_type = &guest.paidOnDrink;
            expenditure = ExpenditureType::foodDrinkStock;
        }
        else if (shopItemDescriptor.IsSouvenir())
        {
            guest.amountOfSouvenirs++;
        }

        if (!gameState.park.flags.has(ParkFlag::noMoney))
            FinancePayment(shopItemDescriptor.Cost, expenditure);

        // Sets the expenditure type to *_FOODDRINK_SALES or *_SHOP_SALES appropriately.
        expenditure = static_cast<ExpenditureType>(static_cast<int32_t>(expenditure) - 1);
        if (hasVoucher)
        {
            guest.removeItem(ShopItem::voucher);
            guest.windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
        }
        else if (!gameState.park.flags.has(ParkFlag::noMoney))
        {
            guest.spendMoney(*expend_type, price, expenditure);
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
    void Guest::onEnterRide(Ride& ride)
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
        if (guestNumRides < 255)
            guestNumRides++;

        setHasRidden(ride);
        GuestUpdateFavouriteRide(*this, ride, satisfaction);
        happinessTarget = std::clamp(happinessTarget + satisfaction, 0, kPeepMaxHappiness);
        GuestUpdateRideNauseaGrowth(*this, ride);
    }

    /**
     *
     *  rct2: 0x0069576E
     */
    void Guest::onExitRide(Ride& ride)
    {
        if (peepFlags.has(PeepFlag::rideShouldBeMarkedAsFavourite))
        {
            peepFlags.unset(PeepFlag::rideShouldBeMarkedAsFavourite);
            favouriteRide = ride.id;
            // TODO fix this flag name or add another one
            windowInvalidateFlags |= PEEP_INVALIDATE_STAFF_STATS;
        }
        happiness = happinessTarget;
        nausea = nauseaTarget;
        windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_STATS;

        if (peepFlags.has(PeepFlag::leavingPark))
            peepFlags.unset(PeepFlag::parkEntranceChosen);

        if (GuestShouldGoOnRideAgain(*this, ride))
        {
            guestHeadingToRideId = ride.id;
            guestIsLostCountdown = 200;
            resetPathfindGoal();
            windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_ACTION;
        }

        if (GuestShouldPreferredIntensityIncrease(*this))
        {
            if (intensity.getMaximum() < 15)
            {
                intensity = intensity.withMaximum(intensity.getMaximum() + 1);
            }
        }

        if (GuestReallyLikedRide(*this, ride))
        {
            insertNewThought(PeepThoughtType::wasGreat, ride.id);

            static constexpr Audio::SoundId laughs[3] = {
                Audio::SoundId::laugh1,
                Audio::SoundId::laugh2,
                Audio::SoundId::laugh3,
            };
            int32_t laughType = ScenarioRand() & 7;
            if (laughType < 3)
            {
                Audio::Play3D(laughs[laughType], getLocation());
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
        if (guest.state != PeepState::walking)
            return;
        if (!guest.guestHeadingToRideId.IsNull())
            return;
        if (guest.peepFlags.has(PeepFlag::leavingPark))
            return;
        if (guest.hasFoodOrDrink())
            return;
        if (guest.x == kLocationNull)
            return;

        auto* ride = GuestFindBestRideToGoOn(guest);
        if (ride != nullptr)
        {
            // Head to that ride
            guest.guestHeadingToRideId = ride->id;
            guest.guestIsLostCountdown = 200;
            guest.resetPathfindGoal();
            guest.windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_ACTION;

            // Make peep look at their map if they have one
            if (guest.hasItem(ShopItem::map))
            {
                guest.readMap();
            }
        }
    }

    static OpenRCT2::BitSet<Limits::kMaxRidesInPark> GuestFindRidesToGoOn(Guest& guest)
    {
        OpenRCT2::BitSet<Limits::kMaxRidesInPark> rideConsideration;

        // FIX  Originally checked for a toy, likely a mistake and should be a map,
        //      but then again this seems to only allow the peep to go on
        //      rides they haven't been on before.
        if (guest.hasItem(ShopItem::map))
        {
            // Consider rides that peep hasn't been on yet
            auto& gameState = getGameState();
            for (auto& ride : RideManager(gameState))
            {
                rideConsideration[ride.id.ToUnderlying()] = true;
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
                        auto rideIndex = trackElement->getRideIndex();
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
            if (guest.hasRidden(ride))
            {
                continue;
            }
            const auto rideIndex = ride.id.ToUnderlying();
            if (rideConsideration.size() > rideIndex && rideConsideration[rideIndex])
            {
                if (!ride.flags.has(RideFlag::queueFull))
                {
                    if (guest.shouldGoOnRide(ride, StationIndex::FromUnderlying(0), false, true) && RideHasRatings(ride))
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
    bool Guest::shouldGoOnRide(Ride& ride, StationIndex entranceNum, bool atQueue, bool thinking)
    {
        // Indicates whether a peep is physically at the ride, or is just thinking about going on the ride.
        bool peepAtRide = !thinking;

        if (ride.status == RideStatus::open && !ride.flags.has(RideFlag::brokenDown))
        {
            // Peeps that are leaving the park will refuse to go on any rides, with the exception of free transport rides.
            assert(ride.type < std::size(kRideTypeDescriptors));
            if (!ride.getRideTypeDescriptor().flags.has(RtdFlag::isTransportRide) || ride.value == kRideValueUndefined
                || RideGetPrice(ride) != 0)
            {
                if (peepFlags.has(PeepFlag::leavingPark))
                {
                    choseNotToGoOnRide(ride, peepAtRide, false);
                    return false;
                }
            }

            if (ride.getRideTypeDescriptor().flags.has(RtdFlag::isShopOrFacility))
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
                    Guest* lastPeepInQueue = getGameState().entities.getEntity<Guest>(station.LastPeepInQueue);
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
                        if (maxD <= 13 && lastPeepInQueue->timeInQueue > 10)
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
            if (!ride.getRideTypeDescriptor().flags.has(RtdFlag::isTransportRide) || ride.value == kRideValueUndefined
                || ridePrice != 0)
            {
                if (previousRide == ride.id)
                {
                    choseNotToGoOnRide(ride, peepAtRide, false);
                    return false;
                }

                auto& gameState = getGameState();
                // Basic price checks
                if (ridePrice != 0 && !GuestHasVoucherForFreeRide(*this, ride) && !gameState.park.flags.has(ParkFlag::noMoney))
                {
                    if (ridePrice > cashInPocket)
                    {
                        if (peepAtRide)
                        {
                            if (cashInPocket <= 0)
                            {
                                insertNewThought(PeepThoughtType::spentMoney);
                            }
                            else
                            {
                                insertNewThought(PeepThoughtType::cantAffordRide, ride.id);
                            }
                        }
                        choseNotToGoOnRide(ride, peepAtRide, true);
                        return false;
                    }
                }

                // If happy enough, peeps will ignore the fact that a ride has recently crashed.
                if (ride.lastCrashType != RIDE_CRASH_TYPE_NONE && happiness < 225)
                {
                    if (peepAtRide)
                    {
                        insertNewThought(PeepThoughtType::notSafe, ride.id);
                        if (happinessTarget >= 64)
                        {
                            happinessTarget -= 8;
                        }
                        ride.updatePopularity(0);
                    }
                    choseNotToGoOnRide(ride, peepAtRide, true);
                    return false;
                }

                if (RideHasRatings(ride))
                {
                    // If a peep has already decided that they're going to go on a ride, they'll skip the weather and
                    // excitement check and will only do a basic intensity check when they arrive at the ride itself.
                    if (ride.id == guestHeadingToRideId)
                    {
                        if (ride.ratings.intensity > RideRating::make(10, 00) && !gameState.cheats.ignoreRideIntensity)
                        {
                            GuestRideIsTooIntense(*this, ride, peepAtRide);
                            return false;
                        }
                    }
                    else
                    {
                        const bool isPrecipitating = Weather::isPrecipitating();
                        if (isPrecipitating && !GuestShouldRideWhileRaining(*this, ride))
                        {
                            if (peepAtRide)
                            {
                                insertNewThought(PeepThoughtType::notWhileRaining, ride.id);
                                if (happinessTarget >= 64)
                                {
                                    happinessTarget -= 8;
                                }
                                ride.updatePopularity(0);
                            }
                            choseNotToGoOnRide(ride, peepAtRide, true);
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
                                RideRating_t maxIntensity = std::min(intensity.getMaximum() * 100, 1000) + happiness;
                                RideRating_t minIntensity = (intensity.getMinimum() * 100) - happiness;
                                if (ride.ratings.intensity < minIntensity)
                                {
                                    if (peepAtRide)
                                    {
                                        insertNewThought(PeepThoughtType::moreThrilling, ride.id);
                                        if (happinessTarget >= 64)
                                        {
                                            happinessTarget -= 8;
                                        }
                                        ride.updatePopularity(0);
                                    }
                                    choseNotToGoOnRide(ride, peepAtRide, true);
                                    return false;
                                }
                                if (ride.ratings.intensity > maxIntensity)
                                {
                                    GuestRideIsTooIntense(*this, ride, peepAtRide);
                                    return false;
                                }

                                // nausea calculations.
                                RideRating_t maxNausea = NauseaMaximumThresholds[(EnumValue(nauseaTolerance) & 3)] + happiness;

                                if (ride.ratings.nausea > maxNausea)
                                {
                                    if (peepAtRide)
                                    {
                                        insertNewThought(PeepThoughtType::sickening, ride.id);
                                        if (happinessTarget >= 64)
                                        {
                                            happinessTarget -= 8;
                                        }
                                        ride.updatePopularity(0);
                                    }
                                    choseNotToGoOnRide(ride, peepAtRide, true);
                                    return false;
                                }

                                // Very nauseous peeps will only go on very gentle rides.
                                if (ride.ratings.nausea >= RideRating::make(1, 40) && nausea > 160)
                                {
                                    choseNotToGoOnRide(ride, peepAtRide, false);
                                    return false;
                                }
                            }
                        }
                    }
                }

                // If the ride has not yet been rated and is capable of having g-forces,
                // there's a 90% chance that the peep will ignore it.
                if (!RideHasRatings(ride) && ride.getRideTypeDescriptor().flags.has(RtdFlag::checkGForces))
                {
                    if ((ScenarioRand() & 0xFFFF) > 0x1999u)
                    {
                        choseNotToGoOnRide(ride, peepAtRide, false);
                        return false;
                    }

                    if (!gameState.cheats.ignoreRideIntensity)
                    {
                        if (ride.maxPositiveVerticalG > MakeFixed16_2dp(5, 00)
                            || ride.maxNegativeVerticalG < MakeFixed16_2dp(-4, 00) || ride.maxLateralG > MakeFixed16_2dp(4, 00))
                        {
                            choseNotToGoOnRide(ride, peepAtRide, false);
                            return false;
                        }
                    }
                }

                money64 value = ride.value;

                // If the value of the ride hasn't yet been calculated, peeps will be willing to pay any amount for the ride.
                if (value != kRideValueUndefined && !GuestHasVoucherForFreeRide(*this, ride)
                    && !gameState.park.flags.has(ParkFlag::noMoney))
                {
                    // The amount peeps are willing to pay is decreased by 75% if they had to pay to enter the park.
                    if (peepFlags.has(PeepFlag::hasPaidForParkEntry))
                        value /= 4;

                    // Peeps won't pay more than twice the value of the ride.
                    ridePrice = RideGetPrice(ride);
                    if ((ridePrice > (value * 2)) && !(gameState.cheats.ignorePrice))
                    {
                        if (peepAtRide)
                        {
                            insertNewThought(PeepThoughtType::badValue, ride.id);
                            if (happinessTarget >= 60)
                            {
                                happinessTarget -= 16;
                            }
                            ride.updatePopularity(0);
                        }
                        choseNotToGoOnRide(ride, peepAtRide, true);
                        return false;
                    }

                    // A ride is good value if the price is 50% or less of the ride value and the peep didn't pay to enter the
                    // park.
                    if (ridePrice <= (value / 2) && peepAtRide)
                    {
                        if (!gameState.park.flags.has(ParkFlag::noMoney))
                        {
                            if (!peepFlags.has(PeepFlag::hasPaidForParkEntry))
                            {
                                insertNewThought(PeepThoughtType::goodValue, ride.id);
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

            if (ride.id == guestHeadingToRideId)
            {
                GuestResetRideHeading(*this);
            }

            ride.flags.unset(RideFlag::queueFull);
            return true;
        }

        choseNotToGoOnRide(ride, peepAtRide, false);
        return false;
    }

    static bool GuestShouldGoToShop(Guest& guest, Ride& ride, bool peepAtShop)
    {
        // Peeps won't go to the same shop twice in a row.
        if (ride.id == guest.previousRide)
        {
            guest.choseNotToGoOnRide(ride, peepAtShop, true);
            return false;
        }

        const auto& rtd = ride.getRideTypeDescriptor();
        if (rtd.specialType == RtdSpecialType::toilet)
        {
            if (guest.toilet < 70)
            {
                guest.choseNotToGoOnRide(ride, peepAtShop, true);
                return false;
            }

            // The amount that peeps are willing to pay to use the Toilets scales with their toilet stat.
            // It effectively has a minimum of $0.10 (due to the check above) and a maximum of $0.60.
            if ((RideGetPrice(ride) * 40 > guest.toilet) && !getGameState().cheats.ignorePrice)
            {
                if (peepAtShop)
                {
                    guest.insertNewThought(PeepThoughtType::notPaying, ride.id);
                    if (guest.happinessTarget >= 60)
                    {
                        guest.happinessTarget -= 16;
                    }
                    ride.updatePopularity(0);
                }
                guest.choseNotToGoOnRide(ride, peepAtShop, true);
                return false;
            }
        }

        if (rtd.specialType == RtdSpecialType::firstAid)
        {
            if (guest.nausea < 128)
            {
                guest.choseNotToGoOnRide(ride, peepAtShop, true);
                return false;
            }
        }

        // Basic price checks
        auto ridePrice = RideGetPrice(ride);
        if (ridePrice != 0 && ridePrice > guest.cashInPocket)
        {
            if (peepAtShop)
            {
                if (guest.cashInPocket <= 0)
                {
                    guest.insertNewThought(PeepThoughtType::spentMoney);
                }
                else
                {
                    guest.insertNewThought(PeepThoughtType::cantAffordRide, ride.id);
                }
            }
            guest.choseNotToGoOnRide(ride, peepAtShop, true);
            return false;
        }

        if (peepAtShop)
        {
            ride.updatePopularity(1);
            if (ride.id == guest.guestHeadingToRideId)
            {
                GuestResetRideHeading(guest);
            }
        }
        return true;
    }

    // Used when no logging to an expend type required
    void Guest::spendMoney(money64 amount, ExpenditureType expenditure)
    {
        money64 unused;
        spendMoney(unused, amount, expenditure);
    }

    /**
     *
     *  rct2: 0x0069926C
     * Expend type was previously an offset saved in 0x00F1AEC0
     */
    void Guest::spendMoney(money64& peep_expend_type, money64 amount, ExpenditureType expenditure)
    {
        assert(!getGameState().park.flags.has(ParkFlag::noMoney));

        cashInPocket = std::max(0.00_GBP, cashInPocket - amount);
        cashSpent = AddClamp(cashSpent, amount);

        peep_expend_type = AddClamp(peep_expend_type, amount);

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByNumber(WindowClass::peep, id);

        FinancePayment(-amount, expenditure);

        MoneyEffect::createAt(amount, getLocation(), true);

        Audio::Play3D(Audio::SoundId::purchase, getLocation());
    }

    void Guest::setHasRidden(const Ride& ride)
    {
        RideUse::GetHistory().Add(id, ride.id);

        setHasRiddenRideType(ride.type);
    }

    bool Guest::hasRidden(const Ride& ride) const
    {
        return RideUse::GetHistory().Contains(id, ride.id);
    }

    void Guest::setHasRiddenRideType(ride_type_t rideType)
    {
        RideUse::GetTypeHistory().Add(id, rideType);
    }

    bool Guest::hasRiddenRideType(ride_type_t rideType) const
    {
        return RideUse::GetTypeHistory().Contains(id, rideType);
    }

    void Guest::setParkEntryTime(int32_t entryTime)
    {
        parkEntryTime = entryTime;
    }

    int32_t Guest::getParkEntryTime() const
    {
        return parkEntryTime;
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
        if (guest.hasItem(ShopItem::umbrella) && ride.getRideTypeDescriptor().flags.has(RtdFlag::guestsCanUseUmbrella)
            && (ScenarioRand() & 2) == 0)
        {
            return true;
        }

        return false;
    }

    void Guest::choseNotToGoOnRide(const Ride& ride, bool peepAtRide, bool updateLastRide)
    {
        if (peepAtRide && updateLastRide)
        {
            previousRide = ride.id;
            previousRideTimeOut = 0;
        }

        if (ride.id == guestHeadingToRideId)
        {
            GuestResetRideHeading(*this);
        }
    }

    void Guest::readMap()
    {
        if (isActionInterruptableSafely())
        {
            action = PeepActionType::readMap;
            animationFrameNum = 0;
            animationImageIdOffset = 0;
            updateCurrentAnimationType();
        }
    }

    static bool GuestHasVoucherForFreeRide(Guest& guest, const Ride& ride)
    {
        return guest.hasItem(ShopItem::voucher) && guest.voucherType == VOUCHER_TYPE_RIDE_FREE
            && guest.voucherRideId == ride.id;
    }

    /**
     * When the queue is full, peeps will ignore the ride when thinking about what to go on next.
     * Does not effect peeps that walk up to the queue entrance.
     * This flag is reset the next time a peep successfully joins the queue.
     */
    static void GuestTriedToEnterFullQueue(Guest& guest, Ride& ride)
    {
        ride.flags.set(RideFlag::queueFull);
        guest.previousRide = ride.id;
        guest.previousRideTimeOut = 0;
        // Change status "Heading to" to "Walking" if queue is full
        if (ride.id == guest.guestHeadingToRideId)
        {
            GuestResetRideHeading(guest);
        }
    }

    static void GuestResetRideHeading(Guest& guest)
    {
        guest.guestHeadingToRideId = RideId::GetNull();
        guest.windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_ACTION;
    }

    static void GuestRideIsTooIntense(Guest& guest, Ride& ride, bool peepAtRide)
    {
        if (peepAtRide)
        {
            guest.insertNewThought(PeepThoughtType::intense, ride.id);
            if (guest.happinessTarget >= 64)
            {
                guest.happinessTarget -= 8;
            }
            ride.updatePopularity(0);
        }
        guest.choseNotToGoOnRide(ride, peepAtRide, true);
    }

    /**
     *
     *  rct2: 0x00691C6E
     */
    static Vehicle* PeepChooseCarFromRide(Guest& guest, const Ride& ride, std::span<const uint8_t> carArray)
    {
        uint8_t chosen_car = ScenarioRand();
        if (ride.getRideTypeDescriptor().flags.has(RtdFlag::hasGForces) && ((chosen_car & 0xC) != 0xC))
        {
            chosen_car = (ScenarioRand() & 1) ? 0 : static_cast<uint8_t>(carArray.size()) - 1;
        }
        else
        {
            chosen_car = (chosen_car * static_cast<uint16_t>(carArray.size())) >> 8;
        }

        guest.currentCar = carArray[chosen_car];

        Vehicle* vehicle = getGameState().entities.getEntity<Vehicle>(ride.vehicles[guest.currentTrain]);
        if (vehicle == nullptr)
        {
            return nullptr;
        }
        return vehicle->GetCar(guest.currentCar);
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
        guest->currentSeat = chosen_seat;
        vehicle->next_free_seat++;

        vehicle->peep[guest->currentSeat] = guest->id;
        vehicle->peep_tshirt_colours[guest->currentSeat] = guest->tShirtColour;
    }

    /**
     *
     *  rct2: 0x00691D27
     */
    void Guest::goToRideEntrance(const Ride& ride)
    {
        const auto& station = ride.getStation(currentRideStation);
        if (station.Entrance.IsNull())
        {
            removeFromQueue();
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

        setDestination(location, 2);
        setState(PeepState::enteringRide);
        rideSubState = PeepRideSubState::inEntrance;

        rejoinQueueTimeout = 0;
        guestTimeOnRide = 0;

        removeFromQueue();
    }

    static bool FindVehicleToEnter(
        Guest& guest, const Ride& ride, sfl::static_vector<uint8_t, Limits::kMaxTrainsPerRide>& car_array)
    {
        uint8_t chosen_train = RideStation::kNoTrain;

        if (ride.mode == RideMode::dodgems || ride.mode == RideMode::race)
        {
            if (ride.flags.has(RideFlag::passStationNoStopping))
                return false;

            for (int32_t i = 0; i < ride.numTrains; ++i)
            {
                Vehicle* vehicle = getGameState().entities.getEntity<Vehicle>(ride.vehicles[i]);
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
            chosen_train = ride.getStation(guest.currentRideStation).TrainAtStation;
        }
        if (chosen_train >= Limits::kMaxTrainsPerRide)
        {
            return false;
        }

        guest.currentTrain = chosen_train;

        int32_t i = 0;

        auto vehicle_id = ride.vehicles[chosen_train];
        for (Vehicle* vehicle = getGameState().entities.getEntity<Vehicle>(vehicle_id); vehicle != nullptr;
             vehicle = getGameState().entities.getEntity<Vehicle>(vehicle->next_vehicle_on_train), ++i)
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
        if (guest.destinationTolerance == 0)
        {
            guest.removeFromQueue();
            guest.setState(PeepState::falling);
        }
    }

    static bool PeepCheckRidePriceAtEntrance(Guest& guest, const Ride& ride, money64 ridePrice)
    {
        if ((guest.hasItem(ShopItem::voucher)) && guest.voucherType == VOUCHER_TYPE_RIDE_FREE
            && guest.voucherRideId == guest.currentRide)
            return true;

        if (guest.cashInPocket <= 0 && !getGameState().park.flags.has(ParkFlag::noMoney))
        {
            guest.insertNewThought(PeepThoughtType::spentMoney);
            PeepUpdateRideAtEntranceTryLeave(guest);
            return false;
        }

        if (ridePrice > guest.cashInPocket)
        {
            // Prevent looping of same thought / animation since Destination Tolerance
            // is only 0 exactly at entrance and will immediately change as guest
            // tries to leave hereafter
            if (guest.destinationTolerance == 0)
            {
                guest.insertNewThought(PeepThoughtType::cantAffordRide, guest.currentRide);
            }
            PeepUpdateRideAtEntranceTryLeave(guest);
            return false;
        }

        auto value = ride.value;
        if (value != kRideValueUndefined)
        {
            if (((value * 2) < ridePrice) && !(getGameState().cheats.ignorePrice))
            {
                guest.insertNewThought(PeepThoughtType::badValue, guest.currentRide);
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
        if (guest.timeInQueue >= 4500)
            satisfaction -= 35;
        else if (guest.timeInQueue >= 2250)
            satisfaction -= 10;
        else if (guest.timeInQueue <= 750)
            satisfaction += 10;

        // Peeps get a small boost in satisfaction if they've been on a ride of the same type before,
        // and this boost is doubled if they've already been on this particular ride.
        if (guest.hasRiddenRideType(ride.type))
            satisfaction += 10;

        if (guest.hasRidden(*GetRide(guest.currentRide)))
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
        guest.peepFlags.unset(PeepFlag::rideShouldBeMarkedAsFavourite);
        uint8_t peepRideRating = std::clamp((ride.ratings.excitement / 4) + satisfaction, 0, kPeepMaxHappiness);
        if (peepRideRating >= guest.favouriteRideRating)
        {
            if (guest.happiness >= 160 && guest.happinessTarget >= 160)
            {
                guest.favouriteRideRating = peepRideRating;
                guest.peepFlags.set(PeepFlag::rideShouldBeMarkedAsFavourite);
            }
        }
    }

    /* rct2: 0x00695555 */
    static int16_t GuestCalculateRideValueSatisfaction(Guest& guest, const Ride& ride)
    {
        if (getGameState().park.flags.has(ParkFlag::noMoney))
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

        if ((ride.value + ((ride.value * guest.happiness) / 256)) >= ridePrice)
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
        RideRating_t maxIntensity = guest.intensity.getMaximum() * 100;
        RideRating_t minIntensity = guest.intensity.getMinimum() * 100;
        if (minIntensity <= ride.ratings.intensity && maxIntensity >= ride.ratings.intensity)
        {
            intensitySatisfaction--;
        }
        minIntensity -= guest.happiness * 2;
        maxIntensity += guest.happiness;
        if (minIntensity <= ride.ratings.intensity && maxIntensity >= ride.ratings.intensity)
        {
            intensitySatisfaction--;
        }
        minIntensity -= guest.happiness * 2;
        maxIntensity += guest.happiness;
        if (minIntensity <= ride.ratings.intensity && maxIntensity >= ride.ratings.intensity)
        {
            intensitySatisfaction--;
        }

        // Although it's not shown in the interface, a peep with Average or High nausea tolerance
        // has a minimum preferred nausea value. (For peeps with None or Low, this is set to zero.)
        RideRating_t minNausea = kNauseaMinimumThresholds[(EnumValue(guest.nauseaTolerance) & 3)];
        RideRating_t maxNausea = NauseaMaximumThresholds[(EnumValue(guest.nauseaTolerance) & 3)];
        if (minNausea <= ride.ratings.nausea && maxNausea >= ride.ratings.nausea)
        {
            nauseaSatisfaction--;
        }
        minNausea -= guest.happiness * 2;
        maxNausea += guest.happiness;
        if (minNausea <= ride.ratings.nausea && maxNausea >= ride.ratings.nausea)
        {
            nauseaSatisfaction--;
        }
        minNausea -= guest.happiness * 2;
        maxNausea += guest.happiness;
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
        const auto nauseaMultiplier = std::clamp(256 - guest.happinessTarget, 64, 200);
        const auto rideGeneratedNausea = (ride.ratings.nausea * nauseaMultiplier) / 512;
        const auto hungerAdjustedNausea = ((rideGeneratedNausea * std::max<uint8_t>(128, guest.hunger)) / 128) * 2;
        const auto nauseaGrowthRateChange = hungerAdjustedNausea >> (EnumValue(guest.nauseaTolerance) & 3);
        guest.nauseaTarget = static_cast<uint8_t>(std::min<int32_t>(guest.nauseaTarget + nauseaGrowthRateChange, 255));
    }

    static bool GuestShouldGoOnRideAgain(Guest& guest, const Ride& ride)
    {
        if (!ride.getRideTypeDescriptor().flags.has(RtdFlag::guestsWillRideAgain))
            return false;
        if (!RideHasRatings(ride))
            return false;
        if (ride.ratings.intensity > RideRating::make(10, 00) && !getGameState().cheats.ignoreRideIntensity)
            return false;
        if (guest.happiness < 180)
            return false;
        if (guest.energy < 100)
            return false;
        if (guest.nausea > 160)
            return false;
        if (guest.hunger < 30)
            return false;
        if (guest.thirst < 20)
            return false;
        if (guest.toilet > 170)
            return false;

        uint8_t r = (ScenarioRand() & 0xFF);
        if (r <= 128)
        {
            if (guest.guestNumRides > 7)
                return false;
            if (r > 64)
                return false;
        }

        return true;
    }

    static bool GuestShouldPreferredIntensityIncrease(Guest& guest)
    {
        if (getGameState().park.flags.has(ParkFlag::guestPreferLessIntenseRides))
            return false;
        if (guest.happiness < 200)
            return false;

        return (ScenarioRand() & 0xFF) >= static_cast<uint8_t>(guest.intensity);
    }

    static bool GuestReallyLikedRide(Guest& guest, const Ride& ride)
    {
        if (guest.happiness < 215)
            return false;
        if (guest.nausea > 120)
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
            return PeepThoughtType::none;

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
                    if (tileElement->isGhost())
                    {
                        continue;
                    }

                    switch (tileElement->getType())
                    {
                        case TileElementType::path:
                        {
                            if (!tileElement->asPath()->hasAddition())
                                break;

                            auto* pathAddEntry = tileElement->asPath()->getAdditionEntry();
                            if (pathAddEntry == nullptr)
                            {
                                return PeepThoughtType::none;
                            }
                            if (tileElement->asPath()->additionIsGhost())
                                break;

                            if (pathAddEntry->flags
                                & (PATH_ADDITION_FLAG_JUMPING_FOUNTAIN_WATER | PATH_ADDITION_FLAG_JUMPING_FOUNTAIN_SNOW))
                            {
                                num_fountains++;
                                break;
                            }
                            if (tileElement->asPath()->isBroken())
                            {
                                num_rubbish++;
                            }
                            break;
                        }
                        case TileElementType::largeScenery:
                        case TileElementType::smallScenery:
                            num_scenery++;
                            break;
                        case TileElementType::track:
                        {
                            auto* ride = GetRide(tileElement->asTrack()->getRideIndex());
                            if (ride == nullptr)
                                break;

                            bool isPlayingMusic = ride->flags.has(RideFlag::music) && ride->status != RideStatus::closed
                                && !ride->flags.hasAny(RideFlag::brokenDown, RideFlag::crashed);
                            if (!isPlayingMusic)
                                break;

                            const auto* musicObject = ride->getMusicObject();
                            if (musicObject == nullptr)
                                break;

                            if (musicObject->GetNiceFactor() == MusicNiceFactor::nice)
                            {
                                nearby_music |= 1;
                            }
                            else if (musicObject->GetNiceFactor() == MusicNiceFactor::overbearing)
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
            return PeepThoughtType::fountains;

        if (num_scenery >= 40 && num_rubbish < 8)
            return PeepThoughtType::scenery;

        if (nearby_music == 1 && num_rubbish < 20)
            return PeepThoughtType::music;

        if (num_rubbish < 2 && !getGameState().cheats.disableLittering)
            // if disable littering cheat is enabled, peeps will not have the "clean and tidy park" thought
            return PeepThoughtType::veryClean;

        return PeepThoughtType::none;
    }

    /**
     *
     *  rct2: 0x0068F9A9
     */
    static void GuestUpdateHunger(Guest& guest)
    {
        if (guest.hunger >= 3)
        {
            guest.hunger -= 2;

            guest.energyTarget = std::min<uint16_t>(guest.energyTarget + 2, kPeepMaxEnergyTarget);
            guest.toilet = std::min(guest.toilet + 1, 255);
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
        if (guest.energyTarget >= 33)
        {
            guest.energyTarget -= 2;
        }

        if (getGameState().weatherCurrent.temperature >= 21 && guest.thirst >= 5)
        {
            guest.thirst--;
        }

        if (guest.outsideOfPark)
        {
            return;
        }

        /* Peeps that are happy enough, have enough energy and
         * (if appropriate) have enough money will always stay
         * in the park. */
        if (!guest.peepFlags.has(PeepFlag::leavingPark))
        {
            if (getGameState().park.flags.has(ParkFlag::noMoney))
            {
                if (guest.energy >= 70 && guest.happiness >= 60)
                {
                    return;
                }
            }
            else
            {
                if (guest.energy >= 55 && guest.happiness >= 45 && guest.cashInPocket >= 5.00_GBP)
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
        guest.guestHeadingToRideId = RideId::GetNull();
        if (guest.peepFlags.has(PeepFlag::leavingPark))
        {
            if (guest.guestIsLostCountdown < 60)
            {
                return;
            }
        }
        else
        {
            guest.guestIsLostCountdown = 254;
            guest.peepFlags.set(PeepFlag::leavingPark);
            guest.peepFlags.unset(PeepFlag::parkEntranceChosen);
        }

        guest.insertNewThought(PeepThoughtType::goHome);

        auto* windowMgr = Ui::GetWindowManager();
        WindowBase* w = windowMgr->FindByNumber(WindowClass::peep, guest.id);
        if (w != nullptr)
            w->onPrepareDraw();
        windowMgr->InvalidateByNumber(WindowClass::peep, guest.id);
    }

    template<typename T>
    static void PeepHeadForNearestRide(Guest& guest, bool considerOnlyCloseRides, T predicate)
    {
        if (guest.state != PeepState::sitting && guest.state != PeepState::watching && guest.state != PeepState::walking)
        {
            return;
        }
        if (guest.peepFlags.has(PeepFlag::leavingPark))
            return;
        if (guest.x == kLocationNull)
            return;
        if (!guest.guestHeadingToRideId.IsNull())
        {
            auto ride = GetRide(guest.guestHeadingToRideId);
            if (ride != nullptr && predicate(*ride))
            {
                return;
            }
        }

        OpenRCT2::BitSet<Limits::kMaxRidesInPark> rideConsideration;
        if (!considerOnlyCloseRides && (guest.hasItem(ShopItem::map)))
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
                        auto rideIndex = trackElement->getRideIndex();
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
                if (!ride.flags.has(RideFlag::queueFull))
                {
                    if (guest.shouldGoOnRide(ride, StationIndex::FromUnderlying(0), false, true))
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
            guest.guestHeadingToRideId = closestRide->id;
            guest.guestIsLostCountdown = 200;
            guest.resetPathfindGoal();
            guest.windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_ACTION;
            guest.timeLost = 0;
        }
    }

    static void GuestHeadForNearestRideWithFlag(Guest& guest, bool considerOnlyCloseRides, RtdFlag rtdFlag)
    {
        PeepHeadForNearestRide(guest, considerOnlyCloseRides, [rtdFlag](const Ride& ride) {
            return ride.getRideTypeDescriptor().flags.has(rtdFlag);
        });
    }

    static void GuestHeadForNearestRideWithSpecialType(Guest& guest, bool considerOnlyCloseRides, RtdSpecialType specialType)
    {
        if ((specialType == RtdSpecialType::toilet) && guest.hasFoodOrDrink())
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
        auto thoughtType = PeepThoughtType::hungry;

        const auto& rtd = GetRideTypeDescriptor(rideType);
        if (!rtd.flags.has(RtdFlag::sellsFood))
        {
            thoughtType = PeepThoughtType::thirsty;
            if (!rtd.flags.has(RtdFlag::sellsDrinks))
            {
                thoughtType = PeepThoughtType::runningOut;
                if (rtd.specialType != RtdSpecialType::cashMachine)
                {
                    thoughtType = PeepThoughtType::toilet;
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
            PeepThought* thought = &guest.thoughts[i];

            if (thought->type == PeepThoughtType::none)
                break;

            if (thought->type != thoughtType)
                continue;

            if (i < kPeepMaxThoughts - 1)
            {
                memmove(thought, thought + 1, sizeof(PeepThought) * (kPeepMaxThoughts - i - 1));
            }

            guest.thoughts[kPeepMaxThoughts - 1].type = PeepThoughtType::none;

            guest.windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_THOUGHTS;
            i--;
        }
    }

    /**
     *
     *  rct2: 0x0069AEB7
     */
    static bool PeepShouldUseCashMachine(Guest& guest, RideId rideIndex)
    {
        if (getGameState().park.flags.has(ParkFlag::noMoney))
            return false;
        if (guest.peepFlags.has(PeepFlag::leavingPark))
            return false;
        if (guest.cashInPocket > 20.00_GBP)
            return false;
        if (115 + (ScenarioRand() % 128) > guest.happiness)
            return false;
        if (guest.energy < 80)
            return false;

        auto ride = GetRide(rideIndex);
        if (ride != nullptr)
        {
            ride->updateSatisfaction(guest.happiness >> 6);
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
    void Guest::updateBuying()
    {
        if (!checkForPath())
            return;

        auto ride = GetRide(currentRide);
        if (ride == nullptr || ride->status != RideStatus::open)
        {
            setState(PeepState::falling);
            return;
        }

        if (subState == 1)
        {
            if (!isActionWalking())
            {
                updateAction();
                invalidate();
                return;
            }

            const auto& rtd = GetRideTypeDescriptor(ride->type);
            if (rtd.specialType == RtdSpecialType::cashMachine)
            {
                if (currentRide != previousRide)
                {
                    cashInPocket += 50.00_GBP;
                }
                auto* windowMgr = Ui::GetWindowManager();
                windowMgr->InvalidateByNumber(WindowClass::peep, id);
            }
            orientation ^= 0x10;

            auto destination = CoordsXY{ 16, 16 } + nextLoc;
            setDestination(destination);
            peepDirection = DirectionReverse(peepDirection);

            setState(PeepState::walking);
            return;
        }

        bool item_bought = false;

        if (currentRide != previousRide)
        {
            const auto& rtd = GetRideTypeDescriptor(ride->type);
            if (rtd.specialType == RtdSpecialType::cashMachine)
            {
                item_bought = PeepShouldUseCashMachine(*this, currentRide);
                if (!item_bought)
                {
                    previousRide = currentRide;
                    previousRideTimeOut = 0;
                }
                else
                {
                    action = PeepActionType::withdrawMoney;
                    animationFrameNum = 0;
                    animationImageIdOffset = 0;

                    updateCurrentAnimationType();

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
        subState = 1;
    }

    /**
     *
     *  rct2: 0x00691A3B
     */
    void Guest::updateRideAtEntrance()
    {
        auto ride = GetRide(currentRide);
        if (ride == nullptr)
            return;

        // The peep will keep advancing in the entranceway
        // whilst in this state. When it has reached the very
        // front of the queue destination tolerance is set to
        // zero to indicate it is final decision time (try_leave will pass).
        // When a peep has to return to the queue without getting on a ride
        // this is the state it will return to.
        if (destinationTolerance != 0)
        {
            int16_t xy_distance;
            if (auto loc = updateAction(xy_distance); loc.has_value())
            {
                int16_t actionZ = z;
                if (xy_distance < 16)
                {
                    const auto& station = ride->getStation(currentRideStation);
                    auto entrance = station.Entrance.ToCoordsXYZ();
                    actionZ = entrance.z + 2;
                }
                moveTo({ loc.value(), actionZ });
            }
            else
            {
                destinationTolerance = 0;
                orientation ^= (1 << 4);
                invalidate();
            }
        }

        sfl::static_vector<uint8_t, Limits::kMaxTrainsPerRide> carArray;

        if (ride->getRideTypeDescriptor().flags.has(RtdFlag::noVehicles))
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

        if (ride->flags.has(RideFlag::brokenDown))
            return;

        auto ridePrice = RideGetPrice(*ride);
        if (ridePrice != 0)
        {
            if (!PeepCheckRidePriceAtEntrance(*this, *ride, ridePrice))
                return;
        }

        if (!ride->getRideTypeDescriptor().flags.has(RtdFlag::noVehicles))
        {
            Vehicle* vehicle = PeepChooseCarFromRide(*this, *ride, carArray);
            PeepChooseSeatFromCar(this, *ride, vehicle);
        }
        goToRideEntrance(*ride);
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
        guest.mazeLastEdge = entrance_loc.direction + 1;

        entrance_loc.x += CoordsDirectionDelta[entrance_loc.direction].x;
        entrance_loc.y += CoordsDirectionDelta[entrance_loc.direction].y;

        uint8_t direction = entrance_loc.direction * 4 + 11;
        if (ScenarioRand() & 0x40)
        {
            direction += 4;
            guest.mazeLastEdge += 2;
        }

        direction &= 0xF;
        // Direction is 11, 15, 3, or 7
        guest.var37 = direction;
        guest.mazeLastEdge &= 3;

        entrance_loc.x += kMazeEntranceStart[direction / 4].x;
        entrance_loc.y += kMazeEntranceStart[direction / 4].y;

        guest.setDestination(entrance_loc, 3);

        ride.curNumCustomers++;
        guest.onEnterRide(ride);
        guest.rideSubState = PeepRideSubState::mazePathfinding;
    }

    void PeepUpdateRideLeaveEntranceSpiralSlide(Guest& guest, Ride& ride, CoordsXYZD& entrance_loc)
    {
        entrance_loc = { ride.getStation(guest.currentRideStation).GetStart(), entrance_loc.direction };

        TileElement* tile_element = RideGetStationStartTrackElement(ride, guest.currentRideStation);

        uint8_t direction_track = (tile_element == nullptr ? 0 : tile_element->getDirection());

        guest.var37 = (entrance_loc.direction << 2) | (direction_track << 4);

        entrance_loc += kSpiralSlideWalkingPath[guest.var37];

        guest.setDestination(entrance_loc);
        guest.timesSlidDown = 0;

        ride.curNumCustomers++;
        guest.onEnterRide(ride);
        guest.rideSubState = PeepRideSubState::approachSpiralSlide;
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
                News::AddItemToQueue(
                    News::ItemType::ride, STR_GUESTS_GETTING_STUCK_ON_RIDE, guest.currentRide.ToUnderlying(), ft);
            }
        }
    }

    uint8_t Guest::getWaypointedSeatLocation(const Ride& ride, const CarEntry* vehicle_type, uint8_t track_direction) const
    {
        // The seatlocation can be split into segments around the ride base
        // to decide the segment first split off the segmentable seat location
        // from the fixed section
        uint8_t seatLocationSegment = currentSeat & 0x7;
        uint8_t seatLocationFixed = currentSeat & 0xF8;

        // Enterprise has more segments (8) compared to the normal (4)
        if (ride.getRideTypeDescriptor().specialType != RtdSpecialType::enterprise)
            track_direction *= 2;

        // Type 1 loading doesn't do segments and all peeps go to the same
        // location on the ride
        if (vehicle_type->guestLoadingWaypointSegments == 0)
        {
            track_direction /= 2;
            seatLocationSegment = 0;
            seatLocationFixed = 0;
        }
        seatLocationSegment += track_direction;
        seatLocationSegment &= 0x7;
        return seatLocationSegment + seatLocationFixed;
    }

    void Guest::updateRideLeaveEntranceWaypoints(const Ride& ride)
    {
        const auto& station = ride.getStation(currentRideStation);
        if (station.Entrance.IsNull())
        {
            return;
        }
        uint8_t direction_entrance = station.Entrance.direction;

        TileElement* tile_element = RideGetStationStartTrackElement(ride, currentRideStation);

        uint8_t direction_track = (tile_element == nullptr ? 0 : tile_element->getDirection());

        auto vehicle = getGameState().entities.getEntity<Vehicle>(ride.vehicles[currentTrain]);
        if (vehicle == nullptr)
        {
            // TODO: Goto ride exit on failure.
            return;
        }
        const auto* rideEntry = vehicle->GetRideEntry();
        const auto* carEntry = &rideEntry->Cars[vehicle->vehicle_type];

        var37 = (direction_entrance | getWaypointedSeatLocation(ride, carEntry, direction_track) * 4) * 4;

        const auto& rtd = ride.getRideTypeDescriptor();
        CoordsXY waypoint = rtd.GetGuestWaypointLocation(*vehicle, ride, currentRideStation);

        const auto waypointIndex = var37 / 4u;
        if (waypointIndex < carEntry->guestLoadingWaypoints.size())
        {
            Guard::Assert(carEntry->guestLoadingWaypoints.size() >= static_cast<size_t>(waypointIndex));
            waypoint.x += carEntry->guestLoadingWaypoints[waypointIndex][0].x;
            waypoint.y += carEntry->guestLoadingWaypoints[waypointIndex][0].y;
        }

        setDestination(waypoint);
        rideSubState = PeepRideSubState::approachVehicleWaypoints;
    }

    /**
     *
     *  rct2: 0x006921D3
     */
    void Guest::updateRideAdvanceThroughEntrance()
    {
        auto ride = GetRide(currentRide);
        if (ride == nullptr)
            return;

        int16_t actionZ, xy_distance;

        const auto* rideEntry = ride->getRideEntry();

        if (auto loc = updateAction(xy_distance); loc.has_value())
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

            if (rideSubState == PeepRideSubState::inEntrance && xy_distance < distanceThreshold)
            {
                rideSubState = PeepRideSubState::freeVehicleCheck;
            }

            actionZ = ride->getStation(currentRideStation).GetBaseZ();

            distanceThreshold += 4;
            if (xy_distance < distanceThreshold)
            {
                actionZ += ride->getRideTypeDescriptor().Heights.PlatformHeight;
            }

            moveTo({ loc.value(), actionZ });
            return;
        }

        if (rideSubState == PeepRideSubState::inEntrance)
        {
            rideSubState = PeepRideSubState::freeVehicleCheck;
            return;
        }

        if (ride->getRideTypeDescriptor().flags.has(RtdFlag::noVehicles))
        {
            const auto& station = ride->getStation(currentRideStation);
            auto entranceLocation = station.Entrance.ToCoordsXYZD();
            if (entranceLocation.IsNull())
            {
                return;
            }

            const auto& rtd = GetRideTypeDescriptor(ride->type);
            rtd.UpdateLeaveEntrance(*this, *ride, entranceLocation);
            return;
        }

        Vehicle* vehicle = getGameState().entities.getEntity<Vehicle>(ride->vehicles[currentTrain]);
        if (vehicle == nullptr)
        {
            return;
        }

        vehicle = vehicle->GetCar(currentCar);
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
            updateRideLeaveEntranceWaypoints(*ride);
            return;
        }

        if (vehicle_type->flags.has(CarEntryFlag::useDodgemCarPlacement))
        {
            setDestination(vehicle->getLocation(), 15);
            rideSubState = PeepRideSubState::approachVehicle;
            return;
        }

        int8_t load_position = 0;
        // Safe, in case current seat > number of loading positions
        uint16_t numSeatPositions = static_cast<uint16_t>(vehicle_type->guestLoadingPositions.size());
        if (numSeatPositions != 0)
        {
            size_t loadPositionIndex = numSeatPositions - 1;
            if (currentSeat < numSeatPositions)
            {
                loadPositionIndex = currentSeat;
            }
            load_position = vehicle_type->guestLoadingPositions[loadPositionIndex];
        }

        auto destination = getDestination();
        auto loadPositionWithReversal = (vehicle->flags.has(VehicleFlag::carIsReversed)) ? -load_position : load_position;
        switch (vehicle->orientation / 8)
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
        setDestination(destination);

        rideSubState = PeepRideSubState::approachVehicle;
    }

    /**
     *
     *  rct2: 0x0069321D
     */
    static void PeepGoToRideExit(Guest& guest, const Ride& ride, int16_t x, int16_t y, int16_t z, uint8_t exit_direction)
    {
        z += ride.getRideTypeDescriptor().Heights.PlatformHeight;

        guest.moveTo({ x, y, z });

        Guard::Assert(guest.currentRideStation.ToUnderlying() < Limits::kMaxStationsPerRide);
        auto exit = ride.getStation(guest.currentRideStation).Exit;
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

        guest.setDestination({ x, y }, 2);

        guest.orientation = exit_direction * 8;
        guest.rideSubState = PeepRideSubState::approachExit;
    }

    /**
     *
     *  rct2: 0x006920B4
     */
    void Guest::updateRideFreeVehicleEnterRide(Ride& ride)
    {
        auto ridePrice = RideGetPrice(ride);
        if (ridePrice != 0)
        {
            if ((hasItem(ShopItem::voucher)) && (voucherType == VOUCHER_TYPE_RIDE_FREE) && (voucherRideId == currentRide))
            {
                removeItem(ShopItem::voucher);
                windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
            }
            else
            {
                ride.totalProfit = AddClamp<money64>(ride.totalProfit, ridePrice);
                ride.windowInvalidateFlags.set(RideInvalidateFlag::income);
                spendMoney(paidOnRides, ridePrice, ExpenditureType::parkRideTickets);
            }
        }

        rideSubState = PeepRideSubState::leaveEntrance;
        uint8_t queueTime = daysInQueue;
        if (queueTime < 253)
            queueTime += 3;

        queueTime /= 2;
        auto& station = ride.getStation(currentRideStation);
        if (queueTime != station.QueueTime)
        {
            station.QueueTime = queueTime;
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->InvalidateByNumber(WindowClass::ride, currentRide.ToUnderlying());
        }

        if (peepFlags.has(PeepFlag::tracking))
        {
            auto ft = Formatter();
            formatNameTo(ft);
            ride.formatNameTo(ft);

            StringId msg_string;
            if (ride.getRideTypeDescriptor().flags.has(RtdFlag::describeAsInside))
                msg_string = STR_PEEP_TRACKING_PEEP_IS_IN_X;
            else
                msg_string = STR_PEEP_TRACKING_PEEP_IS_ON_X;

            if (Config::Get().notifications.guestOnRide)
            {
                News::AddItemToQueue(News::ItemType::peepOnRide, msg_string, id, ft);
            }
        }

        const auto& rtd = ride.getRideTypeDescriptor();
        if (rtd.specialType == RtdSpecialType::spiralSlide)
        {
            switchToSpecialSprite(1);
        }

        updateRideAdvanceThroughEntrance();
    }

    /**
     *
     *  rct2: 0x00691FD4
     */
    static void PeepUpdateRideNoFreeVehicleRejoinQueue(Guest& guest, Ride& ride)
    {
        TileCoordsXYZD entranceLocation = ride.getStation(guest.currentRideStation).Entrance;

        int32_t x = entranceLocation.x * 32;
        int32_t y = entranceLocation.y * 32;
        if (entranceLocation.direction < DirectionOffsets.size())
        {
            x += 16 - DirectionOffsets[entranceLocation.direction].x * 20;
            y += 16 - DirectionOffsets[entranceLocation.direction].y * 20;
        }

        guest.setDestination({ x, y }, 2);
        guest.setState(PeepState::queuingFront);
        guest.rideSubState = PeepRideSubState::atEntrance;

        ride.queueInsertGuestAtFront(guest.currentRideStation, &guest);
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
    void Guest::updateRideFreeVehicleCheck()
    {
        auto ride = GetRide(currentRide);
        if (ride == nullptr)
            return;

        if (ride->getRideTypeDescriptor().flags.has(RtdFlag::noVehicles))
        {
            if (ride->status != RideStatus::open || ride->vehicleChangeTimeout != 0 || (++rejoinQueueTimeout) == 0)
            {
                PeepUpdateRideNoFreeVehicleRejoinQueue(*this, *ride);
                return;
            }

            updateRideFreeVehicleEnterRide(*ride);
            return;
        }

        auto& gameState = getGameState();
        Vehicle* vehicle = gameState.entities.getEntity<Vehicle>(ride->vehicles[currentTrain]);
        if (vehicle == nullptr)
        {
            // TODO: Leave ride on failure goes for all returns on nullptr in this function
            return;
        }
        vehicle = vehicle->GetCar(currentCar);
        if (vehicle == nullptr)
            return;

        const auto* rideEntry = vehicle->GetRideEntry();
        if (rideEntry == nullptr)
        {
            return;
        }

        if (rideEntry->Cars[0].flags.has(CarEntryFlag::isMiniGolf))
        {
            vehicle->miniGolfFlags.unset(MiniGolfFlag::flag5);

            for (size_t i = 0; i < ride->numTrains; ++i)
            {
                Vehicle* train = gameState.entities.getEntity<Vehicle>(ride->vehicles[i]);
                if (train == nullptr)
                    continue;

                Vehicle* second_vehicle = gameState.entities.getEntity<Vehicle>(train->next_vehicle_on_train);
                if (second_vehicle == nullptr)
                    continue;

                if (second_vehicle->num_peeps == 0)
                    continue;

                if (second_vehicle->miniGolfFlags.has(MiniGolfFlag::flag5))
                    continue;

                return;
            }
        }

        if (!vehicle->IsUsedInPairs())
        {
            updateRideFreeVehicleEnterRide(*ride);
            return;
        }

        if (ride->mode == RideMode::forwardRotation || ride->mode == RideMode::backwardRotation)
        {
            if (currentSeat & 1 || !(vehicle->next_free_seat & 1))
            {
                updateRideFreeVehicleEnterRide(*ride);
                return;
            }
        }
        else
        {
            uint8_t seat = currentSeat | 1;
            if (seat < vehicle->next_free_seat)
            {
                updateRideFreeVehicleEnterRide(*ride);
                return;
            }
        }

        Vehicle* currentTrainEntity = gameState.entities.getEntity<Vehicle>(ride->vehicles[currentTrain]);
        if (currentTrainEntity == nullptr)
        {
            return;
        }
        if (ride->status == RideStatus::open && ++rejoinQueueTimeout != 0
            && !currentTrainEntity->flags.has(VehicleFlag::readyToDepart))
        {
            return;
        }

        if (ride->mode != RideMode::forwardRotation && ride->mode != RideMode::backwardRotation)
        {
            if (vehicle->next_free_seat - 1 != currentSeat)
                return;
        }

        vehicle->next_free_seat--;
        vehicle->peep[currentSeat] = EntityId::GetNull();

        PeepUpdateRideNoFreeVehicleRejoinQueue(*this, *ride);
    }

    void Guest::updateRideApproachVehicle()
    {
        if (auto loc = updateAction(); loc.has_value())
        {
            moveTo({ loc.value(), z });
            return;
        }
        rideSubState = PeepRideSubState::enterVehicle;
    }

    void Guest::updateRideEnterVehicle()
    {
        auto& gameState = getGameState();
        auto* ride = GetRide(currentRide);
        if (ride != nullptr)
        {
            auto* vehicle = gameState.entities.getEntity<Vehicle>(ride->vehicles[currentTrain]);
            if (vehicle != nullptr)
            {
                vehicle = vehicle->GetCar(currentCar);
                if (vehicle == nullptr)
                {
                    return;
                }

                if (ride->mode != RideMode::forwardRotation && ride->mode != RideMode::backwardRotation)
                {
                    if (currentSeat != vehicle->num_peeps)
                        return;
                }

                if (vehicle->IsUsedInPairs())
                {
                    auto* seatedGuest = gameState.entities.getEntity<Guest>(vehicle->peep[currentSeat ^ 1]);
                    if (seatedGuest != nullptr)
                    {
                        if (seatedGuest->rideSubState != PeepRideSubState::enterVehicle)
                            return;

                        vehicle->num_peeps++;
                        ride->curNumCustomers++;

                        vehicle->ApplyMass(seatedGuest->mass);
                        seatedGuest->moveTo({ kLocationNull, 0, 0 });
                        seatedGuest->setState(PeepState::onRide);
                        seatedGuest->guestTimeOnRide = 0;
                        seatedGuest->rideSubState = PeepRideSubState::onRide;
                        seatedGuest->onEnterRide(*ride);
                    }
                }

                vehicle->num_peeps++;
                ride->curNumCustomers++;

                vehicle->ApplyMass(mass);
                vehicle->invalidate();

                moveTo({ kLocationNull, 0, 0 });

                setState(PeepState::onRide);

                guestTimeOnRide = 0;
                rideSubState = PeepRideSubState::onRide;
                onEnterRide(*ride);
            }
        }
    }

    /**
     *
     *  rct2: 0x00693028
     */
    void Guest::updateRideLeaveVehicle()
    {
        auto& gameState = getGameState();
        auto ride = GetRide(currentRide);
        if (ride == nullptr)
            return;

        Vehicle* vehicle = gameState.entities.getEntity<Vehicle>(ride->vehicles[currentTrain]);
        if (vehicle == nullptr)
            return;

        StationIndex ride_station = vehicle->current_station;
        vehicle = vehicle->GetCar(currentCar);
        if (vehicle == nullptr)
        {
            return;
        }

        // Check if ride is NOT Ferris Wheel.
        if (ride->mode != RideMode::forwardRotation && ride->mode != RideMode::backwardRotation)
        {
            if (vehicle->num_peeps - 1 != currentSeat)
                return;
        }

        animationImageIdOffset++;
        if (animationImageIdOffset & 3)
            return;

        animationImageIdOffset = 0;

        vehicle->num_peeps--;
        vehicle->ApplyMass(-mass);
        vehicle->invalidate();

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
        currentRideStation = ride_station;
        const auto* rideEntry = vehicle->GetRideEntry();
        if (rideEntry == nullptr)
        {
            return;
        }

        const auto* carEntry = &rideEntry->Cars[vehicle->vehicle_type];

        assert(currentRideStation.ToUnderlying() < Limits::kMaxStationsPerRide);
        auto& station = ride->getStation(currentRideStation);

        if (!carEntry->flags.has(CarEntryFlag::loadingWaypoints))
        {
            TileCoordsXYZD exitLocation = station.Exit;
            CoordsXYZD platformLocation;
            platformLocation.z = station.GetBaseZ();

            platformLocation.direction = DirectionReverse(exitLocation.direction);

            if (!ride->getRideTypeDescriptor().flags.has(RtdFlag::vehicleIsIntegral))
            {
                for (; vehicle != nullptr && !vehicle->IsHead();
                     vehicle = gameState.entities.getEntity<Vehicle>(vehicle->prev_vehicle_on_ride))
                {
                    auto trackType = vehicle->GetTrackType();
                    if (trackType == TrackElemType::flat || trackType > TrackElemType::middleStation)
                        continue;

                    bool foundStation = false;
                    for (auto* trackElement : TileElementsView<TrackElement>(vehicle->TrackLocation))
                    {
                        if (trackElement->getBaseZ() != vehicle->TrackLocation.z)
                            continue;

                        if (trackElement->getStationIndex() != currentRideStation)
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
                        specialDirection = ((vehicle->orientation + 3) / 8) + 1;
                        specialDirection &= 3;

                        if (vehicle->TrackSubposition == VehicleTrackSubposition::goKartsRightLane)
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
            if (currentSeat < carEntry->guestLoadingPositions.size())
            {
                int8_t loadPosition = carEntry->guestLoadingPositions[currentSeat];

                switch (vehicle->orientation / 8)
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
                    "CurrentSeat %d is too large! (Vehicle entry has room for %d.)", currentSeat,
                    carEntry->guestLoadingPositions.size());
            }

            platformLocation.z = station.GetBaseZ();

            PeepGoToRideExit(
                *this, *ride, platformLocation.x, platformLocation.y, platformLocation.z, platformLocation.direction);
            return;
        }

        auto exitLocation = station.Exit.ToCoordsXYZD();
        if (exitLocation.IsNull())
        {
            return;
        }

        TileElement* trackElement = RideGetStationStartTrackElement(*ride, currentRideStation);

        Direction station_direction = (trackElement == nullptr ? 0 : trackElement->getDirection());

        vehicle = gameState.entities.getEntity<Vehicle>(ride->vehicles[currentTrain]);
        if (vehicle == nullptr)
        {
            return;
        }

        const auto& rtd = ride->getRideTypeDescriptor();
        CoordsXYZ waypointLoc = { rtd.GetGuestWaypointLocation(*vehicle, *ride, currentRideStation),
                                  exitLocation.z + ride->getRideTypeDescriptor().Heights.PlatformHeight };

        rideEntry = vehicle->GetRideEntry();
        carEntry = &rideEntry->Cars[vehicle->vehicle_type];
        if (carEntry == nullptr)
            return;

        var37 = ((exitLocation.direction | getWaypointedSeatLocation(*ride, carEntry, station_direction) * 4) * 4) | 1;

        CoordsXYZ exitWaypointLoc = waypointLoc;

        const auto waypointIndex = var37 / 4u;
        if (waypointIndex < carEntry->guestLoadingWaypoints.size())
        {
            exitWaypointLoc.x += carEntry->guestLoadingWaypoints[waypointIndex][2].x;
            exitWaypointLoc.y += carEntry->guestLoadingWaypoints[waypointIndex][2].y;
        }

        if (ride->getRideTypeDescriptor().specialType == RtdSpecialType::motionSimulator)
            exitWaypointLoc.z += 15;

        moveTo(exitWaypointLoc);

        if (waypointIndex < carEntry->guestLoadingWaypoints.size())
        {
            waypointLoc.x += carEntry->guestLoadingWaypoints[waypointIndex][1].x;
            waypointLoc.y += carEntry->guestLoadingWaypoints[waypointIndex][1].y;
        }

        setDestination(waypointLoc, 2);
        rideSubState = PeepRideSubState::approachExitWaypoints;
    }

    /**
     *
     *  rct2: 0x0069376A
     */
    void Guest::updateRidePrepareForExit()
    {
        const auto* ride = GetRide(currentRide);
        if (ride == nullptr || currentRideStation.ToUnderlying() >= std::size(ride->getStations()))
            return;

        auto exit = ride->getStation(currentRideStation).Exit;
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

        setDestination(newDestination, 2);
        rideSubState = PeepRideSubState::inExit;
    }

    /**
     *
     *  rct2: 0x0069374F
     */
    void Guest::updateRideApproachExit()
    {
        if (auto loc = updateAction(); loc.has_value())
        {
            moveTo({ loc.value(), z });
            return;
        }

        updateRidePrepareForExit();
    }

    /**
     *
     *  rct2: 0x0069382E
     */
    void Guest::updateRideInExit()
    {
        auto ride = GetRide(currentRide);
        if (ride == nullptr)
            return;

        int16_t xy_distance;

        if (auto loc = updateAction(xy_distance); loc.has_value())
        {
            if (xy_distance >= 16)
            {
                int16_t actionZ = ride->getStation(currentRideStation).GetBaseZ();

                actionZ += ride->getRideTypeDescriptor().Heights.PlatformHeight;
                moveTo({ loc.value(), actionZ });
                return;
            }

            switchToSpecialSprite(0);
            moveTo({ loc.value(), z });
        }

        if (ride->flags.has(RideFlag::onRidePhoto))
        {
            ShopItem secondaryItem = ride->getRideTypeDescriptor().PhotoItem;
            if (GuestDecideAndBuyItem(*this, *ride, secondaryItem, ride->price[1]))
            {
                ride->numSecondaryItemsSold = AddClamp(ride->numSecondaryItemsSold, 1u);
            }
        }
        rideSubState = PeepRideSubState::leaveExit;
    }
#pragma warning(default : 6011)

    CoordsXY GetGuestWaypointLocationDefault(const Vehicle& vehicle, const Ride& ride, const StationIndex& CurrentRideStation)
    {
        return ride.getStation(CurrentRideStation).Start.ToTileCentre();
    }

    CoordsXY GetGuestWaypointLocationEnterprise(
        const Vehicle& vehicle, const Ride& ride, const StationIndex& CurrentRideStation)
    {
        return { vehicle.x, vehicle.y };
    }

    /**
     *
     *  rct2: 0x006926AD
     */
    void Guest::updateRideApproachVehicleWaypoints()
    {
        auto ride = GetRide(currentRide);
        if (ride == nullptr)
            return;

        int16_t xy_distance;
        uint8_t waypoint = var37 & 3;

        const auto& rtd = ride->getRideTypeDescriptor();
        if (auto loc = updateAction(xy_distance); loc.has_value())
        {
            rtd.UpdateRideApproachVehicleWaypoints(*this, loc.value(), xy_distance);
            return;
        }

        if (waypoint == 2)
        {
            rideSubState = PeepRideSubState::enterVehicle;
            return;
        }

        waypoint++;
        // This is incrementing the actual peep waypoint
        var37++;

        Vehicle* vehicle = getGameState().entities.getEntity<Vehicle>(ride->vehicles[currentTrain]);
        if (vehicle == nullptr)
        {
            return;
        }

        CoordsXY targetLoc = rtd.GetGuestWaypointLocation(*vehicle, *ride, currentRideStation);

        const auto* rideEntry = vehicle->GetRideEntry();
        if (rideEntry == nullptr)
        {
            return;
        }

        const auto& vehicle_type = rideEntry->Cars[vehicle->vehicle_type];
        const auto waypointIndex = var37 / 4u;
        if (waypointIndex < vehicle_type.guestLoadingWaypoints.size())
        {
            Guard::Assert(waypoint < 3);
            targetLoc.x += vehicle_type.guestLoadingWaypoints[waypointIndex][waypoint].x;
            targetLoc.y += vehicle_type.guestLoadingWaypoints[waypointIndex][waypoint].y;
        }

        setDestination(targetLoc);
    }

    void UpdateRideApproachVehicleWaypointsMotionSimulator(Guest& guest, const CoordsXY& loc, int16_t& xy_distance)
    {
        auto ride = GetRide(guest.currentRide);
        // Motion simulators have steps. This moves the peeps up the steps.
        int16_t actionZ = ride->getStation(guest.currentRideStation).GetBaseZ() + 2;

        uint8_t waypoint = guest.var37 & 3;
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
        guest.moveTo({ loc, actionZ });
    }

    void UpdateRideApproachVehicleWaypointsDefault(Guest& guest, const CoordsXY& loc, int16_t& xy_distance)
    {
        guest.moveTo({ loc, guest.z });
    }

    /**
     *
     *  rct2: 0x0069357D
     */
    void Guest::updateRideApproachExitWaypoints()
    {
        auto ride = GetRide(currentRide);
        if (ride == nullptr)
            return;

        int16_t xy_distance;

        if (auto loc = updateAction(xy_distance); loc.has_value())
        {
            int16_t actionZ;

            if (ride->getRideTypeDescriptor().specialType == RtdSpecialType::motionSimulator)
            {
                actionZ = ride->getStation(currentRideStation).GetBaseZ() + 2;

                if ((var37 & 3) == 1)
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
            moveTo({ loc.value(), actionZ });
            return;
        }

        if ((var37 & 3) != 0)
        {
            if ((var37 & 3) == 3)
            {
                updateRidePrepareForExit();
                return;
            }

            var37--;
            Vehicle* vehicle = getGameState().entities.getEntity<Vehicle>(ride->vehicles[currentTrain]);
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

            const size_t carPosition = var37 / 4;
            if (carPosition >= carEntry.guestLoadingWaypoints.size())
                return;

            const auto waypoint = var37 & 3;
            Guard::Assert(waypoint < 3);

            const auto& rtd = ride->getRideTypeDescriptor();

            CoordsXY targetLoc = rtd.GetGuestWaypointLocation(*vehicle, *ride, currentRideStation);
            targetLoc += carEntry.guestLoadingWaypoints[carPosition][waypoint];
            setDestination(targetLoc);
            return;
        }

        var37 |= 3;

        auto targetLoc = ride->getStation(currentRideStation).Exit.ToCoordsXYZD().ToTileCentre();
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

        setDestination(targetLoc);
    }

    /**
     *
     *  rct2: 0x006927B3
     */
    void Guest::updateRideApproachSpiralSlide()
    {
        auto ride = GetRide(currentRide);
        if (ride == nullptr)
            return;

        if (auto loc = updateAction(); loc.has_value())
        {
            moveTo({ loc.value(), z });
            return;
        }

        uint8_t waypoint = var37 & 3;

        if (waypoint == 3)
        {
            subState = 15;
            spiralSlideSubstate = PeepSpiralSlideSubState::goingUp;
            spiralSlideGoingUpTimer = 0;
            var37 = (var37 / 4) & 0xC;
            moveTo({ kLocationNull, y, z });
            return;
        }

        [[maybe_unused]] const auto& rtd = ride->getRideTypeDescriptor();
        if (waypoint == 2)
        {
            bool lastRide = false;
            // If the player has closed the ride, leave ASAP, even if the guest hasn't entered the structure yet
            if (ride->status != RideStatus::open)
                lastRide = true;
            else
            {
                if (timesSlidDown != 0)
                {
                    if (ride->mode == RideMode::singleRidePerAdmission)
                        lastRide = true;
                    if (timesSlidDown > static_cast<uint8_t>(ScenarioRand() & 0xF))
                        lastRide = true;
                }
                timesSlidDown++;
            }

            if (lastRide)
            {
                auto exit = ride->getStation(currentRideStation).Exit;
                waypoint = 1;
                auto directionTemp = exit.direction;
                if (exit.direction == kInvalidDirection)
                {
                    directionTemp = 0;
                }
                var37 = (directionTemp * 4) | (var37 & 0x30) | waypoint;
                CoordsXY targetLoc = ride->getStation(currentRideStation).Start;

                assert(rtd.specialType == RtdSpecialType::spiralSlide);
                targetLoc += kSpiralSlideWalkingPath[var37];

                setDestination(targetLoc);
                rideSubState = PeepRideSubState::leaveSpiralSlide;
                return;
            }
        }

        waypoint++;
        // Actually increment the real peep waypoint
        var37++;

        CoordsXY targetLoc = ride->getStation(currentRideStation).Start;

        assert(rtd.specialType == RtdSpecialType::spiralSlide);
        targetLoc += kSpiralSlideWalkingPath[var37];

        setDestination(targetLoc);
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
    void Guest::updateRideOnSpiralSlide()
    {
        auto ride = GetRide(currentRide);

        if (ride == nullptr)
            return;

        const auto& rtd = ride->getRideTypeDescriptor();
        if (rtd.specialType != RtdSpecialType::spiralSlide)
            return;

        if ((var37 & 3) == 0)
        {
            switch (spiralSlideSubstate)
            {
                case PeepSpiralSlideSubState::goingUp:
                    spiralSlideGoingUpTimer++;
                    if (spiralSlideGoingUpTimer >= kTicksToGoUpSpiralSlide)
                        spiralSlideSubstate = PeepSpiralSlideSubState::prepareToSlide;

                    return;
                case PeepSpiralSlideSubState::prepareToSlide:
                    if (ride->slideInUse || ride->flags.has(RideFlag::brokenDown))
                        return;

                    ride->slideInUse = 1;
                    ride->slidePeep = id;
                    ride->slidePeepTShirtColour = tShirtColour;
                    ride->spiralSlideProgress = 0;
                    spiralSlideSubstate = PeepSpiralSlideSubState::slidingDown;

                    return;
                case PeepSpiralSlideSubState::finishedSliding:
                {
                    auto newLocation = ride->getStation(currentRideStation).Start;
                    uint8_t dir = (var37 / 4) & 3;

                    // Set the location that the guest walks to go on slide again
                    auto destination = newLocation + kSpiralSlideEndWaypoint[dir];
                    setDestination(destination);

                    // Move the guest sprite to just at the end of the slide
                    newLocation.x += kSpiralSlideEnd[dir].x;
                    newLocation.y += kSpiralSlideEnd[dir].y;

                    moveTo({ newLocation, z });

                    orientation = (var37 & 0xC) * 2;

                    var37++;
                    return;
                }
                case PeepSpiralSlideSubState::slidingDown: // Handled by updateSpiralSlide in Ride.cpp
                default:
                    return;
            }
        }

        if (auto loc = updateAction(); loc.has_value())
        {
            moveTo({ loc.value(), z });
            return;
        }

        uint8_t waypoint = 2;
        var37 = (var37 * 4 & 0x30) + waypoint;

        CoordsXY targetLoc = ride->getStation(currentRideStation).Start;

        targetLoc += kSpiralSlideWalkingPath[var37];

        setDestination(targetLoc);
        rideSubState = PeepRideSubState::approachSpiralSlide;
    }

    /**
     *
     *  rct2: 0x00692C6B
     */
    void Guest::updateRideLeaveSpiralSlide()
    {
        // Iterates through the spiral slide waypoints until it reaches
        // waypoint 0. Then it readies to leave the ride by the entrance.
        if (auto loc = updateAction(); loc.has_value())
        {
            moveTo({ loc.value(), z });
            return;
        }

        auto ride = GetRide(currentRide);
        if (ride == nullptr)
            return;

        uint8_t waypoint = var37 & 3;

        if (waypoint != 0)
        {
            if (waypoint == 3)
            {
                updateRidePrepareForExit();
                return;
            }

            waypoint--;
            // Actually decrement the peep waypoint
            var37--;
            CoordsXY targetLoc = ride->getStation(currentRideStation).Start;

            [[maybe_unused]] const auto& rtd = ride->getRideTypeDescriptor();
            assert(rtd.specialType == RtdSpecialType::spiralSlide);
            targetLoc += kSpiralSlideWalkingPath[var37];

            setDestination(targetLoc);
            return;
        }

        // Actually force the final waypoint
        var37 |= 3;

        auto targetLoc = ride->getStation(currentRideStation).Exit.ToCoordsXYZD().ToTileCentre();

        int16_t xShift = DirectionOffsets[DirectionReverse(targetLoc.direction)].x;
        int16_t yShift = DirectionOffsets[DirectionReverse(targetLoc.direction)].y;

        int16_t shiftMultiplier = 20;

        xShift *= shiftMultiplier;
        yShift *= shiftMultiplier;

        targetLoc.x -= xShift;
        targetLoc.y -= yShift;

        setDestination(targetLoc);
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
    void Guest::updateRideMazePathfinding()
    {
        if (auto loc = updateAction(); loc.has_value())
        {
            moveTo({ loc.value(), z });
            return;
        }

        auto ride = GetRide(currentRide);
        if (ride == nullptr)
            return;

        if (var37 == 16)
        {
            updateRidePrepareForExit();
            return;
        }

        if (isActionInterruptable())
        {
            if (energy > 80 && !peepFlags.has(PeepFlag::slowWalk) && !Weather::isPrecipitating()
                && (ScenarioRand() & 0xFFFF) <= 2427)
            {
                action = PeepActionType::jump;
                animationFrameNum = 0;
                animationImageIdOffset = 0;
                updateCurrentAnimationType();
            }
        }

        auto targetLoc = getDestination().ToTileStart();

        auto stationBaseZ = ride->getStation().GetBaseZ();

        // Find the station track element
        auto trackElement = MapGetTrackElementAt({ targetLoc, stationBaseZ });
        if (trackElement == nullptr)
        {
            return;
        }

        uint16_t mazeEntry = trackElement->getMazeEntry();
        // Var37 is 3, 7, 11 or 15
        uint8_t hedges[4]{ 0xFF, 0xFF, 0xFF, 0xFF };
        uint8_t openCount = 0;
        uint8_t mazeReverseLastEdge = DirectionReverse(mazeLastEdge);
        for (uint8_t i = 0; i < kNumOrthogonalDirections; ++i)
        {
            if (!(mazeEntry & (1 << kMazeCurrentDirectionToOpenHedge[var37 / 4][i])) && i != mazeReverseLastEdge)
            {
                hedges[openCount++] = i;
            }
        }

        if (openCount == 0)
        {
            if ((mazeEntry & (1 << kMazeCurrentDirectionToOpenHedge[var37 / 4][mazeReverseLastEdge])))
            {
                return;
            }
            hedges[openCount++] = mazeReverseLastEdge;
        }

        uint8_t chosenEdge = hedges[ScenarioRand() % openCount];
        assert(chosenEdge != 0xFF);

        targetLoc = getDestination() + CoordsDirectionDelta[chosenEdge] / 2;

        enum class MazeType
        {
            invalid,
            hedge,
            entranceOrExit
        };
        MazeType mazeType = MazeType::invalid;

        for (auto* tileElement : TileElementsView(targetLoc))
        {
            if (stationBaseZ != tileElement->getBaseZ())
                continue;

            if (tileElement->getType() == TileElementType::track)
            {
                mazeType = MazeType::hedge;
                break;
            }

            if (tileElement->getType() == TileElementType::entrance
                && tileElement->asEntrance()->getEntranceType() == EntranceType::rideExit)
            {
                mazeType = MazeType::entranceOrExit;
                break;
            }
        }

        switch (mazeType)
        {
            case MazeType::invalid:
                mazeLastEdge++;
                mazeLastEdge &= 3;
                return;
            case MazeType::hedge:
                setDestination(targetLoc);
                var37 = kMazeGetNewDirectionFromEdge[var37 / 4][chosenEdge];
                mazeLastEdge = chosenEdge;
                break;
            case MazeType::entranceOrExit:
                targetLoc = getDestination();
                if (chosenEdge & 1)
                {
                    targetLoc.x = targetLoc.ToTileCentre().x;
                }
                else
                {
                    targetLoc.y = targetLoc.ToTileCentre().y;
                }
                setDestination(targetLoc);
                var37 = 16;
                mazeLastEdge = chosenEdge;
                break;
        }

        if (auto loc = updateAction(); loc.has_value())
        {
            moveTo({ loc.value(), z });
        }
    }

    /**
     *
     *  rct2: 0x006938D2
     */
    void Guest::updateRideLeaveExit()
    {
        auto ride = GetRide(currentRide);

        if (auto loc = updateAction(); loc.has_value())
        {
            if (ride != nullptr)
            {
                moveTo({ loc.value(), ride->getStation(currentRideStation).GetBaseZ() });
            }
            return;
        }

        if (ride != nullptr)
        {
            onExitRide(*ride);

            if (peepFlags.has(PeepFlag::tracking))
            {
                auto ft = Formatter();
                formatNameTo(ft);
                ride->formatNameTo(ft);

                if (Config::Get().notifications.guestLeftRide)
                {
                    News::AddItemToQueue(News::ItemType::peepOnRide, STR_PEEP_TRACKING_LEFT_RIDE_X, id, ft);
                }
            }
        }

        interactionRideIndex = RideId::GetNull();
        setState(PeepState::falling);

        CoordsXY targetLoc = { x, y };

        // Find the station track element
        for (auto* pathElement : TileElementsView<PathElement>(targetLoc))
        {
            int16_t height = MapHeightFromSlope(targetLoc, pathElement->getSlopeDirection(), pathElement->isSloped());
            height += pathElement->getBaseZ();

            int16_t z_diff = z - height;
            if (z_diff > 0 || z_diff < -16)
                continue;

            moveTo({ x, y, height });
            return;
        }
    }

    /**
     *
     *  rct2: 0x0069299C
     */
    void Guest::updateRideShopApproach()
    {
        if (auto loc = updateAction(); loc.has_value())
        {
            moveTo({ loc.value(), z });
            return;
        }

        rideSubState = PeepRideSubState::interactShop;
    }

    /**
     *
     *  rct2: 0x006929BB
     */
    void Guest::updateRideShopInteract()
    {
        auto ride = GetRide(currentRide);
        if (ride == nullptr)
            return;

        const int16_t tileCentreX = nextLoc.x + 16;
        const int16_t tileCentreY = nextLoc.y + 16;

        const auto& rtd = ride->getRideTypeDescriptor();
        if (rtd.specialType == RtdSpecialType::firstAid)
        {
            if (nausea <= 35)
            {
                rideSubState = PeepRideSubState::leaveShop;

                setDestination({ tileCentreX, tileCentreY }, 3);
                happinessTarget = std::min(happinessTarget + 30, kPeepMaxHappiness);
                happiness = happinessTarget;
            }
            else
            {
                nausea--;
                nauseaTarget = nausea;
            }
            return;
        }

        if (toilet != 0)
        {
            toilet--;
            return;
        }

        // Do not play toilet flush sound on title screen as it's considered loud and annoying
        if (gLegacyScene != LegacyScene::titleSequence)
        {
            Audio::Play3D(Audio::SoundId::toiletFlush, getLocation());
        }

        rideSubState = PeepRideSubState::leaveShop;

        setDestination({ tileCentreX, tileCentreY }, 3);

        happinessTarget = std::min(happinessTarget + 30, kPeepMaxHappiness);
        happiness = happinessTarget;
        GuestStopPurchaseThought(*this, ride->type);
    }

    /**
     *
     *  rct2: 0x00692935
     */
    void Guest::updateRideShopLeave()
    {
        if (auto loc = updateAction(); loc.has_value())
        {
            const auto curLoc = getLocation();
            moveTo({ loc.value(), curLoc.z });

            const auto newLoc = getLocation().ToTileStart();
            if (newLoc.x != nextLoc.x)
                return;
            if (newLoc.y != nextLoc.y)
                return;
        }

        // #11758 Previously SetState(PeepState::walking) caused Peeps to double-back to exit point of shop
        setState(PeepState::falling);

        auto ride = GetRide(currentRide);
        if (ride != nullptr)
        {
            ride->totalCustomers = AddClamp(ride->totalCustomers, 1u);
            ride->windowInvalidateFlags.set(RideInvalidateFlag::customers);
            ride->updateSatisfaction(happiness / 64);
        }
    }

    /* From peep_update */
    static void GuestUpdatethoughts(Guest& guest)
    {
        // Thoughts must always have a gap of at least
        // 220 ticks in age between them. In order to
        // allow this when a thought is new it enters
        // a holding zone. Before it becomes fresh.
        int32_t add_fresh = 1;
        int32_t fresh_thought = -1;
        for (int32_t i = 0; i < kPeepMaxThoughts; i++)
        {
            if (guest.thoughts[i].type == PeepThoughtType::none)
                break;

            if (guest.thoughts[i].freshness == 1)
            {
                add_fresh = 0;
                // If thought is fresh we wait 220 ticks
                // before allowing a new thought to become fresh.
                if (++guest.thoughts[i].fresh_timeout >= 220)
                {
                    guest.thoughts[i].fresh_timeout = 0;
                    // Thought is no longer fresh
                    guest.thoughts[i].freshness++;
                    add_fresh = 1;
                }
            }
            else if (guest.thoughts[i].freshness > 1)
            {
                if (++guest.thoughts[i].fresh_timeout == 0)
                {
                    // When thought is older than ~6900 ticks remove it
                    if (++guest.thoughts[i].freshness >= 28)
                    {
                        guest.windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_THOUGHTS;

                        // Clear top thought, push others up
                        if (i < kPeepMaxThoughts - 2)
                        {
                            memmove(
                                &guest.thoughts[i], &guest.thoughts[i + 1], sizeof(PeepThought) * (kPeepMaxThoughts - i - 1));
                        }
                        guest.thoughts[kPeepMaxThoughts - 1].type = PeepThoughtType::none;
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
            guest.thoughts[fresh_thought].freshness = 1;
            guest.windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_THOUGHTS;
        }
    }

    void Guest::update()
    {
        if (peepFlags.has(PeepFlag::positionFrozen))
        {
            if (!peepFlags.has(PeepFlag::animationFrozen))
            {
                // This is circumventing other logic, so only update every few ticks
                if ((getGameState().currentTicks & 3) == 0)
                {
                    if (isActionWalking())
                        updateWalkingAnimation();
                    else
                        updateActionAnimation();
                    invalidate();
                }
            }
            return;
        }
        else if (peepFlags.has(PeepFlag::animationFrozen))
        {
            // Animation is frozen while position is not. This allows a peep to walk
            // around without its sprite being updated, which looks very glitchy.
            // We'll just remove the flag and continue as normal, in this case.
            peepFlags.unset(PeepFlag::animationFrozen);
        }

        if (!previousRide.IsNull())
        {
            if (++previousRideTimeOut >= 720)
            {
                previousRide = RideId::GetNull();
            }
        }

        GuestUpdatethoughts(*this);

        // Walking speed logic
        const auto stepsToTake = getStepsToTake();
        const auto carryCheck = stepProgress + stepsToTake;
        stepProgress = carryCheck;

        if (carryCheck <= 255)
        {
            updateEasterEggInteractions();
        }
        else
        {
            // Loc68FD2F
            switch (state)
            {
                case PeepState::falling:
                    updateFalling();
                    break;
                case PeepState::one:
                    update1();
                    break;
                case PeepState::onRide:
                    // No action
                    break;
                case PeepState::picked:
                    updatePicked();
                    break;
                case PeepState::queuingFront:
                    updateRide();
                    break;
                case PeepState::leavingRide:
                    updateRide();
                    break;
                case PeepState::walking:
                    updateWalking();
                    break;
                case PeepState::queuing:
                    updateQueuing();
                    break;
                case PeepState::enteringRide:
                    updateRide();
                    break;
                case PeepState::sitting:
                    updateSitting();
                    break;
                case PeepState::enteringPark:
                    updateEnteringPark();
                    break;
                case PeepState::leavingPark:
                    updateLeavingPark();
                    break;
                case PeepState::buying:
                    updateBuying();
                    break;
                case PeepState::watching:
                    updateWatching();
                    break;
                case PeepState::usingBin:
                    updateUsingBin();
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
    void Guest::updateRide()
    {
        nextFlags &= ~PEEP_NEXT_FLAG_IS_SLOPED;

        switch (rideSubState)
        {
            case PeepRideSubState::atEntrance:
                updateRideAtEntrance();
                break;
            case PeepRideSubState::inEntrance:
                updateRideAdvanceThroughEntrance();
                break;
            case PeepRideSubState::freeVehicleCheck:
                updateRideFreeVehicleCheck();
                break;
            case PeepRideSubState::leaveEntrance:
                updateRideAdvanceThroughEntrance();
                break;
            case PeepRideSubState::approachVehicle:
                updateRideApproachVehicle();
                break;
            case PeepRideSubState::enterVehicle:
                updateRideEnterVehicle();
                break;
            case PeepRideSubState::onRide:
                // No action, on ride.
                break;
            case PeepRideSubState::leaveVehicle:
                updateRideLeaveVehicle();
                break;
            case PeepRideSubState::approachExit:
                updateRideApproachExit();
                break;
            case PeepRideSubState::inExit:
                updateRideInExit();
                break;
            case PeepRideSubState::approachVehicleWaypoints:
                updateRideApproachVehicleWaypoints();
                break;
            case PeepRideSubState::approachExitWaypoints:
                updateRideApproachExitWaypoints();
                break;
            case PeepRideSubState::approachSpiralSlide:
                updateRideApproachSpiralSlide();
                break;
            case PeepRideSubState::onSpiralSlide:
                updateRideOnSpiralSlide();
                break;
            case PeepRideSubState::leaveSpiralSlide:
                updateRideLeaveSpiralSlide();
                break;
            case PeepRideSubState::mazePathfinding:
                updateRideMazePathfinding();
                break;
            case PeepRideSubState::leaveExit:
                updateRideLeaveExit();
                break;
            case PeepRideSubState::approachShop:
                updateRideShopApproach();
                break;
            case PeepRideSubState::interactShop:
                updateRideShopInteract();
                break;
            case PeepRideSubState::leaveShop:
                updateRideShopLeave();
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
    void Guest::updateWalking()
    {
        if (!checkForPath())
            return;

        const auto currentTicks = getGameState().currentTicks;

        if (isActionInterruptableSafely())
        {
            PeepActionType NewAction = action;

            if (peepFlags.has(PeepFlag::waving) && (0xFFFF & ScenarioRand()) < 936)
                NewAction = PeepActionType::wave2;
            else if (peepFlags.has(PeepFlag::photo) && (0xFFFF & ScenarioRand()) < 936)
                NewAction = PeepActionType::takePhoto;
            else if (peepFlags.has(PeepFlag::painting) && (0xFFFF & ScenarioRand()) < 936)
                NewAction = PeepActionType::drawPicture;

            if (NewAction != action)
            {
                action = NewAction;
                animationFrameNum = 0;
                animationImageIdOffset = 0;
                updateCurrentAnimationType();
            }
        }

        if (peepFlags.has(PeepFlag::litter))
        {
            if (!getNextIsSurface())
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
                    const auto loc = getLocation();
                    int32_t litterX = loc.x + (ScenarioRand() & 0x7) - 3;
                    int32_t litterY = loc.y + (ScenarioRand() & 0x7) - 3;
                    Direction litterDirection = (ScenarioRand() & 0x3);

                    Litter::create({ litterX, litterY, loc.z, litterDirection }, litterType);
                }
            }
        }
        else if (hasEmptyContainer())
        {
            if ((!getNextIsSurface()) && (static_cast<uint32_t>(id.ToUnderlying() & 0x1FF) == (currentTicks & 0x1FF))
                && ((0xFFFF & ScenarioRand()) <= 4096))
            {
                int32_t container = Numerics::bitScanForward(getEmptyContainerFlags());
                auto litterType = Litter::Type::vomit;

                if (container != -1)
                {
                    auto item = static_cast<ShopItem>(container);
                    removeItem(item);
                    litterType = Litter::Type(GetShopItemDescriptor(item).Type);
                }

                windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                updateAnimationGroup();

                const auto loc = getLocation();
                int32_t litterX = loc.x + (ScenarioRand() & 0x7) - 3;
                int32_t litterY = loc.y + (ScenarioRand() & 0x7) - 3;
                Direction litterDirection = (ScenarioRand() & 0x3);

                Litter::create({ litterX, litterY, loc.z, litterDirection }, litterType);
            }
        }

        if (shouldWaitForLevelCrossing())
        {
            // Wait for vehicle to pass
            updateWaitingAtCrossing();

            return;
        }

        const auto [pathingResult, _] = performNextAction();
        if (!(pathingResult & PATHING_DESTINATION_REACHED))
            return;

        if (getNextIsSurface())
        {
            auto surfaceElement = MapGetSurfaceElementAt(nextLoc);

            if (surfaceElement != nullptr)
            {
                int32_t water_height = surfaceElement->getWaterHeight();
                if (water_height > 0)
                {
                    moveTo({ x, y, water_height });
                    setState(PeepState::falling);
                    return;
                }
            }
        }

        checkIfLost();
        checkCantFindRide();
        checkCantFindExit();

        if (updateWalkingFindBench())
            return;

        if (updateWalkingFindBin())
            return;

        GuestUpdateWalkingBreakScenery(*this);

        if (state != PeepState::walking)
            return;

        if (peepFlags.has(PeepFlag::leavingPark))
            return;

        if (nausea > 140)
            return;

        if (happiness < 120)
            return;

        if (toilet > 140)
            return;

        uint16_t chance = hasFoodOrDrink() ? 13107 : 2849;

        if ((ScenarioRand() & 0xFFFF) > chance)
            return;

        if (getNextIsSurface() || getNextIsSloped())
            return;

        TileElement* tileElement = MapGetFirstElementAt(nextLoc);
        if (tileElement == nullptr)
            return;

        for (;; tileElement++)
        {
            if (tileElement->getType() == TileElementType::path)
            {
                if (nextLoc.z == tileElement->getBaseZ())
                    break;
            }
            if (tileElement->isLastForTile())
            {
                return;
            }
        }

        // Only watch rides if the guest is not underground
        if (MapIsLocationUnderground(nextLoc))
            return;

        int32_t positions_free = 15;

        if (tileElement->asPath()->hasAddition())
        {
            if (!tileElement->asPath()->additionIsGhost())
            {
                auto* pathAddEntry = tileElement->asPath()->getAdditionEntry();
                if (pathAddEntry == nullptr)
                {
                    return;
                }

                if ((pathAddEntry->flags & PATH_ADDITION_FLAG_IS_BENCH))
                    positions_free = 9;
            }
        }

        int32_t edges = (tileElement->asPath()->getEdges()) ^ 0xF;
        if (edges == 0)
            return;

        uint8_t chosen_edge = ScenarioRand() & 0x3;

        while (!(edges & (1 << chosen_edge)))
            chosen_edge = (chosen_edge + 1) & 3;

        RideId ride_to_view;
        uint8_t ride_seat_to_view;
        if (isOnLevelCrossing() || !GuestFindRideToLookAt(*this, chosen_edge, &ride_to_view, &ride_seat_to_view))
            return;

        // Check if there is a peep watching (and if there is place for us)
        for (auto peep : EntityTileList<Peep>({ x, y }))
        {
            if (peep->state != PeepState::watching)
                continue;

            if (z != peep->z)
                continue;

            if ((peep->var37 & 0x3) != chosen_edge)
                continue;

            positions_free &= ~(1 << ((peep->var37 & 0x1C) >> 2));
        }

        if (!positions_free)
            return;

        uint8_t chosen_position = ScenarioRand() & 0x3;

        while (!(positions_free & (1 << chosen_position)))
            chosen_position = (chosen_position + 1) & 3;

        currentRide = ride_to_view;
        currentSeat = ride_seat_to_view;
        var37 = chosen_edge | (chosen_position << 2);

        setState(PeepState::watching);
        subState = 0;

        int32_t destX = (x & 0xFFE0) + kWatchingPositionOffsets[var37 & 0x1F].x;
        int32_t destY = (y & 0xFFE0) + kWatchingPositionOffsets[var37 & 0x1F].y;

        setDestination({ destX, destY }, 3);

        if (currentSeat & 1)
        {
            insertNewThought(PeepThoughtType::newRide);
        }
        if (currentRide.IsNull())
        {
            insertNewThought(PeepThoughtType::scenery);
        }
    }

    /**
     *
     *  rct2: 0x69185D
     */
    void Guest::updateQueuing()
    {
        if (!checkForPath())
        {
            removeFromQueue();
            return;
        }
        auto ride = GetRide(currentRide);
        if (ride == nullptr || ride->status != RideStatus::open)
        {
            removeFromQueue();
            setState(PeepState::one);
            return;
        }

        // If not in the queue then at front of queue
        if (rideSubState != PeepRideSubState::inQueue)
        {
            bool is_front = true;
            // Fix #4819: Occasionally the peep->guestNextInQueue is incorrectly set
            // to prevent this from causing the peeps to enter a loop
            // first check if the next in queue is actually nearby
            // if they are not then it's safe to assume that this is
            // the front of the queue.
            Peep* nextGuest = getGameState().entities.getEntity<Guest>(guestNextInQueue);
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
                destinationTolerance = 0;
                setState(PeepState::queuingFront);
                rideSubState = PeepRideSubState::atEntrance;
            }

            return;
        }

        performNextAction();
        if (!isActionInterruptable())
            return;
        if (animationGroup == PeepAnimationGroup::normal)
        {
            if (timeInQueue >= 2000 && (0xFFFF & ScenarioRand()) <= 119)
            {
                // Eat Food/Look at watch
                action = PeepActionType::eatFood;
                animationFrameNum = 0;
                animationImageIdOffset = 0;
                updateCurrentAnimationType();
            }
            if (timeInQueue >= 3500 && (0xFFFF & ScenarioRand()) <= 93)
            {
                // Create the I have been waiting in line ages thought
                insertNewThought(PeepThoughtType::queuingAges, currentRide);
            }
        }
        else
        {
            if (!(timeInQueue & 0x3F) && isActionIdle() && nextAnimationType == PeepAnimationType::watchRide)
            {
                switch (animationGroup)
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
                        action = PeepActionType::eatFood;
                        animationFrameNum = 0;
                        animationImageIdOffset = 0;
                        updateCurrentAnimationType();
                        break;
                    default:
                        break;
                }
            }
        }
        if (timeInQueue < 4300)
            return;

        if (happiness <= 65 && (0xFFFF & ScenarioRand()) < 2184)
        {
            // Give up queueing for the ride
            orientation ^= (1 << 4);
            invalidate();
            removeFromQueue();
            setState(PeepState::one);
        }
    }

    /**
     * rct2: 0x691451
     */
    void Guest::updateEnteringPark()
    {
        if (var37 != 1)
        {
            const auto [pathingResult, _] = performNextAction();
            if ((pathingResult & PATHING_OUTSIDE_PARK))
            {
                DecrementGuestsHeadingForPark();
                PeepEntityRemove(this);
            }
            return;
        }
        if (auto loc = updateAction(); loc.has_value())
        {
            moveTo({ loc.value(), z });
            return;
        }
        setState(PeepState::falling);

        outsideOfPark = false;
        parkEntryTime = getGameState().currentTicks;
        IncrementGuestsInPark();
        DecrementGuestsHeadingForPark();
        auto intent = Intent(INTENT_ACTION_UPDATE_GUEST_COUNT);
        ContextBroadcastIntent(&intent);
    }

    /**
     *
     *  rct2: 0x6914CD
     */
    void Guest::updateLeavingPark()
    {
        if (var37 != 0)
        {
            const auto [pathingResult, _] = performNextAction();
            if (!(pathingResult & PATHING_OUTSIDE_PARK))
                return;
            PeepEntityRemove(this);
            return;
        }

        if (auto loc = updateAction(); loc.has_value())
        {
            moveTo({ loc.value(), z });
            return;
        }

        outsideOfPark = true;
        destinationTolerance = 5;
        DecrementGuestsInPark();
        auto intent = Intent(INTENT_ACTION_UPDATE_GUEST_COUNT);
        ContextBroadcastIntent(&intent);
        var37 = 1;

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByClass(WindowClass::guestList);

        const auto [pathingResult, _] = performNextAction();
        if (!(pathingResult & PATHING_OUTSIDE_PARK))
            return;
        remove();
    }

    /**
     *
     *  rct2: 0x6916D6
     */
    void Guest::updateWatching()
    {
        if (subState == 0)
        {
            if (!checkForPath())
                return;

            const auto [pathingResult, _] = performNextAction();
            if (!(pathingResult & PATHING_DESTINATION_REACHED))
                return;

            setDestination(getLocation());

            orientation = (var37 & 3) * 8;

            action = PeepActionType::idle;
            nextAnimationType = PeepAnimationType::watchRide;

            switchNextAnimationType();

            subState++;

            timeToStand = std::clamp(((129 - energy) * 16 + 50) / 2, 0, 255);
            updateAnimationGroup();
        }
        else if (subState == 1)
        {
            if (!isActionInterruptable())
            {
                // 6917F6
                updateAction();
                invalidate();
                if (!isActionWalking())
                    return;
                action = PeepActionType::idle;
            }
            else
            {
                if (hasFoodOrDrink())
                {
                    if ((ScenarioRand() & 0xFFFF) <= 1310)
                    {
                        action = PeepActionType::eatFood;
                        animationFrameNum = 0;
                        animationImageIdOffset = 0;
                        updateCurrentAnimationType();
                        return;
                    }
                }

                if ((ScenarioRand() & 0xFFFF) <= 655)
                {
                    action = PeepActionType::takePhoto;
                    animationFrameNum = 0;
                    animationImageIdOffset = 0;
                    updateCurrentAnimationType();
                    return;
                }

                if ((standingFlags & 1))
                {
                    if ((ScenarioRand() & 0xFFFF) <= 655)
                    {
                        action = PeepActionType::wave;
                        animationFrameNum = 0;
                        animationImageIdOffset = 0;
                        updateCurrentAnimationType();
                        return;
                    }
                }
            }

            standingFlags ^= (1 << 7);
            if (!(standingFlags & (1 << 7)))
                return;

            timeToStand--;
            if (timeToStand != 0)
                return;

            setState(PeepState::walking);
            updateAnimationGroup();
            // Send peep to the centre of current tile.

            auto destination = getLocation().ToTileCentre();
            setDestination(destination, 5);
            updateCurrentAnimationType();
        }
    }

    /**
     *
     *  rct2: 0x00691089
     */
    void Guest::updateUsingBin()
    {
        switch (usingBinSubState)
        {
            case PeepUsingBinSubState::walkingToBin:
            {
                if (!checkForPath())
                    return;

                const auto [pathingResult, _] = performNextAction();
                if (pathingResult & PATHING_DESTINATION_REACHED)
                {
                    usingBinSubState = PeepUsingBinSubState::goingBack;
                }
                break;
            }
            case PeepUsingBinSubState::goingBack:
            {
                if (!isActionWalking())
                {
                    updateAction();
                    invalidate();
                    return;
                }

                PathElement* foundElement = nullptr;
                for (auto* pathElement : TileElementsView<PathElement>(nextLoc))
                {
                    if (pathElement->getBaseZ() != nextLoc.z)
                        continue;

                    if (!pathElement->hasAddition())
                        break;

                    auto* pathAddEntry = pathElement->getAdditionEntry();
                    if (!(pathAddEntry->flags & PATH_ADDITION_FLAG_IS_BIN))
                        break;

                    if (pathElement->isBroken())
                        break;

                    if (pathElement->additionIsGhost())
                        break;

                    foundElement = pathElement;
                    break;
                }

                if (foundElement == nullptr)
                {
                    stateReset();
                    return;
                }

                // Bin selection is one of 4 corners
                uint8_t selectedBin = var37 * 2;

                // This counts down 2 = No rubbish, 0 = full
                uint8_t spaceLeftInBin = 0x3 & (foundElement->getAdditionStatus() >> selectedBin);
                uint64_t emptyContainers = getEmptyContainerFlags();

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
                        removeItem(item);
                        windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                        updateAnimationGroup();
                        continue;
                    }

                    auto litterType = Litter::Type(GetShopItemDescriptor(item).Type);

                    int32_t litterX = x + (ScenarioRand() & 7) - 3;
                    int32_t litterY = y + (ScenarioRand() & 7) - 3;

                    Litter::create({ litterX, litterY, z, static_cast<Direction>(ScenarioRand() & 3) }, litterType);
                    removeItem(item);
                    windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;

                    updateAnimationGroup();
                }

                uint8_t additionStatus = foundElement->getAdditionStatus();
                // Place new amount in bin by first clearing the value
                additionStatus &= ~(3 << selectedBin);
                // Then placing the new value.
                additionStatus |= spaceLeftInBin << selectedBin;
                foundElement->setAdditionStatus(additionStatus);

                MapInvalidateTileZoom0({ nextLoc, foundElement->getBaseZ(), foundElement->getClearanceZ() });
                stateReset();
                break;
            }
            default:
                Guard::Assert(false, "Invalid sub state");
                break;
        }
    }

    /* Simplifies 0x690582. Returns true if should find bench*/
    bool Guest::shouldFindBench()
    {
        if (peepFlags.has(PeepFlag::leavingPark))
        {
            return false;
        }

        if (hasFoodOrDrink())
        {
            if (hunger < 128 || happiness < 128)
            {
                if (!getNextIsSurface() && !getNextIsSloped())
                {
                    return true;
                }
            }
        }

        if (nausea <= 170 && energy > 50)
        {
            return false;
        }

        return !getNextIsSurface() && !getNextIsSloped();
    }

    static PathElement* FindBench(const CoordsXYZ& loc)
    {
        for (auto* pathElement : TileElementsView<PathElement>(loc))
        {
            if (pathElement->getBaseZ() != loc.z)
                continue;

            if (!pathElement->hasAddition())
                continue;

            auto* pathAddEntry = pathElement->getAdditionEntry();
            if (pathAddEntry == nullptr || !(pathAddEntry->flags & PATH_ADDITION_FLAG_IS_BENCH))
                continue;

            if (pathElement->isBroken())
                continue;

            if (pathElement->additionIsGhost())
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
    bool Guest::updateWalkingFindBench()
    {
        if (!shouldFindBench())
            return false;

        auto* pathElement = FindBench(nextLoc);
        if (pathElement == nullptr)
            return false;

        int32_t edges = pathElement->getEdges() ^ 0xF;
        if (edges == 0)
            return false;
        uint8_t chosen_edge = ScenarioRand() & 0x3;

        while (!(edges & (1 << chosen_edge)))
            chosen_edge = (chosen_edge + 1) & 0x3;

        uint8_t free_edge = 3;

        // Check if there is no peep sitting in chosen_edge
        for (auto peep : EntityTileList<Peep>({ x, y }))
        {
            if (peep->state != PeepState::sitting)
                continue;

            if (z != peep->z)
                continue;

            if ((peep->var37 & 0x3) != chosen_edge)
                continue;

            free_edge &= ~(1 << ((peep->var37 & 0x4) >> 2));
        }

        if (!free_edge)
            return false;

        free_edge ^= 0x3;
        if (!free_edge)
        {
            if (ScenarioRand() & 0x8000000)
                free_edge = 1;
        }

        var37 = ((free_edge & 1) << 2) | chosen_edge;

        setState(PeepState::sitting);

        sittingSubState = PeepSittingSubState::tryingToSit;

        int32_t benchX = (x & 0xFFE0) + BenchUseOffsets[var37 & 0x7].x;
        int32_t benchY = (y & 0xFFE0) + BenchUseOffsets[var37 & 0x7].y;

        setDestination({ benchX, benchY }, 3);

        return true;
    }

    static PathElement* FindBin(const CoordsXYZ& loc)
    {
        for (auto* pathElement : TileElementsView<PathElement>(loc))
        {
            if (pathElement->getBaseZ() != loc.z)
                continue;

            if (!pathElement->hasAddition())
                continue;

            auto* pathAddEntry = pathElement->getAdditionEntry();
            if (pathAddEntry == nullptr || !(pathAddEntry->flags & PATH_ADDITION_FLAG_IS_BIN))
                continue;

            if (pathElement->isBroken())
                continue;

            if (pathElement->additionIsGhost())
                continue;

            return pathElement;
        }

        return nullptr;
    }

    bool Guest::updateWalkingFindBin()
    {
        auto peep = this;
        if (!peep->hasEmptyContainer())
            return false;

        if (peep->getNextIsSurface())
            return false;

        auto* pathElement = FindBin(peep->nextLoc);
        if (pathElement == nullptr)
            return false;

        int32_t edges = (pathElement->getEdges()) ^ 0xF;
        if (edges == 0)
            return false;

        uint8_t chosen_edge = ScenarioRand() & 0x3;

        // Note: Bin quantity is inverted 0 = full, 3 = empty
        uint8_t bin_quantities = pathElement->getAdditionStatus();

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

        peep->var37 = chosen_edge;

        peep->setState(PeepState::usingBin);
        peep->usingBinSubState = PeepUsingBinSubState::walkingToBin;

        int32_t binX = (peep->x & 0xFFE0) + BinUseOffsets[peep->var37 & 0x3].x;
        int32_t binY = (peep->y & 0xFFE0) + BinUseOffsets[peep->var37 & 0x3].y;

        peep->setDestination({ binX, binY }, 3);

        return true;
    }

    static PathElement* FindBreakableElement(const CoordsXYZ& loc)
    {
        for (auto* pathElement : TileElementsView<PathElement>(loc))
        {
            if (pathElement->getBaseZ() != loc.z)
                continue;

            if (!pathElement->hasAddition())
                continue;

            auto* pathAddEntry = pathElement->getAdditionEntry();
            if (pathAddEntry == nullptr || !(pathAddEntry->flags & PATH_ADDITION_FLAG_BREAKABLE))
                continue;

            if (pathElement->isBroken())
                continue;

            if (pathElement->additionIsGhost())
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

        if (!guest.peepFlags.has(PeepFlag::angry))
        {
            if (guest.happiness >= 48)
                return;
            if (guest.energy < 85)
                return;
            if (guest.state != PeepState::walking)
                return;

            if ((guest.litterCount & 0xC0) != 0xC0 && (guest.disgustingCount & 0xC0) != 0xC0)
                return;

            if ((ScenarioRand() & 0xFFFF) > 3276)
                return;
        }

        if (guest.getNextIsSurface())
            return;

        auto* tileElement = FindBreakableElement(guest.nextLoc);
        if (tileElement == nullptr)
            return;

        int32_t edges = tileElement->getEdges();
        if (edges == 0xF)
            return;

        // Check if a peep is already sitting on the bench. If so, do not vandalise it.
        for (auto* otherGuest : EntityTileList<Peep>({ guest.x, guest.y }))
        {
            if ((otherGuest->state != PeepState::sitting) || (guest.z != otherGuest->z))
            {
                continue;
            }

            return;
        }

        for (auto innerPeep : EntityList<Staff>())
        {
            if (innerPeep->assignedStaffType != StaffType::security || innerPeep->x == kLocationNull)
                continue;

            int32_t xDist = abs(innerPeep->x - guest.x);
            int32_t yDist = abs(innerPeep->y - guest.y);

            if (std::max(xDist, yDist) < 224)
            {
                innerPeep->staffVandalsStopped = AddClamp(innerPeep->staffVandalsStopped, 1u);
                innerPeep->windowInvalidateFlags |= PEEP_INVALIDATE_STAFF_STATS;
                return;
            }
        }

        tileElement->setIsBroken(true);

        MapInvalidateTileZoom1({ guest.nextLoc, tileElement->getBaseZ(), tileElement->getBaseZ() + 32 });

        guest.angriness = 16;
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
            if (tileElement->isGhost())
                return false;
        }

        auto ride = GetRide(tileElement->asTrack()->getRideIndex());
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

        if (ride->getRideTypeDescriptor().flags.has(RtdFlag::interestingToLookAt))
        {
            if ((ScenarioRand() & 0xFFFF) > 0x3333)
            {
                return false;
            }
        }
        else if (ride->getRideTypeDescriptor().flags.has(RtdFlag::slightlyInterestingToLookAt))
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
        auto ride = GetRide(tileElement->asTrack()->getRideIndex());
        if (ride == nullptr)
            return false;

        *rideToView = ride->id;
        if (ride->ratings.isNull())
        {
            *rideSeatToView = 1;
            if (ride->status != RideStatus::open)
            {
                if (tileElement->getClearanceZ() > guest.nextLoc.z + (8 * kCoordsZStep))
                {
                    *rideSeatToView |= (1 << 1);
                }

                return true;
            }
        }
        else
        {
            *rideSeatToView = 0;
            if (ride->status == RideStatus::open && !ride->flags.has(RideFlag::brokenDown))
            {
                if (tileElement->getClearanceZ() > guest.nextLoc.z + (8 * kCoordsZStep))
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
        auto surfaceElement = MapGetSurfaceElementAt(guest.nextLoc);
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
                if (tileElement->isGhost())
                    continue;
            }
            if (tileElement->getType() != TileElementType::wall)
                continue;
            if (tileElement->getDirection() != edge)
                continue;
            auto wallEntry = tileElement->asWall()->getEntry();
            if (wallEntry == nullptr || (wallEntry->flags2.has(WallSceneryFlag2::isTransparent)))
                continue;
            if (guest.nextLoc.z + (4 * kCoordsZStep) <= tileElement->getBaseZ())
                continue;
            if (guest.nextLoc.z + (1 * kCoordsZStep) >= tileElement->getClearanceZ())
                continue;

            return false;
        } while (!(tileElement++)->isLastForTile());

        uint16_t x = guest.nextLoc.x + CoordsDirectionDelta[edge].x;
        uint16_t y = guest.nextLoc.y + CoordsDirectionDelta[edge].y;
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

        // TODO: Extract loop A
        do
        {
            // Ghosts are purely this-client-side and should not cause any interaction,
            // as that may lead to a desync.
            if (Network::GetMode() != Network::Mode::none)
            {
                if (tileElement->isGhost())
                    continue;
            }
            if (tileElement->getType() != TileElementType::wall)
                continue;
            if (DirectionReverse(tileElement->getDirection()) != edge)
                continue;
            auto wallEntry = tileElement->asWall()->getEntry();
            if (wallEntry == nullptr || (wallEntry->flags2.has(WallSceneryFlag2::isTransparent)))
                continue;
            if (guest.nextLoc.z + (4 * kCoordsZStep) <= tileElement->getBaseZ())
                continue;
            if (guest.nextLoc.z + (1 * kCoordsZStep) >= tileElement->getClearanceZ())
                continue;

            return false;
        } while (!(tileElement++)->isLastForTile());

        // TODO: Extract loop B
        tileElement = reinterpret_cast<TileElement*>(surfaceElement);
        do
        {
            // Ghosts are purely this-client-side and should not cause any interaction,
            // as that may lead to a desync.
            if (Network::GetMode() != Network::Mode::none)
            {
                if (tileElement->isGhost())
                    continue;
            }

            if (tileElement->getClearanceZ() + (1 * kCoordsZStep) < guest.nextLoc.z)
                continue;
            if (guest.nextLoc.z + (6 * kCoordsZStep) < tileElement->getBaseZ())
                continue;

            if (tileElement->getType() == TileElementType::track)
            {
                if (PeepShouldWatchRide(tileElement))
                {
                    return Loc690FD0(guest, rideToView, rideSeatToView, tileElement);
                }
            }

            if (tileElement->getType() == TileElementType::largeScenery)
            {
                const auto* sceneryEntry = tileElement->asLargeScenery()->getEntry();
                if (sceneryEntry == nullptr || !sceneryEntry->flags.has(LargeSceneryFlag::isPhotogenic))
                {
                    continue;
                }

                *rideSeatToView = 0;
                if (tileElement->getClearanceZ() >= guest.nextLoc.z + (8 * kCoordsZStep))
                {
                    *rideSeatToView = 0x02;
                }

                *rideToView = RideId::GetNull();

                return true;
            }
        } while (!(tileElement++)->isLastForTile());

        // TODO: Extract loop C
        tileElement = reinterpret_cast<TileElement*>(surfaceElement);
        do
        {
            // Ghosts are purely this-client-side and should not cause any interaction,
            // as that may lead to a desync.
            if (Network::GetMode() != Network::Mode::none)
            {
                if (tileElement->isGhost())
                    continue;
            }
            if (tileElement->getClearanceZ() + (1 * kCoordsZStep) < guest.nextLoc.z)
                continue;
            if (guest.nextLoc.z + (6 * kCoordsZStep) < tileElement->getBaseZ())
                continue;
            if (tileElement->getType() == TileElementType::surface)
                continue;
            if (tileElement->getType() == TileElementType::path)
                continue;

            if (tileElement->getType() == TileElementType::wall)
            {
                auto wallEntry = tileElement->asWall()->getEntry();
                if (wallEntry == nullptr || (wallEntry->flags2.has(WallSceneryFlag2::isTransparent)))
                {
                    continue;
                }
            }

            return false;
        } while (!(tileElement++)->isLastForTile());

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
                if (tileElement->isGhost())
                    continue;
            }
            if (tileElement->getType() != TileElementType::wall)
                continue;
            if (DirectionReverse(tileElement->getDirection()) != edge)
                continue;
            auto wallEntry = tileElement->asWall()->getEntry();
            if (wallEntry == nullptr || (wallEntry->flags2.has(WallSceneryFlag2::isTransparent)))
                continue;
            if (guest.nextLoc.z + (6 * kCoordsZStep) <= tileElement->getBaseZ())
                continue;
            if (guest.nextLoc.z >= tileElement->getClearanceZ())
                continue;

            return false;
        } while (!(tileElement++)->isLastForTile());

        // TODO: Extract loop B
        tileElement = reinterpret_cast<TileElement*>(surfaceElement);
        do
        {
            // Ghosts are purely this-client-side and should not cause any interaction,
            // as that may lead to a desync.
            if (Network::GetMode() != Network::Mode::none)
            {
                if (tileElement->isGhost())
                    continue;
            }
            if (tileElement->getClearanceZ() + (1 * kCoordsZStep) < guest.nextLoc.z)
                continue;
            if (guest.nextLoc.z + (8 * kCoordsZStep) < tileElement->getBaseZ())
                continue;

            if (tileElement->getType() == TileElementType::track)
            {
                if (PeepShouldWatchRide(tileElement))
                {
                    return Loc690FD0(guest, rideToView, rideSeatToView, tileElement);
                }
            }

            if (tileElement->getType() == TileElementType::largeScenery)
            {
                auto* sceneryEntry = tileElement->asLargeScenery()->getEntry();
                if (!(sceneryEntry == nullptr || sceneryEntry->flags.has(LargeSceneryFlag::isPhotogenic)))
                {
                    continue;
                }

                *rideSeatToView = 0;
                if (tileElement->getClearanceZ() >= guest.nextLoc.z + (8 * kCoordsZStep))
                {
                    *rideSeatToView = 0x02;
                }

                *rideToView = RideId::GetNull();

                return true;
            }
        } while (!(tileElement++)->isLastForTile());

        // TODO: Extract loop C
        tileElement = reinterpret_cast<TileElement*>(surfaceElement);
        do
        {
            // Ghosts are purely this-client-side and should not cause any interaction,
            // as that may lead to a desync.
            if (Network::GetMode() != Network::Mode::none)
            {
                if (tileElement->isGhost())
                    continue;
            }
            if (tileElement->getClearanceZ() + (1 * kCoordsZStep) < guest.nextLoc.z)
                continue;
            if (guest.nextLoc.z + (8 * kCoordsZStep) < tileElement->getBaseZ())
                continue;
            if (tileElement->getType() == TileElementType::surface)
                continue;
            if (tileElement->getType() == TileElementType::path)
                continue;

            if (tileElement->getType() == TileElementType::wall)
            {
                auto wallEntry = tileElement->asWall()->getEntry();
                if (wallEntry == nullptr || (wallEntry->flags2.has(WallSceneryFlag2::isTransparent)))
                {
                    continue;
                }
            }

            return false;
        } while (!(tileElement++)->isLastForTile());

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
                if (tileElement->isGhost())
                    continue;
            }
            if (tileElement->getType() != TileElementType::wall)
                continue;
            if (DirectionReverse(tileElement->getDirection()) != edge)
                continue;
            auto wallEntry = tileElement->asWall()->getEntry();
            if (wallEntry == nullptr || (wallEntry->flags2.has(WallSceneryFlag2::isTransparent)))
                continue;
            if (guest.nextLoc.z + (8 * kCoordsZStep) <= tileElement->getBaseZ())
                continue;
            if (guest.nextLoc.z >= tileElement->getClearanceZ())
                continue;

            return false;
        } while (!(tileElement++)->isLastForTile());

        // TODO: Extract loop B
        tileElement = reinterpret_cast<TileElement*>(surfaceElement);
        do
        {
            // Ghosts are purely this-client-side and should not cause any interaction,
            // as that may lead to a desync.
            if (Network::GetMode() != Network::Mode::none)
            {
                if (tileElement->isGhost())
                    continue;
            }
            if (tileElement->getClearanceZ() + (1 * kCoordsZStep) < guest.nextLoc.z)
                continue;
            if (guest.nextLoc.z + (10 * kCoordsZStep) < tileElement->getBaseZ())
                continue;

            if (tileElement->getType() == TileElementType::track)
            {
                if (PeepShouldWatchRide(tileElement))
                {
                    return Loc690FD0(guest, rideToView, rideSeatToView, tileElement);
                }
            }

            if (tileElement->getType() == TileElementType::largeScenery)
            {
                const auto* sceneryEntry = tileElement->asLargeScenery()->getEntry();
                if (sceneryEntry == nullptr || !sceneryEntry->flags.has(LargeSceneryFlag::isPhotogenic))
                {
                    continue;
                }

                *rideSeatToView = 0;
                if (tileElement->getClearanceZ() >= guest.nextLoc.z + (8 * kCoordsZStep))
                {
                    *rideSeatToView = 0x02;
                }

                *rideToView = RideId::GetNull();

                return true;
            }
        } while (!(tileElement++)->isLastForTile());

        return false;
    }

    /* Part of 0x0069B8CC rct2: 0x0069BC31 */
    void Guest::setAnimationGroup(PeepAnimationGroup new_sprite_type)
    {
        if (animationGroup == new_sprite_type)
            return;

        animationGroup = new_sprite_type;
        animationImageIdOffset = 0;
        walkingAnimationFrameNum = 0;

        if (isActionInterruptable())
            action = PeepActionType::walking;

        auto& objManager = GetContext()->GetObjectManager();
        auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(animationObjectIndex);

        peepFlags.unset(PeepFlag::slowWalk);
        if (animObj->IsSlowWalking(new_sprite_type))
        {
            peepFlags.set(PeepFlag::slowWalk);
        }

        animationType = PeepAnimationType::invalid;
        updateCurrentAnimationType();

        if (state == PeepState::sitting)
        {
            action = PeepActionType::idle;
            nextAnimationType = PeepAnimationType::sittingIdle;
            switchNextAnimationType();
        }
        if (state == PeepState::watching)
        {
            action = PeepActionType::idle;
            nextAnimationType = PeepAnimationType::watchRide;
            switchNextAnimationType();
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
    void Guest::updateAnimationGroup()
    {
        if (animationGroup == PeepAnimationGroup::balloon && (ScenarioRand() & 0xFFFF) <= 327)
        {
            bool isBalloonPopped = false;
            if (x != kLocationNull)
            {
                if ((ScenarioRand() & 0xFFFF) <= 13107)
                {
                    isBalloonPopped = true;
                    Audio::Play3D(Audio::SoundId::balloonPop, { x, y, z });
                }
                Balloon::create({ x, y, z + 9 }, balloonColour, isBalloonPopped);
            }
            removeItem(ShopItem::balloon);
            windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
        }

        if (Weather::isPrecipitating() && (hasItem(ShopItem::umbrella)) && x != kLocationNull)
        {
            CoordsXY loc = { x, y };
            if (MapIsLocationValid(loc.ToTileStart()))
            {
                TileElement* tileElement = MapGetFirstElementAt(loc);
                while (true)
                {
                    if (tileElement == nullptr)
                        break;
                    if (z < tileElement->getBaseZ())
                        break;

                    if (tileElement->isLastForTile())
                    {
                        setAnimationGroup(PeepAnimationGroup::umbrella);
                        return;
                    }
                    tileElement++;
                }
            }
        }

        for (auto& itemPref : item_order_preference)
        {
            if (hasItem(itemPref.item))
            {
                setAnimationGroup(itemPref.sprite_type);
                return;
            }
        }

        if (state == PeepState::watching && standingFlags & (1 << 1))
        {
            setAnimationGroup(PeepAnimationGroup::watching);
            return;
        }

        if (nausea > 170)
        {
            setAnimationGroup(PeepAnimationGroup::veryNauseous);
            return;
        }

        if (nausea > 140)
        {
            setAnimationGroup(PeepAnimationGroup::nauseous);
            return;
        }

        if (energy <= 64 && happiness < 128)
        {
            setAnimationGroup(PeepAnimationGroup::headDown);
            return;
        }

        if (energy <= 80 && happiness < 128)
        {
            setAnimationGroup(PeepAnimationGroup::armsCrossed);
            return;
        }

        if (toilet > 220)
        {
            setAnimationGroup(PeepAnimationGroup::requireToilet);
            return;
        }

        setAnimationGroup(PeepAnimationGroup::normal);
    }

    bool Guest::headingForRideOrParkExit() const
    {
        return peepFlags.has(PeepFlag::leavingPark) || !guestHeadingToRideId.IsNull();
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

    void Guest::insertNewThought(PeepThoughtType thought_type)
    {
        insertNewThought(thought_type, kPeepThoughtItemNone);
    }

    void Guest::insertNewThought(PeepThoughtType thought_type, ShopItem shopItem)
    {
        insertNewThought(thought_type, static_cast<uint16_t>(shopItem));
    }

    void Guest::insertNewThought(PeepThoughtType thought_type, RideId rideId)
    {
        insertNewThought(thought_type, rideId.ToUnderlying());
    }
    /**
     *
     *  rct2: 0x699F5A
     * al:thoughtType
     * ah:thoughtArguments
     * esi: peep
     */
    void Guest::insertNewThought(PeepThoughtType thoughtType, uint16_t thoughtArguments)
    {
        PeepActionType newAction = PeepThoughtToActionMap[EnumValue(thoughtType)].action;
        if (newAction != PeepActionType::walking && isActionInterruptableSafely())
        {
            action = newAction;
            animationFrameNum = 0;
            animationImageIdOffset = 0;
            updateCurrentAnimationType();
        }

        for (int32_t i = 0; i < kPeepMaxThoughts; ++i)
        {
            PeepThought* thought = &thoughts[i];
            // Remove the oldest thought by setting it to NONE.
            if (thought->type == PeepThoughtType::none)
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

        memmove(&std::get<1>(thoughts), &std::get<0>(thoughts), sizeof(PeepThought) * (kPeepMaxThoughts - 1));

        auto& thought = std::get<0>(thoughts);
        thought.type = thoughtType;
        thought.item = thoughtArguments;
        thought.freshness = 0;
        thought.fresh_timeout = 0;

        windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_THOUGHTS;
    }

    // clang-format off
    /** rct2: 0x009823A0 */
    static constexpr PeepNauseaTolerance nausea_tolerance_distribution[] = {
        PeepNauseaTolerance::none,
        PeepNauseaTolerance::low, PeepNauseaTolerance::low,
        PeepNauseaTolerance::average, PeepNauseaTolerance::average, PeepNauseaTolerance::average,
        PeepNauseaTolerance::high, PeepNauseaTolerance::high, PeepNauseaTolerance::high, PeepNauseaTolerance::high, PeepNauseaTolerance::high, PeepNauseaTolerance::high,
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
    Guest* Guest::generate(const CoordsXYZ& coords)
    {
        auto& gameState = getGameState();
        if (gameState.entities.getNumFreeEntities() < 400)
            return nullptr;

        Guest* peep = gameState.entities.createEntity<Guest>();

        peep->animationObjectIndex = findPeepAnimationsIndexForType(AnimationPeepType::guest);
        peep->animationGroup = PeepAnimationGroup::normal;
        peep->outsideOfPark = true;
        peep->state = PeepState::falling;
        peep->action = PeepActionType::walking;
        peep->specialSprite = 0;
        peep->animationImageIdOffset = 0;
        peep->walkingAnimationFrameNum = 0;
        peep->animationType = PeepAnimationType::walking;
        peep->peepFlags = {};
        peep->favouriteRide = RideId::GetNull();
        peep->favouriteRideRating = 0;

        auto& objManager = GetContext()->GetObjectManager();
        auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep->animationObjectIndex);

        const auto& spriteBounds = animObj->GetSpriteBounds(peep->animationGroup, peep->animationType);
        peep->spriteData.width = spriteBounds.spriteWidth;
        peep->spriteData.heightMin = spriteBounds.spriteHeightNegative;
        peep->spriteData.heightMax = spriteBounds.spriteHeightPositive;
        peep->orientation = 0;

        peep->moveTo(coords);
        peep->mass = (ScenarioRand() & 0x1F) + 45;
        peep->pathCheckOptimisation = 0;
        peep->interactionRideIndex = RideId::GetNull();
        peep->previousRide = RideId::GetNull();
        std::get<0>(peep->thoughts).type = PeepThoughtType::none;
        peep->windowInvalidateFlags = 0;

        uint8_t intensityHighest = (ScenarioRand() & 0x7) + 3;
        uint8_t intensityLowest = std::min(intensityHighest, static_cast<uint8_t>(7)) - 3;

        if (intensityHighest >= 7)
            intensityHighest = 15;

        /* Check which intensity boxes are enabled
         * and apply the appropriate intensity settings. */
        if (gameState.park.flags.has(ParkFlag::guestPreferLessIntenseRides))
        {
            if (gameState.park.flags.has(ParkFlag::guestPreferMoreIntenseRides))
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
        else if (gameState.park.flags.has(ParkFlag::guestPreferMoreIntenseRides))
        {
            intensityLowest = 9;
            intensityHighest = 15;
        }

        peep->intensity = IntensityRange(intensityLowest, intensityHighest);

        uint8_t nauseaTolerance = ScenarioRand() & 0x7;
        if (gameState.park.flags.has(ParkFlag::guestPreferMoreIntenseRides))
        {
            nauseaTolerance += 4;
        }

        peep->nauseaTolerance = nausea_tolerance_distribution[nauseaTolerance];

        /* Scenario editor limits initial guest happiness to between 37..253.
         * To be on the safe side, assume the value could have been hacked
         * to any value 0..255. */
        peep->happiness = gameState.scenarioOptions.guestInitialHappiness;
        /* Assume a default initial happiness of 0 is wrong and set
         * to 128 (50%) instead. */
        if (gameState.scenarioOptions.guestInitialHappiness == 0)
            peep->happiness = 128;
        /* Initial value will vary by -15..16 */
        int8_t happinessDelta = (ScenarioRand() & 0x1F) - 15;
        /* Adjust by the delta, clamping at min=0 and max=255. */
        peep->happiness = std::clamp(peep->happiness + happinessDelta, 0, kPeepMaxHappiness);
        peep->happinessTarget = peep->happiness;
        peep->nausea = 0;
        peep->nauseaTarget = 0;

        /* Scenario editor limits initial guest hunger to between 37..253.
         * To be on the safe side, assume the value could have been hacked
         * to any value 0..255. */
        peep->hunger = gameState.scenarioOptions.guestInitialHunger;
        /* Initial value will vary by -15..16 */
        int8_t hungerDelta = (ScenarioRand() & 0x1F) - 15;
        /* Adjust by the delta, clamping at min=0 and max=255. */
        peep->hunger = std::clamp(peep->hunger + hungerDelta, 0, kPeepMaxHunger);

        /* Scenario editor limits initial guest thirst to between 37..253.
         * To be on the safe side, assume the value could have been hacked
         * to any value 0..255. */
        peep->thirst = gameState.scenarioOptions.guestInitialThirst;
        /* Initial value will vary by -15..16 */
        int8_t thirstDelta = (ScenarioRand() & 0x1F) - 15;
        /* Adjust by the delta, clamping at min=0 and max=255. */
        peep->thirst = std::clamp(peep->thirst + thirstDelta, 0, kPeepMaxThirst);

        peep->toilet = 0;
        peep->timeToConsume = 0;

        peep->guestNumRides = 0;
        peep->peepId = gameState.nextGuestNumber++;
        peep->name = nullptr;

        money64 cash = (static_cast<money64>(ScenarioRand() & 0x3) * 100) - 100 + gameState.scenarioOptions.guestInitialCash;
        if (cash < 0)
            cash = 0;

        if (gameState.scenarioOptions.guestInitialCash == 0.00_GBP)
        {
            cash = 500;
        }

        if (gameState.park.flags.has(ParkFlag::noMoney))
        {
            cash = 0;
        }

        if (gameState.scenarioOptions.guestInitialCash == kMoney64Undefined)
        {
            cash = 0;
        }

        peep->cashInPocket = cash;
        peep->cashSpent = 0;
        peep->parkEntryTime = -1;
        peep->resetPathfindGoal();
        peep->removeAllItems();
        peep->guestHeadingToRideId = RideId::GetNull();
        peep->guestNextInQueue = EntityId::GetNull();
        peep->litterCount = 0;
        peep->disgustingCount = 0;
        peep->vandalismSeen = 0;
        peep->paidToEnter = 0;
        peep->paidOnRides = 0;
        peep->paidOnFood = 0;
        peep->paidOnDrink = 0;
        peep->paidOnSouvenirs = 0;
        peep->amountOfFood = 0;
        peep->amountOfDrinks = 0;
        peep->amountOfSouvenirs = 0;
        peep->surroundingsThoughtTimeout = 0;
        peep->angriness = 0;
        peep->timeLost = 0;

        uint8_t tshirtColour = static_cast<uint8_t>(ScenarioRand() % std::size(kTshirtColours));
        peep->tShirtColour = kTshirtColours[tshirtColour];

        uint8_t trousersColour = static_cast<uint8_t>(ScenarioRand() % std::size(kTrouserColours));
        peep->trousersColour = kTrouserColours[trousersColour];

        /* Minimum energy is capped at 32 and maximum at 128, so this initialises
         * a peep with approx 34%-100% energy. (65 - 32) / (128 - 32) ≈ 34% */
        uint8_t energy = (ScenarioRand() % 64) + 65;
        peep->energy = energy;
        peep->energyTarget = energy;

        IncrementGuestsHeadingForPark();

#ifdef ENABLE_SCRIPTING
        auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
        if (hookEngine.HasSubscriptions(Scripting::HookType::guestGeneration))
        {
            hookEngine.Call(Scripting::HookType::guestGeneration, { { "id", peep->id.ToUnderlying() } }, true);
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
        if (peep->angriness > 0)
            return PEEP_FACE_OFFSET_ANGRY;

        // VERY_VERY_SICK
        if (peep->nausea > 200)
            return PEEP_FACE_OFFSET_VERY_VERY_SICK;

        // VERY_SICK
        if (peep->nausea > 170)
            return PEEP_FACE_OFFSET_VERY_SICK;

        // SICK
        if (peep->nausea > 140)
            return PEEP_FACE_OFFSET_SICK;

        // VERY_TIRED
        if (peep->energy < 46)
            return PEEP_FACE_OFFSET_VERY_TIRED;

        // TIRED
        if (peep->energy < 70)
            return PEEP_FACE_OFFSET_TIRED;

        int32_t offset = PEEP_FACE_OFFSET_VERY_VERY_UNHAPPY;
        // There are 7 different happiness based faces
        for (int32_t i = 37; peep->happiness >= i; i += 37)
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
    bool Guest::updateQueuePosition(PeepActionType previous_action)
    {
        timeInQueue = AddClamp<uint16_t>(timeInQueue, 1);

        auto* guestNext = getGameState().entities.getEntity<Guest>(guestNextInQueue);
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

            if (peepDirection != guestNext->peepDirection)
                return false;

            switch (guestNext->peepDirection)
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

        if (!isActionInterruptable())
            updateAction();

        if (!isActionWalking())
            return true;

        action = PeepActionType::idle;
        nextAnimationType = PeepAnimationType::watchRide;
        if (previous_action != PeepActionType::idle)
            invalidate();
        return true;
    }

    /**
     *
     *  rct2: 0x006966A9
     */
    void Guest::removeFromQueue()
    {
        auto ride = GetRide(currentRide);
        if (ride == nullptr)
            return;

        auto& station = ride->getStation(currentRideStation);
        // Make sure we don't underflow, building while paused might reset it to 0 where peeps have
        // not yet left the queue.
        if (station.QueueLength > 0)
        {
            station.QueueLength--;
        }

        if (id == station.LastPeepInQueue)
        {
            station.LastPeepInQueue = guestNextInQueue;
            return;
        }

        auto& gameState = getGameState();
        auto* otherGuest = gameState.entities.getEntity<Guest>(station.LastPeepInQueue);
        if (otherGuest == nullptr)
        {
            LOG_ERROR("Invalid Guest Queue list!");
            return;
        }
        for (; otherGuest != nullptr; otherGuest = gameState.entities.getEntity<Guest>(otherGuest->guestNextInQueue))
        {
            if (id == otherGuest->guestNextInQueue)
            {
                otherGuest->guestNextInQueue = guestNextInQueue;
                return;
            }
        }
    }

    uint64_t Guest::getItemFlags() const
    {
        return itemFlags;
    }

    void Guest::setItemFlags(uint64_t value)
    {
        itemFlags = value;
    }

    void Guest::removeAllItems()
    {
        itemFlags = 0;
        timeToConsume = 0;
    }

    void Guest::removeItem(ShopItem item)
    {
        itemFlags &= ~EnumToFlag(item);
        timeToConsume = 0;
    }

    void Guest::giveItem(ShopItem item)
    {
        itemFlags |= EnumToFlag(item);

        const auto& shopItemDescriptor = GetShopItemDescriptor(item);
        uint16_t consumptionTime = shopItemDescriptor.ConsumptionTime;
        timeToConsume = std::min((timeToConsume + consumptionTime), 255);
    }

    bool Guest::hasItem(ShopItem peepItem) const
    {
        return getItemFlags() & EnumToFlag(peepItem);
    }

    static bool IsThoughtShopItemRelated(const PeepThoughtType type)
    {
        switch (type)
        {
            case PeepThoughtType::alreadyGot:
            case PeepThoughtType::haventFinished:
            case PeepThoughtType::cantAffordItem:
                return true;
            default:
                break;
        }
        return false;
    }

    void Guest::removeRideFromMemory(RideId rideId)
    {
        if (state == PeepState::watching)
        {
            if (currentRide == rideId)
            {
                currentRide = RideId::GetNull();
                if (timeToStand >= 50)
                {
                    // make peep stop watching the ride
                    timeToStand = 50;
                }
            }
        }

        // remove any free voucher for this ride from peep
        if (hasItem(ShopItem::voucher))
        {
            if (voucherType == VOUCHER_TYPE_RIDE_FREE && voucherRideId == rideId)
            {
                removeItem(ShopItem::voucher);
            }
        }

        // remove any photos of this ride from peep
        if (hasItem(ShopItem::photo))
        {
            if (photo1RideRef == rideId)
            {
                removeItem(ShopItem::photo);
            }
        }
        if (hasItem(ShopItem::photo2))
        {
            if (photo2RideRef == rideId)
            {
                removeItem(ShopItem::photo2);
            }
        }
        if (hasItem(ShopItem::photo3))
        {
            if (photo3RideRef == rideId)
            {
                removeItem(ShopItem::photo3);
            }
        }
        if (hasItem(ShopItem::photo4))
        {
            if (photo4RideRef == rideId)
            {
                removeItem(ShopItem::photo4);
            }
        }

        if (guestHeadingToRideId == rideId)
        {
            guestHeadingToRideId = RideId::GetNull();
        }
        if (favouriteRide == rideId)
        {
            favouriteRide = RideId::GetNull();
        }

        // Erase all thoughts that contain the ride.
        for (auto it = std::begin(thoughts); it != std::end(thoughts);)
        {
            const auto& entry = *it;
            if (entry.type == PeepThoughtType::none)
                break;

            // Ride ids and shop item ids might have the same value, look only for ride thoughts.
            if (IsThoughtShopItemRelated(entry.type) || entry.rideId != rideId)
            {
                it++;
                continue;
            }

            if (auto itNext = std::next(it); itNext != std::end(thoughts))
            {
                // Overwrite this entry by shifting all entries that follow.
                std::rotate(it, itNext, std::end(thoughts));
            }

            // Last slot is now free.
            auto& lastEntry = thoughts.back();
            lastEntry.type = PeepThoughtType::none;
            lastEntry.item = kPeepThoughtItemNone;
        }
    }

    void Guest::throwUp()
    {
        hunger /= 2;
        nauseaTarget /= 2;

        if (nausea < 30)
            nausea = 0;
        else
            nausea -= 30;

        windowInvalidateFlags |= PEEP_INVALIDATE_PEEP_2;

        const auto curLoc = getLocation();
        Litter::create({ curLoc, orientation }, (id.ToUnderlying() & 1) ? Litter::Type::vomitAlt : Litter::Type::vomit);

        static constexpr Audio::SoundId coughs[4] = {
            Audio::SoundId::cough1,
            Audio::SoundId::cough2,
            Audio::SoundId::cough3,
            Audio::SoundId::cough4,
        };
        auto soundId = coughs[ScenarioRand() & 3];
        Audio::Play3D(soundId, curLoc);
    }

    void Guest::serialise(DataSerialiser& stream)
    {
        Peep::serialise(stream);
        stream << guestNumRides;
        stream << guestNextInQueue;
        stream << parkEntryTime;
        stream << guestHeadingToRideId;
        stream << guestIsLostCountdown;
        stream << guestTimeOnRide;
        stream << paidToEnter;
        stream << paidOnRides;
        stream << paidOnFood;
        stream << paidOnDrink;
        stream << paidOnSouvenirs;
        stream << outsideOfPark;
        stream << happiness;
        stream << happinessTarget;
        stream << nausea;
        stream << nauseaTarget;
        stream << hunger;
        stream << thirst;
        stream << toilet;
        stream << timeToConsume;
        stream << intensity;
        stream << nauseaTolerance;
        stream << timeInQueue;
        stream << cashInPocket;
        stream << cashSpent;
        stream << photo1RideRef;
        stream << photo2RideRef;
        stream << photo3RideRef;
        stream << photo4RideRef;
        stream << rejoinQueueTimeout;
        stream << previousRide;
        stream << previousRideTimeOut;
        stream << thoughts;
        stream << litterCount;
        stream << disgustingCount;
        stream << amountOfFood;
        stream << amountOfDrinks;
        stream << amountOfSouvenirs;
        stream << vandalismSeen;
        stream << voucherType;
        stream << voucherRideId;
        stream << surroundingsThoughtTimeout;
        stream << angriness;
        stream << timeLost;
        stream << daysInQueue;
        stream << balloonColour;
        stream << umbrellaColour;
        stream << hatColour;
        stream << favouriteRide;
        stream << favouriteRideRating;
        stream << itemFlags;
    }
} // namespace OpenRCT2
