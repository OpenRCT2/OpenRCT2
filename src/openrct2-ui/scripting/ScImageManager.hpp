/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "CustomImages.h"

#    include <openrct2/Context.h>
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
            dukglue_register_method(ctx, &ScImageManager::getImageInfo, "getImageInfo");
            dukglue_register_method(ctx, &ScImageManager::getPixelData, "getPixelData");
            dukglue_register_method(ctx, &ScImageManager::setPixelData, "setPixelData");
            dukglue_register_method(ctx, &ScImageManager::draw, "draw");
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
            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();
            auto range = AllocateCustomImages(plugin, count);
            return range ? CreateImageIndexRange(range->BaseId, range->Count) : ToDuk(_ctx, undefined);
        }

        void free(const DukValue& dukRange)
        {
            auto start = dukRange["start"].as_int();
            auto count = dukRange["count"].as_int();

            ImageList range(start, count);

            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();
            if (!FreeCustomImages(plugin, range))
            {
                duk_error(_ctx, DUK_ERR_ERROR, "This plugin did not allocate the specified image range.");
            }
        }

        DukValue getImageInfo(int32_t id)
        {
            return DukGetImageInfo(_ctx, id);
        }

        DukValue getPixelData(int32_t id)
        {
            return DukGetImagePixelData(_ctx, id);
        }

        void setPixelData(int32_t id, const DukValue& pixelData)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();
            if (!DoesPluginOwnImage(plugin, id))
            {
                duk_error(_ctx, DUK_ERR_ERROR, "This plugin did not allocate the specified image.");
            }

            DukSetPixelData(_ctx, id, pixelData);
        }

        void draw(int32_t id, const DukValue& dukSize, const DukValue& callback)
        {
            auto width = dukSize["width"].as_int();
            auto height = dukSize["height"].as_int();

            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();
            if (!DoesPluginOwnImage(plugin, id))
            {
                duk_error(_ctx, DUK_ERR_ERROR, "This plugin did not allocate the specified image.");
            }

            DukDrawCustomImage(scriptEngine, id, { width, height }, callback);
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
