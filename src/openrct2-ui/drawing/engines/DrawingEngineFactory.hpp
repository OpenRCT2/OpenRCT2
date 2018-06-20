/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

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
                switch ((int32_t)type) {
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
