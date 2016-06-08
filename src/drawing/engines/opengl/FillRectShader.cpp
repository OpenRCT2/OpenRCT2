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

#ifndef DISABLE_OPENGL

#include "FillRectShader.h"

FillRectShader::FillRectShader() : OpenGLShaderProgram("fillrect")
{
    GetLocations();

    glGenBuffers(1, &_vbo);
    glGenVertexArrays(1, &_vao);

    Use();
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBindVertexArray(_vao);
    glEnableVertexAttribArray(vPosition);
    glVertexAttribIPointer(vPosition, 2, GL_INT, 0, 0);
}

FillRectShader::~FillRectShader()
{
    glDeleteBuffers(1, &_vbo);
    glDeleteVertexArrays(1, &_vao);

    glBindVertexArray(_vao);
}

void FillRectShader::GetLocations()
{
    uScreenSize = GetUniformLocation("uScreenSize");
    uClip       = GetUniformLocation("uClip");
    uFlags      = GetUniformLocation("uFlags");
    uColour     = GetUniformLocation("uColour");

    vPosition   = GetAttributeLocation("vPosition");
}

void FillRectShader::SetScreenSize(sint32 width, sint32 height)
{
    glUniform2i(uScreenSize, width, height);
}

void FillRectShader::SetClip(sint32 left, sint32 top, sint32 right, sint32 bottom)
{
    glUniform4i(uClip, left, top, right, bottom);
}

void FillRectShader::SetFlags(uint32 flags)
{
    glUniform1i(uFlags, flags);
}

void FillRectShader::SetColour(vec4f colour)
{
    glUniform4f(uColour, colour.r, colour.g, colour.b, colour.a);
}

void FillRectShader::Draw(sint32 left, sint32 top, sint32 right, sint32 bottom)
{
    vec2i vertices[] = {
        { left,  top    },
        { right, top    },
        { right, bottom },
        { left,  bottom },
    };

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

    glBindVertexArray(_vao);
    glDrawArrays(GL_QUADS, 0, 4);
}

#endif /* DISABLE_OPENGL */
