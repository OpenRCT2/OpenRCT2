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

#ifndef NO_TTF

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../localisation/localisation.h"
#include "../platform/platform.h"
#include "../rct2.h"
#include "ttf.h"

static bool _ttfInitialised = false;

#define TTF_SURFACE_CACHE_SIZE 256
#define TTF_GETWIDTH_CACHE_SIZE 1024

typedef struct ttf_cache_entry
{
    TTFSurface *    surface;
    TTF_Font *      font;
    utf8 *          text;
    uint32          lastUseTick;
} ttf_cache_entry;

typedef struct ttf_getwidth_cache_entry
{
    uint32      width;
    TTF_Font *  font;
    utf8 *      text;
    uint32      lastUseTick;
} ttf_getwidth_cache_entry;

static ttf_cache_entry _ttfSurfaceCache[TTF_SURFACE_CACHE_SIZE] = { 0 };
static sint32 _ttfSurfaceCacheCount = 0;
static sint32 _ttfSurfaceCacheHitCount = 0;
static sint32 _ttfSurfaceCacheMissCount = 0;

static ttf_getwidth_cache_entry _ttfGetWidthCache[TTF_GETWIDTH_CACHE_SIZE] = { 0 };
static sint32 _ttfGetWidthCacheCount = 0;
static sint32 _ttfGetWidthCacheHitCount = 0;
static sint32 _ttfGetWidthCacheMissCount = 0;

static TTF_Font * ttf_open_font(const utf8 * fontPath, sint32 ptSize);
static void ttf_close_font(TTF_Font * font);
static uint32 ttf_surface_cache_hash(TTF_Font * font, const utf8 * text);
static void ttf_surface_cache_dispose(ttf_cache_entry * entry);
static void ttf_surface_cache_dispose_all();
static void ttf_getwidth_cache_dispose_all();
static bool ttf_get_size(TTF_Font * font, const utf8 * text, sint32 * width, sint32 * height);
static TTFSurface * ttf_render(TTF_Font * font, const utf8 * text);

bool ttf_initialise()
{
    if (!_ttfInitialised) {
        if (TTF_Init() != 0) {
            log_error("Couldn't initialise FreeType engine");
            return false;
        }

        for (sint32 i = 0; i < 4; i++) {
            TTFFontDescriptor *fontDesc = &(gCurrentTTFFontSet->size[i]);

            utf8 fontPath[MAX_PATH];
            if (!platform_get_font_path(fontDesc, fontPath, sizeof(fontPath))) {
                log_error("Unable to load font '%s'", fontDesc->font_name);
                return false;
            }

            fontDesc->font = ttf_open_font(fontPath, fontDesc->ptSize);
            if (fontDesc->font == NULL) {
                log_error("Unable to load '%s'", fontPath);
                return false;
            }
        }
        _ttfInitialised = true;
    }
    return true;
}

void ttf_dispose()
{
    if (_ttfInitialised)
    {
        ttf_surface_cache_dispose_all();
        ttf_getwidth_cache_dispose_all();

        for (sint32 i = 0; i < 4; i++) {
            TTFFontDescriptor *fontDesc = &(gCurrentTTFFontSet->size[i]);
            if (fontDesc->font != NULL) {
                ttf_close_font(fontDesc->font);
                fontDesc->font = NULL;
            }
        }

        TTF_Quit();
        _ttfInitialised = false;
    }
}

static TTF_Font * ttf_open_font(const utf8 * fontPath, sint32 ptSize)
{
    return TTF_OpenFont(fontPath, ptSize);
}

static void ttf_close_font(TTF_Font * font)
{
    TTF_CloseFont(font);
}

static uint32 ttf_surface_cache_hash(TTF_Font *font, const utf8 *text)
{
    uint32 hash = (uint32)((((uintptr_t)font * 23) ^ 0xAAAAAAAA) & 0xFFFFFFFF);
    for (const utf8 *ch = text; *ch != 0; ch++) {
        hash = ror32(hash, 3) ^ (*ch * 13);
    }
    return hash;
}

static void ttf_surface_cache_dispose(ttf_cache_entry *entry)
{
    if (entry->surface != NULL) {
        ttf_free_surface(entry->surface);
        free(entry->text);

        entry->surface = NULL;
        entry->font = NULL;
        entry->text = NULL;
    }
}

static void ttf_surface_cache_dispose_all()
{
    for (sint32 i = 0; i < TTF_SURFACE_CACHE_SIZE; i++) {
        ttf_surface_cache_dispose(&_ttfSurfaceCache[i]);
        _ttfSurfaceCacheCount--;
    }
}

TTFSurface * ttf_surface_cache_get_or_add(TTF_Font * font, const utf8 * text)
{
    ttf_cache_entry *entry;

    uint32 hash = ttf_surface_cache_hash(font, text);
    sint32 index = hash % TTF_SURFACE_CACHE_SIZE;
    for (sint32 i = 0; i < TTF_SURFACE_CACHE_SIZE; i++) {
        entry = &_ttfSurfaceCache[index];

        // Check if entry is a hit
        if (entry->surface == NULL) break;
        if (entry->font == font && strcmp(entry->text, text) == 0) {
            _ttfSurfaceCacheHitCount++;
            entry->lastUseTick = gCurrentDrawCount;
            return entry->surface;
        }

        // If entry hasn't been used for a while, replace it
        if (entry->lastUseTick < gCurrentDrawCount - 64) {
            break;
        }

        // Check if next entry is a hit
        if (++index >= TTF_SURFACE_CACHE_SIZE) index = 0;
    }

    // Cache miss, replace entry with new surface
    entry = &_ttfSurfaceCache[index];
    ttf_surface_cache_dispose(entry);

    TTFSurface * surface = ttf_render(font, text);
    if (surface == NULL) {
        return NULL;
    }

    _ttfSurfaceCacheMissCount++;
    // printf("CACHE HITS: %d   MISSES: %d)\n", _ttfSurfaceCacheHitCount, _ttfSurfaceCacheMissCount);

    _ttfSurfaceCacheCount++;
    entry->surface = surface;
    entry->font = font;
    entry->text = _strdup(text);
    entry->lastUseTick = gCurrentDrawCount;
    return entry->surface;
}

static void ttf_getwidth_cache_dispose(ttf_getwidth_cache_entry *entry)
{
    if (entry->text != NULL) {
        free(entry->text);

        entry->width = 0;
        entry->font = NULL;
        entry->text = NULL;
    }
}

static void ttf_getwidth_cache_dispose_all()
{
    for (sint32 i = 0; i < TTF_GETWIDTH_CACHE_SIZE; i++) {
        ttf_getwidth_cache_dispose(&_ttfGetWidthCache[i]);
        _ttfGetWidthCacheCount--;
    }
}

uint32 ttf_getwidth_cache_get_or_add(TTF_Font * font, const utf8 * text)
{
    ttf_getwidth_cache_entry *entry;

    uint32 hash = ttf_surface_cache_hash(font, text);
    sint32 index = hash % TTF_GETWIDTH_CACHE_SIZE;
    for (sint32 i = 0; i < TTF_GETWIDTH_CACHE_SIZE; i++) {
        entry = &_ttfGetWidthCache[index];

        // Check if entry is a hit
        if (entry->text == NULL) break;
        if (entry->font == font && strcmp(entry->text, text) == 0) {
            _ttfGetWidthCacheHitCount++;
            entry->lastUseTick = gCurrentDrawCount;
            return entry->width;
        }

        // If entry hasn't been used for a while, replace it
        if (entry->lastUseTick < gCurrentDrawCount - 64) {
            break;
        }

        // Check if next entry is a hit
        if (++index >= TTF_GETWIDTH_CACHE_SIZE) index = 0;
    }

    // Cache miss, replace entry with new width
    entry = &_ttfGetWidthCache[index];
    ttf_getwidth_cache_dispose(entry);

    sint32 width, height;
    ttf_get_size(font, text, &width, &height);

    _ttfGetWidthCacheMissCount++;

    _ttfGetWidthCacheCount++;
    entry->width = width;
    entry->font = font;
    entry->text = _strdup(text);
    entry->lastUseTick = gCurrentDrawCount;
    return entry->width;
}

TTFFontDescriptor * ttf_get_font_from_sprite_base(uint16 spriteBase)
{
    return &gCurrentTTFFontSet->size[font_get_size_from_sprite_base(spriteBase)];
}

bool ttf_provides_glyph(const TTF_Font * font, codepoint_t codepoint)
{
    return TTF_GlyphIsProvided(font, codepoint);
}

static bool ttf_get_size(TTF_Font * font, const utf8 * text, sint32 * outWidth, sint32 * outHeight)
{
    return TTF_SizeUTF8(font, text, outWidth, outHeight);
}

static TTFSurface * ttf_render(TTF_Font * font, const utf8 * text)
{
    return TTF_RenderUTF8_Solid(font, text, 0x000000FF);
}

void ttf_free_surface(TTFSurface * surface)
{
    free((void *)surface->pixels);
    free(surface);
}

#else

#include "ttf.h"

bool ttf_initialise()
{
    return false;
}

void ttf_dispose()
{
}

#endif // NO_TTF
