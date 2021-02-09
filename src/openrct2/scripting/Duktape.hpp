/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../world/Map.h"

#    include <cstdio>
#    include <dukglue/dukglue.h>
#    include <duktape.h>
#    include <optional>
#    include <stdexcept>

namespace OpenRCT2::Scripting
{
    template<typename T> DukValue GetObjectAsDukValue(duk_context* ctx, const std::shared_ptr<T>& value)
    {
        dukglue::types::DukType<std::shared_ptr<T>>::template push<T>(ctx, value);
        return DukValue::take_from_stack(ctx);
    }

    template<typename T> T AsOrDefault(const DukValue& value, const T& defaultValue = {}) = delete;

    inline std::string AsOrDefault(const DukValue& value, std::string_view defaultValue)
    {
        return value.type() == DukValue::STRING ? value.as_string() : std::string(defaultValue);
    }

    template<> inline std::string AsOrDefault(const DukValue& value, const std::string& defaultValue)
    {
        return value.type() == DukValue::STRING ? value.as_string() : defaultValue;
    }

    template<> inline int32_t AsOrDefault(const DukValue& value, const int32_t& defaultValue)
    {
        return value.type() == DukValue::NUMBER ? value.as_int() : defaultValue;
    }

    template<> inline bool AsOrDefault(const DukValue& value, const bool& defaultValue)
    {
        return value.type() == DukValue::BOOLEAN ? value.as_bool() : defaultValue;
    }

    /**
     * Allows creation of an object on the duktape stack and setting properties on it before
     * retrieving the DukValue instance of it.
     */
    class DukObject
    {
    private:
        duk_context* _ctx{};
        duk_idx_t _idx = DUK_INVALID_INDEX;

    public:
        DukObject(duk_context* ctx)
            : _ctx(ctx)
        {
        }

        DukObject(const DukObject&) = delete;

        DukObject(DukObject&& m) noexcept
        {
            _ctx = m._ctx;
            _idx = m._idx;
            m._ctx = {};
            m._idx = {};
        }

        ~DukObject()
        {
            PopObjectIfExists();
        }

        void Set(const char* name, std::nullptr_t)
        {
            EnsureObjectPushed();
            duk_push_null(_ctx);
            duk_put_prop_string(_ctx, _idx, name);
        }

        void Set(const char* name, bool value)
        {
            EnsureObjectPushed();
            duk_push_boolean(_ctx, value);
            duk_put_prop_string(_ctx, _idx, name);
        }

        void Set(const char* name, int32_t value)
        {
            EnsureObjectPushed();
            duk_push_int(_ctx, value);
            duk_put_prop_string(_ctx, _idx, name);
        }

        void Set(const char* name, uint32_t value)
        {
            EnsureObjectPushed();
            duk_push_uint(_ctx, value);
            duk_put_prop_string(_ctx, _idx, name);
        }

        void Set(const char* name, uint64_t value)
        {
            EnsureObjectPushed();
            duk_push_number(_ctx, value);
            duk_put_prop_string(_ctx, _idx, name);
        }

        void Set(const char* name, std::string_view value)
        {
            EnsureObjectPushed();
            duk_push_lstring(_ctx, value.data(), value.size());
            duk_put_prop_string(_ctx, _idx, name);
        }

        void Set(const char* name, const char* value)
        {
            Set(name, std::string_view(value));
        }

        void Set(const char* name, const DukValue& value)
        {
            EnsureObjectPushed();
            value.push();
            duk_put_prop_string(_ctx, _idx, name);
        }

        template<typename T> void Set(const char* name, const std::optional<T>& value)
        {
            if (value)
            {
                EnsureObjectPushed();
                duk_push_null(_ctx);
                duk_put_prop_string(_ctx, _idx, name);
            }
            else
            {
                Set(name, *value);
            }
        }

        DukValue Take()
        {
            EnsureObjectPushed();
            auto result = DukValue::take_from_stack(_ctx, _idx);
            _idx = DUK_INVALID_INDEX;
            return result;
        }

    private:
        void PopObjectIfExists()
        {
            if (_idx != DUK_INVALID_INDEX)
            {
                duk_remove(_ctx, _idx);
                _idx = DUK_INVALID_INDEX;
            }
        }

        void EnsureObjectPushed()
        {
            if (_idx == DUK_INVALID_INDEX)
            {
                _idx = duk_push_object(_ctx);
            }
        }
    };

    class DukStackFrame
    {
    private:
        duk_context* _ctx{};
        duk_idx_t _top;

    public:
        DukStackFrame(duk_context* ctx)
            : _ctx(ctx)
        {
            _top = duk_get_top(ctx);
        }

        ~DukStackFrame()
        {
            auto top = duk_get_top(_ctx);
            if (top != _top)
            {
                duk_set_top(_ctx, _top);
                _ctx = {};
                std::fprintf(stderr, "duktape stack was not returned to original state!");
            }
            _ctx = {};
        }

        DukStackFrame(const DukStackFrame&) = delete;
        DukStackFrame(DukStackFrame&&) = delete;
    };

    /**
     * Bi-directional map for converting between strings and enums / numbers.
     */
    template<typename T> class DukEnumMap
    {
    private:
        std::unordered_map<std::string_view, T> _s2n;
        std::unordered_map<T, std::string_view> _n2s;

    public:
        DukEnumMap(const std::initializer_list<std::pair<std::string_view, T>>& items)
        {
            _s2n = std::unordered_map<std::string_view, T>(items.begin(), items.end());
            for (const auto& kvp : items)
            {
                _n2s.emplace(std::get<1>(kvp), std::get<0>(kvp));
            }
        }

        std::string_view operator[](T k) const
        {
            auto it = _n2s.find(k);
            if (it == _n2s.end())
            {
                return "";
            }
            return it->second;
        }

        T operator[](std::string_view k) const
        {
            auto it = _s2n.find(k);
            if (it == _s2n.end())
            {
                return static_cast<T>(0);
            }
            return it->second;
        }
    };

    inline duk_ret_t duk_json_decode_wrapper(duk_context* ctx, void*)
    {
        duk_json_decode(ctx, -1);
        return 1;
    }

    inline std::optional<DukValue> DuktapeTryParseJson(duk_context* ctx, std::string_view json)
    {
        duk_push_lstring(ctx, json.data(), json.size());
        if (duk_safe_call(ctx, duk_json_decode_wrapper, nullptr, 1, 1) == DUK_EXEC_SUCCESS)
        {
            return DukValue::take_from_stack(ctx);
        }
        else
        {
            // Pop error off stack
            duk_pop(ctx);
            return std::nullopt;
        }
    }

    std::string ProcessString(const DukValue& value);

    template<typename T> DukValue ToDuk(duk_context* ctx, const T& value) = delete;
    template<typename T> T FromDuk(const DukValue& s) = delete;

    template<> inline DukValue ToDuk(duk_context* ctx, const std::nullptr_t&)
    {
        duk_push_null(ctx);
        return DukValue::take_from_stack(ctx);
    }

    template<> inline DukValue ToDuk(duk_context* ctx, const bool& value)
    {
        duk_push_boolean(ctx, value);
        return DukValue::take_from_stack(ctx);
    }

    template<> inline DukValue ToDuk(duk_context* ctx, const int32_t& value)
    {
        duk_push_int(ctx, value);
        return DukValue::take_from_stack(ctx);
    }

    template<> inline DukValue ToDuk(duk_context* ctx, const std::string_view& value)
    {
        duk_push_lstring(ctx, value.data(), value.size());
        return DukValue::take_from_stack(ctx);
    }

    template<> inline DukValue ToDuk(duk_context* ctx, const std::string& value)
    {
        return ToDuk(ctx, std::string_view(value));
    }

    template<size_t TLen> inline DukValue ToDuk(duk_context* ctx, const char (&value)[TLen])
    {
        duk_push_string(ctx, value);
        return DukValue::take_from_stack(ctx);
    }

    template<typename T> inline DukValue ToDuk(duk_context* ctx, const std::optional<T>& value)
    {
        return value ? ToDuk(ctx, *value) : ToDuk(ctx, nullptr);
    }

    template<> CoordsXY inline FromDuk(const DukValue& d)
    {
        CoordsXY result;
        result.x = AsOrDefault(d["x"], 0);
        result.y = AsOrDefault(d["y"], 0);
        return result;
    }

    template<> DukValue inline ToDuk(duk_context* ctx, const CoordsXY& coords)
    {
        DukObject dukCoords(ctx);
        dukCoords.Set("x", coords.x);
        dukCoords.Set("y", coords.y);
        return dukCoords.Take();
    }

    template<> DukValue inline ToDuk(duk_context* ctx, const ScreenCoordsXY& coords)
    {
        DukObject dukCoords(ctx);
        dukCoords.Set("x", coords.x);
        dukCoords.Set("y", coords.y);
        return dukCoords.Take();
    }

    template<> inline DukValue ToDuk(duk_context* ctx, const CoordsXYZ& value)
    {
        if (value.isNull())
        {
            return ToDuk(ctx, nullptr);
        }
        else
        {
            DukObject dukCoords(ctx);
            dukCoords.Set("x", value.x);
            dukCoords.Set("y", value.y);
            dukCoords.Set("z", value.z);
            return dukCoords.Take();
        }
    }

    template<> inline CoordsXYZ FromDuk(const DukValue& value)
    {
        CoordsXYZ result;
        if (value.type() == DukValue::Type::OBJECT)
        {
            result.x = AsOrDefault(value["x"], 0);
            result.y = AsOrDefault(value["y"], 0);
            result.z = AsOrDefault(value["z"], 0);
        }
        else
        {
            result.setNull();
        }
        return result;
    }

    template<> inline DukValue ToDuk(duk_context* ctx, const CoordsXYZD& value)
    {
        if (value.isNull())
        {
            return ToDuk(ctx, nullptr);
        }
        else
        {
            DukObject dukCoords(ctx);
            dukCoords.Set("x", value.x);
            dukCoords.Set("y", value.y);
            dukCoords.Set("z", value.z);
            dukCoords.Set("direction", value.direction);
            return dukCoords.Take();
        }
    }

    template<> inline CoordsXYZD FromDuk(const DukValue& value)
    {
        CoordsXYZD result;
        if (value.type() == DukValue::Type::OBJECT)
        {
            result.x = AsOrDefault(value["x"], 0);
            result.y = AsOrDefault(value["y"], 0);
            result.z = AsOrDefault(value["z"], 0);
            result.direction = AsOrDefault(value["direction"], 0);
        }
        else
        {
            result.setNull();
        }
        return result;
    }

} // namespace OpenRCT2::Scripting

#endif
