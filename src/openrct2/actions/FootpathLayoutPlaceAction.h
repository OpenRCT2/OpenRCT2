/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Footpath.h"
#include "GameAction.h"

namespace OpenRCT2::GameActions
{
    class FootpathLayoutPlaceAction final : public GameActionBase<GameCommand::PlacePathLayout>
    {
    private:
        CoordsXYZ _loc;
        uint8_t _slope{};
        ObjectEntryIndex _type{};
        ObjectEntryIndex _railingsType{};
        uint8_t _edges{};
        PathConstructFlags _constructFlags{};

    public:
        FootpathLayoutPlaceAction() = default;
        FootpathLayoutPlaceAction(
            const CoordsXYZ& loc, uint8_t slope, ObjectEntryIndex type, ObjectEntryIndex railingsType, uint8_t edges,
            PathConstructFlags constructFlags = 0);

        void AcceptParameters(GameActionParameterVisitor& visitor) override;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query() const override;
        Result Execute() const override;

    private:
        Result ElementInsertQuery(Result res) const;
        Result ElementInsertExecute(Result res) const;
        bool IsSameAsEntranceElement(const EntranceElement& entranceElement) const;
    };
} // namespace OpenRCT2::GameActions
