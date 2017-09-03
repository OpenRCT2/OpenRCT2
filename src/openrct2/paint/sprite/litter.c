#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "sprite.h"
#include "../../world/sprite.h"
#include "../../interface/viewport.h"
#include "../paint.h"

enum {
    SPR_LITTER_SICK = 23101,

    SPR_LITTER_SICK_ALT = 23103,

    SPR_LITTER_EMPTY_CAN = 23105,

    SPR_LITTER_RUBBISH = 23107,

    SPR_LITTER_EMPTY_BURGER_BOX = 23109,

    SPR_LITTER_EMPTY_CUP = 23111,

    SPR_LITTER_EMPTY_BOX = 23113,

    SPR_LITTER_EMPTY_BOTTLE = 23115,

    SPR_LITTER_EMPTY_BOWL_RED = 23117,

    SPR_LITTER_EMPTY_DRINK_CART = 23121,

    SPR_LITTER_EMPTY_JUICE_CUP = 23125,

    SPR_LITTER_EMPTY_BOWL_BLUE = 23129,
};

typedef struct litter_sprite {
    uint16 base_id;
    uint8 direction_mask;
} litter_sprite;

/** rct2: 0x0097EF6C */
static const litter_sprite litter_sprites[] = {
    { SPR_LITTER_SICK,              0x1 },
    { SPR_LITTER_SICK_ALT,          0x1 },
    { SPR_LITTER_EMPTY_CAN,         0x1 },
    { SPR_LITTER_RUBBISH,           0x1 },
    { SPR_LITTER_EMPTY_BURGER_BOX,  0x1 },
    { SPR_LITTER_EMPTY_CUP,         0x1 },
    { SPR_LITTER_EMPTY_BOX,         0x1 },
    { SPR_LITTER_EMPTY_BOTTLE,      0x1 },
    { SPR_LITTER_EMPTY_BOWL_RED,    0x3 },
    { SPR_LITTER_EMPTY_DRINK_CART,  0x3 },
    { SPR_LITTER_EMPTY_JUICE_CUP,   0x3 },
    { SPR_LITTER_EMPTY_BOWL_BLUE,   0x3 },
};

/**
 * Litter Paint Setup
 *  rct2: 0x006736FC
 */
void litter_paint(paint_session * session, rct_litter *litter, sint32 imageDirection)
{
    rct_drawpixelinfo *dpi;

    dpi = session->Unk140E9A8;
    if (dpi->zoom_level != 0) return; // If zoomed at all no litter drawn

    // litter has no sprite direction so remove that
    imageDirection >>= 3;
    // Some litter types have only 1 direction so remove
    // anything that isn't required.
    imageDirection &= litter_sprites[litter->type].direction_mask;

    uint32 image_id = imageDirection + litter_sprites[litter->type].base_id;

    sub_98197C(session, image_id, 0, 0, 4, 4, -1, litter->z, -4, -4, litter->z + 2, get_current_rotation());
}
