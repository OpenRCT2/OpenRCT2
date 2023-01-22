/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Font.h"

#include <string_view>
namespace OpenRCT2
{
    bool TTFInitialise();
    void TTFDispose();

#ifndef NO_TTF

    struct TTFSurface
    {
        const void* pixels;
        int32_t w;
        int32_t h;
        int32_t pitch;
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
    TTFSurface* TTF_RenderUTF8_Solid(TTF_Font* font, const char* text, uint32_t colour);
    TTFSurface* TTF_RenderUTF8_Shaded(TTF_Font* font, const char* text, uint32_t fg, uint32_t bg);
    void TTF_CloseFont(TTF_Font* font);
    void TTF_SetFontHinting(TTF_Font* font, int hinting);
    int TTF_GetFontHinting(const TTF_Font* font);
    void TTF_Quit(void);

#endif // NO_TTF
} // namespace OpenRCT2
