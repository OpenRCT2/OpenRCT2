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

#ifdef DISABLE_OPENGL

#include "../IDrawingEngine.h"

IDrawingEngine * DrawingEngineFactory::CreateOpenGL()
{
    return nullptr;
}

#else

#include <unordered_map>
#include <vector>
#include <SDL_platform.h>

#include "OpenGLAPI.h"

#include "../../core/Exception.hpp"
#include "../../core/Math.hpp"
#include "../../core/Memory.hpp"
#include "../IDrawingContext.h"
#include "../IDrawingEngine.h"
#include "../Rain.h"

extern "C"
{
    #include "../../config.h"
    #include "../../interface/window.h"
    #include "../../intro.h"
    #include "../drawing.h"
}

class OpenGLDrawingEngine;

struct vec2f
{
    union { float x; float s; float r; };
    union { float y; float t; float g; };
};

struct vec4f
{
    union { float x; float s; float r; };
    union { float y; float t; float g; };
    union { float z; float p; float b; };
    union { float w; float q; float a; };
};

class OpenGLDrawingContext : public IDrawingContext
{
private:
    OpenGLDrawingEngine *   _engine;
    rct_drawpixelinfo *     _dpi;

    sint32 _offsetX;
    sint32 _offsetY;
    sint32 _clipLeft;
    sint32 _clipTop;
    sint32 _clipRight;
    sint32 _clipBottom;

    std::vector<GLuint> _textures;
    std::unordered_map<uint32, GLuint> _imageTextureMap;

public:
    OpenGLDrawingContext(OpenGLDrawingEngine * engine);
    ~OpenGLDrawingContext() override;

    IDrawingEngine * GetEngine() override;

    void Clear(uint32 colour) override;
    void FillRect(uint32 colour, sint32 x, sint32 y, sint32 w, sint32 h) override;
    void DrawLine(uint32 colour, sint32 x1, sint32 y1, sint32 x2, sint32 y2) override;
    void DrawSprite(uint32 image, sint32 x, sint32 y, uint32 tertiaryColour) override;
    void DrawSpritePaletteSet(uint32 image, sint32 x, sint32 y, uint8 * palette, uint8 * unknown) override;
    void DrawSpriteRawMasked(sint32 x, sint32 y, uint32 maskImage, uint32 colourImage) override;

    void SetDPI(rct_drawpixelinfo * dpi);
    void InvalidateImage(uint32 image);

private:
    GLuint GetOrLoadImageTexture(uint32 image);
    GLuint LoadImageTexture(uint32 image);
    void * GetImageAsARGB(uint32 image, uint32 tertiaryColour, uint32 * outWidth, uint32 * outHeight);
    void FreeTextures();
};

class OpenGLDrawingEngine : public IDrawingEngine
{
private:
    SDL_Window *    _window         = nullptr;
    SDL_GLContext   _context;

    uint32  _width      = 0;
    uint32  _height     = 0;
    uint32  _pitch      = 0;
    size_t  _bitsSize   = 0;
    uint8 * _bits       = nullptr;

    rct_drawpixelinfo _bitsDPI  = { 0 };

    OpenGLDrawingContext *    _drawingContext;

public:
    SDL_Color Palette[256];
    vec4f     GLPalette[256];

    OpenGLDrawingEngine()
    {
        _drawingContext = new OpenGLDrawingContext(this);
    }

    ~OpenGLDrawingEngine() override
    {
        delete _drawingContext;
        delete [] _bits;

        SDL_GL_DeleteContext(_context);
    }

    void Initialise(SDL_Window * window) override
    {
        _window = window;

        _context = SDL_GL_CreateContext(_window);
        SDL_GL_MakeCurrent(_window, _context);

        if (!OpenGLAPI::Initialise())
        {
            throw Exception("Unable to initialise OpenGL.");
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void Resize(uint32 width, uint32 height) override
    {
        ConfigureBits(width, height, width);

        glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    }

    void SetPalette(SDL_Color * palette) override
    {
        for (int i = 0; i < 256; i++)
        {
            SDL_Color colour = palette[i];
            Palette[i] = colour;
            GLPalette[i] = { colour.r / 255.0f,
                             colour.g / 255.0f,
                             colour.b / 255.0f,
                             colour.a / 255.0f };
        }
    }

    void Invalidate(sint32 left, sint32 top, sint32 right, sint32 bottom) override
    {
    }

    void Draw() override
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION_MATRIX);
        glLoadIdentity();
        glOrtho(0, _width, _height, 0, -1.0, 1.0);

        glMatrixMode(GL_MODELVIEW_MATRIX);
        glLoadIdentity();
        glScalef(1, -1.0f, 0);
        glTranslatef(-1.0f, -1.0f, 0);
        glScalef(2.0f / _width, 2.0f / _height, 0);

        if (gIntroState != INTRO_STATE_NONE) {
            intro_draw(&_bitsDPI);
        } else {
            window_update_all_viewports();
            window_draw_all(&_bitsDPI, 0, 0, _width, _height);
            window_update_all();

            gfx_draw_pickedup_peep(&_bitsDPI);

            rct2_draw(&_bitsDPI);
        }
        Display();
    }
    
    sint32 Screenshot() override
    {
        // Not implemented
        return -1;
    }

    void CopyRect(sint32 x, sint32 y, sint32 width, sint32 height, sint32 dx, sint32 dy) override
    {
        // Not applicable for this engine
    }

    IDrawingContext * GetDrawingContext(rct_drawpixelinfo * dpi) override
    {
        _drawingContext->SetDPI(dpi);
        return _drawingContext;
    }

    rct_drawpixelinfo * GetDrawingPixelInfo() override
    {
        return &_bitsDPI;
    }

    DRAWING_ENGINE_FLAGS GetFlags() override
    {
        return DEF_NONE;
    }

    void InvalidateImage(uint32 image) override
    {
        _drawingContext->InvalidateImage(image);
    }

    rct_drawpixelinfo * GetDPI()
    {
        return &_bitsDPI;
    }

private:

    void ConfigureBits(uint32 width, uint32 height, uint32 pitch)
    {
        size_t  newBitsSize = pitch * height;
        uint8 * newBits = new uint8[newBitsSize];
        if (_bits == nullptr)
        {
            Memory::Set(newBits, 0, newBitsSize);
        }
        else
        {
            if (_pitch == pitch)
            {
                Memory::Copy(newBits, _bits, Math::Min(_bitsSize, newBitsSize));
            }
            else
            {
                uint8 * src = _bits;
                uint8 * dst = newBits;

                uint32 minWidth = Math::Min(_width, width);
                uint32 minHeight = Math::Min(_height, height);
                for (uint32 y = 0; y < minHeight; y++)
                {
                    Memory::Copy(dst, src, minWidth);
                    if (pitch - minWidth > 0)
                    {
                        Memory::Set(dst + minWidth, 0, pitch - minWidth);
                    }
                    src += _pitch;
                    dst += pitch;
                }
            }
            delete [] _bits;
        }

        _bits = newBits;
        _bitsSize = newBitsSize;
        _width = width;
        _height = height;
        _pitch = pitch;

        rct_drawpixelinfo * dpi = &_bitsDPI;
        dpi->bits = _bits;
        dpi->x = 0;
        dpi->y = 0;
        dpi->width = width;
        dpi->height = height;
        dpi->pitch = _pitch - width;
    }

    void Display()
    {
        SDL_GL_SwapWindow(_window);
    }
};

IDrawingEngine * DrawingEngineFactory::CreateOpenGL()
{
    return new OpenGLDrawingEngine();
}

OpenGLDrawingContext::OpenGLDrawingContext(OpenGLDrawingEngine * engine)
{
    _engine = engine;
}

OpenGLDrawingContext::~OpenGLDrawingContext()
{

}

IDrawingEngine * OpenGLDrawingContext::GetEngine()
{
    return _engine;
}

void OpenGLDrawingContext::Clear(uint32 colour)
{
    FillRect(colour, _clipLeft, _clipTop, _clipRight, _clipBottom);
}

void OpenGLDrawingContext::FillRect(uint32 colour, sint32 left, sint32 top, sint32 right, sint32 bottom)
{
    vec4f paletteColour = _engine->GLPalette[colour & 0xFF];

    if (left > right)
    {
        left ^= right;
        right ^= left;
        left ^= right;
    }
    if (top > bottom)
    {
        top ^= bottom;
        bottom ^= top;
        top ^= bottom;
    }

    left += _offsetX;
    top += _offsetY;
    right += _offsetX;
    bottom += _offsetY;

    left = Math::Max(left, _clipLeft);
    top = Math::Max(top, _clipTop);
    right = Math::Min(right, _clipRight);
    bottom = Math::Min(bottom, _clipBottom);

    if (right < left || bottom < top)
    {
        return;
    }

    glDisable(GL_TEXTURE_2D);

    if (colour & 0x2000000)
    {
        glColor4f(paletteColour.r, paletteColour.g, paletteColour.b, 0.4f);
    }
    else
    {
        glColor3f(paletteColour.r, paletteColour.g, paletteColour.b);
    }

    glBegin(GL_QUADS);
        glVertex2i(left,  top);
        glVertex2i(left,  bottom + 1);
        glVertex2i(right + 1, bottom + 1);
        glVertex2i(right + 1, top);
    glEnd();
}

void OpenGLDrawingContext::DrawLine(uint32 colour, sint32 x1, sint32 y1, sint32 x2, sint32 y2)
{
    vec4f paletteColour = _engine->GLPalette[colour & 0xFF];
    glColor3f(paletteColour.r, paletteColour.g, paletteColour.b);
    glBegin(GL_LINES);
        glVertex2i(x1, y1);
        glVertex2i(x2, y2);
    glEnd();
}

void OpenGLDrawingContext::DrawSprite(uint32 image, sint32 x, sint32 y, uint32 tertiaryColour)
{
    int g1Id = image & 0x7FFFF;
    rct_g1_element * g1Element = gfx_get_g1_element(g1Id);

    if (_dpi->zoom_level != 0)
    {
        if (g1Element->flags & (1 << 4))
        {
            rct_drawpixelinfo zoomedDPI;
            zoomedDPI.bits = _dpi->bits;
            zoomedDPI.x = _dpi->x >> 1;
            zoomedDPI.y = _dpi->y >> 1;
            zoomedDPI.height = _dpi->height >> 1;
            zoomedDPI.width = _dpi->width >> 1;
            zoomedDPI.pitch = _dpi->pitch;
            zoomedDPI.zoom_level = _dpi->zoom_level - 1;
            SetDPI(&zoomedDPI);
            DrawSprite((image << 28) | (g1Id - g1Element->zoomed_offset), x >> 1, y >> 1, tertiaryColour);
            return;
        }
        if (g1Element->flags & (1 << 5))
        {
            return;
        }
    }

    GLuint texture = GetOrLoadImageTexture(image);

    sint32 drawOffsetX = g1Element->x_offset;
    sint32 drawOffsetY = g1Element->y_offset;
    sint32 drawWidth = (uint16)g1Element->width >> _dpi->zoom_level;
    sint32 drawHeight = (uint16)g1Element->height >> _dpi->zoom_level;

    sint32 left = x + drawOffsetX;
    sint32 top = y + drawOffsetY;
    sint32 right = left + drawWidth;
    sint32 bottom = top + drawHeight;
    // FillRect(g1Id & 0xFF, left, top, right, bottom);

    if (left > right)
    {
        left ^= right;
        right ^= left;
        left ^= right;
    }
    if (top > bottom)
    {
        top ^= bottom;
        bottom ^= top;
        top ^= bottom;
    }

    left += _offsetX;
    top += _offsetY;
    right += _offsetX;
    bottom += _offsetY;

    vec2f texCoords[4] = { { 0, 0 },
                           { 0, 1 },
                           { 1, 1 },
                           { 1, 0 } };

    sint32 leftChop = _clipLeft - left;
    if (leftChop > 0)
    {
        left += leftChop;
        texCoords[0].x =
        texCoords[1].x = (float)leftChop / g1Element->width;
    }

    sint32 rightChop = right - _clipRight;
    if (rightChop > 0)
    {
        right -= rightChop;
        texCoords[2].x =
        texCoords[3].x = 1.0f - ((float)rightChop / g1Element->width);
    }

    sint32 topChop = _clipTop - top;
    if (topChop > 0)
    {
        top += topChop;
        texCoords[0].y =
        texCoords[3].y = (float)topChop / g1Element->height;
    }

    sint32 bottomChop = bottom - _clipBottom;
    if (bottomChop > 0)
    {
        bottom -= bottomChop;
        texCoords[1].y =
        texCoords[2].y = 1.0f - ((float)bottomChop / g1Element->height);
    }

    if (right < left || bottom < top)
    {
        return;
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
        glTexCoord2f(texCoords[0].s, texCoords[0].t);
        glVertex2i(left,  top);
        glTexCoord2f(texCoords[1].s, texCoords[1].t);
        glVertex2i(left,  bottom);
        glTexCoord2f(texCoords[2].s, texCoords[2].t);
        glVertex2i(right, bottom);
        glTexCoord2f(texCoords[3].s, texCoords[3].t);
        glVertex2i(right, top);
    glEnd();
}

void OpenGLDrawingContext::DrawSpritePaletteSet(uint32 image, sint32 x, sint32 y, uint8 * palette, uint8 * unknown)
{
    DrawSprite(image, x, y, 0);
}

void OpenGLDrawingContext::DrawSpriteRawMasked(sint32 x, sint32 y, uint32 maskImage, uint32 colourImage)
{
}

void OpenGLDrawingContext::SetDPI(rct_drawpixelinfo * dpi)
{
    rct_drawpixelinfo * screenDPI = _engine->GetDPI();
    size_t bitsSize = (size_t)screenDPI->height * (size_t)(screenDPI->width + screenDPI->pitch);
    size_t bitsOffset = (size_t)(dpi->bits - screenDPI->bits);

    assert(bitsOffset < bitsSize);

    _clipLeft = bitsOffset % (screenDPI->width + screenDPI->pitch);
    _clipTop = bitsOffset / (screenDPI->width + screenDPI->pitch);

    _clipRight = _clipLeft + dpi->width;
    _clipBottom = _clipTop + dpi->height;
    _offsetX = _clipLeft - dpi->x;
    _offsetY = _clipTop - dpi->y;

    _dpi = dpi;
}

GLuint OpenGLDrawingContext::GetOrLoadImageTexture(uint32 image)
{
    auto kvp = _imageTextureMap.find(image);
    if (kvp != _imageTextureMap.end())
    {
        return kvp->second;
    }

    GLuint texture = LoadImageTexture(image);
    _textures.push_back(texture);
    _imageTextureMap[image] = texture;

    // if ((_textures.size() % 100) == 0)
    // {
    //     printf("Textures: %d\n", _textures.size());
    // }

    return texture;
}

GLuint OpenGLDrawingContext::LoadImageTexture(uint32 image)
{
    GLuint texture;
    glGenTextures(1, &texture);

    uint32 width, height;
    void * pixels32 = GetImageAsARGB(image, 0, &width, &height);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels32);

    delete [] (uint8 *) pixels32;

    return texture;
}

void * OpenGLDrawingContext::GetImageAsARGB(uint32 image, uint32 tertiaryColour, uint32 * outWidth, uint32 * outHeight)
{
    int g1Id = image & 0x7FFFF;
    rct_g1_element * g1Element = gfx_get_g1_element(g1Id);

    uint32 width = (uint32)g1Element->width;
    uint32 height = (uint32)g1Element->height;

    size_t numPixels = width * height;
    uint8 * pixels8 = new uint8[numPixels];
    Memory::Set(pixels8, 0, numPixels);

    rct_drawpixelinfo dpi;
    dpi.bits = pixels8;
    dpi.pitch = 0;
    dpi.x = 0;
    dpi.y = 0;
    dpi.width = width;
    dpi.height = height;
    dpi.zoom_level = 0;
    gfx_draw_sprite_software(&dpi, image, -g1Element->x_offset, -g1Element->y_offset, tertiaryColour);

    uint8 * pixels32 = new uint8[width * height * 4];
    uint8 * src = pixels8;
    uint8 * dst = pixels32;
    for (size_t i = 0; i < numPixels; i++)
    {
        uint8 paletteIndex = *src++;
        if (paletteIndex == 0)
        {
            // Transparent
            *dst++ = 0;
            *dst++ = 0;
            *dst++ = 0;
            *dst++ = 0;
        }
        else
        {
            SDL_Color colour = _engine->Palette[paletteIndex];
            *dst++ = colour.r;
            *dst++ = colour.g;
            *dst++ = colour.b;
            *dst++ = 255;
        }
    }

    delete[] pixels8;

    *outWidth = width;
    *outHeight = height;
    return pixels32;
}

void OpenGLDrawingContext::InvalidateImage(uint32 image)
{
    auto kvp = _imageTextureMap.find(image);
    if (kvp != _imageTextureMap.end())
    {
        GLuint texture = kvp->second;
        glDeleteTextures(1, &texture);

        _imageTextureMap.erase(kvp);
    }
}

void OpenGLDrawingContext::FreeTextures()
{
    glDeleteTextures(_textures.size(), _textures.data());
}

#endif /* DISABLE_OPENGL */
