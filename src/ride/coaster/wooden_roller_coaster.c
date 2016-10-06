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

#include "../../drawing/drawing.h"
#include "../../interface/viewport.h"
#include "../../paint/map_element/map_element.h"
#include "../../paint/paint.h"
#include "../../paint/supports.h"
#include "../../sprites.h"
#include "../../world/map.h"
#include "../../world/sprite.h"
#include "../ride_data.h"
#include "../track_data.h"
#include "../track_paint.h"

enum {
	SPR_WOODEN_RC_FLAT_RAILS_SW_NE = 24619,
	SPR_WOODEN_RC_FLAT_RAILS_NW_SE = 24620,
	SPR_WOODEN_RC_FLAT_CHAIN_RAILS_SW_NE = 24615,
	SPR_WOODEN_RC_FLAT_CHAIN_RAILS_NW_SE = 24616,
	SPR_WOODEN_RC_FLAT_CHAIN_RAILS_NE_SW = 24617,
	SPR_WOODEN_RC_FLAT_CHAIN_RAILS_SE_NW = 24618,

	SPR_WOODEN_RC_FLAT_SW_NE = 23753,
	SPR_WOODEN_RC_FLAT_NW_SE = 23754,
	SPR_WOODEN_RC_FLAT_CHAIN_SW_NE = 23749,
	SPR_WOODEN_RC_FLAT_CHAIN_NW_SE = 23750,
	SPR_WOODEN_RC_FLAT_CHAIN_NE_SW = 23751,
	SPR_WOODEN_RC_FLAT_CHAIN_SE_NW = 23752,
};

static uint32 wooden_rc_get_track_colour()
{
	return (gTrackColours[SCHEME_TRACK] & ~0xF80000) | gTrackColours[SCHEME_SUPPORTS];
}

static uint32 wooden_rc_get_rails_colour()
{
	return gTrackColours[SCHEME_TRACK];
}

static void wooden_rc_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	static const uint32 imageIds[4][2] = {
		{ SPR_WOODEN_RC_FLAT_SW_NE, SPR_WOODEN_RC_FLAT_CHAIN_SW_NE },
		{ SPR_WOODEN_RC_FLAT_NW_SE, SPR_WOODEN_RC_FLAT_CHAIN_NW_SE },
		{ SPR_WOODEN_RC_FLAT_SW_NE, SPR_WOODEN_RC_FLAT_CHAIN_NE_SW },
		{ SPR_WOODEN_RC_FLAT_NW_SE, SPR_WOODEN_RC_FLAT_CHAIN_SE_NW },
	};
	static const uint32 railsImageIds[4][2] = {
		{ SPR_WOODEN_RC_FLAT_RAILS_SW_NE, SPR_WOODEN_RC_FLAT_CHAIN_RAILS_SW_NE },
		{ SPR_WOODEN_RC_FLAT_RAILS_NW_SE, SPR_WOODEN_RC_FLAT_CHAIN_RAILS_NW_SE },
		{ SPR_WOODEN_RC_FLAT_RAILS_SW_NE, SPR_WOODEN_RC_FLAT_CHAIN_RAILS_NE_SW },
		{ SPR_WOODEN_RC_FLAT_RAILS_NW_SE, SPR_WOODEN_RC_FLAT_CHAIN_RAILS_SE_NW },
	};

	uint8 isChained = track_element_is_lift_hill(mapElement) ? 1 : 0;
	uint32 imageId = imageIds[direction][isChained] | wooden_rc_get_track_colour();
	uint32 railsImageId = railsImageIds[direction][isChained] | wooden_rc_get_rails_colour();
	sub_98197C_rotated(direction, imageId, 0, 2, 32, 25, 2, height, 0, 3, height);
	sub_98199C_rotated(direction, railsImageId, 0, 2, 32, 25, 2, height, 0, 3, height);
	wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_wooden_rc(int trackType, int direction)
{
	switch (trackType) {
	case TRACK_ELEM_FLAT:
		return wooden_rc_track_flat;
	}
	return NULL;
}
