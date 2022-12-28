#include "PsGlobalFunctions.h"

#include "../../Supports.h"
#include "PsImageId.h"
#include "PsPaintSession.h"
#include "../../Paint.h"
#include "../../../core/Console.hpp"

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

        void Register(sol::state& lua)
        {
            lua["WoodenASupportsPaintSetup"] = WoodenASupportsPaintSetup;
            lua["TestPaintSession"] = TestPaintSession;
            lua["TestImageId"] = TestImageId;
        }
    } // namespace PsGlobalFunctions
} // namespace OpenRCT2::PaintScripting
