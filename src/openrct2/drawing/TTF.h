/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3
 *****************************************************************************/

#pragma once

#include "Font.h"

bool ttf_initialise();
void ttf_dispose();

#ifndef NO_TTF

struct TTFSurface {
    const void *    pixels;
    sint32          w;
    sint32          h;
    sint32          pitch;
};

TTFFontDescriptor * ttf_get_font_from_sprite_base(uint16 spriteBase);
void ttf_toggle_hinting();
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
TTFSurface * TTF_RenderUTF8_Shaded(TTF_Font *font, const char *text, uint32 fg, uint32 bg);
void TTF_CloseFont(TTF_Font *font);
void TTF_SetFontHinting(TTF_Font* font, int hinting);
int TTF_GetFontHinting(const TTF_Font* font);
void TTF_Quit(void);

#endif // NO_TTF
