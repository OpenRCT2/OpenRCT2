/*****************************************************************************
 * Copyright (c) 2014 Ted John
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

#ifndef _FORMAT_CODES_H_
#define _FORMAT_CODES_H_

uint32 format_get_code(const char *token);
const char *format_get_token(uint32 code);

enum {
	// Font format codes

	// The next byte specifies the X coordinate
	FORMAT_MOVE_X = 1,
	// The next byte specifies the palette
	FORMAT_ADJUST_PALETTE,

	// Moves to the next line
	FORMAT_NEWLINE = 5,
	// Moves less than NEWLINE
	FORMAT_NEWLINE_SMALLER,

	FORMAT_TINYFONT,
	FORMAT_BIGFONT,
	FORMAT_MEDIUMFONT,
	FORMAT_SMALLFONT,

	FORMAT_OUTLINE,
	FORMAT_OUTLINE_OFF,

	// Changes the colour of the text to a predefined window colour.
	FORMAT_WINDOW_COLOUR_1,
	FORMAT_WINDOW_COLOUR_2,
	FORMAT_WINDOW_COLOUR_3,

	// The next 2 bytes specify the X and Y coordinates
	FORMAT_NEWLINE_X_Y = 17,

	// The next 4 bytes specify the sprite
	FORMAT_INLINE_SPRITE = 23,

	// Argument format codes
	FORMAT_ARGUMENT_CODE_START = 123,
	FORMAT_COMMA32 = 123,
	FORMAT_INT32,
	FORMAT_COMMA2DP32,
	FORMAT_COMMA16,
	FORMAT_UINT16,
	FORMAT_CURRENCY2DP,
	FORMAT_CURRENCY,
	FORMAT_STRINGID,
	FORMAT_STRINGID2,
	FORMAT_STRING,
	FORMAT_MONTHYEAR,
	FORMAT_MONTH,
	FORMAT_VELOCITY,
	FORMAT_POP16,
	FORMAT_PUSH16,
	FORMAT_DURATION,
	FORMAT_REALTIME,
	FORMAT_LENGTH,
	FORMAT_SPRITE,
	FORMAT_ARGUMENT_CODE_END = FORMAT_SPRITE,

	// Colour format codes
	FORMAT_COLOUR_CODE_START = 142,
	FORMAT_BLACK = 142,
	FORMAT_GREY,
	FORMAT_WHITE,
	FORMAT_RED,
	FORMAT_GREEN,
	FORMAT_YELLOW,
	FORMAT_TOPAZ,
	FORMAT_CELADON,
	FORMAT_BABYBLUE,
	FORMAT_PALELAVENDER,
	FORMAT_PALEGOLD,
	FORMAT_LIGHTPINK,
	FORMAT_PEARLAQUA,
	FORMAT_PALESILVER,
	FORMAT_COLOUR_CODE_END = FORMAT_PALESILVER,

	// Extra non-ascii characters
	FORMAT_AMINUSCULE = 159,
	FORMAT_CENT = 162,
	FORMAT_POUND = 163,
	FORMAT_YEN = 165,
	FORMAT_COPYRIGHT = 169,
	FORMAT_LEFTGUILLEMET = 171,
	FORMAT_DEGREE = 176,
	FORMAT_SQUARED = 178,
	FORMAT_RIGHTGUILLEMET = 187,
	FORMAT_INVERTEDQUESTION = 191,

	FORMAT_OPENQUOTES = 8220,
	FORMAT_ENDQUOTES = 8221,

	FORMAT_BULLET = 8226,
	FORMAT_POWERNEGATIVEONE = 8315,
	FORMAT_EURO = 8364,

	FORMAT_APPROX = 8776,

	FORMAT_UP = 9650,
	FORMAT_RIGHT = 9654,
	FORMAT_DOWN = 9660,
	FORMAT_LEFT = 9664,

	FORMAT_SMALLUP = 9652,
	FORMAT_SMALLDOWN = 9662,

	FORMAT_TICK = 10003,
	FORMAT_CROSS = 10005,

	// Format codes that need suitable unicode allocations
	FORMAT_SYMBOL_i = 20000,
	FORMAT_SYMBOL_RAILWAY = 20001,
	FORMAT_SYMBOL_ROAD = 20002,
	FORMAT_SYMBOL_FLAG = 20003,
};

#endif