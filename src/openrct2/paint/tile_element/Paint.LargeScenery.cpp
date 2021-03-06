/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../Game.h"
#include "../../config/Config.h"
#include "../../interface/Viewport.h"
#include "../../localisation/Localisation.h"
#include "../../object/LargeSceneryObject.h"
#include "../../ride/Ride.h"
#include "../../ride/TrackDesign.h"
#include "../../util/Util.h"
#include "../../world/Banner.h"
#include "../../world/LargeScenery.h"
#include "../../world/Map.h"
#include "../../world/Scenery.h"
#include "../Paint.h"
#include "../Supports.h"
#include "Paint.TileElement.h"

#include <iterator>

// 6B8172:
static void large_scenery_paint_supports(
    paint_session* session, uint8_t direction, uint16_t height, const TileElement* tileElement, uint32_t dword_F4387C,
    const rct_large_scenery_tile* tile)
{
    if (tile->flags & LARGE_SCENERY_TILE_FLAG_NO_SUPPORTS)
    {
        return;
    }

    int32_t ax = 0;
    int32_t supportHeight = height;

    if (supportHeight & 0xF)
    {
        supportHeight &= 0xFFFFFFF0;
        ax = 49;
    }

    int32_t supportImageColourFlags = IMAGE_TYPE_REMAP;

    if (dword_F4387C)
    {
        supportImageColourFlags = dword_F4387C;
    }

    wooden_b_supports_paint_setup(session, (direction & 1), ax, supportHeight, supportImageColourFlags, nullptr);

    int32_t clearanceHeight = ceil2(tileElement->GetClearanceZ() + 15, 16);

    if (tile->flags & LARGE_SCENERY_TILE_FLAG_ALLOW_SUPPORTS_ABOVE)
    {
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, clearanceHeight, 0x20);
    }
    else
    {
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    }

    paint_util_set_general_support_height(session, clearanceHeight, 0x20);
}

static rct_large_scenery_text_glyph* large_scenery_sign_get_glyph(rct_large_scenery_text* text, uint32_t codepoint)
{
    if (codepoint >= std::size(text->glyphs))
    {
        return &text->glyphs[static_cast<size_t>('?')];
    }
    return &text->glyphs[codepoint];
}

static int32_t large_scenery_sign_text_width(const utf8* str, rct_large_scenery_text* text)
{
    int32_t width = 0;
    uint32_t codepoint;
    while ((codepoint = utf8_get_next(str, &str)) != 0)
    {
        width += large_scenery_sign_get_glyph(text, codepoint)->width;
    }
    return width;
}

static int32_t large_scenery_sign_text_height(const utf8* str, rct_large_scenery_text* text)
{
    int32_t height = 0;
    uint32_t codepoint;
    while ((codepoint = utf8_get_next(str, &str)) != 0)
    {
        height += large_scenery_sign_get_glyph(text, codepoint)->height;
    }
    return height;
}

static void large_scenery_sign_fit_text(const utf8* str, rct_large_scenery_text* text, bool height, utf8* fitStr, size_t bufLen)
{
    utf8* fitStrEnd = fitStr;
    safe_strcpy(fitStr, str, bufLen);
    int32_t w = 0;
    uint32_t codepoint;
    while (w <= text->max_width && (codepoint = utf8_get_next(fitStrEnd, const_cast<const utf8**>(&fitStrEnd))) != 0)
    {
        if (height)
        {
            w += large_scenery_sign_get_glyph(text, codepoint)->height;
        }
        else
        {
            w += large_scenery_sign_get_glyph(text, codepoint)->width;
        }
    }
    *fitStrEnd = 0;
}

static int32_t div_to_minus_infinity(int32_t a, int32_t b)
{
    return (a / b) - (a % b < 0);
}

static void large_scenery_sign_paint_line(
    paint_session* session, const utf8* str, rct_large_scenery_text* text, int32_t textImage, int32_t textColour,
    uint8_t direction, int32_t y_offset)
{
    utf8 fitStr[32];
    large_scenery_sign_fit_text(str, text, false, fitStr, sizeof(fitStr));
    int32_t width = large_scenery_sign_text_width(fitStr, text);
    int32_t x_offset = text->offset[(direction & 1)].x;
    int32_t acc = y_offset * ((direction & 1) ? -1 : 1);
    if (!(text->flags & LARGE_SCENERY_TEXT_FLAG_VERTICAL))
    {
        // sign is horizontal, centre text:
        x_offset -= (width / 2);
        acc -= (width / 2);
    }
    uint32_t codepoint;
    const utf8* fitStrPtr = fitStr;
    while ((codepoint = utf8_get_next(fitStrPtr, &fitStrPtr)) != 0)
    {
        int32_t glyph_offset = large_scenery_sign_get_glyph(text, codepoint)->image_offset;
        uint8_t glyph_type = direction & 1;
        if (text->flags & LARGE_SCENERY_TEXT_FLAG_VERTICAL)
        { // vertical sign
            glyph_offset *= 2;
        }
        else
        {
            glyph_offset *= 4;
            // set slightly different glyph on horizontal sign, which was rendered 1/2 pixel lower to deal with aliasing:
            if (direction & 1)
            {
                if (!(acc & 1))
                {
                    glyph_type += 2;
                }
            }
            else
            {
                if ((acc & 1))
                {
                    glyph_type += 2;
                }
            }
        }
        int32_t image_id = (textImage + glyph_offset + glyph_type) | textColour;
        if (direction == 3)
        {
            PaintAttachToPreviousPS(session, image_id, x_offset, -div_to_minus_infinity(acc, 2));
        }
        else
        {
            if (text->flags & LARGE_SCENERY_TEXT_FLAG_VERTICAL)
            {
                PaintAttachToPreviousPS(session, image_id, x_offset, div_to_minus_infinity(acc, 2));
            }
            else
            {
                PaintAttachToPreviousAttach(session, image_id, x_offset, div_to_minus_infinity(acc, 2));
            }
        }
        x_offset += large_scenery_sign_get_glyph(text, codepoint)->width;
        acc += large_scenery_sign_get_glyph(text, codepoint)->width;
    }
}

struct boundbox
{
    LocationXY16 offset;
    LocationXY16 length;
};

// clang-format off
static constexpr const boundbox s98E3C4[] = {
    { 3, 3, 26, 26 },
    { 17, 17, 12, 12 },
    { 17, 3, 12, 12 },
    { 17, 3, 12, 26 },
    { 3, 3, 12, 12 },
    { 3, 3, 26, 26 },
    { 3, 3, 28, 12 },
    { 3, 3, 26, 26 },
    { 3, 17, 12, 12 },
    { 3, 17, 26, 12 },
    { 3, 3, 26, 26 },
    { 3, 3, 26, 26 },
    { 3, 3, 12, 28 },
    { 3, 3, 26, 26 },
    { 3, 3, 26, 26 },
    { 3, 3, 26, 26 },
    { 1, 1, 30, 30 },
};
// clang-format on

/*
 *
 * rct2: 0x006B7F0C
 */
void large_scenery_paint(paint_session* session, uint8_t direction, uint16_t height, const TileElement* tileElement)
{
    if (session->ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES)
    {
        return;
    }
    session->InteractionType = ViewportInteractionItem::LargeScenery;
    uint32_t sequenceNum = tileElement->AsLargeScenery()->GetSequenceIndex();
    const LargeSceneryObject* object = tileElement->AsLargeScenery()->GetObject();
    if (object == nullptr)
        return;

    const rct_scenery_entry* entry = tileElement->AsLargeScenery()->GetEntry();
    if (entry == nullptr)
        return;

    uint32_t image_id = (sequenceNum << 2) + entry->image + 4 + direction;
    const rct_large_scenery_tile* tile = object->GetTileForSequence(sequenceNum);
    if (tile == nullptr)
        return;

    uint32_t dword_F4387C = 0;
    image_id |= SPRITE_ID_PALETTE_COLOUR_2(
        tileElement->AsLargeScenery()->GetPrimaryColour(), tileElement->AsLargeScenery()->GetSecondaryColour());
    LocationXYZ16 boxlength;
    LocationXYZ16 boxoffset;
    if (gTrackDesignSaveMode)
    {
        if (!track_design_save_contains_tile_element(tileElement))
        {
            sequenceNum = SPRITE_ID_PALETTE_COLOUR_1(EnumValue(FilterPaletteID::Palette46));
            image_id &= 0x7FFFF;
            dword_F4387C = sequenceNum;
            image_id |= dword_F4387C;
        }
    }
    if (tileElement->IsGhost())
    {
        session->InteractionType = ViewportInteractionItem::None;
        sequenceNum = CONSTRUCTION_MARKER;
        image_id &= 0x7FFFF;
        dword_F4387C = sequenceNum;
        image_id |= dword_F4387C;
    }
    int32_t ah = tile->z_clearance;
    if (ah > 0x80)
    {
        ah = 0x80;
    }
    ah -= 3;
    uint16_t edi = tile->flags;
    int32_t esi = 16;
    if (edi & 0xF00)
    {
        edi &= 0xF000;
        edi = rol16(edi, direction);
        esi = (edi & 0xF) | (edi >> 12);
    }
    boxoffset.x = s98E3C4[esi].offset.x;
    boxoffset.y = s98E3C4[esi].offset.y;
    boxoffset.z = height;
    boxlength.x = s98E3C4[esi].length.x;
    boxlength.y = s98E3C4[esi].length.y;
    boxlength.z = ah;
    PaintAddImageAsParent(session, image_id, 0, 0, boxlength.x, boxlength.y, ah, height, boxoffset.x, boxoffset.y, boxoffset.z);
    if (entry->large_scenery.scrolling_mode == SCROLLING_MODE_NONE || direction == 1 || direction == 2)
    {
        large_scenery_paint_supports(session, direction, height, tileElement, dword_F4387C, tile);
        return;
    }
    if (entry->large_scenery.flags & LARGE_SCENERY_FLAG_3D_TEXT)
    {
        if (entry->large_scenery.tiles[1].x_offset != static_cast<int16_t>(static_cast<uint16_t>(0xFFFF)))
        {
            int32_t sequenceDirection = (tileElement->AsLargeScenery()->GetSequenceIndex() - 1) & 3;
            if (sequenceDirection != direction)
            {
                large_scenery_paint_supports(session, direction, height, tileElement, dword_F4387C, tile);
                return;
            }
        }
        rct_drawpixelinfo* dpi = &session->DPI;
        if (dpi->zoom_level > 1)
        {
            large_scenery_paint_supports(session, direction, height, tileElement, dword_F4387C, tile);
            return;
        }
        // 6B8331:
        // Draw sign text:
        int32_t textColour = tileElement->AsLargeScenery()->GetSecondaryColour();
        if (dword_F4387C)
        {
            textColour = COLOUR_GREY;
        }
        textColour = (textColour << 19) | IMAGE_TYPE_REMAP;
        auto banner = tileElement->AsLargeScenery()->GetBanner();
        if (banner != nullptr)
        {
            auto ft = Formatter();
            banner->FormatTextTo(ft);
            utf8 signString[256];
            format_string(signString, sizeof(signString), STR_STRINGID, ft.Data());
            rct_large_scenery_text* text = entry->large_scenery.text;
            int32_t y_offset = (text->offset[(direction & 1)].y * 2);
            if (text->flags & LARGE_SCENERY_TEXT_FLAG_VERTICAL)
            {
                // Draw vertical sign:
                y_offset += 1;
                utf8 fitStr[32];
                large_scenery_sign_fit_text(signString, text, true, fitStr, sizeof(fitStr));
                safe_strcpy(fitStr, fitStr, sizeof(fitStr));
                const utf8* fitStrPtr = fitStr;
                int32_t height2 = large_scenery_sign_text_height(fitStr, text);
                uint32_t codepoint;
                while ((codepoint = utf8_get_next(fitStrPtr, &fitStrPtr)) != 0)
                {
                    utf8 str[5] = { 0 };
                    utf8_write_codepoint(str, codepoint);
                    large_scenery_sign_paint_line(
                        session, str, entry->large_scenery.text, entry->large_scenery.text_image, textColour, direction,
                        y_offset - height2);
                    y_offset += large_scenery_sign_get_glyph(text, codepoint)->height * 2;
                }
            }
            else
            {
                y_offset -= (direction & 1);
                if (text->flags & LARGE_SCENERY_TEXT_FLAG_TWO_LINE)
                {
                    // Draw two-line sign:
                    int32_t width = large_scenery_sign_text_width(signString, text);
                    if (width > text->max_width)
                    {
                        y_offset -= large_scenery_sign_get_glyph(text, 'A')->height + 1;
                        utf8* src = signString;
                        for (int32_t i = 0; i < 2; i++)
                        {
                            utf8 str1[64] = { 0 };
                            utf8* dst = str1;
                            utf8* srcold = src;
                            utf8* spacesrc = nullptr;
                            utf8* spacedst = nullptr;
                            int32_t w = 0;
                            uint32_t codepoint = utf8_get_next(src, const_cast<const utf8**>(&src));
                            do
                            {
                                w += large_scenery_sign_get_glyph(text, codepoint)->width;
                                if (codepoint == ' ')
                                {
                                    spacesrc = src;
                                    spacedst = dst;
                                }
                            } while (w <= text->max_width && (dst = utf8_write_codepoint(dst, codepoint)) != nullptr
                                     && (srcold = src) != nullptr
                                     && (codepoint = utf8_get_next(src, const_cast<const utf8**>(&src))) != '\0');
                            src = srcold;
                            if (spacesrc && codepoint)
                            {
                                *spacedst = 0;
                                src = spacesrc;
                            }
                            large_scenery_sign_paint_line(
                                session, str1, entry->large_scenery.text, entry->large_scenery.text_image, textColour,
                                direction, y_offset);
                            y_offset += (large_scenery_sign_get_glyph(text, 'A')->height + 1) * 2;
                        }
                    }
                    else
                    {
                        large_scenery_sign_paint_line(
                            session, signString, entry->large_scenery.text, entry->large_scenery.text_image, textColour,
                            direction, y_offset);
                    }
                }
                else
                {
                    // Draw one-line sign:
                    large_scenery_sign_paint_line(
                        session, signString, entry->large_scenery.text, entry->large_scenery.text_image, textColour, direction,
                        y_offset);
                }
            }
        }
        return;
    }
    rct_drawpixelinfo* dpi = &session->DPI;
    if (dpi->zoom_level > 0)
    {
        large_scenery_paint_supports(session, direction, height, tileElement, dword_F4387C, tile);
        return;
    }
    uint8_t sequenceDirection2 = (tileElement->AsLargeScenery()->GetSequenceIndex() - 1) & 3;
    if (sequenceDirection2 != direction)
    {
        large_scenery_paint_supports(session, direction, height, tileElement, dword_F4387C, tile);
        return;
    }
    // Draw scrolling text:
    uint8_t textColour = tileElement->AsLargeScenery()->GetSecondaryColour();
    if (dword_F4387C)
    {
        textColour = COLOUR_GREY;
    }
    if (direction == 0)
    {
        textColour = ColourMapA[textColour].mid_dark;
    }
    else
    {
        textColour = ColourMapA[textColour].light;
    }
    // 6B809A:
    uint16_t scrollMode = entry->large_scenery.scrolling_mode + ((direction + 1) & 0x3);
    auto banner = tileElement->AsLargeScenery()->GetBanner();
    if (banner != nullptr)
    {
        auto ft = Formatter();
        banner->FormatTextTo(ft);
        utf8 signString[256];
        if (gConfigGeneral.upper_case_banners)
        {
            format_string_to_upper(signString, sizeof(signString), STR_SCROLLING_SIGN_TEXT, ft.Data());
        }
        else
        {
            format_string(signString, sizeof(signString), STR_SCROLLING_SIGN_TEXT, ft.Data());
        }

        uint16_t stringWidth = gfx_get_string_width(signString, FontSpriteBase::TINY);
        uint16_t scroll = stringWidth > 0 ? (gCurrentTicks / 2) % stringWidth : 0;
        PaintAddImageAsChild(
            session, scrolling_text_setup(session, STR_SCROLLING_SIGN_TEXT, ft, scroll, scrollMode, textColour), 0, 0, 1, 1, 21,
            height + 25, boxoffset.x, boxoffset.y, boxoffset.z);
    }

    large_scenery_paint_supports(session, direction, height, tileElement, dword_F4387C, tile);
}
