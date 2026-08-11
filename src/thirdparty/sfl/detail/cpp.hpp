//
// Copyright (c) 2022 Slaven Falandys
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#ifndef SFL_DETAIL_CPP_HPP_INCLUDED
#define SFL_DETAIL_CPP_HPP_INCLUDED

#include <cassert>

#define SFL_ASSERT(x) assert(x)

#define SFL_CPP_14 201402L
#define SFL_CPP_17 201703L
#define SFL_CPP_20 202002L

#if defined(_MSC_VER) && defined(_MSVC_LANG)
    #define SFL_CPP_VERSION _MSVC_LANG
#else
    #define SFL_CPP_VERSION __cplusplus
#endif

#if SFL_CPP_VERSION >= SFL_CPP_14
    #define SFL_CONSTEXPR_14 constexpr
#else
    #define SFL_CONSTEXPR_14
#endif

#if SFL_CPP_VERSION >= SFL_CPP_17
    #define SFL_NODISCARD [[nodiscard]]
#else
    #define SFL_NODISCARD
#endif

#ifdef SFL_NO_EXCEPTIONS
    #define SFL_TRY      if (true)
    #define SFL_CATCH(x) if (false)
    #define SFL_RETHROW
#else
    #define SFL_TRY      try
    #define SFL_CATCH(x) catch (x)
    #define SFL_RETHROW  throw
#endif

#endif // SFL_DETAIL_CPP_HPP_INCLUDED
