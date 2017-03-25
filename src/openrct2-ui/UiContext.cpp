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

#include <memory>
#include <vector>
#include <openrct2/Context.h>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/ui/UiContext.h>
#include "drawing/engines/DrawingEngines.h"
#include "UiContext.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Ui;

class UiContext : public IUiContext
{
public:
    UiContext()
    {
    }

    ~UiContext() override
    {
    }

    // Window
    void *  GetWindow() override { return nullptr; }
    sint32  GetWidth() override { return 0; }
    sint32  GetHeight() override { return 0; }
    void    SetFullscreenMode(FULLSCREEN_MODE mode) override { }

    // Drawing
    IDrawingEngine * CreateDrawingEngine(DRAWING_ENGINE_TYPE type) override
    {
        switch ((sint32)type) {
        case DRAWING_ENGINE_SOFTWARE:
            return CreateSoftwareDrawingEngine();
        case DRAWING_ENGINE_SOFTWARE_WITH_HARDWARE_DISPLAY:
            return CreateHardwareDisplayDrawingEngine();
#ifndef DISABLE_OPENGL
        case DRAWING_ENGINE_OPENGL:
            return CreateOpenGLDrawingEngine();
#endif
        default:
            return nullptr;
        }
    }

    // Text input
    bool                        IsTextInputActive() override { return false; }
    const TextInputSession *    StartTextInput(utf8 * buffer, sint32 bufferSize) override { return nullptr; }
    void                        StopTextInput() override { }
};

IUiContext * OpenRCT2::Ui::CreateContext()
{
    return new UiContext();
}
