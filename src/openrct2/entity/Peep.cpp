/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Peep.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Diagnostic.h"
#include "../Game.h"
#include "../GameState.h"
#include "../Input.h"
#include "../OpenRCT2.h"
#include "../SpriteIds.h"
#include "../actions/GameAction.h"
#include "../audio/Audio.h"
#include "../audio/AudioChannel.h"
#include "../audio/AudioMixer.h"
#include "../config/Config.h"
#include "../core/EnumUtils.hpp"
#include "../core/Guard.hpp"
#include "../core/String.hpp"
#include "../drawing/LightFX.h"
#include "../entity/Balloon.h"
#include "../entity/EntityList.h"
#include "../entity/EntityRegistry.h"
#include "../entity/EntityTweener.h"
#include "../interface/Viewport.h"
#include "../interface/WindowBase.h"
#include "../localisation/Formatter.h"
#include "../localisation/Formatting.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../network/Network.h"
#include "../object/ObjectManager.h"
#include "../object/PeepAnimationsObject.h"
#include "../paint/Paint.h"
#include "../peep/GuestPathfinding.h"
#include "../peep/PeepSpriteIds.h"
#include "../profiling/Profiling.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/ShopItem.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../scenario/Scenario.h"
#include "../ui/WindowManager.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Climate.h"
#include "../world/ConstructionClearance.h"
#include "../world/Entrance.h"
#include "../world/Footpath.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/tile_element/EntranceElement.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TrackElement.h"
#include "PatrolArea.h"
#include "Staff.h"

#include <cassert>
#include <iterator>
#include <limits>
#include <map>
#include <memory>
#include <optional>

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;
using namespace OpenRCT2::Drawing;

using OpenRCT2::Drawing::LightFx::LightType;

static uint8_t _backupAnimationImageIdOffset;
static TileElement* _peepRideEntranceExitElement;

static std::shared_ptr<IAudioChannel> _crowdSoundChannel = nullptr;

static void GuestReleaseBalloon(Guest* peep, int16_t spawn_height);

static PeepAnimationType PeepSpecialSpriteToAnimationGroupMap[] = {
    PeepAnimationType::walking,
    PeepAnimationType::holdMat,
    PeepAnimationType::staffMower,
};

static PeepAnimationType PeepActionToAnimationGroupMap[] = {
    PeepAnimationType::checkTime,
    PeepAnimationType::eatFood,
    PeepAnimationType::shakeHead,
    PeepAnimationType::emptyPockets,
    PeepAnimationType::sittingEatFood,
    PeepAnimationType::sittingLookAroundLeft,
    PeepAnimationType::sittingLookAroundRight,
    PeepAnimationType::wow,
    PeepAnimationType::throwUp,
    PeepAnimationType::jump,
    PeepAnimationType::staffSweep,
    PeepAnimationType::drowning,
    PeepAnimationType::staffAnswerCall,
    PeepAnimationType::staffAnswerCall2,
    PeepAnimationType::staffCheckBoard,
    PeepAnimationType::staffFix,
    PeepAnimationType::staffFix2,
    PeepAnimationType::staffFixGround,
    PeepAnimationType::staffFix3,
    PeepAnimationType::staffWatering,
    PeepAnimationType::joy,
    PeepAnimationType::readMap,
    PeepAnimationType::wave,
    PeepAnimationType::staffEmptyBin,
    PeepAnimationType::wave2,
    PeepAnimationType::takePhoto,
    PeepAnimationType::clap,
    PeepAnimationType::disgust,
    PeepAnimationType::drawPicture,
    PeepAnimationType::beingWatched,
    PeepAnimationType::withdrawMoney,
};

template<>
bool EntityBase::Is<Peep>() const
{
    return Type == EntityType::guest || Type == EntityType::staff;
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
        case PeepState::one:
        case PeepState::queuingFront:
        case PeepState::onRide:
        case PeepState::enteringRide:
        case PeepState::leavingRide:
        case PeepState::enteringPark:
        case PeepState::leavingPark:
        case PeepState::fixing:
        case PeepState::buying:
        case PeepState::inspecting:
            return false;
        case PeepState::falling:
        case PeepState::walking:
        case PeepState::queuing:
        case PeepState::sitting:
        case PeepState::picked:
        case PeepState::patrolling:
        case PeepState::mowing:
        case PeepState::sweeping:
        case PeepState::answering:
        case PeepState::watching:
        case PeepState::emptyingBin:
        case PeepState::usingBin:
        case PeepState::watering:
        case PeepState::headingToInspection:
            return true;
    }
    return false;
}

int32_t PeepGetStaffCount()
{
    return getGameState().entities.GetEntityListCount(EntityType::staff);
}

/**
 *
 *  rct2: 0x0068F0A9
 */
void PeepUpdateAll()
{
    PROFILED_FUNCTION();

    if (isInEditorMode())
        return;

    const auto currentTicks = getGameState().currentTicks;

    constexpr auto kTicks128Mask = 128u - 1u;
    const auto currentTicksMasked = currentTicks & kTicks128Mask;

    uint32_t index = 0;
    // Warning this loop can delete peeps
    for (auto peep : EntityList<Guest>())
    {
        if ((index & kTicks128Mask) == currentTicksMasked)
        {
            peep->Tick128UpdateGuest(index);
        }

        // 128 tick can delete so double check its not deleted
        if (peep->Type == EntityType::guest)
        {
            peep->Update();
        }

        index++;
    }

    for (auto staff : EntityList<Staff>())
    {
        if ((index & kTicks128Mask) == currentTicksMasked)
        {
            staff->Tick128UpdateStaff();
        }

        // 128 tick can delete so double check its not deleted
        if (staff->Type == EntityType::staff)
        {
            staff->Update();
        }

        index++;
    }
}

void PeepUpdateAllBoundingBoxes()
{
    for (auto* peep : EntityList<Guest>())
    {
        peep->UpdateSpriteBoundingBox();
    }

    for (auto* peep : EntityList<Staff>())
    {
        peep->UpdateSpriteBoundingBox();
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
    if ((PathCheckOptimisation & 0xF) != (Id.ToUnderlying() & 0xF))
    {
        // This condition makes the check happen less often
        // As a side effect peeps hover for a short,
        // random time when a path below them has been deleted
        return true;
    }

    TileElement* tile_element = MapGetFirstElementAt(NextLoc);

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
    SetState(PeepState::falling);
    return false;
}

bool Peep::ShouldWaitForLevelCrossing() const
{
    if (IsOnPathBlockedByVehicle())
    {
        // Try to get out of the way
        return false;
    }

    auto curPos = TileCoordsXYZ(GetLocation());
    auto dstPos = TileCoordsXYZ(CoordsXYZ{ GetDestination(), NextLoc.z });
    if ((curPos.x != dstPos.x || curPos.y != dstPos.y) && FootpathIsBlockedByVehicle(dstPos))
    {
        return true;
    }

    return false;
}

bool Peep::IsOnLevelCrossing() const
{
    auto loc = GetLocation();
    auto pathElement = MapGetFootpathElement(loc);
    if (pathElement != nullptr)
    {
        return pathElement->AsPath()->IsLevelCrossing(loc);
    }
    return false;
}

bool Peep::IsOnPathBlockedByVehicle() const
{
    auto curPos = TileCoordsXYZ(GetLocation());
    return FootpathIsBlockedByVehicle(curPos);
}

PeepAnimationType Peep::GetAnimationType()
{
    if (IsActionInterruptable())
    { // PeepActionType::none1 or PeepActionType::none2
        return PeepSpecialSpriteToAnimationGroupMap[SpecialSprite];
    }

    if (EnumValue(Action) < std::size(PeepActionToAnimationGroupMap))
    {
        return PeepActionToAnimationGroupMap[EnumValue(Action)];
    }

    Guard::Assert(
        EnumValue(Action) >= std::size(PeepActionToAnimationGroupMap) && Action < PeepActionType::idle,
        "Invalid peep action %u", EnumValue(Action));
    return PeepAnimationType::walking;
}

/*
 *  rct2: 0x00693B58
 */
void Peep::UpdateCurrentAnimationType()
{
    PeepAnimationType newAnimationType = GetAnimationType();
    if (AnimationType == newAnimationType)
    {
        return;
    }

    AnimationType = newAnimationType;

    Invalidate();
    UpdateSpriteBoundingBox();
    Invalidate();
}

void Peep::UpdateSpriteBoundingBox()
{
    auto& objManager = GetContext()->GetObjectManager();
    auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(AnimationObjectIndex);

    const auto& spriteBounds = animObj->GetSpriteBounds(AnimationGroup, AnimationType);
    SpriteData.Width = spriteBounds.spriteWidth;
    SpriteData.HeightMin = spriteBounds.spriteHeightNegative;
    SpriteData.HeightMax = spriteBounds.spriteHeightPositive;
}

/* rct2: 0x00693BE5 */
void Peep::SwitchToSpecialSprite(uint8_t special_sprite_id)
{
    if (special_sprite_id == SpecialSprite)
        return;

    SpecialSprite = special_sprite_id;

    if (IsActionInterruptable())
    {
        AnimationImageIdOffset = 0;
    }
    UpdateCurrentAnimationType();
}

void Peep::StateReset()
{
    SetState(PeepState::one);
    SwitchToSpecialSprite(0);
}

/** rct2: 0x00981D7C, 0x00981D7E */
static constexpr CoordsXY kWalkingOffsetByDirection[kNumOrthogonalDirections] = {
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

    _backupAnimationImageIdOffset = AnimationImageIdOffset;
    if (Action == PeepActionType::idle)
    {
        Action = PeepActionType::walking;
    }

    CoordsXY differenceLoc = GetLocation();
    differenceLoc -= GetDestination();

    int32_t x_delta = abs(differenceLoc.x);
    int32_t y_delta = abs(differenceLoc.y);

    xy_distance = x_delta + y_delta;

    // We're taking an easier route if we're just walking
    if (IsActionWalking())
    {
        return UpdateWalkingAction(differenceLoc, xy_distance);
    }

    if (!UpdateActionAnimation())
    {
        AnimationImageIdOffset = 0;
        Action = PeepActionType::walking;
        UpdateCurrentAnimationType();
        return { { x, y } };
    }

    // Should we throw up, and are we at the frame where sick appears?
    if (auto* guest = As<Guest>(); guest != nullptr)
    {
        if (Action == PeepActionType::throwUp && AnimationFrameNum == 15)
        {
            guest->ThrowUp();
        }
    }

    return { { x, y } };
}

bool Peep::UpdateActionAnimation()
{
    auto& objManager = GetContext()->GetObjectManager();
    auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(AnimationObjectIndex);

    const PeepAnimation& peepAnimation = animObj->GetPeepAnimation(AnimationGroup, AnimationType);
    AnimationFrameNum++;

    // If last frame of action
    if (AnimationFrameNum >= peepAnimation.frameOffsets.size())
    {
        return false;
    }

    AnimationImageIdOffset = peepAnimation.frameOffsets[AnimationFrameNum];
    return true;
}

std::optional<CoordsXY> Peep::UpdateWalkingAction(const CoordsXY& differenceLoc, int16_t& xy_distance)
{
    if (!IsActionWalking())
    {
        return std::nullopt;
    }

    if (xy_distance <= DestinationTolerance)
    {
        return std::nullopt;
    }

    int32_t x_delta = abs(differenceLoc.x);
    int32_t y_delta = abs(differenceLoc.y);

    int32_t nextDirection = 0;
    if (x_delta < y_delta)
    {
        nextDirection = 1;
        if (differenceLoc.y >= 0)
        {
            nextDirection = 3;
        }
    }
    else
    {
        nextDirection = 2;
        if (differenceLoc.x >= 0)
        {
            nextDirection = 0;
        }
    }

    Orientation = nextDirection * 8;

    CoordsXY loc = { x, y };
    loc += kWalkingOffsetByDirection[nextDirection];

    UpdateWalkingAnimation();

    return loc;
}

void Peep::UpdateWalkingAnimation()
{
    auto& objManager = GetContext()->GetObjectManager();
    auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(AnimationObjectIndex);

    WalkingAnimationFrameNum++;
    const PeepAnimation& peepAnimation = animObj->GetPeepAnimation(AnimationGroup, AnimationType);
    if (WalkingAnimationFrameNum >= peepAnimation.frameOffsets.size())
    {
        WalkingAnimationFrameNum = 0;
    }
    AnimationImageIdOffset = peepAnimation.frameOffsets[WalkingAnimationFrameNum];
}

/**
 *  rct2: 0x0069A409
 * Decreases rider count if on/entering a ride.
 */
void PeepDecrementNumRiders(Peep* peep)
{
    if (peep->State == PeepState::onRide || peep->State == PeepState::enteringRide)
    {
        auto ride = GetRide(peep->CurrentRide);
        if (ride != nullptr)
        {
            ride->numRiders = std::max(0, ride->numRiders - 1);
            ride->windowInvalidateFlags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
        }
    }
}

/**
 * Call after changing a peeps state to insure that all relevant windows update.
 * Note also increase ride count if on/entering a ride.
 *  rct2: 0x0069A42F
 */
void PeepWindowStateUpdate(Peep* peep)
{
    auto* windowMgr = Ui::GetWindowManager();
    WindowBase* w = windowMgr->FindByNumber(WindowClass::peep, peep->Id.ToUnderlying());
    if (w != nullptr)
        w->onPrepareDraw();

    if (peep->Is<Guest>())
    {
        if (peep->State == PeepState::onRide || peep->State == PeepState::enteringRide)
        {
            auto ride = GetRide(peep->CurrentRide);
            if (ride != nullptr)
            {
                ride->numRiders++;
                ride->windowInvalidateFlags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
            }
        }

        windowMgr->InvalidateByNumber(WindowClass::peep, peep->Id);
        windowMgr->InvalidateByClass(WindowClass::guestList);
    }
    else
    {
        windowMgr->InvalidateByNumber(WindowClass::peep, peep->Id);
        windowMgr->InvalidateByClass(WindowClass::staffList);
    }
}

void Peep::Pickup()
{
    if (auto* guest = As<Guest>(); guest != nullptr)
    {
        guest->RemoveFromRide();
    }
    MoveTo({ kLocationNull, y, z });
    SetState(PeepState::picked);
    SubState = 0;
}

void Peep::PickupAbort(int32_t old_x)
{
    if (State != PeepState::picked)
        return;

    MoveTo({ old_x, y, z + 8 });

    if (x != kLocationNull)
    {
        SetState(PeepState::falling);
        Action = PeepActionType::walking;
        SpecialSprite = 0;
        AnimationImageIdOffset = 0;
        AnimationType = PeepAnimationType::walking;
        PathCheckOptimisation = 0;
    }

    gPickupPeepImage = ImageId();
}

// Returns GameActions::Status::OK when a peep can be dropped at the given location. When apply is set to true the peep gets
// dropped.
GameActions::Result Peep::Place(const TileCoordsXYZ& location, bool apply)
{
    auto* pathElement = MapGetPathElementAt(location);
    TileElement* tileElement = reinterpret_cast<TileElement*>(pathElement);
    if (pathElement == nullptr)
    {
        tileElement = reinterpret_cast<TileElement*>(MapGetSurfaceElementAt(location));
    }
    if (tileElement == nullptr)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE, kStringIdNone);
    }

    // Set the coordinate of destination to be exactly
    // in the middle of a tile.
    CoordsXYZ destination = { location.ToCoordsXY().ToTileCentre(), tileElement->GetBaseZ() + 16 };

    if (!MapIsLocationOwned(destination))
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
        SetState(PeepState::falling);
        Action = PeepActionType::walking;
        SpecialSprite = 0;
        AnimationImageIdOffset = 0;
        AnimationType = PeepAnimationType::walking;
        PathCheckOptimisation = 0;
        EntityTweener::Get().Reset();
        if (auto* guest = As<Guest>(); guest != nullptr)
        {
            AnimationType = PeepAnimationType::invalid;
            guest->HappinessTarget = std::max(guest->HappinessTarget - 10, 0);
            UpdateCurrentAnimationType();
        }
    }

    return GameActions::Result();
}

/**
 *
 *  rct2: 0x0069A535
 */
void PeepEntityRemove(Peep* peep)
{
    auto* guest = peep->As<Guest>();
    if (guest != nullptr)
    {
        guest->RemoveFromRide();
    }
    peep->Invalidate();

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->CloseByNumber(WindowClass::peep, peep->Id);
    windowMgr->CloseByNumber(WindowClass::firePrompt, EnumValue(peep->Type));

    auto* staff = peep->As<Staff>();
    // Needed for invalidations after sprite removal
    bool wasGuest = staff == nullptr;
    if (wasGuest)
    {
        News::DisableNewsItems(News::ItemType::peepOnRide, peep->Id.ToUnderlying());
    }
    else
    {
        staff->ClearPatrolArea();
        UpdateConsolidatedPatrolAreas();

        News::DisableNewsItems(News::ItemType::peep, staff->Id.ToUnderlying());
    }
    getGameState().entities.EntityRemove(peep);

    auto intent = Intent(wasGuest ? INTENT_ACTION_REFRESH_GUEST_LIST : INTENT_ACTION_REFRESH_STAFF_LIST);
    ContextBroadcastIntent(&intent);
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
            DecrementGuestsInPark();
            auto intent = Intent(INTENT_ACTION_UPDATE_GUEST_COUNT);
            ContextBroadcastIntent(&intent);
        }
        if (State == PeepState::enteringPark)
        {
            DecrementGuestsHeadingForPark();
        }
    }
    PeepEntityRemove(this);
}

/**
 * Falling and its subset drowning
 *  rct2: 0x690028
 */
void Peep::UpdateFalling()
{
    if (Action == PeepActionType::drowning)
    {
        // Check to see if we are ready to drown.
        UpdateAction();
        Invalidate();
        if (Action == PeepActionType::drowning)
            return;

        if (Config::Get().notifications.GuestDied)
        {
            auto ft = Formatter();
            FormatNameTo(ft);
            News::AddItemToQueue(News::ItemType::blank, STR_NEWS_ITEM_GUEST_DROWNED, x | (y << 16), ft);
        }

        auto& gameState = getGameState();
        gameState.park.ratingCasualtyPenalty = std::min(gameState.park.ratingCasualtyPenalty + 25, 1000);
        Remove();
        return;
    }

    // If not drowning then falling. Note: peeps 'fall' after leaving a ride/enter the park.
    TileElement* tile_element = MapGetFirstElementAt(CoordsXY{ x, y });
    TileElement* saved_map = nullptr;
    int32_t saved_height = 0;

    if (tile_element != nullptr)
    {
        do
        {
            // If a path check if we are on it
            if (tile_element->GetType() == TileElementType::Path)
            {
                int32_t height = MapHeightFromSlope(
                                     { x, y }, tile_element->AsPath()->GetSlopeDirection(), tile_element->AsPath()->IsSloped())
                    + tile_element->GetBaseZ();

                if (height < z - 1 || height > z + 8)
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

                        if (auto* guest = As<Guest>(); guest != nullptr)
                        {
                            // Drop balloon if held
                            GuestReleaseBalloon(guest, height);
                            guest->InsertNewThought(PeepThoughtType::Drowning);
                        }

                        Action = PeepActionType::drowning;
                        AnimationFrameNum = 0;
                        AnimationImageIdOffset = 0;

                        UpdateCurrentAnimationType();
                        PeepWindowStateUpdate(this);
                        return;
                    }
                }
                int32_t map_height = TileElementHeight({ x, y });
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
    SetState(PeepState::one);
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
        SetState(PeepState::walking);
    }
    else
    {
        SetState(PeepState::patrolling);
    }

    SetDestination(GetLocation(), 10);
    PeepDirection = Orientation >> 3;
}

void Peep::SetState(PeepState new_state)
{
    PeepDecrementNumRiders(this);
    State = new_state;
    PeepWindowStateUpdate(this);
}

/**
 *
 *  rct2: 0x690009
 */
void Peep::UpdatePicked()
{
    if (getGameState().currentTicks & 0x1F)
        return;
    SubState++;
    auto* guest = As<Guest>();
    if (SubState == 13 && guest != nullptr)
    {
        guest->InsertNewThought(PeepThoughtType::Help);
    }
}

uint32_t Peep::GetStepsToTake() const
{
    uint32_t stepsToTake = Energy;
    if (stepsToTake < 95 && State == PeepState::queuing)
        stepsToTake = 95;
    if ((PeepFlags & PEEP_FLAGS_SLOW_WALK) && State != PeepState::queuing)
        stepsToTake /= 2;
    if (IsActionWalking() && GetNextIsSloped())
    {
        stepsToTake /= 2;
        if (State == PeepState::queuing)
            stepsToTake += stepsToTake / 2;
    }
    // Ensure guests make it across a level crossing in time
    constexpr auto minStepsForCrossing = 55;
    if (stepsToTake < minStepsForCrossing && IsOnPathBlockedByVehicle())
        stepsToTake = minStepsForCrossing;

    return stepsToTake;
}

/**
 *
 *  rct2: 0x0069BF41
 */
void PeepProblemWarningsUpdate()
{
    auto& gameState = getGameState();

    Ride* ride;
    uint32_t hungerCounter = 0, lostCounter = 0, noexitCounter = 0, thirstCounter = 0, litterCounter = 0, disgustCounter = 0,
             toiletCounter = 0, vandalismCounter = 0;
    uint8_t* warningThrottle = gameState.park.peepWarningThrottle;

    int32_t inQueueCounter = 0;
    int32_t tooLongQueueCounter = 0;
    std::map<RideId, int32_t> queueComplainingGuestsMap;

    for (auto peep : EntityList<Guest>())
    {
        if (peep->OutsideOfPark)
            continue;

        if (peep->State == PeepState::queuing || peep->State == PeepState::queuingFront)
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
                ride = GetRide(peep->GuestHeadingToRideId);
                if (ride != nullptr && !ride->getRideTypeDescriptor().HasFlag(RtdFlag::sellsFood))
                    hungerCounter++;
                break;

            case PeepThoughtType::Thirsty:
                if (peep->GuestHeadingToRideId.IsNull())
                {
                    thirstCounter++;
                    break;
                }
                ride = GetRide(peep->GuestHeadingToRideId);
                if (ride != nullptr && !ride->getRideTypeDescriptor().HasFlag(RtdFlag::sellsDrinks))
                    thirstCounter++;
                break;

            case PeepThoughtType::Toilet:
                if (peep->GuestHeadingToRideId.IsNull())
                {
                    toiletCounter++;
                    break;
                }
                ride = GetRide(peep->GuestHeadingToRideId);
                if (ride != nullptr && ride->getRideTypeDescriptor().specialType != RtdSpecialType::toilet)
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
    else if (hungerCounter >= kPeepHungerWarningThreshold && hungerCounter >= gameState.park.numGuestsInPark / 16)
    {
        warningThrottle[0] = 4;
        if (Config::Get().notifications.GuestWarnings)
        {
            constexpr auto thoughtId = static_cast<uint32_t>(PeepThoughtType::Hungry);
            News::AddItemToQueue(News::ItemType::peeps, STR_PEEPS_ARE_HUNGRY, thoughtId, {});
        }
    }

    if (warningThrottle[1])
        --warningThrottle[1];
    else if (thirstCounter >= kPeepThirstWarningThreshold && thirstCounter >= gameState.park.numGuestsInPark / 16)
    {
        warningThrottle[1] = 4;
        if (Config::Get().notifications.GuestWarnings)
        {
            constexpr auto thoughtId = static_cast<uint32_t>(PeepThoughtType::Thirsty);
            News::AddItemToQueue(News::ItemType::peeps, STR_PEEPS_ARE_THIRSTY, thoughtId, {});
        }
    }

    if (warningThrottle[2])
        --warningThrottle[2];
    else if (toiletCounter >= kPeepToiletWarningThreshold && toiletCounter >= gameState.park.numGuestsInPark / 16)
    {
        warningThrottle[2] = 4;
        if (Config::Get().notifications.GuestWarnings)
        {
            constexpr auto thoughtId = static_cast<uint32_t>(PeepThoughtType::Toilet);
            News::AddItemToQueue(News::ItemType::peeps, STR_PEEPS_CANT_FIND_TOILET, thoughtId, {});
        }
    }

    if (warningThrottle[3])
        --warningThrottle[3];
    else if (litterCounter >= kPeepLitterWarningThreshold && litterCounter >= gameState.park.numGuestsInPark / 32)
    {
        warningThrottle[3] = 4;
        if (Config::Get().notifications.GuestWarnings)
        {
            constexpr auto thoughtId = static_cast<uint32_t>(PeepThoughtType::BadLitter);
            News::AddItemToQueue(News::ItemType::peeps, STR_PEEPS_DISLIKE_LITTER, thoughtId, {});
        }
    }

    if (warningThrottle[4])
        --warningThrottle[4];
    else if (disgustCounter >= kPeepDisgustWarningThreshold && disgustCounter >= gameState.park.numGuestsInPark / 32)
    {
        warningThrottle[4] = 4;
        if (Config::Get().notifications.GuestWarnings)
        {
            constexpr auto thoughtId = static_cast<uint32_t>(PeepThoughtType::PathDisgusting);
            News::AddItemToQueue(News::ItemType::peeps, STR_PEEPS_DISGUSTED_BY_PATHS, thoughtId, {});
        }
    }

    if (warningThrottle[5])
        --warningThrottle[5];
    else if (vandalismCounter >= kPeepVandalismWarningThreshold && vandalismCounter >= gameState.park.numGuestsInPark / 32)
    {
        warningThrottle[5] = 4;
        if (Config::Get().notifications.GuestWarnings)
        {
            constexpr auto thoughtId = static_cast<uint32_t>(PeepThoughtType::Vandalism);
            News::AddItemToQueue(News::ItemType::peeps, STR_PEEPS_DISLIKE_VANDALISM, thoughtId, {});
        }
    }

    if (warningThrottle[6])
        --warningThrottle[6];
    else if (noexitCounter >= kPeepNoExitWarningThreshold)
    {
        warningThrottle[6] = 4;
        if (Config::Get().notifications.GuestWarnings)
        {
            constexpr auto thoughtId = static_cast<uint32_t>(PeepThoughtType::CantFindExit);
            News::AddItemToQueue(News::ItemType::peeps, STR_PEEPS_GETTING_LOST_OR_STUCK, thoughtId, {});
        }
    }
    else if (lostCounter >= kPeepLostWarningThreshold)
    {
        warningThrottle[6] = 4;
        if (Config::Get().notifications.GuestWarnings)
        {
            constexpr auto thoughtId = static_cast<uint32_t>(PeepThoughtType::Lost);
            News::AddItemToQueue(News::ItemType::peeps, STR_PEEPS_GETTING_LOST_OR_STUCK, thoughtId, {});
        }
    }

    if (warningThrottle[7])
        --warningThrottle[7];
    else if (tooLongQueueCounter > kPeepTooLongQueueThreshold && tooLongQueueCounter > inQueueCounter / 20)
    { // The amount of guests complaining about queue duration is at least 5% of the amount of queuing guests.
      // This includes guests who are no longer queuing.
        warningThrottle[7] = 4;
        if (Config::Get().notifications.GuestWarnings)
        {
            auto rideWithMostQueueComplaints = std::max_element(
                queueComplainingGuestsMap.begin(), queueComplainingGuestsMap.end(),
                [](auto& lhs, auto& rhs) { return lhs.second < rhs.second; });
            auto rideId = rideWithMostQueueComplaints->first.ToUnderlying();
            News::AddItemToQueue(News::ItemType::ride, STR_PEEPS_COMPLAINING_ABOUT_QUEUE_LENGTH_WARNING, rideId, {});
        }
    }
}

void PeepStopCrowdNoise()
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
void PeepUpdateCrowdNoise()
{
    PROFILED_FUNCTION();

    if (OpenRCT2::Audio::gGameSoundsOff)
        return;

    if (!Config::Get().sound.SoundEnabled)
        return;

    if (gLegacyScene == LegacyScene::scenarioEditor)
        return;

    auto viewport = gMusicTrackingViewport;
    if (viewport == nullptr)
        return;

    // Count the number of peeps visible
    auto visiblePeeps = 0;

    for (auto peep : EntityList<Guest>())
    {
        if (peep->x == kLocationNull)
            continue;
        if (viewport->viewPos.x > peep->SpriteData.SpriteRect.GetRight())
            continue;
        if (viewport->viewPos.x + viewport->ViewWidth() < peep->SpriteData.SpriteRect.GetLeft())
            continue;
        if (viewport->viewPos.y > peep->SpriteData.SpriteRect.GetBottom())
            continue;
        if (viewport->viewPos.y + viewport->ViewHeight() < peep->SpriteData.SpriteRect.GetTop())
            continue;

        visiblePeeps += peep->State == PeepState::queuing ? 1 : 2;
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
            _crowdSoundChannel = CreateAudioChannel(SoundId::crowdAmbience, true, 0);
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
void PeepApplause()
{
    for (auto peep : EntityList<Guest>())
    {
        if (peep->OutsideOfPark)
            continue;

        // Release balloon
        GuestReleaseBalloon(peep, peep->z + 9);

        // Clap
        if ((peep->State == PeepState::walking || peep->State == PeepState::queuing) && peep->IsActionInterruptableSafely())
        {
            peep->Action = PeepActionType::clap;
            peep->AnimationFrameNum = 0;
            peep->AnimationImageIdOffset = 0;
            peep->UpdateCurrentAnimationType();
        }
    }

    // Play applause noise
    OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::applause, 0, ContextGetWidth() / 2);
}

/**
 *
 *  rct2: 0x0069C35E
 */
void PeepUpdateDaysInQueue()
{
    for (auto peep : EntityList<Guest>())
    {
        if (!peep->OutsideOfPark && peep->State == PeepState::queuing)
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
        case PeepState::falling:
            ft.Add<StringId>(Action == PeepActionType::drowning ? STR_DROWNING : STR_WALKING);
            break;
        case PeepState::one:
            ft.Add<StringId>(STR_WALKING);
            break;
        case PeepState::onRide:
        case PeepState::leavingRide:
        case PeepState::enteringRide:
        {
            auto ride = GetRide(CurrentRide);
            if (ride != nullptr)
            {
                ft.Add<StringId>(ride->getRideTypeDescriptor().HasFlag(RtdFlag::describeAsInside) ? STR_IN_RIDE : STR_ON_RIDE);
                ride->formatNameTo(ft);
            }
            else
            {
                ft.Add<StringId>(STR_ON_RIDE).Add<StringId>(kStringIdNone);
            }
            break;
        }
        case PeepState::buying:
        {
            ft.Add<StringId>(STR_AT_RIDE);
            auto ride = GetRide(CurrentRide);
            if (ride != nullptr)
            {
                ride->formatNameTo(ft);
            }
            else
            {
                ft.Add<StringId>(kStringIdNone);
            }
            break;
        }
        case PeepState::walking:
        case PeepState::usingBin:
        {
            if (auto* guest = As<Guest>(); guest != nullptr)
            {
                if (!guest->GuestHeadingToRideId.IsNull())
                {
                    auto ride = GetRide(guest->GuestHeadingToRideId);
                    if (ride != nullptr)
                    {
                        ft.Add<StringId>(STR_HEADING_FOR);
                        ride->formatNameTo(ft);
                    }
                }
                else
                {
                    ft.Add<StringId>((PeepFlags & PEEP_FLAGS_LEAVING_PARK) ? STR_LEAVING_PARK : STR_WALKING);
                }
            }
            break;
        }
        case PeepState::queuingFront:
        case PeepState::queuing:
        {
            auto ride = GetRide(CurrentRide);
            if (ride != nullptr)
            {
                ft.Add<StringId>(STR_QUEUING_FOR);
                ride->formatNameTo(ft);
            }
            break;
        }
        case PeepState::sitting:
            ft.Add<StringId>(STR_SITTING);
            break;
        case PeepState::watching:
            if (!CurrentRide.IsNull())
            {
                auto ride = GetRide(CurrentRide);
                if (ride != nullptr)
                {
                    ft.Add<StringId>((StandingFlags & 0x1) ? STR_WATCHING_CONSTRUCTION_OF : STR_WATCHING_RIDE);
                    ride->formatNameTo(ft);
                }
            }
            else
            {
                ft.Add<StringId>((StandingFlags & 0x1) ? STR_WATCHING_NEW_RIDE_BEING_CONSTRUCTED : STR_LOOKING_AT_SCENERY);
            }
            break;
        case PeepState::picked:
            ft.Add<StringId>(STR_SELECT_LOCATION);
            break;
        case PeepState::patrolling:
        case PeepState::enteringPark:
        case PeepState::leavingPark:
            ft.Add<StringId>(STR_WALKING);
            break;
        case PeepState::mowing:
            ft.Add<StringId>(STR_MOWING_GRASS);
            break;
        case PeepState::sweeping:
            ft.Add<StringId>(STR_SWEEPING_FOOTPATH);
            break;
        case PeepState::watering:
            ft.Add<StringId>(STR_WATERING_GARDENS);
            break;
        case PeepState::emptyingBin:
            ft.Add<StringId>(STR_EMPTYING_LITTER_BIN);
            break;
        case PeepState::answering:
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
                auto ride = GetRide(CurrentRide);
                if (ride != nullptr)
                {
                    ride->formatNameTo(ft);
                }
                else
                {
                    ft.Add<StringId>(kStringIdNone);
                }
            }
            break;
        case PeepState::fixing:
        {
            ft.Add<StringId>(STR_FIXING_RIDE);
            auto ride = GetRide(CurrentRide);
            if (ride != nullptr)
            {
                ride->formatNameTo(ft);
            }
            else
            {
                ft.Add<StringId>(kStringIdNone);
            }
            break;
        }
        case PeepState::headingToInspection:
        {
            ft.Add<StringId>(STR_HEADING_TO_RIDE_FOR_INSPECTION);
            auto ride = GetRide(CurrentRide);
            if (ride != nullptr)
            {
                ride->formatNameTo(ft);
            }
            else
            {
                ft.Add<StringId>(kStringIdNone);
            }
            break;
        }
        case PeepState::inspecting:
        {
            ft.Add<StringId>(STR_INSPECTING_RIDE);
            auto ride = GetRide(CurrentRide);
            if (ride != nullptr)
            {
                ride->formatNameTo(ft);
            }
            else
            {
                ft.Add<StringId>(kStringIdNone);
            }
            break;
        }
    }
}

static constexpr StringId kStaffNames[] = {
    STR_HANDYMAN_X,
    STR_MECHANIC_X,
    STR_SECURITY_GUARD_X,
    STR_ENTERTAINER_X,
};

void Peep::FormatNameTo(Formatter& ft) const
{
    if (Name == nullptr)
    {
        auto& gameState = getGameState();
        const bool showGuestNames = gameState.park.flags & PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
        const bool showStaffNames = gameState.park.flags & PARK_FLAGS_SHOW_REAL_STAFF_NAMES;

        auto* staff = As<Staff>();
        const bool isStaff = staff != nullptr;

        if ((!isStaff && showGuestNames) || (isStaff && showStaffNames))
        {
            auto nameId = PeepId;
            if (isStaff)
            {
                // Prevent staff from getting the same names by offsetting the name table based on staff type.
                nameId *= 256 * EnumValue(staff->AssignedStaffType) + 1;
            }

            auto realNameStringId = GetRealNameStringIDFromPeepID(nameId);
            ft.Add<StringId>(realNameStringId);
        }
        else if (isStaff)
        {
            auto staffNameIndex = static_cast<uint8_t>(staff->AssignedStaffType);
            if (staffNameIndex >= std::size(kStaffNames))
            {
                staffNameIndex = 0;
            }

            ft.Add<StringId>(kStaffNames[staffNameIndex]);
            ft.Add<uint32_t>(PeepId);
        }
        else
        {
            ft.Add<StringId>(STR_GUEST_X).Add<uint32_t>(PeepId);
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
    return FormatStringIDLegacy(STR_STRINGID, ft.Data());
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
    return Action == PeepActionType::walking;
}

bool Peep::IsActionIdle() const
{
    return Action == PeepActionType::idle;
}

bool Peep::IsActionInterruptable() const
{
    return IsActionIdle() || IsActionWalking();
}

/**
 * Used to avoid peep action and animation triggers that cause them to stop moving and might put them at risk
 * of getting run over at level crossings, such as guests reading the map and entertainers performing.
 */
bool Peep::IsActionInterruptableSafely() const
{
    return IsActionInterruptable() && !IsOnLevelCrossing();
}

void PeepSetMapTooltip(Peep* peep)
{
    auto ft = Formatter();
    if (auto* guest = peep->As<Guest>(); guest != nullptr)
    {
        ft.Add<StringId>((peep->PeepFlags & PEEP_FLAGS_TRACKING) ? STR_TRACKED_GUEST_MAP_TIP : STR_GUEST_MAP_TIP);
        ft.Add<uint32_t>(GetPeepFaceSpriteSmall(guest));
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
    intent.PutExtra(INTENT_EXTRA_FORMATTER, &ft);
    ContextBroadcastIntent(&intent);
}

/**
 *  rct2: 0x00693BAB
 */
void Peep::SwitchNextAnimationType()
{
    // TBD: Add nextAnimationType as function parameter and make peep->NextAnimationType obsolete?
    if (NextAnimationType != AnimationType)
    {
        Invalidate();
        AnimationType = NextAnimationType;

        auto& objManager = GetContext()->GetObjectManager();
        auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(AnimationObjectIndex);

        const auto& spriteBounds = animObj->GetSpriteBounds(AnimationGroup, NextAnimationType);
        SpriteData.Width = spriteBounds.spriteWidth;
        SpriteData.HeightMin = spriteBounds.spriteHeightNegative;
        SpriteData.HeightMax = spriteBounds.spriteHeightPositive;
        Invalidate();
    }
}

/**
 *
 *  rct2: 0x00693EF2
 */
static void PeepReturnToCentreOfTile(Peep* peep)
{
    peep->PeepDirection = DirectionReverse(peep->PeepDirection);
    auto destination = peep->GetLocation().ToTileCentre();
    peep->SetDestination(destination, 5);
}

/**
 *
 *  rct2: 0x00693f2C
 */
static bool PeepInteractWithEntrance(Peep* peep, const CoordsXYE& coords, uint8_t& pathing_result)
{
    auto tile_element = coords.element;
    uint8_t entranceType = tile_element->AsEntrance()->GetEntranceType();
    auto rideIndex = tile_element->AsEntrance()->GetRideIndex();

    if ((entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE) || (entranceType == ENTRANCE_TYPE_RIDE_EXIT))
    {
        // If an entrance or exit that doesn't belong to the ride we are queuing for ignore the entrance/exit
        // This can happen when paths clip through entrance/exits
        if (peep->State == PeepState::queuing && peep->CurrentRide != rideIndex)
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
        PeepReturnToCentreOfTile(peep);
        return true;
    }

    if (entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE)
    {
        auto ride = GetRide(rideIndex);
        if (ride == nullptr)
            return false;

        auto* guest = peep->As<Guest>();
        if (guest == nullptr)
        {
            // Default staff behaviour attempting to enter a
            // ride entrance is to turn around.
            peep->InteractionRideIndex = RideId::GetNull();
            PeepReturnToCentreOfTile(peep);
            return true;
        }

        if (guest->State == PeepState::queuing)
        {
            // Guest is in the ride queue.
            guest->RideSubState = PeepRideSubState::atQueueFront;
            guest->AnimationImageIdOffset = _backupAnimationImageIdOffset;
            return true;
        }

        // Guest is on a normal path, i.e. ride has no queue.
        if (guest->InteractionRideIndex == rideIndex)
        {
            // Peep is retrying the ride entrance without leaving
            // the path tile and without trying any other ride
            // attached to this path tile. i.e. stick with the
            // peeps previous decision not to go on the ride.
            PeepReturnToCentreOfTile(guest);
            return true;
        }

        guest->TimeLost = 0;
        auto stationNum = tile_element->AsEntrance()->GetStationIndex();
        // Guest walks up to the ride for the first time since entering
        // the path tile or since considering another ride attached to
        // the path tile.
        if (!guest->ShouldGoOnRide(*ride, stationNum, false, false))
        {
            // Peep remembers that this is the last ride they
            // considered while on this path tile.
            guest->InteractionRideIndex = rideIndex;
            PeepReturnToCentreOfTile(guest);
            return true;
        }

        // Guest has decided to go on the ride.
        guest->AnimationImageIdOffset = _backupAnimationImageIdOffset;
        guest->InteractionRideIndex = rideIndex;

        auto& station = ride->getStation(stationNum);
        auto previous_last = station.LastPeepInQueue;
        station.LastPeepInQueue = guest->Id;
        guest->GuestNextInQueue = previous_last;
        station.QueueLength++;

        guest->CurrentRide = rideIndex;
        guest->CurrentRideStation = stationNum;
        guest->DaysInQueue = 0;
        guest->SetState(PeepState::queuing);
        guest->RideSubState = PeepRideSubState::atQueueFront;
        guest->TimeInQueue = 0;
        if (guest->PeepFlags & PEEP_FLAGS_TRACKING)
        {
            auto ft = Formatter();
            guest->FormatNameTo(ft);
            ride->formatNameTo(ft);
            if (Config::Get().notifications.GuestQueuingForRide)
            {
                News::AddItemToQueue(News::ItemType::peepOnRide, STR_PEEP_TRACKING_PEEP_JOINED_QUEUE_FOR_X, guest->Id, ft);
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
            PeepReturnToCentreOfTile(peep);
            return true;
        }

        // If not the centre of the entrance arch
        if (tile_element->AsEntrance()->GetSequenceIndex() != 0)
        {
            PeepReturnToCentreOfTile(guest);
            return true;
        }

        auto& gameState = getGameState();
        uint8_t entranceDirection = tile_element->GetDirection();
        if (entranceDirection != guest->PeepDirection)
        {
            if (DirectionReverse(entranceDirection) != guest->PeepDirection)
            {
                PeepReturnToCentreOfTile(guest);
                return true;
            }

            // Peep is leaving the park.
            if (guest->State != PeepState::walking)
            {
                PeepReturnToCentreOfTile(guest);
                return true;
            }

            if (!(guest->PeepFlags & PEEP_FLAGS_LEAVING_PARK))
            {
                // If the park is open and leaving flag isn't set return to centre
                if (gameState.park.flags & PARK_FLAGS_PARK_OPEN)
                {
                    PeepReturnToCentreOfTile(guest);
                    return true;
                }
            }

            auto destination = guest->GetDestination() + CoordsDirectionDelta[guest->PeepDirection];
            guest->SetDestination(destination, 9);
            guest->MoveTo({ coords, guest->z });
            guest->SetState(PeepState::leavingPark);

            guest->Var37 = 0;
            if (guest->PeepFlags & PEEP_FLAGS_TRACKING)
            {
                auto ft = Formatter();
                guest->FormatNameTo(ft);
                if (Config::Get().notifications.GuestLeftPark)
                {
                    News::AddItemToQueue(News::ItemType::peepOnRide, STR_PEEP_TRACKING_LEFT_PARK, guest->Id, ft);
                }
            }
            return true;
        }

        // Peep is entering the park.

        if (guest->State != PeepState::enteringPark)
        {
            PeepReturnToCentreOfTile(guest);
            return true;
        }

        if (!(gameState.park.flags & PARK_FLAGS_PARK_OPEN))
        {
            guest->State = PeepState::leavingPark;
            guest->Var37 = 1;
            DecrementGuestsHeadingForPark();
            PeepWindowStateUpdate(guest);
            PeepReturnToCentreOfTile(guest);
            return true;
        }

        bool found = false;
        auto entrance = std::find_if(gameState.park.entrances.begin(), gameState.park.entrances.end(), [coords](const auto& e) {
            return coords.ToTileStart() == e;
        });
        if (entrance != gameState.park.entrances.end())
        {
            int16_t z = entrance->z / 8;
            entranceDirection = entrance->direction;
            auto nextLoc = coords.ToTileStart() + CoordsDirectionDelta[entranceDirection];

            // Make sure there is a path right behind the entrance, otherwise turn around
            TileElement* nextTileElement = MapGetFirstElementAt(nextLoc);
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
                        if (z != nextTileElement->BaseHeight)
                        {
                            continue;
                        }
                        found = true;
                        break;
                    }

                    if (DirectionReverse(slopeDirection) != entranceDirection)
                        continue;

                    if (z - 2 != nextTileElement->BaseHeight)
                        continue;
                    found = true;
                    break;
                }

                if (z != nextTileElement->BaseHeight)
                {
                    continue;
                }
                found = true;
                break;
            } while (!(nextTileElement++)->IsLastForTile());
        }

        if (!found)
        {
            guest->State = PeepState::leavingPark;
            guest->Var37 = 1;
            DecrementGuestsHeadingForPark();
            PeepWindowStateUpdate(guest);
            PeepReturnToCentreOfTile(guest);
            return true;
        }

        auto entranceFee = Park::GetEntranceFee();
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
                guest->State = PeepState::leavingPark;
                guest->Var37 = 1;
                DecrementGuestsHeadingForPark();
                PeepWindowStateUpdate(guest);
                PeepReturnToCentreOfTile(guest);
                return true;
            }

            gameState.park.totalIncomeFromAdmissions += entranceFee;
            guest->SpendMoney(guest->PaidToEnter, entranceFee, ExpenditureType::parkEntranceTickets);
            guest->PeepFlags |= PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY;
        }

        getGameState().park.totalAdmissions++;

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByNumber(WindowClass::parkInformation, 0);

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
static void PeepFootpathMoveForward(Peep* peep, const CoordsXYE& coords, bool vandalism)
{
    const auto* pathElement = coords.element->AsPath();
    assert(pathElement != nullptr);

    peep->NextLoc = { coords.ToTileStart(), pathElement->GetBaseZ() };
    peep->SetNextFlags(pathElement->GetSlopeDirection(), pathElement->IsSloped(), false);

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
            if ((ScenarioRand() & 0xFFFF) <= 10922)
            {
                guest->InsertNewThought(PeepThoughtType::Vandalism);
                guest->HappinessTarget = std::max(0, guest->HappinessTarget - 17);
            }
            vandalThoughtTimeout = 3;
        }
    }

    if (vandalThoughtTimeout && (ScenarioRand() & 0xFFFF) <= 4369)
    {
        vandalThoughtTimeout--;
    }

    guest->VandalismSeen = (vandalThoughtTimeout << 6) | vandalisedTiles;

    constexpr uint16_t kThresholdCrowdCount = 10;
    constexpr uint8_t kThresholdLitterCount = 3;
    constexpr uint8_t kThresholdVomitCount = 3;

    // Don't allow this loop to become too expensive, lets just have enough to potentially satisfy
    // all the conditions.
    constexpr uint32_t kMaxIterations = kThresholdCrowdCount + kThresholdLitterCount + kThresholdVomitCount;

    uint16_t crowdCount = 0;
    uint8_t litterCount = 0;
    uint8_t vomitCount = 0;
    uint32_t iterations = 0;

    auto quad = EntityTileList(coords);
    for (auto* otherEnt : quad)
    {
        if (iterations++ >= kMaxIterations)
        {
            break;
        }

        if (std::abs(otherEnt->z - guest->NextLoc.z) > 16)
            continue;

        if (const auto* otherPeep = otherEnt->As<Peep>(); otherPeep != nullptr)
        {
            if (otherPeep->State != PeepState::walking)
                continue;

            crowdCount++;
            continue;
        }

        if (const auto* litter = otherEnt->As<Litter>(); litter != nullptr)
        {
            if (litter->SubType != Litter::Type::Vomit && litter->SubType != Litter::Type::VomitAlt)
            {
                litterCount++;
            }
            else
            {
                vomitCount++;
            }
        }
    }

    if (crowdCount >= kThresholdCrowdCount && guest->State == PeepState::walking && (ScenarioRand() & 0xFFFF) <= 21845)
    {
        guest->InsertNewThought(PeepThoughtType::Crowded);
        guest->HappinessTarget = std::max(0, guest->HappinessTarget - 14);
    }

    litterCount = std::min(kThresholdLitterCount, litterCount);
    vomitCount = std::min(kThresholdVomitCount, vomitCount);

    uint8_t disgustingTime = guest->DisgustingCount & 0xC0;
    uint8_t disgustingCount = ((guest->DisgustingCount & 0xF) << 2) | vomitCount;
    guest->DisgustingCount = disgustingCount | disgustingTime;

    if (disgustingTime & 0xC0 && (ScenarioRand() & 0xFFFF) <= 4369)
    {
        // Reduce the disgusting time
        guest->DisgustingCount -= 0x40;
    }
    else
    {
        uint8_t totalDisgustingCount = 0;
        for (uint8_t time = 0; time < 3; time++)
        {
            totalDisgustingCount += (disgustingCount >> (2 * time)) & 0x3;
        }

        if (totalDisgustingCount >= kThresholdVomitCount && (ScenarioRand() & 0xFFFF) <= 10922)
        {
            guest->InsertNewThought(PeepThoughtType::PathDisgusting);
            guest->HappinessTarget = std::max(0, guest->HappinessTarget - 17);
            // Reset disgusting time
            guest->DisgustingCount |= 0xC0;
        }
    }

    uint8_t litterTime = guest->LitterCount & 0xC0;
    litterCount = ((guest->LitterCount & 0xF) << 2) | litterCount;
    guest->LitterCount = litterCount | litterTime;

    if (litterTime & 0xC0 && (ScenarioRand() & 0xFFFF) <= 4369)
    {
        // Reduce the litter time
        guest->LitterCount -= 0x40;
    }
    else
    {
        uint8_t totalLitter = 0;
        for (uint8_t time = 0; time < 3; time++)
        {
            totalLitter += (litterCount >> (2 * time)) & 0x3;
        }

        if (totalLitter >= kThresholdLitterCount && (ScenarioRand() & 0xFFFF) <= 10922)
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
static void PeepInteractWithPath(Peep* peep, const CoordsXYE& coords)
{
    // 0x00F1AEE2
    const auto* pathElement = coords.element->AsPath();
    assert(pathElement != nullptr);

    bool vandalismPresent = false;
    if (pathElement->HasAddition() && pathElement->IsBroken() && (pathElement->GetEdges()) != 0xF)
    {
        vandalismPresent = true;
    }

    int16_t z = pathElement->GetBaseZ();
    auto* guest = peep->As<Guest>();
    if (MapIsLocationOwned({ coords, z }))
    {
        if (guest != nullptr && guest->OutsideOfPark)
        {
            PeepReturnToCentreOfTile(guest);
            return;
        }
    }
    else
    {
        if (guest == nullptr || !guest->OutsideOfPark)
        {
            PeepReturnToCentreOfTile(peep);
            return;
        }
    }

    if (guest != nullptr && pathElement->IsQueue())
    {
        auto rideIndex = pathElement->GetRideIndex();
        if (guest->State == PeepState::queuing)
        {
            // Check if this queue is connected to the ride the
            // peep is queuing for, i.e. the player hasn't edited
            // the queue, rebuilt the ride, etc.
            if (guest->CurrentRide == rideIndex)
            {
                PeepFootpathMoveForward(guest, coords, vandalismPresent);
            }
            else
            {
                // Queue got disconnected from the original ride.
                guest->InteractionRideIndex = RideId::GetNull();
                guest->RemoveFromQueue();
                guest->SetState(PeepState::one);
                PeepFootpathMoveForward(guest, coords, vandalismPresent);
            }
        }
        else
        {
            // Peep is not queuing.
            guest->TimeLost = 0;
            auto stationNum = pathElement->GetStationIndex();

            if (pathElement->HasQueueBanner()
                && pathElement->GetQueueBannerDirection()
                    == DirectionReverse(guest->PeepDirection) // Ride sign is facing the direction the peep is walking
            )
            {
                /* Peep is approaching the entrance of a ride queue.
                 * Decide whether to go on the ride. */
                auto* ride = GetRide(rideIndex);
                if (ride != nullptr && guest->ShouldGoOnRide(*ride, stationNum, true, false))
                {
                    // Peep has decided to go on the ride at the queue.
                    guest->InteractionRideIndex = rideIndex;

                    // Add the peep to the ride queue.
                    auto& station = ride->getStation(stationNum);
                    auto old_last_peep = station.LastPeepInQueue;
                    station.LastPeepInQueue = guest->Id;
                    guest->GuestNextInQueue = old_last_peep;
                    station.QueueLength++;

                    PeepDecrementNumRiders(guest);
                    guest->CurrentRide = rideIndex;
                    guest->CurrentRideStation = stationNum;
                    guest->State = PeepState::queuing;
                    guest->DaysInQueue = 0;
                    PeepWindowStateUpdate(guest);

                    guest->RideSubState = PeepRideSubState::inQueue;
                    guest->DestinationTolerance = 2;
                    guest->TimeInQueue = 0;
                    if (guest->PeepFlags & PEEP_FLAGS_TRACKING)
                    {
                        auto ft = Formatter();
                        guest->FormatNameTo(ft);
                        ride->formatNameTo(ft);
                        if (Config::Get().notifications.GuestQueuingForRide)
                        {
                            News::AddItemToQueue(
                                News::ItemType::peepOnRide, STR_PEEP_TRACKING_PEEP_JOINED_QUEUE_FOR_X, guest->Id, ft);
                        }
                    }

                    // Force set centre of tile to prevent issues with guests accidentally skipping the queue
                    auto queueTileCentre = CoordsXY{ CoordsXY{ guest->NextLoc } + CoordsDirectionDelta[guest->PeepDirection] }
                                               .ToTileCentre();
                    guest->SetDestination(queueTileCentre);

                    PeepFootpathMoveForward(guest, coords, vandalismPresent);
                }
                else
                {
                    // Peep has decided not to go on the ride.
                    PeepReturnToCentreOfTile(guest);
                }
            }
            else
            {
                /* Peep is approaching a queue tile without a ride
                 * sign facing the peep. */
                PeepFootpathMoveForward(guest, coords, vandalismPresent);
            }
        }
    }
    else
    {
        peep->InteractionRideIndex = RideId::GetNull();
        if (guest != nullptr && peep->State == PeepState::queuing)
        {
            guest->RemoveFromQueue();
            guest->SetState(PeepState::one);
        }
        PeepFootpathMoveForward(peep, coords, vandalismPresent);
    }
}

/**
 *
 *  rct2: 0x00693F70
 */
static bool PeepInteractWithShop(Peep* peep, const CoordsXYE& coords)
{
    RideId rideIndex = coords.element->AsTrack()->GetRideIndex();
    auto ride = GetRide(rideIndex);
    if (ride == nullptr || !ride->getRideTypeDescriptor().HasFlag(RtdFlag::isShopOrFacility))
        return false;

    auto* guest = peep->As<Guest>();
    if (guest == nullptr)
    {
        PeepReturnToCentreOfTile(peep);
        return true;
    }

    // If we are queuing ignore the 'shop'
    // This can happen when paths clip through track
    if (guest->State == PeepState::queuing)
    {
        return false;
    }

    guest->TimeLost = 0;

    if (ride->status != RideStatus::open)
    {
        PeepReturnToCentreOfTile(guest);
        return true;
    }

    if (guest->InteractionRideIndex == rideIndex)
    {
        PeepReturnToCentreOfTile(guest);
        return true;
    }

    if (guest->PeepFlags & PEEP_FLAGS_LEAVING_PARK)
    {
        PeepReturnToCentreOfTile(guest);
        return true;
    }

    if (ride->getRideTypeDescriptor().HasFlag(RtdFlag::guestsShouldGoInsideFacility))
    {
        guest->TimeLost = 0;
        if (!guest->ShouldGoOnRide(*ride, StationIndex::FromUnderlying(0), false, false))
        {
            PeepReturnToCentreOfTile(guest);
            return true;
        }

        auto cost = ride->price[0];
        if (cost != 0 && !(getGameState().park.flags & PARK_FLAGS_NO_MONEY))
        {
            ride->totalProfit = AddClamp(ride->totalProfit, cost);
            ride->windowInvalidateFlags |= RIDE_INVALIDATE_RIDE_INCOME;
            guest->SpendMoney(cost, ExpenditureType::parkRideTickets);
        }

        auto coordsCentre = coords.ToTileCentre();
        guest->SetDestination(coordsCentre, 3);
        guest->CurrentRide = rideIndex;
        guest->SetState(PeepState::enteringRide);
        guest->RideSubState = PeepRideSubState::approachShop;

        guest->GuestTimeOnRide = 0;
        ride->curNumCustomers++;
        if (guest->PeepFlags & PEEP_FLAGS_TRACKING)
        {
            auto ft = Formatter();
            guest->FormatNameTo(ft);
            ride->formatNameTo(ft);
            StringId string_id = ride->getRideTypeDescriptor().HasFlag(RtdFlag::describeAsInside)
                ? STR_PEEP_TRACKING_PEEP_IS_IN_X
                : STR_PEEP_TRACKING_PEEP_IS_ON_X;
            if (Config::Get().notifications.GuestUsedFacility)
            {
                News::AddItemToQueue(News::ItemType::peepOnRide, string_id, guest->Id, ft);
            }
        }
    }
    else
    {
        if (guest->GuestHeadingToRideId == rideIndex)
            guest->GuestHeadingToRideId = RideId::GetNull();
        guest->AnimationImageIdOffset = _backupAnimationImageIdOffset;
        guest->SetState(PeepState::buying);
        guest->CurrentRide = rideIndex;
        guest->SubState = 0;
    }

    return true;
}

/**
 *
 *  rct2: 0x00693C9E
 */
std::pair<uint8_t, TileElement*> Peep::PerformNextAction()
{
    uint8_t pathingResult = 0;
    TileElement* tileResult = nullptr;

    PeepActionType previousAction = Action;

    if (Action == PeepActionType::idle)
        Action = PeepActionType::walking;

    auto* guest = As<Guest>();
    if (State == PeepState::queuing && guest != nullptr)
    {
        if (guest->UpdateQueuePosition(previousAction))
        {
            return { pathingResult, tileResult };
        }
    }

    std::optional<CoordsXY> loc;
    if (loc = UpdateAction(); !loc.has_value())
    {
        pathingResult |= PATHING_DESTINATION_REACHED;
        uint8_t result = 0;

        if (guest != nullptr)
        {
            result = PathFinding::CalculateNextDestination(*guest);
        }
        else
        {
            auto* staff = As<Staff>();
            result = staff->DoPathFinding();
        }

        if (result != 0)
            return { pathingResult, tileResult };

        if (loc = UpdateAction(); !loc.has_value())
            return { pathingResult, tileResult };
    }

    auto newLoc = *loc;
    CoordsXY truncatedNewLoc = newLoc.ToTileStart();
    if (truncatedNewLoc == CoordsXY{ NextLoc })
    {
        int16_t height = GetZOnSlope(newLoc.x, newLoc.y);
        MoveTo({ newLoc.x, newLoc.y, height });
        return { pathingResult, tileResult };
    }

    if (MapIsEdge(newLoc))
    {
        if (guest != nullptr && guest->OutsideOfPark)
        {
            pathingResult |= PATHING_OUTSIDE_PARK;
        }
        PeepReturnToCentreOfTile(this);
        return { pathingResult, tileResult };
    }

    TileElement* tileElement = MapGetFirstElementAt(newLoc);
    if (tileElement == nullptr)
        return { pathingResult, tileResult };

    int16_t base_z = std::max(0, (z / 8) - 2);
    int16_t top_z = (z / 8) + 1;

    do
    {
        if (base_z > tileElement->BaseHeight)
            continue;
        if (top_z < tileElement->BaseHeight)
            continue;
        if (tileElement->IsGhost())
            continue;

        if (tileElement->GetType() == TileElementType::Path)
        {
            PeepInteractWithPath(this, { newLoc, tileElement });
            tileResult = tileElement;

            return { pathingResult, tileResult };
        }

        if (tileElement->GetType() == TileElementType::Track)
        {
            if (PeepInteractWithShop(this, { newLoc, tileElement }))
            {
                tileResult = tileElement;
                return { pathingResult, tileResult };
            }
        }
        else if (tileElement->GetType() == TileElementType::Entrance)
        {
            if (PeepInteractWithEntrance(this, { newLoc, tileElement }, pathingResult))
            {
                tileResult = tileElement;
                return { pathingResult, tileResult };
            }
        }
    } while (!(tileElement++)->IsLastForTile());

    if (Is<Staff>() || (GetNextIsSurface()))
    {
        int16_t height = abs(TileElementHeight(newLoc) - z);
        if (height <= 3 || (Is<Staff>() && height <= 32))
        {
            InteractionRideIndex = RideId::GetNull();
            if (guest != nullptr && State == PeepState::queuing)
            {
                guest->RemoveFromQueue();
                SetState(PeepState::one);
            }

            if (!MapIsLocationInPark(newLoc))
            {
                PeepReturnToCentreOfTile(this);
                return { pathingResult, tileResult };
            }

            auto surfaceElement = MapGetSurfaceElementAt(newLoc);
            if (surfaceElement == nullptr)
            {
                PeepReturnToCentreOfTile(this);
                return { pathingResult, tileResult };
            }

            int16_t water_height = surfaceElement->GetWaterHeight();
            if (water_height > 0)
            {
                PeepReturnToCentreOfTile(this);
                return { pathingResult, tileResult };
            }

            auto* staff = As<Staff>();
            if (staff != nullptr && !GetNextIsSurface())
            {
                // Prevent staff from leaving the path on their own unless they're allowed to mow.
                if (!((staff->StaffOrders & STAFF_ORDERS_MOWING) && staff->StaffMowingTimeout >= 12))
                {
                    PeepReturnToCentreOfTile(staff);
                    return { pathingResult, tileResult };
                }
            }

            // The peep is on a surface and not on a path
            NextLoc = { truncatedNewLoc, surfaceElement->GetBaseZ() };
            SetNextFlags(0, false, true);

            height = GetZOnSlope(newLoc.x, newLoc.y);
            MoveTo({ newLoc.x, newLoc.y, height });
            return { pathingResult, tileResult };
        }
    }

    PeepReturnToCentreOfTile(this);
    return { pathingResult, tileResult };
}

/**
 * Gets the height including the bit depending on how far up the slope the peep
 * is.
 *  rct2: 0x00694921
 */
int32_t Peep::GetZOnSlope(int32_t tile_x, int32_t tile_y)
{
    if (tile_x == kLocationNull)
        return 0;

    if (GetNextIsSurface())
    {
        return TileElementHeight({ tile_x, tile_y });
    }

    uint8_t slope = GetNextDirection();
    return NextLoc.z + MapHeightFromSlope({ tile_x, tile_y }, slope, GetNextIsSloped());
}

StringId GetRealNameStringIDFromPeepID(uint32_t id)
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
    dx += kRealNameStart;
    return dx;
}

int32_t PeepCompare(const EntityId sprite_index_a, const EntityId sprite_index_b)
{
    Peep const* peep_a = getGameState().entities.GetEntity<Peep>(sprite_index_a);
    Peep const* peep_b = getGameState().entities.GetEntity<Peep>(sprite_index_b);
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
        if (getGameState().park.flags & PARK_FLAGS_SHOW_REAL_GUEST_NAMES)
        {
            // Potentially could find a more optional way of sorting dynamic real names
        }
        else
        {
            // Simple ID comparison for when both peeps use a number or a generated name
            return peep_a->PeepId - peep_b->PeepId;
        }
    }

    // Compare their names as strings
    char nameA[256]{};
    Formatter ft;
    peep_a->FormatNameTo(ft);
    OpenRCT2::FormatStringLegacy(nameA, sizeof(nameA), STR_STRINGID, ft.Data());

    char nameB[256]{};
    ft.Rewind();
    peep_b->FormatNameTo(ft);
    OpenRCT2::FormatStringLegacy(nameB, sizeof(nameB), STR_STRINGID, ft.Data());
    return String::logicalCmp(nameA, nameB);
}

/**
 *
 *  rct2: 0x0069926C
 */
void PeepUpdateNames()
{
    auto& gameState = getGameState();
    auto& config = Config::Get().general;

    if (config.ShowRealNamesOfGuests)
        gameState.park.flags |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
    else
        gameState.park.flags &= ~PARK_FLAGS_SHOW_REAL_GUEST_NAMES;

    if (config.ShowRealNamesOfStaff)
        gameState.park.flags |= PARK_FLAGS_SHOW_REAL_STAFF_NAMES;
    else
        gameState.park.flags &= ~PARK_FLAGS_SHOW_REAL_STAFF_NAMES;

    auto intent = Intent(INTENT_ACTION_REFRESH_GUEST_LIST);
    ContextBroadcastIntent(&intent);
    GfxInvalidateScreen();
}

void IncrementGuestsInPark()
{
    auto& gameState = getGameState();
    if (gameState.park.numGuestsInPark < UINT32_MAX)
    {
        gameState.park.numGuestsInPark++;
    }
    else
    {
        Guard::Fail("Attempt to increment guests in park above max value (65535).");
    }
}

void IncrementGuestsHeadingForPark()
{
    auto& gameState = getGameState();
    if (gameState.park.numGuestsHeadingForPark < UINT32_MAX)
    {
        gameState.park.numGuestsHeadingForPark++;
    }
    else
    {
        Guard::Fail("Attempt to increment guests heading for park above max value (65535).");
    }
}

void DecrementGuestsInPark()
{
    auto& gameState = getGameState();
    if (gameState.park.numGuestsInPark > 0)
    {
        gameState.park.numGuestsInPark--;
    }
    else
    {
        LOG_ERROR("Attempt to decrement guests in park below zero.");
    }
}

void DecrementGuestsHeadingForPark()
{
    auto& gameState = getGameState();

    if (gameState.park.numGuestsHeadingForPark > 0)
    {
        gameState.park.numGuestsHeadingForPark--;
    }
    else
    {
        LOG_ERROR("Attempt to decrement guests heading for park below zero.");
    }
}

static void GuestReleaseBalloon(Guest* peep, int16_t spawn_height)
{
    if (peep->HasItem(ShopItem::Balloon))
    {
        peep->RemoveItem(ShopItem::Balloon);

        if (peep->AnimationGroup == PeepAnimationGroup::balloon && peep->x != kLocationNull)
        {
            Balloon::Create({ peep->x, peep->y, spawn_height }, peep->BalloonColour, false);
            peep->WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
            peep->UpdateAnimationGroup();
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
    if (guest != nullptr && State == PeepState::queuing)
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
    stream << AnimationGroup;
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
    stream << AnimationType;
    stream << NextAnimationType;
    stream << AnimationImageIdOffset;
    stream << Action;
    stream << AnimationFrameNum;
    stream << StepProgress;
    stream << PeepDirection;
    stream << InteractionRideIndex;
    stream << PeepId;
    stream << PathCheckOptimisation;
    stream << PathfindGoal;
    stream << PathfindHistory;
    stream << WalkingAnimationFrameNum;
    stream << PeepFlags;
}

void Peep::Paint(PaintSession& session, int32_t imageDirection) const
{
    PROFILED_FUNCTION();

    if (LightFx::IsAvailable())
    {
        if (Is<Staff>())
        {
            auto loc = GetLocation();
            switch (Orientation)
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

            LightFx::Add3DLight(*this, 0, loc, LightType::Spot1);
        }
    }

    if (session.DPI.zoom_level > ZoomLevel{ 2 })
    {
        return;
    }

    PeepAnimationType actionAnimationGroup = AnimationType;
    uint8_t imageOffset = AnimationImageIdOffset;

    if (Action == PeepActionType::idle)
    {
        actionAnimationGroup = NextAnimationType;
        imageOffset = 0;
    }

    auto& objManager = GetContext()->GetObjectManager();
    auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(AnimationObjectIndex);

    uint32_t baseImageId = animObj->GetPeepAnimation(AnimationGroup, actionAnimationGroup).baseImage;

    // Offset frame onto the base image, using rotation except for the 'picked up' state
    if (actionAnimationGroup != PeepAnimationType::hanging)
        baseImageId += (imageDirection >> 3) + imageOffset * 4;
    else
        baseImageId += imageOffset;

    auto imageId = ImageId(baseImageId, TshirtColour, TrousersColour);

    // In the following 4 calls to PaintAddImageAsParent/PaintAddImageAsChild, we add 5 (instead of 3) to the
    // bound_box_offset_z to make sure peeps are drawn on top of railways
    auto bb = BoundBoxXYZ{ { 0, 0, z + 5 }, { 1, 1, 11 } };
    auto offset = CoordsXYZ{ 0, 0, z };
    PaintAddImageAsParent(session, imageId, { 0, 0, z }, bb);

    auto* guest = As<Guest>();
    if (guest == nullptr)
        return;

    // Can't display any accessories whilst drowning or clapping
    if (Action == PeepActionType::drowning || Action == PeepActionType::clap)
        return;

    // There are only 6 walking frames available for each item,
    // as well as 1 sprite for sitting and 1 for standing still.
    auto itemFrame = imageOffset % 6;
    if (actionAnimationGroup == PeepAnimationType::watchRide)
        itemFrame = 6;
    else if (actionAnimationGroup == PeepAnimationType::sittingIdle)
        itemFrame = 7;

    if (AnimationGroup == PeepAnimationGroup::hat)
    {
        auto itemOffset = kPeepSpriteHatItemStart;
        imageId = ImageId(itemOffset + (imageDirection >> 3) + itemFrame * 4, guest->HatColour);
        PaintAddImageAsChild(session, imageId, offset, bb);
        return;
    }

    if (AnimationGroup == PeepAnimationGroup::balloon)
    {
        auto itemOffset = kPeepSpriteBalloonItemStart;
        imageId = ImageId(itemOffset + (imageDirection >> 3) + itemFrame * 4, guest->BalloonColour);
        PaintAddImageAsChild(session, imageId, offset, bb);
        return;
    }

    if (AnimationGroup == PeepAnimationGroup::umbrella)
    {
        auto itemOffset = kPeepSpriteUmbrellaItemStart;
        imageId = ImageId(itemOffset + (imageDirection >> 3) + itemFrame * 4, guest->UmbrellaColour);
        PaintAddImageAsChild(session, imageId, offset, bb);
        return;
    }
}

/**
 *
 *  rct2: 0x0069A98C
 */
void Peep::ResetPathfindGoal()
{
    PathfindGoal.SetNull();
    PathfindGoal.direction = kInvalidDirection;
}
