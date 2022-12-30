#pragma once

#include <sol/sol.hpp>

struct Vehicle;
namespace OpenRCT2::PaintScripting
{
    class PsVehicle
    {
    public:
        PsVehicle();
        PsVehicle(const Vehicle& vehicle);

        static void Register(sol::state& lua);

        const Vehicle& GetVehicle() const;
    private:
        const Vehicle* _vehicle;
    };
}
