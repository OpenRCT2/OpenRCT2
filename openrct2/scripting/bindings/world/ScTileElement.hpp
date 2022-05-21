/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../../../Context.h"
#    include "../../../common.h"
#    include "../../../core/Guard.hpp"
#    include "../../../entity/EntityRegistry.h"
#    include "../../../ride/Track.h"
#    include "../../../world/Footpath.h"
#    include "../../../world/Scenery.h"
#    include "../../../world/Surface.h"
#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"

#    include <cstdio>
#    include <cstring>
#    include <utility>

namespace OpenRCT2::Scripting
{
    class ScTileElement
    {
    protected:
        CoordsXY _coords;
        TileElement* _element;

    public:
        ScTileElement(const CoordsXY& coords, TileElement* element);

    private:
        std::string type_get() const;
        void type_set(std::string value);

        uint8_t baseHeight_get() const;
        void baseHeight_set(uint8_t newBaseHeight);

        uint16_t baseZ_get() const;
        void baseZ_set(uint16_t value);

        uint8_t clearanceHeight_get() const;
        void clearanceHeight_set(uint8_t newClearanceHeight);

        uint16_t clearanceZ_get() const;
        void clearanceZ_set(uint16_t value);

        DukValue slope_get() const;
        void slope_set(uint8_t value);

        DukValue waterHeight_get() const;
        void waterHeight_set(int32_t value);

        DukValue surfaceStyle_get() const;
        void surfaceStyle_set(uint32_t value);

        DukValue edgeStyle_get() const;
        void edgeStyle_set(uint32_t value);

        DukValue grassLength_get() const;
        void grassLength_set(uint8_t value);

        DukValue hasOwnership_get() const;

        DukValue hasConstructionRights_get();

        DukValue ownership_get() const;
        void ownership_set(uint8_t value);

        DukValue parkFences_get() const;
        void parkFences_set(uint8_t value);

        DukValue trackType_get() const;
        void trackType_set(uint16_t value);

        DukValue rideType_get() const;
        void rideType_set(uint16_t value);

        DukValue sequence_get() const;
        void sequence_set(const DukValue& value);

        DukValue ride_get() const;
        void ride_set(const DukValue& value);

        DukValue station_get() const;
        void station_set(const DukValue& value);

        DukValue hasChainLift_get() const;
        void hasChainLift_set(bool value);

        DukValue mazeEntry_get() const;
        void mazeEntry_set(const DukValue& value);

        DukValue colourScheme_get() const;
        void colourScheme_set(const DukValue& value);

        DukValue seatRotation_get() const;
        void seatRotation_set(const DukValue& value);

        DukValue brakeBoosterSpeed_get() const;
        void brakeBoosterSpeed_set(const DukValue& value);

        DukValue isInverted_get() const;
        void isInverted_set(bool value);

        DukValue hasCableLift_get() const;
        void hasCableLift_set(bool value);

        DukValue object_get() const;
        void object_set(const DukValue& value);

        bool isHidden_get() const;
        void isHidden_set(bool hide);

        DukValue age_get() const;
        void age_set(uint8_t value);

        DukValue quadrant_get() const;
        void quadrant_set(uint8_t value);

        uint8_t occupiedQuadrants_get() const;
        void occupiedQuadrants_set(uint8_t value);

        bool isGhost_get() const;
        void isGhost_set(bool value);

        DukValue primaryColour_get() const;
        void primaryColour_set(uint8_t value);

        DukValue secondaryColour_get() const;
        void secondaryColour_set(uint8_t value);

        DukValue tertiaryColour_get() const;
        void tertiaryColour_set(uint8_t value);

        DukValue bannerIndex_get() const;
        void bannerIndex_set(const DukValue& value);

        // Deprecated in favor of seperate 'edges' and 'corners' properties,
        // left here to maintain compatibility with older plugins.
        /** @deprecated */
        uint8_t edgesAndCorners_get() const;
        /** @deprecated */
        void edgesAndCorners_set(uint8_t value);

        DukValue edges_get() const;
        void edges_set(uint8_t value);

        DukValue corners_get() const;
        void corners_set(uint8_t value);

        DukValue slopeDirection_get() const;
        void slopeDirection_set(const DukValue& value);

        DukValue isQueue_get() const;
        void isQueue_set(bool value);

        DukValue queueBannerDirection_get() const;
        void queueBannerDirection_set(const DukValue& value);

        DukValue isBlockedByVehicle_get() const;
        void isBlockedByVehicle_set(bool value);

        DukValue isWide_get() const;
        void isWide_set(bool value);

        DukValue addition_get() const;
        void addition_set(const DukValue& value);

        DukValue surfaceObject_get() const;
        void surfaceObject_set(const DukValue& value);

        DukValue railingsObject_get() const;
        void railingsObject_set(const DukValue& value);

        DukValue additionStatus_get() const;
        void additionStatus_set(const DukValue& value);

        DukValue isAdditionBroken_get() const;
        void isAdditionBroken_set(const DukValue& value);

        DukValue isAdditionGhost_get() const;
        void isAdditionGhost_set(const DukValue& value);

        DukValue footpathObject_get() const;
        void footpathObject_set(const DukValue& value);

        DukValue footpathSurfaceObject_get() const;
        void footpathSurfaceObject_set(const DukValue& value);

        DukValue direction_get() const;
        void direction_set(uint8_t value);

        void Invalidate();

    public:
        static void Register(duk_context* ctx);
    };

} // namespace OpenRCT2::Scripting

#endif
