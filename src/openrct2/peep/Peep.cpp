/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Peep.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Game.h"
#include "../Input.h"
#include "../OpenRCT2.h"
#include "../actions/GameAction.h"
#include "../audio/AudioMixer.h"
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../core/Guard.hpp"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../network/network.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/ShopItem.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../scenario/Scenario.h"
#include "../sprites.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Climate.h"
#include "../world/Entrance.h"
#include "../world/Footpath.h"
#include "../world/LargeScenery.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/SmallScenery.h"
#include "../world/Sprite.h"
#include "../world/Surface.h"
#include "GuestPathfinding.h"
#include "Staff.h"

#include <algorithm>
#include <iterator>
#include <limits>

uint8_t gGuestChangeModifier;
uint32_t gNumGuestsInPark;
uint32_t gNumGuestsInParkLastWeek;
uint32_t gNumGuestsHeadingForPark;

money16 gGuestInitialCash;
uint8_t gGuestInitialHappiness;
uint8_t gGuestInitialHunger;
uint8_t gGuestInitialThirst;

uint32_t gNextGuestNumber;

uint8_t gPeepWarningThrottle[16];

static uint8_t _unk_F1AEF0;
static TileElement* _peepRideEntranceExitElement;

static void* _crowdSoundChannel = nullptr;

static void peep_128_tick_update(Peep* peep, int32_t index);
static void peep_release_balloon(Guest* peep, int16_t spawn_height);
// clang-format off

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
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Wow, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_SHOP_ITEM_SINGULAR },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_SHOP_ITEM_INDEFINITE },
    { PeepActionType::ShakeHead, PEEP_THOUGHT_ACTION_FLAG_SHOP_ITEM_INDEFINITE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Wave, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Joy, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::CheckTime, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Wave, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Wave, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::Disgust, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::BeingWatched, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::ShakeHead, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::Joy, PEEP_THOUGHT_ACTION_NO_FLAGS },
    { PeepActionType::None2, PEEP_THOUGHT_ACTION_FLAG_RIDE },
};

static PeepActionSpriteType PeepSpecialSpriteToSpriteTypeMap[] = {
    PeepActionSpriteType::None,
    PeepActionSpriteType::HoldMat,
    PeepActionSpriteType::StaffMower
};

static PeepActionSpriteType PeepActionToSpriteTypeMap[] = {
    PeepActionSpriteType::CheckTime,
    PeepActionSpriteType::EatFood,
    PeepActionSpriteType::ShakeHead,
    PeepActionSpriteType::EmptyPockets,
    PeepActionSpriteType::SittingEatFood,
    PeepActionSpriteType::SittingLookAroundLeft,
    PeepActionSpriteType::SittingLookAroundRight,
    PeepActionSpriteType::Wow,
    PeepActionSpriteType::ThrowUp,
    PeepActionSpriteType::Jump,
    PeepActionSpriteType::StaffSweep,
    PeepActionSpriteType::Drowning,
    PeepActionSpriteType::StaffAnswerCall,
    PeepActionSpriteType::StaffAnswerCall2,
    PeepActionSpriteType::StaffCheckboard,
    PeepActionSpriteType::StaffFix,
    PeepActionSpriteType::StaffFix2,
    PeepActionSpriteType::StaffFixGround,
    PeepActionSpriteType::StaffFix3,
    PeepActionSpriteType::StaffWatering,
    PeepActionSpriteType::Joy,
    PeepActionSpriteType::ReadMap,
    PeepActionSpriteType::Wave,
    PeepActionSpriteType::StaffEmptyBin,
    PeepActionSpriteType::Wave2,
    PeepActionSpriteType::TakePhoto,
    PeepActionSpriteType::Clap,
    PeepActionSpriteType::Disgust,
    PeepActionSpriteType::DrawPicture,
    PeepActionSpriteType::BeingWatched,
    PeepActionSpriteType::WithdrawMoney
};

const bool gSpriteTypeToSlowWalkMap[] = {
    false, false, false, false, false, false, false, false,
    false, false, false, true,  false, false, true,  true,
    true,  true,  true,  false, true,  false, true,  true,
    true,  false, false, true,  true,  false, false, true,
    true,  true,  true,  true,  true,  true,  false, true,
    false, true,  true,  true,  true,  true,  true,  true,
};

// clang-format on

template<> bool SpriteBase::Is<Peep>() const
{
    return sprite_identifier == SpriteIdentifier::Peep;
}

Guest* Peep::AsGuest()
{
    return AssignedPeepType == PeepType::Guest ? static_cast<Guest*>(this) : nullptr;
}

Staff* Peep::AsStaff()
{
    return AssignedPeepType == PeepType::Staff ? static_cast<Staff*>(this) : nullptr;
}

uint8_t Peep::GetNextDirection() const
{
    return NextFlags & PEEP_NEXT_FLAG_DIRECTION_MASK;
}

bool Peep::GetNextIsSloped() const
{
    return NextFlags & PEEP_NEXT_FLAG_IS_SLOPED;
}

bool Peep::GetNextIsSurface() const
{
    return NextFlags & PEEP_NEXT_FLAG_IS_SURFACE;
}

void Peep::SetNextFlags(uint8_t next_direction, bool is_sloped, bool is_surface)
{
    NextFlags = next_direction & PEEP_NEXT_FLAG_DIRECTION_MASK;
    NextFlags |= is_sloped ? PEEP_NEXT_FLAG_IS_SLOPED : 0;
    NextFlags |= is_surface ? PEEP_NEXT_FLAG_IS_SURFACE : 0;
}

bool Peep::CanBePickedUp() const
{
    switch (State)
    {
        case PeepState::One:
        case PeepState::QueuingFront:
        case PeepState::OnRide:
        case PeepState::EnteringRide:
        case PeepState::LeavingRide:
        case PeepState::EnteringPark:
        case PeepState::LeavingPark:
        case PeepState::Fixing:
        case PeepState::Buying:
        case PeepState::Inspecting:
            return false;
        case PeepState::Falling:
        case PeepState::Walking:
        case PeepState::Queuing:
        case PeepState::Sitting:
        case PeepState::Picked:
        case PeepState::Patrolling:
        case PeepState::Mowing:
        case PeepState::Sweeping:
        case PeepState::Answering:
        case PeepState::Watching:
        case PeepState::EmptyingBin:
        case PeepState::UsingBin:
        case PeepState::Watering:
        case PeepState::HeadingToInspection:
            return true;
    }
    return false;
}

Peep* try_get_guest(uint16_t spriteIndex)
{
    return TryGetEntity<Guest>(spriteIndex);
}

int32_t peep_get_staff_count()
{
    auto list = EntityList<Staff>(EntityListId::Peep);
    auto count = std::distance(list.begin(), list.end());

    return count;
}

/**
 *
 *  rct2: 0x0068F0A9
 */
void peep_update_all()
{
    if (gScreenFlags & SCREEN_FLAGS_EDITOR)
        return;

    int32_t i = 0;
    // Warning this loop can delete peeps
    for (auto peep : EntityList<Guest>(EntityListId::Peep))
    {
        if (static_cast<uint32_t>(i & 0x7F) != (gCurrentTicks & 0x7F))
        {
            peep->Update();
        }
        else
        {
            peep_128_tick_update(peep, i);
            // 128 tick can delete so double check its not deleted
            if (peep->sprite_identifier == SpriteIdentifier::Peep)
            {
                peep->Update();
            }
        }

        i++;
    }

    for (auto staff : EntityList<Staff>(EntityListId::Peep))
    {
        if (static_cast<uint32_t>(i & 0x7F) != (gCurrentTicks & 0x7F))
        {
            staff->Update();
        }
        else
        {
            peep_128_tick_update(staff, i);
            // 128 tick can delete so double check its not deleted
            if (staff->sprite_identifier == SpriteIdentifier::Peep)
            {
                staff->Update();
            }
        }

        i++;
    }
}

/**
 *
 *  rct2: 0x0068F41A
 *  Called every 128 ticks
 */
static void peep_128_tick_update(Peep* peep, int32_t index)
{
    auto guest = peep->AsGuest();
    if (guest != nullptr)
    {
        guest->Tick128UpdateGuest(index);
    }
    else
    {
        auto staff = peep->AsStaff();
        if (staff != nullptr)
        {
            staff->Tick128UpdateStaff();
        }
    }
}

/*
 * rct2: 0x68F3AE
 * Set peep state to falling if path below has gone missing, return true if current path is valid, false if peep starts falling.
 */
bool Peep::CheckForPath()
{
    PathCheckOptimisation++;
    if ((PathCheckOptimisation & 0xF) != (sprite_index & 0xF))
    {
        // This condition makes the check happen less often
        // As a side effect peeps hover for a short,
        // random time when a path below them has been deleted
        return true;
    }

    TileElement* tile_element = map_get_first_element_at(NextLoc);

    uint8_t map_type = TILE_ELEMENT_TYPE_PATH;
    if (GetNextIsSurface())
    {
        map_type = TILE_ELEMENT_TYPE_SURFACE;
    }

    do
    {
        if (tile_element == nullptr)
            break;
        if (tile_element->GetType() == map_type)
        {
            if (NextLoc.z == tile_element->GetBaseZ())
            {
                // Found a suitable path or surface
                return true;
            }
        }
    } while (!(tile_element++)->IsLastForTile());

    // Found no suitable path
    SetState(PeepState::Falling);
    return false;
}

PeepActionSpriteType Peep::GetActionSpriteType()
{
    if (Action >= PeepActionType::None1)
    { // PeepActionType::None1 or PeepActionType::None2
        return PeepSpecialSpriteToSpriteTypeMap[SpecialSprite];
    }
    else if (EnumValue(Action) < std::size(PeepActionToSpriteTypeMap))
    {
        return PeepActionToSpriteTypeMap[EnumValue(Action)];
    }
    else
    {
        openrct2_assert(
            EnumValue(Action) >= std::size(PeepActionToSpriteTypeMap) && Action < PeepActionType::None1,
            "Invalid peep action %u", EnumValue(Action));
        return PeepActionSpriteType::None;
    }
}

/*
 *  rct2: 0x00693B58
 */
void Peep::UpdateCurrentActionSpriteType()
{
    if (EnumValue(SpriteType) >= std::size(g_peep_animation_entries))
    {
        return;
    }
    PeepActionSpriteType newActionSpriteType = GetActionSpriteType();
    if (ActionSpriteType == newActionSpriteType)
    {
        return;
    }

    Invalidate();
    ActionSpriteType = newActionSpriteType;

    const rct_sprite_bounds* spriteBounds = &GetSpriteBounds(SpriteType, ActionSpriteType);
    sprite_width = spriteBounds->sprite_width;
    sprite_height_negative = spriteBounds->sprite_height_negative;
    sprite_height_positive = spriteBounds->sprite_height_positive;

    Invalidate();
}

/* rct2: 0x00693BE5 */
void Peep::SwitchToSpecialSprite(uint8_t special_sprite_id)
{
    if (special_sprite_id == SpecialSprite)
        return;

    SpecialSprite = special_sprite_id;

    // If NONE_1 or NONE_2
    if (Action >= PeepActionType::None1)
    {
        ActionSpriteImageOffset = 0;
    }
    UpdateCurrentActionSpriteType();
}

void Peep::StateReset()
{
    SetState(PeepState::One);
    SwitchToSpecialSprite(0);
}

/** rct2: 0x00981D7C, 0x00981D7E */
static constexpr const CoordsXY word_981D7C[4] = { { -2, 0 }, { 0, 2 }, { 2, 0 }, { 0, -2 } };

std::optional<CoordsXY> Peep::UpdateAction()
{
    int16_t xy_distance;
    return UpdateAction(xy_distance);
}

/**
 *
 *  rct2: 0x6939EB
 * Also used to move peeps to the correct position to
 * start an action. Returns true if the correct destination
 * has not yet been reached. xy_distance is how close the
 * peep is to the target.
 */
std::optional<CoordsXY> Peep::UpdateAction(int16_t& xy_distance)
{
    _unk_F1AEF0 = ActionSpriteImageOffset;
    if (Action == PeepActionType::None1)
    {
        Action = PeepActionType::None2;
    }

    CoordsXY diffrenceLoc = { x - DestinationX, y - DestinationY };

    int32_t x_delta = abs(diffrenceLoc.x);
    int32_t y_delta = abs(diffrenceLoc.y);

    xy_distance = x_delta + y_delta;

    if (Action == PeepActionType::None1 || Action == PeepActionType::None2)
    {
        if (xy_distance <= DestinationTolerance)
        {
            return std::nullopt;
        }
        int32_t nextDirection = 0;
        if (x_delta < y_delta)
        {
            nextDirection = 8;
            if (diffrenceLoc.y >= 0)
            {
                nextDirection = 24;
            }
        }
        else
        {
            nextDirection = 16;
            if (diffrenceLoc.x >= 0)
            {
                nextDirection = 0;
            }
        }
        sprite_direction = nextDirection;
        CoordsXY loc = { x, y };
        loc += word_981D7C[nextDirection / 8];
        WalkingFrameNum++;
        const rct_peep_animation* peepAnimation = &GetPeepAnimation(SpriteType);
        const uint8_t* imageOffset = peepAnimation[EnumValue(ActionSpriteType)].frame_offsets;
        if (WalkingFrameNum >= peepAnimation[EnumValue(ActionSpriteType)].num_frames)
        {
            WalkingFrameNum = 0;
        }
        ActionSpriteImageOffset = imageOffset[WalkingFrameNum];
        return loc;
    }

    const rct_peep_animation* peepAnimation = &GetPeepAnimation(SpriteType);
    ActionFrame++;

    // If last frame of action
    if (ActionFrame >= peepAnimation[EnumValue(ActionSpriteType)].num_frames)
    {
        ActionSpriteImageOffset = 0;
        Action = PeepActionType::None2;
        UpdateCurrentActionSpriteType();
        return { { x, y } };
    }
    ActionSpriteImageOffset = peepAnimation[EnumValue(ActionSpriteType)].frame_offsets[ActionFrame];

    // If not throwing up and not at the frame where sick appears.
    if (Action != PeepActionType::ThrowUp || ActionFrame != 15)
    {
        return { { x, y } };
    }

    // We are throwing up
    Hunger /= 2;
    NauseaTarget /= 2;

    if (Nausea < 30)
        Nausea = 0;
    else
        Nausea -= 30;

    WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_2;

    // Create sick at location
    litter_create({ x, y, z, sprite_direction }, (sprite_index & 1) ? LITTER_TYPE_SICK_ALT : LITTER_TYPE_SICK);

    static constexpr OpenRCT2::Audio::SoundId coughs[4] = { OpenRCT2::Audio::SoundId::Cough1, OpenRCT2::Audio::SoundId::Cough2,
                                                            OpenRCT2::Audio::SoundId::Cough3,
                                                            OpenRCT2::Audio::SoundId::Cough4 };
    auto soundId = coughs[scenario_rand() & 3];
    OpenRCT2::Audio::Play3D(soundId, { x, y, z });

    return { { x, y } };
}

/**
 *  rct2: 0x0069A409
 * Decreases rider count if on/entering a ride.
 */
void peep_decrement_num_riders(Peep* peep)
{
    if (peep->State == PeepState::OnRide || peep->State == PeepState::EnteringRide)
    {
        auto ride = get_ride(peep->CurrentRide);
        if (ride != nullptr)
        {
            ride->num_riders = std::max(0, ride->num_riders - 1);
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
        }
    }
}

/**
 * Call after changing a peeps state to insure that all relevant windows update.
 * Note also increase ride count if on/entering a ride.
 *  rct2: 0x0069A42F
 */
void peep_window_state_update(Peep* peep)
{
    rct_window* w = window_find_by_number(WC_PEEP, peep->sprite_index);
    if (w != nullptr)
        window_event_invalidate_call(w);

    if (peep->AssignedPeepType == PeepType::Guest)
    {
        if (peep->State == PeepState::OnRide || peep->State == PeepState::EnteringRide)
        {
            auto ride = get_ride(peep->CurrentRide);
            if (ride != nullptr)
            {
                ride->num_riders++;
                ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
            }
        }

        window_invalidate_by_number(WC_PEEP, peep->sprite_index);
        window_invalidate_by_class(WC_GUEST_LIST);
    }
    else
    {
        window_invalidate_by_number(WC_PEEP, peep->sprite_index);
        window_invalidate_by_class(WC_STAFF_LIST);
    }
}

void Peep::Pickup()
{
    auto guest = AsGuest();
    if (guest != nullptr)
    {
        guest->RemoveFromRide();
    }
    MoveTo({ LOCATION_NULL, y, z });
    SetState(PeepState::Picked);
    SubState = 0;
}

void Peep::PickupAbort(int32_t old_x)
{
    if (State != PeepState::Picked)
        return;

    MoveTo({ old_x, y, z + 8 });

    if (x != LOCATION_NULL)
    {
        SetState(PeepState::Falling);
        Action = PeepActionType::None2;
        SpecialSprite = 0;
        ActionSpriteImageOffset = 0;
        ActionSpriteType = PeepActionSpriteType::None;
        PathCheckOptimisation = 0;
    }

    gPickupPeepImage = UINT32_MAX;
}

// Returns GameActions::Status::OK when a peep can be dropped at the given location. When apply is set to true the peep gets
// dropped.
std::unique_ptr<GameActions::Result> Peep::Place(const TileCoordsXYZ& location, bool apply)
{
    auto* pathElement = map_get_path_element_at(location);
    TileElement* tileElement = reinterpret_cast<TileElement*>(pathElement);
    if (!pathElement)
    {
        tileElement = reinterpret_cast<TileElement*>(map_get_surface_element_at(location.ToCoordsXYZ()));
    }

    if (!tileElement)
        return std::make_unique<GameActions::Result>(GameActions::Status::InvalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE);

    // Set the coordinate of destination to be exactly
    // in the middle of a tile.
    CoordsXYZ destination = { location.ToCoordsXY().ToTileCentre(), tileElement->GetBaseZ() + 16 };

    if (!map_is_location_owned(destination))
    {
        return std::make_unique<GameActions::Result>(GameActions::Status::NotOwned, STR_ERR_CANT_PLACE_PERSON_HERE);
    }

    if (auto res = MapCanConstructAt({ destination, destination.z, destination.z + (1 * 8) }, { 0b1111, 0 });
        res->Error != GameActions::Status::Ok)
    {
        if (res->ErrorMessage.GetStringId() != STR_RAISE_OR_LOWER_LAND_FIRST)
        {
            if (res->ErrorMessage.GetStringId() != STR_FOOTPATH_IN_THE_WAY)
            {
                return std::make_unique<GameActions::Result>(
                    GameActions::Status::NoClearance, STR_ERR_CANT_PLACE_PERSON_HERE, res->ErrorMessage.GetStringId(),
                    res->ErrorMessageArgs.data());
            }
        }
    }

    if (apply)
    {
        MoveTo(destination);
        SetState(PeepState::Falling);
        Action = PeepActionType::None2;
        SpecialSprite = 0;
        ActionSpriteImageOffset = 0;
        ActionSpriteType = PeepActionSpriteType::None;
        PathCheckOptimisation = 0;
        EntityTweener::Get().Reset();

        if (AssignedPeepType == PeepType::Guest)
        {
            ActionSpriteType = PeepActionSpriteType::Invalid;
            HappinessTarget = std::max(HappinessTarget - 10, 0);
            UpdateCurrentActionSpriteType();
        }
    }

    return std::make_unique<GameActions::Result>();
}

/**
 *
 *  rct2: 0x0069A535
 */
void peep_sprite_remove(Peep* peep)
{
    auto guest = peep->AsGuest();
    if (guest != nullptr)
    {
        guest->RemoveFromRide();
    }
    peep->Invalidate();

    window_close_by_number(WC_PEEP, peep->sprite_index);

    window_close_by_number(WC_FIRE_PROMPT, EnumValue(peep->sprite_identifier));

    // Needed for invalidations after sprite removal
    bool wasGuest = peep->AssignedPeepType == PeepType::Guest;
    if (peep->AssignedPeepType == PeepType::Guest)
    {
        News::DisableNewsItems(News::ItemType::PeepOnRide, peep->sprite_index);
    }
    else
    {
        gStaffModes[peep->StaffId] = StaffMode::None;
        peep->AssignedPeepType = PeepType::Invalid;
        staff_update_greyed_patrol_areas();
        peep->AssignedPeepType = PeepType::Staff;

        News::DisableNewsItems(News::ItemType::Peep, peep->sprite_index);
    }
    sprite_remove(peep);

    auto intent = Intent(wasGuest ? INTENT_ACTION_REFRESH_GUEST_LIST : INTENT_ACTION_REFRESH_STAFF_LIST);
    context_broadcast_intent(&intent);
}

/**
 * New function removes peep from park existence. Works with staff.
 */
void Peep::Remove()
{
    if (AssignedPeepType == PeepType::Guest)
    {
        if (!OutsideOfPark)
        {
            decrement_guests_in_park();
            auto intent = Intent(INTENT_ACTION_UPDATE_GUEST_COUNT);
            context_broadcast_intent(&intent);
        }
        if (State == PeepState::EnteringPark)
        {
            decrement_guests_heading_for_park();
        }
    }
    peep_sprite_remove(this);
}

/**
 * Falling and its subset drowning
 *  rct2: 0x690028
 */
void Peep::UpdateFalling()
{
    if (Action == PeepActionType::Drowning)
    {
        // Check to see if we are ready to drown.
        UpdateAction();
        Invalidate();
        if (Action == PeepActionType::Drowning)
            return;

        if (gConfigNotifications.guest_died)
        {
            auto ft = Formatter();
            FormatNameTo(ft);
            News::AddItemToQueue(News::ItemType::Blank, STR_NEWS_ITEM_GUEST_DROWNED, x | (y << 16), ft);
        }

        gParkRatingCasualtyPenalty = std::min(gParkRatingCasualtyPenalty + 25, 1000);
        Remove();
        return;
    }

    // If not drowning then falling. Note: peeps 'fall' after leaving a ride/enter the park.
    TileElement* tile_element = map_get_first_element_at({ x, y });
    TileElement* saved_map = nullptr;
    int32_t saved_height = 0;

    if (tile_element != nullptr)
    {
        do
        {
            // If a path check if we are on it
            if (tile_element->GetType() == TILE_ELEMENT_TYPE_PATH)
            {
                int32_t height = map_height_from_slope(
                                     { x, y }, tile_element->AsPath()->GetSlopeDirection(), tile_element->AsPath()->IsSloped())
                    + tile_element->GetBaseZ();

                if (height < z - 1 || height > z + 4)
                    continue;

                saved_height = height;
                saved_map = tile_element;
                break;
            } // If a surface get the height and see if we are on it
            else if (tile_element->GetType() == TILE_ELEMENT_TYPE_SURFACE)
            {
                // If the surface is water check to see if we could be drowning
                if (tile_element->AsSurface()->GetWaterHeight() > 0)
                {
                    int32_t height = tile_element->AsSurface()->GetWaterHeight();

                    if (height - 4 >= z && height < z + 20)
                    {
                        // Looks like we are drowning!
                        MoveTo({ x, y, height });

                        auto guest = AsGuest();
                        if (guest != nullptr)
                        {
                            // Drop balloon if held
                            peep_release_balloon(guest, height);
                        }

                        InsertNewThought(PeepThoughtType::Drowning, PEEP_THOUGHT_ITEM_NONE);

                        Action = PeepActionType::Drowning;
                        ActionFrame = 0;
                        ActionSpriteImageOffset = 0;

                        UpdateCurrentActionSpriteType();
                        peep_window_state_update(this);
                        return;
                    }
                }
                int32_t map_height = tile_element_height({ x, y });
                if (map_height < z || map_height - 4 > z)
                    continue;
                saved_height = map_height;
                saved_map = tile_element;
            } // If not a path or surface go see next element
            else
                continue;
        } while (!(tile_element++)->IsLastForTile());
    }

    // This will be null if peep is falling
    if (saved_map == nullptr)
    {
        if (z <= 1)
        {
            // Remove peep if it has gone to the void
            Remove();
            return;
        }
        MoveTo({ x, y, z - 2 });
        return;
    }

    MoveTo({ x, y, saved_height });

    NextLoc = { CoordsXY{ x, y }.ToTileStart(), saved_map->GetBaseZ() };

    if (saved_map->GetType() != TILE_ELEMENT_TYPE_PATH)
    {
        SetNextFlags(0, false, true);
    }
    else
    {
        SetNextFlags(saved_map->AsPath()->GetSlopeDirection(), saved_map->AsPath()->IsSloped(), false);
    }
    SetState(PeepState::One);
}

/**
 *
 *  rct2: 0x6902A2
 */
void Peep::Update1()
{
    if (!CheckForPath())
        return;

    if (AssignedPeepType == PeepType::Guest)
    {
        SetState(PeepState::Walking);
    }
    else
    {
        SetState(PeepState::Patrolling);
    }

    DestinationX = x;
    DestinationY = y;
    DestinationTolerance = 10;
    PeepDirection = sprite_direction >> 3;
}

void Peep::SetState(PeepState new_state)
{
    peep_decrement_num_riders(this);
    State = new_state;
    peep_window_state_update(this);
}

/**
 *
 *  rct2: 0x690009
 */
void Peep::UpdatePicked()
{
    if (gCurrentTicks & 0x1F)
        return;
    SubState++;
    if (SubState == 13)
    {
        InsertNewThought(PeepThoughtType::Help, PEEP_THOUGHT_ITEM_NONE);
    }
}

/* From peep_update */
static void peep_update_thoughts(Peep* peep)
{
    // Thoughts must always have a gap of at least
    // 220 ticks in age between them. In order to
    // allow this when a thought is new it enters
    // a holding zone. Before it becomes fresh.
    int32_t add_fresh = 1;
    int32_t fresh_thought = -1;
    for (int32_t i = 0; i < PEEP_MAX_THOUGHTS; i++)
    {
        if (peep->Thoughts[i].type == PeepThoughtType::None)
            break;

        if (peep->Thoughts[i].freshness == 1)
        {
            add_fresh = 0;
            // If thought is fresh we wait 220 ticks
            // before allowing a new thought to become fresh.
            if (++peep->Thoughts[i].fresh_timeout >= 220)
            {
                peep->Thoughts[i].fresh_timeout = 0;
                // Thought is no longer fresh
                peep->Thoughts[i].freshness++;
                add_fresh = 1;
            }
        }
        else if (peep->Thoughts[i].freshness > 1)
        {
            if (++peep->Thoughts[i].fresh_timeout == 0)
            {
                // When thought is older than ~6900 ticks remove it
                if (++peep->Thoughts[i].freshness >= 28)
                {
                    peep->WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_THOUGHTS;

                    // Clear top thought, push others up
                    if (i < PEEP_MAX_THOUGHTS - 2)
                    {
                        memmove(
                            &peep->Thoughts[i], &peep->Thoughts[i + 1], sizeof(rct_peep_thought) * (PEEP_MAX_THOUGHTS - i - 1));
                    }
                    peep->Thoughts[PEEP_MAX_THOUGHTS - 1].type = PeepThoughtType::None;
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
        peep->Thoughts[fresh_thought].freshness = 1;
        peep->WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_THOUGHTS;
    }
}

/**
 *
 *  rct2: 0x0068FC1E
 */
void Peep::Update()
{
    if (AssignedPeepType == PeepType::Guest)
    {
        if (PreviousRide != RIDE_ID_NULL)
            if (++PreviousRideTimeOut >= 720)
                PreviousRide = RIDE_ID_NULL;

        peep_update_thoughts(this);
    }

    // Walking speed logic
    uint32_t stepsToTake = Energy;
    if (stepsToTake < 95 && State == PeepState::Queuing)
        stepsToTake = 95;
    if ((PeepFlags & PEEP_FLAGS_SLOW_WALK) && State != PeepState::Queuing)
        stepsToTake /= 2;
    if (Action == PeepActionType::None2 && (GetNextIsSloped()))
    {
        stepsToTake /= 2;
        if (State == PeepState::Queuing)
            stepsToTake += stepsToTake / 2;
    }

    uint32_t carryCheck = StepProgress + stepsToTake;
    StepProgress = carryCheck;
    if (carryCheck <= 255)
    {
        auto guest = AsGuest();
        if (guest != nullptr)
        {
            guest->UpdateEasterEggInteractions();
        }
    }
    else
    {
        // loc_68FD2F
        switch (State)
        {
            case PeepState::Falling:
                UpdateFalling();
                break;
            case PeepState::One:
                Update1();
                break;
            case PeepState::OnRide:
                // No action
                break;
            case PeepState::Picked:
                UpdatePicked();
                break;
            default:
            {
                auto guest = AsGuest();
                if (guest != nullptr)
                {
                    guest->UpdateGuest();
                }
                else
                {
                    auto staff = AsStaff();
                    if (staff != nullptr)
                    {
                        staff->UpdateStaff(stepsToTake);
                    }
                    else
                    {
                        assert(false);
                    }
                }
                break;
            }
        }
    }
}

/**
 *
 *  rct2: 0x0069BF41
 */
void peep_problem_warnings_update()
{
    Ride* ride;
    uint32_t hunger_counter = 0, lost_counter = 0, noexit_counter = 0, thirst_counter = 0, litter_counter = 0,
             disgust_counter = 0, toilet_counter = 0, vandalism_counter = 0;
    uint8_t* warning_throttle = gPeepWarningThrottle;

    for (auto peep : EntityList<Guest>(EntityListId::Peep))
    {
        if (peep->OutsideOfPark || peep->Thoughts[0].freshness > 5)
            continue;

        switch (peep->Thoughts[0].type)
        {
            case PeepThoughtType::Lost: // 0x10
                lost_counter++;
                break;

            case PeepThoughtType::Hungry: // 0x14
                if (peep->GuestHeadingToRideId == RIDE_ID_NULL)
                {
                    hunger_counter++;
                    break;
                }
                ride = get_ride(peep->GuestHeadingToRideId);
                if (ride != nullptr && !ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
                    hunger_counter++;
                break;

            case PeepThoughtType::Thirsty:
                if (peep->GuestHeadingToRideId == RIDE_ID_NULL)
                {
                    thirst_counter++;
                    break;
                }
                ride = get_ride(peep->GuestHeadingToRideId);
                if (ride != nullptr && !ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_SELLS_DRINKS))
                    thirst_counter++;
                break;

            case PeepThoughtType::Toilet:
                if (peep->GuestHeadingToRideId == RIDE_ID_NULL)
                {
                    toilet_counter++;
                    break;
                }
                ride = get_ride(peep->GuestHeadingToRideId);
                if (ride != nullptr && !ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_TOILET))
                    toilet_counter++;
                break;

            case PeepThoughtType::BadLitter: // 0x1a
                litter_counter++;
                break;
            case PeepThoughtType::CantFindExit: // 0x1b
                noexit_counter++;
                break;
            case PeepThoughtType::PathDisgusting: // 0x1f
                disgust_counter++;
                break;
            case PeepThoughtType::Vandalism: // 0x21
                vandalism_counter++;
                break;
            default:
                break;
        }
    }
    // could maybe be packed into a loop, would lose a lot of clarity though
    if (warning_throttle[0])
        --warning_throttle[0];
    else if (hunger_counter >= PEEP_HUNGER_WARNING_THRESHOLD && hunger_counter >= gNumGuestsInPark / 16)
    {
        warning_throttle[0] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            News::AddItemToQueue(News::ItemType::Peeps, STR_PEEPS_ARE_HUNGRY, 20, {});
        }
    }

    if (warning_throttle[1])
        --warning_throttle[1];
    else if (thirst_counter >= PEEP_THIRST_WARNING_THRESHOLD && thirst_counter >= gNumGuestsInPark / 16)
    {
        warning_throttle[1] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            News::AddItemToQueue(News::ItemType::Peeps, STR_PEEPS_ARE_THIRSTY, 21, {});
        }
    }

    if (warning_throttle[2])
        --warning_throttle[2];
    else if (toilet_counter >= PEEP_TOILET_WARNING_THRESHOLD && toilet_counter >= gNumGuestsInPark / 16)
    {
        warning_throttle[2] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            News::AddItemToQueue(News::ItemType::Peeps, STR_PEEPS_CANT_FIND_TOILET, 22, {});
        }
    }

    if (warning_throttle[3])
        --warning_throttle[3];
    else if (litter_counter >= PEEP_LITTER_WARNING_THRESHOLD && litter_counter >= gNumGuestsInPark / 32)
    {
        warning_throttle[3] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            News::AddItemToQueue(News::ItemType::Peeps, STR_PEEPS_DISLIKE_LITTER, 26, {});
        }
    }

    if (warning_throttle[4])
        --warning_throttle[4];
    else if (disgust_counter >= PEEP_DISGUST_WARNING_THRESHOLD && disgust_counter >= gNumGuestsInPark / 32)
    {
        warning_throttle[4] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            News::AddItemToQueue(News::ItemType::Peeps, STR_PEEPS_DISGUSTED_BY_PATHS, 31, {});
        }
    }

    if (warning_throttle[5])
        --warning_throttle[5];
    else if (vandalism_counter >= PEEP_VANDALISM_WARNING_THRESHOLD && vandalism_counter >= gNumGuestsInPark / 32)
    {
        warning_throttle[5] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            News::AddItemToQueue(News::ItemType::Peeps, STR_PEEPS_DISLIKE_VANDALISM, 33, {});
        }
    }

    if (warning_throttle[6])
        --warning_throttle[6];
    else if (noexit_counter >= PEEP_NOEXIT_WARNING_THRESHOLD)
    {
        warning_throttle[6] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            News::AddItemToQueue(News::ItemType::Peeps, STR_PEEPS_GETTING_LOST_OR_STUCK, 27, {});
        }
    }
    else if (lost_counter >= PEEP_LOST_WARNING_THRESHOLD)
    {
        warning_throttle[6] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            News::AddItemToQueue(News::ItemType::Peeps, STR_PEEPS_GETTING_LOST_OR_STUCK, 16, {});
        }
    }
}

void peep_stop_crowd_noise()
{
    if (_crowdSoundChannel != nullptr)
    {
        Mixer_Stop_Channel(_crowdSoundChannel);
        _crowdSoundChannel = nullptr;
    }
}

/**
 *
 *  rct2: 0x006BD18A
 */
void peep_update_crowd_noise()
{
    if (OpenRCT2::Audio::gGameSoundsOff)
        return;

    if (!gConfigSound.sound_enabled)
        return;

    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        return;

    auto viewport = g_music_tracking_viewport;
    if (viewport == nullptr)
        return;

    // Count the number of peeps visible
    auto visiblePeeps = 0;

    for (auto peep : EntityList<Guest>(EntityListId::Peep))
    {
        if (peep->sprite_left == LOCATION_NULL)
            continue;
        if (viewport->viewPos.x > peep->sprite_right)
            continue;
        if (viewport->viewPos.x + viewport->view_width < peep->sprite_left)
            continue;
        if (viewport->viewPos.y > peep->sprite_bottom)
            continue;
        if (viewport->viewPos.y + viewport->view_height < peep->sprite_top)
            continue;

        visiblePeeps += peep->State == PeepState::Queuing ? 1 : 2;
    }

    // This function doesn't account for the fact that the screen might be so big that 100 peeps could potentially be very
    // spread out and therefore not produce any crowd noise. Perhaps a more sophisticated solution would check how many peeps
    // were in close proximity to each other.

    // Allows queuing peeps to make half as much noise, and at least 6 peeps must be visible for any crowd noise
    visiblePeeps = (visiblePeeps / 2) - 6;
    if (visiblePeeps < 0)
    {
        // Mute crowd noise
        if (_crowdSoundChannel != nullptr)
        {
            Mixer_Stop_Channel(_crowdSoundChannel);
            _crowdSoundChannel = nullptr;
        }
    }
    else
    {
        int32_t volume;

        // Formula to scale peeps to dB where peeps [0, 120] scales approximately logarithmically to [-3314, -150] dB/100
        // 207360000 maybe related to DSBVOLUME_MIN which is -10,000 (dB/100)
        volume = 120 - std::min(visiblePeeps, 120);
        volume = volume * volume * volume * volume;
        volume = (((207360000 - volume) / viewport->zoom) - 207360000) / 65536 - 150;

        // Load and play crowd noise if needed and set volume
        if (_crowdSoundChannel == nullptr)
        {
            _crowdSoundChannel = Mixer_Play_Music(PATH_ID_CSS2, MIXER_LOOP_INFINITE, false);
            if (_crowdSoundChannel != nullptr)
            {
                Mixer_Channel_SetGroup(_crowdSoundChannel, OpenRCT2::Audio::MixerGroup::Sound);
            }
        }
        if (_crowdSoundChannel != nullptr)
        {
            Mixer_Channel_Volume(_crowdSoundChannel, DStoMixerVolume(volume));
        }
    }
}

/**
 *
 *  rct2: 0x0069BE9B
 */
void peep_applause()
{
    for (auto peep : EntityList<Guest>(EntityListId::Peep))
    {
        if (peep->OutsideOfPark)
            continue;

        // Release balloon
        peep_release_balloon(peep, peep->z + 9);

        // Clap
        if ((peep->State == PeepState::Walking || peep->State == PeepState::Queuing) && peep->Action >= PeepActionType::None1)
        {
            peep->Action = PeepActionType::Clap;
            peep->ActionFrame = 0;
            peep->ActionSpriteImageOffset = 0;
            peep->UpdateCurrentActionSpriteType();
        }
    }

    // Play applause noise
    OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Applause, 0, context_get_width() / 2);
}

/**
 *
 *  rct2: 0x0069C35E
 */
void peep_update_days_in_queue()
{
    for (auto peep : EntityList<Guest>(EntityListId::Peep))
    {
        if (!peep->OutsideOfPark && peep->State == PeepState::Queuing)
        {
            if (peep->DaysInQueue < 255)
            {
                peep->DaysInQueue += 1;
            }
        }
    }
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
 *  rct2: 0x699F5A
 * al:thoughtType
 * ah:thoughtArguments
 * esi: peep
 */
void Peep::InsertNewThought(PeepThoughtType thoughtType, uint8_t thoughtArguments)
{
    PeepActionType newAction = PeepThoughtToActionMap[EnumValue(thoughtType)].action;
    if (newAction != PeepActionType::None2 && this->Action >= PeepActionType::None1)
    {
        Action = newAction;
        ActionFrame = 0;
        ActionSpriteImageOffset = 0;
        UpdateCurrentActionSpriteType();
    }

    for (int32_t i = 0; i < PEEP_MAX_THOUGHTS; ++i)
    {
        rct_peep_thought* thought = &Thoughts[i];
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
                memmove(thought, thought + 1, sizeof(rct_peep_thought) * (PEEP_MAX_THOUGHTS - i - 1));
            }
            break;
        }
    }

    memmove(&Thoughts[1], &Thoughts[0], sizeof(rct_peep_thought) * (PEEP_MAX_THOUGHTS - 1));

    Thoughts[0].type = thoughtType;
    Thoughts[0].item = thoughtArguments;
    Thoughts[0].freshness = 0;
    Thoughts[0].fresh_timeout = 0;

    WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_THOUGHTS;
}

/**
 *
 *  rct2: 0x0069A05D
 */
Peep* Peep::Generate(const CoordsXYZ& coords)
{
    if (GetNumFreeEntities() < 400)
        return nullptr;

    Peep* peep = &create_sprite(SpriteIdentifier::Peep)->peep;
    peep->sprite_identifier = SpriteIdentifier::Peep;
    peep->SpriteType = PeepSpriteType::Normal;
    peep->OutsideOfPark = true;
    peep->State = PeepState::Falling;
    peep->Action = PeepActionType::None2;
    peep->SpecialSprite = 0;
    peep->ActionSpriteImageOffset = 0;
    peep->WalkingFrameNum = 0;
    peep->ActionSpriteType = PeepActionSpriteType::None;
    peep->PeepFlags = 0;
    peep->FavouriteRide = RIDE_ID_NULL;
    peep->FavouriteRideRating = 0;

    const rct_sprite_bounds* spriteBounds = &GetSpriteBounds(peep->SpriteType, peep->ActionSpriteType);
    peep->sprite_width = spriteBounds->sprite_width;
    peep->sprite_height_negative = spriteBounds->sprite_height_negative;
    peep->sprite_height_positive = spriteBounds->sprite_height_positive;

    peep->MoveTo(coords);
    peep->sprite_direction = 0;
    peep->Mass = (scenario_rand() & 0x1F) + 45;
    peep->PathCheckOptimisation = 0;
    peep->InteractionRideIndex = RIDE_ID_NULL;
    peep->AssignedPeepType = PeepType::Guest;
    peep->PreviousRide = RIDE_ID_NULL;
    peep->Thoughts->type = PeepThoughtType::None;
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
    std::fill_n(peep->RidesBeenOn, 32, 0x00);

    peep->GuestNumRides = 0;
    std::fill_n(peep->RideTypesBeenOn, 16, 0x00);
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
    peep->GuestHeadingToRideId = RIDE_ID_NULL;
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

    return peep;
}

void Peep::FormatActionTo(Formatter& ft) const
{
    switch (State)
    {
        case PeepState::Falling:
            ft.Add<rct_string_id>(Action == PeepActionType::Drowning ? STR_DROWNING : STR_WALKING);
            break;
        case PeepState::One:
            ft.Add<rct_string_id>(STR_WALKING);
            break;
        case PeepState::OnRide:
        case PeepState::LeavingRide:
        case PeepState::EnteringRide:
        {
            auto ride = get_ride(CurrentRide);
            if (ride != nullptr)
            {
                ft.Add<rct_string_id>(ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IN_RIDE) ? STR_IN_RIDE : STR_ON_RIDE);
                ride->FormatNameTo(ft);
            }
            else
            {
                ft.Add<rct_string_id>(STR_ON_RIDE).Add<rct_string_id>(STR_NONE);
            }
            break;
        }
        case PeepState::Buying:
        {
            ft.Add<rct_string_id>(STR_AT_RIDE);
            auto ride = get_ride(CurrentRide);
            if (ride != nullptr)
            {
                ride->FormatNameTo(ft);
            }
            else
            {
                ft.Add<rct_string_id>(STR_NONE);
            }
            break;
        }
        case PeepState::Walking:
        case PeepState::UsingBin:
            if (GuestHeadingToRideId != RIDE_ID_NULL)
            {
                auto ride = get_ride(GuestHeadingToRideId);
                if (ride != nullptr)
                {
                    ft.Add<rct_string_id>(STR_HEADING_FOR);
                    ride->FormatNameTo(ft);
                }
            }
            else
            {
                ft.Add<rct_string_id>((PeepFlags & PEEP_FLAGS_LEAVING_PARK) ? STR_LEAVING_PARK : STR_WALKING);
            }
            break;
        case PeepState::QueuingFront:
        case PeepState::Queuing:
        {
            auto ride = get_ride(CurrentRide);
            if (ride != nullptr)
            {
                ft.Add<rct_string_id>(STR_QUEUING_FOR);
                ride->FormatNameTo(ft);
            }
            break;
        }
        case PeepState::Sitting:
            ft.Add<rct_string_id>(STR_SITTING);
            break;
        case PeepState::Watching:
            if (CurrentRide != RIDE_ID_NULL)
            {
                auto ride = get_ride(CurrentRide);
                if (ride != nullptr)
                {
                    ft.Add<rct_string_id>((StandingFlags & 0x1) ? STR_WATCHING_CONSTRUCTION_OF : STR_WATCHING_RIDE);
                    ride->FormatNameTo(ft);
                }
            }
            else
            {
                ft.Add<rct_string_id>((StandingFlags & 0x1) ? STR_WATCHING_NEW_RIDE_BEING_CONSTRUCTED : STR_LOOKING_AT_SCENERY);
            }
            break;
        case PeepState::Picked:
            ft.Add<rct_string_id>(STR_SELECT_LOCATION);
            break;
        case PeepState::Patrolling:
        case PeepState::EnteringPark:
        case PeepState::LeavingPark:
            ft.Add<rct_string_id>(STR_WALKING);
            break;
        case PeepState::Mowing:
            ft.Add<rct_string_id>(STR_MOWING_GRASS);
            break;
        case PeepState::Sweeping:
            ft.Add<rct_string_id>(STR_SWEEPING_FOOTPATH);
            break;
        case PeepState::Watering:
            ft.Add<rct_string_id>(STR_WATERING_GARDENS);
            break;
        case PeepState::EmptyingBin:
            ft.Add<rct_string_id>(STR_EMPTYING_LITTER_BIN);
            break;
        case PeepState::Answering:
            if (SubState == 0)
            {
                ft.Add<rct_string_id>(STR_WALKING);
            }
            else if (SubState == 1)
            {
                ft.Add<rct_string_id>(STR_ANSWERING_RADIO_CALL);
            }
            else
            {
                ft.Add<rct_string_id>(STR_RESPONDING_TO_RIDE_BREAKDOWN_CALL);
                auto ride = get_ride(CurrentRide);
                if (ride != nullptr)
                {
                    ride->FormatNameTo(ft);
                }
                else
                {
                    ft.Add<rct_string_id>(STR_NONE);
                }
            }
            break;
        case PeepState::Fixing:
        {
            ft.Add<rct_string_id>(STR_FIXING_RIDE);
            auto ride = get_ride(CurrentRide);
            if (ride != nullptr)
            {
                ride->FormatNameTo(ft);
            }
            else
            {
                ft.Add<rct_string_id>(STR_NONE);
            }
            break;
        }
        case PeepState::HeadingToInspection:
        {
            ft.Add<rct_string_id>(STR_HEADING_TO_RIDE_FOR_INSPECTION);
            auto ride = get_ride(CurrentRide);
            if (ride != nullptr)
            {
                ride->FormatNameTo(ft);
            }
            else
            {
                ft.Add<rct_string_id>(STR_NONE);
            }
            break;
        }
        case PeepState::Inspecting:
        {
            ft.Add<rct_string_id>(STR_INSPECTING_RIDE);
            auto ride = get_ride(CurrentRide);
            if (ride != nullptr)
            {
                ride->FormatNameTo(ft);
            }
            else
            {
                ft.Add<rct_string_id>(STR_NONE);
            }
            break;
        }
    }
}

void Peep::FormatNameTo(Formatter& ft) const
{
    if (Name == nullptr)
    {
        if (AssignedPeepType == PeepType::Staff)
        {
            static constexpr const rct_string_id staffNames[] = {
                STR_HANDYMAN_X,
                STR_MECHANIC_X,
                STR_SECURITY_GUARD_X,
                STR_ENTERTAINER_X,
            };

            auto staffNameIndex = static_cast<uint8_t>(AssignedStaffType);
            if (staffNameIndex > sizeof(staffNames))
            {
                staffNameIndex = 0;
            }

            ft.Add<rct_string_id>(staffNames[staffNameIndex]);
            ft.Add<uint32_t>(Id);
        }
        else if (gParkFlags & PARK_FLAGS_SHOW_REAL_GUEST_NAMES)
        {
            auto realNameStringId = get_real_name_string_id_from_id(Id);
            ft.Add<rct_string_id>(realNameStringId);
        }
        else
        {
            ft.Add<rct_string_id>(STR_GUEST_X).Add<uint32_t>(Id);
        }
    }
    else
    {
        ft.Add<rct_string_id>(STR_STRING).Add<const char*>(Name);
    }
}

std::string Peep::GetName() const
{
    Formatter ft;
    FormatNameTo(ft);
    return format_string(STR_STRINGID, ft.Data());
}

bool Peep::SetName(std::string_view value)
{
    if (value.empty())
    {
        std::free(Name);
        Name = nullptr;
        return true;
    }
    else
    {
        auto newNameMemory = static_cast<char*>(std::malloc(value.size() + 1));
        if (newNameMemory != nullptr)
        {
            std::memcpy(newNameMemory, value.data(), value.size());
            newNameMemory[value.size()] = '\0';
            std::free(Name);
            Name = newNameMemory;
            return true;
        }
    }
    return false;
}

/**
 * rct2: 0x00698342
 * thought.item (eax)
 * thought.type (ebx)
 * argument_1 (esi & ebx)
 * argument_2 (esi+2)
 */
void peep_thought_set_format_args(const rct_peep_thought* thought, Formatter& ft)
{
    ft.Add<rct_string_id>(PeepThoughts[EnumValue(thought->type)]);

    PeepThoughtToActionFlag flags = PeepThoughtToActionMap[EnumValue(thought->type)].flags;
    if (flags & PEEP_THOUGHT_ACTION_FLAG_RIDE)
    {
        auto ride = get_ride(thought->item);
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
        ft.Add<rct_string_id>(GetShopItemDescriptor(ShopItem(thought->item)).Naming.Singular);
    }
    else if (flags & PEEP_THOUGHT_ACTION_FLAG_SHOP_ITEM_INDEFINITE)
    {
        ft.Add<rct_string_id>(GetShopItemDescriptor(ShopItem(thought->item)).Naming.Indefinite);
    }
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

static int32_t get_face_sprite_offset(Peep* peep)
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
int32_t get_peep_face_sprite_small(Peep* peep)
{
    return face_sprite_small[get_face_sprite_offset(peep)];
}

/**
 * Function split into large and small sprite
 *  rct2: 0x00698721
 */
int32_t get_peep_face_sprite_large(Peep* peep)
{
    return face_sprite_large[get_face_sprite_offset(peep)];
}

void peep_set_map_tooltip(Peep* peep)
{
    auto ft = Formatter();
    if (peep->AssignedPeepType == PeepType::Guest)
    {
        ft.Add<rct_string_id>((peep->PeepFlags & PEEP_FLAGS_TRACKING) ? STR_TRACKED_GUEST_MAP_TIP : STR_GUEST_MAP_TIP);
        ft.Add<uint32_t>(get_peep_face_sprite_small(peep));
        peep->FormatNameTo(ft);
        peep->FormatActionTo(ft);
    }
    else
    {
        ft.Add<rct_string_id>(STR_STAFF_MAP_TIP);
        peep->FormatNameTo(ft);
        peep->FormatActionTo(ft);
    }

    auto intent = Intent(INTENT_ACTION_SET_MAP_TOOLTIP);
    intent.putExtra(INTENT_EXTRA_FORMATTER, &ft);
    context_broadcast_intent(&intent);
}

/**
 *  rct2: 0x00693BAB
 */
void Peep::SwitchNextActionSpriteType()
{
    // TBD: Add nextActionSpriteType as function parameter and make peep->NextActionSpriteType obsolete?
    if (NextActionSpriteType != ActionSpriteType)
    {
        Invalidate();
        ActionSpriteType = NextActionSpriteType;
        const rct_sprite_bounds* spriteBounds = &GetSpriteBounds(SpriteType, NextActionSpriteType);
        sprite_width = spriteBounds->sprite_width;
        sprite_height_negative = spriteBounds->sprite_height_negative;
        sprite_height_positive = spriteBounds->sprite_height_positive;
        Invalidate();
    }
}

/**
 *
 *  rct2: 0x00693CBB
 */
static bool peep_update_queue_position(Peep* peep, PeepActionType previous_action)
{
    peep->TimeInQueue++;

    auto* guestNext = GetEntity<Guest>(peep->GuestNextInQueue);
    if (guestNext == nullptr)
    {
        return false;
    }

    int16_t x_diff = abs(guestNext->x - peep->x);
    int16_t y_diff = abs(guestNext->y - peep->y);
    int16_t z_diff = abs(guestNext->z - peep->z);

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
            if ((peep->x & 0xFFE0) != (guestNext->x & 0xFFE0) || (peep->y & 0xFFE0) != (guestNext->y & 0xFFE0))
                return false;
        }

        if (peep->sprite_direction != guestNext->sprite_direction)
            return false;

        switch (guestNext->sprite_direction / 8)
        {
            case 0:
                if (peep->x >= guestNext->x)
                    return false;
                break;
            case 1:
                if (peep->y <= guestNext->y)
                    return false;
                break;
            case 2:
                if (peep->x <= guestNext->x)
                    return false;
                break;
            case 3:
                if (peep->y >= guestNext->y)
                    return false;
                break;
        }
    }

    if (peep->Action < PeepActionType::None1)
        peep->UpdateAction();

    if (peep->Action != PeepActionType::None2)
        return true;

    peep->Action = PeepActionType::None1;
    peep->NextActionSpriteType = PeepActionSpriteType::WatchRide;
    if (previous_action != PeepActionType::None1)
        peep->Invalidate();
    return true;
}

/**
 *
 *  rct2: 0x00693EF2
 */
static void peep_return_to_centre_of_tile(Peep* peep)
{
    peep->PeepDirection = direction_reverse(peep->PeepDirection);
    peep->DestinationX = (peep->x & 0xFFE0) + 16;
    peep->DestinationY = (peep->y & 0xFFE0) + 16;
    peep->DestinationTolerance = 5;
}

/**
 *
 *  rct2: 0x00693f2C
 */
static bool peep_interact_with_entrance(Peep* peep, const CoordsXYE& coords, uint8_t& pathing_result)
{
    auto tile_element = coords.element;
    uint8_t entranceType = tile_element->AsEntrance()->GetEntranceType();
    auto rideIndex = tile_element->AsEntrance()->GetRideIndex();

    if ((entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE) || (entranceType == ENTRANCE_TYPE_RIDE_EXIT))
    {
        // If an entrance or exit that doesn't belong to the ride we are queuing for ignore the entrance/exit
        // This can happen when paths clip through entrance/exits
        if (peep->State == PeepState::Queuing && peep->CurrentRide != rideIndex)
        {
            return false;
        }
    }
    // Store some details to determine when to override the default
    // behaviour (defined below) for when staff attempt to enter a ride
    // to fix/inspect it.
    if (entranceType == ENTRANCE_TYPE_RIDE_EXIT)
    {
        pathing_result |= PATHING_RIDE_EXIT;
        _peepRideEntranceExitElement = tile_element;
    }
    else if (entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE)
    {
        pathing_result |= PATHING_RIDE_ENTRANCE;
        _peepRideEntranceExitElement = tile_element;
    }

    if (entranceType == ENTRANCE_TYPE_RIDE_EXIT)
    {
        // Default guest/staff behaviour attempting to enter a
        // ride exit is to turn around.
        peep->InteractionRideIndex = RIDE_ID_NULL;
        peep_return_to_centre_of_tile(peep);
        return true;
    }

    if (entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE)
    {
        auto ride = get_ride(rideIndex);
        if (ride == nullptr)
            return false;

        auto guest = peep->AsGuest();
        if (guest == nullptr)
        {
            // Default staff behaviour attempting to enter a
            // ride entrance is to turn around.
            peep->InteractionRideIndex = RIDE_ID_NULL;
            peep_return_to_centre_of_tile(peep);
            return true;
        }

        if (peep->State == PeepState::Queuing)
        {
            // Guest is in the ride queue.
            peep->RideSubState = PeepRideSubState::AtQueueFront;
            peep->ActionSpriteImageOffset = _unk_F1AEF0;
            return true;
        }

        // Guest is on a normal path, i.e. ride has no queue.
        if (peep->InteractionRideIndex == rideIndex)
        {
            // Peep is retrying the ride entrance without leaving
            // the path tile and without trying any other ride
            // attached to this path tile. i.e. stick with the
            // peeps previous decision not to go on the ride.
            peep_return_to_centre_of_tile(peep);
            return true;
        }

        peep->TimeLost = 0;
        auto stationNum = tile_element->AsEntrance()->GetStationIndex();
        // Guest walks up to the ride for the first time since entering
        // the path tile or since considering another ride attached to
        // the path tile.
        if (!guest->ShouldGoOnRide(ride, stationNum, false, false))
        {
            // Peep remembers that this is the last ride they
            // considered while on this path tile.
            peep->InteractionRideIndex = rideIndex;
            peep_return_to_centre_of_tile(peep);
            return true;
        }

        // Guest has decided to go on the ride.
        peep->ActionSpriteImageOffset = _unk_F1AEF0;
        peep->InteractionRideIndex = rideIndex;

        uint16_t previous_last = ride->stations[stationNum].LastPeepInQueue;
        ride->stations[stationNum].LastPeepInQueue = peep->sprite_index;
        peep->GuestNextInQueue = previous_last;
        ride->stations[stationNum].QueueLength++;

        peep->CurrentRide = rideIndex;
        peep->CurrentRideStation = stationNum;
        peep->DaysInQueue = 0;
        peep->SetState(PeepState::Queuing);
        peep->RideSubState = PeepRideSubState::AtQueueFront;
        peep->TimeInQueue = 0;
        if (peep->PeepFlags & PEEP_FLAGS_TRACKING)
        {
            auto ft = Formatter();
            peep->FormatNameTo(ft);
            ride->FormatNameTo(ft);
            if (gConfigNotifications.guest_queuing_for_ride)
            {
                News::AddItemToQueue(
                    News::ItemType::PeepOnRide, STR_PEEP_TRACKING_PEEP_JOINED_QUEUE_FOR_X, peep->sprite_index, ft);
            }
        }
    }
    else
    {
        // PARK_ENTRANCE
        auto guest = peep->AsGuest();
        if (guest == nullptr)
        {
            // Staff cannot leave the park, so go back.
            peep_return_to_centre_of_tile(peep);
            return true;
        }

        // If not the centre of the entrance arch
        if (tile_element->AsEntrance()->GetSequenceIndex() != 0)
        {
            peep_return_to_centre_of_tile(peep);
            return true;
        }

        uint8_t entranceDirection = tile_element->GetDirection();
        if (entranceDirection != peep->PeepDirection)
        {
            if (direction_reverse(entranceDirection) != peep->PeepDirection)
            {
                peep_return_to_centre_of_tile(peep);
                return true;
            }

            // Peep is leaving the park.
            if (peep->State != PeepState::Walking)
            {
                peep_return_to_centre_of_tile(peep);
                return true;
            }

            if (!(peep->PeepFlags & PEEP_FLAGS_LEAVING_PARK))
            {
                // If the park is open and leaving flag isn't set return to centre
                if (gParkFlags & PARK_FLAGS_PARK_OPEN)
                {
                    peep_return_to_centre_of_tile(peep);
                    return true;
                }
            }

            peep->DestinationX += CoordsDirectionDelta[peep->PeepDirection].x;
            peep->DestinationY += CoordsDirectionDelta[peep->PeepDirection].y;
            peep->DestinationTolerance = 9;
            peep->MoveTo({ coords, peep->z });
            peep->SetState(PeepState::LeavingPark);

            peep->Var37 = 0;
            if (peep->PeepFlags & PEEP_FLAGS_TRACKING)
            {
                auto ft = Formatter();
                peep->FormatNameTo(ft);
                if (gConfigNotifications.guest_left_park)
                {
                    News::AddItemToQueue(News::ItemType::PeepOnRide, STR_PEEP_TRACKING_LEFT_PARK, peep->sprite_index, ft);
                }
            }
            return true;
        }

        // Peep is entering the park.

        if (peep->State != PeepState::EnteringPark)
        {
            peep_return_to_centre_of_tile(peep);
            return true;
        }

        if (!(gParkFlags & PARK_FLAGS_PARK_OPEN))
        {
            peep->State = PeepState::LeavingPark;
            peep->Var37 = 1;
            decrement_guests_heading_for_park();
            peep_window_state_update(peep);
            peep_return_to_centre_of_tile(peep);
            return true;
        }

        bool found = false;
        auto entrance = std::find_if(
            gParkEntrances.begin(), gParkEntrances.end(), [coords](const auto& e) { return coords.ToTileStart() == e; });
        if (entrance != gParkEntrances.end())
        {
            int16_t z = entrance->z / 8;
            entranceDirection = entrance->direction;
            auto nextLoc = coords.ToTileStart() + CoordsDirectionDelta[entranceDirection];

            // Make sure there is a path right behind the entrance, otherwise turn around
            TileElement* nextTileElement = map_get_first_element_at(nextLoc);
            do
            {
                if (nextTileElement == nullptr)
                    break;
                if (nextTileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
                    continue;

                if (nextTileElement->AsPath()->IsQueue())
                    continue;

                if (nextTileElement->AsPath()->IsSloped())
                {
                    uint8_t slopeDirection = nextTileElement->AsPath()->GetSlopeDirection();
                    if (slopeDirection == entranceDirection)
                    {
                        if (z != nextTileElement->base_height)
                        {
                            continue;
                        }
                        found = true;
                        break;
                    }

                    if (direction_reverse(slopeDirection) != entranceDirection)
                        continue;

                    if (z - 2 != nextTileElement->base_height)
                        continue;
                    found = true;
                    break;
                }
                else
                {
                    if (z != nextTileElement->base_height)
                    {
                        continue;
                    }
                    found = true;
                    break;
                }
            } while (!(nextTileElement++)->IsLastForTile());
        }

        if (!found)
        {
            peep->State = PeepState::LeavingPark;
            peep->Var37 = 1;
            decrement_guests_heading_for_park();
            peep_window_state_update(peep);
            peep_return_to_centre_of_tile(peep);
            return true;
        }

        money16 entranceFee = park_get_entrance_fee();
        if (entranceFee != 0)
        {
            if (peep->HasItem(ShopItem::Voucher))
            {
                if (peep->VoucherType == VOUCHER_TYPE_PARK_ENTRY_HALF_PRICE)
                {
                    entranceFee /= 2;
                    peep->RemoveItem(ShopItem::Voucher);
                    peep->WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                }
                else if (peep->VoucherType == VOUCHER_TYPE_PARK_ENTRY_FREE)
                {
                    entranceFee = 0;
                    peep->RemoveItem(ShopItem::Voucher);
                    peep->WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                }
            }
            if (entranceFee > peep->CashInPocket)
            {
                peep->State = PeepState::LeavingPark;
                peep->Var37 = 1;
                decrement_guests_heading_for_park();
                peep_window_state_update(peep);
                peep_return_to_centre_of_tile(peep);
                return true;
            }

            gTotalIncomeFromAdmissions += entranceFee;
            guest->SpendMoney(peep->PaidToEnter, entranceFee, ExpenditureType::ParkEntranceTickets);
            peep->PeepFlags |= PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY;
        }

        gTotalAdmissions++;
        window_invalidate_by_number(WC_PARK_INFORMATION, 0);

        peep->Var37 = 1;
        peep->DestinationX += CoordsDirectionDelta[peep->PeepDirection].x;
        peep->DestinationY += CoordsDirectionDelta[peep->PeepDirection].y;
        peep->DestinationTolerance = 7;
        peep->MoveTo({ coords, peep->z });
    }
    return true;
}

/**
 *
 *  rct2: 0x006946D8
 */
static void peep_footpath_move_forward(Peep* peep, const CoordsXYE& coords, bool vandalism)
{
    auto tile_element = coords.element;
    peep->NextLoc = { coords.ToTileStart(), tile_element->GetBaseZ() };
    peep->SetNextFlags(tile_element->AsPath()->GetSlopeDirection(), tile_element->AsPath()->IsSloped(), false);

    int16_t z = peep->GetZOnSlope(coords.x, coords.y);

    if (peep->AssignedPeepType == PeepType::Staff)
    {
        peep->MoveTo({ coords, z });
        return;
    }

    uint8_t vandalThoughtTimeout = (peep->VandalismSeen & 0xC0) >> 6;
    // Advance the vandalised tiles by 1
    uint8_t vandalisedTiles = (peep->VandalismSeen * 2) & 0x3F;

    if (vandalism)
    {
        // Add one more to the vandalised tiles
        vandalisedTiles |= 1;
        // If there has been 2 vandalised tiles in the last 6
        if (vandalisedTiles & 0x3E && (vandalThoughtTimeout == 0))
        {
            if ((scenario_rand() & 0xFFFF) <= 10922)
            {
                peep->InsertNewThought(PeepThoughtType::Vandalism, PEEP_THOUGHT_ITEM_NONE);
                peep->HappinessTarget = std::max(0, peep->HappinessTarget - 17);
            }
            vandalThoughtTimeout = 3;
        }
    }

    if (vandalThoughtTimeout && (scenario_rand() & 0xFFFF) <= 4369)
    {
        vandalThoughtTimeout--;
    }

    peep->VandalismSeen = (vandalThoughtTimeout << 6) | vandalisedTiles;
    uint16_t crowded = 0;
    uint8_t litter_count = 0;
    uint8_t sick_count = 0;
    auto quad = EntityTileList(coords);
    for (auto entity : quad)
    {
        if (auto other_peep = entity->As<Peep>(); other_peep != nullptr)
        {
            if (other_peep->State != PeepState::Walking)
                continue;

            if (abs(other_peep->z - peep->NextLoc.z) > 16)
                continue;
            crowded++;
            continue;
        }
        else if (auto litter = entity->As<Litter>(); litter != nullptr)
        {
            if (abs(litter->z - peep->NextLoc.z) > 16)
                continue;

            litter_count++;
            if (litter->SubType != LITTER_TYPE_SICK && litter->SubType != LITTER_TYPE_SICK_ALT)
                continue;

            litter_count--;
            sick_count++;
        }
    }

    if (crowded >= 10 && peep->State == PeepState::Walking && (scenario_rand() & 0xFFFF) <= 21845)
    {
        peep->InsertNewThought(PeepThoughtType::Crowded, PEEP_THOUGHT_ITEM_NONE);
        peep->HappinessTarget = std::max(0, peep->HappinessTarget - 14);
    }

    litter_count = std::min(static_cast<uint8_t>(3), litter_count);
    sick_count = std::min(static_cast<uint8_t>(3), sick_count);

    uint8_t disgusting_time = peep->DisgustingCount & 0xC0;
    uint8_t disgusting_count = ((peep->DisgustingCount & 0xF) << 2) | sick_count;
    peep->DisgustingCount = disgusting_count | disgusting_time;

    if (disgusting_time & 0xC0 && (scenario_rand() & 0xFFFF) <= 4369)
    {
        // Reduce the disgusting time
        peep->DisgustingCount -= 0x40;
    }
    else
    {
        uint8_t total_sick = 0;
        for (uint8_t time = 0; time < 3; time++)
        {
            total_sick += (disgusting_count >> (2 * time)) & 0x3;
        }

        if (total_sick >= 3 && (scenario_rand() & 0xFFFF) <= 10922)
        {
            peep->InsertNewThought(PeepThoughtType::PathDisgusting, PEEP_THOUGHT_ITEM_NONE);
            peep->HappinessTarget = std::max(0, peep->HappinessTarget - 17);
            // Reset disgusting time
            peep->DisgustingCount |= 0xC0;
        }
    }

    uint8_t litter_time = peep->LitterCount & 0xC0;
    litter_count = ((peep->LitterCount & 0xF) << 2) | litter_count;
    peep->LitterCount = litter_count | litter_time;

    if (litter_time & 0xC0 && (scenario_rand() & 0xFFFF) <= 4369)
    {
        // Reduce the litter time
        peep->LitterCount -= 0x40;
    }
    else
    {
        uint8_t total_litter = 0;
        for (uint8_t time = 0; time < 3; time++)
        {
            total_litter += (litter_count >> (2 * time)) & 0x3;
        }

        if (total_litter >= 3 && (scenario_rand() & 0xFFFF) <= 10922)
        {
            peep->InsertNewThought(PeepThoughtType::BadLitter, PEEP_THOUGHT_ITEM_NONE);
            peep->HappinessTarget = std::max(0, peep->HappinessTarget - 17);
            // Reset litter time
            peep->LitterCount |= 0xC0;
        }
    }

    peep->MoveTo({ coords, z });
}

/**
 *
 *  rct2: 0x0069455E
 */
static void peep_interact_with_path(Peep* peep, const CoordsXYE& coords)
{
    // 0x00F1AEE2
    auto tile_element = coords.element;
    bool vandalism_present = false;
    if (tile_element->AsPath()->HasAddition() && (tile_element->AsPath()->IsBroken())
        && (tile_element->AsPath()->GetEdges()) != 0xF)
    {
        vandalism_present = true;
    }

    int16_t z = tile_element->GetBaseZ();
    if (map_is_location_owned({ coords, z }))
    {
        if (peep->OutsideOfPark)
        {
            peep_return_to_centre_of_tile(peep);
            return;
        }
    }
    else
    {
        if (!peep->OutsideOfPark)
        {
            peep_return_to_centre_of_tile(peep);
            return;
        }
    }

    auto guest = peep->AsGuest();
    if (guest != nullptr && tile_element->AsPath()->IsQueue())
    {
        auto rideIndex = tile_element->AsPath()->GetRideIndex();
        if (peep->State == PeepState::Queuing)
        {
            // Check if this queue is connected to the ride the
            // peep is queuing for, i.e. the player hasn't edited
            // the queue, rebuilt the ride, etc.
            if (peep->CurrentRide == rideIndex)
            {
                peep_footpath_move_forward(peep, { coords, tile_element }, vandalism_present);
            }
            else
            {
                // Queue got disconnected from the original ride.
                peep->InteractionRideIndex = RIDE_ID_NULL;
                guest->RemoveFromQueue();
                peep->SetState(PeepState::One);
                peep_footpath_move_forward(peep, { coords, tile_element }, vandalism_present);
            }
        }
        else
        {
            // Peep is not queuing.
            peep->TimeLost = 0;
            auto stationNum = tile_element->AsPath()->GetStationIndex();

            if ((tile_element->AsPath()->HasQueueBanner())
                && (tile_element->AsPath()->GetQueueBannerDirection()
                    == direction_reverse(peep->PeepDirection)) // Ride sign is facing the direction the peep is walking
            )
            {
                /* Peep is approaching the entrance of a ride queue.
                 * Decide whether to go on the ride. */
                auto ride = get_ride(rideIndex);
                if (ride != nullptr && guest->ShouldGoOnRide(ride, stationNum, true, false))
                {
                    // Peep has decided to go on the ride at the queue.
                    peep->InteractionRideIndex = rideIndex;

                    // Add the peep to the ride queue.
                    uint16_t old_last_peep = ride->stations[stationNum].LastPeepInQueue;
                    ride->stations[stationNum].LastPeepInQueue = peep->sprite_index;
                    peep->GuestNextInQueue = old_last_peep;
                    ride->stations[stationNum].QueueLength++;

                    peep_decrement_num_riders(peep);
                    peep->CurrentRide = rideIndex;
                    peep->CurrentRideStation = stationNum;
                    peep->State = PeepState::Queuing;
                    peep->DaysInQueue = 0;
                    peep_window_state_update(peep);

                    peep->RideSubState = PeepRideSubState::InQueue;
                    peep->DestinationTolerance = 2;
                    peep->TimeInQueue = 0;
                    if (peep->PeepFlags & PEEP_FLAGS_TRACKING)
                    {
                        auto ft = Formatter();
                        peep->FormatNameTo(ft);
                        ride->FormatNameTo(ft);
                        if (gConfigNotifications.guest_queuing_for_ride)
                        {
                            News::AddItemToQueue(
                                News::ItemType::PeepOnRide, STR_PEEP_TRACKING_PEEP_JOINED_QUEUE_FOR_X, peep->sprite_index, ft);
                        }
                    }

                    peep_footpath_move_forward(peep, { coords, tile_element }, vandalism_present);
                }
                else
                {
                    // Peep has decided not to go on the ride.
                    peep_return_to_centre_of_tile(peep);
                }
            }
            else
            {
                /* Peep is approaching a queue tile without a ride
                 * sign facing the peep. */
                peep_footpath_move_forward(peep, { coords, tile_element }, vandalism_present);
            }
        }
    }
    else
    {
        peep->InteractionRideIndex = RIDE_ID_NULL;
        if (peep->State == PeepState::Queuing)
        {
            peep->RemoveFromQueue();
            peep->SetState(PeepState::One);
        }
        peep_footpath_move_forward(peep, { coords, tile_element }, vandalism_present);
    }
}

/**
 *
 *  rct2: 0x00693F70
 */
static bool peep_interact_with_shop(Peep* peep, const CoordsXYE& coords)
{
    ride_id_t rideIndex = coords.element->AsTrack()->GetRideIndex();
    auto ride = get_ride(rideIndex);
    if (ride == nullptr || !ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
        return false;

    auto guest = peep->AsGuest();
    if (guest == nullptr)
    {
        peep_return_to_centre_of_tile(peep);
        return true;
    }

    // If we are queuing ignore the 'shop'
    // This can happen when paths clip through track
    if (peep->State == PeepState::Queuing)
    {
        return false;
    }

    peep->TimeLost = 0;

    if (ride->status != RIDE_STATUS_OPEN)
    {
        peep_return_to_centre_of_tile(peep);
        return true;
    }

    if (peep->InteractionRideIndex == rideIndex)
    {
        peep_return_to_centre_of_tile(peep);
        return true;
    }

    if (peep->PeepFlags & PEEP_FLAGS_LEAVING_PARK)
    {
        peep_return_to_centre_of_tile(peep);
        return true;
    }

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_PEEP_SHOULD_GO_INSIDE_FACILITY))
    {
        peep->TimeLost = 0;
        if (!guest->ShouldGoOnRide(ride, 0, false, false))
        {
            peep_return_to_centre_of_tile(peep);
            return true;
        }

        money16 cost = ride->price[0];
        if (cost != 0 && !(gParkFlags & PARK_FLAGS_NO_MONEY))
        {
            ride->total_profit += cost;
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
            // TODO: Refactor? SpendMoney previously accepted nullptr to not track money, passing a temporary variable as a
            // workaround
            money16 money = 0;
            guest->SpendMoney(money, cost, ExpenditureType::ParkRideTickets);
        }

        auto coordsCentre = coords.ToTileCentre();
        peep->DestinationX = coordsCentre.x;
        peep->DestinationY = coordsCentre.y;
        peep->DestinationTolerance = 3;

        peep->CurrentRide = rideIndex;
        peep->SetState(PeepState::EnteringRide);
        peep->RideSubState = PeepRideSubState::ApproachShop;

        peep->GuestTimeOnRide = 0;
        ride->cur_num_customers++;
        if (peep->PeepFlags & PEEP_FLAGS_TRACKING)
        {
            auto ft = Formatter();
            peep->FormatNameTo(ft);
            ride->FormatNameTo(ft);
            rct_string_id string_id = ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IN_RIDE) ? STR_PEEP_TRACKING_PEEP_IS_IN_X
                                                                                             : STR_PEEP_TRACKING_PEEP_IS_ON_X;
            if (gConfigNotifications.guest_used_facility)
            {
                News::AddItemToQueue(News::ItemType::PeepOnRide, string_id, peep->sprite_index, ft);
            }
        }
    }
    else
    {
        if (peep->GuestHeadingToRideId == rideIndex)
            peep->GuestHeadingToRideId = RIDE_ID_NULL;
        peep->ActionSpriteImageOffset = _unk_F1AEF0;
        peep->SetState(PeepState::Buying);
        peep->CurrentRide = rideIndex;
        peep->SubState = 0;
    }

    return true;
}

void Peep::PerformNextAction(uint8_t& pathing_result)
{
    TileElement* tmpTile;
    PerformNextAction(pathing_result, tmpTile);
}

/**
 *
 *  rct2: 0x00693C9E
 */
void Peep::PerformNextAction(uint8_t& pathing_result, TileElement*& tile_result)
{
    pathing_result = 0;
    PeepActionType previousAction = Action;

    if (Action == PeepActionType::None1)
        Action = PeepActionType::None2;

    if (State == PeepState::Queuing)
    {
        if (peep_update_queue_position(this, previousAction))
            return;
    }

    std::optional<CoordsXY> loc;
    if (!(loc = UpdateAction()))
    {
        pathing_result |= PATHING_DESTINATION_REACHED;
        uint8_t result = 0;

        auto guest = AsGuest();
        if (guest != nullptr)
        {
            result = guest_path_finding(guest);
        }
        else
        {
            auto staff = AsStaff();
            result = staff->DoPathFinding();
        }

        if (result != 0)
            return;

        if (!(loc = UpdateAction()))
            return;
    }

    auto newLoc = *loc;
    CoordsXY truncatedNewLoc = newLoc.ToTileStart();
    if (truncatedNewLoc == CoordsXY{ NextLoc })
    {
        int16_t height = GetZOnSlope(newLoc.x, newLoc.y);
        MoveTo({ newLoc.x, newLoc.y, height });
        return;
    }

    if (map_is_edge(newLoc))
    {
        if (OutsideOfPark)
        {
            pathing_result |= PATHING_OUTSIDE_PARK;
        }
        peep_return_to_centre_of_tile(this);
        return;
    }

    TileElement* tileElement = map_get_first_element_at(newLoc);
    if (tileElement == nullptr)
        return;
    int16_t base_z = std::max(0, (z / 8) - 2);
    int16_t top_z = (z / 8) + 1;

    do
    {
        if (base_z > tileElement->base_height)
            continue;
        if (top_z < tileElement->base_height)
            continue;
        if (tileElement->IsGhost())
            continue;

        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
        {
            peep_interact_with_path(this, { newLoc, tileElement });
            tile_result = tileElement;
            return;
        }
        else if (tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK)
        {
            if (peep_interact_with_shop(this, { newLoc, tileElement }))
            {
                tile_result = tileElement;
                return;
            }
        }
        else if (tileElement->GetType() == TILE_ELEMENT_TYPE_ENTRANCE)
        {
            if (peep_interact_with_entrance(this, { newLoc, tileElement }, pathing_result))
            {
                tile_result = tileElement;
                return;
            }
        }
    } while (!(tileElement++)->IsLastForTile());

    if (AssignedPeepType == PeepType::Staff || (GetNextIsSurface()))
    {
        int16_t height = abs(tile_element_height(newLoc) - z);
        if (height <= 3 || (AssignedPeepType == PeepType::Staff && height <= 32))
        {
            InteractionRideIndex = RIDE_ID_NULL;
            if (State == PeepState::Queuing)
            {
                RemoveFromQueue();
                SetState(PeepState::One);
            }

            if (!map_is_location_in_park(newLoc))
            {
                peep_return_to_centre_of_tile(this);
                return;
            }

            auto surfaceElement = map_get_surface_element_at(newLoc);
            if (surfaceElement == nullptr)
            {
                peep_return_to_centre_of_tile(this);
                return;
            }

            int16_t water_height = surfaceElement->GetWaterHeight();
            if (water_height > 0)
            {
                peep_return_to_centre_of_tile(this);
                return;
            }

            if (AssignedPeepType == PeepType::Staff && !GetNextIsSurface())
            {
                // Prevent staff from leaving the path on their own unless they're allowed to mow.
                if (!((this->StaffOrders & STAFF_ORDERS_MOWING) && this->StaffMowingTimeout >= 12))
                {
                    peep_return_to_centre_of_tile(this);
                    return;
                }
            }

            // The peep is on a surface and not on a path
            NextLoc = { truncatedNewLoc, surfaceElement->GetBaseZ() };
            SetNextFlags(0, false, true);

            height = GetZOnSlope(newLoc.x, newLoc.y);
            MoveTo({ newLoc.x, newLoc.y, height });
            return;
        }
    }

    peep_return_to_centre_of_tile(this);
}

/**
 * Gets the height including the bit depending on how far up the slope the peep
 * is.
 *  rct2: 0x00694921
 */
int32_t Peep::GetZOnSlope(int32_t tile_x, int32_t tile_y)
{
    if (tile_x == LOCATION_NULL)
        return 0;

    if (GetNextIsSurface())
    {
        return tile_element_height({ tile_x, tile_y });
    }

    uint8_t slope = GetNextDirection();
    return NextLoc.z + map_height_from_slope({ tile_x, tile_y }, slope, GetNextIsSloped());
}

rct_string_id get_real_name_string_id_from_id(uint32_t id)
{
    // Generate a name_string_idx from the peep Id using bit twiddling
    uint16_t ax = static_cast<uint16_t>(id + 0xF0B);
    uint16_t dx = 0;
    static constexpr uint16_t twiddlingBitOrder[] = { 4, 9, 3, 7, 5, 8, 2, 1, 6, 0, 12, 11, 13, 10 };
    for (size_t i = 0; i < std::size(twiddlingBitOrder); i++)
    {
        dx |= (ax & (1 << twiddlingBitOrder[i]) ? 1 : 0) << i;
    }
    ax = dx & 0xF;
    dx *= 4;
    ax *= 4096;
    dx += ax;
    if (dx < ax)
    {
        dx += 0x1000;
    }
    dx /= 4;
    dx += REAL_NAME_START;
    return dx;
}

int32_t peep_compare(const uint16_t sprite_index_a, const uint16_t sprite_index_b)
{
    Peep const* peep_a = GetEntity<Peep>(sprite_index_a);
    Peep const* peep_b = GetEntity<Peep>(sprite_index_b);
    if (peep_a == nullptr || peep_b == nullptr)
    {
        return 0;
    }

    // Compare types
    if (peep_a->AssignedPeepType != peep_b->AssignedPeepType)
    {
        return static_cast<int32_t>(peep_a->AssignedPeepType) - static_cast<int32_t>(peep_b->AssignedPeepType);
    }

    if (peep_a->Name == nullptr && peep_b->Name == nullptr)
    {
        if (gParkFlags & PARK_FLAGS_SHOW_REAL_GUEST_NAMES)
        {
            // Potentially could find a more optional way of sorting dynamic real names
        }
        else
        {
            // Simple ID comparison for when both peeps use a number or a generated name
            return peep_a->Id - peep_b->Id;
        }
    }

    // Compare their names as strings
    char nameA[256]{};
    Formatter ft;
    peep_a->FormatNameTo(ft);
    format_string(nameA, sizeof(nameA), STR_STRINGID, ft.Data());

    char nameB[256]{};
    ft.Rewind();
    peep_b->FormatNameTo(ft);
    format_string(nameB, sizeof(nameB), STR_STRINGID, ft.Data());
    return strlogicalcmp(nameA, nameB);
}

/**
 *
 *  rct2: 0x0069926C
 */
void peep_update_names(bool realNames)
{
    if (realNames)
    {
        gParkFlags |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
        // Peep names are now dynamic
    }
    else
    {
        gParkFlags &= ~PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
        // Peep names are now dynamic
    }

    auto intent = Intent(INTENT_ACTION_REFRESH_GUEST_LIST);
    context_broadcast_intent(&intent);
    gfx_invalidate_screen();
}

void increment_guests_in_park()
{
    if (gNumGuestsInPark < UINT32_MAX)
    {
        gNumGuestsInPark++;
    }
    else
    {
        openrct2_assert(false, "Attempt to increment guests in park above max value (65535).");
    }
}

void increment_guests_heading_for_park()
{
    if (gNumGuestsHeadingForPark < UINT32_MAX)
    {
        gNumGuestsHeadingForPark++;
    }
    else
    {
        openrct2_assert(false, "Attempt to increment guests heading for park above max value (65535).");
    }
}

void decrement_guests_in_park()
{
    if (gNumGuestsInPark > 0)
    {
        gNumGuestsInPark--;
    }
    else
    {
        log_error("Attempt to decrement guests in park below zero.");
    }
}

void decrement_guests_heading_for_park()
{
    if (gNumGuestsHeadingForPark > 0)
    {
        gNumGuestsHeadingForPark--;
    }
    else
    {
        log_error("Attempt to decrement guests heading for park below zero.");
    }
}

static void peep_release_balloon(Guest* peep, int16_t spawn_height)
{
    if (peep->HasItem(ShopItem::Balloon))
    {
        peep->RemoveItem(ShopItem::Balloon);

        if (peep->SpriteType == PeepSpriteType::Balloon && peep->x != LOCATION_NULL)
        {
            create_balloon({ peep->x, peep->y, spawn_height }, peep->BalloonColour, false);
            peep->WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
            peep->UpdateSpriteType();
        }
    }
}

/**
 *
 *  rct2: 0x006966A9
 */
void Peep::RemoveFromQueue()
{
    auto ride = get_ride(CurrentRide);
    if (ride == nullptr)
        return;

    auto& station = ride->stations[CurrentRideStation];
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

/**
 *
 *  rct2: 0x0069A512
 */
void Peep::RemoveFromRide()
{
    if (State == PeepState::Queuing)
    {
        RemoveFromQueue();
    }
    StateReset();
}

uint64_t Peep::GetItemFlags() const
{
    return ItemFlags;
}

void Peep::SetItemFlags(uint64_t itemFlags)
{
    ItemFlags = itemFlags;
}

void Peep::RemoveAllItems()
{
    ItemFlags = 0;
}

void Peep::RemoveItem(ShopItem item)
{
    ItemFlags &= ~EnumToFlag(item);
}

void Peep::GiveItem(ShopItem item)
{
    ItemFlags |= EnumToFlag(item);
}

bool Peep::HasItem(ShopItem peepItem) const
{
    return GetItemFlags() & EnumToFlag(peepItem);
}
