#pragma once

#include "../../../scripting/Duktape.hpp"
#include <memory>

struct Ride;
namespace OpenRCT2::PaintScripting
{
    class PsStationObject;
    class PsRide
    {
    public:
        PsRide(Ride& ride);
        PsRide();

        void Update(const Ride& ride);
        static void Register(duk_context* context);

        const Ride& GetRide() const;
    private:
        const Ride* _ride;
        std::shared_ptr<PsStationObject> getStationObject() const;
    };
}
