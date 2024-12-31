/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "BitmapReader.h"

#include <SDL.h>
#include <cstring>
#include <openrct2/core/Imaging.h>
#include <stdexcept>

namespace OpenRCT2::Ui
{
    static std::vector<uint8_t> ReadToVector(std::istream& stream)
    {
        std::vector<uint8_t> result;
        if (!stream.eof() && !stream.fail())
        {
            stream.seekg(0, std::ios_base::end);
            auto size = stream.tellg();
            result.resize(size);
            stream.seekg(0, std::ios_base::beg);
            stream.read(reinterpret_cast<char*>(result.data()), size);
        }
        return result;
    }

    // TODO Bitmaps aren't very complicated to read so we should probably just write our
    //      own implementation in libopenrct2 and spare the AOT implementation registration.
    static Image ReadBitmap(std::istream& istream, IMAGE_FORMAT format)
    {
        auto buffer = ReadToVector(istream);
        auto sdlStream = SDL_RWFromConstMem(buffer.data(), static_cast<int>(buffer.size()));
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
                auto src = static_cast<const uint8_t*>(bitmap->pixels);
                auto dst = image.Pixels.data();
                if (numChannels == 4)
                {
                    for (int32_t y = 0; y < bitmap->h; y++)
                    {
                        std::memcpy(dst, src, bitmap->w);
                        src += bitmap->pitch;
                        dst += bitmap->w;
                    }
                }
                else
                {
                    for (int32_t y = 0; y < bitmap->h; y++)
                    {
                        for (int32_t x = 0; x < bitmap->w; x++)
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

            SDL_FreeSurface(bitmap);
            throw std::runtime_error("Unable to lock surface.");
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
} // namespace OpenRCT2::Ui
