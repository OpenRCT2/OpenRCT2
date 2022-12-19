#pragma once

#include "../../../scripting/Duktape.hpp"
#include "PsImageId.h"

struct PaintSession;

namespace OpenRCT2::PaintScripting
{
    class PsPaintSession
    {
    public:
        PsPaintSession();

        static void Register(duk_context* context);
        void Update(PaintSession& paintSession);

        PaintSession* GetSession();
    private:
        PaintSession* _paintSession;
        std::array<std::shared_ptr<PsImageId>, 4> _trackColours;
        std::shared_ptr<PsImageId> getTrackColour(size_t index) const;
    };
}
