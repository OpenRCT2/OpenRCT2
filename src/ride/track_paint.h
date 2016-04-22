#ifndef _TRACK_PAINT_H
#define _TRACK_PAINT_H

#ifdef __cplusplus
extern "C"
{
#endif
    #include "../common.h"
    #include "../world/map.h"
    #include "ride.h"
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

constexpr uint16 NO_SUPPORT = 0xFFFF;

enum
{
    DF_NE = 1 << 0,
    DF_SE = 1 << 1,
    DF_SW = 1 << 2,
    DF_NW = 1 << 3,
};

enum
{
    FLOOR_STYLE_TILE_SMALL,
    FLOOR_STYLE_TILE_LARGE,
    FLOOR_STYLE_BROWN_RUBBER,
    FLOOR_STYLE_STEEL,
};

enum supportStyle
{
    SUPPORT_STYLE_WOOD
};

enum
{
    FLAT_RIDE_ELEM_MOTION_SIMULATOR  = 110,
    FLAT_RIDE_ELEM_FACILITY          = 118,
    FLAT_RIDE_ELEM_SHOP              = 118,
    FLAT_RIDE_ELEM_INFORMATION_KIOSK = 121,
    FLAT_RIDE_ELEM_TOP_SPIN          = 123,
};

class TileDrawingContext
{
public:
    uint8             ViewRotation;

    sint32            X;
    sint32            Y;
    sint32            Z;

    bool draw_98196C(uint32 imageId, sint8 offsetX, sint8 offsetY, sint16 lengthX, sint16 lengthY, sint8 lengthZ, sint16 offsetZ);
    bool draw_98197C(uint32 imageId, sint8 offsetX, sint8 offsetY, sint16 lengthX, sint16 lengthY, sint8 lengthZ, sint16 offsetZ, sint16 boundOffsetX, sint16 boundOffsetY, sint16 boundOffsetZ);
    bool draw_98199C(uint32 imageId, sint8 offsetX, sint8 offsetY, sint16 lengthX, sint16 lengthY, sint8 lengthZ, sint16 offsetZ, sint16 boundOffsetX, sint16 boundOffsetY, sint16 boundOffsetZ);
    bool DrawSupports(supportStyle style, uint8 direction, uint16 special, uint32 imageFlags, sint16 offsetZ = 0, bool * underground = nullptr);
    void UpdateTileMaxHeight(sint16 height, uint8 byte_0141E9DA);

    void SetSupportSegmentZ(sint8 subX, sint8 subY, uint16 height);
    void SetSupportZ(uint16 height);
};

namespace RideDrawingUtils
{
    void DrawFloor(uint8 floorType, uint32 imageFlags, sint32 z, rct_ride *ride, TileDrawingContext *context);
    void DrawFence(uint8 fenceType, uint8 direction, sint32 z, rct_ride *ride, TileDrawingContext *context);
    void DrawFenceChecked(uint8 fenceType, uint8 direction, sint32 z, rct_ride *ride, TileDrawingContext *context);
    void DrawFencesChecked(uint8 fenceDirections, uint8 fenceType, sint32 z, rct_ride * ride, TileDrawingContext * context);
};

typedef void (*RideDrawFunction)(rct_ride * ride, uint8 trackType, uint8 trackSequence, uint8 direction, rct_map_element * mapElement, TileDrawingContext * context);

#endif /* __cplusplus */

#ifdef __cplusplus
    extern "C"
    {
#endif

        void viewport_track_paint_setup_2(uint8 rideIndex, uint8 direction, sint32 height, rct_map_element * mapElement);

#ifdef __cplusplus
    }
#endif

typedef void (*TRACK_PAINT_FUNCTION)(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element* mapElement);
typedef TRACK_PAINT_FUNCTION (*TRACK_PAINT_FUNCTION_GETTER)(int trackType, int direction);

#endif
