#ifndef _DRAWING_FONT_H_
#define _DRAWING_FONT_H_

#include <SDL_ttf.h>
#include "../common.h"

enum {
	FONT_SIZE_TINY = 2,
	FONT_SIZE_SMALL = 0,
	FONT_SIZE_MEDIUM = 1,
	FONT_SIZE_BIG = 3,
	FONT_SIZE_COUNT = 4
};

enum {
	FONT_SPRITE_GLYPH_COUNT = 224,

	FONT_SPRITE_BASE_TINY = 448,
	FONT_SPRITE_BASE_SMALL = 0,
	FONT_SPRITE_BASE_MEDIUM = 224,
	FONT_SPRITE_BASE_BIG = 672
};

typedef struct {
	utf8 *filename;
	int ptSize;
	int offset_x;
	int offset_y;
	int line_height;
	TTF_Font *font;
} TTFFontDescriptor;

typedef struct {
	TTFFontDescriptor size[FONT_SIZE_COUNT];
} TTFFontSetDescriptor;

extern TTFFontSetDescriptor *gCurrentTTFFontSet;

void font_sprite_initialise_characters();
int font_sprite_get_codepoint_offset(int codepoint);
int font_sprite_get_codepoint_width(int fontSpriteBase, int codepoint);
int font_sprite_get_codepoint_sprite(int fontSpriteBase, int codepoint);
int font_get_size_from_sprite_base(uint16 spriteBase);
int font_get_line_height(int fontSpriteBase);
int font_get_line_height_small(int fontSpriteBase);

#endif