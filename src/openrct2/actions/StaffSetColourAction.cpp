/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StaffSetColourAction.h"

#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../entity/EntityList.h"
#include "../entity/Staff.h"
#include "../localisation/StringIds.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"

StaffSetColourAction::StaffSetColourAction(StaffType staffType, uint8_t colour)
    : _staffType(static_cast<uint8_t>(staffType))
    , _colour(colour)
{
}

uint16_t StaffSetColourAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void StaffSetColourAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_staffType) << DS_TAG(_colour);
}

GameActions::Result StaffSetColourAction::Query() const
{
    auto staffType = static_cast<StaffType>(_staffType);
    if (staffType != StaffType::Handyman && staffType != StaffType::Mechanic && staffType != StaffType::Security)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }
    return GameActions::Result();
}

GameActions::Result StaffSetColourAction::Execute() const
{
    // Update global uniform colour property
    if (!staff_set_colour(static_cast<StaffType>(_staffType), _colour))
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
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

    gfx_invalidate_screen();
    return GameActions::Result();
}
