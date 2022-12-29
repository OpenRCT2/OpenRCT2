#include "PsGlobalFunctions.h"

#include "../../../core/Console.hpp"
#include "../../Paint.h"
#include "../../Supports.h"
#include "PsImageId.h"
#include "PsPaintSession.h"
#include "PsRide.h"
#include "PsTrackElement.h"
#include "../../../ride/TrackPaint.h"

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

        void Register(sol::state& lua)
        {
            lua["WoodenASupportsPaintSetup"] = WoodenASupportsPaintSetup;
            lua["TrackPaintUtilPaintFences"] = TrackPaintUtilPaintFences;
            lua["TestPaintSession"] = TestPaintSession;
            lua["TestImageId"] = TestImageId;
        }
    } // namespace PsGlobalFunctions
} // namespace OpenRCT2::PaintScripting
