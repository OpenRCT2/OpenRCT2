/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Location.hpp"

#include <optional>
#include <quickjs.h>

namespace OpenRCT2::Scripting
{
    struct JSCallback
    {
        JSContext* context = nullptr;
        JSValue callback = JS_UNDEFINED;

        JSCallback() {};

        JSCallback(JSContext* ctx, JSValue func)
            : context(ctx)
            , callback(ctx && JS_IsFunction(ctx, func) ? JS_DupValue(ctx, func) : JS_UNDEFINED)
        {
        }

        JSCallback(const JSCallback& other)
            : context(other.context)
            , callback(JS_DupValue(other.context, other.callback))
        {
        }

        JSCallback(JSCallback&& other) noexcept
            : context(other.context)
            , callback(other.callback)
        {
            other.context = nullptr;
            other.callback = JS_UNDEFINED;
        }

        JSCallback& operator=(const JSCallback& other)
        {
            if (this != &other)
            {
                if (context)
                {
                    JS_FreeValue(context, callback);
                }
                context = other.context;
                callback = JS_DupValue(other.context, other.callback);
            }
            return *this;
        }

        JSCallback& operator=(JSCallback&& other) noexcept
        {
            if (this != &other)
            {
                if (context)
                {
                    JS_FreeValue(context, callback);
                }
                context = other.context;
                callback = other.callback;
                other.context = nullptr;
                other.callback = JS_UNDEFINED;
            }
            return *this;
        }

        ~JSCallback()
        {
            if (context)
            {
                JS_FreeValue(context, callback);
            }
        }
    };

    inline JSCallback GetCallbackProperty(JSContext* ctx, JSValue obj, const char* name)
    {
        JSValue property = JS_GetPropertyStr(ctx, obj, name);
        JSCallback out(ctx, property);
        JS_FreeValue(ctx, property);
        return out;
    }

    inline std::string GetStdString(JSContext* ctx, JSValue obj)
    {
        if (JS_IsString(obj))
        {
            const char* buf = JS_ToCString(ctx, obj);
            std::string str(buf);
            JS_FreeCString(ctx, buf);
            return str;
        }
        return {};
    }

    inline std::string GetStdString(JSContext* ctx, JSValue obj, const char* property)
    {
        JSValue val = JS_GetPropertyStr(ctx, obj, property);
        std::string output = GetStdString(ctx, val);
        JS_FreeValue(ctx, val);
        return output;
    }

    inline std::optional<int32_t> GetOptionalInt(JSContext* ctx, JSValue obj, const char* property)
    {
        JSValue val = JS_GetPropertyStr(ctx, obj, property);
        std::optional<int32_t> output = std::nullopt;
        if (JS_IsNumber(val))
        {
            int32_t intVal = -1;
            if (JS_ToInt32(ctx, &intVal, val) >= 0)
            {
                output = std::make_optional(intVal);
            }
        }
        JS_FreeValue(ctx, val);
        return output;
    }

    inline int32_t GetInt(JSContext* ctx, JSValue obj, const char* property)
    {
        JSValue val = JS_GetPropertyStr(ctx, obj, property);
        int32_t output = -1;
        if (JS_IsNumber(val))
        {
            JS_ToInt32(ctx, &output, val);
        }
        JS_FreeValue(ctx, val);
        return output;
    }

    inline JSCallback GetJSCallback(JSContext* ctx, JSValue obj, const char* property)
    {
        JSValue val = JS_GetPropertyStr(ctx, obj, property);
        JSCallback out(ctx, val);
        JS_FreeValue(ctx, val);
        return out;
    }

    inline JSValue ToJSValue(JSContext* ctx, const CoordsXY& coords)
    {
        JSValue obj = JS_NewObject(ctx);
        JS_SetPropertyStr(ctx, obj, "x", JS_NewInt32(ctx, coords.x));
        JS_SetPropertyStr(ctx, obj, "y", JS_NewInt32(ctx, coords.y));
        return obj;
    }

    inline JSValue ToJSValue(JSContext* ctx, const TileCoordsXY& coords)
    {
        JSValue obj = JS_NewObject(ctx);
        JS_SetPropertyStr(ctx, obj, "x", JS_NewInt32(ctx, coords.x));
        JS_SetPropertyStr(ctx, obj, "y", JS_NewInt32(ctx, coords.y));
        return obj;
    }

    inline JSValue ToJSValue(JSContext* ctx, const ScreenCoordsXY& coords)
    {
        JSValue obj = JS_NewObject(ctx);
        JS_SetPropertyStr(ctx, obj, "x", JS_NewInt32(ctx, coords.x));
        JS_SetPropertyStr(ctx, obj, "y", JS_NewInt32(ctx, coords.y));
        return obj;
    }

    inline JSValue ToJSValue(JSContext* ctx, const CoordsXYZ& value)
    {
        if (value.IsNull())
        {
            return JS_NULL;
        }

        JSValue obj = JS_NewObject(ctx);
        JS_SetPropertyStr(ctx, obj, "x", JS_NewInt32(ctx, value.x));
        JS_SetPropertyStr(ctx, obj, "y", JS_NewInt32(ctx, value.y));
        JS_SetPropertyStr(ctx, obj, "z", JS_NewInt32(ctx, value.z));
        return obj;
    }
} // namespace OpenRCT2::Scripting
