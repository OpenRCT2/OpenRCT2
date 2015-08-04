#include "../addresses.h"
#include "../config.h"
#include "../localisation/localisation.h"
#include "drawing.h"

/* size: 0xA12 */
typedef struct {
	rct_string_id string_id;	// 0x00
	uint32 string_args_0;		// 0x02
	uint32 string_args_1;		// 0x06
	uint16 position;			// 0x0A
	uint16 mode;				// 0x0C
	uint32 id;					// 0x0E
	uint8 bitmap[64 * 8 * 5];	// 0x12
} rct_draw_scroll_text;

rct_draw_scroll_text *gDrawScrollTextList = RCT2_ADDRESS(RCT2_ADDRESS_DRAW_SCROLL_LIST, rct_draw_scroll_text);
uint8 *gCharacterBitmaps = RCT2_ADDRESS(RCT2_ADDRESS_CHARACTER_BITMAP, uint8);

void scrolling_text_set_bitmap_for_sprite(utf8 *text, int scroll, uint8 *bitmap, sint16 *scrollPositionOffsets);
void scrolling_text_set_bitmap_for_ttf(utf8 *text, int scroll, uint8 *bitmap, sint16 *scrollPositionOffsets);

void scrolling_text_initialise_bitmaps()
{
	uint8 drawingSurface[64];
	rct_drawpixelinfo dpi = {
		.bits = (char*)&drawingSurface,
		.x = 0,
		.y = 0,
		.width = 8,
		.height = 8,
		.pitch = 0,
		.zoom_level = 0
	};

	
	for (int i = 0; i < 224; i++) {
		memset(drawingSurface, 0, sizeof(drawingSurface));
		gfx_draw_sprite(&dpi, i + 0x10D5, -1, 0, 0);

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
		if (
			scrollText->string_id == stringId &&
			scrollText->string_args_0 == RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint32) &&
			scrollText->string_args_1 == RCT2_GLOBAL(0x13CE956, uint32) &&
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
	int edi = character & 0x7F;
	int offset = 0;
	if (character >= 0x80) offset = 2;
	return RCT2_ADDRESS(0x0141FC47, uint8)[offset + (edi * 8)];
}

static void scrolling_text_format(utf8 *dst, rct_draw_scroll_text *scrollText)
{
	if (gConfigGeneral.upper_case_banners) {
		format_string_to_upper(dst, scrollText->string_id, &scrollText->string_args_0);
	} else {
		format_string(dst, scrollText->string_id, &scrollText->string_args_0);
	}
}

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
	rct_drawpixelinfo* dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*);

	if (dpi->zoom_level != 0) return 0x626;

	RCT2_GLOBAL(RCT2_ADDRESS_DRAW_SCROLL_NEXT_ID, uint32)++;

	int scrollIndex = scrolling_text_get_matching_or_oldest(stringId, scroll, scrollingMode);
	if (scrollIndex >= 0x606) return scrollIndex;

	// Setup scrolling text
	rct_draw_scroll_text* scrollText = &gDrawScrollTextList[scrollIndex];
	scrollText->string_id = stringId;
	scrollText->string_args_0 = RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint32);
	scrollText->string_args_1 = RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 4, uint32);
	scrollText->position = scroll;
	scrollText->mode = scrollingMode;
	scrollText->id = RCT2_GLOBAL(RCT2_ADDRESS_DRAW_SCROLL_NEXT_ID, uint32);

	// Create the string to draw
	utf8 scrollString[256];
	scrolling_text_format(scrollString, scrollText);

	sint16* scrollingModePositions = RCT2_ADDRESS(RCT2_ADDRESS_SCROLLING_MODE_POSITIONS, uint16*)[scrollingMode];
	
	memset(scrollText->bitmap, 0, 320 * 8);
	if (gUseTrueTypeFont) {
		scrolling_text_set_bitmap_for_ttf(scrollString, scroll, scrollText->bitmap, scrollingModePositions);
	} else {
		scrolling_text_set_bitmap_for_sprite(scrollString, scroll, scrollText->bitmap, scrollingModePositions);
	}

	return scrollIndex + 0x606;
}

void scrolling_text_set_bitmap_for_sprite(utf8 *text, int scroll, uint8 *bitmap, sint16 *scrollPositionOffsets)
{
	uint8 characterColour = scrolling_text_get_colour(RCT2_GLOBAL(0x013CE959, uint8));

	utf8 *ch = text;
	while (true) {
		uint32 codepoint = utf8_get_next(ch, &ch);

		// If at the end of the string loop back to the start
		if (codepoint == 0) {
			ch = text;
			continue;
		}

		// Set any change in colour
		if (codepoint <= FORMAT_COLOUR_CODE_END && codepoint >= FORMAT_COLOUR_CODE_START){
			codepoint -= FORMAT_COLOUR_CODE_START;
			characterColour = RCT2_GLOBAL(0x009FF048, uint8*)[codepoint * 4];
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

TTFFontDescriptor *ttf_get_font_from_sprite_base(uint16 spriteBase);
SDL_Surface *_ttf_surface_cache_get_or_add(TTF_Font *font, const utf8 *text);

void scrolling_text_set_bitmap_for_ttf(utf8 *text, int scroll, uint8 *bitmap, sint16 *scrollPositionOffsets)
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
	while ((codepoint = utf8_get_next(ch, &ch)) != 0) {
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
		colour = scrolling_text_get_colour(RCT2_GLOBAL(0x013CE959, uint8));
	} else {
		colour = RCT2_GLOBAL(0x009FF048, uint8*)[(colour - FORMAT_COLOUR_CODE_START) * 4];
	}

	SDL_Surface *surface = _ttf_surface_cache_get_or_add(fontDesc->font, text);
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
