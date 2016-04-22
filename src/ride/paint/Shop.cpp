#include "paint.h"

namespace Shop
{
    /**
     *
     *  rct2: 0x00761160
     */
    void Draw(rct_ride * ride, uint8 trackType, uint8 trackSequence, uint8 direction, rct_map_element *mapElement, TileDrawingContext * context)
    {
        if (trackType != FLAT_RIDE_ELEM_SHOP &&
            trackType != FLAT_RIDE_ELEM_INFORMATION_KIOSK) return;

        bool hasSupports = context->DrawSupports(SUPPORT_STYLE_WOOD, 0, 0, RCT2_GLOBAL(0x00F441A4, uint32));
        context->SetSupportZ(NO_SUPPORT);

        rct_ride_entry_vehicle *firstVehicleEntry = &get_ride_entry_by_ride(ride)->vehicles[0];

        uint32 imageId = RCT2_GLOBAL(0x00F44198, uint32);
        if (imageId & 0x80000000) {
            imageId &= 0x60FFFFFF;
        }
        imageId += firstVehicleEntry->base_image_id;
        imageId += direction;

        if (hasSupports) {
            uint32 foundationImageId = RCT2_GLOBAL(0x00F441A4, uint32);
            foundationImageId |= 3395;
            context->draw_98197C(foundationImageId, 0, 0, 28, 28, 45, 0, 2, 2, 0);

            context->draw_98199C(imageId, 0, 0, 28, 28, 45, 0, 2, 2, 0);
        } else {
            context->draw_98197C(imageId, 0, 0, 28, 28, 45, 0, 2, 2, 0);
        }

        context->UpdateTileMaxHeight(48, 0x20);
    }
}