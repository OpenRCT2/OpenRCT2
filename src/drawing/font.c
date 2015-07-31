#include "../addresses.h"
#include "../localisation/localisation.h"
#include "../sprites.h"
#include "drawing.h"
#include "font.h"

static const int SpriteFontLineHeight[] = { 6, 10, 10, 18 };

static uint8 *_spriteFontCharacterWidths = (uint8*)RCT2_ADDRESS_FONT_CHAR_WIDTH;

TTFFontSetDescriptor *gCurrentTTFFontSet;

/**
 *
 *  rct2: 0x006C19AC
 */
void font_sprite_initialise_characters()
{
	uint8* pCharacterWidth = _spriteFontCharacterWidths;
	for (int fontSize = 0; fontSize < FONT_SIZE_COUNT; fontSize++) {
		int glyphOffset = fontSize * FONT_SPRITE_GLYPH_COUNT;
		for (uint8 glyphIndex = 0; glyphIndex < FONT_SPRITE_GLYPH_COUNT; glyphIndex++) {
			rct_g1_element g1 = g1Elements[glyphIndex + SPR_CHAR_START + glyphOffset];
			
			int width = fontSize == FONT_SIZE_BIG ? g1.width + 1 : g1.width - 1;
			if (glyphIndex >= (FORMAT_ARGUMENT_CODE_START - 32) && glyphIndex < (FORMAT_COLOUR_CODE_END - 32)) {
				width = 0;
			}
			*pCharacterWidth++ = (uint8)width;
		}
	}
	
	scrolling_text_initialise_bitmaps();

	for (int i = 0; i < 32; i++) {
		rct_g1_element* g1 = &g1Elements[0x606 + i];
		uint8* unknown_pointer = RCT2_ADDRESS(0x009C3852, uint8) + 0xA12 * i;
		g1->offset = unknown_pointer;
		g1->width = 64;
		g1->height = 40;
		*((uint16*)unknown_pointer) = 0xFFFF;
		*((uint32*)(unknown_pointer + 0x0E)) = 0;
	}
}

int font_sprite_get_codepoint_offset(int codepoint)
{
	switch (codepoint) {
	case FORMAT_ENDQUOTES: return 34 - 32;

	case FORMAT_AMINUSCULE: return 159 - 32;
	case FORMAT_UP: return 160 - 32;
	case FORMAT_SYMBOL_i: return 160 - 32;
	case FORMAT_CENT: return 162 - 32;
	case FORMAT_POUND: return 163 - 32;

	case FORMAT_YEN: return 165 - 32;



	case FORMAT_COPYRIGHT: return 169 - 32;
	case FORMAT_DOWN: return 170 - 32;
	case FORMAT_LEFTGUILLEMET: return 171 - 32;
	case FORMAT_TICK: return 172 - 32;
	case FORMAT_CROSS: return 173 - 32;

	case FORMAT_RIGHT: return 175 - 32;
	case FORMAT_DEGREE: return 176 - 32;
	case FORMAT_SYMBOL_RAILWAY: return 177 - 32;
	case FORMAT_SQUARED: return 178 - 32;

	case FORMAT_OPENQUOTES: return 180 - 32;
	case FORMAT_EURO: return 181 - 32;
	case FORMAT_SYMBOL_ROAD: return 182 - 32;
	case FORMAT_SYMBOL_FLAG: return 183 - 32;
	case FORMAT_APPROX: return 184 - 32;
	case FORMAT_POWERNEGATIVEONE: return 185 - 32;
	case FORMAT_BULLET: return 186 - 32;
	case FORMAT_RIGHTGUILLEMET: return 187 - 32;
	case FORMAT_SMALLUP: return 188 - 32;
	case FORMAT_SMALLDOWN: return 189 - 32;
	case FORMAT_LEFT: return 190 - 32;
	case FORMAT_INVERTEDQUESTION: return 191 - 32;
	default:
		if (codepoint < 32 || codepoint >= 256) codepoint = '?';
		return codepoint - 32;
	}
}

int font_sprite_get_codepoint_width(int fontSpriteBase, int codepoint)
{
	return _spriteFontCharacterWidths[fontSpriteBase + font_sprite_get_codepoint_offset(codepoint)];
}

int font_sprite_get_codepoint_sprite(int fontSpriteBase, int codepoint)
{
	return SPR_CHAR_START + ((IMAGE_TYPE_USE_PALETTE << 28) | (fontSpriteBase + font_sprite_get_codepoint_offset(codepoint)));
}

int font_get_size_from_sprite_base(uint16 spriteBase)
{
	switch (spriteBase) {
	case FONT_SPRITE_BASE_TINY:
		return 0;
	case FONT_SPRITE_BASE_SMALL:
		return 1;
	default:
	case FONT_SPRITE_BASE_MEDIUM:
		return 2;
	case FONT_SPRITE_BASE_BIG:
		return 3;
	}
}

int font_get_line_height(int fontSpriteBase)
{
	int fontSize = font_get_size_from_sprite_base(fontSpriteBase);
	if (gUseTrueTypeFont) {
		return gCurrentTTFFontSet->size[fontSize].line_height;
	} else {
		return SpriteFontLineHeight[fontSize];
	}
}

int font_get_line_height_small(int fontSpriteBase)
{
	return font_get_line_height(fontSpriteBase) / 2;
}
