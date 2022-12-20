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
        std::vector<std::shared_ptr<PsImageId>> getTrackColours() const;
    };
}
