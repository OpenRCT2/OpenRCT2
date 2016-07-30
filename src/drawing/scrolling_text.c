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

#include "../addresses.h"
#include "../config.h"
#include "../interface/colour.h"
#include "../localisation/localisation.h"
#include "drawing.h"

#pragma pack(push, 1)
/* size: 0xA12 */
typedef struct rct_draw_scroll_text {
	rct_string_id string_id;	// 0x00
	uint32 string_args_0;		// 0x02
	uint32 string_args_1;		// 0x06
	uint16 position;			// 0x0A
	uint16 mode;				// 0x0C
	uint32 id;					// 0x0E
	uint8 bitmap[64 * 8 * 5];	// 0x12
} rct_draw_scroll_text;
assert_struct_size(rct_draw_scroll_text, 0xA12);
#pragma pack(pop)

rct_draw_scroll_text *gDrawScrollTextList = RCT2_ADDRESS(RCT2_ADDRESS_DRAW_SCROLL_LIST, rct_draw_scroll_text);
uint8 *gCharacterBitmaps = RCT2_ADDRESS(RCT2_ADDRESS_CHARACTER_BITMAP, uint8);

void scrolling_text_set_bitmap_for_sprite(utf8 *text, int scroll, uint8 *bitmap, const sint16 *scrollPositionOffsets);
void scrolling_text_set_bitmap_for_ttf(utf8 *text, int scroll, uint8 *bitmap, const sint16 *scrollPositionOffsets);

void scrolling_text_initialise_bitmaps()
{
	uint8 drawingSurface[64];
	rct_drawpixelinfo dpi = {
		.bits = (uint8 *)&drawingSurface,
		.x = 0,
		.y = 0,
		.width = 8,
		.height = 8,
		.pitch = 0,
		.zoom_level = 0
	};


	for (int i = 0; i < 224; i++) {
		memset(drawingSurface, 0, sizeof(drawingSurface));
		gfx_draw_sprite_software(&dpi, i + 0x10D5, -1, 0, 0);

		for (int x = 0; x < 8; x++) {
			uint8 val = 0;
			for (int y = 0; y < 8; y++) {
				val >>= 1;
				if (dpi.bits[x + y * 8] == 1) {
					val |= 0x80;
				}
			}
			gCharacterBitmaps[i * 8 + x] = val;
		}

	}
}

static uint8 *font_sprite_get_codepoint_bitmap(int codepoint)
{
	return &gCharacterBitmaps[font_sprite_get_codepoint_offset(codepoint) * 8];
}


static int scrolling_text_get_matching_or_oldest(rct_string_id stringId, uint16 scroll, uint16 scrollingMode)
{
	uint32 oldestId = 0xFFFFFFFF;
	int scrollIndex = -1;
	rct_draw_scroll_text* oldestScroll = NULL;
	for (int i = 0; i < 32; i++) {
		rct_draw_scroll_text *scrollText = &gDrawScrollTextList[i];
		if (oldestId >= scrollText->id) {
			oldestId = scrollText->id;
			scrollIndex = i;
			oldestScroll = scrollText;
		}

		// If exact match return the matching index
		uint32 stringArgs0, stringArgs1;
		memcpy(&stringArgs0, gCommonFormatArgs + 0, sizeof(uint32));
		memcpy(&stringArgs1, gCommonFormatArgs + 4, sizeof(uint32));
		if (
			scrollText->string_id == stringId &&
			scrollText->string_args_0 == stringArgs0 &&
			scrollText->string_args_1 == stringArgs1 &&
			scrollText->position == scroll &&
			scrollText->mode == scrollingMode
		) {
			scrollText->id = RCT2_GLOBAL(RCT2_ADDRESS_DRAW_SCROLL_NEXT_ID, uint32);
			return i + 0x606;
		}
	}
	return scrollIndex;
}

static uint8 scrolling_text_get_colour(uint32 character)
{
	int colour = character & 0x7F;
	if (character & (1 << 7)) {
		return ColourMapA[colour].light;
	} else {
		return ColourMapA[colour].mid_dark;
	}
}

static void scrolling_text_format(utf8 *dst, rct_draw_scroll_text *scrollText)
{
	if (gConfigGeneral.upper_case_banners) {
		format_string_to_upper(dst, scrollText->string_id, &scrollText->string_args_0);
	} else {
		format_string(dst, scrollText->string_id, &scrollText->string_args_0);
	}
}

extern bool TempForScrollText;

static const sint16 _scrollpos0[] = {
	803, 804, 741, 742, 679, 680, 617, 618, 555, 556, 493, 494, 431, 432, 369, 370, 307, 308, 245, 246, 183, 184,
	121, 122,
	-1
};
static const sint16 _scrollpos1[] = {
	69, 70, 135, 136, 201, 202, 267, 268, 333, 334, 399, 400, 465, 466, 531, 532, 597, 598, 663, 664, 729, 730, 795,
	796, -1
};
static const sint16 _scrollpos2[] = {
	76, 77, 142, 143, 208, 209, 274, 275, 340, 341, 406, 407, 472, 473, 538, 539, 604, 605, 670, 671, 672, 609, 610,
	547, 548,
	485, 486, 423, 424, 361, 362, 299, 300, 237, 238, 175, 176, 113, 114, -1
};
static const sint16 _scrollpos3[] = {
	16, 81, 82, 147, 212, 213, 278, 343, 344, 409, 410, 475, 476, 541, 542, 607, 608, 673, 674, 739, 740, 805, 806,
	871, 872, 937, 938, 939, 1004, 1005, 1006, 1071, 1072, -1
};
static const sint16 _scrollpos4[] = {
	1103, 1104, 1041, 1042, 1043, 980, 981, 982, 919, 920, 857, 858, 795, 796, 733, 734, 671, 672, 609, 610, 547,
	548, 485, 486, 423, 424, 361, 298, 299, 236, 173, 174, 111, 48, -1
};
static const sint16 _scrollpos5[] = {
	772, 773, 710, 711, 648, 649, 586, 587, 524, 525, 462, 463, 400, 401, 338, 339, 276, 277, 214, 215, 152, 153,
	90, 91, -1
};
static const sint16 _scrollpos6[] = {
	100, 101, 166, 167, 232, 233, 298, 299, 364, 365, 430, 431, 496, 497, 562, 563, 628, 629, 694, 695, 760, 761,
	826, 827, -1
};
static const sint16 _scrollpos7[] = {
	712, 713, 650, 651, 588, 589, 526, 527, 464, 465, 402, 403, 340, 341, 278, 279, 216, 217, 154, 155, 92, 93, -1
};
static const sint16 _scrollpos8[] = {
	164, 165, 230, 231, 296, 297, 362, 363, 428, 429, 494, 495, 560, 561, 626, 627, 692, 693, 758, 759, -1
};
static const sint16 _scrollpos9[] = {
	587, 588, 589, 590, 591, 528, 529, 466, 467, 468, 405, 406, 343, 280, 281, 218, 219, 156, 93, -1
};
static const sint16 _scrollpos10[] = {
	98, 163, 228, 229, 294, 295, 360, 425, 426, 491, 492, 493, 558, 559, 624, 625, 626, 627, 628, -1
};
static const sint16 _scrollpos11[] = {
	654, 591, 592, 529, 530, 467, 468, 405, 406, 343, 344, 281, 282, 219, 220, 157, 158, 95, 96, 33, 34, -1
};
static const sint16 _scrollpos12[] = {
	97, 162, 163, 228, 229, 294, 295, 360, 361, 426, 427, 492, 493, 558, 559, 624, 625, 690, 691, 756, 757, -1
};
static const sint16 _scrollpos13[] = {
	716, 653, 654, 591, 592, 529, 530, 467, 468, 405, 406, 343, 344, 281, 282, 219, 220, 157, 158, 95, -1
};
static const sint16 _scrollpos14[] = {
	97, 162, 163, 228, 229, 294, 295, 360, 361, 426, 427, 492, 493, 558, 559, 624, 625, 690, 691, 756, 757, -1
};
static const sint16 _scrollpos15[] = {
	650, 651, 588, 589, 526, 527, 464, 465, 402, 403, 340, 341, 278, 279, 216, 217, 154, 155, 92, 93, 30, 31, -1
};
static const sint16 _scrollpos16[] = {
	33, 34, 99, 100, 165, 166, 231, 232, 297, 298, 363, 364, 429, 430, 495, 496, 561, 562, 627, 628, 693, 694, -1
};
static const sint16 _scrollpos17[] = {
	710, 711, 648, 649, 586, 587, 524, 525, 462, 463, 400, 401, 338, 339, 276, 277, 214, 215, 152, 153, 90, 91, 28,
	29, -1
};
static const sint16 _scrollpos18[] = {
	34, 35, 100, 101, 166, 167, 232, 233, 298, 299, 364, 365, 430, 431, 496, 497, 562, 563, 628, 629, 694, 695, 760,
	761, -1
};
static const sint16 _scrollpos19[] = {
	77, 78, 143, 144, 209, 210, 275, 276, 341, 342, 407, 408, 473, 474, 539, 540, 605, 606, 671, 672, 673, 610, 611,
	548, 549, 486, 487, 424, 425, 362, 363, 300, 301, 238, 239, 176, 177, 114, 115, -1
};
static const sint16 _scrollpos20[] = {
	76, 205, 270, 335, 400, 465, 466, 531, 532, 597, 598, 599, 664, 665, 666, 667, 668, 669, 670, 671, 672, 673,
	674, 675, 676, 677, 678, 615, 616, 617, 554, 555, 492, 493, 430, 367, 304, 241, -1
};
static const sint16 _scrollpos21[] = {
	76, 77, 142, 143, 208, 209, 274, 275, 340, 341, 406, 407, 472, 473, 538, 539, 604, 605, 670, 671, 672, 609, 610,
	547, 548, 485, 486, 423, 424, 361, 362, 299, 300, 237, 238, 175, 176, 113, -1
};
static const sint16 _scrollpos22[] = {
	80, 81, 146, 147, 212, 213, 278, 279, 344, 345, 410, 411, 412, 413, 414, 415, 416, 417, 418, 419, 420, 421, 422,
	359, 360, 297, 298, 235, 236, 173, 174, 111, 112, -1
};
static const sint16 _scrollpos23[] = {
	79, 144, 145, 210, 275, 340, 341, 342, 407, 408, 409, 410, 475, 476, 477, 478, 479, 480, 481, 482, 483, 484,
	421, 422, 423, 424, 361, 362, 363, 300, 237, 238, 175, 112, -1
};
static const sint16 _scrollpos24[] = {
	584, 585, 522, 523, 460, 461, 398, 399, 336, 337, 274, 275, 212, 213, 150, 151, 88, 89, 26, 27, -1
};
static const sint16 _scrollpos25[] = {
	36, 37, 102, 103, 168, 169, 234, 235, 300, 301, 366, 367, 432, 433, 498, 499, 564, 565, 630, 631, -1
};
static const sint16 _scrollpos26[] = {
	836, 837, 774, 775, 712, 713, 650, 651, 588, 589, 526, 527, 464, 465, 402, 403, 340, 341, 278, 279, 216, 217,
	154, 155, 92, 93, 30, 31, -1
};
static const sint16 _scrollpos27[] = {
	32, 33, 98, 99, 164, 165, 230, 231, 296, 297, 362, 363, 428, 429, 494, 495, 560, 561, 626, 627, 692, 693, 758,
	759, 824, 825, 890, 891, -1
};
static const sint16 _scrollpos28[] = {
	838, 839, 776, 777, 714, 715, 652, 653, 590, 591, 528, 529, 466, 467, 404, 405, 342, 343, 280, 281, 218, 219,
	156, 157, 94, 95, 32, 33, -1
};
static const sint16 _scrollpos29[] = {
	30, 31, 96, 97, 162, 163, 228, 229, 294, 295, 360, 361, 426, 427, 492, 493, 558, 559, 624, 625, 690, 691, 756,
	757, 822, 823, 888, 889, -1
};
static const sint16 _scrollpos30[] = {
	1922, 1923, 1860, 1861, 1798, 1799, 1736, 1737, 1674, 1675, 1612, 1613, 1550, 1551, 1488, 1489, 1426, 1427,
	1364, 1365, 1302, 1303, 1240, 1241, 1178, 1179, 1116, 1117, 1054, 1055, 992, 993, 930, 931, 868, 869, 806,
	807, 744, 745, 682, 683, 620, 621, 558, 559, 496, 497, 434, 435, 372, 373, 310, 311, 248, 249, 186, 187, 124,
	125, 62, -1
};
static const sint16 _scrollpos31[] = {
	1, 66, 67, 132, 133, 198, 199, 264, 265, 330, 331, 396, 397, 462, 463, 528, 529, 594, 595, 660, 661, 726, 727,
	792, 793, 858, 859, 924, 925, 990, 991, 1056, 1057, 1122, 1123, 1188, 1189, 1254, 1255, 1320,
	1321, 1386, 1387, 1452, 1453, 1518, 1519, 1584, 1585, 1650, 1651, 1716, 1717, 1782, 1783, 1848, 1849, 1914,
	1915, 1980, 1981, -1
};
static const sint16 _scrollpos32[] = {
	12, 77, 78, 143, 144, 209, 210, 275, 276, 341, 342, 407, 408, 473, 474, 539, 540, 605, 606, 671, 672, 737, 738,
	803, 804, 869, 870, 935, 936, 1001, 1002, 1067, 1068, 1133, 1134, 1199, 1200, 1265, 1266, -1
};
static const sint16 _scrollpos33[] = {
	1292, 1293, 1230, 1231, 1168, 1169, 1106, 1107, 1044, 1045, 982, 983, 920, 921, 858, 859, 796, 797, 734, 735,
	672, 673, 610, 611, 548, 549, 486, 487, 424, 425, 362, 363, 300, 301, 238, 239, 176, 177, 114, 115, -1
};
static const sint16 _scrollpos34[] = {
	898, 899, 836, 837, 774, 775, 712, 713, 650, 651, 588, 589, 526, 527, 464, 465, 402, 403, 340, 341, 278, 279,
	216, 217, 154, 155, 92, 93, 30, -1
};
static const sint16 _scrollpos35[] = {
	33, 34, 99, 100, 165, 166, 231, 232, 297, 298, 363, 364, 429, 430, 495, 496, 561, 562, 627, 628, 693, 694, 759,
	760, 825, 826, 891, 892, 957, -1
};
static const sint16 _scrollpos36[] = {
	4, 69, 134, 199, 200, 265, 330, 331, 396, 397, 462, 463, 528, 529, 594, 595, 660, 661, 662, 727, 728, 729, 794,
	795, 796, 797, 798, -1
};
static const sint16 _scrollpos37[] = {
	864, 801, 802, 803, 804, 741, 742, 743, 680, 681, 682, 619, 620, 557, 558, 495, 496, 433, 434, 371, 372, 309,
	246, 247, 184, 121, 58, -1
};

static const sint16* _scrollPositions[38] = {
	_scrollpos0,
	_scrollpos1,
	_scrollpos2,
	_scrollpos3,
	_scrollpos4,
	_scrollpos5,
	_scrollpos6,
	_scrollpos7,
	_scrollpos8,
	_scrollpos9,
	_scrollpos10,
	_scrollpos11,
	_scrollpos12,
	_scrollpos13,
	_scrollpos14,
	_scrollpos15,
	_scrollpos16,
	_scrollpos17,
	_scrollpos18,
	_scrollpos19,
	_scrollpos20,
	_scrollpos21,
	_scrollpos22,
	_scrollpos23,
	_scrollpos24,
	_scrollpos25,
	_scrollpos26,
	_scrollpos27,
	_scrollpos28,
	_scrollpos29,
	_scrollpos30,
	_scrollpos31,
	_scrollpos32,
	_scrollpos33,
	_scrollpos34,
	_scrollpos35,
	_scrollpos36,
	_scrollpos37,
};

/**
 *
 *  rct2: 0x006C42D9
 * @param stringId (ax)
 * @param scroll (cx)
 * @param scrollingMode (bp)
 * @returns ebx
 */
int scrolling_text_setup(rct_string_id stringId, uint16 scroll, uint16 scrollingMode)
{
	if (TempForScrollText) {
		memcpy(gCommonFormatArgs, (const void*)0x013CE952, 16);
	}

	rct_drawpixelinfo* dpi = unk_140E9A8;

	if (dpi->zoom_level != 0) return 0x626;

	RCT2_GLOBAL(RCT2_ADDRESS_DRAW_SCROLL_NEXT_ID, uint32)++;

	int scrollIndex = scrolling_text_get_matching_or_oldest(stringId, scroll, scrollingMode);
	if (scrollIndex >= 0x606) return scrollIndex;

	// Setup scrolling text
	uint32 stringArgs0, stringArgs1;
	memcpy(&stringArgs0, gCommonFormatArgs + 0, sizeof(uint32));
	memcpy(&stringArgs1, gCommonFormatArgs + 4, sizeof(uint32));

	rct_draw_scroll_text* scrollText = &gDrawScrollTextList[scrollIndex];
	scrollText->string_id = stringId;
	scrollText->string_args_0 = stringArgs0;
	scrollText->string_args_1 = stringArgs1;
	scrollText->position = scroll;
	scrollText->mode = scrollingMode;
	scrollText->id = RCT2_GLOBAL(RCT2_ADDRESS_DRAW_SCROLL_NEXT_ID, uint32);

	// Create the string to draw
	utf8 scrollString[256];
	scrolling_text_format(scrollString, scrollText);

	const sint16* scrollingModePositions = _scrollPositions[scrollingMode];

	memset(scrollText->bitmap, 0, 320 * 8);
	if (gUseTrueTypeFont) {
		scrolling_text_set_bitmap_for_ttf(scrollString, scroll, scrollText->bitmap, scrollingModePositions);
	} else {
		scrolling_text_set_bitmap_for_sprite(scrollString, scroll, scrollText->bitmap, scrollingModePositions);
	}

	uint32 imageId = 0x606 + scrollIndex;
	drawing_engine_invalidate_image(imageId);
	return imageId;
}

void scrolling_text_set_bitmap_for_sprite(utf8 *text, int scroll, uint8 *bitmap, const sint16 *scrollPositionOffsets)
{
	uint8 characterColour = scrolling_text_get_colour(gCommonFormatArgs[7]);

	utf8 *ch = text;
	while (true) {
		uint32 codepoint = utf8_get_next(ch, (const utf8**)&ch);

		// If at the end of the string loop back to the start
		if (codepoint == 0) {
			ch = text;
			continue;
		}

		// Set any change in colour
		if (codepoint <= FORMAT_COLOUR_CODE_END && codepoint >= FORMAT_COLOUR_CODE_START){
			codepoint -= FORMAT_COLOUR_CODE_START;
			characterColour = g1Elements[4914].offset[codepoint * 4];
			continue;
		}

		// If another type of control character ignore
		if (codepoint < 32) continue;

		int characterWidth = font_sprite_get_codepoint_width(FONT_SPRITE_BASE_TINY, codepoint);
		uint8 *characterBitmap = font_sprite_get_codepoint_bitmap(codepoint);
		for (; characterWidth != 0; characterWidth--, characterBitmap++) {
			// Skip any none displayed columns
			if (scroll != 0) {
				scroll--;
				continue;
			}

			sint16 scrollPosition = *scrollPositionOffsets;
			if (scrollPosition == -1) return;
			if (scrollPosition > -1) {
				uint8 *dst = &bitmap[scrollPosition];
				for (uint8 char_bitmap = *characterBitmap; char_bitmap != 0; char_bitmap >>= 1){
					if (char_bitmap & 1) *dst = characterColour;

					// Jump to next row
					dst += 64;
				}
			}
			scrollPositionOffsets++;
		}
	}
}

void scrolling_text_set_bitmap_for_ttf(utf8 *text, int scroll, uint8 *bitmap, const sint16 *scrollPositionOffsets)
{
	TTFFontDescriptor *fontDesc = ttf_get_font_from_sprite_base(FONT_SPRITE_BASE_TINY);
	if (fontDesc->font == NULL) {
		scrolling_text_set_bitmap_for_sprite(text, scroll, bitmap, scrollPositionOffsets);
		return;
	}

	// Currently only supports one colour
	uint8 colour = 0;

	utf8 *dstCh = text;
	utf8 *ch = text;
	int codepoint;
	while ((codepoint = utf8_get_next(ch, (const utf8**)&ch)) != 0) {
		if (utf8_is_format_code(codepoint)) {
			if (codepoint >= FORMAT_COLOUR_CODE_START && codepoint <= FORMAT_COLOUR_CODE_END) {
				colour = (uint8)codepoint;
			}
		} else {
			dstCh = utf8_write_codepoint(dstCh, codepoint);
		}
	}
	*dstCh = 0;

	if (colour == 0) {
		colour = scrolling_text_get_colour(gCommonFormatArgs[7]);
	} else {
		colour = RCT2_GLOBAL(0x009FF048, uint8*)[(colour - FORMAT_COLOUR_CODE_START) * 4];
	}

	SDL_Surface *surface = ttf_surface_cache_get_or_add(fontDesc->font, text);
	if (surface == NULL) {
		return;
	}

	if (SDL_MUSTLOCK(surface) && SDL_LockSurface(surface) == -1) {
		return;
	}

	int pitch = surface->pitch;
	int width = surface->w;
	int height = surface->h;
	uint8 *src = surface->pixels;

	// Offset
	height -= 3;
	src += 3 * pitch;
	height = min(height, 8);

	int x = 0;
	while (true) {
		// Skip any none displayed columns
		if (scroll == 0) {
			sint16 scrollPosition = *scrollPositionOffsets;
			if (scrollPosition == -1) return;
			if (scrollPosition > -1) {
				uint8 *dst = &bitmap[scrollPosition];

				for (int y = 0; y < height; y++) {
					if (src[y * pitch + x] != 0) *dst = colour;

					// Jump to next row
					dst += 64;
				}
			}
			scrollPositionOffsets++;
		} else {
			scroll--;
		}

		x++;
		if (x >= width) x = 0;
	}

	if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
}
