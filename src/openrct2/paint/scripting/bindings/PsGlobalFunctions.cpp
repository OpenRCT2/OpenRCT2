#include "PsGlobalFunctions.h"

#include "../../../ride/TrackPaint.h"
#include "../../../scripting/Duktape.hpp"
#include "../../Paint.h"
#include "../../Supports.h"
#include "PsCoordsXY.h"
#include "PsImageId.h"
#include "PsPaintSession.h"
#include "PsRide.h"
#include "PsStationObject.h"
#include "PsTrackElement.h"

namespace OpenRCT2::PaintScripting
{
    void PsGlobalFunctions::WoodenASupportsPaintSetup(
        PsPaintSession* session, int32_t supportType, int32_t special, int32_t height, PsImageId* imageId)
    {
        ::WoodenASupportsPaintSetup(*session->GetSession(), supportType, special, height, imageId->GetImageId());
    }

    bool PsGlobalFunctions::IsEqual(int32_t val1, int32_t val2)
    {
        return val1 == val2;
    }

    bool PsGlobalFunctions::TestSession(PsPaintSession* session)
    {
        Console::WriteLine("session.Flags=%d", session->GetSession()->Flags);
        return true;
    }

    void PsGlobalFunctions::TrackPaintUtilPaintFloor(
        PsPaintSession* session, int8_t edges, PsImageId* imageId, int16_t height, std::vector<uint32_t> sprites,
        PsStationObject* stationObject)
    {
        ::track_paint_util_paint_floor(
            *session->GetSession(), edges, imageId->GetImageId(), height, sprites.data(), stationObject->GetStationObject());
    }

    void PsGlobalFunctions::TrackPaintUtilPaintFences(
        PsPaintSession* session, uint8_t edges, PsCoordsXY* coords, PsTrackElement* trackElement, PsRide* ride,
        PsImageId* imageId, uint16_t height, std::vector<uint32_t> sprites, uint8_t rotation)
    {
        ::track_paint_util_paint_fences(
            *session->GetSession(), edges, coords->GetCoords(), trackElement->GetTrackElement(), ride->GetRide(),
            imageId->GetImageId(), height, sprites.data(), rotation);
    }

    void PsGlobalFunctions::Register(duk_context* context)
    {
        dukglue_register_function(context, &PsGlobalFunctions::WoodenASupportsPaintSetup, "WoodenASupportsPaintSetup");
        dukglue_register_function(context, &PsGlobalFunctions::TrackPaintUtilPaintFloor, "TrackPaintUtilPaintFloor");
        dukglue_register_function(context, &PsGlobalFunctions::TrackPaintUtilPaintFences, "TrackPaintUtilPaintFences");
        dukglue_register_function(context, &PsGlobalFunctions::IsEqual, "IsEqual");
        dukglue_register_function(context, &PsGlobalFunctions::TestSession, "TestSession");
    }
} // namespace OpenRCT2::PaintScripting
