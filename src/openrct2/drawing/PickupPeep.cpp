#include "PickupPeep.h"

#include "../interface/ScreenCoords.hpp"
#include "../interface/Viewport.h"
#include "../interface/Window.h"
#include "../interface/WindowBase.h"
#include "../interface/ZoomLevel.h"
#include "Drawing.Sprite.h"
#include "Drawing.h"
#include "ImageId.hpp"
#include "RenderTarget.h"

namespace OpenRCT2::Drawing
{
    static ImageId _pickupPeepImage{};
    static ScreenCoordsXY _pickupPeepPosition{};
    static ZoomLevel _pickupPeepZoom{};

    static constexpr std::array<int8_t, 3> kPickedUpPeepYOffsets = { 0, 16, 48 };

    void pickupPeepSetImage(ImageIndex baseImageId, Colour primaryColour, Colour secondaryColour)
    {
        _pickupPeepImage = ImageId(baseImageId, primaryColour, secondaryColour);
    }

    void pickupPeepSetPosition(ScreenCoordsXY position)
    {
        _pickupPeepPosition = position;

        auto* mainWindow = WindowGetMain();
        if (mainWindow != nullptr)
        {
            _pickupPeepZoom = std::min(mainWindow->viewport->zoom, ZoomLevel{ 0 });
        }
    }

    void pickupPeepClear()
    {
        _pickupPeepImage = ImageId();
    }

    void pickupPeepInvalidate()
    {
        if (!_pickupPeepImage.HasValue())
            return;

        auto* g1 = GfxGetG1Element(_pickupPeepImage);
        if (g1 == nullptr)
            return;

        auto zoom = _pickupPeepZoom;
        auto xOffset = -int8_t(_pickupPeepZoom);
        auto yOffset = kPickedUpPeepYOffsets[xOffset];

        int32_t left = _pickupPeepPosition.x + zoom.ApplyInversedTo(g1->xOffset) + xOffset;
        int32_t top = _pickupPeepPosition.y + zoom.ApplyInversedTo(g1->yOffset) + yOffset;
        int32_t right = left + zoom.ApplyInversedTo(g1->width);
        int32_t bottom = top + zoom.ApplyInversedTo(g1->height);

        GfxSetDirtyBlocks({ { left, top }, { right, bottom } });
    }

    void pickupPeepDraw(RenderTarget& rt)
    {
        if (!_pickupPeepImage.HasValue())
            return;

        assert(rt.zoom_level == ZoomLevel{ 0 });

        auto zoom = _pickupPeepZoom;
        auto xOffset = -int8_t(_pickupPeepZoom);
        auto yOffset = kPickedUpPeepYOffsets[xOffset];

        auto pos = ScreenCoordsXY{ zoom.ApplyTo(_pickupPeepPosition.x + xOffset),
                                   zoom.ApplyTo(_pickupPeepPosition.y + yOffset) };

        rt.zoom_level = zoom;
        rt.pitch = zoom.ApplyTo(rt.pitch);

        GfxDrawSprite(rt, _pickupPeepImage, pos);

        rt.pitch = zoom.ApplyInversedTo(rt.pitch);
        rt.zoom_level = ZoomLevel{ 0 };
    }
} // namespace OpenRCT2::Drawing
