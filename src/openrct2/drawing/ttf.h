#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#pragma once

#include "font.h"

bool ttf_initialise();
void ttf_dispose();

#ifndef NO_TTF

typedef struct TTFSurface {
    const void *    pixels;
    sint32          w;
    sint32          h;
    sint32          pitch;
} TTFSurface;

TTFFontDescriptor * ttf_get_font_from_sprite_base(uint16 spriteBase);
TTFSurface * ttf_surface_cache_get_or_add(TTF_Font * font, const utf8 * text);
uint32 ttf_getwidth_cache_get_or_add(TTF_Font * font, const utf8 * text);
bool ttf_provides_glyph(const TTF_Font * font, codepoint_t codepoint);
void ttf_free_surface(TTFSurface * surface);

// TTF_SDLPORT
int TTF_Init(void);
TTF_Font * TTF_OpenFont(const char *file, int ptsize);
int TTF_GlyphIsProvided(const TTF_Font *font, codepoint_t ch);
int TTF_SizeUTF8(TTF_Font *font, const char *text, int *w, int *h);
TTFSurface * TTF_RenderUTF8_Solid(TTF_Font *font, const char *text, uint32 colour);
void TTF_CloseFont(TTF_Font *font);
void TTF_Quit(void);

#endif // NO_TTF
