/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MapOwnership.h"

#include "Location.hpp"
#include "TileElementsView.h"
#include "tile_element/EntranceElement.h"

namespace OpenRCT2
{
    OwnershipFlags CheckMaxAllowableLandRightsForTile(const CoordsXYZ& tileMapPos)
    {
        OwnershipFlags destOwnership = { OwnershipFlag::landOwned };

        const auto tilePos = TileCoordsXYZ{ tileMapPos };
        for (const auto* tileElement : TileElementsView(tileMapPos))
        {
            auto type = tileElement->getType();
            if (type == TileElementType::path
                || (type == TileElementType::entrance
                    && tileElement->asEntrance()->getEntranceType() == EntranceType::parkEntrance))
            {
                destOwnership = OwnershipFlag::constructionRightsOwned;
                // Do not own construction rights if too high/below surface
                if (tileElement->baseHeight - kConstructionRightsClearanceSmall > tilePos.z
                    || tileElement->baseHeight < tilePos.z)
                {
                    destOwnership = kUnowned;
                    break;
                }
            }
        }

        return destOwnership;
    }
} // namespace OpenRCT2
