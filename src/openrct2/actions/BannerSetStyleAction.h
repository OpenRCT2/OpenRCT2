/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

// There is also the BannerSetColourAction that sets primary colour but this action takes banner index rather than x, y, z,
// direction
enum class BannerSetStyleType : uint8_t
{
    PrimaryColour,
    TextColour,
    NoEntry,
    Count
};

class BannerSetStyleAction final : public GameActionBase<GameCommand::SetBannerStyle>
{
private:
    BannerSetStyleType _type{ BannerSetStyleType::Count };
    BannerIndex _bannerIndex{ BannerIndex::GetNull() };
    uint8_t _parameter{};

public:
    BannerSetStyleAction() = default;
    BannerSetStyleAction(BannerSetStyleType type, BannerIndex bannerIndex, uint8_t parameter);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;
};
