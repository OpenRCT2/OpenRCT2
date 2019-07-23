/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../interface/InteractiveConsole.h"
#include "Duktape.hpp"

namespace OpenRCT2::Scripting
{
    class ScConsole
    {
    private:
        InteractiveConsole& _console;

    public:
        ScConsole(InteractiveConsole& console)
            : _console(console)
        {
        }

        void clear()
        {
            _console.Clear();
        }

        void log(DukValue val)
        {
            std::string str;
            switch (val.type())
            {
                case DukValue::Type::UNDEFINED:
                    str = "undefined";
                    break;
                case DukValue::Type::NULLREF:
                    str = "null";
                    break;
                case DukValue::Type::BOOLEAN:
                    str = val.as_bool() ? "true" : "false";
                    break;
                case DukValue::Type::NUMBER:
                {
                    const auto d = val.as_double();
                    const duk_int_t i = val.as_int();
                    if (AlmostEqual<double>(d, i))
                    {
                        str = std::to_string(i);
                    }
                    else
                    {
                        str = std::to_string(d);
                    }
                    break;
                }
                case DukValue::Type::STRING:
                    str = val.as_string();
                    break;
                case DukValue::Type::OBJECT:
                    str = "{}";
                    break;
                case DukValue::Type::BUFFER:
                    str = "buffer";
                    break;
                case DukValue::Type::POINTER:
                    str = "pointer";
                    break;
                case DukValue::Type::LIGHTFUNC:
                    break;
            }

            _console.WriteLine(str);
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_method(ctx, &ScConsole::clear, "clear");
            dukglue_register_method(ctx, &ScConsole::log, "log");
        }

    private:
        // Taken from http://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
        template<class T>
        static typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type AlmostEqual(T x, T y, int32_t ulp = 20)
        {
            // the machine epsilon has to be scaled to the magnitude of the values used
            // and multiplied by the desired precision in ULPs (units in the last place)
            return std::abs(x - y) <= std::numeric_limits<T>::epsilon() * std::abs(x + y) * ulp
                // unless the result is subnormal
                || std::abs(x - y)
                < (std::numeric_limits<T>::min)(); // TODO: Remove parentheses around min once the macro is removed
        }
    };
} // namespace OpenRCT2::Scripting
