/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

enum class LandBuyRightSetting : uint8_t
{
    BuyLand,
    BuyConstructionRights,
    Count
};

class LandBuyRightsAction final : public GameActionBase<GameCommand::BuyLandRights>
{
private:
    MapRange _range;
    LandBuyRightSetting _setting{ LandBuyRightSetting::Count };

    constexpr static StringId kErrorTitles[] = {
        STR_CANT_BUY_LAND,
        STR_CANT_BUY_CONSTRUCTION_RIGHTS_HERE,
    };

public:
    LandBuyRightsAction() = default;
    LandBuyRightsAction(const MapRange& range, LandBuyRightSetting setting);
    LandBuyRightsAction(const CoordsXY& coord, LandBuyRightSetting setting);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;

private:
    OpenRCT2::GameActions::Result QueryExecute(bool isExecuting) const;
    OpenRCT2::GameActions::Result MapBuyLandRightsForTile(const CoordsXY& loc, bool isExecuting) const;
};
