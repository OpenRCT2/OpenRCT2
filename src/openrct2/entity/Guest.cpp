/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/ShopItem.h"
#include "../ride/Track.h"
#include "../scenario/Scenario.h"
#include "../scripting/HookEngine.h"
#include "../scripting/ScriptEngine.h"
#include "../util/Math.hpp"
#include "../windows/Intent.h"
#include "../world/Climate.h"
#include "../world/Footpath.h"
#include "../world/LargeScenery.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Surface.h"
#include "../world/TileElementsView.h"
#include "Peep.h"
#include "Staff.h"
#include "guest/GuestRideHelper.h"

#include <algorithm>
#include <functional>
#include <iterator>

using namespace OpenRCT2;

// clang-format off
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

static PeepThoughtType peep_assess_surroundings(int16_t centre_x, int16_t centre_y, int16_t centre_z);

template<> bool EntityBase::Is<Guest>() const
{
    return Type == EntityType::Guest;
}

static bool IsValidLocation(const CoordsXYZ& coords)
{
    if (coords.x != LOCATION_NULL)
    {
        if (MapIsLocationValid(coords))
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
                GuestRideHelper(*this).PickRideToGoOn();

                if (GuestHeadingToRideId.IsNull())
                {
                    HappinessTarget = std::max(HappinessTarget - 128, 0);
                    LeavePark();
                    UpdateHunger();
                    loc_68F9F3();
                    loc_68FA89();
                    return;
                }
            }
        }

        if ((scenario_rand() & 0xFFFF) <= ((HasItem(ShopItem::Map)) ? 8192u : 2184u))
        {
            GuestRideHelper(*this).PickRideToGoOn();
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
                            GuestRideHelper(*this).HeadForNearestRideWithFlags(false, RIDE_TYPE_FLAG_SELLS_FOOD);
                            break;
                        case PeepThoughtType::Thirsty:
                            GuestRideHelper(*this).HeadForNearestRideWithFlags(false, RIDE_TYPE_FLAG_SELLS_DRINKS);
                            break;
                        case PeepThoughtType::Toilet:
                            GuestRideHelper(*this).HeadForNearestRideWithFlags(false, RIDE_TYPE_FLAG_IS_TOILET);
                            break;
                        case PeepThoughtType::RunningOut:
                            GuestRideHelper(*this).HeadForNearestRideWithFlags(false, RIDE_TYPE_FLAG_IS_CASH_MACHINE);
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
                    GuestRideHelper(*this).HeadForNearestRideWithFlags(true, RIDE_TYPE_FLAG_IS_FIRST_AID);
                }
                InsertNewThought(thought_type);
            }
        }

        switch (State)
        {
            case PeepState::Walking:
            case PeepState::LeavingPark:
            case PeepState::EnteringPark:
                DecideWhetherToLeavePark();
                UpdateHunger();
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
                UpdateHunger();
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
                UpdateHunger();
                break;
            case PeepState::EnteringRide:
                if (SubState == 17 || SubState == 15)
                {
                    DecideWhetherToLeavePark();
                }
                UpdateHunger();
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
    rct_window* w = window_find_by_number(WindowClass::Peep, sprite_index);

    if (w != nullptr)
    {
        window_event_invalidate_call(w);
    }

    window_invalidate_by_number(WindowClass::Peep, sprite_index);
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
bool Guest::DecideAndBuyItem(Ride& ride, ShopItem shopItem, money32 price)
{
    money32 itemValue;

    bool hasVoucher = false;

    bool isRainingAndUmbrella = shopItem == ShopItem::Umbrella && ClimateIsRaining();

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
        && ClimateIsRaining())
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
                    InsertNewThought(GetShopItemDescriptor(shopItem).TooMuchThought, ride.id);
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
                    InsertNewThought(GetShopItemDescriptor(shopItem).GoodValueThought, ride.id);
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
        ride.UpdateSatisfaction(satisfaction);
    }

    // The peep has now decided to buy the item (or, specifically, has not been
    // dissuaded so far).
    GiveItem(shopItem);
    const auto hasRandomShopColour = ride.HasLifecycleFlag(RIDE_LIFECYCLE_RANDOM_SHOP_COLOURS);

    if (shopItem == ShopItem::TShirt)
        TshirtColour = hasRandomShopColour ? scenario_rand_max(COLOUR_COUNT - 1) : ride.track_colour[0].main;

    if (shopItem == ShopItem::Hat)
        HatColour = hasRandomShopColour ? scenario_rand_max(COLOUR_COUNT - 1) : ride.track_colour[0].main;

    if (shopItem == ShopItem::Balloon)
        BalloonColour = hasRandomShopColour ? scenario_rand_max(COLOUR_COUNT - 1) : ride.track_colour[0].main;

    if (shopItem == ShopItem::Umbrella)
        UmbrellaColour = hasRandomShopColour ? scenario_rand_max(COLOUR_COUNT - 1) : ride.track_colour[0].main;

    if (shopItem == ShopItem::Map)
        ResetPathfindGoal();

    uint16_t consumptionTime = GetShopItemDescriptor(shopItem).ConsumptionTime;
    TimeToConsume = std::min((TimeToConsume + consumptionTime), 255);

    if (shopItem == ShopItem::Photo)
        Photo1RideRef = ride.id;

    if (shopItem == ShopItem::Photo2)
        Photo2RideRef = ride.id;

    if (shopItem == ShopItem::Photo3)
        Photo3RideRef = ride.id;

    if (shopItem == ShopItem::Photo4)
        Photo4RideRef = ride.id;

    WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
    UpdateSpriteType();
    if (PeepFlags & PEEP_FLAGS_TRACKING)
    {
        auto ft = Formatter();
        FormatNameTo(ft);
        ft.Add<StringId>(GetShopItemDescriptor(shopItem).Naming.Indefinite);
        if (gConfigNotifications.GuestBoughtItem)
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
    ride.total_profit += (price - GetShopItemDescriptor(shopItem).Cost);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
    ride.cur_num_customers++;
    ride.total_customers++;
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;

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

    window_invalidate_by_number(WindowClass::Peep, sprite_index);

    finance_payment(-amount, expenditure);

    if (gConfigGeneral.ShowGuestPurchases && !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
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

void Guest::SetParkEntryTime(int32_t entryTime)
{
    ParkEntryTime = entryTime;
}

int32_t Guest::GetParkEntryTime() const
{
    return ParkEntryTime;
}

void Guest::ReadMap()
{
    if (IsActionInterruptable() && !IsOnLevelCrossing())
    {
        Action = PeepActionType::ReadMap;
        ActionFrame = 0;
        ActionSpriteImageOffset = 0;
        UpdateCurrentActionSpriteType();
    }
}

/**
 *
 *  rct2: 0x0069BC9A
 */
static PeepThoughtType peep_assess_surroundings(int16_t centre_x, int16_t centre_y, int16_t centre_z)
{
    if ((TileElementHeight({ centre_x, centre_y })) > centre_z)
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
                        auto* ride = get_ride(tileElement->AsTrack()->GetRideIndex());
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
void Guest::UpdateHunger()
{
    if (Hunger >= 3)
    {
        Hunger -= 2;

        EnergyTarget = std::min(EnergyTarget + 2, PEEP_MAX_ENERGY_TARGET);
        Toilet = std::min(Toilet + 1, 255);
    }
}

/**
 * Main purpose is to decide when peeps leave the park due to
 * low happiness, low energy and (if appropriate) low money.
 *
 *  rct2: 0x0068F8CD
 */
void Guest::DecideWhetherToLeavePark()
{
    if (EnergyTarget >= 33)
    {
        EnergyTarget -= 2;
    }

    if (gClimateCurrent.Temperature >= 21 && Thirst >= 5)
    {
        Thirst--;
    }

    if (OutsideOfPark)
    {
        return;
    }

    /* Peeps that are happy enough, have enough energy and
     * (if appropriate) have enough money will always stay
     * in the park. */
    if (!(PeepFlags & PEEP_FLAGS_LEAVING_PARK))
    {
        if (gParkFlags & PARK_FLAGS_NO_MONEY)
        {
            if (Energy >= 70 && Happiness >= 60)
            {
                return;
            }
        }
        else
        {
            if (Energy >= 55 && Happiness >= 45 && CashInPocket >= 5.00_GBP)
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
    LeavePark();
}

/**
 *
 *  rct2: 0x0068F93E
 */
void Guest::LeavePark()
{
    GuestHeadingToRideId = RideId::GetNull();
    if (PeepFlags & PEEP_FLAGS_LEAVING_PARK)
    {
        if (GuestIsLostCountdown < 60)
        {
            return;
        }
    }
    else
    {
        GuestIsLostCountdown = 254;
        PeepFlags |= PEEP_FLAGS_LEAVING_PARK;
        PeepFlags &= ~PEEP_FLAGS_PARK_ENTRANCE_CHOSEN;
    }

    InsertNewThought(PeepThoughtType::GoHome);

    rct_window* w = window_find_by_number(WindowClass::Peep, sprite_index);
    if (w != nullptr)
        window_event_invalidate_call(w);
    window_invalidate_by_number(WindowClass::Peep, sprite_index);
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
            window_invalidate_by_number(WindowClass::Peep, sprite_index);
        }
        sprite_direction ^= 0x10;

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

                item_bought = DecideAndBuyItem(*ride, ride_type->shop_item[1], price);
                if (item_bought)
                {
                    ride->no_secondary_items_sold++;
                }
            }

            if (!item_bought && ride_type->shop_item[0] != ShopItem::None)
            {
                money16 price = ride->price[0];

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

void Guest::UpdateGuest()
{
    switch (State)
    {
        case PeepState::QueuingFront:
            GuestRideHelper(*this).UpdateRide();
            break;
        case PeepState::LeavingRide:
            GuestRideHelper(*this).UpdateRide();
            break;
        case PeepState::Walking:
            UpdateWalking();
            break;
        case PeepState::Queuing:
            GuestRideHelper(*this).UpdateQueuing();
            break;
        case PeepState::EnteringRide:
            GuestRideHelper(*this).UpdateRide();
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

static void peep_update_walking_break_scenery(Guest* peep);

/**
 *
 *  rct2: 0x0069030A
 */
void Guest::UpdateWalking()
{
    if (!CheckForPath())
        return;

    if (!IsOnLevelCrossing())
    {
        if (PeepFlags & PEEP_FLAGS_WAVING && IsActionInterruptable() && (0xFFFF & scenario_rand()) < 936)
        {
            Action = PeepActionType::Wave2;
            ActionFrame = 0;
            ActionSpriteImageOffset = 0;

            UpdateCurrentActionSpriteType();
        }

        if (PeepFlags & PEEP_FLAGS_PHOTO && IsActionInterruptable() && (0xFFFF & scenario_rand()) < 936)
        {
            Action = PeepActionType::TakePhoto;
            ActionFrame = 0;
            ActionSpriteImageOffset = 0;

            UpdateCurrentActionSpriteType();
        }

        if (PeepFlags & PEEP_FLAGS_PAINTING && IsActionInterruptable() && (0xFFFF & scenario_rand()) < 936)
        {
            Action = PeepActionType::DrawPicture;
            ActionFrame = 0;
            ActionSpriteImageOffset = 0;

            UpdateCurrentActionSpriteType();
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
    if (IsOnLevelCrossing() || !GuestRideHelper(*this).FindRideToLookAt(chosen_edge, &ride_to_view, &ride_seat_to_view))
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
    NextActionSpriteType = PeepActionSpriteType::WatchRide;
    SwitchNextActionSpriteType();

    if (HasFoodOrDrink())
    {
        if ((scenario_rand() & 0xFFFF) <= 1310)
        {
            Action = PeepActionType::EatFood;
            ActionFrame = 0;
            ActionSpriteImageOffset = 0;
        }

        UpdateCurrentActionSpriteType();

        return;
    }

    if ((scenario_rand() & 0xFFFF) <= 64)
    {
        Action = PeepActionType::Wave2;
        ActionFrame = 0;
        ActionSpriteImageOffset = 0;
    }

    UpdateCurrentActionSpriteType();
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
    ContextBroadcastIntent(&intent);
    Var37 = 1;

    window_invalidate_by_class(WindowClass::GuestList);
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
                if (!(emptyContainers & (1uLL << curContainer)))
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

    MapInvalidateTileZoom1({ peep->NextLoc, tileElement->GetBaseZ(), tileElement->GetBaseZ() + 32 });

    peep->Angriness = 16;
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

    if (ClimateIsRaining() && (HasItem(ShopItem::Umbrella)) && x != LOCATION_NULL)
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
    ft.Add<StringId>(PeepThoughts[EnumValue(thought->type)]);

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
            ft.Add<StringId>(STR_NONE);
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

bool IsThoughtShopItemRelated(const PeepThoughtType type)
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
