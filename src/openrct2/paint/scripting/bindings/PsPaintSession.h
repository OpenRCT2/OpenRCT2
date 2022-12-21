#pragma once

#include "../../../scripting/Duktape.hpp"


struct PaintSession;

namespace OpenRCT2::PaintScripting
{
    class PsImageId;
    class PsCoordsXY;
    class PsPaintSession
    {
    public:
        PsPaintSession();

        static void Register(duk_context* context);
        void Update(PaintSession& paintSession);

        PaintSession* GetSession();
    private:
        PaintSession* _paintSession;
        std::vector<std::shared_ptr<PsImageId>> getTrackColours() const;

        std::shared_ptr<PsCoordsXY> getMapPosition() const;

        uint8_t getCurrentRotation() const;
    };
}
