/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _PAINT_SPRITE_H
#define _PAINT_SPRITE_H

#include "../../common.h"
#include "../../world/Sprite.h"

struct paint_session;

void sprite_paint_setup(paint_session * session, const uint16_t x, const uint16_t y);

void misc_paint(paint_session * session, const rct_sprite *misc, int32_t imageDirection);
void litter_paint(paint_session * session, const rct_litter *litter, int32_t imageDirection);
void peep_paint(paint_session * session, const rct_peep *peep, int32_t imageDirection);

extern const uint32_t vehicle_particle_base_sprites[5];

#endif
