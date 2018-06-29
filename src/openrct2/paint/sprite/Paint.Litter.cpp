/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.Sprite.h"
#include "../../world/Sprite.h"
#include "../../interface/Viewport.h"
#include "../Paint.h"

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

struct litter_sprite {
    uint16_t base_id;
    uint8_t direction_mask;
};

/** rct2: 0x0097EF6C */
static constexpr const litter_sprite litter_sprites[] = {
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
void litter_paint(paint_session * session, const rct_litter *litter, int32_t imageDirection)
{
    rct_drawpixelinfo *dpi;

    dpi = session->DPI;
    if (dpi->zoom_level != 0) return; // If zoomed at all no litter drawn

    // litter has no sprite direction so remove that
    imageDirection >>= 3;
    // Some litter types have only 1 direction so remove
    // anything that isn't required.
    imageDirection &= litter_sprites[litter->type].direction_mask;

    uint32_t image_id = imageDirection + litter_sprites[litter->type].base_id;

    // In the following call to sub_98197C, we add 4 (instead of 2) to the
    //  bound_box_offset_z to make sure litter is drawn on top of railways
    sub_98197C(session, image_id, 0, 0, 4, 4, -1, litter->z, -4, -4, litter->z + 4);
}
