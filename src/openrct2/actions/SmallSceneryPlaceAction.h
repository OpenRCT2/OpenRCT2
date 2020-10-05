/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/TileElement.h"
#include "GameAction.h"

class SmallSceneryPlaceActionResult final : public GameActions::Result
{
public:
    SmallSceneryPlaceActionResult();
    SmallSceneryPlaceActionResult(GameActions::Status error);
    SmallSceneryPlaceActionResult(GameActions::Status error, rct_string_id message);
    SmallSceneryPlaceActionResult(GameActions::Status error, rct_string_id message, uint8_t* args);

    uint8_t GroundFlags{ 0 };
    TileElement* tileElement = nullptr;
};

DEFINE_GAME_ACTION(SmallSceneryPlaceAction, GameCommand::PlaceScenery, SmallSceneryPlaceActionResult)
{
private:
    CoordsXYZD _loc;
    uint8_t _quadrant{};
    ObjectEntryIndex _sceneryType{};
    uint8_t _primaryColour{};
    uint8_t _secondaryColour{};

public:
    SmallSceneryPlaceAction() = default;
    SmallSceneryPlaceAction(
        const CoordsXYZD& loc, uint8_t quadrant, ObjectEntryIndex sceneryType, uint8_t primaryColour, uint8_t secondaryColour);

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

    uint32_t GetCooldownTime() const override;
    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};
