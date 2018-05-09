#pragma region Copyright (c) 2018 OpenRCT2 Developers
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

#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <SDL2/SDL.h>
#include <openrct2/core/Imaging.h>
#include "BitmapReader.h"

static std::vector<uint8> ReadToVector(std::istream &stream)
{
    std::vector<uint8> result;
    if (!stream.eof() && !stream.fail())
    {
        stream.seekg(0, std::ios_base::end);
        auto size = stream.tellg();
        result.resize(size);
        stream.seekg(0, std::ios_base::beg);
        stream.read((char *)result.data(), size);
    }
    return result;
}

// TODO Bitmaps aren't very complicated to read so we should probably just write our
//      own implementation in libopenrct2 and spare the AOT implementation registration.
static Image ReadBitmap(std::istream &istream, IMAGE_FORMAT format)
{
    auto buffer = ReadToVector(istream);
    auto sdlStream = SDL_RWFromConstMem(buffer.data(), (int)buffer.size());
    auto bitmap = SDL_LoadBMP_RW(sdlStream, 1);
    if (bitmap != nullptr)
    {
        auto numChannels = bitmap->format->BytesPerPixel;
        if (numChannels < 3 || bitmap->format->BitsPerPixel < 24)
        {
            SDL_FreeSurface(bitmap);
            throw std::runtime_error("Only 24-bit bitmaps are supported.");
        }

        // Copy pixels over, then discard the surface
        if (SDL_LockSurface(bitmap) == 0)
        {
            Image image;
            image.Width = bitmap->w;
            image.Height = bitmap->h;
            image.Depth = 32;
            image.Pixels.resize(bitmap->w * bitmap->h * 4);
            image.Stride = bitmap->w * 4;

            // Clear image with 0xFF
            std::fill(image.Pixels.begin(), image.Pixels.end(), 0xFF);

            // Copy pixels over
            auto src = (const uint8 *)bitmap->pixels;
            auto dst = image.Pixels.data();
            if (numChannels == 4)
            {
                for (sint32 y = 0; y < bitmap->h; y++)
                {
                    std::memcpy(dst, src, bitmap->w);
                    src += bitmap->pitch;
                    dst += bitmap->w;
                }
            }
            else
            {
                for (sint32 y = 0; y < bitmap->h; y++)
                {
                    for (sint32 x = 0; x < bitmap->w; x++)
                    {
                        std::memcpy(dst, src, 3);
                        src += 3;
                        dst += 4;
                    }
                    src += bitmap->pitch - (bitmap->w * 3);
                }
            }
            SDL_UnlockSurface(bitmap);
            SDL_FreeSurface(bitmap);

            return image;
        }
        else
        {
            SDL_FreeSurface(bitmap);
            throw std::runtime_error("Unable to lock surface.");
        }
    }
    else
    {
        throw std::runtime_error(SDL_GetError());
    }
}

void RegisterBitmapReader()
{
    Imaging::SetReader(IMAGE_FORMAT::BITMAP, ReadBitmap);
}
