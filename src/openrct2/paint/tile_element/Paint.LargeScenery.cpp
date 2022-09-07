/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Paint.h"

#include "../../Game.h"
#include "../../config/Config.h"
#include "../../core/Numerics.hpp"
#include "../../core/String.hpp"
#include "../../interface/Viewport.h"
#include "../../localisation/Formatter.h"
#include "../../localisation/Localisation.h"
#include "../../object/LargeSceneryObject.h"
#include "../../profiling/Profiling.h"
#include "../../ride/Ride.h"
#include "../../ride/TrackDesign.h"
#include "../../util/Util.h"
#include "../../world/Banner.h"
#include "../../world/LargeScenery.h"
#include "../../world/Map.h"
#include "../../world/Scenery.h"
#include "../../world/TileInspector.h"
#include "../Boundbox.h"
#include "../Supports.h"
#include "Paint.TileElement.h"

// clang-format off
static constexpr const BoundBoxXY LargeSceneryBoundBoxes[] = {
    { { 3, 3 }, { 26, 26 } },
    { { 17, 17 }, { 12, 12 } },
    { { 17, 3 }, { 12, 12 } },
    { { 17, 3 }, { 12, 26 } },
    { { 3, 3 }, { 12, 12 } },
    { { 3, 3 }, { 26, 26 } },
    { { 3, 3 }, { 28, 12 } },
    { { 3, 3 }, { 26, 26 } },
    { { 3, 17 }, { 12, 12 } },
    { { 3, 17 }, { 26, 12 } },
    { { 3, 3 }, { 26, 26 } },
    { { 3, 3 }, { 26, 26 } },
    { { 3, 3 }, { 12, 28 } },
    { { 3, 3 }, { 26, 26 } },
    { { 3, 3 }, { 26, 26 } },
    { { 3, 3 }, { 26, 26 } },
    { { 1, 1 }, { 30, 30 } },
};
// clang-format on

static void PaintLargeScenerySupports(
    paint_session& session, uint8_t direction, uint16_t height, const LargeSceneryElement& tileElement, ImageId imageTemplate,
    const rct_large_scenery_tile& tile)
{
    PROFILED_FUNCTION();

    if (tile.flags & LARGE_SCENERY_TILE_FLAG_NO_SUPPORTS)
        return;

    auto special = 0;
    auto supportHeight = height;
    if (supportHeight & 0xF)
    {
        supportHeight &= ~0xF;
        special = 49;
    }

    wooden_b_supports_paint_setup(session, (direction & 1), special, supportHeight, imageTemplate);

    int32_t clearanceHeight = ceil2(tileElement.GetClearanceZ() + 15, 16);
    if (tile.flags & LARGE_SCENERY_TILE_FLAG_ALLOW_SUPPORTS_ABOVE)
    {
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, clearanceHeight, 0x20);
    }
    else
    {
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    }
    paint_util_set_general_support_height(session, clearanceHeight, 0x20);
}

static std::string_view LargeSceneryCalculateDisplayText(const LargeSceneryText& text, std::string_view s, bool height)
{
    size_t totalSize = 0;
    CodepointView view(s);
    auto it = view.begin();
    while (it != view.end() && totalSize <= text.max_width)
    {
        auto glyph = text.GetGlyph(*it, ' ');
        totalSize += height ? glyph.height : glyph.width;
        it++;
    }

    auto totalLength = it.GetIndex();
    return s.substr(0, totalLength);
}

static int32_t DivToMinusInfinity(int32_t a, int32_t b)
{
    return (a / b) - (a % b < 0);
}

static void PaintLargeScenery3DTextLine(
    paint_session& session, const LargeSceneryEntry& sceneryEntry, const LargeSceneryText& text, std::string_view line,
    ImageId imageTemplate, Direction direction, int32_t offsetY)
{
    PROFILED_FUNCTION();

    line = LargeSceneryCalculateDisplayText(text, line, false);
    auto width = text.MeasureWidth(line);
    auto offsetX = text.offset[(direction & 1)].x;
    auto acc = offsetY * ((direction & 1) ? -1 : 1);
    if (!(text.flags & LARGE_SCENERY_TEXT_FLAG_VERTICAL))
    {
        // sign is horizontal, centre text:
        offsetX -= (width / 2);
        acc -= (width / 2);
    }

    for (auto codepoint : CodepointView(line))
    {
        auto glyph = text.GetGlyph(codepoint, ' ');
        // Upcasting from uint8_t to uint32_t to avoid an overflow.
        uint32_t glyphOffset = glyph.image_offset;
        auto glyphType = direction & 1;
        if (text.flags & LARGE_SCENERY_TEXT_FLAG_VERTICAL)
        {
            glyphOffset *= 2;
        }
        else
        {
            glyphOffset *= 4;
            // set slightly different glyph on horizontal sign, which was rendered 1/2 pixel lower to deal with aliasing:
            if (direction & 1)
            {
                if (!(acc & 1))
                {
                    glyphType += 2;
                }
            }
            else
            {
                if ((acc & 1))
                {
                    glyphType += 2;
                }
            }
        }

        auto imageIndex = sceneryEntry.text_image + glyphOffset + glyphType;
        auto imageId = imageTemplate.WithIndex(imageIndex);
        if (direction == 3)
        {
            PaintAttachToPreviousPS(session, imageId, offsetX, -DivToMinusInfinity(acc, 2));
        }
        else if (text.flags & LARGE_SCENERY_TEXT_FLAG_VERTICAL)
        {
            PaintAttachToPreviousPS(session, imageId, offsetX, DivToMinusInfinity(acc, 2));
        }
        else
        {
            PaintAttachToPreviousAttach(session, imageId, offsetX, DivToMinusInfinity(acc, 2));
        }
        offsetX += glyph.width;
        acc += glyph.width;
    }
}

static bool Is3DTextSingleLine(const LargeSceneryText& text, std::string_view s)
{
    if (text.flags & LARGE_SCENERY_TEXT_FLAG_TWO_LINE)
    {
        auto width = text.MeasureWidth(s);
        return width <= text.max_width;
    }
    return true;
}

static void PaintLargeScenery3DText(
    paint_session& session, const LargeSceneryEntry& sceneryEntry, const rct_large_scenery_tile& tile,
    const LargeSceneryElement& tileElement, uint8_t direction, uint16_t height, bool isGhost)
{
    PROFILED_FUNCTION();

    if (sceneryEntry.tiles[1].x_offset != -1)
    {
        auto sequenceDirection = (tileElement.GetSequenceIndex() - 1) & 3;
        if (sequenceDirection != direction)
        {
            return;
        }
    }

    if (session.DPI.zoom_level > ZoomLevel{ 1 })
        return;

    auto banner = tileElement.GetBanner();
    if (banner == nullptr)
        return;

    const auto* text = sceneryEntry.text;
    if (text == nullptr)
        return;

    auto textColour = isGhost ? static_cast<colour_t>(COLOUR_GREY) : tileElement.GetSecondaryColour();
    auto imageTemplate = ImageId().WithPrimary(textColour);

    char signString[256];
    auto ft = Formatter();
    banner->FormatTextTo(ft);
    format_string(signString, sizeof(signString), STR_STRINGID, ft.Data());

    auto offsetY = text->offset[(direction & 1)].y * 2;
    if (text->flags & LARGE_SCENERY_TEXT_FLAG_VERTICAL)
    {
        // Vertical sign
        offsetY++;
        auto displayText = LargeSceneryCalculateDisplayText(*text, signString, true);
        auto displayTextHeight = text->MeasureHeight(displayText);
        for (auto codepoint : CodepointView(displayText))
        {
            char line[8]{};
            utf8_write_codepoint(line, codepoint);
            PaintLargeScenery3DTextLine(
                session, sceneryEntry, *text, line, imageTemplate, direction, offsetY - displayTextHeight);

            auto glyph = text->GetGlyph(codepoint, ' ');
            offsetY += glyph.height * 2;
        }
    }
    else
    {
        // Horizontal sign
        offsetY -= (direction & 1);
        if (Is3DTextSingleLine(*text, signString))
        {
            PaintLargeScenery3DTextLine(session, sceneryEntry, *text, signString, imageTemplate, direction, offsetY);
        }
        else
        {
            auto lineHeight = text->GetGlyph('A')->height + 1;
            offsetY -= lineHeight;

            // Split the string into two lines at best position
            auto current = std::string_view(signString);
            std::string_view next;
            for (int32_t lineIndex = 0; lineIndex < 2; lineIndex++)
            {
                std::string_view best;

                CodepointView view(current);
                auto lineWidth = 0;
                auto it = view.begin();
                while (it != view.end() && lineWidth < text->max_width)
                {
                    // Trim any leading spaces
                    auto codepoint = *it;
                    if (codepoint != ' ' || lineWidth != 0)
                    {
                        // Determine if this is a good place to split
                        if (codepoint == ' ' || codepoint == '\n')
                        {
                            auto index = it.GetIndex();
                            best = current.substr(0, index);
                            next = current.substr(index + 1);
                            if (codepoint == '\n')
                                break;
                        }

                        auto glyph = text->GetGlyph(*it, ' ');
                        lineWidth += glyph.width;
                    }
                    it++;
                }

                if (best.empty())
                {
                    // No good split found, or reached end of string
                    auto index = it.GetIndex();
                    best = current.substr(0, index);
                    next = current.substr(index);
                }

                PaintLargeScenery3DTextLine(session, sceneryEntry, *text, best, imageTemplate, direction, offsetY);

                current = next;
                offsetY += lineHeight * 2;
            }
        }
    }
}

static void PaintLargeSceneryScrollingText(
    paint_session& session, const LargeSceneryEntry& sceneryEntry, const LargeSceneryElement& tileElement, uint8_t direction,
    uint16_t height, const CoordsXYZ& bbOffset, bool isGhost)
{
    PROFILED_FUNCTION();

    auto textColour = isGhost ? static_cast<colour_t>(COLOUR_GREY) : tileElement.GetSecondaryColour();
    auto textPaletteIndex = direction == 0 ? ColourMapA[textColour].mid_dark : ColourMapA[textColour].light;

    auto banner = tileElement.GetBanner();
    if (banner == nullptr)
        return;

    auto ft = Formatter();
    banner->FormatTextTo(ft);

    char text[256];
    if (gConfigGeneral.upper_case_banners)
    {
        format_string_to_upper(text, sizeof(text), STR_SCROLLING_SIGN_TEXT, ft.Data());
    }
    else
    {
        format_string(text, sizeof(text), STR_SCROLLING_SIGN_TEXT, ft.Data());
    }

    auto scrollMode = sceneryEntry.scrolling_mode + ((direction + 1) & 3);
    auto stringWidth = gfx_get_string_width(text, FontSpriteBase::TINY);
    auto scroll = stringWidth > 0 ? (gCurrentTicks / 2) % stringWidth : 0;
    auto imageId = scrolling_text_setup(session, STR_SCROLLING_SIGN_TEXT, ft, scroll, scrollMode, textPaletteIndex);
    PaintAddImageAsChild(session, imageId, { 0, 0, height + 25 }, { 1, 1, 21 }, bbOffset);
}

void PaintLargeScenery(paint_session& session, uint8_t direction, uint16_t height, const LargeSceneryElement& tileElement)
{
    PROFILED_FUNCTION();

    if (session.ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES)
        return;

    auto sequenceNum = tileElement.GetSequenceIndex();
    const auto* object = tileElement.GetObject();
    if (object == nullptr)
        return;

    const auto* sceneryEntry = tileElement.GetEntry();
    if (sceneryEntry == nullptr)
        return;

    const auto* tile = object->GetTileForSequence(sequenceNum);
    if (tile == nullptr)
        return;

    session.InteractionType = ViewportInteractionItem::LargeScenery;

    auto isGhost = false;
    ImageId imageTemplate;
    if (gTrackDesignSaveMode && !track_design_save_contains_tile_element(reinterpret_cast<const TileElement*>(&tileElement)))
    {
        imageTemplate = ImageId().WithRemap(FilterPaletteID::Palette46);
        isGhost = true;
    }
    else if (tileElement.IsGhost())
    {
        session.InteractionType = ViewportInteractionItem::None;
        imageTemplate = ImageId().WithRemap(FilterPaletteID::Palette44);
        isGhost = true;
    }
    else if (OpenRCT2::TileInspector::IsElementSelected(reinterpret_cast<const TileElement*>(&tileElement)))
    {
        imageTemplate = ImageId().WithRemap(FilterPaletteID::Palette44);
        isGhost = true;
    }
    else
    {
        if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR)
        {
            imageTemplate = imageTemplate.WithPrimary(tileElement.GetPrimaryColour());
        }
        if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_SECONDARY_COLOUR)
        {
            imageTemplate = imageTemplate.WithSecondary(tileElement.GetSecondaryColour());
        }
        if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_TERTIARY_COLOUR)
        {
            imageTemplate = imageTemplate.WithTertiary(tileElement.GetTertiaryColour());
        }
    }

    auto boxlengthZ = std::min<uint8_t>(tile->z_clearance, 128) - 3;
    auto flags = tile->flags;
    auto bbIndex = 16;
    if (flags & 0xF00)
    {
        flags &= 0xF000;
        flags = Numerics::rol16(flags, direction);
        bbIndex = (flags & 0xF) | (flags >> 12);
    }
    const CoordsXYZ& bbOffset = { LargeSceneryBoundBoxes[bbIndex].offset, height };
    const CoordsXYZ& bbLength = { LargeSceneryBoundBoxes[bbIndex].length, boxlengthZ };

    auto imageIndex = sceneryEntry->image + 4 + (sequenceNum << 2) + direction;
    PaintAddImageAsParent(session, imageTemplate.WithIndex(imageIndex), { 0, 0, height }, bbLength, bbOffset);

    if (sceneryEntry->scrolling_mode != SCROLLING_MODE_NONE && direction != 1 && direction != 2)
    {
        if (sceneryEntry->flags & LARGE_SCENERY_FLAG_3D_TEXT)
        {
            PaintLargeScenery3DText(session, *sceneryEntry, *tile, tileElement, direction, height, isGhost);
        }
        else if (session.DPI.zoom_level <= ZoomLevel{ 0 })
        {
            auto sequenceDirection2 = (tileElement.GetSequenceIndex() - 1) & 3;
            if (sequenceDirection2 == direction)
            {
                PaintLargeSceneryScrollingText(session, *sceneryEntry, tileElement, direction, height, bbOffset, isGhost);
            }
        }
    }
    PaintLargeScenerySupports(
        session, direction, height, tileElement, isGhost ? imageTemplate : ImageId(0, COLOUR_BLACK), *tile);
}
