/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "ITTF.h"

#ifndef NO_TTF

#    include <mutex>

struct ttf_cache_entry
{
    TTFSurface* surface;
    TTF_Font* font;
    utf8* text;
    uint32_t lastUseTick;
};

struct ttf_getwidth_cache_entry
{
    uint32_t width;
    TTF_Font* font;
    utf8* text;
    uint32_t lastUseTick;
};

#    define TTF_SURFACE_CACHE_SIZE 256
#    define TTF_GETWIDTH_CACHE_SIZE 1024
struct TTF final : public ITTF
{
    bool ttf_initialise() override;
    void ttf_dispose() override;
    TTFFontDescriptor* ttf_get_font_from_sprite_base(FontSpriteBase spriteBase) override;
    void ttf_toggle_hinting() override;
    TTFSurface* ttf_surface_cache_get_or_add(TTF_Font* font, std::string_view text) override;
    uint32_t ttf_getwidth_cache_get_or_add(TTF_Font* font, std::string_view text) override;
    bool ttf_provides_glyph(const TTF_Font* font, codepoint_t codepoint) override;
    void ttf_free_surface(TTFSurface* surface) override;

    // TTF_SDLPORT
    int TTF_Init(void) override;
    TTF_Font* TTF_OpenFont(const char* file, int ptsize) override;
    int TTF_GlyphIsProvided(const TTF_Font* font, codepoint_t ch) override;
    int TTF_SizeUTF8(TTF_Font* font, const char* text, int* w, int* h) override;
    TTFSurface* TTF_RenderUTF8_Solid(TTF_Font* font, const char* text, uint32_t colour) override;
    TTFSurface* TTF_RenderUTF8_Shaded(TTF_Font* font, const char* text, uint32_t fg, uint32_t bg) override;
    void TTF_CloseFont(TTF_Font* font) override;
    void TTF_SetFontHinting(TTF_Font* font, int hinting) override;
    int TTF_GetFontHinting(const TTF_Font* font) override;
    void TTF_Quit(void) override;

private:
    bool _ttfInitialised = false;
    ttf_cache_entry _ttfSurfaceCache[TTF_SURFACE_CACHE_SIZE] = {};
    int32_t _ttfSurfaceCacheCount = 0;
    int32_t _ttfSurfaceCacheHitCount = 0;
    int32_t _ttfSurfaceCacheMissCount = 0;

    ttf_getwidth_cache_entry _ttfGetWidthCache[TTF_GETWIDTH_CACHE_SIZE] = {};
    int32_t _ttfGetWidthCacheCount = 0;
    int32_t _ttfGetWidthCacheHitCount = 0;
    int32_t _ttfGetWidthCacheMissCount = 0;

    std::mutex _mutex;

    TTF_Font* ttf_open_font(const utf8* fontPath, int32_t ptSize);
    void ttf_close_font(TTF_Font* font);
    void ttf_surface_cache_dispose(ttf_cache_entry* entry);
    void ttf_surface_cache_dispose_all();
    void ttf_getwidth_cache_dispose(ttf_getwidth_cache_entry* entry);
    void ttf_getwidth_cache_dispose_all();
    bool ttf_get_size(TTF_Font* font, std::string_view text, int32_t* outWidth, int32_t* outHeight);
    void ttf_toggle_hinting(bool);
    TTFSurface* ttf_render(TTF_Font* font, std::string_view text);

    TTF_Font* TTF_OpenFontIndex(const char* file, int ptsize, long index);
    TTF_Font* TTF_OpenFontIndexRW(FILE* src, int freesrc, int ptsize, long index);
};

#endif // NO_TTF
