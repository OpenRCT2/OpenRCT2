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
#include "../audio/AudioChannel.h"
#include "../audio/AudioMixer.h"
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../core/Guard.hpp"
#include "../drawing/LightFX.h"
#include "../entity/Balloon.h"
#include "../entity/EntityRegistry.h"
#include "../entity/EntityTweener.h"
#include "../interface/Window.h"
#include "../localisation/Formatter.h"
#include "../localisation/Localisation.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../network/network.h"
#include "../paint/Paint.h"
#include "../peep/GuestPathfinding.h"
#include "../profiling/Profiling.h"
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
#include "../world/ConstructionClearance.h"
#include "../world/Entrance.h"
#include "../world/Footpath.h"
#include "../world/LargeScenery.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/SmallScenery.h"
#include "../world/Surface.h"
#include "PatrolArea.h"
#include "Staff.h"

#include <algorithm>
#include <iterator>
#include <limits>
#include <map>
#include <memory>
#include <optional>

using namespace OpenRCT2::Audio;

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

std::unique_ptr<GuestPathfinding> gGuestPathfinder = std::make_unique<OriginalPathfinding>();

static uint8_t _unk_F1AEF0;
static TileElement* _peepRideEntranceExitElement;

static std::shared_ptr<IAudioChannel> _crowdSoundChannel = nullptr;

static void peep_128_tick_update(Peep* peep, int32_t index);
static void peep_release_balloon(Guest* peep, int16_t spawn_height);

static PeepActionSpriteType PeepSpecialSpriteToSpriteTypeMap[] = {
    PeepActionSpriteType::None,
    PeepActionSpriteType::HoldMat,
    PeepActionSpriteType::StaffMower,
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
    PeepActionSpriteType::WithdrawMoney,
};

const bool gSpriteTypeToSlowWalkMap[] = {
    false, false, false, false, false, false, false, false, false, false, false, true, false, false, true,  true,
    true,  true,  true,  false, true,  false, true,  true,  true,  false, false, true, true,  false, false, true,
    true,  true,  true,  true,  true,  true,  false, true,  false, true,  true,  true, true,  true,  true,  true,
};

template<> bool EntityBase::Is<Peep>() const
{
    return Type == EntityType::Guest || Type == EntityType::Staff;
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

int32_t peep_get_staff_count()
{
    return GetEntityListCount(EntityType::Staff);
}

/**
 *
 *  rct2: 0x0068F0A9
 */
void peep_update_all()
{
    PROFILED_FUNCTION();

    if (gScreenFlags & SCREEN_FLAGS_EDITOR)
        return;

    int32_t i = 0;
    // Warning this loop can delete peeps
    for (auto peep : EntityList<Guest>())
    {
        if (static_cast<uint32_t>(i & 0x7F) != (gCurrentTicks & 0x7F))
        {
            peep->Update();
        }
        else
        {
            peep_128_tick_update(peep, i);
            // 128 tick can delete so double check its not deleted
            if (peep->Type == EntityType::Guest)
            {
                peep->Update();
            }
        }

        i++;
    }

    for (auto staff : EntityList<Staff>())
    {
        if (static_cast<uint32_t>(i & 0x7F) != (gCurrentTicks & 0x7F))
        {
            staff->Update();
        }
        else
        {
            peep_128_tick_update(staff, i);
            // 128 tick can delete so double check its not deleted
            if (staff->Type == EntityType::Staff)
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
    PROFILED_FUNCTION();

    auto* guest = peep->As<Guest>();
    if (guest != nullptr)
    {
        guest->Tick128UpdateGuest(index);
    }
    else
    {
        auto* staff = peep->As<Staff>();
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
    PROFILED_FUNCTION();

    PathCheckOptimisation++;
    if ((PathCheckOptimisation & 0xF) != (sprite_index.ToUnderlying() & 0xF))
    {
        // This condition makes the check happen less often
        // As a side effect peeps hover for a short,
        // random time when a path below them has been deleted
        return true;
    }

    TileElement* tile_element = map_get_first_element_at(NextLoc);

    auto mapType = TileElementType::Path;
    if (GetNextIsSurface())
    {
        mapType = TileElementType::Surface;
    }

    do
    {
        if (tile_element == nullptr)
            break;
        if (tile_element->GetType() == mapType)
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
    if (IsActionInterruptable())
    { // PeepActionType::None1 or PeepActionType::None2
        return PeepSpecialSpriteToSpriteTypeMap[SpecialSprite];
    }

    if (EnumValue(Action) < std::size(PeepActionToSpriteTypeMap))
    {
        return PeepActionToSpriteTypeMap[EnumValue(Action)];
    }

    openrct2_assert(
        EnumValue(Action) >= std::size(PeepActionToSpriteTypeMap) && Action < PeepActionType::Idle, "Invalid peep action %u",
        EnumValue(Action));
    return PeepActionSpriteType::None;
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

    if (IsActionInterruptable())
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
static constexpr const CoordsXY word_981D7C[4] = {
    { -2, 0 },
    { 0, 2 },
    { 2, 0 },
    { 0, -2 },
};

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
    PROFILED_FUNCTION();

    _unk_F1AEF0 = ActionSpriteImageOffset;
    if (Action == PeepActionType::Idle)
    {
        Action = PeepActionType::Walking;
    }

    CoordsXY differenceLoc = GetLocation();
    differenceLoc -= GetDestination();

    int32_t x_delta = abs(differenceLoc.x);
    int32_t y_delta = abs(differenceLoc.y);

    xy_distance = x_delta + y_delta;

    if (IsActionWalking())
    {
        if (xy_distance <= DestinationTolerance)
        {
            return std::nullopt;
        }
        int32_t nextDirection = 0;
        if (x_delta < y_delta)
        {
            nextDirection = 8;
            if (differenceLoc.y >= 0)
            {
                nextDirection = 24;
            }
        }
        else
        {
            nextDirection = 16;
            if (differenceLoc.x >= 0)
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
        Action = PeepActionType::Walking;
        UpdateCurrentActionSpriteType();
        return { { x, y } };
    }
    ActionSpriteImageOffset = peepAnimation[EnumValue(ActionSpriteType)].frame_offsets[ActionFrame];

    auto* guest = As<Guest>();
    // If not throwing up and not at the frame where sick appears.
    if (Action != PeepActionType::ThrowUp || ActionFrame != 15 || guest == nullptr)
    {
        return { { x, y } };
    }

    // We are throwing up
    guest->Hunger /= 2;
    guest->NauseaTarget /= 2;

    if (guest->Nausea < 30)
        guest->Nausea = 0;
    else
        guest->Nausea -= 30;

    WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_2;

    const auto curLoc = GetLocation();
    Litter::Create(
        { curLoc, sprite_direction }, (sprite_index.ToUnderlying() & 1) ? Litter::Type::VomitAlt : Litter::Type::Vomit);

    static constexpr OpenRCT2::Audio::SoundId coughs[4] = {
        OpenRCT2::Audio::SoundId::Cough1,
        OpenRCT2::Audio::SoundId::Cough2,
        OpenRCT2::Audio::SoundId::Cough3,
        OpenRCT2::Audio::SoundId::Cough4,
    };
    auto soundId = coughs[scenario_rand() & 3];
    OpenRCT2::Audio::Play3D(soundId, curLoc);

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
    rct_window* w = window_find_by_number(WindowClass::Peep, peep->sprite_index.ToUnderlying());
    if (w != nullptr)
        window_event_invalidate_call(w);

    if (peep->Is<Guest>())
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

        window_invalidate_by_number(WindowClass::Peep, peep->sprite_index);
        window_invalidate_by_class(WindowClass::GuestList);
    }
    else
    {
        window_invalidate_by_number(WindowClass::Peep, peep->sprite_index);
        window_invalidate_by_class(WindowClass::StaffList);
    }
}

void Peep::Pickup()
{
    auto* guest = As<Guest>();
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
        Action = PeepActionType::Walking;
        SpecialSprite = 0;
        ActionSpriteImageOffset = 0;
        ActionSpriteType = PeepActionSpriteType::None;
        PathCheckOptimisation = 0;
    }

    gPickupPeepImage = ImageId();
}

// Returns GameActions::Status::OK when a peep can be dropped at the given location. When apply is set to true the peep gets
// dropped.
GameActions::Result Peep::Place(const TileCoordsXYZ& location, bool apply)
{
    auto* pathElement = map_get_path_element_at(location);
    TileElement* tileElement = reinterpret_cast<TileElement*>(pathElement);
    if (pathElement == nullptr)
    {
        tileElement = reinterpret_cast<TileElement*>(map_get_surface_element_at(location.ToCoordsXYZ()));
    }
    if (tileElement == nullptr)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE, STR_NONE);
    }

    // Set the coordinate of destination to be exactly
    // in the middle of a tile.
    CoordsXYZ destination = { location.ToCoordsXY().ToTileCentre(), tileElement->GetBaseZ() + 16 };

    if (!map_is_location_owned(destination))
    {
        return GameActions::Result(GameActions::Status::NotOwned, STR_ERR_CANT_PLACE_PERSON_HERE, STR_LAND_NOT_OWNED_BY_PARK);
    }

    if (auto res = MapCanConstructAt({ destination, destination.z, destination.z + (1 * 8) }, { 0b1111, 0 });
        res.Error != GameActions::Status::Ok)
    {
        const auto stringId = std::get<StringId>(res.ErrorMessage);
        if (stringId != STR_RAISE_OR_LOWER_LAND_FIRST && stringId != STR_FOOTPATH_IN_THE_WAY)
        {
            return GameActions::Result(
                GameActions::Status::NoClearance, STR_ERR_CANT_PLACE_PERSON_HERE, stringId, res.ErrorMessageArgs.data());
        }
    }

    if (apply)
    {
        MoveTo(destination);
        SetState(PeepState::Falling);
        Action = PeepActionType::Walking;
        SpecialSprite = 0;
        ActionSpriteImageOffset = 0;
        ActionSpriteType = PeepActionSpriteType::None;
        PathCheckOptimisation = 0;
        EntityTweener::Get().Reset();
        auto* guest = As<Guest>();
        if (guest != nullptr)
        {
            ActionSpriteType = PeepActionSpriteType::Invalid;
            guest->HappinessTarget = std::max(guest->HappinessTarget - 10, 0);
            UpdateCurrentActionSpriteType();
        }
    }

    return GameActions::Result();
}

/**
 *
 *  rct2: 0x0069A535
 */
void peep_sprite_remove(Peep* peep)
{
    auto* guest = peep->As<Guest>();
    if (guest != nullptr)
    {
        guest->RemoveFromRide();
    }
    peep->Invalidate();

    window_close_by_number(WindowClass::Peep, peep->sprite_index);

    window_close_by_number(WindowClass::FirePrompt, EnumValue(peep->Type));

    auto* staff = peep->As<Staff>();
    // Needed for invalidations after sprite removal
    bool wasGuest = staff == nullptr;
    if (wasGuest)
    {
        News::DisableNewsItems(News::ItemType::PeepOnRide, peep->sprite_index.ToUnderlying());
    }
    else
    {
        staff->ClearPatrolArea();
        UpdateConsolidatedPatrolAreas();

        News::DisableNewsItems(News::ItemType::Peep, staff->sprite_index.ToUnderlying());
    }
    EntityRemove(peep);

    auto intent = Intent(wasGuest ? INTENT_ACTION_REFRESH_GUEST_LIST : INTENT_ACTION_REFRESH_STAFF_LIST);
    context_broadcast_intent(&intent);
}

/**
 * New function removes peep from park existence. Works with staff.
 */
void Peep::Remove()
{
    auto* guest = As<Guest>();
    if (guest != nullptr)
    {
        if (!guest->OutsideOfPark)
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
    TileElement* tile_element = map_get_first_element_at(CoordsXY{ x, y });
    TileElement* saved_map = nullptr;
    int32_t saved_height = 0;

    if (tile_element != nullptr)
    {
        do
        {
            // If a path check if we are on it
            if (tile_element->GetType() == TileElementType::Path)
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
            else if (tile_element->GetType() == TileElementType::Surface)
            {
                // If the surface is water check to see if we could be drowning
                if (tile_element->AsSurface()->GetWaterHeight() > 0)
                {
                    int32_t height = tile_element->AsSurface()->GetWaterHeight();

                    if (height - 4 >= z && height < z + 20)
                    {
                        // Looks like we are drowning!
                        MoveTo({ x, y, height });

                        auto* guest = As<Guest>();
                        if (guest != nullptr)
                        {
                            // Drop balloon if held
                            peep_release_balloon(guest, height);
                            guest->InsertNewThought(PeepThoughtType::Drowning);
                        }

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

    if (saved_map->GetType() != TileElementType::Path)
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

    if (Is<Guest>())
    {
        SetState(PeepState::Walking);
    }
    else
    {
        SetState(PeepState::Patrolling);
    }

    SetDestination(GetLocation(), 10);
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
    auto* guest = As<Guest>();
    if (SubState == 13 && guest != nullptr)
    {
        guest->InsertNewThought(PeepThoughtType::Help);
    }
}

/* From peep_update */
static void peep_update_thoughts(Guest* peep)
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
                        memmove(&peep->Thoughts[i], &peep->Thoughts[i + 1], sizeof(PeepThought) * (PEEP_MAX_THOUGHTS - i - 1));
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
    auto* guest = As<Guest>();
    if (guest != nullptr)
    {
        if (!guest->PreviousRide.IsNull())
            if (++guest->PreviousRideTimeOut >= 720)
                guest->PreviousRide = RideId::GetNull();

        peep_update_thoughts(guest);
    }

    // Walking speed logic
    uint32_t stepsToTake = Energy;
    if (stepsToTake < 95 && State == PeepState::Queuing)
        stepsToTake = 95;
    if ((PeepFlags & PEEP_FLAGS_SLOW_WALK) && State != PeepState::Queuing)
        stepsToTake /= 2;
    if (IsActionWalking() && GetNextIsSloped())
    {
        stepsToTake /= 2;
        if (State == PeepState::Queuing)
            stepsToTake += stepsToTake / 2;
    }

    uint32_t carryCheck = StepProgress + stepsToTake;
    StepProgress = carryCheck;
    if (carryCheck <= 255)
    {
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
                if (guest != nullptr)
                {
                    guest->UpdateGuest();
                }
                else
                {
                    auto* staff = As<Staff>();
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
    uint32_t hungerCounter = 0, lostCounter = 0, noexitCounter = 0, thirstCounter = 0, litterCounter = 0, disgustCounter = 0,
             toiletCounter = 0, vandalismCounter = 0;
    uint8_t* warningThrottle = gPeepWarningThrottle;

    int32_t inQueueCounter = 0;
    int32_t tooLongQueueCounter = 0;
    std::map<RideId, int32_t> queueComplainingGuestsMap;

    for (auto peep : EntityList<Guest>())
    {
        if (peep->OutsideOfPark)
            continue;

        if (peep->State == PeepState::Queuing || peep->State == PeepState::QueuingFront)
            inQueueCounter++;

        if (peep->Thoughts[0].freshness > 5)
            continue;

        switch (peep->Thoughts[0].type)
        {
            case PeepThoughtType::Lost: // 0x10
                lostCounter++;
                break;

            case PeepThoughtType::Hungry: // 0x14
                if (peep->GuestHeadingToRideId.IsNull())
                {
                    hungerCounter++;
                    break;
                }
                ride = get_ride(peep->GuestHeadingToRideId);
                if (ride != nullptr && !ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE))
                    hungerCounter++;
                break;

            case PeepThoughtType::Thirsty:
                if (peep->GuestHeadingToRideId.IsNull())
                {
                    thirstCounter++;
                    break;
                }
                ride = get_ride(peep->GuestHeadingToRideId);
                if (ride != nullptr && !ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_SELLS_DRINKS))
                    thirstCounter++;
                break;

            case PeepThoughtType::Toilet:
                if (peep->GuestHeadingToRideId.IsNull())
                {
                    toiletCounter++;
                    break;
                }
                ride = get_ride(peep->GuestHeadingToRideId);
                if (ride != nullptr && !ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IS_TOILET))
                    toiletCounter++;
                break;

            case PeepThoughtType::BadLitter: // 0x1a
                litterCounter++;
                break;
            case PeepThoughtType::CantFindExit: // 0x1b
                noexitCounter++;
                break;
            case PeepThoughtType::PathDisgusting: // 0x1f
                disgustCounter++;
                break;
            case PeepThoughtType::Vandalism: // 0x21
                vandalismCounter++;
                break;
            case PeepThoughtType::QueuingAges:
                tooLongQueueCounter++;
                queueComplainingGuestsMap[peep->Thoughts[0].rideId]++;
                break;
            default:
                break;
        }
    }
    // could maybe be packed into a loop, would lose a lot of clarity though
    if (warningThrottle[0])
        --warningThrottle[0];
    else if (hungerCounter >= PEEP_HUNGER_WARNING_THRESHOLD && hungerCounter >= gNumGuestsInPark / 16)
    {
        warningThrottle[0] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            constexpr auto thoughtId = static_cast<uint32_t>(PeepThoughtType::Hungry);
            News::AddItemToQueue(News::ItemType::Peeps, STR_PEEPS_ARE_HUNGRY, thoughtId, {});
        }
    }

    if (warningThrottle[1])
        --warningThrottle[1];
    else if (thirstCounter >= PEEP_THIRST_WARNING_THRESHOLD && thirstCounter >= gNumGuestsInPark / 16)
    {
        warningThrottle[1] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            constexpr auto thoughtId = static_cast<uint32_t>(PeepThoughtType::Thirsty);
            News::AddItemToQueue(News::ItemType::Peeps, STR_PEEPS_ARE_THIRSTY, thoughtId, {});
        }
    }

    if (warningThrottle[2])
        --warningThrottle[2];
    else if (toiletCounter >= PEEP_TOILET_WARNING_THRESHOLD && toiletCounter >= gNumGuestsInPark / 16)
    {
        warningThrottle[2] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            constexpr auto thoughtId = static_cast<uint32_t>(PeepThoughtType::Toilet);
            News::AddItemToQueue(News::ItemType::Peeps, STR_PEEPS_CANT_FIND_TOILET, thoughtId, {});
        }
    }

    if (warningThrottle[3])
        --warningThrottle[3];
    else if (litterCounter >= PEEP_LITTER_WARNING_THRESHOLD && litterCounter >= gNumGuestsInPark / 32)
    {
        warningThrottle[3] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            constexpr auto thoughtId = static_cast<uint32_t>(PeepThoughtType::BadLitter);
            News::AddItemToQueue(News::ItemType::Peeps, STR_PEEPS_DISLIKE_LITTER, thoughtId, {});
        }
    }

    if (warningThrottle[4])
        --warningThrottle[4];
    else if (disgustCounter >= PEEP_DISGUST_WARNING_THRESHOLD && disgustCounter >= gNumGuestsInPark / 32)
    {
        warningThrottle[4] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            constexpr auto thoughtId = static_cast<uint32_t>(PeepThoughtType::PathDisgusting);
            News::AddItemToQueue(News::ItemType::Peeps, STR_PEEPS_DISGUSTED_BY_PATHS, thoughtId, {});
        }
    }

    if (warningThrottle[5])
        --warningThrottle[5];
    else if (vandalismCounter >= PEEP_VANDALISM_WARNING_THRESHOLD && vandalismCounter >= gNumGuestsInPark / 32)
    {
        warningThrottle[5] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            constexpr auto thoughtId = static_cast<uint32_t>(PeepThoughtType::Vandalism);
            News::AddItemToQueue(News::ItemType::Peeps, STR_PEEPS_DISLIKE_VANDALISM, thoughtId, {});
        }
    }

    if (warningThrottle[6])
        --warningThrottle[6];
    else if (noexitCounter >= PEEP_NOEXIT_WARNING_THRESHOLD)
    {
        warningThrottle[6] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            constexpr auto thoughtId = static_cast<uint32_t>(PeepThoughtType::CantFindExit);
            News::AddItemToQueue(News::ItemType::Peeps, STR_PEEPS_GETTING_LOST_OR_STUCK, thoughtId, {});
        }
    }
    else if (lostCounter >= PEEP_LOST_WARNING_THRESHOLD)
    {
        warningThrottle[6] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            constexpr auto thoughtId = static_cast<uint32_t>(PeepThoughtType::Lost);
            News::AddItemToQueue(News::ItemType::Peeps, STR_PEEPS_GETTING_LOST_OR_STUCK, thoughtId, {});
        }
    }

    if (warningThrottle[7])
        --warningThrottle[7];
    else if (tooLongQueueCounter > PEEP_TOO_LONG_QUEUE_THRESHOLD && tooLongQueueCounter > inQueueCounter / 20)
    { // The amount of guests complaining about queue duration is at least 5% of the amount of queuing guests.
      // This includes guests who are no longer queuing.
        warningThrottle[7] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            auto rideWithMostQueueComplaints = std::max_element(
                queueComplainingGuestsMap.begin(), queueComplainingGuestsMap.end(),
                [](auto& lhs, auto& rhs) { return lhs.second < rhs.second; });
            auto rideId = rideWithMostQueueComplaints->first.ToUnderlying();
            News::AddItemToQueue(News::ItemType::Ride, STR_PEEPS_COMPLAINING_ABOUT_QUEUE_LENGTH_WARNING, rideId, {});
        }
    }
}

void peep_stop_crowd_noise()
{
    if (_crowdSoundChannel != nullptr)
    {
        _crowdSoundChannel->Stop();
        _crowdSoundChannel = nullptr;
    }
}

/**
 *
 *  rct2: 0x006BD18A
 */
void peep_update_crowd_noise()
{
    PROFILED_FUNCTION();

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

    for (auto peep : EntityList<Guest>())
    {
        if (peep->x == LOCATION_NULL)
            continue;
        if (viewport->viewPos.x > peep->SpriteRect.GetRight())
            continue;
        if (viewport->viewPos.x + viewport->view_width < peep->SpriteRect.GetLeft())
            continue;
        if (viewport->viewPos.y > peep->SpriteRect.GetBottom())
            continue;
        if (viewport->viewPos.y + viewport->view_height < peep->SpriteRect.GetTop())
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
            _crowdSoundChannel->SetVolume(0);
        }
    }
    else
    {
        int32_t volume;

        // Formula to scale peeps to dB where peeps [0, 120] scales approximately logarithmically to [-3314, -150] dB/100
        // 207360000 maybe related to DSBVOLUME_MIN which is -10,000 (dB/100)
        volume = 120 - std::min(visiblePeeps, 120);
        volume = volume * volume * volume * volume;
        volume = (viewport->zoom.ApplyInversedTo(207360000 - volume) - 207360000) / 65536 - 150;

        // Load and play crowd noise if needed and set volume
        if (_crowdSoundChannel == nullptr || _crowdSoundChannel->IsDone())
        {
            _crowdSoundChannel = CreateAudioChannel(SoundId::CrowdAmbience, true, 0);
            if (_crowdSoundChannel != nullptr)
            {
                _crowdSoundChannel->SetGroup(OpenRCT2::Audio::MixerGroup::Sound);
            }
        }
        if (_crowdSoundChannel != nullptr)
        {
            _crowdSoundChannel->SetVolume(DStoMixerVolume(volume));
        }
    }
}

/**
 *
 *  rct2: 0x0069BE9B
 */
void peep_applause()
{
    for (auto peep : EntityList<Guest>())
    {
        if (peep->OutsideOfPark)
            continue;

        // Release balloon
        peep_release_balloon(peep, peep->z + 9);

        // Clap
        if ((peep->State == PeepState::Walking || peep->State == PeepState::Queuing) && peep->IsActionInterruptable())
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
    for (auto peep : EntityList<Guest>())
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

void Peep::FormatActionTo(Formatter& ft) const
{
    switch (State)
    {
        case PeepState::Falling:
            ft.Add<StringId>(Action == PeepActionType::Drowning ? STR_DROWNING : STR_WALKING);
            break;
        case PeepState::One:
            ft.Add<StringId>(STR_WALKING);
            break;
        case PeepState::OnRide:
        case PeepState::LeavingRide:
        case PeepState::EnteringRide:
        {
            auto ride = get_ride(CurrentRide);
            if (ride != nullptr)
            {
                ft.Add<StringId>(ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IN_RIDE) ? STR_IN_RIDE : STR_ON_RIDE);
                ride->FormatNameTo(ft);
            }
            else
            {
                ft.Add<StringId>(STR_ON_RIDE).Add<StringId>(STR_NONE);
            }
            break;
        }
        case PeepState::Buying:
        {
            ft.Add<StringId>(STR_AT_RIDE);
            auto ride = get_ride(CurrentRide);
            if (ride != nullptr)
            {
                ride->FormatNameTo(ft);
            }
            else
            {
                ft.Add<StringId>(STR_NONE);
            }
            break;
        }
        case PeepState::Walking:
        case PeepState::UsingBin:
        {
            auto* guest = As<Guest>();
            if (guest != nullptr)
            {
                if (!guest->GuestHeadingToRideId.IsNull())
                {
                    auto ride = get_ride(guest->GuestHeadingToRideId);
                    if (ride != nullptr)
                    {
                        ft.Add<StringId>(STR_HEADING_FOR);
                        ride->FormatNameTo(ft);
                    }
                }
                else
                {
                    ft.Add<StringId>((PeepFlags & PEEP_FLAGS_LEAVING_PARK) ? STR_LEAVING_PARK : STR_WALKING);
                }
            }
            break;
        }
        case PeepState::QueuingFront:
        case PeepState::Queuing:
        {
            auto ride = get_ride(CurrentRide);
            if (ride != nullptr)
            {
                ft.Add<StringId>(STR_QUEUING_FOR);
                ride->FormatNameTo(ft);
            }
            break;
        }
        case PeepState::Sitting:
            ft.Add<StringId>(STR_SITTING);
            break;
        case PeepState::Watching:
            if (!CurrentRide.IsNull())
            {
                auto ride = get_ride(CurrentRide);
                if (ride != nullptr)
                {
                    ft.Add<StringId>((StandingFlags & 0x1) ? STR_WATCHING_CONSTRUCTION_OF : STR_WATCHING_RIDE);
                    ride->FormatNameTo(ft);
                }
            }
            else
            {
                ft.Add<StringId>((StandingFlags & 0x1) ? STR_WATCHING_NEW_RIDE_BEING_CONSTRUCTED : STR_LOOKING_AT_SCENERY);
            }
            break;
        case PeepState::Picked:
            ft.Add<StringId>(STR_SELECT_LOCATION);
            break;
        case PeepState::Patrolling:
        case PeepState::EnteringPark:
        case PeepState::LeavingPark:
            ft.Add<StringId>(STR_WALKING);
            break;
        case PeepState::Mowing:
            ft.Add<StringId>(STR_MOWING_GRASS);
            break;
        case PeepState::Sweeping:
            ft.Add<StringId>(STR_SWEEPING_FOOTPATH);
            break;
        case PeepState::Watering:
            ft.Add<StringId>(STR_WATERING_GARDENS);
            break;
        case PeepState::EmptyingBin:
            ft.Add<StringId>(STR_EMPTYING_LITTER_BIN);
            break;
        case PeepState::Answering:
            if (SubState == 0)
            {
                ft.Add<StringId>(STR_WALKING);
            }
            else if (SubState == 1)
            {
                ft.Add<StringId>(STR_ANSWERING_RADIO_CALL);
            }
            else
            {
                ft.Add<StringId>(STR_RESPONDING_TO_RIDE_BREAKDOWN_CALL);
                auto ride = get_ride(CurrentRide);
                if (ride != nullptr)
                {
                    ride->FormatNameTo(ft);
                }
                else
                {
                    ft.Add<StringId>(STR_NONE);
                }
            }
            break;
        case PeepState::Fixing:
        {
            ft.Add<StringId>(STR_FIXING_RIDE);
            auto ride = get_ride(CurrentRide);
            if (ride != nullptr)
            {
                ride->FormatNameTo(ft);
            }
            else
            {
                ft.Add<StringId>(STR_NONE);
            }
            break;
        }
        case PeepState::HeadingToInspection:
        {
            ft.Add<StringId>(STR_HEADING_TO_RIDE_FOR_INSPECTION);
            auto ride = get_ride(CurrentRide);
            if (ride != nullptr)
            {
                ride->FormatNameTo(ft);
            }
            else
            {
                ft.Add<StringId>(STR_NONE);
            }
            break;
        }
        case PeepState::Inspecting:
        {
            ft.Add<StringId>(STR_INSPECTING_RIDE);
            auto ride = get_ride(CurrentRide);
            if (ride != nullptr)
            {
                ride->FormatNameTo(ft);
            }
            else
            {
                ft.Add<StringId>(STR_NONE);
            }
            break;
        }
    }
}

static constexpr const StringId _staffNames[] = {
    STR_HANDYMAN_X,
    STR_MECHANIC_X,
    STR_SECURITY_GUARD_X,
    STR_ENTERTAINER_X,
};

void Peep::FormatNameTo(Formatter& ft) const
{
    if (Name == nullptr)
    {
        auto* staff = As<Staff>();
        if (staff != nullptr)
        {
            auto staffNameIndex = static_cast<uint8_t>(staff->AssignedStaffType);
            if (staffNameIndex >= std::size(_staffNames))
            {
                staffNameIndex = 0;
            }

            ft.Add<StringId>(_staffNames[staffNameIndex]);
            ft.Add<uint32_t>(Id);
        }
        else if (gParkFlags & PARK_FLAGS_SHOW_REAL_GUEST_NAMES)
        {
            auto realNameStringId = get_real_name_string_id_from_id(Id);
            ft.Add<StringId>(realNameStringId);
        }
        else
        {
            ft.Add<StringId>(STR_GUEST_X).Add<uint32_t>(Id);
        }
    }
    else
    {
        ft.Add<StringId>(STR_STRING).Add<const char*>(Name);
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

    auto newNameMemory = static_cast<char*>(std::malloc(value.size() + 1));
    if (newNameMemory != nullptr)
    {
        std::memcpy(newNameMemory, value.data(), value.size());
        newNameMemory[value.size()] = '\0';
        std::free(Name);
        Name = newNameMemory;
        return true;
    }
    return false;
}

bool Peep::IsActionWalking() const
{
    return Action == PeepActionType::Walking;
}

bool Peep::IsActionIdle() const
{
    return Action == PeepActionType::Idle;
}

bool Peep::IsActionInterruptable() const
{
    return IsActionIdle() || IsActionWalking();
}

void peep_set_map_tooltip(Peep* peep)
{
    auto ft = Formatter();
    auto* guest = peep->As<Guest>();
    if (guest != nullptr)
    {
        ft.Add<StringId>((peep->PeepFlags & PEEP_FLAGS_TRACKING) ? STR_TRACKED_GUEST_MAP_TIP : STR_GUEST_MAP_TIP);
        ft.Add<uint32_t>(get_peep_face_sprite_small(guest));
        guest->FormatNameTo(ft);
        guest->FormatActionTo(ft);
    }
    else
    {
        ft.Add<StringId>(STR_STAFF_MAP_TIP);
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
 *  rct2: 0x00693EF2
 */
static void peep_return_to_centre_of_tile(Peep* peep)
{
    peep->PeepDirection = direction_reverse(peep->PeepDirection);
    auto destination = peep->GetLocation().ToTileCentre();
    peep->SetDestination(destination, 5);
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
        peep->InteractionRideIndex = RideId::GetNull();
        peep_return_to_centre_of_tile(peep);
        return true;
    }

    if (entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE)
    {
        auto ride = get_ride(rideIndex);
        if (ride == nullptr)
            return false;

        auto* guest = peep->As<Guest>();
        if (guest == nullptr)
        {
            // Default staff behaviour attempting to enter a
            // ride entrance is to turn around.
            peep->InteractionRideIndex = RideId::GetNull();
            peep_return_to_centre_of_tile(peep);
            return true;
        }

        if (guest->State == PeepState::Queuing)
        {
            // Guest is in the ride queue.
            guest->RideSubState = PeepRideSubState::AtQueueFront;
            guest->ActionSpriteImageOffset = _unk_F1AEF0;
            return true;
        }

        // Guest is on a normal path, i.e. ride has no queue.
        if (guest->InteractionRideIndex == rideIndex)
        {
            // Peep is retrying the ride entrance without leaving
            // the path tile and without trying any other ride
            // attached to this path tile. i.e. stick with the
            // peeps previous decision not to go on the ride.
            peep_return_to_centre_of_tile(guest);
            return true;
        }

        guest->TimeLost = 0;
        auto stationNum = tile_element->AsEntrance()->GetStationIndex();
        // Guest walks up to the ride for the first time since entering
        // the path tile or since considering another ride attached to
        // the path tile.
        if (!guest->ShouldGoOnRide(ride, stationNum, false, false))
        {
            // Peep remembers that this is the last ride they
            // considered while on this path tile.
            guest->InteractionRideIndex = rideIndex;
            peep_return_to_centre_of_tile(guest);
            return true;
        }

        // Guest has decided to go on the ride.
        guest->ActionSpriteImageOffset = _unk_F1AEF0;
        guest->InteractionRideIndex = rideIndex;

        auto& station = ride->GetStation(stationNum);
        auto previous_last = station.LastPeepInQueue;
        station.LastPeepInQueue = guest->sprite_index;
        guest->GuestNextInQueue = previous_last;
        station.QueueLength++;

        guest->CurrentRide = rideIndex;
        guest->CurrentRideStation = stationNum;
        guest->DaysInQueue = 0;
        guest->SetState(PeepState::Queuing);
        guest->RideSubState = PeepRideSubState::AtQueueFront;
        guest->TimeInQueue = 0;
        if (guest->PeepFlags & PEEP_FLAGS_TRACKING)
        {
            auto ft = Formatter();
            guest->FormatNameTo(ft);
            ride->FormatNameTo(ft);
            if (gConfigNotifications.guest_queuing_for_ride)
            {
                News::AddItemToQueue(
                    News::ItemType::PeepOnRide, STR_PEEP_TRACKING_PEEP_JOINED_QUEUE_FOR_X, guest->sprite_index, ft);
            }
        }
    }
    else
    {
        // PARK_ENTRANCE
        auto* guest = peep->As<Guest>();
        if (guest == nullptr)
        {
            // Staff cannot leave the park, so go back.
            peep_return_to_centre_of_tile(peep);
            return true;
        }

        // If not the centre of the entrance arch
        if (tile_element->AsEntrance()->GetSequenceIndex() != 0)
        {
            peep_return_to_centre_of_tile(guest);
            return true;
        }

        uint8_t entranceDirection = tile_element->GetDirection();
        if (entranceDirection != guest->PeepDirection)
        {
            if (direction_reverse(entranceDirection) != guest->PeepDirection)
            {
                peep_return_to_centre_of_tile(guest);
                return true;
            }

            // Peep is leaving the park.
            if (guest->State != PeepState::Walking)
            {
                peep_return_to_centre_of_tile(guest);
                return true;
            }

            if (!(guest->PeepFlags & PEEP_FLAGS_LEAVING_PARK))
            {
                // If the park is open and leaving flag isn't set return to centre
                if (gParkFlags & PARK_FLAGS_PARK_OPEN)
                {
                    peep_return_to_centre_of_tile(guest);
                    return true;
                }
            }

            auto destination = guest->GetDestination() + CoordsDirectionDelta[guest->PeepDirection];
            guest->SetDestination(destination, 9);
            guest->MoveTo({ coords, guest->z });
            guest->SetState(PeepState::LeavingPark);

            guest->Var37 = 0;
            if (guest->PeepFlags & PEEP_FLAGS_TRACKING)
            {
                auto ft = Formatter();
                guest->FormatNameTo(ft);
                if (gConfigNotifications.guest_left_park)
                {
                    News::AddItemToQueue(News::ItemType::PeepOnRide, STR_PEEP_TRACKING_LEFT_PARK, guest->sprite_index, ft);
                }
            }
            return true;
        }

        // Peep is entering the park.

        if (guest->State != PeepState::EnteringPark)
        {
            peep_return_to_centre_of_tile(guest);
            return true;
        }

        if (!(gParkFlags & PARK_FLAGS_PARK_OPEN))
        {
            guest->State = PeepState::LeavingPark;
            guest->Var37 = 1;
            decrement_guests_heading_for_park();
            peep_window_state_update(guest);
            peep_return_to_centre_of_tile(guest);
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
                if (nextTileElement->GetType() != TileElementType::Path)
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

                if (z != nextTileElement->base_height)
                {
                    continue;
                }
                found = true;
                break;
            } while (!(nextTileElement++)->IsLastForTile());
        }

        if (!found)
        {
            guest->State = PeepState::LeavingPark;
            guest->Var37 = 1;
            decrement_guests_heading_for_park();
            peep_window_state_update(guest);
            peep_return_to_centre_of_tile(guest);
            return true;
        }

        money16 entranceFee = park_get_entrance_fee();
        if (entranceFee != 0)
        {
            if (guest->HasItem(ShopItem::Voucher))
            {
                if (guest->VoucherType == VOUCHER_TYPE_PARK_ENTRY_HALF_PRICE)
                {
                    entranceFee /= 2;
                    guest->RemoveItem(ShopItem::Voucher);
                    guest->WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                }
                else if (guest->VoucherType == VOUCHER_TYPE_PARK_ENTRY_FREE)
                {
                    entranceFee = 0;
                    guest->RemoveItem(ShopItem::Voucher);
                    guest->WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                }
            }
            if (entranceFee > guest->CashInPocket)
            {
                guest->State = PeepState::LeavingPark;
                guest->Var37 = 1;
                decrement_guests_heading_for_park();
                peep_window_state_update(guest);
                peep_return_to_centre_of_tile(guest);
                return true;
            }

            gTotalIncomeFromAdmissions += entranceFee;
            guest->SpendMoney(guest->PaidToEnter, entranceFee, ExpenditureType::ParkEntranceTickets);
            guest->PeepFlags |= PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY;
        }

        gTotalAdmissions++;
        window_invalidate_by_number(WindowClass::ParkInformation, 0);

        guest->Var37 = 1;
        auto destination = guest->GetDestination();
        destination += CoordsDirectionDelta[guest->PeepDirection];
        guest->SetDestination(destination, 7);
        guest->MoveTo({ coords, guest->z });
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
    CoordsXYZ coordsXYZ = { coords.ToTileStart(), tile_element->GetBaseZ() };
    if (footpath_is_blocked_by_vehicle(tile_element))
    {
        auto staff = peep->As<Staff>();
        if (staff == nullptr || !staff->IsMechanicHeadingToFixRideBlockingPath(coordsXYZ))
        {
            // Wait for vehicle to pass
            peep->Action = PeepActionType::Idle;
            return;
        }
    }

    peep->NextLoc = coordsXYZ;
    peep->SetNextFlags(tile_element->AsPath()->GetSlopeDirection(), tile_element->AsPath()->IsSloped(), false);

    int16_t z = peep->GetZOnSlope(coords.x, coords.y);

    auto* guest = peep->As<Guest>();
    if (guest == nullptr)
    {
        peep->MoveTo({ coords, z });
        return;
    }

    uint8_t vandalThoughtTimeout = (guest->VandalismSeen & 0xC0) >> 6;
    // Advance the vandalised tiles by 1
    uint8_t vandalisedTiles = (guest->VandalismSeen * 2) & 0x3F;

    if (vandalism)
    {
        // Add one more to the vandalised tiles
        vandalisedTiles |= 1;
        // If there has been 2 vandalised tiles in the last 6
        if (vandalisedTiles & 0x3E && (vandalThoughtTimeout == 0))
        {
            if ((scenario_rand() & 0xFFFF) <= 10922)
            {
                guest->InsertNewThought(PeepThoughtType::Vandalism);
                guest->HappinessTarget = std::max(0, guest->HappinessTarget - 17);
            }
            vandalThoughtTimeout = 3;
        }
    }

    if (vandalThoughtTimeout && (scenario_rand() & 0xFFFF) <= 4369)
    {
        vandalThoughtTimeout--;
    }

    guest->VandalismSeen = (vandalThoughtTimeout << 6) | vandalisedTiles;
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

            if (abs(other_peep->z - guest->NextLoc.z) > 16)
                continue;
            crowded++;
            continue;
        }

        if (auto litter = entity->As<Litter>(); litter != nullptr)
        {
            if (abs(litter->z - guest->NextLoc.z) > 16)
                continue;

            litter_count++;
            if (litter->SubType != Litter::Type::Vomit && litter->SubType != Litter::Type::VomitAlt)
                continue;

            litter_count--;
            sick_count++;
        }
    }

    if (crowded >= 10 && guest->State == PeepState::Walking && (scenario_rand() & 0xFFFF) <= 21845)
    {
        guest->InsertNewThought(PeepThoughtType::Crowded);
        guest->HappinessTarget = std::max(0, guest->HappinessTarget - 14);
    }

    litter_count = std::min(static_cast<uint8_t>(3), litter_count);
    sick_count = std::min(static_cast<uint8_t>(3), sick_count);

    uint8_t disgusting_time = guest->DisgustingCount & 0xC0;
    uint8_t disgusting_count = ((guest->DisgustingCount & 0xF) << 2) | sick_count;
    guest->DisgustingCount = disgusting_count | disgusting_time;

    if (disgusting_time & 0xC0 && (scenario_rand() & 0xFFFF) <= 4369)
    {
        // Reduce the disgusting time
        guest->DisgustingCount -= 0x40;
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
            guest->InsertNewThought(PeepThoughtType::PathDisgusting);
            guest->HappinessTarget = std::max(0, guest->HappinessTarget - 17);
            // Reset disgusting time
            guest->DisgustingCount |= 0xC0;
        }
    }

    uint8_t litter_time = guest->LitterCount & 0xC0;
    litter_count = ((guest->LitterCount & 0xF) << 2) | litter_count;
    guest->LitterCount = litter_count | litter_time;

    if (litter_time & 0xC0 && (scenario_rand() & 0xFFFF) <= 4369)
    {
        // Reduce the litter time
        guest->LitterCount -= 0x40;
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
            guest->InsertNewThought(PeepThoughtType::BadLitter);
            guest->HappinessTarget = std::max(0, guest->HappinessTarget - 17);
            // Reset litter time
            guest->LitterCount |= 0xC0;
        }
    }

    guest->MoveTo({ coords, z });
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
    auto* guest = peep->As<Guest>();
    if (map_is_location_owned({ coords, z }))
    {
        if (guest != nullptr && guest->OutsideOfPark)
        {
            peep_return_to_centre_of_tile(guest);
            return;
        }
    }
    else
    {
        if (guest == nullptr || !guest->OutsideOfPark)
        {
            peep_return_to_centre_of_tile(peep);
            return;
        }
    }

    if (guest != nullptr && tile_element->AsPath()->IsQueue())
    {
        auto rideIndex = tile_element->AsPath()->GetRideIndex();
        if (guest->State == PeepState::Queuing)
        {
            // Check if this queue is connected to the ride the
            // peep is queuing for, i.e. the player hasn't edited
            // the queue, rebuilt the ride, etc.
            if (guest->CurrentRide == rideIndex)
            {
                peep_footpath_move_forward(guest, { coords, tile_element }, vandalism_present);
            }
            else
            {
                // Queue got disconnected from the original ride.
                guest->InteractionRideIndex = RideId::GetNull();
                guest->RemoveFromQueue();
                guest->SetState(PeepState::One);
                peep_footpath_move_forward(guest, { coords, tile_element }, vandalism_present);
            }
        }
        else
        {
            // Peep is not queuing.
            guest->TimeLost = 0;
            auto stationNum = tile_element->AsPath()->GetStationIndex();

            if ((tile_element->AsPath()->HasQueueBanner())
                && (tile_element->AsPath()->GetQueueBannerDirection()
                    == direction_reverse(guest->PeepDirection)) // Ride sign is facing the direction the peep is walking
            )
            {
                /* Peep is approaching the entrance of a ride queue.
                 * Decide whether to go on the ride. */
                auto ride = get_ride(rideIndex);
                if (ride != nullptr && guest->ShouldGoOnRide(ride, stationNum, true, false))
                {
                    // Peep has decided to go on the ride at the queue.
                    guest->InteractionRideIndex = rideIndex;

                    // Add the peep to the ride queue.
                    auto& station = ride->GetStation(stationNum);
                    auto old_last_peep = station.LastPeepInQueue;
                    station.LastPeepInQueue = guest->sprite_index;
                    guest->GuestNextInQueue = old_last_peep;
                    station.QueueLength++;

                    peep_decrement_num_riders(guest);
                    guest->CurrentRide = rideIndex;
                    guest->CurrentRideStation = stationNum;
                    guest->State = PeepState::Queuing;
                    guest->DaysInQueue = 0;
                    peep_window_state_update(guest);

                    guest->RideSubState = PeepRideSubState::InQueue;
                    guest->DestinationTolerance = 2;
                    guest->TimeInQueue = 0;
                    if (guest->PeepFlags & PEEP_FLAGS_TRACKING)
                    {
                        auto ft = Formatter();
                        guest->FormatNameTo(ft);
                        ride->FormatNameTo(ft);
                        if (gConfigNotifications.guest_queuing_for_ride)
                        {
                            News::AddItemToQueue(
                                News::ItemType::PeepOnRide, STR_PEEP_TRACKING_PEEP_JOINED_QUEUE_FOR_X, guest->sprite_index, ft);
                        }
                    }

                    peep_footpath_move_forward(guest, { coords, tile_element }, vandalism_present);
                }
                else
                {
                    // Peep has decided not to go on the ride.
                    peep_return_to_centre_of_tile(guest);
                }
            }
            else
            {
                /* Peep is approaching a queue tile without a ride
                 * sign facing the peep. */
                peep_footpath_move_forward(guest, { coords, tile_element }, vandalism_present);
            }
        }
    }
    else
    {
        peep->InteractionRideIndex = RideId::GetNull();
        if (guest != nullptr && peep->State == PeepState::Queuing)
        {
            guest->RemoveFromQueue();
            guest->SetState(PeepState::One);
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
    RideId rideIndex = coords.element->AsTrack()->GetRideIndex();
    auto ride = get_ride(rideIndex);
    if (ride == nullptr || !ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY))
        return false;

    auto* guest = peep->As<Guest>();
    if (guest == nullptr)
    {
        peep_return_to_centre_of_tile(peep);
        return true;
    }

    // If we are queuing ignore the 'shop'
    // This can happen when paths clip through track
    if (guest->State == PeepState::Queuing)
    {
        return false;
    }

    guest->TimeLost = 0;

    if (ride->status != RideStatus::Open)
    {
        peep_return_to_centre_of_tile(guest);
        return true;
    }

    if (guest->InteractionRideIndex == rideIndex)
    {
        peep_return_to_centre_of_tile(guest);
        return true;
    }

    if (guest->PeepFlags & PEEP_FLAGS_LEAVING_PARK)
    {
        peep_return_to_centre_of_tile(guest);
        return true;
    }

    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_PEEP_SHOULD_GO_INSIDE_FACILITY))
    {
        guest->TimeLost = 0;
        if (!guest->ShouldGoOnRide(ride, StationIndex::FromUnderlying(0), false, false))
        {
            peep_return_to_centre_of_tile(guest);
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
        guest->SetDestination(coordsCentre, 3);
        guest->CurrentRide = rideIndex;
        guest->SetState(PeepState::EnteringRide);
        guest->RideSubState = PeepRideSubState::ApproachShop;

        guest->GuestTimeOnRide = 0;
        ride->cur_num_customers++;
        if (guest->PeepFlags & PEEP_FLAGS_TRACKING)
        {
            auto ft = Formatter();
            guest->FormatNameTo(ft);
            ride->FormatNameTo(ft);
            StringId string_id = ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IN_RIDE) ? STR_PEEP_TRACKING_PEEP_IS_IN_X
                                                                                               : STR_PEEP_TRACKING_PEEP_IS_ON_X;
            if (gConfigNotifications.guest_used_facility)
            {
                News::AddItemToQueue(News::ItemType::PeepOnRide, string_id, guest->sprite_index, ft);
            }
        }
    }
    else
    {
        if (guest->GuestHeadingToRideId == rideIndex)
            guest->GuestHeadingToRideId = RideId::GetNull();
        guest->ActionSpriteImageOffset = _unk_F1AEF0;
        guest->SetState(PeepState::Buying);
        guest->CurrentRide = rideIndex;
        guest->SubState = 0;
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

    if (Action == PeepActionType::Idle)
        Action = PeepActionType::Walking;

    auto* guest = As<Guest>();
    if (State == PeepState::Queuing && guest != nullptr)
    {
        if (guest->UpdateQueuePosition(previousAction))
            return;
    }

    std::optional<CoordsXY> loc;
    if (loc = UpdateAction(); !loc.has_value())
    {
        pathing_result |= PATHING_DESTINATION_REACHED;
        uint8_t result = 0;

        if (guest != nullptr)
        {
            result = gGuestPathfinder->CalculateNextDestination(*guest);
        }
        else
        {
            auto* staff = As<Staff>();
            result = staff->DoPathFinding();
        }

        if (result != 0)
            return;

        if (loc = UpdateAction(); !loc.has_value())
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
        if (guest != nullptr && guest->OutsideOfPark)
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

        if (tileElement->GetType() == TileElementType::Path)
        {
            peep_interact_with_path(this, { newLoc, tileElement });
            tile_result = tileElement;
            return;
        }

        if (tileElement->GetType() == TileElementType::Track)
        {
            if (peep_interact_with_shop(this, { newLoc, tileElement }))
            {
                tile_result = tileElement;
                return;
            }
        }
        else if (tileElement->GetType() == TileElementType::Entrance)
        {
            if (peep_interact_with_entrance(this, { newLoc, tileElement }, pathing_result))
            {
                tile_result = tileElement;
                return;
            }
        }
    } while (!(tileElement++)->IsLastForTile());

    if (Is<Staff>() || (GetNextIsSurface()))
    {
        int16_t height = abs(tile_element_height(newLoc) - z);
        if (height <= 3 || (Is<Staff>() && height <= 32))
        {
            InteractionRideIndex = RideId::GetNull();
            if (guest != nullptr && State == PeepState::Queuing)
            {
                guest->RemoveFromQueue();
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

            auto* staff = As<Staff>();
            if (staff != nullptr && !GetNextIsSurface())
            {
                // Prevent staff from leaving the path on their own unless they're allowed to mow.
                if (!((staff->StaffOrders & STAFF_ORDERS_MOWING) && staff->StaffMowingTimeout >= 12))
                {
                    peep_return_to_centre_of_tile(staff);
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

StringId get_real_name_string_id_from_id(uint32_t id)
{
    // Generate a name_string_idx from the peep Id using bit twiddling
    uint16_t ax = static_cast<uint16_t>(id + 0xF0B);
    uint16_t dx = 0;
    static constexpr uint16_t twiddlingBitOrder[] = {
        4, 9, 3, 7, 5, 8, 2, 1, 6, 0, 12, 11, 13, 10,
    };
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

int32_t peep_compare(const EntityId sprite_index_a, const EntityId sprite_index_b)
{
    Peep const* peep_a = GetEntity<Peep>(sprite_index_a);
    Peep const* peep_b = GetEntity<Peep>(sprite_index_b);
    if (peep_a == nullptr || peep_b == nullptr)
    {
        return 0;
    }

    // Compare types
    if (peep_a->Type != peep_b->Type)
    {
        return static_cast<int32_t>(peep_a->Type) - static_cast<int32_t>(peep_b->Type);
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
            Balloon::Create({ peep->x, peep->y, spawn_height }, peep->BalloonColour, false);
            peep->WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
            peep->UpdateSpriteType();
        }
    }
}

/**
 *
 *  rct2: 0x0069A512
 */
void Peep::RemoveFromRide()
{
    auto* guest = As<Guest>();
    if (guest != nullptr && State == PeepState::Queuing)
    {
        guest->RemoveFromQueue();
    }
    StateReset();
}

void Peep::SetDestination(const CoordsXY& coords)
{
    DestinationX = static_cast<uint16_t>(coords.x);
    DestinationY = static_cast<uint16_t>(coords.y);
}

void Peep::SetDestination(const CoordsXY& coords, int32_t tolerance)
{
    SetDestination(coords);
    DestinationTolerance = tolerance;
}

CoordsXY Peep::GetDestination() const
{
    return CoordsXY{ DestinationX, DestinationY };
}

void Peep::Serialise(DataSerialiser& stream)
{
    EntityBase::Serialise(stream);
    if (stream.IsLoading())
    {
        Name = nullptr;
    }
    stream << NextLoc;
    stream << NextFlags;
    stream << State;
    stream << SubState;
    stream << SpriteType;
    stream << TshirtColour;
    stream << TrousersColour;
    stream << DestinationX;
    stream << DestinationY;
    stream << DestinationTolerance;
    stream << Var37;
    stream << Energy;
    stream << EnergyTarget;
    stream << Mass;
    // stream << base.WindowInvalidateFlags;
    stream << CurrentRide;
    stream << CurrentRideStation;
    stream << CurrentTrain;
    stream << CurrentCar;
    stream << CurrentSeat;
    stream << SpecialSprite;
    stream << ActionSpriteType;
    stream << NextActionSpriteType;
    stream << ActionSpriteImageOffset;
    stream << Action;
    stream << ActionFrame;
    stream << StepProgress;
    stream << PeepDirection;
    stream << InteractionRideIndex;
    stream << Id;
    stream << PathCheckOptimisation;
    stream << PathfindGoal;
    stream << PathfindHistory;
    stream << WalkingFrameNum;
    stream << PeepFlags;
}

void Peep::Paint(paint_session& session, int32_t imageDirection) const
{
    PROFILED_FUNCTION();

    if (lightfx_is_available())
    {
        if (Is<Staff>())
        {
            auto loc = GetLocation();
            switch (sprite_direction)
            {
                case 0:
                    loc.x -= 10;
                    break;
                case 8:
                    loc.y += 10;
                    break;
                case 16:
                    loc.x += 10;
                    break;
                case 24:
                    loc.y -= 10;
                    break;
                default:
                    return;
            }

            LightfxAdd3DLight(*this, 0, loc, LightType::Spot1);
        }
    }

    rct_drawpixelinfo* dpi = &session.DPI;
    if (dpi->zoom_level > ZoomLevel{ 2 })
    {
        return;
    }

    PeepActionSpriteType actionSpriteType = ActionSpriteType;
    uint8_t imageOffset = ActionSpriteImageOffset;

    if (Action == PeepActionType::Idle)
    {
        actionSpriteType = NextActionSpriteType;
        imageOffset = 0;
    }

    // In the following 4 calls to PaintAddImageAsParent/PaintAddImageAsChild, we add 5 (instead of 3) to the
    //  bound_box_offset_z to make sure peeps are drawn on top of railways
    uint32_t baseImageId = (imageDirection >> 3) + GetPeepAnimation(SpriteType, actionSpriteType).base_image + imageOffset * 4;
    uint32_t imageId = baseImageId | TshirtColour << 19 | TrousersColour << 24 | IMAGE_TYPE_REMAP | IMAGE_TYPE_REMAP_2_PLUS;
    PaintAddImageAsParent(session, imageId, { 0, 0, z }, { 1, 1, 11 }, { 0, 0, z + 5 });

    auto* guest = As<Guest>();
    if (guest != nullptr)
    {
        if (baseImageId >= 10717 && baseImageId < 10749)
        {
            imageId = (baseImageId + 32) | guest->HatColour << 19 | IMAGE_TYPE_REMAP;
            PaintAddImageAsChild(session, imageId, { 0, 0, z }, { 1, 1, 11 }, { 0, 0, z + 5 });
            return;
        }

        if (baseImageId >= 10781 && baseImageId < 10813)
        {
            imageId = (baseImageId + 32) | guest->BalloonColour << 19 | IMAGE_TYPE_REMAP;
            PaintAddImageAsChild(session, imageId, { 0, 0, z }, { 1, 1, 11 }, { 0, 0, z + 5 });
            return;
        }

        if (baseImageId >= 11197 && baseImageId < 11229)
        {
            imageId = (baseImageId + 32) | guest->UmbrellaColour << 19 | IMAGE_TYPE_REMAP;
            PaintAddImageAsChild(session, imageId, { 0, 0, z }, { 1, 1, 11 }, { 0, 0, z + 5 });
            return;
        }
    }
}
