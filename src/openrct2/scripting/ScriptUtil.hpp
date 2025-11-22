/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "../world/Location.hpp"

    #include <functional>
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
            , callback(
                  other.context && JS_IsFunction(other.context, other.callback) ? JS_DupValue(other.context, other.callback)
                                                                                : JS_UNDEFINED)
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

        bool IsValid() const
        {
            return context && JS_IsFunction(context, callback);
        }

        ~JSCallback() noexcept
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
        size_t len;
        if (const char* buf = JS_ToCStringLen(ctx, &len, obj))
        {
            std::string str(buf, len);
            JS_FreeCString(ctx, buf);
            return str;
        }
        return {};
    }

    inline std::string JSToStdString(JSContext* ctx, JSValue obj, const char* property)
    {
        JSValue val = JS_GetPropertyStr(ctx, obj, property);
        std::string output = JSToStdString(ctx, val);
        JS_FreeValue(ctx, val);
        return output;
    }

    inline JSValue JSFromStdString(JSContext* ctx, const std::string_view str)
    {
        return JS_NewStringLen(ctx, str.data(), str.length());
    }

    inline std::optional<std::string> JSToOptionalStdString(JSContext* ctx, JSValue obj, const char* property)
    {
        JSValue val = JS_GetPropertyStr(ctx, obj, property);
        std::optional<std::string> output = std::nullopt;
        if (JS_IsString(val))
        {
            output = JSToStdString(ctx, val);
        }
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

    inline std::optional<int64_t> JSToOptionalInt64(JSContext* ctx, JSValue obj, const char* property)
    {
        JSValue val = JS_GetPropertyStr(ctx, obj, property);
        std::optional<int64_t> output = std::nullopt;
        if (JS_IsNumber(val))
        {
            int64_t intVal = -1;
            if (JS_ToInt64(ctx, &intVal, val) >= 0)
            {
                output = std::make_optional(intVal);
            }
        }
        JS_FreeValue(ctx, val);
        return output;
    }

    inline std::optional<uint32_t> JSToOptionalUint(JSContext* ctx, JSValue obj, const char* property)
    {
        JSValue val = JS_GetPropertyStr(ctx, obj, property);
        std::optional<uint32_t> output = std::nullopt;
        if (JS_IsNumber(val))
        {
            uint32_t uintVal = 0;
            if (JS_ToUint32(ctx, &uintVal, val) >= 0)
            {
                output = std::make_optional(uintVal);
            }
        }
        JS_FreeValue(ctx, val);
        return output;
    }

    inline std::optional<bool> JSToOptionalBool(JSContext* ctx, JSValue obj, const char* property)
    {
        JSValue val = JS_GetPropertyStr(ctx, obj, property);
        std::optional<bool> output = std::nullopt;
        if (JS_IsBool(val))
        {
            const int result = JS_ToBool(ctx, val);
            if (result != -1)
            {
                output = std::make_optional(static_cast<bool>(result));
            }
        }
        JS_FreeValue(ctx, val);
        return output;
    }

    inline bool AsOrDefault(JSContext* ctx, JSValue obj, const char* property, bool def)
    {
        JSValue val = JS_GetPropertyStr(ctx, obj, property);
        bool output;
        if (JS_IsBool(val))
        {
            int result = JS_ToBool(ctx, val);
            output = result >= 0 ? result : def;
        }
        else
        {
            output = def;
        }
        JS_FreeValue(ctx, val);
        return output;
    }

    inline int32_t AsOrDefault(JSContext* ctx, JSValue obj, const char* property, int32_t def)
    {
        JSValue val = JS_GetPropertyStr(ctx, obj, property);
        int32_t output;
        if (!JS_IsNumber(val))
        {
            output = def;
        }
        else if (JS_ToInt32(ctx, &output, val) < 0)
        {
            output = def;
        }
        JS_FreeValue(ctx, val);
        return output;
    }

    inline uint32_t AsOrDefault(JSContext* ctx, JSValue obj, const char* property, uint32_t def)
    {
        JSValue val = JS_GetPropertyStr(ctx, obj, property);
        uint32_t output;
        if (!JS_IsNumber(val))
        {
            output = def;
        }
        else if (JS_ToUint32(ctx, &output, val) < 0)
        {
            output = def;
        }
        JS_FreeValue(ctx, val);
        return output;
    }

    inline int64_t AsOrDefault(JSContext* ctx, JSValue obj, const char* property, int64_t def)
    {
        JSValue val = JS_GetPropertyStr(ctx, obj, property);
        int64_t output;
        if (!JS_IsNumber(val))
        {
            output = def;
        }
        else if (JS_ToInt64(ctx, &output, val) < 0)
        {
            output = def;
        }
        JS_FreeValue(ctx, val);
        return output;
    }

    // Note the default parameter needs to be const char*
    // If you use a type like string_view, calls can instead resolve to the boolean version of the function.
    inline std::string AsOrDefault(JSContext* ctx, JSValue obj, const char* property, const char* def)
    {
        JSValue val = JS_GetPropertyStr(ctx, obj, property);
        std::string output;
        if (!JS_IsString(val))
        {
            output = std::string(def);
        }
        else
        {
            output = JSToStdString(ctx, val);
        }
        JS_FreeValue(ctx, val);
        return output;
    }

    inline void JSIterateArray(JSContext* ctx, JSValue val, const std::function<void(JSContext*, JSValue)>& callback)
    {
        if (JS_IsArray(val))
        {
            int64_t arrayLen = -1;
            JS_GetLength(ctx, val, &arrayLen);
            for (int64_t i = 0; i < arrayLen; i++)
            {
                JSValue elem = JS_GetPropertyInt64(ctx, val, i);
                callback(ctx, elem);
                JS_FreeValue(ctx, elem);
            }
        }
    }

    inline void JSIterateArray(
        JSContext* ctx, JSValue obj, const char* property, const std::function<void(JSContext*, JSValue)>& callback)
    {
        JSValue val = JS_GetPropertyStr(ctx, obj, property);
        JSIterateArray(ctx, val, callback);
        JS_FreeValue(ctx, val);
    }

    inline int32_t JSToInt(JSContext* ctx, JSValue val)
    {
        int32_t output = -1;
        if (JS_IsNumber(val))
        {
            JS_ToInt32(ctx, &output, val);
        }
        return output;
    }

    inline int32_t JSToInt(JSContext* ctx, JSValue obj, const char* property)
    {
        JSValue val = JS_GetPropertyStr(ctx, obj, property);
        int32_t output = JSToInt(ctx, val);
        JS_FreeValue(ctx, val);
        return output;
    }

    inline int64_t JSToInt64(JSContext* ctx, JSValue val)
    {
        int64_t output = -1;
        if (JS_IsNumber(val))
        {
            JS_ToInt64(ctx, &output, val);
        }
        return output;
    }

    inline int64_t JSToInt64(JSContext* ctx, JSValue obj, const char* property)
    {
        JSValue val = JS_GetPropertyStr(ctx, obj, property);
        int64_t output = JSToInt64(ctx, val);
        JS_FreeValue(ctx, val);
        return output;
    }

    inline uint32_t JSToUint(JSContext* ctx, JSValue val)
    {
        uint32_t output = 0;
        if (JS_IsNumber(val))
        {
            JS_ToUint32(ctx, &output, val);
        }
        return output;
    }

    inline uint32_t JSToUint(JSContext* ctx, JSValue obj, const char* property)
    {
        JSValue val = JS_GetPropertyStr(ctx, obj, property);
        uint32_t output = JSToUint(ctx, val);
        JS_FreeValue(ctx, val);
        return output;
    }

    inline CoordsXY JSToCoordsXY(JSContext* ctx, JSValue obj)
    {
        return { AsOrDefault(ctx, obj, "x", 0), AsOrDefault(ctx, obj, "y", 0) };
    }

    inline CoordsXY JSToCoordXY(JSContext* ctx, JSValue obj, const char* property)
    {
        JSValue val = JS_GetPropertyStr(ctx, obj, property);
        CoordsXY output = JSToCoordsXY(ctx, val);
        JS_FreeValue(ctx, val);
        return output;
    }

    inline CoordsXYZ JSToCoordsXYZ(JSContext* ctx, JSValue obj)
    {
        CoordsXYZ result;
        if (JS_IsObject(obj))
        {
            result.x = AsOrDefault(ctx, obj, "x", 0);
            result.y = AsOrDefault(ctx, obj, "y", 0);
            result.z = AsOrDefault(ctx, obj, "z", 0);
        }
        else
        {
            result.SetNull();
        }
        return result;
    }

    inline CoordsXYZD JSToCoordsXYZD(JSContext* ctx, JSValue obj)
    {
        CoordsXYZD result;
        if (JS_IsObject(obj))
        {
            result = CoordsXYZD(JSToCoordsXYZ(ctx, obj), AsOrDefault(ctx, obj, "direction", 0));
        }
        else
        {
            result.SetNull();
        }
        return result;
    }

    inline JSValue ToJSValue(JSContext* ctx, uint8_t val)
    {
        return JS_NewInt32(ctx, val);
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

    inline JSValue ToJSValue(JSContext* ctx, const ScreenSize& size)
    {
        JSValue obj = JS_NewObject(ctx);
        JS_SetPropertyStr(ctx, obj, "width", JS_NewInt32(ctx, size.width));
        JS_SetPropertyStr(ctx, obj, "height", JS_NewInt32(ctx, size.height));
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

    inline JSValue ToJSValue(JSContext* ctx, const CoordsXYZD& value)
    {
        if (value.IsNull())
        {
            return JS_NULL;
        }

        JSValue obj = ToJSValue(ctx, static_cast<const CoordsXYZ&>(value));
        JS_SetPropertyStr(ctx, obj, "direction", JS_NewUint32(ctx, value.direction));
        return obj;
    }

    template<typename T>
    JSValue ToJSValue(JSContext* ctx, const std::optional<T>& value)
    {
        return value ? ToJSValue(ctx, *value) : JS_NULL;
    }

    #define JS_THROW_IF_GAME_STATE_NOT_MUTABLE()                                                                               \
        if (!Scripting::IsGameStateMutable())                                                                                  \
        {                                                                                                                      \
            JS_ThrowPlainError(ctx, "Game state is not mutable in this context.");                                             \
            return JS_EXCEPTION;                                                                                               \
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

    #define JS_UNPACK_INT64(var, ctx, val)                                                                                     \
        int64_t var;                                                                                                           \
        if (!JS_IsNumber(val))                                                                                                 \
        {                                                                                                                      \
            JS_ThrowTypeError(ctx, "Expected number");                                                                         \
            return JS_EXCEPTION;                                                                                               \
        }                                                                                                                      \
        if (JS_ToInt64(ctx, &var, val) < 0)                                                                                    \
        {                                                                                                                      \
            return JS_EXCEPTION;                                                                                               \
        }

    #define JS_UNPACK_UINT32(var, ctx, val)                                                                                    \
        uint32_t var;                                                                                                          \
        if (!JS_IsNumber(val))                                                                                                 \
        {                                                                                                                      \
            JS_ThrowTypeError(ctx, "Expected number");                                                                         \
            return JS_EXCEPTION;                                                                                               \
        }                                                                                                                      \
        if (JS_ToUint32(ctx, &var, val) < 0)                                                                                   \
        {                                                                                                                      \
            return JS_EXCEPTION;                                                                                               \
        }

    #define JS_UNPACK_MONEY64(var, ctx, val)                                                                                   \
        money64 var;                                                                                                           \
        if (!JS_IsNumber(val))                                                                                                 \
        {                                                                                                                      \
            JS_ThrowTypeError(ctx, "Expected number");                                                                         \
            return JS_EXCEPTION;                                                                                               \
        }                                                                                                                      \
        if (JS_ToInt64(ctx, &var, val) < 0)                                                                                    \
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
        {                                                                                                                      \
            size_t len;                                                                                                        \
            if (const char* buf = JS_ToCStringLen(ctx, &len, val))                                                             \
            {                                                                                                                  \
                var = std::string(buf, len);                                                                                   \
                JS_FreeCString(ctx, buf);                                                                                      \
            }                                                                                                                  \
            else                                                                                                               \
            {                                                                                                                  \
                return JS_EXCEPTION;                                                                                           \
            }                                                                                                                  \
        }

    #define JS_UNPACK_BOOL(var, ctx, val)                                                                                      \
        bool var;                                                                                                              \
        if (!JS_IsBool(val))                                                                                                   \
        {                                                                                                                      \
            JS_ThrowTypeError(ctx, "Expected boolean");                                                                        \
            return JS_EXCEPTION;                                                                                               \
        }                                                                                                                      \
        {                                                                                                                      \
            const int result = JS_ToBool(ctx, val);                                                                            \
            if (result == -1)                                                                                                  \
            {                                                                                                                  \
                return JS_EXCEPTION;                                                                                           \
            }                                                                                                                  \
            var = result;                                                                                                      \
        }

    #define JS_UNPACK_ARRAY(var, ctx, val)                                                                                     \
        JSValue var = val;                                                                                                     \
        if (!JS_IsArray(val))                                                                                                  \
        {                                                                                                                      \
            JS_ThrowTypeError(ctx, "Expected array");                                                                          \
            return JS_EXCEPTION;                                                                                               \
        }

    #define JS_UNPACK_CALLBACK(var, ctx, val)                                                                                  \
        JSCallback var(ctx, val);                                                                                              \
        if (!var.IsValid())                                                                                                    \
        {                                                                                                                      \
            JS_ThrowTypeError(ctx, "Expected function");                                                                       \
            return JS_EXCEPTION;                                                                                               \
        }

    #define JS_UNPACK_OBJECT(var, ctx, val)                                                                                    \
        JSValue var = val;                                                                                                     \
        if (!JS_IsObject(var))                                                                                                 \
        {                                                                                                                      \
            JS_ThrowTypeError(ctx, "Expected object");                                                                         \
            return JS_EXCEPTION;                                                                                               \
        }

    #define JS_UNPACK_COORDSXY(var, ctx, val)                                                                                  \
        CoordsXY var;                                                                                                          \
        if (!JS_IsObject(var))                                                                                                 \
        {                                                                                                                      \
            JS_ThrowTypeError(ctx, "Expected CoordsXY");                                                                       \
            return JS_EXCEPTION;                                                                                               \
        }                                                                                                                      \
        {                                                                                                                      \
            auto x = JSToOptionalInt(ctx, position, "x");                                                                      \
            auto y = JSToOptionalInt(ctx, position, "y");                                                                      \
            if (x.has_value() && y.has_value())                                                                                \
            {                                                                                                                  \
                var = CoordsXY(x.value(), y.value());                                                                          \
            }                                                                                                                  \
            else                                                                                                               \
            {                                                                                                                  \
                JS_ThrowTypeError(ctx, "Expected CoordsXY");                                                                   \
                return JS_EXCEPTION;                                                                                           \
            }                                                                                                                  \
        }
} // namespace OpenRCT2::Scripting

#endif
