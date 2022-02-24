/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "CustomImages.h"

namespace OpenRCT2::Scripting
{
    enum class PixelDataKind
    {
        Unknown,
        Raw,
        Rle,
        Palette,
        Png
    };

    struct PixelData
    {
        PixelDataKind Type;
        int32_t Width;
        int32_t Height;
        int32_t Stride;
        DukValue Data;
    };

    DukValue DukGetImageInfo(duk_context* ctx, ImageIndex id)
    {
        auto* g1 = gfx_get_g1_element(id);
        if (g1 == nullptr)
        {
            return ToDuk(ctx, undefined);
        }

        DukObject obj(ctx);
        obj.Set("id", id);
        obj.Set("offset", ToDuk<ScreenCoordsXY>(ctx, { g1->x_offset, g1->y_offset }));
        obj.Set("width", g1->width);
        obj.Set("height", g1->height);

        obj.Set("isBMP", (g1->flags & G1_FLAG_BMP) != 0);
        obj.Set("isRLE", (g1->flags & G1_FLAG_RLE_COMPRESSION) != 0);
        obj.Set("isPalette", (g1->flags & G1_FLAG_PALETTE) != 0);
        obj.Set("noZoom", (g1->flags & G1_FLAG_NO_ZOOM_DRAW) != 0);

        if (g1->flags & G1_FLAG_HAS_ZOOM_SPRITE)
        {
            obj.Set("nextZoomId", id - g1->zoomed_offset);
        }
        else
        {
            obj.Set("nextZoomId", undefined);
        }
        return obj.Take();
    }

    static const char* GetPixelDataTypeForG1(const rct_g1_element& g1)
    {
        if (g1.flags & G1_FLAG_RLE_COMPRESSION)
            return "rle";
        else if (g1.flags & G1_FLAG_PALETTE)
            return "palette";
        return "raw";
    }

    DukValue DukGetImagePixelData(duk_context* ctx, ImageIndex id)
    {
        auto* g1 = gfx_get_g1_element(id);
        if (g1 == nullptr)
        {
            return ToDuk(ctx, undefined);
        }
        auto dataSize = g1_calculate_data_size(g1);
        auto* type = GetPixelDataTypeForG1(*g1);

        // Copy the G1 data to a JS buffer wrapped in a Uint8Array
        duk_push_fixed_buffer(ctx, dataSize);
        duk_size_t bufferSize{};
        auto* buffer = duk_get_buffer_data(ctx, -1, &bufferSize);
        if (buffer != nullptr && bufferSize == dataSize)
        {
            std::memcpy(buffer, g1->offset, dataSize);
        }
        duk_push_buffer_object(ctx, -1, 0, dataSize, DUK_BUFOBJ_UINT8ARRAY);
        duk_remove(ctx, -2);
        auto data = DukValue::take_from_stack(ctx, -1);

        DukObject obj(ctx);
        obj.Set("type", type);
        obj.Set("width", g1->width);
        obj.Set("height", g1->height);
        obj.Set("data", data);
        return obj.Take();
    }

    static void SetPixelDataFromBuffer(
        uint8_t* dst, const uint8_t* src, size_t srcLen, int32_t width, int32_t height, int32_t stride)
    {
        auto srcEnd = src + srcLen;
        auto dstLen = static_cast<size_t>(width) * height;
        if (stride == width)
        {
            std::memcpy(dst, src, std::min(srcLen, dstLen));
            if (dstLen > srcLen)
            {
                std::memset(dst + srcLen, 0, dstLen - srcLen);
            }
        }
        else
        {
            std::memset(dst, 0, dstLen);
            auto srcLine = src;
            for (int32_t y = 0; y < height; y++)
            {
                auto dstI = y * width;
                auto lineWidth = std::min<size_t>(srcEnd - srcLine, width);
                std::memcpy(dst + dstI, srcLine, lineWidth);
                if (lineWidth < width)
                {
                    break;
                }
                srcLine += stride;
            }
        }
    }

    static std::vector<uint8_t> DukGetDataFromBufferLikeObject(const DukValue& data)
    {
        std::vector<uint8_t> result;
        auto ctx = data.context();
        if (data.is_array())
        {
            // From array of numbers
            data.push();
            auto len = duk_get_length(ctx, -1);
            result.resize(len);
            for (duk_uarridx_t i = 0; i < len; i++)
            {
                if (duk_get_prop_index(ctx, -1, i))
                {
                    result[i] = duk_get_int(ctx, -1) & 0xFF;
                    duk_pop(ctx);
                }
            }
            duk_pop(ctx);
        }
        else if (data.type() == DukValue::Type::STRING)
        {
            // From base64 string
            data.push();
            duk_base64_decode(ctx, -1);
            duk_size_t bufferLen{};
            const auto* buffer = reinterpret_cast<uint8_t*>(duk_get_buffer_data(ctx, -1, &bufferLen));
            if (buffer != nullptr)
            {
                result = std::vector<uint8_t>(buffer, buffer + bufferLen);
            }
            duk_pop(ctx);
        }
        else if (data.type() == DukValue::Type::OBJECT)
        {
            // From Uint8Array
            data.push();
            duk_size_t bufferLen{};
            const auto* buffer = reinterpret_cast<uint8_t*>(duk_get_buffer_data(ctx, -1, &bufferLen));
            if (buffer != nullptr)
            {
                result = std::vector<uint8_t>(buffer, buffer + bufferLen);
            }
            duk_pop(ctx);
        }
        return result;
    }

    static uint8_t* GetBufferFromPixelData(duk_context* ctx, const PixelData& pixelData)
    {
        auto padding = pixelData.Stride - pixelData.Width;
        auto imageData = new (std::nothrow) uint8_t[pixelData.Width * pixelData.Height];
        if (imageData == nullptr)
        {
            throw std::runtime_error("Unable to allocate memory for pixel data.");
        }

        // Ensure image data is auto freed if exception occurs
        std::unique_ptr<uint8_t[]> uniqueImageData(imageData);
        switch (pixelData.Type)
        {
            case PixelDataKind::Raw:
            {
                // Set the pixel data
                auto& data = pixelData.Data;
                if (data.is_array())
                {
                    // From array of numbers
                    data.push();
                    duk_uarridx_t i = 0;
                    for (int32_t y = 0; y < pixelData.Height; y++)
                    {
                        for (int32_t x = 0; x < pixelData.Width; x++)
                        {
                            auto dstI = y * pixelData.Width + x;
                            if (duk_get_prop_index(ctx, -1, i))
                            {
                                imageData[dstI] = duk_get_int(ctx, -1) & 0xFF;
                                duk_pop(ctx);
                            }
                            else
                            {
                                imageData[dstI] = 0;
                            }
                            i++;
                        }
                        i += padding;
                    }
                    duk_pop(ctx);
                }
                else if (data.type() == DukValue::Type::STRING)
                {
                    // From base64 string
                    data.push();
                    duk_base64_decode(ctx, -1);
                    duk_size_t bufferLen{};
                    const auto* buffer = reinterpret_cast<uint8_t*>(duk_get_buffer_data(ctx, -1, &bufferLen));
                    if (buffer != nullptr)
                    {
                        SetPixelDataFromBuffer(
                            imageData, buffer, bufferLen, pixelData.Width, pixelData.Height, pixelData.Stride);
                    }
                    duk_pop(ctx);
                }
                else if (data.type() == DukValue::Type::OBJECT)
                {
                    // From Uint8Array
                    data.push();
                    duk_size_t bufferLen{};
                    const auto* buffer = reinterpret_cast<uint8_t*>(duk_get_buffer_data(ctx, -1, &bufferLen));
                    if (buffer != nullptr)
                    {
                        SetPixelDataFromBuffer(
                            imageData, buffer, bufferLen, pixelData.Width, pixelData.Height, pixelData.Stride);
                    }
                    duk_pop(ctx);
                }
                break;
            }
            case PixelDataKind::Rle:
            {
                auto data = DukGetDataFromBufferLikeObject(pixelData.Data);
                std::memcpy(imageData, data.data(), data.size());
                break;
            }
            case PixelDataKind::Png:
            default:
                throw std::runtime_error("Unsupported pixel data type.");
        }
        return uniqueImageData.release();
    }

    template<> PixelDataKind FromDuk(const DukValue& d)
    {
        if (d.type() == DukValue::Type::STRING)
        {
            auto& s = d.as_string();
            if (s == "raw")
                return PixelDataKind::Raw;
            if (s == "rle")
                return PixelDataKind::Rle;
            if (s == "palette")
                return PixelDataKind::Palette;
            if (s == "png")
                return PixelDataKind::Png;
        }
        return PixelDataKind::Unknown;
    }

    static PixelData GetPixelDataFromDuk(const DukValue& dukPixelData)
    {
        PixelData pixelData;
        pixelData.Type = FromDuk<PixelDataKind>(dukPixelData["type"]);
        pixelData.Width = dukPixelData["width"].as_int();
        pixelData.Height = dukPixelData["height"].as_int();
        pixelData.Stride = AsOrDefault(dukPixelData["stride"], pixelData.Width);
        pixelData.Data = dukPixelData["data"];
        return pixelData;
    }

    static void ReplacePixelDataForImage(ImageIndex id, const PixelData& pixelData, void* data)
    {
        // Setup the g1 element
        rct_g1_element el{};
        auto* lastel = gfx_get_g1_element(id);
        if (lastel != nullptr)
        {
            el = *lastel;
            delete[] el.offset;
        }
        el.offset = reinterpret_cast<uint8_t*>(data);
        el.width = pixelData.Width;
        el.height = pixelData.Height;
        el.flags = 0;
        if (pixelData.Type == PixelDataKind::Rle)
        {
            el.flags |= G1_FLAG_RLE_COMPRESSION;
        }
        gfx_set_g1_element(id, &el);
    }

    void DukSetPixelData(duk_context* ctx, ImageIndex id, const DukValue& dukPixelData)
    {
        auto pixelData = GetPixelDataFromDuk(dukPixelData);
        try
        {
            auto newData = GetBufferFromPixelData(ctx, pixelData);
            ReplacePixelDataForImage(id, pixelData, newData);
        }
        catch (const std::runtime_error& e)
        {
            duk_error(ctx, DUK_ERR_ERROR, e.what());
        }
    }

} // namespace OpenRCT2::Scripting

#endif
