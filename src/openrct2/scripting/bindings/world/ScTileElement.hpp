/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "../../../Context.h"
    #include "../../../core/Guard.hpp"
    #include "../../../entity/EntityRegistry.h"
    #include "../../../ride/Track.h"
    #include "../../../world/Footpath.h"
    #include "../../../world/Scenery.h"
    #include "../../ScriptEngine.h"

    #include <cstdio>
    #include <cstring>
    #include <utility>

namespace OpenRCT2::Scripting
{
    class ScTileElement;
    extern ScTileElement gScTileElement;

    class ScTileElement : public ScBase
    {
    private:
        static JSValue type_get(JSContext* ctx, JSValue thisValue);
        static JSValue type_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue baseHeight_get(JSContext* ctx, JSValue thisValue);
        static JSValue baseHeight_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue baseZ_get(JSContext* ctx, JSValue thisValue);
        static JSValue baseZ_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue clearanceHeight_get(JSContext* ctx, JSValue thisValue);
        static JSValue clearanceHeight_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue clearanceZ_get(JSContext* ctx, JSValue thisValue);
        static JSValue clearanceZ_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue slope_get(JSContext* ctx, JSValue thisValue);
        static JSValue slope_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue waterHeight_get(JSContext* ctx, JSValue thisValue);
        static JSValue waterHeight_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue surfaceStyle_get(JSContext* ctx, JSValue thisValue);
        static JSValue surfaceStyle_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue edgeStyle_get(JSContext* ctx, JSValue thisValue);
        static JSValue edgeStyle_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue grassLength_get(JSContext* ctx, JSValue thisValue);
        static JSValue grassLength_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue hasOwnership_get(JSContext* ctx, JSValue thisValue);

        static JSValue hasConstructionRights_get(JSContext* ctx, JSValue thisValue);

        static JSValue ownership_get(JSContext* ctx, JSValue thisValue);
        static JSValue ownership_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue parkFences_get(JSContext* ctx, JSValue thisValue);
        static JSValue parkFences_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue trackType_get(JSContext* ctx, JSValue thisValue);
        static JSValue trackType_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue rideType_get(JSContext* ctx, JSValue thisValue);
        static JSValue rideType_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue sequence_get(JSContext* ctx, JSValue thisValue);
        static JSValue sequence_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue ride_get(JSContext* ctx, JSValue thisValue);
        static JSValue ride_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue station_get(JSContext* ctx, JSValue thisValue);
        static JSValue station_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue hasChainLift_get(JSContext* ctx, JSValue thisValue);
        static JSValue hasChainLift_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue mazeEntry_get(JSContext* ctx, JSValue thisValue);
        static JSValue mazeEntry_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue colourScheme_get(JSContext* ctx, JSValue thisValue);
        static JSValue colourScheme_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue seatRotation_get(JSContext* ctx, JSValue thisValue);
        static JSValue seatRotation_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue brakeBoosterSpeed_get(JSContext* ctx, JSValue thisValue);
        static JSValue brakeBoosterSpeed_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue isInverted_get(JSContext* ctx, JSValue thisValue);
        static JSValue isInverted_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue hasCableLift_get(JSContext* ctx, JSValue thisValue);
        static JSValue hasCableLift_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue isHighlighted_get(JSContext* ctx, JSValue thisValue);
        static JSValue isHighlighted_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue object_get(JSContext* ctx, JSValue thisValue);
        static JSValue object_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue isHidden_get(JSContext* ctx, JSValue thisValue);
        static JSValue isHidden_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue age_get(JSContext* ctx, JSValue thisValue);
        static JSValue age_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue quadrant_get(JSContext* ctx, JSValue thisValue);
        static JSValue quadrant_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue occupiedQuadrants_get(JSContext* ctx, JSValue thisValue);
        static JSValue occupiedQuadrants_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue isGhost_get(JSContext* ctx, JSValue thisValue);
        static JSValue isGhost_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue primaryColour_get(JSContext* ctx, JSValue thisValue);
        static JSValue primaryColour_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue secondaryColour_get(JSContext* ctx, JSValue thisValue);
        static JSValue secondaryColour_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue tertiaryColour_get(JSContext* ctx, JSValue thisValue);
        static JSValue tertiaryColour_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue bannerIndex_get(JSContext* ctx, JSValue thisValue);
        static JSValue bannerIndex_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        // Deprecated in favor of separate 'edges' and 'corners' properties,
        // left here to maintain compatibility with older plugins.
        static JSValue edgesAndCorners_get(JSContext* ctx, JSValue thisValue);
        static JSValue edgesAndCorners_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue edges_get(JSContext* ctx, JSValue thisValue);
        static JSValue edges_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue corners_get(JSContext* ctx, JSValue thisValue);
        static JSValue corners_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue slopeDirection_get(JSContext* ctx, JSValue thisValue);
        static JSValue slopeDirection_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue isQueue_get(JSContext* ctx, JSValue thisValue);
        static JSValue isQueue_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue queueBannerDirection_get(JSContext* ctx, JSValue thisValue);
        static JSValue queueBannerDirection_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue isBlockedByVehicle_get(JSContext* ctx, JSValue thisValue);
        static JSValue isBlockedByVehicle_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue isWide_get(JSContext* ctx, JSValue thisValue);
        static JSValue isWide_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue addition_get(JSContext* ctx, JSValue thisValue);
        static JSValue addition_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue surfaceObject_get(JSContext* ctx, JSValue thisValue);
        static JSValue surfaceObject_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue railingsObject_get(JSContext* ctx, JSValue thisValue);
        static JSValue railingsObject_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue additionStatus_get(JSContext* ctx, JSValue thisValue);
        static JSValue additionStatus_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue isAdditionBroken_get(JSContext* ctx, JSValue thisValue);
        static JSValue isAdditionBroken_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue isAdditionGhost_get(JSContext* ctx, JSValue thisValue);
        static JSValue isAdditionGhost_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue footpathObject_get(JSContext* ctx, JSValue thisValue);
        static JSValue footpathObject_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue footpathSurfaceObject_get(JSContext* ctx, JSValue thisValue);
        static JSValue footpathSurfaceObject_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue direction_get(JSContext* ctx, JSValue thisValue);
        static JSValue direction_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue bannerText_get(JSContext* ctx, JSValue thisValue);
        static JSValue bannerText_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static JSValue isNoEntry_get(JSContext* ctx, JSValue thisValue);
        static JSValue isNoEntry_set(JSContext* ctx, JSValue thisValue, JSValue jsValue);

        static void RemoveBannerEntryIfNeeded(TileElement* element, CoordsXY& coords);
        static void CreateBannerEntryIfNeeded(TileElement* element, CoordsXY& coords);

        static TileElement* GetTileElement(JSValue thisValue);

    public:
        static const LargeSceneryElement* GetOtherLargeSceneryElement(
            const CoordsXY& loc, const LargeSceneryElement* largeScenery);

        JSValue New(JSContext* ctx, TileElement* element, CoordsXY& coords);
        void Register(JSContext* ctx);

    private:
        static void Finalize(JSRuntime* rt, JSValue thisValue);
    };

} // namespace OpenRCT2::Scripting

#endif
