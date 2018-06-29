/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../interface/Colour.h"
#include "../drawing/Drawing.h"
#include "../world/Location.hpp"

struct rct_tile_element;

#pragma pack(push, 1)
/* size 0x12 */
struct attached_paint_struct {
    uint32_t image_id;        // 0x00
    union {
        uint32_t tertiary_colour;
        // If masked image_id is masked_id
        uint32_t colour_image_id;
    };
    uint16_t x;       // 0x08
    uint16_t y;       // 0x0A
    uint8_t flags;    // 0x0C
    uint8_t pad_0D;
    attached_paint_struct* next;    //0x0E
};
#ifdef PLATFORM_32BIT
// TODO: drop packing from this when all rendering is done.
assert_struct_size(attached_paint_struct, 0x12);
#endif

enum PAINT_QUADRANT_FLAGS {
    PAINT_QUADRANT_FLAG_IDENTICAL = (1 << 0),
    PAINT_QUADRANT_FLAG_BIGGER = (1 << 7),
    PAINT_QUADRANT_FLAG_NEXT = (1 << 1),
};

struct paint_struct_bound_box {
    uint16_t x;
    uint16_t y;
    uint16_t z;
    uint16_t x_end;
    uint16_t y_end;
    uint16_t z_end;
};

/* size 0x34 */
struct paint_struct {
    uint32_t image_id;        // 0x00
    union {
        uint32_t tertiary_colour; // 0x04
        // If masked image_id is masked_id
        uint32_t colour_image_id; // 0x04
    };
    paint_struct_bound_box bounds; // 0x08
    uint16_t x;               // 0x14
    uint16_t y;               // 0x16
    uint16_t quadrant_index;
    uint8_t flags;
    uint8_t quadrant_flags;
    attached_paint_struct* attached_ps; //0x1C
    paint_struct* var_20;
    paint_struct* next_quadrant_ps; // 0x24
    uint8_t sprite_type;      //0x28
    uint8_t var_29;
    uint16_t pad_2A;
    uint16_t map_x;           // 0x2C
    uint16_t map_y;           // 0x2E
    rct_tile_element *tileElement; // 0x30 (or sprite pointer)
};
#ifdef PLATFORM_32BIT
// TODO: drop packing from this when all rendering is done.
assert_struct_size(paint_struct, 0x34);
#endif

/* size 0x1E */
struct paint_string_struct {
    rct_string_id string_id;        // 0x00
    paint_string_struct *next;      // 0x02
    uint16_t x;                       // 0x06
    uint16_t y;                       // 0x08
    uint32_t args[4];                 // 0x0A
    uint8_t *y_offsets;               // 0x1A
};
#ifdef PLATFORM_32BIT
assert_struct_size(paint_string_struct, 0x1e);
#endif
#pragma pack(pop)

union paint_entry {
    paint_struct basic;
    attached_paint_struct attached;
    paint_string_struct string;
};

struct sprite_bb {
    uint32_t sprite_id;
    LocationXYZ16 offset;
    LocationXYZ16 bb_offset;
    LocationXYZ16 bb_size;
};

enum PAINT_STRUCT_FLAGS {
    PAINT_STRUCT_FLAG_IS_MASKED = (1 << 0)
};

struct support_height {
    uint16_t height;
    uint8_t slope;
    uint8_t pad;
};

struct tunnel_entry {
    uint8_t height;
    uint8_t type;
};

#define MAX_PAINT_QUADRANTS 512
#define TUNNEL_MAX_COUNT    65

struct paint_session
{
    rct_drawpixelinfo *      DPI;
    paint_entry              PaintStructs[4000];
    paint_struct *           Quadrants[MAX_PAINT_QUADRANTS];
    uint32_t                   QuadrantBackIndex;
    uint32_t                   QuadrantFrontIndex;
    const void *             CurrentlyDrawnItem;
    paint_entry *            EndOfPaintStructArray;
    paint_entry *            NextFreePaintStruct;
    LocationXY16             SpritePosition;
    paint_struct             UnkF1A4CC;
    paint_struct *           UnkF1AD28;
    attached_paint_struct *  UnkF1AD2C;
    uint8_t                    InteractionType;
    uint8_t                    CurrentRotation;
    support_height           SupportSegments[9];
    support_height           Support;
    paint_string_struct *    PSStringHead;
    paint_string_struct *    LastPSString;
    paint_struct *           WoodenSupportsPrependTo;
    LocationXY16             MapPosition;
    tunnel_entry             LeftTunnels[TUNNEL_MAX_COUNT];
    uint8_t                    LeftTunnelCount;
    tunnel_entry             RightTunnels[TUNNEL_MAX_COUNT];
    uint8_t                    RightTunnelCount;
    uint8_t                    VerticalTunnelHeight;
    const rct_tile_element * SurfaceElement;
    rct_tile_element *       PathElementOnSameHeight;
    rct_tile_element *       TrackElementOnSameHeight;
    bool                     DidPassSurface;
    uint8_t                    Unk141E9DB;
    uint16_t                   WaterHeight;
    uint32_t                   TrackColours[4];
};

extern paint_session gPaintSession;

// Globals for paint clipping
extern uint8_t gClipHeight;
extern LocationXY8 gClipSelectionA;
extern LocationXY8 gClipSelectionB;

/** rct2: 0x00993CC4. The white ghost that indicates not-yet-built elements. */
#define CONSTRUCTION_MARKER (COLOUR_DARK_GREEN << 19 | COLOUR_GREY << 24 | IMAGE_TYPE_REMAP);
extern bool gShowDirtyVisuals;
extern bool gPaintBoundingBoxes;
extern bool gPaintBlockedTiles;
extern bool gPaintWidePathsAsGhost;

paint_struct * sub_98196C(
    paint_session * session,
    uint32_t          image_id,
    int8_t           x_offset,
    int8_t           y_offset,
    int16_t          bound_box_length_x,
    int16_t          bound_box_length_y,
    int8_t           bound_box_length_z,
    int16_t          z_offset);
paint_struct * sub_98197C(
    paint_session * session,
    uint32_t          image_id,
    int8_t           x_offset,
    int8_t           y_offset,
    int16_t          bound_box_length_x,
    int16_t          bound_box_length_y,
    int8_t           bound_box_length_z,
    int16_t          z_offset,
    int16_t          bound_box_offset_x,
    int16_t          bound_box_offset_y,
    int16_t          bound_box_offset_z);
paint_struct * sub_98198C(
    paint_session * session,
    uint32_t          image_id,
    int8_t           x_offset,
    int8_t           y_offset,
    int16_t          bound_box_length_x,
    int16_t          bound_box_length_y,
    int8_t           bound_box_length_z,
    int16_t          z_offset,
    int16_t          bound_box_offset_x,
    int16_t          bound_box_offset_y,
    int16_t          bound_box_offset_z);
paint_struct * sub_98199C(
    paint_session * session,
    uint32_t          image_id,
    int8_t           x_offset,
    int8_t           y_offset,
    int16_t          bound_box_length_x,
    int16_t          bound_box_length_y,
    int8_t           bound_box_length_z,
    int16_t          z_offset,
    int16_t          bound_box_offset_x,
    int16_t          bound_box_offset_y,
    int16_t          bound_box_offset_z);

paint_struct * sub_98196C_rotated(paint_session * session, uint8_t direction, uint32_t image_id, int8_t x_offset, int8_t y_offset, int16_t bound_box_length_x, int16_t bound_box_length_y, int8_t bound_box_length_z, int16_t z_offset);
paint_struct * sub_98197C_rotated(paint_session * session, uint8_t direction, uint32_t image_id, int8_t x_offset, int8_t y_offset, int16_t bound_box_length_x, int16_t bound_box_length_y, int8_t bound_box_length_z, int16_t z_offset, int16_t bound_box_offset_x, int16_t bound_box_offset_y, int16_t bound_box_offset_z);
paint_struct * sub_98199C_rotated(paint_session * session, uint8_t direction, uint32_t image_id, int8_t x_offset, int8_t y_offset, int16_t bound_box_length_x, int16_t bound_box_length_y, int8_t bound_box_length_z, int16_t z_offset, int16_t bound_box_offset_x, int16_t bound_box_offset_y, int16_t bound_box_offset_z);

void paint_util_push_tunnel_rotated(paint_session * session, uint8_t direction, uint16_t height, uint8_t type);

bool paint_attach_to_previous_attach(paint_session * session, uint32_t image_id, uint16_t x, uint16_t y);
bool paint_attach_to_previous_ps(paint_session * session, uint32_t image_id, uint16_t x, uint16_t y);
void paint_floating_money_effect(paint_session * session, money32 amount, rct_string_id string_id, int16_t y, int16_t z, int8_t y_offsets[], int16_t offset_x, uint32_t rotation);

paint_session * paint_session_alloc(rct_drawpixelinfo * dpi);
void paint_session_free(paint_session *);
void paint_session_generate(paint_session * session);
paint_struct paint_session_arrange(paint_session * session);
paint_struct * paint_arrange_structs_helper(paint_struct * ps_next, uint16_t quadrantIndex, uint8_t flag, uint8_t rotation);
void paint_draw_structs(rct_drawpixelinfo * dpi, paint_struct * ps, uint32_t viewFlags);
void paint_draw_money_structs(rct_drawpixelinfo * dpi, paint_string_struct * ps);

// TESTING
#ifdef __TESTPAINT__
    void testpaint_clear_ignore();
    void testpaint_ignore(uint8_t direction, uint8_t trackSequence);
    void testpaint_ignore_all();
    bool testpaint_is_ignored(uint8_t direction, uint8_t trackSequence);

    #define TESTPAINT_IGNORE(direction, trackSequence) testpaint_ignore(direction, trackSequence)
    #define TESTPAINT_IGNORE_ALL() testpaint_ignore_all()
#else
    #define TESTPAINT_IGNORE(direction, trackSequence)
    #define TESTPAINT_IGNORE_ALL()
#endif
