/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StaffSetColourAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../entity/EntityList.h"
#include "../entity/Staff.h"
#include "../localisation/StringIds.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"

namespace OpenRCT2::GameActions
{
    StaffSetColourAction::StaffSetColourAction(StaffType staffType, Drawing::Colour colour)
        : _staffType(static_cast<uint8_t>(staffType))
        , _colour(colour)
    {
    }

    void StaffSetColourAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("staffType", _staffType);
        visitor.Visit("colour", _colour);
    }

    uint16_t StaffSetColourAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void StaffSetColourAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_staffType) << DS_TAG(_colour);
    }

    Result StaffSetColourAction::Query(GameState_t& gameState) const
    {
        auto staffType = static_cast<StaffType>(_staffType);
        if (staffType != StaffType::handyman && staffType != StaffType::mechanic && staffType != StaffType::security)
        {
            LOG_ERROR("Staff color can't be changed for staff type %d", _staffType);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_ACTION_INVALID_FOR_THAT_STAFF_TYPE);
        }
        return Result();
    }

    Result StaffSetColourAction::Execute(GameState_t& gameState) const
    {
        // Update global uniform colour property
        auto res = StaffSetColour(static_cast<StaffType>(_staffType), _colour);
        if (res.error != Status::ok)
        {
            return res;
        }

        // Update each staff member's uniform
        for (auto peep : EntityList<Staff>())
        {
            if (peep->AssignedStaffType == static_cast<StaffType>(_staffType))
            {
                peep->TshirtColour = _colour;
                peep->TrousersColour = _colour;
            }
        }

        GfxInvalidateScreen();
        return Result();
    }
} // namespace OpenRCT2::GameActions
