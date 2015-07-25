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
static TTF_Font *_ttfFont = NULL;

/**
 *
 *  rct2: 0x006C19AC
 */
void gfx_load_character_widths(){

	uint8* char_width_pointer = RCT2_ADDRESS(RCT2_ADDRESS_FONT_CHAR_WIDTH, uint8);
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
			width += RCT2_ADDRESS(RCT2_ADDRESS_FONT_CHAR_WIDTH, uint8)[*current_font_sprite_base + (*curr_char - 0x20)];
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
	if (_ttfInitialised) {
		return ttf_get_string_width(buffer);
	}

	// Current font sprites
	uint16* current_font_sprite_base;
	// Width of string
	int width;
	rct_g1_element g1_element;
	
	current_font_sprite_base = RCT2_ADDRESS(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16);
	width = 0;

	for (uint8* curr_char = (uint8*)buffer; *curr_char != (uint8)0; curr_char++) {

		if (*curr_char >= 0x20) {
			width += RCT2_ADDRESS(RCT2_ADDRESS_FONT_CHAR_WIDTH, uint8)[*current_font_sprite_base + (*curr_char - 0x20)];
			continue;
		}
		switch(*curr_char) {
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
			continue;
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
	return width;
}

/**
 *  Clip the text in buffer to width, add ellipsis and return the new width of the clipped string
 *
 *  rct2: 0x006C2460
 * buffer (esi)
 * width (edi)
 */
int gfx_clip_string(char* buffer, int width)
{
	// Location of font sprites
	uint16 current_font_sprite_base;
	// Width the string has to fit into
	unsigned int max_width;
	// Character to change to ellipsis
	unsigned char* last_char;
	// Width of the string, including ellipsis

	unsigned int clipped_width;

	rct_g1_element g1_element;

	if (width < 6) {
		*buffer = 0;
		return 0;
	}
	
	current_font_sprite_base = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16);
	max_width = width - (3 * RCT2_ADDRESS(0x141E9F6, uint8)[current_font_sprite_base]);

	clipped_width = 0;
	last_char = buffer;

	for (unsigned char* curr_char = buffer; *curr_char != (uint8)0; curr_char++) {
		if (*curr_char < 0x20) {
			switch (*curr_char) {
			case FORMAT_MOVE_X:
				curr_char++;
				clipped_width = *curr_char;
				continue;
			case FORMAT_ADJUST_PALETTE:
			case 3:
			case 4:
				curr_char++;
				continue;
			case FORMAT_NEWLINE:
			case FORMAT_NEWLINE_SMALLER:
				continue;
			case FORMAT_TINYFONT:
				current_font_sprite_base = 0x1C0;
				break;
			case FORMAT_BIGFONT:
				current_font_sprite_base = 0x2A0;
				break;
			case FORMAT_MEDIUMFONT:
				current_font_sprite_base = 0x0E0;
				break;
			case FORMAT_SMALLFONT:
				current_font_sprite_base = 0;
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
				clipped_width += g1_element.width;
				curr_char += 4;
				continue;
			default:
				if (*curr_char <= 0x16) { //case 0x11? FORMAT_NEW_LINE_X_Y
					curr_char += 2;
					continue;
				}
				curr_char += 4;//never happens?
				continue;
			}
			max_width = width - (3 * RCT2_ADDRESS(0x141E9F6, uint8)[current_font_sprite_base]);
		}

		clipped_width += RCT2_ADDRESS(RCT2_ADDRESS_FONT_CHAR_WIDTH, uint8)[current_font_sprite_base + (*curr_char - 0x20)];

		if ((int)clipped_width > width) {
// 			*((uint32*)last_char) = '...';
			strcpy(last_char-3, "...");
			clipped_width = width;
			return clipped_width;
		}
		if (clipped_width <= max_width) {
			last_char = curr_char+1;
		}
	}
	return clipped_width;
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

			line_width += RCT2_ADDRESS(RCT2_ADDRESS_FONT_CHAR_WIDTH, uint8)[*font_height + (*curr_char - 0x20)];

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
	if (_ttfInitialised) {
		ttf_draw_string(dpi, buffer, colour, x, y);
		return;
	}

	int eax, ebx, ebp;
	rct_g1_element* g1_element;

	// Maximum length/height of string
	int max_x = x;
	int max_y = y;

	// 
	uint16 *current_font_flags = RCT2_ADDRESS(RCT2_ADDRESS_CURRENT_FONT_FLAGS, uint16);
	sint16 *current_font_sprite_base = RCT2_ADDRESS(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, sint16);

	uint8* palette_pointer = text_palette;

	// Flag for skipping non-printing characters
	int skip_char = 0;

	if (colour != 0xFE) {

		if (x >= dpi->x + dpi->width)
			return;
		
		if (x + 0x280 <= dpi->x)
			return;

		if (y >= dpi->y + dpi->height)
			return;

		if (y + 0x5A <= dpi->y) {
			return;
		}

		if (colour != 0xFF) {

			// switch_colour:
			*current_font_flags = 0;
			if (*current_font_sprite_base < 0) {
				*current_font_flags |= 4;
				if (*current_font_sprite_base != -1) {
					*current_font_flags |= 8;
				}
				*current_font_sprite_base = 224;
			}
			if (colour & (1 << 5)) {
				*current_font_flags |= 2;
			}
			colour &= ~(1 << 5);

			if (!(colour & 0x40)) {
				ebp = colour;
				if (*current_font_flags & 1) {
					if ((y + 0x13 <= dpi->y) || (dpi->y + dpi->height <= y)) {
						skip_char = 1;
					} else {
						skip_char = 0;
					}
				} else {
					colour_char_window(ebp, current_font_flags, palette_pointer);
				}
			} else {
				*current_font_flags |= 1;
				colour &= 0x1F;

				if (*current_font_flags & 4) {
					if (*current_font_flags & 8) {
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
				palette_pointer[1] = eax & 0xFF;
				palette_pointer[2] = (eax >> 8) & 0xFF;
				palette_pointer[3] = (eax >> 16) & 0xFF;
				palette_pointer[4] = (eax >> 24) & 0xFF;
				RCT2_GLOBAL(0x009ABDA4, uint32) = (uint32)palette_pointer;
				eax = 0;
			}
		}
	}

	if ((y + 0x13 <= dpi->y) || (dpi->y + dpi->height <= y)) {
		skip_char = 1;
	}

	for (uint8 al = *buffer; al > 0; ++buffer, al = *buffer) {

		// Skip to the next printing character
		if (skip_char) {
			if (al < 0x20) {
				// Control codes
				skip_char = 0;
			} else if (al >= FORMAT_COLOUR_CODE_START && al <= FORMAT_COLOUR_CODE_END) {
				// Colour codes
				if (*current_font_flags == 1) {
					if ((y + 0x13 <= dpi->y) || (dpi->y + dpi->height <= y)) {
						skip_char = 1;
					} else {
						skip_char = 0;
					}
					continue;
				}
				colour_char(al - FORMAT_COLOUR_CODE_START, current_font_flags, palette_pointer);
				continue;
			} else {
				continue;
			}
		}

		// Control codes
		switch (al) {
		case FORMAT_MOVE_X://Start New Line at start+buffer x, same y. (Overwrite?)
			max_x = x + (uint8)*++buffer;
			break;
		case FORMAT_ADJUST_PALETTE:
			al = *++buffer;
			if (*current_font_flags & 1) {
				if ((y + 0x13 <= dpi->y) || (dpi->y + dpi->height <= y)) {
					skip_char = 1;
					break;
				}
			}

			eax = palette_to_g1_offset[al]; //RCT2_ADDRESS(0x097FCBC, uint32)[al * 4];
			g1_element = &g1Elements[eax];
			ebx = g1_element->offset[0xF9] + (1 << 8);
			if (!(*current_font_flags & 2)) {
				ebx = ebx & 0xFF;
			}

			palette_pointer[1] = ebx & 0xff;
			palette_pointer[2] = (ebx >> 8) & 0xff;
			//Adjust the text palette
			memcpy(palette_pointer + 3, &(g1_element->offset[0xF7]), 2);
			memcpy(palette_pointer + 5, &(g1_element->offset[0xFA]), 2);
			//Set the palette pointer
			RCT2_GLOBAL(0x009ABDA4, uint32) = (uint32)palette_pointer;


			if ((y + 0x13 <= dpi->y) || (dpi->y + dpi->height <= y)) {
				skip_char = 1;
			}
			break;
		case FORMAT_NEWLINE://Start New Line at set y lower
			max_x = x;
			if (*current_font_sprite_base <= 224) {
				max_y += 10;
				break;
			}
			else if (*current_font_sprite_base == 448) {
				max_y += 6;
				break;
			}
			max_y += 18;
			break;
		case FORMAT_NEWLINE_SMALLER://Start New Line at set y lower
			max_x = x;
			if (*current_font_sprite_base <= 224) {
				max_y += 5;
				break;
			}
			else if (*current_font_sprite_base == 448) {
				max_y += 3;
				break;
			}
			max_y += 9;
			break;
		case FORMAT_TINYFONT:
			*current_font_sprite_base = 448;
			break;
		case FORMAT_BIGFONT:
			*current_font_sprite_base = 672;
			break;
		case FORMAT_MEDIUMFONT:
			*current_font_sprite_base = 224;
			break;
		case FORMAT_SMALLFONT:
			*current_font_sprite_base = 0;
			break;
		case FORMAT_OUTLINE:
			*current_font_flags |= 2;
			break;
		case FORMAT_OUTLINE_OFF:
			*current_font_flags &= 0x0FFFD;
			break;
		case FORMAT_WINDOW_COLOUR_1:
			ebp = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WINDOW_COLOUR_1, uint8);
			if (*current_font_flags & 1) {
				if ((y + 0x13 <= dpi->y) || (dpi->y + dpi->height <= y)) {
					skip_char = 1;
				}
				else {
					skip_char = 0;
				}
				break;
			}
			colour_char_window(ebp, current_font_flags, palette_pointer);
			break;
		case FORMAT_WINDOW_COLOUR_2:
			ebp = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WINDOW_COLOUR_2, uint8);
			if (*current_font_flags & 1) {
				if ((y + 0x13 <= dpi->y) || (dpi->y + dpi->height <= y)) {
					skip_char = 1;
				}
				else {
					skip_char = 0;
				}
				break;
			}
			colour_char_window(ebp, current_font_flags, palette_pointer);
			break;
		case FORMAT_WINDOW_COLOUR_3:
			ebp = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WINDOW_COLOUR_3, uint8);
			if (*current_font_flags & 1) {
				if ((y + 0x13 <= dpi->y) || (dpi->y + dpi->height <= y)) {
					skip_char = 1;
				}
				else {
					skip_char = 0;
				}
				break;
			}
			colour_char_window(ebp, current_font_flags, palette_pointer);
			break;
		case FORMAT_NEWLINE_X_Y: //Start new line at specified x,y
			max_x = x + *++buffer;
			max_y = y + *++buffer;
			break;
		case FORMAT_INLINE_SPRITE:
			buffer += 4;
			if (max_x >= dpi->x + dpi->width) {
				skip_char = 1;
				break;
			}
			uint32 image_id = *((uint32*)(buffer - 3));
			uint32 image_offset = image_id & 0x7FFFF;
			g1_element = &g1Elements[image_offset];

			gfx_draw_sprite(dpi, image_id, max_x, max_y, 0);

			max_x = max_x + g1_element->width;
			break;
		default:
			// Colour codes
			if ((al >= FORMAT_COLOUR_CODE_START) && (al <= FORMAT_COLOUR_CODE_END)){
				
				if (*current_font_flags == 1) {
					if ((y + 0x13 <= dpi->y) || (dpi->y + dpi->height <= y)) {
						skip_char = 1;
					} else {
						skip_char = 0;
					}
					continue;
				}
				colour_char(al - FORMAT_COLOUR_CODE_START, current_font_flags, palette_pointer);
				continue;
			}
				
			// Normal Characters
			if (max_x >= dpi->x + dpi->width) {
				skip_char = 1;
			}
			if (max_x + 0x1A < dpi->x) {
				ebx = al-0x20;
				ebx += *current_font_sprite_base;
				max_x = max_x + (RCT2_ADDRESS(RCT2_ADDRESS_FONT_CHAR_WIDTH, uint8)[ebx] & 0xFF);
				continue;
			}

			uint32 char_offset = al - 0x20 + *current_font_sprite_base;
			RCT2_GLOBAL(0x00EDF81C, uint32) = (IMAGE_TYPE_USE_PALETTE << 28);

			gfx_draw_sprite_palette_set(dpi, ((IMAGE_TYPE_USE_PALETTE << 28) | char_offset) + SPR_CHAR_START, max_x, max_y, palette_pointer, NULL);
			max_x += (RCT2_ADDRESS(RCT2_ADDRESS_FONT_CHAR_WIDTH, uint8)[char_offset] & 0xFF);
			continue;
		} 
	}

	gLastDrawStringX = max_x;
	gLastDrawStringY = max_y;
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

		_ttfFont = TTF_OpenFont("C:\\Windows\\Fonts\\tahoma.ttf", 11);
		if (_ttfFont == NULL) {
			TTF_Quit();
			return false;
		}

		_ttfInitialised = true;
	}
	return true;
}

void ttf_dispose()
{
	if (!_ttfInitialised)
		return;

	if (_ttfFont != NULL)
		TTF_CloseFont(_ttfFont);

	TTF_Quit();
}

enum {
	TEXT_DRAW_FLAG_NO_DRAW = 1 << 31
};

typedef struct {
	int x;
	int y;
	int flags;
	uint8 colour;
} text_draw_info;

static void ttf_draw_string_raw(rct_drawpixelinfo *dpi, const utf8 *text, text_draw_info *info)
{
	if (!_ttfInitialised && !ttf_initialise())
		return;

	if (info->flags & TEXT_DRAW_FLAG_NO_DRAW) {
		int width, height;

		TTF_SizeUTF8(_ttfFont, text, &width, &height);
		info->x += width;
		return;
	} else {
		uint8 colour = info->colour;
		SDL_Color c = { 0, 0, 0, 255 };
		SDL_Surface *surface = TTF_RenderUTF8_Solid(_ttfFont, text, c);
		if (surface == NULL)
			return;

		if (SDL_LockSurface(surface) != 0) {
			SDL_FreeSurface(surface);
			return;
		}

		int width = surface->w;
		int height = surface->h;
		int skipX = info->x - dpi->x;
		int skipY = info->y - dpi->y;
		info->x += width;

		uint8 *src = surface->pixels;
		uint8 *dst = dpi->bits;

		if (skipX < 0) {
			width += skipX;
			src += -skipX;
		}
		if (skipY < 0) {
			height += skipY;
			src += (-skipY * surface->pitch);
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

static bool utf8_is_format_code(int codepoint)
{
	if (codepoint < 32) return true;
	if (codepoint >= 123 && codepoint <= 155) return true;
	return false;
}

static const utf8 *ttf_process_format_code(rct_drawpixelinfo *dpi, const utf8 *text, text_draw_info *info)
{
	const utf8 *nextCh;
	int codepoint;

	codepoint = utf8_get_next(text, &nextCh);
	switch (codepoint) {
	case FORMAT_MOVE_X:
		info->x += *nextCh++;
		break;
	case FORMAT_ADJUST_PALETTE:
	case 3:
	case 4:
		nextCh++;
		break;
	case FORMAT_NEWLINE:
	case FORMAT_NEWLINE_SMALLER:
		break;
	case FORMAT_TINYFONT:
		break;
	case FORMAT_BIGFONT:
		break;
	case FORMAT_MEDIUMFONT:
		break;
	case FORMAT_SMALLFONT:
		break;
	case FORMAT_OUTLINE:
	case FORMAT_OUTLINE_OFF:
		break;
	case FORMAT_WINDOW_COLOUR_1:
	{
		uint8 palette[5];
		uint16 flags = info->flags;
		colour_char_window(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WINDOW_COLOUR_1, uint8) - FORMAT_COLOUR_CODE_START, &flags, palette);
		info->colour = palette[1];
		break;
	}
	case FORMAT_WINDOW_COLOUR_2:
	{
		uint8 palette[5];
		uint16 flags = info->flags;
		colour_char_window(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WINDOW_COLOUR_2, uint8) - FORMAT_COLOUR_CODE_START, &flags, palette);
		info->colour = palette[1];
		break;
	}
	case FORMAT_WINDOW_COLOUR_3:
	{
		uint8 palette[5];
		uint16 flags = info->flags;
		colour_char_window(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WINDOW_COLOUR_3, uint8) - FORMAT_COLOUR_CODE_START, &flags, palette);
		info->colour = palette[1];
		break;
	}
	case 0x10:
		break;
	case FORMAT_INLINE_SPRITE:
		// g1Element = g1Elements[*((uint32*)(nextCh)) & 0x7FFFF];
		// width += g1Element.width;
		nextCh += 4;
		break;
	default:
		if (codepoint >= FORMAT_COLOUR_CODE_START && codepoint <= FORMAT_COLOUR_CODE_END) {
			if (info->flags == 1) {
				
			}
			
			uint8 palette[5];
			uint16 flags = info->flags;
			colour_char(codepoint - FORMAT_COLOUR_CODE_START, &flags, palette);
			info->colour = palette[1];
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
	int codepoint;

	do {
		codepoint = utf8_get_next(ch, &ch);
	} while (!utf8_is_format_code(codepoint));
	if (codepoint == 0) {
		ttf_draw_string_raw(dpi, text, info);
		return ch - 1;
	} else {
		int length = ch - text - 1;
		memcpy(buffer, text, length);
		buffer[length] = 0;
		ttf_draw_string_raw(dpi, buffer, info);
		return ch;
	}
}

static void ttf_draw_string(rct_drawpixelinfo *dpi, char *text, int colour, int x, int y)
{
	text_draw_info info;
	info.colour = colour;
	info.flags = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_FLAGS, uint16);
	info.x = x;
	info.y = y;

	const utf8 *ch = text;
	const utf8 *nextCh;
	int codepoint;
	while ((codepoint = utf8_get_next(ch, &nextCh)) != 0) {
		if (utf8_is_format_code(codepoint)) {
			ch = ttf_process_format_code(dpi, ch, &info);
		} else {
			ch = ttf_process_glyph_run(dpi, ch, &info);
		}
	}

	gLastDrawStringX = info.x;
	gLastDrawStringY = info.y;
}

static int ttf_get_string_width(const utf8 *text)
{
	text_draw_info info;
	info.colour = 0;
	info.flags = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_FLAGS, uint16);
	info.x = 0;
	info.y = 0;

	info.flags |= TEXT_DRAW_FLAG_NO_DRAW;

	const utf8 *ch = text;
	const utf8 *nextCh;
	int codepoint;
	while ((codepoint = utf8_get_next(ch, &nextCh)) != 0) {
		if (utf8_is_format_code(codepoint)) {
			ch = ttf_process_format_code(NULL, ch, &info);
		} else {
			ch = ttf_process_glyph_run(NULL, ch, &info);
		}
	}

	return info.x;
}
