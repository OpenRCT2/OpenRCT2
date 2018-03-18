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

#include "GLSLTypes.h"
#include "OpenGLShaderProgram.h"
#include "DrawCommands.h"
#include <SDL2/SDL_pixels.h>
#include <vector>

class DrawRectShader final : public OpenGLShaderProgram
{
private:
    GLuint uScreenSize;
    GLuint uTexture;
    GLuint uPaletteTex;

    GLuint uPeelingTex;
    GLuint uPeeling;

    GLuint vVertMat;
    GLuint vVertVec;

    GLuint vClip;
    GLuint vTexColourAtlas;
    GLuint vTexColourBounds;
    GLuint vTexMaskAtlas;
    GLuint vTexMaskBounds;
    GLuint vPalettes;
    GLuint vFlags;
    GLuint vColour;
    GLuint vBounds;
    GLuint vDepth;

    GLuint _vbo;
    GLuint _vboInstances;
    GLuint _vao;

    GLsizei _instanceCount = 0;

public:
    DrawRectShader();
    ~DrawRectShader() override;

    void SetScreenSize(sint32 width, sint32 height);
    void EnablePeeling(GLuint peelingTex);
    void DisablePeeling();

    void SetInstances(const RectCommandBatch& instances);
    void DrawInstances();

private:
    void GetLocations();
};
