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

    inline std::string AsOrDefault(const DukValue& value, const std::string_view& defaultValue)
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

        void Set(const char* name, const std::string_view& value)
        {
            EnsureObjectPushed();
            duk_push_lstring(_ctx, value.data(), value.size());
            duk_put_prop_string(_ctx, _idx, name);
        }

        void Set(const char* name, const DukValue& value)
        {
            EnsureObjectPushed();
            value.push();
            duk_put_prop_string(_ctx, _idx, name);
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

    inline duk_ret_t duk_json_decode_wrapper(duk_context* ctx, void*)
    {
        duk_json_decode(ctx, -1);
        return 1;
    }

    inline std::optional<DukValue> DuktapeTryParseJson(duk_context* ctx, const std::string_view& json)
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
} // namespace OpenRCT2::Scripting

#endif
