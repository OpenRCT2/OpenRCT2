/*****************************************************************************
 * Copyright (c) 2014 Ted John, Peter Hill, Duncan Frost
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include <SDL_ttf.h>
#include "../addresses.h"
#include "../localisation/localisation.h"
#include "../sprites.h"
#include "../world/map.h"
#include "drawing.h"

static int ttf_get_string_width(const utf8 *text);
static void ttf_draw_string(rct_drawpixelinfo *dpi, char *buffer, int colour, int x, int y);

static bool _ttfInitialised = false;

#define TTF_SURFACE_CACHE_SIZE 256
#define TTF_GETWIDTH_CACHE_SIZE 1024

typedef struct {
	SDL_Surface *surface;
	TTF_Font *font;
	utf8 *text;
	uint32 lastUseTick;
} ttf_cache_entry;

static ttf_cache_entry _ttfSurfaceCache[TTF_SURFACE_CACHE_SIZE] = { 0 };
static int _ttfSurfaceCacheCount = 0;
static int _ttfSurfaceCacheHitCount = 0;
static int _ttfSurfaceCacheMissCount = 0;

typedef struct {
	uint32 width;
	TTF_Font *font;
	utf8 *text;
	uint32 lastUseTick;
} ttf_getwidth_cache_entry;

static ttf_getwidth_cache_entry _ttfGetWidthCache[TTF_GETWIDTH_CACHE_SIZE] = { 0 };
static int _ttfGetWidthCacheCount = 0;
static int _ttfGetWidthCacheHitCount = 0;
static int _ttfGetWidthCacheMissCount = 0;

/**
 *
 *  rct2: 0x006C23B1
 */
int gfx_get_string_width_new_lined(utf8 *text)
{
	utf8 *ch = text;
	utf8 *firstCh = text;
	utf8 *nextCh;
	utf8 backup;
	int codepoint;

	int width = 0;
	int maxWidth = 0;
	while ((codepoint = utf8_get_next(ch, &nextCh)) != 0) {
		if (codepoint == FORMAT_NEWLINE || codepoint == FORMAT_NEWLINE_SMALLER) {
			backup = *nextCh;
			*nextCh = 0;
			maxWidth = max(maxWidth, gfx_get_string_width(firstCh));
			*nextCh = backup;
			firstCh = nextCh;
		}
		ch = nextCh;
	}
	maxWidth = max(maxWidth, gfx_get_string_width(firstCh));

	return maxWidth;
}

/**
 *  Return the width of the string in buffer
 *
 *  rct2: 0x006C2321
 * buffer (esi)
 */
int gfx_get_string_width(char* buffer)
{
	return ttf_get_string_width(buffer);
}

/**
 *  Clip the text in buffer to width, add ellipsis and return the new width of the clipped string
 *
 *  rct2: 0x006C2460
 * buffer (esi)
 * width (edi)
 */
int gfx_clip_string(utf8 *text, int width)
{
	int clippedWidth;

	if (width < 6) {
		*text = 0;
		return 0;
	}

	clippedWidth = gfx_get_string_width(text);
	if (clippedWidth <= width) {
		return clippedWidth;
	}

	utf8 backup[4];
	utf8 *ch = text;
	utf8 *nextCh = text;
	utf8 *clipCh = text;
	int codepoint;
	while ((codepoint = utf8_get_next(ch, &nextCh)) != 0) {
		if (utf8_is_format_code(codepoint)) {
			ch = nextCh;
			ch += utf8_get_format_code_arg_length(codepoint);
			continue;
		}

		for (int i = 0; i < 4; i++) { backup[i] = nextCh[i]; };
		for (int i = 0; i < 3; i++) { nextCh[i] = '.'; }
		nextCh[3] = 0;

		int queryWidth = gfx_get_string_width(text);
		if (queryWidth < width) {
			clipCh = nextCh;
			clippedWidth = queryWidth;
		} else {
			for (int i = 0; i < 3; i++) { clipCh[i] = '.'; }
			clipCh[3] = 0;
			return clippedWidth;
		}

		for (int i = 0; i < 4; i++) { nextCh[i] = backup[i]; };
		ch = nextCh;
	}
	return gfx_get_string_width(text);
}

/**
 *  Wrap the text in buffer to width, returns width of longest line.
 *
 *  Inserts NULL where line should break (as \n is used for something else),
 *  so the number of lines is returned in num_lines. font_height seems to be
 *  a control character for line height.
 *
 *  rct2: 0x006C21E2
 * buffer (esi)
 * width (edi) - in
 * num_lines (edi) - out
 * font_height (ebx) - out
 */
int gfx_wrap_string(utf8 *text, int width, int *outNumLines, int *outFontHeight)
{
	int lineWidth = 0;
	int maxWidth = 0;
	*outNumLines = 0;

	// Pointer to the start of the current word
	utf8 *currentWord = NULL;

	// Width of line up to current word
	int currentWidth;

	utf8 *ch = text;
	utf8 *firstCh = text;
	utf8 *nextCh;
	int codepoint;
	while ((codepoint = utf8_get_next(ch, &nextCh)) != 0) {
		if (codepoint == ' ') {
			currentWord = ch;
			currentWidth = lineWidth;
		} else if (codepoint == FORMAT_NEWLINE) {
			*ch++ = 0;
			maxWidth = max(maxWidth, lineWidth);
			(*outNumLines)++;
			lineWidth = 0;
			currentWord = NULL;
			firstCh = ch;
			continue;
		} else if (utf8_is_format_code(codepoint)) {
			ch = nextCh;
			ch += utf8_get_format_code_arg_length(codepoint);
			continue;
		}

		uint8 saveCh = *nextCh;
		*nextCh = 0;
		lineWidth = gfx_get_string_width(firstCh);
		*nextCh = saveCh;

		if (lineWidth <= width) {
			ch = nextCh;
		} else if (currentWord == NULL) {
			// Single word is longer than line, insert null terminator
			ch += utf8_insert_codepoint(ch, 0);
			maxWidth = max(maxWidth, lineWidth);
			(*outNumLines)++;
			lineWidth = 0;
			currentWord = NULL;
			firstCh = ch;
		} else {
			ch = currentWord;
			*ch++ = 0;

			maxWidth = max(maxWidth, currentWidth);
			(*outNumLines)++;
			lineWidth = 0;
			currentWord = NULL;
			firstCh = ch;
		}
	}

	*outFontHeight = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16);
	return maxWidth == 0 ? lineWidth : maxWidth;
}

/**
 * Draws i formatted text string left aligned at i specified position but clips
 * the text with an elipsis if the text width exceeds the specified width.
 *  rct2: 0x006C1B83
 * dpi (edi)
 * format (bx)
 * args (esi)
 * colour (al)
 * x (cx)
 * y (dx)
 * width (bp)
 */
void gfx_draw_string_left_clipped(rct_drawpixelinfo* dpi, int format, void* args, int colour, int x, int y, int width)
{
	char* buffer;

	buffer = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char);
	format_string(buffer, format, args);

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 0xE0;

	// Clip text - return value is not needed
	gfx_clip_string(buffer, width);

	gfx_draw_string(dpi, buffer, colour, x, y);
}

/**
 * Draws i formatted text string centred at i specified position but clips the
 * text with an elipsis if the text width exceeds the specified width.
 *  rct2: 0x006C1BBA
 * dpi (edi)
 * format (bx)
 * args (esi)
 * colour (al)
 * x (cx)
 * y (dx)
 * width (bp)
 */
void gfx_draw_string_centred_clipped(rct_drawpixelinfo *dpi, int format, void *args, int colour, int x, int y, int width)
{
	char* buffer;
	short text_width;

	buffer = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char);
	format_string(buffer, format, args);

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 0xE0;

	// Clip text
	text_width = gfx_clip_string(buffer, width);

	// Draw the text centred
	if (text_width <= 0xFFFF) {
		x -= (text_width - 1) / 2;
		gfx_draw_string(dpi, buffer, colour, x, y);
	}
}

/**
 * Draws i formatted text string right aligned.
 *  rct2: 0x006C1BFC
 * dpi (edi)
 * format (bx)
 * args (esi)
 * colour (al)
 * x (cx)
 * y (dx)
 */
void gfx_draw_string_right(rct_drawpixelinfo* dpi, int format, void* args, int colour, int x, int y)
{
	char* buffer;
	short text_width;

	buffer = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char);
	format_string(buffer, format, args);

	// Measure text width
	text_width = gfx_get_string_width(buffer);

	// Draw the text right aligned
	x -= text_width;
	gfx_draw_string(dpi, buffer, colour, x, y);
}

/**
 * Draws i formatted text string centred at i specified position.
 *  rct2: 0x006C1D6C
 * dpi (edi)
 * format (bx)
 * x (cx)
 * y (dx)
 * colour (al)
 * args (esi)
 */
void gfx_draw_string_centred(rct_drawpixelinfo *dpi, int format, int x, int y, int colour, void *args)
{
	char* buffer;
	short text_width;

	buffer = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char);
	format_string(buffer, format, args);

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 0xE0;

	// Measure text width
	text_width = gfx_get_string_width(buffer);

	// Draw the text centred
	if (text_width <= 0xFFFF) {
		x -= text_width / 2;
		gfx_draw_string(dpi, buffer, colour, x, y);
	}
}

/**
 * 
 *  rct2: 0x006C1E53
 * dpi (edi)
 * args (esi)
 * x (cx)
 * y (dx)
 * width (bp)
 * colour (al)
 * format (ebx)
 */
int gfx_draw_string_centred_wrapped(rct_drawpixelinfo *dpi, void *args, int x, int y, int width, int format, int colour)
{
	int font_height, line_height, line_width, line_y, num_lines;
	// Location of font sprites
	uint16* current_font_sprite_base;

	char* buffer = RCT2_ADDRESS(0x009C383D, char);

	current_font_sprite_base = RCT2_ADDRESS(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16);
	*current_font_sprite_base = 0xE0;

	gfx_draw_string(dpi, buffer, colour, dpi->x, dpi->y);

	buffer = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char);

	format_string(buffer, format, args);

	*current_font_sprite_base = 0xE0;

	// line_width unused here
	line_width = gfx_wrap_string(buffer, width, &num_lines, &font_height);
	line_height = font_get_line_height(font_height);

	if (*buffer == FORMAT_OUTLINE) {
		line_height = line_height + 1;
	}

	font_height = (line_height / 2) * num_lines;
	line_y = y - font_height;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_FLAGS, uint16) = 0;

	for (int line = 0; line <= num_lines; ++line) {
		int half_width = gfx_get_string_width(buffer) / 2;
		gfx_draw_string(dpi, buffer, 0xFE, x - half_width, line_y);

		buffer = get_string_end(buffer) + 1;
		line_y += line_height;
	}

	return line_y - y;
}

/**
 * 
 *  rct2: 0x006C2105
 * dpi (edi)
 * args (esi)
 * x (cx)
 * y (dx)
 * width (bp)
 * format (bx)
 * colour (al)
 */
int gfx_draw_string_left_wrapped(rct_drawpixelinfo *dpi, void *args, int x, int y, int width, int format, int colour)
{
	// font height might actually be something else
	int fontSpriteBase, lineHeight, lineY, numLines;

	// Location of font sprites
	uint16* current_font_sprite_base = RCT2_ADDRESS(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16);
	*current_font_sprite_base = FONT_SPRITE_BASE_MEDIUM;

	char* buffer = RCT2_ADDRESS(0x009C383D, char);
	gfx_draw_string(dpi, buffer, colour, dpi->x, dpi->y);
	buffer = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char);
	format_string(buffer, format, args);

	*current_font_sprite_base = FONT_SPRITE_BASE_MEDIUM;
	gfx_wrap_string(buffer, width, &numLines, &fontSpriteBase);
	lineHeight = font_get_line_height(fontSpriteBase);

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_FLAGS, uint16) = 0;
	lineY = y;
	for (int line = 0; line <= numLines; ++line) {
		gfx_draw_string(dpi, buffer, 0xFE, x, lineY);
		buffer = get_string_end(buffer) + 1;
		lineY += lineHeight;
	} 
	return lineY - y;
}

/**
 * Draws i formatted text string.
 *  rct2: 0x006C1B2F
 * dpi (edi)
 * format (bx)
 * args (esi)
 * colour (al)
 * x (cx)
 * y (dx)
 */
void gfx_draw_string_left(rct_drawpixelinfo *dpi, int format, void *args, int colour, int x, int y)
{
	char* buffer;

	buffer = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char);
	format_string(buffer, format, args);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 224;
	gfx_draw_string(dpi, buffer, colour, x, y);
}

/**
 * Draws text that is left aligned and vertically centred.
 */
void gfx_draw_string_left_centred(rct_drawpixelinfo *dpi, rct_string_id format, void *args, int colour, int x, int y)
{
	char* buffer;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 224;
	buffer = (char*)RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER;
	format_string(buffer, format, args);
	int height = string_get_height_raw(buffer);
	gfx_draw_string(dpi, buffer, colour, x, y - (height / 2));
}

/**
 * Changes the palette so that the next character changes colour
 */
void colour_char(uint8 colour, uint16* current_font_flags, uint8* palette_pointer) {

	int eax;

	rct_g1_element g1_element = g1Elements[0x1332];
	eax = ((uint32*)g1_element.offset)[colour & 0xFF];

	if (!(*current_font_flags & 2)) {
		eax = eax & 0x0FF0000FF;
	}
	// Adjust text palette. Store current colour? 
	palette_pointer[1] = eax & 0xFF;
	palette_pointer[2] = (eax >> 8) & 0xFF;
	palette_pointer[3] = (eax >> 16) & 0xFF;
	palette_pointer[4] = (eax >> 24) & 0xFF;
	RCT2_GLOBAL(0x009ABDA4, uint32) = (uint32)palette_pointer;
}

/**
 * Changes the palette so that the next character changes colour
 *  This is specific to changing to a predefined window related colour
 */
void colour_char_window(uint8 colour, uint16* current_font_flags,uint8* palette_pointer) {

	int eax;

	eax = RCT2_ADDRESS(0x0141FD45, uint8)[colour * 8];
	if (*current_font_flags & 2) {
		eax |= 0x0A0A00;
	}
	 //Adjust text palette. Store current colour? 
	palette_pointer[1] = eax & 0xFF;
	palette_pointer[2] = (eax >> 8) & 0xFF;
	palette_pointer[3] = (eax >> 16) & 0xFF;
	palette_pointer[4] = (eax >> 24) & 0xFF;
	RCT2_GLOBAL(0x009ABDA4, uint32) = (uint32)palette_pointer;
}

/**
 * 
 *  rct2: 0x00682702
 * dpi (edi)
 * buffer (esi)
 * colour (al)
 * x (cx)
 * y (dx)
 */
void gfx_draw_string(rct_drawpixelinfo *dpi, char *buffer, int colour, int x, int y)
{
	ttf_draw_string(dpi, buffer, colour, x, y);
}

void draw_string_left_underline(rct_drawpixelinfo *dpi, int format, void *args, int colour, int x, int y)
{
	char buffer[128];
	int width;

	format_string(buffer, format, args);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 224;
	width = gfx_get_string_width(buffer);
	gfx_draw_string(dpi, buffer, colour, x, y);
	gfx_fill_rect(dpi, x, y + 11, x + width, y + 11, text_palette[1]);
	if (text_palette[2] != 0)
		gfx_fill_rect(dpi, x + 1, y + 12, x + width + 1, y + 12, text_palette[2]);
}

void draw_string_right_underline(rct_drawpixelinfo *dpi, int format, void *args, int colour, int x, int y)
{
	char buffer[128];
	int width;

	format_string(buffer, format, args);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 224;
	width = gfx_get_string_width(buffer);
	x -= width;
	gfx_draw_string(dpi, buffer, colour, x, y);
	gfx_fill_rect(dpi, x, y + 11, x + width, y + 11, text_palette[1]);
	if (text_palette[2] != 0)
		gfx_fill_rect(dpi, x + 1, y + 12, x + width + 1, y + 12, text_palette[2]);
}

void draw_string_centred_underline(rct_drawpixelinfo *dpi, int format, void *args, int colour, int x, int y)
{
	char buffer[128];
	int width;

	format_string(buffer, format, args);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 224;
	width = gfx_get_string_width(buffer);
	x -= width / 2;
	gfx_draw_string(dpi, buffer, colour, x, y);
	gfx_fill_rect(dpi, x, y + 11, x + width, y + 11, text_palette[1]);
	if (text_palette[2] != 0)
		gfx_fill_rect(dpi, x + 1, y + 12, x + width + 1, y + 12, text_palette[2]);
}

/**
 *
 * rct2: 0x006C1DB7
 *
 * left     : cx
 * top      : dx
 * numLines : bp
 * text     : esi
 * dpi      : edi
 */
void draw_string_centred_raw(rct_drawpixelinfo *dpi, int x, int y, int numLines, char *text)
{
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 224;
	gfx_draw_string(dpi, (char*)0x009C383D, 0, dpi->x, dpi->y);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_FLAGS, uint16) = 0;

	for (int i = 0; i <= numLines; i++) {
		int width = gfx_get_string_width(text);
		gfx_draw_string(dpi, text, 254, x - (width / 2), y);

		char c;
		while ((c = *text++) != 0) {
			if (c >= 32) continue;
			if (c <= 4) {
				text++;
				continue;
			}
			if (c <= 16) continue;
			text += 2;
			if (c <= 22) continue;
			text += 2;
		}

		y += 10;
		if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) <= 224)
			continue;
		
		y -= 4;
		if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) <= 448)
			continue;

		y += 12;
	}
}

int string_get_height_raw(char *buffer)
{
	uint16 fontBase = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16);

	int height = 0;
	if (fontBase <= 224)
		height += 10;
	else if (fontBase == 448)
		height += 6;

	char *ch = buffer;
	while (*ch != 0) {
		char c = *ch++;
		switch (c) {
		case FORMAT_NEWLINE:
			if (fontBase <= 224) {
				height += 10;
				break;
			} else if (fontBase == 448) {
				height += 6;
				break;
			}
			height += 18;
			break;
		case FORMAT_NEWLINE_SMALLER:
			if (fontBase <= 224) {
				height += 5;
				break;
			} else if (fontBase == 448) {
				height += 3;
				break;
			}
			height += 9;
			break;
		case FORMAT_TINYFONT:
			fontBase = 448;
			break;
		case FORMAT_BIGFONT:
			fontBase = 672;
			break;
		case FORMAT_MEDIUMFONT:
			fontBase = 224;
			break;
		case FORMAT_SMALLFONT:
			fontBase = 0;
			break;
		default:
			if (c >= 32) continue;
			if (c <= 4) {
				ch++;
				continue;
			}
			if (c <= 16) continue;
			ch += 2;
			if (c <= 22) continue;
			ch += 2;
			break;
		}
	}

	return height;
}

/**
 *
 * rct2: 0x006C1F57
 *
 * colour   : al
 * format   : bx
 * x        : cx
 * y        : dx
 * text     : esi
 * dpi      : edi
 * width    : bp
 * ticks    : ebp >> 16
 */
void gfx_draw_string_centred_wrapped_partial(rct_drawpixelinfo *dpi, int x, int y, int width, int colour, rct_string_id format, void *args, int ticks)
{
	int numLines, fontSpriteBase, lineHeight, lineY;
	utf8 *buffer = (utf8*)RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = FONT_SPRITE_BASE_MEDIUM;
	gfx_draw_string(dpi, (char*)0x009C383D, colour, dpi->x, dpi->y);
	format_string(buffer, format, args);


	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = FONT_SPRITE_BASE_MEDIUM;
	gfx_wrap_string(buffer, width, &numLines, &fontSpriteBase);
	lineHeight = font_get_line_height(fontSpriteBase);

	int numCharactersDrawn = 0;
	int numCharactersToDraw = ticks;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_FLAGS, uint16) = 0;
	lineY = y - ((numLines * lineHeight) / 2);
	for (int line = 0; line <= numLines; line++) {
		int halfWidth = gfx_get_string_width(buffer) / 2;
		
		utf8 *ch = buffer;
		utf8 *nextCh;
		int codepoint;
		while ((codepoint = utf8_get_next(ch, &nextCh)) != 0) {
			if (!utf8_is_format_code(codepoint)) {
				numCharactersDrawn++;
				if (numCharactersDrawn > numCharactersToDraw) {
					*ch = 0;
					break;
				}
			}
			ch = nextCh;
		}

		gfx_draw_string(dpi, buffer, 0xFE, x - halfWidth, lineY);

		if (numCharactersDrawn > numCharactersToDraw) {
			break;
		}

		buffer = get_string_end(buffer) + 1;
		lineY += lineHeight;
	} 
}

static uint32 _ttf_surface_cache_hash(TTF_Font *font, const utf8 *text)
{
	uint32 hash = ((uint32)font * 23) ^ 0xAAAAAAAA;
	for (const uint8 *ch = text; *ch != 0; ch++) {
		hash = ror32(hash, 3) ^ (*ch * 13);
	}
	return hash;
}

static void _ttf_surface_cache_dispose(ttf_cache_entry *entry)
{
	if (entry->surface != NULL) {
		SDL_FreeSurface(entry->surface);
		free(entry->text);

		entry->surface = NULL;
		entry->font = NULL;
		entry->text = NULL;
	}
}

static void _ttf_surface_cache_dispose_all()
{
	for (int i = 0; i < TTF_SURFACE_CACHE_SIZE; i++) {
		_ttf_surface_cache_dispose(&_ttfSurfaceCache[i]);
		_ttfSurfaceCacheCount--;
	}
}

SDL_Surface *_ttf_surface_cache_get_or_add(TTF_Font *font, const utf8 *text)
{
	ttf_cache_entry *entry;

	uint32 hash = _ttf_surface_cache_hash(font, text);
	int index = hash % TTF_SURFACE_CACHE_SIZE;
	for (int i = 0; i < TTF_SURFACE_CACHE_SIZE; i++) {
		entry = &_ttfSurfaceCache[index];

		// Check if entry is a hit
		if (entry->surface == NULL) break;
		if (entry->font == font && strcmp(entry->text, text) == 0) {
			_ttfSurfaceCacheHitCount++;
			entry->lastUseTick = gCurrentDrawCount;
			return entry->surface;
		}

		// If entry hasn't been used for a while, replace it
		if (entry->lastUseTick < gCurrentDrawCount - 64) {
			break;
		}

		// Check if next entry is a hit
		if (++index >= TTF_SURFACE_CACHE_SIZE) index = 0;
	}

	// Cache miss, replace entry with new surface
	entry = &_ttfSurfaceCache[index];
	_ttf_surface_cache_dispose(entry);

	SDL_Color c = { 0, 0, 0, 255 };
	SDL_Surface *surface = TTF_RenderUTF8_Solid(font, text, c);
	if (surface == NULL) {
		return NULL;
	}

	_ttfSurfaceCacheMissCount++;
	// printf("CACHE HITS: %d   MISSES: %d)\n", _ttfSurfaceCacheHitCount, _ttfSurfaceCacheMissCount);

	_ttfSurfaceCacheCount++;
	entry->surface = surface;
	entry->font = font;
	entry->text = _strdup(text);
	entry->lastUseTick = gCurrentDrawCount;
	return entry->surface;
}

static void _ttf_getwidth_cache_dispose(ttf_getwidth_cache_entry *entry)
{
	if (entry->text != NULL) {
		free(entry->text);

		entry->width = 0;
		entry->font = NULL;
		entry->text = NULL;
	}
}

static void _ttf_getwidth_cache_dispose_all()
{
	for (int i = 0; i < TTF_GETWIDTH_CACHE_SIZE; i++) {
		_ttf_getwidth_cache_dispose(&_ttfGetWidthCache[i]);
		_ttfGetWidthCacheCount--;
	}
}

static uint32 _ttf_getwidth_cache_get_or_add(TTF_Font *font, const utf8 *text)
{
	ttf_getwidth_cache_entry *entry;

	uint32 hash = _ttf_surface_cache_hash(font, text);
	int index = hash % TTF_GETWIDTH_CACHE_SIZE;
	for (int i = 0; i < TTF_GETWIDTH_CACHE_SIZE; i++) {
		entry = &_ttfGetWidthCache[index];

		// Check if entry is a hit
		if (entry->text == NULL) break;
		if (entry->font == font && strcmp(entry->text, text) == 0) {
			_ttfGetWidthCacheHitCount++;
			entry->lastUseTick = gCurrentDrawCount;
			return entry->width;
		}

		// If entry hasn't been used for a while, replace it
		if (entry->lastUseTick < gCurrentDrawCount - 64) {
			break;
		}

		// Check if next entry is a hit
		if (++index >= TTF_GETWIDTH_CACHE_SIZE) index = 0;
	}

	// Cache miss, replace entry with new width
	entry = &_ttfGetWidthCache[index];
	_ttf_getwidth_cache_dispose(entry);

	int width, height;
	TTF_SizeUTF8(font, text, &width, &height);

	_ttfGetWidthCacheMissCount++;

	_ttfGetWidthCacheCount++;
	entry->width = width;
	entry->font = font;
	entry->text = _strdup(text);
	entry->lastUseTick = gCurrentDrawCount;
	return entry->width;
}

bool ttf_initialise()
{
	if (!_ttfInitialised) {
		if (TTF_Init() != 0)
			return false;

		for (int i = 0; i < 4; i++) {
			TTFFontDescriptor *fontDesc = &(gCurrentTTFFontSet->size[i]);
			
			utf8 fontPath[MAX_PATH] = "C:\\Windows\\Fonts\\";
			strcat(fontPath, fontDesc->filename);

			fontDesc->font = TTF_OpenFont(fontPath, fontDesc->ptSize);
			if (fontDesc->font == NULL) {
				TTF_Quit();
				return false;
			}
		}

		_ttfInitialised = true;
	}
	return true;
}

void ttf_dispose()
{
	if (!_ttfInitialised)
		return;

	_ttf_surface_cache_dispose_all();
	_ttf_getwidth_cache_dispose_all();

	for (int i = 0; i < 4; i++) {
		TTFFontDescriptor *fontDesc = &(gCurrentTTFFontSet->size[i]);
		if (fontDesc->font != NULL) {
			TTF_CloseFont(fontDesc->font);
			fontDesc->font = NULL;
		}
	}

	TTF_Quit();
	_ttfInitialised = false;
}

TTFFontDescriptor *ttf_get_font_from_sprite_base(uint16 spriteBase)
{
	return &gCurrentTTFFontSet->size[font_get_size_from_sprite_base(spriteBase)];
}

enum {
	TEXT_DRAW_FLAG_INSET = 1 << 0,
	TEXT_DRAW_FLAG_OUTLINE = 1 << 1,
	TEXT_DRAW_FLAG_TTF = 1 << 30,
	TEXT_DRAW_FLAG_NO_DRAW = 1 << 31
};

typedef struct {
	int startX;
	int startY;
	int x;
	int y;
	int flags;
	uint8 palette[8];
	uint16 font_sprite_base;
} text_draw_info;

static void ttf_draw_character_sprite(rct_drawpixelinfo *dpi, int codepoint, text_draw_info *info)
{
	int characterWidth = font_sprite_get_codepoint_width(info->font_sprite_base, codepoint);
	int sprite = font_sprite_get_codepoint_sprite(info->font_sprite_base, codepoint);

	if (!(info->flags & TEXT_DRAW_FLAG_NO_DRAW)) {
		RCT2_GLOBAL(0x009ABDA4, uint8*) = (uint8*)&info->palette;
		RCT2_GLOBAL(0x00EDF81C, uint32) = (IMAGE_TYPE_USE_PALETTE << 28);
		gfx_draw_sprite_palette_set(dpi, sprite, info->x, info->y, info->palette, NULL);
	}

	info->x += characterWidth;
}

static void ttf_draw_string_raw_sprite(rct_drawpixelinfo *dpi, const utf8 *text, text_draw_info *info)
{
	const utf8 *ch = text;
	int codepoint;

	while (!utf8_is_format_code(codepoint = utf8_get_next(ch, &ch))) {
		ttf_draw_character_sprite(dpi, codepoint, info);
	};
}

static void ttf_draw_string_raw_ttf(rct_drawpixelinfo *dpi, const utf8 *text, text_draw_info *info)
{
	if (!_ttfInitialised && !ttf_initialise())
		return;

	TTFFontDescriptor *fontDesc = ttf_get_font_from_sprite_base(info->font_sprite_base);
	if (info->flags & TEXT_DRAW_FLAG_NO_DRAW) {
		info->x += _ttf_getwidth_cache_get_or_add(fontDesc->font, text);
		return;
	} else {
		uint8 colour = info->palette[1];
		SDL_Surface *surface = _ttf_surface_cache_get_or_add(fontDesc->font, text);
		if (surface == NULL)
			return;

		if (SDL_MUSTLOCK(surface)) {
			if (SDL_LockSurface(surface) != 0) {
				return;
			}
		}
		
		int fontSize = font_get_size_from_sprite_base(info->font_sprite_base);
		int drawX = info->x + fontDesc->offset_x;
		int drawY = info->y + fontDesc->offset_y;
		int width = surface->w;
		int height = surface->h;

		int overflowX = (dpi->x + dpi->width) - (drawX + width);
		int overflowY = (dpi->y + dpi->height) - (drawY + height);
		if (overflowX < 0) width += overflowX;
		if (overflowY < 0) height += overflowY;
		int skipX = drawX - dpi->x;
		int skipY = drawY - dpi->y;
		info->x += width;

		uint8 *src = surface->pixels;
		uint8 *dst = dpi->bits;

		if (skipX < 0) {
			width += skipX;
			src += -skipX;
			skipX = 0;
		}
		if (skipY < 0) {
			height += skipY;
			src += (-skipY * surface->pitch);
			skipY = 0;
		}

		dst += skipX;
		dst += skipY * (dpi->width + dpi->pitch);

		int srcScanSkip = surface->pitch - width;
		int dstScanSkip = dpi->width + dpi->pitch - width;
		for (int yy = 0; yy < height; yy++) {
			for (int xx = 0; xx < width; xx++) {
				if (*src != 0) {
					*dst = colour;
					if (info->flags & TEXT_DRAW_FLAG_INSET) {
						*(dst + width + dstScanSkip + 1) = info->palette[3];
					} else if (info->flags & TEXT_DRAW_FLAG_OUTLINE) {
						*(dst + width + dstScanSkip + 1) = info->palette[3];
					}
				}
				src++;
				dst++;
			}
			src += srcScanSkip;
			dst += dstScanSkip;
		}

		if (SDL_MUSTLOCK(surface)) {
			SDL_UnlockSurface(surface);
		}
	}
}

static void ttf_draw_string_raw(rct_drawpixelinfo *dpi, const utf8 *text, text_draw_info *info)
{
	if (info->flags & TEXT_DRAW_FLAG_TTF) {
		ttf_draw_string_raw_ttf(dpi, text, info);
	} else {
		ttf_draw_string_raw_sprite(dpi, text, info);
	}
}

static const utf8 *ttf_process_format_code(rct_drawpixelinfo *dpi, const utf8 *text, text_draw_info *info)
{
	const utf8 *nextCh;
	int codepoint;

	codepoint = utf8_get_next(text, &nextCh);
	switch (codepoint) {
	case FORMAT_MOVE_X:
		info->x = info->startX + (uint8)(*nextCh++);
		break;
	case FORMAT_ADJUST_PALETTE:
	{
		uint16 eax = palette_to_g1_offset[*nextCh++];
		rct_g1_element *g1Element = &g1Elements[eax];
		uint32 ebx = g1Element->offset[249] + 256;
		if (!(info->flags & TEXT_DRAW_FLAG_OUTLINE)) {
			ebx = ebx & 0xFF;
		}
		info->palette[1] = ebx & 0xFF;
		info->palette[2] = (ebx >> 8) & 0xFF;

		// Adjust the text palette
		memcpy(info->palette + 3, &(g1Element->offset[247]), 2);
		memcpy(info->palette + 5, &(g1Element->offset[250]), 2);

		// Set the palette pointer
		RCT2_GLOBAL(0x009ABDA4, uint32) = (uint32)&info->palette;
		break;
	}
	case 3:
	case 4:
		nextCh++;
		break;
	case FORMAT_NEWLINE:
		info->x = info->startX;
		info->y += font_get_line_height(info->font_sprite_base);
		break;
	case FORMAT_NEWLINE_SMALLER:
		info->x = info->startX;
		info->y += font_get_line_height_small(info->font_sprite_base);
		break;
	case FORMAT_TINYFONT:
		info->font_sprite_base = 448;
		break;
	case FORMAT_SMALLFONT:
		info->font_sprite_base = 0;
		break;
	case FORMAT_MEDIUMFONT:
		info->font_sprite_base = 224;
		break;
	case FORMAT_BIGFONT:
		info->font_sprite_base = 672;
		break;
	case FORMAT_OUTLINE:
		info->flags |= TEXT_DRAW_FLAG_OUTLINE;
		break;
	case FORMAT_OUTLINE_OFF:
		info->flags &= ~TEXT_DRAW_FLAG_OUTLINE;
		break;
	case FORMAT_WINDOW_COLOUR_1:
	{
		uint16 flags = info->flags;
		colour_char_window(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WINDOW_COLOUR_1, uint8), &flags, info->palette);
		break;
	}
	case FORMAT_WINDOW_COLOUR_2:
	{
		uint16 flags = info->flags;
		colour_char_window(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WINDOW_COLOUR_2, uint8), &flags, info->palette);
		break;
	}
	case FORMAT_WINDOW_COLOUR_3:
	{
		uint16 flags = info->flags;
		colour_char_window(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WINDOW_COLOUR_3, uint8), &flags, info->palette);
		break;
	}
	case 0x10:
		break;
	case FORMAT_INLINE_SPRITE:
	{
		uint32 imageId = *((uint32*)(nextCh));
		rct_g1_element *g1Element = &g1Elements[imageId & 0x7FFFF];
		if (!(info->flags & TEXT_DRAW_FLAG_NO_DRAW)) {
			gfx_draw_sprite(dpi, imageId, info->x, info->y, 0);
		}
		info->x += g1Element->width;
		nextCh += 4;
		break;
	}
	default:
		if (codepoint >= FORMAT_COLOUR_CODE_START && codepoint <= FORMAT_COLOUR_CODE_END) {
			uint16 flags = info->flags;
			colour_char(codepoint - FORMAT_COLOUR_CODE_START, &flags, info->palette);
		} else if (codepoint <= 0x16) { //case 0x11? FORMAT_NEW_LINE_X_Y
			nextCh += 2;
		} else {
			nextCh += 4;//never happens?
		}
		break;
	}
	return nextCh;
}

static const utf8 *ttf_process_glyph_run(rct_drawpixelinfo *dpi, const utf8 *text, text_draw_info *info)
{
	utf8 buffer[512];
	const utf8 *ch = text;
	const utf8 *lastCh;
	int codepoint;

	bool isTTF = info->flags & TEXT_DRAW_FLAG_TTF;
	while (!utf8_is_format_code(codepoint = utf8_get_next(ch, &lastCh))) {
		if (isTTF && utf8_should_use_sprite_for_codepoint(codepoint)) {
			break;
		}
		ch = lastCh;
	}
	if (codepoint == 0) {
		ttf_draw_string_raw(dpi, text, info);
		return ch;
	} else {
		int length = ch - text;
		memcpy(buffer, text, length);
		buffer[length] = 0;
		ttf_draw_string_raw(dpi, buffer, info);
		return ch;
	}
}

static void ttf_process_string(rct_drawpixelinfo *dpi, const utf8 *text, text_draw_info *info)
{
	const utf8 *ch = text;
	const utf8 *nextCh;
	int codepoint;

	bool isTTF = info->flags & TEXT_DRAW_FLAG_TTF;

	while ((codepoint = utf8_get_next(ch, &nextCh)) != 0) {
		if (utf8_is_format_code(codepoint)) {
			ch = ttf_process_format_code(dpi, ch, info);
		} else if (isTTF && utf8_should_use_sprite_for_codepoint(codepoint)) {
			ttf_draw_character_sprite(dpi, codepoint, info);
			ch = nextCh;
		} else {
			ch = ttf_process_glyph_run(dpi, ch, info);
		}
	}
}

static void ttf_process_initial_colour(int colour, text_draw_info *info)
{
	if (colour != 254 && colour != 255) {
		info->flags &= ~(1 | 2 | 4 | 8);
		if ((sint16)info->font_sprite_base < 0) {
			info->flags |= 4;
			if ((sint16)info->font_sprite_base != -1) {
				info->flags |= 8;
			}
			info->font_sprite_base = 224;
		}
		if (colour & (1 << 5)) {
			info->flags |= TEXT_DRAW_FLAG_OUTLINE;
		}
		colour &= ~(1 << 5);
		if (!(colour & (1 << 6))) {
			if (!(info->flags & 1)) {
				uint16 flags = info->flags;
				colour_char_window(colour, &flags, (uint8*)&info->palette);
			}
		} else {
			info->flags |= 1;
			colour &= 0x1F;

			uint32 eax;
			if (info->flags & 4) {
				if (info->flags & 8) {
					eax = RCT2_ADDRESS(0x0141FC48, uint8)[colour * 8];
					eax = eax << 16;
					eax = eax | RCT2_ADDRESS(0x0141FC46, uint8)[colour * 8];
				} else {
					eax = RCT2_ADDRESS(0x0141FC49, uint8)[colour * 8];
					eax = eax << 16;
					eax = eax | RCT2_ADDRESS(0x0141FC47, uint8)[colour * 8];
				}
			} else {
				eax = RCT2_ADDRESS(0x0141FC4A, uint8)[colour * 8];
				eax = eax << 16;
				eax = eax | RCT2_ADDRESS(0x0141FC48, uint8)[colour * 8];
			}

			// Adjust text palette. Store current colour? ;
			info->palette[1] = eax & 0xFF;
			info->palette[2] = (eax >> 8) & 0xFF;
			info->palette[3] = (eax >> 16) & 0xFF;
			info->palette[4] = (eax >> 24) & 0xFF;
			RCT2_GLOBAL(0x009ABDA4, uint8*) = (uint8*)&info->palette;
			eax = 0;
		}
	}
}

static void ttf_draw_string(rct_drawpixelinfo *dpi, char *text, int colour, int x, int y)
{
	text_draw_info info;
	info.font_sprite_base = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16);
	info.flags = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_FLAGS, uint16);
	info.startX = x;
	info.startY = x;
	info.x = x;
	info.y = y;

	if (gUseTrueTypeFont) info.flags |= TEXT_DRAW_FLAG_TTF;

	memcpy(info.palette, text_palette, sizeof(info.palette));
	ttf_process_initial_colour(colour, &info);
	ttf_process_string(dpi, text, &info);
	memcpy(text_palette, info.palette, sizeof(info.palette));

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = info.font_sprite_base;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_FLAGS, uint16) = info.flags;

	gLastDrawStringX = info.x;
	gLastDrawStringY = info.y;
}

static int ttf_get_string_width(const utf8 *text)
{
	text_draw_info info;
	info.font_sprite_base = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16);
	info.flags = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_FLAGS, uint16);
	info.startX = 0;
	info.startY = 0;
	info.x = 0;
	info.y = 0;

	info.flags |= TEXT_DRAW_FLAG_NO_DRAW;
	if (gUseTrueTypeFont) info.flags |= TEXT_DRAW_FLAG_TTF;

	ttf_process_string(NULL, text, &info);

	return info.x;
}
