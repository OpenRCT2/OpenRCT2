/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StaffHireNewAction.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../entity/EntityRegistry.h"
#include "../entity/Staff.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../object/ObjectManager.h"
#include "../object/PeepAnimationsObject.h"
#include "../ride/Ride.h"
#include "../scenario/Scenario.h"
#include "../ui/WindowManager.h"
#include "../world/Entrance.h"
#include "../world/Park.h"

#include <set>

using namespace OpenRCT2;

StaffHireNewAction::StaffHireNewAction(
    bool autoPosition, StaffType staffType, ObjectEntryIndex costumeIndex, uint32_t staffOrders)
    : _autoPosition(autoPosition)
    , _staffType(static_cast<uint8_t>(staffType))
    , _costumeIndex(costumeIndex)
    , _staffOrders(staffOrders)
{
}

void StaffHireNewAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("autoPosition", _autoPosition);
    visitor.Visit("staffType", _staffType);
    visitor.Visit("costumeIndex", _costumeIndex);
    visitor.Visit("staffOrders", _staffOrders);
}

uint16_t StaffHireNewAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void StaffHireNewAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_autoPosition) << DS_TAG(_staffType) << DS_TAG(_costumeIndex) << DS_TAG(_staffOrders);
}

GameActions::Result StaffHireNewAction::Query() const
{
    return QueryExecute(false);
}

GameActions::Result StaffHireNewAction::Execute() const
{
    return QueryExecute(true);
}

GameActions::Result StaffHireNewAction::QueryExecute(bool execute) const
{
    auto res = GameActions::Result();
    res.Expenditure = ExpenditureType::Wages;

    if (_staffType >= static_cast<uint8_t>(StaffType::Count))
    {
        LOG_ERROR("Invalid staff type %u", static_cast<uint32_t>(_staffType));
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_HIRE_NEW_STAFF, STR_ERR_VALUE_OUT_OF_RANGE);
    }

    if (GetNumFreeEntities() < 400)
    {
        return GameActions::Result(GameActions::Status::NoFreeElements, STR_CANT_HIRE_NEW_STAFF, STR_TOO_MANY_PEOPLE_IN_GAME);
    }

    if (_staffType == static_cast<uint8_t>(StaffType::Entertainer))
    {
        auto costumes = findAllPeepAnimationsIndexesForType(AnimationPeepType::Entertainer);
        if (std::find(costumes.begin(), costumes.end(), _costumeIndex) == costumes.end())
        {
            LOG_ERROR("Unavailable entertainer costume %u", static_cast<uint32_t>(_costumeIndex));
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_CANT_HIRE_NEW_STAFF, STR_ERR_VALUE_OUT_OF_RANGE);
        }
    }

    Staff* newPeep = CreateEntity<Staff>();
    if (newPeep == nullptr)
    {
        // Too many peeps exist already.
        return GameActions::Result(GameActions::Status::NoFreeElements, STR_CANT_HIRE_NEW_STAFF, STR_TOO_MANY_PEOPLE_IN_GAME);
    }

    if (execute == false)
    {
        // In query we just want to see if we can obtain a sprite slot.
        EntityRemove(newPeep);

        res.SetData(StaffHireNewActionResult{ EntityId::GetNull() });
    }
    else
    {
        newPeep->WindowInvalidateFlags = 0;
        newPeep->Action = PeepActionType::Walking;
        newPeep->SpecialSprite = 0;
        newPeep->AnimationImageIdOffset = 0;
        newPeep->WalkingAnimationFrameNum = 0;
        newPeep->AnimationType = PeepAnimationType::Walking;
        newPeep->PathCheckOptimisation = 0;
        newPeep->PeepFlags = 0;
        newPeep->StaffLawnsMown = 0;
        newPeep->StaffGardensWatered = 0;
        newPeep->StaffLitterSwept = 0;
        newPeep->StaffBinsEmptied = 0;
        newPeep->StaffOrders = _staffOrders;

        // We search for the first available Id for a given staff type
        std::set<uint32_t> usedStaffIds;

        for (auto searchPeep : EntityList<Staff>())
        {
            if (static_cast<uint8_t>(searchPeep->AssignedStaffType) != _staffType)
                continue;

            usedStaffIds.insert(searchPeep->PeepId);
        }

        uint32_t newStaffId = 1;
        while (usedStaffIds.find(newStaffId) != usedStaffIds.end())
        {
            newStaffId++;
        }

        newPeep->PeepId = newStaffId;
        newPeep->AssignedStaffType = static_cast<StaffType>(_staffType);

        auto animPeepType = AnimationPeepType(static_cast<uint8_t>(_staffType) + 1);
        ObjectEntryIndex animObjectIndex = _costumeIndex;
        if (animPeepType != AnimationPeepType::Entertainer)
            animObjectIndex = findPeepAnimationsIndexForType(animPeepType);

        newPeep->Name = nullptr;
        newPeep->AnimationObjectIndex = animObjectIndex;
        newPeep->AnimationGroup = PeepAnimationGroup::Normal;

        auto& objManager = GetContext()->GetObjectManager();
        auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(animObjectIndex);

        newPeep->PeepFlags &= ~PEEP_FLAGS_SLOW_WALK;
        if (animObj->IsSlowWalking(PeepAnimationGroup::Normal))
            newPeep->PeepFlags |= PEEP_FLAGS_SLOW_WALK;

        const auto& spriteBounds = animObj->GetSpriteBounds(newPeep->AnimationGroup);
        newPeep->SpriteData.Width = spriteBounds.sprite_width;
        newPeep->SpriteData.HeightMin = spriteBounds.sprite_height_negative;
        newPeep->SpriteData.HeightMax = spriteBounds.sprite_height_positive;

        if (_autoPosition)
        {
            AutoPositionNewStaff(newPeep);
        }
        else
        {
            // NOTE: This state is required for the window to act.
            newPeep->State = PeepState::Picked;

            // INVESTIGATE: x and y are LOCATION_NULL at this point.
            newPeep->MoveTo(newPeep->GetLocation());
        }

        // Staff uses this
        newPeep->As<Staff>()->SetHireDate(GetDate().GetMonthsElapsed());
        newPeep->PathfindGoal.x = 0xFF;
        newPeep->PathfindGoal.y = 0xFF;
        newPeep->PathfindGoal.z = 0xFF;
        newPeep->PathfindGoal.direction = INVALID_DIRECTION;

        uint8_t colour = StaffGetColour(static_cast<StaffType>(_staffType));
        newPeep->TshirtColour = colour;
        newPeep->TrousersColour = colour;

        // Staff energy determines their walking speed
        switch (GetGameState().Cheats.selectedStaffSpeed)
        {
            case StaffSpeedCheat::None:
                newPeep->Energy = kCheatsStaffNormalSpeed;
                newPeep->EnergyTarget = kCheatsStaffNormalSpeed;
                break;

            case StaffSpeedCheat::Frozen:
                newPeep->Energy = kCheatsStaffFreezeSpeed;
                newPeep->EnergyTarget = kCheatsStaffFreezeSpeed;
                break;

            case StaffSpeedCheat::Fast:
                newPeep->Energy = kCheatsStaffFastSpeed;
                newPeep->EnergyTarget = kCheatsStaffFastSpeed;
                break;
        }

        newPeep->StaffMowingTimeout = 0;
        newPeep->PatrolInfo = nullptr;

        res.SetData(StaffHireNewActionResult{ newPeep->Id });
    }

    return res;
}

void StaffHireNewAction::AutoPositionNewStaff(Peep* newPeep) const
{
    // Find a location to place new staff member
    newPeep->State = PeepState::Falling;

    uint32_t count = 0;
    PathElement* guest_tile = nullptr;

    // Count number of walking guests
    {
        for (auto guest : EntityList<Guest>())
        {
            if (guest->State == PeepState::Walking)
            {
                // Check the walking guest's tile. Only count them if they're on a path tile.
                guest_tile = MapGetPathElementAt(TileCoordsXYZ{ guest->NextLoc });
                if (guest_tile != nullptr)
                    ++count;
            }
        }
    }

    CoordsXYZ newLocation{};
    if (count > 0)
    {
        // Place staff at a random guest
        uint32_t rand = ScenarioRandMax(count);
        Guest* chosenGuest = nullptr;

        for (auto guest : EntityList<Guest>())
        {
            if (guest->State == PeepState::Walking)
            {
                guest_tile = MapGetPathElementAt(TileCoordsXYZ{ guest->NextLoc });
                if (guest_tile != nullptr)
                {
                    if (rand == 0)
                    {
                        chosenGuest = guest;
                        break;
                    }
                    --rand;
                }
            }
        }

        if (chosenGuest != nullptr)
        {
            newLocation = chosenGuest->GetLocation();
        }
        else
        {
            // User must pick a location
            newPeep->State = PeepState::Picked;
            newLocation = newPeep->GetLocation();
        }
    }
    else
    {
        // No walking guests; pick random park entrance
        const auto& gameState = GetGameState();
        if (!gameState.Park.Entrances.empty())
        {
            auto rand = ScenarioRandMax(static_cast<uint32_t>(gameState.Park.Entrances.size()));
            const auto& entrance = gameState.Park.Entrances[rand];
            auto dir = entrance.direction;
            newLocation = entrance;
            // TODO: Replace with CoordsDirectionDelta
            newLocation.x += 16 + ((dir & 1) == 0 ? ((dir & 2) ? 32 : -32) : 0);
            newLocation.y += 16 + ((dir & 1) == 1 ? ((dir & 2) ? -32 : 32) : 0);
        }
        else
        {
            // User must pick a location
            newPeep->State = PeepState::Picked;
            newLocation = newPeep->GetLocation();
        }
    }

    newPeep->MoveTo(newLocation + CoordsXYZ{ 0, 0, 16 });
}
