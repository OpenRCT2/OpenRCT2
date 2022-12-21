#pragma once

#include "../../../scripting/Duktape.hpp"
#include <memory>

struct Vehicle;
namespace OpenRCT2::PaintScripting
{
    class PsVehicle
    {
    public:
        PsVehicle();
        PsVehicle(const Vehicle& vehicle);

        static void Register(duk_context* context);

        const Vehicle& GetVehicle() const;

    private:
        const Vehicle* _vehicle;
    };
}
