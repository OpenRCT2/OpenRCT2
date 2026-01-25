/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SmallScenerySetColourAction.h"

#include "../Cheats.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/Ride.h"
#include "../ride/TrackDesign.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/tile_element/SmallSceneryElement.h"

namespace OpenRCT2::GameActions
{
    SmallScenerySetColourAction::SmallScenerySetColourAction(
        const CoordsXYZ& loc, uint8_t quadrant, ObjectEntryIndex sceneryType, Drawing::Colour primaryColour,
        Drawing::Colour secondaryColour, Drawing::Colour tertiaryColour)
        : _loc(loc)
        , _quadrant(quadrant)
        , _sceneryType(sceneryType)
        , _primaryColour(primaryColour)
        , _secondaryColour(secondaryColour)
        , _tertiaryColour(tertiaryColour)
    {
    }

    void SmallScenerySetColourAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit(_loc);
        visitor.Visit("quadrant", _quadrant);
        visitor.Visit("sceneryType", _sceneryType);
        visitor.Visit("primaryColour", _primaryColour);
        visitor.Visit("secondaryColour", _secondaryColour);
        visitor.Visit("tertiaryColour", _tertiaryColour);
    }

    uint16_t SmallScenerySetColourAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void SmallScenerySetColourAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc) << DS_TAG(_quadrant) << DS_TAG(_sceneryType) << DS_TAG(_primaryColour)
               << DS_TAG(_secondaryColour) << DS_TAG(_tertiaryColour);
    }

    Result SmallScenerySetColourAction::Query(GameState_t& gameState) const
    {
        return QueryExecute(gameState, false);
    }

    Result SmallScenerySetColourAction::Execute(GameState_t& gameState) const
    {
        return QueryExecute(gameState, true);
    }

    Result SmallScenerySetColourAction::QueryExecute(GameState_t& gameState, bool isExecuting) const
    {
        auto res = Result();
        res.expenditure = ExpenditureType::landscaping;
        res.position.x = _loc.x + 16;
        res.position.y = _loc.y + 16;
        res.position.z = _loc.z;
        res.errorTitle = STR_CANT_REPAINT_THIS;

        if (!LocationValid(_loc))
        {
            return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, STR_OFF_EDGE_OF_MAP);
        }

        if (gLegacyScene != LegacyScene::scenarioEditor && !gameState.cheats.sandboxMode)
        {
            if (!MapIsLocationOwned(_loc))
            {
                return Result(Status::notOwned, STR_CANT_REPAINT_THIS, STR_LAND_NOT_OWNED_BY_PARK);
            }
        }

        auto sceneryElement = MapGetSmallSceneryElementAt(_loc, _sceneryType, _quadrant);

        if (sceneryElement == nullptr)
        {
            LOG_ERROR("Small scenery not found at: x = %d, y = %d, z = %d", _loc.x, _loc.y, _loc.z);
            return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, kStringIdNone);
        }

        if ((GetFlags().has(CommandFlag::ghost)) && !(sceneryElement->IsGhost()))
        {
            return res;
        }

        if (isExecuting)
        {
            sceneryElement->SetPrimaryColour(_primaryColour);
            sceneryElement->SetSecondaryColour(_secondaryColour);
            sceneryElement->SetTertiaryColour(_tertiaryColour);

            MapInvalidateTileFull(_loc);
        }

        return res;
    }
} // namespace OpenRCT2::GameActions
