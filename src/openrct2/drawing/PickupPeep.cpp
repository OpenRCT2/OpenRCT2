#include "PickupPeep.h"

#include "../interface/ScreenCoords.hpp"
#include "../interface/ZoomLevel.h"
#include "Drawing.Sprite.h"
#include "Drawing.h"
#include "ImageId.hpp"
#include "RenderTarget.h"

ImageId gPickupPeepImage;
int32_t gPickupPeepX;
int32_t gPickupPeepY;
ZoomLevel gPickupPeepZoom;

constexpr std::array<int8_t, 3> kPickedUpPeepYOffsets = { 0, 16, 48 };

using OpenRCT2::Drawing::RenderTarget;

void GfxInvalidatePickedUpPeep()
{
    if (!gPickupPeepImage.HasValue())
        return;

    auto* g1 = GfxGetG1Element(gPickupPeepImage);
    if (g1 == nullptr)
        return;

    auto zoom = gPickupPeepZoom;
    auto xOffset = -int8_t(gPickupPeepZoom);
    auto yOffset = kPickedUpPeepYOffsets[xOffset];

    int32_t left = gPickupPeepX + zoom.ApplyInversedTo(g1->xOffset) + xOffset;
    int32_t top = gPickupPeepY + zoom.ApplyInversedTo(g1->yOffset) + yOffset;
    int32_t right = left + zoom.ApplyInversedTo(g1->width);
    int32_t bottom = top + zoom.ApplyInversedTo(g1->height);

    GfxSetDirtyBlocks({ { left, top }, { right, bottom } });
}

void GfxDrawPickedUpPeep(RenderTarget& rt)
{
    if (!gPickupPeepImage.HasValue())
        return;

    assert(rt.zoom_level == ZoomLevel{ 0 });

    auto zoom = gPickupPeepZoom;
    auto xOffset = -int8_t(gPickupPeepZoom);
    auto yOffset = kPickedUpPeepYOffsets[xOffset];

    auto pos = ScreenCoordsXY{ zoom.ApplyTo(gPickupPeepX + xOffset), zoom.ApplyTo(gPickupPeepY + yOffset) };

    rt.zoom_level = zoom;
    rt.pitch = zoom.ApplyTo(rt.pitch);

    GfxDrawSprite(rt, gPickupPeepImage, pos);

    rt.pitch = zoom.ApplyInversedTo(rt.pitch);
    rt.zoom_level = ZoomLevel{ 0 };
}
