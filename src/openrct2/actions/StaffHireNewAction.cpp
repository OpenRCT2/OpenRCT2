/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include "../entity/EntityRegistry.h"
#include "../entity/Staff.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/Ride.h"
#include "../scenario/Scenario.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../world/Entrance.h"
#include "../world/Park.h"

/* rct2: 0x009929FC */
static constexpr const PeepSpriteType spriteTypes[] = {
    PeepSpriteType::Handyman,
    PeepSpriteType::Mechanic,
    PeepSpriteType::Security,
    PeepSpriteType::EntertainerPanda,
};

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
        // Invalid staff type.
        LOG_ERROR("Tried to use invalid staff type: %u", static_cast<uint32_t>(_staffType));

        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_HIRE_NEW_STAFF, STR_NONE);
    }

    if (GetNumFreeEntities() < 400)
    {
        return GameActions::Result(GameActions::Status::NoFreeElements, STR_CANT_HIRE_NEW_STAFF, STR_TOO_MANY_PEOPLE_IN_GAME);
    }

    if (_staffType == static_cast<uint8_t>(StaffType::Entertainer))
    {
        if (static_cast<uint8_t>(_entertainerType) >= static_cast<uint8_t>(EntertainerCostume::Count))
        {
            // Invalid entertainer costume
            LOG_ERROR("Tried to use invalid entertainer type: %u", static_cast<uint32_t>(_entertainerType));

            return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_HIRE_NEW_STAFF, STR_NONE);
        }

        uint32_t availableCostumes = StaffGetAvailableEntertainerCostumes();
        if (!(availableCostumes & (1 << static_cast<uint8_t>(_entertainerType))))
        {
            // Entertainer costume unavailable
            LOG_ERROR("Tried to use unavailable entertainer type: %u", static_cast<uint32_t>(_entertainerType));

            return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_HIRE_NEW_STAFF, STR_NONE);
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
        newPeep->ActionSpriteImageOffset = 0;
        newPeep->WalkingFrameNum = 0;
        newPeep->ActionSpriteType = PeepActionSpriteType::None;
        newPeep->PathCheckOptimisation = 0;
        newPeep->PeepFlags = 0;
        newPeep->StaffLawnsMown = 0;
        newPeep->StaffGardensWatered = 0;
        newPeep->StaffLitterSwept = 0;
        newPeep->StaffBinsEmptied = 0;
        newPeep->StaffOrders = _staffOrders;

        // We search for the first available Id for a given staff type
        uint32_t newStaffId = 0;
        for (;;)
        {
            bool found = false;
            ++newStaffId;
            for (auto searchPeep : EntityList<Staff>())
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

        const SpriteBounds* spriteBounds = &GetSpriteBounds(spriteType);
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

            // INVESTIGATE: x and y are LOCATION_NULL at this point.
            newPeep->MoveTo(newPeep->GetLocation());
        }

        // Staff uses this
        newPeep->As<Staff>()->SetHireDate(gDateMonthsElapsed);
        newPeep->PathfindGoal.x = 0xFF;
        newPeep->PathfindGoal.y = 0xFF;
        newPeep->PathfindGoal.z = 0xFF;
        newPeep->PathfindGoal.direction = INVALID_DIRECTION;

        uint8_t colour = StaffGetColour(static_cast<StaffType>(_staffType));
        newPeep->TshirtColour = colour;
        newPeep->TrousersColour = colour;

        // Staff energy determines their walking speed
        newPeep->Energy = 0x60;
        newPeep->EnergyTarget = 0x60;
        newPeep->StaffMowingTimeout = 0;

        newPeep->PatrolInfo = nullptr;

        res.SetData(StaffHireNewActionResult{ newPeep->sprite_index });
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
        if (!gParkEntrances.empty())
        {
            auto rand = ScenarioRandMax(static_cast<uint32_t>(gParkEntrances.size()));
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
            newLocation = newPeep->GetLocation();
        }
    }

    newPeep->MoveTo(newLocation + CoordsXYZ{ 0, 0, 16 });
}
