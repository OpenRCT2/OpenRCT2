/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include <openrct2/drawing/Image.h>
#    include <openrct2/scripting/Duktape.hpp>
#    include <openrct2/sprites.h>

namespace OpenRCT2::Scripting
{
    class ScImageManager
    {
    private:
        duk_context* _ctx{};

    public:
        ScImageManager(duk_context* ctx)
            : _ctx(ctx)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_method(ctx, &ScImageManager::getPredefinedRange, "getPredefinedRange");
            dukglue_register_method(ctx, &ScImageManager::getAvailableAllocationRanges, "getAvailableAllocationRanges");
            dukglue_register_method(ctx, &ScImageManager::allocate, "allocate");
            dukglue_register_method(ctx, &ScImageManager::free, "free");
            dukglue_register_method(ctx, &ScImageManager::setPixelData, "setPixelData");
        }

    private:
        DukValue getPredefinedRange(const std::string& name) const
        {
            if (name == "g1")
            {
                return CreateImageIndexRange(0, SPR_G1_END);
            }
            else if (name == "g2")
            {
                return CreateImageIndexRange(SPR_G2_BEGIN, SPR_G2_END - SPR_G2_BEGIN);
            }
            else if (name == "csg")
            {
                return CreateImageIndexRange(SPR_CSG_BEGIN, SPR_CSG_END - SPR_CSG_BEGIN);
            }
            else if (name == "allocated")
            {
                return CreateImageIndexRange(SPR_IMAGE_LIST_BEGIN, SPR_IMAGE_LIST_END - SPR_IMAGE_LIST_BEGIN);
            }
            else
            {
                return ToDuk(_ctx, nullptr);
            }
        }

        DukValue getAvailableAllocationRanges() const
        {
            auto ranges = GetAvailableAllocationRanges();
            duk_push_array(_ctx);
            duk_uarridx_t index = 0;
            for (const auto& range : ranges)
            {
                auto value = CreateImageIndexRange(range.BaseId, range.Count);
                value.push();
                duk_put_prop_index(_ctx, /* duk stack index */ -2, index);
                index++;
            }
            return DukValue::take_from_stack(_ctx);
        }

        DukValue allocate(int32_t count)
        {
            std::vector<rct_g1_element> images;
            images.resize(count);

            auto base = gfx_object_allocate_images(images.data(), count);
            if (base == ImageIndexUndefined)
            {
                return ToDuk(_ctx, nullptr);
            }
            return CreateImageIndexRange(base, count);
        }

        void free(const DukValue& range)
        {
            auto start = range["start"].as_int();
            auto count = range["count"].as_int();
            gfx_object_free_images(start, count);
        }

        static void setPixelDataFromBuffer(
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

        void setPixelData(int32_t id, const DukValue& pixelData)
        {
            auto dukType = pixelData["type"];
            auto& type = dukType.as_string();
            if (type == "raw")
            {
                auto width = pixelData["width"].as_int();
                auto height = pixelData["height"].as_int();
                auto stride = AsOrDefault(pixelData["stride"], width);
                auto data = pixelData["data"];
                auto padding = stride - width;

                auto imageData = new (std::nothrow) uint8_t[width * height];
                if (imageData == nullptr)
                {
                    duk_pop(_ctx);
                    duk_error(_ctx, DUK_ERR_ERROR, "Unable to allocate memory for pixel data.");
                }
                else
                {
                    // Setup the g1 element
                    rct_g1_element el{};
                    auto* lastel = gfx_get_g1_element(id);
                    if (lastel != nullptr)
                    {
                        el = *lastel;
                        delete el.offset;
                    }

                    el.offset = imageData;
                    el.width = width;
                    el.height = height;
                    gfx_set_g1_element(id, &el);

                    // Set the pixel data
                    if (data.is_array())
                    {
                        // From array of numbers
                        data.push();
                        duk_uarridx_t i = 0;
                        for (int32_t y = 0; y < height; y++)
                        {
                            for (int32_t x = 0; x < width; x++)
                            {
                                auto dstI = y * width + x;
                                if (duk_get_prop_index(_ctx, -1, i))
                                {
                                    imageData[dstI] = duk_get_int(_ctx, -1) & 0xFF;
                                    duk_pop(_ctx);
                                }
                                else
                                {
                                    imageData[dstI] = 0;
                                }
                                i++;
                            }
                            i += padding;
                        }
                        duk_pop(_ctx);
                    }
                    else if (data.type() == DukValue::Type::STRING)
                    {
                        // From base64 string
                        data.push();
                        duk_base64_decode(_ctx, -1);
                        duk_size_t bufferLen{};
                        const auto* buffer = reinterpret_cast<uint8_t*>(duk_get_buffer_data(_ctx, -1, &bufferLen));
                        if (buffer != nullptr)
                        {
                            setPixelDataFromBuffer(imageData, buffer, bufferLen, width, height, stride);
                        }
                        duk_pop(_ctx);
                    }
                    else if (data.type() == DukValue::Type::OBJECT)
                    {
                        // From Uint8Array
                        data.push();
                        duk_size_t bufferLen{};
                        const auto* buffer = reinterpret_cast<uint8_t*>(duk_get_buffer_data(_ctx, -1, &bufferLen));
                        if (buffer != nullptr)
                        {
                            setPixelDataFromBuffer(imageData, buffer, bufferLen, width, height, stride);
                        }
                        duk_pop(_ctx);
                    }
                }
            }
            else
            {
                duk_error(_ctx, DUK_ERR_ERROR, "Unsupported pixel data type.");
            }
        }

        DukValue CreateImageIndexRange(size_t start, size_t count) const
        {
            DukObject obj(_ctx);
            obj.Set("start", static_cast<uint32_t>(start));
            obj.Set("count", static_cast<uint32_t>(count));
            return obj.Take();
        }
    };
} // namespace OpenRCT2::Scripting

#endif
