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

#    include <openrct2/drawing/ImageImporter.h>
using namespace OpenRCT2::Drawing;

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

    enum class PixelDataPaletteKind
    {
        None,
        Keep,
        Closest,
        Dither
    };

    struct PixelData
    {
        PixelDataKind Type;
        int32_t Width;
        int32_t Height;
        int32_t Stride;
        PixelDataPaletteKind Palette;
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

    static std::vector<uint8_t> GetBufferFromDukStack(duk_context* ctx)
    {
        std::vector<uint8_t> result;
        duk_size_t bufferLen{};
        const auto* buffer = reinterpret_cast<uint8_t*>(duk_get_buffer_data(ctx, -1, &bufferLen));
        if (buffer != nullptr)
        {
            result.resize(bufferLen);
            std::memcpy(result.data(), buffer, bufferLen);
        }
        return result;
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
            result = GetBufferFromDukStack(ctx);
            duk_pop(ctx);
        }
        else if (data.type() == DukValue::Type::OBJECT)
        {
            // From Uint8Array
            data.push();
            result = GetBufferFromDukStack(ctx);
            duk_pop(ctx);
        }
        return result;
    }

    static std::vector<uint8_t> RemovePadding(const std::vector<uint8_t>& srcData, const PixelData& pixelData)
    {
        std::vector<uint8_t> unpadded(pixelData.Width * pixelData.Height, 0);
        auto* src = srcData.data();
        auto* dst = unpadded.data();
        for (int32_t y = 0; y < pixelData.Height; y++)
        {
            std::memcpy(dst, src, pixelData.Width);
            src += pixelData.Stride;
            dst += pixelData.Width;
        }
        return unpadded;
    }

    static std::vector<uint8_t> GetBufferFromPixelData(duk_context* ctx, PixelData& pixelData)
    {
        std::vector<uint8_t> imageData;
        switch (pixelData.Type)
        {
            case PixelDataKind::Raw:
            {
                auto data = DukGetDataFromBufferLikeObject(pixelData.Data);
                if (pixelData.Stride != pixelData.Width)
                {
                    // Make sure data is expected size for RemovePadding
                    data.resize(pixelData.Stride * pixelData.Height);
                    data = RemovePadding(data, pixelData);
                }

                // Make sure data is expected size
                data.resize(pixelData.Width * pixelData.Height);
                break;
            }
            case PixelDataKind::Rle:
            {
                imageData = DukGetDataFromBufferLikeObject(pixelData.Data);
                break;
            }
            case PixelDataKind::Png:
            {
                auto imageFormat = pixelData.Palette == PixelDataPaletteKind::Keep ? IMAGE_FORMAT::PNG : IMAGE_FORMAT::PNG_32;
                auto palette = pixelData.Palette == PixelDataPaletteKind::Keep ? ImageImporter::Palette::KeepIndices
                                                                               : ImageImporter::Palette::OpenRCT2;
                auto importMode = ImageImporter::ImportMode::Default;
                if (pixelData.Palette == PixelDataPaletteKind::Closest)
                    importMode = ImageImporter::ImportMode::Closest;
                else if (pixelData.Palette == PixelDataPaletteKind::Dither)
                    importMode = ImageImporter::ImportMode::Dithering;
                auto pngData = DukGetDataFromBufferLikeObject(pixelData.Data);
                auto image = Imaging::ReadFromBuffer(pngData, imageFormat);

                ImageImporter importer;
                auto importResult = importer.Import(image, 0, 0, palette, ImageImporter::ImportFlags::RLE, importMode);

                pixelData.Type = PixelDataKind::Rle;
                pixelData.Width = importResult.Element.width;
                pixelData.Height = importResult.Element.height;

                imageData = std::move(importResult.Buffer);
                break;
            }
            default:
                throw std::runtime_error("Unsupported pixel data type.");
        }
        return imageData;
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

    template<> PixelDataPaletteKind FromDuk(const DukValue& d)
    {
        if (d.type() == DukValue::Type::STRING)
        {
            auto& s = d.as_string();
            if (s == "keep")
                return PixelDataPaletteKind::Keep;
            if (s == "closest")
                return PixelDataPaletteKind::Closest;
            if (s == "dither")
                return PixelDataPaletteKind::Dither;
        }
        return PixelDataPaletteKind::None;
    }

    static PixelData GetPixelDataFromDuk(const DukValue& dukPixelData)
    {
        PixelData pixelData;
        pixelData.Type = FromDuk<PixelDataKind>(dukPixelData["type"]);
        pixelData.Palette = FromDuk<PixelDataPaletteKind>(dukPixelData["palette"]);
        pixelData.Width = AsOrDefault(dukPixelData["width"], 0);
        pixelData.Height = AsOrDefault(dukPixelData["height"], 0);
        pixelData.Stride = AsOrDefault(dukPixelData["stride"], pixelData.Width);
        pixelData.Data = dukPixelData["data"];
        return pixelData;
    }

    static void ReplacePixelDataForImage(ImageIndex id, const PixelData& pixelData, std::vector<uint8_t>&& data)
    {
        // Setup the g1 element
        rct_g1_element el{};
        auto* lastel = gfx_get_g1_element(id);
        if (lastel != nullptr)
        {
            el = *lastel;
            delete[] el.offset;
        }

        // Copy data into new unmanaged uint8_t[]
        auto newData = new uint8_t[data.size()];
        std::memcpy(newData, data.data(), data.size());

        el.offset = newData;
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
            ReplacePixelDataForImage(id, pixelData, std::move(newData));
        }
        catch (const std::runtime_error& e)
        {
            duk_error(ctx, DUK_ERR_ERROR, e.what());
        }
    }

} // namespace OpenRCT2::Scripting

#endif
