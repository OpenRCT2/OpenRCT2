/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef __ENABLE_SCRIPTING__

#    include <dukglue/dukglue.h>
#    include <duktape.h>

template<typename T> DukValue GetObjectAsDukValue(duk_context* ctx, const std::shared_ptr<T>& value)
{
    dukglue::types::DukType<std::shared_ptr<T>>::template push<T>(ctx, value);
    return DukValue::take_from_stack(ctx);
}

template<typename T>
T AsOrDefault(const DukValue& value, const T& defaultValue = {}) = delete;

template<>
inline std::string AsOrDefault(const DukValue& value, const std::string& defaultValue)
{
    return value.type() == DukValue::STRING ? value.as_string() : defaultValue;
}

template<>
inline int32_t AsOrDefault(const DukValue& value, const int32_t& defaultValue)
{
    return value.type() == DukValue::NUMBER ? value.as_int() : defaultValue;
}

#endif
