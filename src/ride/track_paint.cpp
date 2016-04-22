/*****************************************************************************
* Copyright (c) 2014 Ted John
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* This file is part of OpenRCT2.
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "paint/paint.h"

extern "C"
{
    #include "../addresses.h"
    #include "../config.h"
    #include "../drawing/drawing.h"
    #include "../drawing/supports.h"
    #include "../interface/viewport.h"
    #include "../interface/window.h"
    #include "../localisation/localisation.h"
    #include "../sprites.h"
    #include "../world/map.h"
    #include "../world/sprite.h"
    #include "ride_data.h"
    #include "track_data.h"
    #include "track_paint.h"
}

bool TileDrawingContext::draw_98197C(uint32 imageId, sint8 offsetX, sint8 offsetY, sint16 lengthX, sint16 lengthY, sint16 offsetZ, sint32 height, sint16 boundOffsetX, sint16 boundOffsetY, sint16 boundOffsetZ)
{
    return sub_98197C(imageId, offsetX, offsetY, lengthX, lengthY, Z + offsetZ, height, boundOffsetX, boundOffsetY, Z + boundOffsetZ, ViewRotation);
}

bool TileDrawingContext::draw_98199C(uint32 imageId, sint8 offsetX, sint8 offsetY, sint16 lengthX, sint16 lengthY, sint16 offsetZ, sint32 height, sint16 boundOffsetX, sint16 boundOffsetY, sint16 boundOffsetZ)
{
    return sub_98199C(imageId, offsetX, offsetY, lengthX, lengthY, Z + offsetZ, height, boundOffsetX, boundOffsetY, Z + boundOffsetZ, ViewRotation);
}

bool TileDrawingContext::DrawSupports(supportStyle style, uint8 direction, uint16 special, uint32 imageFlags, sint16 zOffset, bool * underground)
{
    if (style == SUPPORT_STYLE_WOOD)
    {
        return wooden_a_supports_paint_setup(direction, special, Z + zOffset, imageFlags, underground);
    }
    return false;
}

void TileDrawingContext::UpdateTileMaxHeight(sint16 height, uint8 byte_0141E9DA)
{
    if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) < height)
    {
        RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = height;
        RCT2_GLOBAL(0x0141E9DA, uint8) = 32;
    }
}

void TileDrawingContext::SetSupportSegmentZ(sint8 subX, sint8 subY, uint16 height)
{
    switch (subX) {
    case -1:
        switch (subY) {
        case -1: RCT2_GLOBAL(0x141E9B4, uint16) = height; break;
        case  0: RCT2_GLOBAL(0x141E9C8, uint16) = height; break;
        case +1: RCT2_GLOBAL(0x141E9B8, uint16) = height; break;
        }
        break;
    case  0:
        switch (subY) {
        case -1: RCT2_GLOBAL(0x141E9CC, uint16) = height; break;
        case  0: RCT2_GLOBAL(0x141E9C4, uint16) = height; break;
        case +1: RCT2_GLOBAL(0x141E9D0, uint16) = height; break;
        }
        break;
    case +1:
        switch (subY) {
        case -1: RCT2_GLOBAL(0x141E9BC, uint16) = height; break;
        case  0: RCT2_GLOBAL(0x141E9D4, uint16) = height; break;
        case +1: RCT2_GLOBAL(0x141E9C0, uint16) = height; break;
        }
        break;
    }
}

void TileDrawingContext::SetSupportZ(uint16 height)
{
    for (sint8 y = -1; y <= 1; y++)
    {
        for (sint8 x = -1; x <= 1; x++)
        {
            SetSupportSegmentZ(x, y, height);
        }
    }
}

static rct_sxy8 GetEntranceCheckOffset(uint8 direction, uint8 rotation)
{
    const static rct_sxy8 entranceCheckOffsets[] = {
        { -1,  0 },
        {  0, -1 },
        {  1,  0 },
        {  0,  1 },

        {  0,  1 },
        { -1,  0 },
        {  0, -1 },
        {  1,  0 },

        {  1,  0 },
        {  0,  1 },
        { -1,  0 },
        {  0, -1 },

        {  0, -1 },
        {  1,  0 },
        {  0,  1 },
        { -1,  0 },
    };

#if DEBUG
    assert(direction <= 3);
    assert(rotation <= 3);
#endif
    return entranceCheckOffsets[(direction * 4) + rotation];
}

namespace RideDrawingUtils
{
    void DrawFloor(uint8 style, uint32 imageFlags, sint32 z, rct_ride * ride, TileDrawingContext * context)
    {
    }

    void DrawFence(uint8 fenceType, uint8 direction, sint32 z, rct_ride * ride, TileDrawingContext * context)
    {
    }

    void DrawFenceChecked(uint8 fenceType, uint8 direction, sint32 z, rct_ride * ride, rct_map_element * mapElement, TileDrawingContext * context)
    {
    }

    void DrawFencesChecked(uint8 fenceDirections, uint8 fenceType, sint32 z, rct_ride * ride, TileDrawingContext * context)
    {
    }
}

static RideDrawFunction GetRideDrawFunction(uint8 rideType)
{
    switch (rideType) {
    case RIDE_TYPE_FOOD_STALL:        return Shop::Draw;
    case RIDE_TYPE_1D:                return Shop::Draw;
    case RIDE_TYPE_DRINK_STALL:       return Shop::Draw;
    case RIDE_TYPE_1F:                return Shop::Draw;
    case RIDE_TYPE_SHOP:              return Shop::Draw;
    case RIDE_TYPE_22:                return Shop::Draw;
    case RIDE_TYPE_INFORMATION_KIOSK: return Shop::Draw;
    case RIDE_TYPE_TOILETS:           return Facility::Draw;
    case RIDE_TYPE_TOP_SPIN:          return TopSpin::Draw;
    case RIDE_TYPE_CASH_MACHINE:      return Shop::Draw;
    case RIDE_TYPE_FIRST_AID:         return Facility::Draw;
    }
    return nullptr;
}

extern "C" void viewport_track_paint_setup_2(uint8 rideIndex, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    rct_ride * ride = get_ride(rideIndex);
    uint8 trackType = mapElement->properties.track.type;
    uint8 trackSequence = mapElement->properties.track.sequence & 0x0F;

    RideDrawFunction drawFunction = GetRideDrawFunction(ride->type);
    if (drawFunction == nullptr)
    {
        TRACK_PAINT_FUNCTION * * trackTypeList = (TRACK_PAINT_FUNCTION**)RideTypeTrackPaintFunctionsOld[ride->type];
        uint32 * trackDirectionList = (uint32*)trackTypeList[trackType];

        // Have to call from this point as it pushes esi and expects callee to pop it
        RCT2_CALLPROC_X(0x006C4934,
                        ride->type,
                        (int)trackDirectionList,
                        direction,
                        height,
                        (int)mapElement,
                        rideIndex * sizeof(rct_ride),
                        trackSequence);
    }
    else
    {
        TileDrawingContext dc;
        dc.ViewRotation = get_current_rotation();
        dc.X = RCT2_GLOBAL(0x009DE56A, uint16);
        dc.X = RCT2_GLOBAL(0x009DE56E, uint16);
        dc.Z = height;

        drawFunction(ride, trackType, trackSequence, direction, mapElement, &dc);
    }
}
