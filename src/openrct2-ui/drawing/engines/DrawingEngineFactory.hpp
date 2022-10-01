/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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

        struct IUiContext;

        [[nodiscard]] std::unique_ptr<IDrawingEngine> CreateSoftwareDrawingEngine(const std::shared_ptr<IUiContext>& uiContext);
        [[nodiscard]] std::unique_ptr<IDrawingEngine> CreateHardwareDisplayDrawingEngine(
            const std::shared_ptr<IUiContext>& uiContext);
#ifndef DISABLE_OPENGL
        [[nodiscard]] std::unique_ptr<IDrawingEngine> CreateOpenGLDrawingEngine(const std::shared_ptr<IUiContext>& uiContext);
#endif

        class DrawingEngineFactory final : public IDrawingEngineFactory
        {
        public:
            [[nodiscard]] std::unique_ptr<IDrawingEngine> Create(
                DrawingEngine type, const std::shared_ptr<IUiContext>& uiContext) override
            {
                switch (type)
                {
                    case DrawingEngine::Software:
                        return CreateSoftwareDrawingEngine(uiContext);
                    case DrawingEngine::SoftwareWithHardwareDisplay:
                        return CreateHardwareDisplayDrawingEngine(uiContext);
#ifndef DISABLE_OPENGL
                    case DrawingEngine::OpenGL:
                        return CreateOpenGLDrawingEngine(uiContext);
#endif
                    default:
                        return nullptr;
                }
            }
        };
    } // namespace Ui
} // namespace OpenRCT2
