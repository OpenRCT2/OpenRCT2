/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StaffHireNewAction.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../peep/Staff.h"
#include "../ride/Ride.h"
#include "../scenario/Scenario.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../world/Entrance.h"
#include "../world/Park.h"
#include "../world/Sprite.h"

StaffHireNewActionResult::StaffHireNewActionResult()
    : GameActions::Result(GameActions::Status::Ok, STR_CANT_HIRE_NEW_STAFF)
{
}

StaffHireNewActionResult::StaffHireNewActionResult(GameActions::Status error, rct_string_id message)
    : GameActions::Result(error, STR_CANT_HIRE_NEW_STAFF, message)
{
}

StaffHireNewAction::StaffHireNewAction(
    bool autoPosition, StaffType staffType, EntertainerCostume entertainerType, uint32_t staffOrders)
    : _autoPosition(autoPosition)
    , _staffType(static_cast<uint8_t>(staffType))
    , _entertainerType(entertainerType)
    , _staffOrders(staffOrders)
{
}

void StaffHireNewAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("autoPosition", _autoPosition);
    visitor.Visit("staffType", _staffType);
    visitor.Visit("entertainerType", _entertainerType);
    visitor.Visit("staffOrders", _staffOrders);
}

uint16_t StaffHireNewAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void StaffHireNewAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_autoPosition) << DS_TAG(_staffType) << DS_TAG(_entertainerType) << DS_TAG(_staffOrders);
}

GameActions::Result::Ptr StaffHireNewAction::Query() const
{
    return QueryExecute(false);
}

GameActions::Result::Ptr StaffHireNewAction::Execute() const
{
    return QueryExecute(true);
}

GameActions::Result::Ptr StaffHireNewAction::QueryExecute(bool execute) const
{
    auto res = std::make_unique<StaffHireNewActionResult>();

    res->Expenditure = ExpenditureType::Wages;

    if (_staffType >= static_cast<uint8_t>(StaffType::Count))
    {
        // Invalid staff type.
        log_error("Tried to use invalid staff type: %u", static_cast<uint32_t>(_staffType));

        return MakeResult(GameActions::Status::InvalidParameters, STR_NONE);
    }

    if (GetNumFreeEntities() < 400)
    {
        return MakeResult(GameActions::Status::NoFreeElements, STR_TOO_MANY_PEOPLE_IN_GAME);
    }

    if (_staffType == static_cast<uint8_t>(StaffType::Entertainer))
    {
        if (static_cast<uint8_t>(_entertainerType) >= static_cast<uint8_t>(EntertainerCostume::Count))
        {
            // Invalid entertainer costume
            log_error("Tried to use invalid entertainer type: %u", static_cast<uint32_t>(_entertainerType));

            return MakeResult(GameActions::Status::InvalidParameters, STR_NONE);
        }

        uint32_t availableCostumes = staff_get_available_entertainer_costumes();
        if (!(availableCostumes & (1 << static_cast<uint8_t>(_entertainerType))))
        {
            // Entertainer costume unavailable
            log_error("Tried to use unavailable entertainer type: %u", static_cast<uint32_t>(_entertainerType));

            return MakeResult(GameActions::Status::InvalidParameters, STR_NONE);
        }
    }

    // Look for a free slot in the staff modes.
    int32_t staffIndex;
    for (staffIndex = 0; staffIndex < STAFF_MAX_COUNT; ++staffIndex)
    {
        if (gStaffModes[staffIndex] == StaffMode::None)
            break;
    }

    if (staffIndex == STAFF_MAX_COUNT)
    {
        // Too many staff members exist already.
        return MakeResult(GameActions::Status::NoFreeElements, STR_TOO_MANY_STAFF_IN_GAME);
    }

    Peep* newPeep = &(create_sprite(SpriteIdentifier::Peep)->peep);
    if (newPeep == nullptr)
    {
        // Too many peeps exist already.
        return MakeResult(GameActions::Status::NoFreeElements, STR_TOO_MANY_PEOPLE_IN_GAME);
    }

    if (execute == false)
    {
        // In query we just want to see if we can obtain a sprite slot.
        sprite_remove(newPeep);
    }
    else
    {
        newPeep->sprite_identifier = SpriteIdentifier::Peep;
        newPeep->WindowInvalidateFlags = 0;
        newPeep->Action = PeepActionType::None2;
        newPeep->SpecialSprite = 0;
        newPeep->ActionSpriteImageOffset = 0;
        newPeep->WalkingFrameNum = 0;
        newPeep->ActionSpriteType = PeepActionSpriteType::None;
        newPeep->PathCheckOptimisation = 0;
        newPeep->AssignedPeepType = PeepType::Staff;
        newPeep->OutsideOfPark = false;
        newPeep->PeepFlags = 0;
        newPeep->PaidToEnter = 0;
        newPeep->PaidOnRides = 0;
        newPeep->PaidOnFood = 0;
        newPeep->PaidOnSouvenirs = 0;
        newPeep->FavouriteRide = RIDE_ID_NULL;
        newPeep->StaffOrders = _staffOrders;

        // We search for the first available Id for a given staff type
        uint32_t newStaffId = 0;
        for (;;)
        {
            bool found = false;
            ++newStaffId;
            for (auto searchPeep : EntityList<Staff>(EntityListId::Peep))
            {
                if (static_cast<uint8_t>(searchPeep->AssignedStaffType) != _staffType)
                    continue;

                if (searchPeep->Id == newStaffId)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
                break;
        }

        newPeep->Id = newStaffId;
        newPeep->AssignedStaffType = static_cast<StaffType>(_staffType);

        PeepSpriteType spriteType = spriteTypes[_staffType];
        if (_staffType == static_cast<uint8_t>(StaffType::Entertainer))
        {
            spriteType = EntertainerCostumeToSprite(_entertainerType);
        }
        newPeep->Name = nullptr;
        newPeep->SpriteType = spriteType;

        const rct_sprite_bounds* spriteBounds = &GetSpriteBounds(spriteType);
        newPeep->sprite_width = spriteBounds->sprite_width;
        newPeep->sprite_height_negative = spriteBounds->sprite_height_negative;
        newPeep->sprite_height_positive = spriteBounds->sprite_height_positive;

        if (_autoPosition)
        {
            AutoPositionNewStaff(newPeep);
        }
        else
        {
            // NOTE: This state is required for the window to act.
            newPeep->State = PeepState::Picked;

            newPeep->MoveTo({ newPeep->x, newPeep->y, newPeep->z });
        }

        // Staff uses this
        newPeep->As<Staff>()->SetHireDate(gDateMonthsElapsed);
        newPeep->PathfindGoal.x = 0xFF;
        newPeep->PathfindGoal.y = 0xFF;
        newPeep->PathfindGoal.z = 0xFF;
        newPeep->PathfindGoal.direction = INVALID_DIRECTION;

        uint8_t colour = staff_get_colour(static_cast<StaffType>(_staffType));
        newPeep->TshirtColour = colour;
        newPeep->TrousersColour = colour;

        // Staff energy determines their walking speed
        newPeep->Energy = 0x60;
        newPeep->EnergyTarget = 0x60;
        newPeep->StaffMowingTimeout = 0;

        newPeep->StaffId = staffIndex;

        gStaffModes[staffIndex] = StaffMode::Walk;

        for (int32_t i = 0; i < STAFF_PATROL_AREA_SIZE; i++)
        {
            gStaffPatrolAreas[staffIndex * STAFF_PATROL_AREA_SIZE + i] = 0;
        }

        res->peepSriteIndex = newPeep->sprite_index;
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
        for (auto guest : EntityList<Guest>(EntityListId::Peep))
        {
            if (guest->State == PeepState::Walking)
            {
                // Check the walking guest's tile. Only count them if they're on a path tile.
                guest_tile = map_get_path_element_at(TileCoordsXYZ{ guest->NextLoc });
                if (guest_tile != nullptr)
                    ++count;
            }
        }
    }

    CoordsXYZ newLocation{};
    if (count > 0)
    {
        // Place staff at a random guest
        uint32_t rand = scenario_rand_max(count);
        Guest* chosenGuest = nullptr;

        for (auto guest : EntityList<Guest>(EntityListId::Peep))
        {
            if (guest->State == PeepState::Walking)
            {
                guest_tile = map_get_path_element_at(TileCoordsXYZ{ guest->NextLoc });
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
            newLocation.x = chosenGuest->x;
            newLocation.y = chosenGuest->y;
            newLocation.z = chosenGuest->z;
        }
        else
        {
            // User must pick a location
            newPeep->State = PeepState::Picked;
            newLocation.x = newPeep->x;
            newLocation.y = newPeep->y;
            newLocation.z = newPeep->z;
        }
    }
    else
    {
        // No walking guests; pick random park entrance
        if (!gParkEntrances.empty())
        {
            auto rand = scenario_rand_max(static_cast<uint32_t>(gParkEntrances.size()));
            const auto& entrance = gParkEntrances[rand];
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
            newLocation.x = newPeep->x;
            newLocation.y = newPeep->y;
            newLocation.z = newPeep->z;
        }
    }

    newPeep->MoveTo(newLocation + CoordsXYZ{ 0, 0, 16 });
}
