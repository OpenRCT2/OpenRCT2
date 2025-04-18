/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "../world/Location.hpp"

    #include <optional>
    #include <quickjs.h>
    #include <string>

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

    inline JSCallback JSToCallback(JSContext* ctx, JSValue obj, const char* name)
    {
        JSValue property = JS_GetPropertyStr(ctx, obj, name);
        JSCallback out(ctx, property);
        JS_FreeValue(ctx, property);
        return out;
    }

    inline std::string JSToStdString(JSContext* ctx, JSValue obj)
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

    inline JSValue JSFromStdString(JSContext* ctx, std::string str)
    {
        return JS_NewString(ctx, str.c_str());
    }

    inline std::string JSToStdString(JSContext* ctx, JSValue obj, const char* property)
    {
        JSValue val = JS_GetPropertyStr(ctx, obj, property);
        std::string output = JSToStdString(ctx, val);
        JS_FreeValue(ctx, val);
        return output;
    }

    inline std::optional<int32_t> JSToOptionalInt(JSContext* ctx, JSValue obj, const char* property)
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

    inline int32_t JSToInt(JSContext* ctx, JSValue obj, const char* property)
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

    #define JS_UNPACK_INT32(var, ctx, val)                                                                                     \
        int32_t var;                                                                                                           \
        if (!JS_IsNumber(val))                                                                                                 \
        {                                                                                                                      \
            JS_ThrowTypeError(ctx, "Expected number");                                                                         \
            return JS_EXCEPTION;                                                                                               \
        }                                                                                                                      \
        if (JS_ToInt32(ctx, &var, val) < 0)                                                                                    \
        {                                                                                                                      \
            return JS_EXCEPTION;                                                                                               \
        }

    #define JS_UNPACK_STR(var, ctx, val)                                                                                       \
        std::string var;                                                                                                       \
        if (!JS_IsString(val))                                                                                                 \
        {                                                                                                                      \
            JS_ThrowTypeError(ctx, "Expected string");                                                                         \
            return JS_EXCEPTION;                                                                                               \
        }                                                                                                                      \
        if (const char* buf = JS_ToCString(ctx, val))                                                                          \
        {                                                                                                                      \
            var = std::string(buf);                                                                                            \
            JS_FreeCString(ctx, buf);                                                                                          \
        }                                                                                                                      \
        else                                                                                                                   \
        {                                                                                                                      \
            return JS_EXCEPTION;                                                                                               \
        }

    #define JS_UNPACK_BOOL(var, ctx, val)                                                                                      \
        bool var;                                                                                                              \
        if (!JS_IsBool(val))                                                                                                   \
        {                                                                                                                      \
            JS_ThrowTypeError(ctx, "Expected boolean");                                                                        \
            return JS_EXCEPTION;                                                                                               \
        }                                                                                                                      \
        {                                                                                                                      \
            const int result = JS_ToBool(ctx, value);                                                                          \
            if (result == -1)                                                                                                  \
            {                                                                                                                  \
                return JS_EXCEPTION;                                                                                           \
            }                                                                                                                  \
            var = result;                                                                                                      \
        }

} // namespace OpenRCT2::Scripting

#endif