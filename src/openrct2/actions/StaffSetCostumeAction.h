/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../peep/Staff.h"
#include "../world/Sprite.h"
#include "GameAction.h"

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

DEFINE_GAME_ACTION(StaffSetCostumeAction, GAME_COMMAND_SET_STAFF_COSTUME, GameActions::Result)
{
private:
    uint16_t _spriteIndex{ SPRITE_INDEX_NULL };
    EntertainerCostume _costume = EntertainerCostume::Count;

public:
    StaffSetCostumeAction() = default;
    StaffSetCostumeAction(uint16_t spriteIndex, EntertainerCostume costume)
        : _spriteIndex(spriteIndex)
        , _costume(costume)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
    }

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};
