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

#include "../interface/viewport.h"
#include "../paint/paint.h"
#include "supports.h"
#include "map_element/map_element.h"

/** rct2: 0x0097AF20, 0x0097AF21 */
const rct_xy8 loc_97AF20[] = {
	{4,  4},
	{28, 4},
	{4,  28},
	{28, 28},
	{16, 16},
	{16, 4},
	{4,  16},
	{28, 16},
	{16, 28}
};

/** rct2: 0x0097AF32 */
static const uint8 _97AF32[] = {
	5, 2, 5, 2, 5, 2, 5, 2,
	7, 1, 7, 1, 7, 1, 7, 1,
	6, 3, 6, 3, 6, 3, 6, 3,
	8, 0, 8, 0, 8, 0, 8, 0,
	5, 3, 6, 0, 8, 1, 7, 2,
	1, 2, 1, 2, 1, 2, 1, 2,
	0, 3, 0, 3, 0, 3, 0, 3,
	3, 1, 3, 1, 3, 1, 3, 1,
	2, 0, 2, 0, 2, 0, 2, 0,

	6, 1, 6, 1, 6, 1, 6, 1,
	5, 0, 5, 0, 5, 0, 5, 0,
	8, 2, 8, 2, 8, 2, 8, 2,
	7, 3, 7, 3, 7, 3, 7, 3,
	6, 0, 8, 1, 7, 2, 5, 3,
	0, 0, 0, 0, 0, 0, 0, 0,
	2, 1, 2, 1, 2, 1, 2, 1,
	1, 3, 1, 3, 1, 3, 1, 3,
	3, 2, 3, 2, 3, 2, 3, 2,

	1, 6, 1, 6, 1, 6, 1, 6,
	3, 5, 3, 5, 3, 5, 3, 5,
	0, 7, 0, 7, 0, 7, 0, 7,
	2, 4, 2, 4, 2, 4, 2, 4,
	8, 1, 7, 2, 5, 3, 6, 0,
	4, 1, 4, 1, 4, 1, 4, 1,
	4, 2, 4, 2, 4, 2, 4, 2,
	4, 0, 4, 0, 4, 0, 4, 0,
	4, 3, 4, 3, 4, 3, 4, 3,

	2, 5, 2, 5, 2, 5, 2, 5,
	0, 4, 0, 4, 0, 4, 0, 4,
	3, 6, 3, 6, 3, 6, 3, 6,
	1, 7, 1, 7, 1, 7, 1, 7,
	7, 2, 5, 3, 6, 0, 8, 1,
	8, 5, 8, 5, 8, 5, 8, 5,
	7, 6, 7, 6, 7, 6, 7, 6,
	6, 4, 6, 4, 6, 4, 6, 4,
	5, 7, 5, 7, 5, 7, 5, 7,
};

/** rct2: 0x0097B052, 0x0097B053 */
const rct_xy16 loc_97B052[] = {
	{-15, -1},
	{0,   -2},
	{-2,  -1},
	{-1,  -15},
	{-26, -1},
	{0,   -2},
	{-2,  -1},
	{-1,  -26}
};

/** rct2: 0x0097B062, 0x0097B063 */
const rct_xy8 _97B062[] = {
	{ 18,  3 },
	{  3, 18 },
	{ 18,  3 },
	{  3, 18 },
	{ 32,  3 },
	{  3, 32 },
	{ 32,  3 },
	{  3, 32 }
};

/** rct2: 0x0097B072 */
const uint32 _97B072[][8] = {
	{ 3370, 3371, 3370, 3371, 3372, 3373, 3372, 3373 },
	{ 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 },
	{ 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 },
	{ 3370, 3371, 3370, 3371, 3372, 3373, 3372, 3373 },
	{ 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 },
	{ 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 },
	{ 3378, 3383, 3378, 3383, 3380, 3385, 3380, 3385 },
	{ 3378, 3383, 3378, 3383, 3380, 3385, 3380, 3385 },
	{ 3382, 3379, 3382, 3379, 3384, 3381, 3384, 3381 },
	{ 3382, 3379, 3382, 3379, 3384, 3381, 3384, 3381 },
	{ 3378, 3379, 3378, 3379, 3380, 3381, 3380, 3381 },
	{ 3386, 3387, 3386, 3387, 3388, 3389, 3388, 3389 },
	{ 3370, 3371, 3370, 3371, 3372, 3373, 3372, 3373 },
};

/** rct2: 0x0097B142 */
static const uint8 word_97B142[] = {
	6,
	3,
	3,
	6,
	3,
	3,
	6,
	6,
	6,
	6,
	4,
	3,
	6,
};

typedef struct metal_supports_images {
	uint16 base_id;
	uint16 beam_id;
} metal_supports_images;

/** rct2: 0x0097B15C */
static const metal_supports_images _97B15C[] = {
	{ 3243, 3209 },
	{ 3279, 3262 },
	{ 3298, 3262 },
	{ 3334, 3317 },
	{    0, 3658 },
	{    0, 3658 },
	{    0, 3141 },
	{    0, 3158 },
	{    0, 3175 },
	{    0, 3192 },
	{    0, 3124 },
	{ 3243, 3209 },
	{ 3334, 3353 },
};

/** rct2: 0x0097B190 */
static const metal_supports_images _97B190[] = {
	{ 3243, 3226 }, // 3209
	{ 3279, 3262 },
	{ 3298, 3262 },
	{ 3334, 3317 },
	{    0, 3658 },
	{    0, 3658 },
	{    0, 3141 },
	{    0, 3158 },
	{    0, 3175 },
	{    0, 3192 },
	{    0, 3124 },
	{ 3243, 3226 }, // 3209
	{ 3334, 3353 },
};

/** rct2: 0x0097B404 */
static const uint8 metal_supports_slope_image_map[] = {
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,  0,
	 0,  0,  0,  0,  0,  0,  0, 15,  0,  0,  0, 16,  0, 17, 18,  0,
};

typedef struct supports_id_desc {
	uint16 full;
	uint16 half;
	uint16 flat;
	uint16 slope;
} supports_id_desc;

/* 0x0097B1C4 */
const supports_id_desc WoodenSupportImageIds[] = {
	{ 3392, 3393, 3394, 3536 },
	{ 3390, 3391, 3394, 3514 },
	{ 3558, 3559, 3560, 3570 },
	{ 3561, 3562, 3563, 3592 },
	{ 3564, 3565, 3566, 3614 },
	{ 3567, 3568, 3569, 3636 },
	{ 3677, 3678, 3680, 3739 },
	{ 3675, 3676, 3679, 3717 },
	{ 3761, 3762, 3763, 3773 },
	{ 3764, 3765, 3766, 3795 },
	{ 3767, 3768, 3769, 3817 },
	{ 3770, 3771, 3772, 3839 },
};

/* 0x0097B224 */
const uint16 WoodenCurveSupportImageIds[] = {
	3465,
	3465,
	0,
	0,
	0,
	0,
	3681,
	3681,
	0,
	0,
	0,
	0,
};

typedef struct unk_supports_desc_bound_box {
	struct {
		uint8 x, y, z;
	} offset;
	struct {
		uint8 y, x, z;
	} length;
} unk_supports_desc_bound_box;

typedef struct unk_supports_desc {
	unk_supports_desc_bound_box bounding_box;
	uint8 var_6;
	uint8 var_7;
} unk_supports_desc;

/* 0x0097B23C */
const unk_supports_desc byte_97B23C[] = {
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{10, 0,  0}, {10, 32, 44}}, 1, 1},
	{{{0,  10, 0}, {32, 10, 44}}, 1, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{10, 0,  0}, {10, 32, 44}}, 1, 1},
	{{{0,  10, 0}, {32, 10, 44}}, 1, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{10, 0,  2}, {10, 32, 76}}, 1, 1},
	{{{0,  10, 2}, {32, 10, 76}}, 1, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  4}},  0, 1},
	{{{0,  0,  0}, {1,  1,  4}},  0, 1},
	{{{0,  0,  0}, {1,  1,  4}},  0, 1},
	{{{0,  0,  0}, {1,  1,  4}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{0,  0,  0}, {1,  1,  8}},  0, 1},
	{{{2,  2,  1}, {28, 28, 2}},  0, 1},
};

/* 0x0097B3C4 */
const uint16 word_97B3C4[] = {
	0,
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	12,
	13,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	14,
	0,
	0,
	0,
	17,
	0,
	16,
	15,
	0,
};

#ifdef NO_RCT2
paint_struct * gWoodenSupportsPrependTo;
#endif

/**
 * Adds paint structs for wooden supports.
 *  rct2: 0x006629BC
 * @param supportType (edi) Type and direction of supports.
 * @param special (ax) Used for curved supports.
 * @param height (dx) The height of the supports.
 * @param imageColourFlags (ebp) The colour and palette flags for the support sprites.
 * @param[out] underground (Carry flag) true if underground.
 * @returns (al) true if any supports have been drawn, otherwise false.
 */
bool wooden_a_supports_paint_setup(int supportType, int special, int height, uint32 imageColourFlags, bool* underground)
{
	if (underground != NULL){
		*underground = false;
	}

	if (gCurrentViewportFlags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS) {
		return false;
	}

	if (!(g141E9DB & G141E9DB_FLAG_1)) {
		return false;
	}

	int z = floor2(gSupport.height + 15, 16);
	height -= z;
	if (height < 0) {
		if (underground != NULL) {
			*underground = true;
		}
		return false;
	}
	height /= 16;

	bool hasSupports = false;
	bool drawFlatPiece = false;
	int rotation = get_current_rotation();

	// Draw base support (usually shaped to the slope)
	int slope = gSupport.slope;
	if (slope & (1 << 5)) {
		// Above scenery (just put a base piece above it)
		drawFlatPiece = true;
	} else if (slope & (1 << 4)) {
		// Steep diagonal (place the correct shaped support for the slope)
		height -= 2;
		if (height < 0) {
			if (underground != NULL) {
				*underground = true;
			}
			return false;
		}

		int imageId = WoodenSupportImageIds[supportType].slope;
		if (imageId == 0) {
			drawFlatPiece = true;
		} else {
			imageId += word_97B3C4[slope & 0x1F];
			imageId |= imageColourFlags;
			sub_98197C(imageId, 0, 0, 32, 32, 11, z, 0, 0, z + 2, rotation);

			sub_98197C(imageId + 4, 0, 0, 32, 32, 11, z + 16, 0, 0, z + 16 + 2, rotation);

			hasSupports = true;
		}
		z += 32;
	} else if ((slope & 0x0F) != 0) {
		// 1 to 3 quarters up
		height--;
		if (height < 0) {
			if (underground != NULL) {
				*underground = true;
			}
			return false;
		}

		int imageId = WoodenSupportImageIds[supportType].slope;
		if (imageId == 0) {
			drawFlatPiece = true;
		} else {
			imageId += word_97B3C4[slope & 0x1F];
			imageId |= imageColourFlags;

			sub_98197C(imageId, 0, 0, 32, 32, 11, z, 0, 0, z + 2, rotation);
			hasSupports = true;
		}
		z += 16;
	}

	// Draw flat base support
	if (drawFlatPiece) {
		int imageId = WoodenSupportImageIds[supportType].flat | imageColourFlags;
		sub_98196C(imageId, 0, 0, 32, 32, 0, z - 2, rotation);
		hasSupports = true;
	}

	// Draw repeated supports for left over space
	while (height != 0) {
		if ((z & 16) == 0 && height >= 2 && z + 16 != gUnk141E9DC) {
			// Full support
			int imageId = WoodenSupportImageIds[supportType].full | imageColourFlags;
			uint8 ah = special == 2 ? 23 : 28;
			sub_98196C(imageId, 0, 0, 32, 32, ah, z, rotation);
			hasSupports = true;
			z += 32;
			height -= 2;
		} else {
			// Half support
			int imageId = WoodenSupportImageIds[supportType].half | imageColourFlags;
			uint8 ah = special == 1 ? 7 : 12;
			sub_98196C(imageId, 0, 0, 32, 32, ah, z, rotation);
			hasSupports = true;
			z += 16;
			height -= 1;
		}
	}

	// Draw special pieces, e.g. curved supports
	if (special != 0) {
		special = (special - 1) & 0xFFFF;

		int imageId = WoodenCurveSupportImageIds[supportType];
		if (imageId != 0 && byte_97B23C[special].var_7 != 0) {
			imageId += special;
			imageId |= imageColourFlags;

			unk_supports_desc_bound_box bBox = byte_97B23C[special].bounding_box;

			if (byte_97B23C[special].var_6 == 0 || gWoodenSupportsPrependTo == NULL) {
				sub_98197C(imageId, 0, 0, bBox.length.x, bBox.length.y, bBox.length.z, z, bBox.offset.x, bBox.offset.y, bBox.offset.z + z, rotation);
				hasSupports = true;
			} else {
				hasSupports = true;
				paint_struct* ps = sub_98198C(imageId, 0, 0, bBox.length.x, bBox.length.y, bBox.length.z, z, bBox.offset.x, bBox.offset.y, bBox.offset.z + z, rotation);
				if (ps != NULL) {
					paint_struct* edi = gWoodenSupportsPrependTo;
					edi->var_20 = ps;
				}
			}
		}
	}

	return hasSupports;
}

/**
 * Wooden supports
 *  rct2: 0x00662D5C
 */
bool wooden_b_supports_paint_setup(int supportType, int special, int height, uint32 imageColourFlags)
{
#ifdef NO_RCT2
	return 0;
#else
	int eax = special, ebx = 0, ecx = 0, edx = height, esi = 0, _edi = supportType, ebp = imageColourFlags;
	RCT2_CALLFUNC_X(0x00662D5C, &eax, &ebx, &ecx, &edx, &esi, &_edi, &ebp);
	return eax & 0xFF;
#endif
}

/**
 * Metal pole supports
 * @param supportType (edi)
 * @param segment (ebx)
 * @param special (ax)
 * @param height (edx)
 * @param imageColourFlags (ebp)
 *  rct2: 0x00663105
 */
bool metal_a_supports_paint_setup(int supportType, int segment, int special, int height, uint32 imageColourFlags)
{
	if (gCurrentViewportFlags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS) {
		return false;
	}

	if (!(g141E9DB & G141E9DB_FLAG_1)) {
		return false;
	}

	sint16 originalHeight = height;
	int originalSegment = segment;

	const uint8 rotation = get_current_rotation();
	sint16 unk9E3294 = -1;
	if (height < gSupportSegments[segment].height){
		unk9E3294 = height;

		height -= word_97B142[supportType];
		if (height < 0)
			return false;

		const uint8* esi = &(_97AF32[rotation * 2]);

		uint8 newSegment = esi[segment * 8];
		if (height <= gSupportSegments[newSegment].height) {
			esi += 72;
			newSegment = esi[segment * 8];
			if (height <= gSupportSegments[newSegment].height) {
				esi += 72;
				newSegment = esi[segment * 8];
				if (height <= gSupportSegments[newSegment].height) {
					esi += 72;
					newSegment = esi[segment * 8];
					if (height <= gSupportSegments[newSegment].height) {
						return false;
					}
				}
			}
		}

		uint8 ebp = esi[segment * 8 + 1];

		sint8 xOffset = loc_97AF20[segment].x;
		sint8 yOffset = loc_97AF20[segment].y;
		xOffset += loc_97B052[ebp].x;
		yOffset += loc_97B052[ebp].y;

		sint16 boundBoxLengthX = _97B062[ebp].x;
		sint16 boundBoxLengthY = _97B062[ebp].y;

		uint32 image_id = _97B072[supportType][ebp];
		image_id |= imageColourFlags;
		sub_98196C(image_id, xOffset, yOffset, boundBoxLengthX, boundBoxLengthY, 1, height, rotation);

		segment = newSegment;
	}
	sint16 si = height;
	if (gSupportSegments[segment].slope & (1 << 5) ||
		height - gSupportSegments[segment].height < 6 ||
		_97B15C[supportType].base_id == 0
		) {

		height = gSupportSegments[segment].height;
	}else{
		sint8 xOffset = loc_97AF20[segment].x;
		sint8 yOffset = loc_97AF20[segment].y;

		uint32 image_id = _97B15C[supportType].base_id;
		image_id += metal_supports_slope_image_map[gSupportSegments[segment].slope & 0x1F];
		image_id |= imageColourFlags;

		sub_98196C(image_id, xOffset, yOffset, 0, 0, 5, gSupportSegments[segment].height, rotation);

		height = gSupportSegments[segment].height + 6;
	}


	// Work out if a small support segment required to bring support to normal
	// size (aka floor2(x, 16))
	sint16 heightDiff = floor2(height + 16, 16);
	if (heightDiff > si) {
		heightDiff = si;
	}

	heightDiff -= height;

	if (heightDiff > 0) {
		sint8 xOffset = loc_97AF20[segment].x;
		sint8 yOffset = loc_97AF20[segment].y;

		uint32 image_id = _97B15C[supportType].beam_id;
		image_id += heightDiff - 1;
		image_id |= imageColourFlags;


		sub_98196C(image_id, xOffset, yOffset, 0, 0, heightDiff - 1, height, rotation);
	}

	height += heightDiff;
	//6632e6

	for (uint8 count = 0; ;count++) {
		if (count >= 4)
			count = 0;

		sint16 z = height + 16;
		if (z > si) {
			z = si;
		}

		z -= height;
		if (z <= 0)
			break;

		sint8 xOffset = loc_97AF20[segment].x;
		sint8 yOffset = loc_97AF20[segment].y;

		uint32 image_id = _97B15C[supportType].beam_id;
		image_id += z - 1;
		image_id |= imageColourFlags;

		if (count == 3 && z == 0x10)
			image_id++;

		sub_98196C(image_id, xOffset, yOffset, 0, 0, z - 1, height, rotation);

		height += z;
	}

	gSupportSegments[segment].height = unk9E3294;
	gSupportSegments[segment].slope = 0x20;

	height = originalHeight;
	segment = originalSegment;
	if (special == 0)
		return true;

	if (special < 0) {
		special = -special;
		height--;
	}

	sint16 boundBoxOffsetX = loc_97AF20[segment].x;
	sint16 boundBoxOffsetY = loc_97AF20[segment].y;
	sint16 boundBoxOffsetZ = height;
	si = height + special;

	while (1) {
		sint16 z = height + 16;
		if (z > si) {
			z = si;
		}

		z -= height;
		if (z <= 0)
			break;

		sint8 xOffset = loc_97AF20[segment].x;
		sint8 yOffset = loc_97AF20[segment].y;

		uint32 image_id = _97B190[supportType].beam_id;
		image_id += z - 1;
		image_id |= imageColourFlags;

		sub_98197C(image_id, xOffset, yOffset, 0, 0, 0, height, boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ, rotation);

		height += z;
	}
	
	return true;

	//int eax = special, ebx = 0, ecx = 0, edx = height, esi = 0, _edi = supportType, ebp = imageColourFlags;
	//RCT2_CALLFUNC_X(0x00663105, &eax, &ebx, &ecx, &edx, &esi, &_edi, &ebp);
	//return eax & 0xFF;
}

/**
 * Metal pole supports
 *  rct2: 0x00663584
 */
bool metal_b_supports_paint_setup(int supportType, uint8 segment, int special, int height, uint32 imageColourFlags)
{
#ifdef NO_RCT2
	return 0;
#else
	int eax = special, ebx = segment, ecx = 0, edx = height, esi = 0, _edi = supportType, ebp = imageColourFlags;
	RCT2_CALLFUNC_X(0x00663584, &eax, &ebx, &ecx, &edx, &esi, &_edi, &ebp);
	return eax & 0xFF;
#endif
}

/**
 *  rct2: 0x006A2ECC
 */
bool path_a_supports_paint_setup(int supportType, int special, int height, uint32 imageColourFlags, rct_footpath_entry * pathEntry, bool * underground)
{
#ifdef NO_RCT2
	return 0;
#else
	RCT2_GLOBAL(0xF3EF6C, rct_footpath_entry *) = pathEntry;
	int eax = special, ebx = 0, ecx = 0, edx = height, esi = 0, _edi = supportType, ebp = imageColourFlags;
	RCT2_CALLFUNC_X(0x006A2ECC, &eax, &ebx, &ecx, &edx, &esi, &_edi, &ebp);
	return eax & 0xFF;
#endif
}

/**
 *
 *  rct2: 0x006A326B
 */
bool path_b_supports_paint_setup(int supportType, int special, int height, uint32 imageColourFlags, rct_footpath_entry * pathEntry)
{
#ifdef NO_RCT2
	return 0;
#else
	RCT2_GLOBAL(0xF3EF6C, rct_footpath_entry *) = pathEntry;
	int eax = special, ebx = supportType, ecx = 0, edx = height, esi = 0, _edi = 0, ebp = imageColourFlags;
	RCT2_CALLFUNC_X(0x006A326B, &eax, &ebx, &ecx, &edx, &esi, &_edi, &ebp);
	return eax & 0xFF;
#endif
}
