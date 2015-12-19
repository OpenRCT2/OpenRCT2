#include "../addresses.h"
#include "../interface/viewport.h"
#include "supports.h"

typedef struct {
	uint16 full;
	uint16 half;
	uint16 flat;
	uint16 unk;
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
	uint8 var_0;
	uint8 var_1;
	uint8 var_2;
	uint8 var_3;
	uint8 var_4;
	uint8 var_5;
	uint8 var_6;
	uint8 var_7;
} unk_supports_desc;

/* 0x0097B23C */
const unk_supports_desc byte_97B23C[] = {
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 10, 0, 0, 10, 32, 44, 1, 1 },
	{ 0, 10, 0, 32, 10, 44, 1, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 10, 0, 0, 10, 32, 44, 1, 1 },
	{ 0, 10, 0, 32, 10, 44, 1, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 10, 0, 2, 10, 32, 76, 1, 1 },
	{ 0, 10, 2, 32, 10, 76, 1, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 4, 0, 1 },
	{ 0, 0, 0, 1, 1, 4, 0, 1 },
	{ 0, 0, 0, 1, 1, 4, 0, 1 },
	{ 0, 0, 0, 1, 1, 4, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 0, 0, 0, 1, 1, 8, 0, 1 },
	{ 2, 2, 1, 28, 28, 2, 0, 1 },
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
 *
 *  rct2: 0x006629BC
 *  dx: height
 *  ax: unknown
 *  ebp: imageColourFlags
 *  edi: unknown
 * @returns true if any supports have been drawn.
 */
bool wooden_a_supports_paint_setup(int height, uint16 ax, uint32 imageColourFlags, int supportType)
{
	// {
	// 	int eax = ax, ebx = 0, ecx = 0, edx = height, esi = 0, _edi = supportType, ebp = imageColourFlags;
	// 	RCT2_CALLFUNC_X(0x006629BC, &eax, &ebx, &ecx, &edx, &esi, &_edi, &ebp);
	// 	return eax & 0xFF;
	// }

	RCT2_GLOBAL(0x009E32B1, uint8) = 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_VIEWPORT_FLAGS, uint16) & VIEWPORT_FLAG_INVISIBLE_SUPPORTS) {
		return false;
	}

	if (!(RCT2_GLOBAL(0x0141E9DB, uint8) & 1)) {
		return false;
	}

	int z = floor2(RCT2_GLOBAL(0x00141E9D8, uint16) + 15, 16);
	height -= z;
	if (height < 0) {
		return true;
	}
	height /= 16;

	bool drawFlatPiece = false;
	int ecx = RCT2_GLOBAL(0x0141E9DA, uint8);
	if (ecx & (1 << 5)) {
		drawFlatPiece = true;
	} else if (ecx & (1 << 4)) {
		height -= 2;
		if (height < 0) {
			return true;
		}

		int imageId = WoodenSupportImageIds[supportType].unk;
		if (imageId != 0) {
			ecx &= 0x1F;
			imageId += word_97B3C4[ecx];
			imageId |= imageColourFlags;

			RCT2_GLOBAL(0x009DEA52, uint16) = 0;
			RCT2_GLOBAL(0x009DEA54, uint16) = 0;
			RCT2_GLOBAL(0x009DEA56, uint16) = z + 2;
			sub_98197C(0, 11, imageId, 0, z, 32, 32, get_current_rotation());

			RCT2_GLOBAL(0x009E32B1, uint8) = 1;
			z += 32;
		} else {
			z += 32;
			drawFlatPiece = true;
		}
	} else if (ecx & 0x0F) {
		height--;
		if (height < 0) {
			return true;
		}
		int imageId = WoodenSupportImageIds[supportType].unk;
		if (imageId == 0) {
			z += 16;
			drawFlatPiece = true;
		} else {
			ecx &= 0x1F;
			imageId += word_97B3C4[ecx];
			imageId |= imageColourFlags;

			RCT2_GLOBAL(0x009DEA52, uint16) = 0;
			RCT2_GLOBAL(0x009DEA54, uint16) = 0;
			RCT2_GLOBAL(0x009DEA56, uint16) = z + 2;
			sub_98196C(0, 11, imageId, 0, z, 32, 32, get_current_rotation());
			z += 16;
			RCT2_GLOBAL(0x009E32B1, uint8) = 1;
		}
	}

	if (drawFlatPiece) {
		int imageId = WoodenSupportImageIds[supportType].flat | imageColourFlags;
		sub_98197C(0, 0, imageId, 0, z - 2, 32, 32, get_current_rotation());
		RCT2_GLOBAL(0x009E32B1, uint8) = 1;
	}

	while (height != 0) {
		if ((z & 16) == 0 && height >= 2 && z + 16 != RCT2_GLOBAL(0x00141E9DC, uint16)) {
			// Full support
			int imageId = WoodenSupportImageIds[supportType].full | imageColourFlags;
			uint8 ah = ax == 2 ? 23 : 28;
			sub_98196C(0, ah, imageId, 0, z, 32, 32, get_current_rotation());
			RCT2_GLOBAL(0x009E32B1, uint8) = 1;
			z += 32;
			height -= 2;
		} else {
			// Half support
			int imageId = WoodenSupportImageIds[supportType].half | imageColourFlags;
			uint8 ah = ax == 1 ? 7 : 12;
			sub_98196C(0, ah, imageId, 0, z, 32, 32, get_current_rotation());
			z += 16;
			RCT2_GLOBAL(0x009E32B1, uint8) = 1;
			height -= 1;
		}
	}

	if (ax != 0) {
		int eax = (ax - 1) & 0xFFFF;
		int imageId = WoodenCurveSupportImageIds[supportType];
		if (imageId != 0 && byte_97B23C[eax].var_7 != 0) {
			imageId += eax;
			imageId |= imageColourFlags;
			RCT2_GLOBAL(0x009DEA52, uint16) = byte_97B23C[eax].var_0;
			RCT2_GLOBAL(0x009DEA54, uint16) = byte_97B23C[eax].var_1;
			RCT2_GLOBAL(0x009DEA56, uint16) = byte_97B23C[eax].var_2 + z;
			uint16 lengthY = byte_97B23C[eax].var_3;
			uint16 lengthX = byte_97B23C[eax].var_4;
			uint8 ah = byte_97B23C[eax].var_5;
			if (byte_97B23C[eax].var_6 == 0 || RCT2_GLOBAL(0x009DEA58, uint32) == 0) {
				sub_98197C(0, ah, imageId, 0, z, lengthY, lengthX, get_current_rotation());
				RCT2_GLOBAL(0x009E32B1, uint8) = 1;
			} else {
				RCT2_GLOBAL(0x009E32B1, uint8) = 1;
				if (!sub_98198C(0, ah, imageId, 0, z, lengthY, lengthX, get_current_rotation())) {
					int edi = RCT2_GLOBAL(0x009DEA58, uint32);
					RCT2_GLOBAL(edi + 0x20, uint32) = imageColourFlags;
				}
			}
		}
	}
	return false;
}

/**
 * Wooden supports
 *  rct2: 0x00662D5C
 */
bool wooden_b_supports_paint_setup(int height, uint16 ax, uint32 imageColourFlags, int supportType)
{
	int eax = ax, ebx = 0, ecx = 0, edx = height, esi = 0, _edi = supportType, ebp = imageColourFlags;
	RCT2_CALLFUNC_X(0x00662D5C, &eax, &ebx, &ecx, &edx, &esi, &_edi, &ebp);
	return eax & 0xFF;
}

/**
 * Metal pole supports
 *  rct2: 0x00663105
 */
bool metal_a_wooden_a_supports_paint_setup(int height, uint16 ax, uint32 imageColourFlags, int supportType)
{
	int eax = ax, ebx = 0, ecx = 0, edx = height, esi = 0, _edi = supportType, ebp = imageColourFlags;
	RCT2_CALLFUNC_X(0x00663105, &eax, &ebx, &ecx, &edx, &esi, &_edi, &ebp);
	return eax & 0xFF;
}

/**
 * Metal pole supports
 *  rct2: 0x00663584
 */
bool metal_b_wooden_a_supports_paint_setup(int height, uint16 ax, uint32 imageColourFlags, int supportType)
{
	int eax = ax, ebx = 0, ecx = 0, edx = height, esi = 0, _edi = supportType, ebp = imageColourFlags;
	RCT2_CALLFUNC_X(0x00663584, &eax, &ebx, &ecx, &edx, &esi, &_edi, &ebp);
	return eax & 0xFF;
}

/**
 * Some kind of supports
 *  rct2: 0x006A2ECC
 */
bool sub_6A2ECC(int height, uint16 ax, uint32 imageColourFlags, int supportType)
{
	int eax = ax, ebx = 0, ecx = 0, edx = height, esi = 0, _edi = supportType, ebp = imageColourFlags;
	RCT2_CALLFUNC_X(0x006A2ECC, &eax, &ebx, &ecx, &edx, &esi, &_edi, &ebp);
	return eax & 0xFF;
}

/**
 *
 *  rct2: 0x006A326B
 */
bool path_wooden_a_supports_paint_setup(int height, uint16 ax, uint32 imageColourFlags, int supportType)
{
	int eax = ax, ebx = 0, ecx = 0, edx = height, esi = 0, _edi = supportType, ebp = imageColourFlags;
	RCT2_CALLFUNC_X(0x006A326B, &eax, &ebx, &ecx, &edx, &esi, &_edi, &ebp);
	return eax & 0xFF;
}
