/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "CustomImages.h"

#    include "ScGraphicsContext.hpp"

#    include <openrct2/Context.h>
#    include <openrct2/drawing/Image.h>
#    include <openrct2/drawing/ImageImporter.h>
#    include <openrct2/drawing/X8DrawingEngine.h>
#    include <openrct2/scripting/Plugin.h>

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

    struct AllocatedImageList
    {
        std::shared_ptr<Plugin> Owner;
        ImageList Range;
    };

    static std::vector<AllocatedImageList> _allocatedImages;

    static void FreeImages(ImageList range)
    {
        for (ImageIndex i = 0; i < range.Count; i++)
        {
            auto index = range.BaseId + i;
            auto g1 = GfxGetG1Element(index);
            if (g1 != nullptr)
            {
                // Free pixel data
                delete[] g1->offset;

                // Replace slot with empty element
                G1Element empty{};
                GfxSetG1Element(index, &empty);
            }
        }
        GfxObjectFreeImages(range.BaseId, range.Count);
    }

    std::optional<ImageList> AllocateCustomImages(const std::shared_ptr<Plugin>& plugin, uint32_t count)
    {
        std::vector<G1Element> images;
        images.resize(count);

        auto base = GfxObjectAllocateImages(images.data(), count);
        if (base == ImageIndexUndefined)
        {
            return {};
        }
        auto range = ImageList(base, count);

        AllocatedImageList item;
        item.Owner = plugin;
        item.Range = range;
        _allocatedImages.push_back(std::move(item));
        return range;
    }

    bool FreeCustomImages(const std::shared_ptr<Plugin>& plugin, ImageList range)
    {
        auto it = std::find_if(
            _allocatedImages.begin(), _allocatedImages.end(),
            [&plugin, range](const AllocatedImageList& item) { return item.Owner == plugin && item.Range == range; });
        if (it == _allocatedImages.end())
        {
            return false;
        }

        FreeImages(it->Range);
        _allocatedImages.erase(it);
        return true;
    }

    bool DoesPluginOwnImage(const std::shared_ptr<Plugin>& plugin, ImageIndex index)
    {
        auto it = std::find_if(
            _allocatedImages.begin(), _allocatedImages.end(),
            [&plugin, index](const AllocatedImageList& item) { return item.Owner == plugin && item.Range.Contains(index); });
        return it != _allocatedImages.end();
    }

    static void FreeCustomImages(const std::shared_ptr<Plugin>& plugin)
    {
        auto it = _allocatedImages.begin();
        while (it != _allocatedImages.end())
        {
            if (it->Owner == plugin)
            {
                FreeImages(it->Range);
                it = _allocatedImages.erase(it);
            }
            else
            {
                it++;
            }
        }
    }

    void InitialiseCustomImages(ScriptEngine& scriptEngine)
    {
        scriptEngine.SubscribeToPluginStoppedEvent([](std::shared_ptr<Plugin> plugin) -> void { FreeCustomImages(plugin); });
    }

    DukValue DukGetImageInfo(duk_context* ctx, ImageIndex id)
    {
        auto* g1 = GfxGetG1Element(id);
        if (g1 == nullptr)
        {
            return ToDuk(ctx, undefined);
        }

        DukObject obj(ctx);
        obj.Set("id", id);
        obj.Set("offset", ToDuk<ScreenCoordsXY>(ctx, { g1->x_offset, g1->y_offset }));
        obj.Set("width", g1->width);
        obj.Set("height", g1->height);

        obj.Set("hasTransparent", (g1->flags & G1_FLAG_HAS_TRANSPARENCY) != 0);
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

    static const char* GetPixelDataTypeForG1(const G1Element& g1)
    {
        if (g1.flags & G1_FLAG_RLE_COMPRESSION)
            return "rle";
        else if (g1.flags & G1_FLAG_PALETTE)
            return "palette";
        return "raw";
    }

    DukValue DukGetImagePixelData(duk_context* ctx, ImageIndex id)
    {
        auto* g1 = GfxGetG1Element(id);
        if (g1 == nullptr)
        {
            return ToDuk(ctx, undefined);
        }
        auto dataSize = G1CalculateDataSize(g1);
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
                imageData = std::move(data);
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
        G1Element el{};
        auto* lastel = GfxGetG1Element(id);
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
        GfxSetG1Element(id, &el);
        DrawingEngineInvalidateImage(id);
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

    void DukDrawCustomImage(ScriptEngine& scriptEngine, ImageIndex id, ScreenSize size, const DukValue& callback)
    {
        auto* ctx = scriptEngine.GetContext();
        auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();

        auto drawingEngine = std::make_unique<X8DrawingEngine>(GetContext()->GetUiContext());
        DrawPixelInfo dpi;
        dpi.DrawingEngine = drawingEngine.get();
        dpi.width = size.width;
        dpi.height = size.height;

        auto createNewImage = false;
        auto g1 = GfxGetG1Element(id);
        if (g1 == nullptr || g1->width != size.width || g1->height != size.height || (g1->flags & G1_FLAG_RLE_COMPRESSION))
        {
            createNewImage = true;
        }

        if (createNewImage)
        {
            auto bufferSize = size.width * size.height;
            dpi.bits = new uint8_t[bufferSize];
            std::memset(dpi.bits, 0, bufferSize);

            // Draw the original image if we are creating a new one
            GfxDrawSprite(&dpi, ImageId(id), { 0, 0 });
        }
        else
        {
            dpi.bits = g1->offset;
        }

        auto dukG = GetObjectAsDukValue(ctx, std::make_shared<ScGraphicsContext>(ctx, dpi));
        scriptEngine.ExecutePluginCall(plugin, callback, { dukG }, false);

        if (createNewImage)
        {
            G1Element newg1{};
            if (g1 != nullptr)
            {
                delete[] g1->offset;
                newg1 = *g1;
            }
            newg1.offset = dpi.bits;
            newg1.width = size.width;
            newg1.height = size.height;
            newg1.flags = 0;
            GfxSetG1Element(id, &newg1);
        }

        DrawingEngineInvalidateImage(id);
    }

} // namespace OpenRCT2::Scripting

#endif
