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

#include "../../config/Config.h"
#include "../../core/Util.hpp"
#include "../../Game.h"
#include "../../interface/Viewport.h"
#include "../../localisation/Localisation.h"
#include "../../ride/Ride.h"
#include "../../ride/TrackDesign.h"
#include "../../util/Util.h"
#include "../../world/Banner.h"
#include "../../world/Map.h"
#include "../../world/LargeScenery.h"
#include "../../world/Scenery.h"
#include "../Paint.h"
#include "../Supports.h"
#include "TileElement.h"

// 6B8172:
static void large_scenery_paint_supports(
    paint_session *          session,
    uint8                    direction,
    uint16                   height,
    const rct_tile_element * tileElement,
    uint32                   dword_F4387C,
    rct_large_scenery_tile * tile)
{
    if (tile->flags & LARGE_SCENERY_TILE_FLAG_NO_SUPPORTS) {
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

    wooden_b_supports_paint_setup(session, (direction & 1), ax, supportHeight, supportImageColourFlags, nullptr);

    sint32 clearanceHeight = ceil2(tileElement->clearance_height * 8 + 15, 16);

    if (tile->flags & LARGE_SCENERY_TILE_FLAG_ALLOW_SUPPORTS_ABOVE) {
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, clearanceHeight, 0x20);
    } else {
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    }

    paint_util_set_general_support_height(session, clearanceHeight, 0x20);
}

static rct_large_scenery_text_glyph *large_scenery_sign_get_glyph(rct_large_scenery_text *text, uint32 codepoint)
{
    if (codepoint >= Util::CountOf(text->glyphs)) {
        return &text->glyphs[(size_t)'?'];
    }
    return &text->glyphs[codepoint];
}

static sint32 large_scenery_sign_text_width(const utf8 *str, rct_large_scenery_text *text)
{
    sint32 width = 0;
    uint32 codepoint;
    while ((codepoint = utf8_get_next(str, &str)) != 0) {
        width += large_scenery_sign_get_glyph(text, codepoint)->width;
    }
    return width;
}

static sint32 large_scenery_sign_text_height(const utf8 *str, rct_large_scenery_text *text)
{
    sint32 height = 0;
    uint32 codepoint;
    while ((codepoint = utf8_get_next(str, &str)) != 0) {
        height += large_scenery_sign_get_glyph(text, codepoint)->height;
    }
    return height;
}

static const utf8 *large_scenery_sign_fit_text(const utf8 *str, rct_large_scenery_text *text, bool height)
{
    static utf8 fitStr[32];
    utf8 *fitStrEnd = fitStr;
    safe_strcpy(fitStr, str, sizeof(fitStr));
    sint32 w = 0;
    uint32 codepoint;
    while (w <= text->max_width && (codepoint = utf8_get_next(fitStrEnd, (const utf8**)&fitStrEnd)) != 0) {
        if (height) {
            w += large_scenery_sign_get_glyph(text, codepoint)->height;
        } else {
            w += large_scenery_sign_get_glyph(text, codepoint)->width;
        }
    }
    *fitStrEnd = 0;
    return fitStr;
}

static sint32 div_to_minus_infinity(sint32 a, sint32 b) {
    return (a / b) - (a % b < 0);
}

static void large_scenery_sign_paint_line(paint_session * session, const utf8 *str, rct_large_scenery_text *text, sint32 textImage, sint32 textColour, uint8 direction, sint32 y_offset)
{
    const utf8 *fitStr = large_scenery_sign_fit_text(str, text, false);
    sint32 width = large_scenery_sign_text_width(fitStr, text);
    sint32 x_offset = text->offset[(direction & 1)].x;
    sint32 acc = y_offset * ((direction & 1) ? -1 : 1);
    if (!(text->flags & LARGE_SCENERY_TEXT_FLAG_VERTICAL)) {
        // sign is horizontal, centre text:
        x_offset -= (width / 2);
        acc -= (width / 2);
    }
    uint32 codepoint;
    while ((codepoint = utf8_get_next(fitStr, &fitStr)) != 0) {
        sint32 glyph_offset = large_scenery_sign_get_glyph(text, codepoint)->image_offset;
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
            paint_attach_to_previous_ps(session, image_id, x_offset, -div_to_minus_infinity(acc, 2));
        } else {
            if (text->flags & LARGE_SCENERY_TEXT_FLAG_VERTICAL) {
                paint_attach_to_previous_ps(session, image_id, x_offset, div_to_minus_infinity(acc, 2));
            } else {
                paint_attach_to_previous_attach(session, image_id, x_offset, div_to_minus_infinity(acc, 2));
            }
        }
        x_offset += large_scenery_sign_get_glyph(text, codepoint)->width;
        acc += large_scenery_sign_get_glyph(text, codepoint)->width;
    }
}

struct boundbox {
    LocationXY16 offset;
    LocationXY16 length;
};

static constexpr const boundbox s98E3C4[] = {
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
void large_scenery_paint(paint_session * session, uint8 direction, uint16 height, const rct_tile_element * tileElement)
{
    if (gCurrentViewportFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES)
    {
        return;
    }
    session->InteractionType = VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY;
    uint32 sequenceNum = scenery_large_get_sequence(tileElement);
    rct_scenery_entry *entry = get_large_scenery_entry(scenery_large_get_type(tileElement));
    if (entry == nullptr)
        return;

    uint32 image_id = (sequenceNum << 2) + entry->image + 4 + direction;
    rct_large_scenery_tile *tile = &entry->large_scenery.tiles[sequenceNum];
    uint32 dword_F4387C = 0;
    image_id |= SPRITE_ID_PALETTE_COLOUR_2(scenery_large_get_primary_colour(tileElement), scenery_large_get_secondary_colour(tileElement));
    LocationXYZ16 boxlength;
    LocationXYZ16 boxoffset;
    if (gTrackDesignSaveMode) {
        if (!track_design_save_contains_tile_element(tileElement)) {
            sequenceNum = SPRITE_ID_PALETTE_COLOUR_1(PALETTE_46);
            image_id &= 0x7FFFF;
            dword_F4387C = sequenceNum;
            image_id |= dword_F4387C;
        }
    }
    if (tileElement->flags & TILE_ELEMENT_FLAG_GHOST) {
        session->InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;
        sequenceNum = CONSTRUCTION_MARKER;
        image_id &= 0x7FFFF;
        dword_F4387C = sequenceNum;
        image_id |= dword_F4387C;
    }
    sint32 ah = tile->z_clearance;
    if (ah > 0x80) {
        ah = 0x80;
    }
    ah -= 3;
    uint16 edi = tile->flags;
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
    sub_98197C(session, image_id, 0, 0, boxlength.x, boxlength.y, ah, height, boxoffset.x, boxoffset.y, boxoffset.z);
    if (entry->large_scenery.scrolling_mode == 0xFF || direction == 1 || direction == 2) {
        large_scenery_paint_supports(session, direction, height, tileElement, dword_F4387C, tile);
        return;
    }
    if (entry->large_scenery.flags & LARGE_SCENERY_FLAG_3D_TEXT) {
        if (entry->large_scenery.tiles[1].x_offset != (sint16)(uint16)0xFFFF) {
            sint32 sequenceDirection = (scenery_large_get_sequence(tileElement) - 1) & 3;
            if (sequenceDirection != direction) {
                large_scenery_paint_supports(session, direction, height, tileElement, dword_F4387C, tile);
                return;
            }
        }
        rct_drawpixelinfo* dpi = session->Unk140E9A8;
        if (dpi->zoom_level > 1) {
            large_scenery_paint_supports(session, direction, height, tileElement, dword_F4387C, tile);
            return;
        }
        // 6B8331:
        // Draw sign text:
        set_format_arg(0, uint32, 0);
        set_format_arg(4, uint32, 0);
        sint32 textColour = scenery_large_get_secondary_colour(tileElement);
        if (dword_F4387C) {
            textColour = COLOUR_GREY;
        }
        textColour = (textColour << 19) | IMAGE_TYPE_REMAP;
        uint32 bannerIndex = scenery_large_get_banner_id(tileElement);
        rct_banner *banner = &gBanners[bannerIndex];
        rct_string_id stringId = banner->string_idx;
        if (banner->flags & BANNER_FLAG_LINKED_TO_RIDE) {
            Ride * ride = get_ride(banner->colour);
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
            safe_strcpy(fitStr, large_scenery_sign_fit_text(signString, text, true), sizeof(fitStr));
            sint32 height2 = large_scenery_sign_text_height(fitStr, text);
            uint32 codepoint;
            while ((codepoint = utf8_get_next(fitStrPtr, &fitStrPtr)) != 0) {
                utf8 str[5] = {0};
                utf8_write_codepoint(str, codepoint);
                large_scenery_sign_paint_line(session, str, entry->large_scenery.text, entry->large_scenery.text_image, textColour, direction, y_offset - height2);
                y_offset += large_scenery_sign_get_glyph(text, codepoint)->height * 2;
            }
        } else {
            y_offset -= (direction & 1);
            if (text->flags & LARGE_SCENERY_TEXT_FLAG_TWO_LINE) {
                // Draw two-line sign:
                sint32 width = large_scenery_sign_text_width(signString, text);
                if (width > text->max_width) {
                    y_offset -= large_scenery_sign_get_glyph(text, 'A')->height + 1;
                    utf8 *src = signString;
                    for (sint32 i = 0; i < 2; i++) {
                        utf8 str1[64] = {0};
                        utf8 *dst = str1;
                        utf8 *srcold = src;
                        utf8 *spacesrc = nullptr;
                        utf8 *spacedst = nullptr;
                        sint32 w = 0;
                        uint32 codepoint = utf8_get_next(src, (const utf8**)&src);
                        do {
                            w += large_scenery_sign_get_glyph(text, codepoint)->width;
                            if (codepoint == ' ') {
                                spacesrc = src;
                                spacedst = dst;
                            }
                        } while(w <= text->max_width && (dst = utf8_write_codepoint(dst, codepoint)) != nullptr && (srcold = src) != nullptr && (codepoint = utf8_get_next(src, (const utf8**)&src)) != '\0');
                        src = srcold;
                        if (spacesrc && codepoint) {
                            *spacedst = 0;
                            src = spacesrc;
                        }
                        large_scenery_sign_paint_line(session, str1, entry->large_scenery.text, entry->large_scenery.text_image, textColour, direction, y_offset);
                        y_offset += (large_scenery_sign_get_glyph(text, 'A')->height + 1) * 2;
                    }
                } else {
                    large_scenery_sign_paint_line(session, signString, entry->large_scenery.text, entry->large_scenery.text_image, textColour, direction, y_offset);
                }
            } else {
                // Draw one-line sign:
                large_scenery_sign_paint_line(session, signString, entry->large_scenery.text, entry->large_scenery.text_image, textColour, direction, y_offset);
            }
        }
        return;
    }
    rct_drawpixelinfo* dpi = session->Unk140E9A8;
    if (dpi->zoom_level > 0) {
        large_scenery_paint_supports(session, direction, height, tileElement, dword_F4387C, tile);
        return;
    }
    uint8 sequenceDirection2 = (scenery_large_get_sequence(tileElement) - 1) & 3;
    if (sequenceDirection2 != direction) {
        large_scenery_paint_supports(session, direction, height, tileElement, dword_F4387C, tile);
        return;
    }
    // Draw scrolling text:
    set_format_arg(0, uint32, 0);
    set_format_arg(4, uint32, 0);
    uint8 textColour = scenery_large_get_secondary_colour(tileElement);
    if (dword_F4387C) {
        textColour = COLOUR_GREY;
    }
    if (direction == 3) {
        textColour |= (1 << 7);
    }
    // 6B809A:
    set_format_arg(7, uint8, textColour);
    uint32 bannerIndex = scenery_large_get_banner_id(tileElement);
    uint16 scrollMode = entry->large_scenery.scrolling_mode + ((direction + 1) & 0x3);
    rct_banner *banner = &gBanners[bannerIndex];
    set_format_arg(0, rct_string_id, banner->string_idx);
    if (banner->flags & BANNER_FLAG_LINKED_TO_RIDE) {
        Ride * ride = get_ride(banner->colour);
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
    sub_98199C(
        session, scrolling_text_setup(session, stringId, scroll, scrollMode), 0, 0, 1, 1, 21, height + 25, boxoffset.x,
        boxoffset.y, boxoffset.z);

    large_scenery_paint_supports(session, direction, height, tileElement, dword_F4387C, tile);
}
