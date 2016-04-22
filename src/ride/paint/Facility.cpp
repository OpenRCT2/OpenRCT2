#include "paint.h"

namespace Facility
{
    /**
     *
     *  rct2: 0x00762D44
     */
    void Draw(rct_ride * ride, uint8 trackType, uint8 trackSequence, uint8 direction, rct_map_element * mapElement, TileDrawingContext * context)
    {
        if (trackType != FLAT_RIDE_ELEM_FACILITY) return;

        bool hasSupports = context->DrawSupports(SUPPORT_STYLE_WOOD, 0, context->Z, RCT2_GLOBAL(0x00F441A4, uint32));
        context->SetSupportZ(NO_SUPPORT);

        rct_ride_entry_vehicle * firstVehicleEntry = &get_ride_entry_by_ride(ride)->vehicles[0];

        uint32 imageId = RCT2_GLOBAL(0x00F44198, uint32);
        imageId |= firstVehicleEntry->base_image_id;
        imageId += (direction + 2) & 3;

        int lengthX = (direction & 1) == 0 ? 28 : 2;
        int lengthY = (direction & 1) == 0 ? 2 : 28;

        if (hasSupports) {
            uint32 foundationImageId = RCT2_GLOBAL(0x00F441A4, uint32);
            foundationImageId |= 3395;

            context->draw_98197C(foundationImageId, 0, 0, lengthX, lengthY, 29, 0, direction == 3 ? 28 : 2, direction == 0 ? 28 : 2, 0);

            // Door image or base
            context->draw_98199C(imageId, 0, 0, lengthX, lengthY, 29, 0, direction == 3 ? 28 : 2, direction == 0 ? 28 : 2, 0);
        } else {
            // Door image or base
            context->draw_98197C(imageId, 0, 0, lengthX, lengthY, 29, 0, direction == 3 ? 28 : 2, direction == 0 ? 28 : 2, 0);
        }

        // Base image if door was drawn
        if (direction == 1) {
            imageId += 2;
            context->draw_98197C(imageId, 0, 0, 2, 28, 29, 0, 28, 2, 0);
        } else if (direction == 2) {
            imageId += 4;
            context->draw_98197C(imageId, 0, 0, 28, 2, 29, 0, 2, 28, 0);
        }

        context->UpdateTileMaxHeight(32, 0x20);
    }
}