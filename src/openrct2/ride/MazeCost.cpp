/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MazeCost.h"

#include "../core/Money.hpp"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "../world/tile_element/SurfaceElement.h"
#include "Ride.h"
#include "RideData.h"
#include "TrackData.h"

using namespace OpenRCT2::TrackMetaData;

namespace OpenRCT2
{
    money64 MazeCalculateCost(money64 constructionCost, const Ride& ride, const CoordsXYZ& loc)
    {
        const auto& ted = GetTrackElementDescriptor(TrackElemType::maze);
        money64 price = (ride.getRideTypeDescriptor().BuildCosts.TrackPrice * ted.priceModifier) >> 16;

        auto surfaceElement = MapGetSurfaceElementAt(loc);
        auto heightDifference = (loc.z - surfaceElement->GetBaseZ()) / kCoordsZPerTinyZ;
        money64 supportCost = heightDifference * ride.getRideTypeDescriptor().BuildCosts.SupportPrice;

        return constructionCost + price + supportCost;
    }
} // namespace OpenRCT2
