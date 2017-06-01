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
#include <SDL_pixels.h>
#include <vector>

// Per-instance data for images
struct DrawImageInstance {
    vec4i clip;
    sint32 texColourAtlas;
    vec4f texColourBounds;
    sint32 texMaskAtlas;
    vec4f texMaskBounds;
    sint32 texPaletteAtlas;
    vec4f texPaletteBounds;
    sint32 flags;
    vec4f colour;
    vec4i bounds;
    sint32 mask;
};

class DrawImageShader final : public OpenGLShaderProgram
{
private:
    GLuint uScreenSize;
    GLuint uTexture;
    GLuint uPalette;

    GLuint vIndex;
    GLuint vClip;
    GLuint vTexColourAtlas;
    GLuint vTexColourBounds;
    GLuint vTexMaskAtlas;
    GLuint vTexMaskBounds;
    GLuint vTexPaletteAtlas;
    GLuint vTexPaletteBounds;
    GLuint vFlags;
    GLuint vColour;
    GLuint vBounds;
    GLuint vMask;

    GLuint _vbo;
    GLuint _vboInstances;
    GLuint _vao;

public:
    DrawImageShader();
    ~DrawImageShader() override;

    void SetScreenSize(sint32 width, sint32 height);
    void SetPalette(const vec4f *glPalette);
    void DrawInstances(const std::vector<DrawImageInstance>& instances);

private:
    void GetLocations();
};
