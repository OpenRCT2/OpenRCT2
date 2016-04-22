#include "paint.h"

namespace Shop
{
    /**
     *
     *  rct2: 0x00761160
     */
    void Draw(RideDrawingContext * dc)
    {
        if (dc->TrackType != FLAT_RIDE_ELEM_SHOP &&
            dc->TrackType != FLAT_RIDE_ELEM_INFORMATION_KIOSK) return;

        bool hasSupports = dc->DrawSupports(SUPPORT_STYLE_WOOD, 0, dc->Z, RCT2_GLOBAL(0x00F441A4, uint32));
        dc->SetSupportZ(NO_SUPPORT);

        rct_ride_entry_vehicle *firstVehicleEntry = &dc->RideEntry->vehicles[0];

        uint32 imageId = RCT2_GLOBAL(0x00F44198, uint32);
        if (imageId & 0x80000000) {
            imageId &= 0x60FFFFFF;
        }
        imageId += firstVehicleEntry->base_image_id;
        imageId += dc->Direction;

        sint16 height16 = (sint16)dc->Z;
        if (hasSupports) {
            uint32 foundationImageId = RCT2_GLOBAL(0x00F441A4, uint32);
            foundationImageId |= 3395;
            dc->draw_98197C(foundationImageId, 0, 0, 28, 28, 45, dc->Z, 2, 2, height16);

            dc->draw_98199C(imageId, 0, 0, 28, 28, 45, dc->Z, 2, 2, height16);
        } else {
            dc->draw_98197C(imageId, 0, 0, 28, 28, 45, dc->Z, 2, 2, height16);
        }

        dc->UpdateTileMaxHeight(height16 + 48, 32);
    }
}