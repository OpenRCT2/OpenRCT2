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

#include "../../config/Config.h"
#include "../../drawing/lightfx.h"
#include "../../interface/viewport.h"
#include "../../paint/sprite/sprite.h"
#include "../../peep/peep.h"
#include "../../world/sprite.h"
#include "../paint.h"

/**
 *
 *  rct2: 0x0068F0FB
 */
void peep_paint(paint_session * session, rct_peep * peep, sint32 imageDirection)
{
#ifdef __ENABLE_LIGHTFX__
    if (gConfigGeneral.enable_light_fx) {
        if (peep->type == PEEP_TYPE_STAFF) {
            sint16 peep_x, peep_y, peep_z;

            peep_x = peep->x;
            peep_y = peep->y;
            peep_z = peep->z;

            switch (peep->sprite_direction) {
            case 0:
                peep_x -= 10;
                break;
            case 8:
                peep_y += 10;
                break;
            case 16:
                peep_x += 10;
                break;
            case 24:
                peep_y -= 10;
                break;
            default:
                return;
            };

            lightfx_add_3d_light(peep->sprite_index, 0x0000 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, peep_x, peep_y, peep_z, LIGHTFX_LIGHT_TYPE_SPOT_1);
        }
    }
#endif

    rct_drawpixelinfo * dpi = session->Unk140E9A8;
    if (dpi->zoom_level > 2) {
        return;
    }

    if (gCurrentViewportFlags & VIEWPORT_FLAG_INVISIBLE_PEEPS) {
        return;
    }

    rct_peep_animation_entry sprite = g_peep_animation_entries[peep->sprite_type];

    uint8 spriteType = peep->action_sprite_type;
    uint8 imageOffset = peep->action_sprite_image_offset;

    if (peep->action == PEEP_ACTION_NONE_1) {
        spriteType = peep->next_action_sprite_type;
        imageOffset = 0;
    }

    uint32 baseImageId = (imageDirection >> 3) + sprite.sprite_animation[spriteType].base_image + imageOffset * 4;
    uint32 imageId = baseImageId | peep->tshirt_colour << 19 | peep->trousers_colour << 24 | IMAGE_TYPE_REMAP | IMAGE_TYPE_REMAP_2_PLUS;
    sub_98197C(session, imageId, 0, 0, 1, 1, 11, peep->z, 0, 0, peep->z + 3, get_current_rotation());

    if (baseImageId >= 10717 && baseImageId < 10749) {
        imageId = (baseImageId + 32) | peep->hat_colour << 19 | IMAGE_TYPE_REMAP;
        sub_98199C(session, imageId, 0, 0, 1, 1, 11, peep->z, 0, 0, peep->z + 3, get_current_rotation());
        return;
    }

    if (baseImageId >= 10781 && baseImageId < 10813) {
        imageId = (baseImageId + 32) | peep->balloon_colour << 19 | IMAGE_TYPE_REMAP;
        sub_98199C(session, imageId, 0, 0, 1, 1, 11, peep->z, 0, 0, peep->z + 3, get_current_rotation());
        return;
    }

    if (baseImageId >= 11197 && baseImageId < 11229) {
        imageId = (baseImageId + 32) | peep->umbrella_colour << 19 | IMAGE_TYPE_REMAP;
        sub_98199C(session, imageId, 0, 0, 1, 1, 11, peep->z, 0, 0, peep->z + 3, get_current_rotation());
        return;
    }
}
