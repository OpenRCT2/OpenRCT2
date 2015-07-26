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
#include "drawing.h"

static int ttf_get_string_width(const utf8 *text);
static void ttf_draw_string(rct_drawpixelinfo *dpi, char *buffer, int colour, int x, int y);

static bool _ttfInitialised = false;
static TTF_Font *_ttfFont[4] = { NULL };
static int _ttfFontOffsetX = 0;
static int _ttfFontOffsetY = 0;

static const int TTFFontSizes[] = { 7, 9, 11, 13 };

enum {
	FONT_SPRITE_BASE_TINY = 448,
	FONT_SPRITE_BASE_SMALL = 0,
	FONT_SPRITE_BASE_MEDIUM = 224,
	FONT_SPRITE_BASE_BIG = 672
};

static uint8 *_spriteFontCharacterWidths = (uint8*)RCT2_ADDRESS_FONT_CHAR_WIDTH;

/**
 *
 *  rct2: 0x006C19AC
 */
void gfx_load_character_widths(){

	uint8* char_width_pointer = _spriteFontCharacterWidths;
	for (int char_set_offset = 0; char_set_offset < 4*0xE0; char_set_offset+=0xE0){
		for (uint8 c = 0; c < 0xE0; c++, char_width_pointer++){
			rct_g1_element g1 = g1Elements[c + SPR_CHAR_START + char_set_offset];
			int width;

			if (char_set_offset == 0xE0*3) width = g1.width + 1;
			else width = g1.width - 1;

			if (c >= (FORMAT_ARGUMENT_CODE_START - 0x20) && c < (FORMAT_COLOUR_CODE_END - 0x20)){
				width = 0;
			}
			*char_width_pointer = (uint8)width;
		}
		
	}
	
	uint8 drawing_surface[0x40];
	rct_drawpixelinfo dpi = { 
		.bits = (char*)&drawing_surface, 
		.width = 8, 
		.height = 8, 
		.x = 0, 
		.y = 0, 
		.pitch = 0, 
		.zoom_level = 0};

	
	for (int i = 0; i < 0xE0; ++i){
		memset(drawing_surface, 0, sizeof(drawing_surface));
		gfx_draw_sprite(&dpi, i + 0x10D5, -1, 0, 0);

		for (int x = 0; x < 8; ++x){
			uint8 val = 0;
			for (int y = 0; y < 8; ++y){
				val >>= 1;
				if (dpi.bits[x + y * 8]==1){
					val |= 0x80;
				}
			}
			RCT2_ADDRESS(RCT2_ADDRESS_CHARACTER_BITMAP, uint8)[i * 8 + x] = val;
		}

	}

	for (int i = 0; i < 0x20; ++i){
		rct_g1_element* g1 = &g1Elements[0x606 + i];
		uint8* unknown_pointer = RCT2_ADDRESS(0x9C3852, uint8) + 0xa12 * i;
		g1->offset = unknown_pointer;
		g1->width = 0x40;
		g1->height = 0x28;
		*((uint16*)unknown_pointer) = 0xFFFF;
		*((uint32*)(unknown_pointer + 0x0E)) = 0;
	}
}


/* rct2: 0x006C23B1 */
int gfx_get_string_width_new_lined(char* buffer){
	// Current font sprites
	uint16* current_font_sprite_base;
	// Width of string
	int width = 0, max_width = 0, no_lines = 1;
	rct_g1_element g1_element;

	current_font_sprite_base = RCT2_ADDRESS(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16);

	for (uint8* curr_char = (uint8*)buffer; *curr_char != (uint8)0; curr_char++) {

		if (*curr_char >= 0x20) {
			width += _spriteFontCharacterWidths[*current_font_sprite_base + (*curr_char - 0x20)];
			continue;
		}
		switch (*curr_char) {
		case FORMAT_MOVE_X:
			curr_char++;
			width = *curr_char;
			break;
		case FORMAT_ADJUST_PALETTE:
		case 3:
		case 4:
			curr_char++;
			break;
		case FORMAT_NEWLINE:
		case FORMAT_NEWLINE_SMALLER:
			no_lines++;
			max_width = max(max_width, width);
			width = 0;
			break;
		case FORMAT_TINYFONT:
			*current_font_sprite_base = 0x1C0;
			break;
		case FORMAT_BIGFONT:
			*current_font_sprite_base = 0x2A0;
			break;
		case FORMAT_MEDIUMFONT:
			*current_font_sprite_base = 0x0E0;
			break;
		case FORMAT_SMALLFONT:
			*current_font_sprite_base = 0;
			break;
		case FORMAT_OUTLINE:
		case FORMAT_OUTLINE_OFF:
		case FORMAT_WINDOW_COLOUR_1:
		case FORMAT_WINDOW_COLOUR_2:
		case FORMAT_WINDOW_COLOUR_3:
		case 0x10:
			continue;
		case FORMAT_INLINE_SPRITE:
			g1_element = g1Elements[*((uint32*)(curr_char + 1)) & 0x7FFFF];
			width += g1_element.width;
			curr_char += 4;
			break;
		default:
			if (*curr_char <= 0x16) { //case 0x11? FORMAT_NEW_LINE_X_Y
				curr_char += 2;
				continue;
			}
			curr_char += 4;//never happens?
			break;
		}
	}

	if (width > max_width)
		return width;
	return max_width;
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
	rct_g1_element g1_element;
	uint16 fontSpriteBase;
	int maxWidth;

	if (width < 6) {
		*text = 0;
		return 0;
	}

	fontSpriteBase = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16);
	int dotCharacterWidth = _spriteFontCharacterWidths[fontSpriteBase + ('.' - ' ')];
	int elipsisCharacterWidth = dotCharacterWidth * 3;
	
	maxWidth = width - elipsisCharacterWidth;

	int clippedWidth = 0;
	utf8 *lastCh = text;
	utf8 *ch = text;
	int codepoint;
	while ((codepoint = utf8_get_next(ch, &ch)) != 0) {
		if (utf8_is_format_code(codepoint)) {
			switch (codepoint) {
			case FORMAT_MOVE_X:
				clippedWidth = *ch++;
				break;
			case FORMAT_ADJUST_PALETTE:
			case 3:
			case 4:
				ch++;
				break;
			case FORMAT_NEWLINE:
			case FORMAT_NEWLINE_SMALLER:
				break;
			case FORMAT_TINYFONT:
				fontSpriteBase = 448;
				break;
			case FORMAT_SMALLFONT:
				fontSpriteBase = 0;
				break;
			case FORMAT_MEDIUMFONT:
				fontSpriteBase = 224;
				break;
			case FORMAT_BIGFONT:
				fontSpriteBase = 672;
				break;
			case FORMAT_OUTLINE:
			case FORMAT_OUTLINE_OFF:
			case FORMAT_WINDOW_COLOUR_1:
			case FORMAT_WINDOW_COLOUR_2:
			case FORMAT_WINDOW_COLOUR_3:
			case 0x10:
				break;
			case FORMAT_INLINE_SPRITE:
				g1_element = g1Elements[*((uint32*)(ch - 1)) & 0x7FFFF];
				clippedWidth += g1_element.width;
				ch += 4;
				break;
			default:
				if (codepoint >= FORMAT_COLOUR_CODE_START || codepoint <= FORMAT_COLOUR_CODE_END) {
					break;
				}
				if (codepoint <= 22) { //case 0x11? FORMAT_NEW_LINE_X_Y
					ch += 2;
				} else {
					ch += 4;//never happens?
				}
				break;
			}

			dotCharacterWidth = _spriteFontCharacterWidths[fontSpriteBase + ('.' - ' ')];
			elipsisCharacterWidth = dotCharacterWidth * 3;
			maxWidth = width - elipsisCharacterWidth;
		} else {
			clippedWidth += _spriteFontCharacterWidths[fontSpriteBase + utf8_get_sprite_offset_for_codepoint(codepoint)];
			if (clippedWidth > width) {
				strcpy(lastCh - 3, "...");
				clippedWidth = width;
				return clippedWidth;
			}
			if (clippedWidth <= maxWidth) {
				lastCh = ch + 1;
			}
		}
	}
	return clippedWidth;
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
int gfx_wrap_string(char* buffer, int width, int* num_lines, int* font_height)
{
	unsigned int line_width = 0;
	unsigned int max_width = 0;
	rct_g1_element g1_element;

    *num_lines = 0;
	*font_height = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16);

	// Pointer to the start of the current word
	unsigned char* curr_word = NULL;
	// Width of line up to current word
	unsigned int curr_width;

	for (unsigned char* curr_char = buffer; *curr_char != (uint8)0; curr_char++) {

		// Remember start of current word and line width up to this word
        if (*curr_char == ' ') {
            curr_word = curr_char;
            curr_width = line_width;
        }

		// 5 is RCT2 new line?
        if (*curr_char != 5) {
			if (*curr_char < ' ') {
				switch(*curr_char) {
				case FORMAT_MOVE_X:
				case FORMAT_ADJUST_PALETTE:
				case 3:
				case 4:
					curr_char++;
					continue;
				case FORMAT_NEWLINE:
				case FORMAT_NEWLINE_SMALLER:
					continue;
				case FORMAT_TINYFONT:
					*font_height = 0x1C0;
					continue;
				case FORMAT_BIGFONT:
					*font_height = 0x2A0;
					continue;
				case FORMAT_MEDIUMFONT:
					*font_height = 0xE0;
					continue;
				case FORMAT_SMALLFONT:
					*font_height = 0;
					continue;
				case FORMAT_OUTLINE:
				case FORMAT_OUTLINE_OFF:
				case FORMAT_WINDOW_COLOUR_1:
				case FORMAT_WINDOW_COLOUR_2:
				case FORMAT_WINDOW_COLOUR_3:
				case 0x10:
					continue;
				case FORMAT_INLINE_SPRITE:
					g1_element = g1Elements[*((uint32*)(curr_char + 1)) & 0x7FFFF];
					line_width += g1_element.width;
					curr_char += 4;
					break;
				default:
					if (*curr_char <= 0x16) {
						curr_char += 2;
						continue;
					}
					curr_char += 4;
					continue;
				}
			}

			line_width += _spriteFontCharacterWidths[*font_height + (*curr_char - 0x20)];

			if ((int)line_width <= width) {
				continue;
			}
			if (curr_word == 0) {
				curr_char--;
				unsigned char* old_char = curr_char;
				unsigned char swap_char = 0;
				unsigned char temp;
				// Insert NULL at current character
				// Aboslutely no guarantee that this won't overrun!
				do {
					temp = swap_char;
					swap_char = *curr_char;
					*curr_char = temp;
					curr_char++;
				} while(swap_char != 0);

				*curr_char = swap_char;
				curr_char = old_char;
				curr_char++;
				*num_lines += 1;

				if (line_width > max_width) {
					max_width = line_width;
				}
				line_width = 0;
				curr_word = 0;
				continue;
			}
			curr_char = curr_word;
			line_width = curr_width;
        }

        *num_lines += 1;
        *curr_char = 0;

		if (line_width > max_width) {
			max_width = line_width;
		}
		line_width = 0;
		curr_word = 0;
	}
	if (max_width == 0)return line_width;
	return max_width;
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

	line_height = 0x0A;

	if (font_height > 0xE0) {
		line_height = 6;
		if (font_height != 0x1C0) {
			line_height = 0x12;
		}
	}

	if (*buffer == 0x0B) {
		line_height = line_height + 1;
	}

	font_height = (line_height / 2) * num_lines;
	line_y = y - font_height;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_FLAGS, uint16) = 0;

	for (int line = 0; line <= num_lines; ++line) {
		int half_width = gfx_get_string_width(buffer) / 2;
		gfx_draw_string(dpi, buffer, 0xFE, x - half_width, line_y);

		buffer += get_string_length(buffer) + 1;
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
	int font_height, line_height, line_width, line_y, num_lines;

	// Location of font sprites
	uint16* current_font_sprite_base = RCT2_ADDRESS(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16);
	*current_font_sprite_base = 0xE0;

	char* buffer = RCT2_ADDRESS(0x009C383D, char);

	gfx_draw_string(dpi, buffer, colour, dpi->x, dpi->y);

	buffer = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char);

	format_string(buffer, format, args);

	*current_font_sprite_base = 0xE0;

	// Line width unused here
	line_width = gfx_wrap_string(buffer, width, &num_lines, &font_height);

	line_height = 0x0A;

	if (font_height > 0xE0) {
		line_height = 6;
		if (font_height != 0x1C0) {
			line_height = 0x12;
		}
	}

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_FLAGS, uint16) = 0;

	line_y = y;

	for (int line = 0; line <= num_lines; ++line) {
		gfx_draw_string(dpi, buffer, 0xFE, x, line_y);
		buffer += get_string_length(buffer) + 1;
        line_y += line_height;
	} 

	return line_y - y;
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
void sub_6C1F57(rct_drawpixelinfo *dpi, int x, int y, int width, int colour, rct_string_id format, void *args, int ticks)
{
	RCT2_CALLPROC_X(
		0x006C1F57,
		colour,
		format,
		x,
		y,
		(int)args,
		(int)dpi,
		(width & 0xFFFF) | (ticks << 16)
	);
}

bool ttf_initialise()
{
	if (!_ttfInitialised) {
		if (TTF_Init() != 0)
			return false;

		utf8 fontPath[MAX_PATH] = "C:\\Windows\\Fonts\\";
		strcat(fontPath, gTrueTypeFontPath);
		for (int i = 0; i < 4; i++) {
			_ttfFont[i] = TTF_OpenFont(fontPath, TTFFontSizes[i]);
			if (_ttfFont[i] == NULL) {
				TTF_Quit();
				return false;
			}
		}

		_ttfFontOffsetX = 0;
		_ttfFontOffsetY = -3;
		_ttfInitialised = true;
	}
	return true;
}

void ttf_dispose()
{
	if (!_ttfInitialised)
		return;

	if (_ttfFont != NULL) {
		for (int i = 0; i < 4; i++) {
			TTF_CloseFont(_ttfFont[i]);
			_ttfFont[i] = NULL;
		}
	}

	TTF_Quit();
	_ttfInitialised = false;
}

enum {
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

static void ttf_draw_string_raw_sprite(rct_drawpixelinfo *dpi, const utf8 *text, text_draw_info *info)
{
	const utf8 *ch = text;
	int codepoint;

	while (!utf8_is_format_code(codepoint = utf8_get_next(ch, &ch))) {
		uint32 charOffset = info->font_sprite_base + utf8_get_sprite_offset_for_codepoint(codepoint);
		int charWidth = _spriteFontCharacterWidths[charOffset] & 0xFF;

		if (!(info->flags & TEXT_DRAW_FLAG_NO_DRAW)) {
			RCT2_GLOBAL(0x009ABDA4, uint8*) = (uint8*)&info->palette;
			RCT2_GLOBAL(0x00EDF81C, uint32) = (IMAGE_TYPE_USE_PALETTE << 28);
			gfx_draw_sprite_palette_set(dpi, SPR_CHAR_START + ((IMAGE_TYPE_USE_PALETTE << 28) | charOffset), info->x, info->y, info->palette, NULL);
		}

		info->x += charWidth;
	};
}

static void ttf_draw_string_raw_ttf(rct_drawpixelinfo *dpi, const utf8 *text, text_draw_info *info)
{
	if (!_ttfInitialised && !ttf_initialise())
		return;

	TTF_Font *font;
	switch (info->font_sprite_base) {
	case FONT_SPRITE_BASE_TINY:
		font = _ttfFont[0];
		break;
	case FONT_SPRITE_BASE_SMALL:
		font = _ttfFont[1];
		break;
	default:
	case FONT_SPRITE_BASE_MEDIUM:
		font = _ttfFont[2];
		break;
	case FONT_SPRITE_BASE_BIG:
		font = _ttfFont[3];
		break;
	}

	int fontStyle = TTF_GetFontStyle(font);
	int newFontStyle = 0;
	if (info->flags & TEXT_DRAW_FLAG_OUTLINE) {
		newFontStyle |= TTF_STYLE_BOLD;
	}
	if (fontStyle != newFontStyle) {
		TTF_SetFontStyle(font, newFontStyle);
	}

	if (info->flags & TEXT_DRAW_FLAG_NO_DRAW) {
		int width, height;

		TTF_SizeUTF8(font, text, &width, &height);
		info->x += width;
		return;
	} else {
		uint8 colour = info->palette[1];
		SDL_Color c = { 0, 0, 0, 255 };
		SDL_Surface *surface = TTF_RenderUTF8_Solid(font, text, c);
		if (surface == NULL)
			return;

		if (SDL_LockSurface(surface) != 0) {
			SDL_FreeSurface(surface);
			return;
		}

		int drawX = info->x + _ttfFontOffsetX;
		int drawY = info->y + _ttfFontOffsetY;
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
				if (*src != 0) *dst = colour;
				src++;
				dst++;
			}
			src += srcScanSkip;
			dst += dstScanSkip;
		}
		SDL_UnlockSurface(surface);

		SDL_FreeSurface(surface);
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
		info->x = info->startX + *nextCh++;
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
		if (info->font_sprite_base <= 224) { info->y += 28; }
		else if (info->font_sprite_base <= 448) { info->y += 24; }
		else { info->y += 18; }
		break;
	case FORMAT_NEWLINE_SMALLER:
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

	while (!utf8_is_format_code(codepoint = utf8_get_next(ch, &lastCh))) {
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
			info->flags &= ~TEXT_DRAW_FLAG_TTF;
			ch = ttf_process_glyph_run(dpi, ch, info);
			info->flags |= TEXT_DRAW_FLAG_TTF;
		} else {
			ch = ttf_process_glyph_run(dpi, ch, info);
		}
	}
}

static void ttf_process_initial_colour(int colour, text_draw_info *info)
{
	if (colour != 254 && colour != 255) {
		info->flags &= ~(1 | 2 | 4 | 8);
		if (info->font_sprite_base < 0) {
			info->flags |= 4;
			if (info->font_sprite_base != -1) {
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
