/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Font.h"

#include <string_view>

#ifdef NO_TTF

struct ITTF
{
    bool ttf_initialise();
    void ttf_dispose();
};

#else

struct TTFSurface
{
    const void* pixels;
    int32_t w;
    int32_t h;
    int32_t pitch;
};

struct ITTF
{
    virtual ~ITTF() = default;
    virtual bool ttf_initialise() abstract;
    virtual void ttf_dispose() abstract;
    virtual TTFFontDescriptor* ttf_get_font_from_sprite_base(FontSpriteBase spriteBase) abstract;
    virtual void ttf_toggle_hinting() abstract;
    virtual TTFSurface* ttf_surface_cache_get_or_add(TTF_Font* font, std::string_view text) abstract;
    virtual uint32_t ttf_getwidth_cache_get_or_add(TTF_Font* font, std::string_view text) abstract;
    virtual bool ttf_provides_glyph(const TTF_Font* font, codepoint_t codepoint) abstract;
    virtual void ttf_free_surface(TTFSurface* surface) abstract;

    // TTF_SDLPORT
    virtual int TTF_Init(void) abstract;
    virtual TTF_Font* TTF_OpenFont(const char* file, int ptsize) abstract;
    virtual int TTF_GlyphIsProvided(const TTF_Font* font, codepoint_t ch) abstract;
    virtual int TTF_SizeUTF8(TTF_Font* font, const char* text, int* w, int* h) abstract;
    virtual TTFSurface* TTF_RenderUTF8_Solid(TTF_Font* font, const char* text, uint32_t colour) abstract;
    virtual TTFSurface* TTF_RenderUTF8_Shaded(TTF_Font* font, const char* text, uint32_t fg, uint32_t bg) abstract;
    virtual void TTF_CloseFont(TTF_Font* font) abstract;
    virtual void TTF_SetFontHinting(TTF_Font* font, int hinting) abstract;
    virtual int TTF_GetFontHinting(const TTF_Font* font) abstract;
    virtual void TTF_Quit(void) abstract;
};

#endif // NO_TTF
