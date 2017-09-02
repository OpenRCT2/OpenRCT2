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

#include "map_element.h"
#include "../paint.h"
#include "../supports.h"
#include "../../config/Config.h"
#include "../../game.h"
#include "../../interface/viewport.h"
#include "../../localisation/localisation.h"
#include "../../util/util.h"
#include "../../world/map.h"
#include "../../world/scenery.h"

// 6B8172:
static void scenery_multiple_paint_supports(uint8 direction, uint16 height, rct_map_element *mapElement, uint32 dword_F4387C, rct_large_scenery_tile *tile)
{
    if (tile->var_7 & 0x20) {
        return;
    }

    sint32 ax = 0;
    sint32 supportHeight = height;

    if (supportHeight & 0xF) {
        supportHeight &= 0xFFFFFFF0;
        ax = 49;
    }

    sint32 supportImageColourFlags = IMAGE_TYPE_REMAP;

    if (dword_F4387C) {
        supportImageColourFlags = dword_F4387C;
    }

    wooden_b_supports_paint_setup((direction & 1), ax, supportHeight, supportImageColourFlags, NULL);

    sint32 clearanceHeight = ceil2(mapElement->clearance_height * 8 + 15, 16);

    if (tile->var_7 & 0x40) {
        paint_util_set_segment_support_height(SEGMENTS_ALL, clearanceHeight, 0x20);
    } else {
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    }

    paint_util_set_general_support_height(clearanceHeight, 0x20);
}

static rct_large_scenery_text_glyph *scenery_multiple_sign_get_glyph(rct_large_scenery_text *text, uint32 codepoint)
{
    if (codepoint >= countof(text->glyphs)) {
        return &text->glyphs['?'];
    }
    return &text->glyphs[codepoint];
}

static sint32 scenery_multiple_sign_text_width(const utf8 *str, rct_large_scenery_text *text)
{
    sint32 width = 0;
    uint32 codepoint;
    while ((codepoint = utf8_get_next(str, &str)) != 0) {
        width += scenery_multiple_sign_get_glyph(text, codepoint)->width;
    }
    return width;
}

static sint32 scenery_multiple_sign_text_height(const utf8 *str, rct_large_scenery_text *text)
{
    sint32 height = 0;
    uint32 codepoint;
    while ((codepoint = utf8_get_next(str, &str)) != 0) {
        height += scenery_multiple_sign_get_glyph(text, codepoint)->height;
    }
    return height;
}

static const utf8 *scenery_multiple_sign_fit_text(const utf8 *str, rct_large_scenery_text *text, bool height)
{
    static utf8 fitStr[32];
    utf8 *fitStrEnd = fitStr;
    safe_strcpy(fitStr, str, sizeof(fitStr));
    sint32 w = 0;
    uint32 codepoint;
    while (w <= text->max_width && (codepoint = utf8_get_next(fitStrEnd, (const utf8**)&fitStrEnd)) != 0) {
        if (height) {
            w += scenery_multiple_sign_get_glyph(text, codepoint)->height;
        } else {
            w += scenery_multiple_sign_get_glyph(text, codepoint)->width;
        }
    }
    *fitStrEnd = 0;
    return fitStr;
}

static sint32 div_to_minus_infinity(sint32 a, sint32 b) {
    return (a / b) - (a % b < 0);
}

static void scenery_multiple_sign_paint_line(const utf8 *str, rct_large_scenery_text *text, sint32 textImage, sint32 textColour, uint8 direction, sint32 y_offset)
{
    const utf8 *fitStr = scenery_multiple_sign_fit_text(str, text, false);
    sint32 width = scenery_multiple_sign_text_width(fitStr, text);
    sint32 x_offset = text->offset[(direction & 1)].x;
    sint32 acc = y_offset * ((direction & 1) ? -1 : 1);
    if (!(text->flags & LARGE_SCENERY_TEXT_FLAG_VERTICAL)) {
        // sign is horizontal, centre text:
        x_offset -= (width / 2);
        acc -= (width / 2);
    }
    uint32 codepoint;
    while ((codepoint = utf8_get_next(fitStr, &fitStr)) != 0) {
        sint32 glyph_offset = scenery_multiple_sign_get_glyph(text, codepoint)->image_offset;
        uint8 glyph_type = direction & 1;
        if (text->flags & LARGE_SCENERY_TEXT_FLAG_VERTICAL) { // vertical sign
            glyph_offset *= 2;
        } else {
            glyph_offset *= 4;
            // set slightly different glyph on horizontal sign, which was rendered 1/2 pixel lower to deal with aliasing:
            if (direction & 1) {
                if (!(acc & 1)) {
                    glyph_type += 2;
                }
            } else {
                if ((acc & 1)) {
                    glyph_type += 2;
                }
            }
        }
        sint32 image_id = (textImage + glyph_offset + glyph_type) | textColour;
        if (direction == 3) {
            paint_attach_to_previous_ps(image_id, x_offset, -div_to_minus_infinity(acc, 2));
        } else {
            if (text->flags & LARGE_SCENERY_TEXT_FLAG_VERTICAL) {
                paint_attach_to_previous_ps(image_id, x_offset, div_to_minus_infinity(acc, 2));
            } else {
                paint_attach_to_previous_attach(image_id, x_offset, div_to_minus_infinity(acc, 2));
            }
        }
        x_offset += scenery_multiple_sign_get_glyph(text, codepoint)->width;
        acc += scenery_multiple_sign_get_glyph(text, codepoint)->width;
    }
}

typedef struct boundbox {
    rct_xy16 offset;
    rct_xy16 length;
} boundbox;

static const boundbox s98E3C4[] = {
    { 3,   3, 26, 26 },
    { 17, 17, 12, 12 },
    { 17,  3, 12, 12 },
    { 17,  3, 12, 26 },
    { 3,   3, 12, 12 },
    { 3,   3, 26, 26 },
    { 3,   3, 28, 12 },
    { 3,   3, 26, 26 },
    { 3,  17, 12, 12 },
    { 3,  17, 26, 12 },
    { 3,   3, 26, 26 },
    { 3,   3, 26, 26 },
    { 3,   3, 12, 28 },
    { 3,   3, 26, 26 },
    { 3,   3, 26, 26 },
    { 3,   3, 26, 26 },
    { 1,   1, 30, 30 }
};

/*
*
* rct2: 0x006B7F0C
*/
void scenery_multiple_paint(paint_session * session, uint8 direction, uint16 height, rct_map_element *mapElement) {
    //RCT2_CALLPROC_X(0x6B7F0C, 0, 0, direction, height, (sint32)mapElement, 0, 0); return;
    session->InteractionType = VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY;
    uint32 ebp = mapElement->properties.scenerymultiple.type >> 10;
    rct_scenery_entry *entry = get_large_scenery_entry(mapElement->properties.scenerymultiple.type & 0x3FF);
    if (entry == (void*)-1)
        return;

    uint32 image_id = (ebp << 2) + entry->image + 4 + direction;
    rct_large_scenery_tile *tile = &entry->large_scenery.tiles[ebp];
    uint32 dword_F4387C = 0;
    image_id |= ((mapElement->properties.scenerymultiple.colour[0] & 0x1F) << 19) | ((mapElement->properties.scenerymultiple.colour[1] & 0x1F) << 24) | IMAGE_TYPE_REMAP | IMAGE_TYPE_REMAP_2_PLUS;
    rct_xyz16 boxlength;
    rct_xyz16 boxoffset;
    if (gTrackDesignSaveMode) {
        if (!track_design_save_contains_map_element(mapElement)) {
            ebp = 0x21700000;
            image_id &= 0x7FFFF;
            dword_F4387C = ebp;
            image_id |= dword_F4387C;
        }
    }
    if (mapElement->flags & MAP_ELEMENT_FLAG_GHOST) {
        session->InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;
        ebp = construction_markers[gConfigGeneral.construction_marker_colour];
        image_id &= 0x7FFFF;
        dword_F4387C = ebp;
        image_id |= dword_F4387C;
    }
    sint32 ah = tile->z_clearance;
    if (ah > 0x80) {
        ah = 0x80;
    }
    ah -= 3;
    uint16 edi = tile->var_7;
    sint32 esi = 16;
    if (edi & 0xF00) {
        edi &= 0xF000;
        edi = rol16(edi, direction);
        esi = (edi & 0xF) | (edi >> 12);
    }
    boxoffset.x = s98E3C4[esi].offset.x;
    boxoffset.y = s98E3C4[esi].offset.y;
    boxoffset.z = height;
    boxlength.x =  s98E3C4[esi].length.x;
    boxlength.y =  s98E3C4[esi].length.y;
    boxlength.z = ah;
    sub_98197C(image_id, 0, 0, boxlength.x, boxlength.y, ah, height, boxoffset.x, boxoffset.y, boxoffset.z, get_current_rotation());
    if (entry->large_scenery.scrolling_mode == 0xFF || direction == 1 || direction == 2) {
        scenery_multiple_paint_supports(direction, height, mapElement, dword_F4387C, tile);
        return;
    }
    if (entry->large_scenery.flags & LARGE_SCENERY_FLAG_3D_TEXT) {
        if (entry->large_scenery.tiles[1].x_offset != (sint16)(uint16)0xFFFF) {
            sint32 al = ((mapElement->properties.surface.terrain >> 2) - 1) & 3;
            if (al != direction) {
                scenery_multiple_paint_supports(direction, height, mapElement, dword_F4387C, tile);
                return;
            }
        }
        rct_drawpixelinfo* dpi = session->Unk140E9A8;
        if (dpi->zoom_level > 1) {
            scenery_multiple_paint_supports(direction, height, mapElement, dword_F4387C, tile);
            return;
        }
        // 6B8331:
        // Draw sign text:
        set_format_arg(0, uint32, 0);
        set_format_arg(4, uint32, 0);
        sint32 textColour = mapElement->properties.scenerymultiple.colour[1] & 0x1F;
        if (dword_F4387C) {
            textColour = COLOUR_GREY;
        }
        textColour = (textColour << 19) | IMAGE_TYPE_REMAP;
        uint32 bannerIndex = (mapElement->type & 0xC0) | ((mapElement->properties.scenerymultiple.colour[0] & 0xE0) >> 2) | ((mapElement->properties.scenerymultiple.colour[1] & 0xE0) >> 5);
        rct_banner *banner = &gBanners[bannerIndex];
        rct_string_id stringId = banner->string_idx;
        if (banner->flags & BANNER_FLAG_LINKED_TO_RIDE) {
            rct_ride * ride = get_ride(banner->colour);
            stringId = ride->name;
            set_format_arg(0, uint32, ride->name_arguments);
        }
        utf8 signString[256];
        format_string(signString, sizeof(signString), stringId, gCommonFormatArgs);
        rct_large_scenery_text *text = entry->large_scenery.text;
        sint32 y_offset = (text->offset[(direction & 1)].y * 2);
        if (text->flags & LARGE_SCENERY_TEXT_FLAG_VERTICAL) {
            // Draw vertical sign:
            y_offset += 1;
            utf8 fitStr[32];
            const utf8 *fitStrPtr = fitStr;
            safe_strcpy(fitStr, scenery_multiple_sign_fit_text(signString, text, true), sizeof(fitStr));
            sint32 height2 = scenery_multiple_sign_text_height(fitStr, text);
            uint32 codepoint;
            while ((codepoint = utf8_get_next(fitStrPtr, &fitStrPtr)) != 0) {
                utf8 str[5] = {0};
                utf8_write_codepoint(str, codepoint);
                scenery_multiple_sign_paint_line(str, entry->large_scenery.text, entry->large_scenery.text_image, textColour, direction, y_offset - height2);
                y_offset += scenery_multiple_sign_get_glyph(text, codepoint)->height * 2;
            }
        } else {
            y_offset -= (direction & 1);
            if (text->flags & LARGE_SCENERY_TEXT_FLAG_TWO_LINE) {
                // Draw two-line sign:
                sint32 width = scenery_multiple_sign_text_width(signString, text);
                if (width > text->max_width) {
                    y_offset -= scenery_multiple_sign_get_glyph(text, 'A')->height + 1;
                    utf8 *src = signString;
                    for (sint32 i = 0; i < 2; i++) {
                        utf8 str1[64] = {0};
                        utf8 *dst = str1;
                        utf8 *srcold = src;
                        utf8 *spacesrc = 0;
                        utf8 *spacedst = 0;
                        sint32 w = 0;
                        uint32 codepoint = utf8_get_next(src, (const utf8**)&src);
                        do {
                            w += scenery_multiple_sign_get_glyph(text, codepoint)->width;
                            if (codepoint == ' ') {
                                spacesrc = src;
                                spacedst = dst;
                            }
                        } while(w <= text->max_width && (dst = utf8_write_codepoint(dst, codepoint)) != NULL && (srcold = src) != NULL && (codepoint = utf8_get_next(src, (const utf8**)&src)) != '\0');
                        src = srcold;
                        if (spacesrc && codepoint) {
                            *spacedst = 0;
                            src = spacesrc;
                        }
                        scenery_multiple_sign_paint_line(str1, entry->large_scenery.text, entry->large_scenery.text_image, textColour, direction, y_offset);
                        y_offset += (scenery_multiple_sign_get_glyph(text, 'A')->height + 1) * 2;
                    }
                } else {
                    scenery_multiple_sign_paint_line(signString, entry->large_scenery.text, entry->large_scenery.text_image, textColour, direction, y_offset);
                }
            } else {
                // Draw one-line sign:
                scenery_multiple_sign_paint_line(signString, entry->large_scenery.text, entry->large_scenery.text_image, textColour, direction, y_offset);
            }
        }
        return;
    }
    rct_drawpixelinfo* dpi = session->Unk140E9A8;
    if (dpi->zoom_level > 0) {
        scenery_multiple_paint_supports(direction, height, mapElement, dword_F4387C, tile);
        return;
    }
    uint8 al = ((mapElement->properties.surface.terrain >> 2) - 1) & 3;
    if (al != direction) {
        scenery_multiple_paint_supports(direction, height, mapElement, dword_F4387C, tile);
        return;
    }
    // Draw scrolling text:
    set_format_arg(0, uint32, 0);
    set_format_arg(4, uint32, 0);
    uint8 textColour = mapElement->properties.banner.unused & 0x1F;
    if (dword_F4387C) {
        textColour = COLOUR_GREY;
    }
    if (direction == 3) {
        textColour |= (1 << 7);
    }
    // 6B809A:
    set_format_arg(7, uint8, textColour);
    uint32 bannerIndex = (mapElement->type & 0xC0) | ((mapElement->properties.scenerymultiple.colour[0] & 0xE0) >> 2) | ((mapElement->properties.scenerymultiple.colour[1] & 0xE0) >> 5);
    uint16 scrollMode = entry->large_scenery.scrolling_mode + ((direction + 1) & 0x3);
    rct_banner *banner = &gBanners[bannerIndex];
    set_format_arg(0, rct_string_id, banner->string_idx);
    if (banner->flags & BANNER_FLAG_LINKED_TO_RIDE) {
        rct_ride * ride = get_ride(banner->colour);
        set_format_arg(0, rct_string_id, ride->name);
        set_format_arg(2, uint32, ride->name_arguments);
    }
    utf8 signString[256];
    rct_string_id stringId = STR_SCROLLING_SIGN_TEXT;
    if (gConfigGeneral.upper_case_banners) {
        format_string_to_upper(signString, sizeof(signString), stringId, gCommonFormatArgs);
    } else {
        format_string(signString, sizeof(signString), stringId, gCommonFormatArgs);
    }

    gCurrentFontSpriteBase = FONT_SPRITE_BASE_TINY;

    uint16 string_width = gfx_get_string_width(signString);
    uint16 scroll = (gCurrentTicks / 2) % string_width;
    sub_98199C(scrolling_text_setup(stringId, scroll, scrollMode), 0, 0, 1, 1, 21, height + 25, boxoffset.x, boxoffset.y, boxoffset.z, get_current_rotation());

    scenery_multiple_paint_supports(direction, height, mapElement, dword_F4387C, tile);
}
