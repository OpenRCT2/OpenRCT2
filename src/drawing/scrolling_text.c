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
			RCT2_ADDRESS(RCT2_ADDRESS_CHARACTER_BITMAP, uint8)[i * 8 + x] = val;
		}

	}
}

/**
 *
 *  rct2: 0x006C42D9
 */
int scrolling_text_setup(rct_string_id stringId, uint16 scroll, uint16 scrollingMode)
{
	rct_drawpixelinfo* dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*);

	if (dpi->zoom_level != 0) return 0x626;

	RCT2_GLOBAL(RCT2_ADDRESS_DRAW_SCROLL_NEXT_ID, uint32)++;

	// Find the oldest scroll for use as the newest
	uint32 oldestId = 0xFFFFFFFF;
	uint8 scrollIndex = 0xFF;
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

	// Setup scrolling text
	rct_draw_scroll_text* scrollText = oldestScroll;
	scrollText->string_id = stringId;
	scrollText->string_args_0 = RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint32);
	scrollText->string_args_1 = RCT2_GLOBAL(0x13CE956, uint32);
	scrollText->position = scroll;
	scrollText->mode = scrollingMode;
	scrollText->id = RCT2_GLOBAL(RCT2_ADDRESS_DRAW_SCROLL_NEXT_ID, uint32_t);

	uint8* scrollPixelPointer = scrollText->bitmap;
	memset(scrollPixelPointer, 0, 320 * 8);

	// Convert string id back into a string for processing
	utf8 scrollString[MAX_PATH];
	if (gConfigGeneral.upper_case_banners)
		format_string_to_upper(scrollString, stringId, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS);
	else
		format_string(scrollString, stringId, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS);

	// Setup character colour from ???
	uint32 character = RCT2_GLOBAL(0x13CE959, uint8);
	int edi = character & 0x7F;
	int offs = 0;
	if (character >= 0x80) offs = 2;
	uint8 characterColour = RCT2_ADDRESS(0x0141FC47, uint8)[offs + edi * 8];

	sint16* scrollingModePositions = RCT2_ADDRESS(RCT2_ADDRESS_SCROLLING_MODE_POSITIONS, uint16_t*)[scrollingMode];
	uint8* formatResult = scrollString;
	while (true) {
		character = utf8_get_next(formatResult, &formatResult);

		// If at the end of the string loop back to the start
		if (character == 0) {
			formatResult = scrollString;
			continue;
		}

		// Set any change in colour
		if (character <= FORMAT_COLOUR_CODE_END && character >= FORMAT_COLOUR_CODE_START){
			character -= FORMAT_COLOUR_CODE_START;
			characterColour = RCT2_GLOBAL(0x009FF048, uint8*)[character * 4];
			continue;
		}

		// If another type of control character ignore
		if (character < 32) continue;

		// Convert to an indexable character
		character = utf8_get_sprite_offset_for_codepoint(character);

		uint8 characterWidth = RCT2_ADDRESS(RCT2_ADDRESS_FONT_CHAR_WIDTH + 448, uint8)[character];
		uint8* characterBitmap = &(RCT2_ADDRESS(RCT2_ADDRESS_CHARACTER_BITMAP, uint8)[character * 8]);
		for (; characterWidth != 0; characterWidth--, characterBitmap++) {
			// Skip any none displayed columns
			if (scroll != 0){
				scroll--;
				continue;
			}

			sint16 scrollPosition = *scrollingModePositions;
			if (scrollPosition == -1) return scrollIndex + 0x606;
			if (scrollPosition > -1) {
				uint8* dst = &scrollPixelPointer[scrollPosition];
				for (uint8 char_bitmap = *characterBitmap; char_bitmap != 0; char_bitmap >>= 1){
					if (char_bitmap & 1) *dst = characterColour;
					
					// Jump to next row 
					dst += 64;
				}
			}
			scrollingModePositions++;
		}
	}
}