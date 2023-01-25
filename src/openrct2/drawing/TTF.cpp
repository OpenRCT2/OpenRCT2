/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef NO_TTF

#    include <atomic>
#    include <mutex>
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wdocumentation"
#    include <ft2build.h>
#    include FT_FREETYPE_H
#    pragma clang diagnostic pop

#    include "../OpenRCT2.h"
#    include "../config/Config.h"
#    include "../core/Numerics.hpp"
#    include "../core/String.hpp"
#    include "../localisation/Localisation.h"
#    include "../localisation/LocalisationService.h"
#    include "../platform/Platform.h"
#    include "TTF.h"

static bool _ttfInitialised = false;

#    define TTF_SURFACE_CACHE_SIZE 256
#    define TTF_GETWIDTH_CACHE_SIZE 1024

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

static ttf_cache_entry _ttfSurfaceCache[TTF_SURFACE_CACHE_SIZE] = {};
static int32_t _ttfSurfaceCacheCount = 0;
static int32_t _ttfSurfaceCacheHitCount = 0;
static int32_t _ttfSurfaceCacheMissCount = 0;

static ttf_getwidth_cache_entry _ttfGetWidthCache[TTF_GETWIDTH_CACHE_SIZE] = {};
static int32_t _ttfGetWidthCacheCount = 0;
static int32_t _ttfGetWidthCacheHitCount = 0;
static int32_t _ttfGetWidthCacheMissCount = 0;

static std::mutex _mutex;

static TTF_Font* TTFOpenFont(const utf8* fontPath, int32_t ptSize);
static void TTFCloseFont(TTF_Font* font);
static void TTFSurfaceCacheDispose(ttf_cache_entry* entry);
static void TTFSurfaceCacheDisposeAll();
static void TTFGetWidthCacheDisposeAll();
static bool TTFGetSize(TTF_Font* font, std::string_view text, int32_t* outWidth, int32_t* outHeight);
static void TTFToggleHinting(bool);
static TTFSurface* TTFRender(TTF_Font* font, std::string_view text);

template<typename T> class FontLockHelper
{
    T& _mutex;
    const bool _enabled;

public:
    FontLockHelper(T& mutex)
        : _mutex(mutex)
        , _enabled(gConfigGeneral.MultiThreading)
    {
        if (_enabled)
            _mutex.lock();
    }
    ~FontLockHelper()
    {
        if (_enabled)
            _mutex.unlock();
    }
};

static void TTFToggleHinting(bool)
{
    if (!LocalisationService_UseTrueTypeFont())
    {
        return;
    }

    for (int32_t i = 0; i < FontStyleCount; i++)
    {
        TTFFontDescriptor* fontDesc = &(gCurrentTTFFontSet->size[i]);
        bool use_hinting = gConfigFonts.EnableHinting && fontDesc->hinting_threshold;
        TTF_SetFontHinting(fontDesc->font, use_hinting ? 1 : 0);
    }

    if (_ttfSurfaceCacheCount)
    {
        TTFSurfaceCacheDisposeAll();
    }
}

bool TTFInitialise()
{
    FontLockHelper<std::mutex> lock(_mutex);

    if (_ttfInitialised)
        return true;

    if (TTF_Init() != 0)
    {
        LOG_ERROR("Couldn't initialise FreeType engine");
        return false;
    }

    for (int32_t i = 0; i < FontStyleCount; i++)
    {
        TTFFontDescriptor* fontDesc = &(gCurrentTTFFontSet->size[i]);

        auto fontPath = Platform::GetFontPath(*fontDesc);
        if (fontPath.empty())
        {
            LOG_VERBOSE("Unable to load font '%s'", fontDesc->font_name);
            return false;
        }

        fontDesc->font = TTFOpenFont(fontPath.c_str(), fontDesc->ptSize);
        if (fontDesc->font == nullptr)
        {
            LOG_VERBOSE("Unable to load '%s'", fontPath.c_str());
            return false;
        }
    }

    TTFToggleHinting(true);

    _ttfInitialised = true;

    return true;
}

void TTFDispose()
{
    FontLockHelper<std::mutex> lock(_mutex);

    if (!_ttfInitialised)
        return;

    TTFSurfaceCacheDisposeAll();
    TTFGetWidthCacheDisposeAll();

    for (int32_t i = 0; i < FontStyleCount; i++)
    {
        TTFFontDescriptor* fontDesc = &(gCurrentTTFFontSet->size[i]);
        if (fontDesc->font != nullptr)
        {
            TTFCloseFont(fontDesc->font);
            fontDesc->font = nullptr;
        }
    }

    TTF_Quit();

    _ttfInitialised = false;
}

static TTF_Font* TTFOpenFont(const utf8* fontPath, int32_t ptSize)
{
    return TTF_OpenFont(fontPath, ptSize);
}

static void TTFCloseFont(TTF_Font* font)
{
    TTF_CloseFont(font);
}

static uint32_t TTFSurfaceCacheHash(TTF_Font* font, std::string_view text)
{
    uint32_t hash = static_cast<uint32_t>(((reinterpret_cast<uintptr_t>(font) * 23) ^ 0xAAAAAAAA) & 0xFFFFFFFF);
    for (auto c : text)
    {
        hash = Numerics::ror32(hash, 3) ^ (c * 13);
    }
    return hash;
}

static void TTFSurfaceCacheDispose(ttf_cache_entry* entry)
{
    if (entry->surface != nullptr)
    {
        TTFFreeSurface(entry->surface);
        free(entry->text);

        entry->surface = nullptr;
        entry->font = nullptr;
        entry->text = nullptr;
    }
}

static void TTFSurfaceCacheDisposeAll()
{
    for (int32_t i = 0; i < TTF_SURFACE_CACHE_SIZE; i++)
    {
        TTFSurfaceCacheDispose(&_ttfSurfaceCache[i]);
        _ttfSurfaceCacheCount--;
    }
}

void TTFToggleHinting()
{
    FontLockHelper<std::mutex> lock(_mutex);
    TTFToggleHinting(true);
}

TTFSurface* TTFSurfaceCacheGetOrAdd(TTF_Font* font, std::string_view text)
{
    ttf_cache_entry* entry;

    uint32_t hash = TTFSurfaceCacheHash(font, text);
    int32_t index = hash % TTF_SURFACE_CACHE_SIZE;

    FontLockHelper<std::mutex> lock(_mutex);

    for (int32_t i = 0; i < TTF_SURFACE_CACHE_SIZE; i++)
    {
        entry = &_ttfSurfaceCache[index];

        // Check if entry is a hit
        if (entry->surface == nullptr)
            break;
        if (entry->font == font && String::Equals(entry->text, text))
        {
            _ttfSurfaceCacheHitCount++;
            entry->lastUseTick = gCurrentDrawCount;
            return entry->surface;
        }

        // If entry hasn't been used for a while, replace it
        if (entry->lastUseTick < gCurrentDrawCount - 64)
        {
            break;
        }

        // Check if next entry is a hit
        if (++index >= TTF_SURFACE_CACHE_SIZE)
            index = 0;
    }

    // Cache miss, replace entry with new surface
    entry = &_ttfSurfaceCache[index];
    TTFSurfaceCacheDispose(entry);

    TTFSurface* surface = TTFRender(font, text);
    if (surface == nullptr)
    {
        return nullptr;
    }

    _ttfSurfaceCacheMissCount++;
    // printf("CACHE HITS: %d   MISSES: %d)\n", _ttfSurfaceCacheHitCount, _ttfSurfaceCacheMissCount);

    _ttfSurfaceCacheCount++;
    entry->surface = surface;
    entry->font = font;
    entry->text = strndup(text.data(), text.size());
    entry->lastUseTick = gCurrentDrawCount;
    return entry->surface;
}

static void TTFGetWidthCacheDispose(ttf_getwidth_cache_entry* entry)
{
    if (entry->text != nullptr)
    {
        free(entry->text);

        entry->width = 0;
        entry->font = nullptr;
        entry->text = nullptr;
    }
}

static void TTFGetWidthCacheDisposeAll()
{
    for (int32_t i = 0; i < TTF_GETWIDTH_CACHE_SIZE; i++)
    {
        TTFGetWidthCacheDispose(&_ttfGetWidthCache[i]);
        _ttfGetWidthCacheCount--;
    }
}

uint32_t TTFGetWidthCacheGetOrAdd(TTF_Font* font, std::string_view text)
{
    ttf_getwidth_cache_entry* entry;

    uint32_t hash = TTFSurfaceCacheHash(font, text);
    int32_t index = hash % TTF_GETWIDTH_CACHE_SIZE;

    FontLockHelper<std::mutex> lock(_mutex);

    for (int32_t i = 0; i < TTF_GETWIDTH_CACHE_SIZE; i++)
    {
        entry = &_ttfGetWidthCache[index];

        // Check if entry is a hit
        if (entry->text == nullptr)
            break;
        if (entry->font == font && String::Equals(entry->text, text))
        {
            _ttfGetWidthCacheHitCount++;
            entry->lastUseTick = gCurrentDrawCount;
            return entry->width;
        }

        // If entry hasn't been used for a while, replace it
        if (entry->lastUseTick < gCurrentDrawCount - 64)
        {
            break;
        }

        // Check if next entry is a hit
        if (++index >= TTF_GETWIDTH_CACHE_SIZE)
            index = 0;
    }

    // Cache miss, replace entry with new width
    entry = &_ttfGetWidthCache[index];
    TTFGetWidthCacheDispose(entry);

    int32_t width, height;
    TTFGetSize(font, text, &width, &height);

    _ttfGetWidthCacheMissCount++;

    _ttfGetWidthCacheCount++;
    entry->width = width;
    entry->font = font;
    entry->text = strndup(text.data(), text.size());
    entry->lastUseTick = gCurrentDrawCount;
    return entry->width;
}

TTFFontDescriptor* TTFGetFontFromSpriteBase(FontStyle fontStyle)
{
    FontLockHelper<std::mutex> lock(_mutex);
    return &gCurrentTTFFontSet->size[EnumValue(fontStyle)];
}

bool TTFProvidesGlyph(const TTF_Font* font, codepoint_t codepoint)
{
    return TTF_GlyphIsProvided(font, codepoint);
}

static bool TTFGetSize(TTF_Font* font, std::string_view text, int32_t* outWidth, int32_t* outHeight)
{
    thread_local std::string buffer;
    buffer.assign(text);
    return TTF_SizeUTF8(font, buffer.c_str(), outWidth, outHeight);
}

static TTFSurface* TTFRender(TTF_Font* font, std::string_view text)
{
    thread_local std::string buffer;
    buffer.assign(text);
    if (TTF_GetFontHinting(font) != 0)
    {
        return TTF_RenderUTF8_Shaded(font, buffer.c_str(), 0x000000FF, 0x000000FF);
    }

    return TTF_RenderUTF8_Solid(font, buffer.c_str(), 0x000000FF);
}

void TTFFreeSurface(TTFSurface* surface)
{
    free(const_cast<void*>(surface->pixels));
    free(surface);
}

#else

#    include "TTF.h"

bool TTFInitialise()
{
    return false;
}

void TTFDispose()
{
}

#endif // NO_TTF
