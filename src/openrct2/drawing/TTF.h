/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Font.h"

#include <string_view>

bool TTFInitialise();
void TTFDispose();
struct TTFSurface;

#ifndef NO_TTF

struct TTFSurface
{
    const void* pixels;
    int32_t w;
    int32_t h;
};

TTFFontDescriptor* TTFGetFontFromSpriteBase(FontStyle fontStyle);
void TTFToggleHinting();
TTFSurface* TTFSurfaceCacheGetOrAdd(TTF_Font* font, std::string_view text);
uint32_t TTFGetWidthCacheGetOrAdd(TTF_Font* font, std::string_view text);
bool TTFProvidesGlyph(const TTF_Font* font, codepoint_t codepoint);
void TTFFreeSurface(TTFSurface* surface);

// TTF_SDLPORT
int TTF_Init(void);
TTF_Font* TTF_OpenFont(const char* file, int ptsize);
int TTF_GlyphIsProvided(const TTF_Font* font, codepoint_t ch);
int TTF_SizeUTF8(TTF_Font* font, const char* text, int* w, int* h);
TTFSurface* TTF_RenderUTF8(TTF_Font* font, const char* text, bool shaded);
void TTF_CloseFont(TTF_Font* font);
void TTF_SetFontHinting(TTF_Font* font, int hinting);
int TTF_GetFontHinting(const TTF_Font* font);
void TTF_Quit(void);

#endif // NO_TTF
