#include "PsGlobalFunctions.h"

#include "../../../core/Console.hpp"
#include "../../../ride/TrackPaint.h"
#include "../../Paint.h"
#include "../../Supports.h"
#include "PsBoundBoxXYZ.h"
#include "PsCoordsXYZ.h"
#include "PsImageId.h"
#include "PsPaintSession.h"
#include "PsRide.h"
#include "PsTrackElement.h"

namespace OpenRCT2::PaintScripting
{
    namespace PsGlobalFunctions
    {

        static void WoodenASupportsPaintSetup(
            PsPaintSession& session, int32_t supportType, int32_t special, int32_t height, PsImageId& imageTemplate)
        {
            auto& _session = session.GetPaintSession();
            ::WoodenASupportsPaintSetup(_session, supportType, special, height, imageTemplate.GetImageId());
        }

        static void TestPaintSession(PsPaintSession& session)
        {
            PaintSession& _session = session.GetPaintSession();
            Console::WriteLine("session.flags=%d", _session.Flags);
        }

        static void TestImageId(PsImageId& imageId)
        {
            const ImageId& _imageId = imageId.GetImageId();
            Console::WriteLine("imageId = %d", _imageId.ToUInt32());
        }

        static void TrackPaintUtilPaintFences(
            PsPaintSession& session, uint8_t edges, PsCoordsXY& coords, PsTrackElement& trackElement, PsRide& ride,
            PsImageId& imageId, int32_t height, const sol::table& sprites, uint8_t currentRotation)
        {
            std::vector<uint32_t> spritesVector;
            for (const auto& keyval : sprites)
            {
                spritesVector.push_back(keyval.second.as<uint32_t>());
            }
            ::track_paint_util_paint_fences(
                session.GetPaintSession(), edges, coords.GetCoords(), trackElement.GetTrackElement(), ride.GetRide(),
                imageId.GetImageId(), height, spritesVector.data(), currentRotation);
        }

        static void TrackPaintUtilPaintFloor(
            PsPaintSession& session, int32_t edges, PsImageId& imageId, int32_t height, const sol::table& sprites,
            PsStationObject& stationObject)
        {
            std::vector<uint32_t> spritesVector;
            for (const auto& keyval : sprites)
            {
                spritesVector.push_back(keyval.second.as<uint32_t>());
            }
            ::track_paint_util_paint_floor(
                session.GetPaintSession(), edges, imageId.GetImageId(), height, spritesVector.data(),
                &stationObject.GetStationObject());
        }

        static void PaintAddImageAsParent(
            PsPaintSession& session, PsImageId& imageId, PsCoordsXYZ& offset, PsBoundBoxXYZ& boundBox)
        {
            ::PaintAddImageAsParent(
                session.GetPaintSession(), imageId.GetImageId(), offset.GetCoords(), boundBox.GetBoundBoxXYZ());
        }

        static void PaintUtilSetSegmentSupportHeight(PsPaintSession& session, int32_t segments, uint16_t height, uint8_t slope)
        {
            ::PaintUtilSetSegmentSupportHeight(session.GetPaintSession(), segments, height, slope);
        }

        static void PaintUtilSetGeneralSupportHeight(PsPaintSession& session, int16_t height, uint8_t slope)
        {
            ::PaintUtilSetGeneralSupportHeight(session.GetPaintSession(), height, slope);
        }

        void Register(sol::state& lua)
        {
            lua["WoodenASupportsPaintSetup"] = WoodenASupportsPaintSetup;
            lua["TrackPaintUtilPaintFences"] = TrackPaintUtilPaintFences;
            lua["PaintAddImageAsParent"] = PaintAddImageAsParent;
            lua["TrackPaintUtilPaintFloor"] = TrackPaintUtilPaintFloor;
            lua["PaintUtilSetSegmentSupportHeight"] = PaintUtilSetSegmentSupportHeight;
            lua["PaintUtilSetGeneralSupportHeight"] = PaintUtilSetGeneralSupportHeight;
            lua["TestPaintSession"] = TestPaintSession;
            lua["TestImageId"] = TestImageId;
        }
    } // namespace PsGlobalFunctions
} // namespace OpenRCT2::PaintScripting
