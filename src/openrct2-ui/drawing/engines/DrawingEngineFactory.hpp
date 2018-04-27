#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#pragma once

#include <memory>
#include <openrct2/common.h>
#include <openrct2/drawing/IDrawingEngine.h>

namespace OpenRCT2
{
    namespace Ui
    {
        using namespace OpenRCT2::Drawing;

        interface IUiContext;

        std::unique_ptr<IDrawingEngine> CreateSoftwareDrawingEngine(const std::shared_ptr<IUiContext>& uiContext);
        std::unique_ptr<IDrawingEngine> CreateHardwareDisplayDrawingEngine(const std::shared_ptr<IUiContext>& uiContext);
#ifndef DISABLE_OPENGL
        std::unique_ptr<IDrawingEngine> CreateOpenGLDrawingEngine(const std::shared_ptr<IUiContext>& uiContext);
#endif

        class DrawingEngineFactory final : public IDrawingEngineFactory
        {
        public:
            std::unique_ptr<IDrawingEngine> Create(DRAWING_ENGINE_TYPE type, const std::shared_ptr<IUiContext>& uiContext) override
            {
                switch ((sint32)type) {
                case DRAWING_ENGINE_SOFTWARE:
                    return CreateSoftwareDrawingEngine(uiContext);
                case DRAWING_ENGINE_SOFTWARE_WITH_HARDWARE_DISPLAY:
                    return CreateHardwareDisplayDrawingEngine(uiContext);
#ifndef DISABLE_OPENGL
                case DRAWING_ENGINE_OPENGL:
                    return CreateOpenGLDrawingEngine(uiContext);
#endif
                default:
                    return nullptr;
                }
            }
        };
    }
}
