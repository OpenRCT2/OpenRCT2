#include "../addresses.h"
#include "../interface/viewport.h"
#include "supports.h"

typedef struct {
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

typedef struct {
	struct {
		uint8 x, y, z;
	} offset;
	struct {
		uint8 y, x, z;
	} length;
} unk_supports_desc_bound_box;

typedef struct {
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

	if (!(RCT2_GLOBAL(0x0141E9DB, uint8) & 1)) {
		return false;
	}

	int z = floor2(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) + 15, 16);
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
	int slope = RCT2_GLOBAL(0x0141E9DA, uint8);
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
		if ((z & 16) == 0 && height >= 2 && z + 16 != RCT2_GLOBAL(0x00141E9DC, uint16)) {
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

			if (byte_97B23C[special].var_6 == 0 || RCT2_GLOBAL(0x009DEA58, uint32) == 0) {
				sub_98197C(imageId, 0, 0, bBox.length.x, bBox.length.y, bBox.length.z, z, bBox.offset.x, bBox.offset.y, bBox.offset.z + z, rotation);
				hasSupports = true;
			} else {
				hasSupports = true;
				if (sub_98198C(imageId, 0, 0, bBox.length.x, bBox.length.y, bBox.length.z, z, bBox.offset.x, bBox.offset.y, bBox.offset.z + z, rotation)) {
					int edi = RCT2_GLOBAL(0x009DEA58, uint32);
					RCT2_GLOBAL(edi + 0x20, uint32) = imageColourFlags;
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
	int eax = special, ebx = 0, ecx = 0, edx = height, esi = 0, _edi = supportType, ebp = imageColourFlags;
	RCT2_CALLFUNC_X(0x00662D5C, &eax, &ebx, &ecx, &edx, &esi, &_edi, &ebp);
	return eax & 0xFF;
}

/**
 * Metal pole supports
 * eax = special,
 * ebx = segment, 
 * edx = height, 
 * edi = supportType, 
 * ebp = imageColourFlags;
 *  rct2: 0x00663105
 */
bool metal_a_supports_paint_setup(int supportType, int segment, int special, int height, uint32 imageColourFlags)
{
	if (gCurrentViewportFlags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS) {
		return false;
	}

	if (!(RCT2_GLOBAL(0x0141E9DB, uint8) & 1)) {
		return false;
	}

	sint16 originalHeight = height;

	RCT2_GLOBAL(0x009E3294, sint16) = -1;
	if (height < RCT2_ADDRESS(0x0141E9B4, uint16)[segment * 2]){
		RCT2_GLOBAL(0x009E3294, sint16) = height;

		height -= RCT2_ADDRESS(0x0097B142, sint16)[supportType];
		if (height < 0)
			return false;

		uint8* esi = &(RCT2_ADDRESS(0x0097AF32, uint8)[get_current_rotation() * 2]);

		uint8 newSegment = esi[segment * 8];
		if (height <= RCT2_ADDRESS(0x0141E9B4, uint16)[newSegment * 2]) {
			esi += 72;
			newSegment = esi[segment * 8];
			if (height <= RCT2_ADDRESS(0x0141E9B4, uint16)[newSegment * 2]) {
				esi += 72;
				newSegment = esi[segment * 8];
				if (height <= RCT2_ADDRESS(0x0141E9B4, uint16)[newSegment * 2]) {
					esi += 72;
					newSegment = esi[segment * 8];
					if (height <= RCT2_ADDRESS(0x0141E9B4, uint16)[newSegment * 2]) {
						esi += 72;
						newSegment = esi[segment * 8];
						return true;
					}
				}
			}
		}

		uint8 ebp = esi[segment * 8 + 1];

		sint8 xOffset = RCT2_ADDRESS(0x0097AF20, sint8)[segment * 2];
		sint8 yOffset = RCT2_ADDRESS(0x0097AF20 + 1, sint8)[segment * 2];
		xOffset += RCT2_ADDRESS(0x0097B052, sint8)[ebp * 2];
		yOffset += RCT2_ADDRESS(0x0097B052 + 1, sint8)[ebp * 2];

		sint16 boundBoxLengthX = RCT2_ADDRESS(0x0097B062, uint8)[ebp * 2];
		sint16 boundBoxLengthY = RCT2_ADDRESS(0x0097B062 + 1, uint8)[ebp * 2];

		uint32 image_id = RCT2_ADDRESS(0x0097B072, uint16)[supportType * 8 + ebp];
		image_id |= imageColourFlags;
		sub_98196C(image_id, xOffset, yOffset, boundBoxLengthX, boundBoxLengthY, 1, height, get_current_rotation());

		segment = newSegment;
	}
	sint16 si = height;
	if (RCT2_ADDRESS(0x00141E9B4 + 2, uint8)[segment * 4] & (1 << 5) ||
		height - RCT2_ADDRESS(0x00141E9B4, uint16)[segment * 2] < 6 ||
		RCT2_ADDRESS(0x0097B15C, uint16)[supportType * 2] == 0
		) {

		height = RCT2_ADDRESS(0x00141E9B4, uint16)[segment * 2];
	}else{
		sint8 xOffset = RCT2_ADDRESS(0x0097AF20, sint8)[segment * 2];
		sint8 yOffset = RCT2_ADDRESS(0x0097AF20 + 1, sint8)[segment * 2];

		uint32 image_id = RCT2_ADDRESS(0x0097B15C, uint16)[supportType * 2];
		image_id += RCT2_ADDRESS(0x0097B404, sint16)[RCT2_ADDRESS(0x00141E9B4 + 2, sint16)[segment * 2] & 0x1F];
		image_id |= imageColourFlags;

		sub_98196C(image_id, xOffset, yOffset, 0, 0, 5, RCT2_ADDRESS(0x0141E9B4, uint16)[segment * 2], get_current_rotation());

		height = RCT2_ADDRESS(0x0141E9B4, uint16)[segment * 2] + 6;
	}


	// Work out if a small support segment required to bring support to normal
	// size (aka floor2(x, 16))
	sint16 heightDiff = floor2(height + 16, 16);
	if (heightDiff > si) {
		heightDiff = si;
	}

	heightDiff -= height;

	if (heightDiff > 0) {
		sint8 xOffset = RCT2_ADDRESS(0x0097AF20, sint8)[segment * 2];
		sint8 yOffset = RCT2_ADDRESS(0x0097AF20 + 1, sint8)[segment * 2];

		uint32 image_id = RCT2_ADDRESS(0x0097B15C + 2, uint16)[supportType * 2];
		image_id += heightDiff - 1;
		image_id |= imageColourFlags;


		sub_98196C(image_id, xOffset, yOffset, 0, 0, heightDiff - 1, height, get_current_rotation());
	}

	height += heightDiff;
	//6632e6

	for (uint8 count = 0; ;count++) {
		if (count > 4)
			count = 0;

		sint16 z = height + 16;
		if (z > si) {
			z = si;
		}

		z -= height;
		if (z <= 0)
			break;

		sint8 xOffset = RCT2_ADDRESS(0x0097AF20, sint8)[segment * 2];
		sint8 yOffset = RCT2_ADDRESS(0x0097AF20 + 1, sint8)[segment * 2];

		uint32 image_id = RCT2_ADDRESS(0x0097B15C + 2, uint16)[supportType * 2];
		image_id += z - 1;
		image_id |= imageColourFlags;

		if (count == 3 && z == 0x10)
			image_id++;

		sub_98196C(image_id, xOffset, yOffset, 0, 0, z - 1, height, get_current_rotation());

		height += z;
	}

	RCT2_ADDRESS(0x00141E9B4, uint16)[segment * 2] = RCT2_GLOBAL(0x009E3294, sint16);
	RCT2_ADDRESS(0x00141E9B4 + 2, uint8)[segment * 4] = 0x20;

	height = originalHeight;
	if (special == 0)
		return true;

	if (special < 0) {
		special = -special;
		height--;
	}

	sint16 boundBoxOffsetX = RCT2_ADDRESS(0x0097AF20, uint8)[segment * 2];
	sint16 boundBoxOffsetY = RCT2_ADDRESS(0x0097AF21, uint8)[segment * 2];
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

		sint8 xOffset = RCT2_ADDRESS(0x0097AF20, sint8)[segment * 2];
		sint8 yOffset = RCT2_ADDRESS(0x0097AF20 + 1, sint8)[segment * 2];

		uint32 image_id = RCT2_ADDRESS(0x0097B15C + 2, uint16)[supportType * 2];
		image_id += z - 1;
		image_id |= imageColourFlags;


		sub_98197C(image_id, xOffset, yOffset, 0, 0, z - 1, height, boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ, get_current_rotation());

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
bool metal_b_wooden_a_supports_paint_setup(int supportType, int special, int height, uint32 imageColourFlags)
{
	int eax = special, ebx = 0, ecx = 0, edx = height, esi = 0, _edi = supportType, ebp = imageColourFlags;
	RCT2_CALLFUNC_X(0x00663584, &eax, &ebx, &ecx, &edx, &esi, &_edi, &ebp);
	return eax & 0xFF;
}

/**
 * Some kind of supports
 *  rct2: 0x006A2ECC
 */
bool sub_6A2ECC(int supportType, int special, int height, uint32 imageColourFlags)
{
	int eax = special, ebx = 0, ecx = 0, edx = height, esi = 0, _edi = supportType, ebp = imageColourFlags;
	RCT2_CALLFUNC_X(0x006A2ECC, &eax, &ebx, &ecx, &edx, &esi, &_edi, &ebp);
	return eax & 0xFF;
}

/**
 *
 *  rct2: 0x006A326B
 */
bool path_wooden_a_supports_paint_setup(int supportType, int special, int height, uint32 imageColourFlags)
{
	int eax = special, ebx = 0, ecx = 0, edx = height, esi = 0, _edi = supportType, ebp = imageColourFlags;
	RCT2_CALLFUNC_X(0x006A326B, &eax, &ebx, &ecx, &edx, &esi, &_edi, &ebp);
	return eax & 0xFF;
}
