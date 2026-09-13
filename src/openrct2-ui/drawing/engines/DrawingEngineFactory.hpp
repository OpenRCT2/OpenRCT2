/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include <openrct2/core/Guard.hpp>
#include <openrct2/drawing/IDrawingEngine.h>

namespace OpenRCT2::Ui
{
    struct IUiContext;

    [[nodiscard]] std::unique_ptr<Drawing::IDrawingEngine> CreateHardwareDisplayDrawingEngine(IUiContext& uiContext);
#ifndef DISABLE_OPENGL
    [[nodiscard]] std::unique_ptr<Drawing::IDrawingEngine> CreateOpenGLDrawingEngine(IUiContext& uiContext);
#endif

    class DrawingEngineFactory final : public Drawing::IDrawingEngineFactory
    {
    public:
        [[nodiscard]] std::unique_ptr<Drawing::IDrawingEngine> Create(DrawingEngine type, IUiContext& uiContext) override
        {
            switch (type)
            {
                case DrawingEngine::SoftwareWithHardwareDisplay:
                    return CreateHardwareDisplayDrawingEngine(uiContext);
#ifndef DISABLE_OPENGL
                case DrawingEngine::OpenGL:
                    return CreateOpenGLDrawingEngine(uiContext);
#endif
                default:
                    Guard::Fail("Unknown renderer: %u", static_cast<uint32_t>(type));
                    return nullptr;
            }
        }
    };
} // namespace OpenRCT2::Ui
