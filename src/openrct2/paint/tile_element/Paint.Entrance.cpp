/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Paint.h"

#include "../../Context.h"
#include "../../Game.h"
#include "../../GameState.h"
#include "../../SpriteIds.h"
#include "../../config/Config.h"
#include "../../drawing/Drawing.h"
#include "../../drawing/LightFX.h"
#include "../../drawing/ScrollingText.h"
#include "../../interface/Viewport.h"
#include "../../localisation/Formatter.h"
#include "../../localisation/Formatting.h"
#include "../../object/EntranceObject.h"
#include "../../object/ObjectManager.h"
#include "../../object/StationObject.h"
#include "../../profiling/Profiling.h"
#include "../../ride/RideData.h"
#include "../../ride/TrackDesign.h"
#include "../../world/Entrance.h"
#include "../../world/Footpath.h"
#include "../../world/Park.h"
#include "../../world/TileInspector.h"
#include "../../world/tile_element/EntranceElement.h"
#include "../support/WoodenSupports.h"
#include "Paint.TileElement.h"
#include "Segment.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;

using OpenRCT2::Drawing::LightFx::LightType;

static void PaintRideEntranceExitScrollingText(
    PaintSession& session, const EntranceElement& entranceEl, const StationObject& stationObj, Direction direction,
    int32_t height)
{
    PROFILED_FUNCTION();

    if (stationObj.ScrollingMode == kScrollingModeNone)
        return;

    if (entranceEl.GetEntranceType() == ENTRANCE_TYPE_RIDE_EXIT)
        return;

    auto ride = GetRide(entranceEl.GetRideIndex());
    if (ride == nullptr)
        return;

    auto ft = Formatter();
    ft.Add<StringId>(STR_RIDE_ENTRANCE_NAME);
    if (ride->status == RideStatus::open && !(ride->lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN))
    {
        ride->formatNameTo(ft);
    }
    else
    {
        ft.Add<StringId>(STR_RIDE_ENTRANCE_CLOSED);
    }

    char text[256];
    if (Config::Get().general.upperCaseBanners)
    {
        FormatStringToUpper(text, sizeof(text), STR_BANNER_TEXT_FORMAT, ft.Data());
    }
    else
    {
        FormatStringLegacy(text, sizeof(text), STR_BANNER_TEXT_FORMAT, ft.Data());
    }
    auto stringWidth = GfxGetStringWidth(text, FontStyle::tiny);
    auto scroll = stringWidth > 0 ? (getGameState().currentTicks / 2) % stringWidth : 0;

    PaintAddImageAsChild(
        session,
        ScrollingText::setup(session, STR_BANNER_TEXT_FORMAT, ft, scroll, stationObj.ScrollingMode, PaletteIndex::transparent),
        { 0, 0, height + stationObj.Height }, { { 2, 2, height + stationObj.Height }, { 28, 28, 51 } });
}

static void PaintRideEntranceExitLightEffects(PaintSession& session, int32_t height, const EntranceElement& entranceEl)
{
    PROFILED_FUNCTION();

    if (LightFx::IsAvailable())
    {
        if (entranceEl.GetEntranceType() == ENTRANCE_TYPE_RIDE_ENTRANCE)
        {
            LightFx::Add3DLightMagicFromDrawingTile(session.MapPosition, 0, 0, height + 45, LightType::Lantern3);
        }

        switch (entranceEl.GetDirection())
        {
            case 0:
                LightFx::Add3DLightMagicFromDrawingTile(session.MapPosition, 16, 0, height + 16, LightType::Lantern2);
                break;
            case 1:
                LightFx::Add3DLightMagicFromDrawingTile(session.MapPosition, 0, -16, height + 16, LightType::Lantern2);
                break;
            case 2:
                LightFx::Add3DLightMagicFromDrawingTile(session.MapPosition, -16, 0, height + 16, LightType::Lantern2);
                break;
            case 3:
                LightFx::Add3DLightMagicFromDrawingTile(session.MapPosition, 0, 16, height + 16, LightType::Lantern2);
                break;
        }
    }
}

static void PaintRideEntranceExit(PaintSession& session, uint8_t direction, int32_t height, const EntranceElement& entranceEl)
{
    PROFILED_FUNCTION();

    auto rideIndex = entranceEl.GetRideIndex();
    if ((session.ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES)
        || (gTrackDesignSaveMode && rideIndex != gTrackDesignSaveRideIndex))
    {
        return;
    }

    auto ride = GetRide(rideIndex);
    if (ride == nullptr)
    {
        return;
    }

    auto stationObj = ride->getStationObject();
    if (stationObj == nullptr || stationObj->BaseImageId == kImageIndexUndefined)
    {
        return;
    }

    session.InteractionType = ViewportInteractionItem::ride;

    PaintRideEntranceExitLightEffects(session, height, entranceEl);

    auto hasGlass = (stationObj->Flags & StationObjectFlags::isTransparent) != 0;
    auto colourPrimary = ride->trackColours[0].main;
    auto imageTemplate = ImageId(0);
    ImageId glassImageTemplate;
    if (hasGlass)
    {
        glassImageTemplate = ImageId().WithTransparency(colourPrimary);
    }

    if (entranceEl.IsGhost())
    {
        session.InteractionType = ViewportInteractionItem::none;
        imageTemplate = ImageId().WithRemap(FilterPaletteID::paletteGhost);
    }
    else if (session.SelectedElement == reinterpret_cast<const TileElement*>(&entranceEl))
    {
        imageTemplate = ImageId().WithRemap(FilterPaletteID::paletteGhost);
    }
    else
    {
        if (stationObj->Flags & StationObjectFlags::hasPrimaryColour)
        {
            imageTemplate = imageTemplate.WithPrimary(colourPrimary);
        }
        if (stationObj->Flags & StationObjectFlags::hasSecondaryColour)
        {
            auto colourSecondary = ride->trackColours[0].additional;
            imageTemplate = imageTemplate.WithSecondary(colourSecondary);
        }
    }

    // Format modified to stop repeated code

    // Each entrance is split into 2 images for drawing
    // Certain entrance styles have another 2 images to draw for coloured windows

    auto isExit = entranceEl.GetEntranceType() == ENTRANCE_TYPE_RIDE_EXIT;

    // Back
    ImageIndex imageIndex = isExit ? stationObj->BaseImageId + direction + 8 : stationObj->BaseImageId + direction;
    ImageIndex glassImageIndex = isExit ? stationObj->BaseImageId + direction + 24 : stationObj->BaseImageId + direction + 16;
    PaintAddImageAsParentRotated(
        session, direction, imageTemplate.WithIndex(imageIndex), { 0, 0, height }, { { 2, 2, height }, { 28, 8, 30 } });
    if (hasGlass)
    {
        PaintAddImageAsChildRotated(
            session, direction, glassImageTemplate.WithIndex(glassImageIndex), { 0, 0, height },
            { { 2, 2, height }, { 28, 8, 30 } });
    }

    // Front
    const auto frontBoundBoxZ = isExit ? 1 : 17;
    imageIndex += 4;
    PaintAddImageAsParent(
        session, imageTemplate.WithIndex(imageIndex), { 0, 0, height }, { { 2, 2, height + 30 }, { 28, 28, frontBoundBoxZ } });
    if (hasGlass)
    {
        glassImageIndex += 4;
        PaintAddImageAsChild(
            session, glassImageTemplate.WithIndex(glassImageIndex), { 0, 0, height },
            { { 2, 2, height + 30 }, { 28, 28, frontBoundBoxZ } });
    }

    PaintUtilPushTunnelRotated(session, direction, height, TunnelType::SquareFlat);

    if (!entranceEl.IsGhost())
        PaintRideEntranceExitScrollingText(session, entranceEl, *stationObj, direction, height);

    auto supportsImageTemplate = imageTemplate;
    if (!entranceEl.IsGhost())
    {
        supportsImageTemplate = ImageId().WithPrimary(OpenRCT2::Drawing::Colour::saturatedBrown);
    }
    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::truss, WoodenSupportSubType::neSw, direction, height, supportsImageTemplate);

    height += isExit ? 40 : 56;
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height);
}

static void PaintParkEntranceScrollingText(
    PaintSession& session, const EntranceObject& entrance, Direction direction, int32_t height)
{
    PROFILED_FUNCTION();

    if ((direction + 1) & (1 << 1))
        return;

    auto scrollingMode = entrance.GetScrollingMode();
    if (scrollingMode == kScrollingModeNone)
        return;

    auto ft = Formatter();
    auto& gameState = getGameState();
    if (gameState.park.flags & PARK_FLAGS_PARK_OPEN)
    {
        const auto& park = gameState.park;
        auto name = park.name.c_str();
        ft.Add<StringId>(STR_STRING);
        ft.Add<const char*>(name);
    }
    else
    {
        ft.Add<StringId>(STR_BANNER_TEXT_CLOSED);
        ft.Add<uint32_t>(0);
    }

    char text[256];
    if (Config::Get().general.upperCaseBanners)
    {
        FormatStringToUpper(text, sizeof(text), STR_BANNER_TEXT_FORMAT, ft.Data());
    }
    else
    {
        FormatStringLegacy(text, sizeof(text), STR_BANNER_TEXT_FORMAT, ft.Data());
    }

    auto stringWidth = GfxGetStringWidth(text, FontStyle::tiny);
    auto scroll = stringWidth > 0 ? (gameState.currentTicks / 2) % stringWidth : 0;
    auto imageIndex = ScrollingText::setup(
        session, STR_BANNER_TEXT_FORMAT, ft, scroll, scrollingMode + direction / 2, PaletteIndex::transparent);
    auto textHeight = height + entrance.GetTextHeight();
    PaintAddImageAsChild(session, imageIndex, { 0, 0, textHeight }, { { 2, 2, textHeight }, { 28, 28, 47 } });
}

static void PaintParkEntranceLightEffects(PaintSession& session)
{
    PROFILED_FUNCTION();

    if (LightFx::IsAvailable())
    {
        LightFx::Add3DLightMagicFromDrawingTile(session.MapPosition, 0, 0, 155, LightType::Lantern3);
    }
}

static void PaintParkEntrance(PaintSession& session, uint8_t direction, int32_t height, const EntranceElement& entranceEl)
{
    PROFILED_FUNCTION();

    if (gTrackDesignSaveMode || (session.ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES))
        return;

    PaintParkEntranceLightEffects(session);

    session.InteractionType = ViewportInteractionItem::parkEntrance;

    ImageId imageTemplate;
    if (entranceEl.IsGhost())
    {
        session.InteractionType = ViewportInteractionItem::none;
        imageTemplate = ImageId().WithRemap(FilterPaletteID::paletteGhost);
    }
    else if (session.SelectedElement == reinterpret_cast<const TileElement*>(&entranceEl))
    {
        imageTemplate = ImageId().WithRemap(FilterPaletteID::paletteGhost);
    }

    auto& objManager = GetContext()->GetObjectManager();
    const auto* entrance = objManager.GetLoadedObject<EntranceObject>(entranceEl.getEntryIndex());
    auto sequence = entranceEl.GetSequenceIndex();
    switch (sequence)
    {
        case EntranceSequence::Centre:
        {
            // Footpath
            auto surfaceDescriptor = entranceEl.GetPathSurfaceDescriptor();
            if (surfaceDescriptor != nullptr)
            {
                auto imageIndex = (surfaceDescriptor->image + 5 * (1 + (direction & 1)));
                PaintAddImageAsParent(
                    session, imageTemplate.WithIndex(imageIndex), { 0, 0, height }, { { 0, 2, height }, { 32, 28, 0 } });
            }

            // Entrance
            if (entrance != nullptr)
            {
                auto imageIndex = entrance->GetImage(sequence, direction);
                PaintAddImageAsParent(
                    session, imageTemplate.WithIndex(imageIndex), { 0, 0, height }, { { 2, 2, height + 32 }, { 28, 28, 47 } });

                if (!entranceEl.IsGhost())
                    PaintParkEntranceScrollingText(session, *entrance, direction, height);
            }
            break;
        }
        case EntranceSequence::Left:
        case EntranceSequence::Right:
            if (entrance != nullptr)
            {
                auto imageIndex = entrance->GetImage(sequence, direction);
                PaintAddImageAsParent(
                    session, imageTemplate.WithIndex(imageIndex), { 0, 0, height }, { { 3, 3, height }, { 26, 26, 79 } });
            }
            break;
    }

    auto supportsImageTemplate = imageTemplate;
    if (!entranceEl.IsGhost())
    {
        supportsImageTemplate = ImageId().WithPrimary(OpenRCT2::Drawing::Colour::saturatedBrown);
    }
    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::truss, WoodenSupportSubType::neSw, direction, height, supportsImageTemplate);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 80);
}

static void PaintHeightMarkers(PaintSession& session, const EntranceElement& entranceEl, int32_t height)
{
    PROFILED_FUNCTION();

    if (PaintShouldShowHeightMarkers(session, VIEWPORT_FLAG_PATH_HEIGHTS))
    {
        if (entranceEl.GetDirections() & 0xF)
        {
            auto heightMarkerBaseZ = entranceEl.GetBaseZ() + 3;
            ImageIndex baseImageIndex = SPR_HEIGHT_MARKER_BASE;
            baseImageIndex += heightMarkerBaseZ / 16;
            baseImageIndex += GetHeightMarkerOffset();
            baseImageIndex -= kMapBaseZ;
            auto imageId = ImageId(baseImageIndex, OpenRCT2::Drawing::Colour::grey);
            PaintAddImageAsParent(session, imageId, { 16, 16, height }, { { 31, 31, heightMarkerBaseZ + 64 }, { 1, 1, 0 } });
        }
    }
}

void PaintEntrance(PaintSession& session, uint8_t direction, int32_t height, const EntranceElement& entranceElement)
{
    PROFILED_FUNCTION();

    session.InteractionType = ViewportInteractionItem::label;

    PaintHeightMarkers(session, entranceElement, height);
    switch (entranceElement.GetEntranceType())
    {
        case ENTRANCE_TYPE_RIDE_ENTRANCE:
        case ENTRANCE_TYPE_RIDE_EXIT:
            PaintRideEntranceExit(session, direction, height, entranceElement);
            break;
        case ENTRANCE_TYPE_PARK_ENTRANCE:
            PaintParkEntrance(session, direction, height, entranceElement);
            break;
    }
}
