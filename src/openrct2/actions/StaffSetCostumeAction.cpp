/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StaffSetCostumeAction.h"

#include "../Context.h"
#include "../entity/EntityRegistry.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../windows/Intent.h"

/** rct2: 0x00982134 */
constexpr const bool peep_slow_walking_types[] = {
    false, // PeepSpriteType::Normal
    false, // PeepSpriteType::Handyman
    false, // PeepSpriteType::Mechanic
    false, // PeepSpriteType::Security
    false, // PeepSpriteType::EntertainerPanda
    false, // PeepSpriteType::EntertainerTiger
    false, // PeepSpriteType::EntertainerElephant
    false, // PeepSpriteType::EntertainerRoman
    false, // PeepSpriteType::EntertainerGorilla
    false, // PeepSpriteType::EntertainerSnowman
    false, // PeepSpriteType::EntertainerKnight
    true,  // PeepSpriteType::EntertainerAstronaut
    false, // PeepSpriteType::EntertainerBandit
    false, // PeepSpriteType::EntertainerSheriff
    true,  // PeepSpriteType::EntertainerPirate
    true,  // PeepSpriteType::Balloon
};

StaffSetCostumeAction::StaffSetCostumeAction(EntityId spriteIndex, EntertainerCostume costume)
    : _spriteIndex(spriteIndex)
    , _costume(costume)
{
}

uint16_t StaffSetCostumeAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void StaffSetCostumeAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_spriteIndex) << DS_TAG(_costume);
}

GameActions::Result StaffSetCostumeAction::Query() const
{
    if (_spriteIndex.ToUnderlying() >= MAX_ENTITIES || _spriteIndex.IsNull())
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    auto* staff = TryGetEntity<Staff>(_spriteIndex);
    if (staff == nullptr)
    {
        log_warning("Invalid game command for sprite %u", _spriteIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    auto spriteType = EntertainerCostumeToSprite(_costume);
    if (EnumValue(spriteType) > std::size(peep_slow_walking_types))
    {
        log_warning("Invalid game command for sprite %u", _spriteIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }
    return GameActions::Result();
}

GameActions::Result StaffSetCostumeAction::Execute() const
{
    auto* staff = TryGetEntity<Staff>(_spriteIndex);
    if (staff == nullptr)
    {
        log_warning("Invalid game command for sprite %u", _spriteIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    auto spriteType = EntertainerCostumeToSprite(_costume);
    staff->SpriteType = spriteType;
    staff->PeepFlags &= ~PEEP_FLAGS_SLOW_WALK;
    if (peep_slow_walking_types[EnumValue(spriteType)])
    {
        staff->PeepFlags |= PEEP_FLAGS_SLOW_WALK;
    }
    staff->ActionFrame = 0;
    staff->UpdateCurrentActionSpriteType();
    staff->Invalidate();

    window_invalidate_by_number(WindowClass::Peep, _spriteIndex);
    auto intent = Intent(INTENT_ACTION_REFRESH_STAFF_LIST);
    context_broadcast_intent(&intent);

    auto res = GameActions::Result();
    res.Position = staff->GetLocation();

    return res;
}
