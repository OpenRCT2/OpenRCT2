/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GLSLTypes.h"
#include "OpenGLAPI.h"
#include "OpenGLFramebuffer.h"
#include "OpenGLShaderProgram.h"
#include "PostProcessShader.h"

#include <vector>

namespace OpenRCT2::Ui
{
    class PostProcessing
    {
        std::vector<std::string> _available;
        std::vector<std::unique_ptr<PostProcessShader>> _postProcessShaders;
        std::unique_ptr<OpenGLFramebuffer> _postProcessFramebuffer;

    public:
        PostProcessing();

        void Initialize();

        void Resize(int32_t width, int32_t height);

        void Update();

        void Apply(OpenGLFramebuffer& screenFrameBuffer);
    };

} // namespace OpenRCT2::Ui
