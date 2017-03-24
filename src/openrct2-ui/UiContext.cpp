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

#include "UiContext.h"
#include "drawing\engines\DrawingEngines.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;

class UiContext : public IUiContext
{
private:
    IContext * const _context;

    // Drawing engines
    SoftwareDrawingEngineFactory        _softwareDrawingFactory;
    HardwareDisplayDrawingEngineFactory _hardwareDrawingFactory;
#ifndef DISABLE_OPENGL
    OpenGLDrawingEngineFactory          _openglDrawingFactory;
#endif

public:
    UiContext(IContext * context) :
        _context(context)
    {
    }

    ~UiContext() override
    {
    }
};

IUiContext * OpenRCT2::Ui::CreateContext(IContext * context)
{
    return new UiContext(context);
}
