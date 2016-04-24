#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

/**
 * Litter Paint Setup
 *  rct2: 0x006736FC
 */
void litter_paint(rct_litter *litter, int imageDirection)
{
	rct_drawpixelinfo *dpi;

	dpi = unk_140E9A8;
	if (dpi->zoom_level != 0) return; // If zoomed at all no litter drawn

	// litter has no sprite direction so remove that
	imageDirection >>= 3;
	// Some litter types have only 1 direction so remove
	// anything that isn't required.
	imageDirection &= RCT2_ADDRESS(0x97EF6C, uint32)[litter->type * 2 + 1];

	uint32 image_id = imageDirection + RCT2_ADDRESS(0x97EF6C, uint32)[litter->type * 2];

	sub_98197C(image_id, 0, 0, 4, 4, -1, litter->z, -4, -4, litter->z + 2, get_current_rotation());
}
