/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#pragma once

namespace OpenRCT2::Profiling
{
#if defined(__clang__) || defined(__GNUC__)
#    define PROFILING_FUNC_NAME __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#    define PROFILING_FUNC_NAME __FUNCSIG__
#else
#    error "Unsupported compiler"
#endif

#define PROFILED_FUNCTION_NAME(func)                                                                                           \
    static constexpr auto _profiling_func_name = func;                                                                         \
    struct Profiler_FunctionLiteral                                                                                            \
    {                                                                                                                          \
        static constexpr const char* Str()                                                                                     \
        {                                                                                                                      \
            return _profiling_func_name;                                                                                       \
        }                                                                                                                      \
    };

#if defined(__clang_major__) && __clang_major__ <= 5
    // Clang 5 crashes using the profiler, we need to disable it.
#    define PROFILED_FUNCTION()
#else

#    define PROFILED_FUNCTION()                                                                                                \
        PROFILED_FUNCTION_NAME(PROFILING_FUNC_NAME)                                                                            \
        static auto& _profiling_func = ::OpenRCT2::Profiling::Detail::Storage<Profiler_FunctionLiteral>::Data;                 \
        ::OpenRCT2::Profiling::ScopedProfiling<decltype(_profiling_func)> _profiling_scope(_profiling_func);
#endif

} // namespace OpenRCT2::Profiling
