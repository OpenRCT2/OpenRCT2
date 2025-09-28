/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "CustomImages.h"

    #include "ScGraphicsContext.hpp"

    #include <openrct2/Context.h>
    #include <openrct2/drawing/Image.h>
    #include <openrct2/drawing/ImageImporter.h>
    #include <openrct2/drawing/X8DrawingEngine.h>
    #include <openrct2/scripting/Plugin.h>
    #include <thirdparty/base64.hpp>

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
        JSValue Data;
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
        if (base == kImageIndexUndefined)
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

    JSValue JSGetImageInfo(JSContext* ctx, ImageIndex id)
    {
        auto* g1 = GfxGetG1Element(id);
        if (g1 == nullptr)
        {
            return JS_UNDEFINED;
        }

        JSValue obj = JS_NewObject(ctx);
        JS_SetPropertyStr(ctx, obj, "id", JS_NewInt32(ctx, id));
        JS_SetPropertyStr(ctx, obj, "offset", ToJSValue(ctx, ScreenCoordsXY{ g1->xOffset, g1->yOffset }));
        JS_SetPropertyStr(ctx, obj, "width", JS_NewInt32(ctx, g1->width));
        JS_SetPropertyStr(ctx, obj, "height", JS_NewInt32(ctx, g1->height));

        JS_SetPropertyStr(ctx, obj, "hasTransparent", JS_NewBool(ctx, g1->flags.has(G1Flag::hasTransparency)));
        JS_SetPropertyStr(ctx, obj, "isRLE", JS_NewBool(ctx, g1->flags.has(G1Flag::hasRLECompression)));
        JS_SetPropertyStr(ctx, obj, "isPalette", JS_NewBool(ctx, g1->flags.has(G1Flag::isPalette)));
        JS_SetPropertyStr(ctx, obj, "noZoom", JS_NewBool(ctx, g1->flags.has(G1Flag::noZoomDraw)));

        if (g1->flags.has(G1Flag::hasZoomSprite))
        {
            JS_SetPropertyStr(ctx, obj, "nextZoomId", JS_NewInt32(ctx, id - g1->zoomedOffset));
        }
        else
        {
            JS_SetPropertyStr(ctx, obj, "nextZoomId", JS_UNDEFINED);
        }
        return obj;
    }

    static std::string_view GetPixelDataTypeForG1(const G1Element& g1)
    {
        if (g1.flags.has(G1Flag::hasRLECompression))
            return "rle";
        else if (g1.flags.has(G1Flag::isPalette))
            return "palette";
        return "raw";
    }

    JSValue JSGetImagePixelData(JSContext* ctx, ImageIndex id)
    {
        auto* g1 = GfxGetG1Element(id);
        if (g1 == nullptr)
        {
            return JS_UNDEFINED;
        }
        auto dataSize = G1CalculateDataSize(g1);
        auto type = GetPixelDataTypeForG1(*g1);

        // Copy the G1 data to a JS buffer wrapped in a Uint8Array
        JSValue data = JS_NewUint8ArrayCopy(ctx, g1->offset, dataSize);

        JSValue obj = JS_NewObject(ctx);
        JS_SetPropertyStr(ctx, obj, "type", JSFromStdString(ctx, type));
        JS_SetPropertyStr(ctx, obj, "width", JS_NewInt32(ctx, g1->width));
        JS_SetPropertyStr(ctx, obj, "height", JS_NewInt32(ctx, g1->height));
        JS_SetPropertyStr(ctx, obj, "data", data);
        return obj;
    }

    static std::vector<uint8_t> GetDataFromBufferLikeObject(JSContext* ctx, JSValue data)
    {
        std::vector<uint8_t> result;
        if (JS_IsArray(data))
        {
            // From array of numbers
            int64_t arrSz = 0;
            JS_GetLength(ctx, data, &arrSz);
            if (arrSz > 0)
            {
                result.resize(arrSz);
                JSIterateArray(ctx, data, [&result](JSContext* ctx, JSValue val) { result.push_back(JSToInt(ctx, val)); });
            }
        }
        else if (JS_IsString(data))
        {
            std::string str = JSToStdString(ctx, data);
            result = base64::decode_into<std::vector<uint8_t>>(str);
        }
        else if (JS_IsArrayBuffer(data))
        {
            // From Uint8Array
            size_t sz = 0;
            uint8_t* arr = JS_GetUint8Array(ctx, &sz, data);
            if (arr)
            {
                result = std::vector<uint8_t>(arr, arr + sz);
            }
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

    static ImportMode getImportModeFromPalette(const PixelDataPaletteKind& palette)
    {
        switch (palette)
        {
            case PixelDataPaletteKind::Closest:
                return ImportMode::Closest;
            case PixelDataPaletteKind::Dither:
                return ImportMode::Dithering;
            case PixelDataPaletteKind::None:
            case PixelDataPaletteKind::Keep:
            default:
                return ImportMode::Default;
        }
    }

    static std::vector<uint8_t> GetBufferFromPixelData(JSContext* ctx, PixelData& pixelData)
    {
        std::vector<uint8_t> imageData;
        switch (pixelData.Type)
        {
            case PixelDataKind::Raw:
            {
                auto data = GetDataFromBufferLikeObject(ctx, pixelData.Data);
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
                imageData = GetDataFromBufferLikeObject(ctx, pixelData.Data);
                break;
            }
            case PixelDataKind::Png:
            {
                auto imageFormat = pixelData.Palette == PixelDataPaletteKind::Keep ? ImageFormat::png : ImageFormat::png32;
                auto palette = pixelData.Palette == PixelDataPaletteKind::Keep ? Palette::KeepIndices : Palette::OpenRCT2;
                auto importMode = getImportModeFromPalette(pixelData.Palette);
                auto pngData = GetDataFromBufferLikeObject(ctx, pixelData.Data);
                auto image = Imaging::ReadFromBuffer(pngData, imageFormat);
                constexpr uint8_t flags = EnumToFlag(ImportFlags::RLE);
                ImageImportMeta meta = { { 0, 0 }, palette, flags, importMode };

                ImageImporter importer;
                auto importResult = importer.Import(image, meta);

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

    static PixelDataKind PixelDataKindFromJS(const std::string& s)
    {
        if (s == "raw")
            return PixelDataKind::Raw;
        if (s == "rle")
            return PixelDataKind::Rle;
        if (s == "palette")
            return PixelDataKind::Palette;
        if (s == "png")
            return PixelDataKind::Png;
        return PixelDataKind::Unknown;
    }

    static PixelDataPaletteKind PixelDataPaletteKindFromJS(const std::string& s)
    {
        if (s == "keep")
            return PixelDataPaletteKind::Keep;
        if (s == "closest")
            return PixelDataPaletteKind::Closest;
        if (s == "dither")
            return PixelDataPaletteKind::Dither;
        return PixelDataPaletteKind::None;
    }

    static PixelData GetPixelDataFromJS(JSContext* ctx, JSValue jsPixelData)
    {
        PixelData pixelData;
        pixelData.Type = PixelDataKindFromJS(JSToStdString(ctx, jsPixelData, "type"));
        pixelData.Palette = PixelDataPaletteKindFromJS(JSToStdString(ctx, jsPixelData, "palette"));
        pixelData.Width = AsOrDefault(ctx, jsPixelData, "width", static_cast<int32_t>(0));
        pixelData.Height = AsOrDefault(ctx, jsPixelData, "height", static_cast<int32_t>(0));
        pixelData.Stride = AsOrDefault(ctx, jsPixelData, "stride", static_cast<int32_t>(pixelData.Width));
        // Note: this must be JS_FreeValued
        pixelData.Data = JS_GetPropertyStr(ctx, jsPixelData, "data");
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
        el.flags = {};
        if (pixelData.Type == PixelDataKind::Rle)
        {
            el.flags.set(G1Flag::hasRLECompression);
        }
        GfxSetG1Element(id, &el);
        DrawingEngineInvalidateImage(id);
    }

    void JSSetPixelData(JSContext* ctx, ImageIndex id, JSValue jsPixelData)
    {
        auto pixelData = GetPixelDataFromJS(ctx, jsPixelData);
        auto newData = GetBufferFromPixelData(ctx, pixelData);
        ReplacePixelDataForImage(id, pixelData, std::move(newData));
        JS_FreeValue(ctx, pixelData.Data);
    }

    void JSDrawCustomImage(ScriptEngine& scriptEngine, ImageIndex id, ScreenSize size, const JSCallback& callback)
    {
        auto* ctx = scriptEngine.GetContext();
        auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();

        auto drawingEngine = std::make_unique<X8DrawingEngine>(GetContext()->GetUiContext());
        RenderTarget rt;
        rt.DrawingEngine = drawingEngine.get();
        rt.width = size.width;
        rt.height = size.height;

        auto createNewImage = false;
        auto g1 = GfxGetG1Element(id);
        if (g1 == nullptr || g1->width != size.width || g1->height != size.height || g1->flags.has(G1Flag::hasRLECompression))
        {
            createNewImage = true;
        }

        if (createNewImage)
        {
            auto bufferSize = size.width * size.height;
            rt.bits = new uint8_t[bufferSize];
            std::memset(rt.bits, 0, bufferSize);

            drawingEngine->BeginDraw();

            // Draw the original image if we are creating a new one
            GfxDrawSprite(rt, ImageId(id), { 0, 0 });

            drawingEngine->EndDraw();
        }
        else
        {
            rt.bits = g1->offset;
        }

        if (callback.IsValid())
        {
            drawingEngine->BeginDraw();
            scriptEngine.ExecutePluginCall(plugin, callback.callback, { gScGraphicsContext.New(ctx, rt) }, false);
            drawingEngine->EndDraw();
        }

        if (createNewImage)
        {
            G1Element newg1{};
            if (g1 != nullptr)
            {
                delete[] g1->offset;
                newg1 = *g1;
            }
            newg1.offset = rt.bits;
            newg1.width = size.width;
            newg1.height = size.height;
            newg1.flags = {};
            GfxSetG1Element(id, &newg1);
        }

        DrawingEngineInvalidateImage(id);
    }

} // namespace OpenRCT2::Scripting

#endif
