#include "paint.h"

namespace Facility
{
    /**
     *
     *  rct2: 0x00762D44
     */
    void Draw(RideDrawingContext * dc)
    {
        if (dc->TrackType != FLAT_RIDE_ELEM_FACILITY) return;

        bool hasSupports = dc->DrawSupports(SUPPORT_STYLE_WOOD, 0, dc->Z, RCT2_GLOBAL(0x00F441A4, uint32));
        dc->SetSupportZ(NO_SUPPORT);

        rct_ride_entry_vehicle * firstVehicleEntry = &dc->RideEntry->vehicles[0];

        uint32 imageId = RCT2_GLOBAL(0x00F44198, uint32);
        imageId |= firstVehicleEntry->base_image_id;
        imageId += (dc->Direction + 2) & 3;

        sint16 height16 = (sint16)dc->Z;
        int lengthX = (dc->Direction & 1) == 0 ? 28 : 2;
        int lengthY = (dc->Direction & 1) == 0 ? 2 : 28;

        if (hasSupports) {
            uint32 foundationImageId = RCT2_GLOBAL(0x00F441A4, uint32);
            foundationImageId |= 3395;

            dc->draw_98197C(foundationImageId, 0, 0, lengthX, lengthY, 29, dc->Z, dc->Direction == 3 ? 28 : 2, dc->Direction == 0 ? 28 : 2, height16);

            // Door image or base
            dc->draw_98199C(imageId, 0, 0, lengthX, lengthY, 29, dc->Z, dc->Direction == 3 ? 28 : 2, dc->Direction == 0 ? 28 : 2, height16);
        } else {
            // Door image or base
            dc->draw_98197C(imageId, 0, 0, lengthX, lengthY, 29, dc->Z, dc->Direction == 3 ? 28 : 2, dc->Direction == 0 ? 28 : 2, height16);
        }

        // Base image if door was drawn
        if (dc->Direction == 1) {
            imageId += 2;
            dc->draw_98197C(imageId, 0, 0, 2, 28, 29, dc->Z, 28, 2, height16);
        } else if (dc->Direction == 2) {
            imageId += 4;
            dc->draw_98197C(imageId, 0, 0, 28, 2, 29, dc->Z, 2, 28, height16);
        }

        dc->UpdateTileMaxHeight(height16 + 32, 32);
    }
}