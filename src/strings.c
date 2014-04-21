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

#include <stdio.h>
#include "addresses.h"
#include "rct2.h"
#include "strings.h"

/**
 * Writes a formatted string to a buffer.
 *  rct2: 0x006C2555
 * dest (edi)
 * format (ax)
 * args (ecx)
 */
void format_string(char *dest, rct_string_id format, void *args)
{
	RCT2_CALLPROC_X(0x006C2555, format, 0, args, 0, 0, dest, 0);
}

void generate_string_file()
{
	FILE* f;
	uint8** str;
	uint8* c;
	int i;

	f = fopen("english.txt", "w");

	for (i = 0; i < 4442; i++) {
		str = ((uint8**)(0x009BF2D4 + (i * 4)));
		if (*str == (uint8*)0xFFFFFFFF)
			continue;
		c = *str;

		fprintf(f, "STR_%04d    :", i);
		while (*c != '\0') {
			switch (*c) {
			case 7:		fputs("{TINYFONT}", f);				break;
			case 8:		fputs("{BIGFONT}", f);				break;
			case 9:		fputs("{MEDIUMFONT}", f);			break;
			case 10:	fputs("{SMALLFONT}", f);			break;

			case 11:	fputs("{OUTLINE}", f);				break;

			case 34:	fputs("{ENDQUOTES}", f);			break;
			case 123:	fputs("{COMMA32}", f);				break;
			case 125:	fputs("{COMMA2DP32}", f);			break;
			case 126:	fputs("{COMMA16}", f);				break;
			case 128:	fputs("{CURRENCY2DP}", f);			break;
			case 129:	fputs("{CURRENCY}", f);				break;
			case 130:	fputs("{STRING}", f);				break;
			case 133:	fputs("{MONTHYEAR}", f);			break;
			case 135:	fputs("{VELOCITY}", f);				break;
			case 140:	fputs("{LENGTH}", f);				break;
			case 141:	fputs("{SPRITE}", f);				break;

			case 142:	fputs("{BLACK}", f);				break;
			case 143:	fputs("{GREY}", f);					break;
			case 144:	fputs("{WHITE}", f);				break;
			case 145:	fputs("{RED}", f);					break;
			case 146:	fputs("{GREEN}", f);				break;
			case 147:	fputs("{YELLOW}", f);				break;
			case 148:	fputs("{TOPAZ}", f);				break;
			case 149:	fputs("{CELADON}", f);				break;
			case 150:	fputs("{BABYBLUE}", f);				break;
			case 151:	fputs("{PALELAVENDER}", f);			break;
			case 152:	fputs("{PALEGOLD}", f);				break;
			case 153:	fputs("{LIGHTPINK}", f);			break;
			case 154:	fputs("{PEARLAQUA}", f);			break;
			case 155:	fputs("{PALESILVER}", f);			break;

			case 159:	fputs("{AMINUSCULE}", f);			break;
			case 160:	fputs("{UP}", f);					break;
			case 163:	fputs("{POUND}", f);				break;
			case 165:	fputs("{YEN}", f);					break;
			case 169:	fputs("{COPYRIGHT}", f);			break;
			case 170:	fputs("{DOWN}", f);					break;
			case 171:	fputs("{LEFTGUILLEMET}", f);		break;
			case 172:	fputs("{TICK}", f);					break;
			case 173:	fputs("{CROSS}", f);				break;
			case 175:	fputs("{RIGHT}", f);				break;
			case 176:	fputs("{DEGREE}", f);				break;
			case 178:	fputs("{SQUARED}", f);				break;
			case 180:	fputs("{OPENQUOTES}", f);			break;
			case 181:	fputs("{EURO}", f);					break;
			case 184:	fputs("{APPROX}", f);				break;
			case 185:	fputs("{POWERNEGATIVEONE}", f);		break;
			case 186:	fputs("{BULLET}", f);				break;
			case 187:	fputs("{RIGHTGUILLEMET}", f);		break;
			case 188:	fputs("{SMALLUP}", f);				break;
			case 189:	fputs("{SMALLDOWN}", f);			break;
			case 190:	fputs("{LEFT}", f);					break;
			case 191:	fputs("{INVERTEDQUESTION}", f);		break;
			default:
				if (*c < 32 || *c > 127)
					fprintf(f, "{%d}", *c);
				else
					fputc(*c, f);
				break;
			}

			c++;
		}
		fputc('\n', f);
	}

	fclose(f);
}
