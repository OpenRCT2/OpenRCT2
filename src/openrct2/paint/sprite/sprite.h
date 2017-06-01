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

#ifndef _PAINT_SPRITE_H
#define _PAINT_SPRITE_H

#include "../../common.h"
#include "../../world/sprite.h"

void sprite_paint_setup(const uint16 eax, const uint16 ecx);

void misc_paint(rct_sprite *misc, sint32 imageDirection);
void litter_paint(rct_litter *litter, sint32 imageDirection);
void peep_paint(rct_peep *peep, sint32 imageDirection);

#endif
