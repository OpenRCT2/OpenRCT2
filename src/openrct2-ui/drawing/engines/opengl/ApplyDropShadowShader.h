/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GLSLTypes.h"
#include "OpenGLShaderProgram.h"

namespace OpenRCT2::Ui
{
    class ApplyDropShadowShader final : public OpenGLShaderProgram
    {
    private:
        GLint uCameraDepthTex{ -1 };
        GLint uSunDepthTex{ -1 };
        GLint uSunViewPos{ -1 };
        GLint uSunViewSize{ -1 };
        GLint uSunRotation{ -1 };
        GLint uCameraRotation{ -1 };
        GLint uSunFromLeft{ -1 };
        GLint uSunZoomScale{ -1 };
        GLint uCameraZoomScale{ -1 };
        GLint uShadowStrength{ -1 };

        GLint vPosition{ -1 };
        GLint vTextureCoordinate{ -1 };

        GLuint _vbo{};
        GLuint _vao{};

    public:
        ApplyDropShadowShader();
        ~ApplyDropShadowShader() override;

        static void SetCameraDepthTexture(GLuint texture);
        static void SetSunDepthTexture(GLuint texture);
        void SetParameters(
            float sunViewPosX, float sunViewPosY, float sunViewSizeX, float sunViewSizeY, int sunRotation, float sunZoomScale,
            float cameraZoomScale, float strength, int cameraRotation, bool sunFromLeft);
        void Draw();

    private:
        void GetLocations();
    };
} // namespace OpenRCT2::Ui
