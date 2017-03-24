#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#include "drawing/IDrawingEngine.h"
#include "Context.h"
#include "OpenRCT2.h"

using namespace OpenRCT2;

class Context : public IContext
{
public:
    Context()
    {
    }

    ~Context() override
    {
    }

    IRegistration * RegisterDrawingEngine(sint32 type, Drawing::IDrawingEngineFactory * factory) override
    {
        return Drawing::DrawingEngineFactory::Register((DRAWING_ENGINE)type, factory);
    }

    sint32 RunOpenRCT2(int argc, char * * argv) override
    {
        core_init();
        int runGame = cmdline_run((const char * *)argv, argc);
        if (runGame == 1)
        {
            openrct2_launch();
        }
        return gExitCode;
    }
};

IContext * OpenRCT2::CreateContext()
{
    return new Context();
}
