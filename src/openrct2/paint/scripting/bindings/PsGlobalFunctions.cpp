#include "PsGlobalFunctions.h"

#include "../../../scripting/Duktape.hpp"
#include "../../Supports.h"
#include "PsImageId.h"
#include "PsPaintSession.h"
#include "PsStationObject.h"
#include "../../Paint.h"
#include "../../../ride/TrackPaint.h"

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

    void PsGlobalFunctions::Register(duk_context* context)
    {
        dukglue_register_function(context, &PsGlobalFunctions::WoodenASupportsPaintSetup, "WoodenASupportsPaintSetup");
        dukglue_register_function(context, &PsGlobalFunctions::TrackPaintUtilPaintFloor, "TrackPaintUtilPaintFloor");
        dukglue_register_function(context, &PsGlobalFunctions::IsEqual, "IsEqual");
        dukglue_register_function(context, &PsGlobalFunctions::TestSession, "TestSession");
    }
} // namespace OpenRCT2::PaintScripting
