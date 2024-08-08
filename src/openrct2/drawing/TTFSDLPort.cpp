// Adapted from freetype.h in order to avoid C-style casts.
// clang-format off

#define FT_LOAD_TARGET_ALT(x) (static_cast<FT_Int32>((x)&15) << 16)
#define FT_IMAGE_TAG(value, _x1, _x2, _x3, _x4)                                                                                \
    value                                                                                                                      \
        = ((static_cast<unsigned long>(_x1) << 24) | (static_cast<unsigned long>(_x2) << 16)                                   \
           | (static_cast<unsigned long>(_x3) << 8) | static_cast<unsigned long>(_x4))

/**
 * The following code is from SDL2_ttf (2 Jan 2017).
 * Taking just what was needed for OpenRCT2 with all SDL2 calls
 * removed.
 */

#ifndef NO_TTF

/*
SDL_ttf:  A companion library to SDL for working with TrueType (tm) fonts
Copyright (C) 2001-2017 Sam Lantinga <slouken@libsdl.org>

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#    include "../Diagnostic.h"

#    include <cmath>
#    include <cstring>
#    include <stdio.h>
#    include <stdlib.h>
#    include <string.h>

#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wdocumentation"
#    include <ft2build.h>
#    include FT_FREETYPE_H
#    include FT_OUTLINE_H
#    include FT_STROKER_H
#    include FT_GLYPH_H
#    include FT_TRUETYPE_IDS_H
#    pragma clang diagnostic pop

#    include "TTF.h"

#    pragma warning(disable : 4018) // '<': signed / unsigned mismatch

/* ZERO WIDTH NO-BREAKSPACE (Unicode byte order mark) */
#    define UNICODE_BOM_NATIVE 0xFEFF
#    define UNICODE_BOM_SWAPPED 0xFFFE

/* Set and retrieve the font style */
#    define TTF_STYLE_NORMAL 0x00
#    define TTF_STYLE_BOLD 0x01
#    define TTF_STYLE_ITALIC 0x02
#    define TTF_STYLE_UNDERLINE 0x04
#    define TTF_STYLE_STRIKETHROUGH 0x08

/* Set and retrieve FreeType hinter settings */
#    define TTF_HINTING_NORMAL 0
#    define TTF_HINTING_LIGHT 1
#    define TTF_HINTING_MONO 2
#    define TTF_HINTING_NONE 3

/* FIXME: Right now we assume the gray-scale renderer Freetype is using
supports 256 shades of gray, but we should instead key off of num_grays
in the result FT_Bitmap after the FT_Render_Glyph() call. */
#    define NUM_GRAYS 256

/* Handy routines for converting from fixed point */
#    define FT_FLOOR(X) (((X) & -64) / 64)
#    define FT_CEIL(X) ((((X) + 63) & -64) / 64)

#    define CACHED_METRICS 0x10
#    define CACHED_BITMAP 0x01
#    define CACHED_PIXMAP 0x02

/* Cached glyph information */
struct c_glyph
{
    int stored;
    FT_UInt index;
    FT_Bitmap bitmap;
    FT_Bitmap pixmap;
    int minx;
    int maxx;
    int miny;
    int maxy;
    int yoffset;
    int advance;
    uint16_t cached;
};

/* The structure used to hold internal font information */
struct InternalTTFFont
{
    /* Freetype2 maintains all sorts of useful info itself */
    FT_Face face;

    /* We'll cache these ourselves */
    int height;
    int ascent;
    int descent;
    int lineskip;

    /* The font style */
    int face_style;
    int style;
    int outline;

    /* Whether kerning is desired */
    int kerning;

    /* Extra width in glyph bounds for text styles */
    int glyph_overhang;
    float glyph_italics;

    /* Information in the font for underlining */
    int underline_offset;
    int underline_height;

    /* Cache for style-transformed glyphs */
    c_glyph* current;
    c_glyph cache[257]; /* 257 is a prime */

    /* We are responsible for closing the font stream */
    FILE* src;
    int freesrc;
    FT_Open_Args args;

    /* For non-scalable formats, we must remember which font index size */
    int font_size_family;

    /* really just flags passed into FT_Load_Glyph */
    int hinting;
};

/* Handle a style only if the font does not already handle it */
#    define TTF_HANDLE_STYLE_BOLD(font) (((font)->style & TTF_STYLE_BOLD) && !((font)->face_style & TTF_STYLE_BOLD))
#    define TTF_HANDLE_STYLE_ITALIC(font) (((font)->style & TTF_STYLE_ITALIC) && !((font)->face_style & TTF_STYLE_ITALIC))
#    define TTF_HANDLE_STYLE_UNDERLINE(font) ((font)->style & TTF_STYLE_UNDERLINE)
#    define TTF_HANDLE_STYLE_STRIKETHROUGH(font) ((font)->style & TTF_STYLE_STRIKETHROUGH)

/* Font styles that does not impact glyph drawing */
#    define TTF_STYLE_NO_GLYPH_CHANGE (TTF_STYLE_UNDERLINE | TTF_STYLE_STRIKETHROUGH)

/* The FreeType font engine/library */
static FT_Library library;
static int TTF_initialized = 0;

#    define TTF_SetError LOG_ERROR

#    define TTF_CHECKPOINTER(p, errval)                                                                                        \
        if (!TTF_initialized)                                                                                                  \
        {                                                                                                                      \
            TTF_SetError("Library not initialized");                                                                           \
            return errval;                                                                                                     \
        }                                                                                                                      \
        if (!(p))                                                                                                              \
        {                                                                                                                      \
            TTF_SetError("Passed a NULL pointer");                                                                             \
            return errval;                                                                                                     \
        }

/* Gets the top row of the underline. The outline
is taken into account.
*/
static int TTF_underline_top_row(TTF_Font* font)
{
    /* With outline, the underline_offset is underline_offset+outline. */
    /* So, we don't have to remove the top part of the outline height. */
    return font->ascent - font->underline_offset - 1;
}

/* Gets the bottom row of the underline. The outline
is taken into account.
*/
static int TTF_underline_bottom_row(TTF_Font* font)
{
    int row = TTF_underline_top_row(font) + font->underline_height;
    if (font->outline > 0)
    {
        /* Add underline_offset outline offset and */
        /* the bottom part of the outline. */
        row += font->outline * 2;
    }
    return row;
}

/* Gets the top row of the strikethrough. The outline
is taken into account.
*/
static int TTF_strikethrough_top_row(TTF_Font* font)
{
    /* With outline, the first text row is 'outline'. */
    /* So, we don't have to remove the top part of the outline height. */
    return font->height / 2;
}

static void TTF_initLineMectrics(const TTF_Font* font, const TTFSurface* textbuf, const int row, uint8_t** pdst, int* pheight)
{
    uint8_t* dst;
    int height;

    dst = const_cast<uint8_t*>(static_cast<const uint8_t*>(textbuf->pixels));
    if (row > 0)
    {
        dst += row * textbuf->w;
    }

    height = font->underline_height;
    /* Take outline into account */
    if (font->outline > 0)
    {
        height += font->outline * 2;
    }
    *pdst = dst;
    *pheight = height;
}

/* Draw a solid line of underline_height (+ optional outline)
at the given row. The row value must take the
outline into account.
*/
static void TTF_drawLine_Solid(const TTF_Font* font, const TTFSurface* textbuf, const int row)
{
    int line;
    uint8_t* dst_check = const_cast<uint8_t*>(static_cast<const uint8_t*>(textbuf->pixels)) + textbuf->w * textbuf->h;
    uint8_t* dst;
    int height;

    TTF_initLineMectrics(font, textbuf, row, &dst, &height);

    /* Draw line */
    for (line = height; line > 0 && dst < dst_check; --line)
    {
        /* 1 because 0 is the bg color */
        std::fill_n(dst, textbuf->w, 0x01);
        dst += textbuf->w;
    }
}

/* Draw a shaded line of underline_height (+ optional outline)
   at the given row. The row value must take the
   outline into account.
*/
static void TTF_drawLine_Shaded(const TTF_Font* font, const TTFSurface* textbuf, const int row)
{
    int line;
    uint8_t* dst_check = const_cast<uint8_t*>(static_cast<const uint8_t*>(textbuf->pixels)) + textbuf->w * textbuf->h;
    uint8_t* dst;
    int height;

    TTF_initLineMectrics(font, textbuf, row, &dst, &height);

    /* Draw line */
    for (line = height; line > 0 && dst < dst_check; --line)
    {
        std::fill_n(dst, textbuf->w, NUM_GRAYS - 1);
        dst += textbuf->w;
    }
}

static void TTF_SetFTError(const char* msg, [[maybe_unused]] FT_Error error)
{
#    ifdef USE_FREETYPE_ERRORS
#        undef FTERRORS_H
#        define FT_ERRORDEF(e, v, s) { e, s },
    static const struct
    {
        int err_code;
        const char* err_msg;
    } ft_errors[] = {
#        include <freetype/fterrors.h>
    };
    int i;
    const char* err_msg;
    char buffer[1024];

    err_msg = NULL;
    for (i = 0; i < ((sizeof ft_errors) / (sizeof ft_errors[0])); ++i)
    {
        if (error == ft_errors[i].err_code)
        {
            err_msg = ft_errors[i].err_msg;
            break;
        }
    }
    if (!err_msg)
    {
        err_msg = "unknown FreeType error";
    }
    TTF_SetError("%s: %s", msg, err_msg);
#    else
    TTF_SetError("%s", msg);
#    endif /* USE_FREETYPE_ERRORS */
}

int TTF_Init(void)
{
    int status = 0;

    if (!TTF_initialized)
    {
        FT_Error error = FT_Init_FreeType(&library);
        if (error)
        {
            TTF_SetFTError("Couldn't init FreeType engine", error);
            status = -1;
        }
    }
    if (status == 0)
    {
        ++TTF_initialized;
    }
    return status;
}

static unsigned long RWread(FT_Stream stream, unsigned long offset, unsigned char* buffer, unsigned long count)
{
    FILE* src;

    src = static_cast<FILE*>(stream->descriptor.pointer);
    fseek(src, static_cast<int>(offset), SEEK_SET);
    if (count == 0)
    {
        return 0;
    }
    return static_cast<unsigned long>(fread(buffer, 1, static_cast<int>(count), src));
}

static size_t fsize(FILE* file)
{
    size_t origPos = ftell(file);
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, static_cast<long>(origPos), SEEK_SET);
    return size;
}

static TTF_Font* TTF_OpenFontIndexRW(FILE* src, int freesrc, int ptsize, long index)
{
    TTF_Font* font;
    FT_Error error;
    FT_Face face;
    FT_Fixed scale;
    FT_Stream stream;
    FT_CharMap found;
    int64_t position;
    int i;

    if (!TTF_initialized)
    {
        TTF_SetError("Library not initialized");
        if (src && freesrc)
        {
            fclose(src);
        }
        return NULL;
    }

    if (!src)
    {
        TTF_SetError("Passed a NULL font source");
        return NULL;
    }

    /* Check to make sure we can seek in this stream */
    position = ftell(src);
    if (position < 0)
    {
        TTF_SetError("Can't seek in stream");
        if (freesrc)
        {
            fclose(src);
        }
        return NULL;
    }

    font = static_cast<TTF_Font*>(malloc(sizeof *font));
    if (font == NULL)
    {
        TTF_SetError("Out of memory");
        if (freesrc)
        {
            fclose(src);
        }
        return NULL;
    }
    std::fill_n(reinterpret_cast<uint8_t*>(font), sizeof(*font), 0x00);

    font->src = src;
    font->freesrc = freesrc;

    stream = static_cast<FT_Stream>(malloc(sizeof(*stream)));
    if (stream == NULL)
    {
        TTF_SetError("Out of memory");
        TTF_CloseFont(font);
        return NULL;
    }
    std::fill_n(reinterpret_cast<uint8_t*>(stream), sizeof(*stream), 0x00);

    stream->read = RWread;
    stream->descriptor.pointer = src;
    stream->pos = static_cast<unsigned long>(position);
    stream->size = static_cast<unsigned long>(fsize(src) - position);

    font->args.flags = FT_OPEN_STREAM;
    font->args.stream = stream;

    error = FT_Open_Face(library, &font->args, index, &font->face);
    if (error)
    {
        TTF_SetFTError("Couldn't load font file", error);
        TTF_CloseFont(font);
        return NULL;
    }
    face = font->face;

    /* Set charmap for loaded font */
    found = 0;
    for (i = 0; i < face->num_charmaps; i++)
    {
        FT_CharMap charmap = face->charmaps[i];
        if ((charmap->platform_id == 3 && charmap->encoding_id == 1)    /* Windows Unicode */
            || (charmap->platform_id == 3 && charmap->encoding_id == 0) /* Windows Symbol */
            || (charmap->platform_id == 2 && charmap->encoding_id == 1) /* ISO Unicode */
            || (charmap->platform_id == 0))
        { /* Apple Unicode */
            found = charmap;
            break;
        }
    }
    if (found)
    {
        /* If this fails, continue using the default charmap */
        FT_Set_Charmap(face, found);
    }

    /* Make sure that our font face is scalable (global metrics) */
    if (FT_IS_SCALABLE(face))
    {
        /* Set the character size and use default DPI (72) */
        error = FT_Set_Char_Size(font->face, 0, ptsize * 64, 0, 0);
        if (error)
        {
            TTF_SetFTError("Couldn't set font size", error);
            TTF_CloseFont(font);
            return NULL;
        }

        /* Get the scalable font metrics for this font */
        scale = face->size->metrics.y_scale;
        font->ascent = FT_CEIL(FT_MulFix(face->ascender, scale));
        font->descent = FT_CEIL(FT_MulFix(face->descender, scale));
        font->height = font->ascent - font->descent + /* baseline */ 1;
        font->lineskip = FT_CEIL(FT_MulFix(face->height, scale));
        font->underline_offset = FT_FLOOR(FT_MulFix(face->underline_position, scale));
        font->underline_height = FT_FLOOR(FT_MulFix(face->underline_thickness, scale));
    }
    else
    {
        /* Non-scalable font case.  ptsize determines which family
         * or series of fonts to grab from the non-scalable format.
         * It is not the point size of the font.
         * */
        if (ptsize >= font->face->num_fixed_sizes)
            ptsize = font->face->num_fixed_sizes - 1;
        font->font_size_family = ptsize;
        error = FT_Set_Pixel_Sizes(face, face->available_sizes[ptsize].width, face->available_sizes[ptsize].height);

        /* With non-scalale fonts, Freetype2 likes to fill many of the
         * font metrics with the value of 0.  The size of the
         * non-scalable fonts must be determined differently
         * or sometimes cannot be determined.
         * */
        font->ascent = face->available_sizes[ptsize].height;
        font->descent = 0;
        font->height = face->available_sizes[ptsize].height;
        font->lineskip = FT_CEIL(font->ascent);
        font->underline_offset = FT_FLOOR(face->underline_position);
        font->underline_height = FT_FLOOR(face->underline_thickness);
    }

    if (font->underline_height < 1)
    {
        font->underline_height = 1;
    }

#    ifdef DEBUG_FONTS
    printf("Font metrics:\n");
    printf("\tascent = %d, descent = %d\n", font->ascent, font->descent);
    printf("\theight = %d, lineskip = %d\n", font->height, font->lineskip);
    printf("\tunderline_offset = %d, underline_height = %d\n", font->underline_offset, font->underline_height);
    printf(
        "\tunderline_top_row = %d, strikethrough_top_row = %d\n", TTF_underline_top_row(font), TTF_strikethrough_top_row(font));
#    endif

    /* Initialize the font face style */
    font->face_style = TTF_STYLE_NORMAL;
    if (font->face->style_flags & FT_STYLE_FLAG_BOLD)
    {
        font->face_style |= TTF_STYLE_BOLD;
    }
    if (font->face->style_flags & FT_STYLE_FLAG_ITALIC)
    {
        font->face_style |= TTF_STYLE_ITALIC;
    }

    /* Set the default font style */
    font->style = font->face_style;
    font->outline = 0;
    font->kerning = 1;
    font->glyph_overhang = face->size->metrics.y_ppem / 10;
    /* x offset = cos(((90.0-12)/360)*2*M_PI), or 12 degree angle */
    font->glyph_italics = 0.207f;
    font->glyph_italics *= font->height;

    return font;
}

static TTF_Font* TTF_OpenFontIndex(const char* file, int ptsize, long index)
{
    FILE* rw = fopen(file, "rb");
    if (rw == NULL)
    {
        return NULL;
    }
    return TTF_OpenFontIndexRW(rw, 1, ptsize, index);
}

TTF_Font* TTF_OpenFont(const char* file, int ptsize)
{
    return TTF_OpenFontIndex(file, ptsize, 0);
}

static void Flush_Glyph(c_glyph* glyph)
{
    glyph->stored = 0;
    glyph->index = 0;
    if (glyph->bitmap.buffer != nullptr)
    {
        free(glyph->bitmap.buffer);
        glyph->bitmap.buffer = 0;
    }
    if (glyph->pixmap.buffer != nullptr)
    {
        free(glyph->pixmap.buffer);
        glyph->pixmap.buffer = 0;
    }
    glyph->cached = 0;
}

static void Flush_Cache(TTF_Font* font)
{
    int i;
    int size = sizeof(font->cache) / sizeof(font->cache[0]);

    for (i = 0; i < size; ++i)
    {
        if (font->cache[i].cached)
        {
            Flush_Glyph(&font->cache[i]);
        }
    }
}

static FT_Error Load_Glyph(TTF_Font* font, uint16_t ch, c_glyph* cached, int want)
{
    FT_Face face;
    FT_Error error;
    FT_GlyphSlot glyph;
    FT_Glyph_Metrics* metrics;
    FT_Outline* outline;

    if (!font || !font->face)
    {
        return FT_Err_Invalid_Handle;
    }

    face = font->face;

    /* Load the glyph */
    if (!cached->index)
    {
        cached->index = FT_Get_Char_Index(face, ch);
    }
    error = FT_Load_Glyph(face, cached->index, FT_LOAD_DEFAULT | font->hinting);
    if (error)
    {
        return error;
    }

    /* Get our glyph shortcuts */
    glyph = face->glyph;
    metrics = &glyph->metrics;
    outline = &glyph->outline;

    /* Get the glyph metrics if desired */
    if ((want & CACHED_METRICS) && !(cached->stored & CACHED_METRICS))
    {
        if (FT_IS_SCALABLE(face))
        {
            /* Get the bounding box */
            cached->minx = FT_FLOOR(metrics->horiBearingX);
            cached->maxx = FT_CEIL(metrics->horiBearingX + metrics->width);
            cached->maxy = FT_FLOOR(metrics->horiBearingY);
            cached->miny = cached->maxy - FT_CEIL(metrics->height);
            cached->yoffset = font->ascent - cached->maxy;
            cached->advance = FT_CEIL(metrics->horiAdvance);
        }
        else
        {
            /* Get the bounding box for non-scalable format.
             * Again, freetype2 fills in many of the font metrics
             * with the value of 0, so some of the values we
             * need must be calculated differently with certain
             * assumptions about non-scalable formats.
             * */
            cached->minx = FT_FLOOR(metrics->horiBearingX);
            cached->maxx = FT_CEIL(metrics->horiBearingX + metrics->width);
            cached->maxy = FT_FLOOR(metrics->horiBearingY);
            cached->miny = cached->maxy - FT_CEIL(face->available_sizes[font->font_size_family].height);
            cached->yoffset = 0;
            cached->advance = FT_CEIL(metrics->horiAdvance);
        }

        /* Adjust for bold and italic text */
        if (TTF_HANDLE_STYLE_BOLD(font))
        {
            cached->maxx += font->glyph_overhang;
        }
        if (TTF_HANDLE_STYLE_ITALIC(font))
        {
            cached->maxx += static_cast<int>(ceil(font->glyph_italics));
        }
        cached->stored |= CACHED_METRICS;
    }

    if (((want & CACHED_BITMAP) && !(cached->stored & CACHED_BITMAP))
        || ((want & CACHED_PIXMAP) && !(cached->stored & CACHED_PIXMAP)))
    {
        int mono = (want & CACHED_BITMAP);
        unsigned int i;
        FT_Bitmap* src;
        FT_Bitmap* dst;
        FT_Glyph bitmap_glyph = NULL;

        /* Handle the italic style */
        if (TTF_HANDLE_STYLE_ITALIC(font))
        {
            FT_Matrix shear;

            shear.xx = 1 << 16;
            shear.xy = static_cast<int>(font->glyph_italics * (1 << 16)) / font->height;
            shear.yx = 0;
            shear.yy = 1 << 16;

            FT_Outline_Transform(outline, &shear);
        }

        /* Render as outline */
        if ((font->outline > 0) && glyph->format != FT_GLYPH_FORMAT_BITMAP)
        {
            FT_Stroker stroker;
            FT_Get_Glyph(glyph, &bitmap_glyph);
            error = FT_Stroker_New(library, &stroker);
            if (error)
            {
                return error;
            }
            FT_Stroker_Set(stroker, font->outline * 64, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
            FT_Glyph_Stroke(&bitmap_glyph, stroker, 1 /* delete the original glyph */);
            FT_Stroker_Done(stroker);
            /* Render the glyph */
            error = FT_Glyph_To_Bitmap(&bitmap_glyph, mono ? ft_render_mode_mono : ft_render_mode_normal, 0, 1);
            if (error)
            {
                FT_Done_Glyph(bitmap_glyph);
                return error;
            }
            src = &(reinterpret_cast<FT_BitmapGlyph>(bitmap_glyph))->bitmap;
        }
        else
        {
            /* Render the glyph */
            error = FT_Render_Glyph(glyph, mono ? ft_render_mode_mono : ft_render_mode_normal);
            if (error)
            {
                return error;
            }
            src = &glyph->bitmap;
        }
        /* Copy over information to cache */
        if (mono)
        {
            dst = &cached->bitmap;
        }
        else
        {
            dst = &cached->pixmap;
        }
        std::memcpy(dst, src, sizeof(*dst));

        /* FT_Render_Glyph() and .fon fonts always generate a
         * two-color (black and white) glyphslot surface, even
         * when rendered in ft_render_mode_normal. */
        /* FT_IS_SCALABLE() means that the font is in outline format,
         * but does not imply that outline is rendered as 8-bit
         * grayscale, because embedded bitmap/graymap is preferred
         * (see FT_LOAD_DEFAULT section of FreeType2 API Reference).
         * FT_Render_Glyph() canreturn two-color bitmap or 4/16/256-
         * color graymap according to the format of embedded bitmap/
         * graymap. */
        if (src->pixel_mode == FT_PIXEL_MODE_MONO)
        {
            dst->pitch *= 8;
        }
        else if (src->pixel_mode == FT_PIXEL_MODE_GRAY2)
        {
            dst->pitch *= 4;
        }
        else if (src->pixel_mode == FT_PIXEL_MODE_GRAY4)
        {
            dst->pitch *= 2;
        }

        /* Adjust for bold and italic text */
        if (TTF_HANDLE_STYLE_BOLD(font))
        {
            int bump = font->glyph_overhang;
            dst->pitch += bump;
            dst->width += bump;
        }
        if (TTF_HANDLE_STYLE_ITALIC(font))
        {
            int bump = static_cast<int>(ceil(font->glyph_italics));
            dst->pitch += bump;
            dst->width += bump;
        }

        if (dst->rows != 0)
        {
            dst->buffer = static_cast<unsigned char*>(malloc(dst->pitch * dst->rows));
            if (dst->buffer == nullptr)
            {
                return FT_Err_Out_Of_Memory;
            }
            std::fill_n(dst->buffer, dst->pitch * dst->rows, 0x00);

            for (i = 0; i < src->rows; i++)
            {
                int soffset = i * src->pitch;
                int doffset = i * dst->pitch;
                if (mono)
                {
                    unsigned char* srcp = src->buffer + soffset;
                    unsigned char* dstp = dst->buffer + doffset;
                    unsigned int j;
                    if (src->pixel_mode == FT_PIXEL_MODE_MONO)
                    {
                        for (j = 0; j < src->width; j += 8)
                        {
                            unsigned char c = *srcp++;
                            *dstp++ = (c & 0x80) >> 7;
                            c <<= 1;
                            *dstp++ = (c & 0x80) >> 7;
                            c <<= 1;
                            *dstp++ = (c & 0x80) >> 7;
                            c <<= 1;
                            *dstp++ = (c & 0x80) >> 7;
                            c <<= 1;
                            *dstp++ = (c & 0x80) >> 7;
                            c <<= 1;
                            *dstp++ = (c & 0x80) >> 7;
                            c <<= 1;
                            *dstp++ = (c & 0x80) >> 7;
                            c <<= 1;
                            *dstp++ = (c & 0x80) >> 7;
                        }
                    }
                    else if (src->pixel_mode == FT_PIXEL_MODE_GRAY2)
                    {
                        for (j = 0; j < src->width; j += 4)
                        {
                            unsigned char c = *srcp++;
                            *dstp++ = (((c & 0xA0) >> 6) >= 0x2) ? 1 : 0;
                            c <<= 2;
                            *dstp++ = (((c & 0xA0) >> 6) >= 0x2) ? 1 : 0;
                            c <<= 2;
                            *dstp++ = (((c & 0xA0) >> 6) >= 0x2) ? 1 : 0;
                            c <<= 2;
                            *dstp++ = (((c & 0xA0) >> 6) >= 0x2) ? 1 : 0;
                        }
                    }
                    else if (src->pixel_mode == FT_PIXEL_MODE_GRAY4)
                    {
                        for (j = 0; j < src->width; j += 2)
                        {
                            unsigned char c = *srcp++;
                            *dstp++ = (((c & 0xF0) >> 4) >= 0x8) ? 1 : 0;
                            c <<= 4;
                            *dstp++ = (((c & 0xF0) >> 4) >= 0x8) ? 1 : 0;
                        }
                    }
                    else
                    {
                        for (j = 0; j < src->width; j++)
                        {
                            unsigned char c = *srcp++;
                            *dstp++ = (c >= 0x80) ? 1 : 0;
                        }
                    }
                }
                else if (src->pixel_mode == FT_PIXEL_MODE_MONO)
                {
                    /* This special case wouldn't
                     * be here if the FT_Render_Glyph()
                     * function wasn't buggy when it tried
                     * to render a .fon font with 256
                     * shades of gray.  Instead, it
                     * returns a black and white surface
                     * and we have to translate it back
                     * to a 256 gray shaded surface.
                     * */
                    unsigned char* srcp = src->buffer + soffset;
                    unsigned char* dstp = dst->buffer + doffset;
                    unsigned char c;
                    unsigned int j, k;
                    for (j = 0; j < src->width; j += 8)
                    {
                        c = *srcp++;
                        for (k = 0; k < 8; ++k)
                        {
                            if ((c & 0x80) >> 7)
                            {
                                *dstp++ = NUM_GRAYS - 1;
                            }
                            else
                            {
                                *dstp++ = 0x00;
                            }
                            c <<= 1;
                        }
                    }
                }
                else if (src->pixel_mode == FT_PIXEL_MODE_GRAY2)
                {
                    unsigned char* srcp = src->buffer + soffset;
                    unsigned char* dstp = dst->buffer + doffset;
                    unsigned char c;
                    unsigned int j, k;
                    for (j = 0; j < src->width; j += 4)
                    {
                        c = *srcp++;
                        for (k = 0; k < 4; ++k)
                        {
                            if ((c & 0xA0) >> 6)
                            {
                                *dstp++ = NUM_GRAYS * ((c & 0xA0) >> 6) / 3 - 1;
                            }
                            else
                            {
                                *dstp++ = 0x00;
                            }
                            c <<= 2;
                        }
                    }
                }
                else if (src->pixel_mode == FT_PIXEL_MODE_GRAY4)
                {
                    unsigned char* srcp = src->buffer + soffset;
                    unsigned char* dstp = dst->buffer + doffset;
                    unsigned char c;
                    unsigned int j, k;
                    for (j = 0; j < src->width; j += 2)
                    {
                        c = *srcp++;
                        for (k = 0; k < 2; ++k)
                        {
                            if ((c & 0xF0) >> 4)
                            {
                                *dstp++ = NUM_GRAYS * ((c & 0xF0) >> 4) / 15 - 1;
                            }
                            else
                            {
                                *dstp++ = 0x00;
                            }
                            c <<= 4;
                        }
                    }
                }
                else
                {
                    std::memcpy(dst->buffer + doffset, src->buffer + soffset, src->pitch);
                }
            }
        }

        /* Handle the bold style */
        if (TTF_HANDLE_STYLE_BOLD(font))
        {
            int row;
            int col;
            int offset;
            int pixel;
            uint8_t* pixmap;

            /* The pixmap is a little hard, we have to add and clamp */
            for (row = dst->rows - 1; row >= 0; --row)
            {
                pixmap = dst->buffer + row * dst->pitch;
                for (offset = 1; offset <= font->glyph_overhang; ++offset)
                {
                    for (col = dst->width - 1; col > 0; --col)
                    {
                        if (mono)
                        {
                            pixmap[col] |= pixmap[col - 1];
                        }
                        else
                        {
                            pixel = (pixmap[col] + pixmap[col - 1]);
                            if (pixel > NUM_GRAYS - 1)
                            {
                                pixel = NUM_GRAYS - 1;
                            }
                            pixmap[col] = static_cast<uint8_t>(pixel);
                        }
                    }
                }
            }
        }

        /* Mark that we rendered this format */
        if (mono)
        {
            cached->stored |= CACHED_BITMAP;
        }
        else
        {
            cached->stored |= CACHED_PIXMAP;
        }

        /* Free outlined glyph */
        if (bitmap_glyph)
        {
            FT_Done_Glyph(bitmap_glyph);
        }
    }

    /* We're done, mark this glyph cached */
    cached->cached = ch;

    return 0;
}

static FT_Error Find_Glyph(TTF_Font* font, uint16_t ch, int want)
{
    int retval = 0;
    int hsize = sizeof(font->cache) / sizeof(font->cache[0]);

    int h = ch % hsize;
    font->current = &font->cache[h];

    if (font->current->cached != ch)
        Flush_Glyph(font->current);

    if ((font->current->stored & want) != want)
    {
        retval = Load_Glyph(font, ch, font->current, want);
    }
    return retval;
}

void TTF_CloseFont(TTF_Font* font)
{
    if (font)
    {
        Flush_Cache(font);
        if (font->face)
        {
            FT_Done_Face(font->face);
        }
        if (font->args.stream)
        {
            free(font->args.stream);
        }
        if (font->freesrc)
        {
            fclose(font->src);
        }
        free(font);
    }
}

/* Gets a unicode value from a UTF-8 encoded string and advance the string */
#    define UNKNOWN_UNICODE 0xFFFD
static uint32_t UTF8_getch(const char** src, size_t* srclen)
{
    const uint8_t* p = *reinterpret_cast<const uint8_t**>(src);
    size_t left = 0;
    [[maybe_unused]] bool overlong = false;
    bool underflow = false;
    uint32_t ch = UNKNOWN_UNICODE;

    if (*srclen == 0)
    {
        return UNKNOWN_UNICODE;
    }
    if (p[0] >= 0xFC)
    {
        if ((p[0] & 0xFE) == 0xFC)
        {
            if (p[0] == 0xFC && (p[1] & 0xFC) == 0x80)
            {
                overlong = true;
            }
            ch = static_cast<uint32_t>(p[0] & 0x01);
            left = 5;
        }
    }
    else if (p[0] >= 0xF8)
    {
        if ((p[0] & 0xFC) == 0xF8)
        {
            if (p[0] == 0xF8 && (p[1] & 0xF8) == 0x80)
            {
                overlong = true;
            }
            ch = static_cast<uint32_t>(p[0] & 0x03);
            left = 4;
        }
    }
    else if (p[0] >= 0xF0)
    {
        if ((p[0] & 0xF8) == 0xF0)
        {
            if (p[0] == 0xF0 && (p[1] & 0xF0) == 0x80)
            {
                overlong = true;
            }
            ch = static_cast<uint32_t>(p[0] & 0x07);
            left = 3;
        }
    }
    else if (p[0] >= 0xE0)
    {
        if ((p[0] & 0xF0) == 0xE0)
        {
            if (p[0] == 0xE0 && (p[1] & 0xE0) == 0x80)
            {
                overlong = true;
            }
            ch = static_cast<uint32_t>(p[0] & 0x0F);
            left = 2;
        }
    }
    else if (p[0] >= 0xC0)
    {
        if ((p[0] & 0xE0) == 0xC0)
        {
            if ((p[0] & 0xDE) == 0xC0)
            {
                overlong = true;
            }
            ch = static_cast<uint32_t>(p[0] & 0x1F);
            left = 1;
        }
    }
    else
    {
        if ((p[0] & 0x80) == 0x00)
        {
            ch = static_cast<uint32_t>(p[0]);
        }
    }
    ++*src;
    --*srclen;
    while (left > 0 && *srclen > 0)
    {
        ++p;
        if ((p[0] & 0xC0) != 0x80)
        {
            ch = UNKNOWN_UNICODE;
            break;
        }
        ch <<= 6;
        ch |= (p[0] & 0x3F);
        ++*src;
        --*srclen;
        --left;
    }
    if (left > 0)
    {
        underflow = true;
    }
    /* Technically overlong sequences are invalid and should not be interpreted.
    However, it doesn't cause a security risk here and I don't see any harm in
    displaying them. The application is responsible for any other side effects
    of allowing overlong sequences (e.g. string compares failing, etc.)
    See bug 1931 for sample input that triggers this.
    */
    /*if (overlong) return UNKNOWN_UNICODE;*/
    if (underflow || (ch >= 0xD800 && ch <= 0xDFFF) || (ch == 0xFFFE || ch == 0xFFFF) || ch > 0x10FFFF)
    {
        ch = UNKNOWN_UNICODE;
    }
    return ch;
}

int TTF_GlyphIsProvided(const TTF_Font* font, codepoint_t ch)
{
    return (FT_Get_Char_Index(font->face, ch));
}

int TTF_SizeUTF8(TTF_Font* font, const char* text, int* w, int* h)
{
    int status;
    int x, z;
    int minx, maxx;
    int miny, maxy;
    c_glyph* glyph;
    FT_Error error;
    FT_Long use_kerning;
    FT_UInt prev_index = 0;
    int outline_delta = 0;
    size_t textlen;

    TTF_CHECKPOINTER(text, -1);

    /* Initialize everything to 0 */
    status = 0;
    minx = maxx = 0;
    miny = maxy = 0;

    /* check kerning */
    use_kerning = FT_HAS_KERNING(font->face) && font->kerning;

    /* Init outline handling */
    if (font->outline > 0)
    {
        outline_delta = font->outline * 2;
    }

    /* Load each character and sum it's bounding box */
    textlen = strlen(text);
    x = 0;
    while (textlen > 0)
    {
        uint16_t c = UTF8_getch(&text, &textlen);
        if (c == UNICODE_BOM_NATIVE || c == UNICODE_BOM_SWAPPED)
        {
            continue;
        }

        error = Find_Glyph(font, c, CACHED_METRICS);
        if (error)
        {
            TTF_SetFTError("Couldn't find glyph", error);
            return -1;
        }
        glyph = font->current;

        /* handle kerning */
        if (use_kerning && prev_index && glyph->index)
        {
            FT_Vector delta;
            FT_Get_Kerning(font->face, prev_index, glyph->index, ft_kerning_default, &delta);
            x += delta.x >> 6;
        }

#    if 0
        if ((ch == text) && (glyph->minx < 0)) {
            /* Fixes the texture wrapping bug when the first letter
            * has a negative minx value or horibearing value.  The entire
            * bounding box must be adjusted to be bigger so the entire
            * letter can fit without any texture corruption or wrapping.
            *
            * Effects: First enlarges bounding box.
            * Second, xstart has to start ahead of its normal spot in the
            * negative direction of the negative minx value.
            * (pushes everything to the right).
            *
            * This will make the memory copy of the glyph bitmap data
            * work out correctly.
            * */
            z -= glyph->minx;
        }
#    endif

        z = x + glyph->minx;
        if (minx > z)
        {
            minx = z;
        }
        if (TTF_HANDLE_STYLE_BOLD(font))
        {
            x += font->glyph_overhang;
        }
        if (glyph->advance > glyph->maxx)
        {
            z = x + glyph->advance;
        }
        else
        {
            z = x + glyph->maxx;
        }
        if (maxx < z)
        {
            maxx = z;
        }
        x += glyph->advance;

        if (glyph->miny < miny)
        {
            miny = glyph->miny;
        }
        if (glyph->maxy > maxy)
        {
            maxy = glyph->maxy;
        }
        prev_index = glyph->index;
    }

    /* Fill the bounds rectangle */
    if (w != nullptr)
    {
        /* Add outline extra width */
        *w = (maxx - minx) + outline_delta;
    }
    if (h != nullptr)
    {
        /* Some fonts descend below font height (FletcherGothicFLF) */
        /* Add outline extra height */
        *h = (font->ascent - miny) + outline_delta;
        if (*h < font->height)
        {
            *h = font->height;
        }
        /* Update height according to the needs of the underline style */
        if (TTF_HANDLE_STYLE_UNDERLINE(font))
        {
            int bottom_row = TTF_underline_bottom_row(font);
            if (*h < bottom_row)
            {
                *h = bottom_row;
            }
        }
    }
    return status;
}

TTFSurface* TTF_RenderUTF8(TTF_Font* font, const char* text, bool shaded)
{
    bool first;
    int xstart;
    int width;
    int height;
    TTFSurface* textbuf;
    uint8_t* src;
    uint8_t* dst;
    uint8_t* dst_check;
    unsigned int row, col;
    c_glyph* glyph;

    FT_Bitmap* current;
    FT_Error error;
    FT_Long use_kerning;
    FT_UInt prev_index = 0;
    size_t textlen;

    TTF_CHECKPOINTER(text, NULL);

    /* Get the dimensions of the text surface */
    if ((TTF_SizeUTF8(font, text, &width, &height) < 0) || !width)
    {
        TTF_SetError("Text has zero width");
        return NULL;
    }

    /* Create the target surface */
    textbuf = static_cast<TTFSurface*>(calloc(1, sizeof(TTFSurface)));
    if (textbuf == NULL)
    {
        return NULL;
    }
    textbuf->w = width;
    textbuf->h = height;
    textbuf->pixels = calloc(1, width * height);

    /* Adding bound checking to avoid all kinds of memory corruption errors
    that may occur. */
    dst_check = const_cast<uint8_t*>(static_cast<const uint8_t*>(textbuf->pixels)) + textbuf->w * textbuf->h;

    /* check kerning */
    use_kerning = FT_HAS_KERNING(font->face) && font->kerning;

    /* Load and render each character */
    textlen = strlen(text);
    first = true;
    xstart = 0;
    while (textlen > 0)
    {
        uint16_t c = UTF8_getch(&text, &textlen);
        if (c == UNICODE_BOM_NATIVE || c == UNICODE_BOM_SWAPPED)
        {
            continue;
        }

        error = Find_Glyph(font, c, CACHED_METRICS | (shaded ? CACHED_PIXMAP : CACHED_BITMAP));
        if (error)
        {
            TTF_SetFTError("Couldn't find glyph", error);
            TTFFreeSurface(textbuf);
            return NULL;
        }
        glyph = font->current;
        current = shaded ? &glyph->pixmap : &glyph->bitmap;
        /* Ensure the width of the pixmap is correct. On some cases,
         * freetype may report a larger pixmap than possible.*/
        width = current->width;
        if (font->outline <= 0 && width > glyph->maxx - glyph->minx)
        {
            width = glyph->maxx - glyph->minx;
        }
        /* do kerning, if possible AC-Patch */
        if (use_kerning && prev_index && glyph->index)
        {
            FT_Vector delta;
            FT_Get_Kerning(font->face, prev_index, glyph->index, ft_kerning_default, &delta);
            xstart += delta.x >> 6;
        }
        /* Compensate for wrap around bug with negative minx's */
        if (first && (glyph->minx < 0))
        {
            xstart -= glyph->minx;
        }
        first = false;

        for (row = 0; row < current->rows; ++row)
        {
            /* Make sure we don't go either over, or under the
             * limit */
            if (static_cast<signed>(row) + glyph->yoffset < 0)
            {
                continue;
            }
            if (static_cast<signed>(row) + glyph->yoffset >= textbuf->h)
            {
                continue;
            }
            dst = const_cast<uint8_t*>(static_cast<const uint8_t*>(textbuf->pixels)) + (row + glyph->yoffset) * textbuf->w
                + xstart + glyph->minx;
            src = current->buffer + row * current->pitch;

            for (col = width; col > 0 && dst < dst_check; --col)
            {
                *dst++ |= *src++;
            }
        }

        xstart += glyph->advance;
        if (TTF_HANDLE_STYLE_BOLD(font))
        {
            xstart += font->glyph_overhang;
        }
        prev_index = glyph->index;
    }

    /* Handle the underline style */
    if (TTF_HANDLE_STYLE_UNDERLINE(font))
    {
        row = TTF_underline_top_row(font);
        if (shaded)
            TTF_drawLine_Shaded(font, textbuf, row);
        else
            TTF_drawLine_Solid(font, textbuf, row);
    }

    /* Handle the strikethrough style */
    if (TTF_HANDLE_STYLE_STRIKETHROUGH(font))
    {
        row = TTF_strikethrough_top_row(font);
        if (shaded)
            TTF_drawLine_Shaded(font, textbuf, row);
        else
            TTF_drawLine_Solid(font, textbuf, row);
    }
    return textbuf;
}

void TTF_SetFontHinting(TTF_Font* font, int hinting)
{
    if (hinting == TTF_HINTING_LIGHT)
        font->hinting = FT_LOAD_TARGET_ALT(FT_RENDER_MODE_LIGHT);
    else if (hinting == TTF_HINTING_MONO)
        font->hinting = FT_LOAD_TARGET_ALT(FT_RENDER_MODE_MONO);
    else if (hinting == TTF_HINTING_NONE)
        font->hinting = FT_LOAD_NO_HINTING;
    else
        font->hinting = 0;

    Flush_Cache(font);
}

int TTF_GetFontHinting(const TTF_Font* font)
{
    if (font->hinting == FT_LOAD_TARGET_ALT(FT_RENDER_MODE_LIGHT))
        return TTF_HINTING_LIGHT;
    if (font->hinting == FT_LOAD_TARGET_ALT(FT_RENDER_MODE_MONO))
        return TTF_HINTING_MONO;
    if (font->hinting == FT_LOAD_NO_HINTING)
        return TTF_HINTING_NONE;
    return 0;
}

void TTF_Quit(void)
{
    if (TTF_initialized)
    {
        if (--TTF_initialized == 0)
        {
            FT_Done_FreeType(library);
        }
    }
}

#endif
