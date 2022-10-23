/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Font.h"

#include <string_view>

bool ttf_initialise();
void ttf_dispose();

#ifndef NO_TTF

struct TTFSurface
{
    const void* pixels;
    int32_t w;
    int32_t h;
    int32_t pitch;
};

TTFFontDescriptor* ttf_get_font_from_sprite_base(FontStyle fontStyle);
void ttf_toggle_hinting();
TTFSurface* ttf_surface_cache_get_or_add(TTF_Font* font, std::string_view text);
uint32_t ttf_getwidth_cache_get_or_add(TTF_Font* font, std::string_view text);
bool ttf_provides_glyph(const TTF_Font* font, codepoint_t codepoint);
void ttf_free_surface(TTFSurface* surface);

// TTF_SDLPORT
int TTF_Init(void);
TTF_Font* TTF_OpenFont(const char* file, int ptsize);
int TTF_GlyphIsProvided(const TTF_Font* font, codepoint_t ch);
int TTF_SizeUTF8(TTF_Font* font, const char* text, int* w, int* h);
TTFSurface* TTF_RenderUTF8_Solid(TTF_Font* font, const char* text, uint32_t colour);
TTFSurface* TTF_RenderUTF8_Shaded(TTF_Font* font, const char* text, uint32_t fg, uint32_t bg);
void TTF_CloseFont(TTF_Font* font);
void TTF_SetFontHinting(TTF_Font* font, int hinting);
int TTF_GetFontHinting(const TTF_Font* font);
void TTF_Quit(void);

#endif // NO_TTF
